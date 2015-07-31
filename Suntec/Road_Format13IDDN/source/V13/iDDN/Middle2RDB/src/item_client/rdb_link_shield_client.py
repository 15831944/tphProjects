# -*- coding: cp936 -*-
'''
Created on 2012-9-29

@author: hongchenzai
'''
from base.rdb_ItemBase import ItemBase
from common import rdb_log


class rdb_link_shield_client(ItemBase):
    '''
    shield
    '''

    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self, 'Shield_Client'
                          , 'rdb_link_shield'
                          , 'gid'
                          , 'rdb_link_shield_client'
                          , 'gid')
        
    def Do_CreateTable(self):
        return self.CreateTable2('rdb_link_shield_client')
    
    def Do_CreateFunction(self):    
        if self.CreateFunction2('rdb_split_tileid') == -1:
            return -1
        
    def Do(self):
        sqlcmd = """
                INSERT INTO rdb_link_shield_client(link_id, link_id_t, name_id)
                (
                SELECT rdb_split_tileid(link_id), link_id_t, name_id
                  FROM rdb_link_shield
                  order by gid
                );
        """
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
            return 0
    
    def Do_CreatIndex(self):
        '创建相关表索引.'
        self.CreateIndex2('rdb_link_shield_client_link_id_idx')
        return 0
    
    def _DoContraints(self):
        '添加外键'
        sqlcmd = """
                -- Create FOREIGN KEY
                ALTER TABLE rdb_link_shield_client DROP CONSTRAINT if exists rdb_link_shield_client_link_id_fkey;
                ALTER TABLE rdb_link_shield_client
                  ADD CONSTRAINT rdb_link_shield_client_link_id_fkey FOREIGN KEY (link_id)
                      REFERENCES rdb_link_client (link_id) MATCH SIMPLE
                      ON UPDATE NO ACTION ON DELETE NO ACTION;
                """
        
        #if self.pg.execute2(sqlcmd) == -1:
        #    return -1
        #else:
        #    self.pg.commit2()
        return 0
        