# -*- coding: UTF8 -*-
'''
Created on 2015-10-14

@author: liushengqiang
'''



import os
import time
import threading
import platform.TestCase
import common.networkx
import common.ConfigReader
import copy
import common.cache_file

distance_limit = {
                  ('rdf', 'hkg') : {'4' : 10000.0, '6' : 10000.0},
                  ('zenrin', 'twn') : {'4' : 10000.0, '6' : 100000.0},
                  ('ni', 'chn') : {'4' : 2000000.0, '6' : 3000000.0},
                  ('default') : {'4' : 10000.0, '6' : 50000.0}
                  }

mutex = threading.Lock()
g_path_id = 0

class CReachabilityCase(threading.Thread):
    
    def __init__(self,
                 caseManager,
                 rec_index,
                 search_start_node, 
                 search_start_node_t, 
                 search_end_node,
                 search_end_node_t,
                 distance,
                 level14_flag = False):
        
        threading.Thread.__init__(self)
        self.objCaseManager = caseManager
        self.objGraph = self.objCaseManager.objGraph
        self.layer_no = self.objCaseManager.layer_no
        self.level14_flag = level14_flag
        self.rec_index = rec_index
        self.link_tbl = """rdb_region_link_layer%s_tbl""" % (str(self.layer_no))
        self.node_tbl = """rdb_region_node_layer%s_tbl""" % (str(self.layer_no))
        self.link_laneinfo_tbl = """rdb_region_link_lane_info_layer%s_tbl""" % (str(self.layer_no))
        self.search_start_node = search_start_node
        self.search_start_node_t = search_start_node_t
        self.search_end_node = search_end_node
        self.search_end_node_t = search_end_node_t
        self.distance = distance
        
        if level14_flag == True:
            strLoggerName = "      level 14 layer_no: %s --- Case %s" % (str(self.layer_no), str(self.rec_index))
        else:
            strLoggerName = "      layer_no: %s --- Case %s" % (str(self.layer_no), str(self.rec_index))
        self.logger = common.Logger.CLogger.instance().logger(strLoggerName)
        self.pg = common.database.CDB()
        self.state = "ACTIVE"
    
    def __getPathBetweenNodes(self):
        
        global g_path_id
        if self.level14_flag == False:
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
    
            try:
                paths = self.objGraph.searchShortestPaths2(self.search_start_node, self.search_start_node_t,  self.search_end_node, self.search_end_node_t, 
                                                           distance = self.distance, get_link_cost = common.networkx.CLinkCost.getCost2,
                                                           link_tbl = self.link_tbl, node_tbl = self.node_tbl, link_laneinfo_tbl = self.link_laneinfo_tbl)              
                
                if not paths:
                    self.pg.execute(insert_sqlcmd1, (int(self.layer_no), self.search_start_node, self.search_end_node, None, None, None, None))
                else:
                    for (cur_distance, path) in paths:
                        
                        if mutex.acquire(1):
                            g_path_id = g_path_id + 1
                            path_id = g_path_id
                            mutex.release()
                            
                        seq_num = 0
                        for linkid in path:
                            self.pg.execute(insert_sqlcmd2, (int(self.layer_no), path_id, seq_num, linkid))
                            seq_num += 1
                        self.pg.execute(insert_sqlcmd1, (int(self.layer_no), self.search_start_node, self.search_end_node, cur_distance, path_id, None, False))
                        break # just get only one path
            except:
                self.logger.error('   layer_no: ' + str(self.layer_no) + '---exception happened From Node ' + str(self.search_start_node) + ' To Node ' + str(self.search_end_node) + ' route calculation !!!')
                raise
            
            self.pg.commit()
        else:
            update_sqlcmd = """
                    UPDATE temp_region_accessibility_layer%s_tbl a
                    SET path_14_id = %s, improving = %s
                    WHERE a.gid = %s
                """
            
            insert_sqlcmd = """
                    INSERT INTO temp_region_accessibility_path_layer%s_tbl (
                        path_id, seq_num, link_id
                    )
                    VALUES (%s, %s, %s)
                """
            
            try:
                paths = self.objGraph.searchShortestPaths2(self.search_start_node, self.search_start_node_t,  self.search_end_node, self.search_end_node_t, 
                                                           distance = self.distance, get_link_cost = common.networkx.CLinkCost.getCost2)
                
                if not paths:
                    self.pg.execute(update_sqlcmd, (int(self.layer_no), None, False, self.rec_index))
                else:
                    for (cur_distance, path) in paths:
                        
                        if mutex.acquire(1):
                            g_path_id = g_path_id + 1
                            path_id = g_path_id
                            mutex.release()

                        seq_num = 0
                        for link_id in path:
                            self.pg.execute(insert_sqlcmd, (int(self.layer_no), path_id, seq_num, link_id))
                            seq_num += 1
                                
                        self.pg.execute(update_sqlcmd, (int(self.layer_no), path_id, True, self.rec_index))
                        break
            except:
                self.logger.error('  layer_no: ' + str(self.layer_no) + '---exception happened From Node ' + str(self.search_start_node) + ' To Node ' + str(self.search_end_node) + ' route calculation in level 14!!!')
                raise
            
            self.pg.commit()
        
        return True
    
    def do(self):
        self.logger.info("Case start...")
        try:
            self.pg.connect()
            if self.__getPathBetweenNodes():
                self.state = "OK"
                self.logger.info("Case OK.")
            else:
                self.state = "FAIL"
                self.logger.error("Case FAIL.")
        except:
            self.state = "FAIL"
            self.logger.exception("Case Fail...")
            self.pg.rollback()
        finally:
            self.pg.close()
        self.logger.info("Case end.")
    
    def start(self):
        self.state = "RUNNING"
        threading.Thread.start(self)
    
    def run(self):
        try:
            self.do()
        finally:
            self.objCaseManager.sem.release()
            del self.objCaseManager.runningcaselist[self.rec_index]
    
    def isActive(self):
        return self.state == "ACTIVE"
    
    def isRunning(self):
        return self.state == "RUNNING"
    
    def join(self, timeout=None):
        if self.isRunning():
            threading.Thread.join(self, timeout)

