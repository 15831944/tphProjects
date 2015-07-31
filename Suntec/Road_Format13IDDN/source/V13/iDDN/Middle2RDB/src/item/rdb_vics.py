# -*- coding: cp936 -*-
'''
Created on 2013-10-17

@author: yuanrui
'''
from base.rdb_ItemBase import ItemBase
from common import rdb_log
from common import rdb_common

import os

class rdb_vics_jpn(ItemBase):
    '''jpn vics
    '''

    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self, 'JPN Vics')
                
    def Do(self):
        
        rdb_log.log(self.ItemName, 'restoring vics table ----- start', 'info')
        self._restoreVicsFromIpc()
        rdb_log.log(self.ItemName, 'restoring vics table ----- end', 'info')
        
        rdb_log.log(self.ItemName, 'creating new vics ----- start', 'info')         
        self._createRDBVicsTbl()   
        rdb_log.log(self.ItemName, 'creating new vics ----- end', 'info')
                    
        return 0
    
    def _restoreVicsFromIpc(self):
        sqlcmd = """
                drop table if exists temp_vics_org2rdb; 
                drop table if exists temp_vics_org2rdb_ipc;               
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        op_config = rdb_common.GetPath('resore_paras')    
        if os.system('%s' % (op_config)) == -1:
            return -1

        sqlcmd = """
            drop sequence if exists temp_vics_objectid_seq;
            create sequence temp_vics_objectid_seq;
            
            drop table if exists inf_vics;        
            create table inf_vics as 
            select     nextval('temp_vics_objectid_seq') as objectid
                ,meshcode as vics_meshcode
                ,vicsclass as vicsclass_c
                ,vicsid as vics_link_id
            from (
                select distinct meshcode,vicsclass,vicsid 
                from temp_vics_org2rdb
            ) a;
            
            
            drop sequence if exists temp_vics_inf_objectid_seq;
            create sequence temp_vics_inf_objectid_seq;
            
            drop table if exists lq_vics;
            create table lq_vics as 
            select nextval('temp_vics_inf_objectid_seq') as objectid,
                link_id, linkdir_c, sequence, inf_id 
            from (
                select distinct link_id, linkdir_c, sequence, inf_id 
                from (
                    select  a.linkid as link_id
                        ,case when a.flag  is true and a.linkdir = 1 then 2
                             when a.flag  is true and a.linkdir = 2 then 1
                             else a.linkdir
                        end as linkdir_c
                        ,a.seq as sequence
                        ,b.objectid as inf_id
                    from temp_vics_org2rdb a
                    left join inf_vics b
                    on  a.meshcode = b.vics_meshcode
                        and a.vicsclass = b.vicsclass_c
                        and a.vicsid = b.vics_link_id
                ) a
            ) b; 
            
            drop sequence if exists temp_ipcvics_objectid_seq;
            create sequence temp_ipcvics_objectid_seq;
            
            drop table if exists inf_ipcvics;        
            create table inf_ipcvics as 
            select     nextval('temp_ipcvics_objectid_seq') as objectid
                ,meshcode as vics_meshcode
                ,vicsclass as vicsclass_c
                ,vicsid as vics_link_id
            from (
                select distinct meshcode,vicsclass,vicsid 
                from temp_vics_org2rdb_ipc
            ) a;
            
            
            drop sequence if exists temp_ipcvics_inf_objectid_seq;
            create sequence temp_ipcvics_inf_objectid_seq;
            
            drop table if exists lq_ipcvics;
            create table lq_ipcvics as 
            select     nextval('temp_ipcvics_inf_objectid_seq') as objectid,
                link_id, linkdir_c, sequence, inf_id 
            from (
                select distinct link_id, linkdir_c, sequence, inf_id 
                from (
                    select  a.linkid as link_id
                        ,case when a.flag  is true and a.linkdir = 1 then 2
                             when a.flag  is true and a.linkdir = 2 then 1
                             else a.linkdir
                        end as linkdir_c
                        ,a.seq as sequence
                        ,b.objectid as inf_id
                    from temp_vics_org2rdb_ipc a
                    left join inf_ipcvics b
                    on      a.meshcode = b.vics_meshcode
                        and a.vicsclass = b.vicsclass_c
                        and a.vicsid = b.vics_link_id
                ) a
            ) b;            
                          
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
                                
    def _createRDBVicsTbl(self):

        self.CreateIndex2('lq_vics_inf_id_idx')
        self.CreateIndex2('lq_vics_link_id_idx')
        self.CreateIndex2('lq_vics_link_id_idx')
        self.CreateIndex2('lq_ipcvics_inf_id_idx')
        self.CreateIndex2('lq_ipcvics_link_id_idx')
        self.CreateIndex2('lq_ipcvics_link_id_idx')
                                
        self.CreateTable2('temp_vics_org2rdb_prepare_jpn')
        self.CreateTable2('temp_ipc_vics_org2rdb_prepare_jpn') 
        self.CreateTable2('temp_vics_org2rdb_seq_jpn')
        self.CreateTable2('temp_ipc_vics_org2rdb_seq_jpn')
        
        self.CreateTable2('rdb_vics_org2rdb_jpn')
        self.CreateTable2('rdb_ipc_vics_org2rdb_jpn')
        
        self.CreateFunction2('rdb_make_vics_merge_links_jpn')
        
        sqlcmd = """
        select rdb_make_vics_merge_links_jpn('temp_vics_org2rdb_seq_jpn','rdb_vics_org2rdb_jpn');
        select rdb_make_vics_merge_links_jpn('temp_ipc_vics_org2rdb_seq_jpn','rdb_ipc_vics_org2rdb_jpn');
            """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()          
        
        self._checkResult()
 
    def _checkResult(self):
        
        rdb_log.log(self.ItemName, 'check rdb_vics_org2rdb_jpn  --- start.', 'info')      
        sqlcmd = """
           select count(*) from rdb_vics_org2rdb_jpn 
            where (s_fraction = 0 and e_fraction = 0) or (s_fraction = 1 and e_fraction = 1)
            
            union
            
            select count(*) from (
                select meshcode,vicsclass,vicsid,target_link_id,s_fraction,e_fraction 
                from rdb_vics_org2rdb_jpn 
                group by meshcode,vicsclass,vicsid,target_link_id,s_fraction,e_fraction having count(*) >  1
            ) a
            
            union
            
            select count(*) from rdb_vics_org2rdb_jpn a
            left join (
                select meshcode,vicsclass,vicsid,target_link_id,unnest(s_fraction_array) as s_fraction,unnest(e_fraction_array) as e_fraction,count from (
                    select meshcode,vicsclass,vicsid,target_link_id,array_agg(s_fraction) as s_fraction_array,array_agg(e_fraction) as e_fraction_array,count(*) as count
                     from (
                        select meshcode,vicsclass,vicsid,target_link_id, 
                            case when s_fraction > e_fraction then e_fraction else s_fraction end as s_fraction,
                            case when s_fraction > e_fraction then s_fraction else e_fraction end as e_fraction
                        from temp_vics_org2rdb_prepare_jpn 
                    ) a group by meshcode,vicsclass,vicsid,target_link_id
                ) c
            ) b
            on a.meshcode = b.meshcode and a.vicsid = b.vicsid and a.vicsclass = b.vicsclass and a.target_link_id = b.target_link_id
            where (a.s_fraction <> b.s_fraction or a.e_fraction <> b.e_fraction) and b.count = 1 
            
            union
            
            select count(*) from rdb_vics_org2rdb_jpn a
            left join (
                select meshcode,vicsclass,vicsid,target_link_id,unnest(s_fraction_array) as s_fraction,unnest(e_fraction_array) as e_fraction,count 
                from (
                    select meshcode,vicsclass,vicsid,target_link_id,
                        array_agg(s_fraction) as s_fraction_array,array_agg(e_fraction) as e_fraction_array,
                        count(*) as count 
                    from temp_vics_org2rdb_prepare_jpn
                    group by meshcode,vicsclass,vicsid,target_link_id
                ) c
            ) b
            on a.meshcode = b.meshcode and a.vicsid = b.vicsid and a.vicsclass = b.vicsclass and a.target_link_id = b.target_link_id
            where (a.s_fraction = b.s_fraction and a.e_fraction = b.e_fraction) and b.count <> 1 
            
            union
            
            select count(*) from rdb_vics_org2rdb_jpn a
            left join temp_vics_org2rdb_prepare_jpn b
            on a.meshcode = b.meshcode and a.vicsid = b.vicsid and a.vicsclass = b.vicsclass and a.target_link_id = b.target_link_id
            where a.linkdir <> b.linkdir
            
            union
            
            select count(*) from rdb_vics_org2rdb_jpn a
            left join temp_link_org_rdb b
            on a.target_link_id = b.target_link_id            
            where   (a.e_fraction = 1 and a.e_point <> st_npoints(b.target_geom) - 1)
                or 
                (a.e_fraction = 0 and a.e_point <> 0) 
            
            union
            
            select count(*) from (
                select meshcode,vicsclass,vicsid,target_link_id 
                from (select * from rdb_vics_org2rdb_jpn where s_fraction = 0 and e_fraction = 1) a
                group by meshcode,vicsclass,vicsid,target_link_id having count(*) <> 1
            ) a   ;                                                           
           """
           
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            row = self.pg.fetchall2()
            if row:
                if len(row) > 1:
                    rdb_log.log(self.ItemName, 'check rdb_vics_org2rdb_jpn Failed.', 'error') 
                else:
                    rdb_log.log(self.ItemName, 'check rdb_vics_org2rdb_jpn  --- end.', 'info') 
        
        
        rdb_log.log(self.ItemName, 'check rdb_ipc_vics_org2rdb_jpn  --- start.', 'info')      
        sqlcmd = """

            select count(*) from rdb_ipc_vics_org2rdb_jpn 
            where (s_fraction = 0 and e_fraction = 0) or (s_fraction = 1 and e_fraction = 1)
            
            union
            
            select count(*) from (
                select meshcode,vicsclass,vicsid,target_link_id,s_fraction,e_fraction 
                from rdb_ipc_vics_org2rdb_jpn 
                group by meshcode,vicsclass,vicsid,target_link_id,s_fraction,e_fraction having count(*) >  1
            ) a
            
            union
            
            select count(*) from rdb_ipc_vics_org2rdb_jpn a
            left join (
                select meshcode,vicsclass,vicsid,target_link_id,unnest(s_fraction_array) as s_fraction,unnest(e_fraction_array) as e_fraction,count from (
                    select meshcode,vicsclass,vicsid,target_link_id,array_agg(s_fraction) as s_fraction_array,array_agg(e_fraction) as e_fraction_array,count(*) as count
                     from (
                        select meshcode,vicsclass,vicsid,target_link_id, 
                            case when s_fraction > e_fraction then e_fraction else s_fraction end as s_fraction,
                            case when s_fraction > e_fraction then s_fraction else e_fraction end as e_fraction
                        from temp_ipc_vics_org2rdb_prepare_jpn 
                    ) a group by meshcode,vicsclass,vicsid,target_link_id
                ) c
            ) b
            on a.meshcode = b.meshcode and a.vicsid = b.vicsid and a.vicsclass = b.vicsclass and a.target_link_id = b.target_link_id
            where (a.s_fraction <> b.s_fraction or a.e_fraction <> b.e_fraction) and b.count = 1 
            
            union
            
            select count(*) from rdb_ipc_vics_org2rdb_jpn a
            left join (
                select meshcode,vicsclass,vicsid,target_link_id,unnest(s_fraction_array) as s_fraction,unnest(e_fraction_array) as e_fraction,count 
                from (
                    select meshcode,vicsclass,vicsid,target_link_id,
                        array_agg(s_fraction) as s_fraction_array,array_agg(e_fraction) as e_fraction_array,
                        count(*) as count 
                    from temp_ipc_vics_org2rdb_prepare_jpn
                    group by meshcode,vicsclass,vicsid,target_link_id
                ) c
            ) b
            on a.meshcode = b.meshcode and a.vicsid = b.vicsid and a.vicsclass = b.vicsclass and a.target_link_id = b.target_link_id
            where (a.s_fraction = b.s_fraction and a.e_fraction = b.e_fraction) and b.count <> 1 
            
            union
            
            select count(*) from rdb_ipc_vics_org2rdb_jpn a
            left join temp_ipc_vics_org2rdb_prepare_jpn b
            on a.meshcode = b.meshcode and a.vicsid = b.vicsid and a.vicsclass = b.vicsclass and a.target_link_id = b.target_link_id
            where a.linkdir <> b.linkdir  
            
            union
            
            select count(*) from rdb_ipc_vics_org2rdb_jpn a
            left join temp_link_org_rdb b
            on a.target_link_id = b.target_link_id            
            where   (a.e_fraction = 1 and a.e_point <> st_npoints(b.target_geom) - 1)
                or 
                (a.e_fraction = 0 and a.e_point <> 0) 
            
            union
            
            select count(*) from (
                select meshcode,vicsclass,vicsid,target_link_id 
                from (select * from rdb_ipc_vics_org2rdb_jpn where s_fraction = 0 and e_fraction = 1) a
                group by meshcode,vicsclass,vicsid,target_link_id having count(*) <> 1
            ) a  ;                                                              
           """
           
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            row = self.pg.fetchall2()
            if row:
                if len(row) > 1:
                    rdb_log.log(self.ItemName, 'check rdb_ipc_vics_org2rdb_jpn Failed.', 'error') 
                else:
                    rdb_log.log(self.ItemName, 'check rdb_ipc_vics_org2rdb_jpn  --- end.', 'info') 


