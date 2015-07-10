# -*- coding: UTF8 -*-
'''
Created on 2015-4-29

@author: hcz
'''
from component.rdf.hwy.hwy_prepare_rdf import HwyPrepareRDF


class HwyPrepareNi(HwyPrepareRDF):
    '''
    Hwy Prepare (TomTom)
    '''

    def __init__(self):
        '''
        Constructor
        '''
        HwyPrepareRDF.__init__(self, itemname='HwyPrepareNi')

    def _DoCreateTable(self):
        return 0

    def _DoCreateIndex(self):
        return 0

    def _Do(self):
        # self._make_hwy_exit_poi()
        self._make_all_hwy_Node()
        return 0

    def _make_all_hwy_Node(self):
        '''高速特征点(高速出入口点和高速设施所在点) '''
        self.CreateTable2('mid_all_highway_node')
        sqlcmd = '''
        INSERT INTO mid_all_highway_node(node_id)
        (
        SELECT id::bigint AS node_id
        FROM org_poi
        LEFT JOIN org_n
        ON ST_DWithin(org_n.the_geom, org_poi.the_geom,
                      0.000002694945852358566745)
        WHERE org_poi.kind = '8301'
        ORDER BY id
        )
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return 0
