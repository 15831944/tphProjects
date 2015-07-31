# -*- coding: cp936 -*-
'''
Created on 2013-10-17

@author: yuanrui
'''
from base.rdb_ItemBase import ItemBase
from common import rdb_log
from common import rdb_common

import os

class rdb_link_org2rdb(ItemBase): 
    '''Link org2rdb
    '''

    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self, 'Link org2rdb')

    def Do(self):
        
        # Create table for relationship between original link and RDB link.
        
        self._createOrg2RdbTbl()        
        self._checkOrg2RdbTbl()

    def _createOrg2RdbTbl(self):
        
        rdb_log.log(self.ItemName, 'creating temp_link_org_rdb -----start ', 'info')    
        self._createOrg2RdbTbl_prepare()
        self._createOrg2RdbTbl_common()
        self._createOrg2RdbTbl_special()                
        rdb_log.log(self.ItemName, 'creating temp_link_org_rdb -----end ', 'info')
              
    def _createOrg2RdbTbl_prepare(self):
        pass    
    def _createOrg2RdbTbl_common(self):
        # Create table for geometry relationship between original link and RDB link.             
        sqlcmd = """
            drop table if exists temp_link_org_rdb_step1;
            create table temp_link_org_rdb_step1
            as
            select  a.mid_link_id
                ,case when b.merge_link_dir is null or b.merge_link_dir is true then a.mid_geom
                    else st_reverse(mid_geom)
                end as mid_geom
                ,a.target_link_id,a.target_geom
                ,b.s_node as rdb_s_node,b.e_node as rdb_e_node
                ,b.merge_index as rdb_index 
            from temp_link_org2rdb a 
            left join temp_merge_link_mapping b
            on a.mid_link_id = b.merge_link_id;
            
            drop table if exists temp_link_org_rdb_step2;
            create table temp_link_org_rdb_step2
            as 
            select path,geom as rdb_geom_points,target_link_id,rdb_s_node,rdb_e_node
            from (
                select (st_dumppoints(rdb_geom)).*,target_link_id,rdb_s_node,rdb_e_node 
                from (
                    select st_collect(mid_geom) as rdb_geom,target_link_id,rdb_s_node,rdb_e_node 
                    from (
                        select * from temp_link_org_rdb_step1
                        order by target_link_id,rdb_s_node,rdb_e_node,rdb_index
                    ) a
                    group by target_link_id,rdb_s_node,rdb_e_node
                ) b 
            ) c where not (path[1] > 1 and path[2] = 1); 
            
            drop table if exists temp_link_org_rdb_step3;
            create table temp_link_org_rdb_step3
            as 
            select target_link_id,st_linefrommultipoint(st_collect(rdb_geom_points)) as rdb_geom 
            from 
            (
                select * from temp_link_org_rdb_step2
                order by target_link_id,rdb_s_node,rdb_e_node,path
            ) a group by target_link_id,rdb_s_node,rdb_e_node;
            
            create index temp_link_org_rdb_step3_target_link_id_idx
            on temp_link_org_rdb_step3
            using btree
            (target_link_id);
            
            drop table if exists temp_link_org_rdb_step4;
            create table temp_link_org_rdb_step4 
            as 
            select org_link_id,mid_link_id,mid_geom
                ,rdb_geom,a.target_link_id,target_geom 
            from temp_link_org2rdb a
            left join temp_link_org_rdb_step3 b
            on a.target_link_id = b.target_link_id;
            
                        
            ANALYZE temp_link_org_rdb_step4;
            CREATE INDEX temp_link_org_rdb_step4_mid_geom_idx
            ON temp_link_org_rdb_step4
            USING gist
            (mid_geom);
            
            CREATE INDEX temp_link_org_rdb_step4_rdb_geom_idx
            ON temp_link_org_rdb_step4
            USING gist
            (rdb_geom);
            
            CREATE INDEX temp_link_org_rdb_step4_target_geom_idx
            ON temp_link_org_rdb_step4
            USING gist
            (target_geom);                                   
        """
        self.pg.execute2(sqlcmd)        
        self.pg.commit2()

        # Create percents of middle links on RDB links for self-intersection or ring links. 
        sqlcmd = """
            --- For ring links, self_intersection links, use link length to calculate fraction,
            --- cause 'ST_Line_Locate_Point' can not work.
            drop table if exists temp_link_org_rdb_step5;
            create table temp_link_org_rdb_step5 as 
            select a.*
                ,case when g.link_id is null and 
                        ST_Equals(ST_Startpoint(mid_geom),ST_Startpoint(target_geom)) then 0
                      when g.merge_index = 1 and g.merge_link_dir = 't' and
                        ST_Equals(ST_Startpoint(mid_geom),ST_Startpoint(target_geom)) then 0
                      when g.merge_index = 1 and g.merge_link_dir = 'f' and
                        ST_Equals(ST_Endpoint(mid_geom),ST_Startpoint(target_geom)) then 0
                      else null
                 end as s_fraction
                ,case when g.link_id is null and
                        ST_Equals(ST_Endpoint(mid_geom),ST_Endpoint(target_geom))  then 1               
                      when g.merge_index = g.link_num and g.merge_link_dir = 't' and
                        ST_Equals(ST_Endpoint(mid_geom),ST_Endpoint(target_geom)) then 1
                      when g.merge_index = g.link_num and g.merge_link_dir = 'f' and
                        ST_Equals(ST_Startpoint(mid_geom),ST_Endpoint(target_geom)) then 1
                      else null
                 end as e_fraction  
                ,merge_link_dir,
                case when g.link_id is not null then g.merge_index
                    else 1
                end as index,
                ST_Length_Spheroid(mid_geom,'SPHEROID("WGS_84", 6378137, 298.257223563)') as mid_len,
                ST_Length_Spheroid(target_geom,'SPHEROID("WGS_84", 6378137, 298.257223563)')  as rdb_len
            from (
                select * from temp_link_org_rdb_step4 
                WHERE st_issimple(target_geom) = 'f' or st_isring(target_geom) = 't'
                or ST_NRings(st_buffer(target_geom,0.00001)) > 1
                or target_link_id in 
                (
                    select distinct target_link_id from temp_link_org_rdb_step4
                    where st_issimple(mid_geom) = 'f' or st_isring(mid_geom) = 't'
                    or ST_NRings(st_buffer(mid_geom,0.00001)) > 1
                )
            )a
            left join temp_merge_link_mapping g
            on a.mid_link_id = g.merge_link_id;  
            
            ANALYZE temp_link_org_rdb_step5;
            
            create index temp_link_org_rdb_step5_target_link_id_idx
            on temp_link_org_rdb_step5
            using btree
            (target_link_id);
            
            create index temp_link_org_rdb_step5_index_idx
            on temp_link_org_rdb_step5
            using btree
            (index);                                      
        """
        self.pg.execute2(sqlcmd)        
        self.pg.commit2() 
        
        # Create percents of middle links on RDB links for other normal links. 
        sqlcmd = """
            drop table if exists temp_link_org_rdb_step6;
            create table temp_link_org_rdb_step6
            as 
            select org_link_id,mid_link_id,target_link_id,merge_link_dir
                ,case when s_fraction is not null then s_fraction
                      when s_fraction is null and add_len is null then 0 
                      else round((add_len/rdb_len)::numeric,9)::double precision
                 end as s_fraction
                ,case when e_fraction is not null then e_fraction
                      when e_fraction is null and add_len is null then 
                              round((mid_len/rdb_len)::numeric,9)::double precision
                      else round(((mid_len + add_len)/rdb_len)::numeric,9)::double precision
                 end as e_fraction
            from (
                select org_link_id,mid_link_id,mid_geom,rdb_geom,s_fraction,e_fraction,
                target_link_id,merge_link_dir,target_geom,
                index,mid_len,rdb_len,sum(add_len) as add_len
                from (
                    select a.*,b.mid_len as add_len 
                    from temp_link_org_rdb_step5 a
                    left join  temp_link_org_rdb_step5 b
                    on a.target_link_id = b.target_link_id and a.index > b.index 
                ) c 
                group by org_link_id,mid_link_id,mid_geom,rdb_geom,target_link_id
                ,target_geom,s_fraction,e_fraction,index,mid_len,rdb_len,merge_link_dir
            ) d;
            
            
            create index temp_link_org_rdb_step6_org_link_id_idx
            on temp_link_org_rdb_step6
            using btree
            (org_link_id);
            
            create index temp_link_org_rdb_step6_mid_link_id_idx
            on temp_link_org_rdb_step6
            using btree
            (mid_link_id);
            
            create index temp_link_org_rdb_step6_target_link_id_idx
            on temp_link_org_rdb_step6
            using btree
            (target_link_id);
            
            drop table if exists temp_link_org_rdb;
            create table temp_link_org_rdb as
            select     org_link_id,
                mid_link_id,
                mid_geom,
                round(s_fraction::numeric,12)::double precision as s_fraction,
                round(e_fraction::numeric,12)::double precision as e_fraction,
                target_link_id,
                target_geom,
                case when p_link_dir is not null then cast((p_link_dir::int)#1 as boolean)
                    else cast((case when s_fraction > e_fraction then 1 else 0 end) as boolean) 
                end as flag
            from (
                select  e.org_link_id,
                    e.mid_link_id,
                    e.mid_geom,
                    case when f.s_fraction is null then ST_Line_Locate_Point(e.rdb_geom, ST_StartPoint(e.mid_geom))
                        else f.s_fraction
                    end as s_fraction,
                    case when f.e_fraction is null then ST_Line_Locate_Point(e.rdb_geom, ST_EndPoint(e.mid_geom)) 
                        else f.e_fraction
                    end as e_fraction,
                    e.target_link_id,
                    case when f.merge_link_dir is not null then f.merge_link_dir end as p_link_dir,
                    e.target_geom
                from temp_link_org_rdb_step4 e
                left join temp_link_org_rdb_step6 as f   
                ---- Get fraction of ring links and self_intersection links.
                on e.org_link_id = f.org_link_id and e.mid_link_id = f.mid_link_id 
                and e.target_link_id = f.target_link_id 
            ) a;  
                       
            ANALYZE temp_link_org_rdb;
            CREATE INDEX temp_link_org_rdb_org_link_id_idx
              ON temp_link_org_rdb
              USING btree
              (org_link_id);                                         
        """
        self.pg.execute2(sqlcmd)        
        self.pg.commit2()    
    def _createOrg2RdbTbl_special(self):
        pass       
           
    def _checkOrg2RdbTbl(self): 
             
        rdb_log.log(self.ItemName, 'check temp_link_org_rdb  --- start.', 'info')      
        sqlcmd = """
        select count(*) from rdb_link a
        left join (select distinct target_link_id from temp_link_org_rdb) b
        on a.link_id = b.target_link_id
        where b.target_link_id is null
        
        union
        
        select count(*) from (
        select mid_link_id,target_link_id,count(*) as count from temp_link_org2rdb group by mid_link_id,target_link_id  
        ) a where count > 1
            
        union
            
        select count(*) from temp_link_org_rdb where s_fraction < 0 or s_fraction > 1 or e_fraction < 0 or e_fraction > 1
            
        union
        select count(*) from (
            select target_link_id,max_s,min_s,max_e,min_e,
                case when max_s = 1 or max_e = 1 then 1 else 0 end as max_flag,
                case when min_s = 0 or min_e = 0 then 1 else 0 end as min_flag
                from (
                    select target_link_id,max(s_fraction) as max_s,min(s_fraction) as min_s,max(e_fraction) as max_e,min(e_fraction) as min_e 
                    from temp_link_org_rdb group by target_link_id
                ) a
        ) a where max_flag = 0 or min_flag = 0
        
        union
        
        select count(*) from (
        select mid_link_id,target_link_id,count(*) as count from temp_link_org2rdb group by mid_link_id,target_link_id  
        ) a where count > 1
        
        union
        
        select count(*) from (
            select target_link_id,
                    case when (count = 1 and (fraction = 0 or fraction = 1 )) or count = 2 then 1
                    else 0
                end as flag_count_1,
                    case when count = 2 and (fraction = 0 or fraction = 1 ) then 0
                    else 1
                end as flag_count_2, 
                fraction,
                count
            from (
                select target_link_id,fraction,count(*) as count from (
                    select * from (
                        select target_link_id,s_fraction as fraction from temp_link_org_rdb a 
                        union all
                        select target_link_id,e_fraction as fraction from temp_link_org_rdb a 
                    ) a order by target_link_id
                ) a group by target_link_id,fraction
            ) a
        ) a where flag_count_1 = 0 or flag_count_2 = 0;                                                       
           """
           
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchall2()
        if row:
            if len(row) > 1:
                rdb_log.log(self.ItemName, 'check table temp_link_org_rdb Failed.', 'error') 
            else:
                rdb_log.log(self.ItemName, 'check temp_link_org_rdb  --- end.', 'info')        


