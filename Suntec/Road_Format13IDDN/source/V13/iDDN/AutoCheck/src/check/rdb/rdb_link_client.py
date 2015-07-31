# -*- coding: UTF8 -*-
'''
Created on 2011-12-22

@author: liuxinxing
'''

import platform.TestCase
import rdb_common_check

class CCheckLinkCount(platform.TestCase.CTestCase):
    def _do(self):
        
        sqlcmd = """

                    select count(*) as count1 from rdb_link
                    union
                    select count(*) as count2 from rdb_link_client

                """

        if self.pg.execute(sqlcmd) == -1:
            return 0
        else:
            if self.pg.getcnt() == 1:
                return 1
            else:
                return 0

class CCheckLinkID(platform.TestCase.CTestCase):

    def _do(self):

        sqlcmd = """
                      (SELECT count(a.gid)as count 
                          FROM rdb_link as a
                      left join 
                      rdb_link_client as b
                      on a.link_id_t = b.link_id_t and (a.link_id & ((1::bigint << 32) - 1)) = b.link_id) 
                      union
                      select count(*) as count from rdb_link 

                """

        if self.pg.execute(sqlcmd) == -1:
            return 0
        else:
            if self.pg.getcnt() == 1:
                return 1
            else:
                return 0
        
class CCheckLinkTile(platform.TestCase.CTestCase):

    def _do(self):

            sqlcmd = """

                select count(*) from (
                    SELECT a.link_id_t as tile1,b.link_id_t as tile2
                        FROM rdb_link as a
                    left join 
                    rdb_link_client as b
                    on a.link_id_t = b.link_id_t and (a.link_id & ((1::bigint << 32) - 1)) = b.link_id 
                ) as foo 
                where tile2 is null;

                """

            self.pg.execute(sqlcmd)
            row = self.pg.fetchone()
            if row:
                if row[0] == 0:
                    return True
            return False        

class CCheckEndNodeID(platform.TestCase.CTestCase):

    def _do(self):

        sqlcmd = """ 
                      (SELECT count(a.gid)as count 
                          FROM rdb_link as a
                      left join rdb_link_client as b
                      on (a.end_node_id & ((1::bigint << 32) - 1))  = b.end_node_id and (a.link_id & ((1::bigint << 32) - 1)) = b.link_id 
                      and (a.end_node_id >> 32) = b.end_node_id_t and (a.link_id >> 32) = b.link_id_t) 
                    
                     union

                      select count(*) as count from rdb_link 

                """

        if self.pg.execute(sqlcmd) == -1:
            return 0
        else:
            if self.pg.getcnt() == 1:
                return 1
            else:
                return 0
        
class CCheckLinkType(platform.TestCase.CTestCase):

    def _do(self):

            sqlcmd = """
 
                select * from (   
                      SELECT a.link_type as linktype1,b.link_type as linktype2
                          FROM rdb_link as a
                      left join rdb_link_client as b
                      on a.link_id_t = b.link_id_t and (a.link_id & ((1::bigint << 32) - 1)) = b.link_id 
                ) as c 
                where c.linktype1 <> c.linktype2;
                """

            if self.pg.execute(sqlcmd) == -1:
                return 0
            else:
                if self.pg.getcnt() == 0:
                    return 1
                else:
                    return 0 

class CCheckLinkLength(platform.TestCase.CTestCase):

    def _do(self):

            sqlcmd = """
 
                select * from (   
                      SELECT a.link_length as linklength1,b.link_length as linklength2
                          FROM rdb_link as a
                      left join rdb_link_client as b
                      on a.link_id_t = b.link_id_t  and (a.link_id & ((1::bigint << 32) - 1)) = b.link_id 
                ) as c 
                where c.linklength1 <> c.linklength2;
                """

            if self.pg.execute(sqlcmd) == -1:
                return 0
            else:
                if self.pg.getcnt() == 0:
                    return 1
                else:
                    return 0  
    