class rdb_vics_axf(ItemBase):
    '''axf vics
    '''

    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self, 'AXF Vics')
                
    def Do(self):
        
        rdb_log.log(self.ItemName, 'creating new tmc ----- start', 'info')         
        self._createRDBVicsTbl()   
        rdb_log.log(self.ItemName, 'creating new tmc ----- end', 'info')
                    
        return 0

    def _createRDBVicsTbl(self):
        
        self.CreateIndex2('temp_tmc_chn_rdstmc_link_cnt_idx')
        self.CreateIndex2('temp_tmc_chn_rdstmc_link_idx')
        self.CreateIndex2('temp_tmc_chn_rdstmc_meshid_idx')
        
        self.CreateIndex2('temp_tmc_chn_rdslinkinfo_link_idx')
        self.CreateIndex2('temp_tmc_chn_rdslinkinfo_mesh_idx')
        self.CreateIndex2('temp_tmc_chn_rdslinkinfo_meshid_idx')
        self.CreateIndex2('temp_tmc_chn_rdslinkinfo_road_id_idx')       
                
        self.CreateTable2('temp_vics_org2rdb_prepare_axf')
        self.CreateTable2('temp_vics_org2rdb_seq_axf')
        self.CreateTable2('rdb_vics_org2rdb_axf')
        
        self.CreateFunction2('rdb_make_vics_merge_links_axf')
        
        if self.pg.callproc('rdb_make_vics_merge_links_axf') == -1:
            return 0
        
        self.pg.commit2()
        
        self._checkResult()
  
    def _checkResult(self):  
        
        rdb_log.log(self.ItemName, 'check rdb_vics_org2rdb_axf  --- start.', 'info')      
        sqlcmd = """
            select count(*) from rdb_vics_org2rdb_axf 
            where (s_fraction = 0 and e_fraction = 0) or (s_fraction = 1 and e_fraction = 1)
            
            union
            
            select count(*) from (
                select loc_code_mesh,loc_code,dir,target_link_id,s_fraction,e_fraction 
                from rdb_vics_org2rdb_axf 
                group by loc_code_mesh,loc_code,dir,target_link_id,s_fraction,e_fraction having count(*) >  1
            ) a
            
            union
            
            select count(*) from rdb_vics_org2rdb_axf a
            left join (
                select loc_code_mesh,loc_code,dir,target_link_id,unnest(s_fraction_array) as s_fraction,unnest(e_fraction_array) as e_fraction,count from (
                    select loc_code_mesh,loc_code,dir,target_link_id,array_agg(s_fraction) as s_fraction_array,array_agg(e_fraction) as e_fraction_array,count(*) as count
                     from (
                        select loc_code_mesh,loc_code,dir,target_link_id, 
                            case when s_fraction > e_fraction then e_fraction else s_fraction end as s_fraction,
                            case when s_fraction > e_fraction then s_fraction else e_fraction end as e_fraction
                        from temp_vics_org2rdb_prepare_axf 
                    ) a group by loc_code_mesh,loc_code,dir,target_link_id
                ) c
            ) b
            on a.loc_code_mesh = b.loc_code_mesh and a.loc_code = b.loc_code and a.dir = b.dir and a.target_link_id = b.target_link_id
            where (a.s_fraction <> b.s_fraction or a.e_fraction <> b.e_fraction) and b.count = 1 
            
            union
            
            select count(*) from rdb_vics_org2rdb_axf a
            left join (
                select loc_code_mesh,loc_code,dir,target_link_id,unnest(s_fraction_array) as s_fraction,unnest(e_fraction_array) as e_fraction,count 
                from (
                    select loc_code_mesh,loc_code,dir,target_link_id,
                        array_agg(s_fraction) as s_fraction_array,array_agg(e_fraction) as e_fraction_array,
                        count(*) as count 
                    from temp_vics_org2rdb_prepare_axf
                    group by loc_code_mesh,loc_code,dir,target_link_id
                ) c
            ) b
            on a.loc_code_mesh = b.loc_code_mesh and a.loc_code = b.loc_code and a.dir = b.dir and a.target_link_id = b.target_link_id
            where (a.s_fraction = b.s_fraction and a.e_fraction = b.e_fraction) and b.count <> 1 
            
            union
            
            select count(*) from rdb_vics_org2rdb_axf a
            left join temp_vics_org2rdb_prepare_axf b
            on a.loc_code_mesh = b.loc_code_mesh and a.loc_code = b.loc_code and a.dir = b.dir and a.target_link_id = b.target_link_id
            where a.link_dir <> b.link_dir or a.service_id <> b.service_id 
            
            union
            
            select count(*) from rdb_vics_org2rdb_axf a
            left join temp_link_org2rdb_axf b
            on a.target_link_id = b.target_link_id            
            where   (a.e_fraction = 1 and a.e_point <> st_npoints(b.target_geom) - 1)
                or 
                (a.e_fraction = 0 and a.e_point <> 0) 
            
            union
            
            select count(*) from (
                select loc_code_mesh,loc_code,dir,target_link_id 
                from (select * from rdb_vics_org2rdb_axf where s_fraction = 0 and e_fraction = 1) a
                group by loc_code_mesh,loc_code,dir,target_link_id having count(*) <> 1
            ) a                                                           
           """
           
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            row = self.pg.fetchall2()
            if row:
                if len(row) > 1:
                    rdb_log.log(self.ItemName, 'check table rdb_vics_org2rdb_axf Failed.', 'error') 
                else:
                    rdb_log.log(self.ItemName, 'check rdb_vics_org2rdb_axf  --- end.', 'info')                     


class rdb_traffic_area(ItemBase):
    '''Traffic AreaNo
    '''

    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self, 'Traffic AreaNo')
        
    def Do(self):    
        
        rdb_log.log(self.ItemName, 'creating area no table ----- start', 'info')    
        sqlcmd = """
            select count(*) from pg_tables 
            where tablename = 'rdb_trf_area';                          
            """
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchone2()
        
        if  row[0] == 1:
            self._createTileAreaNoTbl()          
        else:
            self._createTileAreaNoTbl_Null()
            rdb_log.log(self.ItemName, '!!! No original table rdb_trf_area, null table will be created !!!', 'warning')
            
        rdb_log.log(self.ItemName, 'creating area no table ----- end', 'info')
                        
    def _createTileAreaNoTbl(self):
            
        sqlcmd = """  
        drop table if exists rdb_trf_tile_area_no;
        create table rdb_trf_tile_area_no as
        select s.tile_id
            ,case when t.tile_id is not null then t.area_no::smallint
                else 0::smallint
            end as area_no
        from (
            select distinct link_id_t as tile_id from rdb_link
        ) s
        left join (
            select tile_id,area_no_array[1] as area_no
            from (
                select tile_id,array_agg(area_no) as area_no_array
                from (
                    select tile_id,area_no,count(*) as count_link 
                    from (
                        select m.link_id,m.link_geom,m.tile_id,m.tile_geom
                            ,case when n.area_no is not null then n.area_no
                                else 0 end as area_no
                        from (
                            select a.link_id,a.the_geom as link_geom
                                ,a.link_id_t as tile_id,c.the_geom as tile_geom
                            from rdb_link a
                            left join rdb_trf_org2rdb b
                            on a.link_id = b.rdb_link_id
                            left join temp_split_tile c
                            on a.link_id_t = c.tile_id
                            where b.rdb_link_id is not null        
                        ) m 
                        left join rdb_trf_area n
                        on st_intersects(m.link_geom,n.geom)
                    ) p group by tile_id,area_no
                    order by tile_id,count(*) desc
                ) q group by tile_id
            ) o 
        ) t
        on s.tile_id = t.tile_id
        order by s.tile_id; 
        
        delete from rdb_trf_org2rdb a
        using rdb_trf_area b
        where not (a.area_code = rdb_cnv_country_code(b.cc) and a.extra_flag = b.ltn);
        
        delete from rdb_region_trf_org2rdb_layer4 a
        using rdb_trf_area b
        where not (a.area_code = rdb_cnv_country_code(b.cc) and a.extra_flag = b.ltn);
        
        delete from rdb_region_trf_org2rdb_layer6 a
        using rdb_trf_area b
        where not (a.area_code = rdb_cnv_country_code(b.cc) and a.extra_flag = b.ltn); 
        
        delete from rdb_trf_locationtable a
        using rdb_trf_area b
        where not (a.country_code = rdb_cnv_country_code(b.cc) and a.table_no = b.ltn);
                                         
            """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()          
    
    def _createTileAreaNoTbl_Null(self):
        sqlcmd = """  
        drop table if exists rdb_trf_tile_area_no;
        CREATE TABLE rdb_trf_tile_area_no
        (
          tile_id integer,
          area_no smallint
        );                        
            """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()                   

            
