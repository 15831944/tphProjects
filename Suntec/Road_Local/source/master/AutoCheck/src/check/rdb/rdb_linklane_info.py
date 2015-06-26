# -*- coding: UTF8 -*-
'''
Created on 2011-12-22

@author: liuxinxing
'''

import platform.TestCase

class CCheckTableStruct(platform.TestCase.CTestCase):
    def _do(self):
        return self.pg.IsExistTable('rdb_linklane_info')

class CCheckLaneID(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.lane_id)
                    from rdb_linklane_info as a
                    left join (select distinct lane_id from rdb_link) as b
                    on a.lane_id = b.lane_id
                    where b.lane_id is null
                 """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)

class CCheckDisobeyFlag(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(lane_id)
                    from rdb_linklane_info
                    where disobey_flag and not lane_up_down_distingush
                 """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)

class CCheckLaneDirEffective(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(lane_id)
                    from rdb_linklane_info
                    where lane_dir not in (0,1,2,3)
                 """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)

class CCheckOpsLaneNumberEffective(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(lane_id)
                    from rdb_linklane_info
                    where (lane_dir in (1,3)) and not (ops_lane_number >= 0 and ops_lane_number <= 16)
                 """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)

class CCheckOpsWidthEffective(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(lane_id)
                    from rdb_linklane_info
                    where (lane_dir in (1,3)) and ops_width not in (0,1,2,3,4)
                 """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)

class CCheckOpsWidthComplete(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.ops_width)
                    from
                    (select unnest(ARRAY[0,1,2,3]) as ops_width) as a
                    left join
                    (select distinct ops_width from rdb_linklane_info) as b
                    on a.ops_width = b.ops_width
                    where b.ops_width is null
                 """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)

class CCheckOpsWidthComplete_rdf(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(*)
                    from rdb_linklane_info
                    where ops_width <> 4;
                 """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)
    
class CCheckNegLaneNumberEffective(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(lane_id)
                    from rdb_linklane_info
                    where (lane_dir in (2,3)) and not (neg_lane_number >= 0 and neg_lane_number <= 16)
                 """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)

class CCheckNegWidthEffective(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(lane_id)
                    from rdb_linklane_info
                    where (lane_dir in (2,3)) and neg_width not in (0,1,2,3,4)
                 """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)

class CCheckNegWidthComplete(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.neg_width)
                    from
                    (select unnest(ARRAY[0,1,2,3]) as neg_width) as a
                    left join
                    (select distinct neg_width from rdb_linklane_info) as b
                    on a.neg_width = b.neg_width
                    where b.neg_width is null
                 """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)
    
class CCheckNegWidthComplete_rdf(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(*)
                    from rdb_linklane_info
                    where neg_width <> 4;
                 """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)
    
class CCheckLaneUpDownDistingush(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(lane_id)
                    from rdb_linklane_info
                    where not lane_up_down_distingush 
                          and 
                          (
                          not lane_dir in (0,3)
                          or
                          not (ops_lane_number >= 0 and neg_lane_number >= 0)
                          )
                 """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        if rec_cnt > 0:
            return False
        
        sqlcmd = """
                    select count(lane_id)
                    from rdb_linklane_info
                    where lane_up_down_distingush 
                          and not lane_dir in (1,2)
                 """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        if rec_cnt > 0:
            return False
        
        sqlcmd = """
                    select count(lane_id)
                    from rdb_linklane_info
                    where (lane_dir = 1 and not (ops_lane_number >= 0 and neg_lane_number = 0))
                       or (lane_dir = 2 and not (ops_lane_number = 0 and neg_lane_number >= 0))
                 """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        if rec_cnt > 0:
            return False
        
        return True

class CCheckLaneNumSametoGuide(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.in_link_id)
--                    select a.lane_num, a.lane_num_l, a.lane_num_r, c.lane_dir, c.ops_lane_number, c.neg_lane_number
                    from rdb_guideinfo_lane as a
                    left join rdb_link as b
                    on a.in_link_id = b.link_id
                    left join rdb_linklane_info as c
                    on b.lane_id = c.lane_id
                    where not
                            (
                                 (a.lane_num = c.ops_lane_number)
                              or (a.lane_num = c.neg_lane_number)
                              or (a.lane_num = (c.ops_lane_number + c.neg_lane_number))
                              or ((a.lane_num - a.lane_num_l - a.lane_num_r) = c.ops_lane_number)
                              or ((a.lane_num - a.lane_num_l - a.lane_num_r) = c.neg_lane_number)
                              or ((a.lane_num - a.lane_num_l - a.lane_num_r) = (c.ops_lane_number + c.neg_lane_number))
                              or ((a.lane_num + a.lane_num_l + a.lane_num_r) = c.ops_lane_number)
                              or ((a.lane_num + a.lane_num_l + a.lane_num_r) = c.neg_lane_number)
                              or ((a.lane_num + a.lane_num_l + a.lane_num_r) = (c.ops_lane_number + c.neg_lane_number))
                            )
                     and (c.ops_lane_number = 0 or c.neg_lane_number = 0)
                     and not (c.ops_lane_number = 0 and c.neg_lane_number = 0);
                 """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)

