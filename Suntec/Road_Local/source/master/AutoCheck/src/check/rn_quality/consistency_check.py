# -*- coding: UTF8 -*-
'''
Created on 2015-10-14

@author: liushengqiang
'''



import time
import threading
import platform.TestCase
import common.networkx
import math

mutex = threading.Lock()
g_path_id = 0

class CConsistencyCase(threading.Thread):
    
    def __init__(self,
                 caseManager,
                 rec_index,
                 search_start_node, 
                 search_end_node,
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
        self.search_end_node = search_end_node
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
            self.logger.info('   layer_no: ' + str(self.layer_no) + '---caculate route From Node ' + str(self.search_start_node) + ' To Node ' + str(self.search_end_node) + '...')
            paths = self.objGraph._searchBi_AStarPaths(self.search_start_node, 
                                                       self.search_end_node, 
                                                       max_buffer = 100000, 
                                                       max_path_num = 6, 
                                                       get_link_cost = common.networkx.CLinkCost.getCost,
                                                       log = self.logger,
                                                       link_tbl = self.link_tbl,
                                                       node_tbl = self.node_tbl,
                                                       link_laneinfo_tbl = self.link_laneinfo_tbl)
            
            if not paths:
                self.logger.error('   layer_no: ' + str(self.layer_no) + '---exception happened From Node ' + str(self.search_start_node) + ' To Node ' + str(self.search_end_node) + ' route calculation !!!')
                return False
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
        self.level14_flag = False
        if self.layer_no is None:
            self.layer_no = '0'
            self.level14_flag = True
        self.logger = common.Logger.CLogger.instance().logger("   Consistency Case Manager")
        self.pg = common.database.CDB()
        self.pg.connect()
        self.objGraph = objGraph
        
        global g_path_id
        g_path_id = 0
        pass
    
    def do(self):
        
        try:
            self.__loadCaseList(self.level14_flag)
            self.__doAllCase()
            #self.__doAllCase_parallel()
            return True
        except:
            self.logger.exception("error happened...")
            return False
    
    def __loadCaseList(self, level14_flag = False):
        
        self.logger.info('Begin Loading consistency verification cases list(layer_no: ' + str(self.layer_no)+ ')...')
        
        sqlcmd = """ 
            DROP TABLE IF EXISTS temp_region_consistency_layer[layer_no]_tbl;
            CREATE TABLE temp_region_consistency_layer[layer_no]_tbl
            ( 
                gid serial primary key,
                unique_id bigint not null,
                search_s_node bigint not null,
                search_e_node bigint not null,
                cost double precision,
                path_id integer
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
        
        self.__dataPreprocessing(level14_flag)
        
        self.logger.info('End Loading consistency verification cases list(layer_no: ' + str(self.layer_no)+ ').')
        return True
    
    def __dataPreprocessing(self, level14_flag = False):
        
        self.logger.info('Begin preprocessing consistency verification representative point data(layer_no: ' + str(self.layer_no)+ ')...')
        
        if False == level14_flag:
            sqlcmd = """
                    SELECT a.search_s_node, a.search_e_node, 
                        ST_Distance_Sphere(b.the_geom, c.the_geom) as distance
                    FROM temp_region_accessibility_layer[layer_no]_tbl a
                    LEFT JOIN rdb_region_node_layer[layer_no]_tbl b
                        ON a.search_s_node = b.node_id
                    LEFT JOIN rdb_region_node_layer[layer_no]_tbl c
                        ON a.search_e_node = c.node_id
                    WHERE a.region_path_id IS NOT NULL
                    ORDER BY a.search_s_node, a.search_e_node
                """
            sqlcmd = sqlcmd.replace('[layer_no]', self.layer_no)
            self.pg.execute(sqlcmd)
            rows = self.pg.fetchall()
            row_idx = 0
            for row in rows:
                row_idx += 1
                search_s_node = row[0]
                search_e_node = row[1]
                distance = row[2]
                case = CConsistencyCase(self, row_idx, search_s_node, search_e_node, distance)
                self.cases[row_idx] = case
        else:
            if self.pg.IsExistTable('rdb_region_layer4_node_mapping'):
                sqlcmd = """
                        SELECT a.search_s_node, a.search_e_node, 
                            c.node_id AS search_s_node_14, e.node_id AS search_e_node_14, 
                            ST_Distance_Sphere(c.the_geom, e.the_geom) as distance 
                        FROM temp_region_accessibility_layer4_tbl a
                        LEFT JOIN rdb_region_layer4_node_mapping b
                            ON a.search_s_node = b.region_node_id
                        LEFT JOIN rdb_node c
                            ON b.node_id_14 = c.node_id
                        LEFT JOIN rdb_region_layer4_node_mapping d
                            ON a.search_e_node = d.region_node_id
                        LEFT JOIN rdb_node e
                            ON d.node_id_14 = e.node_id
                        WHERE a.region_path_id IS NOT NULL
                        ORDER BY a.search_s_node, a.search_e_node
                    """
                self.pg.execute(sqlcmd)
                rows = self.pg.fetchall()
                row_idx = 0
                for row in rows:
                    row_idx += 1
                    search_s_node = row[2]
                    search_e_node = row[3]
                    distance = row[4]
                    case = CConsistencyCase(self, row_idx, search_s_node, search_e_node, distance, True)
                    self.cases[row_idx] = case
            else:
                self.logger.info('End preprocessing consistency verification representative point data(layer_no: 0) Error.')
                return False
            
        self.logger.info('End preprocessing consistency verification representative point data(layer_no: ' + str(self.layer_no)+ ') Success.')
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
        
        # test
#        self._test_func(6921047589404023245, 6920977220659852533, '4')
#        self._test_func(2306054119741225359, 2306054119741208558, '6')
#        return True
        
        region_layer_list = self.pg.GetRegionLayers()
        #self.__get_paths_between_accessibility_region_nodes(region_layer_list)
        self.__get_paths_between_accessibility_l14_nodes()
        self.__compare_paths(region_layer_list)      
        return True
    
    def __get_paths_between_accessibility_region_nodes(self, region_layer_list):
        self.logger.info('Begin get paths among region nodes...')
        objGraph = common.networkx.CGraph_Cache(1000)
        for layer_no in region_layer_list:
            objManager = CConsistencyCaseManager(layer_no, objGraph)
            objManager.do()
            self.__update_geom(layer_no)
        self.logger.info('End get paths among region nodes.')
        return True
    
    def __get_paths_between_accessibility_l14_nodes(self):
        self.logger.info('Begin get paths among level 14 nodes...')
        objGraph = common.networkx.CGraph_Cache(3000)
        objManager = CConsistencyCaseManager(None, objGraph)
        objManager.do()
        self.__update_geom('0')
        self.logger.info('End get paths among level 14 nodes.')
        return True
    
    def __update_geom(self, layer_no = '4'):
        
        self.logger.info('Begin updating path link geom...')
        
        sqlcmd = """
                DROP INDEX IF EXISTS temp_region_consistency_path_layer[layer_no]_tbl_link_id_idx;
                CREATE INDEX temp_region_consistency_path_layer[layer_no]_tbl_link_id_idx
                  ON temp_region_consistency_path_layer[layer_no]_tbl
                  USING btree
                  (link_id);
                
                analyze temp_region_consistency_path_layer[layer_no]_tbl_link_id_idx;  
            """
        sqlcmd = sqlcmd.replace('[layer_no]', layer_no)
        self.pg.execute(sqlcmd)
        self.pg.commit()
        
        if layer_no == '0':
            sqlcmd = """
                    UPDATE temp_region_consistency_path_layer[layer_no]_tbl a
                    SET the_geom = b.the_geom
                    FROM rdb_link b
                    WHERE a.link_id = b.link_id;
                """
        else:
            sqlcmd = """
                    UPDATE temp_region_consistency_path_layer[layer_no]_tbl a
                    SET the_geom = b.the_geom
                    FROM rdb_region_link_layer[layer_no]_tbl b
                    WHERE a.link_id = b.link_id;
                """
        sqlcmd = sqlcmd.replace('[layer_no]', layer_no)
        self.pg.execute(sqlcmd)
        self.pg.commit()
        
        self.logger.info('End updating path link geom.')  
        return True
    
    def __compare_paths(self, region_layer_list):
        
        sqlcmd = """
                DROP INDEX IF EXISTS temp_region_consistency_layer0_tbl_path_id_idx;
                CREATE INDEX temp_region_consistency_layer0_tbl_path_id_idx
                    ON temp_region_consistency_layer0_tbl
                    USING btree
                    (path_id);
                analyze temp_region_consistency_layer0_tbl;
                
                DROP INDEX IF EXISTS temp_region_consistency_path_layer0_tbl_path_id_idx;
                CREATE INDEX temp_region_consistency_path_layer0_tbl_path_id_idx
                    ON temp_region_consistency_path_layer0_tbl
                    USING btree
                    (path_id);
                DROP INDEX IF EXISTS temp_region_consistency_path_layer0_tbl_link_id_idx;
                CREATE INDEX temp_region_consistency_path_layer0_tbl_link_id_idx
                    ON temp_region_consistency_path_layer0_tbl
                    USING btree
                    (link_id);
                analyze temp_region_consistency_path_layer0_tbl;
            """
        self.pg.execute(sqlcmd)
        self.pg.commit()
        
        for layer_no in region_layer_list:
            #self.__compare_paths_one_layer(layer_no)
            self.__compare_paths_by_region_link_rate(layer_no)
            
        return True
    
    def __compare_paths_by_region_link_rate(self, layer_no):
        self.logger.info('Begin comparing layer: ' + str(layer_no) + ' Path to level 14 Path...')
        
        sqlcmd = """
                DROP TABLE IF EXISTS temp_rdb_region_layer[layer_no]_link_mapping;
                CREATE TABLE temp_rdb_region_layer[layer_no]_link_mapping
                AS (
                    SELECT region_link_id, unnest(link_id_14) AS link_id
                    FROM rdb_region_layer[layer_no]_link_mapping
                );
                DROP INDEX IF EXISTS temp_rdb_region_layer[layer_no]_link_mapping_link_id_idx;
                CREATE INDEX temp_rdb_region_layer[layer_no]_link_mapping_link_id_idx
                    ON temp_rdb_region_layer[layer_no]_link_mapping
                    USING btree
                    (link_id);
                analyze temp_rdb_region_layer[layer_no]_link_mapping;
                
                DROP TABLE IF EXISTS temp_region_consistency_path_layer[layer_no]_compare_tbl;
                CREATE TABLE temp_region_consistency_path_layer[layer_no]_compare_tbl
                (
                    gid serial primary key,
                    l14_s_node bigint,
                    l14_e_node bigint,
                    path_id_14 integer,
                    cost double precision,
                    region_link_rate double precision
                );
            """
        sqlcmd = sqlcmd.replace('[layer_no]', layer_no)
        self.pg.execute(sqlcmd)
        self.pg.commit()
        
        insert_sqlcmd = """
                INSERT INTO temp_region_consistency_path_layer%s_compare_tbl (
                    l14_s_node, l14_e_node, path_id_14, cost, region_link_rate)
                    VALUES (%s, %s, %s, %s, %s)
            """
        sqlcmd = """
                SELECT search_s_node, search_e_node, 
                    array_agg(cost) AS array_cost, 
                    array_agg(path_id) AS array_path_id, 
                    array_agg(path_link_count) AS array_path_link_count, 
                    array_agg(region_link_exist_count) AS array_region_link_count
                FROM (
                    SELECT search_s_node, search_e_node, cost, f.path_id, 
                        path_link_count, region_link_exist_count
                    FROM temp_region_consistency_layer0_tbl f
                    LEFT JOIN (
                        SELECT d.path_id, path_link_count, region_link_exist_count
                        FROM (
                            SELECT path_id, count(*) AS path_link_count
                            FROM temp_region_consistency_path_layer0_tbl
                            GROUP BY path_id
                            ORDER BY path_id
                        ) d
                        LEFT JOIN (
                            SELECT path_id, count(*) AS region_link_exist_count
                            FROM (
                                SELECT path_id, a.link_id, 
                                    CASE WHEN b.link_id IS NULL THEN False ELSE True END AS region_link_exist_flag
                                FROM temp_region_consistency_path_layer0_tbl a
                                LEFT JOIN temp_rdb_region_layer[layer_no]_link_mapping b
                                    ON a.link_id = b.link_id
                            ) c
                            GROUP BY path_id, region_link_exist_flag HAVING region_link_exist_flag = True
                            ORDER BY path_id
                        ) e
                            ON d.path_id = e.path_id
                    ) g
                        ON f.path_id = g.path_id
                    ORDER BY search_s_node, search_e_node, cost
                ) h
                GROUP BY search_s_node, search_e_node
            """
        sqlcmd = sqlcmd.replace('[layer_no]', layer_no)
        self.pg.execute(sqlcmd)
        rows = self.pg.fetchall()
        for row in rows:
            search_s_node = row[0]
            search_e_node = row[1]
            cost_list = row[2] 
            path_id_list = row[3] 
            path_link_count_list = row[4] 
            region_link_count_list = row[5]
            
            list_len = len(cost_list)
            loop = 0
            max_loop = 0
            max_region_link_rate = 0.0
            while loop < list_len:
                region_link_rate = region_link_count_list[loop] * 1.0 / path_link_count_list[loop] * 100
                if region_link_rate > max_region_link_rate:
                    max_region_link_rate = region_link_rate
                    max_loop = loop
                loop += 1
            else:
                self.pg.execute(insert_sqlcmd, (int(layer_no), 
                                                search_s_node, search_e_node, path_id_list[max_loop], 
                                                cost_list[max_loop], max_region_link_rate))
        self.pg.commit()
        self.logger.info('End comparing layer: ' + str(layer_no) + ' Path to level 14 Path.')
        return True
    
    def __compare_paths_one_layer(self, layer_no = '4'):
        self.logger.info('Begin comparing layer: ' + str(layer_no) + ' Path to level 14 Path...')
        sqlcmd = """
                DROP TABLE IF EXISTS temp_region_consistency_path_layer[layer_no]_compare_tbl;
                CREATE TABLE temp_region_consistency_path_layer[layer_no]_compare_tbl
                (
                    gid serial primary key,
                    region_s_node bigint,
                    region_e_node bigint,
                    region_path_cost double precision,
                    region_path_id integer,
                    l14_s_node bigint,
                    l14_e_node bigint,
                    path_cost double precision,
                    path_id_14 integer,
                    coincident_link_rate double precision,
                    path_cost_rate double precision
                );
            """
        sqlcmd = sqlcmd.replace('[layer_no]', layer_no)
        self.pg.execute(sqlcmd)
        self.pg.commit()
            
        sqlcmd = """
                SELECT b.*, f.*
                FROM (
                    SELECT search_s_node, search_e_node, 
                        array_agg(path_id) AS array_path,
                        array_agg(cost) AS array_cost
                    FROM (
                        SELECT *
                        FROM temp_region_consistency_layer[layer_no]_tbl
                        ORDER BY search_s_node, search_e_node, cost
                    ) a
                    GROUP BY search_s_node, search_e_node
                ) b
                LEFT JOIN rdb_region_layer[layer_no]_node_mapping c
                    ON b.search_s_node = c.region_node_id
                LEFT JOIN rdb_region_layer[layer_no]_node_mapping d
                    ON b.search_e_node = d.region_node_id
                LEFT JOIN (
                    SELECT search_s_node, search_e_node, 
                        array_agg(path_id) AS array_path,
                        array_agg(cost) AS array_cost
                    FROM (
                        SELECT *
                        FROM temp_region_consistency_layer0_tbl
                        ORDER BY search_s_node, search_e_node, cost
                    ) e
                    GROUP BY search_s_node, search_e_node
                ) f
                    ON c.node_id_14 = f.search_s_node AND d.node_id_14 = f.search_e_node
                WHERE f.search_s_node IS NOT NULL
            """
        sqlcmd = sqlcmd.replace('[layer_no]', str(layer_no))
        self.pg.execute(sqlcmd)
        region_layer_14_rel_rows = self.pg.fetchall() # region path and level 14 path relation
        
        insert_sqlcmd = """
                INSERT INTO temp_region_consistency_path_layer%s_compare_tbl (
                    region_s_node, region_e_node, region_path_cost, region_path_id, 
                    l14_s_node, l14_e_node, path_cost, path_id_14, 
                    coincident_link_rate, path_cost_rate)
                    VALUES (%s, %s, %s, %s, %s, %s, %s, %s, %s, %s)
            """ 
            
        row_len = len(region_layer_14_rel_rows)
        row_idx = 0
        for region_layer_14_rel_row in region_layer_14_rel_rows:
            row_idx += 1
            self.logger.info(' Begin comparing ' + str(row_idx) + ' / ' + str(row_len) + ' item...')
            region_s_node = region_layer_14_rel_row[0]
            region_e_node = region_layer_14_rel_row[1]
            region_path_id_list = region_layer_14_rel_row[2]
            region_path_cost_list = region_layer_14_rel_row[3]
            l14_s_node = region_layer_14_rel_row[4]
            l14_e_node = region_layer_14_rel_row[5]
            l14_path_id_list = region_layer_14_rel_row[6]
            l14_path_cost_list = region_layer_14_rel_row[7]
            
            sqlcmd = """
                    SELECT path_id, path_cost, array_agg(link_id_14) AS array_link_id_14
                    FROM (
                        SELECT a.path_id, c.path_cost, a.seq_num, a.link_id, unnest(b.link_id_14) AS link_id_14
                        FROM temp_region_consistency_path_layer[layer_no]_tbl a
                        LEFT JOIN rdb_region_layer[layer_no]_link_mapping b
                            ON a.link_id = b.region_link_id
                        LEFT JOIN (
                            SELECT unnest(array[region_path_id_list]) AS path_id,
                                unnest(array[region_path_cost_list]) AS path_cost
                        ) c
                            ON a.path_id = c.path_id
                        WHERE c.path_id IS NOT NULL
                        ORDER BY a.path_id, a.seq_num
                    ) d
                    GROUP BY path_id, path_cost
                """
            sqlcmd = sqlcmd.replace('[layer_no]', str(layer_no))
            sqlcmd = sqlcmd.replace('[region_path_id_list]', str(region_path_id_list))
            sqlcmd = sqlcmd.replace('[region_path_cost_list]', str(region_path_cost_list))
            self.pg.execute(sqlcmd)
            region_path_rows = self.pg.fetchall()
            
            sqlcmd = """
                    SELECT path_id, path_cost, array_agg(link_id) AS array_link
                    FROM (
                        SELECT a.path_id, b.path_cost, link_id
                        FROM temp_region_consistency_path_layer0_tbl a
                        LEFT JOIN (
                            SELECT unnest(array[l14_path_id_list]) AS path_id,
                                unnest(array[l14_path_cost_list]) AS path_cost
                        ) b
                            ON a.path_id = b.path_id
                        WHERE b.path_id IS NOT NULL
                        ORDER BY a.path_id, a.seq_num
                    ) a
                    GROUP BY path_id, path_cost
                """
            sqlcmd = sqlcmd.replace('[l14_path_id_list]', str(l14_path_id_list))
            sqlcmd = sqlcmd.replace('[l14_path_cost_list]', str(l14_path_cost_list))
            self.pg.execute(sqlcmd)
            l14_path_rows = self.pg.fetchall()
            for region_path_row in region_path_rows:
                region_path_id = region_path_row[0]
                region_path_cost = float(region_path_row[1])
                region_path = region_path_row[2]
                max_intersect_percent = -1.0
                max_intersect_percent_path_id = 0
                max_intersect_cost = 0.0
                for l14_path_row in l14_path_rows:
                    l14_path_id = l14_path_row[0]
                    l14_path_cost = float(l14_path_row[1])
                    l14_path = l14_path_row[2]
                    intersect_path = list(set(region_path).intersection(set(l14_path)))
                    percent = len(intersect_path) * 1.0 / len(l14_path) * 100
                    if percent > max_intersect_percent:
                        max_intersect_percent = percent
                        max_intersect_percent_path_id = l14_path_id
                        max_intersect_cost = l14_path_cost
                        if int(max_intersect_percent) == 100:
                            break
                cost_rate = math.fabs(region_path_cost-max_intersect_cost) / max_intersect_cost * 100
                self.pg.execute(insert_sqlcmd, (int(layer_no), 
                                                region_s_node, region_e_node, region_path_cost, region_path_id, 
                                                l14_s_node, l14_e_node, max_intersect_cost, max_intersect_percent_path_id, 
                                                max_intersect_percent, cost_rate))
        self.pg.commit()
        self.logger.info('End comparing layer: ' + str(layer_no) + ' Path to level 14 Path.')
        return True
    
    def _test_func(self, s_node, e_node, layer_no = '4'):
        logger = common.Logger.CLogger.instance().logger('TEST')
        self.pg = common.database.CDB()
        self.pg.connect()
        sqlcmd = """
                DROP TABLE IF EXISTS temp_region_layer[layer_no]_test_tbl;
                CREATE TABLE temp_region_layer[layer_no]_test_tbl
                (
                    path_id integer,
                    seq_num integer,
                    link_id bigint,
                    the_geom geometry
                );
            """
        sqlcmd = sqlcmd.replace('[layer_no]', str(layer_no))
        self.pg.execute(sqlcmd)
        self.pg.commit()
        
        insert_sqlcmd = """
                INSERT INTO temp_region_layer%s_test_tbl (path_id, seq_num, link_id)
                    VALUES (%s, %s, %s)
            """
        objGraph = common.networkx.CGraph_Cache(1000)
        if int(layer_no) == 0:
            link_tbl = 'rdb_link'
            node_tbl = 'rdb_node'
            link_laneinfo_tbl = 'rdb_linklane_info'
        else:
            link_tbl = """rdb_region_link_layer%s_tbl""" % (str(layer_no))
            node_tbl = """rdb_region_node_layer%s_tbl""" % (str(layer_no))
            link_laneinfo_tbl = """rdb_region_link_lane_info_layer%s_tbl""" % (str(layer_no))
        paths = objGraph._searchBi_AStarPaths(s_node, e_node, 
                                              max_buffer = 50000, 
                                              max_path_num = 6,
                                              get_link_cost = common.networkx.CLinkCost.getCost,
                                              log = logger, 
                                              link_tbl = link_tbl, 
                                              node_tbl = node_tbl, 
                                              link_laneinfo_tbl = link_laneinfo_tbl)
        if not paths:
            logger.warning('No Road To Go!!!!!')
        else:
            path_id = 0
            for (cost, path) in paths:
                path_id += 1
                seq_num = 0
                for link in path:
                    seq_num += 1 
                    self.pg.execute(insert_sqlcmd, (int(layer_no), path_id, seq_num, link))
        self.pg.commit()
        
        if int(layer_no) == 0:
            sqlcmd = """
                    UPDATE temp_region_layer[layer_no]_test_tbl a
                    SET the_geom = b.the_geom
                    FROM rdb_link b
                    WHERE a.link_id = b.link_id;
                """
        else:
            sqlcmd = """
                    UPDATE temp_region_layer[layer_no]_test_tbl a
                    SET the_geom = b.the_geom
                    FROM rdb_region_link_layer[layer_no]_tbl b
                    WHERE a.link_id = b.link_id;
                """
        sqlcmd = sqlcmd.replace('[layer_no]', layer_no)
        self.pg.execute(sqlcmd)
        self.pg.commit()
            
        self.pg.close()
        return True