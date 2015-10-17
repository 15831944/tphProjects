# -*- coding: cp936 -*-
import psycopg2

"""
--************************************************************************************************************************************************************************
-- org_junctionview �ظ��������inlink��geometry
drop table if exists org_junctionview_dumpoutlink_thegeom;
select a.*, b.the_geom 
into org_junctionview_dumpoutlink_thegeom
from
(
    select arc1, arc2, unnest(day_pic_list) as day_pic, 
           unnest(night_pic_list) as night_pic, 
           unnest(arrowimg_list) as arrowimg
    from
    (
        SELECT arc1, arc2, array_agg(day_pic) as day_pic_list, array_agg(night_pic) as night_pic_list, array_agg(arrowimg) as arrowimg_list
        FROM org_junctionview_test
        group by arc1, arc2
    ) as t
    where array_upper(day_pic_list, 1) > 1
) as a
left join org_l_tran as b
on a.arc1 = b.routeid;
select * from org_junctionview_dumpoutlink_thegeom;
"""

MAX_DEEP = 10

class NostraJunctionViewFix(object):
    def __init__(self, dbIP, dbName, userName, password):
        self.conn = psycopg2.connect(''' host='%s' dbname='%s' user='%s' password='%s' ''' % \
                                     (dbIP, dbName, userName, password))
        self.pg = self.conn.cursor()
      
    # org_junctionview����ͬһ��inlink+outlink���Ӧ����patternͼ��������ֻӦ��һ�飩
    # ��ʱ��ͼƬ�ظ��ṩ���£�ɾ������һ����¼��   
    def dealWithDuplicateLinkPairs(self):
        sqlcmd = """
                    drop table if exists org_junctionview_fixed;
                    select * into org_junctionview_fixed from org_junctionview;
                    comment on table org_junctionview_fixed is 'removed duplicated arc1&arc2 pairs';
                    select arc1, arc2, day_pic_list, night_pic_list, arrowimg_list
                    from
                    (
                        SELECT arc1, arc2, 
                               (array_agg(day_pic)) as day_pic_list, 
                               (array_agg(night_pic)) as night_pic_list, 
                               (array_agg(arrowimg)) as arrowimg_list
                        FROM org_junctionview
                        group by arc1, arc2
                    ) as t
                    where array_upper(day_pic_list, 1) > 1
                """
        self.pg.execute(sqlcmd)
        rows = self.pg.fetchall()
        for row in rows:
            sqlcmd = """
                    delete from org_junctionview_fixed 
                    where arc1=%d and arc2=%d and day_pic='%s' and 
                          night_pic='%s' and arrowimg='%s'
                    """ % (row[0], row[1], row[2][0], row[3][0], row[4][0])
            self.pg.execute(sqlcmd)
        self.conn.commit()
        return
    
    # ������С��46�׵�innerlink��ǰ������innerlink��
    # ��Ҫ�ֶ�������Щinlink�������ʹ������߼�
    # ����
    # ��org_l_tran���ж���Ϊ9�ĵ�·���ͽ�������ת��Ϊrdb��innerlink��rdb link type Ϊ4��
    # guide���ڻ�֪��linkΪinnerlinkʱ�����ж��䳤�ȣ���С��46�������ɰ��ڵ㡣
    # �������飬������Щinnerlink���������ĳ��ֱ�з�innerlink�ĺ���link��������ѡ��ֱ��link��Ϊ�Ƶ�link��
    # 
    def dealWithInnerLinks_inlink(self):
        sqlcmd = """
                 select a.arc1,
                        a.arc2, 
                 from
                 org_junctionview as a
                 left join org_l_tran as b
                 on a.arc1=b.routeid
                 where b.fw=9 and length(inlink_geom)>46
                 """
        self.pg.execute(sqlcmd)
        rows = self.pg.fetchall()
        for row in rows:
            inlinkid = row[0]
            outlinkid = row[3]
            
            linkList = []
            errMsg = ['']
            newInlink = None
            outlinkNodes = [outlink_start_node, outlink_end_node]
            if (inlink_start_node not in outlinkNodes) and \
               (inlink_end_node in outlinkNodes):
                newInlink = self.getPrevLink(errMsg, inlinkid, inlink_start_node, passlinkList)
            elif (inlink_start_node in outlinkNodes) and \
                 (inlink_end_node not in outlinkNodes):
                newInlink = self.getPrevLink(errMsg, inlinkid, inlink_end_node, passlinkList)
            else:
                print 'ԭʼ���ݴ���org_junctionview�����arc1��arc2��������case��'
                
            if errMsg[0] != '':
                print '����ǰ�Ƶ�inlink��%sʱ��������errMsg: '% (inlinkid, errMsg[0])
                continue
            
            
                
        return
        
    def getPrevLink(self, errMsg, inlinkid, nodeid, passlinkList):
        if len(passlinkList) > MAX_DEEP:
            errMsg[0] = '''todo: passlink exceed 10, break.'''
            return None
        sqlcmd = """
                select a.routeid, a.start_node_id, a.end_node_id, a.the_geom, b.fw from 
                (
                select routeid, start_node_id, end_node_id, the_geom
                from temp_topo_link 
                where start_node_id=%s or end_node_id=%s
                ) as a
                left join org_l_tran as b
                on a.routeid=b.routeid
                """ % (nodeid, nodeid)
        self.pg.execute(sqlcmd)
        rows = self.pg.fetchall()
        inlink_geom = None
        for row in rows:
            if row[0] == inlinkid:
                inlink_geom = row[3]
                break
        if inlink_geom == None:
            errMsg[0] = '''temp_topo_link����ʹ��ĳ��link��nodeδ���ҵ���Ӧ��link��¼��'''
            return None
        
        theBestRow = None
        for row in rows:
            linkid = row[0]
            start_node_id = row[1]
            end_node_id = row[2]
            the_geom = row[3]
            
            if linkid==inlinkid:
                continue
            
            if (theBestRow==None) or (theBestRow[3].angle == the_geom.angle): # todo
                theBestRow = row
                continue
            
        if theBestRow[4] == 9: # pushed link is still an inner link
            start_node_id = theBestRow[1]
            end_node_id = theBestRow[2]
            if start_node_id == nodeid:
                return self.getPrevLink(theBestRow[0], end_node_id, passlinkList)
            elif end_node_id == nodeid:
                return self.getPrevLink(theBestRow[0], start_node_id, passlinkList)
            else:
                errMsg[0] = '''this error should not occurred.'''
                return None
        
            
if __name__ == '__main__':
    dbIP = '''192.168.10.20'''
    dbNameList = ['tmap_AP_NOSTRA_CI']
    userName = '''postgres'''
    password = '''pset123456'''
    for dbName in dbNameList:
        try:
            theX = NostraJunctionViewFix(dbIP, dbName, userName, password)
            theX.dealWithDuplicateLinkPairs()
        except Exception, ex:
            print '''%s/%s.\nmsg:\t%s\n''' % (dbIP, dbName, ex)

    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    