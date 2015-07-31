'''
Created on 2014-07-31

@author: yuanrui
'''

import platform.TestCase

class CCheckTrf14(platform.TestCase.CTestCase):

    def _do(self):
        
        sqlcmd = """
        select count(*) from rdb_trf_org2rdb;
            """
        
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt != 0)
      
class CCheckTrf4(platform.TestCase.CTestCase):

    def _do(self):
        
        sqlcmd = """
        select count(*) from rdb_region_trf_org2rdb_layer4;
            """
        
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt != 0)
    
class CCheckTrf6(platform.TestCase.CTestCase):

    def _do(self):
        
        sqlcmd = """
        select count(*) from rdb_region_trf_org2rdb_layer6;
            """
        
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt != 0)    
    
class CCheckAreaNo_NotNull(platform.TestCase.CTestCase):

    def _do(self):
        
        sqlcmd = """
        select count(*) from rdb_trf_tile_area_no;
            """
        
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt != 0)      

class CCheckAreaNo_Null(platform.TestCase.CTestCase):

    def _do(self):
        
        sqlcmd = """
        select count(*) from rdb_trf_tile_area_no;
            """
        
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)    
    
class CCheckEvent(platform.TestCase.CTestCase):

    def _do(self):
        
        sqlcmd = """
        select count(*) from rdb_trf_event;
            """
            
        if self.pg.execute(sqlcmd) == -1:
            return False
        else:
            rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
            return (rec_cnt != 0)        
    
class CCheckSupplementary(platform.TestCase.CTestCase):

    def _do(self):
        
        sqlcmd = """
        select count(*) from rdb_trf_supplementary;
            """
            
        if self.pg.execute(sqlcmd) == -1:
            return False
        else:
            rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
            return (rec_cnt != 0)
        
class CCheckLocationTable(platform.TestCase.CTestCase):

    def _do(self):
        
        sqlcmd = """
        select count(*) from rdb_trf_locationtable;
            """
            
        if self.pg.execute(sqlcmd) == -1:
            return False
        else:
            rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
            return (rec_cnt != 0)
        
class CCheckTrfLocationCode(platform.TestCase.CTestCase):

    def _do(self):
        
        sqlcmd = """
        select count(*) from rdb_trf_org2rdb a
        left join rdb_trf_locationtable b
        on a.area_code = b.country_code and a.extra_flag = b.table_no and a.infra_id = b.location_code
        where b.location_code is null;
            """
            
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)               