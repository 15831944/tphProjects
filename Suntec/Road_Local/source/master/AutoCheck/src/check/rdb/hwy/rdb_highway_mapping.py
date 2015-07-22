# -*- coding: UTF8 -*-
'''
Created on 2014-12-3

@author: hcz
'''

import platform.TestCase
from ctypes import c_int
HWY_TILE_LAYER = 10
TILE_LAYER_14 = 14
ID_SIZE = 32


def convert_tile_id(tile_id_14):
    if tile_id_14 is None:
        return None
    SHIFT = TILE_LAYER_14 - HWY_TILE_LAYER
    MASK = (1 << HWY_TILE_LAYER) - 1
    tile_z = HWY_TILE_LAYER
    tile_x = (tile_id_14 >> (TILE_LAYER_14 + SHIFT)) & MASK
    tile_y = (tile_id_14 >> SHIFT) & MASK
    new_tile_id = tile_z << (TILE_LAYER_14 * 2)
    new_tile_id |= (tile_x << TILE_LAYER_14)
    new_tile_id |= tile_y
    sign_tile_id = c_int(new_tile_id).value
    return sign_tile_id


# =============================================================================
# CCheckICCount -- ic_count和条目一致
# ==============================================================================
class CCheckICCount(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT count(*)
          FROM rdb_highway_mapping as A
          LEFT JOIN (
            SELECT link_id, count(link_id) as cnt
              FROM rdb_highway_mapping
              group by link_id
          ) as B
          ON A.link_id = b.link_id
          where ic_count <> cnt;
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckLinkID -- link_id
# ==============================================================================
class CCheckLinkID(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT count(a.link_id) as num
          FROM rdb_highway_mapping as a
          left join rdb_link as b
          ON a.link_id = b.link_id
          WHERE b.link_id is null;
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckICNo -- check forward_ic_no and backward_ic_no
# ==============================================================================
class CCheckICNo(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT link_id,
               array_agg(forward_ic_no) as fwd_ic_nos,
               array_agg(backward_ic_no) as bwd_ic_nos
          FROM rdb_highway_mapping
          group by link_id;
        """
        for row in self.pg.get_batch_data(sqlcmd):
            fwd_ic_nos = set(row[1])
            bwd_ic_nos = set(row[2])
            # 前后设施都是0(空)
            if fwd_ic_nos == set([0]) and bwd_ic_nos == set([0]):
                return False
            # 前后设施个数都超1，即多对多
            if len(fwd_ic_nos) > 1 and len(bwd_ic_nos) > 1:
                return False
        return True


# =============================================================================
# CCheckICNo2 -- check forward_ic_no and backward_ic_no
# ==============================================================================
class CCheckICNo2(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT m.link_id,
               array_agg(forward_ic_no) as fwd_ic_nos,
               array_agg(backward_ic_no) as bwd_ic_nos,
               one_way
          FROM rdb_highway_mapping as m
          LEFT JOIN rdb_link as l
          ON m.link_id = l.link_id
          group by m.link_id, one_way;
        """
        for row in self.pg.get_batch_data(sqlcmd):
            fwd_ic_nos = set(row[1])
            bwd_ic_nos = set(row[2])
            one_way = row[3]
            # 前后设施都是0(空)
            if(fwd_ic_nos == set([0]) and
               bwd_ic_nos == set([0]) and
               one_way != 1):  # 非双向
                return False
            if(one_way == 1 and fwd_ic_nos != bwd_ic_nos):
                return False
            # 前后设施个数都超1，即多对多
            if len(fwd_ic_nos) > 1 and len(bwd_ic_nos) > 1:
                return False
        return True

# =============================================================================
# CCheckRoadNo -- 一条link不能有两个Road No
# ==============================================================================
class CCheckRoadNo(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT COUNT(*)
          FROM (
            SELECT link_id
              FROM (
                SELECT distinct link_id, road_no, road_kind
                  FROM rdb_highway_mapping
              ) AS A
              group by link_id, road_no, road_kind
              having count(*) > 1
          ) AS B
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckTileId -- Tile Id
# ==============================================================================
class CCheckTileId(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT distinct link_id, tile_id
          FROM rdb_highway_mapping;
        """
        for row in self.pg.get_batch_data(sqlcmd):
            link_id = row[0]
            tile_id = row[1]
            tile_id_14 = (link_id >> ID_SIZE) & ((1 << ID_SIZE) - 1)
            hwy_tile_id = convert_tile_id(tile_id_14)
            if hwy_tile_id != tile_id:
                return False
        return True


# =============================================================================
# CCheckHwyLink -- check所有的高速link都做到 Highway mapping表
# ==============================================================================
class CCheckHwyLink(platform.TestCase.CTestCase):
    def _do(self):
        self.pg.CreateFunction_ByName('test_exist_branch_in_hwy_mapping')
        sqlcmd = """
        SELECT count(*)
           FROM (
            SELECT a.link_id, link_type,
                   test_exist_branch_in_hwy_mapping(start_node_id,
                                                    end_node_id,
                                                    one_way) as in_hwy_flag
              FROM (
                SELECT link_id, start_node_id, end_node_id, one_way,
                       link_type
                  FROM rdb_link
                  where (toll = 1 or road_type in (0, 1))
              ) as a
              LEFT JOIN rdb_highway_mapping AS  b
              ON a.link_id = b.link_id
              where b.link_id is null
          ) as a
          where in_hwy_flag;
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] <= 2:  # 有两根误判断
            return True
        return False


# =============================================================================
# CCheckHwyLink_17cy -- check所有的高速link都做到 Highway mapping表
# ==============================================================================
class CCheckHwyLink_17cy(platform.TestCase.CTestCase):
    def _do(self):
        self.pg.CreateFunction_ByName('test_exist_branch_in_hwy_mapping')
        sqlcmd = """
        SELECT count(*)   -- a.link_id, link_type, one_way
          FROM (
            SELECT link_id, start_node_id, end_node_id, one_way,
                   link_type
              FROM rdb_link
              where road_type in (0)
          ) as a
          LEFT JOIN rdb_highway_mapping AS  b
          ON a.link_id = b.link_id
          LEFT JOIN rdb_highway_not_hwy_model_link as c
          ON a.link_id = c.link_id
          where b.link_id is null and c.link_id is null;
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False
