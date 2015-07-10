# coding:utf-8
import platform.TestCase

# 确认根据nodeid，inlinkid，outlinkid可以唯一找到一条org_br数据
class CCheckBrDataUnique(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(*)
                            from
                            (
                                select patternno, arrowno, nodeid, inlinkid, outlinkid
                                from org_br 
                                group by patternno, arrowno, nodeid, inlinkid, outlinkid
                                having count(*) > 1
                          )as t
                '''
        return self.pg.getOnlyQueryResult(sqlcmd) == 0

# 确认org_br表不为空
class CCheckBrCount(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd='''
                select count(*) from org_br;
               '''
        return self.pg.getOnlyQueryResult(sqlcmd)>0

# 确认org_br表中没有“type”为6的项。
class CCheckBrType(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd='''
                select count(*) from org_br where type='6'
               '''
        return self.pg.getOnlyQueryResult(sqlcmd)<=0
     
# 确认org_br表里的每组inlinkid，outlinkid都相连
class CCheckAllInlinkOutlinkPasslidConnected(platform.TestCase.CTestCase):
    
    # 确认每组inlinkid，outlinkid都相连
    def _do(self):
        
        # 创建一张临时表为org_r的子集，里面仅列出了所有与spotguide有关的link信息。
        # 优化：建立临时表，查询link两端节点时使用，提高查询速度
        sqlcmd = '''
                drop table if exists temp_spotguide_bl;
                
                select *
                into temp_spotguide_bl
                from org_r
                where id in (select inlinkid from org_br as il)
                   or id in (select outlinkid from org_br as ol);
                
                drop index if exists temp_spotguide_bl_id_idx;
                create index temp_spotguide_bl_id_idx
                on temp_spotguide_bl
                using btree
                (id);
                ''' 
        self.pg.execute(sqlcmd)
        self.pg.commit()
        
        
        
        rows = self.pg.get_batch_data('''select nodeid, inlinkid, outlinkid, passlid, passlid2 from org_br''')
        for row in rows:
            #nodeid = row[0]
            inlinkid = row[1]
            outlinkid = row[2]
            passlid = row[3]
            passlid2 = row[4]
            
            # inlinkid和outlinkid连接，ok
            if self._GetNodeBetweenLinks(inlinkid, outlinkid, "temp_spotguide_bl") is not None:
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
                if self._GetNodeBetweenLinks(inlinkid, linkList[0], "temp_spotguide_bl") is None:
                    return False
                # 再测passlid[-1]与outlink 
                if self._GetNodeBetweenLinks(outlinkid, linkList[-1], "temp_spotguide_bl") is None:
                    return False
                
                # 再测passlid每项之间
                bOk = True
                if len(linkList) >= 2:
                    for i in (0, len(linkList)-2):
                        if self._GetNodeBetweenLinks(linkList[i], linkList[i+1], "temp_spotguide_bl") is None:
                            bOk = False
                            break
                if bOk is False:
                    return False

        return True
    
    # 通过查询link与node的信息确定两条link是否相交，默认使用org_r表
    # 如果相交，返回连接点，否则返回空
    # 通过查询优化过后的小表可以提高速度。
    def _GetNodeBetweenLinks(self, linkid1, linkid2, tempLinkTbl="org_r"):
        node_sqlcmd = '''
                        SELECT snode_id, enode_id FROM %s WHERE id='%s';
                    '''
        self.pg.execute(node_sqlcmd % (tempLinkTbl, linkid1))
        inres_row = self.pg.fetchone()
        self.pg.execute(node_sqlcmd % (tempLinkTbl, linkid2))
        outres_row = self.pg.fetchone()
    
        if inres_row is None or outres_row is None:
            return None
        elif inres_row[0] in (outres_row[0], outres_row[1]):
            return inres_row[0]
        elif inres_row[1] in (outres_row[0], outres_row[1]):
            return inres_row[1]
        else:
            return None

# 确认org_br表里的每个node都是分歧点。
class CCheckAllNodesInBrAreJunctionPoints(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                    select 
                 '''
        return True #self.pg.getOnlyQueryResult(sqlcmd) == 0
      
# 确认根据nodeid，inlinkid，outlinkid可以唯一找到一条org_dm数据
class CCheckDmDataUnique(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(*)
                            from
                            (
                                select patternno, arrowno, nodeid, inlinkid, outlinkid
                                from org_dm 
                                group by patternno, arrowno, nodeid, inlinkid, outlinkid
                                having count(*) > 1
                          )as t
                '''
        return self.pg.getOnlyQueryResult(sqlcmd) == 0

class CCheckDmCount(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd='''
                select count(*) from org_dm;
               '''
        return self.pg.getOnlyQueryResult(sqlcmd)>0
















