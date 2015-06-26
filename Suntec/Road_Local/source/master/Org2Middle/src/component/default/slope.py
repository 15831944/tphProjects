# -*- coding: cp936 -*-
'''
Created on 2015-1-21

@author: zhaojie
'''
import component.component_base

class comp_slope(component.component_base.comp_base):
    '''slope
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'slope_tbl')
        
    def _DoCreateTable(self):
        
        if self.pg.IsExistTable('grad'):
            return 0
        
        self.CreateTable2('grad')
        
        return 0