# -*- coding: cp936 -*-
'''
Created on 2012-3-26

@author: hongchenzai
'''

import component.component_base

class comp_trans_srid_jpn(component.component_base.comp_base):
    '''SRID: 4301==> 4326
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Trans_SRID')
        
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
    
        # link
        self.__transform('road_rlk')
        # node
        self.__transform('road_rnd')
        # 道路结构
        self.__transform('road_rsa')
        
        # POI
        self.__transform('topmap_xda')
        self.__transform('topmap_ant')
        
        self.__transform('middlemap_xda')
        self.__transform('middlemap_ant')
        
        self.__transform('basemap_xda')
        self.__transform('basemap_ant')
        
        self.__transform('citymap_ant')
        self.__transform('citymap_xda')
      
        return 0
    
    def __transform(self, table):
        self.log.info('Transform the table ' + table + '.')
        if self.pg.IsExistTable(table) == False:
            print "Does't exist table " + table + "."
            return 0
        # 判断数据中的SRID
        if self.__CheckSRID(table) == True:
            return 0
        
        # 备分原始数据（对原始表重命名，加上'_org'）
        if self.__RenameOrgTable(table) == -1:
            exit(1)
        
        # 创键新表
        self.pg.connect2()
        if self.CreateTable2(table) == -1:
            exit(1) 
        
        # 拷贝数据，并转换SRID为4326
        if self.__CopyData(table + '_org', table) == -1:
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
    
    def __RenameOrgTable(self, table_name):
        # 给源如表加个后缀"_org"，作为备分。并修改表geometry_columns的相关记录
        print 'backup table: ' + table_name
        self.pg.connect2()
        # drop table_name_org
        if self.pg.IsExistTable(table_name+'_org') == True:
            sqlcmd = """
                SELECT DropGeometryTable('table_name_org');
            """
            sqlcmd = sqlcmd.replace('table_name', table_name)
            if self.pg.execute2(sqlcmd) == -1:
                return -1
            else:
                self.pg.commit2()
                
            sqlcmd = """select Probe_Geometry_Columns();"""
            self.pg.execute2(sqlcmd)
            self.pg.commit2()
        # 取得所有的引索
        index_list = self.__GetIndexList(table_name)
        #print index_list
        # 备份
        sqlcmd = """
        ALTER TABLE table_name RENAME TO table_name_org;
        
        ALTER TABLE table_name_org DROP CONSTRAINT if exists table_name_pkey;
        ALTER TABLE table_name_org
          ADD CONSTRAINT table_name_org_pkey PRIMARY KEY(gid);
        """
        sqlcmd = sqlcmd.replace('table_name', table_name)
        
        self.pg.connect2()
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
        
        sqlcmd = """select Probe_Geometry_Columns();"""
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        # 修改引索的名字
        self.__AlterIndexName(table_name, table_name+"_org", index_list) 
        
        # 修改geometry_columns的表名
        sqlcmd = """
            delete from geometry_columns
               where f_table_name = 'table_name1_org';
               
            update geometry_columns 
               set  f_table_name = 'table_name1_org'
               where f_table_name='table_name1';
        """
        
        sqlcmd = sqlcmd.replace('table_name1', table_name)
        
        #print sqlcmd
        self.pg.connect2()
        if self.pg.execute2(sqlcmd) == -1:
            self.pg.close2()
            return -1
        else:
            self.pg.commit2()
            
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
        col_list1 = ','.join(col_list).replace('the_geom', 'st_transform(the_geom,4326)')
        col_list2 = ','.join(col_list)
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
    
    def transform_the_geom(self, table):
        # 判断，已经是4326了，就不做。
        self.pg.connect2()
        #if self.__check_srid(table) == True:
        #    return 0
        
        # drop table_name_org
        sqlcmd = """
        DROP table if exists table_name_org;
        """
        sqlcmd = sqlcmd.replace('table_name', table)
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
            
        # 备份
        sqlcmd = """
        CREATE table table_name_org as
        (
        select * from table_name
        );
        """
        sqlcmd = sqlcmd.replace('table_name', table)
        
        if self.pg.execute2(sqlcmd) == -1:
            self.pg.close2()
            return -1
        else:
            self.pg.commit2()
            
        # 把原表的4301改成4326
        coord_dimension, geom_type = self.__GetGeomType(table)
        sqlcmd = """
            SELECT AddGeometryColumn('','alter_table','_geo1','4326','geom_type',coord_dimension);
            ALTER TABLE alter_table DROP CONSTRAINT if exists enforce_geotype__geo1;
            ALTER TABLE alter_table DROP CONSTRAINT if exists enforce_dims__geo1;
            ALTER TABLE alter_table DROP CONSTRAINT if exists enforce_geotype__geo1;
            
            update alter_table set _geo1=st_transform(the_geom,4326);
            SELECT DropGeometryColumn ('','alter_table','the_geom');
            
            Alter table alter_table rename _geo1 to the_geom;
            update geometry_columns set  f_geometry_column = 'the_geom' 
              where f_table_name ='alter_table' and f_geometry_column = '_geo1';
            
            ALTER TABLE alter_table
              ADD CONSTRAINT enforce_dims_the_geom CHECK (st_ndims(the_geom) = coord_dimension);
              
            ALTER TABLE alter_table
              ADD CONSTRAINT enforce_geotype_the_geom CHECK (geometrytype(the_geom) = 'geom_type'::text OR the_geom IS NULL);
              
            ALTER TABLE alter_table
              ADD CONSTRAINT enforce_srid_the_geom CHECK (st_srid(the_geom) = 4326);
              
            --SELECT AddGeometryColumn('','alter_table','the_geom','4326','geom_type',coord_dimension);
            --update alter_table set the_geom=_geo1;
            
            --SELECT DropGeometryColumn ('','alter_table','_geo1');
            
        """
        sqlcmd = sqlcmd.replace('alter_table', table)
        sqlcmd = sqlcmd.replace('geom_type', geom_type)
        sqlcmd = sqlcmd.replace('coord_dimension', str(coord_dimension))
        #print sqlcmd
        if self.pg.execute2(sqlcmd) == -1:
            self.pg.close2()
            return -1
        else:
            self.pg.commit2()
        self.pg.close2()
     
    
        
        