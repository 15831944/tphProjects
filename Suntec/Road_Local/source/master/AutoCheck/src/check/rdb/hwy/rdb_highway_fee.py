# -*- coding: UTF8 -*-
'''
Created on 2014-12-16

@author: hcz
'''
import platform.TestCase


# =============================================================================
# CCheckTollUnique -- up_down、facility_id在rdb_highway_fee_toll_info，不能重复
# ==============================================================================
class CCheckTollUnique(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT COUNT(*)
          FROM (
            SELECT up_down, facility_id
              FROM rdb_highway_fee_toll_info
              GROUP BY up_down, facility_id
              HAVING COUNT(*) > 1
          ) AS a
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckTollTicketFlag -- ticket_flag=1时，
#                         facility_id一定存在于rdb_highway_fee_ticket_toll
# ==============================================================================
class CCheckTollTicketFlag(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT COUNT(*)
          FROM rdb_highway_fee_toll_info as a
          LEFT JOIN rdb_highway_fee_ticket_toll as b
          ON a.facility_id = b.from_facility_id
          WHERE ticket_flag = 1 and
                b.from_facility_id is null;
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckTollAloneFlag -- alone_flag=1时，up_down、facility_id
#                        一定存在于rdb_highway_fee_alone_toll
# ==============================================================================
class CCheckTollAloneFlag(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT COUNT(*)
          FROM rdb_highway_fee_toll_info as a
          LEFT JOIN rdb_highway_fee_alone_toll as b
          ON a.facility_id = b.facility_id
             and a.up_down = b.up_down
          WHERE alone_flag = 1 and
                b.facility_id is null;
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckTollTicketFlag -- ticket_flag,  alone_flag, free_flag 不能同时都为 0
# ==============================================================================
class CCheckTollNotNull(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT count(*)
          FROM rdb_highway_fee_toll_info
          where ticket_flag = 0 and
                alone_flag = 0 and
                free_flag = 0
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckAloneUnique -- up_down、facility_id在rdb_highway_fee_alone_toll，不能重复
# ==============================================================================
class CCheckAloneUnique(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT COUNT(*)
          FROM (
            SELECT up_down, facility_id
              FROM rdb_highway_fee_alone_toll
              GROUP BY up_down, facility_id
              HAVING COUNT(*) > 1
          ) AS a;
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckAloneNotNull -- 收费不能都为空
# ==============================================================================
class CCheckAloneNotNull(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT count(*)
          FROM rdb_highway_fee_alone_toll
          WHERE k_money_idx in (-1, 0) and
                s_money_idx in (-1, 0) and
                m_money_idx in (-1, 0) and
                l_money_idx in (-1, 0) and
                vl_money_idx in (-1, 0);
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckAloneTileId
# ==============================================================================
class CCheckAloneTileId(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT count(*)
          FROM rdb_highway_fee_alone_toll as a
          LEFT JOIN rdb_highway_fee_toll_info as b
          ON a.up_down = b.up_down and a.facility_id = b.facility_id
          WHERE a.tile_id <> b.tile_id or b.tile_id is null;
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckTicketFromFacilityId
# from_facility_id在rdb_highway_fee_toll_info里
# ==============================================================================
class CCheckTicketFromFacilityId(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT COUNT(*)
          FROM rdb_highway_fee_ticket_toll AS a
          LEFT JOIN rdb_highway_fee_toll_info AS b
          ON a.from_facility_id = b.facility_id
          where ticket_flag <> 1 or b.facility_id is null;
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckTicketToFacilityId
# to_facility_id在rdb_highway_fee_toll_info里
# ==============================================================================
class CCheckTicketToFacilityId(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT COUNT(*)
          FROM rdb_highway_fee_ticket_toll AS a
          LEFT JOIN rdb_highway_fee_toll_info AS b
          ON a.to_facility_id = b.facility_id
          where ticket_flag <> 1 or b.facility_id is null;
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckTicketFromFacilityId
# from_tile_id和rdb_highway_fee_toll_info的from_facility_id的tile_id一致
# ==============================================================================
class CCheckTicketTileId(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT COUNT(*)
          FROM rdb_highway_fee_ticket_toll AS a
          LEFT JOIN rdb_highway_fee_toll_info AS b
          ON a.from_facility_id = b.facility_id
          where from_tile_id <> b.tile_id or b.tile_id is null
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckTicketUnique -- from_facility_id、to_facility_id不能重复
# ==============================================================================
class CCheckTicketUnique(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT count(*)
          FROM (
            SELECT from_facility_id, to_facility_id
              FROM rdb_highway_fee_ticket_toll
              GROUP BY from_facility_id, to_facility_id
              HAVING COUNT(*) > 1
          ) AS a;
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckTicketPairs -- A到B收费，同时要有B到A的收费
# ==============================================================================
class CCheckTicketPairs(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT count(*)
          FROM (
            SELECT from_facility_id, to_facility_id
              FROM rdb_highway_fee_ticket_toll
          ) AS a
          LEFT JOIN (
            SELECT from_facility_id, to_facility_id
              FROM rdb_highway_fee_ticket_toll
          ) AS b
          ON a.from_facility_id = b.to_facility_id
             and a.to_facility_id = b.from_facility_id
          where b.from_facility_id is null;
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckFreeFromFacilityId -- from_facility_id在rdb_highway_fee_toll_info里
# ==============================================================================
class CCheckFreeFromFacilityId(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT COUNT(*)
          FROM rdb_highway_fee_free_toll AS a
          LEFT JOIN rdb_highway_fee_toll_info AS b
          ON a.from_facility_id = b.facility_id
          where free_flag <> 1 or b.free_flag is null;
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckFreeToFacilityId -- to_facility_id在rdb_highway_fee_toll_info里
# ==============================================================================
class CCheckFreeToFacilityId(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT COUNT(*)
          FROM rdb_highway_fee_free_toll AS a
          LEFT JOIN rdb_highway_fee_toll_info AS b
          ON a.to_facility_id = b.facility_id
          where ticket_flag <> 1 or b.ticket_flag is null;
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckFreeUnique -- from_facility_id、to_facility_id不能重复
# ==============================================================================
class CCheckFreeUnique(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT COUNT(*)
          FROM (
            SELECT from_facility_id, to_facility_id
              FROM rdb_highway_fee_free_toll
              GROUP BY from_facility_id, to_facility_id
              HAVING COUNT(*) > 1
          ) AS a;
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckTicketFromFacilityId -- from_tile_id和from_facility_id的tile_id一致
# ==============================================================================
class CCheckFreeTileId(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT COUNT(*)
          FROM rdb_highway_fee_free_toll AS a
          LEFT JOIN rdb_highway_fee_toll_info AS b
          ON a.from_facility_id = b.facility_id
          where from_tile_id <> b.tile_id or b.tile_id is null
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckSameFromFacilityId -- from_facility_id在rdb_highway_fee_toll_info里
# ==============================================================================
class CCheckSameFromFacilityId(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT COUNT(*)
          FROM rdb_highway_fee_same_facility AS a
          LEFT JOIN rdb_highway_fee_toll_info AS b
          ON a.from_facility_id = b.facility_id
          where ticket_flag <> 1 or b.ticket_flag is null;;
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckFreeToFacilityId -- to_facility_id在rdb_highway_fee_toll_info里
# ==============================================================================
class CCheckSameToFacilityId(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT COUNT(*)
          FROM rdb_highway_fee_same_facility AS a
          LEFT JOIN rdb_highway_fee_toll_info AS b
          ON a.to_facility_id = b.facility_id
          where ticket_flag <> 1 or b.ticket_flag is null;;
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckSameUnique -- 不重复
# ==============================================================================
class CCheckSameUnique(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT COUNT(*)
          FROM (
            SELECT from_facility_id, to_facility_id
              FROM rdb_highway_fee_same_facility
              GROUP BY from_facility_id, to_facility_id
              HAVING COUNT(*) > 1
          ) AS a;
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckSameNest -- 是【入れ子料金所】
# ==============================================================================
class CCheckSameNest(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT COUNT(*)
          FROM rdb_highway_fee_same_facility
          WHERE from_facility_id NOT in (
            SELECT facility_id
              FROM rdb_highway_toll_info
              WHERE nest = 1
          );
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False
