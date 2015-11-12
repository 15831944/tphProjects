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

# spotguide类型和高速相关的时候，其inlink或outlink必须有一条是高速道路
# spotguide高速相关type: 1,2,3,6,8,11
# 高速路road_type: 0,1
class CCheckGuideSpotguideInlinkRoadType_Highway(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd='''
                select count(*) from 
                rdb_guideinfo_spotguidepoint as a
                left join rdb_link as b
                on a.in_link_id=b.link_id and a.in_link_id_t=b.link_id_t
                left join rdb_link as c
                on a.out_link_id=b.link_id and a.out_link_id_t=b.link_id_t
                where (a.type in(1,2,3,6,8,11) 
                       and b.road_type not in(0,1)
                       and c.road_type not in(0,1));
               '''
        cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return cnt == 0  
    
# spotguide类型是普通路口的时候，其inlink必须是非高速
# 12: 收费站不进行check。
class CCheckGuideSpotguideInlinkRoadType_NotHighway(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd='''
                select count(*) from 
                rdb_guideinfo_spotguidepoint as a
                left join rdb_link as b
                on a.in_link_id=b.link_id and a.in_link_id_t=b.link_id_t
                where a.type in (4,5,7,9,10) and b.road_type in (0,1) and b.link_type<>5;
               '''
        cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return cnt == 0 

# 要读懂此case请先理解插图数据的dat作成协议。
# 若某个仕向地有白天黑夜图，则在制作dat时，需要将黑夜白天的信息正确填入dat头中，以便机能组能正确地
# 识别这张图片是黑夜/白天图。
# dat作成协议为dat中的每个图片各分配了2个bit来标记其白天黑夜信息。
# 当图片不需要区分白天黑夜时，填入0(00).
# 当图片是白天图时，填入1(01).
# 当图片是黑夜图时，填入2(10).
# 当图片是傍晚图时，填入3(11).
# 以下仕向地有白天黑夜图：
# Here： 中东，东南亚，巴西，阿根廷
# MMi： 印度
# Sensis：澳大利亚
class CCheckGuideSpotguideIllustDayNightFlag(platform.TestCase.CTestCase):
    def _parseDayNightFlag(self, readBuf):
        import struct
        buf6 = readBuf[0:4]
        hFEFE, nPicCount = struct.unpack("<HH", buf6)
        
        dayNightFlagList = []
        for iPic in range(0, nPicCount):
            buf9 = readBuf[4+9*iPic: 4+9*iPic+9]
            bPicInfo, iOffset, iSize = struct.unpack("<bii", buf9)
            dayNightFlagList.append(bPicInfo & 3)
        return dayNightFlagList
    # 找出所有rdb_guideinfo_spotguidepoint表中pattern_id对应的二进制文件，检查它们的白天黑夜bit位。
    def _do(self):
        sqlcmd='''
select a.type, b.gid, b.data from
(
    select type, array_agg(distinct pattern_id) as pattern_id_list
    from rdb_guideinfo_spotguidepoint
    where type<>12
    group by type
) as a
left join 
rdb_guideinfo_pic_blob_bytea as b
on a.pattern_id_list[1]=b.gid or a.pattern_id_list[array_upper(a.pattern_id_list, 1)]=b.gid;
               '''
        self.pg.execute(sqlcmd)
        rows = self.pg.fetchall()
        for row in rows:
            dayNightFlagList = self._parseDayNightFlag(row[2])
            for oneDayNightFlag in dayNightFlagList:
                if oneDayNightFlag == 0 or oneDayNightFlag == 2: # 这些仕向地只应有一张白天图。
                    return False
        return True

















