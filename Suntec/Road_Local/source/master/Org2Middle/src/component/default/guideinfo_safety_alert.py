# -*- coding: UTF8 -*-
'''
Created on 2015-3

@author: zc
'''
from component.component_base import comp_base

class comp_guideinfo_safety_alert(comp_base):
    '''
    classdocs
    '''
    
    def __init__(self):
        '''
        Constructor
        '''
        comp_base.__init__(self, 'Guideinfo_Safety_Alert')
        
    def _DoCreateTable(self):
        if self.CreateTable2('safety_alert_tbl') == -1:
            return -1
        return 0
    
    def _DoCreateIndex(self):
        'create index.'
        return 0
    
    def _DoCreateFunction(self):
        return 0
    
    def _Do(self):
        return