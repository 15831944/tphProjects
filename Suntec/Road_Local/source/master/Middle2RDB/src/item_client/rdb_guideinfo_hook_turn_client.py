# -*- coding: cp936 -*-
'''
Created on 2015-3-18

@author: zhaojie
'''

from base.rdb_ItemBase import ItemBase, ITEM_EXTEND_FLAG_IDX
from common import rdb_log


class rdb_guideinfo_hook_turn_client(ItemBase):
    '''
    classdocs
    '''


    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self, 'hook_turn'
                          , 'hook_turn_tbl'
                          , 'gid'
                          , 'rdb_guideinfo_hook_turn_client'
                          , 'guideinfo_id'
                          , True)
    def Do_CreateTable(self):
        
        if self.CreateTable2('rdb_guideinfo_hook_turn_client') == -1:
            return -1
        
        return 0
        
    def Do_CreatIndex(self):
        
        if self.CreateIndex2('rdb_guideinfo_hook_turn_client_in_link_id_node_id_idx') == -1:
            return -1
        if self.CreateIndex2('rdb_guideinfo_hook_turn_client_node_id_idx') == -1:
            return -1
        if self.CreateIndex2('rdb_guideinfo_hook_turn_client_node_id_t_idx') == -1:
            return -1
        
        return 0
            
    def Do(self): 
        sqlcmd = """
                  insert into rdb_guideinfo_hook_turn_client
                         (  guideinfo_id
                           ,in_link_id
                           ,in_link_id_t
                           ,node_id
                           ,node_id_t
                           ,out_link_id
                           ,out_link_id_t
                           ,passlink_count
                          )
                    select guideinfo_id
                           ,rdb_split_tileid(in_link_id)
                           ,in_link_id_t
                           ,rdb_split_tileid(node_id)
                           ,node_id_t
                           ,rdb_split_tileid(out_link_id)
                           ,out_link_id_t
                           ,passlink_count
                    from rdb_guideinfo_hook_turn
                    order by guideinfo_id
                  """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        return 0      

