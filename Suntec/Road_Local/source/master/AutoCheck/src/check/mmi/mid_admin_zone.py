# -*- coding: UTF8 -*-
'''
Created on 2015-5-7

@author: zhaojie
'''

import platform.TestCase

class CCheckadminValues(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(*)
                 from mid_admin_zone
                 where ad_order not in(0,1,8)
                '''
        reg_number_count = self.pg.getOnlyQueryResult(sqlcmd)      

        return (reg_number_count == 0)
    
class CCheckadminorder0Number(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                 select count(*)
                 from mid_admin_zone
                 where ad_order = 0
                '''
        reg_number_count = self.pg.getOnlyQueryResult(sqlcmd)      

        return (reg_number_count > 0)
    
class CCheckadminorder1Number(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                 select count(*)
                 from mid_admin_zone
                 where ad_order = 1
                '''
        reg_number_count = self.pg.getOnlyQueryResult(sqlcmd)      

        return (reg_number_count > 0)
    
class CCheckadminorder8Number(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                 select count(*)
                 from mid_admin_zone
                 where ad_order = 8
                '''
        reg_number_count = self.pg.getOnlyQueryResult(sqlcmd)      

        return (reg_number_count > 0)
    
class CCheckadminorder8_parent_idLogic(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                 select count(*)
                 from
                 (
                     select a.ad_code as order8,b.ad_code as order1
                     from 
                     (
                         select ad_code,order1_id
                         from mid_admin_zone
                         where ad_order = 8
                     )as a
                     left join
                     (
                         select ad_code
                         from mid_admin_zone
                         where ad_order = 1
                     )as b
                     on a.order1_id = b.ad_code
                 ) as c
                 where c.order1 is null;
                '''
        reg_number_count = self.pg.getOnlyQueryResult(sqlcmd)      

        return (reg_number_count == 0)
    
class CCheckadminorder1_parent_idLogic(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                 select count(*)
                 from
                 (
                     select a.ad_code as order1,b.ad_code as order0
                     from 
                     (
                         select ad_code,order0_id
                         from mid_admin_zone
                         where ad_order = 1
                     )as a
                     left join
                     (
                         select ad_code
                         from mid_admin_zone
                         where ad_order = 0
                     )as b
                     on a.order0_id = b.ad_code
                 ) as c
                 where c.order0 is null;
                '''
        reg_number_count = self.pg.getOnlyQueryResult(sqlcmd)      

        return (reg_number_count == 0)
    
class CCheckadminorder8_parent_geomLogic(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(*)
                 from
                 (
                     select a.ad_code as order1,a.the_geom as order1_geom,b.the_geom as order8_geom
                     from 
                     (
                         select ad_code,the_geom
                         from mid_admin_zone
                         where ad_order = 1
                     )as a
                     left join
                     (
                         select order1_id,ST_Multi(st_union(the_geom)) as the_geom
                         from mid_admin_zone
                         where ad_order = 8
                         group by order1_id
                     )as b
                     on a.ad_code = b.order1_id
                 ) as temp_all
                 where ST_Equals(order8_geom,order1_geom) = false;
                '''
        reg_number_count = self.pg.getOnlyQueryResult(sqlcmd)      

        return (reg_number_count == 0)
    
class CCheckadminorder1_parent_geomLogic(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                 select count(*)
                 from
                 (
                     select a.ad_code as order0,a.the_geom as order0_geom,b.the_geom as order1_geom
                     from 
                     (
                         select ad_code,the_geom
                         from mid_admin_zone
                         where ad_order = 0
                     )as a
                     left join
                     (
                         select order0_id,ST_Multi(st_union(the_geom)) as the_geom
                         from mid_admin_zone
                         where ad_order = 1
                         group by order0_id
                     )as b
                     on a.ad_code = b.order0_id
                 ) as temp_all
                 where ST_Equals(order0_geom,order1_geom) = false;
                '''
        reg_number_count = self.pg.getOnlyQueryResult(sqlcmd)      

        return (reg_number_count == 0)
    