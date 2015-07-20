# -*- coding: cp936 -*-
import os
import sys
import shutil
import psycopg2

excludedTableList = \
[
"addr",
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
"zip_state_loc"
]

class Signpost_uc_checker(object):
    def __init__(self, dbIP, dbName, userName, password):
        self.conn = psycopg2.connect(''' host='%s' dbname='%s' user='%s' password='%s' ''' % \
                                     (dbIP, dbName, userName, password))
        self.pg = self.conn.cursor()
         
    def checkBegin(self, exportDir):
        if os.path.isdir(exportDir) == True:
            shutil.rmtree(exportDir)
        os.makedirs(exportDir)
        sqlcmd = """
                    copy 
                    (
                    SELECT * FROM %s limit 1
                    )
                    to stdout
                    delimiter as ','
                    csv header;
                """
        tableList = self.listAllTables()
        for oneTable in tableList:
            oldStdOut = sys.stdout
            self.pg.execute(sqlcmd % (oneTable, exportDir, oneTable))      
        return
    
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
            if row[0] not in excludedTableList:
                tableList.append(row[0])
        return tableList
         
if __name__ == '__main__':
    dbIP = '''127.0.0.1'''
    dbNameList = ['AF_TOMTOM_201503_ORG']
    userName = '''postgres'''
    password = '''pset123456'''
    for dbName in dbNameList:
        try:
            datMaker = Signpost_uc_checker(dbIP, dbName, userName, password)
            datMaker.checkBegin("c:/my/20150720")
        except Exception, ex:
            print '''db:\t%s.\nmsg:\t%s\n''' % (dbName, ex)
            
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    