# -*- coding: UTF8 -*-
'''
Created on 2012-12-12

@author: liuxinxing
'''

import platform.TestCase

class CCheckTableStruct(platform.TestCase.CTestCase):
    def _do(self):
        return self.pg.IsExistTable('rdb_guideinfo_forceguide_client')

class CCheckInlinkID(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.guideinfo_id)
                    from rdb_guideinfo_forceguide_client as a
                    left join rdb_guideinfo_forceguide as b
                    on a.guideinfo_id = b.guideinfo_id
                    where a.in_link_id != (b.in_link_id & ((cast(1 as bigint) << 32) - 1))
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckOutlinkID(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.guideinfo_id)
                    from rdb_guideinfo_forceguide_client as a
                    left join rdb_guideinfo_forceguide as b
                    on a.guideinfo_id = b.guideinfo_id
                    where a.out_link_id != (b.out_link_id & ((cast(1 as bigint) << 32) - 1))
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckNodeID(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.guideinfo_id)
                    from rdb_guideinfo_forceguide_client as a
                    left join rdb_guideinfo_forceguide as b
                    on a.guideinfo_id = b.guideinfo_id
                    where a.node_id != (b.node_id & ((cast(1 as bigint) << 32) - 1))
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckInlinkTileID(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.guideinfo_id)
                    from rdb_guideinfo_forceguide_client as a
                    left join rdb_guideinfo_forceguide as b
                    on a.guideinfo_id = b.guideinfo_id
                    where a.in_link_id_t != b.in_link_id_t
                """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckOutlinkTileID(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.guideinfo_id)
                    from rdb_guideinfo_forceguide_client as a
                    left join rdb_guideinfo_forceguide as b
                    on a.guideinfo_id = b.guideinfo_id
                    where a.out_link_id_t != b.out_link_id_t
                """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckNodeTileID(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.guideinfo_id)
                    from rdb_guideinfo_forceguide_client as a
                    left join rdb_guideinfo_forceguide as b
                    on a.guideinfo_id = b.guideinfo_id
                    where a.node_id_t != b.node_id_t
                """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckMultiGuideAttr(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.guideinfo_id)
                    from rdb_guideinfo_forceguide_client as a
                    left join rdb_guideinfo_forceguide as b
                    on a.guideinfo_id = b.guideinfo_id
                    where a.guide_type != b.guide_type
                       or a.position_type != b.position_type
                """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckPassLinkCount(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.guideinfo_id)
                    from rdb_guideinfo_forceguide_client as a
                    left join rdb_guideinfo_forceguide as b
                    on a.guideinfo_id = b.guideinfo_id
                    where a.passlink_count != b.passlink_count
                """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)
