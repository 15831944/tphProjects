# -*- coding: cp936 -*-
'''
Created on 2012-3-13

@author: hongchenzai
'''
from base.rdb_ItemBase import ItemBase
from common import rdb_log
import threading, time

class rdb_line_name(ItemBase):
    '''
    道路名称（地图描画上用）
    '''

    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self, 'line_name')
        
    def Do_CreateTable(self):
        if self.CreateTable2('rdb_line_name') == -1:
            return -1
        if self.CreateTable2('temp_name_sharp_road_name') == -1:
            return -1
        if self.CreateTable2('temp_name_sharp_road_number') == -1:
            return -1
        if self.CreateTable2('temp_line_name_full') == -1:
            return -1
#        if self.CreateTable2('temp_line_name_cut') == -1:
#            return -1
#        if self.CreateTable2('temp_line_name_cut_simplify') == -1:
#            return -1
        return 0
    
    def Do_CreateFunction(self):
        if self.CreateFunction2('rdb_numgeometries') == -1:
            return -1
        if self.CreateFunction2('rdb_geometryn') == -1:
            return -1
        if self.CreateFunction2('rdb_line_substring_for_linename') == -1:
            return -1
        if self.CreateFunction2('rdb_arry_geom_by_idx') == -1:
            return -1
        if self.CreateFunction2('rdb_cvt_line_name_high_level2') == -1:
            return -1
        self.CreateFunction2('rdb_cvt_line_name_high_level')
        return 0
    
    def Do(self):
        return 0
        # 启多线程，合成名称link(以国家为单元)
        #self.CallChildThread_Country()
        # 启多线程，合成名称link(以AdminPlace为单元)
        self.CallChildThread_Hierarchy()
        
        # 把相同的name id 和  road number id的link合成
        #self.MakeTempNameSharp()
        
        self.MakeTempLineNamefull()
        # 对太长的道路名称形状点，按七公里分割
        #self.MakeTempLineNameCut()
        # 对道路名称的形状点，进行抽希。
        #self.MakeTempLineNameCutSimplify()
        # 把分害和抽希后的，形状点，存葋E鷱db_line_name.
        self.InsertFinalLineName()
        # 竵E聄db_link柄涯name_id
        # 由于update耗时长，所以移到做link时，同时做name id.
        #self.__UpdateNameIdOfLink()
        
        return 0
    
    def Do_CreatIndex(self):
        '创建相关柄琪引.'
        self.CreateIndex2('rdb_line_name_kind_code_idx')
        self.CreateIndex2('rdb_line_name_low_level_high_level_idx')
        self.CreateIndex2('rdb_line_name_name_id_idx')
        self.CreateIndex2('rdb_line_name_the_geom_idx')
        return 0
    
    def MakeTempNameSharp(self):
        """1: 把相同name id的link，合成成一条长的线。
           2: 把相同number id的link，合成成一条长的线。
        """
        # road name 
        sqlcmd1 = """
                INSERT INTO temp_name_sharp_road_name(
                            road_name_id, seq_num, scale_type, the_geom)
                (
                SELECT  name_id
                       , generate_series(1, rdb_NumGeometries(the_geom)) as seq_num
                       , scale_type
                       , rdb_GeometryN(the_geom, generate_series(1, rdb_NumGeometries(the_geom)))
                 FROM (
                    SELECT  name_id
                           , scale_type
                           , ST_LineMerge(ST_Union(array_agg(the_geom))) as the_geom
                      FROM link_name_relation_tbl as a
                      LEFT JOIN link_tbl as b
                      ON a.link_id = b.link_id
                      where a.name_type <> 3
                      group by name_id, scale_type
                  ) as A
                );
                """
        # road number
        sqlcmd2 = """
                INSERT INTO temp_name_sharp_road_number(
                            road_name_id, shield_id, seq_num, scale_type, the_geom)
                (
                SELECT  name_id
                       , shield_id
                       , generate_series(1, rdb_NumGeometries(the_geom)) as seq_num
                       , scale_type
                       , rdb_GeometryN(the_geom, generate_series(1, rdb_NumGeometries(the_geom)))
                 FROM (
                    SELECT  name_id
                           , shield_id
                           , scale_type
                           , ST_LineMerge(ST_Union(array_agg(the_geom))) as the_geom
                      FROM link_name_relation_tbl as a
                      LEFT JOIN link_tbl as b
                      ON a.link_id = b.link_id
                      where a.name_type = 3
                      group by name_id, shield_id, scale_type
                 ) as A
                );
                """
            
        rdb_log.log(self.ItemName, 'Now it is inserting to temp_name_sharp_road_name...', 'info')
        # road name
        if self.pg.execute2(sqlcmd1) == -1:
            return -1
        else:
            self.pg.commit2()
        
        # road number
        rdb_log.log(self.ItemName, 'Now it is inserting to temp_name_sharp_road_number...', 'info')
        if self.pg.execute2(sqlcmd2) == -1:
            return -1
        else:
            self.pg.commit2()
            
    def MakeTempLineNamefull(self):
        ''
        ###########################################################################
        # road name
        # 因为现在scale_type已经是根据function_class来分层的，所以不需要再调用rdb_cvt_line_name_high_level2
        sqlcmd1 = """
                INSERT INTO temp_line_name_full(
                            kind_code, name_type, name_prior, low_level, high_level
                            ,name_id, the_geom)
                 (
                    SELECT  
                            kind_code, k.name_type, priority, low_level
                          , k.high_level
                          , new_name_id , the_geom
                      FROM temp_name_sharp_road_name AS s
                      LEFT JOIN temp_kind_code as k
                      ON upper(s.scale_type::character varying(8) || 'xx') = upper(k.ni_code)
                      LEFT JOIN temp_name_dictionary as n
                      ON n.old_name_id = s.road_name_id
                      --LEFT JOIN name_dictionary_tbl as o
                      --ON s.road_name_id = o.name_id
                 );
               """
               
        if self.pg.execute2(sqlcmd1) == -1:
            pass
        else:
            self.pg.commit2()
            
        
        ###########################################################################
        # road number
        # 因为现在scale_type已经是根据function_class来分层的，所以不需要再调用rdb_cvt_line_name_high_level2
        sqlcmd1 = """
                INSERT INTO temp_line_name_full(
                            kind_code, name_type, name_prior, low_level, high_level
                            ,name_id, the_geom)
                 (
                    SELECT  
                            kind_code, k.name_type, priority, low_level
                          , k.high_level
                          , new_name_id, the_geom
                      FROM temp_name_sharp_road_number AS s
                      LEFT JOIN temp_kind_code as k
                      ON upper(scale_type::character varying || 'yy') = upper(k.ni_code)
                      LEFT JOIN temp_name_dictionary as n
                      ON n.old_name_id = s.road_name_id
                      --LEFT JOIN name_dictionary_tbl as o
                      --ON s.road_name_id = o.name_id
                 );
               """
               
        if self.pg.execute2(sqlcmd1) == -1:
            pass
        else:
            self.pg.commit2()
            
        return 0
    
    def MakeTempLineNameCut(self):
        "对太长的道路名称形状点，按七公里分割"
        sqlcmd = """
            INSERT INTO temp_line_name_cut(
                            kind_code, name_type, name_prior, low_level, high_level, 
                            name_id, the_geom)
                (
                 SELECT kind_code, name_type, name_prior, low_level, high_level, 
                       name_id, rdb_arry_geom_by_idx(sub_geom, generate_series(1, array_upper(sub_geom, 1))) 
                 from (
                     SELECT  kind_code, name_type, name_prior, low_level, high_level, 
                           name_id, rdb_line_substring_for_linename(the_geom, 7000.0) as sub_geom
                      FROM temp_line_name_full
                   ) as a
                );
            """
        rdb_log.log(self.ItemName, 'Inserting the cut line name into temp_line_name_cut...', 'info')
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
            return 0
            
    def MakeTempLineNameCutSimplify(self):
        "对道路名称的形状点，进行抽希。"
        sqlcmd = """
            INSERT INTO temp_line_name_cut_simplify(
                        record_id, kind_code, name_type, name_prior, low_level, high_level, 
                        name_id, the_geom)
             (
                SELECT record_id, kind_code, name_type, name_prior, low_level, high_level, 
                       name_id, ST_Simplify(the_geom, 0.000005)
                  FROM temp_line_name_cut
            );
            """
    
        rdb_log.log(self.ItemName, 'Inserting the simplified line name into temp_line_name_cut_simplify...', 'info')
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
            return 0
        
    def InsertFinalLineName(self):
        "把分害和抽希后的，形状点，存葋E鷱db_line_name。"
        rdb_log.log(self.ItemName, 'Inserting final line name...', 'info')

        
        #if pg.CreateFunction2_ByName('rdb_cvt_line_name_high_level') == -1:
        #    pg.close2()
        #    return -1
        
        sqlcmd = """
                INSERT INTO rdb_line_name(
                            kind_code, name_type, name_prior, low_level, high_level, 
                            name_id, the_geom, the_geom_900913)
                 (
                    SELECT  kind_code, name_type, name_prior, low_level, rdb_cvt_line_name_high_level(the_geom, high_level, 1.2), 
                           name_id, the_geom, st_transform(the_geom,3857)
                      FROM temp_line_name_full
                )
                """
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
            return 0
    
    def __UpdateNameIdOfLink(self):
        "竵E耹ink柄蛐的road_name_id及road_number"
        rdb_log.log('line name', 'Start Updating Name Id Of Link', 'info')
            
        sqlcmd = """
                UPDATE rdb_link
                  SET road_name_id = A.name_id
                  from (
                        SELECT tile_link_id, D.new_name_id as name_id
                         FROM link_tbl
                         LEFT JOIN rdb_tile_link
                         ON link_id = old_link_id
                         LEFT JOIN temp_name_dictionary as D
                         ON D.old_name_id = road_number_id and D.seq_nm = 1
                         where road_number_id > 0
                        ) as A
                  where A.tile_link_id = rdb_link.link_id;
                """
        rdb_log.log('line name', 'It is updating Name Id (Road Number) Of Link...', 'info')
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        sqlcmd = '''
                UPDATE rdb_link
                  SET road_name_id = A.name_id
                  from (
                        SELECT  tile_link_id, D.new_name_id as name_id
                          FROM link_tbl
                          LEFT JOIN rdb_tile_link
                          ON link_id = old_link_id
                          LEFT JOIN temp_name_dictionary as D
                          ON D.old_name_id = road_name_id and D.seq_nm = 1
                          where road_name_id > 0
                       ) as A
                   where A.tile_link_id = rdb_link.link_id
                  '''
        
        rdb_log.log('line name', 'It is updating Name Id (Road Name) Of Link...', 'info')
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        rdb_log.log('line name', 'End Update Name Id Of Link', 'info')
        return 0
    
    def CallChildThread_Country(self):
        # 取得所有国家, 从多到少排序
        sqlcmd = """
                Select country_code
                from (
                    SELECT country_code, count(country_code) as cnt
                     FROM link_name_relation_tbl
                     group by country_code
                 ) as a
                order by cnt desc;
                """
        self.pg.execute2(sqlcmd)
        rows = self.pg.fetchall2()
       
        # 按国家
        global country_list, next_country_roadname, next_country_roadnumber, mutex
        country_list = []
        next_country_roadname   = 0
        next_country_roadnumber = 0
        for row in rows:
            country_list.append(row[0])
        print country_list
        # 创建线程
        mutex = threading.Lock()  
        threads = []
        i = 0
        # 启动线程
        while i < 3:
            threads.append(line_name_child02())
            i += 1; 
            
        # 启动线程
        for t in threads:
            t.start()
           
        # 等待子线程结束
        for t in threads:
            t.join()
            
        return 0
    
    def CallChildThread_Hierarchy(self):
        # 取得所有国家, 从多到少排序
        sqlcmd = """
                Select hierarchy_id
                from (
                    SELECT hierarchy_id, count(hierarchy_id) as cnt
                     FROM link_name_relation_tbl
                     group by hierarchy_id
                 ) as a
                order by cnt desc;
                """
        self.pg.execute2(sqlcmd)
        rows = self.pg.fetchall2()
       
        # 按州
        global hierarchy_list, next_hierarchy_roadname, next_hierarchy_roadnumber, mutex_hierarchy
        hierarchy_list = []
        next_hierarchy_roadname   = 0
        next_hierarchy_roadnumber = 0
        for row in rows:
            hierarchy_list.append(row[0])
        print hierarchy_list
        # 创建锁
        mutex_hierarchy = threading.Lock()  
        threads = []
        i = 0
        # 启动线程
        while i < 3:
            threads.append(line_name_child03())
            i += 1; 
            
        # 启动线程
        for t in threads:
            t.start()
           
        # 等待子线程结束
        for t in threads:
            t.join()
            
        return 0


    def _DoContraints(self):
        'create fkey'
        return 0
        sqlcmd = """
                ALTER TABLE rdb_line_name DROP CONSTRAINT if exists rdb_line_name_name_id_fkey;      
                ALTER TABLE rdb_line_name
                  ADD CONSTRAINT rdb_line_name_name_id_fkey FOREIGN KEY (name_id)
                      REFERENCES rdb_name_dictionary (name_id) MATCH FULL
                      ON UPDATE NO ACTION ON DELETE NO ACTION;
                """
        sqlcmd = self.pg.ReplaceDictionary(sqlcmd) 
        
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
            return 0
        

