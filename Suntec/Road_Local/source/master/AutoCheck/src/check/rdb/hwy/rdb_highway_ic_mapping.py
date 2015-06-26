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
        SELECT COUNT(*)
          FROM rdb_highway_ic_mapping
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
# CCheckFacilityID -- 和rdb_highway_ic_info对应的up_down, facility_id一致
# ==============================================================================
class CCheckFacilityID(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT COUNT(*)
          FROM rdb_highway_ic_mapping as a
          LEFT JOIN rdb_highway_ic_info as b
          ON a.ic_no = b.ic_no
          where a.facility_id <> b.facility_id
                or b.facility_id is null
                or a.up_down <> b.up_down
                or b.up_down is null;
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckUnique -- ic_no是不重复的
# ==============================================================================
class CCheckUnique(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT COUNT(*)
          FROM (
            SELECT ic_no
              FROM rdb_highway_ic_mapping
              GROUP BY ic_no
              having COUNT(*) > 1
          ) AS a;
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckTileId -- 和rdb_highway_ic_info里该ic_no的tile_id一致
# ==============================================================================
class CCheckTileId(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT COUNT(*)
          FROM rdb_highway_ic_mapping as a
          LEFT JOIN rdb_highway_ic_info as b
          ON a.ic_no = b.ic_no
          where a.tile_id <> b.tile_id
                or b.tile_id is null;
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckICIndex -- 和rdb_highway_ic_info里该ic_no的index一致
# ==============================================================================
class CCheckICIndex(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT COUNT(*)
          FROM rdb_highway_ic_mapping as a
          LEFT JOIN rdb_highway_ic_info as b
          ON a.ic_no = b.ic_no
          where a.ic_index <> b.index
                or b.index is null;
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False