class rdb_link_org2rdb_axf(rdb_link_org2rdb):
    
    def _createOrg2RdbTbl_prepare(self):
        # Create ID relation table for original link and RDB link.        
        sqlcmd = """
            drop table if exists temp_link_org2rdb;
            create table temp_link_org2rdb
            as
            (
                SELECT ( (a.meshid::bigint << 32) | a.road ) as org_link_id
                   ,case when c.link_id is not null then c.link_id else a.new_road end as mid_link_id
                   ,case when c.link_id is not null then c.the_geom else f.the_geom end as mid_geom
                   ,e.tile_link_id as target_link_id
                   ,g.the_geom as target_geom
                FROM temp_link_mapping as a    
                left join temp_split_newlink as c
                on a.new_road = c.old_link_id
                left join link_tbl_bak_merge f
                on c.link_id = f.link_id or a.new_road = f.link_id
                left join temp_merge_link_mapping as d
                on (c.link_id = d.merge_link_id) or (a.new_road = d.merge_link_id)
                left join rdb_tile_link as e
                on (d.link_id = e.old_link_id) or (c.link_id = e.old_link_id) or (a.new_road = e.old_link_id)
                left join rdb_link g
                on e.tile_link_id = g.link_id
            );
            
            CREATE INDEX temp_link_org2rdb_mid_geom_idx
              ON temp_link_org2rdb
              USING gist
              (mid_geom);
            
            CREATE INDEX temp_link_org2rdb_mid_link_id_idx
              ON temp_link_org2rdb
              USING btree
              (mid_link_id);
            
            CREATE INDEX temp_link_org2rdb_org_link_id_idx
              ON temp_link_org2rdb
              USING btree
              (org_link_id);
            
            CREATE INDEX temp_link_org2rdb_target_geom_idx
              ON temp_link_org2rdb
              USING gist
              (target_geom);
            
            CREATE INDEX temp_link_org2rdb_target_link_id_idx
              ON temp_link_org2rdb
              USING btree
              (target_link_id);                         
        """
        self.pg.execute2(sqlcmd)        
        self.pg.commit2()                

    def _createOrg2RdbTbl_special(self):

        self.CreateFunction2('vics_get_shapepoints') 
        
        sqlcmd = """
            drop table if exists temp_link_org_rdb_bak;
            create table temp_link_org_rdb_bak as select * from temp_link_org_rdb;
            
            drop table if exists temp_link_org_rdb;
            create table temp_link_org_rdb as
            select ((org_link_id << 32) >> 32) as org_link_id
                ,(org_link_id >> 32) as org_mesh_id
                ,mid_link_id,mid_geom,s_fraction,e_fraction
                ,cast(vics_get_shapepoints(target_link_id,target_geom,s_fraction) as smallint) as s_point
                ,cast(vics_get_shapepoints(target_link_id,target_geom,e_fraction) as smallint) as e_point
                ,target_link_id,target_geom,flag 
            from temp_link_org_rdb_bak;
            
            drop table if exists temp_link_org_rdb_bak;            
            CREATE INDEX temp_link_org_rdb_org_mesh_id_idx
              ON temp_link_org_rdb
              USING btree
              (org_mesh_id);  
                                                    
        """
        self.pg.execute2(sqlcmd)        
        self.pg.commit2()    
                        