class CCheckRoadName(platform.TestCase.CTestCase):

    def _do(self):

            sqlcmd = """
 
                select * from (   
                      SELECT a.road_name_id as roadname1,b.road_name_id as roadname2
                          FROM rdb_link as a
                      left join rdb_link_client as b
                      on a.link_id_t = b.link_id_t  and (a.link_id & ((1::bigint << 32) - 1)) = b.link_id  
                ) as c 
                where c.roadname1 <> c.roadname2;
                """

            if self.pg.execute(sqlcmd) == -1:
                return 0
            else:
                if self.pg.getcnt() == 0:
                    return 1
                else:
                    return 0 
        
class CCheckLaneID(platform.TestCase.CTestCase):

    def _do(self):

            sqlcmd = """
 
                select * from (   
                      SELECT a.lane_id as laneid1,b.lane_id as laneid2
                          FROM rdb_link as a
                      left join rdb_link_client as b
                      on a.link_id_t = b.link_id_t  and (a.link_id & ((1::bigint << 32) - 1)) = b.link_id 
                ) as c 
                where c.laneid1 <> c.laneid2;
                """

            if self.pg.execute(sqlcmd) == -1:
                return 0
            else:
                if self.pg.getcnt() == 0:
                    return 1
                else:
                    return 0
                
class CCheckLinkAttr(platform.TestCase.CTestCase):

    def _do(self):

            sqlcmd = """
               
                select count(*) from (
                     SELECT (a.one_way % 4) as one_way,
                         (CASE WHEN a.regulation_flag THEN 1 ELSE 0 END) as regulation_flag,
                         (CASE WHEN a.tilt_flag THEN 1 ELSE 0 END) as tilt_flag,
                         (CASE WHEN a.speed_regulation_flag THEN 1 ELSE 0 END) as speed_regulation_flag,
                         (CASE WHEN a.link_add_info_flag THEN 1 ELSE 0 END) as link_add_info_flag,
                         (CASE WHEN a.shield_flag THEN 1 ELSE 0 END) as shield_flag,
                         ((a.extend_flag >> 1) & 1) as pdm,((a.extend_flag >> 2) & 7) as region_cost,a.link_type,b.multi_link_attri 
                     FROM rdb_link as a
                     left join 
                     rdb_link_client as b
                     on a.link_id = ((b.link_id_t << 32) | b.link_id)
                ) as c 
                where c.one_way <> (c.multi_link_attri & 3)
                or ((c.multi_link_attri >> 2) & 1) <> c.regulation_flag
                or ((c.multi_link_attri >> 3) & 1) <> c.tilt_flag 
                or ((c.multi_link_attri >> 4) & 1) <> c.speed_regulation_flag 
                or ((c.multi_link_attri >> 5) & 1) <> c.link_add_info_flag 
                or ((c.multi_link_attri >> 6) & 1) <> c.shield_flag
                or c.pdm  <> ((c.multi_link_attri >> 7) & 1)
                or c.region_cost <> ((c.multi_link_attri >> 8) & 7)
                or c.link_type <> ((c.multi_link_attri >> 11) & 15)
                """

            self.pg.execute(sqlcmd)
            row = self.pg.fetchone()
            if row:
                if row[0] == 0:
                    return True
            return False
        
class CCheckFnlink(platform.TestCase.CTestCase):

    def _do(self):
 
        sqlcmd = """
   
                      (SELECT count(a.gid)as count 
                          FROM rdb_link as a
                      left join 
                      rdb_link_client as b
                      on a.link_id_t = b.link_id_t  and a.fnlink = b.fnlink and (a.link_id & ((1::bigint << 32) - 1)) = b.link_id 
                      ) 
                      union
                      select count(*) as count from rdb_link 

                """

        if self.pg.execute(sqlcmd) == -1:
            return 0
        else:
            if self.pg.getcnt() == 1:
                return 1
            else:
                return 0
                                  
