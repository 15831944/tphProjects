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
            where st_equals(st_startpoint(the_geom_4326),st_endpoint(the_geom_4326))
                 '''
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0) 
    
class CCheckZLink(platform.TestCase.CTestCase):
    def _do(self):
        self.pg.CreateFunction_ByName('test_link_shape_turn_number')
        sqlcmd='''
            select count(*) from org_road
            where test_link_shape_turn_number(the_geom_4326)>0
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
            and a.linkno <> b.linkno and st_equals(a.the_geom_4326,b.the_geom_4326);
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

class CCheckOneway(platform.TestCase.CTestCase):
    def _do(self):
        
        sqlcmd='''
            select count(1) from org_road
            where oneway not in (0,1,2)
                '''
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)
    
class CCheckNetlevel(platform.TestCase.CTestCase):
    def _do(self):
        
        sqlcmd='''
            select count(1) from org_road
            where netlevel not in (0,1,2,3,4,5)
                '''
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)
    
class CCheckProv_exp(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd='''
            select 
            (
                select count(*) from org_prov_exp where substr(meshcode,1,1)<>'Z' or length(meshcode)<>7
            )
            +
            (
                select count(*)-count(distinct(meshcode,linkno)) from org_prov_exp
            )
            +
            (
                select count(*) from org_prov_exp a 
                left join org_road b
                on substr(a.meshcode,2)=b.meshcode and a.linkno=b.linkno
                where b.meshcode is null
            )
                '''
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)
    
class CCheckGeomType(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd='''
            select count(*) from org_road where geometrytype(the_geom)<>'MULTILINESTRING'
                '''
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        if reg_count<>0 :
            return False
        sqlcmd='''
            select count(*) from org_road where st_numgeometries(the_geom)<>1
                '''
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)
    
class CCheckTunnel(platform.TestCase.CTestCase):
    
    def _do(self):
        
        sqlcmd='''
                select 
                (
                    select count(*) from org_tunnelname a left join org_road b
                     on a.meshcode = b.meshcode and a.roadno=b.linkno
                     where a.tcode='1' and substr(b.elcode,5,1)<>'2' or b.meshcode is null
                )
                 +
                (
                    select count(*) from org_underpass a left join org_road b
                     using(meshcode,linkno)
                     where substr(b.elcode,5,1)<>'2' or b.meshcode is null 
                )
                '''
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)
    
class CCheckRoadName(platform.TestCase.CTestCase):
    
    def _do(self):
        
        sqlcmd='''
                select
                (
                    select count(*)-count(distinct(meshcode,linkno)) from org_underpass
                )
                +
                (
                    select count(*)-count(distinct(meshcode,roadno)) from org_tunnelname
                )
                +
                (
                    select count(*)-count(distinct(meshcode,roadno)) from org_rname_bpmf
                )
                +
                (
                    select count(*)-count(distinct(meshcode,roadno)) from org_l5l6name
                )
                '''
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)
    
class CCheckRoadNumber(platform.TestCase.CTestCase):
    
    def _do(self):
        
        sqlcmd='''
                select count(1) from org_prov_exp
                where position ('Ê¡µÀ' in routeno)=0
               '''
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)
    