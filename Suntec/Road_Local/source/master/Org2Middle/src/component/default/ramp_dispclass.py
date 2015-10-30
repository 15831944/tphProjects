# -*- coding: cp936 -*-
'''
Created on 2012-8-17

@author: hongchenzai
@alter: zhengchao
'''
import component.component_base
import common.common_func
import common.search_road
import networkx as nx
import time

class CGraph(nx.DiGraph):
    
    def all_simple_paths_in_digraph(self):
        
        b_list= list(nx.weakly_connected_component_subgraphs(self))

        paths=[]

        for b in b_list:
            #print b.nodes()
            
            start_node_list = set(filter(lambda x:b.in_degree(x)==0,b.nodes()))|set(filter(lambda x:b.in_degree(x)==1 and b.out_degree(x)==1 and b.predecessors(x)==b.successors(x),b.nodes()))|set(filter(lambda x:b.in_degree(x)+b.out_degree(x)>2,b.nodes()))
            end_node_list = set(filter(lambda x:b.out_degree(x)==0,b.nodes()))|set(filter(lambda x:b.in_degree(x)==1 and b.out_degree(x)==1 and b.predecessors(x)==b.successors(x),b.nodes()))|set(filter(lambda x:b.in_degree(x)+b.out_degree(x)>2,b.nodes()))
            
            for start_node in start_node_list:
                for end_node in end_node_list:
                    if start_node<>end_node:
                        for path in nx.all_simple_paths(self, start_node, end_node):
                            paths.append(path)
                            #if 11271347 in b.nodes():
                            #    print path 
                    
        return paths
                        
    
    def get_linkid_of_path(self, path, link_id='link_id'):
        'nodeid转成linkid'
        linkid_list = list()
        for u, v in zip(path[0:-1], path[1:]):
            linkid = self.get_edge_data(u, v).get(link_id)
            linkid_list.append(str(linkid))
        return linkid_list
    
    def all_simple_paths_in_digraph_link(self):
        
        return map(lambda x:self.get_linkid_of_path(x,'link_id' ),self.all_simple_paths_in_digraph())


class comp_ramp_dispclass(component.component_base.comp_base):
    '''
    Ramp RoadType
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Ramp_DisplayClass')
        
    def _DoCreateTable(self):
        
        if self.CreateTable2('temp_ramp_update_dispclass') == -1:
            return -1 
        if self.CreateTable2('link_tbl_bak_dispclass') == -1:
            return -1 
        
        return 0
    
    def _DoCreateFunction(self):
        
        return 0
        
    def _DoCreateIndex(self): 
        
        return 0
    
    def _Do(self):
        
        
        self._UpdateRampDisplayClass()
        
        return 0
    
    def _updatedipclass(self):
         
        sqlcmd='''
                update link_tbl a
                set display_class=b.max
                from 
                (
                    select link_id,max(display_class) from 
                    temp_ramp_update_dispclass
                    group by link_id
                ) b
                where a.link_id=b.link_id
                '''
        self.pg.execute(sqlcmd)
        self.pg.commit()
        #print time.localtime()
        
    def _UpdateRampDisplayClass(self):
        
        self.log.info('start to update ramp display class')
        #self.log.info('ccc')
        #print time.localtime()
        
        g=CGraph()
        sqlcmd='''
                select link_id,s_node,e_node,one_way_code from link_tbl where link_type=5
                --and one_way_code<>4
                '''
        self.pg.execute2(sqlcmd)
        results=self.pg.fetchall2()
        
        for result in results:
            #print 1
            link_id=result[0]
            s_node=result[1]
            e_node=result[2]
            one_way_code=result[3]
            if one_way_code in (1,2):
                g.add_edge(s_node,e_node,attr_dict={'link_id':link_id})
            if one_way_code in (1,3):
                g.add_edge(e_node,s_node,attr_dict={'link_id':link_id})
        
        paths_node=g.all_simple_paths_in_digraph()
        paths_link=map(lambda x:g.get_linkid_of_path(x,'link_id' ),paths_node)
        #self.log.info('ccc')
        sqlcmd_start='''
                select display_class from link_tbl where link_type<>5
                and ( s_node=%d and one_way_code in (1,3) or e_node=%d and one_way_code in (1,2) )
                        '''
        sqlcmd_end='''
                select display_class from link_tbl where link_type<>5
                and ( s_node=%d and one_way_code in (1,2) or e_node=%d and one_way_code in (1,3) )
                        '''
        sqlcmd_insert='''
                insert into temp_ramp_update_dispclass
                values(%d,%d)
                        '''
        
        
        for idx in range(len(paths_node)):
            #self.log.info('111')
            path_node=paths_node[idx]
            path_link=paths_link[idx]
            #self.log.info('111')
            try:
                start_disp_class = max(set(x[0] for x in self.pg.get_batch_data2(sqlcmd_start%(path_node[0],path_node[0]))))
            except:
                start_disp_class = None
            
            try:
                end_disp_class = max(set(x[0] for x in self.pg.get_batch_data2(sqlcmd_end%(path_node[-1],path_node[-1]))))
            except:
                end_disp_class = None
            
            '''if not (display_class_start and display_class_end):
                continue
            display_class_set=display_class_start|display_class_end'''
            if start_disp_class and end_disp_class:
                min_display_class = min([start_disp_class,end_disp_class])
            elif start_disp_class == None:
                min_display_class = end_disp_class
            elif end_disp_class == None:
                min_display_class = start_disp_class
            elif not start_disp_class and not end_disp_class:
                min_display_class = None
            
            if min_display_class:
                for link_id in path_link:
                    self.pg.execute2(sqlcmd_insert%(int(link_id),min_display_class))
        
        self.pg.commit2()
        
        self._updatedipclass()