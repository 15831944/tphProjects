# -*- coding: UTF8 -*-
'''
Created on 2015-3-19

@author: zhengchao
'''
import platform.TestCase
from check.rdb import rdb_common_check

class CCheckSafetyZoneLinkID(platform.TestCase.CTestCase):
    
    def _do(self):
        
        sqlcmd = """
                    select count(a.link_id)
                    from rdb_guideinfo_safety_zone as a
                    left join rdb_link as b
                    on a.link_id = b.link_id
                    where b.link_id is null
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CChecklinkidTileSame(platform.TestCase.CTestCase):
    def _do(self):
        checkobject = rdb_common_check.CCheckItemTileIDSame(self.pg, 'rdb_guideinfo_safety_zone', 'link_id', 'link_id_t')
        return checkobject.do()

def check_values_sql(strcolumn, str_values):
    sqlcmd = """
            select count(*)
            from rdb_guideinfo_safety_zone
            where [column_name] not in [values_array]
         """
    sqlcmd = sqlcmd.replace('[column_name]', strcolumn)
    sqlcmd = sqlcmd.replace('[values_array]', str_values)
    
    return sqlcmd

class CCheckspeedunit_code_values(platform.TestCase.CTestCase):
    
    def _do(self):
        count_rec = self.pg.getOnlyQueryResult(check_values_sql('speedunit_code', '(0,1,2)'))
        return (count_rec == 0)
class CCheckdirection_values(platform.TestCase.CTestCase):
    
    def _do(self):
        count_rec = self.pg.getOnlyQueryResult(check_values_sql('direction', '(1,2,3)'))
        return (count_rec == 0)
class CChecksafety_type_values(platform.TestCase.CTestCase):
    
    def _do(self):
        count_rec = self.pg.getOnlyQueryResult(check_values_sql('safety_type', '(0,1,2,3)'))
        return (count_rec == 0)
    
class CCheckspeedlimit_values(platform.TestCase.CTestCase):
    
    def _do(self):
        
        sqlcmd = """
            select count(*)
            from rdb_guideinfo_safety_zone
            where (speedlimit <= 0 or speedlimit > 260) and safety_type in (1,3)
            
            union
            select count(*)
            from rdb_guideinfo_safety_zone
            where (speedlimit < 0 or speedlimit > 260) and safety_type = 2           
            
         """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)
