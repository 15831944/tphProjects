# -*- coding: cp936 -*-
'''
Created on 2012-11-26
'''
import os
import psycopg2

def Function1(srcDir):
    if(os.path.exists(srcDir) == False):
        print "source directory does not exist: " + srcDir
        return
    
    conn = psycopg2.connect(''' host='127.0.0.1' dbname='17cy_IND_MMI_CI_tph'
                                user='postgres' password='pset123456' ''')
    pgcur2 = conn.cursor()
    pgcur2.execute("drop table if exists tempTable1")
    pgcur2.execute("create table tempTable1(filename character varying(256))")
    conn.commit()
    
    for curDir,dirNames,fileNames in os.walk(srcDir):
        for oneFile in fileNames:
            if oneFile[-4:] == ".svg":
                pgcur2.execute("insert into tempTable1 (filename) values ('%s')" % oneFile)    
    conn.commit()
    
    pgcur2.execute("select distinct filename from tempTable1")
    rows = pgcur2.fetchall()
    svgFileList = []
    for row in rows:
        svgFileList.append(row[0])
    svgFileCount = len(svgFileList)
   
    pgcur2.execute("select distinct sar_filename from rdfpl_all_jv_lat_display_org")
    rows = pgcur2.fetchall()
    svgFileList2 = []
    for row in rows:
        svgFileList2.append(row[0])
    svgFileCount2 = len(svgFileList2) 
    
    
    
    
    return

if __name__ == '__main__':
    Function1('''C:\\My\\20150409_mea_pic\\2DJ_2015Q1_MEA''')
    pass