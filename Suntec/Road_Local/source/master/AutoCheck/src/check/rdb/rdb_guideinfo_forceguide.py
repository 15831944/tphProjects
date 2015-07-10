# -*- coding: UTF8 -*-
'''
Created on 2012-12-12

@author: liuxinxing
'''

import platform.TestCase

class CCheckTableStruct(platform.TestCase.CTestCase):
    def _do(self):
        return self.pg.IsExistTable('rdb_guideinfo_forceguide')

class CCheckInlinkID(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.in_link_id)
                    from rdb_guideinfo_forceguide as a
                    left join rdb_link as b
                    on a.in_link_id = b.link_id
                    where b.link_id is null
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckOutlinkID(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.out_link_id)
                    from rdb_guideinfo_forceguide as a
                    left join rdb_link as b
                    on a.out_link_id = b.link_id
                    where b.link_id is null
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckNodeID(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.node_id)
                    from rdb_guideinfo_forceguide as a
                    left join rdb_node as b
                    on a.node_id = b.node_id
                    where b.node_id is null
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckGuideType(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(guideinfo_id)
                from rdb_guideinfo_forceguide
                where not (guide_type >= 0 and guide_type <= 13)
                  and not (guide_type = 255)
                  and not (guide_type >= 32 and guide_type <= 43)
                  and not (guide_type >= 48 and guide_type <= 59)
                  and not (guide_type >= 64 and guide_type <= 75)
                  and not (guide_type >= 80 and guide_type <= 248)
                """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckPositionType(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(guideinfo_id)
                from rdb_guideinfo_forceguide
                where not (position_type >= 0 and position_type <= 9)
                """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckPassLinkCount(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(guideinfo_id)
                from rdb_guideinfo_forceguide
                where not (passlink_count >= 0 and passlink_count <= 20)
                """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckInlinkTileID(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.guideinfo_id)
                    from rdb_guideinfo_forceguide as a
                    left join rdb_link as b
                    on a.in_link_id = b.link_id
                    where a.in_link_id_t != b.link_id_t
                """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckOutlinkTileID(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.guideinfo_id)
                    from rdb_guideinfo_forceguide as a
                    left join rdb_link as b
                    on a.out_link_id = b.link_id
                    where a.out_link_id_t != b.link_id_t
                """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckNodeTileID(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.guideinfo_id)
                    from rdb_guideinfo_forceguide as a
                    left join rdb_node as b
                    on a.node_id = b.node_id
                    where a.node_id_t != b.node_id_t
                """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckInOutLinkDifferent(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(guideinfo_id)
                    from rdb_guideinfo_forceguide
                    where in_link_id = out_link_id
                """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckLinkrowContinuous(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.guideinfo_id)
                    from rdb_guideinfo_forceguide as a
                    left join rdb_link as inlink
                    on a.in_link_id = inlink.link_id
                    left join rdb_link as outlink
                    on a.out_link_id = outlink.link_id
                    where (not a.node_id in (inlink.start_node_id, inlink.end_node_id))
                       or (a.passlink_count = 0 and not a.node_id in (outlink.start_node_id, outlink.end_node_id))
                       or (a.node_id = inlink.start_node_id and inlink.one_way in (2,4))
                       or (a.node_id = inlink.end_node_id and inlink.one_way in (3,4))
                       or (a.node_id = outlink.start_node_id and outlink.one_way in (3,4))
                       or (a.node_id = outlink.end_node_id and outlink.one_way in (2,4))
                """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckNodeFlag(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.guideinfo_id)
                    from rdb_guideinfo_forceguide as a
                    left join rdb_node as b
                    on a.node_id = b.node_id
                    where b.extend_flag & (1 << 15) > 0
                """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        if count_rec > 0:
            return False
        
        sqlcmd = """
                    select count(a.guideinfo_id)
                    from (select distinct node_id from rdb_node where extend_flag & (1 << 15) > 0)as a
                    left join rdb_guideinfo_forceguide as b
                    on a.node_id = b.node_id
                    where b.node_id is null
                """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        if count_rec > 0:
            return False
        
        return True

class CCheckRecordNumber(platform.TestCase.CTestCase):
    def _do(self):
        mid_count = self.pg.getOnlyQueryResult("select count(*) from force_guide_tbl")
        rdb_count = self.pg.getOnlyQueryResult("select count(*) from rdb_guideinfo_forceguide")
        return (mid_count == rdb_count)

class CCheckNostraRecNum(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.guideinfo_id)
                    from rdb_guideinfo_forceguide
                """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec > 0)

class CCheckTarRecNum2ConfigRecNum(platform.TestCase.CTestCase):
    def _do(self):
        config_count = 0
        if self.pg.IsExistTable('temp_force_guide_patch_tbl'):
            config_count = self.pg.getOnlyQueryResult("select count(*) from temp_force_guide_patch_tbl")
            
        rdb_count = self.pg.getOnlyQueryResult("select count(*) from rdb_guideinfo_forceguide")
        return (rdb_count >= config_count)
