# -*- coding: cp936 -*-
'''
Created on 2011-12-21

@author: liuxinxing
'''
from base.rdb_ItemBase import ItemBase
from common import rdb_log

class rdb_cond_speed_client(ItemBase):
    '''
    classdocs
    '''
    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self, 'Regulation Speed Client'
                          , 'rdb_cond_speed'
                          , 'gid'
                          , 'rdb_cond_speed_client'
                          , 'link_id'
                          , False)
        
    def Do(self):
                
        if self.CreateTable2('rdb_cond_speed_client') == -1:
            rdb_log.log(self.ItemName, 'Fail to create talbe rdb_cond_speed_client.', 'error')
            return -1
    
        # Create Function 
        if self.CreateFunction2('rdb_cond_speed_merge_attribute') == -1:
            return -1
                
        if self.CreateFunction2('rdb_split_tileid') == -1:
            return -1
        
        # 求最小和最大id
        sqlcmd = """
            SELECT min(gid), max(gid)
              FROM link_tbl;
            """
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchone2()
        min_id  = row[0]
        max_id  = row[1]
        
        sqlcmd = """
                insert into rdb_cond_speed_client (link_id, link_id_t, pos_multi_speed_attr, neg_multi_speed_attr)
                (
                    select rdb_split_tileid(link_id), link_id_t, 
                           rdb_cond_speed_merge_attribute(pos_cond_speed,pos_cond_speed_source,unit),
                           rdb_cond_speed_merge_attribute(neg_cond_speed,neg_cond_speed_source,unit)
                    from rdb_cond_speed
                    WHERE gid >= %s and gid <= %s
                );
                """
                
        self._call_child_thread(min_id, max_id, sqlcmd, 4, 1500000)
#        if self.pg.execute2(sqlcmd) == -1:
#            rdb_log.log(self.ItemName, 'Insert record into cond speed client failed.', 'info') 
#            self.pg.close2()
#            return -1
#            
#        self.pg.commit()
        
        self.CreateIndex2('rdb_cond_speed_client_link_id_t_idx')
        self.CreateIndex2('rdb_cond_speed_client_link_id_idx')
        
        return 0
    
    def _DoContraints(self):
        '添加外键'
        sqlcmd = """
                ALTER TABLE rdb_cond_speed_client DROP CONSTRAINT if exists rdb_cond_speed_client_link_id_fkey;         
                ALTER TABLE rdb_cond_speed_client
                  ADD CONSTRAINT rdb_cond_speed_client_link_id_fkey FOREIGN KEY (link_id)
                      REFERENCES rdb_link_client (link_id) MATCH FULL
                      ON UPDATE NO ACTION ON DELETE NO ACTION;
                """
        #if self.pg.execute2(sqlcmd) == -1:
        #    return -1
        #else:
        #    self.pg.commit2()
        return 0