from common import rdb_database            
 
class line_name_child02(threading.Thread):
    "用国家单元，多线程处理line name."
    def __init__(self):
        threading.Thread.__init__(self)
        self.country_code = '' # 当前处历涯国家
        # 注：这里不用使用db_database.rdb_pg.instance(), 因为这它只能技持单线程
        self.pg       = rdb_database.rdb_pg()
        self.ItemName = ''
        
    def run(self):
        self.pg.connect2()
        global country_list, next_country_roadname, next_country_roadnumber, mutex
        
        while True:
            # 加锁
            mutex.acquire()
            # 查找还有合成line name的国家
            if next_country_roadname < len(country_list) or next_country_roadnumber < len(country_list):
                # 先合成道路名称，再处历蝇号
                if next_country_roadname <= next_country_roadnumber:
                    self.country_code = country_list[next_country_roadname]
                    self.ItemName = "Name_" + str(next_country_roadname)+ "_" + self.country_code
                    next_country_roadname += 1;
                    mutex.release()
                    if next_country_roadname == 2:
                        time.sleep(1)
                    self.MakeTempNameSharpRoadName()
                else: 
                    self.country_code = country_list[next_country_roadnumber]
                    self.ItemName = "Number_" + str(next_country_roadnumber)+ "_"+ self.country_code
                    next_country_roadnumber += 1;
                    mutex.release()
                    if next_country_roadname == 2:
                        time.sleep(1)
                    self.MakeTempNameSharpRoadNumber()
                    pass
            else: # 已经没有合成的国家(有些可能正在被合成)
                    mutex.release()
                    break;
        self.pg.close2()
        return 0
    
    def MakeTempNameSharpRoadName(self):
        """1: 把相同name id的link，合成成一条长的线。
        """
        # road name 
        sqlcmd1 = """
                INSERT INTO temp_name_sharp_road_name(
                            road_name_id, seq_num, scale_type, the_geom)
                (
                SELECT  name_id
                       , generate_series(1, rdb_NumGeometries(the_geom)) as seq_num
                       , scale_type
                       , rdb_GeometryN(the_geom, generate_series(1, rdb_NumGeometries(the_geom)))
                 FROM (
                    SELECT  name_id
                           , scale_type
                           , ST_LineMerge(ST_Union(array_agg(the_geom))) as the_geom
                      FROM link_name_relation_tbl as a
                      LEFT JOIN link_tbl as b
                      ON a.link_id = b.link_id
                      where a.name_type <> 3 and country_code = %s
                      group by name_id, scale_type
                  ) as A
                );
                """
       
        rdb_log.log(self.ItemName, 'Start inserting to temp_name_sharp_road_name...', 'info')
        # road name
        if self.pg.execute2(sqlcmd1, (self.country_code,)) == -1:
            return -1
        else:
            self.pg.commit2()
            rdb_log.log(self.ItemName, 'End...', 'info')  
        return 0   
    
    def MakeTempNameSharpRoadNumber(self):
        """把相同number id的link，合成成一条长的线。
        """
        # road number
        sqlcmd2 = """
                INSERT INTO temp_name_sharp_road_number(
                            road_name_id, shield_id, seq_num, scale_type, the_geom)
                (
                SELECT  name_id
                       , shield_id
                       , generate_series(1, rdb_NumGeometries(the_geom)) as seq_num
                       , scale_type
                       , rdb_GeometryN(the_geom, generate_series(1, rdb_NumGeometries(the_geom)))
                 FROM (
                    SELECT  name_id
                           , shield_id
                           , scale_type
                           , ST_LineMerge(ST_Union(array_agg(the_geom))) as the_geom
                      FROM link_name_relation_tbl as a
                      LEFT JOIN link_tbl as b
                      ON a.link_id = b.link_id
                      where a.name_type = 3 and country_code = %s
                      group by name_id, shield_id, scale_type
                 ) as A
                );
                """
        
        rdb_log.log(self.ItemName, 'Start inserting to temp_name_sharp_road_number...', 'info')
        if self.pg.execute2(sqlcmd2, (self.country_code,)) == -1:
            return -1
        else:
            self.pg.commit2()
            rdb_log.log(self.ItemName, 'End...', 'info')   
        return 0 
    
