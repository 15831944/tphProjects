# -*- coding: cp936 -*-
import psycopg2
import os

class IllustDataGenerator(object):
    def __init__(self, dbIP, dbName, userName, password):
        self.conn = psycopg2.connect(''' host='%s' dbname='%s' user='%s' password='%s' ''' % \
                                     (dbIP, dbName, userName, password))
        self.pg = self.conn.cursor()
         
    def generateSpotguidePointLatLon(self, outputCSV):
        sqlcmd = '''
                select distinct a.node_id, a.node_id_t, st_x(b.the_geom),  st_y(b.the_geom) 
                from
                rdb_guideinfo_spotguidepoint as a
                left join rdb_node as b
                on a.node_id=b.node_id and a.node_id_t=b.node_id_t
                order by st_x(b.the_geom);
                '''
        self.pg.execute(sqlcmd)
        rows = self.pg.fetchall()
        oFStream = open(outputCSV, "w")
        inti = 1
        for row in rows:
            float1 = row[2]
            int1 = int(float1)
            float2 = (float1 - int1)*60.0
            int2 = int(float2)
            float3 = (float2 - int2) * 60.0 + 0.005
            float4 = row[3]
            int4 = int(float4)
            float5 = (float4 - int4)*60.0
            int5 = int(float5)
            float6 = (float5 - int5) * 60.0 + 0.005
            str1 = '''%.05f,%.05f,E %03d°%02d'%05.2f",N %02d°%02d'%05.2f"'''%\
                    (float1, float4, int1, int2, float3, int4, int5, float6)
            oFStream.write(str1 + "\n")
            inti += 1
        oFStream.close()
        return
            
if __name__ == '__main__':
    dbIP = '''172.26.179.194'''
    dbName = '''MEA8_RDF_2014Q4_0080_0007'''
    userName = '''postgres'''
    password = '''pset123456'''
    destDir = '''C:\\My\\20150611_spotguidepoint_statistic\\%s''' % dbIP
    if os.path.isdir(destDir) is False:
        os.mkdir(destDir)
    datMaker = IllustDataGenerator(dbIP, dbName, userName, password)
    datMaker.generateSpotguidePointLatLon('''%s\\%s.txt'''%(destDir, dbName))
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    