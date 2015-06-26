# -*- coding: cp936 -*-
'''
Created on 2012-3-7

@author: sunyifeng
'''
from base.rdb_ItemBase import ItemBase
from common import rdb_log

class rdb_guideinfo_lane_client(ItemBase):
    '''
    classdocs
    '''


    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self, 'rdb_guideinfo_lane_client'
                          , 'rdb_guideinfo_lane'
                          , 'guideinfo_id'
                          , 'rdb_guideinfo_lane_client'
                          , 'guideinfo_id'
                          , False)
        
        
    def Do_CreateTable(self):
        
        if self.CreateTable2('rdb_guideinfo_lane_client') == -1:
            return -1
        
        return 0
    
    def Do_CreateFunction(self):
        if self.CreateFunction2('rdb_cnt_lane_num_client') == -1:
            return -1
        if self.CreateFunction2('rdb_cnt_lane_arrow_client') == -1:
            return -1
        
        return 0
        
    def Do_CreatIndex(self):
        
        if self.CreateIndex2('rdb_guideinfo_lane_client_node_id_idx') == -1:
            return -1
        if self.CreateIndex2('rdb_guideinfo_lane_client_in_link_id_node_id_idx') == -1:
            return -1
        if self.CreateIndex2('rdb_guideinfo_lane_client_node_id_t_idx') == -1:
            return -1
        
        return 0
        
    def Do(self):
        
        sqlcmd = """
                INSERT INTO rdb_guideinfo_lane_client(
                            guideinfo_id, 
                            in_link_id, in_link_id_t,
                            node_id, node_id_t, 
                            out_link_id, out_link_id_t,
                            lane_num, 
                            lane_info, 
                            arrow_info,
                            passlink_count
                            )
                (
                SELECT guideinfo_id, 
                    rdb_split_tileid(in_link_id), in_link_id_t,
                    rdb_split_tileid(node_id), node_id_t, 
                    rdb_split_tileid(out_link_id), out_link_id_t,
                    rdb_cnt_lane_num_client(lane_num, lane_num_l, lane_num_r),
                    lane_info, 
                    arrow_info,
                    passlink_count
                  FROM rdb_guideinfo_lane
                  order by guideinfo_id
                )
                """
        rdb_log.log(self.ItemName, 'Inserting records to table rdb_guideinfo_lane_client...', 'info')
        if self.pg.execute2(sqlcmd) == -1:
            rdb_log.log(self.ItemName, 'Insert records to table rdb_guideinfo_lane_client failed.', 'info')
            return -1
        else:
            self.pg.commit2()
            rdb_log.log(self.ItemName, 'Inserting records to table rdb_guideinfo_lane_client succeeded.', 'info')
        
        
        rdb_log.log(self.ItemName, 'End Make rdb_guideinfo_lane_client.', 'info')
        return 0
    
    def _DoContraints(self):
        '添加外键'
        sqlcmd = """
                ALTER TABLE rdb_guideinfo_lane_client DROP CONSTRAINT if exists rdb_guideinfo_lane_client_in_link_id_fkey;  
                ALTER TABLE rdb_guideinfo_lane_client
                  ADD CONSTRAINT rdb_guideinfo_lane_client_in_link_id_fkey FOREIGN KEY (in_link_id)
                      REFERENCES rdb_link_client (link_id) MATCH FULL
                      ON UPDATE NO ACTION ON DELETE NO ACTION;
                
                ALTER TABLE rdb_guideinfo_lane_client DROP CONSTRAINT if exists rdb_guideinfo_lane_client_node_id_fkey;
                ALTER TABLE rdb_guideinfo_lane_client
                  ADD CONSTRAINT rdb_guideinfo_lane_client_node_id_fkey FOREIGN KEY (node_id)
                      REFERENCES rdb_node_client (node_id) MATCH FULL
                      ON UPDATE NO ACTION ON DELETE NO ACTION;
                
                ALTER TABLE rdb_guideinfo_lane_client DROP CONSTRAINT if exists rdb_guideinfo_lane_client_out_link_id_fkey;
                ALTER TABLE rdb_guideinfo_lane_client
                  ADD CONSTRAINT rdb_guideinfo_lane_client_out_link_id_fkey FOREIGN KEY (out_link_id)
                      REFERENCES rdb_link_client (link_id) MATCH FULL
                      ON UPDATE NO ACTION ON DELETE NO ACTION;
                """
        #if self.pg.execute2(sqlcmd) == -1:
        #    return -1
        #else:
        #    self.pg.commit2()
        return 0