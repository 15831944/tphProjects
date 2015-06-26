# -*- coding: cp936 -*-
'''
Created on 2012-9-29

@author: hongchenzai
'''
from base.rdb_ItemBase import ItemBase
from common import rdb_log


class rdb_link_shield(ItemBase):
    '''
    shield
    '''

    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self, 'Shield'
                          , 'link_shield_tbl'
                          , 'gid'
                          , 'rdb_link_shield'
                          , 'gid')
        
    def Do_CreateTable(self):
        return self.CreateTable2('rdb_link_shield')
    
    def Do(self):
        sqlcmd = """
                INSERT INTO rdb_link_shield(link_id, link_id_t, name_id)
                (
                SELECT tile_link_id, tile_id, new_name_id
                  FROM (
                    SELECT link_id, name_id, seqnm
                      FROM link_shield_tbl
                      --WHERE seqnm = 1
                   ) as a
                 left join rdb_tile_link
                 on a.link_id = old_link_id
                 left join temp_name_dictionary
                 on name_id = old_name_id
                 order by tile_link_id, seqnm
                );
        """
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
            return 0
    
    def Do_CreatIndex(self):
        '创建相关表索引.'
        self.CreateIndex2('rdb_link_shield_link_id_idx')
        self.CreateIndex2('rdb_link_shield_link_id_t_idx')
        return 0
    
    def _DoContraints(self):
        '添加外键'
        sqlcmd = """
                -- Create FOREIGN KEY
                ALTER TABLE rdb_link_shield DROP CONSTRAINT if exists rdb_link_shield_link_id_fkey;
                ALTER TABLE rdb_link_shield
                  ADD CONSTRAINT rdb_link_shield_link_id_fkey FOREIGN KEY (link_id)
                      REFERENCES rdb_link (link_id) MATCH SIMPLE
                      ON UPDATE NO ACTION ON DELETE NO ACTION;
                """
        
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
            return 0
    