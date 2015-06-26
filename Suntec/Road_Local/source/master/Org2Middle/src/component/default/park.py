# -*- coding: cp936 -*-
'''
Created on 2014-4-22

@author: zhaojie
'''
import component.component_base

class comp_park(component.component_base.comp_base):
    '''illust
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'park_tbl')
        
    def _DoCreateTable(self):
        self.CreateTable2('park_link_tbl')
        self.CreateTable2('park_node_tbl')
        self.CreateTable2('park_polygon_tbl')
        return 0
    
    def _DoCreateIndex(self):
        self.CreateIndex2('park_link_id_idx')
        self.CreateIndex2('park_node_id_idx')
        self.CreateIndex2('park_polygon_id_idx')
        return 0