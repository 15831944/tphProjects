# -*- coding: cp936 -*-
'''
Created on 2015-5-6
@author: 
'''
import psycopg2
from common import common_func
from component.default.guideinfo_spotguide import comp_guideinfo_spotguide

class comp_guideinfo_spotguide_ni(comp_guideinfo_spotguide):
    def __init__(self):
        comp_guideinfo_spotguide.__init__(self)

    def _DoCreateTable(self):
        comp_guideinfo_spotguide._DoCreateTable(self)
        return 0

    def _DoCreatIndex(self):
        return 0

    def _DoCreateFunction(self):
        return 0

    def _Do(self):
        self.log.info("ni generate spotguide_tbl begin.")
        self._xxxxxxFunction1()
        self._xxxxxxFunction2()
        self.log.info("ni generate spotguide_tbl end.")
        return 0
    
    def _XxxxxxFunction1(self):
        spotguide_tbl_insert_sqlcmd = '''
                    insert into spotguide_tbl(nodeid, inlinkid, outlinkid,
                                              passlid, passlink_cnt, direction,
                                              guideattr, namekind, guideclass,
                                              patternno, arrowno, type,
                                              is_exist_sar)
                    values( %s, %s, %s,
                            %s, %s, %s,
                            %s, %s, %s,
                            %s, %s, %s,
                            %s)'''
        
        org_br_query_sqlcmd = '''
                    SELECT nodeid, inlinkid, outlinkid, direction, patternno, 
                           arrowno, guidattr, namekind, passlid, passlid2, "type"
                    FROM org_br
                    where patternno is not null and arrowno is not null and "type" <> '6'
                '''
        rows = self.get_batch_data(org_br_query_sqlcmd)
        for row in rows:        
            nodeid = row['nodeid']
            inlinkid = row['inlinkid']
            outlinkid = row['outlinkid']
            direction = row['direction']
            patternno = row['patternno'] 
            arrowno = row['arrowno']
            guidattr = row['guidattr']
            namekind = row['namekind']
            passlid = row['passlid']
            passlid2 = row['passlid2']
            jv_type = row['type']
            
            totalPasslid = passlid
            if(passlid and passlid2):
                totalPasslid = totalPasslid + '|' + passlid2
            
            totalPasslidCount = common_func.CountPassLink(totalPasslid)
            
            isExistSar = False
            
            self.pg.execute2(spotguide_tbl_insert_sqlcmd, 
                             (nodeid, inlinkid, outlinkid, 
                              totalPasslid, totalPasslidCount, direction,
                              guidattr, namekind, 0,
                              patternno, arrowno, jv_type, isExistSar))
        self.pg.commit2()
        
    def _XxxxxxFunction2(self):
        
        spotguide_tbl_insert_sqlcmd = '''
                    insert into spotguide_tbl(nodeid, inlinkid, outlinkid,
                                              passlid, passlink_cnt, direction,
                                              guideattr, namekind, guideclass,
                                              patternno, arrowno, type,
                                              is_exist_sar)
                    values( %s, %s, %s,
                            %s, %s, %s,
                            %s, %s, %s,
                            %s, %s, %s,
                            %s)'''
        
        org_dm_query_sqlcmd = '''
                    SELECT nodeid, inlinkid, outlinkid, patternno, arrowno, passlid
                    FROM org_dm
                '''
        rows = self.get_batch_data(org_dm_query_sqlcmd)
        for row in rows:        
            nodeid = row['nodeid']
            inlinkid = row['inlinkid']
            outlinkid = row['outlinkid']
            patternno = row['patternno'] 
            arrowno = row['arrowno']
            passlid = row['passlid']
                        
            passlidCount = common_func.CountPassLink(passlid)
            
            isExistSar = False
            
            self.pg.execute2(spotguide_tbl_insert_sqlcmd, 
                             (nodeid, inlinkid, outlinkid, 
                              passlid, passlidCount, 0,
                              0, 0, 0,
                              patternno, arrowno, 7, isExistSar)) # 3D交叉点模式图
        self.pg.commit2()
        return

      
def _CheckAllInlinkOutlinkPasslidConnected():
    conn = psycopg2.connect('''host='172.26.179.190'
                    dbname='C_NIMIF_Sample_TMP'
                    user='postgres' password='pset123456' ''')
    pg = conn.cursor()
    
    sqlcmd = '''
                drop table if exists temp_junction_links;
                select * into temp_junction_links
                from link_tbl
                where link_id in (select distinct cast(inlinkid as bigint) from org_br) 
                or 
                link_id in (select distinct cast(outlinkid as bigint) from org_br);
            '''
    pg.execute(sqlcmd)
    conn.commit()
    
    pg.execute('''select nodeid, inlinkid, outlinkid, passlid from org_br''')
    rows = pg.fetchall()
    for row in rows:
        nodeid = row[0]
        inlinkid = row[1]
        outlinkid = row[2]
        passlid = row[3]
        
        # inlinkid和outlinkid连接，ok
        if inlinkid == "13208498":
            i = 0
            i += 0
        if _GetNodeBetweenLinks(pg, inlinkid, outlinkid) is not None:
            continue
        # inlink和outlink不连接，尝试passlid是否将它们连到一起
        else:
            linkList = []
            passlidSplit = passlid.split('|')
            for passlidStr in passlidSplit:
                linkList.append(int(passlidStr))
            
            # 先测inlink与passlid[0]  
            if _GetNodeBetweenLinks(pg, inlinkid, linkList[0]) is None:
                print "error"
                continue
            # 再测outlink与passlid[-1] 
            if _GetNodeBetweenLinks(pg, outlinkid, linkList[-1]) is None:
                print "error"
                continue
            
            # 再测passlid每项
            bOk = True
            if len(linkList) >= 2:
                for i in (0, len(linkList)-2):
                    if _GetNodeBetweenLinks(pg, linkList[i], linkList[i+1]) is None:
                        bOk = False
                        break
            if bOk is False:
                print "error"
                continue
    
# 通过查询link与node的信息确定两条link是否相交
# 如果相交，返回连接点，否则返回空
# "org_city_nw_gc_polyline"是一个巨大的表，查询速度很慢，此处查询优化表temp_junction_links。
# 通过查询优化过后的小表可以提高速度。
def _GetNodeBetweenLinks(pg, linkid1, linkid2):
    node_sqlcmd = '''
                    SELECT s_node, e_node, one_way_code
                    FROM link_tbl
                    where link_id = %s;
                '''
    pg.execute(node_sqlcmd % (linkid1,))
    inres_row = pg.fetchone()
    pg.execute(node_sqlcmd % (linkid2,))
    outres_row = pg.fetchone()

    if inres_row[0] in (outres_row[0], outres_row[1]):
        return inres_row[0]
    elif inres_row[1] in (outres_row[0], outres_row[1]):
        return inres_row[1]
    else:
        return None
    
if __name__ == '__main__':
    _CheckAllInlinkOutlinkPasslidConnected()
    

            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
        
        
        