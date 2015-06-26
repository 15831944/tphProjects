# -*- coding: cp936 -*-
'''
Created on 2012-3-6

@author: sunyifeng
'''

from base.rdb_ItemBase import ItemBase, ITEM_EXTEND_FLAG_IDX
from common import rdb_log


class rdb_guideinfo_natural_guidence(ItemBase):
    '''
    classdocs
    '''


    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self, 'NaturalGuidence'
                          , 'natural_guidence_tbl'
                          , 'gid'
                          , 'rdb_guideinfo_natural_guidence'
                          , 'gid'
                          , True)
        
    def Do(self):
        self.CreateTable2('rdb_guideinfo_condition')
        sqlcmd = """
                insert into rdb_guideinfo_condition
                       (condition_id, start_date, end_date, week_flag, start_time, end_time, exclude_date)
                select condition_id, start_date, end_date, week_flag, start_time, end_time, exclude_date
                from guidence_condition_tbl
                order by condition_id;
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('rdb_guideinfo_condition_condition_id_idx')
        
        self.CreateTable2('rdb_guideinfo_natural_guidence')
        sqlcmd = """
                  insert into rdb_guideinfo_natural_guidence
                         (  in_link_id
                           ,in_link_id_t
                           ,node_id
                           ,node_id_t
                           ,out_link_id
                           ,out_link_id_t
                           ,passlink_count
                           ,feat_position
                           ,feat_importance
                           ,preposition_code
                           ,feat_name
                           ,condition_id
                          )
                  SELECT  a.tile_link_id as in_link_id
                        , a.tile_id as in_tile_id
                        , b.tile_node_id as node_id
                        , b.tile_id as node_tile_id
                        , c.tile_link_id as out_link_id
                        , c.tile_id as out_tile_id
                        , s.passlink_cnt
                        , s.feat_position
                        , s.feat_importance
                        , s.preposition_code
                        , s.feat_name
                        , s.condition_id
                    FROM natural_guidence_tbl as s
                    LEFT JOIN rdb_tile_link as a
                    ON cast(inlinkid as bigint) = a.old_link_id
                    LEFT JOIN rdb_tile_node as b
                    ON cast(nodeid as bigint) = b.old_node_id
                    LEFT JOIN rdb_tile_link as c
                    ON cast(outlinkid as bigint) = c.old_link_id
                    order by s.gid;
                  """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.CreateIndex2('rdb_guideinfo_natural_guidence_in_link_id_node_id_idx')
        self.CreateIndex2('rdb_guideinfo_natural_guidence_node_id_idx')
        self.CreateIndex2('rdb_guideinfo_natural_guidence_node_id_t_idx')
        

