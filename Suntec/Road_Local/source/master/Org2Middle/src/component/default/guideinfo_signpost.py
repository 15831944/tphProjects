# -*- coding: cp936 -*-
'''
Created on 2012-6-3

@author: hongchenzai
'''

import component.component_base

class comp_guideinfo_signpost(component.component_base.comp_base):
    '''方面看板
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'signpost_tbl')
        
    def _DoCreateTable(self):
        self.CreateTable2('signpost_tbl')
        return 0
    
    def _DoCreateIndex(self):
        self.CreateIndex2('signpost_tbl_node_id_idx')
        return 0