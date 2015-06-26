'''
Created on 2012-3-28

@author: hongchenzai
'''

import component.component_base

class comp_guideinfo_towardname(component.component_base.comp_base):
    '''
    classdocs
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Guideinfo_TowardName')

    def _DoCreateTable(self):
        self.CreateTable2('towardname_tbl')
        return 0

    def _DoCreateFunction(self):
        'Create Function'
        return 0

    def _DoCreateIndex(self):
        self.CreateIndex2('towardname_tbl_node_id_idx')
        return 0
