'''
Created on 2012-2-23

@author: sunyifeng
'''

import common
import base
import io
import os

class comp_guideinfo_caution(base.component_base.comp_base):
    
    def __init__(self):
        '''
        Constructor
        '''
        base.component_base.comp_base.__init__(self, 'Guideinfo Caution') 
        
    def _DoCreateTable(self):
        
        self.CreateTable2('caution_tbl')  
        
        return 0
    
    def _DoCreateFunction(self):
        
        return 0
    
    def _Do(self):
        
        return 0

