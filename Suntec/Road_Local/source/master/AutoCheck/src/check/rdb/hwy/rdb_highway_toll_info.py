# -*- coding: UTF8 -*-
'''
Created on 2014-12-17

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
          FROM rdb_highway_toll_info
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
# CCheckTollFlag
# ==============================================================================
class CCheckTollFlag(platform.TestCase.CTestCase):
    def _do(self):
        # 设施料金
        sqlcmd = """
        SELECT count(*)
          FROM rdb_highway_toll_info AS a
          LEFT JOIN rdb_highway_ic_info AS b
          ON a.ic_no = b.ic_no
          where conn_ic_no is null and b.toll_count < 1
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if not row and row[0] != 0:
            return False
        # 接续料金
        sqlcmd = """
        SELECT a.*, b.toll_count
          FROM rdb_highway_toll_info AS a
          LEFT JOIN rdb_highway_conn_info AS b
          ON a.ic_no = b.ic_no and a.conn_ic_no = b.conn_ic_no
          where a.conn_ic_no is not null and
                ((b.toll_count < 1 and conn_direction = 0)
                  or b.toll_count is null);
        """
        if not row and row[0] != 0:
            return False
        return True


# =============================================================================
# CCheckTileId
# ==============================================================================
class CCheckTileId(platform.TestCase.CTestCase):
    def _do(self):
        # 设施料金
        sqlcmd = """
        SELECT count(*)
          FROM rdb_highway_toll_info AS a
          LEFT JOIN rdb_highway_ic_info AS b
          ON a.ic_no = b.ic_no
          where a.tile_id <> b.tile_id;
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckFacilityId -- facility_id都有料金金额情报
# ==============================================================================
class CCheckFacilityId(platform.TestCase.CTestCase):
    def _do(self):
        # 设施料金
        sqlcmd = """
        SELECT COUNT(*)
          FROM rdb_highway_toll_info AS a
          LEFT JOIN rdb_highway_fee_toll_info AS b
          ON a.facility_id = b.facility_id
          where b.facility_id is null
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
              FROM rdb_highway_toll_info
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
        SELECT array_agg(tile_index)
          FROM (
            SELECT ic_no, conn_ic_no, tile_index
              FROM rdb_highway_toll_info
              order by ic_no, conn_ic_no, tile_index
          ) AS a
          GROUP BY ic_no, conn_ic_no
        """
        from check.rdb.hwy.rdb_highway_store_info import check_index_continuous
        self.pg.execute(sqlcmd)
        for toll in self.pg.get_batch_data(sqlcmd):
            all_index = toll[0]
            if not check_index_continuous(all_index):
                return False
        return True


# =============================================================================
# CCheckTollType -- toll_type的值范围(0, 1)
# ==============================================================================
class CCheckTollType(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT count(*)
          FROM rdb_highway_toll_info
          where non_ticket_gate not in (0, 1) or  check_gate not in (0, 1) or
                single_gate not in (0, 1) or cal_gate not in (0, 1) or
                ticket_gate not in (0, 1) or nest not in (0, 1) or
                uturn not in (0, 1) or not_guide not in (0, 1) or
                normal_toll not in (0, 1) or etc_toll not in (0, 1) or
                etc_section not in (0, 1);
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckTollClass -- 1: 入口，2: 出口， 3：通过，4: 分歧
# ==============================================================================
class CCheckTollClass(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT COUNT(toll_class)
          FROM rdb_highway_toll_info
          WHERE toll_class not in (1, 2, 3, 4);
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckFacilityType -- etc_antenna, enter, exit, jct, sa_pa,  gate, unopen都为0
# ==============================================================================
class CCheckFacilityType(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT count(*)
          FROM rdb_highway_toll_info
          WHERE etc_antenna <> 0 or enter <> 0 or
                exit <> 0 or jct <> 0 or
                sa_pa <> 0 or gate <> 0 or
                unopen <> 0;
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False
