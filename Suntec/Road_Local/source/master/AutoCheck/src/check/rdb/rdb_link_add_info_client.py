# -*- coding: UTF8 -*-
'''
Created on 2011-12-22

@author: liuxinxing
'''

import platform.TestCase

class CCheckTableStruct(platform.TestCase.CTestCase):
    def _do(self):
        return self.pg.IsExistTable('rdb_link_add_info_client')

class CCheckLinkIDEffective(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """

                    select count(a.link_id)
                    from rdb_link_add_info_client as a
                    left join rdb_link_client as b
                    on a.link_id = (b.link_id & ((1::bigint << 32) - 1))  and a.link_id_t = b.link_id_t
                    where b.link_id is null

                 """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)

class CCheckNumber(platform.TestCase.CTestCase):
    def _do(self):
        srv_count = self.pg.getOnlyQueryResult("select count(link_id) from rdb_link_add_info")
        clt_count = self.pg.getOnlyQueryResult("select count(link_id) from rdb_link_add_info_client")
        return (srv_count == clt_count)

class CCheckLinkIDSameToServer(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.link_id)
                    from rdb_link_add_info_client as a
                    left join rdb_link_add_info as b
                    on (a.link_id = b.link_id & ((cast(1 as bigint) << 32) - 1)) and  a.link_id_t = b.link_id_t
                    where b.link_id is null
                 """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)

class CCheckTileID(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.link_id)
                    from rdb_link_add_info_client as a
                    left join rdb_link_add_info as b
                    on a.link_id = (b.link_id & ((cast(1 as bigint) << 32) - 1)) and a.link_id_t = b.link_id_t
                    where a.link_id_t != b.link_id_t
                 """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)

class CCheckMultiLinkAddInfo(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.link_id)
--                    select a.multi_link_add_info, b.struct_code, b.shortcut_code, b.parking_flag, b.etc_lane_flag
                    from rdb_link_add_info_client as a
                    left join rdb_link_add_info as b
                    on a.link_id = (b.link_id & ((cast(1 as bigint) << 32) - 1)) and a.link_id_t = b.link_id_t
                    where not
                          (
                          (((a.multi_link_add_info >> 0) & ((1<<4)-1)) = b.struct_code)
                          and
                          (((a.multi_link_add_info >> 4) & ((1<<2)-1)) = b.shortcut_code)
                          and
                          (((a.multi_link_add_info >> 6) & ((1<<2)-1)) = b.parking_flag)
                          and
                          (((a.multi_link_add_info >> 8) & ((1<<2)-1)) = b.etc_lane_flag)
                          )
                 """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)