class rdb_link_org2rdb_jpn(rdb_link_org2rdb):
    
    def _createOrg2RdbTbl_prepare(self):
        # Create ID relation table for original link and RDB link.        
        sqlcmd = """
            drop table if exists temp_link_org2rdb;
            create table temp_link_org2rdb
            as
            (
                select a.org_link_id,a.mid_link_id,a.mid_geom,a.target_link_id,b.the_geom as target_geom 
                from (
                    SELECT    a.objectid as org_link_id, 
                        case when c.link_id is not null then c.link_id 
                             else b.tile_link_id 
                        end as mid_link_id,
                        case when c.link_id is not null then c.the_geom 
                             when c.link_id is null and f.link_id is not null then f.the_geom 
                             else a.the_geom 
                        end as mid_geom,
                        e.tile_link_id as target_link_id
                    FROM temp_road_rlk as a
                    left join middle_tile_link as b
                    on a.objectid = b.old_link_id and a.split_seq_num = b.split_seq_num
                    left join temp_split_newlink as c
                    on b.tile_link_id = c.old_link_id
                    left join link_tbl_bak_merge f
                    on c.link_id = f.link_id or b.tile_link_id = f.link_id
                    left join temp_merge_link_mapping as d
                    on (c.link_id = d.merge_link_id) or (b.tile_link_id = d.merge_link_id)
                    left join rdb_tile_link as e
                    on (d.link_id = e.old_link_id) or (c.link_id = e.old_link_id) or (b.tile_link_id = e.old_link_id)
                ) a
                left join rdb_link b
                on a.target_link_id = b.link_id
            ); 
            
            CREATE INDEX temp_link_org2rdb_mid_geom_idx
              ON temp_link_org2rdb
              USING gist
              (mid_geom);
            
            CREATE INDEX temp_link_org2rdb_mid_link_id_idx
              ON temp_link_org2rdb
              USING btree
              (mid_link_id);
            
            CREATE INDEX temp_link_org2rdb_org_link_id_idx
              ON temp_link_org2rdb
              USING btree
              (org_link_id);
            
            CREATE INDEX temp_link_org2rdb_target_geom_idx
              ON temp_link_org2rdb
              USING gist
              (target_geom);
            
            CREATE INDEX temp_link_org2rdb_target_link_id_idx
              ON temp_link_org2rdb
              USING btree
              (target_link_id);                         
        """
        self.pg.execute2(sqlcmd)        
        self.pg.commit2()                

    def _createOrg2RdbTbl_special(self):

        self.CreateFunction2('vics_get_shapepoints') 
        
        sqlcmd = """
            drop table if exists temp_link_org_rdb_bak;        
            create table temp_link_org_rdb_bak as select * from temp_link_org_rdb;
            
            drop table if exists temp_link_org_rdb;
            create table temp_link_org_rdb as
            select a.org_link_id,a.mid_link_id,a.mid_geom,a.s_fraction,a.e_fraction
                ,b.fromnodeid as s_node, b.tonodeid as e_node
                ,cast(vics_get_shapepoints(target_link_id,target_geom,s_fraction) as smallint) as s_point
                ,cast(vics_get_shapepoints(target_link_id,target_geom,e_fraction) as smallint) as e_point
                ,a.target_link_id,a.target_geom,a.flag 
            from temp_link_org_rdb_bak a
            left join road_rlk b
            on a.org_link_id = b.objectid;

            drop table if exists temp_link_org_rdb_bak;                        
            CREATE INDEX temp_link_org_rdb_s_node_e_node_idx
              ON temp_link_org_rdb
              USING btree
              (s_node, e_node);                                                    
        """
        self.pg.execute2(sqlcmd)        
        self.pg.commit2()    
        

