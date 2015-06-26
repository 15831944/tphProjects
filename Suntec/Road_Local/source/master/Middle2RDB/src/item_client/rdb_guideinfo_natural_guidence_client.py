# -*- coding: cp936 -*-
'''
Created on 2012-3-6

@author: sunyifeng
'''

from base.rdb_ItemBase import ItemBase
from common import rdb_log

class rdb_guideinfo_natural_guidence_client(ItemBase):
    '''
    classdocs
    '''


    def __init__(self):
        '''
        Constructor
        '''
        
        ItemBase.__init__(self, 'NaturalGuidence_client'
                          , 'rdb_guideinfo_natural_guidence'
                          , 'gid'
                          , 'rdb_guideinfo_natural_guidence_client'
                          , 'gid'
                          , False)
        
    def Do_CreateTable(self):
        
        if self.CreateTable2('rdb_guideinfo_natural_guidence_client') == -1:
            return -1
        
        return 0
        
    def Do_CreatIndex(self):
        
        if self.CreateIndex2('rdb_guideinfo_natural_guidence_client_in_link_id_node_id_idx') == -1:
            return -1
        if self.CreateIndex2('rdb_guideinfo_natural_guidence_client_node_id_idx') == -1:
            return -1
        if self.CreateIndex2('rdb_guideinfo_natural_guidence_client_node_id_t_idx') == -1:
            return -1
        
        return 0
        
    def Do(self):
        sqlcmd = """
                insert into rdb_guideinfo_natural_guidence_client
                       ( 
                           gid
                         , in_link_id
                         , in_link_id_t
                         , node_id
                         , node_id_t
                         , out_link_id
                         , out_link_id_t
                         , passlink_count
                         , feat_position
                         , feat_importance
                         , preposition_code
                         , feat_name
                         , condition_id
                        ) 
                (
                    select  gid ,
                            rdb_split_tileid(in_link_id),
                            in_link_id_t,
                            rdb_split_tileid(node_id),
                            node_id_t,
                            rdb_split_tileid(out_link_id),
                            out_link_id_t, 
                            passlink_count,  
                            feat_position,  
                            feat_importance,
                            preposition_code,  
                            feat_name,
                            condition_id
                    from rdb_guideinfo_natural_guidence
                    order by gid
                )
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()             
      
        return 0
    
    