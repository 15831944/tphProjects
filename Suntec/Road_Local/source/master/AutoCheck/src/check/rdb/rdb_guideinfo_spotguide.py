# -*- coding: UTF8 -*-

import platform.TestCase
from check.rdb import rdb_common_check

# in_link_id建立外键依赖于rdb_link.link_id，保证合法性。
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

# node_id建立外键依赖于rdb_node.node_id，保证其合法性
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

# out_link_id建立外键依赖于rdb_link.link_id，保证合法性。
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

# pattern_id建立外键依赖于rdb_guideinfo_pic_blob_bytea.gid，保证其合法性
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

# 检查arrow_id的合法性，如果此值不为0，则必须能在rdb_guideinfo_pic_blob_bytea表中找到与之相关联的项。
class CCheckGuideInfoSpotguideArrowidValidate(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
            select count(*) from 
            rdb_guideinfo_spotguidepoint as a
            left join rdb_guideinfo_pic_blob_bytea as b
            on a.arrow_id=b.gid
            where a.arrow_id<>0 and b.gid is null            
        """
        return self.pg.getOnlyQueryResult(sqlcmd) == 0

# type值必须在rdb协议所规定的类型列表之中   
class CCheckGuideInfoSpotguideTypeValidate(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        ALTER TABLE rdb_guideinfo_spotguidepoint DROP CONSTRAINT if exists check_guide_type;
                ALTER TABLE rdb_guideinfo_spotguidepoint
                  ADD CONSTRAINT check_guide_type CHECK (type in (1,2,3,4,5,6,7,8,9,10,12));
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
        # 当rdb表里的数据与中间表的数据差距超过50条时，报错。
        # 此情形对应的实际情况是未能找到illust图的条数超过： 50。
        if row1[0] < row2[0] - 50:
            return False
        return True

class CCheckGuideSpotguideInLinkOutLinkEqual(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*) from rdb_guideinfo_spotguidepoint where in_link_id = out_link_id
        """
        return 0 == self.pg.getOnlyQueryResult(sqlcmd)

# 每个诱导点对应的rdb_node.extend_flag的第4位必须被正确置为1   
class CCheckGuideSpotguideExtendFlag(platform.TestCase.CTestCase):
    def _do(self):
        checkobject = rdb_common_check.\
        CCheckNodeExtendFlag(self.pg, 'rdb_guideinfo_spotguidepoint', 4)
        return checkobject.do()

# 收费站诱导点对应的rdb_node.extend_flag的第10位必须被正确置为1
class CCheckGuideSpotguideTollFlagMustBe_1_(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
            select count(*) from
            (
                select a.node_id, b.extend_flag 
                from rdb_guideinfo_spotguidepoint as a 
                left join rdb_node as b
                on a.node_id=b.node_id and a.type=12
            ) as c 
            where (c.extend_flag & (1 << 10)) = 0
        """
        return 0 == self.pg.getOnlyQueryResult(sqlcmd)

# spotguide点必须是分叉点，toll station点除外。
class CCheckGuideSpotguidePointMustBeBifurcation(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd='''
            select count(1) 
            from rdb_guideinfo_spotguidepoint as a 
            join rdb_node as b 
            on a.node_id=b.node_id 
            where array_upper(b.branches,1)=2 and a.type<>12
               '''
        cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return cnt == 0     

#
class CCheckGuideSpotguidePointList(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd='''
            
               '''
        cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return cnt == 0  



























