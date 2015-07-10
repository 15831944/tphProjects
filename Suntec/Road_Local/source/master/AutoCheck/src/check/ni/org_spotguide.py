# coding:utf-8
import platform.TestCase

# 确认根据nodeid，inlinkid，outlinkid可以唯一找到一条org_br数据
class CCheckBrDataUnique(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(*) from 
                (select count(*) from org_br group by patternno, arrowno, nodeid) as a
                '''
        rec_count1 = self.pg.getOnlyQueryResult(sqlcmd)
        
        sqlcmd = '''select count(*) from org_br'''
        rec_count2 = self.pg.getOnlyQueryResult(sqlcmd)

        if(rec_count1 != rec_count2):
            print False
        return True
    
# 确认org_br表里的每组inlinkid，outlinkid都相连
class CCheckAllInlinkOutlinkPasslidConnected(platform.TestCase.CTestCase):
    
    # 确认每组inlinkid，outlinkid都相连
    def _do(self):
        rows = self.pg.get_batch_data('''select nodeid, inlinkid, outlinkid, passlid, passlid2 from org_br''')
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
                
                # inlink和outlink不相连且passlid为空，返错
                if len(linkList) == 0:
                    return False
                
                # 先测inlink与passlid[0]  
                if self._GetNodeBetweenLinks(inlinkid, linkList[0]) is None:
                    return False
                # 再测passlid[-1]与outlink 
                if self._GetNodeBetweenLinks(outlinkid, linkList[-1]) is None:
                    return False
                
                # 再测passlid每项之间
                bOk = True
                if len(linkList) >= 2:
                    for i in (0, len(linkList)-2):
                        if self._GetNodeBetweenLinks(linkList[i], linkList[i+1]) is None:
                            bOk = False
                            break
                if bOk is False:
                    return False

        return True
    
    # 通过查询link与node的信息确定两条link是否相交，用到了link_tbl表
    # 如果相交，返回连接点，否则返回空
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
    
        if inres_row is None or outres_row is None:
            return None
        elif inres_row[0] in (outres_row[0], outres_row[1]):
            return inres_row[0]
        elif inres_row[1] in (outres_row[0], outres_row[1]):
            return inres_row[1]
        else:
            return None
        
# 确认根据nodeid，inlinkid，outlinkid可以唯一找到一条org_dm数据
class CCheckDmDataUnique(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(*) from 
                (select count(*) from org_dm group by patternno, arrowno, nodeid) as a
                '''
        rec_count1 = self.pg.getOnlyQueryResult(sqlcmd)
        
        sqlcmd = '''select count(*) from org_dm'''
        rec_count2 = self.pg.getOnlyQueryResult(sqlcmd)

        if(rec_count1 != rec_count2):
            print False
        return True


















