# -*- coding: UTF8 -*-

import platform.TestCase

class CCheckNumber(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(*) from org_lane;
                '''
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)      

        return (rec_count > 0)
class CCheckSideType(platform.TestCase.CTestCase):
    def _do(self):    
        sqlcmd = '''
                select count(*) from org_lane where side not in ('FT','TF');
                '''
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)
class CChecklntyp(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(*) from org_lane where ln_typ <>0;
                '''
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)
class CCheckSeqnr(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(*) from org_lane a join 
                (select edge_id,count(1) as lane_num from org_lane group by edge_id) b 
                using(edge_id) where seqnr>b.lane_num
                '''
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)            
        return (rec_count == 0)
class CCheckDivtyp(platform.TestCase.CTestCase):
    def _do(self):    
        sqlcmd = '''
                select count(*) from org_lane where div_typ not in (0,1,2,3,4)
                '''
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        
        return (rec_count == 0)
class CCheckDrcat(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(*) from org_lane where dr_cat>511
                '''
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)
class CCheckEdgeid(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(*) from org_lane where edge_id not in (select id from org_city_nw_gc_polyline)
                '''
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)         
        return (reg_count == 0)
