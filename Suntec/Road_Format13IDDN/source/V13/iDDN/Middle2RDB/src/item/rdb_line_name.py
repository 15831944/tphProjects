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
    ��·���ƣ���ͼ�軭���ã�
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
        # �����̣߳��ϳ�����link(�Թ���Ϊ��Ԫ)
        #self.CallChildThread_Country()
        # �����̣߳��ϳ�����link(��AdminPlaceΪ��Ԫ)
        self.CallChildThread_Hierarchy()
        
        # ����ͬ��name id ��  road number id��link�ϳ�
        #self.MakeTempNameSharp()
        
        self.MakeTempLineNamefull()
        # ��̫���ĵ�·������״�㣬���߹���ָ�
        #self.MakeTempLineNameCut()
        # �Ե�·���Ƶ���״�㣬���г�ϣ��
        #self.MakeTempLineNameCutSimplify()
        # �ѷֺ��ͳ�ϣ��ģ���״�㣬��ȁE��db_line_name.
        self.InsertFinalLineName()
        # ��E�rdb_link����name_id
        # ����update��ʱ���������Ƶ���linkʱ��ͬʱ��name id.
        #self.__UpdateNameIdOfLink()
        
        return 0
    
    def Do_CreatIndex(self):
        '������ر�����.'
        self.CreateIndex2('rdb_line_name_kind_code_idx')
        self.CreateIndex2('rdb_line_name_low_level_high_level_idx')
        self.CreateIndex2('rdb_line_name_name_id_idx')
        self.CreateIndex2('rdb_line_name_the_geom_idx')
        return 0
    
    def MakeTempNameSharp(self):
        """1: ����ͬname id��link���ϳɳ�һ�������ߡ�
           2: ����ͬnumber id��link���ϳɳ�һ�������ߡ�
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
        # ��Ϊ����scale_type�Ѿ��Ǹ���function_class���ֲ�ģ����Բ���Ҫ�ٵ���rdb_cvt_line_name_high_level2
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
        # ��Ϊ����scale_type�Ѿ��Ǹ���function_class���ֲ�ģ����Բ���Ҫ�ٵ���rdb_cvt_line_name_high_level2
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
        "��̫���ĵ�·������״�㣬���߹���ָ�"
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
        "�Ե�·���Ƶ���״�㣬���г�ϣ��"
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
        "�ѷֺ��ͳ�ϣ��ģ���״�㣬��ȁE��db_line_name��"
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
        "��E�link���е�road_name_id��road_number"
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
        # ȡ�����й���, �Ӷൽ������
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
       
        # ������
        global country_list, next_country_roadname, next_country_roadnumber, mutex
        country_list = []
        next_country_roadname   = 0
        next_country_roadnumber = 0
        for row in rows:
            country_list.append(row[0])
        print country_list
        # �����߳�
        mutex = threading.Lock()  
        threads = []
        i = 0
        # �����߳�
        while i < 3:
            threads.append(line_name_child02())
            i += 1; 
            
        # �����߳�
        for t in threads:
            t.start()
           
        # �ȴ����߳̽���
        for t in threads:
            t.join()
            
        return 0
    
    def CallChildThread_Hierarchy(self):
        # ȡ�����й���, �Ӷൽ������
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
       
        # ����
        global hierarchy_list, next_hierarchy_roadname, next_hierarchy_roadnumber, mutex_hierarchy
        hierarchy_list = []
        next_hierarchy_roadname   = 0
        next_hierarchy_roadnumber = 0
        for row in rows:
            hierarchy_list.append(row[0])
        print hierarchy_list
        # ������
        mutex_hierarchy = threading.Lock()  
        threads = []
        i = 0
        # �����߳�
        while i < 3:
            threads.append(line_name_child03())
            i += 1; 
            
        # �����߳�
        for t in threads:
            t.start()
           
        # �ȴ����߳̽���
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
    "�ù��ҵ�Ԫ�����̴߳���line name."
    def __init__(self):
        threading.Thread.__init__(self)
        self.country_code = '' # ��ǰ�����Ĺ���
        # ע�����ﲻ��ʹ��db_database.rdb_pg.instance(), ��Ϊ����ֻ�ܼ��ֵ��߳�
        self.pg       = rdb_database.rdb_pg()
        self.ItemName = ''
        
    def run(self):
        self.pg.connect2()
        global country_list, next_country_roadname, next_country_roadnumber, mutex
        
        while True:
            # ����
            mutex.acquire()
            # ���һ��кϳ�line name�Ĺ���
            if next_country_roadname < len(country_list) or next_country_roadnumber < len(country_list):
                # �Ⱥϳɵ�·���ƣ��ٴ���Ӭ��
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
            else: # �Ѿ�û�кϳɵĹ���(��Щ�������ڱ��ϳ�)
                    mutex.release()
                    break;
        self.pg.close2()
        return 0
    
    def MakeTempNameSharpRoadName(self):
        """1: ����ͬname id��link���ϳɳ�һ�������ߡ�
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
        """����ͬnumber id��link���ϳɳ�һ�������ߡ�
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
    "��hierarchy��Ԫ�����̴߳���line name."
    def __init__(self):
        threading.Thread.__init__(self)
        self.hierarchy_id = None # ��ǰ����Ĺ���
        # ע�����ﲻ��rdb_database.rdb_pg.instance(), ��Ϊ����Ҫʹ�ö��߳�.
        self.pg       = rdb_database.rdb_pg()
        self.ItemName = ''
        
    def run(self):
        self.pg.connect2()
        global hierarchy_list, next_hierarchy_roadname, next_hierarchy_roadnumber, mutex_hierarchy
        
        while True:
            # ����
            mutex_hierarchy.acquire()
            # ���һ��кϳ�line name�Ĺ���
            if next_hierarchy_roadname < len(hierarchy_list) or next_hierarchy_roadnumber < len(hierarchy_list):
                # �Ⱥϳɵ�·���ƣ��ٴ�����
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
            else: # �Ѿ�û�кϳɵĹ���(��Щ�������ڱ��ϳ�)
                    mutex_hierarchy.release()
                    break;
        self.pg.close2()
        return 0
    
    def MakeTempNameSharpRoadName(self):
        """���ٶ���ͬname id �� link ���кϲ�
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
        # �����·����
        if self.pg.execute2(sqlcmd1, (self.hierarchy_id,)) == -1:
            return -1
        else:
            self.pg.commit2()
            rdb_log.log(self.ItemName, 'End...', 'info')  
        return 0   
    
    def MakeTempNameSharpRoadNumber(self):
        """���ٶ���ͬname id �� link ���кϲ�
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
        
        # �����·����  
        rdb_log.log(self.ItemName, 'Start inserting to temp_name_sharp_road_number...', 'info')
        if self.pg.execute2(sqlcmd2, (self.hierarchy_id,)) == -1:
            return -1
        else:
            self.pg.commit2()
            rdb_log.log(self.ItemName, 'End...', 'info')   
        return 0 
    
    
        
    
    
        