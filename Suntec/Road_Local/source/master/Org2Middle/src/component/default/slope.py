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
        
    def _Do(self):
        self.CreateTable2('grad')
        
        return 0