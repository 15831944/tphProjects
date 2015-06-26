# -*- coding: UTF-8 -*-
'''
Created on 2014-6-27

@author: zhangyongmu
'''
import io
import os
import component.component_base

class comp_guideinfo_building(component.component_base.comp_base):
    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Guideinfo_building')

    def _DoCreateFunction(self):
        pass
    
    def _DoCreateTable(self):
        self.CreateTable2('building_structure')
        self.CreateTable2('mid_logmark')
        return 0
    
    def _Do(self):
        
        return 0