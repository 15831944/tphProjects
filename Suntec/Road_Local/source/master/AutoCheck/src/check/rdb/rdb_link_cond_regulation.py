# -*- coding: UTF8 -*-
'''
Created on 2011-12-22

@author: liuxinxing
'''

import platform.TestCase

class CCheckTableStruct(platform.TestCase.CTestCase):
    def _do(self):
        return self.pg.IsExistTable('rdb_link_cond_regulation')

class CCheckDatetimeFormat(platform.TestCase.CTestCase):
    def _do(self):
        try:
            self.pg.CreateFunction_ByName('rdb_check_regulation_condition')
            self.pg.callproc('rdb_check_regulation_condition')
            return True
        except:
            self.logger.exception('fail to check...')
            return False

class CCheckCondNumber(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from regulation_relation_tbl as a
                inner join condition_regulation_tbl as b
                on a.cond_id = b.cond_id
                """
        mid_count = self.pg.getOnlyQueryResult(sqlcmd)
        rdb_count = self.pg.getOnlyQueryResult("select count(gid) from rdb_link_cond_regulation")
        return (mid_count == rdb_count)

class CCheckCarType(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from rdb_link_cond_regulation
                where (car_type & ((1<<13) | (1<<10))) = 0
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckDayOfWeek(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from rdb_link_cond_regulation
                where  (week_flag != 0)
                       and
                       (
                           (week_flag = 127)
                           or
                           (week_flag & 127 = 0)
                           or
                           (    
                               (week_flag >> 7) & 3 = 0 
                               and 
                               (week_flag >> 9) != 0
                           )
                           or
                           (    
                               (week_flag >> 7) & 3 = 1 
                               and 
                               (week_flag >> 9 << 9) not in (1<<9, 1<<10, 1<<11)
                           )
                           or
                           (    
                               (week_flag >> 7) & 3 = 2 
                               and 
                               (week_flag >> 9 << 9) not in (1<<9, 1<<10, 1<<11, 1<<12, 1<<13, 1<<14)
                           )
                           or
                           (    
                               (week_flag >> 7) & 3 = 3 
                               and 
                               not ((week_flag >> 9) > 0 and (week_flag >> 9) < 53)
                           )
                       )
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)
