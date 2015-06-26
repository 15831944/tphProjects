# -*- coding: cp936 -*-
'''
Created on 2012-3-6

@author: sunyifeng
'''

from base.rdb_ItemBase import ItemBase
from common import rdb_log

class rdb_guideinfo_spotguide_client(ItemBase):
    '''
    classdocs
    '''


    def __init__(self):
        '''
        Constructor
        '''
        
        ItemBase.__init__(self, 'SpotGuide_client'
                          , 'rdb_guideinfo_spotguidepoint'
                          , 'gid'
                          , 'rdb_guideinfo_spotguidepoint_client'
                          , 'gid'
                          , False)
        
    def Do_CreateTable(self):
        
        if self.CreateTable2('rdb_guideinfo_spotguidepoint_client') == -1:
            return -1
        
        return 0
        
    def Do_CreatIndex(self):
        
        if self.CreateIndex2('rdb_guideinfo_spotguidepoint_client_in_link_id_node_id_idx') == -1:
            return -1
        if self.CreateIndex2('rdb_guideinfo_spotguidepoint_client_node_id_idx') == -1:
            return -1
        if self.CreateIndex2('rdb_guideinfo_spotguidepoint_client_node_id_t_idx') == -1:
            return -1
        
        return 0
        
        
    def Do(self):
        
        #rdb_log.log(self.ItemName, 'Start to make SpotGuide_client.', 'info') 
        
        sqlcmd = """
        insert into rdb_guideinfo_spotguidepoint_client( 
          gid,
          in_link_id,
          in_link_id_t,
          node_id,
          node_id_t,
          out_link_id,
          out_link_id_t,
          "type" ,
          passlink_count,
          pattern_id,
          arrow_id,
          point_list) 
        (
            select gid ,rdb_split_tileid(in_link_id),in_link_id_t,
                  rdb_split_tileid(node_id),node_id_t,rdb_split_tileid(out_link_id),
                  out_link_id_t, "type" , passlink_count,  pattern_id,  arrow_id,  point_list
            from rdb_guideinfo_spotguidepoint
            order by gid
        )
        """
        
        if self.pg.execute2(sqlcmd) == -1:
            rdb_log.log(self.ItemName, 'Insert record into spot guidepoint failed.', 'info') 
            return -1
            
        self.pg.commit2()             
      
        #rdb_log.log(self.ItemName, 'End Make SpotGuide_client.', 'info') 
        return 0
    
    