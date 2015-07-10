# -*- coding: cp936 -*-
'''
Created on 2015-5-6
@author: 
'''
import os
import psycopg2

class theChecker(object):
    def __init__(self):
        self.conn = psycopg2.connect('''host='172.26.179.190'
                        dbname='C_NIMIF_Sample_TMP'
                        user='postgres' password='pset123456' ''')
        self.pg = self.conn.cursor()            
    
    def _CheckAllInlinkOutlinkPasslidConnected(self):
        self.pg.execute('''select nodeid, inlinkid, outlinkid, passlid, passlid2 from org_br''')
        rows = self.pg.fetchall()
        for row in rows:
            #nodeid = row[0]
            inlinkid = row[1]
            outlinkid = row[2]
            passlid = row[3]
            passlid2 = row[4]
            
            # inlinkid和outlinkid连接，ok
            if self._GetNodeBetweenLinks(inlinkid, outlinkid) is not None:
                continue
            # inlink和outlink不连接，尝试passlid是否将它们连到一起
            else:
                linkList = []
                if len(passlid) > 0:
                    passlidSplit = passlid.split('|')
                    for passlidStr in passlidSplit:
                        linkList.append(int(passlidStr))
                if len(passlid2) > 0:
                    passlidSplit = passlid2.split('|')
                    for passlidStr in passlidSplit:
                        linkList.append(int(passlidStr))
                
                # 先测inlink与passlid[0]  
                if self._GetNodeBetweenLinks(inlinkid, linkList[0]) is None:
                    print "error,inlinkid %s and oulinkid %s are not connected." % (inlinkid, outlinkid)
                    continue
                # 再测passlid[-1]与outlink 
                if self._GetNodeBetweenLinks(outlinkid, linkList[-1]) is None:
                    print "error,inlinkid %s and oulinkid %s are not connected." % (inlinkid, outlinkid)
                    continue
                
                # 再测passlid每项之间
                bOk = True
                if len(linkList) >= 2:
                    for i in (0, len(linkList)-2):
                        if self._GetNodeBetweenLinks(linkList[i], linkList[i+1]) is None:
                            bOk = False
                            break
                if bOk is False:
                    print "error,inlinkid %s and oulinkid %s are not connected." % (inlinkid, outlinkid)
                    continue
        
    # 通过查询link与node的信息确定两条link是否相交
    # 如果相交，返回连接点，否则返回空
    # "org_city_nw_gc_polyline"是一个巨大的表，查询速度很慢，此处查询优化表temp_junction_links。
    # 通过查询优化过后的小表可以提高速度。
    def _GetNodeBetweenLinks(self, linkid1, linkid2):
        node_sqlcmd = '''
                        SELECT s_node, e_node, one_way_code
                        FROM link_tbl
                        where link_id = %s;
                    '''
        self.pg.execute(node_sqlcmd % (linkid1,))
        inres_row = self.pg.fetchone()
        self.pg.execute(node_sqlcmd % (linkid2,))
        outres_row = self.pg.fetchone()
    
        if inres_row[0] in (outres_row[0], outres_row[1]):
            return inres_row[0]
        elif inres_row[1] in (outres_row[0], outres_row[1]):
            return inres_row[1]
        else:
            return None
    
    # 检查所有的pattern图片都存在
    def _CheckIfAllPatternPicsExsit(self, thePatternPicDir):
        totalPicList = [] # 获取thePatternPicDir下所有pattern图片
        for curDir,dirNames,fileNames in os.walk(thePatternPicDir):
            if(curDir.find("pattern") > -1): # 因为pattern图与arrow图名字相同，这里仅保留pattern图
                for oneFile in fileNames:
                    totalPicList.append(os.path.join(curDir, oneFile))
                    
        self.pg.execute('''select nodeid, inlinkid, outlinkid, patternno from org_br''')
        rows = self.pg.fetchall()
        for row in rows:
            nodeid = row[0]
            inlinkid = row[1]
            outlinkid = row[2]
            patternno = row[3]
            bFound = False
            for onePic in totalPicList:
                if(onePic.find(patternno) > -1):
                    bFound = True
                    break
            if(bFound == False):
                print '''error, pattern picture cannot be found, nodeid %s, inlinkid %s, 
                outlinkid %s, patternno %s''' % (nodeid, inlinkid, outlinkid, patternno)
                
    # 检查所有的arrow图片都存在
    def _CheckIfAllArrowPicsExsit(self, theArrowPicDir):
        totalPicList = [] # 获取theArrowPicDir下所有arrow图片
        for curDir,dirNames,fileNames in os.walk(theArrowPicDir):
            if(curDir.find("arrow") > -1): # 因为pattern图与arrow图名字相同，这里仅保留arrow图
                for oneFile in fileNames:
                    totalPicList.append(os.path.join(curDir, oneFile))
                    
        self.pg.execute('''select nodeid, inlinkid, outlinkid, arrowno from org_br''')
        rows = self.pg.fetchall()
        for row in rows:
            nodeid = row[0]
            inlinkid = row[1]
            outlinkid = row[2]
            arrowno = row[3]
            bFound = False
            for onePic in totalPicList:
                if(onePic.find(arrowno) > -1):
                    bFound = True
                    break
            if(bFound == False):
                print '''error, arrow picture cannot be found, nodeid %s, inlinkid %s, 
                outlinkid %s, arrowno %s''' % (nodeid, inlinkid, outlinkid, arrowno)

    # 检查所有的3D交叉点模式图都存在
    def _CheckIfAll3DPatternPicsExist(self, the3DPicDir):
        totalPicList = [] # 获取the3DPicDir下所有pattern图片
        for curDir,dirNames,fileNames in os.walk(the3DPicDir):
            if(curDir.find("pattern") > -1): # 因为pattern图与arrow图名字相同，这里仅保留pattern图
                for oneFile in fileNames:
                    totalPicList.append(os.path.join(curDir, oneFile))
                    
        self.pg.execute('''select nodeid, inlinkid, outlinkid, patternno from org_dm''')
        rows = self.pg.fetchall()
        for row in rows:
            nodeid = row[0]
            inlinkid = row[1]
            outlinkid = row[2]
            patternno = row[3]
            bFound = False
            for onePic in totalPicList:
                if(onePic.find(patternno) > -1):
                    bFound = True
                    break
            if(bFound == False):
                print '''error, 3D pattern picture cannot be found, nodeid %s, inlinkid %s, 
                outlinkid %s, patternno %s''' % (nodeid, inlinkid, outlinkid, patternno)
    
    # 检查所有的arrow图片都存在
    def _CheckIfAll3DArrowPicsExsit(self, the3DPicDir):
        totalPicList = [] # 获取the3DPicDir下所有arrow图片
        for curDir,dirNames,fileNames in os.walk(the3DPicDir):
            if(curDir.find("arrow") > -1): # 因为pattern图与arrow图名字相同，这里仅保留arrow图
                for oneFile in fileNames:
                    totalPicList.append(os.path.join(curDir, oneFile))
                    
        self.pg.execute('''select nodeid, inlinkid, outlinkid, arrowno from org_br''')
        rows = self.pg.fetchall()
        for row in rows:
            nodeid = row[0]
            inlinkid = row[1]
            outlinkid = row[2]
            arrowno = row[3]
            bFound = False
            for onePic in totalPicList:
                if(onePic.find(arrowno) > -1):
                    bFound = True
                    break
            if(bFound == False):
                print '''error, 3D arrow picture cannot be found, nodeid %s, inlinkid %s, 
                outlinkid %s, arrowno %s''' % (nodeid, inlinkid, outlinkid, arrowno)

    # 确认根据nodeid，inlinkid，outlinkid可以唯一找到一条org_br数据
    def _CheckBrDataUnique(self):
        self.pg.execute('''
                        select count(*) from 
                        (select count(*) from org_br group by patternno, arrowno, nodeid) as a
                        ''')
        dataCount1 = self.pg.fetchone()[0]

        self.pg.execute('''select count(*) from org_br''')
        dataCount2 = self.pg.fetchone()[0]
        if(dataCount1 != dataCount2):
            print '''error, the org_br table'''
    
    # 确认根据nodeid，inlinkid，outlinkid可以唯一找到一条org_dm数据
    def _CheckDmDataUnique(self):
        self.pg.execute('''
                        select count(*) from 
                        (select count(*) from org_dm group by patternno, arrowno, nodeid) as a
                        ''')
        dataCount1 = self.pg.fetchone()[0]

        self.pg.execute('''select count(*) from org_dm''')
        dataCount2 = self.pg.fetchone()[0]
        if(dataCount1 != dataCount2):
            print '''error, the org_dm table'''
            
if __name__ == '__main__':
    aChecker = theChecker()
    aChecker._CheckAllInlinkOutlinkPasslidConnected()
    aChecker._CheckBrDataUnique()
    aChecker._CheckDmDataUnique()
    
    
    
    
    
    