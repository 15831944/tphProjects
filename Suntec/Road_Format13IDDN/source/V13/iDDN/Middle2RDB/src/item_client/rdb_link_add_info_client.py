# -*- coding: cp936 -*-
'''
Created on 2011-12-5

@author: hongchenzai
'''
from base.rdb_ItemBase import ItemBase
from common import rdb_log

class rdb_link_add_info_client(ItemBase):
    '''
    Link的付加信息类
    '''

    def __init__(self):
        ItemBase.__init__(self, 'Link_add_info_client'
                          , 'rdb_link_add_info'
                          , 'link_id'
                          , 'rdb_link_add_info_client'
                          , 'link_id'
                          , False)
    
    def Do_CreateTable(self):
        return self.CreateTable2('rdb_link_add_info_client')
    
    def Do_CreateFunction(self):
        # Create Function 
        if self.CreateFunction2('rdb_cnv_add_info_client') == -1:
            return -1
    
        if self.CreateFunction2('rdb_split_tileid') == -1:
            return -1
        
        return 0
    
    def Do(self):    
    
        sqlcmd1 = """
                INSERT INTO rdb_link_add_info_client(
                            link_id, link_id_t, multi_link_add_info)
                (
                SELECT rdb_split_tileid(link_id), link_id_t,
                    rdb_cnv_add_info_client(struct_code, shortcut_code, parking_flag, etc_lane_flag)
                  FROM rdb_link_add_info
                );
                """
    
        if self.pg.execute2(sqlcmd1) == -1:
            rdb_log.log(self.ItemName, 'Insert record into Link Add Flag failed.', 'info') 
            return -1
        else:
            self.pg.commit()
            
        return 0
    
    def Do_CreatIndex(self):
        '创建相关表索引.'
        self.CreateIndex2('rdb_link_add_info_client_link_id_idx')
        return 0
    
    def _DoContraints(self):
        '添加外键'
        sqlcmd = """
                --ALTER TABLE rdb_link_add_info_client DROP CONSTRAINT if exists rdb_link_add_info_client_link_id_fkey;      
                --ALTER TABLE rdb_link_add_info_client
                --  ADD CONSTRAINT rdb_link_add_info_client_link_id_fkey FOREIGN KEY (link_id)
                --      REFERENCES rdb_link_client (link_id) MATCH FULL
                --      ON UPDATE NO ACTION ON DELETE NO ACTION;
                      
                ALTER TABLE rdb_linklane_info_client DROP CONSTRAINT if exists rdb_linklane_info_client_lane_id_fkey;       
                ALTER TABLE rdb_linklane_info_client
                  ADD CONSTRAINT rdb_linklane_info_client_lane_id_fkey FOREIGN KEY (lane_id)
                      REFERENCES rdb_linklane_info (lane_id) MATCH FULL
                      ON UPDATE NO ACTION ON DELETE NO ACTION;
                """
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
            return 0
        
        