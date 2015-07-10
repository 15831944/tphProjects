# coding:utf-8

import platform.TestCase

class CCheckTable(platform.TestCase.CTestCase):
    def _do(self):
        return self.pg.IsExistTable('org_node')

class CCheckTableCount(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd='''
                select count(1) from org_node
               '''
        return self.pg.getOnlyQueryResult(sqlcmd)>0

class CCheckOverlay(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*) from (
                    select x,y from org_node
                    where crosskind<>3
                    group by x,y having count(distinct(meshcode,nodeno)) > 1
                ) a;
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)
    
    
class CCheckNodeid_adj(platform.TestCase.CTestCase):
    def _do(self):
        #确定a,b为形点相同的图框点时：为一对一的关系，不存在多个图框点重复
        sqlcmd = '''
                select count(1) 
                from org_node_connect a join 
                org_node_connect b 
                on a.meshcode_adj=b.meshcode_self and a.nodeif_adj=b.nodeid_self
                where not (a.meshcode_self=b.meshcode_adj and a.nodeid_self=b.nodeif_adj)
                 '''
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)


class CCheckMesh_rel(platform.TestCase.CTestCase):
    
    def _do(self):
        #确定不会存在有点同时存在于4个图框之中
        sqlcmd='''
                select count(1)
                from org_node_connect
                where mesh_rel not in (2,4,6,8)
                '''
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)
    
class CCheckBifurcationNode(platform.TestCase.CTestCase):
    
    def _do(self):
        
        sqlcmd='''
                select count(*) from org_bifurcation a
                left join org_node b
                on a.meshcode=b.meshcode and a.tnodeno=b.nodeno
                where b.meshcode is null
                '''
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)
    
class CCheckCrossSymbolNode(platform.TestCase.CTestCase):
    
    def _do(self):
        
        sqlcmd='''
                select count(*) from org_cross_symbol a
                left join org_node b
                on a.meshcode=b.meshcode and a.nodeno=b.nodeno
                where b.meshcode is null
                '''
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)
