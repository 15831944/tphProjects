# -*- coding: UTF8 -*-
'''
Created on 2014-8-4

@author: yuanrui
'''

import component.component_base

class comp_node_msm(component.component_base.comp_base):
    '''
    make node_tbl
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Node')

    def _DoCreateTable(self):
        
        self.CreateTable2('node_tbl')

    def _DoCreateFunction(self):

        return 0

    def _Do(self):

        sqlcmd = """
                insert into node_tbl (
                    node_id, kind, light_flag, toll_flag, mainnodeid, node_lid,
                    node_name, the_geom )
                (
                    select a.node_id, null, null, null, 0,
                        array_to_string(b.node_lid, '|'),
                        null as node_name, a.the_geom
                    from temp_topo_node as a
                    left outer join (
                        select n.node_id as node_id, array_agg(l.new_link_id) as node_lid
                        from temp_topo_node n, temp_topo_link l
                        where n.node_id = l.start_node_id or n.node_id = l.end_node_id
                        group by n.node_id
                    ) as b
                    on a.node_id = b.node_id
                )
                """

        self.log.info(': Now it is inserting node_tbl...')
        
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        self.log.info(': end of inserting node_tbl...')

    def _DoCreateIndex(self):

        self.CreateIndex2('node_tbl_node_id_idx')
        self.CreateIndex2('node_tbl_the_geom_idx')

