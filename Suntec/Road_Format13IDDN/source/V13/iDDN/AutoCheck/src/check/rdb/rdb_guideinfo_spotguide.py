# -*- coding: UTF8 -*-

import platform.TestCase
from check.rdb import rdb_common_check

class CCheckGuideInfoSpotguideInlinkidValidate(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = self.pg.GetDropFKeyStr('rdb_guideinfo_spotguidepoint_in_link_id_fkey', 'rdb_guideinfo_spotguidepoint')
        sqlcmd = sqlcmd + \
        """
        ALTER TABLE rdb_guideinfo_spotguidepoint
              ADD CONSTRAINT rdb_guideinfo_spotguidepoint_in_link_id_fkey FOREIGN KEY (in_link_id)
                  REFERENCES rdb_link (link_id) MATCH FULL
                  ON UPDATE NO ACTION ON DELETE NO ACTION;
        """
        
        if self.pg.execute(sqlcmd) == -1:
            return False
        else :
            return True
    
class CCheckGuideInfoSpotguideNodeidValidate(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = self.pg.GetDropFKeyStr('rdb_guideinfo_spotguidepoint_node_id_fkey', 'rdb_guideinfo_spotguidepoint')
        sqlcmd = sqlcmd + \
        """
        ALTER TABLE rdb_guideinfo_spotguidepoint
          ADD CONSTRAINT rdb_guideinfo_spotguidepoint_node_id_fkey FOREIGN KEY (node_id)
              REFERENCES rdb_node (node_id) MATCH FULL
              ON UPDATE NO ACTION ON DELETE NO ACTION;
        """
        
        if self.pg.execute(sqlcmd) == -1:
            return False
        else :
            return True

class CCheckGuideInfoSpotguideOutlinkidValidate(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = self.pg.GetDropFKeyStr('rdb_guideinfo_spotguidepoint_out_link_id_fkey', 'rdb_guideinfo_spotguidepoint')
        sqlcmd = sqlcmd + \
        """
        ALTER TABLE rdb_guideinfo_spotguidepoint
          ADD CONSTRAINT rdb_guideinfo_spotguidepoint_out_link_id_fkey FOREIGN KEY (out_link_id)
              REFERENCES rdb_link (link_id) MATCH FULL
              ON UPDATE NO ACTION ON DELETE NO ACTION;
        """
        
        if self.pg.execute(sqlcmd) == -1:
            return False
        else:
            return True

class CCheckGuideInfoSpotguidePatternidValidate(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = self.pg.GetDropFKeyStr('rdb_guideinfo_spotguidepoint_pattern_id_fkey', 'rdb_guideinfo_spotguidepoint')
        sqlcmd = sqlcmd + \
        """
        ALTER TABLE rdb_guideinfo_spotguidepoint
          ADD CONSTRAINT rdb_guideinfo_spotguidepoint_pattern_id_fkey FOREIGN KEY (pattern_id)
              REFERENCES rdb_guideinfo_pic_blob_bytea (gid) MATCH FULL
              ON UPDATE NO ACTION ON DELETE NO ACTION;
        """
        
        if self.pg.execute(sqlcmd) == -1:
            return False
        else:
            return True

class CCheckGuideInfoSpotguideArrowidValidate(platform.TestCase.CTestCase):
    def _do(self):
        
        # # 判断所有的Arrow_id是否都为 0
        sqlcmd = """
            SELECT count(gid)
              FROM rdb_guideinfo_spotguidepoint
              where arrow_id <> 0;
        """
        self.pg.query(sqlcmd)
        row = self.pg.fetchone()
        if row[0] == 0:
            self.logger.warning('All arrow id are 0.')
            return True
        
        sqlcmd = self.pg.GetDropFKeyStr('rdb_guideinfo_spotguidepoint_arrow_id_fkey', 'rdb_guideinfo_spotguidepoint')
        sqlcmd = sqlcmd + \
        """
        ALTER TABLE rdb_guideinfo_spotguidepoint
          ADD CONSTRAINT rdb_guideinfo_spotguidepoint_arrow_id_fkey FOREIGN KEY (arrow_id)
              REFERENCES rdb_guideinfo_pic_blob_bytea (gid) MATCH FULL
              ON UPDATE NO ACTION ON DELETE NO ACTION;
        """
        
        if self.pg.execute(sqlcmd) == -1:
            return False
        else:
            return True
        
class CCheckGuideInfoSpotguideTypeValidate(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        ALTER TABLE rdb_guideinfo_spotguidepoint DROP CONSTRAINT if exists check_guide_type;
                ALTER TABLE rdb_guideinfo_spotguidepoint
                  ADD CONSTRAINT check_guide_type CHECK (type in (1,2,3,4,5,6, 7,8, 9,10));
        """
        if self.pg.execute(sqlcmd) == -1:
            return False
        else:
            return True

class CCheckGuideSpotguideInlinkTileSame(platform.TestCase.CTestCase):
    def _do(self):
        checkobject = rdb_common_check.\
        CCheckItemTileIDSame(self.pg, 'rdb_guideinfo_spotguidepoint', 'in_link_id', 'in_link_id_t')
        return checkobject.do()
        
class CCheckGuideSpotguideNodeTileSame(platform.TestCase.CTestCase):
    
    def _do(self):
        checkobject = rdb_common_check.\
        CCheckItemTileIDSame(self.pg, 'rdb_guideinfo_spotguidepoint', 'node_id', 'node_id_t')
        return checkobject.do()
    
class CCheckGuideSpotguideOutlinkTileSame(platform.TestCase.CTestCase):
    def _do(self):
        checkobject = rdb_common_check.\
        CCheckItemTileIDSame(self.pg, 'rdb_guideinfo_spotguidepoint', 'out_link_id', 'out_link_id_t')
        return checkobject.do()

class CCheckGuideSpotguidePasslinkCnt(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
            SELECT count(*)
            FROM rdb_guideinfo_spotguidepoint;
        """
        self.pg.execute(sqlcmd)
        row1 = self.pg.fetchone()

        sqlcmd = """
            SELECT count(*)
            FROM spotguide_tbl;
        """
        self.pg.execute(sqlcmd)
        row2 = self.pg.fetchone()
        if row2[0] <> 0 and row1[0] == 0:
            return False
        return True

class CCheckGuideSpotguidePasslinkCnt_special(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
            SELECT count(*)
            FROM rdb_guideinfo_spotguidepoint;
        """
        self.pg.execute(sqlcmd)
        row1 = self.pg.fetchone()

        sqlcmd = """
            SELECT count(*)
            FROM spotguide_tbl;
        """
        self.pg.execute(sqlcmd)
        row2 = self.pg.fetchone()
        if row1[0] < row2[0] - 50:
            return False
        return True

class CCheckGuideSpotguideInLinkOutLinkEqual(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*) from rdb_guideinfo_spotguidepoint where in_link_id = out_link_id
        """
        return 0 == self.pg.getOnlyQueryResult(sqlcmd)
    
class CCheckGuideSpotguideExtendFlag(platform.TestCase.CTestCase):
    def _do(self):
        checkobject = rdb_common_check.\
        CCheckNodeExtendFlag(self.pg, 'rdb_guideinfo_spotguidepoint', 4)
        return checkobject.do()
    