class rdb_traffic(ItemBase): 
    '''traffic
    '''

    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self, 'Traffic')

    def Do(self):
                
        # Create sequence for original traffic links
        self._createTmcLinkSeq()      
        
        # Create org2rdb table for traffic links.               
        self._createTRFTbl()   
        self._checkResult()

        # Create org2rdb table for region traffic links.
        proc_region = rdb_traffic_region(self, rdb_log)
        proc_region._createRegionXTRFTbl('4')
        proc_region._checkRegionXResult('4')
        proc_region._createRegionXTRFTbl('6')
        proc_region._checkRegionXResult('6')

        # Create table for location code and event.
        self._createForTMCOnly()          
        
    def _createTmcLinkSeq(self):
        pass
                       
    def _createTRFTbl(self):    
        rdb_log.log(self.ItemName, 'creating trf ----- start', 'info')        
        self._createTRFTbl_prepare()
        self._createTRFTbl_common()                
        rdb_log.log(self.ItemName, 'creating trf ----- end', 'info')        
    def _createTRFTbl_prepare(self):
        pass    
    def _createTRFTbl_common(self):
        
        self.CreateFunction2('rdb_makeTrfshape2pixel')

        # Create new sequence for traffic links, cause that's a many-to-many relationship between original link and RDB link. 
        sqlcmd = """
        drop table if exists temp_trf_org2rdb_seq;
        create table temp_trf_org2rdb_seq as 
        select type_flag, area_code, infra_id, extra_flag, dir, 
            rdb_link_id,  
            link_dir, 
            pos_type, 
            round((s_fraction * 65535)::numeric,0) as s_fraction,  --- fraction value range [0,65535].
            round((e_fraction * 65535)::numeric,0) as e_fraction, 
            seq_org,
            sub_seq, 
            group_id,
            rdb_cnv_length(length) as length,  --- m as a unit for link length.
            geom_string
        from (
            select type_flag, area_code, infra_id, extra_flag, dir, rdb_link_id,
                link_dir, pos_type,
                --- 'fraction' reprensents a link without direction, so it should be always e_fraction > s_fraction.                 
                case when e_fraction < s_fraction then e_fraction else s_fraction end as s_fraction,
                case when e_fraction < s_fraction then s_fraction else e_fraction end as e_fraction,
                seq_org, sub_seq, group_id,
                ST_Length_Spheroid(target_geom,'SPHEROID("WGS_84", 6378137, 298.257223563)') as length,
                rdb_makeTrfshape2pixel(14::smallint, (((rdb_link_id >> 32)  >> 14) & 16383)::integer, 
                    (((rdb_link_id >> 32) & 16383))::integer, target_geom ) as geom_string
            from temp_trf_org2rdb_prepare
        ) org 
        order by type_flag,area_code,extra_flag,infra_id,dir,group_id,pos_type,seq_org,sub_seq;
                    ---seq_org: original data sequence; sub_seq: split sequence.
        
        --- links in the same location code, same dir, same group, their sequence should be continuous.
        --- range these infra links, and give them new continuous sequence.
        drop table if exists temp_trf_org2rdb_fraction;
        create table temp_trf_org2rdb_fraction as 
        select type_flag,area_code,extra_flag,infra_id,dir,group_id
            ,unnest(rdb_link_id_array) as rdb_link_id,unnest(link_dir_array) as link_dir
            ,unnest(pos_type_array) as pos_type,unnest(s_fraction_array) as s_fraction
            ,unnest(e_fraction_array) as e_fraction,unnest(length_array) as length
            ,unnest(geom_string_array) as geom_string,generate_series(1,sum) as seq
        from (
            select type_flag,area_code,extra_flag,infra_id,dir,group_id
                ,array_agg(rdb_link_id) as rdb_link_id_array,array_agg(link_dir) as link_dir_array
                ,array_agg(pos_type) as pos_type_array,array_agg(s_fraction) as s_fraction_array
                ,array_agg(e_fraction) as e_fraction_array,array_agg(length) as length_array
                ,array_agg(geom_string) as geom_string_array
                ,array_length(array_agg(rdb_link_id),1) as sum 
            from (
                select * from temp_trf_org2rdb_seq 
                order by type_flag,area_code,extra_flag,infra_id,dir,group_id,seq_org,sub_seq
            ) a group by type_flag,area_code,extra_flag,infra_id,dir,group_id
        ) b;  
        ANALYZE temp_trf_org2rdb_seq;
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.CreateFunction2('rdb_make_trf_merge_links')

        sqlcmd = """
        drop table if exists rdb_trf_org2rdb cascade;
        CREATE TABLE rdb_trf_org2rdb
        (
          gid serial primary key,
          type_flag smallint,
          area_code integer,
          infra_id integer,
          extra_flag integer,
          dir smallint,
          rdb_link_id bigint,
          rdb_link_t integer,
          link_dir smallint,
          display_class smallint,
          road_type smallint,
          pos_type smallint,
          s_fraction integer,
          e_fraction integer,
          seq integer,
          ctn_flag smallint default 1,
          group_id smallint,
          length integer,
          infra_len integer,
          geom_string character varying
        ); 

        --- merge fractions of links with the same location code, same dir, same group, same linkid, same linkdir.
        select rdb_make_trf_merge_links('temp_trf_org2rdb_fraction','rdb_trf_org2rdb'); 
        
        --- set infra link length.
        update rdb_trf_org2rdb set infra_len = round(b.sum,0) from(
            select type_flag, area_code, extra_flag, infra_id, dir,
                sum(((e_fraction - s_fraction) / 65535.0) * length) as sum
            from rdb_trf_org2rdb 
            group by type_flag, area_code, extra_flag, infra_id, dir
        ) b
        where rdb_trf_org2rdb.area_code = b.area_code 
            and rdb_trf_org2rdb.extra_flag = b.extra_flag 
            and rdb_trf_org2rdb.infra_id = b.infra_id 
            and rdb_trf_org2rdb.dir = b.dir;  
        
        --- in a same location code, same dir, same group, 
        --- if there's two consecutive links which share a same linkid, but have different fraction,
        --- set the prior's ctn_flag = 0.
        update rdb_trf_org2rdb set ctn_flag = 0 from rdb_trf_org2rdb b
        where rdb_trf_org2rdb.area_code = b.area_code 
            and rdb_trf_org2rdb.extra_flag = b.extra_flag 
            and rdb_trf_org2rdb.infra_id = b.infra_id 
            and rdb_trf_org2rdb.dir = b.dir 
            and rdb_trf_org2rdb.group_id = b.group_id 
            and rdb_trf_org2rdb.rdb_link_id = b.rdb_link_id 
            and rdb_trf_org2rdb.seq = b.seq - 1;   

        --- set display_class value.
        update rdb_trf_org2rdb set display_class = b.display_class from rdb_link b
        where rdb_trf_org2rdb.rdb_link_id = b.link_id;
        --- set road_type value.
        update rdb_trf_org2rdb set road_type = b.road_type from rdb_link b
        where rdb_trf_org2rdb.rdb_link_id = b.link_id;
                
        ANALYZE rdb_trf_org2rdb;                                                           
        """
        self.pg.execute2(sqlcmd) 
        self.pg.commit2()
                
    def _checkResult(self):    
        self._checkResult_Common()
        self._checkResult_special()        
    def _checkResult_Common(self):
        rdb_log.log(self.ItemName, 'check rdb_trf_org2rdb(common)  --- start.', 'info')      
        sqlcmd = """
            --- in a same location code, same dir, same group, linkid & linkdir & fraction should be only.
            select count(*) from (
                select area_code,extra_flag,infra_id,dir,group_id,rdb_link_id,link_dir,s_fraction 
                from rdb_trf_org2rdb 
                group by area_code,extra_flag,infra_id,dir,group_id,rdb_link_id,link_dir,s_fraction 
                having count(*) >  1
            ) a            
            union
            --- fraction's value should be effective.
            select count(*) from rdb_trf_org2rdb 
            where (s_fraction = 0 and e_fraction = 0) or (s_fraction = 65535 and e_fraction = 65535)            
            union
            --- check the merge of fraction is correct or not.
            select count(*) from rdb_trf_org2rdb a
            left join (
                select area_code,extra_flag,infra_id,dir,group_id,rdb_link_id,
                    round((s_fraction * 65535)::numeric,0) as s_fraction,
                    round((e_fraction * 65535)::numeric,0) as e_fraction,
                    count
                    from (
                    select area_code,extra_flag,infra_id,dir,group_id,rdb_link_id,unnest(s_fraction_array) as s_fraction
                        ,unnest(e_fraction_array) as e_fraction,count 
                    from (
                        select area_code,extra_flag,infra_id,dir,group_id,rdb_link_id
                            ,array_agg(s_fraction) as s_fraction_array
                            ,array_agg(e_fraction) as e_fraction_array,count(*) as count
                         from (
                            select area_code,extra_flag,infra_id,dir,group_id,rdb_link_id, link_dir,
                                case when s_fraction > e_fraction then e_fraction else s_fraction 
                                end as s_fraction,
                                case when s_fraction > e_fraction then s_fraction else e_fraction 
                                end as e_fraction
                            from temp_trf_org2rdb_prepare 
                        ) a group by area_code,extra_flag,infra_id,dir,group_id,rdb_link_id,link_dir
                    ) c
                ) pre
            ) b
            on a.area_code = b.area_code and a.extra_flag = b.extra_flag and a.infra_id = b.infra_id 
            and a.group_id = b.group_id and a.dir = b.dir and a.rdb_link_id = b.rdb_link_id
            where (a.s_fraction <> b.s_fraction or a.e_fraction <> b.e_fraction) and b.count = 1            
            union            
            select count(*) from rdb_trf_org2rdb a
            left join (
                select area_code,extra_flag,infra_id,dir,group_id,rdb_link_id
                    ,unnest(s_fraction_array) as s_fraction
                    ,unnest(e_fraction_array) as e_fraction,count 
                from (
                    select area_code,extra_flag,infra_id,dir,group_id,rdb_link_id
                        ,array_agg(s_fraction) as s_fraction_array,
                        array_agg(e_fraction) as e_fraction_array,count(*) as count 
                    from temp_trf_org2rdb_prepare
                    group by area_code,extra_flag,infra_id,dir,group_id,rdb_link_id
                ) c
            ) b
            on a.area_code = b.area_code and a.extra_flag = b.extra_flag 
            and a.infra_id = b.infra_id and a.dir = b.dir and a.group_id = b.group_id 
            and a.rdb_link_id = b.rdb_link_id
            where (a.s_fraction = b.s_fraction and a.e_fraction = b.e_fraction) and b.count <> 1             
            union
            --- a same location code, same dir, same group, same link, 
            --- different fractions should have same link directions. 
            select count(*) from rdb_trf_org2rdb a
            left join temp_trf_org2rdb_prepare b
            on a.area_code = b.area_code and a.extra_flag = b.extra_flag 
            and a.infra_id = b.infra_id and a.dir = b.dir and a.group_id = b.group_id 
            and a.rdb_link_id = b.rdb_link_id
            where a.link_dir <> b.link_dir ;
           """
           
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchall2()
        if row:
            if len(row) > 1:
                rdb_log.log(self.ItemName, 'check table rdb_trf_org2rdb(common) Failed.', 'error') 
            else:
                rdb_log.log(self.ItemName, 'check rdb_trf_org2rdb(common)  --- end.', 'info')            
    def _checkResult_special(self):
        pass        
        
    def _createForTMCOnly(self):
        pass                         
        
                              
class rdb_traffic_rdf(rdb_traffic):
                                         
    def _createTmcLinkSeq(self):
        
        rdb_log.log(self.ItemName, 'creating sequence for tmc link -----start ', 'info')
        # Add nodes to original traffic links.
        sqlcmd = """
            drop table if exists temp_rdf_link_tmc_node cascade;
            create table temp_rdf_link_tmc_node as 
            select a.*,b.ref_node_id,b.nonref_node_id 
            from (
                select *, case when tmc_path_direction in ('+','-') then 1 else 0 end as type, 
                    case when tmc_path_direction in ('+','P') then 0 else 1 end as dir
                from rdf_link_tmc
            )a
            left join temp_rdf_nav_link b
            on a.link_id = b.link_id;
            
            CREATE INDEX temp_rdf_link_tmc_node_dir_idx
              ON temp_rdf_link_tmc_node
              USING btree
              (dir);
            CREATE INDEX temp_rdf_link_tmc_node_ebu_country_code_idx
              ON temp_rdf_link_tmc_node
              USING btree
              (ebu_country_code);
            CREATE INDEX temp_rdf_link_tmc_node_link_id_idx
              ON temp_rdf_link_tmc_node
              USING btree
              (link_id);
            CREATE INDEX temp_rdf_link_tmc_node_location_code_idx
              ON temp_rdf_link_tmc_node
              USING btree
              (location_code);
            CREATE INDEX temp_rdf_link_tmc_node_location_table_nr_idx
              ON temp_rdf_link_tmc_node
              USING btree
              (location_table_nr);                    
        """
        self.pg.execute2(sqlcmd)   
        self.pg.commit2()
        
        self.CreateFunction2('rdb_make_trf_link_seq')
        self.CreateFunction2('rdb_make_trf_link_seq_in_one_direction')        
        self.pg.commit2()
         
        # Give sequence to traffic links by searching road.        
        sqlcmd = """
        analyze temp_rdf_link_tmc_node;
        drop table if exists temp_rdf_link_tmc_seq;
        CREATE TABLE temp_rdf_link_tmc_seq
        (
          link_id bigint NOT NULL,
          seq integer,
          ebu_country_code character(1) NOT NULL,
          location_table_nr integer NOT NULL,
          location_code bigint NOT NULL,
          dir smallint,
          type smallint,
          road_direction character varying,
          group_id smallint
        );
        
        drop table if exists temp_rdf_link_tmc_walked;
        CREATE TABLE temp_rdf_link_tmc_walked
         (
           ebu_country_code character(1),
           location_table_nr integer,
           location_code bigint, 
           dir smallint,
           type smallint,
           link_id bigint
         );
        CREATE INDEX temp_rdf_link_tmc_walked_ebu_country_code_location_table_nr_idx
          ON temp_rdf_link_tmc_walked
          USING btree
          (ebu_country_code, location_table_nr, location_code, dir, type, link_id);

        select rdb_make_trf_link_seq(); 
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
 
        sqlcmd = """
            drop table if exists temp_rdf_link_tmc;
            create table temp_rdf_link_tmc as 
            select seq 
                ,link_id  
                ,ebu_country_code, location_table_nr
                ,case when type = 1 and dir = 0 then '+'
                    when type = 1 and dir = 1 then '-' 
                    when type = 0 and dir = 0 then 'P'
                    when type = 0 and dir = 1 then 'N'
                 end as tmc_path_direction
                ,location_code,road_direction,type,dir,group_id
            from temp_rdf_link_tmc_seq;
            analyze temp_rdf_link_tmc;                                  
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
                
        rdb_log.log(self.ItemName, 'creating sequence for tmc link -----end ', 'info') 
               
    def _createTRFTbl_prepare(self):
         
        self._createCountryCodeAndLang()
        
        sqlcmd = """
        DROP INDEX IF EXISTS nxfklinktmc_link;
        CREATE INDEX nxfklinktmc_link
          ON rdf_link_tmc
          USING btree
          (link_id);                          
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        # Create simple relationship between traffic and RDB link.       
        sqlcmd = """
        drop table if exists temp_trf_org2rdb_prepare;
        create table temp_trf_org2rdb_prepare as 
        select 0 as type_flag
            ,rdb_cnv_country_code(ebu_country_code) as area_code
            ,a.location_code as infra_id
            ,a.location_table_nr as extra_flag
            ,a.dir
            ,e.target_link_id as rdb_link_id
            ,case when e.flag = 't' and a.road_direction = 'T' then 0
                  when e.flag = 't' and a.road_direction = 'F' then 1
                  when e.flag = 'f' and a.road_direction = 'T' then 1
                  when e.flag = 'f' and a.road_direction = 'F' then 0
                  else 0
             end as link_dir  --- e.flag: wether order of link's shapepoints was reversed when link_merge.  
            ,a.type as pos_type
            ,e.s_fraction,e.e_fraction
            ,a.seq as seq_org
            ,case when g.gid is not null and a.road_direction = 'T' then (g.sub_count - g.sub_index) 
                  when g.gid is not null and a.road_direction = 'F' then g.sub_index
                  else 1 
            end as sub_seq  --- get sequence of small links when an original link was split(link_split). 
                            --- if link direction is 'T' then the order should be reversed. 
            ,a.group_id as group_id
            ,e.target_geom    
            ,e.flag as flag
        from temp_rdf_link_tmc a 
        left join temp_link_org_rdb e
        on a.link_id = e.org_link_id
        left join temp_split_newlink g
        on e.mid_link_id = g.link_id; 
        
        ANALYZE  temp_trf_org2rdb_prepare;                        
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()             
    
    def _createForTMCOnly(self):  
        
        self._createLocationTbl()
        self._createEventSupplTbl()
                                              
    def _createCountryCodeAndLang(self):
        pass               

    def _createLocationTbl(self):
        pass   
    def _createEventSupplTbl(self):
   
        sqlcmd = """
            select count(*) from pg_tables 
            where tablename = 'temp_trf_event' or tablename = 'temp_trf_supplementary';                          
            """
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchone2()
        
        if  row[0] == 2:
            self._createEventTbl()          
            self._createSupplementaryTbl()
        else:
            rdb_log.log(self.ItemName, '!!! No original event & supplementary table, Please check original data !!!', 'exception')         
    def _createEventTbl(self): 
        pass
    def _createSupplementaryTbl(self):
        pass

            
class rdb_traffic_rdf_ap(rdb_traffic_rdf):
    
    def _createCountryCodeAndLang(self):
                        
        sqlcmd = """
            CREATE OR REPLACE FUNCTION rdb_cnv_country_code(cc char)
              RETURNS integer
              LANGUAGE plpgsql
            AS $$
            DECLARE
                ecc character varying;
                cc_val integer;
                ecc_val_left integer;
                ecc_val_right integer; 
                ecc_left char;
                ecc_right char;
            BEGIN
                if cc = '8' then ecc = 'F2';
                elseif cc = '7' then ecc = 'F2';
                elseif cc = 'F' then ecc = 'F0';
                elseif cc = '2' then ecc = 'F3';
                elseif cc = 'C' then ecc = 'F2';
                elseif cc = 'A' then ecc = 'F2';
                end if;                
                                
                if cc = 'A' then cc_val = 10;
                elseif cc = 'B' then cc_val = 11;
                elseif cc = 'C' then cc_val = 12;
                elseif cc = 'D' then cc_val = 13;
                elseif cc = 'E' then cc_val = 14;
                elseif cc = 'F' then cc_val = 15;
                else cc_val = cast(cc as integer);
                end if; 
            
                ecc_left = substring(ecc,1,1);
                ecc_right = substring(ecc,2,1);
                
                if ecc_left = 'A' then ecc_val_left = 10;
                elseif ecc_left = 'B' then ecc_val_left = 11;
                elseif ecc_left = 'C' then ecc_val_left = 12;
                elseif ecc_left = 'D' then ecc_val_left = 13;
                elseif ecc_left = 'E' then ecc_val_left = 14;
                elseif ecc_left = 'F' then ecc_val_left = 15;
                else ecc_val_left = cast(ecc_left as integer);
                end if; 
            
                ecc_val_right = cast(ecc_right as integer);
                
                return     ((ecc_val_left << 20) | (ecc_val_right << 16)) |  cc_val;
            END;
            $$;                           
        """
        self.pg.execute2(sqlcmd) 
        self.pg.commit2() 
               
        self.CreateFunction2('rdb_get_json_string_for_traffic')
                               
    def _createLocationTbl(self):

        rdb_log.log(self.ItemName, 'creating location table ----- start', 'info')
        
        sqlcmd = """
            select count(*) from pg_tables 
            where tablename like 'gewi_%';                          
            """
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchone2()
        
        if  row[0] > 0:
            self.CreateTable2('temp_locationtable')           
        else:
            rdb_log.log(self.ItemName, '!!! No original location table, Please check original data !!!', 'exception')
            return 0
                              
        sqlcmd = """

        drop table if exists rdb_trf_locationtable;
        CREATE TABLE rdb_trf_locationtable
        (
          gid serial,
          country_code integer,
          table_no integer,
          location_code integer,
          type smallint,
          subtype smallint,
          location_type smallint,
          first_name character varying,
          second_name character varying,
          road_number character varying,
          road_name character varying,
          area_ref integer,
          line_ref integer,
          neg_offset integer,
          pos_offset integer,
          urban smallint,
          veran smallint,
          tern smallint,
          intersection_code integer,
          in_pos smallint,
          out_pos smallint,
          in_neg smallint,
          out_neg smallint,
          cur_pos smallint,
          cur_neg smallint,
          exit_no character varying,
          div_pos character varying,
          div_neg character varying
        );
                       
        insert into rdb_trf_locationtable(country_code, table_no, location_code, "type", subtype, 
               location_type, first_name, second_name, road_number, road_name, 
               area_ref, line_ref, neg_offset, pos_offset, urban, veran, tern, 
               intersection_code, in_pos, out_pos, in_neg, out_neg, cur_pos, 
               cur_neg, exit_no, div_pos, div_neg)
        select rdb_cnv_country_code(cc) as country_code
            ,table_no, location_code
            ,type, subtype
            ,case when location_type = 'P' then 1
                when location_type = 'L' then 2
                when location_type = 'A' then 3
                else 0
             end as location_type    
            ,case when first_name is not null then rdb_get_json_string_for_traffic('ENG', first_name) 
                    else rdb_get_json_string_for_traffic('ENG', '') 
             end as first_name
            ,case when second_name is not null then rdb_get_json_string_for_traffic('ENG', second_name) 
                    else rdb_get_json_string_for_traffic('ENG', '') 
             end as second_name
            ,road_number
            ,case when road_name is not null then rdb_get_json_string_for_traffic('ENG', road_name) 
                    else rdb_get_json_string_for_traffic('ENG', '') 
             end as road_name    
            ,area_ref, line_ref, neg_offset, pos_offset
            ,urban, veran, tern, intersection_code
            ,in_pos, out_pos, in_neg, out_neg
            ,cur_pos, cur_neg, exit_no, div_pos, div_neg
        from temp_locationtable;
                         
        """
        self.pg.execute2(sqlcmd) 
        self.pg.commit2()
        
        rdb_log.log(self.ItemName, 'creating location table ----- end', 'info')
                         
    def _createEventTbl(self):

        rdb_log.log(self.ItemName, 'creating event table ----- start', 'info')        
        sqlcmd = """
        drop table if exists rdb_trf_event;
        CREATE TABLE rdb_trf_event
        (
          gid serial,
          info_flag smallint,
          direction smallint,
          code integer,
          quantifier smallint,
          reference_flag smallint,
          referent character varying,
          urgency smallint,
          update_class smallint,
          "desc" character varying
        );
        
        insert into rdb_trf_event(info_flag, direction, code, quantifier, reference_flag, 
               referent, urgency, update_class, "desc")
        select  type as info_flag,
                dir as direction,
                eventcode as code,
                null as quantifier,
            ref as reference_flag,
            null as referent,
            urgency,
            cast(updateclass as smallint) as update_class,
            case when event_string is null then rdb_get_json_string_for_traffic('ENG', '')
                else rdb_get_json_string_for_traffic('ENG', event_string)
            end as "desc"
        from temp_trf_event;                           
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        rdb_log.log(self.ItemName, 'creating event table ----- end', 'info')
                        
    def _createSupplementaryTbl(self):
        
        rdb_log.log(self.ItemName, 'creating supplementary table ----- start', 'info')        
        sqlcmd = """
        drop table if exists rdb_trf_supplementary;
        CREATE TABLE rdb_trf_supplementary
        (
          gid serial,
          code integer,
          referent smallint,
          "desc" character varying
        );
        
        insert into rdb_trf_supplementary(code,referent,"desc")
        select cast(suplcode as smallint) as code,null as referent,
            case when suplstring is null then rdb_get_json_string_for_traffic('ENG', '')
                else rdb_get_json_string_for_traffic('ENG', suplstring)
            end as "desc"
        from temp_trf_supplementary;                           
        """
        self.pg.execute2(sqlcmd)   
        self.pg.commit2()     
        rdb_log.log(self.ItemName, 'creating supplementary table ----- end', 'info')

  
