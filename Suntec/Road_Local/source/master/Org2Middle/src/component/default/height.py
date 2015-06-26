# -*- coding: cp936 -*-

import component.component_base

class comp_height(component.component_base.comp_base):

    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'height')
        
    def _DoCreateTable(self):
        self.CreateTable2('node_height_tbl')
        return 0