class CReachabilityCaseManager:
    
    def __init__(self, layer_no, objGraph, level14_flag = False):
        self.cases = {}
        self.layer_no = layer_no
        self.logger = common.Logger.CLogger.instance().logger("   Reachability Case Manager")
        self.pg = common.database.CDB()
        self.pg.connect()
        self.objGraph = objGraph
        self.level14_flag = level14_flag
        
        global g_path_id
        if self.level14_flag == False:  
            g_path_id = 0
        else:
            sqlcmd = """
                    SELECT max(path_id)
                    FROM temp_region_accessibility_path_layer[layer_no]_tbl
                """
            
            sqlcmd = sqlcmd.replace('[layer_no]', self.layer_no)
            g_path_id = self.pg.getOnlyQueryResult(sqlcmd)
        
        pass
    
    def do(self):
        try:
            
            self.__loadCaseList()
            #self.__doAllCase()
            self.__doAllCase_parallel()
            
            return 0
        except:
            self.logger.exception("error happened...")
            return 0
    
    def __loadCaseList(self):
        
        if self.level14_flag == False:
            self.__prepareData()
            
        self.__dataPreprocessing()
        
        return True
    
    def __dataPreprocessing(self):
        
        self.logger.info('Begin preprocessing reachability verification representative point data(layer_no: ' + str(self.layer_no)+ ')...')
        
        if self.level14_flag == False:
            sqlcmd = """
                    SELECT search_start_node, search_start_node_t, search_end_node, search_end_node_t, distance 
                    FROM temp_region_route_node_layer[layer_no]_tbl
                """
            
            sqlcmd = sqlcmd.replace('[layer_no]', self.layer_no)
            self.pg.execute(sqlcmd)
            rows = self.pg.fetchall()
            
            row_idx = 0 
            for row in rows:
                
                row_idx += 1
                search_start_node = row[0]
                search_start_node_t = row[1]
                search_end_node = row[2]
                search_end_node_t = row[3]
                distance = row[4]
                case = CReachabilityCase(self,
                                         row_idx,
                                         search_start_node, 
                                         search_start_node_t, 
                                         search_end_node,
                                         search_end_node_t,
                                         distance)
                self.cases[row_idx] = case
        else:
            sqlcmd = """
                    SELECT a.gid, 
                        d.node_id as search_s_node, d.node_id_t as search_s_node_t, 
                        e.node_id as search_e_node, e.node_id_t as search_e_node_t,
                        ST_Distance_Sphere(d.the_geom, e.the_geom) as distance
                    FROM temp_region_accessibility_layer[layer_no]_tbl a
                    LEFT JOIN rdb_region_layer[layer_no]_node_mapping b
                        ON a.search_s_node = b.region_node_id
                    LEFT JOIN rdb_region_layer[layer_no]_node_mapping c
                        ON a.search_e_node = c.region_node_id
                    LEFT JOIN rdb_node d
                        ON b.node_id_14 = d.node_id
                    LEFT JOIN rdb_node e
                        ON c.node_id_14 = e.node_id
                    WHERE a.improving is null
                """
            
            sqlcmd = sqlcmd.replace('[layer_no]', self.layer_no)
            self.pg.execute(sqlcmd)
            rows = self.pg.fetchall()
            
            for row in rows:
                row_idx = row[0]
                search_start_node = row[1]
                search_start_node_t = row[2]
                search_end_node = row[3]
                search_end_node_t = row[4]
                distance = row[5]
                case = CReachabilityCase(self,
                                         row_idx,
                                         search_start_node, 
                                         search_start_node_t, 
                                         search_end_node,
                                         search_end_node_t,
                                         distance,
                                         self.level14_flag)
                self.cases[row_idx] = case
        
        self.logger.info('End preprocessing reachability verification representative point data(layer_no: ' + str(self.layer_no)+ ').')
        return True
    
    def __doAllCase(self):
        
        self.logger.info("Begin executing caselist...")
        while True:
            for item, value in self.cases.items():
                if value.isActive():
                    value.do()
                    break
            else:
                break
        self.logger.info("End executing caselist.")
        
        return True
    
    def __doAllCase_parallel(self):
        
        self.sem = threading.Semaphore(3)
        self.runningcaselist = {}
        self.logger.info('Begin parallel implementation of reachability test cases(layer_no: ' + str(self.layer_no)+ ')...')
        while True:
            for item, value in self.cases.items():
                if value.isActive():
                    self.sem.acquire()
                    self.runningcaselist[item] = 1
                    value.start()
                    break  
            else:
                if len(self.runningcaselist) == 0:
                    break
                else:
                    time.sleep(1.0)
        self.logger.info('End parallel implementation of reachability test cases(layer_no: ' + str(self.layer_no)+ ').')
        
        return True
    
    def __prepareData(self, delete_road_type = '(7, 8, 9, 14)'):
        
        self.logger.info('Begin preparing reachability verification representative point data(layer_no: ' + str(self.layer_no)+ ')...')
        sqlcmd = """
                DROP TABLE IF EXISTS temp_region_accessibility_layer[layer_no]_tbl;
                CREATE TABLE temp_region_accessibility_layer[layer_no]_tbl 
                (
                    gid serial primary key,
                    search_s_node bigint not null,
                    search_e_node bigint not null,
                    distance double precision,
                    region_path_id bigint,
                    path_14_id bigint,
                    improving boolean
                );
                
                DROP TABLE IF EXISTS temp_region_accessibility_path_layer[layer_no]_tbl;
                CREATE TABLE temp_region_accessibility_path_layer[layer_no]_tbl
                (
                    gid serial primary key,
                    path_id bigint not null,
                    seq_num integer,
                    link_id bigint,
                    the_geom geometry
                );
                
                DROP TABLE IF EXISTS temp_region_selectd_node_layer[layer_no]_tbl;
                CREATE TABLE temp_region_selectd_node_layer[layer_no]_tbl
                (
                    node_id bigint not null,
                    node_id_t integer not null,
                    the_geom geometry
                );
            """
        
        sqlcmd = sqlcmd.replace('[layer_no]', self.layer_no)
        self.pg.execute(sqlcmd)
        self.pg.commit()
        
        self._get_representative_point_data_from_config()
        #self._get_representative_point_data()
        
        distance_limit_val = self._get_distance_limit(self.layer_no)
        
        sqlcmd = """
                DROP INDEX IF EXISTS temp_region_selectd_node_layer[layer_no]_tbl_node_id_idx;
                CREATE INDEX temp_region_selectd_node_layer[layer_no]_tbl_node_id_idx
                    ON temp_region_selectd_node_layer[layer_no]_tbl
                    USING btree
                    (node_id);
                
                analyze temp_region_selectd_node_layer[layer_no]_tbl;
                  
                DROP TABLE IF EXISTS temp_region_route_node_layer[layer_no]_tbl;
                CREATE TABLE temp_region_route_node_layer[layer_no]_tbl
                AS (
                    SELECT a.node_id as search_start_node,
                        a.node_id_t as search_start_node_t,
                        b.node_id as search_end_node,
                        b.node_id_t as search_end_node_t,
                        ST_Distance_Sphere(a.the_geom, b.the_geom) as distance
                    FROM temp_region_selectd_node_layer[layer_no]_tbl a
                    LEFT JOIN temp_region_selectd_node_layer[layer_no]_tbl b
                        ON a.node_id != b.node_id
                    WHERE ST_Distance_Sphere(a.the_geom, b.the_geom) > [distance_limit]
                );
                
                analyze temp_region_route_node_layer[layer_no]_tbl;
            """
        
        sqlcmd = sqlcmd.replace('[layer_no]', self.layer_no)
        sqlcmd = sqlcmd.replace('[distance_limit]', str(distance_limit_val))
        self.pg.execute(sqlcmd)
        self.pg.commit()
        
        self.logger.info('End preparing reachability verification representative point data(layer_no: ' + str(self.layer_no)+ ').')
        return True
    
    def _get_representative_point_data_from_config(self):
        
        sqlcmd = """
                INSERT INTO temp_region_selectd_node_layer[layer_no]_tbl
                SELECT c.node_id, c.node_id_t, c.the_geom
                FROM temp_config_node_list_tbl a
                LEFT JOIN rdb_region_layer[layer_no]_node_mapping b
                    ON a.node_id = b.node_id_14
                LEFT JOIN rdb_region_node_layer[layer_no]_tbl c
                    ON b.region_node_id = c.node_id
                WHERE c.node_id is not null
            """
        
        sqlcmd = sqlcmd.replace('[layer_no]', self.layer_no)
        self.pg.execute(sqlcmd)
        self.pg.commit()
        
        return True
    
    def _get_representative_point_data(self):
        
        single_tile_flag = False
        
        sqlcmd = """
                SELECT count(*)
                FROM (
                    SELECT DISTINCT node_id_t
                    FROM rdb_region_node_layer[layer_no]_tbl
                ) a
            """
        
        sqlcmd = sqlcmd.replace('[layer_no]', self.layer_no)
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row:
            if row[0] < 3:
                single_tile_flag = True
        
        if single_tile_flag:
            self._get_representative_point_from_single_tile()
        else:
            self._get_representative_point_from_tile()
            
        return True
    
    def _get_representative_point_from_single_tile(self):
        
        sqlcmd = """
                INSERT INTO temp_region_selectd_node_layer[layer_no]_tbl
                SELECT a.node_id, a.node_id_t, b.the_geom
                FROM (
                    SELECT node_id_t, link_num, (array_agg(node_id))[1] as node_id
                    FROM rdb_region_node_layer[layer_no]_tbl
                    GROUP BY node_id_t, link_num
                    ORDER BY node_id_t, link_num
                ) a
                LEFT JOIN rdb_region_node_layer[layer_no]_tbl b
                    ON a.node_id = b.node_id
            """
        
        sqlcmd = sqlcmd.replace('[layer_no]', self.layer_no)
        self.pg.execute(sqlcmd)
        self.pg.commit()
        
        return True
    
    def _get_representative_point_from_tile(self):
        
        sqlcmd = """
                INSERT INTO temp_region_selectd_node_layer[layer_no]_tbl
                SELECT d.node_id, d.node_id_t, d.the_geom
                FROM (
                    SELECT b.node_id_t, b.link_num, (array_agg(b.node_id))[1] as node_id
                    FROM (
                        SELECT node_id_t, max(link_num) as max_link_num
                        FROM rdb_region_node_layer[layer_no]_tbl
                        GROUP BY node_id_t
                    ) a
                    LEFT JOIN rdb_region_node_layer[layer_no]_tbl b
                        ON a.node_id_t = b.node_id_t and a.max_link_num = b.link_num
                    GROUP BY b.node_id_t, b.link_num
                ) c
                LEFT JOIN rdb_region_node_layer[layer_no]_tbl d
                    ON c.node_id = d.node_id
            """
        
        sqlcmd = sqlcmd.replace('[layer_no]', self.layer_no)
        self.pg.execute(sqlcmd)
        self.pg.commit()
            
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
        
