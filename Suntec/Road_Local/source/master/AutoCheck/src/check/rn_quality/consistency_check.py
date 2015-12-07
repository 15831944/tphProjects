# -*- coding: UTF8 -*-
'''
Created on 2015-10-14

@author: liushengqiang
'''



import time
import threading
import platform.TestCase
import common.networkx

mutex = threading.Lock()
g_path_id = 0

class CConsistencyCase(threading.Thread):
    
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
        if self.level14_flag == True:
            self.link_tbl = """rdb_link"""
            self.node_tbl = """rdb_node"""
            self.link_laneinfo_tbl = """rdb_linklane_info"""
        else:
            self.link_tbl = """rdb_region_link_layer%s_tbl""" % (str(self.layer_no))
            self.node_tbl = """rdb_region_node_layer%s_tbl""" % (str(self.layer_no))
            self.link_laneinfo_tbl = """rdb_region_link_lane_info_layer%s_tbl""" % (str(self.layer_no))
        self.search_start_node = search_start_node
        self.search_start_node_t = search_start_node_t
        self.search_end_node = search_end_node
        self.search_end_node_t = search_end_node_t
        self.distance = distance
        
        strLoggerName = "      layer_no: %s --- Case %s" % (str(self.layer_no), str(self.rec_index))
        self.logger = common.Logger.CLogger.instance().logger(strLoggerName)
        self.pg = common.database.CDB()
        self.state = "ACTIVE"
        pass
    
    def __getPathBetweenNodes(self):
        
        insert_sqlcmd1 = """
                INSERT INTO temp_region_consistency_layer%s_tbl (
                    unique_id, search_s_node, search_e_node, cost, path_id
                )
                VALUES (%s, %s, %s, %s, %s)
            """
        
        insert_sqlcmd2 = """
                INSERT INTO temp_region_consistency_path_layer%s_tbl (
                    path_id, seq_num, link_id
                )
                VALUES (%s, %s, %s)
            """
        
        try:
            paths = self.objGraph.searchDijkstraPaths(self.search_start_node,
                                                      self.search_start_node_t,
                                                      self.search_end_node,
                                                      self.search_end_node_t,
                                                      max_path_num = 6,
                                                      get_link_cost = common.networkx.CLinkCost.getCost,
                                                      link_tbl = self.link_tbl,
                                                      node_tbl = self.node_tbl,
                                                      link_laneinfo_tbl = self.link_laneinfo_tbl)
            
            if not paths:
                self.logger.error('   layer_no: ' + str(self.layer_no) + '---exception happened From Node ' + str(self.search_start_node) + ' To Node ' + str(self.search_end_node) + ' route calculation !!!')
                raise
            else:
                for (cur_cost, path) in paths:
                    if mutex.acquire(1):
                        global g_path_id
                        g_path_id = g_path_id + 1
                        path_id = g_path_id
                        mutex.release()
                    
                    seq_num = 0
                    for linkid in path:
                        self.pg.execute(insert_sqlcmd2, (int(self.layer_no), path_id, seq_num, linkid))
                        seq_num += 1
                    self.pg.execute(insert_sqlcmd1, (int(self.layer_no), self.rec_index, self.search_start_node, self.search_end_node, cur_cost, path_id))
        except:
            self.logger.error('   layer_no: ' + str(self.layer_no) + '---exception happened From Node ' + str(self.search_start_node) + ' To Node ' + str(self.search_end_node) + ' route calculation !!!')
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
        
