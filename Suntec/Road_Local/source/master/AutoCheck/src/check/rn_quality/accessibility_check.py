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
                  ('zenrin', 'twn') : {'4' : 10000.0, '6' : 100000.0},
                  ('default') : {'4' : 10000.0, '6' : 50000.0}
                  }

class CCheckAccessibility(platform.TestCase.CTestCase):
    
    def _do(self):
        
        # 验证Region各层道路连通性
        region_layer_list = self.pg.GetRegionLayers()
        
        self._check_region_layers_reachability(region_layer_list)
        self._check_unreachable_regions_node_in_level14(region_layer_list)
        
        for layer_no in region_layer_list:
            self._update_geom(layer_no)
        
        # region层可达性验证完毕后，可能存在不可达的情况，为分析不可达的原因，还需对上述不可达点在14层路网验证其可达性
        # 若Region层不可达点在14层路网可达，需要改善路网分层算法；否则，无需改善
        
        return True
    
    def _check_region_layers_reachability(self, region_layer_list):
        
        # region层道路网可达性验证---
        # region层可达性验证仅使用region层路网，不涉及其他层路网数据，可达性验证失败的留待后续处理
        self.logger.info('Begin proving layer reachability...')
        for layer_no in region_layer_list:
            self._check_region_layer_reachability(layer_no)
        self.logger.info('End proving layer reachability.')
        return True
    
    def _check_unreachable_regions_node_in_level14(self, region_layer_list):
        
        # 针对region层可达性验证失败的案例
        # 针对region层可达性验证失败原因有两种：1、本身不可达； 2、道路分层导致region层缺失某些道路
        # 为分析其原因，需要对失败案例做进一步分析，即使用失败案例在level 14验证其可达性
        # 若level 14层可达，则是由于路网分层导致道路缺失，进而导致region某些点不可达；反之，其自身不可达
        self.logger.info('Begin proving unreachable region nodes in level 14...')
        graph = common.networkx.CGraph_Cache(100)
        for layer_no in region_layer_list:
            self._check_unreachable_region_node_in_level14(graph, layer_no)
        self.logger.info('End proving unreachable region nodes in level 14.')    
        return True
            
    def _check_unreachable_region_node_in_level14(self, graph_object, layer_no = '4'):
        
        self.logger.info(' Begin proving layer ' + str(layer_no) + ' unreachable region nodes in level 14...')
        sqlcmd = """
                SELECT max(path_id)
                FROM temp_region_accessibility_path_layer%s_tbl
            """ % (str(layer_no))
        
        cur_max_path_id = self.pg.getOnlyQueryResult(sqlcmd)
        
        insert_sqlcmd1 = """
                UPDATE temp_region_accessibility_layer%s_tbl a
                SET path_14_id = %s, improving = %s
                WHERE a.gid = %s
            """
        
        insert_sqlcmd2 = """
                INSERT INTO temp_region_accessibility_path_layer%s_tbl (
                    path_id, seq_num, link_id
                )
                VALUES (%s, %s, %s)
            """
            
        sqlcmd = """
                SELECT a.gid, 
                    d.node_id as search_s_node, d.node_id_t as search_s_node_t, 
                    e.node_id as search_e_node, e.node_id_t as search_e_node_t,
                    ST_Distance_Sphere(d.the_geom, e.the_geom) as distance
                FROM temp_region_accessibility_layer%s_tbl a
                LEFT JOIN rdb_region_layer%s_node_mapping b
                    ON a.search_s_node = b.region_node_id
                LEFT JOIN rdb_region_layer%s_node_mapping c
                    ON a.search_e_node = c.region_node_id
                LEFT JOIN rdb_node d
                    ON b.node_id_14 = d.node_id
                LEFT JOIN rdb_node e
                    ON c.node_id_14 = e.node_id
                WHERE a.improving is null
            """ % (str(layer_no), str(layer_no), str(layer_no))
        
        self.pg.execute(sqlcmd)
        rows = self.pg.fetchall()
        
        path_id = cur_max_path_id 
        row_cnt = len(rows)
        row_idx = 0 
        
        for row in rows:
            rec_gid = row[0]
            search_s_node = row[1]
            search_s_node_t = row[2]
            search_e_node = row[3]
            search_e_node_t = row[4]
            distance = row[5]
            
            row_idx = row_idx + 1
            self.logger.info('  layer_no: ' + str(layer_no) + '---dealing ' + str(row_idx) + ' / ' + str(row_cnt))
            
            try:
                paths = graph_object.searchShortestPaths(search_s_node, search_s_node_t, search_e_node, search_e_node_t, distance)
                if not paths:
                    self.pg.execute(insert_sqlcmd1, (int(layer_no), None, False, rec_gid))
                else:
                    for (cur_distance, path) in paths:
                        path_id += 1
                        seq_num = 0
                        for link_id in path:
                            self.pg.execute(insert_sqlcmd2, (int(layer_no), path_id, seq_num, link_id))
                            seq_num += 1
                                
                        self.pg.execute(insert_sqlcmd1, (int(layer_no), path_id, True, rec_gid))
                        break # 仅取一条link即可
            except:
                self.logger.error('  layer_no: ' + str(layer_no) + '---exception happened From Node ' + str(row[1]) + ' To Node ' + str(row[3]) + ' route calculation in level 14!!!')
                raise
        
        self.pg.commit()
        
        self.logger.info(' End proving layer ' + str(layer_no) + ' unreachable region nodes in level 14.')   
        return True
        
    def _check_region_layer_reachability(self, layer_no = '4'):
        
        # 该步仅验证Region层的可达性，若Region层不可达，记为不可达，后期处理
        # 创建表单temp_region_accessibility_layerX_tbl记录region层任意两Node间的可达性
        # Region层Node的选择因协议、仕向地、间距有所不同
        # 包括如下信息：起始Node、终止Node、region层路径id（可为空）、level 14路径id（可为空）、是否需要改善标记（True：需要改善 / False：不需要改善）
        
        # 创建表单temp_region_accessibility_path_layerX_tbl记录路径关联的link信息
        # 为在postgis上查看路径，该表增加形点信息
        self.logger.info(' Begin proving layer ' + str(layer_no) + ' region link reachability...')
        
        sqlcmd = """
                DROP TABLE IF EXISTS temp_region_accessibility_layer%s_tbl;
                CREATE TABLE temp_region_accessibility_layer%s_tbl 
                (
                    gid serial primary key,
                    search_s_node bigint not null,
                    search_e_node bigint not null,
                    distance double precision,
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
                    search_s_node, search_e_node, distance, region_path_id, path_14_id, improving
                )
                VALUES (%s, %s, %s, %s, %s, %s)
            """
        
        insert_sqlcmd2 = """
                INSERT INTO temp_region_accessibility_path_layer%s_tbl (
                    path_id, seq_num, link_id
                )
                VALUES (%s, %s, %s)
            """
            
        self._preparing_data(layer_no)

        graph = common.networkx.CGraph_Cache(10)
        
        sqlcmd = """
                SELECT search_start_node, search_start_node_t, search_end_node, search_end_node_t, distance 
                FROM temp_region_route_link_layer%s_tbl
                ORDER BY distance
            """ % (str(layer_no))
        
        self.pg.execute(sqlcmd)
        rows = self.pg.fetchall()
        
        path_id = 0 
        row_cnt = len(rows) 
        row_idx = 0 
        region_link_tbl = """rdb_region_link_layer%s_tbl""" % (str(layer_no))
        region_node_tbl = """rdb_region_node_layer%s_tbl""" % (str(layer_no))
        region_link_laneinfo_tbl = """rdb_region_link_lane_info_layer%s_tbl""" % (str(layer_no))
        for row in rows:
            
            row_idx += 1
            self.logger.info('   layer_no: ' + str(layer_no) + '---dealing ' + str(row_idx) + ' / ' + str(row_cnt))
            
            search_start_node = row[0]
            search_start_node_t = row[1]
            search_end_node = row[2]
            search_end_node_t = row[3]
            distance = row[4]
            
            try:
                paths = graph.searchShortestPaths(search_start_node,
                                                  search_start_node_t,
                                                  search_end_node,
                                                  search_end_node_t,
                                                  distance,
                                                  region_link_tbl,
                                                  region_node_tbl,
                                                  region_link_laneinfo_tbl)
                
                if not paths:
                    self.pg.execute(insert_sqlcmd1, (int(layer_no), search_start_node, search_end_node, None, None, None, None))
                else:
                    for (cur_distance, path) in paths:
                        path_id += 1
                        seq_num = 0
                        for linkid in path:
                            self.pg.execute(insert_sqlcmd2, (int(layer_no), path_id, seq_num, linkid))
                            seq_num += 1
                        self.pg.execute(insert_sqlcmd1, (int(layer_no), search_start_node, search_end_node, cur_distance, path_id, None, False))
                        break # just get only one path
            except:
                self.logger.error('   layer_no: ' + str(layer_no) + '---exception happened From Node ' + str(row[1]) + ' To Node ' + str(row[3]) + ' route calculation !!!')
                raise
        
        self.pg.commit()
        
        self.logger.info(' End proving layer ' + str(layer_no) + ' region link reachability.')
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
        
    def _update_geom(self, layer_no = '4'):
        
        self.logger.info('Begin updating path link geom...')
        
        sqlcmd = """
                DROP INDEX IF EXISTS temp_region_accessibility_path_layer%s_tbl_link_id_idx;
                CREATE INDEX temp_region_accessibility_path_layer%s_tbl_link_id_idx
                  ON temp_region_accessibility_path_layer%s_tbl
                  USING btree
                  (link_id);
                  
                UPDATE temp_region_accessibility_path_layer%s_tbl a
                SET the_geom = b.the_geom
                FROM rdb_region_link_layer%s_tbl b
                WHERE a.link_id = b.link_id;
                
                UPDATE temp_region_accessibility_path_layer%s_tbl a
                SET the_geom = b.the_geom
                FROM rdb_link b
                WHERE a.link_id = b.link_id;
            """ % (str(layer_no), str(layer_no), str(layer_no), str(layer_no), str(layer_no), str(layer_no))
        
        self.pg.execute(sqlcmd)
        self.pg.commit()
        
        self.logger.info('End updating path link geom.')  
        return True
    
    def _check_level14_road_network_accessibility1(self, s_node = None, e_node = None):
        
        sqlcmd = """
                SELECT search_start_node, search_end_node, distance, search_inlink_list, search_outlink_list 
                FROM temp_route_link_tbl
                WHERE old_f_node = %s and old_t_node = %s
            """ % (str(s_node), str(e_node))
        
        self.pg.execute(sqlcmd)
        rows = self.pg.fetchall()
        
        graph = common.networkx.CGraph_PG()
        
        for row in rows:
            search_start_node = row[0]
            search_end_node = row[1]
            distance = row[2]
            search_inlink_list = row[3]
            search_outlink_list = row[4]
            linklist_len = len(search_inlink_list)
            for i in range(0, linklist_len):
                search_inlink = search_inlink_list[i]
                search_outlink = search_outlink_list[i]
                
                try:
                    paths = graph.searchShortestPaths(search_inlink, search_outlink, search_start_node, search_end_node, distance)
                    if not paths:
                        continue
                    else:
                        return (True, paths)
                except:
                    continue
            
        return (False, [])
    
    def _preparing_data(self, layer_no = '4', delete_road_type = '(7, 8, 9, 14)'):
        
        self.logger.info('  Begin preparing reachability verification data...')
        
        sqlcmd = """
                DROP TABLE IF EXISTS temp_region_selectd_node_layer%s_tbl;
                CREATE TABLE temp_region_selectd_node_layer%s_tbl
                (
                    node_id bigint not null,
                    node_id_t integer not null,
                    the_geom geometry
                );
            """ % (str(layer_no), str(layer_no))
        
        self.pg.execute(sqlcmd)
        self.pg.commit()
        
        # 根据不同仕向地layer作成情况的不同，算路选择点的获取方式也会不同
        area = common.ConfigReader.CConfigReader.instance().getCountryName()
        if area.lower() in ('hkg'):
            self._select_road_termination_node_in_tile2(layer_no)
        elif area.lower() in ('twn'):
            if layer_no == '4':
                self._select_single_max_degree_node_in_tile(layer_no)
            else:
                # 台湾layer 6仅有一个道路终止点
                self._select_max_degree_nodes_in_tile(layer_no)
                self._select_road_termination_node_in_tile2(layer_no)
        else:
            pass
        
        distance_limit_val = self._get_distance_limit(layer_no)
        # temp_region_route_link_layerX_tbl 算路的起始region link、终止region link、起始region Node、终止region Node以及算路起始/终止Node之间的距离
        sqlcmd = """
                DROP TABLE IF EXISTS temp_region_route_link_layer%s_tbl;
                CREATE TABLE temp_region_route_link_layer%s_tbl
                AS (
                    SELECT search_start_node, search_start_node_t, search_end_node, search_end_node_t, distance,
                        array_agg(search_inlink) as search_inlink_list,
                        array_agg(search_outlink) as search_outlink_list
                    FROM (
                        SELECT d.link_id as search_inlink, 
                            c.f_node as search_start_node, 
                            c.f_node_t as search_start_node_t,
                            e.link_id as search_outlink, 
                            c.t_node as search_end_node, 
                            c.t_node_t as search_end_node_t, distance
                        FROM (
                            SELECT a.node_id as f_node, a.node_id_t as f_node_t,
                                b.node_id as t_node, b.node_id_t as t_node_t,
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
                        WHERE d.link_id is not null and e.link_id is not null
                        ORDER BY c.f_node, c.t_node, distance
                    ) f
                    GROUP BY search_start_node, search_start_node_t, search_end_node, search_end_node_t, distance
                );
            """ % (str(layer_no), str(layer_no), str(layer_no), str(layer_no), 
                   str(distance_limit_val), str(layer_no), str(layer_no))
        
        self.pg.execute(sqlcmd)
        self.pg.commit()
        
        self.logger.info('  End preparing reachability verification data.')
        return 
    
    def _select_max_degree_nodes_in_tile(self, layer_no = '4'):
        
        # 获取一个Tile内度最大的所有Node
        sqlcmd = """
                INSERT INTO temp_region_selectd_node_layer%s_tbl
                SELECT b.node_id, b.node_id_t, b.the_geom
                FROM (
                    SELECT node_id_t, max(link_num) as max_link_num
                    FROM rdb_region_node_layer%s_tbl
                    GROUP BY node_id_t
                ) a
                LEFT JOIN rdb_region_node_layer%s_tbl b
                    ON a.node_id_t = b.node_id_t
                WHERE a.max_link_num = b.link_num
            """ % (str(layer_no), str(layer_no), str(layer_no))
        
        self.pg.execute(sqlcmd)
        self.pg.commit()
        
    def _select_single_max_degree_node_in_tile(self, layer_no = '4'):
        
        # 获取一个Tile内度最大的任意一个Node
        sqlcmd = """
                INSERT INTO temp_region_selectd_node_layer%s_tbl
                SELECT c.node_id, c.node_id_t, c.the_geom
                FROM (
                    SELECT node_id_t, array_agg(node_id) as node_list
                    FROM (
                        SELECT node_id_t, node_id
                        FROM rdb_region_node_layer%s_tbl
                        ORDER BY node_id_t, link_num DESC
                    ) a
                    GROUP BY node_id_t
                ) b
                LEFT JOIN rdb_region_node_layer%s_tbl c
                    ON node_list[1] = c.node_id
            """ % (str(layer_no), str(layer_no), str(layer_no))
        
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
                SELECT g.node_id, g.node_id_t, g.the_geom
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