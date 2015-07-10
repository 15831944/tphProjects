# coding:utf-8

import platform.TestCase

class CCheckTable(platform.TestCase.CTestCase):
    def _do(self):
        return self.pg.IsExistTable('org_r')

class CCheckTableCount(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd='''
                select count(1) from org_r
               '''
        return self.pg.getOnlyQueryResult(sqlcmd)>0

class CCheckIDDuplication(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*) from (
                    select id from org_r
                    group by id having count(*) > 1
                ) a;
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)
    
class CCheckLinkCircle(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
            select count(*) from org_r 
            where st_equals(st_startpoint(the_geom),st_endpoint(the_geom))
                 '''
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0) 
    
class CCheckZLink(platform.TestCase.CTestCase):
    def _do(self):
        self.pg.CreateFunction_ByName('test_link_shape_turn_number')
        sqlcmd='''
            select count(*) from org_r
            where test_link_shape_turn_number(the_geom)>0
               '''
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0) 
        
class CCheckFKYValid_jnctid(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
            select count(*) from org_r a
            left join org_n b
            on a.snodeid = b.id
            left join org_n c
            on a.enodeid = c.id    
            where b.id is null or c.id is null
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)
    
class CCheckOverlay(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
            select count(*) from org_r a
            inner join org_r b
            on ((a.snodeid  = b.enodeid and a.enodeid = b.snodeid)
            or (a.snodeid = b.snodeid and a.enodeid = b.enodeid))
            and a.id <> b.id and st_equals(a.the_geom,b.the_geom)
                ;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)
    
class CCheckCircle(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
            select count(*) from org_r a
            inner join org_r b
            on (a.snodeid  = b.enodeid and a.enodeid = b.snodeid)
            or (a.snodeid = b.snodeid and a.enodeid = b.enodeid)
            and a.id <> b.id 
                ;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)
    
class CCheckKindnum(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
            select * from org_r where kind_num::int<>length(kind)-length(replace(kind,'|',''))+1
                 '''
        return self.pg.getOnlyQueryResult(sqlcmd)==0

class CCheckKind(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = r'''
            select count(1) from 
            (select unnest(regexp_split_to_Array(kind,E'\\|+')) as kind from org_r ) a
            where substr(a.kind,1,2) not in ('00','01','02','03','04','06','08','09','0a','0b') 
            or substr(a.kind,3,2) not in ('00','01','02','03','04','05','06','07','08','09','0a',
            '0b','0c','0d','0e','0f','11','12','13','14','15','16','17','18','19','1a')
                 '''
        return self.pg.getOnlyQueryResult(sqlcmd)==0

class CCheckWidth(platform.TestCase.CTestCase):
    def _do(self):  
        sqlcmd = '''  
            select count(1) from org_r where width not in ('15','30','55','130')
                '''
        return self.pg.getOnlyQueryResult(sqlcmd)==0

class CCheckDirection(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
            select count(1) from org_r where direction not in ('0','1','2','3')
                 '''
        return self.pg.getOnlyQueryResult(sqlcmd)==0

class CCheckToll(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
            select count(1) from org_r where toll not in ('0','1','2','3')
                '''
        return self.pg.getOnlyQueryResult(sqlcmd)==0
    
class CCheckConst_st(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd= '''
            select count(1) from org_r where const_st not in ('1','4')
                '''  
        return self.pg.getOnlyQueryResult(sqlcmd)==0

class CCheckUndconcrid(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
            select count(1) from org_r a 
            left join org_cr b 
            on a.undconcrid=b.crid 
            where a.undconcrid<>'' and ( a.const_st<>'4' or b.crid is null)
                 '''
        return self.pg.getOnlyQueryResult(sqlcmd)==0
    
class CCheckFuncclass(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
            select count(1) from org_r where funcclass not in ('1','2','3','4','5')
                 '''
        return self.pg.getOnlyQueryResult(sqlcmd)==0

class CCheckLength(platform.TestCase.CTestCase):
    
    def _do(self):
        sqlcmd =  '''
            select count(1) from org_r where length::numeric<0
                '''
        return self.pg.getOnlyQueryResult(sqlcmd)==0
    
class CCheckDetailCity(platform.TestCase.CTestCase):
    
    def _do(self):
        sqlcmd = '''
            select count(1) from org_r where detailcity not in ('0','1','2')
                 '''
        return self.pg.getOnlyQueryResult(sqlcmd)==0
    
class CCheckThrough(platform.TestCase.CTestCase):
    
    def _do(self):
        sqlcmd = '''
            select count(1) from org_r where through not in ('0','1')
                  '''
        return self.pg.getOnlyQueryResult(sqlcmd)==0
    
class CCheckUnthrucrid(platform.TestCase.CTestCase):
    
    def _do(self):
        sqlcmd = '''
            select count(1) from org_r a 
            left join org_cr b 
            on a.unthrucrid=b.crid 
            where a.unthrucrid<>'' and ( a.through<>'0' or b.crid is null)
                 '''
        return self.pg.getOnlyQueryResult(sqlcmd)==0
    
class CCheckOwnership(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd ='''
            select count(1) from org_r where ownership not in ('0','1')
                '''
        return self.pg.getOnlyQueryResult(sqlcmd)==0
    
class CCheckRoad_cond(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
            select count(1) from org_r where road_cond not in ('0','1')
                 '''
        return self.pg.getOnlyQueryResult(sqlcmd)==0
    
class CCheckSpecial(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
            select count(1) from org_r where special not in ('0','1')
                 '''
        return self.pg.getOnlyQueryResult(sqlcmd)==0
    
class CCheckAdminCode(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
            select count(1) from org_r a 
            left join org_admin b on a.admincodel=b.admincode
            left join org_admin c on a.admincoder=c.admincode
            where b.admincode is null or c.admincode is null
                 '''
        return self.pg.getOnlyQueryResult(sqlcmd)==0
    
class CCheckUflag(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
            select count(1) from org_r where uflag not in ('0','1')
                 '''
        return self.pg.getOnlyQueryResult(sqlcmd)==0
    
class CCheckOnewaycrid(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
            select count(1) from org_r a 
            left join org_cr b on a.onewaycrid=b.crid
            where a.onewaycrid<>'' and b.crid is null
                 '''
        return self.pg.getOnlyQueryResult(sqlcmd)==0

class CCheckAccesscrid(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
            select count(1) from org_r a 
            left join org_cr b on a.accesscrid=b.crid
            where a.accesscrid<>'' and b.crid is null
                 '''
        return self.pg.getOnlyQueryResult(sqlcmd)==0
    
class CCheckSpeedclass(platform.TestCase.CTestCase):
    def _do(self):    
        sqlcmd = '''
            select count(1) from org_r where speedclass not in ('1','2','3','4','5','6','7','8')
                '''
        return self.pg.getOnlyQueryResult(sqlcmd)==0
    
class CCheckLanenums2ee2s(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
            select count(1) from org_r where lanenums2e<>'' and lanenums2e::int<1 or lanenume2s<>'' and lanenume2s::int<1
                 '''
        return self.pg.getOnlyQueryResult(sqlcmd)==0
    
class CCheckLanenum(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
            select count(1) from org_r 
            where lanenum<>case when (case when lanenums2e<>'' then lanenums2e::int else 0 end)>3 or 
                                        (case when lanenume2s<>'' then lanenume2s::int else 0 end)>3 then '3'
                                when ((lanenums2e='1' or lanenume2s='1') and lanenums2e in ('','1') and lanenume2s in ('','1') ) then '1'
                                else '2' end
                 '''
        return self.pg.getOnlyQueryResult(sqlcmd)==0
    
class CCheckVehcl_type(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
            select count(1) from org_r where replace(vehcl_type,'1','0')<>repeat('0',32)
                '''
        return self.pg.getOnlyQueryResult(sqlcmd)==0
    
class CCheckElevated(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
            select count(1) from org_r where elevated not in ('0','1')
                 '''
        return self.pg.getOnlyQueryResult(sqlcmd)==0
    
class CCheckStructure(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
            select count(1) from org_r where structure not in ('0','1','2')
                 '''
        return self.pg.getOnlyQueryResult(sqlcmd)==0
    
class CCheckUsefeetype(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
            select count(1) from org_r where usefeetype not in ('1','2','3','4','5','6') and usefeecrid<>''
                                            or usefeetype<>'' and usefeecrid=''
                 '''
        return self.pg.getOnlyQueryResult(sqlcmd)==0
    
class CCheckSpdsrcs2e(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
            select count(1) from org_r where spdsrcs2e not in ('1','2','3') and spdlmts2e<>''
                                            or spdsrcs2e<>'' and spdlmts2e=''
                 '''
        return self.pg.getOnlyQueryResult(sqlcmd)==0
    
class CCheckSpdsrce2s(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
            select count(1) from org_r where spdsrce2s not in ('1','2','3') and spdlmte2s<>''
                                            or spdsrce2s<>'' and spdlmte2s=''
                 '''
        return self.pg.getOnlyQueryResult(sqlcmd)==0
    
class CCheckDc_type(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
            select count(1) from org_r where dc_type not in ('0','1','2')
                 '''
        return self.pg.getOnlyQueryResult(sqlcmd)==0

class CCheckUsefeecrid(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
            select count(1) from org_r a left join org_cr b on a.usefeecrid=b.crid 
            where a.usefeecrid<>'' and b.crid is null
                 '''
        return self.pg.getOnlyQueryResult(sqlcmd)==0
    
class CCheckNopasscrid(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
            select count(1) from org_r a left join org_cr b on a.nopasscrid=b.crid 
            where a.nopasscrid<>'' and b.crid is null
                 '''
        return self.pg.getOnlyQueryResult(sqlcmd)==0
    
class CCheckSpdlmt(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
            select count(1) from org_r where (spdlmts2e<>'' and (spdlmts2e::int<1 or spdlmts2e::int>1800)) or
                                            (spdlmte2s<>'' and (spdlmte2s::int<1 or spdlmte2s::int>1800))
                 '''
        return self.pg.getOnlyQueryResult(sqlcmd)==0