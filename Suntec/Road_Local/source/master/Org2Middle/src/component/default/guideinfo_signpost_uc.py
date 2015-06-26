# -*- coding: cp936 -*-
'''
Created on 2012-10-24

@author: hongchenzai
'''

import component.component_base

class comp_guideinfo_signpost_uc(component.component_base.comp_base):
    '''OverSea方面看板
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'signpost_uc')

    def _DoCreateTable(self):
        self.CreateTable2('signpost_uc_tbl')
        return 0

    def _DoCreateIndex(self):
        self.CreateIndex2('signpost_uc_tbl_node_id_idx')
        return 0
