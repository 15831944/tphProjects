'''
Created on 2012-3-28

@author: hongchenzai
'''
from base import comp_base

class comp_guideinfo_towardname(comp_base):
    '''
    classdocs
    '''

    def __init__(self):
        '''
        Constructor
        '''
        comp_base.__init__(self, 'Guideinfo_TowardName')

    def _DoCreateTable(self):
        self.CreateTable2('towardname_tbl')
        return 0

    def _DoCreateFunction(self):
        'Create Function'
        return 0

    def _DoCreateIndex(self):
        self.CreateIndex2('towardname_tbl_node_id_idx')
        return 0
