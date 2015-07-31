# -*- coding: cp936 -*-
'''
Created on 2012-03-12

@author: hongchenzai
'''
from base.rdb_ItemBase import ItemBase
from common import rdb_log

class rdb_linklane_info_client(ItemBase):
    '''
    Link车线信息类
    '''

    def __init__(self):
        ItemBase.__init__(self, 'Link_lane_info_client'
                          , 'rdb_linklane_info'
                          , 'lane_id'
                          , 'rdb_linklane_info_client'
                          , 'lane_id'
                          , False)
       
    def Do_CreateTable(self):
        return self.CreateTable2('rdb_linklane_info_client')
    
    def Do_CreateFunction(self):
        'Create Function'
        if self.CreateFunction2('rdb_cnv_linklane01_client') == -1:
            return -1
        if self.CreateFunction2('rdb_cnv_linklane02_client') == -1:
            return -1
        return 0
     
    def Do(self):
        sqlcmd1 = """
                INSERT INTO rdb_linklane_info_client(
                            lane_id, multi_lane_attribute_1, multi_lane_attribute_2)
                (
                SELECT lane_id,
                       rdb_cnv_linklane01_client(disobey_flag, lane_dir, lane_up_down_distingush),
                       rdb_cnv_linklane02_client(ops_lane_number, ops_width, neg_lane_number, neg_width)
                  FROM rdb_linklane_info
                );
                """
            
        if self.pg.execute2(sqlcmd1) == -1:
            rdb_log.log(self.ItemName, 'Insert record into rdb_linklane_info_client failed.', 'info') 
            return -1
        else:
            self.pg.commit()
            
        return 0
    
    def Do_CreatIndex(self):
        '创建相关表索引.'
        return 0
        
        