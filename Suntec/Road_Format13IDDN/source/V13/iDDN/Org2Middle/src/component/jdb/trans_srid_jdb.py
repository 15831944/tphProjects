# -*- coding: cp936 -*-
'''
Created on 2012-3-26

@author: hongchenzai
'''

from base import comp_base

class comp_trans_srid_jdb(comp_base):
    '''SRID: 4301==> 4326
    '''

    def __init__(self):
        '''
        Constructor
        '''
        comp_base.__init__(self, 'Trans_SRID')
        
    def _Do(self):
        sqlcmd = """
                UPDATE spatial_ref_sys
                  SET proj4text='+proj=longlat +ellps=bessel towgs84=-146.43,507.89,681.46'
                  WHERE srid=4301;
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        sqlcmd = """select Probe_Geometry_Columns();"""
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        trans_tbl_list = [
                          'road_link',
                          'road_node',
                          'segment_attr',
                          'facil_info_point',
                          'highway_node',
                          'base_admin',
                          'access_point'
                          ]

        for org_tbl in trans_tbl_list:
            self.__transform(org_tbl, org_tbl + '_4326')

        # link
#        self.__transform('road_link')
#        # node
#        self.__transform('road_node')
#        # 道路结构
#        self.__transform('road_rsa')
#        
#        # POI
#        self.__transform('topmap_xda')
#        self.__transform('topmap_ant')
#        
#        self.__transform('middlemap_xda')
#        self.__transform('middlemap_ant')
#        
#        self.__transform('basemap_xda')
#        self.__transform('basemap_ant')
#        
#        self.__transform('citymap_ant')
#        self.__transform('citymap_xda')
      
        return 0
    
    def __transform(self, org_table, dest_table):
        self.log.info('Transform the table ' + org_table + '.')
        # check 原数据表
        if self.pg.IsExistTable(org_table) == False:
            print "Does't exist table " + org_table + "."
            return 0
        
        if self.__CheckSRID(org_table) == True:  # 是4326
            # 已经是4326
            return 0
            
        # check 目标表是否存在
        if self.pg.IsExistTable(dest_table) == True:  # 目标存在
            #  check目标表的SRID是不是4326
            if self.__CheckSRID(dest_table) == True:  # 是4326
                # 判断数据是否已经转换过
                if self.__NeedTranslate(org_table, dest_table) == False:
                    return 0
            else:  # 非4326
                pass   # 继续转换
        else:  # 目标不存在
            pass  # 继续转换
            
        # 创键目标表
        if self.CreateTable2(dest_table) == -1:
            exit(1) 
        
        # 拷贝数据，并转换SRID为4326
        if self.__CopyData(org_table, dest_table) == -1:
            exit(1)
            
        return 0
    
    def __CheckSRID(self, table):
        '判断是不是4326, 如果4326就返回True'
        self.pg.connect1()
        sqlcmd = """
                SELECT srid
                  FROM geometry_columns
                  where f_table_name = '""" + table + "'";
                
        if self.pg.query(sqlcmd) == -1:
            self.pg.close1()
            return False
        
        row = self.pg.fetchone()
        if row:
            if row[0] == 4326:
                self.pg.close1()
                return True
        else:
            print "Does't exist table " + table + "."
            self.pg.close1()
            return True
        
        return False

    def __GetGeomType(self, table):
        # 取得geomtype的类型和维数
        self.pg.connect1()
        sqlcmd = """
                SELECT coord_dimension, type 
                FROM geometry_columns
                where f_table_name = '""" + table + "'";
                
        if self.pg.query(sqlcmd) == -1:
            return None, None
        
        row = self.pg.fetchone()
        if row:
            return row[0], row[1]
        return None, None
    
    def __CopyIndex(self, table_name):
        # 给源如表加个后缀"_org"，作为备分。并修改表geometry_columns的相关记录
        
        return 0
    
    def __CopyData(self, table1, table2):
        # 拷贝数据从table1到table2
        sqlcmd = """
        insert into table2(col_list2) 
        (
        SELECT col_list1
        from table1
        );
        """
        # table1 和 table2的字段名字个数是一样的 
        col_list = self.__GetColumnList(table1)
        # SRID 要转成4326
        col_list1 = ','.join(col_list).replace('shape', 'st_transform(shape,4326)')
        col_list2 = (','.join(col_list)).replace('shape', 'the_geom')
        sqlcmd    = sqlcmd.replace('table1', table1)
        sqlcmd    = sqlcmd.replace('table2', table2)
        sqlcmd    = sqlcmd.replace('col_list1', col_list1)
        sqlcmd    = sqlcmd.replace('col_list2', col_list2)
        
        #print sqlcmd
        self.pg.connect2()
        if self.pg.execute2(sqlcmd) == -1:
            self.pg.close2()
            return -1
        else:
            self.pg.commit2()

        return 0
        
    def __GetColumnList(self, table):
        # 取一个表里有
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
        self.pg.connect1()
        sqlcmd = sqlcmd.replace('table_name', table)
        if self.pg.query(sqlcmd) == -1:
            return None, None
        
        col_list = []
        rows = self.pg.fetchall()
        for row in rows:
            if row:
                col_list.append(row[0])
        return col_list
    
    def __AlterIndexName(self, table1, table2, index_list):
        self.pg.connect2()
        for index in index_list:
            new_index = index.replace(table1, table2)
            sqlcmd = "ALTER INDEX " + index +" RENAME TO " + new_index + ';'
            self.pg.execute2(sqlcmd)
            
        self.pg.commit2()
        return 0
    
    def __GetIndexList(self, table):
        "找该表的所有索引（不包括主键）"
        sqlcmd = """
                SELECT relname
                  FROM pg_class
                  where relfilenode in (
                     SELECT indexrelid
                      FROM pg_index
                      where indrelid in (
                             SELECT relid
                               FROM pg_statio_user_tables
                               where relname = 'table_name'
                        )
                    )
                    and relname not like '%pkey'
        """
        self.pg.connect1()
        sqlcmd = sqlcmd.replace('table_name', table)
        if self.pg.query(sqlcmd) == -1:
            return None, None
        
        index_list = []
        rows = self.pg.fetchall()
        for row in rows:
            if row:
                index_list.append(row[0])
        return index_list
     
    def __NeedTranslate(self, src_table_name, dest_table_name, \
                        src_geom_col = 'shape', dest_geom_col = 'the_geom', srid = '4326'):
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

                    ### 比较坐标 ###
                    sqlcmd = "SELECT count(" + ','.join(CountCols) + ") \n"
                    sqlcmd += "  FROM src_table_name \n"
                    sqlcmd += "  LEFT JOIN dest_table_name \n"
                    sqlcmd += "  ON " + ' and '.join(OnCondCols) + '\n'
                    sqlcmd += "  WHERE dest_table_name." + dest_geom_col +" is null or " + '\n'
                    sqlcmd += "  (not ST_OrderingEquals(st_transform(" \
                                + "src_table_name." + src_geom_col +", " + srid +")," \
                                + "dest_table_name." + dest_geom_col +"));"
                    
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
            return False
        return False
        
        