# -*- coding: UTF8 -*-
'''
Created on 2014-12-22

@author: hcz
'''
import platform.TestCase


# =============================================================================
# CCheckLinkFacilityNum --  link上的设施数要与实际做成的facility_num匹配
# =============================================================================
class CCheckLinkFacilityNum(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
        SELECT a.link_id, facility_num, num
           FROM (
             SELECT link_id, facility_num
               FROM rdb_highway_node_add_info
           ) AS a
           LEFT JOIN (
             SELECT link_id, COUNT(link_id) as num
               FROM rdb_highway_node_add_info
               group by link_id
           ) AS b
           on a.link_id = b.link_id
           where facility_num <> num
           order by a.link_id
        '''
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row:
            return False
        return True


# =============================================================================
# CCheckLinkId --  收费站附加情报link和其他设施附加情报link，不能重叠
# =============================================================================
class CCheckTollLinkId(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
        SELECT count(a.link_id)
        FROM (
            SELECT link_id
            FROM rdb_highway_node_add_info
            where toll_flag = 1
        ) AS a
        inner join (
            SELECT link_id
            FROM rdb_highway_node_add_info
            where toll_flag <> 1
        ) AS b
        On a.link_id = b.link_id
        '''
        for row in self.pg.get_batch_data(sqlcmd):
            count = row[0]
            if count != 0:
                return False
        return True


# =============================================================================
# CCheckLinkId -- link_id存在表rdb_link中
# =============================================================================
class CCheckLinkId(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT count(a.link_id)
          FROM rdb_highway_node_add_info as a
          LEFT JOIN rdb_link as b
          ON a.link_id = b.link_id
          WHERE b.link_id is NULL;
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if not row and row[0] != 0:
            return False
        # link repeat
        sqlcmd = """
        SELECT COUNT(*)
          FROM (
            SELECT link_id
              FROM rdb_highway_node_add_info
              GROUP BY link_id
              HAVING COUNT(*) > 1
          ) AS a;
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if not row and row[0] != 0:
            return False
        return True


# =============================================================================
# CCheckNodeId -- node_id是link_id的端点
# ==============================================================================
class CCheckNodeId(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT count(a.link_id)
          FROM rdb_highway_node_add_info as a
          LEFT JOIN rdb_link as b
          ON a.link_id = b.link_id
          WHERE node_id not in (start_node_id, end_node_id);
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckStartEndFlag
# ==============================================================================
class CCheckStartEndFlag(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT count(a.link_id)
          FROM rdb_highway_node_add_info as a
          LEFT JOIN rdb_link as b
          ON a.link_id = b.link_id
          WHERE (node_id = start_node_id and start_end_flag <> 0) or
                (node_id = end_node_id and start_end_flag <> 1);
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckTollFlag -- 料金案内Flag
# ==============================================================================
class CCheckTollFlag(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT COUNT(*)
          FROM (
            SELECT link_id
              FROM rdb_highway_node_add_info
              WHERE toll_flag = 1 AND (dummy = 1 AND not_guide = 1)
            UNION
            SELECT link_id
              FROM rdb_highway_node_add_info
              WHERE toll_flag <> 1 AND (dummy # not_guide <> 0)
            UNION
            SELECT link_id
              FROM rdb_highway_node_add_info
              WHERE toll_type_num > 0 AND
                NOT (dummy = 1 AND not_guide = 1) AND
                toll_flag <> 1
          ) AS a;
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckFacilityNumNI -- 设施数>0
# ==============================================================================
class CCheckFacilityNumNI(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT COUNT(facility_num)
          FROM rdb_highway_node_add_info
          WHERE facility_num = 0 or
                facility_num is null;
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckFacilityNum -- 设施数0，或者1
# ==============================================================================
class CCheckFacilityNum(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT COUNT(facility_num)
          FROM rdb_highway_node_add_info
          WHERE facility_num NOT IN (0, 1);
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckUpDown -- 0=下り / 1=上り / 3=上下線共有
# ==============================================================================
class CCheckUpDown(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT count(*)
          FROM rdb_highway_node_add_info
          WHERE up_down not in (0, 1, 3);
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckFacilityType -- 所有种别的值范围(0, 1)
# ==============================================================================
class CCheckFacilityType(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT count(link_id)
          FROM rdb_highway_node_add_info
          where etc_antenna not in (0, 1) or  enter not in (0, 1) or
                exit not in (0, 1) or jct not in (0, 1) or
                sa not in (0, 1) or pa not in (0, 1) or
                gate not in (0, 1) or un_open not in (0, 1) or
                dummy not in (0, 1);
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckTollType -- toll_type的值范围(0, 1)
# ==============================================================================
class CCheckTollType(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT count(*)
          FROM rdb_highway_node_add_info
          where non_ticket_gate not in (0, 1) or  check_gate not in (0, 1) or
                single_gate not in (0, 1) or cal_gate not in (0, 1) or
                ticket_gate not in (0, 1) or nest not in (0, 1) or
                uturn not in (0, 1) or not_guide not in (0, 1) or
                normal_toll not in (0, 1) or etc_toll not in (0, 1) or
                etc_section not in (0, 1);
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if not row and row[0] != 0:
            return False

        sqlcmd = """
        SELECT count(*)
          FROM rdb_highway_node_add_info
          where toll_type_num > 0 and
                non_ticket_gate = 0 and check_gate = 0 and
                single_gate = 0 and cal_gate = 0 and
                ticket_gate = 0 and nest = 0 and
                uturn = 0 and not_guide = 0 and
                etc_toll = 0 and etc_section = 0
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if not row and row[0] != 0:
            return False
        return True


# =============================================================================
# CCheckNormalToll -- 所有料金所的normal_toll都是1
# ==============================================================================
class CCheckNormalToll(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT count(*)
          FROM rdb_highway_node_add_info
          where toll_type_num > 0 and normal_toll <> 1;
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
        SELECT link_id, tile_id
          FROM rdb_highway_node_add_info;
        """
        from check.rdb.hwy.rdb_highway_mapping import convert_tile_id
        from check.rdb.hwy.rdb_highway_mapping import ID_SIZE
        for row in self.pg.get_batch_data(sqlcmd):
            link_id = row[0]
            tile_id = row[1]
            tile_id_14 = (link_id >> ID_SIZE) & ((1 << ID_SIZE) - 1)
            hwy_tile_id = convert_tile_id(tile_id_14)
            if hwy_tile_id != tile_id:
                return False
        return True


# =============================================================================
# CCheckHwyFlag
# ==============================================================================
class CCheckHwyFlag(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT count(*)
          FROM rdb_highway_node_add_info AS a
          LEFT JOIN rdb_link_with_all_attri_view AS b
          ON a.link_id = b.link_id
          WHERE hwy_flag <> 1;
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if not row and row[0] != 0:
            return False

        sqlcmd = """
        SELECT count(*)
          FROM rdb_highway_node_add_info AS a
          LEFT JOIN rdb_link AS b
          ON a.link_id = b.link_id
          WHERE hwy_flag <> 1;
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if not row and row[0] != 0:
            return False
        return True
