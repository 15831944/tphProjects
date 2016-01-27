# -*- coding: cp936 -*-
'''
Created on 2013-10-17

@author: yuanrui
'''
from base.rdb_ItemBase import ItemBase
from common import rdb_log
from common import rdb_common

import os

class rdb_vics(ItemBase):
    @staticmethod
    def instance():
        proj_mapping = {
            ('jpn'):                rdb_vics_jpn(),
            ('jdb'):                rdb_vics_jpn(),
            ('axf'):                rdb_vics_axf(),
            ('ta','aus'):           rdb_traffic_ta(),
            ('ta','saf8'):          rdb_traffic_ta(),
            ('rdf','sgp'):          rdb_traffic_rdf_ap(),
            ('rdf','uc'):           rdb_traffic_rdf_uc(),
            ('rdf','me8'):          rdb_vics(),
			('rdf','mea'):          rdb_vics(),
            ('rdf','bra'):          rdb_vics(), 
            ('rdf','ase'):          rdb_traffic_rdf_ase(),                       
            ('nostra'):             rdb_traffic_nostra(),
            ('ni'):                 rdb_traffic_ni(),
            ('zenrin'):             rdb_traffic_zenrin(), 
            ('default'):            rdb_vics(),                      
        }
        return rdb_common.getItem(proj_mapping)

    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self, 'Vics_Dummy')

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
#        self._restoreVicsFromIpc()
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
#        self.CreateIndex2('lq_ipcvics_inf_id_idx')
#        self.CreateIndex2('lq_ipcvics_link_id_idx')
#        self.CreateIndex2('lq_ipcvics_link_id_idx')
                                
        self.CreateTable2('temp_vics_org2rdb_prepare_jpn')
#        self.CreateTable2('temp_ipc_vics_org2rdb_prepare_jpn') 
        self.CreateTable2('temp_vics_org2rdb_seq_jpn')
#        self.CreateTable2('temp_ipc_vics_org2rdb_seq_jpn')
        
        self.CreateTable2('rdb_vics_org2rdb_jpn')
#        self.CreateTable2('rdb_ipc_vics_org2rdb_jpn')
        
        self.CreateFunction2('rdb_make_vics_merge_links_jpn')
        
        sqlcmd = """
        select rdb_make_vics_merge_links_jpn('temp_vics_org2rdb_seq_jpn','rdb_vics_org2rdb_jpn');
---        select rdb_make_vics_merge_links_jpn('temp_ipc_vics_org2rdb_seq_jpn','rdb_ipc_vics_org2rdb_jpn');
            """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()          
        
        self._createRDBVicsSeq()
#        self._createRDBIPCVicsSeq()
        
        self._checkResult()

    def _createRDBVicsSeq(self):
        
        # prepare a common table.
        self.CreateTable2('temp_vics_org2rdb_with_node')        
        sqlcmd = """
            insert into temp_vics_org2rdb_with_node(class0, class1, class2, class3, class4, link_id, linkdir, s_fraction, 
               e_fraction, s_point, e_point, s_node, e_node, first_flag)
            select a.meshcode as class0,a.vicsclass as class1,a.vicsid as class2,0::smallint as class3
                ,null::character varying as class4,a.target_link_id as link_id
                ,a.linkdir - 1 as linkdir
                ,a.s_fraction,a.e_fraction
                ,a.s_point,a.e_point
                ,case when s_fraction <> 0 then null
                    else b.start_node_id 
                end as s_node
                ,case when e_fraction <> 1 then null
                    else b.end_node_id 
                end as e_node
                ,case when c.target_link_id is not null then 0
                    else 1
                end as first_flag
            from rdb_vics_org2rdb_jpn a
            left join rdb_link b
            on a.target_link_id = b.link_id
            left join (
                select distinct a.vics_meshcode,a.vicsclass_c,a.vics_link_id,c.target_link_id
                from inf_vics a
                left join lq_vics b
                on a.objectid = b.inf_id 
                left join temp_link_org_rdb c
                on b.link_id = c.org_link_id
                where b.sequence =  1 
            ) c
            on a.meshcode = c.vics_meshcode and a.vicsclass = c.vicsclass_c and a.vicsid = c.vics_link_id
            and a.target_link_id = c.target_link_id;
            analyze temp_vics_org2rdb_with_node;
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()        
        
        self.CreateIndex2('temp_vics_org2rdb_with_node_classes_idx')
        self.CreateIndex2('temp_vics_org2rdb_with_node_linkdir_idx')
        self.CreateIndex2('temp_vics_org2rdb_with_node_s_node_idx')
        self.CreateIndex2('temp_vics_org2rdb_with_node_e_node_idx')
        
        # create a function for rank the links.
        self.CreateFunction2('rdb_make_vics_link_seq')
        self.CreateFunction2('rdb_make_vics_link_seq_in_one_direction')
        
        self.CreateTable2('temp_vics_link_seq')        
        self.CreateTable2('temp_vics_link_walked')
        self.CreateIndex2('temp_vics_link_walked_classes_idx') 
        self.pg.callproc('rdb_make_vics_link_seq')

        # get the result table.
        self.CreateTable2('rdb_vics_org2rdb_jpn_bak')  
        self.CreateIndex2('rdb_vics_org2rdb_jpn_bak_attr_idx')      
        self.CreateTable2('rdb_vics_org2rdb_jpn')    
        self.CreateIndex2('temp_vics_link_seq_attr_idx')
                    
        sqlcmd = """                
            insert into rdb_vics_org2rdb_jpn(meshcode, vicsclass, vicsid, linkdir, target_link_id, group_id, seq, s_fraction, 
                   e_fraction, s_point, e_point)
            select b.meshcode,b.vicsclass,b.vicsid,b.linkdir,b.target_link_id
                ,a.group_id,a.seq,b.s_fraction,b.e_fraction,b.s_point,b.e_point
            from temp_vics_link_seq a
            left join rdb_vics_org2rdb_jpn_bak b
            on a.class0 = b.meshcode and a.class1 = b.vicsclass and a.class2 = b.vicsid
            and a.link_id = b.target_link_id and a.linkdir = b.linkdir - 1
            order by b.meshcode,b.vicsclass,b.vicsid,a.group_id,a.seq;
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()  
      
        # check
        sqlcmd = """
            select count(*) from rdb_vics_org2rdb_jpn a
            left join (
                select meshcode,vicsclass,vicsid,count(*) 
                from rdb_vics_org2rdb_jpn group by meshcode,vicsclass,vicsid
                having count(*) = 1 
            ) b
            on a.meshcode = b.meshcode and a.vicsclass = b.vicsclass and a.vicsid = b.vicsid
            where b.meshcode is null and 
            (seq = 1 and ((linkdir = 1 and e_fraction <> 1) or (linkdir = 2 and s_fraction <> 0)))
            union
            select count(*) from rdb_vics_org2rdb_jpn where (seq <> 1 and ((linkdir = 1 and s_fraction <> 0) or (linkdir = 2 and e_fraction <> 1)));                                                         
           """
        self.pg.execute2(sqlcmd)   
        row = self.pg.fetchall2()
        if row:
            if len(row) > 1:
                rdb_log.log(self.ItemName, 'rank rdb_vics_org2rdb_jpn Failed.', 'error') 

    def _createRDBIPCVicsSeq(self):
        
        # prepare a common table.
