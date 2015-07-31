# -*- coding: UTF8 -*-
'''
Created on 2012-12-5

@author: yuanrui
'''

import base

class comp_node_nostra(base.component_base.comp_base):
    '''
    make node_tbl
    '''

    def __init__(self):
        '''
        Constructor
        '''
        base.component_base.comp_base.__init__(self, 'Node')

    def _DoCreateTable(self):
        if self.CreateTable2('node_tbl') == -1:
            return -1

        return 0

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
                        select n.node_id as node_id, array_agg(l.routeid) as node_lid
                        from temp_topo_node n, temp_topo_link l
                        where n.node_id = l.start_node_id or n.node_id = l.end_node_id
                        group by n.node_id
                    ) as b
                    on a.node_id = b.node_id
                )
                """

        self.log.info(': Now it is inserting node_tbl...')
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()

        self.log.info(': end of inserting node_tbl...')

        return 0

    def _DoCreateIndex(self):
        'create index.'
        self.CreateIndex2('node_tbl_node_id_idx')
        self.CreateIndex2('node_tbl_the_geom_idx')

        return 0
