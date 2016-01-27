# -*- coding: UTF-8 -*-
'''
Created on 2015-10-26

@author: hcz
'''
from component.rdf.hwy.hwy_prepare_rdf import HwyPrepareRDF
from component.rdf.hwy.hwy_link_mapping import HwyLinkMapping


class HwyPrepareNiPro(HwyPrepareRDF):
    '''
    classdocs
    '''

    def __init__(self, ItemName='HwyPrepareNiPro'):
        '''
        Constructor
        '''
        HwyPrepareRDF.__init__(self, ItemName)

    def _DoCreateIndex(self):
        return 0

    def _DoCreateTable(self):
        return 0

    def _Do(self):
        self._make_all_hwy_node()
        return 0

    def _make_all_hwy_node(self):
        '''高速特征点(高速出入口点和高速设施所在点) '''
        self.CreateTable2('mid_all_highway_node')
        sqlcmd = '''
        INSERT INTO mid_all_highway_node(node_id)
        (
        SELECT distinct nodeid::bigint
          FROM org_hw_junction
          order by nodeid
        )
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return 0