#        self.CreateTable2('temp_vics_org2rdb_with_node')        
#        sqlcmd = """
#            insert into temp_vics_org2rdb_with_node(class0, class1, class2, class3, class4, link_id, linkdir, s_fraction, 
#               e_fraction, s_point, e_point, s_node, e_node, first_flag)
#            select a.meshcode as class0,a.vicsclass as class1,a.vicsid as class2,0::smallint as class3
#                ,null::character varying as class4,a.target_link_id as link_id
#                ,a.linkdir - 1 as linkdir
#                ,a.s_fraction,a.e_fraction
#                ,a.s_point,a.e_point
#                ,case when s_fraction <> 0 then null
#                    else b.start_node_id 
#                end as s_node
#                ,case when e_fraction <> 1 then null
#                    else b.end_node_id 
#                end as e_node
#                ,case when c.target_link_id is not null then 0
#                    else 1
#                end as first_flag
#            from rdb_ipc_vics_org2rdb_jpn a
#            left join rdb_link b
#            on a.target_link_id = b.link_id
#            left join (
#                select distinct a.vics_meshcode,a.vicsclass_c,a.vics_link_id,c.target_link_id
#                from inf_ipcvics a
#                left join lq_ipcvics b
#                on a.objectid = b.inf_id 
#                left join temp_link_org_rdb c
#                on b.link_id = c.org_link_id
#                where b.sequence =  1 
#            ) c
#            on a.meshcode = c.vics_meshcode and a.vicsclass = c.vicsclass_c and a.vicsid = c.vics_link_id
#            and a.target_link_id = c.target_link_id;
#            analyze temp_vics_org2rdb_with_node;
#        """
#        self.pg.execute2(sqlcmd)
#        self.pg.commit2()        
#        
#        self.CreateIndex2('temp_vics_org2rdb_with_node_classes_idx')
#        self.CreateIndex2('temp_vics_org2rdb_with_node_linkdir_idx')
#        self.CreateIndex2('temp_vics_org2rdb_with_node_s_node_idx')
#        self.CreateIndex2('temp_vics_org2rdb_with_node_e_node_idx')
#        
#        # create a function for rank the links.
#        self.CreateFunction2('rdb_make_vics_link_seq')
#        self.CreateFunction2('rdb_make_vics_link_seq_in_one_direction')
#        
#        self.CreateTable2('temp_vics_link_seq')        
#        self.CreateTable2('temp_vics_link_walked')
#        self.CreateIndex2('temp_vics_link_walked_classes_idx') 
#        self.pg.callproc('rdb_make_vics_link_seq')
        
        # get the result table.
        self.CreateTable2('rdb_ipc_vics_org2rdb_jpn_bak')  
        self.CreateIndex2('rdb_ipc_vics_org2rdb_jpn_bak_attr_idx')      
        self.CreateTable2('rdb_ipc_vics_org2rdb_jpn')    
        self.CreateIndex2('temp_vics_link_seq_attr_idx')
                    
        sqlcmd = """                
            insert into rdb_ipc_vics_org2rdb_jpn(meshcode, vicsclass, vicsid, linkdir, target_link_id, group_id, seq, s_fraction, 
                   e_fraction, s_point, e_point)
            select b.meshcode,b.vicsclass,b.vicsid,b.linkdir,b.target_link_id
                ,a.group_id,a.seq,b.s_fraction,b.e_fraction,b.s_point,b.e_point
            from temp_vics_link_seq a
            left join rdb_ipc_vics_org2rdb_jpn_bak b
            on a.class0 = b.meshcode and a.class1 = b.vicsclass and a.class2 = b.vicsid
            and a.link_id = b.target_link_id and a.linkdir = b.linkdir - 1
            order by b.meshcode,b.vicsclass,b.vicsid,a.group_id,a.seq;
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()  
      
        # check
        sqlcmd = """
            select count(*) from rdb_ipc_vics_org2rdb_jpn a
            left join (
                select meshcode,vicsclass,vicsid,count(*) 
                from rdb_ipc_vics_org2rdb_jpn group by meshcode,vicsclass,vicsid
                having count(*) = 1 
            ) b
            on a.meshcode = b.meshcode and a.vicsclass = b.vicsclass and a.vicsid = b.vicsid
            where b.meshcode is null and 
            (seq = 1 and ((linkdir = 1 and e_fraction <> 1) or (linkdir = 2 and s_fraction <> 0)))
            union
            select count(*) from rdb_ipc_vics_org2rdb_jpn where (seq <> 1 and ((linkdir = 1 and s_fraction <> 0) or (linkdir = 2 and e_fraction <> 1)));                                                         
           """
        self.pg.execute2(sqlcmd)   
        row = self.pg.fetchall2()
        if row:
            if len(row) > 1:
                rdb_log.log(self.ItemName, 'rank rdb_ipc_vics_org2rdb_jpn Failed.', 'error') 
                                     
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
            ) a;                                                           
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
        
        
#        rdb_log.log(self.ItemName, 'check rdb_ipc_vics_org2rdb_jpn  --- start.', 'info')      
#        sqlcmd = """
#
#            select count(*) from rdb_ipc_vics_org2rdb_jpn 
#            where (s_fraction = 0 and e_fraction = 0) or (s_fraction = 1 and e_fraction = 1)
#            
#            union
#            
#            select count(*) from (
#                select meshcode,vicsclass,vicsid,target_link_id,s_fraction,e_fraction 
#                from rdb_ipc_vics_org2rdb_jpn 
#                group by meshcode,vicsclass,vicsid,target_link_id,s_fraction,e_fraction having count(*) >  1
#            ) a
#            
#            union
#            
#            select count(*) from rdb_ipc_vics_org2rdb_jpn a
#            left join (
#                select meshcode,vicsclass,vicsid,target_link_id,unnest(s_fraction_array) as s_fraction,unnest(e_fraction_array) as e_fraction,count from (
#                    select meshcode,vicsclass,vicsid,target_link_id,array_agg(s_fraction) as s_fraction_array,array_agg(e_fraction) as e_fraction_array,count(*) as count
#                     from (
#                        select meshcode,vicsclass,vicsid,target_link_id, 
#                            case when s_fraction > e_fraction then e_fraction else s_fraction end as s_fraction,
#                            case when s_fraction > e_fraction then s_fraction else e_fraction end as e_fraction
#                        from temp_ipc_vics_org2rdb_prepare_jpn 
#                    ) a group by meshcode,vicsclass,vicsid,target_link_id
#                ) c
#            ) b
#            on a.meshcode = b.meshcode and a.vicsid = b.vicsid and a.vicsclass = b.vicsclass and a.target_link_id = b.target_link_id
#            where (a.s_fraction <> b.s_fraction or a.e_fraction <> b.e_fraction) and b.count = 1 
#            
#            union
#            
#            select count(*) from rdb_ipc_vics_org2rdb_jpn a
#            left join (
#                select meshcode,vicsclass,vicsid,target_link_id,unnest(s_fraction_array) as s_fraction,unnest(e_fraction_array) as e_fraction,count 
#                from (
#                    select meshcode,vicsclass,vicsid,target_link_id,
#                        array_agg(s_fraction) as s_fraction_array,array_agg(e_fraction) as e_fraction_array,
#                        count(*) as count 
#                    from temp_ipc_vics_org2rdb_prepare_jpn
#                    group by meshcode,vicsclass,vicsid,target_link_id
#                ) c
#            ) b
#            on a.meshcode = b.meshcode and a.vicsid = b.vicsid and a.vicsclass = b.vicsclass and a.target_link_id = b.target_link_id
#            where (a.s_fraction = b.s_fraction and a.e_fraction = b.e_fraction) and b.count <> 1 
#            
#            union
#            
#            select count(*) from rdb_ipc_vics_org2rdb_jpn a
#            left join temp_ipc_vics_org2rdb_prepare_jpn b
#            on a.meshcode = b.meshcode and a.vicsid = b.vicsid and a.vicsclass = b.vicsclass and a.target_link_id = b.target_link_id
#            where a.linkdir <> b.linkdir  
#            
#            union
#            
#            select count(*) from rdb_ipc_vics_org2rdb_jpn a
#            left join temp_link_org_rdb b
#            on a.target_link_id = b.target_link_id            
#            where   (a.e_fraction = 1 and a.e_point <> st_npoints(b.target_geom) - 1)
#                or 
#                (a.e_fraction = 0 and a.e_point <> 0) 
#            
#            union
#            
#            select count(*) from (
#                select meshcode,vicsclass,vicsid,target_link_id 
#                from (select * from rdb_ipc_vics_org2rdb_jpn where s_fraction = 0 and e_fraction = 1) a
#                group by meshcode,vicsclass,vicsid,target_link_id having count(*) <> 1
#            ) a  ;                                                              
#           """
#           
#        if self.pg.execute2(sqlcmd) == -1:
#            return -1
#        else:
#            row = self.pg.fetchall2()
#            if row:
#                if len(row) > 1:
#                    rdb_log.log(self.ItemName, 'check rdb_ipc_vics_org2rdb_jpn Failed.', 'error') 
#                else:
#                    rdb_log.log(self.ItemName, 'check rdb_ipc_vics_org2rdb_jpn  --- end.', 'info') 

            
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
        self.CreateIndex2('temp_tmc_chn_rdslinkinfo_meshid_link_idx')        

        self.CreateIndex2('temp_link_mapping_meshid_road_id_numeric_idx') 
        self.CreateIndex2('temp_link_org_rdb_mesh_id_link_id_idx')  
                
        self.CreateTable2('temp_vics_org2rdb_prepare_axf')
        self.CreateTable2('temp_vics_org2rdb_seq_axf')
        self.CreateTable2('rdb_vics_org2rdb_axf')
        
        self.CreateFunction2('rdb_make_vics_merge_links_axf')
        
        self.pg.callproc('rdb_make_vics_merge_links_axf')
        
        self._createRDBVicsSeq()
        self._checkResult()

    def _createRDBVicsSeq(self):
        
        # prepare a common table.       
        
        self.CreateTable2('temp_vics_org2rdb_with_node')        
        sqlcmd = """
            insert into temp_vics_org2rdb_with_node(class0, class1, class2, class3, class4, link_id, linkdir, s_fraction, 
               e_fraction, s_point, e_point, s_node, e_node, first_flag)
            select a.loc_code_mesh as class0,a.loc_code as class1,a.dir as class2,"type" as class3
                ,service_id as class4,a.target_link_id as link_id,a.link_dir as linkdir,a.s_fraction,a.e_fraction
                ,a.s_point,a.e_point
                ,case when s_fraction <> 0 then null
                    else b.start_node_id 
                end as s_node
                ,case when e_fraction <> 1 then null
                    else b.end_node_id 
                end as e_node,
                1 as first_flag
            from rdb_vics_org2rdb_axf a
            left join rdb_link b
            on a.target_link_id = b.link_id;
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()        
        
        self.CreateIndex2('temp_vics_org2rdb_with_node_classes_idx')
        self.CreateIndex2('temp_vics_org2rdb_with_node_linkdir_idx')
        self.CreateIndex2('temp_vics_org2rdb_with_node_s_node_idx')
        self.CreateIndex2('temp_vics_org2rdb_with_node_e_node_idx')
        
        # create a function for rank the links.
        self.CreateFunction2('rdb_make_vics_link_seq')
        self.CreateFunction2('rdb_make_vics_link_seq_in_one_direction')
        
        self.CreateTable2('temp_vics_link_seq')        
        self.CreateTable2('temp_vics_link_walked')
        self.CreateIndex2('temp_vics_link_walked_classes_idx') 
        self.pg.callproc('rdb_make_vics_link_seq')

        # get the result table.
        self.CreateTable2('rdb_vics_org2rdb_axf_bak')  
        self.CreateIndex2('rdb_vics_org2rdb_axf_bak_attr_idx')      
        self.CreateTable2('rdb_vics_org2rdb_axf')    
        self.CreateIndex2('temp_vics_link_seq_attr_idx')
                    
        sqlcmd = """                
            insert into rdb_vics_org2rdb_axf(loc_code_mesh, loc_code, dir, "type", service_id, target_link_id, link_dir
                , group_id, seq, s_fraction, e_fraction, s_point, e_point)
            select b.loc_code_mesh,b.loc_code,b.dir,b.type,b.service_id,b.target_link_id,b.link_dir
                ,a.group_id,a.seq,b.s_fraction,b.e_fraction,b.s_point,b.e_point
            from temp_vics_link_seq a
            left join rdb_vics_org2rdb_axf_bak b
            on a.class0 = b.loc_code_mesh and a.class1 = b.loc_code and a.class2 = b.dir
            and a.class3 = b.type and a.class4 = b.service_id 
            and a.link_id = b.target_link_id and a.linkdir = b.link_dir
            order by loc_code_mesh,loc_code,dir,"type" desc,group_id,seq;
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()  
      
        # check
        sqlcmd = """
            select count(*) from rdb_vics_org2rdb_axf a
            left join (
                select loc_code_mesh,loc_code,dir,type,service_id,count(*) 
                from rdb_vics_org2rdb_axf group by loc_code_mesh,loc_code,dir,type,service_id
                having count(*) = 1 
            ) b
            on a.loc_code_mesh = b.loc_code_mesh and a.loc_code = b.loc_code and a.dir = b.dir and a.type = b.type and a.service_id = b.service_id
            where b.loc_code_mesh is null and 
            (seq = 1 and ((link_dir = 0 and e_fraction <> 1) or (link_dir = 1 and s_fraction <> 0)))
            union
            select count(*) from rdb_vics_org2rdb_axf where (seq <> 1 and ((link_dir = 0 and s_fraction <> 0) or (link_dir = 1 and e_fraction <> 1)));
           """
        self.pg.execute2(sqlcmd)   
        row = self.pg.fetchall2()
        if row:
            if len(row) > 1:
                rdb_log.log(self.ItemName, 'rank rdb_vics_org2rdb_axf Failed.', 'error') 
  
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
    @staticmethod
    def instance():
        proj_mapping = {
            ('default'):            rdb_traffic_area_null(),
            ('ta','aus'):           rdb_traffic_area_notnull(),
            ('ta','saf8'):          rdb_traffic_area_notnull(),
            ('rdf'):                rdb_traffic_area_null(),
            ('rdf','sgp'):          rdb_traffic_area_notnull(),
            ('rdf','uc'):           rdb_traffic_area_notnull(),
            ('rdf','me8'):          rdb_traffic_area_null(),
            ('rdf','mea'):          rdb_traffic_area_null(),
            ('rdf','bra'):          rdb_traffic_area_null(), 
            ('rdf','ase'):          rdb_traffic_area_null(),                       
            ('nostra'):             rdb_traffic_area_null(),
            ('mmi'):                rdb_traffic_area_null(),
            ('msm'):                rdb_traffic_area_null(),
            ('ni'):                 rdb_traffic_area_null(),
            ('zenrin'):             rdb_traffic_area_notnull(),            
        }
        return rdb_common.getItem(proj_mapping)

    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self, 'AreaNo_Dummy')
        
