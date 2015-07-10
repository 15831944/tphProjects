# -*- coding: UTF8 -*-
'''
Created on 2013-4-23

@author: zhangyongmu
'''

import component.component_base

class comp_hwy_prepare(component.component_base.comp_base):
    '''
    Hwy Prepare (TomTom)
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'HwyPrepare')

    def _DoCreateTable(self):
        self.CreateTable2('mid_all_highway_node')
        return 0

class comp_highway(component.component_base.comp_base):
    '''
    high way class
    '''
    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Highway')

    def _DoCreateTable(self):

        if self.CreateTable2('highway_facility_tbl') == -1:
            return -1
        if self.CreateTable2('highway_sa_pa_tbl') == -1:
            return -1
        self.CreateTable2('mid_all_highway_node')
        return 0

