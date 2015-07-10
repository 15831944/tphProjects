# -*- coding: cp936 -*-
'''
Created on 2015-5-6
@author: 
'''
import os
import psycopg2

if __name__ == "__main__":
    conn = psycopg2.connect('''host='172.26.179.190' dbname='C_NIMIF_Sample_TMP' user='postgres' password='pset123456' ''')
    cur = conn.cursor()
    cur.execute(''' DROP TABLE if exists org_signboard;
                    CREATE TABLE org_signboard
                    (
                      gid serial NOT NULL,
                      mapid character varying(8),
                      id character varying(13),
                      nodeid character varying(13),
                      inlinkid character varying(13),
                      outlinkid character varying(13),
                      patternno character varying(11),
                      arrowno character varying(11),
                      passlid character varying(254),
                      passlid2 character varying(254),
                      folder character varying
                    );
                ''')
    conn.commit()

    # 图片已经手动拷贝。。。。。
   
    cur.execute(''' insert into org_signboard (mapid, id, nodeid, 
                           inlinkid, outlinkid, patternno,
                           arrowno, passlid, passlid2, folder)
                    (select mapid, id, nodeid, 
                           inlinkid, outlinkid, patternno,
                           arrowno, passlid, passlid2, folder 
                     from org_br)''')
    conn.commit()
    

        