# -*- coding: cp936 -*-
'''
Created on 2012-6-3

@author: hongchenzai
'''
from base import comp_base
class comp_guideinfo_spotguide(comp_base):
    '''illust
    '''

    def __init__(self):
        '''
        Constructor
        '''
        comp_base.__init__(self, 'spotguide_tbl')
        
    def _DoCreateTable(self):
        self.CreateTable2('spotguide_tbl')
        return 0
    
    def _DoCreateIndex(self):
        self.CreateIndex2('spotguide_tbl_node_id_idx')
        return 0