# -*- coding: cp936 -*-
'''
Created on 2012-3-6

@author: sunyifeng
'''
from base.rdb_ItemBase import ItemBase
from common import rdb_log

class rdb_guideinfo_towardname_client(ItemBase):
    '''
    classdocs
    '''


    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self, 'Towardname_client'
                          , 'rdb_guideinfo_towardname'
                          , 'gid'
                          , 'rdb_guideinfo_towardname_client'
                          , 'gid'
                          , False)
        
    def Do_CreateTable(self):
        
        if self.CreateTable2('rdb_guideinfo_towardname_client') == -1:
            return -1
        
        return 0
        
    def Do_CreatIndex(self):
        
        if self.CreateIndex2('rdb_guideinfo_towardname_client_node_id_idx') == -1:
            return -1
        if self.CreateIndex2('rdb_guideinfo_towardname_client_in_link_id_node_id_idx') == -1:
            return -1
        if self.CreateIndex2('rdb_guideinfo_towardname_client_node_id_t_idx') == -1:
            return -1
        
        return 0
    
    def Do_CreateFunction(self):
        
        if self.CreateFunction2('rdb_cnt_towardname_client') == -1:
            return -1
        
        return 0       
        
    def Do(self):

        #rdb_log.log(self.ItemName, 'Start Make rdb_guideinfo_towardname_client.', 'info')
        
        sqlcmd = """
                INSERT INTO rdb_guideinfo_towardname_client(
                            in_link_id, in_link_id_t,
                            node_id, node_id_t, 
                            out_link_id, out_link_id_t,
                            toward_name, multi_towardname_attr)
                (
                SELECT rdb_split_tileid(in_link_id), in_link_id_t,
                       rdb_split_tileid(node_id), node_id_t, 
                       rdb_split_tileid(out_link_id), out_link_id_t,
                       toward_name, 
                       rdb_cnt_towardname_client(name_attr, name_kind, passlink_count)
                  FROM rdb_guideinfo_towardname
                  order by gid
                );
                """
        rdb_log.log(self.ItemName, 'Inserting records to table rdb_guideinfo_towardname_client...', 'info')
        if self.pg.execute2(sqlcmd) == -1:
            rdb_log.log(self.ItemName, 'Insert records to table rdb_guideinfo_towardname_client failed.', 'error')
            return -1
        else:
            self.pg.commit2()
            rdb_log.log(self.ItemName, 'Inserting records to table rdb_guideinfo_towardname_client succeeded.', 'info')
                
        
        #rdb_log.log(self.ItemName, 'End Make rdb_guideinfo_towardname_client.', 'info')
        return 0
    
    def _DoContraints(self):
        '添加外键'
        return 0
        sqlcmd = """
                ALTER TABLE rdb_guideinfo_towardname_client DROP CONSTRAINT if exists rdb_guideinfo_towardname_client_in_link_id_fkey;
                ALTER TABLE rdb_guideinfo_towardname_client
                  ADD CONSTRAINT rdb_guideinfo_towardname_client_in_link_id_fkey FOREIGN KEY (in_link_id)
                      REFERENCES rdb_link_client (link_id) MATCH FULL
                      ON UPDATE NO ACTION ON DELETE NO ACTION;
                
                ALTER TABLE rdb_guideinfo_towardname_client DROP CONSTRAINT if exists rdb_guideinfo_towardname_client_node_id_fkey;
                ALTER TABLE rdb_guideinfo_towardname_client
                  ADD CONSTRAINT rdb_guideinfo_towardname_client_node_id_fkey FOREIGN KEY (node_id)
                      REFERENCES rdb_node_client (node_id) MATCH FULL
                      ON UPDATE NO ACTION ON DELETE NO ACTION;
                      
                ALTER TABLE rdb_guideinfo_towardname_client DROP CONSTRAINT if exists rdb_guideinfo_towardname_client_out_link_id_fkey;
                ALTER TABLE rdb_guideinfo_towardname_client
                  ADD CONSTRAINT rdb_guideinfo_towardname_client_out_link_id_fkey FOREIGN KEY (out_link_id)
                      REFERENCES rdb_link_client (link_id) MATCH FULL
                      ON UPDATE NO ACTION ON DELETE NO ACTION;
                      
                --ALTER TABLE rdb_guideinfo_towardname_client DROP CONSTRAINT if exists rdb_guideinfo_towardname_client_name_id_fkey;
                --ALTER TABLE rdb_guideinfo_towardname_client
                --ADD CONSTRAINT rdb_guideinfo_towardname_client_name_id_fkey FOREIGN KEY (name_id)
                --      REFERENCES rdb_name_dictionary (name_id) MATCH FULL
                --      ON UPDATE NO ACTION ON DELETE NO ACTION; 
                """
        sqlcmd = self.pg.ReplaceDictionary(sqlcmd) 
        
        #if self.pg.execute2(sqlcmd) == -1:
        #    return -1
        #else:
        #    self.pg.commit2()
        return 0
    