# -*- coding: UTF8 -*-

import platform.TestCase

class CCheckIDDuplication(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*) from (
                    select id from org_city_nw_gc_polyline 
                    group by id having count(*) > 1
                ) a;
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckFKYValid_jnctid(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
            select count(*) from org_city_nw_gc_polyline a
            left join org_city_jc_point b
            on a.f_jnctid = b.id
            left join org_city_jc_point c
            on a.t_jnctid = c.id    
            where b.id is null or c.id is null
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckFKYValid_bifurcation(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
            select count(*) from org_bifurcation a
            left join org_city_jc_point b
            on a.junctionid = b.id    
            where b.id is null 
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckFKYValid_tollgate(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
            select count(*) from org_toll_booth_point a
            left join org_city_nw_gc_polyline b
            on a.fm_edge = b.id
            left join org_city_nw_gc_polyline c
            on a.to_edge = c.id                
            where b.id is null or c.id is null
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)
            
class CCheckFieldValid_ft(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_city_nw_gc_polyline
                where ft not in (0,1,2)
                ;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)

class CCheckFieldValid_jnctid(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_city_nw_gc_polyline
                where f_jncttyp not in (0,3,4,6) or t_jncttyp not in (0,3,4,6)
                ;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)

class CCheckFieldValid_pj(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_city_nw_gc_polyline
                where pj not in (0,1,2)
                ;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)

class CCheckFieldValid_frc(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_city_nw_gc_polyline
                where frc not in (0,1,2,3,4,5)
                ;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)

class CCheckFieldValid_fow(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_city_nw_gc_polyline
                where fow not in (0,4,10,21,22,23,24,25,26,27,14,15)
                ;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)

class CCheckFieldValid_freeway(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_city_nw_gc_polyline
                where freeway not in (0,1)
                ;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)

class CCheckFieldValid_backrd(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_city_nw_gc_polyline
                where backrd not in (0)
                ;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)

class CCheckFieldValid_tollrd(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_city_nw_gc_polyline
                where tollrd not in ('B','FT')
                ;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)

class CCheckFieldValid_rdcond(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_city_nw_gc_polyline
                where rdcond not in (1)
                ;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)

class CCheckFieldValid_privaterd(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_city_nw_gc_polyline
                where privaterd not in (0,1)
                ;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)
    
class CCheckFieldValid_oneway(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_city_nw_gc_polyline
                where oneway not in ('FT') and oneway is not null
                ;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)
        
class CCheckFieldValid_routable(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_city_nw_gc_polyline
                where routable not in ('0','1')
                ;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)

class CCheckFieldValid_mult_digi(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_city_nw_gc_polyline
                where mult_digi not in (0,1)
                ;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)

class CCheckFieldValid_spd_limit(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_city_nw_gc_polyline
                where spd_limit < 0 or spd_limit > 180
                ;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)
    
class CCheckFieldValid_ftr_cry(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_city_nw_gc_polyline
                where ftr_cry not in ('OHY','NGQ','CGQ','NHY','SHY','DHY','RDA','RDB','RDC','RDD','CTN')
                ;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)

class CCheckFieldValid_toll_rd(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_city_nw_gc_extended_polyline
                where toll_rd not in (0,1)
                ;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)
    
class CCheckFieldValid_div_typ(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_city_nw_gc_extended_polyline
                where div_typ not in (0,1,2)
                ;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)

class CCheckFieldValid_const_st(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_city_nw_gc_extended_polyline
                where const_st not in (0,1)
                ;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)
                    
class CCheckOverlay(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
            select count(*) from org_city_nw_gc_polyline a
            inner join org_city_nw_gc_polyline b
            on ((a.f_jnctid  = b.t_jnctid and a.t_jnctid = b.f_jnctid)
            or (a.f_jnctid = b.f_jnctid and a.t_jnctid = b.t_jnctid))
            and a.id <> b.id and st_equals(a.the_geom,b.the_geom)
                ;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)

class CCheckCircle(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
            select count(*) from org_city_nw_gc_polyline a
            inner join org_city_nw_gc_polyline b
            on (a.f_jnctid  = b.t_jnctid and a.t_jnctid = b.f_jnctid)
            or (a.f_jnctid = b.f_jnctid and a.t_jnctid = b.t_jnctid)
            and a.id <> b.id
                ;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)
  