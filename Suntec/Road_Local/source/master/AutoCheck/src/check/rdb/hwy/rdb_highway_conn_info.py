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
          FROM rdb_highway_conn_info
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
# CCheckSameRoad -- ic_no和conn_ic_no的道路名称相同
# ==============================================================================
class CCheckSameRoad(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT count(*)
          FROM rdb_highway_conn_info as a
          LEFT JOIN rdb_highway_ic_info as b
          ON a.ic_no = b.ic_no
          LEFT JOIN (
              SELECT distinct road_no, name
              FROM rdb_highway_road_info
          ) AS c
          ON b.road_no = c.road_no
          LEFT JOIN (
              SELECT distinct road_no, name
              FROM rdb_highway_road_info
          ) AS d
          ON a.conn_road_no = d.road_no
          where same_road_flag = 1 and c.name <> d.name;
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckSimilarRoad -- ic_no和conn_ic_no的道路名称相似
# ==============================================================================
class CCheckSimilarRoad(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT c.name, d.name
          FROM rdb_highway_conn_info as a
          LEFT JOIN rdb_highway_ic_info as b
          ON a.ic_no = b.ic_no
          LEFT JOIN (
              SELECT distinct road_no, name
              FROM rdb_highway_road_info
          ) AS c
          ON b.road_no = c.road_no
          LEFT JOIN (
              SELECT distinct road_no, name
              FROM rdb_highway_road_info
          ) AS d
          ON a.conn_road_no = d.road_no
          where same_road_flag = 1 and c.name <> d.name;
        """
        for row in self.pg.get_batch_data(sqlcmd):
            name1 = row[0]
            name2 = row[1]
            if not is_similar_name(name1, name2):
                return False
        return True


def is_similar_name(name_str1, name_str2):
    import json
    if not name_str1 and not name_str2:
        return True
    if((name_str1 and not name_str2) or
       (not name_str1 and name_str2)):
        return False
    name1 = json.loads(name_str1)
    name2 = json.loads(name_str2)
    name_list1 = []
    name_list2 = []
    for name_list in name1:
        name_list1 += name_list
    for name_list in name2:
        name_list2 += name_list
    for name in name_list1:
        if name in name_list2:
            return True
    return False


# =============================================================================
# CCheckTileChange -- tile_id和conn_tile_id的道路名称相同
# ==============================================================================
class CCheckTileChange(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT count(*)
          FROM rdb_highway_conn_info
          WHERE (tile_id <> conn_tile_id and tile_change_flag <> 1) or
                (tile_id = conn_tile_id and tile_change_flag = 1)
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckUturn
# ==============================================================================
class CCheckUturn(platform.TestCase.CTestCase):
    def _do(self):
        # Uturn都是same road
        sqlcmd = """
        SELECT count(*)
          FROM rdb_highway_conn_info
          where uturn_flag = 1 and same_road_flag <> 1
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if not row or row[0] != 0:
            return False
        return True


# =============================================================================
# CCheckUturnFacility
# ==============================================================================
class CCheckUturnFacility(platform.TestCase.CTestCase):
    def _do(self):
        # Uturn都是facility相同，上下行不同
        sqlcmd = """
        SELECT count(*)
          FROM rdb_highway_conn_info as a
          LEFT JOIN rdb_highway_ic_info as b
          ON a.ic_no = b.ic_no
          LEFT JOIN rdb_highway_ic_info as c  -- conn facility id
          ON a.conn_ic_no = c.ic_no
          where uturn_flag = 1 and
                (b.facility_id <> c.facility_id or b.up_down = c.up_down)
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if not row or row[0] != 0:
            return False
        return True


# =============================================================================
# CCheckConnRoadNo
# ==============================================================================
class CCheckConnRoadNo(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT count(*)
          FROM rdb_highway_conn_info as a
          LEFT JOIN rdb_highway_ic_info as b
          ON conn_ic_no = b.ic_no
          where a.conn_road_no <> b.road_no or b.road_no is null;
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row or row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckConnICNo
# ==============================================================================
class CCheckConnICNo(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT count(*)
          FROM rdb_highway_conn_info
          where conn_ic_no not in (
            SELECT conn_ic_no
              FROM rdb_highway_ic_info
          );
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row or row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckConnTileId
# ==============================================================================
class CCheckConnTileId(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT count(*)
          FROM rdb_highway_conn_info as a
          LEFT JOIN rdb_highway_ic_info as b
          ON conn_ic_no = b.ic_no
          where a.conn_tile_id <> b.tile_id or b.tile_id is null
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row or row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckTollIndex -- 料金情報起始index
# ==============================================================================
class CCheckTollIndex(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT count(*)
          FROM rdb_highway_conn_info as a
          LEFT JOIN (
            SELECT ic_no, conn_ic_no, min(tile_index) as min_toll_index
              FROM rdb_highway_toll_info
              where conn_ic_no is not null
              group by ic_no, conn_ic_no
          ) as b
          ON a.ic_no = b.ic_no and a.conn_ic_no = b.conn_ic_no
          where a.toll_index <> 0 and
               (a.toll_index <> b.min_toll_index or b.min_toll_index is null);
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row or row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckTollCount -- 料金个数
# ==============================================================================
class CCheckTollCount(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT count(*)
          FROM rdb_highway_conn_info as a
          LEFT JOIN (
            SELECT ic_no, conn_ic_no, count(tile_index) as toll_num
              FROM rdb_highway_toll_info
              where conn_ic_no is not null
              group by ic_no, conn_ic_no
          ) as b
          ON a.ic_no = b.ic_no and a.conn_ic_no = b.conn_ic_no
          where a.toll_count <> 0 and
               (a.toll_count <> b.toll_num or b.toll_num is null);
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row or row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckUnique -- ic_no和conn_ic_no是唯一的，不重叠
# ==============================================================================
class CCheckUnique(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT count(*)
          FROM (
            SELECT ic_no, conn_ic_no, conn_direction
              FROM rdb_highway_conn_info
              GROUP BY ic_no, conn_ic_no, conn_direction
              HAVING COUNT(*) > 1
          ) AS a;
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row or row[0] == 0:
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
              FROM rdb_highway_conn_info
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
              FROM rdb_highway_conn_info
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
