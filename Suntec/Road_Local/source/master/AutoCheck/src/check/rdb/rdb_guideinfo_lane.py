# -*- coding: UTF8 -*-

import platform.TestCase
from check.rdb import rdb_common_check
from string import *
RIGHT_ARROW = [2, 4, 8, 2048, 4096]
LEFT_ARROW = [16, 32, 64, 128, 8192]


class CCheckGuideInfoLaneInlinkidValidate(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = self.pg.GetDropFKeyStr('rdb_guideinfo_lane_in_link_id_fkey', 'rdb_guideinfo_lane')
        sqlcmd = sqlcmd + \
        """
        ALTER TABLE rdb_guideinfo_lane
              ADD CONSTRAINT rdb_guideinfo_lane_in_link_id_fkey FOREIGN KEY (in_link_id)
                  REFERENCES rdb_link (link_id) MATCH FULL
                  ON UPDATE NO ACTION ON DELETE NO ACTION;
        """

        if self.pg.execute(sqlcmd) == -1:
            return False
        else :
            return True


class CCheckGuideInfoLaneNodeidValiadate(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = self.pg.GetDropFKeyStr('rdb_guideinfo_lane_node_id_fkey', 'rdb_guideinfo_lane')
        sqlcmd = sqlcmd + \
        """
        ALTER TABLE rdb_guideinfo_lane
          ADD CONSTRAINT rdb_guideinfo_lane_node_id_fkey FOREIGN KEY (node_id)
              REFERENCES rdb_node (node_id) MATCH FULL
              ON UPDATE NO ACTION ON DELETE NO ACTION;
        """

        if self.pg.execute(sqlcmd) == -1:
            return False
        else :
            return True


class CCheckGuideInfoLaneOutlinkidValidate(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = self.pg.GetDropFKeyStr('rdb_guideinfo_lane_out_link_id_fkey', 'rdb_guideinfo_lane')
        sqlcmd = sqlcmd + \
        """
        ALTER TABLE rdb_guideinfo_lane
          ADD CONSTRAINT rdb_guideinfo_lane_out_link_id_fkey FOREIGN KEY (out_link_id)
              REFERENCES rdb_link (link_id) MATCH FULL
              ON UPDATE NO ACTION ON DELETE NO ACTION;
        """
        if self.pg.execute(sqlcmd) == -1:
            return False
        else:
            return True


class CCheckGuideInfoLaneTotalLaneNumValidate(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        ALTER TABLE rdb_guideinfo_lane DROP CONSTRAINT if exists check_lane_num;
                ALTER TABLE rdb_guideinfo_lane
                  ADD CONSTRAINT check_lane_num CHECK (lane_num <= 16 AND lane_num >= 1);
        """
        if self.pg.execute(sqlcmd) == -1:
            return False
        else:
            return True


class CCheckGuideInfoLaneLeftChangeLane(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                ALTER TABLE rdb_guideinfo_lane DROP CONSTRAINT if exists check_lane_num_l;
                ALTER TABLE rdb_guideinfo_lane
                  ADD CONSTRAINT check_lane_num_l CHECK (lane_num_l >= (-7) AND lane_num_l <= 7);
        """
        if self.pg.execute(sqlcmd) == -1:
            return False
        else:
            return True


class CCheckGuideInfoLaneRightChangeLane(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                ALTER TABLE rdb_guideinfo_lane DROP CONSTRAINT if exists check_lane_num_r;
                ALTER TABLE rdb_guideinfo_lane
                  ADD CONSTRAINT check_lane_num_r CHECK (lane_num_r >= (-7) AND lane_num_r <= 7);
        """
        if self.pg.execute(sqlcmd) == -1:
            return False
        else:
            return True


class CCheckGuideLaneInlinkTileSame(platform.TestCase.CTestCase):
    def _do(self):
        checkobject = rdb_common_check.\
        CCheckItemTileIDSame(self.pg, 'rdb_guideinfo_lane', 'in_link_id', 'in_link_id_t')
        return checkobject.do()


class CCheckGuideLaneNodeTileSame(platform.TestCase.CTestCase):

    def _do(self):
        checkobject = rdb_common_check.\
        CCheckItemTileIDSame(self.pg, 'rdb_guideinfo_lane', 'node_id', 'node_id_t')
        return checkobject.do()


class CCheckGuideLaneOutlinkTileSame(platform.TestCase.CTestCase):
    def _do(self):
        checkobject = rdb_common_check.\
        CCheckItemTileIDSame(self.pg, 'rdb_guideinfo_lane', 'out_link_id', 'out_link_id_t')
        return checkobject.do()
   
class CCheckGuideLanePasslinkCnt(rdb_common_check.CCheckPassLinkCountWithGuideidParam):
    def __init__(self, suite, caseinfo):
        rdb_common_check.CCheckPassLinkCountWithGuideidParam.__init__(self, suite, caseinfo, 'lane_tbl', 'passlink_cnt')
        pass


class CCheckGuideLaneInLinkOutLinkEqual(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*) from rdb_guideinfo_lane where in_link_id = out_link_id
        """
        return 0 == self.pg.getOnlyQueryResult(sqlcmd)


class CCheckGuideLaneExtendFlag(platform.TestCase.CTestCase):
    def _do(self):
        checkobject = rdb_common_check.\
        CCheckNodeExtendFlag(self.pg, 'rdb_guideinfo_lane', 6)
        return checkobject.do()


class CCheckGuideLanePathCnt(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        select *
            from
            (
            SELECT gid, id, nodeid, inlinkid, outlinkid, array_upper(regexp_split_to_array(passlid, E'\\|+'),1) as lenth, passlink_cnt,
                   lanenum, laneinfo, arrowinfo, lanenuml, lanenumr, buslaneinfo
              FROM lane_tbl
            )as a
            where a.lenth <> passlink_cnt;
        """
        self.pg.execute(sqlcmd)
        if  len(self.pg.fetchall()) > 0:
            return False
        else:
            return True


class CCheckGuideLanePathLinkRe(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
            SELECT regexp_split_to_array(passlid, E'\\|+') as passlinks
            FROM lane_tbl
        """
        self.pg.execute(sqlcmd)
        rows = self.pg.fetchall()
        for row in rows:
            len1 = len(row[0])
            lent2 = len(set(row[0]))
            if  len1 != lent2:
                return False
        return True

class CCheckGuideLaneNodeForwarded(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
            select f.node_id
            from (
                select c.node_id
                from(
                    select unnest(nodes) as node_id
                    from(
                        SELECT inlinkid, array_agg(passlink_cnt) as a,array_agg( distinct nodeid) as nodes
                          FROM lane_tbl
                          group by inlinkid
                    ) as b
                where 0 <> all(a)
                ) as c
                left join link_tbl as d
                on c.node_id = d.s_node or c.node_id = d.e_node
            ) as f
            group by f.node_id having count(*) < 3
        """
        self.pg.execute(sqlcmd)
        rows = self.pg.fetchall()
        if  len(rows) > 0:
            return False
        else:
            return True


class CCheckGuideLaneArrowInfo(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
            SELECT lane_num, lane_info, arrow_info,in_link_id,out_link_id
            FROM rdb_guideinfo_lane;
        """
        self.pg.execute(sqlcmd)
        rows = self.pg.fetchall()
        length = len(rows)
        normal_sum = 0
        exception_sum = 0
        if  length > 0:
            for row in rows:
                lane_num = row[0]
                lane_info = row[1]
                arrow_info = row[2]
                left_lane = pow(2, lane_num - 1)
                er_left_value = False
                er_right_value = False
                rt_left_value = False
                rt_right_value = False
                if left_lane <= lane_info:
                    er_left_value = \
                        self._arrow_in_list(arrow_info, RIGHT_ARROW)
                    rt_left_value = \
                        self._arrow_in_list(arrow_info, LEFT_ARROW)

                if (lane_info % 2) != 0:
                    er_right_value = \
                        self._arrow_in_list(arrow_info, LEFT_ARROW)
                    rt_right_value = \
                        self._arrow_in_list(arrow_info, RIGHT_ARROW)

                if rt_left_value or rt_right_value:
                    normal_sum = normal_sum + 1
                if er_left_value or er_right_value:
                    exception_sum = exception_sum + 1
#             print normal_sum, exception_sum
            if normal_sum > exception_sum:
                return True
            else:
                return False
        else:
            return False

    def _arrow_in_list(self, arrow, arrow_list):
        if len(arrow_list) < 1:
            return False
        for temp_arrow in arrow_list:
            if (arrow & temp_arrow) != 0:
                return True
        return False


class CCheckGuideLaneAllStraight(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
            select count(*)
            from(
            SELECT in_link_id, node_id, array_agg(arrow_info) as arrows
              FROM rdb_guideinfo_lane
              where lane_info > 1
              group by in_link_id,node_id
            ) as a
            where array_upper(arrows,1) > 2 and not (1 <> any(arrows));
        """
        self.pg.execute(sqlcmd)
        rows = self.pg.fetchone()
        if  rows[0] > 0:
            return False
        else:
            return True


class CCheckGuideStraightLaneSmallOther(platform.TestCase.CTestCase):
    def _do(self):
        self.pg.CreateFunction_ByName("get_tow_link_angle")
        self.pg.CreateFunction_ByName("check_guide_lane_straight_dir")
        sqlcmd = """
            select check_guide_lane_straight_dir();
        """
        self.pg.execute(sqlcmd)
        rows = self.pg.fetchone()
        if rows[0] == 0:
            return False
        else:
            return True

class CCheckExclusive(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd= '''
            select in_link_id,node_id,lane_num,
                    array_agg(exclusive) as ex_arr,
                    array_agg(lane_info) as laneinfo_arr 
            from 
            (
                select distinct in_link_id,node_id,lane_num,exclusive,lane_info,arrow_info 
                from rdb_guideinfo_lane
            ) a
            group by in_link_id,node_id,lane_num
                '''
        self.pg.execute(sqlcmd)
        rows = self.pg.fetchall()
        for row in rows:
            lanenum=row[2]
            ex_arr=row[3]
            laneinfo_arr=map(lambda x:rjust(bin(x)[2:],lanenum,'0'),row[4])
            if not set(filter(lambda x:ex_arr[x]==1,range(len(ex_arr))))==\
                set(filter(lambda x:x<>-1,map(lambda x:-1 if x.count('1')!=1 else x.index('1'),\
                                              map(lambda x:''.join(x),zip(*laneinfo_arr))))):
                return False
        return True
                
                