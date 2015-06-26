# -*- coding: UTF8 -*-
'''
Created on 2011-12-22

@author: liuxinxing
'''

import platform.TestCase

class CCheckTableStruct(platform.TestCase.CTestCase):
    def _do(self):
        return self.pg.IsExistTable('rdb_linklane_info_client')

class CCheckLaneNumber(platform.TestCase.CTestCase):
    def _do(self):
        srv_count = self.pg.getOnlyQueryResult("select count(lane_id) from rdb_linklane_info")
        clt_count = self.pg.getOnlyQueryResult("select count(lane_id) from rdb_linklane_info_client")
        return (srv_count == clt_count)

class CCheckLaneID(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.lane_id)
                    from rdb_linklane_info_client as a
                    left join rdb_linklane_info as b
                    on a.lane_id = b.lane_id
                    where b.lane_id is null
                 """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)

class CCheckAttr1(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.lane_id)
--                    select a.multi_lane_attribute_1, b.disobey_flag, b.lane_dir, b.lane_up_down_distingush
                    from rdb_linklane_info_client as a
                    left join rdb_linklane_info as b
                    on a.lane_id = b.lane_id
                    where not
                          (
                          ((a.multi_lane_attribute_1 & 1) = cast(b.disobey_flag as integer))
                          and
                          (((a.multi_lane_attribute_1 >> 1) & ((1<<2)-1)) = b.lane_dir)
                          and
                          (((a.multi_lane_attribute_1 >> 3) & 1) = cast(b.lane_up_down_distingush as integer))
                          )
                 """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)

class CCheckAttr2(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.lane_id)
--                    select a.multi_lane_attribute_2, b.ops_lane_number, b.ops_width, b.neg_lane_number, b.neg_width
                    from rdb_linklane_info_client as a
                    left join rdb_linklane_info as b
                    on a.lane_id = b.lane_id
                    where not
                          (
                          (((a.multi_lane_attribute_2 >> 0) & ((1<<4)-1)) = b.ops_lane_number)
                          and
                          (((a.multi_lane_attribute_2 >> 4) & ((1<<4)-1)) = b.ops_width)
                          and
                          (((a.multi_lane_attribute_2 >> 8) & ((1<<4)-1)) = b.neg_lane_number)
                          and
                          (((a.multi_lane_attribute_2 >> 12) & ((1<<4)-1)) = b.neg_width)
                          )
                 """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)