class rdb_link_org2rdb_jdb(rdb_link_org2rdb):
    
    def _createOrg2RdbTbl_prepare(self):
        # Create ID relation table for original link and RDB link.        
        sqlcmd = """
            drop table if exists temp_link_org2rdb;
            create table temp_link_org2rdb
            as
            (
                select a.org_link_id,a.mid_link_id,a.mid_geom,a.target_link_id,b.the_geom as target_geom 
                from (
                    SELECT    a.objectid as org_link_id, 
                        case when c.link_id is not null then c.link_id 
                             else b.tile_link_id 
                        end as mid_link_id,
                        case when c.link_id is not null then c.the_geom 
                             when c.link_id is null and f.link_id is not null then f.the_geom 
                             else a.the_geom 
                        end as mid_geom,
                        e.tile_link_id as target_link_id
                    FROM temp_road_link as a
                    left join middle_tile_link as b
                    on a.objectid = b.old_link_id and a.split_seq_num = b.split_seq_num
                    left join temp_split_newlink as c
                    on b.tile_link_id = c.old_link_id
                    left join link_tbl_bak_merge f
                    on c.link_id = f.link_id or b.tile_link_id = f.link_id
                    left join temp_merge_link_mapping as d
                    on (c.link_id = d.merge_link_id) or (b.tile_link_id = d.merge_link_id)
                    left join rdb_tile_link as e
                    on (d.link_id = e.old_link_id) or (c.link_id = e.old_link_id) or (b.tile_link_id = e.old_link_id)
                ) a
                left join rdb_link b
                on a.target_link_id = b.link_id
            ); 
            
            CREATE INDEX temp_link_org2rdb_mid_geom_idx
              ON temp_link_org2rdb
              USING gist
              (mid_geom);
            
            CREATE INDEX temp_link_org2rdb_mid_link_id_idx
              ON temp_link_org2rdb
              USING btree
              (mid_link_id);
            
            CREATE INDEX temp_link_org2rdb_org_link_id_idx
              ON temp_link_org2rdb
              USING btree
              (org_link_id);
            
            CREATE INDEX temp_link_org2rdb_target_geom_idx
              ON temp_link_org2rdb
              USING gist
              (target_geom);
            
            CREATE INDEX temp_link_org2rdb_target_link_id_idx
              ON temp_link_org2rdb
              USING btree
              (target_link_id);                         
        """
        self.pg.execute2(sqlcmd)        
        self.pg.commit2()                

    def _createOrg2RdbTbl_special(self):

        self.CreateFunction2('vics_get_shapepoints') 
        
        sqlcmd = """
            drop table if exists temp_link_org_rdb_bak;        
            create table temp_link_org_rdb_bak as select * from temp_link_org_rdb;
            
            drop table if exists temp_link_org_rdb;
            create table temp_link_org_rdb as
            select a.org_link_id,a.mid_link_id,a.mid_geom,a.s_fraction,a.e_fraction
                ,b.from_node_id as s_node, b.to_node_id as e_node
                ,cast(vics_get_shapepoints(target_link_id,target_geom,s_fraction) as smallint) as s_point
                ,cast(vics_get_shapepoints(target_link_id,target_geom,e_fraction) as smallint) as e_point
                ,a.target_link_id,a.target_geom,a.flag 
            from temp_link_org_rdb_bak a
            left join road_link b
            on a.org_link_id = b.objectid;

            drop table if exists temp_link_org_rdb_bak;                    
            CREATE INDEX temp_link_org_rdb_s_node_e_node_idx
              ON temp_link_org_rdb
              USING btree
              (s_node, e_node);                                                    
        """
        self.pg.execute2(sqlcmd)        
        self.pg.commit2()    

                                                      
