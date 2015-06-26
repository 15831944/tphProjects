# -*- coding: cp936 -*-
'''
Created on 2011-12-6

@author: hongchenzai
'''
from base.rdb_ItemBase import ItemBase
from common import rdb_log

class rdb_node(ItemBase):
    '''
    node 
    '''

    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self, 'Node'
                          , 'node_tbl'
                          , 'gid'
                          , 'rdb_node'
                          , 'gid')
        
    def Do_CreateTable(self):
        return self.CreateTable2('rdb_node')
    
    def Do_CreateFunction(self):
        'Create Function'
        if self.CreateFunction2('rdb_convert_node_extendflag') == -1:
            return -1
        if self.CreateFunction2('rdb_cal_zm_path') == -1:
            return -1
        
        return 0
    
    def Do(self):
        sqlcmd = """
                INSERT INTO rdb_node(node_id, node_id_t, extend_flag, the_geom, the_geom_900913, link_num, branches)
                (
                    SELECT   rdb_tile_node.tile_node_id, tile_id
                        , rdb_convert_node_extendflag(light_flag = 1, (dr.id is not null), (spotguide.id is not null), 
                                                      (FALSE), (ln.id is not null), (sp.id is not null), 
                                                      (cn.id is not null), toll_flag = 1, bifurcation_flag = 1,
                                                      (1 = ALL(etc_only_flags)), (sp_uc.id is not null),
                                                      (caution.id is not null), (force.id is not null),
                                                      (boundary.id is not null), (hwy.id is not null),
                                                      stopsign_flag = 1, (naturalguide.id is not null),
													  (a.kind in ('101','102','103')),
													  (gbs.id is not null),
													  (hook.id is not null)
                                                      )
                        , a.the_geom
                        , st_transform(a.the_geom,3857)
                        , b.link_num
                        , b.branches
                      FROM  
                        rdb_tile_node 
                        LEFT JOIN (
                            SELECT  node_id, kind, light_flag, stopsign_flag, toll_flag, bifurcation_flag, the_geom
                              FROM node_tbl
                              where kind is null or kind not in ('100')
                            ) as a
                        ON rdb_tile_node.old_node_id = a.node_id
                        LEFT JOIN (
                            -- Get all connected links of a node
                            -- Clockwise
                            SELECT tile_node_id, count(tile_node_id) as link_num,
                                   array_agg(tile_link_id) as branches,
                                   array_agg(etc_only_flag) as etc_only_flags
                              from (
                                SELECT tile_node_id, tile_link_id, etc_only_flag, angle
                                  from (
                                    SELECT  tn.tile_node_id, tl_l.tile_link_id, tl_l.etc_only_flag,
                                        (case WHEN tn.old_node_id = tl_l.s_node THEN fazm ELSE tazm END) as angle
                                      FROM rdb_tile_node as tn, 
                                        (
                                        SELECT tl.tile_link_id 
                                           , temp_l.s_node
                                           , temp_l.e_node
                                           , temp_l.etc_only_flag
                                           , fazm 
                                           , tazm 
                                          FROM rdb_tile_link as tl
                                          LEFT JOIN link_tbl as temp_l
                                          ON tl.old_link_id = temp_l.link_id 
                                          where temp_l.link_id is not null and temp_l.display_class not in(20,21)                           
                                        ) as tl_l
                                       WHERE tn.old_node_id = tl_l.s_node or tn.old_node_id = tl_l.e_node
                                  ) as tn_2
                                  order by tile_node_id, angle desc   -- Clockwise
                              ) AS tn_3
                              group by tile_node_id
                            ) AS b
                        ON rdb_tile_node.tile_node_id = b.tile_node_id    
                        LEFT JOIN (
                            SELECT distinct(node_id) as id
                             FROM rdb_guideinfo_lane
                             ) AS ln
                        ON rdb_tile_node.tile_node_id = ln.id
                        LEFT JOIN (
                            SELECT distinct(node_id) as id
                             FROM rdb_guideinfo_signpost
                             ) AS sp
                        ON rdb_tile_node.tile_node_id = sp.id
                        LEFT JOIN (
                            SELECT distinct(node_id) as id
                            FROM rdb_guideinfo_crossname
                            ) as cn
                        on rdb_tile_node.tile_node_id = cn.id
                        LEFT JOIN (
                            SELECT distinct(node_id) as id
                            FROM rdb_guideinfo_towardname
                            ) as dr
                        on rdb_tile_node.tile_node_id = dr.id
                        LEFT JOIN (
                            SELECT distinct(node_id) as id
                            FROM rdb_guideinfo_spotguidepoint
                            ) as spotguide
                        on rdb_tile_node.tile_node_id = spotguide.id
                        LEFT JOIN (
                            SELECT distinct(node_id) as id
                            FROM rdb_guideinfo_natural_guidence
                            ) as naturalguide
                        on rdb_tile_node.tile_node_id = naturalguide.id
                        LEFT JOIN (
                            SELECT distinct(node_id) as id
                             FROM rdb_guideinfo_signpost_uc
                             ) AS sp_uc
                        ON rdb_tile_node.tile_node_id = sp_uc.id
                        LEFT JOIN (
                            SELECT distinct(node_id) as id
                            FROM rdb_guideinfo_caution
                            ) as caution
                        on rdb_tile_node.tile_node_id = caution.id
                        LEFT JOIN (
                            SELECT distinct(node_id) as id
                            FROM rdb_guideinfo_forceguide
                            ) as force
                        on rdb_tile_node.tile_node_id = force.id
                        LEFT JOIN (
                            SELECT distinct(a.tile_node_id) as id
                            FROM rdb_tile_node as a
                            inner join link_tbl as b
                            on a.old_node_id in (b.s_node,b.e_node) and a.tile_id  != ((14 << 28 ) | b.tile_id)
                            ) as boundary
                        on rdb_tile_node.tile_node_id = boundary.id
                        left join (
                            select distinct(node_id) as id
                              from rdb_highway_facility_data
                        ) as hwy
                        on rdb_tile_node.tile_node_id = hwy.id
                        left join(
                            select distinct(node_id) as id
                                from rdb_guideinfo_building_structure
                        ) as gbs
                        on rdb_tile_node.tile_node_id = gbs.id
                        left join(
                            select distinct(node_id) as id
                                from rdb_guideinfo_hook_turn
                        ) as hook
                        on rdb_tile_node.tile_node_id = hook.id                        
                        where a.node_id is not null and b.link_num is not null
                );
                """
        
        rdb_log.log(self.ItemName, 'Now it is inserting rdb_node...', 'info')
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
        
        #park Internal node    
        sqlcmd = """
                INSERT INTO rdb_node(node_id, node_id_t, extend_flag, the_geom, the_geom_900913, link_num, branches)
                (
                    SELECT   rdb_tile_node.tile_node_id, tile_id
                        , (1<<1)::smallint as extend_flag
                        , a.the_geom
                        , st_transform(a.the_geom,3857)
                        , b.link_num
                        , b.branches
                      FROM  
                        rdb_tile_node 
                        LEFT JOIN (
                            SELECT  node_id, kind,light_flag, toll_flag, bifurcation_flag, the_geom
                              FROM node_tbl
                              where kind in ('100','101','102','103')
                            ) as a
                        ON rdb_tile_node.old_node_id = a.node_id
                        LEFT JOIN (
                            -- Get all connected links of a node
                            -- Clockwise
                            SELECT tile_node_id, count(tile_node_id) as link_num,
                                   array_agg(tile_link_id) as branches,
                                   array_agg(etc_only_flag) as etc_only_flags
                              from (
                                SELECT tile_node_id, tile_link_id, etc_only_flag, angle
                                  from (
                                    SELECT  tn.tile_node_id, tl_l.tile_link_id, tl_l.etc_only_flag,
                                        (case WHEN tn.old_node_id = tl_l.s_node THEN fazm ELSE tazm END) as angle
                                      FROM rdb_tile_node as tn, 
                                        (
                                        SELECT tl.tile_link_id 
                                           , temp_l.s_node
                                           , temp_l.e_node
                                           , temp_l.etc_only_flag
                                           , fazm 
                                           , tazm 
                                          FROM rdb_tile_link as tl
                                          JOIN link_tbl as temp_l
                                          ON tl.old_link_id = temp_l.link_id 
                                          where temp_l.link_id is not null and  temp_l.display_class in (20,21)                                
                                        ) as tl_l
                                       WHERE tn.old_node_id = tl_l.s_node or tn.old_node_id = tl_l.e_node
                                  ) as tn_2
                                  order by tile_node_id, angle desc   -- Clockwise
                              ) AS tn_3
                              group by tile_node_id
                            ) AS b
                        ON rdb_tile_node.tile_node_id = b.tile_node_id
                        left join rdb_node as c
                        on rdb_tile_node.tile_node_id = c.node_id
                        
                        where a.node_id is not null and c.node_id is null
                );
                """
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
                  
        return 0
    
    def Do_CreatIndex(self):
        '创建相关表索引.'
        self.CreateIndex2('rdb_node_node_id_idx')
        self.CreateIndex2('rdb_node_the_geom_idx')
        self.CreateIndex2('rdb_node_node_id_t_idx')
        return 0
    
    def _DoCheckValues(self):
        sqlcmd = """
                -- rdb_node
                ALTER TABLE rdb_node DROP CONSTRAINT if exists check_num;
                ALTER TABLE rdb_node
                  ADD CONSTRAINT check_num CHECK (link_num > 0 and link_num = array_upper(branches, 1));
                """
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
    