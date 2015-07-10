# coding:utf-8

import platform.TestCase

class CCheckIDDuplication(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(1) from
                    (
                        select meshcode,linkno from org_road group by meshcode,linkno having count(1)>1
                    )
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)
    
class CCheckLinkCircle(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
            select count(*) from org_road
            where st_equals(st_startpoint(the_geom),st_endpoint(the_geom))
                 '''
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0) 
    
class CCheckZLink(platform.TestCase.CTestCase):
    def _do(self):
        self.pg.CreateFunction_ByName('test_link_shape_turn_number')
        sqlcmd='''
            select count(*) from org_road
            where test_link_shape_turn_number(the_geom)>0
               '''
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0) 
        
class CCheckFKYValid_jnctid(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
            select count(*) from org_road a
            left join org_node b
            on a.snodeno = b.nodeno and a.meshcode=b.meshcode
            left join org_node c
            on a.enodeno = c.nodeno and a.meshcode=c.meshcode    
            where b.meshcode is null or c.meshcode is null
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)
    
class CCheckOverlay(platform.TestCase.CTestCase):
    def _do(self):
        self.pg.CreateIndex_ByName('org_road_meshcode_linkno_idx')
        self.pg.CreateIndex_ByName('org_road_snodeno_enodeno_idx')
        sqlcmd = """
            select count(*) from org_road a
            inner join org_road b
            on a.meshcode=b.meshcode and ((a.snodeno  = b.enodeno and a.enodeno = b.snodeno)
            or (a.snodeno = b.snodeno and a.enodeno = b.enodeno))
            and a.linkno <> b.linkno and st_equals(a.the_geom,b.the_geom);
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)
    
class CCheckCircle(platform.TestCase.CTestCase):
    def _do(self):
        self.pg.CreateIndex_ByName('org_road_meshcode_linkno_idx')
        self.pg.CreateIndex_ByName('org_road_snodeno_enodeno_idx')
        sqlcmd = """
            select count(*) from org_road a
            inner join org_road b
            on a.meshcode=b.meshcode and ((a.snodeno  = b.enodeno and a.enodeno = b.snodeno)
            or (a.snodeno = b.snodeno and a.enodeno = b.enodeno))
            and a.linkno <> b.linkno ;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)
    
