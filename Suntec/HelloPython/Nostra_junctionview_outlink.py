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

--************************************************************************************************************************************************************************
-- 
update org_junctionview_test set arc2=3200003204891 where arc1=3200001538574 and day_pic='KCB110' and night_pic='KCB110' and arrowimg='KCB110_ARROW1'

"""

class NostraJunctionViewFix(object):
    def __init__(self, dbIP, dbName, userName, password):
        self.conn = psycopg2.connect(''' host='%s' dbname='%s' user='%s' password='%s' ''' % \
                                     (dbIP, dbName, userName, password))
        self.pg = self.conn.cursor()
      
    # org_junctionview����һ��inlink+outlink���Ӧ����patternͼ��������ֻӦ��һ�飩
    # ��ʱ��outlink�����¡�
    # ����qgis�жϵ�·�����Լ�patternͼ���ݣ��ֶ�������Щ�����outlink��   
    def updateOutlinkErrorRecords(self, textList):
        for oneText in textList:
            textSplitList = ','.split(oneText)
            sqlcmd = """
                    update org_junctionview_test set arc2=%s 
                    where arc1=%s and arc2=%s, day_pic='%s' and 
                          night_pic='%s' and arrowimg='%s'
                    """ % (textSplitList[5], textSplitList[0],
                           textSplitList[1], textSplitList[2],
                           textSplitList[3], textSplitList[4])
            self.pg.execute(sqlcmd)
        self.conn.commit()
        return
    
    # org_junctionview���в���inlink��Ϣ��������inner link����ʱ�������޷����ɰ��ڵ㡣
    # ��Ҫ�ֶ�������Щinlink����������ˣ���ʹ������߼�
    def listAllTables(self):
        return
         
if __name__ == '__main__':
    dbIP = '''192.168.10.20'''
    dbNameList = ['AP_NOSTRA_201530_ORG']
    userName = '''postgres'''
    password = '''pset123456'''
    textList = \
    [
    """3200001538574,3200003204891,KCB158,KCB158,KCB158_ARROW1,""",
    """3200001538574,3200003204891,KCB110,KCB110,KCB110_ARROW1,""",
    """3200002291638,3200003991289,MHS028,MHS028,MHS028_ARROW1,""",
    """3200002291638,3200003991289,MHS038,MHS038,MHS038_ARROW1,""",
    """3200002337620,3200002314733,5026,5026,5026_ARROW1,""",
    """3200002337620,3200002314733,CHR030,CHR030,CHR030_ARROW1,"""
    ]
    for dbName in dbNameList:
        try:
            theX = NostraJunctionViewFix(dbIP, dbName, userName, password)
            theX.updateOutlinkErrorRecords(textList)
        except Exception, ex:
            print '''%s/%s.\nmsg:\t%s\n''' % (dbIP, dbName, ex)

    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    