class rdb_link_org2rdb_rdf(rdb_link_org2rdb):
 
    def _createOrg2RdbTbl_prepare(self):
        
        # Create ID relation table for original link and RDB link.        
        sqlcmd = """
            drop table if exists temp_link_org2rdb;
            create table temp_link_org2rdb
            as
            (
                SELECT a.link_id as org_link_id
                   ,case when c.link_id is not null then c.link_id else f.link_id end as mid_link_id
                   ,case when c.link_id is not null then c.the_geom else f.the_geom end as mid_geom
                   ,e.tile_link_id as target_link_id
                   ,g.the_geom as target_geom
                FROM temp_rdf_nav_link as a
                left join temp_split_newlink as c
                on a.link_id = c.old_link_id
                left join link_tbl_bak_merge f
                on c.link_id = f.link_id or a.link_id = f.link_id
                left join temp_merge_link_mapping as d
                on (c.link_id = d.merge_link_id) or (a.link_id = d.merge_link_id)
                left join rdb_tile_link as e
                on (d.link_id = e.old_link_id) or (c.link_id = e.old_link_id) or (a.link_id = e.old_link_id)
                left join rdb_link g
                on e.tile_link_id = g.link_id
            );   
            
            ANALYZE temp_link_org2rdb;
            CREATE INDEX temp_link_org2rdb_mid_geom_idx
              ON temp_link_org2rdb
              USING gist
              (mid_geom);
            
            CREATE INDEX temp_link_org2rdb_mid_link_id_idx
              ON temp_link_org2rdb
              USING btree
              (mid_link_id);
            
            CREATE INDEX temp_link_org2rdb_org_link_id_idx
              ON temp_link_org2rdb
              USING btree
              (org_link_id);
            
            CREATE INDEX temp_link_org2rdb_target_geom_idx
              ON temp_link_org2rdb
              USING gist
              (target_geom);
            
            CREATE INDEX temp_link_org2rdb_target_link_id_idx
              ON temp_link_org2rdb
              USING btree
              (target_link_id);                            
        """
        self.pg.execute2(sqlcmd)        
        self.pg.commit2()        


