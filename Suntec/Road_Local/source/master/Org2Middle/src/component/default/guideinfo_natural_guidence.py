# -*- coding: UTF8 -*-
'''
Created on 2014-11-15

@author: liuxinxing
'''

import component.component_base

class comp_natural_guidence(component.component_base.comp_base):
    '''
    classdocs
    '''
    
    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Natural_Guidence')
        
    def _DoCreateTable(self):
        self.CreateTable2('natural_guidence_tbl')
        self.CreateTable2('guidence_condition_tbl')
        return 0
    
    
    
    
    