# -*- coding: cp936 -*-
'''
Created on 2012-8-17

@author: liuxinxing
'''
import common
import base

class comp_link_merge(base.component_base.comp_base):
    '''
    Link Merge
    '''

    def __init__(self):
        '''
        Constructor
        '''
        base.component_base.comp_base.__init__(self, 'Link_Merge')        
        
    def _Do(self):
        self.__prepareSuspectLinkNode()
        self.__searchLinkrow()
        self.__mergeLinkrow()
        self.__updateReletedTables()
        return 0
    
    def __prepareSuspectLinkNode(self):
        self.log.info('Prepare suspect of merging link and node...')
        
        self.CreateTable2('temp_merge_node_keep')
        self.CreateTable2('temp_merge_link_keep')
        
        self.CreateIndex2('temp_merge_node_keep_node_id_idx')
        self.CreateIndex2('temp_merge_link_keep_link_id_idx')
        
        self.CreateTable2('temp_merge_node_suspect')
        self.CreateIndex2('temp_merge_node_suspect_node_id_idx')
        
        self.CreateTable2('temp_merge_link_suspect')
        self.CreateIndex2('temp_merge_link_suspect_link_id_idx')
        self.CreateIndex2('temp_merge_link_suspect_s_node_idx')
        self.CreateIndex2('temp_merge_link_suspect_e_node_idx')
        self.CreateIndex2('temp_merge_link_suspect_tile_id_length_idx')
        
        self.pg.commit2()
        
        self.log.info('Prepare suspect of merging link and node end.')
    
    def __searchLinkrow(self):
        self.log.info('Search linkrow for merging link...')
        
        self.CreateTable2('temp_merge_link_walked')
        self.CreateTable2('temp_merge_linkrow')
        
        self.CreateIndex2('temp_merge_linkrow_s_node_idx')
        self.CreateIndex2('temp_merge_linkrow_e_node_idx')
        self.CreateIndex2('temp_merge_linkrow_s_node_e_node_idx')
        
        self.CreateFunction2('mid_search_linkrow_in_one_direction')
        self.CreateFunction2('mid_search_linkrow')
        
        self.pg.callproc('mid_search_linkrow')
        self.pg.commit2()

        self.log.info('Search linkrow for merging link end.')
    
    def __mergeLinkrow(self):
        self.log.info('Merge link...')
        self.CreateFunction2('mid_cal_zm')
        
        #
        self.CreateTable2('temp_merge_link_mapping')
        self.CreateIndex2('temp_merge_link_mapping_link_id_idx')
        self.CreateIndex2('temp_merge_link_mapping_merge_link_id_idx')
        
        self.CreateTable2('temp_merge_newlink_maybe_circle')
        self.CreateIndex2('temp_merge_newlink_maybe_circle_link_id_idx')
        self.CreateTable2('temp_merge_newlink')
        self.CreateIndex2('temp_merge_newlink_link_id_idx')
        self.CreateTable2('temp_merge_newnode')
        self.CreateIndex2('temp_merge_newnode_node_id_idx')
        
        self.CreateTable2('link_tbl_bak_merge')
        self.CreateTable2('node_tbl_bak_merge')
        self.CreateIndex2('link_tbl_bak_merge_link_id_idx')
        self.CreateIndex2('node_tbl_bak_merge_node_id_idx')
        
        # make link_tbl
        self.CreateTable2('link_tbl')
        sqlcmd = """
                    insert into link_tbl
                    (
                    link_id, tile_id, s_node, e_node, link_type, road_type, toll, speed_class, length, function_class,
                    lane_dir, lane_num_s2e, lane_num_e2s, elevated, structure, tunnel, rail_cross, paved, uturn, 
                    speed_limit_s2e, speed_limit_e2s, speed_source_s2e, speed_source_e2s, width_s2e, width_e2s,
                    one_way_code, one_way_condition, pass_code, pass_code_condition, road_name, road_number, 
                    name_type, ownership, car_only, slope_code, slope_angle, disobey_flag, up_down_distinguish, 
                    access, extend_flag, etc_only_flag, display_class, fazm, tazm, feature_string, feature_key, the_geom
                    )
                    (
                        select  a.link_id, tile_id, a.s_node, a.e_node, link_type, road_type, toll, speed_class, length, function_class,
                                lane_dir, lane_num_s2e, lane_num_e2s, elevated, structure, tunnel, rail_cross, paved, uturn, 
                                speed_limit_s2e, speed_limit_e2s, speed_source_s2e, speed_source_e2s, width_s2e, width_e2s,
                                a.one_way_code, one_way_condition, pass_code, pass_code_condition, road_name, road_number, 
                                name_type, ownership, car_only, slope_code, slope_angle, disobey_flag, up_down_distinguish, 
                                access, extend_flag, etc_only_flag, display_class, fazm, tazm, feature_string, feature_key, the_geom
                        from link_tbl_bak_merge as a
                        left join temp_merge_link_mapping as b
                        on a.link_id = b.merge_link_id
                        where b.link_id is null
                    );
                    
                    insert into link_tbl
                    (
                    link_id, tile_id, s_node, e_node, link_type, road_type, toll, speed_class, length, function_class,
                    lane_dir, lane_num_s2e, lane_num_e2s, elevated, structure, tunnel, rail_cross, paved, uturn, 
                    speed_limit_s2e, speed_limit_e2s, speed_source_s2e, speed_source_e2s, width_s2e, width_e2s,
                    one_way_code, one_way_condition, pass_code, pass_code_condition, road_name, road_number, 
                    name_type, ownership, car_only, slope_code, slope_angle, disobey_flag, up_down_distinguish, 
                    access, extend_flag, etc_only_flag, display_class, fazm, tazm, feature_string, feature_key, the_geom
                    )
                    (
                        select  b.link_id, tile_id, b.s_node, b.e_node, link_type, road_type, toll, speed_class, b.length, function_class,
                                lane_dir, lane_num_s2e, lane_num_e2s, elevated, structure, tunnel, rail_cross, paved, uturn, 
                                speed_limit_s2e, speed_limit_e2s, speed_source_s2e, speed_source_e2s, width_s2e, width_e2s,
                                b.one_way_code, one_way_condition, pass_code, pass_code_condition, road_name, road_number, 
                                name_type, ownership, car_only, slope_code, slope_angle, disobey_flag, up_down_distinguish, 
                                access, extend_flag, etc_only_flag, display_class, b.fazm, b.tazm, null, null, b.the_geom
                        from link_tbl_bak_merge as a
                        left join temp_merge_newlink as b
                        on a.link_id = b.link_id
                        where b.link_id is not null
                    );
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.CreateIndex2('link_tbl_link_id_idx')
        self.CreateIndex2('link_tbl_s_node_idx')
        self.CreateIndex2('link_tbl_e_node_idx')
        self.CreateIndex2('link_tbl_the_geom_idx')
        
        # make node_tbl
        self.CreateTable2('node_tbl')
        sqlcmd = """
                    insert into node_tbl
                    (
                    node_id, tile_id, kind, light_flag, toll_flag, org_boundary_flag, tile_boundary_flag, 
                    mainnodeid, node_lid, node_name, feature_string, feature_key, the_geom
                    )
                    (
                        select  distinct
                                a.node_id, a.tile_id, a.kind, a.light_flag, a.toll_flag, 
                                a.org_boundary_flag, a.tile_boundary_flag, a.mainnodeid, 
                                (case when c.node_id is not null then c.node_lid else a.node_lid end) as node_lid, 
                                a.node_name, a.feature_string, a.feature_key, a.the_geom
                        from node_tbl_bak_merge as a
                        left join link_tbl as b
                        on a.node_id in (b.s_node, b.e_node)
                        left join temp_merge_newnode as c
                        on a.node_id = c.node_id
                        where b.link_id is not null
                    );
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.CreateIndex2('node_tbl_node_id_idx')
        self.CreateIndex2('node_tbl_the_geom_idx')
        
        self.log.info('Merge link end.')
    
    def __updateReletedTables(self):
        self.log.info('Update related tables...')
        
        # update guide information
        self.log.info('Update guide information start ...')
        
        self.CreateFunction2('mid_construct_substitude_link_array')
        
        sqlcmd = """
        drop table if exists [the_guide_tbl]_bak_merge;
        
        select * into [the_guide_tbl]_bak_merge from [the_guide_tbl];
        
        drop table if exists temp_[the_guide_tbl]_update_prepare_tbl;
        
        select g.gid, substitude_links_array[1] as in_link_id, substitude_links_array[new_link_cnt] as out_link_id, 
         case when new_link_cnt > 2 then array_to_string(substitude_links_array[2:new_link_cnt-1], '|') else null end as passlid,
         (new_link_cnt - 2) as passlink_cnt into temp_[the_guide_tbl]_update_prepare_tbl
        from 
        (
            select f.gid, substitude_links_array, array_upper(substitude_links_array, 1) as new_link_cnt 
            from
            (
                select gid , mid_construct_substitude_link_array(array_agg(link_id), array_agg(substitude_linkid)) as substitude_links_array 
                from
                (
                    select c.gid ,c.link_idx, c.link_id, 
                    (case when d.link_id is not null then d.link_id else c.link_id end) as substitude_linkid 
                    from
                    (
                        select gid, link_idx, all_links[link_idx] as link_id 
                        from
                        (
                            select gid, generate_series(1, all_links_cnt) as link_idx, all_links 
                            from
                            (
                                -- construct passlinks
                                select  gid,  
                                    case when passlink_cnt = 0 then ARRAY[inlinkid, outlinkid]
                                    else ARRAY[inlinkid] || cast(regexp_split_to_array(passlid, E'\\\\|+') as bigint[]) || ARRAY[outlinkid]
                                    end as all_links, 
                                    (passlink_cnt + 2 ) as all_links_cnt
                                from [the_guide_tbl]
                            ) as a
                        ) as b
                    ) as c
                    left join temp_merge_link_mapping as d -- maybe some link info do not disappear in this table
                    on c.link_id = d.merge_link_id
                    order by c.gid, c.link_idx
                ) as e
                group by e.gid
            ) as f
        ) as g;

        CREATE INDEX temp_[the_guide_tbl]_update_prepare_tbl_gid_idx
            ON temp_[the_guide_tbl]_update_prepare_tbl
            USING btree
            (gid);
        
        update [the_guide_tbl]
        set inlinkid = b.in_link_id,
        outlinkid = b.out_link_id,
        passlid = b.passlid,
        passlink_cnt = b.passlink_cnt
        from temp_[the_guide_tbl]_update_prepare_tbl as b
        where [the_guide_tbl].gid = b.gid;
        """
        
        guide_table_list = [
                            'spotguide_tbl',
                            'signpost_tbl',
                            'signpost_uc_tbl',
                            'lane_tbl',
                            'force_guide_tbl',
                            'towardname_tbl',
                            'caution_tbl',
                            'crossname_tbl',
                            ]
        
        for the_guide_table in guide_table_list:
            self.log.info('updating %s...' % the_guide_table)
            the_sql_cmd = sqlcmd.replace('[the_guide_tbl]', the_guide_table)
            self.pg.execute2(the_sql_cmd)
            self.pg.commit2()
        
        self.log.info('Update guide information end ...')
        
        self.log.info('Update related tables end.')
        