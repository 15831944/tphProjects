#encoding=utf-8
import psycopg2

class PsqlTablesExporter(object):
    def __init__(self, dbIP, dbName, userName, password):
        self.conn = psycopg2.connect(''' host='%s' dbname='%s' user='%s' password='%s' ''' % \
                                     (dbIP, dbName, userName, password))
        self.pg = self.conn.cursor()
         
    def xxxxx(self):
        self.pg.execute("""
drop table if exists temp_spotguide_mdps_pictures;
create table temp_spotguide_mdps_pictures(pic_name varchar not null);
""")
        strPath = r"\\192.168.8.14\Private\Data_Authoring\DB_ExtendSource\1_Asia\HKGMAC\Here\2014Q4\illust\mdps_pic_list.csv"
        with open(strPath, 'a+') as iFStream:
            self.pg.copy_from(iFStream, 'temp_spotguide_mdps_pictures')
        self.conn.commit()
        return
         
if __name__ == '__main__':
    dbIP = '''192.168.10.14'''
    dbNameList = ['17cy_HKGMAC_RDF_CI']
    userName = '''postgres'''
    password = '''pset123456'''
    for dbName in dbNameList:
        try:
            datMaker = PsqlTablesExporter(dbIP, dbName, userName, password)
            datMaker.xxxxx()
        except Exception, ex:
            print '''%s/%s.\nmsg:\t%s\n''' % (dbIP, dbName, ex)
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    