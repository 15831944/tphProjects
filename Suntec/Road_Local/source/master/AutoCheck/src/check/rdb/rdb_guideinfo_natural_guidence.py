# -*- coding: UTF8 -*-

import platform.TestCase
from check.rdb import rdb_common_check

class CCheckInlinkidValidate(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from rdb_guideinfo_natural_guidence as a
                left join rdb_link as b
                on a.in_link_id = b.link_id
                where b.link_id is null
                """
        nRecCount = self.pg.getOnlyQueryResult(sqlcmd)
        return (nRecCount == 0)
    
class CCheckNodeidValidate(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from rdb_guideinfo_natural_guidence as a
                left join rdb_node as b
                on a.node_id = b.node_id
                where b.node_id is null
                """
        nRecCount = self.pg.getOnlyQueryResult(sqlcmd)
        return (nRecCount == 0)

class CCheckOutlinkidValidate(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from rdb_guideinfo_natural_guidence as a
                left join rdb_link as b
                on a.out_link_id = b.link_id
                where b.link_id is null
                """
        nRecCount = self.pg.getOnlyQueryResult(sqlcmd)
        return (nRecCount == 0)
        
class CCheckFeatPositionValidate(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from rdb_guideinfo_natural_guidence
                where feat_position not in (0,1,2,3)
                """
        nRecCount = self.pg.getOnlyQueryResult(sqlcmd)
        return (nRecCount == 0)
        
class CCheckFeatImportanceValidate(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from rdb_guideinfo_natural_guidence
                where not (feat_importance >= 0 and feat_importance <= 10)
                """
        nRecCount = self.pg.getOnlyQueryResult(sqlcmd)
        return (nRecCount == 0)

class CCheckInlinkTileSame(platform.TestCase.CTestCase):
    def _do(self):
        checkobject = rdb_common_check.CCheckItemTileIDSame(self.pg, 'rdb_guideinfo_natural_guidence', 'in_link_id', 'in_link_id_t')
        return checkobject.do()
        
class CCheckNodeTileSame(platform.TestCase.CTestCase):
    
    def _do(self):
        checkobject = rdb_common_check.CCheckItemTileIDSame(self.pg, 'rdb_guideinfo_natural_guidence', 'node_id', 'node_id_t')
        return checkobject.do()
    
class CCheckOutlinkTileSame(platform.TestCase.CTestCase):
    def _do(self):
        checkobject = rdb_common_check.CCheckItemTileIDSame(self.pg, 'rdb_guideinfo_natural_guidence', 'out_link_id', 'out_link_id_t')
        return checkobject.do()

class CCheckInLinkOutLinkEqual(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*) from rdb_guideinfo_natural_guidence where in_link_id = out_link_id
        """
        return 0 == self.pg.getOnlyQueryResult(sqlcmd)

class CCheckExtendFlag(platform.TestCase.CTestCase):
    def _do(self):
        checkobject = rdb_common_check.CCheckNodeExtendFlag(self.pg, 'rdb_guideinfo_natural_guidence', 17)
        return checkobject.do()

class CCheckFeatName(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from rdb_guideinfo_natural_guidence
                where feat_name is null or feat_name not like '%"tts_type": "phoneme"%'
                """
        nRecCount = self.pg.getOnlyQueryResult(sqlcmd)
        return (nRecCount == 0)

class CCheckPreposition(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from rdb_guideinfo_natural_guidence
                where not (preposition_code >= 0 and preposition_code <= 255)
                """
        nRecCount = self.pg.getOnlyQueryResult(sqlcmd)
        return (nRecCount == 0)

class CCheckGuideinfoCondition(platform.TestCase.CTestCase):
    def _do(self):
        try:
            self.pg.CreateFunction_ByName('rdb_check_guideinfo_condition')
            self.pg.callproc('rdb_check_guideinfo_condition')
            return True
        except:
            self.logger.exception('fail to check...')
            return False
    






