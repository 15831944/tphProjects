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
          FROM rdb_highway_service_info
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
# CCheckServerFlag -- ic_no唯一
# ==============================================================================
class CCheckServerFlag(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT count(*)
          FROM rdb_highway_service_info as a
          LEFT JOIN rdb_highway_ic_info as b
          ON a.ic_no = b.ic_no
          where b.servise_flag <> 1 or
                b.servise_flag is null;
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckUnique -- ic_no唯一
# ==============================================================================
class CCheckUnique(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT count(*)
          FROM (
            SELECT ic_no
              FROM rdb_highway_service_info
              group by ic_no
              having count(*) > 1
          ) AS a;
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False
