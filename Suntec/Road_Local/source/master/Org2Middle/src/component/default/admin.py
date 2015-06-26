'''
Created on 2012-2-23

@author: sunyifeng
'''

import component.component_base

class comp_admin(component.component_base.comp_base):
    
    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Admin') 
            
    def _Do(self):
        self.CreateTable2('mid_admin_zone')
        self.CreateTable2('mid_admin_summer_time')
        
