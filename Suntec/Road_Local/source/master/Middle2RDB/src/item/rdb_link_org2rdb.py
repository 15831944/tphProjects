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
    @staticmethod
    def instance():
        proj_mapping = {
            ('jpn'):                rdb_link_org2rdb_jpn(),
            ('jdb'):                rdb_link_org2rdb_jdb(),
            ('axf'):                rdb_link_org2rdb_axf(),
            ('ta'):                 rdb_link_org2rdb_tomtom(),
            ('rdf', 'ase'):         rdb_link_org2rdb_rdf_ase(),            
            ('rdf'):                rdb_link_org2rdb_rdf(),
            ('nostra'):             rdb_link_org2rdb_nostra(),
            ('mmi'):                rdb_link_org2rdb_mmi(),
            ('msm'):                rdb_link_org2rdb_msm(),
            ('ni'):                 rdb_link_org2rdb_ni(),
            ('zenrin'):             rdb_link_org2rdb_zenrin(),
        }
        return rdb_common.getItem(proj_mapping)

    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self, 'Link org2rdb')

    def Do_CreateTable(self):
        
        self.CreateTable2('temp_link_deleted')
        return 0
    
    def Do(self):
        
        # Create table for relationship between original link and RDB link.
        
        self._createOrg2RdbTbl()        
        self._checkOrg2RdbTbl()

    def _createOrg2RdbTbl(self):
        
        rdb_log.log(self.ItemName, 'creating temp_link_org_rdb -----start ', 'info') 
        
        self.CreateFunction2('mid_get_fraction')
           
        self._createOrg2RdbTbl_prepare()
        self._createOrg2RdbTbl_common()
        self._createOrg2RdbTbl_special()                
        rdb_log.log(self.ItemName, 'creating temp_link_org_rdb -----end ', 'info')
              
    def _createOrg2RdbTbl_prepare(self):
        pass    
    
    def _createOrg2RdbTbl_common(self):
        self.log.info('do temp_link_org_rdb...')
        sqlcmd = """
                drop table if exists temp_link_org_rdb;
                create table temp_link_org_rdb
                (
                  org_link_id bigint,
                  org_geom geometry,
                  mid_link_id bigint,
                  mid_geom geometry,
                  s_fraction double precision,
                  e_fraction double precision,
                  target_link_id bigint,
                  target_geom geometry,
                  flag boolean
                );
                """
        self.pg.execute2(sqlcmd)        
        self.pg.commit2()
        
        self.log.info('do temp_link_org_rdb one2one...')
        sqlcmd = """
                insert into temp_link_org_rdb
                            (org_link_id, org_geom, mid_link_id, mid_geom, s_fraction, e_fraction, target_link_id, target_geom, flag)
                select  a.org_link_id, 
                        a.org_geom,
                        a.mid_link_id, 
                        a.mid_geom, 
                        round(0::numeric,9) as s_fraction, 
                        round(1::numeric,9) as e_fraction,
                        a.target_link_id,
                        a.target_geom,
                        false as flag
                from temp_link_org2rdb as a
                left join temp_merge_link_mapping as b
                on a.mid_link_id = b.merge_link_id
                where b.merge_link_id is null;
                """
        self.pg.execute2(sqlcmd)        
        self.pg.commit2()
        
        self.log.info('do temp_link_org_rdb many2many...')
        sqlcmd = """
                drop table if exists temp_link_org2rdb_many2many;
                create table temp_link_org2rdb_many2many
                as
                select  a.*, 
                        b.merge_index, 
                        b.link_num,
                        b.merge_link_dir,
                        ST_Length_Spheroid(a.mid_geom,'SPHEROID("WGS_84", 6378137, 298.257223563)') as mid_len
                from temp_link_org2rdb as a
                left join temp_merge_link_mapping as b
                on a.mid_link_id = b.merge_link_id
                where b.merge_link_id is not null;
                
                create index temp_link_org2rdb_many2many_target_link_id_index
                    on temp_link_org2rdb_many2many
                    using btree
                    (target_link_id);
                
                analyze temp_link_org2rdb_many2many;
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        sqlcmd = """
                drop table if exists temp_link_org2rdb_many2many_sumlength;
                create table temp_link_org2rdb_many2many_sumlength
                as
                select target_link_id, sum(mid_len) as rdb_len
                from temp_link_org2rdb_many2many
                group by target_link_id;
                
                create index temp_link_org2rdb_many2many_sumlength_target_link_id_index
                    on temp_link_org2rdb_many2many_sumlength
                    using btree
                    (target_link_id);
                
                analyze temp_link_org2rdb_many2many_sumlength;
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        sqlcmd = """
                drop table if exists temp_link_org2rdb_many2many_leftlength;
                create table temp_link_org2rdb_many2many_leftlength
                as
                select  mid_link_id, 
                        (case when left_len is null then 0 else left_len end) as left_len
                from
                (
                    select a.mid_link_id, sum(b.mid_len) as left_len
                    from temp_link_org2rdb_many2many as a
                    left join temp_link_org2rdb_many2many as b
                    on a.target_link_id = b.target_link_id and a.merge_index > b.merge_index
                    group by a.mid_link_id
                )as t;
                
                create index temp_link_org2rdb_many2many_leftlength_target_link_id_index
                    on temp_link_org2rdb_many2many_leftlength
                    using btree
                    (mid_link_id);
                
                analyze temp_link_org2rdb_many2many_leftlength;
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        sqlcmd = """
                insert into temp_link_org_rdb
                            (org_link_id, org_geom, mid_link_id, mid_geom, s_fraction, e_fraction, target_link_id, target_geom, flag)
                select  a.org_link_id, 
                        a.org_geom,
                        a.mid_link_id, 
                        a.mid_geom, 
                        (
                        case 
                        when a.merge_link_dir and a.merge_index = 1 then round(0::numeric,9)
                        when a.merge_link_dir then round((c.left_len / b.rdb_len)::numeric,9)
                        else round(((a.mid_len + c.left_len) / b.rdb_len)::numeric,9)
                        end
                        ) as s_fraction, 
                        (
                        case 
                        when a.merge_link_dir and a.merge_index = a.link_num then round(1::numeric,9)
                        when a.merge_link_dir then round(((a.mid_len + c.left_len) / b.rdb_len)::numeric,9)
                        else round((c.left_len / b.rdb_len)::numeric,9)
                        end
                        ) as e_fraction, 
                        a.target_link_id,
                        a.target_geom,
                        (merge_link_dir = false) as flag
                from temp_link_org2rdb_many2many as a
                left join temp_link_org2rdb_many2many_sumlength as b
                on a.target_link_id = b.target_link_id
                left join temp_link_org2rdb_many2many_leftlength as c
                on a.mid_link_id = c.mid_link_id
                """
        self.pg.execute2(sqlcmd)        
        self.pg.commit2()
        
        self.log.info('do temp_link_org_rdb index...')
        sqlcmd = """
                CREATE INDEX temp_link_org_rdb_org_link_id_idx
                    ON temp_link_org_rdb
                    USING btree
                    (org_link_id);    
                CREATE INDEX temp_link_org_rdb_mid_link_id_idx
                    ON temp_link_org_rdb
                    USING btree
                    (mid_link_id);
                ANALYZE temp_link_org_rdb;
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2() 
    
    def _createOrg2RdbTbl_special(self):
        pass       
           
    def _checkOrg2RdbTbl(self): 
             
        rdb_log.log(self.ItemName, 'check temp_link_org_rdb  --- start.', 'info')      
        sqlcmd_list = [
        """
        select count(*) from rdb_link a
        left join temp_link_org_rdb b
        on a.link_id = b.target_link_id
        where b.target_link_id is null
        """,
        
        """
        select count(*) 
        from 
        (
            select mid_link_id,target_link_id
            from temp_link_org2rdb 
            group by mid_link_id, target_link_id 
            having count(*) > 1
            limit 1
        ) a 
        """,
        
        """
        select count(*) 
        from temp_link_org_rdb 
        where s_fraction < 0 or s_fraction > 1 or e_fraction < 0 or e_fraction > 1
        """,
        
        """
        select count(*) 
        from 
        (
            select  target_link_id
            from temp_link_org_rdb 
            group by target_link_id
            having  (max(s_fraction) != 1 and max(e_fraction) != 1)
                    or
                    (min(s_fraction) != 0 and min(e_fraction) != 0)
            limit 1
        ) a
        """,
        
        """
        select count(*) 
        from 
        (
            select a.mid_link_id,a.target_link_id
            from temp_link_org_rdb a
            left join temp_link_deleted b
            on a.org_link_id = b.link_id
            where b.link_id is null
            group by mid_link_id,target_link_id  
            having count(*) > 1
        ) a
        """,
        
        """
        select count(*) 
        from 
        (
            select  target_link_id
            from 
            (
                select target_link_id, fraction, count(*) as count 
                from 
                (
                    select a.* 
                    from 
                    (
                        select org_link_id,target_link_id,s_fraction as fraction from temp_link_org_rdb
                        union all
                        select org_link_id,target_link_id,e_fraction as fraction from temp_link_org_rdb
                    ) a 
                    left join temp_link_deleted b
                    on a.org_link_id = b.link_id
                    where b.link_id is null
                ) a group by target_link_id,fraction
            ) a
            where (count > 2)
                  or
                  (count = 2 and (fraction = 0 or fraction = 1))
                  or
                  (count = 1 and (fraction != 0 and fraction != 1))
            limit 1
        ) a
        """
        ]
        for sqlcmd in sqlcmd_list:
            self.pg.execute2(sqlcmd)
            row = self.pg.fetchone2()
            if row and row[0] > 0:
                rdb_log.log(self.ItemName, 'check table temp_link_org_rdb Failed.', 'error') 
                return 0
        
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
                   ,a.the_geom as org_geom
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
            
            CREATE INDEX temp_link_org2rdb_mid_link_id_idx
              ON temp_link_org2rdb
              USING btree
              (mid_link_id);
            
            CREATE INDEX temp_link_org2rdb_org_link_id_idx
              ON temp_link_org2rdb
              USING btree
              (org_link_id);
            
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
                ,org_geom
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
                        a.the_geom as org_geom,
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
            
            CREATE INDEX temp_link_org2rdb_mid_link_id_idx
              ON temp_link_org2rdb
              USING btree
              (mid_link_id);
            
            CREATE INDEX temp_link_org2rdb_org_link_id_idx
              ON temp_link_org2rdb
              USING btree
              (org_link_id);

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
            select a.org_link_id,a.mid_link_id,a.org_geom,a.mid_geom,a.s_fraction,a.e_fraction
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
                SELECT a.link_id as org_link_id,a.the_geom as org_geom
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
            
            
            CREATE INDEX temp_link_org2rdb_mid_link_id_idx
              ON temp_link_org2rdb
              USING btree
              (mid_link_id);
            
            CREATE INDEX temp_link_org2rdb_org_link_id_idx
              ON temp_link_org2rdb
              USING btree
              (org_link_id);
                        
            CREATE INDEX temp_link_org2rdb_target_link_id_idx
              ON temp_link_org2rdb
              USING btree
              (target_link_id);                            
        """
        self.pg.execute2(sqlcmd)        
        self.pg.commit2()        

class rdb_link_org2rdb_rdf_ase(rdb_link_org2rdb):
 
    def _createOrg2RdbTbl_prepare(self):
        
        # Create ID relation table for original link and RDB link.        
        sqlcmd = """
            drop table if exists temp_link_org2rdb;
            create table temp_link_org2rdb
            as
            (
                SELECT a.link_id as org_link_id,a.the_geom as org_geom
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
            
            
            CREATE INDEX temp_link_org2rdb_mid_link_id_idx
              ON temp_link_org2rdb
              USING btree
              (mid_link_id);
            
            CREATE INDEX temp_link_org2rdb_org_link_id_idx
              ON temp_link_org2rdb
              USING btree
              (org_link_id);
                        
            CREATE INDEX temp_link_org2rdb_target_link_id_idx
              ON temp_link_org2rdb
              USING btree
              (target_link_id);                            
        """
        self.pg.execute2(sqlcmd)        
        self.pg.commit2()        

    def _createOrg2RdbTbl_special(self):

        sqlcmd = """
            drop table if exists temp_link_rdb_org_temp;
            create table temp_link_rdb_org_temp as 
            select a.*,
                case when b.link_id is not null then b.sub_index
                    else 1
                end as index,
                case when b.sub_count is null then 1
                    else b.sub_count 
                end as sum_count,
                ST_Length_Spheroid(mid_geom,'SPHEROID("WGS_84", 6378137, 298.257223563)') as mid_len,
                ST_Length_Spheroid(org_geom,'SPHEROID("WGS_84", 6378137, 298.257223563)')  as org_len
            from temp_link_org_rdb a
            left join temp_split_newlink b
            on a.mid_link_id = b.link_id and a.org_link_id = b.old_link_id
            order by target_link_id,index;                                                
        """
        self.pg.execute2(sqlcmd)        
        self.pg.commit2()   
                
        sqlcmd = """
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
                select d.*,
                    case when index = 1 then 0
                        when add_len is null then 0 
                        else round((add_len/org_len)::numeric,9)::double precision
                    end as s_fraction
                    ,case when index = sum_count then 1
                        when add_len is null then round((mid_len/org_len)::numeric,9)::double precision
                        else round(((mid_len + add_len)/org_len)::numeric,9)::double precision
                    end as e_fraction
                from (
                    select org_link_id,org_geom,mid_link_id,mid_geom,target_link_id,target_geom,
                        index,sum_count,mid_len,org_len,sum(add_len) as add_len
                    from (
                        select a.*,b.mid_len as add_len 
                        from temp_link_rdb_org_temp a
                        left join  temp_link_rdb_org_temp b
                        on a.org_link_id = b.org_link_id and a.index > b.index 
                    ) c 
                    group by org_link_id,org_geom,mid_link_id,mid_geom,target_link_id
                    ,target_geom,index,sum_count,mid_len,org_len
                ) d order by org_link_id,index 
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
                SELECT cast(a.routeid as bigint) as org_link_id,a.the_geom as org_geom
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
                        
            CREATE INDEX temp_link_org2rdb_mid_link_id_idx
              ON temp_link_org2rdb
              USING btree
              (mid_link_id);
            
            CREATE INDEX temp_link_org2rdb_org_link_id_idx
              ON temp_link_org2rdb
              USING btree
              (org_link_id);
            
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
            --- cause 'mid_get_fraction' can not work.
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
                    case when f.s_fraction is null then mid_get_fraction(e.org_geom, ST_StartPoint(e.mid_geom))
                        else f.s_fraction
                    end as s_fraction,
                    case when f.e_fraction is null then mid_get_fraction(e.org_geom, ST_EndPoint(e.mid_geom)) 
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
                SELECT cast(a.id as bigint) as org_link_id,a.the_geom as org_geom
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
                SELECT cast(a.id as bigint) as org_link_id,a.the_geom as org_geom
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
                where b.aid is null and a.frc != -1
            );
            
            CREATE INDEX temp_link_org2rdb_mid_link_id_idx
              ON temp_link_org2rdb
              USING btree
              (mid_link_id);
            
            CREATE INDEX temp_link_org2rdb_org_link_id_idx
              ON temp_link_org2rdb
              USING btree
              (org_link_id);
            
            CREATE INDEX temp_link_org2rdb_target_link_id_idx
              ON temp_link_org2rdb
              USING btree
              (target_link_id);                         
        """
        self.pg.execute2(sqlcmd)        
        self.pg.commit2()                

    def _createOrg2RdbTbl_special(self):
        
        # Add duplicated links which was deleted in org2middle.
        sqlcmd = """
            --- links which was deleted.
            insert into temp_link_deleted(link_id)
                select aid from temp_dupli_link;        
        
            insert into temp_link_org_rdb
                (org_link_id, mid_link_id, mid_geom, s_fraction, e_fraction, target_link_id, 
                 target_geom, flag) 
            select a.aid as org_link_id
                ,c.mid_link_id
                ,c.mid_geom
                ,case when a.node_inverse = 1 then c.e_fraction
                    else c.s_fraction
                end as s_fraction
                ,case when a.node_inverse = 1 then c.s_fraction
                    else c.e_fraction
                end as e_fraction
                ,c.target_link_id
                ,c.target_geom
                ,case when a.node_inverse = 1 and c.flag = 'f' then true
                    when a.node_inverse = 1 and c.flag = 't' then false
                    else c.flag
                end as flag
            from temp_dupli_link a 
            left join org_nw b
            on a.aid = b.id
            left join temp_link_org_rdb c
            on a.bid = c.org_link_id;                                                   
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
                SELECT a.new_link_id as org_link_id,a.the_geom as org_geom
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
            
            CREATE INDEX temp_link_org2rdb_mid_link_id_idx
              ON temp_link_org2rdb
              USING btree
              (mid_link_id);
            
            CREATE INDEX temp_link_org2rdb_org_link_id_idx
              ON temp_link_org2rdb
              USING btree
              (org_link_id);

            CREATE INDEX temp_link_org2rdb_target_link_id_idx
              ON temp_link_org2rdb
              USING btree
              (target_link_id);                         
        """
        self.pg.execute2(sqlcmd)        
        self.pg.commit2()      
        
class rdb_link_org2rdb_ni(rdb_link_org2rdb):
 
    def _createOrg2RdbTbl_prepare(self):
        
        # Create ID relation table for original link and RDB link.        
        sqlcmd = """
            drop table if exists temp_link_org2rdb;
            create table temp_link_org2rdb
            as
            (
                SELECT (a.id::bigint) as org_link_id,a.the_geom as org_geom
                   ,case when c.link_id is not null then c.link_id else f.link_id end as mid_link_id
                   ,case when c.link_id is not null then c.the_geom else f.the_geom end as mid_geom
                   ,e.tile_link_id as target_link_id
                   ,g.the_geom as target_geom
                FROM org_r as a
                left join temp_split_newlink as c
                on (a.id::bigint) = c.old_link_id
                left join link_tbl_bak_merge f
                on c.link_id = f.link_id or (a.id::bigint) = f.link_id
                left join temp_merge_link_mapping as d
                on (c.link_id = d.merge_link_id) or ((a.id::bigint) = d.merge_link_id)
                left join rdb_tile_link as e
                on (d.link_id = e.old_link_id) or (c.link_id = e.old_link_id) or ((a.id::bigint) = e.old_link_id)
                left join rdb_link g
                on e.tile_link_id = g.link_id
            );           
            
            CREATE INDEX temp_link_org2rdb_mid_link_id_idx
              ON temp_link_org2rdb
              USING btree
              (mid_link_id);
            
            CREATE INDEX temp_link_org2rdb_org_link_id_idx
              ON temp_link_org2rdb
              USING btree
              (org_link_id);
                        
            CREATE INDEX temp_link_org2rdb_target_link_id_idx
              ON temp_link_org2rdb
              USING btree
              (target_link_id);                            
        """
        self.pg.execute2(sqlcmd)        
        self.pg.commit2()        

class rdb_link_org2rdb_zenrin(rdb_link_org2rdb):
 
    def _createOrg2RdbTbl_prepare(self):
        
        # Create ID relation table for original link and RDB link.        
        sqlcmd = """
            drop table if exists temp_link_org2rdb;
            create table temp_link_org2rdb
            as
            (
                SELECT a1.link_id as org_link_id,a.the_geom as org_geom
                   ,case when c.link_id is not null then c.link_id else f.link_id end as mid_link_id
                   ,case when c.link_id is not null then c.the_geom else f.the_geom end as mid_geom
                   ,e.tile_link_id as target_link_id
                   ,g.the_geom as target_geom
                FROM org_road as a
                left join temp_link_mapping a1
                on a.meshcode = a1.meshcode and a.linkno = a1.linkno
                left join temp_dummy_link_todelete a2
                on a1.link_id = a2.link_id
                left join temp_split_newlink as c
                on a1.link_id = c.old_link_id
                left join link_tbl_bak_merge f
                on c.link_id = f.link_id or a1.link_id = f.link_id
                left join temp_merge_link_mapping as d
                on (c.link_id = d.merge_link_id) or (a1.link_id = d.merge_link_id)
                left join rdb_tile_link as e
                on (d.link_id = e.old_link_id) or (c.link_id = e.old_link_id) or (a1.link_id = e.old_link_id)
                left join rdb_link g
                on e.tile_link_id = g.link_id
                where a2.link_id is null
            );
            
            CREATE INDEX temp_link_org2rdb_mid_link_id_idx
              ON temp_link_org2rdb
              USING btree
              (mid_link_id);
            
            CREATE INDEX temp_link_org2rdb_org_link_id_idx
              ON temp_link_org2rdb
              USING btree
              (org_link_id);
                        
            CREATE INDEX temp_link_org2rdb_target_link_id_idx
              ON temp_link_org2rdb
              USING btree
              (target_link_id);                            
        """
        self.pg.execute2(sqlcmd)        
        self.pg.commit2()        
       