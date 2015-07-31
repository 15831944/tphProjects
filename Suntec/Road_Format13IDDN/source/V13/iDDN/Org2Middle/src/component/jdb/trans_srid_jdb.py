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
#        # ��·�ṹ
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
        # check ԭ���ݱ�
        if self.pg.IsExistTable(org_table) == False:
            print "Does't exist table " + org_table + "."
            return 0
        
        if self.__CheckSRID(org_table) == True:  # ��4326
            # �Ѿ���4326
            return 0
            
        # check Ŀ����Ƿ����
        if self.pg.IsExistTable(dest_table) == True:  # Ŀ�����
            #  checkĿ����SRID�ǲ���4326
            if self.__CheckSRID(dest_table) == True:  # ��4326
                # �ж������Ƿ��Ѿ�ת����
                if self.__NeedTranslate(org_table, dest_table) == False:
                    return 0
            else:  # ��4326
                pass   # ����ת��
        else:  # Ŀ�겻����
            pass  # ����ת��
            
        # ����Ŀ���
        if self.CreateTable2(dest_table) == -1:
            exit(1) 
        
        # �������ݣ���ת��SRIDΪ4326
        if self.__CopyData(org_table, dest_table) == -1:
            exit(1)
            
        return 0
    
    def __CheckSRID(self, table):
        '�ж��ǲ���4326, ���4326�ͷ���True'
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
        # ȡ��geomtype�����ͺ�ά��
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
        # ��Դ���Ӹ���׺"_org"����Ϊ���֡����޸ı�geometry_columns����ؼ�¼
        
        return 0
    
    def __CopyData(self, table1, table2):
        # �������ݴ�table1��table2
        sqlcmd = """
        insert into table2(col_list2) 
        (
        SELECT col_list1
        from table1
        );
        """
        # table1 �� table2���ֶ����ָ�����һ���� 
        col_list = self.__GetColumnList(table1)
        # SRID Ҫת��4326
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
        # ȡһ��������
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
        "�Ҹñ������������������������"
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
        '''�ж�ת�����Ƿ�棻���ݶ�ת����ɡ�''' 
        # �ж�ԭ���Ƿ����
        if self.pg.IsExistTable(src_table_name):
            # �ж�Ŀ����Ƿ����
            if self.pg.IsExistTable(dest_table_name):
                if 'the_geom' not in self.__GetColumnList(dest_table_name):
                    return True
                
                ### �ж������Ƿ�����
                PkeyCols   = self.pg.GetPKeyColumns(src_table_name)
                CountCols  = []    # Countʹ�õ��ֶ�
                OnCondCols = []    # On����
                sqlcmd     = ''
                for col in PkeyCols:
                    CountCols.append(src_table_name + '.' + col)
                    OnCondCols.append(src_table_name + '.' + col + '=' + dest_table_name + '.' + col)

                    ### �Ƚ����� ###
                    sqlcmd = "SELECT count(" + ','.join(CountCols) + ") \n"
                    sqlcmd += "  FROM src_table_name \n"
                    sqlcmd += "  LEFT JOIN dest_table_name \n"
                    sqlcmd += "  ON " + ' and '.join(OnCondCols) + '\n'
                    sqlcmd += "  WHERE dest_table_name." + dest_geom_col +" is null or " + '\n'
                    sqlcmd += "  (not ST_OrderingEquals(st_transform(" \
                                + "src_table_name." + src_geom_col +", " + srid +")," \
                                + "dest_table_name." + dest_geom_col +"));"
                    
                # �滻����   
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
            else:  # Ŀ�������
                return True
        else:  # ԭ������
            return False
        return False
        
        