class rdb_link_org2rdb_nostra(rdb_link_org2rdb):
                                
    def _createOrg2RdbTbl_prepare(self):
        # Create ID relation table for original link and RDB link.        
        sqlcmd = """
            DROP INDEX IF EXISTS temp_topo_link_routeid_type_idx;
            CREATE INDEX temp_topo_link_routeid_type_idx
              ON temp_topo_link
              USING btree
              (cast(routeid as bigint));
              
            drop table if exists temp_link_org2rdb;
            create table temp_link_org2rdb
            as
            (
                SELECT cast(a.routeid as bigint) as org_link_id
                   ,case when c.link_id is not null then c.link_id else f.link_id end as mid_link_id
                   ,case when c.link_id is not null then c.the_geom else f.the_geom end as mid_geom
                   ,e.tile_link_id as target_link_id
                   ,g.the_geom as target_geom
                FROM temp_topo_link as a
                left join temp_split_newlink as c
                on cast(a.routeid as bigint) = c.old_link_id
                left join link_tbl_bak_merge f
                on c.link_id = f.link_id or cast(a.routeid as bigint) = f.link_id
                left join temp_merge_link_mapping as d
                on (c.link_id = d.merge_link_id) or (cast(a.routeid as bigint) = d.merge_link_id)
                left join rdb_tile_link as e
                on (d.link_id = e.old_link_id) or (c.link_id = e.old_link_id) 
                or (cast(a.routeid as bigint) = e.old_link_id)
                left join rdb_link g
                on e.tile_link_id = g.link_id
            );
            
            CREATE INDEX temp_link_org2rdb_mid_geom_idx
              ON temp_link_org2rdb
              USING gist
              (mid_geom);
            
            CREATE INDEX temp_link_org2rdb_mid_link_id_idx
              ON temp_link_org2rdb
              USING btree
              (mid_link_id);
            
            CREATE INDEX temp_link_org2rdb_org_link_id_idx
              ON temp_link_org2rdb
              USING btree
              (org_link_id);
            
            CREATE INDEX temp_link_org2rdb_target_geom_idx
              ON temp_link_org2rdb
              USING gist
              (target_geom);
            
            CREATE INDEX temp_link_org2rdb_target_link_id_idx
              ON temp_link_org2rdb
              USING btree
              (target_link_id);                           
        """
        self.pg.execute2(sqlcmd)        
        self.pg.commit2()         
    def _createOrg2RdbTbl_special(self):
        
        # Create percents of middle links on original links. 
        sqlcmd = """
            --- For ring links, self_intersection links, use link length to calculate fraction,
            --- cause 'ST_Line_Locate_Point' can not work.
            drop table if exists temp_link_rdb_org_temp;
            create table temp_link_rdb_org_temp as 
            select a.org_link_id,b.the_geom as org_geom,a.mid_link_id,a.mid_geom,a.rdb_geom,a.target_link_id,a.target_geom 
            from temp_link_org_rdb_step4 a
            left join temp_topo_link b
            on a.org_link_id = b.routeid;            
            
            drop table if exists temp_link_rdb_org_temp_circle;
            create table temp_link_rdb_org_temp_circle as 
            select a.*,
                case when b.link_id is not null then b.sub_index
                    else 1
                end as index,
                ST_Length_Spheroid(mid_geom,'SPHEROID("WGS_84", 6378137, 298.257223563)') as mid_len,
                ST_Length_Spheroid(org_geom,'SPHEROID("WGS_84", 6378137, 298.257223563)')  as org_len
            from (
                select * from temp_link_rdb_org_temp 
                WHERE st_issimple(org_geom) = 'f' or st_isring(org_geom) = 't'
            )a
            left join temp_split_newlink b
            on a.mid_link_id = b.link_id and a.org_link_id = b.old_link_id
            order by target_link_id,index;
                        
            drop table if exists temp_link_rdb_org;
            create table temp_link_rdb_org as
            select     org_link_id,
                org_geom,
                mid_link_id,
                mid_geom,
                s_fraction,
                e_fraction,
                target_link_id,
                target_geom
            from (
                select  e.org_link_id,
                    e.org_geom,
                    e.mid_link_id,
                    e.mid_geom,
                    case when f.s_fraction is null then ST_Line_Locate_Point(e.org_geom, ST_StartPoint(e.mid_geom))
                        else f.s_fraction
                    end as s_fraction,
                    case when f.e_fraction is null then ST_Line_Locate_Point(e.org_geom, ST_EndPoint(e.mid_geom)) 
                        else f.e_fraction
                    end as e_fraction,
                    e.target_link_id,
                    e.target_geom
                    from temp_link_rdb_org_temp e
                    left join (                
                        select d.*,
                            case when add_len is null then 0 
                                else round((add_len/org_len)::numeric,9)::double precision
                            end as s_fraction
                            ,case when add_len is null then round((mid_len/org_len)::numeric,9)::double precision
                                else round(((mid_len + add_len)/org_len)::numeric,9)::double precision
                            end as e_fraction
                        from (
                            select org_link_id,mid_link_id,mid_geom,rdb_geom,target_link_id,target_geom,
                                index,mid_len,org_len,sum(add_len) as add_len
                            from (
                                select a.*,b.mid_len as add_len 
                                from temp_link_rdb_org_temp_circle a
                                left join  temp_link_rdb_org_temp_circle b
                                on a.org_link_id = b.org_link_id and a.index > b.index 
                            ) c 
                            group by org_link_id,mid_link_id,mid_geom,rdb_geom,target_link_id
                            ,target_geom,index,mid_len,org_len
                            
                        ) d order by org_link_id,index 
                    ) f
                    on e.org_link_id = f.org_link_id and e.mid_link_id = f.mid_link_id and e.target_link_id = f.target_link_id 
            ) a;
            
            
            CREATE INDEX temp_link_rdb_org_link_id_idx
              ON temp_link_rdb_org
              USING btree
              (org_link_id);
            CREATE INDEX temp_link_rdb_mid_link_id_idx
              ON temp_link_rdb_org
              USING btree
              (mid_link_id);
            CREATE INDEX temp_link_rdb_target_link_id_idx
              ON temp_link_rdb_org
              USING btree
              (target_link_id);                                  
        """
        self.pg.execute2(sqlcmd)        
        self.pg.commit2()
                
        # Create result table for relationship between original link & rdb link. 
        sqlcmd = """
            drop table if exists temp_link_org_rdb_org_temp;
            create table temp_link_org_rdb_org_temp as
            select a.org_link_id,b.org_geom
                ,b.s_fraction as org_s_fraction,b.e_fraction as org_e_fraction
                ,a.mid_link_id,a.mid_geom
                ,a.s_fraction as rdb_s_fraction,a.e_fraction as rdb_e_fraction
                ,a.target_link_id,a.target_geom,a.flag 
            from temp_link_org_rdb a
            left join temp_link_rdb_org b
            on a.org_link_id = b.org_link_id and a.mid_link_id = b.mid_link_id 
            and a.target_link_id = b.target_link_id;                                  
        """
        self.pg.execute2(sqlcmd)        
        self.pg.commit2()

                                   