class CCheckAccessibility(platform.TestCase.CTestCase):
    
    def _do(self):
        
        # 验证Region各层道路连通性
        #self._ReadShapePointList()
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
        objGraph = common.networkx.CGraph_Cache(1000)
        for layer_no in region_layer_list:
            objManager = CReachabilityCaseManager(layer_no, objGraph)
            objManager.do()

        self.logger.info('End proving layer reachability.')
        return True
    
    def _check_unreachable_regions_node_in_level14(self, region_layer_list):
        
        # 针对region层可达性验证失败的案例
        # 针对region层可达性验证失败原因有两种：1、本身不可达； 2、道路分层导致region层缺失某些道路
        # 为分析其原因，需要对失败案例做进一步分析，即使用失败案例在level 14验证其可达性
        # 若level 14层可达，则是由于路网分层导致道路缺失，进而导致region某些点不可达；反之，其自身不可达
        self.logger.info('Begin proving unreachable region nodes in level 14...')
        objGraph = common.networkx.CGraph_Cache(5000)
        for layer_no in region_layer_list:
            objManager = CReachabilityCaseManager(layer_no, objGraph, True)
            objManager.do()
        self.logger.info('End proving unreachable region nodes in level 14.')    
        return True
        
    def _update_geom(self, layer_no = '4'):
        
        self.logger.info('Begin updating path link geom...')
        
        sqlcmd = """
                DROP INDEX IF EXISTS temp_region_accessibility_path_layer[layer_no]_tbl_link_id_idx;
                CREATE INDEX temp_region_accessibility_path_layer[layer_no]_tbl_link_id_idx
                  ON temp_region_accessibility_path_layer[layer_no]_tbl
                  USING btree
                  (link_id);
                  
                UPDATE temp_region_accessibility_path_layer[layer_no]_tbl a
                SET the_geom = b.the_geom
                FROM rdb_region_link_layer[layer_no]_tbl b
                WHERE a.link_id = b.link_id;
                
                UPDATE temp_region_accessibility_path_layer[layer_no]_tbl a
                SET the_geom = b.the_geom
                FROM rdb_link b
                WHERE a.link_id = b.link_id;
            """
        
        sqlcmd = sqlcmd.replace('[layer_no]', layer_no)
        self.pg.execute(sqlcmd)
        self.pg.commit()
        
        self.logger.info('End updating path link geom.')  
        return True
    
    def _ReadShapePointList(self):
    
        pg = common.database.CDB()
        try:
            pg.connect()
            
            sqlcmd = """
                    DROP TABLE IF EXISTS temp_config_shape_point_list_tbl;
                    CREATE TABLE temp_config_shape_point_list_tbl
                    (
                        gid serial primary key,
                        str_geom varchar
                    );
                """
            pg.execute(sqlcmd)
            pg.commit()
            
            file_path = common.ConfigReader.CConfigReader.instance().getPara('shapepointlist')
            if file_path:
                if os.path.exists(file_path):
                    file_obj = common.cache_file.open('temp_config_shape_point_list_tbl')
                    file_handle = open(file_path, 'r')
                    recIdx = 0
                    for line in file_handle.readlines():
                        if line[0] == '#' or len(line) == 0:
                            pass
                        else:
                            recIdx += 1
                            temp_geom = line[:-1]
                            rec_string = '%d\t%s\n' % (recIdx, temp_geom)
                            file_obj.write(rec_string)
                            file_obj.flush()
                            
                    file_obj.seek(0)
                    pg.copy_from(file_obj, 'temp_config_shape_point_list_tbl')
                    pg.commit()
                    common.cache_file.close(file_obj,True)
                    file_handle.close()
            
            sqlcmd = """
                    DROP TABLE IF EXISTS temp_config_node_list_tbl;
                    CREATE TABLE temp_config_node_list_tbl
                    (
                        gid serial primary key,
                        node_id bigint not null,
                        distance double precision
                    );
                    
                    DROP INDEX IF EXISTS temp_config_node_list_tbl_node_id_idx;
                    CREATE INDEX temp_config_node_list_tbl_node_id_idx
                        ON temp_config_node_list_tbl
                        USING btree
                        (node_id);
                """
            pg.execute(sqlcmd)
            pg.commit()
            
            pg.CreateFunction_ByName('update_config_node_list_tbl')
            pg.callproc('update_config_node_list_tbl')
            pg.commit()
        except:
            pg.rollback()
            raise
        finally:
            pg.close()