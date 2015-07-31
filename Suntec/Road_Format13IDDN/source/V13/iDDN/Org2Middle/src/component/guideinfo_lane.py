# -*- coding: cp936 -*-
'''
Created on 2012-3-22

@author: hongchenzai
'''
from base import comp_base

class comp_guideinfo_lane(comp_base):
    '''诱导车线信息类
    '''

    def __init__(self):
        '''
        Constructor
        '''
        comp_base.__init__(self, 'Guideinfo_Lane')
        
    def _DoCreateTable(self):
        self.CreateTable2('lane_tbl')
        return 0
    
    def _DoCreateIndex(self):
        self.CreateIndex2('lane_tbl_node_id_idx')
        return 0