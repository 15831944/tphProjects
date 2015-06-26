# -*- coding: UTF8 -*-
'''
Created on 2014-12-18

@author: hcz
'''
import platform.TestCase


# =============================================================================
# CCheckICNo
# ==============================================================================
class CCheckICNo(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT count(*)
          FROM rdb_highway_illust_info
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
# CCheckTileId
# ==============================================================================
class CCheckTileId(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT count(*)
          FROM rdb_highway_illust_info as a
          LEFT JOIN rdb_highway_ic_info as b
          ON a.ic_no = b.ic_no
          where a.tile_id <> b.tile_id or b.tile_id is null;
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckIllustCount
# ==============================================================================
class CCheckIllustCount(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT count(*)
          FROM rdb_highway_illust_info as a
          LEFT JOIN rdb_highway_ic_info as b
          ON a.ic_no = b.ic_no
          where b.illust_count <> 1 or b.illust_count is null;
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckUnique
# ==============================================================================
class CCheckUnique(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT COUNT(*)
          FROM (
            SELECT ic_no
              FROM rdb_highway_illust_info
              GROUP BY ic_no
              HAVING COUNT(*) > 1
          ) AS a;
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False