class CConsistencyCaseManager:
    
    def __init__(self, layer_no, objGraph):
        self.cases = {}
        self.layer_no = layer_no
        self.logger = common.Logger.CLogger.instance().logger("   Consistency Case Manager")
        self.pg = common.database.CDB()
        self.pg.connect()
        self.objGraph = objGraph
        
        global g_path_id
        g_path_id = 0
        pass
    
    def do(self):
        
        try:
            self.__loadCaseList()
            self.__doAllCase()
            #self.__doAllCase_parallel()
            return True
        except:
            self.logger.exception("error happened...")
            return False
    
    def __loadCaseList(self):
        
        self.logger.info('Begin Loading consistency verification cases list(layer_no: ' + str(self.layer_no)+ ')...')
        
        sqlcmd = """
                DROP TABLE IF EXISTS temp_region_consistency_layer[layer_no]_tbl;
                CREATE TABLE temp_region_consistency_layer[layer_no]_tbl
                (
                    gid serial primary key,
                    unique_id bigint not null,
                    search_s_node bigint not null,
                    search_e_node bigint not null,
                    cost integer,
                    path_id bigint
                );
                
                DROP TABLE IF EXISTS temp_region_consistency_path_layer[layer_no]_tbl;
                CREATE TABLE temp_region_consistency_path_layer[layer_no]_tbl
                (
                    gid serial primary key,
                    path_id bigint not null,
                    seq_num integer,
                    link_id bigint,
                    the_geom geometry
                );
            """
        
        sqlcmd = sqlcmd.replace('[layer_no]', self.layer_no)
        self.pg.execute(sqlcmd)
        self.pg.commit()
        
        self.__dataPreprocessing()
        
        self.logger.info('End Loading consistency verification cases list(layer_no: ' + str(self.layer_no)+ ').')
        return True
    
    def __dataPreprocessing(self):
        
        self.logger.info('Begin preprocessing consistency verification representative point data(layer_no: ' + str(self.layer_no)+ ')...')
        
        sqlcmd = """
                SELECT a.search_s_node, b.node_id_t as search_s_node_t, 
                    a.search_e_node, c.node_id_t as search_e_node_t, 
                    ST_Distance_Sphere(b.the_geom, c.the_geom) as distance
                FROM temp_region_accessibility_layer[layer_no]_tbl a
                LEFT JOIN rdb_region_node_layer[layer_no]_tbl b
                    ON a.search_s_node = b.node_id
                LEFT JOIN rdb_region_node_layer[layer_no]_tbl c
                    ON a.search_e_node = c.node_id
                WHERE a.region_path_id is not null
            """
        
        sqlcmd = sqlcmd.replace('[layer_no]', self.layer_no)
        self.pg.execute(sqlcmd)
        rows = self.pg.fetchall()
        
        row_idx = 0
        for row in rows:
            
            row_idx += 1
            search_s_node = row[0]
            search_s_node_t = row[1]
            search_e_node = row[2]
            search_e_node_t = row[3]
            distance = row[4]
            
            case = CConsistencyCase(self,
                                    row_idx,
                                    search_s_node,
                                    search_s_node_t,
                                    search_e_node,
                                    search_e_node_t,
                                    distance)
            
            self.cases[row_idx] = case
            
            sqlcmd = """
                    SELECT f.node_id as search_s_node, f.node_id_t as search_s_node_t, 
                        g.node_id as search_e_node, g.node_id_t as search_e_node_t, 
                        ST_Distance_Sphere(f.the_geom, g.the_geom) as distance
                    FROM temp_region_accessibility_layer[layer_no]_tbl a
                    LEFT JOIN rdb_region_node_layer[layer_no]_tbl b
                        ON a.search_s_node = b.node_id
                    LEFT JOIN rdb_region_layer[layer_no]_node_mapping d
                        ON b.node_id = d.region_node_id
                    LEFT JOIN rdb_node f
                        ON d.node_id_14 = f.node_id
                    LEFT JOIN rdb_region_node_layer[layer_no]_tbl c
                        ON a.search_e_node = c.node_id
                    LEFT JOIN rdb_region_layer[layer_no]_node_mapping e
                        ON c.node_id = e.region_node_id
                    LEFT JOIN rdb_node g
                        ON e.node_id_14 = g.node_id
                    WHERE a.search_s_node = [search_s_node] and a.search_e_node = [search_e_node]
                """
            
            sqlcmd = sqlcmd.replace('[layer_no]', self.layer_no)
            sqlcmd = sqlcmd.replace('[search_s_node]', str(search_s_node))
            sqlcmd = sqlcmd.replace('[search_e_node]', str(search_e_node))
            self.pg.execute(sqlcmd)
            temp_row = self.pg.fetchone()
            if temp_row:
                temp_row_idx = row_idx + 100000
                temp_search_s_node = temp_row[0]
                temp_search_s_node_t = temp_row[1]
                temp_search_e_node = temp_row[2]
                temp_search_e_node_t = temp_row[3]
                temp_distance = temp_row[4]
                temp_case = CConsistencyCase(self,
                                             temp_row_idx,
                                             temp_search_s_node,
                                             temp_search_s_node_t,
                                             temp_search_e_node,
                                             temp_search_e_node_t,
                                             temp_distance,
                                             True)
                self.cases[temp_row_idx] = temp_case
            else:
                self.logger.error('      ERROR happened---layer_no: ' + str(self.layer_no) + 'node: ' + str(search_s_node) + ',' + str(search_e_node) + 'have not mapping node in level14!!!')
                return False
        
        self.logger.info('End preprocessing consistency verification representative point data(layer_no: ' + str(self.layer_no)+ ').')
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
        self.logger.info('Begin parallel implementation of consistency test cases(layer_no: ' + str(self.layer_no)+ ')...')
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
        self.logger.info('End parallel implementation of consistency test cases(layer_no: ' + str(self.layer_no)+ ').')
        
        return True
    
