# -*- coding: cp936 -*-
import os
import psycopg2

# 20150910��������2015Q3����ʱ����Ҫͳ�ƴ˰汾�����Ƿ�������
# ͨ��ͳ��2014Q4��2015Q1���汾ԭʼ���ݱ�ĸ������Լ�����������Ŀ����������ǵĲ���Ƿ�̫��ó����ԵĹ��㡣

# ���ݿ������Щ������ϵͳ���������Խ������ų����⡣
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
 "zip_state_loc",
 "org_name_vs-a",
 "org_name_vs-b",
 "org_name_vs-c",
 "org_name_vs-d",
 "org_name_vs-e",
 "org_name_vs-f",
 "org_not-in",
 "org_one-way"]

# ��ĳ�����ݿ������еı��ÿ�ű����Ŀ��������һ��txt�С�
# ��ͳ�ƶ�����ݿ�ʱ�������໥���бȽϡ�
class PsqlTablesExporter(object):
    def __init__(self, dbIP, dbName, userName, password):
        self.conn = psycopg2.connect(''' host='%s' dbname='%s' user='%s' password='%s' ''' % \
                                     (dbIP, dbName, userName, password))
        self.pg = self.conn.cursor()
         
    def updateOutlinkErrorRecords(self, exportTxt):
        tableList = self.listAllTables()
        strResultList = []
        for oneTable in tableList:
            sqlcmd = """
                        select count(*) from %s; 
                     """
            
            self.pg.execute(sqlcmd % oneTable)
            row = self.pg.fetchone()
            recordCount = row[0]
            strOutput = """%s: %d\n""" % (oneTable, recordCount)
            strResultList.append(strOutput)
            
        strResultList.sort()
        with open(exportTxt, "w") as oFStream:
            for oneStr in strResultList:
                oFStream.write(oneStr)
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
                tableList.append(row[0])
        return tableList
         
if __name__ == '__main__':
    dbIP = '''192.168.10.19'''
    dbNameList = ['SGP_MYS_RDF_2015Q3_0065_0019']
    userName = '''postgres'''
    password = '''pset123456'''
    outputDir = r"""C:\Users\hexin\Desktop\20151012\sgp_mys_for_hyp"""
    if os.path.isdir(outputDir) == False:
        os.makedirs(outputDir)
    for dbName in dbNameList:
        try:
            datMaker = PsqlTablesExporter(dbIP, dbName, userName, password)
            datMaker.updateOutlinkErrorRecords(os.path.join(outputDir, dbIP+'-'+dbName+'.txt'))
        except Exception, ex:
            print '''%s/%s.\nmsg:\t%s\n''' % (dbIP, dbName, ex)

    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    