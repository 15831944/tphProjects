# -*- coding: UTF8 -*-
'''
Created on 2013-12-2

@author: yuanrui
'''

import platform.TestCase

class CCheckObjectIDDuplication(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*) from (
                    select objectid from road_link 
                    group by objectid having count(*) > 1
                ) a;
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckFieldValid_roadclass(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from road_link
                where roadclass_c not in 
                (1,2,3,4,5,6,7,8,9,11,101,102,103,104,105,106,107,108,201,202,203,204,302,303,304,305,402)
                ;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)

class CCheckFieldValid_naviclass(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from road_link
                where naviclass_c not in 
                (0,1,2,3,4,5,6,101,102,103,104,105,106,107,108,201,403)
                ;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)

class CCheckFieldValid_linkclass(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from road_link
                where linkclass_c not in 
                (0,1,2,3,4,5,6,7,9)
                ;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)

class CCheckFieldValid_width(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from road_link
                where roadwidth_c not in 
                (0,1,2,3,4)
                ;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)

class CCheckFieldValid_smartic(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from road_link
                where smartic_c not in 
                (0,1,2)
                ;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)

class CCheckFieldValid_nopassage(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from road_link
                where nopassage_c not in 
                (0,1,2,3,4,5)
                ;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)
                    
class CCheckFieldValid_oneway(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from road_link
                where oneway_c not in 
                (0,1,2,3,4,5)
                ;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)

class CCheckFieldValid_lane(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from road_link
                where lane_count < 0 or lane_count > 15
                ;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)

class CCheckFieldValid_roadno(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from road_link
                where road_no < 0 or road_no > 9999
                ;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)

class CCheckFieldValid_linkattr(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from segment_attr
                where linkattr_c not in (1,2,3,4,5)
                ;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)
                
class CCheckRoadcode(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
            select count(*) 
            from ( 
                select distinct road_code from road_link
            ) a
            left join road_code_list b
            on (a.road_code = b.road_code or a.road_code - 200000 = b.road_code 
            or a.road_code - 200000 = b.road_code or a.road_code - 400000 = b.road_code ) 
            where a.road_code <> 0 and b.road_code is null
                ;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)

class CCheckWidearea(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
            select count(*)
            from ( 
                select 1 as id,count(*) as count_wide from road_link where widearea_f = 1
            ) a
            left join (
                select 1 as id,count(*) as count_all from road_link
            ) b
            on a.id = b.id
            where a.count_wide * 1.0 / b.count_all > 0.05
                ;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)
    
class CCheckOverlay(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
            select count(*) 
            from (   
                select a.objectid as link1,b.objectid as link2,a.the_geom as geom1,b.the_geom as geom2 
                from (
                    SELECT objectid,the_geom,ST_Length_Spheroid(the_geom,'SPHEROID("WGS_84", 6378137, 298.257223563)') as length
                    from road_link_4326 
                ) a
                left join (
                    SELECT objectid,the_geom,ST_Length_Spheroid(the_geom,'SPHEROID("WGS_84", 6378137, 298.257223563)') as length
                    from road_link_4326
                ) b
                on a.objectid <> b.objectid and a.length = b.length 
                where b.objectid is not null
            ) a where st_equals(geom1,geom2)
                ;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)

class CCheckCircle(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
            select count(*) from road_link_4326 a
            inner join road_link_4326 b
            on (a.from_node_id  = b.to_node_id and a.to_node_id = b.from_node_id)
            or (a.from_node_id = b.from_node_id and a.to_node_id = b.to_node_id)
            and a.objectid <> b.objectid
                ;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)