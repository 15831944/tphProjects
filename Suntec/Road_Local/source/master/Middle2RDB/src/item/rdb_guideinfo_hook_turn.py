# -*- coding: cp936 -*-
'''
Created on 2015-3-18

@author: zhaojie
'''

from base.rdb_ItemBase import ItemBase, ITEM_EXTEND_FLAG_IDX
from common import rdb_log


class rdb_guideinfo_hook_turn(ItemBase):
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
                          , 'rdb_guideinfo_hook_turn'
                          , 'guideinfo_id'
                          , True)
    def Do_CreateTable(self):
        
        if self.CreateTable2('rdb_guideinfo_hook_turn') == -1:
            return -1
        
        return 0
        
    def Do_CreatIndex(self):
        
        if self.CreateIndex2('rdb_guideinfo_hook_turn_in_link_id_node_id_idx') == -1:
            return -1
        if self.CreateIndex2('rdb_guideinfo_hook_turn_node_id_idx') == -1:
            return -1
        if self.CreateIndex2('rdb_guideinfo_hook_turn_node_id_t_idx') == -1:
            return -1
        
        return 0
            
    def Do(self): 
        sqlcmd = """
                  insert into rdb_guideinfo_hook_turn
                         (  in_link_id
                           ,in_link_id_t
                           ,node_id
                           ,node_id_t
                           ,out_link_id
                           ,out_link_id_t
                           ,passlink_count
                          )
                  SELECT  a.tile_link_id as in_link_id
                        , a.tile_id as in_tile_id
                        , b.tile_node_id as node_id
                        , b.tile_id as node_tile_id
                        , c.tile_link_id as out_link_id
                        , c.tile_id as out_tile_id
                        , s.passlink_cnt
                    FROM hook_turn_tbl as s
                    LEFT JOIN rdb_tile_link as a
                    ON cast(inlinkid as bigint) = a.old_link_id
                    LEFT JOIN rdb_tile_node as b
                    ON cast(nodeid as bigint) = b.old_node_id
                    LEFT JOIN rdb_tile_link as c
                    ON cast(outlinkid as bigint) = c.old_link_id
                    where b.old_node_id is not null
                    order by s.id;
                  """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        return 0      

