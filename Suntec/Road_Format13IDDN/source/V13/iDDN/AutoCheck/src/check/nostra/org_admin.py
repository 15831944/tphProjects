# -*- coding: UTF8 -*-
'''
Created on 2011-12-22

@author: liuxinxing
'''

import platform.TestCase

class CCheckAdminZoneNumber(platform.TestCase.CTestCase):
    def _do(self):
        reg_count = self.pg.getOnlyQueryResult("select count(*) from org_admin_poly")
        return (reg_count > 0)

class CCheckPolytype(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_admin_poly
                where polytype not in (1,2,3,4)
                ;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)

class CCheckProvAmpTam(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_admin_poly
                where  prov_code is null or prov_code = ''
                    or amp_code is null or amp_code = ''
                    or tam_code is null or tam_code = ''
                    or prov_namt is null or prov_namt = ''
                    or prov_name is null or prov_name = ''
                    or amp_namt is null or amp_namt = ''
                    or amp_name is null or amp_name = ''
                    or tam_namt is null or tam_namt = ''
                    or tam_name is null or tam_name = ''
                    or proamp is null or proamp = ''
                    or tamamppro is null or tamamppro = ''
                    or postcode is null or postcode = ''
                ;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)

class CCheckGeom(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_admin_poly
                where the_geom is null
                ;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)

class CCheckProvAllExists(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(distinct prov_code)
                from org_admin_poly
                ;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 77)

class CCheckPolygonArea(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select sum(area)
                from
                (
                    select st_area(the_geom, true) as area
                    from org_admin_poly
                )as t
                ;
                """
        area = self.pg.getOnlyQueryResult(sqlcmd)
        return (area > 510000*1000000 and area < 520000*1000000)