class rdb_traffic_area_notnull(ItemBase):
    '''Traffic AreaNo
    '''

    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self, 'Traffic AreaNo')
        
    def Do(self):    
        
        rdb_log.log(self.ItemName, 'creating area no table ----- start', 'info')    
        
        try:
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
                """
            self.pg.execute2(sqlcmd)
            self.pg.commit2()          

            self.CreateIndex2('rdb_trf_tile_area_no_tile_id_idx') 
            
        except:
            
            rdb_log.log(self.ItemName, 'Error in creating area no table !!!', 'error')
            
        rdb_log.log(self.ItemName, 'creating area no table ----- end', 'info')           

class rdb_traffic_area_null(ItemBase):
    '''Traffic AreaNo
    '''

    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self, 'Traffic AreaNo')
        
    def Do(self):    
        
        rdb_log.log(self.ItemName, 'creating null area no table ----- start', 'info')    

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

        rdb_log.log(self.ItemName, 'creating null area no table ----- end', 'info')
                    
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
        self.CreateFunction2('mid_get_fraction')
        
        self._createTrfLinkSeq()      
        
        # Create org2rdb table for traffic links.               
        self._createTRFTbl()   

        # Create org2rdb table for region traffic links.
        self._createRegionTRF()

        # Create table for location code and event.
        self._createForTMCOnly()          

    def _createRegionTRF(self):
        
        proc_region = rdb_traffic_region(self, rdb_log)
        
        layer_list = self.pg.GetRegionLayers()

        for layer_no in layer_list:
            proc_region._createRegionXTRFTbl(layer_no)   

    def _createTrfLinkSeq(self):
        pass
                       
    def _createTRFTbl(self):    
        rdb_log.log(self.ItemName, 'creating trf ----- start', 'info')                 
        self.CreateFunction2('rdb_cnv_country_code_common')       
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

        if self.pg.IsExistTable('rdb_trf_area'):
            sqlcmd = """
                --- delete records not in rdb_trf_area.                 
                delete from rdb_trf_org2rdb 
                where gid in (
                    select a.gid from rdb_trf_org2rdb a
                    left join rdb_trf_area b
                    on (a.area_code = rdb_cnv_country_code_common(b.ecc,b.cc) and a.extra_flag = b.ltn)
                    where b.ltn is null
                );                          
                """
            self.pg.execute2(sqlcmd)  
                                            
    def _createForTMCOnly(self):
        self.CreateFunction2('rdb_get_json_string_for_traffic')
        self.CreateFunction2('rdb_get_json_string_for_trf_names')

        self._createLanguages()
        
        sqlcmd = """
            select count(*) from pg_tables 
            where tablename like 'gewi_%';                          
            """
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchone2()
        
        if  row[0] > 0: 
            self._checkLocationTblVer()
            self._createLocationTbl()         
        else:
            rdb_log.log(self.ItemName, '!!! No original location table, Please check original data !!!', 'exception')
            return 0                
        
        sqlcmd = """
            select count(*) from pg_tables 
            where tablename = 'temp_trf_event' or tablename = 'temp_trf_supplementary';                          
            """
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchone2()
        
        if  row[0] == 2:
            self._createEventSupplTbl()          
        else:
            rdb_log.log(self.ItemName, '!!! No original event & supplementary table, Please check original data !!!', 'exception')                                        

    def _createLanguages(self):
        
        self.CreateTable2('temp_trf_languages')
   
    def _checkLocationTblVer(self):

        if rdb_common.getProjName().lower() == 'ta':
            
            sqlcmd = """
                select count(*) from (
                    select distinct substr(tmclstver,1,1) as ver from org_tl 
                ) a
                left join (    
                    select distinct version as ver from gewi_locationdatasets
                ) b
                on a.ver = b.ver                         
                """
            self.pg.execute2(sqlcmd)
            row = self.pg.fetchone2()
            
            if row[0] == 0: 
                rdb_log.log(self.ItemName, '!!! Version of locationtable is wrong, Please check original data !!!', 'exception')
                return -1
            else:
                return 1
                  
    def _createLocationTbl(self):

        rdb_log.log(self.ItemName, 'creating location table ----- start', 'info')

        self.CreateTable2('temp_trf_names')
        self.CreateTable2('temp_locationtable')   
                                 
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
        select rdb_cnv_country_code_common(a.ecc, a.cc) as country_code
            ,a.table_no, a.location_code
            ,type, subtype
            ,case when location_type = 'P' then 1
                when location_type = 'L' then 2
                when location_type = 'A' then 3
                else 0
             end as location_type    
            ,case when first_name is not null then rdb_get_json_string_for_trf_names(first_lang, first_name) 
                    else rdb_get_json_string_for_trf_names(first_lang, null::varchar[]) 
             end as first_name
            ,case when second_name is not null then rdb_get_json_string_for_trf_names(second_lang, second_name) 
                    else rdb_get_json_string_for_trf_names(second_lang, null::varchar[]) 
             end as second_name
            ,case when road_number is not null then rdb_get_json_string_for_trf_names(roadnumber_lang, array[road_number]) 
                    else rdb_get_json_string_for_trf_names(roadnumber_lang, null::varchar[]) 
             end as road_number
            ,case when road_name is not null then rdb_get_json_string_for_trf_names(road_lang, road_name) 
                    else rdb_get_json_string_for_trf_names(road_lang, null::varchar[]) 
             end as road_name    
            ,area_ref, line_ref, neg_offset, pos_offset
            ,urban, veran, tern, intersection_code
            ,in_pos, out_pos, in_neg, out_neg
            ,cur_pos, cur_neg, exit_no, div_pos, div_neg
        from temp_locationtable a
        left join rdb_trf_area b
        on a.ecc = b.ecc and a.cc = b.cc and a.table_no = b.ltn
        where b.ltn is not null;              
        """
        self.pg.execute2(sqlcmd) 
        self.pg.commit2()
        
        rdb_log.log(self.ItemName, 'creating location table ----- end', 'info')
            
    def _createEventSupplTbl(self):

        rdb_log.log(self.ItemName, 'creating event&supplementary table ----- start', 'info')        
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
        select info_flag,direction,code,quantifier,reference_flag
            ,referent,urgency,update_class
            ,rdb_get_json_string_for_trf_names(lang_code_array,desc_array)
        from (
            select info_flag,direction,code,quantifier,reference_flag
                ,referent,urgency,update_class
                ,array_agg(event_string) as desc_array
                ,array_agg(lang_code) as lang_code_array
            from (
                select  type as info_flag,
                    dir as direction,
                    eventcode as code,
                    quantifier,
                    ref as reference_flag,
                    null::smallint as referent,
                    urgency,
                    cast(updateclass as smallint) as update_class,
                    event_string,n.lang_code
                from temp_trf_event m
                left join (
                    select distinct unnest(language_array) as lang_code 
                    from temp_trf_languages 
                ) n
                on lower(m.lang_code) = lower(n.lang_code)
                where n.lang_code is not null
                order by code,lang_code
                ) a group by info_flag,direction,code,quantifier,reference_flag,referent,urgency,update_class
        ) b;                        
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()      
       
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
        select code,referent
            ,rdb_get_json_string_for_trf_names(lang_cdoe_array,desc_array)
        from (
            select code,referent
                ,array_agg(suplstring) as desc_array
                ,array_agg(lang_code) as lang_cdoe_array
            from (
                select  cast(suplcode as smallint) as code
                    ,null::smallint as referent
                    ,suplstring,n.lang_code
                from temp_trf_supplementary m
                left join (
                    select distinct unnest(language_array) as lang_code 
                    from temp_trf_languages 
                ) n
                on lower(m.lang_code) = lower(n.lang_code)
                where n.lang_code is not null                
                order by code,lang_code
                ) a group by code,referent
        ) b;                         
        """
        self.pg.execute2(sqlcmd)   
        self.pg.commit2()     
        rdb_log.log(self.ItemName, 'creating event&supplementary table ----- end', 'info')

                              
class rdb_traffic_rdf(rdb_traffic):
                                         
    def _createTrfLinkSeq(self):
        
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
            CREATE INDEX temp_rdf_link_tmc_node_ref_node_id_idx
              ON temp_rdf_link_tmc_node
              USING btree
              (ref_node_id);
            CREATE INDEX temp_rdf_link_tmc_node_nonref_node_id_idx
              ON temp_rdf_link_tmc_node
              USING btree
              (nonref_node_id);
            CREATE INDEX temp_rdf_link_tmc_node_location_code_idx
              ON temp_rdf_link_tmc_node
              USING btree
              (location_code);
            CREATE INDEX temp_rdf_link_tmc_node_location_table_nr_idx
              ON temp_rdf_link_tmc_node
              USING btree
              (location_table_nr);                    
            analyze temp_rdf_link_tmc_node;
        """
        self.pg.execute2(sqlcmd)   
        self.pg.commit2()
        
        self.CreateFunction2('rdb_make_trf_link_seq')
        self.CreateFunction2('rdb_make_trf_link_seq_in_one_direction')        
        self.pg.commit2()
         
        # Give sequence to traffic links by searching road.        
        sqlcmd = """
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
                                                                                
    def _createCountryCodeAndLang(self):
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

class rdb_traffic_rdf_uc(rdb_traffic_rdf):
    
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
                if cc = '1' then ecc = 'A0';
                elseif cc = '8' then ecc = 'A0';
                elseif cc = 'F' then ecc = 'A5';
                elseif cc = 'C' then ecc = 'A1';
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

class rdb_traffic_rdf_mea(rdb_traffic_rdf):
    
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
                if cc = '9' then ecc = 'F0';
                elseif cc = '1' then ecc = 'F2';
                elseif cc = '2' then ecc = 'F2';
                elseif cc = '6' then ecc = 'F1';
                elseif cc = '5' then ecc = 'E1';
                elseif cc = 'D' then ecc = 'F2';
                elseif cc = 'A' then ecc = 'E3';
                elseif cc = 'E' then ecc = 'F0';
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

    def _createLanguages(self):
        # Language: English, Arabic         
        sqlcmd = """        
            drop table if exists temp_trf_languages;
            create table temp_trf_languages 
            as
            (
                select cid
                    ,array_agg(lid) as lid_array_org
                    ,array_agg(upper(language)) as language_array
                from (
                    select a.cid,a.lid
                        ,case when b.l_full_name is not null then b.language_code_client
                            else a.language
                        end as language
                    from (
                        select m.cid,m.lid
                            ,case when n.language is not null then n.language
                                else m.language
                            end as language
                        from gewi_languages m
                        left join (
                            select distinct lid,language
                            from gewi_languages 
                            where lower(language) != 'default'
                        ) n
                        on m.lid = n.lid
                    ) a
                    left join rdb_language b
                    on lower(a.language) = lower(b.l_full_name)
                ) a group by cid
            );                    
        """
        self.pg.execute2(sqlcmd) 
        self.pg.commit2()

class rdb_traffic_rdf_bra(rdb_traffic_rdf):
    
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
                if cc = 'B' then ecc = 'A2';
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

    def _createLanguages(self):
        # Language: Portuguese         
        sqlcmd = """        
            drop table if exists temp_trf_languages;
            create table temp_trf_languages 
            as
            (
                select cid
                    ,array_agg(lid) as lid_array_org
                    ,array_agg(upper(language)) as language_array
                from (
                    select a.cid,a.lid
                        ,case when b.l_full_name is not null then b.language_code_client
                            else a.language
                        end as language
                    from (
                        select m.cid,m.lid
                            ,case when n.language is not null then n.language
                                else m.language
                            end as language
                        from gewi_languages m
                        left join (
                            select distinct lid
                                ,case when lower(language) = 'default' then 'Brazil portuguese'
                                    else language
                                end as language
                            from gewi_languages 
                        ) n
                        on m.lid = n.lid
                    ) a
                    left join rdb_language b
                    on lower(a.language) = lower(b.l_full_name)
                ) a group by cid
            );                    
        """
        self.pg.execute2(sqlcmd) 
        self.pg.commit2()

class rdb_traffic_rdf_ase(rdb_traffic):
                          
    def _createTrfLinkSeq(self):
        
        rdb_log.log(self.ItemName, 'creating sequence for RTIC link -----start ', 'info')       

        # Create indexs for original tables.  
        self.CreateIndex2('rdf_link_rtic_nvid_1_idx')
        self.CreateIndex2('rdf_link_rtic_mapid_idx')
        
        # Create meshbox.
        self.CreateTable2('temp_rtic_mesh_box')
        
        # Split link by meshbox.
        sqlcmd = """
            drop table if exists temp_rtic_link_split;
            create table temp_rtic_link_split as
            select nvid,mapid,link_geom,mesh_geom
                ,geom_index
                ,st_geometryn(the_geom, geom_index) as the_geom
            from (
                select nvid,mapid,link_geom,mesh_geom,the_geom
                    ,generate_series(1,geom_count) as geom_index
                from (
                    select nvid,mapid,link_geom,mesh_geom
                        ,st_multi(the_geom) as the_geom
                        ,st_numgeometries(st_multi(the_geom)) as geom_count
                    from (
                        select a.*,b.the_geom as link_geom,c.the_geom as mesh_geom
                            ,case
                                when st_contains(c.the_geom, b.the_geom) then b.the_geom
                                else st_intersection(b.the_geom, c.the_geom)
                            end as the_geom
                        from (
                            select distinct nvid,mapid
                            from rdf_link_rtic
                        ) a
                        left join temp_rdf_nav_link b
                        on a.nvid::bigint = b.link_id
                        left join temp_rtic_mesh_box c
                        on a.mapid = c.mesh_id
                        where b.link_id is not null and st_intersects(b.the_geom,c.the_geom)
                    ) d
                ) e
            ) f;
            
            CREATE INDEX temp_rtic_link_split_mapid_nvid_geom_index_idx
              ON temp_rtic_link_split
              USING btree
              (mapid,nvid,geom_index);            
        """
        self.pg.execute2(sqlcmd)   
        self.pg.commit2()

        # Get percent of splitted links.
        sqlcmd = """
            drop table if exists temp_rtic_link_split_order;
            create table temp_rtic_link_split_order as
            select a.nvid,a.mapid,b.linkid
                ,case when st_equals(link_geom,the_geom) then 0
                    else mid_get_fraction(link_geom, ST_StartPoint(the_geom)) 
                end as s_fraction
                    ,case when st_equals(link_geom,the_geom) then 1
                    else mid_get_fraction(link_geom, ST_EndPoint(the_geom)) 
                end as e_fraction
                    ,a.geom_index,a.the_geom
            from temp_rtic_link_split a
            left join (
                select nvid,mapid
                    ,unnest(linkid_array) as linkid
                    ,generate_series(1,count) as index
                from (
                    select nvid,mapid
                        ,array_agg(linkid) as linkid_array
                        ,array_upper(array_agg(linkid),1) as count
                    from (
                        select nvid,mapid,linkid
                        from rdf_link_rtic
                        order by nvid,mapid,linkid
                    ) m
                    group by nvid,mapid
                ) n 
            ) b
            on a.mapid = b.mapid and a.nvid = b.nvid and a.geom_index = b.index;
            
            CREATE INDEX temp_rtic_link_split_order_nvid_idx
            ON temp_rtic_link_split_order
            USING btree
            (nvid);
            CREATE INDEX temp_rtic_link_split_order_nvid_1_idx
            ON temp_rtic_link_split_order
            USING btree
            (cast(nvid as bigint));
        """
        self.pg.execute2(sqlcmd)   
        self.pg.commit2()

        # Give new node id to rtic link.
        sqlcmd = """
            drop sequence if exists temp_rtic_new_node_id_seq;
            create sequence temp_rtic_new_node_id_seq;
            select setval('temp_rtic_new_node_id_seq', cast(max_id as bigint) + 1000000)
            from
            (
                select max(node_id) as max_id
                from temp_rdf_nav_node
            )as a;
            
            drop table if exists temp_rtic_link_split_addnode;
            create table temp_rtic_link_split_addnode as 
            select nvid,fraction,nextval('temp_rtic_new_node_id_seq') as node_id 
            from (
                select nvid,s_fraction as fraction from temp_rtic_link_split_order
                union
                select nvid,e_fraction as fraction from temp_rtic_link_split_order
            ) a 
            where fraction <> 0 and fraction <> 1;
            
            CREATE INDEX temp_rtic_link_split_addnode_nvid_idx
            ON temp_rtic_link_split_addnode
            USING btree
            (nvid);
            
            drop table if exists temp_rtic_link_split_new;
            create table temp_rtic_link_split_new as
            select distinct nvid,s_node,e_node,linkid,s_fraction,e_fraction
            from (
                select a.nvid
                    ,case when a.s_fraction = 0 then b.ref_node_id
                        when a.s_fraction <> 0 and a.s_fraction = c.fraction then c.node_id
                        else null
                     end as s_node
                    ,case when a.e_fraction = 1 then b.nonref_node_id
                        when a.e_fraction <> 1 and a.e_fraction = d.fraction then d.node_id
                        else null
                     end as e_node
                    ,a.linkid,a.s_fraction,a.e_fraction
                from temp_rtic_link_split_order a
                left join temp_rdf_nav_link b
                on a.nvid::bigint = b.link_id
                left join temp_rtic_link_split_addnode c
                on a.nvid = c.nvid
                left join temp_rtic_link_split_addnode d
                on a.nvid = d.nvid                
            ) d where s_node is not null and e_node is not null;
            
            analyze temp_rtic_link_split_new;
            
            CREATE INDEX temp_rtic_link_split_new_nvid_linkid_idx
            ON temp_rtic_link_split_new
            USING btree
            (nvid,linkid);
        """
        self.pg.execute2(sqlcmd)   
        self.pg.commit2()

        # Get relationship of network road and small rtic links.
        sqlcmd = """
            drop table if exists temp_rtic_link_temp cascade;
            create table temp_rtic_link_temp as 
            select a.meshcode,a.nvid as source_id,a.linkid,b.s_node,b.e_node,a.kind
                ,a.rticid,a.linkdir
                ,b.s_fraction,b.e_fraction
            from (
                SELECT mapid as meshcode, linkid, nvid
                    , kind_u as kind, dir_u as linkdir, middle_u as rticid
                FROM rdf_link_rtic where flag_u <> '0'
                union
                SELECT mapid as meshcode, linkid, nvid
                    , kind_d as kind, dir_d as linkdir, middle_d as rticid
                FROM rdf_link_rtic where flag_d <> '0'
            ) a
            left join temp_rtic_link_split_new b
            on a.nvid = b.nvid and a.linkid = b.linkid
            where b.nvid is not null;
                     
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
                from rdf_link_rtic where kind_d = kind_u and middle_d = middle_u
            ) b
            where a.meshcode = b.meshcode and a.kind = b.kind and a.rticid = b.rticid; 
            
            CREATE INDEX temp_rtic_link_linkid_1_idx
              ON temp_rtic_link
              USING btree
              (cast(linkid as bigint));            
            
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
                    from temp_rtic_link_split_order a
                    left join temp_link_org_rdb_org_temp b
                    on cast(a.nvid as bigint) = b.org_link_id
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
            , case when g.gid is not null and a.linkdir = '2' then (g.sub_count - g.sub_index) 
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

    def _createForTMCOnly(self):
        pass
                    
