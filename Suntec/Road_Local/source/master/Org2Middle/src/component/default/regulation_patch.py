# -*- coding: UTF8 -*-
'''
Created on 2015-10-28

@author: lsq
'''



import component.component_base
import common.cache_file
from common.common_func import GetPath
import os

class comp_regulation_patch(component.component_base.comp_base):
    '''
    classdocs
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Regulation_patch')
    
    def _DoCreateTable(self):
        
        self.CreateTable2('temp_regulation_patch_tbl')
        
        return 0
    
    def _Do(self):
        
        self._deal_temp_patch_regulation_tbl()
        
        return 0
    
    def _deal_temp_patch_regulation_tbl(self):
        
        self.log.info('Now it is dealing regulation_patch.csv...')
        
        # 根据配置文件信息获取regulation补丁数据
        # regulation补丁数据以规制关联link始终端Node的经纬度给出，配置文件组织形式：
        # 各数据以\t分割
        # 经纬度信息    经纬度关联Node的z-level    规制类别    规制车辆类型    开始年    开始月   开始日    开始时    开始分    结束年    结束月   结束日    结束时    结束分
        if self.import_patch() == -1:
            return -1
        
        self._update_temp_regulation_patch_node_tbl()
        self._update_temp_regulation_patch_link_tbl()
        self._update_regulation_related_tbl()
        
        self.log.info('dealing regulation_patch.csv succeeded')
        return 0
    
    def import_patch(self):
        
        regulation_patch_path = GetPath('regulation_patch')
        recIdx = 0
        if regulation_patch_path:
            if os.path.exists(regulation_patch_path):
                self.log.info('Begin importing regulation_patch...')
                temp_file_obj = common.cache_file.open('temp_regulation_patch_tbl')
                f_regulation_patch = open(regulation_patch_path, 'r')
                for line in f_regulation_patch.readlines(): 
                    linelist = []
                    linelist = line.split('\t')
                    
                    if len(linelist) < 16:
                        self.log.error('Regulation Patch file error: record have not enough phase!!!')
                        return -1
                    
                    str_geom = str(linelist[0])
                    str_z = str(linelist[1])
                    regulation_type = int(linelist[2])
                    car_type = int(linelist[3])
                    s_year = int(linelist[4])
                    s_month = int(linelist[5])
                    s_day = int(linelist[6])
                    s_hour = int(linelist[7])
                    s_min = int(linelist[8])
                    e_year = int(linelist[9])
                    e_month = int(linelist[10])
                    e_day = int(linelist[11])
                    e_hour = int(linelist[12])
                    e_min = int(linelist[13])
                    day_of_week = int(linelist[14])
                    exclude_date = int(linelist[15])
                    recIdx = recIdx + 1
                    
                    rec_string = '%d\t%s\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n' % (recIdx, str_geom, str_z, regulation_type, car_type, s_year, s_month, s_day, s_hour, s_min, e_year, e_month, e_day, e_hour, e_min, day_of_week, exclude_date)
                    temp_file_obj.write(rec_string)
                    temp_file_obj.flush()
                    
                temp_file_obj.seek(0)
                self.pg.copy_from2(temp_file_obj, 'temp_regulation_patch_tbl')
                self.pg.commit2()
                common.cache_file.close(temp_file_obj,True)
                f_regulation_patch.close()
                
                self.log.info('End importing regulation_patch.')
                return 0
        
        return -1 
    
    def _update_temp_regulation_patch_node_tbl(self):
        
        self.log.info('Begin converting regulation patch geometry to node...')
        
        sqlcmd = """
                DROP TABLE IF EXISTS temp_patch_node_tbl;
                CREATE TABLE temp_patch_node_tbl
                AS (
                    SELECT gid, 
                        string_to_array(str_geom, ',')::geometry[] as the_geom_list, 
                        string_to_array(str_z, ',')::smallint[] as z_level_list
                    FROM temp_regulation_patch_tbl
                );
            """
        
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.CreateTable2('mid_temp_patch_node_tbl')
        self.CreateFunction2('mid_update_temp_patch_node_tbl')
        self.pg.callproc('mid_update_temp_patch_node_tbl')
        self.pg.commit2()
        
        self.log.info('End converting regulation patch geometry to node.')
        return 0
    
    def _update_temp_regulation_patch_link_tbl(self):
        
        self.log.info('Begin converting regulation patch node to link...')
        
        self.CreateTable2('mid_temp_patch_link_tbl')
        self.CreateFunction2('mid_findpasslinkbybothnodes')
        self.CreateFunction2('mid_update_temp_patch_link_tbl')
        self.pg.callproc('mid_update_temp_patch_link_tbl')
        self.pg.commit2()
        
        self.log.info('End converting regulation patch node to link.')
        return 0
    
    def _update_regulation_related_tbl(self):
        
        self.log.info('Begin updating regulation related table...')
        
        sqlcmd = """
                DROP SEQUENCE IF EXISTS temp_condition_regulation_tbl_seq;
                CREATE SEQUENCE temp_condition_regulation_tbl_seq;
                SELECT setval('temp_condition_regulation_tbl_seq', cast(max_id as bigint))
                FROM
                (
                    SELECT max(cond_id) as max_id
                    FROM condition_regulation_tbl
                ) a;
            """
            
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        sqlcmd = """
                INSERT INTO condition_regulation_tbl (
                    cond_id, car_type, start_year, start_month, start_day, end_year, end_month, 
                    end_day, start_hour, start_minute, end_hour, end_minute, day_of_week, exclude_date
                )
                SELECT nextval('temp_condition_regulation_tbl_seq') as cond_id, car_type,
                    start_year, start_month, start_day, end_year, end_month, end_day, start_hour, 
                    start_minute, end_hour, end_minute, day_of_week, exclude_date
                FROM (
                    SELECT a.car_type, a.start_year, a.start_month, a.start_day, a.end_year, a.end_month, 
                        a.end_day, a.start_hour, a.start_minute, a.end_hour, a.end_minute, a.day_of_week,
                        a.exclude_date
                    FROM temp_regulation_patch_tbl a
                    LEFT JOIN condition_regulation_tbl b
                        ON a.car_type = b.car_type and 
                            a.start_year = b.start_year and 
                            a.start_month = b.start_month and
                            a.start_day = b.start_day and
                            a.start_hour = b.start_hour and
                            a.start_minute = b.start_minute and
                            a.end_year = b.end_year and 
                            a.end_month = b.end_month and
                            a.end_day = b.end_day and
                            a.end_hour = b.end_hour and
                            a.end_minute = b.end_minute and 
                            a.day_of_week = b.day_of_week and 
                            a.exclude_date = b.exclude_date
                    WHERE not (a.start_year = 0 and 
                                a.start_month = 0 and 
                                a.start_day = 0 and 
                                a.start_minute = 0 and 
                                a.end_year = 0 and 
                                a.end_month = 0 and 
                                a.end_day = 0 and 
                                a.end_hour = 0 and 
                                a.end_minute = 0 and
                                a.day_of_week = 0 and
                                a.exclude_date = 0) and 
                                b.gid IS NULL
                ) c
            """
        
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.CreateFunction2('mid_convert_regulation_from_patch')
        self.pg.callproc('mid_convert_regulation_from_patch')
        self.pg.commit2()
        
        self.log.info('End updating regulation related table.')
        return 0