class rdb_link_org2rdb_mmi(rdb_link_org2rdb):
    
    def _createOrg2RdbTbl_prepare(self):
        # Create ID relation table for original link and RDB link.        
        sqlcmd = """
            DROP INDEX IF EXISTS org_city_nw_gc_polyline_id_type_idx;
            CREATE INDEX org_city_nw_gc_polyline_id_type_idx
              ON org_city_nw_gc_polyline
              USING btree
              (cast(id as bigint));
              
            drop table if exists temp_link_org2rdb;
            create table temp_link_org2rdb
            as
            (
                SELECT cast(a.id as bigint) as org_link_id
                   ,case when c.link_id is not null then c.link_id else f.link_id end as mid_link_id
                   ,case when c.link_id is not null then c.the_geom else f.the_geom end as mid_geom
                   ,e.tile_link_id as target_link_id
                   ,g.the_geom as target_geom
                FROM org_city_nw_gc_polyline as a
                left join temp_split_newlink as c
                on cast(a.id as bigint) = c.old_link_id
                left join link_tbl_bak_merge f
                on c.link_id = f.link_id or cast(a.id as bigint) = f.link_id
                left join temp_merge_link_mapping as d
                on (c.link_id = d.merge_link_id) or (cast(a.id as bigint) = d.merge_link_id)
                left join rdb_tile_link as e
                on (d.link_id = e.old_link_id) or (c.link_id = e.old_link_id) 
                or (cast(a.id as bigint) = e.old_link_id)
                left join rdb_link g
                on e.tile_link_id = g.link_id
            );
            
            CREATE INDEX temp_link_org2rdb_mid_geom_idx
              ON temp_link_org2rdb
              USING gist
              (mid_geom);
            
            CREATE INDEX temp_link_org2rdb_mid_link_id_idx
              ON temp_link_org2rdb
              USING btree
              (mid_link_id);
            
            CREATE INDEX temp_link_org2rdb_org_link_id_idx
              ON temp_link_org2rdb
              USING btree
              (org_link_id);
            
            CREATE INDEX temp_link_org2rdb_target_geom_idx
              ON temp_link_org2rdb
              USING gist
              (target_geom);
            
            CREATE INDEX temp_link_org2rdb_target_link_id_idx
              ON temp_link_org2rdb
              USING btree
              (target_link_id);                         
        """
        self.pg.execute2(sqlcmd)        
        self.pg.commit2()                
        

