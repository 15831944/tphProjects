# -*- coding: UTF8 -*-
'''
Created on 2011-12-22

@author: liuxinxing
'''

import platform.TestCase

class CCheckAdminOrder1Count(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(distinct(b.name1, b.name2))
                FROM org_p_area_administration as a
                left join org_attribute_name as b
                on a.meshcode = b.meshcode and a.attrnmno = b.attrnmno
                where elcode in ('550000', '570000')
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 22)

class CCheckAdminOrder8Count(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(distinct(b.name1, b.name2))
                FROM org_p_area_administration as a
                left join org_attribute_name as b
                on a.meshcode = b.meshcode and a.attrnmno = b.attrnmno
                where elcode in ('560000', '580000')
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 368)

class CCheckAdminGeometry(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                FROM org_p_area_administration
                where   elcode in ('550000', '560000', '570000', '580000')
                        and
                        (
                            the_geom is null 
                            or 
                            st_isempty(the_geom)
                            or
                            not st_isvalid(the_geom)
                        )
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckAdminArea(platform.TestCase.CTestCase):
    def _do(self):
        twn_area = 36192 * 1000000
        sqlcmd = """
                select sum(st_area(the_geom_4326, false))
                FROM org_p_area_administration
                where elcode in ('560000', '580000')
                """
        sum_area = self.pg.getOnlyQueryResult(sqlcmd)
        diff_per = abs(sum_area - twn_area) / twn_area
        return (diff_per <= 0.02)

class CCheckAdminName(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                FROM org_p_area_administration as a
                left join org_attribute_name as b
                on a.meshcode = b.meshcode and a.attrnmno = b.attrnmno
                where (elcode in ('550000', '570000') and (b.name1 is null))
                      or
                      (elcode in ('560000', '580000') and (b.name1 is null or b.name2 is null))
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

    