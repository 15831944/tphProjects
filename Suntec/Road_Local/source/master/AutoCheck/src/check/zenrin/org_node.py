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
        sqlcmd = '''
                select count(1) 
                from org_node_connect a join 
                org_node_connect b 
                on a.meshcode_adj=b.meshcode_self and a.nodeid_adj=b.nodeid_self
                where not (a.meshcode_self=b.meshcode_adj and a.nodeid_self=b.nodeid_adj)
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

class CCheckCrosskindNodeConnect(platform.TestCase.CTestCase):
    
    def _do(self):
        
        sqlcmd='''
                select count(1) from
                (
                    select distinct meshcode,nodeno from org_node
                    where crosskind=3
                ) a
                full outer join org_node_connect b
                on a.meshcode::int=b.meshcode_self and a.nodeno=b.nodeid_self
                where a.meshcode is null or b.meshcode_self is null
                '''
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)
    
class CCheckGeom(platform.TestCase.CTestCase):
    
    def _do(self):
        
        sqlcmd='''
            select count(*) from org_node a,org_node b
            where a.meshcode=b.meshcode and a.nodeno=b.nodeno and not st_equals(a.the_geom_4326,b.the_geom_4326)
                '''
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)
    

        