class rdb_traffic_nostra(rdb_traffic):
                          
    def _createTmcLinkSeq(self):
        
        rdb_log.log(self.ItemName, 'creating sequence for RTIC link -----start ', 'info')
        
        self.pg.execute2('''
            DROP INDEX IF EXISTS org_rtic_link_source_id_type_idx;
            CREATE INDEX org_rtic_link_source_id_type_idx
              ON org_rtic_link
              USING btree
              (cast(source_id as double precision));
            DROP INDEX IF EXISTS org_rtic_link_source_id_idx;
            CREATE INDEX org_rtic_link_source_id_idx
              ON org_rtic_link
              USING btree
              (source_id);
          ''')   
        self.pg.commit2()        

        # Add nodes to original traffic links.
        sqlcmd = """
            --- find start node & end node of network road.
            drop table if exists temp_rtic_link_order1;
            create table temp_rtic_link_order1 as
            select distinct a.source_id,b.start_node_id as s_node,b.end_node_id as e_node
            from org_rtic_link a
            left join temp_topo_link b    
            on cast(a.source_id as double precision) = b.routeid;
                        
            CREATE INDEX temp_rtic_link_order1_source_id_type_idx
              ON temp_rtic_link_order1
              USING btree
              (cast(source_id as double precision));
            CREATE INDEX temp_rtic_link_order1_source_id_idx
              ON temp_rtic_link_order1
              USING btree
              (source_id);
              
            --- calculate fraction of small rtic link from network road.
            drop table if exists temp_rtic_link_order2;
            create table temp_rtic_link_order2 as
            select source_id,s_node,e_node,linkid,road_geom,link_geom
                ,case when s_fraction > e_fraction then round(e_fraction::numeric,9)::double precision
                    else round(s_fraction::numeric,9)::double precision end as s_fraction
                ,case when s_fraction > e_fraction then round(s_fraction::numeric,9)::double precision
                    else round(e_fraction::numeric,9)::double precision end as e_fraction
                ,case when s_fraction > e_fraction then 1
                    else 0 end as rev_flag
            from (
                select c.source_id,c.s_node,c.e_node,d.linkid,e.the_geom as road_geom,d.the_geom as link_geom
                    ,ST_Line_Locate_Point(st_linemerge(e.the_geom), ST_StartPoint(d.the_geom)) as s_fraction
                    ,ST_Line_Locate_Point(st_linemerge(e.the_geom), ST_EndPoint(d.the_geom)) as e_fraction
                from temp_rtic_link_order1 c
                left join (
                    select distinct source_id,linkid,the_geom from org_rtic_link
                ) d
                on c.source_id = d.source_id
                left join org_l_tran e
                on cast(c.source_id as double precision) = e.routeid  
            ) f;
            
            CREATE INDEX temp_rtic_link_order2_source_id_s_fraction_idx
              ON temp_rtic_link_order2
              USING btree
              (source_id,s_fraction);
            CREATE INDEX temp_rtic_link_order2_source_id_linkid_idx
              ON temp_rtic_link_order2
              USING btree
              (source_id,linkid);

            ---give new node id to rtic link.
            drop sequence if exists temp_rtic_new_node_id_seq;
            create sequence temp_rtic_new_node_id_seq;
            select setval('temp_rtic_new_node_id_seq', cast(max_id as bigint) + 1000000)
            from
            (
                select max(node_id) as max_id
                from temp_topo_node
            )as a;

            drop table if exists temp_rtic_link_order3;
            create table temp_rtic_link_order3 as 
            select source_id,fraction,nextval('temp_rtic_new_node_id_seq') as node_id 
            from (
                select source_id,s_fraction as fraction from temp_rtic_link_order2
                union
                select source_id,e_fraction as fraction from temp_rtic_link_order2
            ) a 
            where fraction <> 0 and fraction <> 1;
                                 
            drop table if exists temp_rtic_link_order4;
            create table temp_rtic_link_order4 as
            select distinct source_id,s_node,e_node,linkid,road_geom,link_geom,s_fraction,e_fraction,rev_flag 
            from (
                select a.source_id
                    ,case when a.s_fraction = 0 then a.s_node
                        when a.s_fraction <> 0 and a.s_fraction = c.fraction then c.node_id
                        else null
                     end as s_node
                    ,case when a.e_fraction = 1 then a.e_node
                        when a.e_fraction <> 1 and a.e_fraction = d.fraction then d.node_id
                        else null
                     end as e_node
                    ,a.linkid,a.road_geom,a.link_geom,a.s_fraction,a.e_fraction,a.rev_flag
                from temp_rtic_link_order2 a
                left join temp_rtic_link_order3 c
                on a.source_id = c.source_id
                left join temp_rtic_link_order3 d
                on a.source_id = d.source_id                
            ) d where s_node is not null and e_node is not null;
            
            analyze temp_rtic_link_order4;
            
            CREATE INDEX temp_rtic_link_order4_source_id_linkid_idx
              ON temp_rtic_link_order4
              USING btree
              (source_id,linkid);
              
            ---get relationship of network road and small rtic links.
            drop table if exists temp_rtic_link_temp cascade;
            create table temp_rtic_link_temp as 
            select a.meshcode,a.linkid,b.s_node,b.e_node,a.source_id,a.kind
                ,case when b.rev_flag = 1 and a.linkdir = '1' then '2'
                    when b.rev_flag = 1 and a.linkdir = '2' then '1'
                    else a.linkdir
                end as linkdir
                ,a.rticid,b.s_fraction,b.e_fraction
                ,case when b.rev_flag = 1 then ST_Reverse(a.the_geom)
                    else a.the_geom
                end as the_geom
            from (
                SELECT mapid as meshcode, linkid, source_id
                    , kind_u as kind, dir_u as linkdir, middle_u as rticid,the_geom 
                FROM org_rtic_link where flag_u <> '0'
                union
                SELECT mapid as meshcode, linkid, source_id
                    , kind_d as kind, dir_d as linkdir, middle_d as rticid, the_geom
                FROM org_rtic_link where flag_d <> '0'
            ) a
            left join temp_rtic_link_order4 b
            on a.source_id = b.source_id and a.linkid = b.linkid;
            
            CREATE INDEX temp_rtic_link_temp_linkdir_idx
              ON temp_rtic_link_temp
              USING btree
              (linkdir);
            CREATE INDEX temp_rtic_link_temp_linkid_idx
              ON temp_rtic_link_temp
              USING btree
              (linkid);
            CREATE INDEX temp_rtic_link_temp_meshcode_kind_rticid_idx
              ON temp_rtic_link_temp
              USING btree
              (meshcode, kind, rticid);
            CREATE INDEX temp_rtic_link_temp_s_node_e_node_idx
              ON temp_rtic_link_temp
              USING btree
              (s_node, e_node);
            CREATE INDEX temp_rtic_link_temp_source_id_idx
              ON temp_rtic_link_temp
              USING btree
              (source_id);                      
        """
        self.pg.execute2(sqlcmd)   
        self.pg.commit2()
        
        self.CreateFunction2('rdb_make_trf_rtic_link_seq_thai')
        self.CreateFunction2('rdb_make_trf_rtic_link_seq_thai_in_one_direction')        
        self.pg.commit2()
         
        # Give sequence to traffic links by searching road.        
        sqlcmd = """
            drop table if exists temp_rtic_link;
            CREATE TABLE temp_rtic_link
            (
              meshcode character varying NOT NULL,
              kind character varying NOT NULL,
              rticid character varying NOT NULL,
              source_id character varying NOT NULL,
              linkid character varying NOT NULL,
              seq    integer,
              linkdir character varying NOT NULL,
              group_id smallint,
              s_fraction double precision,
              e_fraction double precision
            );
            
            drop table if exists temp_rtic_link_walked;
            CREATE TABLE temp_rtic_link_walked
             (
               meshcode character varying,
               kind character varying,
               rticid character varying,
               source_id character varying,
               linkid character varying
             );
            
            CREATE INDEX temp_rtic_link_walked_complex_idx
              ON temp_rtic_link_walked
              USING btree
              (meshcode, kind, rticid, source_id, linkid);
            
            analyze temp_rtic_link_temp;
            select rdb_make_trf_rtic_link_seq_thai();
            
            CREATE INDEX temp_rtic_link_source_id_1_idx
              ON temp_rtic_link
              USING btree
              (cast(source_id as double precision));   
              
            delete from temp_rtic_link a using (
                select distinct meshcode,kind,rticid from temp_rtic_link 
                where group_id > 0
                union
                select distinct mapid as meshcode,kind_u as kind,middle_u as rticid 
                from org_rtic_link where kind_d = kind_u and middle_d = middle_u
            ) b
            where a.meshcode = b.meshcode and a.kind = b.kind and a.rticid = b.rticid;              
              
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        rdb_log.log(self.ItemName, 'creating sequence for RTIC link -----end ', 'info') 
                                   
    def _createTRFTbl_prepare(self):
        
        # Create percents of rtic link on rdb link, by rtic/org  ->  org/rdb. 
        sqlcmd = """
            drop table if exists temp_link_org_rdb_org;
            create table temp_link_org_rdb_org as
            select distinct org_link_id,linkid,mid_link_id,target_link_id,target_geom,s_fraction,e_fraction,flag
            from (
                select distinct org_link_id,linkid,rtic_s_fraction,rtic_e_fraction
                    ,org_geom,org_s_fraction,org_e_fraction
                    ,mid_link_id,mid_geom,rdb_s_fraction,rdb_e_fraction
                    ,target_link_id,target_geom,rtic2org_s_fraction,rtic2org_e_fraction,flag
                    ,rdb_s_fraction + 
                        ((rtic2org_s_fraction - org_s_fraction) / (org_e_fraction - org_s_fraction)) 
                        * (rdb_e_fraction - rdb_s_fraction) as s_fraction
                    ,rdb_s_fraction + 
                        ((rtic2org_e_fraction - org_s_fraction) / (org_e_fraction - org_s_fraction)) 
                        * (rdb_e_fraction - rdb_s_fraction) as e_fraction        
                from (
                    select b.org_link_id,a.linkid,a.s_fraction as rtic_s_fraction,a.e_fraction as rtic_e_fraction
                        ,b.org_geom,b.org_s_fraction,b.org_e_fraction
                        ,b.mid_link_id,b.mid_geom
                        ,b.rdb_s_fraction,b.rdb_e_fraction
                        ,b.target_link_id,b.target_geom,b.flag
                        ,case when a.s_fraction >= b.org_s_fraction and a.s_fraction < b.org_e_fraction then a.s_fraction
                            when a.s_fraction > b.org_e_fraction or a.e_fraction < b.org_s_fraction then null
                            else b.org_s_fraction
                         end as rtic2org_s_fraction
                        ,case when a.e_fraction <= b.org_e_fraction and a.e_fraction > b.org_s_fraction then a.e_fraction
                            when a.s_fraction > b.org_e_fraction or a.e_fraction < b.org_s_fraction then null
                            else b.org_e_fraction
                         end as rtic2org_e_fraction
                    from temp_rtic_link_order2 a
                    left join temp_link_org_rdb_org_temp b
                    on cast(a.source_id as bigint) = b.org_link_id
                    where a.e_fraction > b.org_s_fraction or b.org_e_fraction > a.s_fraction
                ) c where rtic2org_s_fraction is not null and rtic2org_e_fraction is not null
            ) d;                          
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        # Create simple relationship between traffic and RDB link.       
        sqlcmd = """
        drop table if exists temp_trf_org2rdb_prepare;
        create table temp_trf_org2rdb_prepare as 
        select 1::smallint as type_flag, a.meshcode::integer as area_code
            , a.rticid::integer as infra_id, a.kind::integer - 1 as extra_flag
            , 0::smallint as dir
            , b.target_link_id as rdb_link_id
            , case  when b.flag = 't' and a.linkdir = '1' then 1
                when b.flag = 't' and a.linkdir = '2' then 0
                when b.flag = 'f' and a.linkdir = '1' then 0 
                when b.flag = 'f' and a.linkdir = '2' then 1
              end as link_dir
            , 0::smallint as pos_type
            , b.s_fraction as s_fraction, b.e_fraction as e_fraction
            , a.seq as seq_org
            ,case when g.gid is not null and a.linkdir = '2' then (g.sub_count - g.sub_index) 
                  when g.gid is not null and a.linkdir = '1' then g.sub_index
                  else 1 
             end as sub_seq
            , a.group_id
            , b.target_geom
            , b.flag
        from temp_rtic_link a
        left join temp_link_org_rdb_org b
        on cast(a.source_id as double precision) = b.org_link_id and a.linkid = b.linkid
        left join temp_split_newlink g
        on b.mid_link_id = g.link_id;
        
        ANALYZE  temp_trf_org2rdb_prepare;                        
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()           
                                  
    def _checkResult_special(self):  
        
        rdb_log.log(self.ItemName, 'check rdb_trf_org2rdb(thai rtic)  --- start.', 'info')      
        sqlcmd = """
            select count(*) from 
            (    select type_flag, area_code, extra_flag, infra_id, dir, group_id, max(seq) as sum
                from rdb_trf_org2rdb group by type_flag, area_code, extra_flag, infra_id, dir, group_id
            ) a
            left join 
            (
                select type_flag, area_code, extra_flag, infra_id, dir, group_id, count(seq) as sum
                from rdb_trf_org2rdb group by type_flag, area_code, extra_flag, infra_id, dir, group_id
            ) b
            on a.area_code = b.area_code and a.extra_flag = b.extra_flag and a.infra_id = b.infra_id and a.dir = b.dir and a.group_id = b.group_id
            where a.sum <> b.sum 
        union
            select count(*) from temp_rtic_link_order2 where (s_fraction < 0 or e_fraction < 0) or (s_fraction > 1 or e_fraction > 1)
        union
            select count(*) from (
                select * from rdb_trf_org2rdb where pos_type = 0
            ) a
            left join (
                select type_flag,area_code,extra_flag,infra_id,dir,group_id
                ,min(seq) as min, max(seq) as max
                from (
                select * from rdb_trf_org2rdb 
                where pos_type = 0
                ) m group by type_flag, area_code, extra_flag, infra_id, dir,group_id
            ) b
            on a.area_code = b.area_code and a.extra_flag = b.extra_flag 
            and a.infra_id = b.infra_id and a.dir = b.dir and a.group_id = b.group_id
            where  
            ((a.e_fraction <> 65535 and a.link_dir = 0 and not (a.seq = b.max))
            or (a.s_fraction <> 0 and a.link_dir = 0 and not (a.seq = min))
            or (a.e_fraction <> 65535 and a.link_dir = 1 and not (a.seq = min))
            or (a.s_fraction <> 0 and a.link_dir = 1 and not (a.seq = b.max)))
        union
            select count(*) from (
                select * from rdb_trf_org2rdb 
                where pos_type = 1
            ) a
            left join (
                select type_flag,area_code,extra_flag,infra_id,dir,group_id,count(seq) as sum
                from (
                select * from rdb_trf_org2rdb 
                where pos_type = 1
                ) m group by type_flag, area_code, extra_flag, infra_id, dir,group_id
            ) b
            on a.area_code = b.area_code and a.extra_flag = b.extra_flag 
            and a.infra_id = b.infra_id and a.dir = b.dir and a.group_id = b.group_id
            where 
            ((a.e_fraction <> 65535 and a.link_dir = 0 and not (a.seq = b.sum))
            or (a.s_fraction <> 0 and a.link_dir = 0 and not (a.seq = 1))
            or (a.e_fraction <> 65535 and a.link_dir = 1 and not (a.seq = 1))
            or (a.s_fraction <> 0 and a.link_dir = 1 and not (a.seq = b.sum)))
        union
            select count(*) from rdb_trf_org2rdb a
            left join (
                select * from rdb_trf_org2rdb where ctn_flag = 0
            ) b
            on a.area_code = b.area_code and a.extra_flag = b.extra_flag 
            and a.infra_id = b.infra_id and a.dir = b.dir and a.group_id = b.group_id 
            and a.seq = b.seq + 1
            where b.rdb_link_id is not null 
            and (a.rdb_link_id <> b.rdb_link_id or 
                not ((a.pos_type = 1 and b.pos_type = 0) 
                or (a.pos_type = 0 and b.pos_type = 1))
            )
        union
            select count(*) from rdb_trf_org2rdb a
            left join rdb_trf_org2rdb b
            on a.area_code = b.area_code and a.extra_flag = b.extra_flag 
            and a.infra_id = b.infra_id and a.dir = b.dir and a.group_id = b.group_id 
            and a.seq = b.seq - 1 and a.rdb_link_id = b.rdb_link_id
            where b.rdb_link_id is not null and b.s_fraction <> a.e_fraction;                                                                   
           """
           
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchall2()
        if row:
            if len(row) > 1:
                rdb_log.log(self.ItemName, 'check table rdb_trf_org2rdb(thai rtic) Failed.', 'error') 
            else:
                rdb_log.log(self.ItemName, 'check rdb_trf_org2rdb(thai rtic)  --- end.', 'info')

        
class rdb_traffic_region():
    '''region traffic
    '''
    def  __init__(self, base, log):
        '''
        Constructor
        '''
        self.item = base
        self.pg = self.item.pg
        rdb_log = log
            
    def _createRegionXTRFTbl(self,X):
        rdb_log.log('REGION Traffic', 'creating region %s trf ----- start'%X, 'info') 
        sqlcmd_X = """
        DROP INDEX IF EXISTS rdb_region_layer%X_link_mapping_link_dir_14_idx;
        CREATE INDEX rdb_region_layer%X_link_mapping_link_dir_14_idx
          ON rdb_region_layer%X_link_mapping
          USING btree
          (link_dir_14);
        DROP INDEX IF EXISTS rdb_region_layer%X_link_mapping_link_id_14_idx;
        CREATE INDEX rdb_region_layer%X_link_mapping_link_id_14_idx
          ON rdb_region_layer%X_link_mapping
          USING btree
          (link_id_14);                         
        """
        sqlcmd = sqlcmd_X.replace('%X',X)
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        # create table for ID & geometry relationship between region link and layer14 link.
        sqlcmd = """
        drop table if exists temp_region_trf_org2rdb_layer%X_prepare1;
        create table temp_region_trf_org2rdb_layer%X_prepare1 as
        select p.*,q.the_geom as rdb_geom from (
            select region_link_id
                ,unnest(link_id_14) as link_id_14
                ,unnest(link_dir_14) as link_dir  --- wether order of shapepoints is reversed when merge.
                ,generate_series(1,len) as seq   --- sequence of links when merge.
            from ( 
                select *,array_length(link_id_14,1) as len
                from rdb_region_layer%X_link_mapping
            ) o
        ) p
        left join rdb_link q
        on p.link_id_14 = q.link_id; 
        
        DROP INDEX IF EXISTS temp_region_trf_org2rdb_layer%X_prepare1_region_link_id_idx;
        CREATE INDEX temp_region_trf_org2rdb_layer%X_prepare1_region_link_id_idx
          ON temp_region_trf_org2rdb_layer%X_prepare1
          USING btree
          (region_link_id);
        DROP INDEX IF EXISTS temp_region_trf_org2rdb_layer%X_prepare1_link_id_14_idx;
        CREATE INDEX temp_region_trf_org2rdb_layer%X_prepare1_link_id_14_idx
          ON temp_region_trf_org2rdb_layer%X_prepare1
          USING btree
          (link_id_14); 
        DROP INDEX IF EXISTS temp_region_trf_org2rdb_layer%X_prepare1_link_dir_idx;
        CREATE INDEX temp_region_trf_org2rdb_layer%X_prepare1_link_dir_idx
          ON temp_region_trf_org2rdb_layer%X_prepare1
          USING btree
          (link_dir);  
        DROP INDEX IF EXISTS temp_region_trf_org2rdb_layer%X_prepare1_rdb_geom_idx;
        CREATE INDEX temp_region_trf_org2rdb_layer%X_prepare1_rdb_geom_idx
          ON temp_region_trf_org2rdb_layer%X_prepare1
          USING gist
          (rdb_geom);  
          
        drop table if exists temp_region_trf_org2rdb_layer%X_prepare2;
        create table temp_region_trf_org2rdb_layer%X_prepare2 as        
        select    region_link_id
                , st_linefrommultipoint(st_collect(geom_point)) as region_geom
        from
        (
            select    region_link_id, geom as geom_point
            from
            (
                select    region_link_id, (st_dumppoints(rdb_geom_multiline)).*
                from
                (
                    select region_link_id, st_collect(rdb_geom) as rdb_geom_multiline
                    from
                    (
                    select region_link_id,link_id_14,link_dir
                        ,case when link_dir = 'f' then st_reverse(rdb_geom)
                             else rdb_geom  --- if link_dir = 'f', order of shapepoints should be reversed.
                        end as rdb_geom
                    from temp_region_trf_org2rdb_layer%X_prepare1
                    order by region_link_id,seq
                    ) a group by region_link_id
                ) b
            )as c
            where not (path[1] > 1 and path[2] = 1)
            order by region_link_id, path
        )as d
        group by region_link_id; 

        DROP INDEX IF EXISTS temp_region_trf_org2rdb_layer%X_prepare2_region_link_id_idx;
        CREATE INDEX temp_region_trf_org2rdb_layer%X_prepare2_region_link_id_idx
          ON temp_region_trf_org2rdb_layer%X_prepare2
          USING btree
          (region_link_id);
        DROP INDEX IF EXISTS temp_region_trf_org2rdb_layer%X_prepare2_region_geom_idx;  
        CREATE INDEX temp_region_trf_org2rdb_layer%X_prepare2_region_geom_idx
          ON temp_region_trf_org2rdb_layer%X_prepare2
          USING gist
          (region_geom);                                                         
        """
        sqlcmd = sqlcmd.replace('%X',X)
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        sqlcmd = """
        analyze temp_region_trf_org2rdb_layer%X_prepare1;
        analyze temp_region_trf_org2rdb_layer%X_prepare2;
        drop table if exists temp_region_trf_org2rdb_layer%X_prepare3;
        create table temp_region_trf_org2rdb_layer%X_prepare3 as 
        select a.region_link_id,a.link_id_14,a.link_dir,a.seq,a.rdb_geom,b.region_geom,
            ST_Length_Spheroid(a.rdb_geom,'SPHEROID("WGS_84", 6378137, 298.257223563)') 
                as rdb_len,  --- length of rdb link.
            ST_Length_Spheroid(b.region_geom,'SPHEROID("WGS_84", 6378137, 298.257223563)')  
                as region_len    --- length of region link.
        from temp_region_trf_org2rdb_layer%X_prepare1 a
        left join temp_region_trf_org2rdb_layer%X_prepare2 b
        on a.region_link_id = b.region_link_id;
        
        DROP INDEX IF EXISTS temp_region_trf_org2rdb_layer%X_prepare3_region_link_id_idx;
        CREATE INDEX temp_region_trf_org2rdb_layer%X_prepare3_region_link_id_idx
          ON temp_region_trf_org2rdb_layer%X_prepare3
          USING btree
          (region_link_id);
        DROP INDEX IF EXISTS temp_region_trf_org2rdb_layer%X_prepare3_seq_idx;  
        CREATE INDEX temp_region_trf_org2rdb_layer%X_prepare3_seq_idx
          ON temp_region_trf_org2rdb_layer%X_prepare3
          USING btree
          (seq);                         
        """
        sqlcmd = sqlcmd.replace('%X',X)
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        # Create percentage of small traffic links from region links.
        sqlcmd = """
        drop table if exists temp_region_trf_org2rdb_layer%X_prepare4;
        create table temp_region_trf_org2rdb_layer%X_prepare4 as 
        select d.*,
            case when add_len is null then 0 
                else round((add_len/region_len)::numeric,9)::double precision
            end as rdb_s_fraction
            ,case when add_len is null then round((rdb_len/region_len)::numeric,9)::double precision
                else round(((rdb_len + add_len)/region_len)::numeric,9)::double precision
            end as rdb_e_fraction
        from (
            select region_link_id,link_id_14,link_dir,seq,rdb_len,region_len,sum(add_len) as add_len
            from (
                select a.*,b.rdb_len as add_len 
                from temp_region_trf_org2rdb_layer%X_prepare3 a
                left join  temp_region_trf_org2rdb_layer%X_prepare3 b
                on a.region_link_id = b.region_link_id and a.seq > b.seq 
            ) c 
            group by region_link_id,link_id_14,link_dir,seq,rdb_len,region_len    
        ) d order by region_link_id,seq ;

        drop table if exists temp_region_trf_org2rdb_layer%X_prepare5;
        create table  temp_region_trf_org2rdb_layer%X_prepare5 as  
        select type_flag,area_code,infra_id,extra_flag,dir,region_link_id as rdb_link_id
            ,(region_link_id >> 32) as region_link_t,link_dir,pos_type
            ,round(s_fraction::numeric,0) as s_fraction
            ,round(e_fraction::numeric,0) as e_fraction
            ,seq,group_id,rdb_cnv_length(length) as length
            ,'0'::integer as infra_len
            ,'0'::character varying as geom_string
        from (
            select d.type_flag,d.area_code,d.infra_id,d.extra_flag,d.dir,a.region_link_id
                ,case when a.link_dir = 'f' and d.link_dir = 0 then 1
                      when a.link_dir = 'f' and d.link_dir = 1 then 0
                      else d.link_dir
                 end as link_dir,d.pos_type
                ,case when a.link_dir = 'f' then
                        ((rdb_s_fraction + 
                            (rdb_e_fraction - rdb_s_fraction) * ((65535 - e_fraction) / 65535.0)
                        ) * 65535)
                    else ((rdb_s_fraction + 
                            (rdb_e_fraction - rdb_s_fraction) * (s_fraction / 65535.0)
                        ) * 65535) 
                 end as s_fraction
                ,case when a.link_dir = 'f' then
                        ((rdb_s_fraction + 
                            (rdb_e_fraction - rdb_s_fraction) * ((65535 - s_fraction) / 65535.0)
                        ) * 65535)
                    else ((rdb_s_fraction + 
                        (rdb_e_fraction - rdb_s_fraction) * (e_fraction / 65535.0)
                    ) * 65535) 
                 end as e_fraction
                ,rdb_s_fraction
                ,rdb_e_fraction
                ,d.seq
                ,d.group_id
                ,a.region_len as length
            from temp_region_trf_org2rdb_layer%X_prepare4 a
            left join rdb_trf_org2rdb d
            on a.link_id_14 = d.rdb_link_id
            where d.rdb_link_id is not null
        ) e;                        
        """
        sqlcmd = sqlcmd.replace('%X',X)
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
                                   
        self.item.CreateFunction2('rdb_make_trf_region_link_seqs')

        # create new sequence for region traffic links. cause region links is less than layer14 links
        # which gives rise to the problem that traffic links of region are not continous any more.
        sqlcmd = """
        drop table if exists temp_region_trf_org2rdb_layer%X_prepare6;
        CREATE TABLE temp_region_trf_org2rdb_layer%X_prepare6
        (
          type_flag smallint,
          area_code integer,
          infra_id integer,
          extra_flag integer,
          dir smallint,
          rdb_link_id bigint,
          rdb_link_t integer,
          link_dir smallint,
          pos_type smallint,
          s_fraction integer,
          e_fraction integer,
          seq integer,
          ctn_flag smallint,
          group_id smallint,  
          length integer,
          infra_len integer,
          geom_string character varying
        );
        
        select rdb_make_trf_region_link_seqs('temp_region_trf_org2rdb_layer%X_prepare5',
                    'temp_region_trf_org2rdb_layer%X_prepare6');                                
        """
        sqlcmd = sqlcmd.replace('%X',X)
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.item.CreateFunction2('rdb_make_trf_merge_links')

        # merge continuous links which have the same location code, same group id and same link id.
        sqlcmd = """
        drop table if exists rdb_region_trf_org2rdb_layer%X;
        CREATE TABLE rdb_region_trf_org2rdb_layer%X
        (
          gid serial primary key,
          type_flag smallint,
          area_code integer,
          infra_id integer,
          extra_flag integer,
          dir smallint,
          rdb_link_id bigint,
          rdb_link_t integer,
          link_dir smallint,
          pos_type smallint,
          s_fraction integer,
          e_fraction integer,
          seq integer,
          ctn_flag smallint default 1,
          group_id smallint,  
          length integer,
          infra_len integer,
          geom_string character varying
        );
        
        --- merge fractions of links with the same location code, same dir, same group, same linkid, same linkdir.
        select rdb_make_trf_merge_links('temp_region_trf_org2rdb_layer%X_prepare6',
                    'rdb_region_trf_org2rdb_layer%X');

        --- in a same location code, same dir, same group, 
        --- if there's two consecutive links which share a same linkid, but have different fraction,
        --- set the prior's ctn_flag = 0.                
        update rdb_region_trf_org2rdb_layer%X set ctn_flag = 0 
        from rdb_region_trf_org2rdb_layer%X b
        where rdb_region_trf_org2rdb_layer%X.area_code = b.area_code 
        and rdb_region_trf_org2rdb_layer%X.extra_flag = b.extra_flag 
        and rdb_region_trf_org2rdb_layer%X.infra_id = b.infra_id 
        and rdb_region_trf_org2rdb_layer%X.dir = b.dir 
        and rdb_region_trf_org2rdb_layer%X.group_id = b.group_id 
        and rdb_region_trf_org2rdb_layer%X.rdb_link_id = b.rdb_link_id 
        and rdb_region_trf_org2rdb_layer%X.seq = b.seq - 1;
        
        --- drop unused column.
        alter table rdb_region_trf_org2rdb_layer%X drop column infra_len;
        alter table rdb_region_trf_org2rdb_layer%X drop column geom_string;                                                               
        """
        sqlcmd = sqlcmd.replace('%X',X)
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        rdb_log.log('REGION Traffic', 'creating region %s trf ----- end'%X, 'info')             

    def _checkRegionXResult(self, X):  
        
        rdb_log.log('REGION Traffic', 'check rdb_region_trf_org2rdb_layer%s  --- start.'%X, 'info')      
        sqlcmd = """
        --- in a same location code, same dir, same group, linkid & linkdir & fraction should be only.
        select count(*) from (
            select area_code,extra_flag,infra_id,dir,group_id,rdb_link_id,link_dir,s_fraction 
            from rdb_region_trf_org2rdb_layer%X 
            group by area_code,extra_flag,infra_id,dir,group_id,rdb_link_id,link_dir,s_fraction
            having count(*) >  1
        ) a
        union 
        --- fraction's value should be effective.
        select count(*) from rdb_region_trf_org2rdb_layer%X 
        where (s_fraction = 0 and e_fraction = 0) or (s_fraction = 65535 and e_fraction = 65535)
        union
        select count(*) from rdb_region_trf_org2rdb_layer%X 
        where (s_fraction < 0 or e_fraction < 0) or (s_fraction > 65535 or e_fraction > 65535)
        union
        --- check the merge of fraction is correct or not.
        select count(*) from rdb_region_trf_org2rdb_layer%X a
        left join (
            select area_code,extra_flag,infra_id,dir,group_id,rdb_link_id,
                s_fraction,
                e_fraction,
                count
            from (
                select area_code,extra_flag,infra_id,dir,group_id
                    ,rdb_link_id,unnest(s_fraction_array) as s_fraction
                    ,unnest(e_fraction_array) as e_fraction,count 
                from (
                    select area_code,extra_flag,infra_id,dir,group_id
                        ,rdb_link_id,array_agg(s_fraction) as s_fraction_array
                        ,array_agg(e_fraction) as e_fraction_array
                        ,count(*) as count
                     from temp_region_trf_org2rdb_layer%X_prepare6
                     group by area_code,extra_flag,infra_id,dir,group_id,rdb_link_id
                ) c
            ) pre
        ) b
        on a.area_code = b.area_code and a.extra_flag = b.extra_flag 
        and a.infra_id = b.infra_id and a.dir = b.dir and a.group_id = b.group_id 
        and a.rdb_link_id = b.rdb_link_id
        where (a.s_fraction <> b.s_fraction or a.e_fraction <> b.e_fraction) and b.count = 1 
        union
        --- link sequence should be continuous.
        select count(*) from 
        (    select type_flag, area_code, extra_flag, infra_id, dir,group_id,max(seq) as sum
            from rdb_region_trf_org2rdb_layer%X 
            group by type_flag, area_code, extra_flag, infra_id, dir,group_id
        ) a
        left join 
        (
            select type_flag, area_code, extra_flag, infra_id, dir,group_id,count(seq) as sum
            from rdb_region_trf_org2rdb_layer%X 
            group by type_flag, area_code, extra_flag, infra_id, dir,group_id
        ) b
        on a.area_code = b.area_code and a.extra_flag = b.extra_flag 
        and a.infra_id = b.infra_id and a.dir = b.dir and a.group_id = b.group_id
        where a.sum <> b.sum 
        union
        select count(*) from 
        (    select type_flag, area_code, extra_flag, infra_id, dir,group_id, seq
            from rdb_region_trf_org2rdb_layer%X where pos_type = 1
        ) a
        left join 
        (
            select type_flag, area_code, extra_flag, infra_id, dir,group_id, seq
            from rdb_region_trf_org2rdb_layer%X where pos_type = 0
        ) b
        on a.area_code = b.area_code and a.extra_flag = b.extra_flag 
        and a.infra_id = b.infra_id and a.dir = b.dir and a.group_id = b.group_id
        left join (
            select a.* from 
            (    select type_flag, area_code, extra_flag, infra_id, dir,group_id, seq
                from rdb_region_trf_org2rdb_layer%X where pos_type = 1
            ) a
            left join 
            (
                select type_flag, area_code, extra_flag, infra_id, dir,group_id, seq
                from rdb_region_trf_org2rdb_layer%X where pos_type = 0
            ) b
            on a.area_code = b.area_code and a.extra_flag = b.extra_flag 
            and a.infra_id = b.infra_id and a.dir = b.dir and a.group_id = b.group_id
            where a.seq is not null and b.seq is not null and a.seq > b.seq
        ) d
        on a.area_code = d.area_code and a.extra_flag = d.extra_flag 
        and a.infra_id = d.infra_id and a.dir = d.dir and a.group_id = d.group_id
        where d.seq is null 
        and (a.seq is not null and b.seq is not null and a.seq > b.seq)
        --- record of 's_fraction <> 0 or e_fraction <> 65535', 
        --- it should be infra_link start or infra_link end or border link of external/internal .
        union
        select count(*) from (
            select * from rdb_region_trf_org2rdb_layer%X 
            where pos_type = 1
        ) a
        left join (
            select type_flag,area_code,extra_flag,infra_id,dir,group_id,count(seq) as sum
            from (
                select * from rdb_region_trf_org2rdb_layer%X 
                where pos_type = 1
            ) m group by type_flag, area_code, extra_flag, infra_id, dir,group_id
        ) b
        on a.area_code = b.area_code and a.extra_flag = b.extra_flag 
        and a.infra_id = b.infra_id and a.dir = b.dir and a.group_id = b.group_id
        left join (
            select distinct area_code,extra_flag,infra_id,dir,group_id 
            from rdb_trf_org2rdb m
            left join temp_region_trf_org2rdb_layer%X_prepare1 n
            on m.rdb_link_id = n.link_id_14
            where n.link_id_14 is null
        ) c
        on a.area_code = c.area_code and a.extra_flag = c.extra_flag 
        and a.infra_id = c.infra_id and a.dir = c.dir and a.group_id = c.group_id
        left join (
            select a.* from 
            (    select type_flag, area_code, extra_flag, infra_id, dir,group_id, seq
                from rdb_region_trf_org2rdb_layer%X where pos_type = 1
            ) a
            left join 
            (
                select type_flag, area_code, extra_flag, infra_id, dir,group_id, seq
                from rdb_region_trf_org2rdb_layer%X where pos_type = 0
            ) b
            on a.area_code = b.area_code and a.extra_flag = b.extra_flag 
            and a.infra_id = b.infra_id and a.dir = b.dir and a.group_id = b.group_id
            where a.seq is not null and b.seq is not null and a.seq > b.seq
        ) d
        on a.area_code = d.area_code and a.extra_flag = d.extra_flag 
        and a.infra_id = d.infra_id and a.dir = d.dir and a.group_id = d.group_id
        where c.area_code is null and d.area_code is null and 
        ((a.e_fraction <> 65535 and a.link_dir = 0 and not (a.seq = b.sum))
        or (a.s_fraction <> 0 and a.link_dir = 0 and not (a.seq = 1))
        or (a.e_fraction <> 65535 and a.link_dir = 1 and not (a.seq = 1))
        or (a.s_fraction <> 0 and a.link_dir = 1 and not (a.seq = b.sum)))
        union
        select count(*) from (
            select * from rdb_region_trf_org2rdb_layer%X where pos_type = 0
        ) a
        left join (
            select type_flag,area_code,extra_flag,infra_id,dir,group_id
                ,min(seq) as min, max(seq) as max
            from (
                select * from rdb_region_trf_org2rdb_layer%X 
                where pos_type = 0
            ) m group by type_flag, area_code, extra_flag, infra_id, dir,group_id
        ) b
        on a.area_code = b.area_code and a.extra_flag = b.extra_flag 
        and a.infra_id = b.infra_id and a.dir = b.dir and a.group_id = b.group_id
        left join (
            select distinct area_code,extra_flag,infra_id,dir,group_id 
            from rdb_trf_org2rdb m
            left join temp_region_trf_org2rdb_layer%X_prepare1 n
            on m.rdb_link_id = n.link_id_14
            where n.link_id_14 is null
        ) c
        on a.area_code = c.area_code and a.extra_flag = c.extra_flag 
        and a.infra_id = c.infra_id and a.dir = c.dir and a.group_id = c.group_id
        left join (
            select a.* from 
            (    select type_flag, area_code, extra_flag, infra_id, dir,group_id, seq
                from rdb_region_trf_org2rdb_layer%X where pos_type = 1
            ) a
            left join 
            (
                select type_flag, area_code, extra_flag, infra_id, dir,group_id, seq
                from rdb_region_trf_org2rdb_layer%X where pos_type = 0
            ) b
            on a.area_code = b.area_code and a.extra_flag = b.extra_flag 
            and a.infra_id = b.infra_id and a.dir = b.dir and a.group_id = b.group_id
            where a.seq is not null and b.seq is not null and a.seq > b.seq
        ) d
        on a.area_code = d.area_code and a.extra_flag = d.extra_flag 
        and a.infra_id = d.infra_id and a.dir = d.dir and a.group_id = d.group_id
        where c.area_code is null and d.area_code is null and 
        ((a.e_fraction <> 65535 and a.link_dir = 0 and not (a.seq = b.max))
        or (a.s_fraction <> 0 and a.link_dir = 0 and not (a.seq = min))
        or (a.e_fraction <> 65535 and a.link_dir = 1 and not (a.seq = min))
        or (a.s_fraction <> 0 and a.link_dir = 1 and not (a.seq = b.max)))
        union
        --- record of 'ctn_flag = 0', it's next record should has the same linkid, but different pos_type.   
        select count(*) from rdb_region_trf_org2rdb_layer%X a
        left join (
            select * from rdb_region_trf_org2rdb_layer%X where ctn_flag = 0
        ) b
        on a.area_code = b.area_code and a.extra_flag = b.extra_flag 
        and a.infra_id = b.infra_id and a.dir = b.dir and a.group_id = b.group_id 
        and a.seq = b.seq + 1
        where b.rdb_link_id is not null 
        and (a.rdb_link_id <> b.rdb_link_id or 
            not ((a.pos_type = 1 and b.pos_type = 0) 
                or (a.pos_type = 0 and b.pos_type = 1))
        );                                                          
           """

        sqlcmd = sqlcmd.replace('%X',X)
        self.pg.execute2(sqlcmd)          
        row = self.pg.fetchall2()
        if row:
            if len(row) > 1:
                rdb_log.log('REGION Traffic', 'check rdb_region_trf_org2rdb_layer%s Failed.'%X, 'error') 
            else:
                rdb_log.log('REGION Traffic', 'check rdb_region_trf_org2rdb_layer%s  --- end.'%X, 'info')

