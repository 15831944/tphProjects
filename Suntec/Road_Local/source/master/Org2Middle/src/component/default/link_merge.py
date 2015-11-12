# -*- coding: cp936 -*-
'''
Created on 2012-8-17

@author: liuxinxing
'''
import component.component_base

class comp_link_merge(component.component_base.comp_base):
    '''
    Link Merge
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Link_Merge')        
        
    def _Do(self):
        self.__prepareSuspectLinkNode()
        self.__searchLinkrow()
        self.__mergeLinkrow()
        self.__updateLinkNode()
        self.__updateGuideTables()
        self.__updateRegulation()
        self.__update_park_merge()
        self.__update_height()
        self.__updateStopSign()
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
        
        sqlcmd = """
                drop table if exists temp_tile_list;
                create table temp_tile_list 
                as
                select  distinct tile_id, 
                        (tile_id >> 14) & 16383 as tx, tile_id & 16383 as ty, 14 as tz
                from temp_merge_link_suspect
                ;
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        sqlcmd = """select mid_search_linkrow(%s)"""
        self.pg.multi_execute_in_tile(sqlcmd, 'temp_tile_list', 4, self.log)

        self.log.info('Search linkrow for merging link end.')
    
    def __mergeLinkrow(self):
        self.log.info('Merge link...')
        self.CreateFunction2('mid_cal_zm')
        
        #
        self.log.info('generate temp_merge_link_mapping...')
        self.CreateTable2('temp_merge_link_mapping')
        self.CreateIndex2('temp_merge_link_mapping_link_id_idx')
        self.CreateIndex2('temp_merge_link_mapping_merge_link_id_idx')
        
        self.log.info('generate temp_merge_newlink_maybe_circle...')
        self.CreateTable2('temp_merge_newlink_maybe_circle')
        self.CreateIndex2('temp_merge_newlink_maybe_circle_link_id_idx')
        
        self.log.info('generate temp_merge_newlink...')
        self.CreateTable2('temp_merge_newlink')
        self.CreateIndex2('temp_merge_newlink_link_id_idx')
        self.CreateTable2('temp_merge_newnode')
        self.CreateIndex2('temp_merge_newnode_node_id_idx')
        
        self.log.info('Merge link end.')
    
    def __updateLinkNode(self):
        self.log.info('Updating link and node...')
        self.CreateTable2('link_tbl_bak_merge')
        self.CreateTable2('node_tbl_bak_merge')
        self.CreateIndex2('link_tbl_bak_merge_gid_idx')
        self.CreateIndex2('link_tbl_bak_merge_link_id_idx')
        self.CreateIndex2('node_tbl_bak_merge_gid_idx')
        self.CreateIndex2('node_tbl_bak_merge_node_id_idx')
        
        # update link_tbl
        self.log.info('update link...')
        self.CreateTable2('link_tbl')
        sqlcmd = """
                    insert into link_tbl
                    (
                    link_id, iso_country_code, tile_id, s_node, e_node, link_type, road_type, toll, speed_class, length, function_class,
                    lane_dir, lane_num_s2e, lane_num_e2s, elevated, structure, tunnel, rail_cross, paved, uturn, 
                    speed_limit_s2e, speed_limit_e2s, speed_source_s2e, speed_source_e2s, width_s2e, width_e2s,
                    one_way_code, one_way_condition, pass_code, pass_code_condition, road_name, road_number, 
                    name_type, ownership, car_only, slope_code, slope_angle, disobey_flag, up_down_distinguish, 
                    access, extend_flag, etc_only_flag, bypass_flag, matching_flag, highcost_flag, ipd, urban, 
                    erp, rodizio, soi, display_class, fazm, tazm, feature_string, feature_key, the_geom
                    )
                    (
                        select  a.link_id, a.iso_country_code, tile_id, a.s_node, a.e_node, link_type, road_type, toll, speed_class, length, function_class,
                                lane_dir, lane_num_s2e, lane_num_e2s, elevated, structure, tunnel, rail_cross, paved, uturn, 
                                speed_limit_s2e, speed_limit_e2s, speed_source_s2e, speed_source_e2s, width_s2e, width_e2s,
                                a.one_way_code, one_way_condition, pass_code, pass_code_condition, road_name, road_number, 
                                name_type, ownership, car_only, slope_code, slope_angle, disobey_flag, up_down_distinguish, 
                                access, extend_flag, etc_only_flag, bypass_flag, matching_flag, highcost_flag, ipd, urban, 
                                erp, rodizio, soi, display_class, fazm, tazm, feature_string, feature_key, the_geom
                        from (select * from link_tbl_bak_merge where gid >= %d and gid <= %d) as a
                        left join temp_merge_link_mapping as b
                        on a.link_id = b.merge_link_id
                        where b.link_id is null
                    );
                """
        (min_gid, max_gid) = self.pg.getMinMaxValue('link_tbl_bak_merge', 'gid')
        self.pg.multi_execute(sqlcmd, min_gid, max_gid, 4, 100000, self.log)
                
        sqlcmd = """
                    insert into link_tbl
                    (
                    link_id, iso_country_code, tile_id, s_node, e_node, link_type, road_type, toll, speed_class, length, function_class,
                    lane_dir, lane_num_s2e, lane_num_e2s, elevated, structure, tunnel, rail_cross, paved, uturn, 
                    speed_limit_s2e, speed_limit_e2s, speed_source_s2e, speed_source_e2s, width_s2e, width_e2s,
                    one_way_code, one_way_condition, pass_code, pass_code_condition, road_name, road_number, 
                    name_type, ownership, car_only, slope_code, slope_angle, disobey_flag, up_down_distinguish, 
                    access, extend_flag, etc_only_flag, bypass_flag, matching_flag, highcost_flag, ipd, urban, 
                    erp, rodizio, soi, display_class, fazm, tazm, feature_string, feature_key, the_geom
                    )
                    (
                        select  b.link_id, a.iso_country_code, a.tile_id, b.s_node, b.e_node, link_type, road_type, toll, speed_class, b.length, function_class,
                                lane_dir, lane_num_s2e, lane_num_e2s, elevated, structure, tunnel, rail_cross, paved, uturn, 
                                speed_limit_s2e, speed_limit_e2s, speed_source_s2e, speed_source_e2s, width_s2e, width_e2s,
                                b.one_way_code, one_way_condition, pass_code, pass_code_condition, road_name, road_number, 
                                name_type, ownership, car_only, slope_code, slope_angle, disobey_flag, up_down_distinguish, 
                                access, extend_flag, etc_only_flag, bypass_flag, matching_flag, highcost_flag, ipd, urban, 
                                erp, rodizio, soi, display_class, b.fazm, b.tazm, 
                                (
                                case 
                                when c.feature_string <= d.feature_string then array_to_string(ARRAY[c.feature_string, d.feature_string], ',')
                                else array_to_string(ARRAY[d.feature_string, c.feature_string], ',')
                                end
                                ) as feature_string, 
                                (
                                case 
                                when c.feature_string <= d.feature_string then md5(array_to_string(ARRAY[c.feature_string, d.feature_string], ','))
                                else md5(array_to_string(ARRAY[d.feature_string, c.feature_string], ','))
                                end
                                ) as feature_key, 
                                b.the_geom
                        from temp_merge_newlink as b
                        inner join link_tbl_bak_merge as a
                        on a.link_id = b.link_id
                        inner join node_tbl as c
                        on b.s_node = c.node_id
                        inner join node_tbl as d
                        on b.e_node = d.node_id
                    );
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.CreateIndex2('link_tbl_link_id_idx')
        self.CreateIndex2('link_tbl_s_node_idx')
        self.CreateIndex2('link_tbl_e_node_idx')
        self.CreateIndex2('link_tbl_the_geom_idx')
        
        # make node_tbl
        self.log.info('update node...')
        self.CreateTable2('node_tbl')
        
        sqlcmd = """
                    drop table if exists temp_merge_node_delete;
                    create table temp_merge_node_delete
                    as
                    select a.node_id
                    from node_tbl_bak_merge as a
                    left join link_tbl as b
                    on a.node_id in (b.s_node, b.e_node)
                    where b.link_id is null;
                    create index temp_merge_node_delete_node_id_idx
                        on temp_merge_node_delete
                        using btree
                        (node_id);
                    analyze temp_merge_node_delete;
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
                    
        sqlcmd = """
                    insert into node_tbl
                    (
                    node_id, tile_id, kind, light_flag, stopsign_flag, toll_flag, bifurcation_flag, org_boundary_flag, tile_boundary_flag, 
                    mainnodeid, node_lid, node_name, feature_string, feature_key, z_level, the_geom
                    )
                    (
                        select  a.node_id, a.tile_id, a.kind, a.light_flag, a.stopsign_flag, a.toll_flag, a.bifurcation_flag, 
                                a.org_boundary_flag, a.tile_boundary_flag, a.mainnodeid, 
                                (case when c.node_id is not null then c.node_lid else a.node_lid end) as node_lid, 
                                a.node_name, a.feature_string, a.feature_key, a.z_level, a.the_geom
                        from (select * from node_tbl_bak_merge where gid >= %d and gid <= %d) as a
                        left join temp_merge_node_delete as b
                        on a.node_id = b.node_id
                        left join temp_merge_newnode as c
                        on a.node_id = c.node_id
                        where b.node_id is null
                    );
                """
        (min_gid, max_gid) = self.pg.getMinMaxValue('node_tbl_bak_merge', 'gid')
        self.pg.multi_execute(sqlcmd, min_gid, max_gid, 4, 100000, self.log)
        
        self.CreateIndex2('node_tbl_node_id_idx')
        self.CreateIndex2('node_tbl_the_geom_idx')
        
        self.log.info('Updating link and node end.')
        
        
        sqlcmd = """
                drop table if exists link_tbl_bak_for_linktype_test;
                create table link_tbl_bak_for_linktype_test
                as 
                select *
                from link_tbl;
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
    
    def __updateGuideTables(self):
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
                                    else ARRAY[inlinkid] || cast(string_to_array(passlid, '|') as bigint[]) || ARRAY[outlinkid]
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
                            'natural_guidence_tbl',
                            'hook_turn_tbl',
                            ]
        
        for the_guide_table in guide_table_list:
            self.log.info('updating %s...' % the_guide_table)
            the_sql_cmd = sqlcmd.replace('[the_guide_tbl]', the_guide_table)
            self.pg.execute2(the_sql_cmd)
            self.pg.commit2()
        
        # update guide table index
        sqlcmd = """
                DROP INDEX IF EXISTS [the_guide_tbl]_inlinkid_idx;
                CREATE INDEX [the_guide_tbl]_inlinkid_idx
                    ON [the_guide_tbl]
                    USING btree
                    (inlinkid);
                
                DROP INDEX IF EXISTS [the_guide_tbl]_nodeid_idx;
                CREATE INDEX [the_guide_tbl]_nodeid_idx
                    ON [the_guide_tbl]
                    USING btree
                    (nodeid);
                    
                DROP INDEX IF EXISTS [the_guide_tbl]_outlinkid_idx;
                CREATE INDEX [the_guide_tbl]_outlinkid_idx
                    ON [the_guide_tbl]
                    USING btree
                    (outlinkid);
            """
        for the_guide_table in guide_table_list:
            self.log.info('updating %s index...' % the_guide_table)
            the_sql_cmd = sqlcmd.replace('[the_guide_tbl]', the_guide_table)
            self.pg.execute2(the_sql_cmd)
            self.pg.commit2()
            
        self.log.info('Update guide information end ...')
        return 0
    
    def __updateRegulation(self):
        self.log.info('updating regulation...')
        
        #
        self.CreateTable2('regulation_relation_tbl_bak_merge')
        self.CreateIndex2('regulation_relation_tbl_bak_merge_regulation_id_idx')
        self.CreateTable2('regulation_item_tbl_bak_merge')
        self.CreateIndex2('regulation_item_tbl_bak_merge_regulation_id_idx')
        self.CreateIndex2('regulation_item_tbl_bak_merge_linkid_idx')
        
        #
        self.CreateTable2('temp_merge_update_linkrow_regulation')
        self.CreateIndex2('temp_merge_update_linkrow_regulation_regulation_id_idx')
        
        self.CreateTable2('regulation_relation_tbl')
        self.CreateFunction2('mid_construct_substitude_link_array')
        sqlcmd = """
                    insert into regulation_relation_tbl
                                (regulation_id, nodeid, inlinkid, outlinkid, condtype, is_seasonal, cond_id, gatetype, slope)
                    (
                        select a.regulation_id, a.nodeid, a.inlinkid, a.outlinkid, condtype, is_seasonal, cond_id, gatetype, slope
                        from regulation_relation_tbl_bak_merge as a
                        left join temp_merge_update_linkrow_regulation as b
                        on a.regulation_id = b.regulation_id
                        where b.regulation_id is null
                    );
                    
                    insert into regulation_relation_tbl
                                (regulation_id, nodeid, inlinkid, outlinkid, condtype, is_seasonal, cond_id, gatetype, slope)
                    (
                        select a.regulation_id, a.nodeid, b.inlinkid, b.outlinkid, condtype, is_seasonal, cond_id, gatetype, slope
                        from regulation_relation_tbl_bak_merge as a
                        inner join temp_merge_update_linkrow_regulation as b
                        on a.regulation_id = b.regulation_id
                    );
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.CreateTable2('regulation_item_tbl')
        sqlcmd = """
                    insert into regulation_item_tbl
                                (regulation_id, linkid, nodeid, seq_num)
                    (
                        select a.regulation_id, a.linkid, a.nodeid, a.seq_num
                        from regulation_item_tbl_bak_merge as a
                        left join temp_merge_update_linkrow_regulation as b
                        on a.regulation_id = b.regulation_id
                        where b.regulation_id is null
                        order by a.regulation_id, a.seq_num
                    );
                    
                    insert into regulation_item_tbl
                                (regulation_id, linkid, nodeid, seq_num)
                    (
                        select  regulation_id, 
                                (
                                case 
                                when seq_num = 1 then link_array[1]
                                when seq_num = 2 then null
                                else link_array[seq_num - 1]
                                end
                                )as linkid,
                                (
                                case
                                when seq_num = 2 then nodeid
                                else null
                                end
                                )as nodeid,
                                seq_num
                        from
                        (
                            select regulation_id, nodeid, link_array, generate_series(1,seq_count) as seq_num
                            from
                            (
                                select  a.regulation_id, b.nodeid, b.link_array, 
                                        (case when b.link_num = 1 then 1 else link_num + 1 end) as seq_count
                                from (select * from regulation_item_tbl_bak_merge where seq_num = 1) as a
                                inner join temp_merge_update_linkrow_regulation as b
                                on a.regulation_id = b.regulation_id
                            )as c
                        )as d
                        order by regulation_id, seq_num
                    );
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.log.info('updating regulation end.')
    
    def __update_park_merge(self):
        self.log.info('start update park for merge')
        #park link
        self.CreateTable2('park_link_tbl_bak_merge')
        self.CreateIndex2('park_link_tbl_bak_merge_park_link_id_idx')
        self.CreateTable2('park_link_tbl')
        
        sqlcmd = '''
                insert into park_link_tbl(park_link_id,park_link_oneway,park_s_node_id,park_e_node_id,
                                    park_link_length,park_link_type,park_link_connect_type,park_floor,
                                    park_link_lean,park_link_toll,park_link_add1,park_link_add2,
                                    park_region_id,the_geom)
                select distinct link_id, one_way_code, s_node, e_node, length,
                    park_link_type, park_link_connect_type, park_floor, park_link_lean, park_link_toll, 
                    park_link_add1, park_link_add2, park_region_id,a.the_geom
                from link_tbl as a
                left join 
                park_link_tbl_bak_merge as b
                on a.link_id = b.park_link_id
                where display_class in(30,31);
                '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
       
        #park node
        self.CreateTable2('park_node_tbl_bak_merge')
        self.CreateIndex2('park_node_tbl_bak_merge_park_node_id_idx')
        self.CreateTable2('park_node_tbl')
        
        sqlcmd = '''
                    insert into park_node_tbl(park_node_id, base_node_id, park_node_type
                                    , node_name, park_region_id, the_geom)
                    select  distinct
                            park_node_id, base_node_id, park_node_type, node_name, a.park_region_id, a.the_geom
                    from park_node_tbl_bak_merge as a
                    left join
                    park_link_tbl as b
                    on a.park_node_id in (b.park_s_node_id, b.park_e_node_id)
                    where b.park_link_id is not null;
                '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
      
        self.log.info('end update park for merge')
        return 0

    def __update_height(self):
        self.log.info('updating node begin...')        
        
        #update merge node height
        sqlcmd = '''
            delete from node_height_tbl
            using (
                select a.node_id from node_height_tbl a
                left join node_tbl b
                on a.node_id = b.node_id
                where b.node_id is null
            ) node_del
            where node_height_tbl.node_id = node_del.node_id
            '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.log.info('updating node end.')
        return 0 
    
    def __updateStopSign(self):
        self.log.info('updating stopsign begin...')        
        
        self.CreateTable2('stopsign_tbl_bak_merge')
        self.CreateTable2('stopsign_tbl') 
        
        sqlcmd = '''
            insert into stopsign_tbl(link_id, node_id, pos_flag,the_geom)
            (
                select case when b.link_id is not null then b.link_id
                        else a.link_id
                    end as link_id
                    ,a.node_id
                    ,case when (b.link_id is not null and b.merge_link_dir = 't')
                            or b.link_id is null then a.pos_flag
                        when b.link_id is not null and b.merge_link_dir = 'f' then 3
                        else 0
                    end as pos_flag
                    ,a.the_geom 
                from stopsign_tbl_bak_merge a
                left join temp_merge_link_mapping b
                on a.link_id = b.merge_link_id
                where pos_flag = 2
                
                union
                
                select case when b.link_id is not null then b.link_id
                        else a.link_id
                    end as link_id
                    ,a.node_id
                    ,case when (b.link_id is not null and b.merge_link_dir = 't')
                            or b.link_id is null then a.pos_flag
                        when b.link_id is not null and b.merge_link_dir = 'f' then 2
                        else 0
                    end as pos_flag
                    ,a.the_geom 
                from stopsign_tbl_bak_merge a
                left join temp_merge_link_mapping b
                on a.link_id = b.merge_link_id
                where pos_flag = 3
            );
            '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.CreateIndex2('stopsign_tbl_link_id_idx')
        
        self.log.info('updating stopsign end.')
        return 0      
               