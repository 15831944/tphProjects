# -*- coding: cp936 -*-
'''
Created on 2012-8-17

@author: liuxinxing
'''
import base

class comp_link_split(base.component_base.comp_base):
    '''
    Link Split
    '''

    def __init__(self):
        '''
        Constructor
        '''
        base.component_base.comp_base.__init__(self, 'Link_Split')        
        
    def _Do(self):
        self.__backupTableList()
        self.__makeLinkEnvelopeTile()
        
        self.__splitLinkByTile()                                        #Phase1
        self.__deleteDuplicatedGeomOnTileFrameForSubLink()              #Phase2
        self.__mergeSubLinkWithCircleGeomInTile()                       #Phase3
        self.__splitSubLinkWithCircleGeom()                             #Phase4
        self.__sortSubLink()                                            #Phase5
        self.__deleteShortSubLink()                                     #Additional Phase6
        self.__makeNewIDForSubLink()
        
        self.__updateRegulation()
        self.__updateGuide()
        self.__updateLinkNode()
        
        return 0
    
    def __backupTableList(self):
        self.log.info('making table list backup begin...')
        
        self.CreateTable2('link_tbl_bak_splitting')
        self.CreateIndex2('link_tbl_bak_splitting_link_id_idx')
        self.CreateIndex2('link_tbl_bak_splitting_s_node_idx')
        self.CreateIndex2('link_tbl_bak_splitting_e_node_idx')
        self.CreateTable2('node_tbl_bak_splitting')
        self.CreateIndex2('node_tbl_bak_splitting_node_id_idx')
        
        self.CreateTable2('regulation_relation_tbl_bak_splitting')
        self.CreateIndex2('regulation_relation_tbl_bak_splitting_regulation_id_idx')
        self.CreateTable2('regulation_item_tbl_bak_splitting')
        self.CreateIndex2('regulation_item_tbl_bak_splitting_regulation_id_idx')
        self.CreateIndex2('regulation_item_tbl_bak_splitting_linkid_idx')
        
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
        
        for the_table in guide_table_list:
            sqlcmd = """
                        drop table if exists [the_table]_bak_splitting;
                        create table [the_table]_bak_splitting
                        as
                        select * from [the_table];
                    """
            the_sql_cmd = sqlcmd.replace('[the_table]', the_table)
            self.pg.execute2(the_sql_cmd)
            self.pg.commit2()
        
        self.log.info('making table list backup end.')
    
    def __makeLinkEnvelopeTile(self):
        self.log.info('making link envelope tile begin...')
        
        self.CreateFunction2('rdb_cal_tilex')
        self.CreateFunction2('rdb_cal_tiley')
        self.CreateFunction2('rdb_cal_lon')
        self.CreateFunction2('rdb_cal_lat')
        self.CreateFunction2('rdb_get_tile_area_byxy')
        
        self.CreateTable2('temp_split_link_envelope_tile')
        sqlcmd = """
                    insert into temp_split_link_envelope_tile
                                (link_id, tile_z, x_min, x_max, y_min, y_max, tile_x, tile_y, tile_id)
                    (
                        select  link_id, tile_z, x_min, x_max, y_min, y_max, 
                                tile_x, tile_y, ((tile_z << 28) | (tile_x << 14) | tile_y) as tile_id
                        from
                        (
                            select  link_id, tile_z, x_min, x_max, y_min, y_max, 
                                    tile_x, generate_series(y_min, y_max) as tile_y
                            from
                            (
                                select  link_id, tile_z, x_min, x_max, y_min, y_max, 
                                        generate_series(x_min, x_max) as tile_x
                                from
                                (
                                    select  link_id, tile_z, 
                                            rdb_cal_tilex(point_left_top, tile_z) as x_min,
                                            rdb_cal_tilex(point_right_bottom, tile_z) as x_max,
                                            rdb_cal_tiley(point_left_top, tile_z) as y_min,
                                            rdb_cal_tiley(point_right_bottom, tile_z) as y_max
                                    from
                                    (
                                        select  link_id, 14 as tile_z, 
                                                ST_Point(st_xmin(link_box_geom), st_ymax(link_box_geom)) as point_left_top, 
                                                ST_Point(st_xmax(link_box_geom), st_ymin(link_box_geom)) as point_right_bottom
                                        from
                                        (
                                            select link_id, box2d(the_geom) as link_box_geom
                                            from link_tbl
                                        )as a
                                    )as b
                                )as c
                            )as d
                        )as e
                    );
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('temp_split_link_envelope_tile_link_id_idx')
        self.CreateIndex2('temp_split_link_envelope_tile_tile_id_idx')
        
        self.CreateTable2('temp_split_tile')
        sqlcmd = """
                    insert into temp_split_tile
                                (tile_id, tile_x, tile_y, tile_z, the_geom)
                    (
                        select    tile_id, tile_x, tile_y, tile_z, rdb_get_tile_area_byxy(tile_x, tile_y, tile_z) as the_geom
                        from
                        (
                            select    distinct tile_id, tile_x, tile_y, tile_z
                            from temp_split_link_envelope_tile
                        )as a
                    );
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('temp_split_tile_tile_id_idx')
        self.CreateIndex2('temp_split_tile_the_geom_idx')
        
        self.pg.commit2()
        
        self.log.info('making link envelope tile end.')
    
    def __splitLinkByTile(self):
        self.log.info('Phase1: splitting link by tile begin...')
        
        self.CreateTable2('temp_split_sublink_phase1')
        sqlcmd = """
                    insert into temp_split_sublink_phase1
                                (link_id, tile_id, the_geom)
                    (
                        select  link_id, tile_id, the_geom
                        from
                        (
                            select  link_id, tile_id, st_geometryn(the_geom, geom_index) as the_geom
                            from
                            (
                                select  link_id, tile_id, the_geom, generate_series(1,geom_count) as geom_index
                                from
                                (
                                    select  link_id, tile_id, 
                                            st_multi(the_geom) as the_geom,
                                            st_numgeometries(st_multi(the_geom)) as geom_count
                                    from
                                    (
                                        select  a.link_id, a.tile_id, 
                                                (
                                                case
                                                when st_contains(c.the_geom, b.the_geom) then b.the_geom
                                                else st_intersection(b.the_geom, c.the_geom)
                                                end
                                                )as the_geom
                                        from 
                                        (
                                            select *
                                            from temp_split_link_envelope_tile
                                            where (x_min != x_max) or (y_min != y_max)
                                        ) as a
                                        left join link_tbl_bak_splitting as b
                                        on a.link_id = b.link_id
                                        left join temp_split_tile as c
                                        on a.tile_id = c.tile_id
                                        where st_intersects(b.the_geom, c.the_geom)
                                    )as t
                                )as t2
                            )as t3
                        )as t4
                        where (st_numpoints(the_geom) >= 2)
                              and
                              not (st_numpoints(the_geom) <= 3 and st_isclosed(the_geom))
                    );
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.CreateIndex2('temp_split_sublink_phase1_link_id_idx')
        self.CreateIndex2('temp_split_sublink_phase1_the_geom_idx')
        
        self.log.info('Phase1: splitting link by tile end.')
    
    def __deleteDuplicatedGeomOnTileFrameForSubLink(self):
        self.log.info('Phase2: Deleting duplicated geom on tile frame for sublink begin...')
        
        self.CreateFunction2('rdb_get_tile_dummy_border')
        
        self.CreateTable2('temp_split_sublink_phase2')
        
        sqlcmd = """
                    insert into temp_split_sublink_phase2
                                (gid_phase1, link_id, tile_id, the_geom)
                    (
                        select  gid, link_id, tile_id, the_geom
                        from
                        (
                            select  gid, link_id, tile_id, st_geometryn(the_geom,geom_index) as the_geom
                            from
                            (
                                select  gid, link_id, tile_id, the_geom, 
                                        generate_series(1,geom_count) as geom_index
                                from
                                (
                                    select  gid, link_id, tile_id, st_multi(the_geom) as the_geom,
                                            st_numgeometries(st_multi(the_geom)) as geom_count
                                    from
                                    (
                                        select  a.gid, a.link_id, a.tile_id, 
                                                st_difference(a.the_geom, rdb_get_tile_dummy_border(b.tile_x, b.tile_y, b.tile_z)) as the_geom
                                        from
                                        (
                                            select distinct a.link_id
                                            from temp_split_sublink_phase1 as a
                                            left join temp_split_tile as b
                                            on a.tile_id = b.tile_id
                                            where not st_containsproperly(b.the_geom, a.the_geom)
                                        )as lk
                                        left join temp_split_sublink_phase1 as a
                                        on lk.link_id = a.link_id
                                        left join temp_split_tile as b
                                        on a.tile_id = b.tile_id
                                    )as t
                                )as t2
                            )as t3
                        )as t4
                        where (st_numpoints(the_geom) >= 2)
                              and
                              not (st_numpoints(the_geom) <= 3 and st_isclosed(the_geom))
                    );
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.CreateIndex2('temp_split_sublink_phase2_link_id_idx')
        
        sqlcmd = """
                    insert into temp_split_sublink_phase2
                                (gid_phase1, link_id, tile_id, the_geom)
                    (
                        select  a.gid, a.link_id, a.tile_id, a.the_geom
                        from temp_split_sublink_phase1 as a
                        left join 
                        (
                            select distinct link_id
                            from temp_split_sublink_phase2
                        )as b
                        on a.link_id = b.link_id
                        where b.link_id is null
                    );
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.CreateIndex2('temp_split_sublink_phase2_gid_phase1_idx')
        self.CreateIndex2('temp_split_sublink_phase2_the_geom_idx')
        
        self.log.info('Phase2: Deleting duplicated geom on tile frame for sublink end.')
    
    def __mergeSubLinkWithCircleGeomInTile(self):
        self.log.info('Phase3: merging sublink with circle in tile begin...')
        
        self.CreateTable2('temp_split_sublink_phase3')
        self.CreateFunction2('rdb_get_fraction')
        self.CreateFunction2('rdb_sort_sublink')
        self.CreateFunction2('rdb_merge_sublink_in_tile')
        self.CreateFunction2('rdb_merge_sorted_sublink_in_tile')
        
        sqlcmd = """
                    insert into temp_split_sublink_phase3
                                (link_id, tile_id, the_geom)
                    (
                        select  link_id, 
                                merge_tile_array[sub_index] as tile_id, 
                                merge_geom_array[sub_index] as the_geom
                        from
                        (
                            select  link_id, link_geom, merge_tile_array, merge_geom_array,
                                    generate_series(1, array_upper(merge_geom_array,1)) as sub_index
                            from
                            (
                                select  link_id, link_geom, 
                                        (rdb_merge_sublink_in_tile(link_id, link_geom, sub_tile_array, sub_geom_array)).*
                                from
                                (
                                    select  link_id, link_geom, 
                                            array_agg(tile_id) as sub_tile_array,
                                            array_agg(sub_geom) as sub_geom_array
                                    from
                                    (
                                        select c.link_id, c.the_geom as link_geom, b.tile_id, b.the_geom as sub_geom
                                        from
                                        (
                                            select a.link_id
                                            from 
                                            (
                                                select distinct link_id
                                                from temp_split_sublink_phase2 as a
                                                group by link_id, tile_id having count(*) > 1
                                            ) as a
                                            inner join link_tbl_bak_splitting as b
                                            on a.link_id = b.link_id and not st_issimple(b.the_geom)
                                        )as a
                                        left join temp_split_sublink_phase2 as b
                                        on a.link_id = b.link_id
                                        left join link_tbl_bak_splitting as c
                                        on a.link_id = c.link_id
                                    )as t
                                    group by link_id, link_geom
                                )as t2
                            )as t3
                        )as t4
                    );
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('temp_split_sublink_phase2_link_id_idx')
        
        sqlcmd = """
                    insert into temp_split_sublink_phase3
                                (link_id, tile_id, the_geom)
                    (
                        select  a.link_id, a.tile_id, a.the_geom
                        from temp_split_sublink_phase2 as a
                        left join 
                        (
                            select distinct link_id
                            from temp_split_sublink_phase3
                        )as b
                        on a.link_id = b.link_id
                        where b.link_id is null
                    );
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('temp_split_sublink_phase3_the_geom_idx')
        
        self.log.info('Phase3: merging sublink with circle in tile end...')
    
    def __splitSubLinkWithCircleGeom(self):
        self.log.info('Phase4: splitting sublink with circle geom begin...')
        
        self.CreateTable2('temp_split_sublink_phase4')
        self.CreateFunction2('rdb_split_circle')
        
        sqlcmd = """
                    insert into temp_split_sublink_phase4
                                (gid_phase3, link_id, tile_id, the_geom)
                    (
                        select  gid, link_id, tile_id, unnest(geom_array) as the_geom
                        from
                        (
                            select  gid, link_id, tile_id, rdb_split_circle(the_geom) as geom_array
                            from temp_split_sublink_phase3
                            where st_isclosed(the_geom)
                        )as t
                    );
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.CreateIndex2('temp_split_sublink_phase4_gid_phase3_idx')
        
        sqlcmd = """
                    insert into temp_split_sublink_phase4
                                (gid_phase3, link_id, tile_id, the_geom)
                    (
                        select  a.gid, a.link_id, a.tile_id, a.the_geom
                        from temp_split_sublink_phase3 as a
                        left join 
                        (
                            select distinct gid_phase3
                            from temp_split_sublink_phase4
                        )as b
                        on a.gid = b.gid_phase3
                        where b.gid_phase3 is null
                    );
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.CreateIndex2('temp_split_sublink_phase4_link_id_idx')
        self.CreateIndex2('temp_split_sublink_phase4_the_geom_idx')
        
        self.log.info('Phase4: splitting sublink with circle geom end.')
    
    def __sortSubLink(self):
        self.log.info('Phase5: sorting sublink begin...')
        
        self.CreateTable2('temp_split_sublink_phase5')
        self.CreateFunction2('rdb_get_fraction')
        self.CreateFunction2('rdb_sort_sublink')
        
        sqlcmd = """
                    insert into temp_split_sublink_phase5
                                (link_id, sub_count, sub_index, tile_id, the_geom)
                    (
                        select  link_id, sub_count, sub_index, 
                                sort_tile_array[sub_index] as tile_id,
                                sort_geom_array[sub_index] as the_geom
                        from
                        (
                            select  link_id, 
                                    array_upper(sort_geom_array, 1) as sub_count,
                                    sort_tile_array,
                                    sort_geom_array,
                                    generate_series(1, array_upper(sort_geom_array, 1)) as sub_index
                            from
                            (
                                select  link_id, (rdb_sort_sublink(link_id, link_geom, sub_tile_array, sub_geom_array)).*
                                from
                                (
                                    select  link_id, link_geom, 
                                            array_agg(tile_id) as sub_tile_array,
                                            array_agg(the_geom) as sub_geom_array
                                    from
                                    (
                                        select  a.link_id, b.the_geom as link_geom, a.tile_id, a.the_geom
                                        from temp_split_sublink_phase4 as a
                                        left join link_tbl_bak_splitting as b
                                        on a.link_id = b.link_id
                                    )as t
                                    group by link_id, link_geom
                                )as t2
                            )as t3
                        )as t4
                    );
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.CreateIndex2('temp_split_sublink_phase5_link_id_idx')
        self.CreateIndex2('temp_split_sublink_phase5_the_geom_idx')
        
        self.log.info('Phase5: sorting sublink end.')
    
    def __deleteShortSubLink(self):
        self.log.info('Phase6: deleting short sublinks begin...')
        
        # functions definition
        self.CreateFunction2('lonlat2pixel_tile')
        self.CreateFunction2('lonlat2pixel')
        self.CreateFunction2('tile_bbox')
        self.CreateFunction2('world2lonlat')
        self.CreateFunction2('lonlat2world')
        self.CreateFunction2('pixel2world')
        self.CreateFunction2('rdb_move_point_pixel')
        self.CreateFunction2('rdb_merge_sorted_sublink_in_tile')
        
        # find short sublink, and its related node
        self.CreateTable2('temp_split_short_sublink')
        self.CreateIndex2('temp_split_short_sublink_link_id_idx')
        self.CreateIndex2('temp_split_short_sublink_s_node_idx')
        self.CreateIndex2('temp_split_short_sublink_e_node_idx')
        
        self.CreateTable2('temp_split_short_node_degree')
        self.CreateIndex2('temp_split_short_node_degree_node_id_idx')
        
        # find short sublink which can not be deleted
        self.CreateTable2('temp_split_keep_sublink')
        self.CreateIndex2('temp_split_keep_sublink_link_id_idx')
        
        # delete_sublink = short_sublink - keep_sublink
        self.CreateTable2('temp_split_delete_sublink')
        self.CreateIndex2('temp_split_delete_sublink_link_id_idx')
        self.CreateTable2('temp_split_delete_sublink_environment')
        self.CreateIndex2('temp_split_delete_sublink_environment_link_id_idx')
        
        # register moving geometry of node, which is related to short sublink
        self.CreateFunction2('rdb_get_proxy_xy_advice')
        self.CreateTable2('temp_split_move_node')
        self.CreateIndex2('temp_split_move_node_node_id_idx')
        
        # register the deleting sublink and its proxy point
        self.CreateFunction2('rdb_find_proxy_point_of_short_sublink')
        self.CreateTable2('temp_split_delete_sublink_point')
        self.CreateIndex2('temp_split_delete_sublink_point_link_id_idx')
        
        # register moving geometry of link
        self.CreateTable2('temp_split_move_link')
        self.CreateIndex2('temp_split_move_link_link_id_idx')
        
        # register moving geometry of sublink
        self.CreateFunction2('rdb_recalc_tile_for_move_sublink')
        self.CreateTable2('temp_split_move_sublink')
        self.CreateIndex2('temp_split_move_sublink_link_id_idx')
        
        # delete short sublink
        self.CreateFunction2('rdb_delete_short_sublink')
        self.CreateTable2('temp_split_sublink_phase6')
        sqlcmd = """
                    insert into temp_split_sublink_phase6
                                (link_id, sub_count, sub_index, tile_id, the_geom)
                    select  link_id, sub_count, sub_index,
                            keep_tile_array[sub_index] as tile_id,
                            keep_geom_array[sub_index] as the_geom
                    from
                    (
                        select  link_id, keep_tile_array, keep_geom_array, 
                                sub_count, generate_series(1,sub_count) as sub_index
                        from
                        (
                            select  link_id, keep_tile_array, keep_geom_array,
                                    array_upper(keep_tile_array, 1) as sub_count
                            from
                            (
                                select  link_id, 
                                        (rdb_delete_short_sublink(tile_array, geom_array, flag_array, pt_array)).*
                                from
                                (
                                    select  link_id, 
                                            array_agg(tile_id) as tile_array, 
                                            array_agg(the_geom) as geom_array,
                                            array_agg(delete_flag) as flag_array,
                                            array_agg(proxy_point) as pt_array
                                    from
                                    (
                                        select  a.link_id, a.sub_index, 
                                                (case when b.link_id is null then a.tile_id else b.tile_id end) as tile_id,
                                                (case when b.link_id is null then a.the_geom else b.the_geom end) as the_geom,
                                                (case when c.link_id is null then 0 else 1 end) as delete_flag,
                                                c.proxy_point
                                        from
                                        (
                                            select distinct link_id
                                            from
                                            (
                                                select distinct link_id
                                                from temp_split_move_sublink
                                                union
                                                select distinct link_id
                                                from temp_split_delete_sublink
                                            )as a
                                        )as t
                                        inner join temp_split_sublink_phase5 as a
                                        on t.link_id = a.link_id
                                        left join temp_split_move_sublink as b
                                        on a.link_id = b.link_id and a.sub_index = b.sub_index
                                        left join temp_split_delete_sublink_point as c
                                        on a.link_id = c.link_id and a.sub_index = c.sub_index
                                        order by a.link_id, a.sub_index
                                    )as a
                                    group by link_id
                                )as b
                            )as c
                        )as d
                    )as e;
                """
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('temp_split_sublink_phase6_link_id_idx')
        
        sqlcmd = """
                    insert into temp_split_sublink_phase6
                                (link_id, sub_count, sub_index, tile_id, the_geom)
                    select a.link_id, a.sub_count, a.sub_index, a.tile_id, a.the_geom
                    from temp_split_sublink_phase5 as a
                    left join (select distinct link_id from temp_split_sublink_phase6) as b
                    on a.link_id = b.link_id
                    where b.link_id is null;
                """
        self.pg.execute(sqlcmd)
        self.pg.commit2()

        self.CreateIndex2('temp_split_sublink_phase6_the_geom_idx')
        
        self.log.info('Phase6 end.')
    
    def __makeNewIDForSubLink(self):
        self.log.info('making new id begin...')
        
        #
        self.CreateTable2('temp_split_newnode')
        sqlcmd = """
                    drop sequence if exists new_nodeid_seq;
                    create sequence new_nodeid_seq;
                    select setval('new_nodeid_seq', max_id + 1000000)
                    from
                    (
                        select max(node_id) as max_id
                        from node_tbl
                    )as a;
                    
                    insert into temp_split_newnode
                                (node_id, old_link_id, sub_index, tile_id, the_geom)
                    (
                        select  nextval('new_nodeid_seq') as node_id, 
                                link_id as old_link_id,
                                (sub_index - 1) as sub_index,
                                tile_id,
                                st_startpoint(the_geom) as the_geom
                        from temp_split_sublink_phase6
                        where sub_index > 1 order by old_link_id,sub_index
                    );
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('temp_split_newnode_node_id_idx')
        self.CreateIndex2('temp_split_newnode_old_link_id_idx')
        self.CreateIndex2('temp_split_newnode_sub_index_idx')
        self.CreateIndex2('temp_split_newnode_the_geom_idx')
        
        #
        sqlcmd = """
                    delete from temp_split_link_envelope_tile as a
                    using
                    (
                        select link_id, tile_id
                        from temp_split_sublink_phase6
                        where sub_count = 1
                    )as b
                    where a.link_id = b.link_id and a.tile_id != b.tile_id
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        
        self.CreateTable2('temp_split_newlink')
        sqlcmd = """
                    drop sequence if exists new_linkid_seq;
                    create sequence new_linkid_seq;
                    select setval('new_linkid_seq', max_id + 1000000)
                    from
                    (
                        select max(link_id) as max_id
                        from link_tbl_bak_splitting
                    )as a;
                    
                    insert into temp_split_newlink
                                (link_id, s_node, e_node, old_link_id, sub_count, sub_index, tile_id, the_geom)
                    (
                        select  nextval('new_linkid_seq') as link_id,
                                (case when s.node_id is not null then s.node_id else d.s_node end) as s_node,
                                (case when e.node_id is not null then e.node_id else d.e_node end) as e_node,
                                c.old_link_id, c.sub_count, c.sub_index, c.tile_id, c.the_geom
                        from
                        (
                            select  link_id as old_link_id, sub_count, sub_index, tile_id, the_geom,
                                    (sub_index - 1) as s_index,
                                    sub_index as e_index
                            from temp_split_sublink_phase6
                            where sub_count > 1
                        )as c
                        left join link_tbl_bak_splitting as d
                        on c.old_link_id = d.link_id
                        left join temp_split_newnode as s
                        on c.old_link_id = s.old_link_id and c.s_index = s.sub_index
                        left join temp_split_newnode as e
                        on c.old_link_id = e.old_link_id and c.e_index = e.sub_index
                        order by old_link_id,sub_index
                    );
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('temp_split_newlink_link_id_idx')
        self.CreateIndex2('temp_split_newlink_s_node_idx')
        self.CreateIndex2('temp_split_newlink_e_node_idx')
        self.CreateIndex2('temp_split_newlink_old_link_id_idx')
        self.CreateIndex2('temp_split_newlink_sub_index_idx')
        self.CreateIndex2('temp_split_newlink_the_geom_idx')
        
        self.log.info('making new id end.')
    
    def __updateRegulation(self):
        self.log.info('updating regulation begin...')
        
        self.CreateFunction2('rdb_get_split_link')
        self.CreateFunction2('rdb_get_split_link_array')
        self.CreateFunction2('rdb_get_junction_node_list')
        self.CreateFunction2('rdb_get_split_linkrow')
        
        self.CreateTable2('temp_split_update_linkrow_regulation')
        self.CreateIndex2('temp_split_update_linkrow_regulation_regulation_id_idx')
        
        self.CreateTable2('regulation_relation_tbl')
        sqlcmd = """
                    insert into regulation_relation_tbl
                                (regulation_id, nodeid, inlinkid, outlinkid, condtype, cond_id, gatetype, slope)
                    (
                        select a.regulation_id, a.nodeid, a.inlinkid, a.outlinkid, condtype, cond_id, gatetype, slope
                        from regulation_relation_tbl_bak_splitting as a
                        left join temp_split_update_linkrow_regulation as b
                        on a.regulation_id = b.regulation_id
                        where b.regulation_id is null
                    );
                    
                    insert into regulation_relation_tbl
                                (regulation_id, nodeid, inlinkid, outlinkid, condtype, cond_id, gatetype, slope)
                    (
                        select a.regulation_id, a.nodeid, b.inlinkid, b.outlinkid, condtype, cond_id, gatetype, slope
                        from regulation_relation_tbl_bak_splitting as a
                        inner join temp_split_update_linkrow_regulation as b
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
                        from regulation_item_tbl_bak_splitting as a
                        left join temp_split_update_linkrow_regulation as b
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
                                from (select * from regulation_item_tbl_bak_splitting where seq_num = 1) as a
                                inner join temp_split_update_linkrow_regulation as b
                                on a.regulation_id = b.regulation_id
                            )as c
                        )as d
                        order by regulation_id, seq_num
                    );
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.log.info('updating regulation end.')
    
    def __updateGuide(self):
        self.log.info('updating guide begin...')
        
        sqlcmd = """
                drop table if exists temp_split_update_linkrow_[the_guide_tbl];
                create table temp_split_update_linkrow_[the_guide_tbl]
                as
                (
                    select  gid, 
                            link_num, 
                            link_array,
                            nodeid, 
                            link_array[1] as inlinkid, 
                            (case when link_num = 1 then null else link_array[link_num] end) as outlinkid, 
                            (case when link_num > 2 then (link_num - 2) else 0 end) as passlink_cnt,
                            array_to_string(link_array[2:link_num-1], '|') as passlid
                    from
                    (
                        select  gid, nodeid, link_array, array_upper(link_array, 1) as link_num
                        from
                        (
                            select  gid, nodeid, 
                                    rdb_get_split_linkrow(nodeid, link_array, s_array, e_array) as link_array
                            from
                            (
                                select     gid, 
                                        nodeid,
                                        array_agg(link_id) as link_array, 
                                        array_agg(s_node) as s_array,
                                        array_agg(e_node) as e_array
                                from
                                (
                                    select     c.gid, c.nodeid, c.link_index, c.link_id, d.s_node, d.e_node
                                    from 
                                    (
                                        select    gid, nodeid, link_index, link_array[link_index] as link_id
                                        from
                                        (
                                            select  gid, 
                                                    nodeid,
                                                    link_num,
                                                    link_array,
                                                    generate_series(1,link_num) as link_index
                                            from
                                            (
                                                select     gid, 
                                                        nodeid,
                                                        (passlink_cnt + 2) as link_num,
                                                        (
                                                        case 
                                                        when passlink_cnt = 0 then ARRAY[inlinkid, outlinkid]
                                                        else ARRAY[inlinkid] || cast(regexp_split_to_array(passlid, E'\\\\|+') as bigint[]) || ARRAY[outlinkid]
                                                        end
                                                        )as link_array
                                                from [the_guide_tbl]
                                            )as a
                                        )as b
                                    )as c
                                    left join link_tbl_bak_splitting as d
                                    on c.link_id = d.link_id
                                    order by c.gid, c.nodeid, c.link_index
                                )as a
                                group by gid, nodeid
                            )as b
                        )as c
                    )as d
                );

                CREATE INDEX temp_split_update_linkrow_[the_guide_tbl]_gid_idx
                    ON temp_split_update_linkrow_[the_guide_tbl]
                    USING btree
                    (gid);
                
                update [the_guide_tbl] as a
                set inlinkid = b.inlinkid, outlinkid = b.outlinkid, passlid = b.passlid, passlink_cnt = b.passlink_cnt
                from temp_split_update_linkrow_[the_guide_tbl] as b
                where a.gid = b.gid;
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
        
        self.log.info('updating guide end.')
    
    def __updateLinkNode(self):
        # update link
        self.log.info('updating link begin...')
        
        self.CreateFunction2('mid_cnv_length')
        self.CreateFunction2('mid_cal_zm')
                
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
                        select  a.link_id, c.tile_id, a.s_node, a.e_node, link_type, road_type, toll, speed_class, a.length, 
                                function_class, lane_dir, lane_num_s2e, lane_num_e2s, elevated, structure, tunnel, rail_cross, paved, uturn, 
                                speed_limit_s2e, speed_limit_e2s, speed_source_s2e, speed_source_e2s, width_s2e, width_e2s,
                                one_way_code, one_way_condition, pass_code, pass_code_condition, road_name, road_number, 
                                name_type, ownership, car_only, slope_code, slope_angle, disobey_flag, up_down_distinguish, 
                                access, extend_flag, etc_only_flag, display_class,
                                mid_cal_zm(a.the_geom, 1) as fazm,
                                mid_cal_zm(a.the_geom, -1) as tazm,
                                feature_string, feature_key, 
                                (
                                 case 
                                      when e.link_id is not null then e.the_geom 
                                      when d.link_id is not null then d.the_geom 
                                      else a.the_geom 
                                 end
                                ) as the_geom
                        from link_tbl_bak_splitting as a
                        left join (select old_link_id from temp_split_newlink where sub_index = 1) as b
                        on a.link_id = b.old_link_id
                        left join temp_split_link_envelope_tile as c
                        on a.link_id = c.link_id
                        left join temp_split_move_link as d
                        on a.link_id = d.link_id
                        left join (select * from temp_split_sublink_phase6 where sub_count = 1) as e
                        on a.link_id = e.link_id
                        where b.old_link_id is null
                        order by a.gid
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
                        select  b.link_id, b.tile_id, b.s_node, b.e_node, link_type, road_type, toll, speed_class, 
                                ST_Length_Spheroid(b.the_geom,'SPHEROID("WGS_84", 6378137, 298.257223563)') as link_length, 
                                function_class, lane_dir, lane_num_s2e, lane_num_e2s, elevated, structure, tunnel, rail_cross, paved, uturn, 
                                speed_limit_s2e, speed_limit_e2s, speed_source_s2e, speed_source_e2s, width_s2e, width_e2s,
                                one_way_code, one_way_condition, pass_code, pass_code_condition, road_name, road_number, 
                                name_type, ownership, car_only, slope_code, slope_angle, disobey_flag, up_down_distinguish, 
                                access, extend_flag, etc_only_flag, display_class,
                                (case when b.sub_index = 1 then mid_cal_zm(a.the_geom, 1) else mid_cal_zm(b.the_geom, 1) end) as fazm,
                                (case when b.sub_index = b.sub_count then mid_cal_zm(a.the_geom, -1) else mid_cal_zm(b.the_geom, -1) end) as tazm,
                                null as feature_string, null as feature_key, 
                                b.the_geom
                        from link_tbl_bak_splitting as a
                        inner join temp_split_newlink as b
                        on a.link_id = b.old_link_id
                        order by b.link_id
                    );
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.CreateIndex2('link_tbl_link_id_idx')
        self.CreateIndex2('link_tbl_s_node_idx')
        self.CreateIndex2('link_tbl_e_node_idx')
        self.CreateIndex2('link_tbl_the_geom_idx')
        
        self.log.info('updating link end.')
        
        
        # update node
        self.log.info('updating node begin...')
        
        self.CreateTable2('node_tbl')
        self.CreateFunction2('rdb_geom2tileid_z')
        
        sqlcmd = """
                    insert into node_tbl
                                (node_id, tile_id, kind, light_flag, toll_flag, org_boundary_flag, 
                                tile_boundary_flag, mainnodeid, node_lid, node_name, feature_string, feature_key, the_geom)
                    (
                        select  node_id, rdb_geom2tileid_z(the_geom, 14) as tile_id, kind, light_flag, toll_flag, org_boundary_flag, 
                                tile_boundary_flag, mainnodeid, node_lid, node_name, feature_string, feature_key, the_geom
                        from
                        (
                            select  a.node_id, kind, light_flag, toll_flag, org_boundary_flag, 
                                    tile_boundary_flag, mainnodeid, node_lid, node_name, feature_string, feature_key, 
                                    (case when b.node_id is not null then b.the_geom else a.the_geom end) as the_geom
                            from node_tbl_bak_splitting as a
                            left join temp_split_move_node as b
                            on a.node_id = b.node_id
                        )as t
                    );

                    insert into node_tbl
                                (node_id, tile_id, kind, light_flag, toll_flag, org_boundary_flag, 
                                tile_boundary_flag, mainnodeid, node_lid, node_name, feature_string, feature_key, the_geom)
                    (
                        select  node_id, rdb_geom2tileid_z(the_geom, 14) as tile_id, null, 0, 0, 0,
                                1, 0, null, null, null as feature_string, null as feature_key, the_geom
                        from temp_split_newnode
                    );
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.CreateIndex2('node_tbl_node_id_idx')
        self.CreateIndex2('node_tbl_the_geom_idx')
        
        self.log.info('updating node end.')
        
        