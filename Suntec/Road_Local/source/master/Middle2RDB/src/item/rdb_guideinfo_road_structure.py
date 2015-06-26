# -*- coding: cp936 -*-
'''
Created on 2012-2-7

@author: hongchenzai
'''
from base.rdb_ItemBase import ItemBase
from common import rdb_log

class rdb_guideinfo_road_structure(ItemBase):
    '''
    classdocs
    '''
    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self, 'Road_Structure')
    
    def Do_CreateTable(self):
        self.CreateTable2('rdb_guideinfo_road_structure')
        return 0

    def Do_CreatIndex(self):
        '创建相关表索引.'
        self.CreateIndex2('rdb_guideinfo_road_structure_in_link_id_node_id_idx')
        self.CreateIndex2('rdb_guideinfo_road_structure_node_id_idx')    
        self.CreateIndex2('rdb_guideinfo_road_structure_node_id_t_idx')    
        return 0
