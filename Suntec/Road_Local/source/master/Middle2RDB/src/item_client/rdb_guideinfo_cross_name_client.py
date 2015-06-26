# -*- coding: cp936 -*-
'''
Created on 2012-3-7

@author: sunyifeng
'''

from base.rdb_ItemBase import ItemBase
from common import rdb_log

class rdb_guideinfo_cross_name_client(ItemBase):
    '''
    classdocs
    '''

    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self, 'Cross_name_client'
                          , 'rdb_guideinfo_crossname'
                          , 'guideinfo_id'
                          , 'rdb_guideinfo_crossname_client'
                          , 'guideinfo_id'
                          , False)
        
    def Do_CreateTable(self):
        
        if self.CreateTable2('rdb_guideinfo_crossname_client') == -1:
            return -1
        
        return 0
        
    def Do_CreatIndex(self):
        
        if self.CreateIndex2('rdb_guideinfo_crossname_client_node_id_t_node_id_idx') == -1:
            return -1
        if self.CreateIndex2('rdb_guideinfo_crossname_client_node_id_t_idx') == -1:
            return -1
        
        return 0

        
    def Do(self):
        
        sqlcmd = """
                INSERT INTO rdb_guideinfo_crossname_client(
                            in_link_id, 
                            in_link_id_t, 
                            node_id, 
                            node_id_t, 
                            out_link_id, 
                            out_link_id_t, 
                            cross_name)
                (
                SELECT 
                    rdb_split_tileid(in_link_id), 
                    in_link_id_t, 
                    rdb_split_tileid(node_id), 
                    node_id_t, 
                    rdb_split_tileid(out_link_id), 
                    out_link_id_t, 
                    cross_name
                  FROM rdb_guideinfo_crossname
                  order by guideinfo_id
                );
                """
        
        rdb_log.log(self.ItemName, "making rdb_guideinfo_crossname_client ...", "info")
        
        if self.pg.execute2(sqlcmd) == -1:
            rdb_log.log(self.ItemName, "failed to make rdb_guideinfo_crossname_client.", "error")
            return -1
        else:
            self.pg.commit2()
            
        rdb_log.log(self.ItemName, 'succeeded to make rdb_guideinfo_crossname_client.', 'info')
        
        return 0
    
    def _DoContraints(self):
        '添加外键'
        return 0
        sqlcmd = """
                --ALTER TABLE rdb_guideinfo_crossname_client DROP CONSTRAINT if exists rdb_guideinfo_crossname_client_in_link_id_fkey;
                --ALTER TABLE rdb_guideinfo_crossname_client
                --  ADD CONSTRAINT rdb_guideinfo_crossname_client_in_link_id_fkey FOREIGN KEY (in_link_id)
                --      REFERENCES rdb_link_client (link_id) MATCH FULL
                 --     ON UPDATE NO ACTION ON DELETE NO ACTION;
                      
                --ALTER TABLE rdb_guideinfo_crossname_client DROP CONSTRAINT if exists rdb_guideinfo_crossname_client_node_id_fkey;
                --ALTER TABLE rdb_guideinfo_crossname_client
                --  ADD CONSTRAINT rdb_guideinfo_crossname_client_node_id_fkey FOREIGN KEY (node_id)
                --      REFERENCES rdb_node_client (node_id) MATCH FULL
                --      ON UPDATE NO ACTION ON DELETE NO ACTION;
                      
                --ALTER TABLE rdb_guideinfo_crossname_client DROP CONSTRAINT if exists rdb_guideinfo_crossname_client_out_link_id_fkey;
                --ALTER TABLE rdb_guideinfo_crossname_client
                --  ADD CONSTRAINT rdb_guideinfo_crossname_client_out_link_id_fkey FOREIGN KEY (out_link_id)
                --      REFERENCES rdb_link_client (link_id) MATCH FULL
                --      ON UPDATE NO ACTION ON DELETE NO ACTION;
                
                ALTER TABLE rdb_guideinfo_crossname_client DROP CONSTRAINT if exists rdb_guideinfo_crossname_client_name_id_fkey;
                ALTER TABLE rdb_guideinfo_crossname_client
                  ADD CONSTRAINT rdb_guideinfo_crossname_client_name_id_fkey FOREIGN KEY (name_id)
                      REFERENCES rdb_name_dictionary (name_id) MATCH FULL
                      ON UPDATE NO ACTION ON DELETE NO ACTION;
                """
        sqlcmd = self.pg.ReplaceDictionary(sqlcmd) 
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
            return 0
    
    
        
    