class rdb_link_org2rdb_tomtom(rdb_link_org2rdb):
    
    def _createOrg2RdbTbl_prepare(self):
        # Create ID relation table for original link and RDB link.        
        sqlcmd = """

            DROP INDEX IF EXISTS org_nw_id_type_idx;
            CREATE INDEX org_nw_id_type_idx
              ON org_nw
              USING btree
              (cast(id as bigint));
              
            drop table if exists temp_link_org2rdb;
            create table temp_link_org2rdb
            as
            (
                SELECT cast(a.id as bigint) as org_link_id
                   ,case when c.link_id is not null then c.link_id else f.link_id end as mid_link_id
                   ,case when c.link_id is not null then c.the_geom else f.the_geom end as mid_geom
                   ,e.tile_link_id as target_link_id
                   ,g.the_geom as target_geom
                FROM org_nw as a
                left join temp_dupli_link b
                on cast(a.id as bigint) = b.aid
                left join temp_circle_link_new_seq_link h
                on cast(a.id as bigint) = h.old_link_id
                left join temp_split_newlink as c
                on cast(a.id as bigint) = c.old_link_id or h.link_id = c.old_link_id
                left join link_tbl_bak_merge f
                on c.link_id = f.link_id or cast(a.id as bigint) = f.link_id or h.link_id = f.link_id
                left join temp_merge_link_mapping as d
                on (c.link_id = d.merge_link_id) or (cast(a.id as bigint) = d.merge_link_id) 
                or h.link_id = d.merge_link_id
                left join rdb_tile_link as e
                on (d.link_id = e.old_link_id) or (c.link_id = e.old_link_id) 
                or (cast(a.id as bigint) = e.old_link_id) or (h.link_id = e.old_link_id)
                left join rdb_link g
                on e.tile_link_id = g.link_id
                where b.aid is null
            );
            
            CREATE INDEX temp_link_org2rdb_mid_geom_idx
              ON temp_link_org2rdb
              USING gist
              (mid_geom);
            
            CREATE INDEX temp_link_org2rdb_mid_link_id_idx
              ON temp_link_org2rdb
              USING btree
              (mid_link_id);
            
            CREATE INDEX temp_link_org2rdb_org_link_id_idx
              ON temp_link_org2rdb
              USING btree
              (org_link_id);
            
            CREATE INDEX temp_link_org2rdb_target_geom_idx
              ON temp_link_org2rdb
              USING gist
              (target_geom);
            
            CREATE INDEX temp_link_org2rdb_target_link_id_idx
              ON temp_link_org2rdb
              USING btree
              (target_link_id);                         
        """
        self.pg.execute2(sqlcmd)        
        self.pg.commit2()                
        
class rdb_link_org2rdb_msm(rdb_link_org2rdb):
                                
    def _createOrg2RdbTbl_prepare(self):
        # Create ID relation table for original link and RDB link.        
        sqlcmd = """
            drop table if exists temp_link_org2rdb;
            create table temp_link_org2rdb
            as
            (
                SELECT a.new_link_id as org_link_id
                   ,case when c.link_id is not null then c.link_id else f.link_id end as mid_link_id
                   ,case when c.link_id is not null then c.the_geom else f.the_geom end as mid_geom
                   ,e.tile_link_id as target_link_id
                   ,g.the_geom as target_geom
                FROM temp_topo_link as a
                left join org_processed_line as b
                on a.folder = b.folder and a.link_id = b.link_id
                left join temp_dupli_link as i
                on a.new_link_id = i.aid
                left join temp_circle_link_new_seq_link as h
                on a.new_link_id = h.old_link_id               
                left join temp_split_newlink as c
                on a.new_link_id = c.old_link_id or h.link_id = c.old_link_id
                left join link_tbl_bak_merge as f
                on c.link_id = f.link_id or a.new_link_id = f.link_id or h.link_id = f.link_id
                left join temp_merge_link_mapping as d
                on (c.link_id = d.merge_link_id) or a.new_link_id = d.merge_link_id or h.link_id = d.merge_link_id
                left join rdb_tile_link as e
                on (d.link_id = e.old_link_id) or (c.link_id = e.old_link_id) 
                or a.new_link_id = e.old_link_id or (h.link_id = e.old_link_id)
                left join rdb_link as g
                on e.tile_link_id = g.link_id
                where b.type not in (40) and i.aid is null
            );
            
            CREATE INDEX temp_link_org2rdb_mid_geom_idx
              ON temp_link_org2rdb
              USING gist
              (mid_geom);
            
            CREATE INDEX temp_link_org2rdb_mid_link_id_idx
              ON temp_link_org2rdb
              USING btree
              (mid_link_id);
            
            CREATE INDEX temp_link_org2rdb_org_link_id_idx
              ON temp_link_org2rdb
              USING btree
              (org_link_id);
            
            CREATE INDEX temp_link_org2rdb_target_geom_idx
              ON temp_link_org2rdb
              USING gist
              (target_geom);
            
            CREATE INDEX temp_link_org2rdb_target_link_id_idx
              ON temp_link_org2rdb
              USING btree
              (target_link_id);                         
        """
        self.pg.execute2(sqlcmd)        
        self.pg.commit2()      