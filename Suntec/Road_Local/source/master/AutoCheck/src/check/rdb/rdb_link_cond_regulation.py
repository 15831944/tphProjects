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
