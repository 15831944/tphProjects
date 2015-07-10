# -*- coding: cp936 -*-
'''
Created on 2012-4-12

@author: hongchenzai
'''
from base.rdb_ItemBase import ItemBase
from common import rdb_log
from common import rdb_common
import rdb_dictionary

class rdb_view(ItemBase):
    '''创建view
    '''

    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self, 'View')

    def Do(self):
        self._createViewForBinaryData()
        self._createViewForCheckLogic()

        return 0

    def _createViewForBinaryData(self):
        if self.pg.IsExistTable('rdb_region_link_layer8_tbl'):
            self._createViewForBinaryData_Layer8()
            return
        
        #
        sqlcmd = """
                ----------------------------------------------------------------------
                -- rdb_tile_list
                ----------------------------------------------------------------------
                CREATE OR REPLACE VIEW rdb_tile_list AS
                select tile_id, (tile_id >> 14) & 16383 as x, tile_id & 16383 as y, (tile_id >> 28) & 15 as z
                from
                (
                    select distinct tile_id
                    from
                    (
                        select distinct link_id_t as tile_id
                        from rdb_link
                        union
                        select distinct node_id_t as tile_id
                        from rdb_node
                        union
                        select distinct link_id_t as tile_id
                        from rdb_region_link_layer4_tbl
                        union
                        select distinct node_id_t as tile_id
                        from rdb_region_node_layer4_tbl
                        union
                        select distinct link_id_t as tile_id
                        from rdb_region_link_layer6_tbl
                        union
                        select distinct node_id_t as tile_id
                        from rdb_region_node_layer6_tbl
                        union
                        select distinct tile_id
                        from rdb_tile_admin_zone
                    )as t
                )as t;

----------------------------------------------------------------------
                -- rdb region link with light and toll view
                ----------------------------------------------------------------------
                CREATE OR REPLACE VIEW rdb_region_link_layer6_tbl_view AS 
                select a.link_id, link_id_t, start_node_id, end_node_id, road_type, pdm_flag,one_way, 
                   function_code, link_length, link_type, road_name, toll, bypass_flag, highcost_flag,
                   regulation_exist_state, fazm_path, tazm_path, link_add_info_flag, speed_regulation_flag, 
                   lane_id, link_length_modify, link_length_unit, abs_link_id, abs_link_diff, abs_link_dir, the_geom, 
                   case when b.light_cnt is not null then b.light_cnt else 0 end as light_cnt,
                   case when b.toll_cnt is not null then b.toll_cnt else 0 end as toll_cnt,
                   (case when c.link_id is null then -1 else c.s_link_id end) as s_sequence_link_id,
                   (case when c.link_id is null then -1 else c.e_link_id end) as e_sequence_link_id,
                   (case when d.link_id is null then false else true end) as forecast_flag
                from rdb_region_link_layer6_tbl a
                left join rdb_region_layer6_light_tollnode_tbl as b
                on a.link_id = b.region_link_id
                left join rdb_region_link_layer6_sequence as c
                on a.link_id = c.link_id
                left join (
                    select distinct link_id from rdb_forecast_link
                ) d
                on a.link_id = d.link_id;
                
                CREATE OR REPLACE VIEW rdb_region_link_layer4_tbl_view AS
                select a.link_id, link_id_t, start_node_id, end_node_id, road_type, pdm_flag,one_way, 
                   function_code, link_length, link_type, road_name, toll, bypass_flag, highcost_flag,  
                   regulation_exist_state, fazm_path, tazm_path, link_add_info_flag, speed_regulation_flag, 
                   lane_id, link_length_modify, link_length_unit, abs_link_id, abs_link_diff, abs_link_dir, the_geom, 
                   case when b.light_cnt is not null then b.light_cnt else 0 end as light_cnt,
                   case when b.toll_cnt is not null then b.toll_cnt else 0 end as toll_cnt,
                   (case when c.link_id is null then -1 else c.s_link_id end) as s_sequence_link_id,
                   (case when c.link_id is null then -1 else c.e_link_id end) as e_sequence_link_id,
                   (case when d.link_id is null then false else true end) as forecast_flag  
                from rdb_region_link_layer4_tbl a
                left join rdb_region_layer4_light_tollnode_tbl as b
                on a.link_id = b.region_link_id
                left join rdb_region_link_layer4_sequence as c
                on a.link_id = c.link_id
                left join (
                    select distinct link_id from rdb_forecast_link
                ) d
                on a.link_id = d.link_id;
                """
        # sqlcmd = self.pg.ReplaceDictionary(sqlcmd)
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        # 创建名称字典View
        # self._CreateDictionaryView()

        # pic_tile
        self.CreateTable2('rdb_pic_tile_info')

        # 记录保序
        # (guide_table_name, gid, order_attrs)
        table_list = [
                      ('rdb_guideinfo_caution', 'guideinfo_id', 'in_link_id, node_id, out_link_id, passlink_count, data_kind, voice_id'),
                      ('rdb_guideinfo_forceguide', 'guideinfo_id', 'in_link_id, node_id, out_link_id, passlink_count, guide_type, position_type'),
                      ('rdb_guideinfo_lane', 'guideinfo_id', 'in_link_id, node_id, out_link_id, passlink_count, lane_num, lane_info, arrow_info, lane_num_l, lane_num_r'),
                      ('rdb_guideinfo_spotguidepoint', 'gid', 'in_link_id, node_id, out_link_id, passlink_count, type, pattern_id, arrow_id, point_list'),
                      ('rdb_link_regulation', 'record_no', 'regulation_type, is_seasonal, link_num, first_link_id, last_link_id, last_link_dir, key_string'),
                      ('rdb_guideinfo_crossname', 'guideinfo_id', 'in_link_id, node_id, out_link_id, cross_name'),
                      ('rdb_guideinfo_signpost', 'gid', 'in_link_id, node_id, out_link_id, passlink_count, sp_name, pattern_id, arrow_id'),
                      ('rdb_guideinfo_signpost_uc', 'gid', 'in_link_id, node_id, out_link_id, passlink_count, sp_name, route_no1, route_no2, route_no3, route_no4, exit_no'),
                      ('rdb_guideinfo_towardname', 'gid', 'in_link_id, node_id, out_link_id, passlink_count, toward_name, name_attr, name_kind'),
                      ('rdb_region_link_regulation_layer4_tbl', 'record_no', 'regulation_type, is_seasonal, link_num, first_link_id, last_link_id, last_link_dir, key_string'),
                      ('rdb_region_link_regulation_layer6_tbl', 'record_no', 'regulation_type, is_seasonal, link_num, first_link_id, last_link_id, last_link_dir, key_string'),
                      ('rdb_guideinfo_road_structure', 'guideinfo_id', 'in_link_id, node_id, out_link_id, type_code, structure_name'),
                      ('rdb_guideinfo_building_structure', 'guideinfo_id', 'in_link_id, node_id, out_link_id, type_code, centroid_lontitude, centroid_lantitude, building_name'),
                      ]
        for (table_name, gid, order_attrs) in table_list:
            sqlcmd = """
                    alter table %s drop column if exists order_id CASCADE;
                    alter table %s add column order_id integer;
                    update %s as a set order_id = b.binary_order_id
                    from
                    (
                        select *, row_number() over (order by %s) as binary_order_id
                        from %s
                    ) as b
                    where a.%s = b.%s;
                    """ % (table_name, table_name, table_name, order_attrs, table_name, gid, gid)
            self.pg.execute2(sqlcmd)
            self.pg.commit2()

    def _createViewForBinaryData_Layer8(self):
        #
        sqlcmd = """
                ----------------------------------------------------------------------
                -- rdb_tile_list
                ----------------------------------------------------------------------
                CREATE OR REPLACE VIEW rdb_tile_list AS
                select tile_id, (tile_id >> 14) & 16383 as x, tile_id & 16383 as y, (tile_id >> 28) & 15 as z
                from
                (
                    select distinct tile_id
                    from
                    (
                        select distinct link_id_t as tile_id
                        from rdb_link
                        union
                        select distinct node_id_t as tile_id
                        from rdb_node
                        union
                        select distinct link_id_t as tile_id
                        from rdb_region_link_layer4_tbl
                        union
                        select distinct node_id_t as tile_id
                        from rdb_region_node_layer4_tbl
                        union
                        select distinct link_id_t as tile_id
                        from rdb_region_link_layer6_tbl
                        union
                        select distinct node_id_t as tile_id
                        from rdb_region_node_layer6_tbl
                        union
                        select distinct link_id_t as tile_id
                        from rdb_region_link_layer8_tbl
                        union
                        select distinct node_id_t as tile_id
                        from rdb_region_node_layer8_tbl
                        union
                        select distinct tile_id
                        from rdb_tile_admin_zone
                    )as t
                )as t;

                ----------------------------------------------------------------------
                -- rdb region link with light and toll view
                ----------------------------------------------------------------------
                CREATE OR REPLACE VIEW rdb_region_link_layer8_tbl_view AS 
                select a.link_id, link_id_t, start_node_id, end_node_id, road_type, pdm_flag,one_way, 
                   function_code, link_length, link_type, road_name, toll, bypass_flag, highcost_flag,  
                   regulation_exist_state, fazm_path, tazm_path, link_add_info_flag, speed_regulation_flag, 
                   lane_id, link_length_modify, link_length_unit, abs_link_id, abs_link_diff, abs_link_dir, the_geom, 
                   case when b.light_cnt is not null then b.light_cnt else 0 end as light_cnt,
                   case when b.toll_cnt is not null then b.toll_cnt else 0 end as toll_cnt,
                   (case when c.link_id is null then -1 else c.s_link_id end) as s_sequence_link_id,
                   (case when c.link_id is null then -1 else c.e_link_id end) as e_sequence_link_id      
                from rdb_region_link_layer8_tbl a
                left join rdb_region_layer8_light_tollnode_tbl as b
                on a.link_id = b.region_link_id
                left join rdb_region_link_layer8_sequence as c
                on a.link_id = c.link_id;
                
                CREATE OR REPLACE VIEW rdb_region_link_layer6_tbl_view AS 
                select a.link_id, link_id_t, start_node_id, end_node_id, road_type, pdm_flag,one_way, 
                   function_code, link_length, link_type, road_name, toll, bypass_flag, highcost_flag,  
                   regulation_exist_state, fazm_path, tazm_path, link_add_info_flag, speed_regulation_flag, 
                   lane_id, link_length_modify, link_length_unit, abs_link_id, abs_link_diff, abs_link_dir, the_geom, 
                   case when b.light_cnt is not null then b.light_cnt else 0 end as light_cnt,
                   case when b.toll_cnt is not null then b.toll_cnt else 0 end as toll_cnt,
                   (case when c.link_id is null then -1 else c.s_link_id end) as s_sequence_link_id,
                   (case when c.link_id is null then -1 else c.e_link_id end) as e_sequence_link_id   
                from rdb_region_link_layer6_tbl a
                left join rdb_region_layer6_light_tollnode_tbl as b
                on a.link_id = b.region_link_id
                left join rdb_region_link_layer6_sequence as c
                on a.link_id = c.link_id;
                
                CREATE OR REPLACE VIEW rdb_region_link_layer4_tbl_view AS
                select a.link_id, link_id_t, start_node_id, end_node_id, road_type, pdm_flag,one_way, 
                   function_code, link_length, link_type, road_name, toll, bypass_flag, highcost_flag,  
                   regulation_exist_state, fazm_path, tazm_path, link_add_info_flag, speed_regulation_flag, 
                   lane_id, link_length_modify, link_length_unit, abs_link_id, abs_link_diff, abs_link_dir, the_geom, 
                   case when b.light_cnt is not null then b.light_cnt else 0 end as light_cnt,
                   case when b.toll_cnt is not null then b.toll_cnt else 0 end as toll_cnt,
                   (case when c.link_id is null then -1 else c.s_link_id end) as s_sequence_link_id,
                   (case when c.link_id is null then -1 else c.e_link_id end) as e_sequence_link_id  
                from rdb_region_link_layer4_tbl a
                left join rdb_region_layer4_light_tollnode_tbl as b
                on a.link_id = b.region_link_id
                left join rdb_region_link_layer4_sequence as c
                on a.link_id = c.link_id;
                """
        # sqlcmd = self.pg.ReplaceDictionary(sqlcmd)
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        # 创建名称字典View
        # self._CreateDictionaryView()

        # pic_tile
        self.CreateTable2('rdb_pic_tile_info')

        # 记录保序
        # (guide_table_name, gid, order_attrs)
        table_list = [
                      ('rdb_guideinfo_caution', 'guideinfo_id', 'in_link_id, node_id, out_link_id, passlink_count, data_kind, voice_id'),
                      ('rdb_guideinfo_forceguide', 'guideinfo_id', 'in_link_id, node_id, out_link_id, passlink_count, guide_type, position_type'),
                      ('rdb_guideinfo_lane', 'guideinfo_id', 'in_link_id, node_id, out_link_id, passlink_count, lane_num, lane_info, arrow_info, lane_num_l, lane_num_r'),
                      ('rdb_guideinfo_spotguidepoint', 'gid', 'in_link_id, node_id, out_link_id, passlink_count, type, pattern_id, arrow_id, point_list'),
                      ('rdb_link_regulation', 'record_no', 'regulation_type, is_seasonal, link_num, first_link_id, last_link_id, last_link_dir, key_string'),
                      ('rdb_guideinfo_crossname', 'guideinfo_id', 'in_link_id, node_id, out_link_id, cross_name'),
                      ('rdb_guideinfo_signpost', 'gid', 'in_link_id, node_id, out_link_id, passlink_count, sp_name, pattern_id, arrow_id'),
                      ('rdb_guideinfo_signpost_uc', 'gid', 'in_link_id, node_id, out_link_id, passlink_count, sp_name, route_no1, route_no2, route_no3, route_no4, exit_no'),
                      ('rdb_guideinfo_towardname', 'gid', 'in_link_id, node_id, out_link_id, passlink_count, toward_name, name_attr, name_kind'),
                      ('rdb_region_link_regulation_layer4_tbl', 'record_no', 'regulation_type, is_seasonal, link_num, first_link_id, last_link_id, last_link_dir, key_string'),
                      ('rdb_region_link_regulation_layer6_tbl', 'record_no', 'regulation_type, is_seasonal, link_num, first_link_id, last_link_id, last_link_dir, key_string'),
                      ('rdb_region_link_regulation_layer8_tbl', 'record_no', 'regulation_type, is_seasonal, link_num, first_link_id, last_link_id, last_link_dir, key_string'),
                      ('rdb_guideinfo_road_structure', 'guideinfo_id', 'in_link_id, node_id, out_link_id, type_code, structure_name'),
                      ('rdb_guideinfo_building_structure', 'guideinfo_id', 'in_link_id, node_id, out_link_id, type_code, centroid_lontitude, centroid_lantitude, building_name'),
                      ]
        for (table_name, gid, order_attrs) in table_list:
            sqlcmd = """
                    alter table %s drop column if exists order_id CASCADE;
                    alter table %s add column order_id integer;
                    update %s as a set order_id = b.binary_order_id
                    from
                    (
                        select *, row_number() over (order by %s) as binary_order_id
                        from %s
                    ) as b
                    where a.%s = b.%s;
                    """ % (table_name, table_name, table_name, order_attrs, table_name, gid, gid)
            self.pg.execute2(sqlcmd)
            self.pg.commit2()

    def _createViewForCheckLogic(self):
        sqlcmd = """
                ----------------------------------------------------------------------
                -- view_cond_speed
                ----------------------------------------------------------------------
                CREATE OR REPLACE VIEW view_cond_speed AS
                SELECT rdb_cond_speed.*,  the_geom
                  FROM rdb_cond_speed
                  LEFT JOIN rdb_link
                  ON rdb_cond_speed.link_id = rdb_link.link_id;


                ----------------------------------------------------------------------
                -- view_guideinfo_lane
                ----------------------------------------------------------------------
                CREATE OR REPLACE VIEW view_guideinfo_lane AS
                SELECT guideinfo_id, in_link_id, in_link_id_t, ln.node_id, ln.node_id_t,
                       out_link_id, out_link_id_t, lane_num, lane_info, arrow_info,
                       lane_num_l, lane_num_r, passlink_count, the_geom
                  FROM rdb_guideinfo_lane as ln
                  LEFT JOIN rdb_node
                  ON ln.node_id = rdb_node.node_id;

                ----------------------------------------------------------------------
                -- view_guideinfo_signpost
                ----------------------------------------------------------------------
                CREATE OR REPLACE VIEW view_guideinfo_signpost AS
                SELECT A.gid, in_link_id, in_link_id_t, A.node_id, A.node_id_t, out_link_id,
                       out_link_id_t, sp_name, passlink_count, pattern_id, arrow_id, the_geom
                  FROM rdb_guideinfo_signpost as A
                  LEFT JOIN rdb_node
                  ON A.node_id = rdb_node.node_id;

                ----------------------------------------------------------------------
                -- view_guideinfo_spotguidepoint
                ----------------------------------------------------------------------
                CREATE OR REPLACE VIEW view_guideinfo_spotguidepoint AS
                SELECT A.gid, in_link_id, in_link_id_t, A.node_id, A.node_id_t, out_link_id,
                       out_link_id_t, "type", passlink_count, pattern_id, arrow_id, the_geom
                  FROM rdb_guideinfo_spotguidepoint as A
                  LEFT JOIN rdb_node
                  ON A.node_id = rdb_node.node_id;


                ----------------------------------------------------------------------
                -- view_link_add_info
                ----------------------------------------------------------------------
                CREATE OR REPLACE VIEW view_link_add_info AS
                SELECT gid, A.link_id, A.link_id_t, struct_code, shortcut_code, parking_flag,
                       etc_lane_flag, the_geom
                  FROM rdb_link_add_info as A
                  LEFT JOIN rdb_link
                  ON A.link_id = rdb_link.link_id;

                ----------------------------------------------------------------------
                -- view_linklane_info
                ----------------------------------------------------------------------
                CREATE OR REPLACE VIEW view_linklane_info AS
                SELECT link_id, rdb_link.lane_id, disobey_flag, lane_dir, lane_up_down_distingush, ops_lane_number,
                       ops_width, neg_lane_number, neg_width, the_geom
                  FROM rdb_link
                  LEFT JOIN rdb_linklane_info
                  ON rdb_link.lane_id = rdb_linklane_info.lane_id;
                """

        # sqlcmd = self.pg.ReplaceDictionary(sqlcmd)
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _CreateDictionaryView(self):
        '''创建名称字典View'''

        # 取得所有字典表名
        name_list = self._GetDictionaryTblNames()
        if len(name_list) == 0:
            return 0

        # 创建View
        sqlcmd = "CREATE OR REPLACE VIEW rdb_name_dictionary_view AS ("
        name_cnt = 0
        while name_cnt < len(name_list):
            if name_cnt > 0:
                sqlcmd = sqlcmd + " UNION "
            sqlcmd = sqlcmd + " SELECT name_id, name, '%s' as language from %s" % (name_list[name_cnt], name_list[name_cnt])
            name_cnt += 1
        sqlcmd = sqlcmd + " );"

        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        return 0

    def _GetDictionaryTblNames(self):
        '''取得所有字典表名'''
        sqlcmd = """SELECT  tablename
                   FROM  pg_tables
                   where tablename like 'rdb_name_dictionary%'
                   order by tablename;
                """
        self.pg.execute2(sqlcmd)
        name_list = []
        rows = self.pg.fetchall2()
        for row in rows:
            name_list.append(row[0])
        return name_list
