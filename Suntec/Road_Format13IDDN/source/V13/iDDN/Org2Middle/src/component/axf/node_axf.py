# -*- coding: cp936 -*-
'''
Created on 2012-3-23

@author: sunyifeng
'''

import base

class comp_node_axf(base.component_base.comp_base):
    '''
    classdocs
    '''


    def __init__(self):
        '''
        Constructor
        '''
        base.component_base.comp_base.__init__(self, 'Node')
        
    def _DoCreateTable(self):
        if self.CreateTable2('node_tbl') == -1:
            return -1

        if self.CreateTable2('temp_node_lid') == -1:
            return -1
        
        if self.CreateTable2('temp_node_u') == -1:
            return -1
        
        return 0
    
    def _DoCreateIndex(self):
        'create index.'
        
        if self.CreateIndex2('node_tbl_node_id_idx') == -1:
            return -1
        
        if self.CreateIndex2('node_tbl_the_geom_idx') == -1:
            return -1

        return 0
    
    def _DoCreateFunction(self):
        
        if self.CreateFunction2('mid_cnv_node_kind') == -1:
            return -1

        return 0
    
    def _Do(self):
        
        sqlcmd = """
        insert into temp_node_u(meshid, node, new_node)
        select a.meshid, a.node, a.new_node 
            from temp_node_mapping as a
            where g_node = new_node;
        """
        
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.CreateIndex2('org_roadsegment_fnode_idx')
        self.CreateIndex2('org_roadsegment_tnode_idx')
        self.CreateIndex2('org_roadnode_node_idx')
        self.CreateIndex2('temp_node_u_meshid_node_idx')
        self.CreateIndex2('temp_node_u_new_node_idx')
        

        # 合成node相连的所有link
        self._MakeNodeLid() 
            
        sqlcmd = """
                insert into node_tbl (
                    node_id, kind, light_flag, toll_flag, mainnodeid, node_lid, name_id, feature_string, feature_key, the_geom )
                select 
                    m.new_node, 
                    mid_cnv_node_kind(nodetype) as kind,
                    signlight as light_flag,
                    tollgate as toll_flag, 
                    0 as mainnodeid, 
                    array_to_string(b.lid, '|') as node_lid, 
                    c.new_name_id as name_id, 
                    array_to_string(ARRAY[a.meshid, a.node_id],',') as feature_string,
                    md5(array_to_string(ARRAY[a.meshid, a.node_id],',')) as feature_key,
                    st_scale(m.the_geom, 1/3600.0, 1/3600.0) as the_geom
                from org_roadnode as a
                inner join
                (select * from temp_node_mapping where g_node = new_node) as m 
                on a.meshid = m.meshid and a.node = m.node
                left outer join temp_node_lid as b
                on m.new_node = b.node_id
                left outer join temp_node_name as c
                on m.new_node = c.new_node;
        """
        
        self.log.info(self.ItemName + ': Now it is inserting node_tbl...')
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
            
        self.log.info(self.ItemName + ': end of inserting node_tbl...')
                  
        return 0
    
    
    def _MakeNodeLid(self):
        "合成node相连的所有link"
        threads = []
        
        threads.append(node_child_thread(1))  # 起点
        threads.append(node_child_thread(2))  # 终点
        
        # 启动线程
        for t in threads:
            t.start()
           
        # 等待子线程结束
        for t in threads:
            t.join()
            
        # 把起点和终点的所有link合并
        sqlcmd = """
            insert into temp_node_lid(node_id, lid)
            (
                select a.new_node,  b.lid || c.lid 
                  from temp_node_u as a
                  LEFT JOIN temp_node_lid_ref as b
                  ON a.new_node = b.node_id
                  LEFT JOIN temp_node_lid_nonref as c
                  ON a.new_node = c.node_id
            );
        """
        
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else :
            self.pg.commit2()
            
#####################################################################################                 
# node的子线程类
#####################################################################################                 

import threading
import common  
class node_child_thread(threading.Thread):
    "用hierarchy单元，多线程处理line name."
    def __init__(self, task_type):
        threading.Thread.__init__(self)
        self.task_type = task_type
        self.pg        = common.pg_client()
        
    def run(self):
        self.pg.connect2()
        
        if self.task_type == 1:
            # 起点
            print "Start _MakeNodeLidRef."
            self._MakeNodeLidRef()
            print "End _MakeNodeLidRef."
        else:
            # 终点
            print "Start _MakeNodeLidRef."
            self._MakeNodeLidNonRef()
            print "End _MakeNodeLidRef."
        self.pg.close2()
        return 0
    
    def _MakeNodeLidRef(self):
        # 起点
        self.pg.CreateTable2_ByName('temp_node_lid_ref')
        
        sqlcmd = """
            insert into temp_node_lid_ref(node_id, lid)
            (
                select s_node, array_agg(link_id) 
                  from link_tbl
                  group by s_node
            );
        """
        
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else :
            self.pg.commit2()
            
    def _MakeNodeLidNonRef(self):
        # 终点
        self.pg.CreateTable2_ByName('temp_node_lid_nonref')
        
        sqlcmd = """
            insert into temp_node_lid_nonref(node_id, lid)
            (
                select e_node, array_agg(link_id)
                  from link_tbl
                  group by e_node
            );
        """
        
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else :
            self.pg.commit2()
        
    