class rdb_traffic_nostra(rdb_traffic):
                          
    def _createTrfLinkSeq(self):
        
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
                    ,mid_get_fraction(st_linemerge(e.the_geom), ST_StartPoint(d.the_geom)) as s_fraction
                    ,mid_get_fraction(st_linemerge(e.the_geom), ST_EndPoint(d.the_geom)) as e_fraction
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
                                  
    def _createForTMCOnly(self):
        pass

class rdb_traffic_ta(rdb_traffic):
                                         
    def _createTrfLinkSeq(self):
        
        rdb_log.log(self.ItemName, 'creating sequence for tmc link -----start ', 'info')

        # Create a temp table by imitating RDF data.
        sqlcmd = """
            drop table if exists temp_rdf_link_tmc_node cascade;
            create table temp_rdf_link_tmc_node as
            select a.id::bigint as link_id,b.f_jnctid::bigint as ref_node_id,b.t_jnctid::bigint as nonref_node_id
                ,case when substring(a.rdstmc,1,1) = '+' then 'F'::char(1)
                    else 'T'::char(1) end as road_direction
                ,substring(a.rdstmc,2,1)::char(1) as ebu_country_code
                ,substring(a.rdstmc,3,2)::int as location_table_nr
                ,case when substring(a.rdstmc,5,1) in ('+','P') then 0
                    when substring(a.rdstmc,5,1) in ('-','N') then 1
                    else null
                end as dir
                ,substring(a.rdstmc,6,6)::int as location_code    
                ,case when substring(a.rdstmc,5,1) in ('+','-') then 1
                    when substring(a.rdstmc,5,1) in ('P','N') then 0
                    else null
                end as type
                ,d.seqnr 
            from org_rd a
            left join org_nw b
            on a.id = b.id
            left join org_tp c
            on a.tmcpathid = c.id
            left join org_tg d
            on c.id = d.id and a.id = d.trpelid
            order by ebu_country_code,location_table_nr,location_code,dir,seqnr;  
            
            CREATE INDEX temp_rdf_link_tmc_node_dir_idx
              ON temp_rdf_link_tmc_node
              USING btree
              (dir);
            CREATE INDEX temp_rdf_link_tmc_node_road_direction_idx
              ON temp_rdf_link_tmc_node
              USING btree
              (road_direction);              
            CREATE INDEX temp_rdf_link_tmc_node_ebu_country_code_idx
              ON temp_rdf_link_tmc_node
              USING btree
              (ebu_country_code);
            CREATE INDEX temp_rdf_link_tmc_node_location_code_idx
              ON temp_rdf_link_tmc_node
              USING btree
              (location_code);
            CREATE INDEX temp_rdf_link_tmc_node_location_table_nr_idx
              ON temp_rdf_link_tmc_node
              USING btree
              (location_table_nr);                  
            CREATE INDEX temp_rdf_link_tmc_node_link_id_idx
              ON temp_rdf_link_tmc_node
              USING btree
              (link_id);
            CREATE INDEX temp_rdf_link_tmc_node_ref_node_id_idx
              ON temp_rdf_link_tmc_node
              USING btree
              (ref_node_id);
            CREATE INDEX temp_rdf_link_tmc_node_nonref_node_id_idx
              ON temp_rdf_link_tmc_node
              USING btree
              (nonref_node_id);       
            CREATE INDEX temp_rdf_link_tmc_node_complex_feature_idx
              ON temp_rdf_link_tmc_node
              USING btree
              (ebu_country_code, location_table_nr, location_code, dir, link_id,road_direction,ref_node_id,nonref_node_id);                       
            analyze temp_rdf_link_tmc_node;                                               
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        # Ordering traffic links by searching road.  
        self.CreateFunction2('rdb_make_trf_link_seq')
        self.CreateFunction2('rdb_make_trf_link_seq_in_one_direction')        
        self.pg.commit2()
                
        sqlcmd = """
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

        # Create a temp table after ordering.
        sqlcmd = """                                
        drop table if exists temp_trf_link_order cascade;
        create table temp_trf_link_order as
        select ebu_country_code as country_code
            ,location_table_nr,location_code
            ,dir,type,link_id
            ,case when road_direction = 'F' then 0
                else 1 end as link_dir    
            ,seq,group_id
        from temp_rdf_link_tmc_seq;
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
                         
        rdb_log.log(self.ItemName, 'creating sequence for tmc link -----end ', 'info') 
               
    def _createTRFTbl_prepare(self):
         
        # Create simple relationship between traffic and RDB link.       
        sqlcmd = """

        drop table if exists temp_split_full_seq;
        create table temp_split_full_seq as
        select old_link_id,unnest(link_id_array) as link_id
            ,sub_index1_array,sub_index2_array
            ,generate_series(1,sub_count) as sub_index
            ,sub_count
        from (
            select old_link_id
                ,array_agg(link_id) as link_id_array
                ,array_agg(sub_index1) as sub_index1_array
                ,array_agg(sub_index2) as sub_index2_array
                ,array_upper(array_agg(link_id),1) as sub_count
            from (
                select case when a.old_link_id is null then b.old_link_id
                        else a.old_link_id
                    end as old_link_id
                    ,case when b.old_link_id is not null then b.link_id
                        else a.link_id
                    end as link_id
                    ,a.sub_index as sub_index1
                    ,b.sub_index as sub_index2    
                from temp_circle_link_new_seq_link a
                full join temp_split_newlink b
                on a.link_id = b.old_link_id
                order by a.old_link_id,a.sub_index,b.sub_index
            ) m group by old_link_id
        ) n;           
             
        drop table if exists temp_trf_org2rdb_prepare;
        create table temp_trf_org2rdb_prepare as 
        select 0 as type_flag
            ,rdb_cnv_country_code_common(h.ecc,h.ccd) as area_code
            ,a.location_code as infra_id
            ,a.location_table_nr as extra_flag
            ,a.dir
            ,e.target_link_id as rdb_link_id
            ,case when e.flag = 't' and a.link_dir = 1 then 0
                  when e.flag = 't' and a.link_dir = 0 then 1
                  when e.flag = 'f' and a.link_dir = 1 then 1
                  when e.flag = 'f' and a.link_dir = 0 then 0
                  else 0
             end as link_dir   
            ,a.type as pos_type
            ,e.s_fraction,e.e_fraction
            ,a.seq as seq_org
            ,case when g.old_link_id is not null and a.link_dir = 1 then (g.sub_count - g.sub_index) 
                  when g.old_link_id is not null and a.link_dir = 0 then g.sub_index
                  else 1 
            end as sub_seq  --- get sequence of small links when an original link was split(link_split). 
                            --- if link direction is 'T' then the order should be reversed. 
            ,a.group_id as group_id
            ,e.target_geom    
            ,e.flag as flag --- e.flag: wether order of link's shapepoints was reversed when link_merge. 
        from temp_trf_link_order a 
        left join temp_link_org_rdb e
        on a.link_id = e.org_link_id
        left join temp_split_full_seq g
        on e.mid_link_id = g.link_id
        left join gewi_countries h
        on a.country_code = h.ccd; 
        
        ANALYZE  temp_trf_org2rdb_prepare;                          
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2() 

    def _createLanguages(self):
        # Language: English          
        sqlcmd = """        
            drop table if exists temp_trf_languages;
            create table temp_trf_languages 
            as
            (
                select cid
                    ,array_agg(lid) as lid_array_org
                    ,array_agg(upper(language)) as language_array
                from (
                    select a.cid,a.lid
                        ,case when b.l_full_name is not null then b.language_code_client
                            else a.language
                        end as language
                    from (
                        select m.cid,m.lid
                            ,case when n.language is not null then n.language
                                else m.language
                            end as language
                        from gewi_languages m
                        left join (
                            select distinct lid,language
                            from gewi_languages 
                            where lower(language) != 'default'
                        ) n
                        on m.lid = n.lid
                        where m.lid = '1'
                    ) a
                    left join rdb_language b
                    on lower(a.language) = lower(b.l_full_name)
                ) a group by cid
            );                    
        """
        self.pg.execute2(sqlcmd) 
        self.pg.commit2()
        
