# -*- coding: UTF8 -*-

import platform.TestCase

class CCheckGuideInfoLaneClientInlinkidValidate(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = self.pg.GetDropFKeyStr('rdb_guideinfo_lane_client_in_link_id_fkey', 'rdb_guideinfo_lane_client')
        sqlcmd = sqlcmd + \
        """
        ALTER TABLE rdb_guideinfo_lane_client
              ADD CONSTRAINT rdb_guideinfo_lane_client_in_link_id_fkey FOREIGN KEY (in_link_id)
                  REFERENCES rdb_link_client (link_id) MATCH FULL
                  ON UPDATE NO ACTION ON DELETE NO ACTION;
        """
        
        if self.pg.execute(sqlcmd) == -1:
            return False
        else :
            return True
        
class CCheckGuideInfoLaneClientNodeidValiadate(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = self.pg.GetDropFKeyStr('rdb_guideinfo_lane_client_node_id_fkey', 'rdb_guideinfo_lane_client')
        sqlcmd = sqlcmd + \
        """
        ALTER TABLE rdb_guideinfo_lane_client
          ADD CONSTRAINT rdb_guideinfo_lane_client_node_id_fkey FOREIGN KEY (node_id)
              REFERENCES rdb_node_client (node_id) MATCH FULL
              ON UPDATE NO ACTION ON DELETE NO ACTION;
        """
        
        if self.pg.execute(sqlcmd) == -1:
            return False
        else :
            return True
        
class CCheckGuideInfoLaneClientOutlinkidValidate(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = self.pg.GetDropFKeyStr('rdb_guideinfo_lane_client_out_link_id_fkey', 'rdb_guideinfo_lane_client')
        sqlcmd = sqlcmd + \
        """
        ALTER TABLE rdb_guideinfo_lane_client
          ADD CONSTRAINT rdb_guideinfo_lane_client_out_link_id_fkey FOREIGN KEY (out_link_id)
              REFERENCES rdb_link_client (link_id) MATCH FULL
              ON UPDATE NO ACTION ON DELETE NO ACTION;
        """
        
        if self.pg.execute(sqlcmd) == -1:
            return False
        else:
            return True
        
class CCheckGuideInfoLaneClientRecordsFull(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = "select count(guideinfo_id) from rdb_guideinfo_lane"
        
        server_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        
        sqlcmd = "select count(guideinfo_id) from rdb_guideinfo_lane_client"
        
        client_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        
        return server_cnt == client_cnt

class CCheckGuideInfoLaneClientInLinkIDValidate(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        select count(guideinfo_id) from
        (
            select A.guideinfo_id, A.in_link_id as ser_linkid, B.in_link_id as client_linkid 
            from rdb_guideinfo_lane as A 
            left join rdb_guideinfo_lane_client as B
            on A.guideinfo_id = B.guideinfo_id
        ) as C
        where cast((ser_linkid << 32) >> 32 as int) <>  client_linkid
        """
        
        return 0 == self.pg.getOnlyQueryResult(sqlcmd)
    
class CCheckGuideInfoLaneClientNodeIDValidate(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        select count(guideinfo_id) from
        (
            select A.guideinfo_id, A.node_id as ser_nodeid, B.node_id as client_nodeid 
            from rdb_guideinfo_lane as A 
            left join rdb_guideinfo_lane_client as B
            on A.guideinfo_id = B.guideinfo_id
        ) as C
        where cast((ser_nodeid << 32) >> 32 as int) <>  client_nodeid
        """
        
        return 0 == self.pg.getOnlyQueryResult(sqlcmd)
    
class CCheckGuideInfoLaneClientOutLinkIDValidate(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        select count(guideinfo_id) from
        (
            select A.guideinfo_id, A.out_link_id as ser_linkid, B.out_link_id as client_linkid 
            from rdb_guideinfo_lane as A 
            left join rdb_guideinfo_lane_client as B
            on A.guideinfo_id = B.guideinfo_id
        ) as C
        where cast((ser_linkid << 32) >> 32 as int) <>  client_linkid
        """
        
        return 0 == self.pg.getOnlyQueryResult(sqlcmd)
    
class CCheckGuideInfoLaneClientInLinkIDTileSame(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        select count(guideinfo_id) from
        (
            select A.guideinfo_id, A.in_link_id_t as client_t, B.in_link_id_t as server_t
            from rdb_guideinfo_lane_client as A 
            left join rdb_guideinfo_lane as B
            on A.guideinfo_id = B.guideinfo_id
        ) as C
        where client_t <> server_t
        """
        
        return 0 == self.pg.getOnlyQueryResult(sqlcmd)

class CCheckGuideInfoLaneClientOutLinkIDTileSame(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        select count(guideinfo_id) from
        (
            select A.guideinfo_id, A.out_link_id_t as client_t, B.out_link_id_t as server_t
            from rdb_guideinfo_lane_client as A 
            left join rdb_guideinfo_lane as B
            on A.guideinfo_id = B.guideinfo_id
        ) as C
        where client_t <> server_t
        """
        return 0 == self.pg.getOnlyQueryResult(sqlcmd)
    
class CCheckGuideInfoLaneClientNodeIDTileSame(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        select count(guideinfo_id) from
        (
            select A.guideinfo_id, A.node_id_t as client_t, B.node_id_t as server_t
            from rdb_guideinfo_lane_client as A 
            left join rdb_guideinfo_lane as B
            on A.guideinfo_id = B.guideinfo_id
        ) as C
        where client_t <> server_t
        """
        return 0 == self.pg.getOnlyQueryResult(sqlcmd)

class CCheckGuideInfoLaneClientMultiLaneCntValidate(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        select count(guideinfo_id) 
        from
        (
            select A.guideinfo_id, A.lane_num as client_lane_cnt, B.lane_num as server_lane_cnt,
            B.lane_num_l as server_lane_l_cnt, B.lane_num_r as server_lane_r_cnt, 
            case when (A.lane_num << 4) >= 0 then 0 else 1 end as lane_r_pos_or_neg_flag ,
            case when A.lane_num >=0 then 0 else 1 end as lane_l_pos_or_neg_flag,
            (((A.lane_num << 4) >> 12)& 7) as lane_r_abs_value,
            ((A.lane_num >> 12)& 7) as lane_l_abs_value 
            from rdb_guideinfo_lane_client as A 
            left join rdb_guideinfo_lane as B
            on A.guideinfo_id = B.guideinfo_id
        ) as C
        where ((client_lane_cnt & 255) != server_lane_cnt)
        or case when lane_r_pos_or_neg_flag = 0 then lane_r_abs_value else 0 - lane_r_abs_value end != server_lane_r_cnt
        or case when lane_l_pos_or_neg_flag = 0 then lane_l_abs_value else 0 - lane_l_abs_value end != server_lane_l_cnt
        """
        return 0 == self.pg.getOnlyQueryResult(sqlcmd)
    
class CCheckGuideInfoLaneClientCheckLaneInfo(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        select count(guideinfo_id) from
        (
            select A.guideinfo_id, A.lane_info as client_lane_info, B.lane_info as server_lane_info
            from rdb_guideinfo_lane_client as A 
            left join rdb_guideinfo_lane as B
            on A.guideinfo_id = B.guideinfo_id
        ) as C
        where client_lane_info <> server_lane_info
        """
        return 0 == self.pg.getOnlyQueryResult(sqlcmd)
    
class CCheckGuideInfoLaneClientCheckArrowInfo(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        select count(guideinfo_id) from
        (
            select A.guideinfo_id, A.arrow_info as client_arrow_info, B.arrow_info as server_arrow_info
            from rdb_guideinfo_lane_client as A 
            left join rdb_guideinfo_lane as B
            on A.guideinfo_id = B.guideinfo_id
        ) as C
        where client_arrow_info <> server_arrow_info
        """
        return 0 == self.pg.getOnlyQueryResult(sqlcmd)   
    
        
    
