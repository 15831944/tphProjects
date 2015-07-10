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
            
            # inlinkid��outlinkid���ӣ�ok
            if self._GetNodeBetweenLinks(inlinkid, outlinkid) is not None:
                continue
            # inlink��outlink�����ӣ�����passlid�Ƿ���������һ��
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
                
                # �Ȳ�inlink��passlid[0]  
                if self._GetNodeBetweenLinks(inlinkid, linkList[0]) is None:
                    print "error,inlinkid %s and oulinkid %s are not connected." % (inlinkid, outlinkid)
                    continue
                # �ٲ�passlid[-1]��outlink 
                if self._GetNodeBetweenLinks(outlinkid, linkList[-1]) is None:
                    print "error,inlinkid %s and oulinkid %s are not connected." % (inlinkid, outlinkid)
                    continue
                
                # �ٲ�passlidÿ��֮��
                bOk = True
                if len(linkList) >= 2:
                    for i in (0, len(linkList)-2):
                        if self._GetNodeBetweenLinks(linkList[i], linkList[i+1]) is None:
                            bOk = False
                            break
                if bOk is False:
                    print "error,inlinkid %s and oulinkid %s are not connected." % (inlinkid, outlinkid)
                    continue
        
    # ͨ����ѯlink��node����Ϣȷ������link�Ƿ��ཻ
    # ����ཻ���������ӵ㣬���򷵻ؿ�
    # "org_city_nw_gc_polyline"��һ���޴�ı���ѯ�ٶȺ������˴���ѯ�Ż���temp_junction_links��
    # ͨ����ѯ�Ż������С���������ٶȡ�
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
    
    # ������е�patternͼƬ������
    def _CheckIfAllPatternPicsExsit(self, thePatternPicDir):
        totalPicList = [] # ��ȡthePatternPicDir������patternͼƬ
        for curDir,dirNames,fileNames in os.walk(thePatternPicDir):
            if(curDir.find("pattern") > -1): # ��Ϊpatternͼ��arrowͼ������ͬ�����������patternͼ
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
                
    # ������е�arrowͼƬ������
    def _CheckIfAllArrowPicsExsit(self, theArrowPicDir):
        totalPicList = [] # ��ȡtheArrowPicDir������arrowͼƬ
        for curDir,dirNames,fileNames in os.walk(theArrowPicDir):
            if(curDir.find("arrow") > -1): # ��Ϊpatternͼ��arrowͼ������ͬ�����������arrowͼ
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

    # ������е�3D�����ģʽͼ������
    def _CheckIfAll3DPatternPicsExist(self, the3DPicDir):
        totalPicList = [] # ��ȡthe3DPicDir������patternͼƬ
        for curDir,dirNames,fileNames in os.walk(the3DPicDir):
            if(curDir.find("pattern") > -1): # ��Ϊpatternͼ��arrowͼ������ͬ�����������patternͼ
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
    
    # ������е�arrowͼƬ������
    def _CheckIfAll3DArrowPicsExsit(self, the3DPicDir):
        totalPicList = [] # ��ȡthe3DPicDir������arrowͼƬ
        for curDir,dirNames,fileNames in os.walk(the3DPicDir):
            if(curDir.find("arrow") > -1): # ��Ϊpatternͼ��arrowͼ������ͬ�����������arrowͼ
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

    # ȷ�ϸ���nodeid��inlinkid��outlinkid����Ψһ�ҵ�һ��org_br����
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
    
    # ȷ�ϸ���nodeid��inlinkid��outlinkid����Ψһ�ҵ�һ��org_dm����
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
    
    
    
    
    
    