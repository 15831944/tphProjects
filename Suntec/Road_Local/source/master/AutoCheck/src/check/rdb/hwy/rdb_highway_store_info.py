# -*- coding: UTF8 -*-
'''
Created on 2014-12-17

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
          FROM rdb_highway_store_info
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
# CCheckStoreFlag
# ==============================================================================
class CCheckStoreFlag(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT count(*)
          FROM rdb_highway_store_info AS a
          LEFT JOIN rdb_highway_ic_info AS b
          ON a.ic_no = b.ic_no
          WHERE store_count < 1
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
              FROM rdb_highway_store_info
              GROUP BY ic_no, bis_time_flag, bis_time_num,
                   store_kind, open_hour, open_min,
                   close_hour, close_min, holiday,
                   goldenweek, newyear, yearend,
                   bonfestival, sunday, saturday,
                   friday, thursday, wednesday,
                   tuesday, monday
              HAVING COUNT(*) > 1
          ) AS a;
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
            SELECT tile_id, tile_index
              FROM rdb_highway_store_info
              GROUP BY  tile_id, tile_index
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
        SELECT array_agg(one_store) as all_store_index
          FROM (
            SELECT ic_no, store_kind,
                   array_to_string(array_agg(tile_index), ',') as one_store
              FROM (
                SELECT ic_no, store_kind, tile_index
                  FROM rdb_highway_store_info
                  order by ic_no, store_kind, tile_index
              ) AS a
              GROUP BY ic_no, store_kind
              order by ic_no, store_kind
          ) AS b
          GROUP BY ic_no
          ORDER BY ic_no;
        """
        self.pg.execute(sqlcmd)
        for ic_store in self.pg.get_batch_data(sqlcmd):
            all_store_index = ic_store[0]
            all_idx_list = []
            for one_store_idx in all_store_index:
                one_idx_list = [int(n) for n in one_store_idx.split(',')]
                if not check_index_continuous(one_idx_list):
                    return False
                all_idx_list += one_idx_list
            if not check_index_continuous(all_idx_list):
                return False
        return True


def check_index_continuous(index_list):
    index_list = []
    index_list.sort()
    i = 1
    while i < len(index_list):
        if index_list[i - 1] != (index_list[i] + 1):
            return False
    return True
