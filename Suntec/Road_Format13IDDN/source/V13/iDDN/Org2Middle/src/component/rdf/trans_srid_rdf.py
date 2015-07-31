# -*- coding: cp936 -*-
'''
Created on 2011-12-5

@author: hongchenzai
'''
import base
import threading

class comp_trans_srid_rdf(base.component_base.comp_base):
    def __init__(self):
        '''
        Constructor
        '''
        base.component_base.comp_base.__init__(self, 'Trans_SRID')

    
    def _DoCreateIndex(self):

        if self.CreateIndex2('temp_wkt_building_900913_the_geom_idx') == -1:
            return -1
        if self.CreateIndex2('temp_wkt_face_900913_the_geom_idx') == -1:
            return -1
        if self.CreateIndex2('temp_wkt_link_900913_the_geom_idx') == -1:
            return -1
        if self.CreateIndex2('temp_wkt_location_900913_the_geom_idx') == -1:
            return -1
        return 0
    
    def _Do(self):
        self.__trans_wtk_link()
        self.__trans_wkt_building()
        self.__trans_wkt_face()
        self.__trans_wkt_location()

        return 0
    
    def __trans_wtk_link(self):
        if not self.__NeedTranslate('temp_wkt_link', 'temp_wkt_link_900913'):
            return 0
        
        self.log.info('Start trans_wtk_link_900913.')
        if self.CreateTable2('temp_wkt_link_900913') == -1:
            return -1
        
        # 求最小和最大id
        sqlcmd = """
            SELECT min(link_id), max(link_id)
              FROM temp_wkt_link;
            """
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchone2()
        min_id  = row[0]
        max_id  = row[1]
        
        sqlcmd  = """
                INSERT INTO temp_wkt_link_900913(link_id, the_geom)
                (
                SELECT link_id, ST_TransForm(the_geom, 900913)
                  FROM temp_wkt_link
                  where link_id >= %s and link_id <= %s
                );
                """
        self.__call_child_thread(min_id, max_id, sqlcmd)
        
        self.log.info('End trans_wtk_link.')
        return 0
    
    def __trans_wkt_building(self):
        if not self.__NeedTranslate('temp_wkt_building', 'temp_wkt_building_900913'):
            return 0
        
        self.log.info('Start trans_wkt_building_900913.')
        if self.CreateTable2('temp_wkt_building_900913') == -1:
            return -1
        # 求最小和最大id
        sqlcmd = """
            SELECT min(building_id), max(building_id)
              FROM temp_wkt_building;
            """
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchone2()
        min_id  = row[0]
        max_id  = row[1]
        
        sqlcmd  = """
                INSERT INTO temp_wkt_building_900913(
                            building_id, the_geom)
                (
                SELECT building_id, ST_TransForm(the_geom, 900913) 
                  FROM temp_wkt_building
                  where building_id >= %s and building_id <= %s
                );
                """
        self.__call_child_thread(min_id, max_id, sqlcmd)
        
        self.log.info('End trans_wkt_building.')
        return 0
    
    def __trans_wkt_face(self):
        if not self.__NeedTranslate('temp_wkt_face', 'temp_wkt_face_900913'):
            return 0
        self.log.info('Start trans_wkt_face_900913.')
        if self.CreateTable2('temp_wkt_face_900913') == -1:
            return -1
        # 求最小和最大id
        sqlcmd = """
            SELECT min(face_id), max(face_id)
              FROM temp_wkt_face;
            """
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchone2()
        min_id  = row[0]
        max_id  = row[1]
        
        sqlcmd  = """
                INSERT INTO temp_wkt_face_900913(
                            face_id, the_geom)
                (
                SELECT face_id, ST_TransForm(the_geom, 900913) 
                  FROM temp_wkt_face
                  where face_id >= %s and face_id <= %s
                );
                """
        self.__call_child_thread(min_id, max_id, sqlcmd)
        
        self.log.info('End trans_wkt_face.')
        return 0
    
    def __trans_wkt_location(self):
        if not self.__NeedTranslate('temp_wkt_location', 'temp_wkt_location_900913'):
            return 0
        self.log.info('Start trans_wkt_location_900913.')
        if self.CreateTable2('temp_wkt_location_900913') == -1:
            return -1
        # 求最小和最大id
        sqlcmd = """
            SELECT min(location_id), max(location_id)
              FROM temp_wkt_location;
            """
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchone2()
        min_id  = row[0]
        max_id  = row[1]
        
        sqlcmd  = """
                INSERT INTO temp_wkt_location_900913(
                            location_id, the_geom)
                (
                SELECT location_id, ST_TransForm(the_geom, 900913)   
                  FROM temp_wkt_location
                  where location_id >= %s and location_id <= %s
                );
                """
        self.__call_child_thread(min_id, max_id, sqlcmd)
        
        self.log.info('End trans_wkt_location.')
        return 0

    
    def __call_child_thread(self, _min_id, _max_id, sqlcmd, thread_num = 4, record_num = 1500000):
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
    
    def __NeedTranslate(self, table_name, table_name_900913):
        '''判定转换表是否存；内容都转换完成。''' 
        #table_name_900913 = table_name + '_900913'
        # 判断原表是否存在
        if self.pg.IsExistTable(table_name):
            # 判断900913表是否存在
            if self.pg.IsExistTable(table_name_900913):
                if 'the_geom' not in self.__GetColumnList(table_name_900913):
                    return True
                
                ### 判断内容是否完整
                PkeyCols   = self.pg.GetPKeyColumns(table_name)
                CountCols  = []    # Count使用的字段
                OnCondCols = []    # On条件
                for col in PkeyCols:
                    CountCols.append(table_name + '.' + col)
                    OnCondCols.append(table_name + '.' + col + '=' + table_name_900913 + '.' + col)
                sqlcmd = "SELECT count(" + ','.join(CountCols) + ") \n"
                sqlcmd += "  FROM table_name \n"
                sqlcmd += "  LEFT JOIN table_name_900913 \n"
                sqlcmd += "  ON " + ' and '.join(OnCondCols) + '\n'
                sqlcmd += """  WHERE not ST_OrderingEquals(ST_TransForm(table_name.the_geom, 900913), 
                                   table_name_900913.the_geom); """
                # 替换表名   
                sqlcmd = sqlcmd.replace('table_name_900913', table_name_900913)
                sqlcmd = sqlcmd.replace('table_name', table_name)
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
                pass
            else:  # 00913表不存在
                return True
        else:  # 原表不存在
            return False
        return False
    
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
                break
            
            from_id = next_id
            to_id   = 0
            
            if max_id > (next_id + self._number) :
                to_id   = next_id + self._number
                next_id = next_id + self._number + 1
                print from_id, to_id, self._number, next_id
            else:
                to_id   = max_id
                next_id = max_id + 1
                print from_id, to_id, self._number, next_id
            mutex.release()
            
            if self.pg.execute2(self._sqlcmd, (from_id, to_id)) == -1:
                return -1
            else :
                self.pg.commit2()
        
        self.pg.close2()
        return 0
