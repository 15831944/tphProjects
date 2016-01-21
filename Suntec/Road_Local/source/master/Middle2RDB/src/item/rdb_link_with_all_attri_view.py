# -*- coding: UTF8 -*-
'''
Created on 2013-12-17

@author: hongchenzai
'''
from base.rdb_ItemBase import ItemBase
from common import rdb_log


class rdb_link_with_all_attri_view(ItemBase):
    '''
    classdocs
    '''
    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self,
                          'Link_With_All_Attri_View',
                          'rdb_link',
                          'gid',
                          'rdb_link_with_all_attri_view',
                          'gid',
                          False)

    def Do_CreateTable(self):
        self.CreateTable2('rdb_link_with_all_attri_view')

    def Do(self):
        self.CreateFunction2('rdb_calc_length_by_unit')  # 变换长度单位
        
        #
        sqlcmd = """
                -- add section for regulaion flag in rdb_link
                -- 0: without regulation
                -- 1: with regulation and overide in first link
                -- 2: with regualtion and override in last link
                -- 3: with regulation and both in first/last link
                DROP TABLE IF EXISTS temp_rdb_link_regulation_exist_state CASCADE;
                
                SELECT link_id, rdb_split_tileid(link_id) AS link_id_32, rdb_split_to_tileid(link_id) AS link_id_32_t, 
                    (CASE 
                        WHEN array_length(regulation_exist_state_array,1) = 2 then 3 
                        ELSE regulation_exist_state_array[1] 
                    END) AS regulation_exist_state
                INTO temp_rdb_link_regulation_exist_state 
                FROM ( 
                    SELECT link_id, array_agg(regulation_exist_state) as regulation_exist_state_array 
                    FROM ( 
                        SELECT link_id, regulation_exist_state 
                        FROM ( 
                            SELECT first_link_id as link_id, 1 AS regulation_exist_state 
                            FROM rdb_link_regulation
                            
                            UNION
                             
                            SELECT last_link_id as link_id, 2 AS regulation_exist_state 
                            FROM rdb_link_regulation 
                        ) a 
                        ORDER BY link_id, regulation_exist_state
                    ) b
                    GROUP BY link_id
                ) c;
                
                CREATE INDEX temp_rdb_link_regulation_exist_state_link_id_idx
                    ON temp_rdb_link_regulation_exist_state
                    USING btree
                    (link_id);
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        sqlcmd = """
                DROP TABLE IF EXISTS temp_rdb_link_bigint_2_int_mapping CASCADE;
        
                SELECT link_id ,link_id_t, rdb_split_tileid(link_id) as link_id_32 INTO temp_rdb_link_bigint_2_int_mapping FROM rdb_link;
                
                CREATE INDEX temp_rdb_link_bigint_2_int_mapping_linkid_idx
                    ON temp_rdb_link_bigint_2_int_mapping
                    USING btree
                    (link_id);
                
                CREATE INDEX temp_rdb_link_bigint_2_int_mapping_linkid_32_idx
                    ON temp_rdb_link_bigint_2_int_mapping
                    USING btree
                    (link_id_32);
                
                CREATE INDEX temp_rdb_link_bigint_2_int_mapping_linkid_t_idx
                    ON temp_rdb_link_bigint_2_int_mapping
                    USING btree
                    (link_id_t);
            """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        self.CreateFunction2('rdb_merge_common_link_attribute') 
        self.CreateFunction2('rdb_smallint_2_octal_bytea')
        self.CreateFunction2('rdb_integer_2_octal_bytea')
        self.CreateFunction2('rdb_makeshape2bytea')
        self.CreateFunction2('rdb_split_tileid')
        self.CreateFunction2('lonlat2pixel_tile')
        self.CreateFunction2('lonlat2pixel')
        self.CreateFunction2('tile_bbox')
        self.CreateFunction2('world2lonlat')
        self.CreateFunction2('lonlat2world')
        self.CreateFunction2('pixel2world')       
        self.CreateFunction2('rdb_makeshape2pixelbytea')
        
        sqlcmd = """
                DROP TABLE IF EXISTS temp_forecast_link_id_tbl;
                CREATE TABLE temp_forecast_link_id_tbl
                (
                    link_id bigint not null primary key
                );
                
                INSERT INTO temp_forecast_link_id_tbl (
                    link_id)
                (
                    SELECT DISTINCT link_id FROM rdb_forecast_link
                )
            """
        
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        sqlcmd = """
                INSERT INTO rdb_link_with_all_attri_view (gid, link_id, link_id_t, display_class, 
                    start_node_id, start_node_id_t,end_node_id, end_node_id_t, road_type, one_way, 
                    function_code, link_length, road_name, road_name_id, road_number, lane_id, toll, 
                    regulation_flag, tilt_flag, speed_regulation_flag, link_add_info_flag, fazm, tazm, fnlink, 
                    fnlink_t, tnlink, tnlink_t, link_type, extend_flag, bypass_flag, matching_flag, highcost_flag, 
                    fazm_path, tazm_path, shield_flag, regulation_exist_state, link_length_modify, link_length_unit, 
                    geom_blob, pdm_flag, common_main_link_attri, pass_side, admin_wide_regulation, region_cost, 
                    reserve, width, s_sequence_link_id, e_sequence_link_id, abs_link_id, forecast_flag, the_geom)
                (
                    SELECT a.gid, a.link_id, a.link_id_t, a.display_class,
                        a.start_node_id, a.start_node_id_t, a.end_node_id, a.end_node_id_t, a.road_type, a.one_way,
                        a.function_code, a.link_length, a.road_name, a.road_name_id, a.road_number, a.lane_id, a.toll,
                        a.regulation_flag, a.tilt_flag, a.speed_regulation_flag, a.link_add_info_flag, a.fazm,
                        a.tazm, a.fnlink, a.fnlink_t, a.tnlink, a.tnlink_t, a.link_type, a.extend_flag, a.bypass_flag, 
                        a.matching_flag, a.highcost_flag, a.fazm_path, a.tazm_path, a.shield_flag,
                        (CASE 
                            WHEN b.regulation_exist_state IS NULL THEN 0 
                            ELSE b.regulation_exist_state 
                        END) AS regulation_exist_state,
                        (rdb_calc_length_by_unit(a.link_length))[2] AS link_length_modify,
                        (rdb_calc_length_by_unit(a.link_length))[1] AS link_length_unit,
                        --rdb_makeshape2pixelbytea(14::smallint, (a.link_id_t >> 14) & 16383, (a.link_id_t & 16383), a.the_geom ) AS geom_blob,
                        NULL AS geom_blob, -- no use and set null
                        (CASE 
                            WHEN ((a.extend_flag >> 1) & 1::smallint) = 1 THEN true 
                            ELSE false 
                        END) AS pdm_flag, 
                        rdb_merge_common_link_attribute(a.extend_flag, a.toll, a.function_code, a.road_type, a.display_class),
                        a.extend_flag::integer & 1 AS pass_side,
                        (a.extend_flag >> 1)::integer & 1 AS admin_wide_regulation,
                        (a.extend_flag >> 2)::integer & 7 AS region_cost,
                        (a.extend_flag >> 5)::integer & 2047 AS reserve,
                        (CASE 
                            WHEN a.one_way in (0,1) THEN (4 - d.ops_width) 
                            WHEN a.one_way = 2 THEN (4 - d.ops_width) 
                            WHEN a.one_way = 3 THEN (4 - d.neg_width) 
                            ELSE 0 
                        END) AS width,
                        (CASE WHEN e.link_id is null THEN -1 ELSE e.s_link_id END) AS s_sequence_link_id,
                        (CASE WHEN e.link_id is null THEN -1 ELSE e.e_link_id END) AS e_sequence_link_id,
                        (CASE WHEN f.abs_link_id is null THEN 0 ELSE f.abs_link_id END) AS abs_link_id,
                        (CASE WHEN g.link_id is null THEN false ELSE true END) AS forecast_flag,
                        a.the_geom_4096
                    FROM rdb_link a
                    LEFT JOIN temp_rdb_link_regulation_exist_state b
                        ON a.link_id = b.link_id
                    LEFT JOIN temp_rdb_link_bigint_2_int_mapping m
                        ON a.link_id_t = m.link_id_t AND a.link_id = m.link_id
                    LEFT JOIN rdb_linklane_info d
                        ON a.lane_id = d.lane_id
                    LEFT JOIN rdb_link_sequence e
                        ON a.link_id = e.link_id
                    LEFT JOIN rdb_link_abs f
                        ON a.link_id = f.link_id
                    LEFT JOIN temp_forecast_link_id_tbl g 
                        ON a.link_id = g.link_id
                    WHERE a.gid >= %d AND a.gid <= %d
                );
            """
        
        (min_gid, max_gid) = self.pg.getMinMaxValue('rdb_link', 'gid')
        self.pg.multi_execute(sqlcmd, min_gid, max_gid, 4, 100000)

        self.CreateIndex2('rdb_link_with_all_attri_view_link_id_t_idx')
        self.CreateIndex2('rdb_link_with_all_attri_view_link_id_idx')

        # CLUSTER
        sqlcmd = """
            CLUSTER rdb_link_with_all_attri_view_link_id_t_idx
              ON rdb_link_with_all_attri_view;
            """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()