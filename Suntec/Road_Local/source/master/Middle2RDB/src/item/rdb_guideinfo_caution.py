# -*- coding: cp936 -*-
'''
Created on 2012-2-7

@author: hongchenzai
'''
from base.rdb_ItemBase import ItemBase, ITEM_EXTEND_FLAG_IDX
from common import rdb_log

class rdb_guideinfo_caution(ItemBase):
    '''
    classdocs
    '''
    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self, 'Caution'
                          , 'caution_tbl'
                          , 'gid'
                          , 'rdb_guideinfo_caution'
                          , 'guideinfo_id'
                          , True
                          , ITEM_EXTEND_FLAG_IDX.get('CAUTION'))
    
    def Do_CreateTable(self):
        self.CreateTable2('rdb_guideinfo_caution')
        return 0

    def Do_CreatIndex(self):
        '创建相关表索引.'
        self.CreateIndex2('rdb_guideinfo_caution_in_link_id_node_id_idx')
        self.CreateIndex2('rdb_guideinfo_caution_node_id_idx')    
        self.CreateIndex2('rdb_guideinfo_caution_node_id_t_idx')    
        return 0
    
    def Do(self):
        
        sqlcmd = """
            INSERT INTO rdb_guideinfo_caution(
                        guideinfo_id, in_link_id, in_link_id_t, node_id, node_id_t, out_link_id, 
                        out_link_id_t, passlink_count, data_kind, voice_id, strTTS, image_id)
                SELECT a.gid, b.tile_link_id, b.tile_id, c.tile_node_id, c.tile_id, d.tile_link_id, 
                        d.tile_id, a.passlink_cnt, a.data_kind, a.voice_id, a.strTTS, a.image_id
                    FROM caution_tbl as a
                    left join rdb_tile_link as b
                    on a.inlinkid = b.old_link_id
                    left join rdb_tile_node as c
                    on a.nodeid = c.old_node_id
                    left join rdb_tile_link as d
                    on a.outlinkid = d.old_link_id
                    order by a.gid
        """
        
        rdb_log.log(self.ItemName, 'Start making rdb_guideinfo_caution', 'info') 
        
        if self.pg.execute2(sqlcmd) == -1:
            rdb_log.log(self.ItemName, 'making rdb_guideinfo_caution data failed.', 'error')
            return -1
        
        self.pg.commit2()
    
        rdb_log.log(self.ItemName, 'succeeded to make rdb_guideinfo_caution.', 'info')
        
        
