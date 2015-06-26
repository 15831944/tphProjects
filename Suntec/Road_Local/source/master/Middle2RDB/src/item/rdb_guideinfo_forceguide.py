# -*- coding: cp936 -*-
'''
Created on 2012-2-7

@author: hongchenzai
'''
from base.rdb_ItemBase import ItemBase, ITEM_EXTEND_FLAG_IDX
from common import rdb_log

class rdb_guideinfo_forceguide(ItemBase):
    '''
    classdocs
    '''
    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self, 'Force Guide'
                          , 'force_guide_tbl'
                          , 'gid'
                          , 'rdb_guideinfo_forceguide'
                          , 'guideinfo_id'
                          , True
                          , ITEM_EXTEND_FLAG_IDX.get('FORCE_GUIDE'))
    
    def Do_CreateTable(self):
        self.CreateTable2('rdb_guideinfo_forceguide')
        return 0

    def Do_CreatIndex(self):
        '创建相关表索引.'
        self.CreateIndex2('rdb_guideinfo_forceguide_in_link_id_node_id_idx')
        self.CreateIndex2('rdb_guideinfo_forceguide_node_id_idx')    
        self.CreateIndex2('rdb_guideinfo_forceguide_node_id_t_idx')    
        return 0
    
    def Do(self):
        self.CreateFunction2('rdb_get_friendly_guidetype')
        self.CreateFunction2('rdb_get_friendly_guidetype_common_road')
        
        sqlcmd = """
            INSERT INTO rdb_guideinfo_forceguide(
                        guideinfo_id, in_link_id, in_link_id_t, node_id, node_id_t, out_link_id, 
                        out_link_id_t, passlink_count, guide_type, position_type)
                SELECT a.gid, b.tile_link_id, b.tile_id, c.tile_node_id, c.tile_id, 
                        d.tile_link_id, d.tile_id, a.passlink_cnt, 
                        case when e.gid is null and f.link_id is not null and a.guide_type = 1 then 0
                             when e.gid is null and (f.link_id is null or 
                                     (f.link_id is not null and a.guide_type <> 1)
                                  ) then a.guide_type 
                             else e.friendly_guidetype 
                        end as guide_type,
                        a.position_type
                    FROM force_guide_tbl as a
                    left join (
                        select link_id,toll from link_tbl where toll <> 1
                    ) f
                    on a.inlinkid = f.link_id
                    inner join rdb_tile_link as b
                    on a.inlinkid = b.old_link_id
                    inner join rdb_tile_node as c
                    on a.nodeid = c.old_node_id
                    inner join rdb_tile_link as d
                    on a.outlinkid = d.old_link_id                    
                    left join
                    (
                        select  gid, friendly_guidetype
                        from
                        (
                            select  a.gid, 
                                    (
                                    case 
                                    when c.toll = 1 then rdb_get_friendly_guidetype(b.laneinfo, a.guide_type,b.arrowinfo)
                                    else rdb_get_friendly_guidetype_common_road(b.laneinfo, a.guide_type)
                                    end
                                    )as friendly_guidetype
                            from force_guide_tbl as a
                            left join lane_tbl as b
                            on (a.nodeid = b.nodeid) 
                               and (a.inlinkid = b.inlinkid)
                               and (a.outlinkid = b.outlinkid) 
                               and not (a.passlid is distinct from b.passlid)
                            left join link_tbl as c
                            on a.inlinkid = c.link_id
                            where b.gid is not null
                        )as t
                        where friendly_guidetype is not null
                    )as e
                    on a.gid = e.gid
                    order by a.gid
        """
        
        rdb_log.log(self.ItemName, 'Start making rdb_guideinfo_forceguide', 'info') 
        
        if self.pg.execute2(sqlcmd) == -1:
            rdb_log.log(self.ItemName, 'making rdb_guideinfo_forceguide data failed.', 'error')
            return -1
        
        self.pg.commit2()
    
        rdb_log.log(self.ItemName, 'succeeded to make rdb_guideinfo_forceguide.', 'info')
        
        
