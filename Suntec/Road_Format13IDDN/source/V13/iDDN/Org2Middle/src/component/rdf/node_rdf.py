# -*- coding: cp936 -*-
'''
Created on 2012-3-23

@author: sunyifeng
'''

import base

class comp_node_rdf(base.component_base.comp_base):
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
        
        if self.CreateTable2('temp_node_light') == -1:
            return -1
        
        if self.CreateTable2('temp_node_toll') == -1:
            return -1

        if self.CreateTable2('temp_node_lid') == -1:
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

        return 0
    
    def _Do(self):
        
        if self.CreateIndex2('temp_rdf_nav_link_ref_node_id_idx') == -1:
            return -1
        
        if self.CreateIndex2('temp_rdf_nav_link_nonref_node_id_idx') == -1:
            return -1
        
        if self.CreateIndex2('temp_rdf_nav_node_node_id_idx') == -1:
            return -1
        
        sqlcmd = """
            insert into temp_node_light(node_id)
                    select distinct node_id from rdf_nav_strand as t1
                        inner join rdf_condition as t2 
                        on t1.nav_strand_id = t2.nav_strand_id
                        where t2.condition_type = 16 and t1.node_id is not null
        """
        
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()

        if self.CreateIndex2('temp_node_light_node_id_idx') == -1:
            return -1
                 
        sqlcmd = """
            insert into temp_node_toll(node_id)
                    select distinct node_id from rdf_nav_strand as t1
                        inner join rdf_condition as t2 
                        on t1.nav_strand_id = t2.nav_strand_id
                        where t2.condition_type = 1 and t1.node_id is not null
        """
        
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()    
                    
        if self.CreateIndex2('temp_node_toll_node_id_idx') == -1:
            return -1
                
        # 合成node相连的所有link
        self._MakeNodeLid() 
        
        sqlcmd = """
                insert into node_tbl (
                    node_id, kind, light_flag, toll_flag, mainnodeid, node_lid, 
                    node_name, the_geom 
                )
                select 
                    a.node_id, 
                    null as kind,
                    case when b.node_id is null then 0 else 1 end as light_flag,
                    case when c.node_id is null then 0 else 1 end as toll_flag, 
                    0 as mainnodeid, 
                    array_to_string(d.lid, '|') as node_lid, 
                    null as node_name, 
                    a.the_geom as the_geom
                from temp_rdf_nav_node as a
                left outer join temp_node_light as b on a.node_id = b.node_id
                left outer join temp_node_toll as c on a.node_id = c.node_id
                left outer join temp_node_lid as d on a.node_id = d.node_id;
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
                select a.node_id,  b.lid || c.lid 
                  from temp_rdf_nav_node as a
                  LEFT JOIN temp_node_lid_ref as b
                  ON a.node_id = b.node_id
                  LEFT JOIN temp_node_lid_nonref as c
                  ON a.node_id = c.node_id
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
                select ref_node_id, array_agg(link_id) 
                  from temp_rdf_nav_link
                  group by ref_node_id
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
                select nonref_node_id, array_agg(link_id)
                  from temp_rdf_nav_link
                  group by nonref_node_id
            );
        """
        
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else :
            self.pg.commit2()
        
    