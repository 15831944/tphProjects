# -*- coding: UTF8 -*-

import platform.TestCase

class CCheckNumber(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd='''
                select count(*) from org_lane_connectivity 
               '''
        rec_count = self.pg.getOnlyQueryResult(sqlcmd) 
        return (rec_count>0)
class CCheckJunction(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(*) from org_lane_connectivity where junction not in (select id from org_city_jc_point)
                '''
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)      

        return (rec_count == 0)
class CCheckFromEdgeSeqnr(platform.TestCase.CTestCase):
    def _do(self):    
        sqlcmd = '''
                select count(*) from org_lane_connectivity a left join org_lane b on a.from_edge=b.edge_id and a.fr_seqnr=b.seqnr where b.edge_id is null
                '''
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)
class CCheckToEdgeSeqnr(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(*) from org_lane_connectivity a left join org_lane b on a.to_edge=b.edge_id and a.to_seqnr=b.seqnr where b.edge_id is null
                '''
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)
class CCheckFromEdgeToEdge(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(*) from org_lane_connectivity where from_edge=to_edge
                '''
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)            
        return (rec_count == 0)