class CCheckTnlink(platform.TestCase.CTestCase):

    def _do(self):  

        sqlcmd = """
                     (SELECT count(a.gid)as count 
                          FROM rdb_link as a
                      left join 
                      rdb_link_client as b
                      on a.link_id_t = b.link_id_t  and a.tnlink = b.tnlink and (a.link_id & ((1::bigint << 32) - 1)) = b.link_id 
                     ) 
                    union
                    select count(*) as count from rdb_link 
                """

        if self.pg.execute(sqlcmd) == -1:
            return 0
        else:
            if self.pg.getcnt() == 1:
                return 1
            else:
                return 0
        
class CCheckFazmPath(platform.TestCase.CTestCase):

    def _do(self):
 
        sqlcmd = """
   
                  (SELECT count(a.gid)as count 
                          FROM rdb_link as a
                      left join 
                      rdb_link_client as b
                      on a.link_id_t = b.link_id_t  and a.fazm_path = b.fazm_path and (a.link_id & ((1::bigint << 32) - 1)) = b.link_id 
                  )
                  union
                  select count(*) as count from rdb_link 

                """

        if self.pg.execute(sqlcmd) == -1:
            return 0
        else:
            if self.pg.getcnt() == 1:
                return 1
            else:
                return 0
               
class CCheckTazmPath(platform.TestCase.CTestCase):

    def _do(self):

        sqlcmd = """
    
                   (SELECT count(a.gid)as count 
                          FROM rdb_link as a
                      left join 
                      rdb_link_client as b
                      on a.link_id_t = b.link_id_t  and a.tazm_path = b.tazm_path and (a.link_id & ((1::bigint << 32) - 1)) = b.link_id
                  ) 
                    union
                  (
                      select count(*) as count from rdb_link 
                  ) 

                """
        if self.pg.execute(sqlcmd) == -1:
            return 0
        else:
            if self.pg.getcnt() == 1:
                return 1
            else:
                return 0
                
class CCheckGeomBlob(platform.TestCase.CTestCase):

    def _do(self):

            if self.pg.CreateFunction_ByName('pixel_tile2lonlat') == -1:
                return 0
            if self.pg.CreateFunction_ByName('pixel2lonlat_check') == -1:
                return 0                    
            if self.pg.CreateFunction_ByName('tile_bbox_check') == -1:
                return 0
            if self.pg.CreateFunction_ByName('world2lonlat_check') == -1:
                return 0
            if self.pg.CreateFunction_ByName('lonlat2world_check') == -1:
                return 0
            if self.pg.CreateFunction_ByName('pixel2world_check') == -1:
                return 0

            if self.pg.CreateFunction_ByName('CheckGeomBlob') == -1:
                return 0

            sqlcmd = """
                  SELECT min(gid), max(gid)
                  FROM rdb_link_client;
                """
            self.pg.execute(sqlcmd)
            row = self.pg.fetchone()
            min_id = row[0]
            max_id = row[1]

            sqlcmd = """
                select CheckGeomBlob(link_id, link_id_t, geom_blob, the_geom)
                  from rdb_link_client
                  where gid >= %s and gid <= %s
                ;
            """

            if self.pg._call_child_thread(min_id, max_id, sqlcmd, 4, 100000) == -1:
                return False
            else:
                return True
        
class CCheckLinkCommonAttr(platform.TestCase.CTestCase):

    def _do(self):

            sqlcmd = """
                              
                select count(*) from (
                     SELECT (a.extend_flag & 1) as pass_side,
                         a.toll,a.function_code,a.road_type,b.common_main_link_attri
                     FROM rdb_link as a
                     left join 
                     rdb_link_client as b
                     on a.link_id = ((b.link_id_t << 32) | b.link_id)
                ) as c 
                where c.pass_side <> (c.common_main_link_attri & 1)
                or ((c.common_main_link_attri >> 1) & 3) <> c.toll
                or ((c.common_main_link_attri >> 3) & 7) <> c.function_code 
                or ((c.common_main_link_attri >> 6) & 15) <> c.road_type 
                """

            self.pg.execute(sqlcmd)
            row = self.pg.fetchone()
            if row:
                if row[0] == 0:
                    return True
            return False