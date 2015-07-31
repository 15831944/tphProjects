# -*- coding: cp936 -*-
'''
Created on 2012-2-7

@author: hongchenzai
'''
from base.rdb_ItemBase import ItemBase
from common import rdb_log

class rdb_guideinfo_building_structure_client(ItemBase):
    '''
    classdocs
    '''
    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self, 'Building_Structure_Client'
                          , 'rdb_guideinfo_building_structure'
                          , 'guideinfo_id'
                          , 'rdb_guideinfo_building_structure_client'
                          , 'guideinfo_id'
                          , False)
    
    def Do_CreateTable(self):
        self.CreateTable2('rdb_guideinfo_building_structure_client')
        return 0

    def Do_CreatIndex(self):
        '创建相关表索引.'
        self.CreateIndex2('rdb_guideinfo_building_structure_client_in_link_id_node_id_idx')
        self.CreateIndex2('rdb_guideinfo_building_structure_client_node_id_idx')   
        return 0
