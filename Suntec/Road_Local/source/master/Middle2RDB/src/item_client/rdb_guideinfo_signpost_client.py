# -*- coding: cp936 -*-
'''
Created on 2012-3-6

@author: sunyifeng
'''

from base.rdb_ItemBase import ItemBase
from common import rdb_log


class rdb_guideinfo_signpost_client(ItemBase):
    '''
    classdocs
    '''


    def __init__(self):
        '''
        Constructor
        '''
        
        ItemBase.__init__(self, 'rdb_guideinfo_signpost_client'
                          , 'rdb_guideinfo_signpost'
                          , 'gid'
                          , 'rdb_guideinfo_signpost_client'
                          , 'guideinfo_id'
                          , False)
        
        
    def Do_CreateTable(self):
        
        if self.CreateTable2('rdb_guideinfo_signpost_client') == -1:
            return -1
        
        return 0
        
    def Do_CreatIndex(self):
        
        if self.CreateIndex2('rdb_guideinfo_signpost_client_in_link_id_node_id_idx') == -1:
            return -1
        if self.CreateIndex2('rdb_guideinfo_signpost_client_node_id_idx') == -1:
            return -1
        if self.CreateIndex2('rdb_guideinfo_signpost_client_node_id_t_idx') == -1:
            return -1
        
        return 0
        
        
    def Do(self):
       
        rdb_log.log(self.ItemName, 'Start make rdb_guideinfo_signpost_client', 'info')
            
        sqlcmd = """
        insert into rdb_guideinfo_signpost_client(
          in_link_id,
          in_link_id_t,
          node_id,
          node_id_t,
          out_link_id,
          out_link_id_t,
          sp_name,
          passlink_count,
          pattern_id,
          arrow_id,
          is_pattern) 
        (
            select rdb_split_tileid(in_link_id),
                   in_link_id_t,
                   rdb_split_tileid(node_id),
                   node_id_t,
                   rdb_split_tileid(out_link_id),
                   out_link_id_t,
                   sp_name, 
                   passlink_count,  
                   pattern_id,  
                   arrow_id,
                   is_pattern
            from rdb_guideinfo_signpost
            order by gid
        )
        """
        
        if self.pg.execute2(sqlcmd) == -1:
            rdb_log.log(self.ItemName, 'Insert record into rdb_guideinfo_signpost_client failed.', 'info') 
            return -1
            
        self.pg.commit2()
         
        rdb_log.log(self.ItemName, 'End Make GudieInfo rdb_guideinfo_signpost_client', 'info')
        
        return 0 
    
    def _DoContraints(self):
        '添加外键'
        return 0
        sqlcmd = """
                --ALTER TABLE rdb_guideinfo_signpost_client DROP CONSTRAINT if exists rdb_guideinfo_signpost_client_in_link_id_fkey;           
                --ALTER TABLE rdb_guideinfo_signpost_client
                -- ADD CONSTRAINT rdb_guideinfo_signpost_client_in_link_id_fkey FOREIGN KEY (in_link_id)
                --      REFERENCES rdb_link_client (link_id) MATCH FULL
                --      ON UPDATE NO ACTION ON DELETE NO ACTION;
                
                ALTER TABLE rdb_guideinfo_signpost_client DROP CONSTRAINT if exists rdb_guideinfo_signpost_client_pattern_id_fkey;
                ALTER TABLE rdb_guideinfo_signpost_client
                  ADD CONSTRAINT rdb_guideinfo_signpost_client_pattern_id_fkey FOREIGN KEY (pattern_id)
                      REFERENCES rdb_guideinfo_pic_blob_bytea (gid) MATCH FULL
                      ON UPDATE NO ACTION ON DELETE NO ACTION;
                      
                ALTER TABLE rdb_guideinfo_signpost_client DROP CONSTRAINT if exists rdb_guideinfo_signpost_client_arrow_id_fkey;
                ALTER TABLE rdb_guideinfo_signpost_client
                  ADD CONSTRAINT rdb_guideinfo_signpost_client_arrow_id_fkey FOREIGN KEY (arrow_id)
                      REFERENCES rdb_guideinfo_pic_blob_bytea (gid) MATCH FULL
                      ON UPDATE NO ACTION ON DELETE NO ACTION;
                
                --ALTER TABLE rdb_guideinfo_signpost_client DROP CONSTRAINT if exists rdb_guideinfo_signpost_client_node_id_fkey;
                --ALTER TABLE rdb_guideinfo_signpost_client
                --  ADD CONSTRAINT rdb_guideinfo_signpost_client_node_id_fkey FOREIGN KEY (node_id)
                --      REFERENCES rdb_node_client (node_id) MATCH FULL
                --      ON UPDATE NO ACTION ON DELETE NO ACTION;
                
                --ALTER TABLE rdb_guideinfo_signpost_client DROP CONSTRAINT if exists rdb_guideinfo_signpost_client_out_link_id_fkey;    
                --ALTER TABLE rdb_guideinfo_signpost_client
                --  ADD CONSTRAINT rdb_guideinfo_signpost_client_out_link_id_fkey FOREIGN KEY (out_link_id)
                --      REFERENCES rdb_link_client (link_id) MATCH FULL
                --      ON UPDATE NO ACTION ON DELETE NO ACTION;
                """
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
            return 0