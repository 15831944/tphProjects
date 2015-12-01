# -*- coding: cp936 -*-
import os
import shutil
import psycopg2

# ��Щ������ϵͳ������ע��
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

# ��ĳ�����ݿ��е�ÿ����������е��������csv�ļ��У��Է���ʹ��visio���벢�鿴���ݿ��ṹ��ͼ��
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
    
    # �˺�����ѯĳ���ݿ��е����б������ڴ˺�������ӱ�Ĺ���������
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
            # �����ǹ���������
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
    
    # ���������б��������csv�ļ���
    # 1.����һ���������ݿ��visio�ļ�
    # 2.ʹ�÷��򹤳�
    # 3.����һ������microsoft text driver������Դ������ǰ�浼��������csv�ļ�������������Դ���ļ����С�
    # 4.ʹ�����潨��������Դ�����ɵ�������csv��visio�У����ɿ������б�ṹ��    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    