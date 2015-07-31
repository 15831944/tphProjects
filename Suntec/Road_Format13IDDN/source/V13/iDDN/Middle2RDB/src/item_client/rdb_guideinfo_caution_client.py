# -*- coding: cp936 -*-
'''
Created on 2012-2-7

@author: hongchenzai
'''
from base.rdb_ItemBase import ItemBase
from common import rdb_log

class rdb_guideinfo_caution_client(ItemBase):
    '''
    classdocs
    '''
    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self, 'Caution_client'
                          , 'rdb_guideinfo_caution'
                          , 'guideinfo_id'
                          , 'rdb_guideinfo_caution_client'
                          , 'guideinfo_id'
                          , False)
    
    def Do_CreateTable(self):
        self.CreateTable2('rdb_guideinfo_caution_client')
        return 0

    def Do_CreatIndex(self):
        '创建相关表索引.'
        self.CreateIndex2('rdb_guideinfo_caution_client_in_link_id_node_id_idx')
        self.CreateIndex2('rdb_guideinfo_caution_client_node_id_idx')    
        return 0

    def Do(self):
        
        sqlcmd = """
            INSERT INTO rdb_guideinfo_caution_client(
                        guideinfo_id, in_link_id, in_link_id_t, node_id, node_id_t, out_link_id, 
                        out_link_id_t, passlink_count, data_kind, voice_id)
                SELECT guideinfo_id, 
                        rdb_split_tileid(in_link_id), in_link_id_t, 
                        rdb_split_tileid(node_id), node_id_t, 
                        rdb_split_tileid(out_link_id), out_link_id_t, 
                        passlink_count, data_kind, voice_id
                    FROM rdb_guideinfo_caution
                    order by guideinfo_id;
        """
        
        rdb_log.log(self.ItemName, 'Start making rdb_guideinfo_caution_client', 'info') 
        
        if self.pg.execute2(sqlcmd) == -1:
            rdb_log.log(self.ItemName, 'making rdb_guideinfo_caution_client data failed.', 'error')
            return -1
        
        self.pg.commit2()
    
        rdb_log.log(self.ItemName, 'succeeded to make rdb_guideinfo_caution_client.', 'info')
        
        
        