class line_name_child03(threading.Thread):
    "用hierarchy单元，多线程处理line name."
    def __init__(self):
        threading.Thread.__init__(self)
        self.hierarchy_id = None # 当前处理的国家
        # 注：这里不用rdb_database.rdb_pg.instance(), 因为这里要使用多线程.
        self.pg       = rdb_database.rdb_pg()
        self.ItemName = ''
        
    def run(self):
        self.pg.connect2()
        global hierarchy_list, next_hierarchy_roadname, next_hierarchy_roadnumber, mutex_hierarchy
        
        while True:
            # 加锁
            mutex_hierarchy.acquire()
            # 查找还有合成line name的国家
            if next_hierarchy_roadname < len(hierarchy_list) or next_hierarchy_roadnumber < len(hierarchy_list):
                # 先合成道路名称，再处理番号
                if next_hierarchy_roadname <= next_hierarchy_roadnumber:
                    self.hierarchy_id = hierarchy_list[next_hierarchy_roadname]
                    self.ItemName = "Name_" + str(next_hierarchy_roadname)+ "_hierarchy_id_" + str(self.hierarchy_id)
                    next_hierarchy_roadname += 1;
                    mutex_hierarchy.release()
                    if next_hierarchy_roadname == 2:
                        time.sleep(1)
                    self.MakeTempNameSharpRoadName()
                else: 
                    self.hierarchy_id = hierarchy_list[next_hierarchy_roadnumber]
                    self.ItemName = "Number_" + str(next_hierarchy_roadnumber)+ "_hierarchy_id_"+ str(self.hierarchy_id)
                    next_hierarchy_roadnumber += 1;
                    mutex_hierarchy.release()
                    if next_hierarchy_roadname == 2:
                        time.sleep(1)
                    self.MakeTempNameSharpRoadNumber()
                    pass
            else: # 已经没有合成的国家(有些可能正在被合成)
                    mutex_hierarchy.release()
                    break;
        self.pg.close2()
        return 0
    
    def MakeTempNameSharpRoadName(self):
        """不再对相同name id 的 link 进行合并
        """
        # road name 
        sqlcmd1 = """
                INSERT INTO temp_name_sharp_road_name(
                            road_name_id, seq_num, scale_type, the_geom)
                (
                SELECT  name_id
                       , 1              --generate_series(1, rdb_NumGeometries(the_geom)) as seq_num
                       , scale_type
                       , the_geom       --rdb_GeometryN(the_geom, generate_series(1, rdb_NumGeometries(the_geom)))
                 FROM (
                    SELECT  name_id
                           , scale_type
                           , the_geom   -- ST_LineMerge(ST_Union(array_agg(the_geom))) as the_geom
                      FROM link_name_relation_tbl as a
                      LEFT JOIN link_tbl as b
                      ON a.link_id = b.link_id
                      where a.name_type <> 3 and hierarchy_id = %s
                      --group by name_id, scale_type
                  ) as A
                );
                """
       
        rdb_log.log(self.ItemName, 'Start inserting to temp_name_sharp_road_name...', 'info')
        # 插入道路名称
        if self.pg.execute2(sqlcmd1, (self.hierarchy_id,)) == -1:
            return -1
        else:
            self.pg.commit2()
            rdb_log.log(self.ItemName, 'End...', 'info')  
        return 0   
    
    def MakeTempNameSharpRoadNumber(self):
        """不再对相同name id 的 link 进行合并
        """
        # road number
        sqlcmd2 = """
                INSERT INTO temp_name_sharp_road_number(
                            road_name_id, shield_id, seq_num, scale_type, the_geom)
                (
                SELECT  name_id
                       , shield_id
                       , 1            --generate_series(1, rdb_NumGeometries(the_geom)) as seq_num
                       , scale_type
                       , the_geom     --rdb_GeometryN(the_geom, generate_series(1, rdb_NumGeometries(the_geom)))
                 FROM (
                    SELECT  name_id
                           , shield_id
                           , scale_type
                           , the_geom -- ST_LineMerge(ST_Union(array_agg(the_geom))) as the_geom
                      FROM link_name_relation_tbl as a
                      LEFT JOIN link_tbl as b
                      ON a.link_id = b.link_id
                      where a.name_type = 3 and hierarchy_id = %s
                      --group by name_id, shield_id, scale_type
                 ) as A
                );
                """
        
        # 插入道路番号  
        rdb_log.log(self.ItemName, 'Start inserting to temp_name_sharp_road_number...', 'info')
        if self.pg.execute2(sqlcmd2, (self.hierarchy_id,)) == -1:
            return -1
        else:
            self.pg.commit2()
            rdb_log.log(self.ItemName, 'End...', 'info')   
        return 0 
    
    
        
    
    
        