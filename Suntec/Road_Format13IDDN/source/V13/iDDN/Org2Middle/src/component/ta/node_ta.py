# -*- coding: cp936 -*-
'''
Created on 2012-3-23

@author: sunyifeng
'''

import base

class comp_node_ta(base.component_base.comp_base):
    '''
    classdocs
    '''


    def __init__(self):
        '''
        Constructor
        '''
        base.component_base.comp_base.__init__(self, 'Node')
        
    def _DoCreateTable(self):
        self.CreateTable2('node_tbl')
        self.CreateTable2('temp_jc')
        self.CreateTable2('temp_node_lid')
#        self.CreateTable2('temp_node_lid_ref')
#        self.CreateTable2('temp_node_lid_nonref')
        
        return 0
    
    def _DoCreateIndex(self):
        'create index.'
        
        self.CreateIndex2('node_tbl_node_id_idx')
        self.CreateIndex2('node_tbl_the_geom_idx')

        return 0
    
    def _DoCreateFunction(self):
        
        self.CreateFunction2('mid_cnv_node_kind')

        return 0
    
    def _Do(self):
        
        self.CreateIndex2('nw_f_jnctid_idx')
        self.CreateIndex2('nw_t_jnctid_idx')

        sqlcmd = """
            insert into temp_jc(id, feattyp, jncttyp, elev, geom)
                select distinct id, feattyp, jncttyp, elev, the_geom 
                from org_jc where feattyp = 4120;
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
#        self.CreateIndex2('temp_jc_id_idx')
        

        # �ϳ�node����������link
        self._MakeNodeLid() 
            
        sqlcmd = """
                insert into node_tbl (
                    node_id, kind, light_flag, toll_flag, mainnodeid, node_lid, node_name, the_geom )
                select 
                    a.id, 
                    mid_cnv_node_kind(jncttyp) as kind,
                    null as light_flag,
                    null as toll_flag, 
                    0 as mainnodeid, 
                    array_to_string(b.lid, '|') as node_lid, 
                    null as node_name, 
                    geom
                from temp_jc as a
                left outer join temp_node_lid as b
                on a.id = b.node_id;
        """
        
        self.log.info(self.ItemName + ': Now it is inserting node_tbl...')
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
            
        self.log.info(self.ItemName + ': end of inserting node_tbl...')
                  
        return 0
    
    
    def _MakeNodeLid(self):
        "�ϳ�node����������link"
        threads = []
        
        threads.append(node_child_thread(1))  # ���
        threads.append(node_child_thread(2))  # �յ�
        
        # �����߳�
        for t in threads:
            t.start()
           
        # �ȴ����߳̽���
        for t in threads:
            t.join()
            
        # �������յ������link�ϲ�
        sqlcmd = """
            insert into temp_node_lid(node_id, lid)
            (
                select a.id,  b.lid || c.lid 
                  from temp_jc as a
                  LEFT JOIN temp_node_lid_ref as b
                  ON a.id = b.node_id
                  LEFT JOIN temp_node_lid_nonref as c
                  ON a.id = c.node_id
            );
        """
        
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else :
            self.pg.commit2()
            
#####################################################################################                 
# node�����߳���
#####################################################################################                 

import threading
import common  
class node_child_thread(threading.Thread):
    "���̴߳���."
    def __init__(self, task_type):
        threading.Thread.__init__(self)
        self.task_type = task_type
        self.pg        = common.pg_client()
        
    def run(self):
        self.pg.connect2()
        
        if self.task_type == 1:
            # ���
            print "Start _MakeNodeLidRef."
            self._MakeNodeLidRef()
            print "End _MakeNodeLidRef."
        else:
            # �յ�
            print "Start _MakeNodeLidRef."
            self._MakeNodeLidNonRef()
            print "End _MakeNodeLidRef."
        self.pg.close2()
        return 0
    
    def _MakeNodeLidRef(self):
        # ���
        self.pg.CreateTable2_ByName('temp_node_lid_ref')
        
        sqlcmd = """
            insert into temp_node_lid_ref(node_id, lid)
            (
                select f_jnctid, array_agg(id) 
                  from org_nw
                  where frc != -1
                  group by f_jnctid
            );
        """
        
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else :
            self.pg.commit2()
            
    def _MakeNodeLidNonRef(self):
        # �յ�
        self.pg.CreateTable2_ByName('temp_node_lid_nonref')
        
        sqlcmd = """
            insert into temp_node_lid_nonref(node_id, lid)
            (
                select t_jnctid, array_agg(id) 
                  from org_nw
                  where frc != -1
                  group by t_jnctid
            );
        """
        
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else :
            self.pg.commit2()
        
    
