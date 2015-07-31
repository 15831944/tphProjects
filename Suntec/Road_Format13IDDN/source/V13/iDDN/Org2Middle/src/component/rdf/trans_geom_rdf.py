# -*- coding: cp936 -*-
'''
Created on 2011-12-5

@author: hongchenzai
'''
import base

class comp_trans_geom_rdf(base.component_base.comp_base):
    def __init__(self):
        '''
        Constructor
        '''
        base.component_base.comp_base.__init__(self, 'Trans_geom')
    
    def _DoCreateIndex(self):
        if self.CreateIndex2('temp_rdf_nav_link_the_geom_idx') == -1:
            return -1
        if self.CreateIndex2('temp_rdf_nav_link_right_admin_place_id_idx') == -1:
            return -1
        if self.CreateIndex2('temp_rdf_nav_node_the_geom_idx') == -1:
            return -1
        if self.CreateIndex2('temp_wkt_building_the_geom_idx') == -1:
            return -1
        if self.CreateIndex2('temp_wkt_face_the_geom_idx') == -1:
            return -1
        if self.CreateIndex2('temp_wkt_link_the_geom_idx') == -1:
            return -1
        if self.CreateIndex2('temp_wkt_location_the_geom_idx') == -1:
            return -1
        if self.CreateIndex2('temp_wkt_node_the_geom_idx') == -1:
            return -1
        return 0
    
    def _Do(self):
        self.__trans_wtk_link()
        self.__trans_wkt_node()
        self.__trans_wkt_building()
        self.__trans_wkt_face()
        self.__trans_wkt_location()
        self.__trans_rdf_nav_link()
        self.__trans_rdf_nav_node()
        return 0
    
    def __trans_wtk_link(self):
        if self.CreateIndex2('wkt_link_link_id_idx') == -1:
            return -1
        
        if not self.__NeedTranslate('wkt_link', 'temp_wkt_link', 'link'):
            return 0
        
        self.log.info('Start trans_wtk_link.')
        if self.CreateTable2('temp_wkt_link') == -1:
            return -1
        
        # 求最小和最大id
        sqlcmd = """
            SELECT min(link_id), max(link_id)
              FROM wkt_link;
            """
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchone2()
        min_id  = row[0]
        max_id  = row[1]
        
        sqlcmd  = """
                INSERT INTO temp_wkt_link(link_id, the_geom)
                (
                SELECT link_id, ST_GeometryFromText(link, 4326)
                  FROM wkt_link
                  where link_id >= %s and link_id <= %s
                );
                """
        self.__call_child_thread(min_id, max_id, sqlcmd)
        
        self.log.info('End trans_wtk_link.')
        return 0
    
    def __trans_wkt_node(self):
        if self.CreateIndex2('wkt_node_node_id_idx') == -1:
            return -1
        
        if not self.__NeedTranslate('wkt_node', 'temp_wkt_node', 'node'):
            return 0
        self.log.info('Start trans_wkt_node.')
        if self.CreateTable2('temp_wkt_node') == -1:
            return -1
        # 求最小和最大id
        sqlcmd = """
            SELECT min(node_id), max(node_id)
              FROM wkt_node;
            """
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchone2()
        min_id  = row[0]
        max_id  = row[1]
        
        sqlcmd  = """
                INSERT INTO temp_wkt_node(
                            node_id, the_geom)
                (
                SELECT node_id, ST_GeometryFromText(node, 4326)
                  FROM wkt_node
                  where node_id >= %s and node_id <= %s
                );
                """
        self.__call_child_thread(min_id, max_id, sqlcmd)
        
        self.log.info('End trans_wkt_node.')
        return 0
    
    def __trans_wkt_building(self):
        if self.CreateIndex2('wkt_building_building_id_idx') == -1:
            return -1
        
        if not self.__NeedTranslate('wkt_building', 'temp_wkt_building', 'building'):
            return 0
        
        self.log.info('Start trans_wkt_building.')
        if self.CreateTable2('temp_wkt_building') == -1:
            return -1
        # 求最小和最大id
        sqlcmd = """
            SELECT min(building_id), max(building_id)
              FROM wkt_building;
            """
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchone2()
        min_id  = row[0]
        max_id  = row[1]
        
        sqlcmd  = """
                INSERT INTO temp_wkt_building(
                            building_id, the_geom)
                (
                SELECT building_id, ST_GeometryFromText(building, 4326) 
                  FROM wkt_building
                  where building_id >= %s and building_id <= %s
                );
                """
        self.__call_child_thread(min_id, max_id, sqlcmd)
        
        self.log.info('End trans_wkt_building.')
        return 0
    
    def __trans_wkt_face(self):
        if self.CreateIndex2('wkt_face_face_id_idx') == -1:
            return -1
        
        if not self.__NeedTranslate('wkt_face', 'temp_wkt_face', 'face'):
            return 0
        self.log.info('Start trans_wkt_face.')
        if self.CreateTable2('temp_wkt_face') == -1:
            return -1
        # 求最小和最大id
        sqlcmd = """
            SELECT min(face_id), max(face_id)
              FROM wkt_face;
            """
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchone2()
        min_id  = row[0]
        max_id  = row[1]
        
        sqlcmd  = """
                INSERT INTO temp_wkt_face(
                            face_id, the_geom)
                (
                SELECT face_id, ST_GeometryFromText(face, 4326)  
                  FROM wkt_face
                  where face_id >= %s and face_id <= %s
                );
                """
        self.__call_child_thread(min_id, max_id, sqlcmd)
        
        self.log.info('End trans_wkt_face.')
        return 0
    
    def __trans_wkt_location(self):
        if self.CreateIndex2('wkt_location_location_id_idx') == -1:
            return -1
        # 判断是否要进行转换
        if not self.__NeedTranslate('wkt_location', 'temp_wkt_location', 'location'):
            return 0
        self.log.info('Start trans_wkt_location.')
        if self.CreateTable2('temp_wkt_location') == -1:
            return -1
        # 求最小和最大id
        sqlcmd = """
            SELECT min(location_id), max(location_id)
              FROM wkt_location;
            """
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchone2()
        min_id  = row[0]
        max_id  = row[1]
        
        sqlcmd  = """
                INSERT INTO temp_wkt_location(
                            location_id, the_geom)
                (
                SELECT location_id, ST_GeometryFromText("location", 4326)   
                  FROM wkt_location
                  where location_id >= %s and location_id <= %s
                );
                """
        self.__call_child_thread(min_id, max_id, sqlcmd)
        
        self.log.info('End trans_wkt_location.')
        return 0
    
    def __trans_rdf_nav_link(self):
        # 判断是否要进行转换
        if not self.__NeedTranslate('temp_wkt_link', 'temp_rdf_nav_link'):
            return 0
        
        self.log.info('Start trans_rdf_nav_link.')
        if self.CreateTable2('temp_rdf_nav_link') == -1:
            return -1
        # 求最小和最大id
        sqlcmd = """
            SELECT min(link_id), max(link_id)
              FROM rdf_nav_link;
            """
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchone2()
        min_id  = row[0]
        max_id  = row[1]
        
        sqlcmd  = """
                INSERT INTO temp_rdf_nav_link(
                            link_id, iso_country_code, access_id, status_id, functional_class, 
                            controlled_access, travel_direction, boat_ferry, rail_ferry, 
                            multi_digitized, divider, divider_legal, frontage, paved, ramp, 
                            private, tollway, poi_access, intersection_category, speed_category, 
                            lane_category, coverage_indicator, from_ref_num_lanes, to_ref_num_lanes, 
                            physical_num_lanes, from_ref_speed_limit, to_ref_speed_limit, 
                            speed_limit_source, low_mobility, public_access, ref_node_id, 
                            nonref_node_id, left_admin_place_id, right_admin_place_id, left_postal_area_id, 
                            right_postal_area_id, bridge, tunnel, the_geom)
                (
                SELECT      a.link_id, iso_country_code, access_id, status_id, functional_class, 
                            controlled_access, travel_direction, boat_ferry, rail_ferry, 
                            multi_digitized, divider, divider_legal, frontage, paved, ramp, 
                            private, tollway, poi_access, intersection_category, speed_category, 
                            lane_category, coverage_indicator, from_ref_num_lanes, to_ref_num_lanes, 
                            physical_num_lanes, from_ref_speed_limit, to_ref_speed_limit, 
                            speed_limit_source, low_mobility, public_access, ref_node_id, 
                            nonref_node_id, left_admin_place_id, right_admin_place_id, left_postal_area_id, 
                            right_postal_area_id, bridge, tunnel, the_geom
                  FROM (
                    SELECT *
                    FROM rdf_nav_link
                    where link_id >= %s and link_id <= %s 
                  ) AS a
                  LEFT JOIN rdf_link
                  ON a.link_id = rdf_link.link_id
                  LEFT JOIN temp_wkt_link
                  ON a.link_id = temp_wkt_link.link_id
                );
                """
        # 调用子线程
        self.__call_child_thread(min_id, max_id, sqlcmd, 3, 3000000)
        
        # 随后制作temp_rdf_nav_node，要用以下两个索引
        if self.CreateIndex2('temp_rdf_nav_link_nonref_node_id_idx') == -1:
            return -1
        if self.CreateIndex2('temp_rdf_nav_link_ref_node_id_idx') == -1:
            return -1
        
        self.log.info('End trans_rdf_nav_link.')
        return 0
    
    def __trans_rdf_nav_node(self):
        "Node信息"
        # 判断是否要进行转换
        if not self.__NeedTranslate('temp_wkt_node', 'temp_rdf_nav_node'):
            return 0
        
        self.log.info('Start trans_rdf_nav_node.')
        if self.CreateTable2('temp_rdf_nav_node') == -1:
            return -1
        
        if self.CreateTable2('temp_nav_node') == -1:
            return -1
        
        # 从Nav_Link表导出Nav_node
        sqlcmd = """
            insert into temp_nav_node
            (
                select distinct(node_id)
                from (
                SELECT ref_node_id as node_id
                  FROM temp_rdf_nav_link
                union
                SELECT nonref_node_id as node_id
                   FROM temp_rdf_nav_link
                ) as a
            );
        """
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
        
        # 求最小和最大id
        sqlcmd = """
                select min(node_id), max(node_id)
                from temp_nav_node;
            """
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchone2()
        min_id  = row[0]
        max_id  = row[1]
        
        sqlcmd  = """
                    INSERT INTO temp_rdf_nav_node(
                                node_id, lat, lon, z_coord, zlevel, is_aligned, the_geom)
                    (
                    SELECT  a.node_id, lat, lon, z_coord, zlevel, is_aligned, the_geom
                      from (
                        select node_id 
                        from temp_nav_node
                        where node_id >= %s and node_id <= %s
                      ) as a
                      LEFT JOIN rdf_node
                      on a.node_id = rdf_node.node_id
                      LEFT JOIN temp_wkt_node
                      ON a.node_id = temp_wkt_node.node_id
                    );
                    """
        # 调用子线程
        self.__call_child_thread(min_id, max_id, sqlcmd, 4, 3000000)
        
        
        self.log.info('End trans_rdf_nav_node.')
        return 0
        
    def __GetColumnList(self, table):
        # 取一个表里有
        col_list = []
        sqlcmd = """
                    SELECT attname
                      FROM pg_attribute
                      where attrelid in (
                          SELECT relid 
                              FROM pg_statio_user_tables
                              where relname = 'table_name'
                              ) 
                            and attnum > 0
                            and attname not like '%pg.dropped%'          
                      order by attrelid, attnum;
                """
        sqlcmd = sqlcmd.replace('table_name', table)
        self.pg.execute2(sqlcmd)
        rows = self.pg.fetchall2()
        for row in rows:
            if row:
                col_list.append(row[0])
        return col_list
    
    def __NeedTranslate(self, src_table_name, dest_table_name, src_geom_col = 'the_geom'):
        '''判定转换表是否存；内容都转换完成。''' 
        # 判断原表是否存在
        if self.pg.IsExistTable(src_table_name):
            # 判断目标表是否存在
            if self.pg.IsExistTable(dest_table_name):
                if 'the_geom' not in self.__GetColumnList(dest_table_name):
                    return True
                
                ### 判断内容是否完整
                PkeyCols   = self.pg.GetPKeyColumns(src_table_name)
                CountCols  = []    # Count使用的字段
                OnCondCols = []    # On条件
                sqlcmd     = ''
                for col in PkeyCols:
                    CountCols.append(src_table_name + '.' + col)
                    OnCondCols.append(src_table_name + '.' + col + '=' + dest_table_name + '.' + col)

                if ('temp_rdf_nav_node' == dest_table_name \
                    or 'temp_rdf_nav_link' == dest_table_name):
                    ### 比较条目数 ###
                    if 'temp_rdf_nav_link' == dest_table_name:
                        sqlcmd = """SELECT count(link_id)
                                   FROM rdf_nav_link
                                   where link_id not in 
                                   (
                                   select link_id 
                                     from temp_rdf_nav_link
                                   );"""
                    else :
                        sqlcmd = """
                                select count(node_id)
                                  from (
                                    SELECT ref_node_id as node_id
                                      FROM temp_rdf_nav_link
                                    union
                                    SELECT nonref_node_id as node_id
                                      FROM temp_rdf_nav_link
                                  ) as a
                                  where node_id not in (
                                    select node_id 
                                      from temp_rdf_nav_node
                                  );"""
                    self.pg.execute2(sqlcmd)
                    row = self.pg.fetchone2()
                    if row:
                        if row[0] > 0 :
                            return True
                        else:
                            pass
                    else:
                        pass
                    
                    ### 比较坐标 ###
                    sqlcmd = "SELECT count(" + ','.join(CountCols) + ") \n"
                    sqlcmd += "  FROM dest_table_name \n"
                    sqlcmd += "  LEFT JOIN src_table_name \n"
                    sqlcmd += "  ON " + ' and '.join(OnCondCols) + '\n'
                    sqlcmd += "  WHERE not ST_OrderingEquals(src_table_name." + src_geom_col +", dest_table_name.the_geom)"
                    sqlcmd += "       or dest_table_name.the_geom is null;"
                    
                else:
                    ### 比较坐标 ###
                    sqlcmd = "SELECT count(" + ','.join(CountCols) + ") \n"
                    sqlcmd += "  FROM src_table_name \n"
                    sqlcmd += "  LEFT JOIN dest_table_name \n"
                    sqlcmd += "  ON " + ' and '.join(OnCondCols) + '\n'
                    sqlcmd += "  WHERE not ST_OrderingEquals(ST_GeometryFromText(" + src_geom_col +", 4326), dest_table_name.the_geom)"
                    sqlcmd += "       or dest_table_name.the_geom is null;"
                # 替换表名
                sqlcmd = sqlcmd.replace('dest_table_name', dest_table_name)
                sqlcmd = sqlcmd.replace('src_table_name', src_table_name)
                #print sqlcmd
                self.pg.execute2(sqlcmd)
                row = self.pg.fetchone2()
                if row:
                    if row[0] > 0 :
                        return True
                    else:
                        return False
                else:
                    return False
            else:  # 目标表不存在
                return True
        else:  # 原表不存在
            self.log.error('Dose not exist table %s' % src_table_name)
            return False
        return False
    
    def __call_child_thread(self, _min_id, _max_id, sqlcmd, thread_num = 4, record_num = 1500000):
        "调用多线程"
        global max_id, next_id, mutex
        next_id = _min_id
        max_id  = _max_id
       
        # 创建线程
        mutex   = threading.Lock()  
        threads = []
        i       = 0
        
        # 启动线程
        while i < thread_num:
            threads.append(child_thread(sqlcmd, i, record_num))
            i += 1; 
            
        # 启动线程
        for t in threads:
            t.start()
        
        # 等待子线程结束
        for t in threads:
            t.join()
            
        return 0
    
#####################################################################################                 
# 线程类
#####################################################################################                 

import threading
import common
class child_thread(threading.Thread):
    def __init__(self, sqlcmd, i, number = 1500000):
        threading.Thread.__init__(self)
        self.pg        = common.pg_client()
        self._sqlcmd   = sqlcmd
        self._number   = number + (200000 * i)
        
    def run(self):
        self.pg.connect2()
        global max_id, next_id, mutex
        
        while True:
            # 加锁
            mutex.acquire()
            if next_id > max_id :
                mutex.release()
                return 0
            
            from_id = next_id
            to_id   = 0
            
            if max_id > (next_id + self._number) :
                to_id   = next_id + self._number
                next_id = next_id + self._number + 1
                #print from_id, to_id, self._number, next_id
            else:
                to_id   = max_id
                next_id = max_id + 1
                #print from_id, to_id, self._number, next_id
            mutex.release()
            
            if self.pg.execute2(self._sqlcmd, (from_id, to_id)) == -1:
                return -1
            
            self.pg.commit2()
        
        return 0
    
    