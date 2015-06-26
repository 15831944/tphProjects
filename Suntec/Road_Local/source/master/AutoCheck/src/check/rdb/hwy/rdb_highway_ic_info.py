# -*- coding: UTF8 -*-
'''
Created on 2014-12-18

@author: hcz
'''
import platform.TestCase


# =============================================================================
# CCheckICNo -- 一般设施ic_no唯一；边界设施ic_no成对。
# ==============================================================================
class CCheckICNo(platform.TestCase.CTestCase):
    def _do(self):
        # 一般设施ic_no唯一
        sqlcmd = """
        SELECT COUNT(*)
          FROM (-- normal facility
            SELECT ic_no
              FROM rdb_highway_ic_info
              WHERE boundary <> 1
              GROUP BY ic_no
              HAVING COUNT(*) > 1
            UNION
            -- Boundary
            SELECT ic_no
              FROM rdb_highway_ic_info
              WHERE boundary = 1
              GROUP BY ic_no
              HAVING COUNT(*) <> 2
          ) AS a;
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckEnterExitFlg -- 只有JCT(原始种别是IC)、Ramp、IC、SmartIC才有出/入口存在Flag
# ==============================================================================
class CCheckEnterExitFlg(platform.TestCase.CTestCase):
    def _do(self):
        # 一般设施ic_no唯一
        sqlcmd = """
        SELECT count(ic_no)
          FROM (
            SELECT ic_no
              FROM rdb_highway_ic_info
              WHERE (enter = 1 or exit = 1) and
                (tollgate = 1 or pa = 1 or sa = 1 or boundary = 1)
            UNION
            SELECT ic_no
              FROM rdb_highway_ic_info
              WHERE (enter = 1 or exit = 1) and
                not (jct = 1 or ic = 1 or ramp = 1 or smart_ic = 1)
          ) AS a;
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckEnterFlg -- 探索点里存在入口探索点
# ==============================================================================
class CCheckEnterFlg(platform.TestCase.CTestCase):
    def _do(self):
        # 一般设施ic_no唯一
        sqlcmd = """
        SELECT COUNT(*)
          FROM rdb_highway_ic_info as a
          LEFT JOIN (
              SELECT ic_no, enter_flag
                FROM rdb_highway_path_point
                where enter_flag = 1
          ) as b
          ON a.ic_no = b.ic_no
          WHERE enter = 1 and b.enter_flag is NULL;
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckExitFlg -- 探索点里存在出口探索点
# ==============================================================================
class CCheckExitFlg(platform.TestCase.CTestCase):
    def _do(self):
        # 一般设施ic_no唯一
        sqlcmd = """
        SELECT COUNT(*)
          FROM rdb_highway_ic_info as a
          LEFT JOIN (
              SELECT ic_no, enter_flag
                FROM rdb_highway_path_point
                where exit_flag = 1
          ) as b
          ON a.ic_no = b.ic_no
          WHERE exit = 1 and b.enter_flag is NULL;
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckTollCount -- 收费站数
# ==============================================================================
class CCheckTollCount(platform.TestCase.CTestCase):
    def _do(self):
        # 一般设施ic_no唯一
        sqlcmd = """
        SELECT count(*)
          FROM rdb_highway_ic_info as a
          LEFT JOIN (
            -- Get toll number of IC
            SELECT ic_no, count(ic_no) as toll_count
              FROM rdb_highway_toll_info
              WHERE conn_ic_no is null
              GROUP BY ic_no
          ) as b
          ON a.ic_no = b.ic_no
          WHERE a.toll_count > 0 and
                a.toll_count <> b.toll_count and
                b.toll_count is not null;
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckPathPointCount -- 探索点
# ==============================================================================
class CCheckPathPointCount(platform.TestCase.CTestCase):
    def _do(self):
        # 一般设施ic_no唯一
        sqlcmd = """
        SELECT count(*)
          FROM (
            SELECT ic_no, sum(path_count) as path_count
              FROM rdb_highway_ic_info
              GROUP BY ic_no
          ) as a
          LEFT JOIN (
            SELECT ic_no, count(ic_no) as path_count
              FROM rdb_highway_path_point
              GROUP BY ic_no
          ) as b
          ON a.ic_no = b.ic_no
          WHERE a.path_count <> b.path_count and
                b.path_count is not null;
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckConnCount -- 接续情报
# ==============================================================================
class CCheckConnCount(platform.TestCase.CTestCase):
    def _do(self):
        # 一般设施ic_no唯一
        sqlcmd = """
        SELECT count(*)
          FROM rdb_highway_ic_info as a
          LEFT JOIN (
            SELECT ic_no, count(ic_no) as conn_count
              FROM rdb_highway_conn_info
              GROUP BY ic_no
          ) as b
          ON a.ic_no = b.ic_no
          WHERE boundary <> 1 and
                a.conn_count <> b.conn_count and
                b.conn_count is not null;
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckConnTileId -- 接续情报
# ==============================================================================
class CCheckConnTileId(platform.TestCase.CTestCase):
    def _do(self):
        # boundary设施，conn_tile_id是另个边界的tile_id
        sqlcmd = """
        SELECT COUNT(*)
          FROM (
            SELECT array_agg(tile_id) as tile_ids,
                   array_agg(conn_tile_id) as conn_tile_ids
              FROM rdb_highway_ic_info
              WHERE boundary = 1
              GROUP BY ic_no
          ) AS a
          where tile_ids[1] = tile_ids[2] or
                conn_tile_ids[1] = conn_tile_ids[2] or
                tile_ids[1] <> conn_tile_ids[2] or
                tile_ids[2] <> conn_tile_ids[1];
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckTollIndex -- 等于该设施第一个料金情报的index
# ==============================================================================
class CCheckTollIndex(platform.TestCase.CTestCase):
    def _do(self):
        # boundary设施，conn_tile_id是另个边界的tile_id
        sqlcmd = """
        SELECT count(*)
          FROM rdb_highway_ic_info as a
          LEFT JOIN (
            SELECT ic_no, min(tile_index) as min_tile_index
              FROM rdb_highway_toll_info
              GROUP BY ic_no
          ) as b
          ON a.ic_no = b.ic_no
          WHERE toll_count > 0 and
                (toll_index <> min_tile_index or min_tile_index is null);
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckRoadNo
# ==============================================================================
class CCheckRoadNo(platform.TestCase.CTestCase):
    def _do(self):
        # boundary设施，conn_tile_id是另个边界的tile_id
        sqlcmd = """
        SELECT count(*)
          FROM rdb_highway_ic_info as ic
          LEFT JOIN (
            SELECT ic_no, road_no
              FROM rdb_highway_path_point as a
              LEFT JOIN rdb_highway_mapping as b
              ON a.link_id = b.link_id
              WHERE center_flag = 1
            union
            SELECT ic_no, road_no
              FROM rdb_highway_path_point as a
              LEFT JOIN rdb_highway_mapping as b
              ON a.link_id = b.link_id
              WHERE main_flag = 1
          ) as c
          ON ic.ic_no = c.ic_no
          WHERE ic.road_no <> c.road_no or c.road_no is null;
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckRoadIndex
# ==============================================================================
class CCheckRoadIndex(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT count(*)
          FROM rdb_highway_ic_info as ic
          LEFT JOIN rdb_highway_road_info AS road
          ON ic.tile_id = road.tile_id and
             ic.up_down = road.up_down and
             ic.road_no = road.road_no
          WHERE ic.road_index <> road.tile_index or
                road.tile_index is null;
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
          FROM rdb_highway_ic_info as ic
          LEFT JOIN (
            SELECT ic_no, COUNT(*) AS illust_count
              FROM rdb_highway_illust_info
              GROUP BY ic_no
          ) as illust
          ON ic.ic_no = illust.ic_no
          WHERE ic.illust_count > 0 and
                (ic.illust_count <> illust.illust_count or
                 illust.illust_count is null);
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckStoreCount
# ==============================================================================
class CCheckStoreCount(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT count(*)
          FROM rdb_highway_ic_info as ic
          LEFT JOIN (
            SELECT ic_no, count(*) as store_count
              FROM (
                SELECT distinct ic_no, store_kind
                  FROM rdb_highway_store_info
              ) AS a
              group by ic_no
          ) as store
          ON ic.ic_no = store.ic_no
          WHERE ic.store_count > 0 and
                (ic.store_count <> store.store_count or
                 store.store_count is null);
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckServiseFlag
# ==============================================================================
class CCheckServiseFlag(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT count(*)
          FROM rdb_highway_ic_info as ic
          WHERE ic.servise_flag <> 1 and ic_no in (
            SELECT ic_no
              FROM rdb_highway_service_info
            )
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckICOrder
# ==============================================================================
class CCheckICOrder(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT road_no, up_down,
               min(tile_id) as tile_id, count(ic_no) as ic_num
          FROM rdb_highway_ic_info
          group by road_no, up_down
          order by road_no, up_down
        """
        ic_mng = self.load_ic()
        for road in self.pg.get_batch_data(sqlcmd):
            road_no = road[0]
            up_down = road[1]
            tile_id = road[2]
            ic_num = road[3]
            # 取得当road的所有设施(包括其他tile)
            all_ic = ic_mng.get_all_ic(tile_id, up_down, road_no)
            if len(all_ic) != ic_num:
                return False
            if not self.check_forward_reverse_flag(all_ic):
                return False
        return True

    def check_forward_reverse_flag(self, all_ic):
        '''并设Flag.'''
        same_flg = False
        prev_is_boundary = False
        for ic in all_ic:
            forward_flag = ic.get('forward_flag')
            reverse_flag = ic.get('reverse_flag')
            boundary = ic.get("boundary")
            if prev_is_boundary and not boundary:
                return False
            if not same_flg:
                if (reverse_flag, forward_flag) == (0, 1):
                    same_flg = True  # 并设开始
                    if boundary:
                        return False
            else:
                if (reverse_flag, forward_flag) == (1, 1):
                    if boundary:
                        return False
                    else:
                        continue
                else:
                    if (reverse_flag, forward_flag) == (1, 0):
                        if not boundary:
                            same_flg = False  # 并设结束
                        else:
                            if prev_is_boundary:
                                return False
                            else:
                                prev_is_boundary = True
                        continue
                    if (reverse_flag, forward_flag) == (0, 1):
                        if not boundary:
                            return False
                        else:
                            if prev_is_boundary:
                                prev_is_boundary = False
                            else:
                                return False
        return True

    def load_ic(self):
        sqlcmd = """
         SELECT ic_no, forward_flag, reverse_flag,
                road_no, up_down, road_index,
                conn_tile_id, tile_id, index,
                boundary
          FROM rdb_highway_ic_info
          order by tile_id, index;
        """
        ic_mng = CICMng()
        for ic_info in self.pg.get_batch_data(sqlcmd):
            ic = {"ic_no": ic_info[0],
                  "forward_flag": ic_info[1],
                  "reverse_flag": ic_info[2],
                  "road_no": ic_info[3],
                  "up_down": ic_info[4],
                  "road_index": ic_info[5],
                  "conn_tile_id": ic_info[6],
                  "tile_id": ic_info[7],
                  "tile_index": ic_info[8],
                  "boundary": ic_info[9],
                  }
            ic_mng.add_ic(ic)
        return ic_mng


# =============================================================================
# CCheckFicilityType
# ==============================================================================
class CCheckFicilityType(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT count(*)
          FROM rdb_highway_ic_info
          where enter not in (0, 1) or exit not in (0, 1) or
                tollgate not in (0, 1) or jct not in (0, 1) or
                pa not in (0, 1) or sa not in (0, 1) or
                ic not in (0, 1) or ramp not in (0, 1) or
                smart_ic not in (0, 1) or boundary not in (0, 1) or
                enter not in (0, 1) or exit not in (0, 1) or
                barrier not in (0, 1) or dummy not in (0, 1) or
                new not in (0, 1) or unopen not in (0, 1)
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if not row and row[0] != 0:
            return False

        sqlcmd = """
        SELECT count(*)
          FROM rdb_highway_ic_info
          where enter = 0 and exit = 0 and
                tollgate = 0 and jct = 0 and
                pa = 0 and sa = 0 and
                ic = 0 and ramp = 0 and
                smart_ic = 0 and boundary = 0;
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if not row and row[0] != 0:
            return False
        return True


# =============================================================================
# CCheckUpDown -- 0…下り、1…上り
# ==============================================================================
class CCheckUpDown(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT count(ic_no)
          FROM rdb_highway_ic_info
          where up_down not in (0, 1);
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False


# =============================================================================
# CICMng
# ==============================================================================
class CICMng(object):
    def __init__(self):
        self.ic_map = {}

    def add_ic(self, ic):
        tile_id = ic.get('tile_id')
        if tile_id in self.ic_map:
            ic_list = self.ic_map.get(tile_id)
        else:
            ic_list = []
        tile_index = ic.get('tile_index')
        ic_list.insert(tile_index, ic)
        self.ic_map[tile_id] = ic_list

    def get_ic(self, tile_id, ic_no):
        ic_list = self.ic_map.get(tile_id)
        for ic in ic_list:
            if ic.get('ic_no') == ic_no:
                return ic
        return None

    def get_prev_ic(self, ic, next_ic=None):
        tile_id = ic.get('tile_id')
        ic_list = self.ic_map.get(tile_id)
        if ic_list:
            tile_index = ic.get('tile_index')
            ic_no = ic.get('ic_no')
            if ic.get('boundary') and not self.is_last_ic(tile_id, ic):
                if not self.is_same_ic_no(ic, next_ic):
                    conn_tile_id = ic.get('conn_tile_id')
                    prev_ic = self.get_ic(conn_tile_id, ic_no)
                    if self.is_same_road(ic, prev_ic):
                        return prev_ic
                    else:
                        return False
            if tile_index > 0:
                prev_ic = ic_list[tile_index - 1]
                if self.is_same_road(ic, prev_ic):
                    return prev_ic
        return None

    def is_same_road(self, ic1, ic2):
        if(ic1.get('road_no') == ic2.get('road_no') and
           ic1.get('up_down') == ic2.get('up_down')):
            return True
        return False

    def is_same_ic_no(self, ic1, ic2):
        if not ic1 or not ic2:
            return False
        if ic1.get('ic_no') == ic2.get('ic_no'):
            return True
        return False

    def get_next_ic(self, ic, prev_ic=None):
        tile_id = ic.get('tile_id')
        ic_list = self.ic_map.get(tile_id)
        if ic_list:
            ic_no = ic.get('ic_no')
            if ic.get('boundary') and not self.is_first_ic(tile_id, ic):
                if not self.is_same_ic_no(ic, prev_ic):
                    conn_tile_id = ic.get('conn_tile_id')
                    next_ic = self.get_ic(conn_tile_id, ic_no)
                    if self.is_same_road(ic, next_ic):
                        return next_ic
                    else:
                        return None
            tile_index = ic.get('tile_index')
            if tile_index + 1 < len(ic_list):
                next_ic = ic_list[tile_index + 1]
                if self.is_same_road(ic, next_ic):
                    return next_ic
        return None

    def is_last_ic(self, tile_id, ic):
        ic_list = self.ic_map.get(tile_id)
        if ic_list:
            tile_index = ic.get('tile_index')
            if tile_index == len(ic_list) - 1:
                return True
            else:
                next_ic = ic_list[tile_index + 1]
                if not self.is_same_road(ic, next_ic):
                    return True
        return False

    def is_first_ic(self, tile_id, ic):
        ic_list = self.ic_map.get(tile_id)
        if ic_list:
            tile_index = ic.get('tile_index')
            if tile_index == 0:
                return True
            else:
                prev_ic = ic_list[tile_index - 1]
                if not self.is_same_road(ic, prev_ic):
                    return True
        return False

    def get_all_ic(self, tile_id, up_down, road_no):
        '''Get All IC of road_no.'''
        all_ic = []
        ic = self.get_start_ic(tile_id, up_down, road_no)
        if ic:
            all_ic.append(ic)
        else:
            return all_ic
        curr_ic = ic
        next_ic = None
        while curr_ic:
            curr_ic = self.get_prev_ic(curr_ic, next_ic)
            if curr_ic:
                all_ic.insert(0, curr_ic)
                next_ic = all_ic[1]
        curr_ic = ic
        prev_ic = None
        while curr_ic:
            curr_ic = self.get_next_ic(curr_ic, prev_ic)
            if curr_ic:
                all_ic.append(curr_ic)
                prev_ic = all_ic[-2]
        return all_ic

    def get_start_ic(self, tile_id, up_down, road_no):
        ic_list = self.ic_map.get(tile_id)
        for ic in ic_list:
            if(ic.get('up_down') == up_down and
               ic.get('road_no') == road_no):
                return ic
        return None
