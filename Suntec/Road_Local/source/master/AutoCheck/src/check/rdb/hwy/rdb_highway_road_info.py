# -*- coding: UTF8 -*-
'''
Created on 2014-12-16

@author: hcz
'''
import platform.TestCase


# =============================================================================
# CCheckRoadUnique -- road_no、road_attr在tile内是唯一的
# ==============================================================================
class CCheckRoadUnique(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT COUNT(*)
          FROM (
            SELECT road_no, road_attr, tile_id
              FROM rdb_highway_road_info
              group by road_no, road_attr, tile_id
              having count(*) > 1
          ) as a;
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckCircle -- 本线link形成一环
# ==============================================================================
class CCheckCircle(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        """
        # aself.pg.execute(sqlcmd)
        # row = self.pg.fetchone()
        # if row and row[0] == 0:
        #    return True
        return False


# =============================================================================
# CCheckStartICNo -- 等于该tile内该侧道路上最小的设施
# ==============================================================================
class CCheckStartICNo(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT COUNT(*)
          FROM rdb_highway_road_info as a
          LEFT JOIN (
            SELECT road_no, tile_id, up_down, min(ic_no) as min_ic_no
              FROM rdb_highway_ic_info
              group by road_no, tile_id, up_down
          ) as b
          ON a.road_no = b.road_no
             and a.tile_id = b.tile_id
             and a.up_down = b.up_down
          WHERE start_ic_no <> min_ic_no;
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckICNum -- 等于该tile内该侧道路上设施数
# ==============================================================================
class CCheckICNum(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT COUNT(*)
        FROM rdb_highway_road_info as a
        LEFT JOIN (
            SELECT road_no, tile_id, up_down, count(*) as ic_num
            FROM rdb_highway_ic_info
            group by road_no, tile_id, up_down
        ) as b
        ON a.road_no = b.road_no
        and a.tile_id = b.tile_id
        and a.up_down = b.up_down
        WHERE ic_count <> ic_num;
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckRoadType -- 和rdb_highway_mapping中同一road_no同侧本线link（path_type=MAIN）
#                   数目最多的road_type一致
# ==============================================================================
class CCheckRoadType(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT count(*)
          FROM rdb_highway_road_info AS road
          LEFT JOIN (
            -- Get road_type of Max Number
            SELECT road_no, road_kind, (ARRAY_AGG(road_type))[1] as road_type
              FROM (
                SELECT road_no, road_kind, road_type, count(road_type) AS num
                  FROM rdb_highway_mapping as a
                  LEFT JOIN rdb_link as b
                  ON a.link_id = b.link_id
                  where path_type = 'MAIN'
                  group by road_no, road_kind, road_type
                  order by road_no, road_kind, num desc, road_type
              ) AS c
              GROUP BY road_no, road_kind
          ) as d
          ON road.road_no = d.road_no and road.road_attr = d.road_kind
          where road.road_kind <> d.road_type or d.road_type is null;
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckTileID -- 和起始设施的tile_id一致
# ==============================================================================
class CCheckTileID(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT COUNT(*)
          FROM rdb_highway_road_info AS a
          LEFT JOIN (
            -- NOT boundary ic
            SELECT ic_no, tile_id
              FROM rdb_highway_ic_info
              WHERE boundary <> 1
            UNION
            -- boundary start ic
            SELECT a.ic_no, b.tile_id
              FROM (
                SELECT ic_no, tile_id
                  FROM rdb_highway_ic_info
                  WHERE boundary = 1
              ) AS a
              INNER JOIN (
                SELECT ic_no, tile_id
                  FROM rdb_highway_path_point
                  WHERE main_flag = 1
              ) AS b
              ON a.ic_no = b.ic_no and a.tile_id = b.tile_id
              order by ic_no
          ) AS b
          ON a.start_ic_no = b.ic_no
          where a.tile_id <> b.tile_id or b.tile_id is NULL;
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False
