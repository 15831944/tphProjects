# -*- coding: UTF8 -*-
'''
Created on 2015-3-18

@author: hcz
'''
from component.rdf.hwy.hwy_prepare_rdf import HwyPrepareRDF


class HwyPrepareTa(HwyPrepareRDF):
    '''
    Hwy Prepare (TomTom)
    '''

    def __init__(self):
        '''
        Constructor
        '''
        HwyPrepareRDF.__init__(self)

    def _DoCreateTable(self):
        return 0

    def _DoCreateIndex(self):
        return 0

    def _Do(self):
        self._make_all_hwy_Node()

    def _make_all_hwy_Node(self):
        # 高速特征点(高速出入口点和高速设施所在点)
        self.CreateTable2('mid_all_highway_node')
