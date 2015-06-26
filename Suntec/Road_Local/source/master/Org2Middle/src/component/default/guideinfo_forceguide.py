# -*- coding: UTF8 -*-
'''
Created on 2012-11-9

@author: hongchenzai
'''

import component.component_base

class com_guideinfo_forceguide(component.component_base.comp_base):
    '''强制诱导的Base Class
    '''


    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Guideinfo_ForceGuide')
        
    def _DoCreateTable(self):
        
        if self.CreateTable2('force_guide_tbl') == -1:
            return -1
        
        return 0
    
    def _DoCreateFunction(self):
        
        return 0
        
        
    def _Do(self):
        
        return 0