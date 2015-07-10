# -*- coding: UTF8 -*-
'''
Created on 2015-6-29

@author: PC_ZH
'''
from component.component_base import comp_base


class HwyPrepareNi(comp_base):
    '''
    Hwy Prepare
    '''

    def __init__(self, itemname='HwyPrepareNi'):
        '''
        Constructor
        '''
        comp_base.__init__(self, itemname)

    def _DoCreateTable(self):
        return 0

    def _DoCreateIndex(self):
        return 0

    def _Do(self):
        self._make_all_hwy_node()
        return 0

    def _make_all_hwy_node(self):
        # 高速特征点(高速出入口点和高速设施所在点)
        self.CreateTable2('mid_all_highway_node')
        return 0
