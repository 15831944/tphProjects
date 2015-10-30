# -*- coding: UTF8 -*-
'''
Created on 2015-10-14

@author: liushengqiang
'''



import platform.TestCase
import common.networkx
import common.ConfigReader
import copy

distance_limit = {
                  ('rdf', 'hkg') : {'4' : 10000.0, '6' : 50000.0},
                  ('default') : {'4' : 10000.0, '6' : 50000.0}
                  }

class CCheckAccessibility(platform.TestCase.CTestCase):
    
    def _do(self):
        
        # 验证Region各层道路连通性
        region_layer_list = self.pg.GetRegionLayers()
        for layer_no in region_layer_list:
            self._check_region_layer_road_network_accessibility(layer_no)
        
        return True
        
    def _check_region_layer_road_network_accessibility(self, layer_no = '4'):
        
        # 创建表单temp_region_accessibility_layerX_tbl记录region层任意两Node间的可达性
        # Region层Node的选择因协议、仕向地、间距有所不同
        # 包括如下信息：起始Node、终止Node、region层路径id（可为空）、level 14路径id（可为空）、是否需要改善标记（True：需要改善 / False：不需要改善）
        
        # 创建表单temp_region_accessibility_path_layerX_tbl记录路径关联的link信息
        # 为在postgis上查看路径，该表增加形点信息
        
        sqlcmd = """
                DROP TABLE IF EXISTS temp_region_accessibility_layer%s_tbl;
                CREATE TABLE temp_region_accessibility_layer%s_tbl 
                (
                    gid serial primary key,
                    search_s_node bigint not null,
                    search_e_node bigint not null,
                    region_path_id bigint,
                    path_14_id bigint,
                    improving boolean
                );
                
                DROP TABLE IF EXISTS temp_region_accessibility_path_layer%s_tbl;
                CREATE TABLE temp_region_accessibility_path_layer%s_tbl
                (
                    gid serial primary key,
                    path_id bigint not null,
                    seq_num integer,
                    link_id bigint,
                    the_geom geometry
                );
            """ % (str(layer_no), str(layer_no), str(layer_no), str(layer_no))
        
        self.pg.execute(sqlcmd)
        self.pg.commit()
        
        insert_sqlcmd1 = """
                INSERT INTO temp_region_accessibility_layer%s_tbl (
                    search_s_node, search_e_node, region_path_id, path_14_id, improving
                )
                VALUES (%s, %s, %s, %s, %s)
            """
        
        insert_sqlcmd2 = """
                INSERT INTO temp_region_accessibility_path_layer%s_tbl (
                    path_id, seq_num, link_id
                )
                VALUES (%s, %s, %s)
            """
        
        graph = common.networkx.CGraph_PG()
        
        distance_limit = self._get_distance_limit(layer_no)
        self._prepare_route_calc_data(layer_no)
        
        sqlcmd = """
                SELECT search_inlink, search_start_node, search_outlink, search_end_node, distance 
                FROM temp_region_route_link_layer%s_tbl
            """ % (str(layer_no))
        
        self.pg.execute(sqlcmd)
        rows = self.pg.fetchall()
        
        region_link_tbl = """rdb_region_link_layer%s_tbl""" % str(layer_no)
        region_link_lane_info_tbl = """rdb_region_link_lane_info_layer%s_tbl""" % str(layer_no)
        path_id = 0
        for row in rows:
            path_exist = False
            
            try:
                paths = graph.searchShortestPaths(row[0], row[2], row[1], row[3], row[4], region_link_tbl, region_link_lane_info_tbl)
                if not paths:
                    pass
                else:
                    path_exist = True 
                    for (cur_distance, path) in paths:
                        path_id = path_id + 1
                        seq_num = 0
                        for x in path:
                            self.pg.execute(insert_sqlcmd2, (int(layer_no), path_id, seq_num, x))
                            seq_num = seq_num + 1
                        
                        self.pg.execute(insert_sqlcmd1, (int(layer_no), row[1], row[3], path_id, None, False))
                        self.logger.error('layer_no: ' + str(layer_no) + '---From Node ' + str(row[1]) + ' To Node ' + str(row[3]) + ' can be reached !!!')
                    continue
            except:
                continue
            
            if path_exist == False:
                # search path in level14. if exists the path, region network must be improved
                (path_exist, paths) = self._check_level14_road_network_accessibility(row[1], row[3])
                if False == path_exist:
                    self.pg.execute(insert_sqlcmd1, (int(layer_no), row[1], row[3], None, None, False))
                    self.logger.error('layer_no: ' + str(layer_no) + '---From Node ' + str(row[1]) + ' To Node ' + str(row[3]) + ' can not be reached !!!')
                else: 
                    for (cur_distance, path) in paths:
                        path_id = path_id + 1
                        seq_num = 0
                        for x in path:
                            self.pg.execute(insert_sqlcmd2, (int(layer_no), path_id, seq_num, x))
                            seq_num = seq_num + 1
                            
                        self.pg.execute(insert_sqlcmd1, (int(layer_no), row[1], row[3], None, path_id, True))                
                    self.logger.error('layer_no: ' + str(layer_no) + '---From Node ' + str(row[1]) + ' To Node ' + str(row[3]) + ' region path need be improved !!!')
                    
        self.pg.commit()
        
        # 更新路径表中的形点信息
        self._update_the_geom_in_path_tbl(layer_no)
        
        return True
    
    def _get_distance_limit(self, layer_no = 4):
        
        # 不同协议不同仕向地不同Region层对应的Node间间距不一样
        proj_name = common.ConfigReader.CConfigReader.instance().getProjName().lower()
        proj_country = common.ConfigReader.CConfigReader.instance().getCountryName().lower()
        
        if distance_limit.has_key((proj_name, proj_country)):
            return distance_limit[(proj_name, proj_country)][layer_no]
        elif distance_limit.has_key(proj_name):
            return distance_limit[proj_name][layer_no]
        else:
            return distance_limit['default'][layer_no]
        
    def _update_the_geom_in_path_tbl(self, layer_no = '4'):
        
        sqlcmd = """
                DROP INDEX IF EXISTS temp_region_accessibility_path_layer%s_tbl_link_id_idx;
                CREATE INDEX temp_region_accessibility_path_layer%s_tbl_link_id_idx
                  ON temp_region_accessibility_path_layer%s_tbl
                  USING btree
                  (link_id);
                  
                UPDATE temp_region_accessibility_path_layer%s_tbl a
                SET a.the_geom = b.the_geom
                FROM rdb_region_link_layer%s_tbl b
                WHERE a.link_id = b.link_id;
                
                UPDATE temp_region_accessibility_path_layer%s_tbl a
                SET a.the_geom = b.the_geom
                FROM rdb_link b
                WHERE a.link_id = b.link_id;
            """ % (str(layer_no), str(layer_no), str(layer_no), str(layer_no), str(layer_no), str(layer_no))
        
        self.pg.execute(sqlcmd)
        self.pg.commit()
            
        return True
    
    def _check_level14_road_network_accessibility(self, s_node = None, e_node = None):
        
        sqlcmd = """
                SELECT search_inlink, search_start_node, search_outlink, search_end_node, distance 
                FROM temp_route_link_tbl
                WHERE old_f_node = %s and old_t_node = %s
            """ % (str(s_node), str(e_node))
        
        self.pg.execute(sqlcmd)
        rows = self.pg.fetchall()
        
        graph = common.networkx.CGraph_PG()
        
        for row in rows:
            try:
                paths = graph.searchShortestPaths(row[0], row[2], row[1], row[3], row[4])
                
                if not paths:
                    continue
                else:
                    return (True, paths)
            except:
                continue
            
        return (False, [])
    
    def _prepare_route_calc_data(self, layer_no = '4', distance_limit = '10000.0', delete_road_type = '(7, 8, 9, 14)'):
        
        sqlcmd = """
                DROP TABLE IF EXISTS temp_region_selectd_node_layer%s_tbl;
                CREATE TABLE temp_region_selectd_node_layer%s_tbl
                (
                    node_id bigint,
                    the_geom geometry
                );
            """ % (str(layer_no), str(layer_no))
        
        self.pg.execute(sqlcmd)
        self.pg.commit()
        
        # 根据不同仕向地layer作成情况的不同，算路选择点的获取方式也会不同
        area = common.ConfigReader.CConfigReader.instance().getCountryName()
        if area.upper() in ('HKG'):
            self._select_road_termination_node_in_tile2(layer_no)
        else:
            pass
        
        # temp_region_route_link_layerX_tbl 算路的起始region link、终止region link、起始region Node、终止region Node以及算路起始/终止Node之间的距离
        sqlcmd = """
                DROP TABLE IF EXISTS temp_region_route_link_layer%s_tbl;
                CREATE TABLE temp_region_route_link_layer%s_tbl
                AS (
                    SELECT d.link_id as search_inlink, c.f_node as search_start_node, 
                        e.link_id as search_outlink, c.t_node as search_end_node, distance
                    FROM (
                        SELECT a.node_id as f_node, b.node_id as t_node,
                            ST_Distance_Sphere(a.the_geom, b.the_geom) as distance
                        FROM temp_region_selectd_node_layer%s_tbl a
                        LEFT JOIN temp_region_selectd_node_layer%s_tbl b
                            ON a.node_id != b.node_id
                        WHERE ST_Distance_Sphere(a.the_geom, b.the_geom) > %s
                    ) c
                    LEFT JOIN rdb_region_link_layer%s_tbl d
                        ON (c.f_node = d.start_node_id and d.one_way in (1, 3)) or 
                            (c.f_node = d.end_node_id and d.one_way in (1, 2))
                    LEFT JOIN rdb_region_link_layer%s_tbl e
                        ON (c.t_node = e.start_node_id and e.one_way in (1, 2)) or 
                            (c.t_node = e.end_node_id and e.one_way in (1, 3))
                    where d.link_id is not null and e.link_id is not null
                );
            """ % (str(layer_no), str(layer_no), str(layer_no), str(layer_no), 
                   str(distance_limit), str(layer_no), str(layer_no))
        
        self.pg.execute(sqlcmd)
        self.pg.commit()

        sqlcmd = """
                DROP TABLE IF EXISTS temp_route_link_tbl;
                CREATE TABLE temp_route_link_tbl
                AS (
                    SELECT g.link_id as search_inlink, f.f_node as search_start_node, 
                        h.link_id as search_outlink, f.t_node as search_end_node, 
                        old_f_node, old_t_node, f.distance
                    FROM (
                        SELECT c.f_node as old_f_node, d.node_id_14 as f_node, 
                            c.t_node as old_t_node, e.node_id_14 as t_node, c.distance
                        FROM (
                            SELECT a.node_id as f_node, b.node_id as t_node,
                                ST_Distance_Sphere(a.the_geom, b.the_geom) as distance
                            FROM temp_region_selectd_node_layer%s_tbl a
                            LEFT JOIN temp_region_selectd_node_layer%s_tbl b
                                ON a.node_id != b.node_id
                            WHERE ST_Distance_Sphere(a.the_geom, b.the_geom) > %s
                        ) c
                        LEFT JOIN rdb_region_layer%s_node_mapping d
                            on c.f_node = d.region_node_id
                        LEFT JOIN rdb_region_layer%s_node_mapping e
                            on c.t_node = e.region_node_id
                    ) f
                    LEFT JOIN rdb_link g
                        ON (f.f_node = g.start_node_id and g.one_way in (1, 3)) or 
                            (f.f_node = g.end_node_id and g.one_way in (1, 2))
                    LEFT JOIN rdb_link h
                        ON (f.t_node = h.start_node_id and h.one_way in (1, 2)) or 
                            (f.t_node = h.end_node_id and h.one_way in (1, 3))
                    WHERE g.link_id is not null and 
                        h.link_id is not null and 
                        g.road_type not in %s and 
                        h.road_type not in %s
                );
                
                DROP INDEX IF EXISTS temp_route_link_tbl_old_f_node_old_t_node_idx;
                CREATE INDEX temp_route_link_tbl_old_f_node_old_t_node_idx
                  ON temp_route_link_tbl
                  USING btree
                  (old_f_node, old_t_node);
            """ % (str(layer_no), str(layer_no), str(distance_limit), str(layer_no), 
                   str(layer_no), str(delete_road_type), str(delete_road_type))
        
        self.pg.execute(sqlcmd)
        self.pg.commit()
    
    def _select_max_degree_nodes_in_tile(self, region_selected_node_tbl, region_node_tbl):
        
        # 获取一个Tile内度最大的所有Node
        sqlcmd = """
                INSERT INTO %s
                SELECT a.node_id, the_geom
                FROM %s a
                LEFT JOIN (
                    SELECT node_id_t, max(link_num) as max_link_num
                    FROM %s
                    GROUP BY node_id_t
                ) b
                    ON a.node_id_t = b.node_id_t
                WHERE a.link_num = b.max_link_num
                ORDER BY a.node_id_t
            """ % (str(region_selected_node_tbl), str(region_node_tbl), str(region_node_tbl))
        
        self.pg.execute(sqlcmd)
        self.pg.commit()
        
    def _select_single_max_degree_node_in_tile(self, region_selected_node_tbl, region_node_tbl):
        
        # 获取一个Tile内度最大的任意一个Node
        sqlcmd = """
                INSERT INTO %s
                SELECT c.node_id, c.the_geom
                FROM (
                    SELECT node_id_t, array_agg(node_id) as node_list
                    FROM (
                        SELECT node_id_t, node_id
                        FROM %s
                        ORDER BY node_id_t, link_num DESC
                    ) a
                    GROUP BY node_id_t
                ) b
                LEFT JOIN %s c
                    ON node_list[1] = c.node_id
            """ % (str(region_selected_node_tbl), str(region_node_tbl), str(region_node_tbl))
        
        self.pg.execute(sqlcmd)
        self.pg.commit()
    
    def _select_road_termination_node_in_tile(self, region_selected_node_tbl, region_node_tbl):
        
        # 获取一个Tile内道路终止点
        sqlcmd = """
                INSERT INTO %s
                SELECT node_id, the_geom
                FROM %s
                WHERE link_num = 1
            """ % (str(region_selected_node_tbl), str(region_node_tbl))
            
        self.pg.execute(sqlcmd)
        self.pg.commit()
    
    def _select_road_termination_node_in_tile2(self, layer_no = '4'):
        
        # 获取Region层道路终止点时，为防止Node过多，删除因道路分层产生的终止点
        # 香港仕向地采用该方法：因为香港仕向地只有一个Tile，而且度最大的Node又比较集中，因此选择孤立道路点
        # 为使用统一的算路接口，针对道路终止点，取其邻接Node作为算路Node
        
        sqlcmd = """
                INSERT INTO temp_region_selectd_node_layer%s_tbl
                SELECT f.node_id, g.the_geom
                FROM (
                    SELECT DISTINCT (CASE WHEN d.node_id = e.start_node_id THEN e.end_node_id 
                                        ELSE e.start_node_id END) as node_id
                    FROM (
                        SELECT a.node_id
                        FROM rdb_region_node_layer%s_tbl a
                        LEFT JOIN rdb_region_layer%s_node_mapping b
                            ON a.node_id = b.region_node_id
                        LEFT JOIN rdb_node c
                            ON b.node_id_14 = c.node_id
                        WHERE a.link_num = 1 and c.link_num = 1
                    ) d
                    LEFT JOIN rdb_region_link_layer%s_tbl e
                        ON node_id in (start_node_id, end_node_id)
                ) f
                LEFT JOIN rdb_region_node_layer%s_tbl g
                    ON f.node_id = g.node_id
            """ % (str(layer_no), str(layer_no), str(layer_no), str(layer_no), str(layer_no))
        
        self.pg.execute(sqlcmd)
        self.pg.commit()