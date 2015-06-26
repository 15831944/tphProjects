# -*- coding: cp936 -*-

import component.component_base

class comp_hook_turn(component.component_base.comp_base):

    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'hook_turn')
        
    def _DoCreateTable(self):
        self.CreateTable2('hook_turn_tbl')
        return 0
