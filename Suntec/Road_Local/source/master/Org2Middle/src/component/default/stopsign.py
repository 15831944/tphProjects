# -*- coding: cp936 -*-

import component.component_base

class comp_stopsign(component.component_base.comp_base):

    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'stopsign')
        
    def _DoCreateTable(self):
        self.CreateTable2('stopsign_tbl')
        return 0