class CCheckConsistency(platform.TestCase.CTestCase):
    
    def _do(self):
        
        region_layer_list = self.pg.GetRegionLayers()
        self.__get_paths_between_accessibility_nodes(region_layer_list)
        self.__compare_paths(region_layer_list)      
        return True
    
    def __get_paths_between_accessibility_nodes(self, region_layer_list):
        
        objGraph = common.networkx.CGraph_Cache(5000)
        for layer_no in region_layer_list:
            objManager = CConsistencyCaseManager(layer_no, objGraph)
            objManager.do()
        
        for layer_no in region_layer_list:
            self.__update_geom(layer_no)
            
        return True
    
    def __update_geom(self, layer_no = '4'):
        
        self.logger.info('Begin updating path link geom...')
        
        sqlcmd = """
                DROP INDEX IF EXISTS temp_region_consistency_path_layer[layer_no]_tbl_link_id_idx;
                CREATE INDEX temp_region_consistency_path_layer[layer_no]_tbl_link_id_idx
                  ON temp_region_consistency_path_layer[layer_no]_tbl
                  USING btree
                  (link_id);
                  
                UPDATE temp_region_consistency_path_layer[layer_no]_tbl a
                SET the_geom = b.the_geom
                FROM rdb_region_link_layer[layer_no]_tbl b
                WHERE a.link_id = b.link_id;
                
                UPDATE temp_region_consistency_path_layer[layer_no]_tbl a
                SET the_geom = b.the_geom
                FROM rdb_link b
                WHERE a.link_id = b.link_id;
            """
        
        sqlcmd = sqlcmd.replace('[layer_no]', layer_no)
        self.pg.execute(sqlcmd)
        self.pg.commit()
        
        self.logger.info('End updating path link geom.')  
        return True
    
    def __compare_paths(self, region_layer_list):
        
        for layer_no in region_layer_list:
            self.__compare_paths_one_layer(layer_no)
            
        return True
    
    def __compare_paths_one_layer(self, layer_no = '4'):
        
        sqlcmd = """
                DROP TABLE IF EXISTS temp_region_consistency_path_link_layer[layer_no]_tbl;
                CREATE TABLE temp_region_consistency_path_link_layer[layer_no]_tbl
                AS (
                    SELECT e.unique_id, e.cost, e.path_id, f.link_id_array
                    FROM temp_region_consistency_layer[layer_no]_tbl e
                    LEFT JOIN (
                        SELECT path_id, (case when link_id_14_array is not null then link_id_14_array else link_id_array end) as link_id_array
                        FROM (
                            SELECT path_id, array_agg(link_id) as link_id_array, string_to_array(array_to_string(array_agg(link_id_14_array), ',')::varchar, ',')::bigint[] as link_id_14_array
                            FROM (
                                SELECT a.path_id, a.seq_num, a.link_id, array_to_string(b.link_id_14, ',')::varchar as link_id_14_array
                                FROM temp_region_consistency_path_layer[layer_no]_tbl a
                                LEFT JOIN rdb_region_layer[layer_no]_link_mapping b
                                    ON a.link_id = b.region_link_id
                                ORDER BY path_id, seq_num
                            ) c
                            GROUP BY path_id
                        ) d
                    ) f
                        ON e.path_id = f.path_id
                );
                
                DROP INDEX IF EXISTS temp_region_consistency_path_link_layer[layer_no]_tbl_unique_id_idx;
                CREATE INDEX temp_region_consistency_path_link_layer[layer_no]_tbl_unique_id_idx
                    ON temp_region_consistency_path_link_layer[layer_no]_tbl
                    USING btree
                    (unique_id);
                    
                analyze temp_region_consistency_path_link_layer[layer_no]_tbl;
            """
        
        sqlcmd = sqlcmd.replace('[layer_no]', str(layer_no))
        self.pg.execute(sqlcmd)
        self.pg.commit()
        
        sqlcmd = """
                SELECT *
                FROM temp_region_consistency_path_link_layer[layer_no]_tbl
                WHERE unique_id <= 100000
            """
        
        sqlcmd = sqlcmd.replace('[layer_no]', str(layer_no))
        self.pg.execute(sqlcmd)
        rows = self.pg.fetchall()
        for row in rows:   
            unique_id = row[0]
            region_cost = row[1]
            region_path_id = row[2]
            region_link_array = row[3]
            
            temp_unique_id = unique_id + 100000
            sqlcmd = """
                    SELECT *
                    FROM temp_region_consistency_path_link_layer[layer_no]_tbl
                    WHERE unique_id = [unique_id]
                """
            
            sqlcmd = sqlcmd.replace('[layer_no]', str(layer_no))
            sqlcmd = sqlcmd.replace('[unique_id]', str(temp_unique_id))
            self.pg.execute(sqlcmd)
            temp_rows = self.pg.fetchall()
            for temp_row in temp_rows:
                level14_cost = temp_row[1]
                level14_path_id = temp_row[2]
                level14_link_array = temp_row[3]
                level14_link_array_len = len(level14_link_array)
                
                intersect_link_array = list(set(region_link_array).intersection(set(level14_link_array)))
                intersect_link_array_len = len(intersect_link_array)
                
                percentage = int(intersect_link_array_len * 1.0 / level14_link_array_len * 100)
                print percentage
            
            pass
        
        return True