# -*- coding: UTF8 -*-
'''
Created on 2014-12-16

@author: hcz
'''
import platform.TestCase


# =============================================================================
# CCheckICNo -- ic_no存在表rdb_highway_ic_info中
# ==============================================================================
class CCheckICNo(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT count(*)
          FROM rdb_highway_path_point
          where ic_no not in (
            SELECT ic_no
              FROM rdb_highway_ic_info
          );
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckNodeId -- 表示、出口、入口是link的脱出点，否则，是link的进入点
# ==============================================================================
class CCheckNodeId(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT COUNT(*)
          FROM rdb_highway_path_point AS a
          LEFT JOIN rdb_link AS b
          ON a.link_id = b.link_id
          where (center_flag = 1 or exit_flag = 1 or center_flag = 1) and
                ((one_way = 2 and node_id <> end_node_id) or    -- Positive
                 (one_way = 3 and node_id <> start_node_id) or  -- reverse
                 one_way not in (2, 3))
        UNION
        SELECT COUNT(*)
          FROM rdb_highway_path_point AS a
          LEFT JOIN rdb_link AS b
          ON a.link_id = b.link_id
          where (main_flag = 1) and
                ((one_way = 2 and node_id <> start_node_id) or  -- Positive
                 (one_way = 3 and node_id <> end_node_id) or    -- reverse
                 one_way not in (2, 3))
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckLonLat -- 表示、出口、入口是link的脱出点坐标一致，否则，是link的进入点坐标一致
# ==============================================================================
class CCheckLonLat(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT count(*)
          FROM rdb_highway_path_point as a
          LEFT JOIN rdb_node as b
          ON a.node_id = b.node_id
          where lonlat <> test_check_geom_2_lonlat(b.the_geom)
        """
        self.pg.CreateFunction_ByName('test_check_geom_2_lonlat')
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckTileId
# ==============================================================================
class CCheckTileId(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT COUNT(*)
          FROM (
            SELECT a.ic_no
              FROM rdb_highway_ic_info as a
              LEFT JOIN rdb_highway_path_point as b
              on a.ic_no = b.ic_no
              WHERE boundary <> 1 and
                    (a.tile_id <> b.tile_id or b.tile_id is null)
            union
            SELECT a.ic_no
              FROM rdb_highway_ic_info as a
              LEFT JOIN rdb_highway_path_point as b
              on a.ic_no = b.ic_no
              WHERE boundary = 1 and
                ((b.enter_flag = 1 and
                 (a.tile_id <> b.tile_id and a.conn_tile_id <> b.tile_id)) or
                 (b.main_flag = 1 and
                 (a.tile_id <> b.tile_id and a.conn_tile_id <> b.tile_id))
                )
          ) AS c;
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckBoundaryLonLat -- 表示中心和本線地点同个点
# ==============================================================================
class CCheckBoundaryLonLat(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT count(*)
          FROM (
            SELECT ic_no, node_id
              FROM (
                SELECT distinct a.ic_no, node_id
                  FROM rdb_highway_ic_info AS a
                  LEFT JOIN rdb_highway_path_point AS b
                  ON a.ic_no = b.ic_no
                  WHERE boundary = 1
              ) AS c
              group by ic_no, node_id
              having count(*) > 1
          ) AS d;
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckIndexUnique -- Index值唯一
# ==============================================================================
class CCheckIndexUnique(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT count(*)
          FROM (
            SELECT tile_id, index
              FROM rdb_highway_path_point
              GROUP BY  tile_id, index
              HAVING COUNT(*) > 1
          ) AS a;
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckIndexValue -- 同个设施内值是连续的
# ==============================================================================
class CCheckIndexValue(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT ic_no, array_agg(index)
          FROM (
            SELECT ic_no, index
              FROM rdb_highway_path_point
              ORDER BY ic_no, index
          ) as A
          group by ic_no
          order by ic_no;
        """
        from check.rdb.hwy.rdb_highway_store_info import check_index_continuous
        self.pg.execute(sqlcmd)
        for toll in self.pg.get_batch_data(sqlcmd):
            all_index = toll[0]
            if not check_index_continuous(all_index):
                return False
        return True
