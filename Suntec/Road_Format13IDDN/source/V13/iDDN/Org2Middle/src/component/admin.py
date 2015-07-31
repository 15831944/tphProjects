'''
Created on 2012-2-23

@author: sunyifeng
'''

import base

class comp_admin(base.component_base.comp_base):
    
    def __init__(self):
        '''
        Constructor
        '''
        base.component_base.comp_base.__init__(self, 'Admin') 
            
    def _Do(self):
        self.CreateTable2('mid_admin_zone')
        
