# -*- coding: cp936 -*-
import psycopg2
latlonDelta = 0.00001
def queryRdbSpotguidePoint(pg):
    sqlcmd = '''
            select distinct st_x(b.the_geom), st_y(b.the_geom), a.node_id, a.node_id_t, c.old_node_id
            from
            rdb_guideinfo_spotguidepoint as a
            left join rdb_node as b
            on a.node_id=b.node_id and a.node_id_t=b.node_id_t
            left join rdb_tile_node as c
            on a.node_id=c.tile_node_id and a.node_id_t=c.tile_id
            order by st_x(b.the_geom);
            '''
    pg.execute(sqlcmd)
    return pg.fetchall()

def queryMidSpotguidePoint(pg):
    sqlcmd = '''
            select distinct nodeid 
            from spotguide_tbl
             '''
    pg.execute(sqlcmd)
    return pg.fetchall()

def querySpotguidePicBlob(pg):
    sqlcmd = '''
            select gid, image_id 
            from rdb_guideinfo_pic_blob_bytea
             '''
    pg.execute(sqlcmd)
    return pg.fetchall()

def queryOrgSpotguidePoint(pg):
    sqlcmd = '''
             select dp_node_id, lat, lon
             '''

if __name__ == '__main__':
    dbIP = '''172.26.179.184'''
    dbName = '''MEA8_RDF_2014Q4_0080_0007_test'''
    userName = '''postgres'''
    password = '''pset123456'''
    conn1 = psycopg2.connect(''' host='%s' dbname='%s' user='%s' password='%s' ''' %(dbIP, dbName, userName, password))
    pg1 = conn1.cursor()
    
    dbIP = '''172.26.179.184'''
    dbName = '''MEA8_RDF_2015Q1_0065_0012'''
    userName = '''postgres'''
    password = '''pset123456'''
    conn2 = psycopg2.connect(''' host='%s' dbname='%s' user='%s' password='%s' ''' %(dbIP, dbName, userName, password))
    pg2 = conn2.cursor()
    
    rdbRows14Q4 = queryRdbSpotguidePoint(pg1)
    rdbRwos15Q1 = queryRdbSpotguidePoint(pg2)
    len1 = len(rdbRows14Q4)
    len2 = len(rdbRwos15Q1)
    
    # ȡ���
    rdb_dif_14Q4_to_15Q1 = list(set(rdbRows14Q4).difference(set(rdbRwos15Q1)))
    rdb_dif_15Q1_to_14Q4 = list(set(rdbRwos15Q1).difference(set(rdbRows14Q4)))
    len3 = len(rdb_dif_14Q4_to_15Q1)
    len4 = len(rdb_dif_15Q1_to_14Q4)
    
    # ��Ϊ��γ�Ȳ����latlonDelta������ͬһ���㡣
    # �ڲ���б����ȥ��ͬ�㣬ȡ�þ��Բ�֡�
    rdb_simList = []
    for i in range(len(rdb_dif_14Q4_to_15Q1)-1, 0, -1):
        for j in range(len(rdb_dif_15Q1_to_14Q4)-1, 0, -1):
            if(rdb_dif_14Q4_to_15Q1[i][0]-rdb_dif_15Q1_to_14Q4[j][0] < latlonDelta 
               and rdb_dif_14Q4_to_15Q1[i][1]-rdb_dif_15Q1_to_14Q4[j][1] < latlonDelta):
                rdb_simList.append((rdb_dif_14Q4_to_15Q1[i], rdb_dif_15Q1_to_14Q4[j]))
                del rdb_dif_14Q4_to_15Q1[i]
                del rdb_dif_15Q1_to_14Q4[j]
                break
    len5 = len(rdb_simList)
    len6 = len(rdb_dif_14Q4_to_15Q1)
    len7 = len(rdb_dif_15Q1_to_14Q4)
    
    # 14Q4�д��ڶ�15Q1��û�еĵ㣬ͨ�����ǵ�ԭʼnode_id������15Q1��ԭʼ��
    # rdfpl_all_jv_lat_display_org���ҵ����ǣ�
    nodeidList = []
    for one14Q4 in rdb_dif_14Q4_to_15Q1:
        nodeidList.append(one14Q4[4])
    sqlcmd = ""
        
    # ���ҵ��Ĳ��֣�����ͨ��
    
    # 15Q1�д��ڶ�14Q4��û�еģ�
    oFStream = open("c:\\my\\20150611_spotguidepoint_statistic\\2.csv", "w")
    for one15Q1 in rdb_dif_15Q1_to_14Q4:
        oFStream.write(str(one15Q1[4]) + "\n")
    oFStream.close()
        

        
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    