# -*- coding: UTF8 -*-
'''
Created on 2011-12-22

@author: liuxinxing
'''

import platform.TestCase

class CCheckTableStruct(platform.TestCase.CTestCase):
    def _do(self):
        return self.pg.IsExistTable('rdb_link_regulation')

class CCheckRegulationNumber(platform.TestCase.CTestCase):
    def _do(self):
        mid_count = self.pg.getOnlyQueryResult("select count(distinct(regulation_id, condtype)) from regulation_relation_tbl")
        rdb_count = self.pg.getOnlyQueryResult("select count(record_no) from rdb_link_regulation")
        return (mid_count == rdb_count)

class CCheckLinkrowContinuous(platform.TestCase.CTestCase):
    def _do(self):
        try:
            self.pg.CreateFunction_ByName('rdb_check_regulation_item')
            self.pg.callproc('rdb_check_regulation_item')
            return True
        except:
            self.logger.exception('fail to check...')
            return False

class CCheckConditionMatching(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.regulation_id)
                    from (select * from rdb_link_regulation where regulation_id > 0) as a
                    left join rdb_link_cond_regulation as b
                    on a.regulation_id = b.regulation_id
                    where b.regulation_id is null
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        if count_rec > 0:
            return False
        
#        sqlcmd = """
#                    select count(a.regulation_id)
#                    from rdb_link_cond_regulation as a
#                    left join rdb_link_regulation as b
#                    on a.regulation_id = b.regulation_id
#                    where b.regulation_id is null
#                 """
#        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
#        if count_rec > 0:
#            return False
        
        return True

class CCheckLastLinkID(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.last_link_id)
                    from rdb_link_regulation as a
                    left join rdb_link as b
                    on a.last_link_id = b.link_id
                    where b.link_id is null
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        if count_rec > 0:
            return False
                
        return True

class CCheckLinkFlag(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.link_id)
                    from 
                    (
                    select distinct unnest(string_to_array(key_string, ',')::bigint[]) as link_id
                    from rdb_link_regulation
                    ) as a
                    left join rdb_link as b
                    on a.link_id = b.link_id
                    where not b.regulation_flag
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        if count_rec > 0:
            return False
        
        sqlcmd = """
                    select count(a.link_id)
                    from (select link_id from rdb_link where regulation_flag) as a
                    left join 
                    (
                    select distinct unnest(string_to_array(key_string, ',')::bigint[]) as link_id
                    from rdb_link_regulation
                    ) as b
                    on a.link_id = b.link_id
                    where b.link_id is null
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        if count_rec > 0:
            return False
        
        return True

class CCheckLastLinkDir(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(a.regulation_id)
                from rdb_link_regulation as a
                left join rdb_link as b
                on a.last_link_id = b.link_id
                where (a.last_link_dir = 1 and b.one_way in (3,0))
                   or (a.last_link_dir = 2 and b.one_way in (2,0))
                   or (a.last_link_dir = 0 and b.one_way != 1)
                """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckIsSeasonal(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(regulation_id)
                from rdb_link_regulation
                where is_seasonal
                """
        count_seasonal = self.pg.getOnlyQueryResult(sqlcmd)
        
        sqlcmd = """
                select count(regulation_id)
                from rdb_link_regulation
                """
        count_all = self.pg.getOnlyQueryResult(sqlcmd)
        
        return (count_seasonal > 0) and (count_seasonal < count_all)