class rdb_traffic_ni(rdb_traffic):
                          
    def _createTrfLinkSeq(self):
        
        rdb_log.log(self.ItemName, 'creating sequence for RTIC link -----start ', 'info')       

        # Add nodes to original traffic links.  
        self.CreateIndex2('org_rtic_incar_linkid_idx')
        sqlcmd = """
            drop table if exists temp_rtic_link_temp cascade;
            create table temp_rtic_link_temp as 
            select a.meshcode, a.linkid, b.snodeid::bigint as s_node, b.enodeid::bigint as e_node
                ,a.kind, a.linkdir, a.rticid, b.the_geom as the_geom
            from (
                SELECT mapid as meshcode, linkid
                    , kind_u as kind, dir_u as linkdir, middle_u as rticid 
                FROM org_rtic_incar where flag_u <> '0'
                and folder not in ('aomen', 'xianggang')
                union
                SELECT mapid as meshcode, linkid
                    , kind_d as kind, dir_d as linkdir, middle_d as rticid
                FROM org_rtic_incar where flag_d <> '0'
                and folder not in ('aomen', 'xianggang')
            ) a
            left join org_r b
            on a.linkid = b.id;
            
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
            CREATE INDEX temp_rtic_link_temp_s_node_idx
              ON temp_rtic_link_temp
              USING btree
              (s_node);
            CREATE INDEX temp_rtic_link_temp_e_node_idx
              ON temp_rtic_link_temp
              USING btree
              (e_node);
        """
        self.pg.execute2(sqlcmd)   
        self.pg.commit2()
        
        self.CreateFunction2('rdb_make_trf_rtic_link_seq')
        self.CreateFunction2('rdb_make_trf_rtic_link_seq_in_one_direction')        
        self.pg.commit2()
         
        # Give sequence to traffic links by searching road.        
        sqlcmd = """
            drop table if exists temp_rtic_link;
            CREATE TABLE temp_rtic_link
            (
              meshcode character varying NOT NULL,
              kind character varying NOT NULL,
              rticid character varying NOT NULL,
              linkid character varying NOT NULL,
              seq    integer,
              linkdir character varying NOT NULL,
              group_id smallint
            );
            
            drop table if exists temp_rtic_link_walked;
            CREATE TABLE temp_rtic_link_walked
             (
               meshcode character varying,
               kind character varying,
               rticid character varying,
               linkid character varying
             );
            
            CREATE INDEX temp_rtic_link_walked_complex_idx
              ON temp_rtic_link_walked
              USING btree
              (meshcode, kind, rticid, linkid);
            
            analyze temp_rtic_link_temp;
            select rdb_make_trf_rtic_link_seq();
            
              
            delete from temp_rtic_link a using (
                select distinct meshcode,kind,rticid from temp_rtic_link 
                where group_id > 0
                union
                select distinct mapid as meshcode,kind_u as kind,middle_u as rticid 
                from org_rtic_incar where kind_d = kind_u and middle_d = middle_u
            ) b
            where a.meshcode = b.meshcode and a.kind = b.kind and a.rticid = b.rticid; 
            
            CREATE INDEX temp_rtic_link_linkid_1_idx
              ON temp_rtic_link
              USING btree
              (cast(linkid as bigint));            
            
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        rdb_log.log(self.ItemName, 'creating sequence for RTIC link -----end ', 'info') 
                                   
    def _createTRFTbl_prepare(self):
        
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
        left join temp_link_org_rdb b
        on a.linkid::bigint = b.org_link_id
        left join temp_split_newlink g
        on b.mid_link_id = g.link_id;
        
        ANALYZE  temp_trf_org2rdb_prepare;                        
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()           
                                  
    def _createForTMCOnly(self):
        pass

class rdb_traffic_zenrin(rdb_traffic):
                                         
    def _createTrfLinkSeq(self):
        
        rdb_log.log(self.ItemName, 'creating sequence for tmc link -----start ', 'info')
        
        sqlcmd = """
            drop index if exists org_rdstmc_meshcode_roadno_idx;
            CREATE INDEX org_rdstmc_meshcode_roadno_idx
              ON org_rdstmc
              USING btree
              (meshcode, roadno);
        
            drop index if exists org_rdstmc_meshcode_snodeno_idx;
            CREATE INDEX org_rdstmc_meshcode_snodeno_idx
              ON org_rdstmc
              USING btree
              (meshcode, snodeno);
           
            drop index if exists org_rdstmc_meshcode_enodeno_idx;
            CREATE INDEX org_rdstmc_meshcode_enodeno_idx
              ON org_rdstmc
              USING btree
              (meshcode, enodeno);                               
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        sqlcmd = """
            drop table if exists temp_rdf_link_tmc_node cascade;
            create table temp_rdf_link_tmc_node as 
            select a1.link_id as link_id
                ,'D'::char(1) as ebu_country_code
                ,9 as location_table_nr
                ,a.loc_dire as tmc_path_direction        
                ,a.loc_code as location_code
                ,(case when a.link_dire = '+' then 'F' else 'T' end)::char(1) as road_direction
                ,case when a.loc_dire in ('+','-') then 1 else 0 end as type         
                ,case when a.loc_dire in ('+','P') then 0 else 1 end as dir
                ,b.node_id as ref_node_id
                ,c.node_id as nonref_node_id
            from org_rdstmc a
            left join temp_link_mapping a1
            on a.meshcode = a1.meshcode and a.roadno = a1.linkno
            left join temp_node_mapping b
            on a.meshcode = b.meshcode and a.snodeno = b.nodeno
            left join temp_node_mapping c
            on a.meshcode = c.meshcode and a.enodeno = c.nodeno;
            
            analyze temp_rdf_link_tmc_node;
            
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
            CREATE INDEX temp_rdf_link_tmc_node_ref_node_id_idx
              ON temp_rdf_link_tmc_node
              USING btree
              (ref_node_id);
            CREATE INDEX temp_rdf_link_tmc_node_nonref_node_id_idx
              ON temp_rdf_link_tmc_node
              USING btree
              (nonref_node_id);
            CREATE INDEX temp_rdf_link_tmc_node_location_code_idx
              ON temp_rdf_link_tmc_node
              USING btree
              (location_code);
            CREATE INDEX temp_rdf_link_tmc_node_location_table_nr_idx
              ON temp_rdf_link_tmc_node
              USING btree
              (location_table_nr);                    
            analyze temp_rdf_link_tmc_node;
        """
        self.pg.execute2(sqlcmd)   
        self.pg.commit2()
        
        self.CreateFunction2('rdb_make_trf_link_seq')
        self.CreateFunction2('rdb_make_trf_link_seq_in_one_direction')        
        self.pg.commit2()
         
        # Give sequence to traffic links by searching road.        
        sqlcmd = """
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
        
        analyze temp_rdf_link_tmc_seq;
        
        drop table if exists temp_rdf_link_tmc_seq_link_id_idx;
        CREATE INDEX temp_rdf_link_tmc_seq_link_id_idx
          ON temp_rdf_link_tmc_seq
          USING btree
          (link_id);
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
                
        rdb_log.log(self.ItemName, 'creating sequence for tmc link -----end ', 'info') 
               
    def _createTRFTbl_prepare(self):
                
        # Create simple relationship between traffic and RDB link.       
        sqlcmd = """
        drop table if exists temp_trf_org2rdb_prepare;
        create table temp_trf_org2rdb_prepare as 
        select 0 as type_flag
            ,rdb_cnv_country_code_common('F1', ebu_country_code) as area_code
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
        from temp_rdf_link_tmc_seq a 
        left join temp_link_org_rdb e
        on a.link_id = e.org_link_id
        left join temp_split_newlink g
        on e.mid_link_id = g.link_id; 
        
        ANALYZE  temp_trf_org2rdb_prepare;                        
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()             

    def _createLanguages(self):
        # Language: Traditional chinese.          
        sqlcmd = """        
            drop table if exists temp_trf_languages;
            create table temp_trf_languages 
            as
            (
                select cid
                    ,array_agg(lid) as lid_array_org
                    ,array_agg(language) as language_array
                from (
                    select 1::int as cid
                    ,1::int as lid
                    ,'CHT'::character varying as language
                ) a group by cid
            );   
        """
        self.pg.execute2(sqlcmd) 
        self.pg.commit2()

    def _createLocationTbl(self):

        rdb_log.log(self.ItemName, 'creating location table ----- start', 'info')
                                 
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
        select rdb_cnv_country_code_common('F1', 'D') as country_code
            ,table_no::integer, location_code::integer
            ,(substring(sub_type, 2, position('.' in sub_type) - 2))::smallint as type
            ,(substring(sub_type, position('.' in sub_type) + 1,char_length(sub_type)))::smallint as subtype
            ,case when substring(sub_type,1,1) = 'P' then 1
                when substring(sub_type,1,1) = 'L' then 2
                when substring(sub_type,1,1) = 'A' then 3
                else 0
             end as location_type    
            ,case when cfirst_name is not null then rdb_get_json_string_for_trf_names(b.language_array, array[cfirst_name]) 
                    else rdb_get_json_string_for_trf_names(b.language_array, null::varchar[]) 
             end as first_name
            ,case when csecond_name is not null then rdb_get_json_string_for_trf_names(b.language_array, array[csecond_name]) 
                    else rdb_get_json_string_for_trf_names(b.language_array, null::varchar[]) 
             end as second_name
            ,rdb_get_json_string_for_trf_names(b.language_array, null::varchar[]) as road_number
            ,case when croad_name is not null then rdb_get_json_string_for_trf_names(b.language_array, array[croad_name]) 
                    else rdb_get_json_string_for_trf_names(b.language_array, null::varchar[]) 
             end as road_name
            ,(ltrim(area_reference,'0'))::integer as area_ref
            ,(ltrim(linear_reference,'0'))::integer as line_ref
            ,(ltrim(negative_offset,'0'))::integer as neg_offset
            ,(ltrim(positive_offset,'0'))::integer as pos_offset
            ,0 as urban, 0 as veran, 0 as tern
            ,(string_to_array(intersection_refs,', '))[1]::integer as intersection_code
            ,0 as in_pos, 0 as out_pos, 0 as in_neg, 0 as out_neg
            ,0 as cur_pos, 0 as cur_neg, null as exit_no, null as div_pos, null as div_neg
        from gewi_locationtable a
        left join temp_trf_languages b
        on true = true;                
        """
        self.pg.execute2(sqlcmd) 
        self.pg.commit2()
        
        rdb_log.log(self.ItemName, 'creating location table ----- end', 'info')
                    
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

        # create table for ID & geometry relationship between region link and layer14 link.
        sqlcmd = """
        drop table if exists temp_region_trf_org2rdb_layer%X_prepare1;
        create table temp_region_trf_org2rdb_layer%X_prepare1 as
        select p.*,q.link_length::float from (
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
        --DROP INDEX IF EXISTS temp_region_trf_org2rdb_layer%X_prepare1_link_dir_idx;
        --CREATE INDEX temp_region_trf_org2rdb_layer%X_prepare1_link_dir_idx
        --  ON temp_region_trf_org2rdb_layer%X_prepare1
        --  USING btree
        --  (link_dir);  
        --DROP INDEX IF EXISTS temp_region_trf_org2rdb_layer%X_prepare1_rdb_geom_idx;
        --CREATE INDEX temp_region_trf_org2rdb_layer%X_prepare1_rdb_geom_idx
        --  ON temp_region_trf_org2rdb_layer%X_prepare1
        --  USING gist
        --  (rdb_geom);  
        analyze temp_region_trf_org2rdb_layer%X_prepare1;                                            
        """
        sqlcmd = sqlcmd.replace('%X',X)
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        sqlcmd = """
        drop table if exists temp_region_trf_org2rdb_layer%X_prepare2;
        create table temp_region_trf_org2rdb_layer%X_prepare2 as
        select region_link_id, (sum(link_length))::float as link_length
        from temp_region_trf_org2rdb_layer%X_prepare1
        group by region_link_id;
        
        CREATE INDEX temp_region_trf_org2rdb_layer%X_prepare2_region_link_id_idx
          ON temp_region_trf_org2rdb_layer%X_prepare2
          USING btree
          (region_link_id);
        
        drop table if exists temp_region_trf_org2rdb_layer%X_prepare3;
        create table temp_region_trf_org2rdb_layer%X_prepare3 as 
        select a.region_link_id,a.link_id_14,a.link_dir,a.seq,
            a.link_length as rdb_len,  --- length of rdb link.
            b.link_length as region_len    --- length of region link.
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
        from rdb_region_trf_org2rdb_layer%X b,rdb_region_link_layer%X_tbl c
        where rdb_region_trf_org2rdb_layer%X.area_code = b.area_code 
        and rdb_region_trf_org2rdb_layer%X.extra_flag = b.extra_flag 
        and rdb_region_trf_org2rdb_layer%X.infra_id = b.infra_id 
        and rdb_region_trf_org2rdb_layer%X.dir = b.dir 
        and rdb_region_trf_org2rdb_layer%X.group_id = b.group_id 
        and rdb_region_trf_org2rdb_layer%X.rdb_link_id = b.rdb_link_id 
        and rdb_region_trf_org2rdb_layer%X.seq = b.seq - 1
        and rdb_region_trf_org2rdb_layer%X.rdb_link_id = c.link_id and not ST_IsRing(c.the_geom);
        
        --- drop unused column.
        alter table rdb_region_trf_org2rdb_layer%X drop column infra_len;
        alter table rdb_region_trf_org2rdb_layer%X drop column geom_string;       
                                          
        analyze rdb_region_trf_org2rdb_layer%X;
        """
        sqlcmd = sqlcmd.replace('%X',X)
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        rdb_log.log('REGION Traffic', 'creating region %s trf ----- end'%X, 'info')             


