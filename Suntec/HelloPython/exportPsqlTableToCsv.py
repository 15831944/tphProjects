# -*- coding: cp936 -*-
import os
import shutil
import psycopg2

# 这些表属于系统表，不关注。
excludedTableList = \
["addr",
 "addrfeat",
 "bg",
 "county",
 "county_lookup",
 "countysub_lookup",
 "cousub",
 "direction_lookup",
 "edges",
 "faces",
 "featnames",
 "geocode_settings",
 "layer",
 "loader_lookuptables",
 "loader_platform",
 "loader_variables",
 "pagc_gaz",
 "pagc_lex",
 "pagc_rules",
 "place",
 "place_lookup",
 "secondary_unit_lookup",
 "spatial_ref_sys",
 "state",
 "state_lookup",
 "street_type_lookup",
 "tabblock",
 "topology",
 "tract",
 "zcta5",
 "zip_lookup",
 "zip_lookup_all",
 "zip_lookup_base",
 "zip_state",
 "zip_state_loc"]

# 将某个数据库中的每个表的所有列导出到多个csv文件中，以方便使用visio导入并查看数据库表结构视图。
class PsqlTablesExporter(object):
    def __init__(self, dbIP, dbName, userName, password):
        self.conn = psycopg2.connect(''' host='%s' dbname='%s' user='%s' password='%s' ''' % \
                                     (dbIP, dbName, userName, password))
        self.pg = self.conn.cursor()
         
    def exportTablesToDir(self, exportDir):
        if os.path.isdir(exportDir) == True:
            shutil.rmtree(exportDir)
        os.makedirs(exportDir)
        tableList = self.listAllTables()
        for oneTable in tableList:
            sqlcmd = """
                        select column_name, data_type 
                        from information_schema.columns
                        where table_schema='public' and table_name='%s'; 
                     """
            
            with open(os.path.join(exportDir, oneTable+'.csv'), "w") as oFStream:
                self.pg.execute(sqlcmd % oneTable)
                rows = self.pg.fetchall()
                columnList = []
                for row in rows:
                    columnList.append(' : '.join(row))
                oFStream.write(','.join(columnList))
        return
    
    # 此函数查询某数据库中的所有表，可以在此函数中添加表的过滤条件。
    def listAllTables(self):
        tableList = []
        sqlcmd = """ select A.relname,B.reltuples  
                     from pg_stat_user_tables A, pg_class B 
                     where A.relname=B.relname 
                     order by A.relname 
                 """
        self.pg.execute(sqlcmd)
        rows = self.pg.fetchall()
        for row in rows:
            tableName = row[0]
            # 这里是过滤条件。
            if tableName not in excludedTableList:
                #if tableName.find("rdb_") == 0:
                tableList.append(row[0])
        return tableList
         
if __name__ == '__main__':
    dbIP = '''192.168.10.20'''
    dbNameList = ['C_NIMIF_15SUM_0082_0003']
    userName = '''postgres'''
    password = '''pset123456'''
    for dbName in dbNameList:
        try:
            datMaker = PsqlTablesExporter(dbIP, dbName, userName, password)
            datMaker.exportTablesToDir(r"""C:\My\text_db_src""")
        except Exception, ex:
            print '''%s/%s.\nmsg:\t%s\n''' % (dbIP, dbName, ex)
    
    # 导出了所有表的列名到csv文件后：
    # 1.创建一个基于数据库的visio文件
    # 2.使用反向工程
    # 3.建立一个基于microsoft text driver的数据源，并将前面导出的所有csv文件拷贝到该数据源的文件夹中。
    # 4.使用上面建立的数据源，即可导入所有csv到visio中，即可看到所有表结构。    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    