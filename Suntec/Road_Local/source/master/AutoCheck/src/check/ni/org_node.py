# coding:utf-8

import platform.TestCase

class CCheckTable(platform.TestCase.CTestCase):
    def _do(self):
        return self.pg.IsExistTable('org_n')

class CCheckTableCount(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd='''
                select count(1) from org_n
               '''
        return self.pg.getOnlyQueryResult(sqlcmd)>0

class CCheckIDDuplication(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*) from (
                    select id from org_n
                    group by id having count(*) > 1
                ) a;
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)
    
class CCheckCrossflag(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd ='''
                select count(1) from org_n where cross_flag not in ('0','1','2','3')
                '''
        return self.pg.getOnlyQueryResult(sqlcmd)==0
    
class CCheckLightflag(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd ='''
                select count(1) from org_n where light_flag not in ('0','1')
                '''
        return self.pg.getOnlyQueryResult(sqlcmd)==0
    
class CCheckCross_lid(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(1) from org_n where cross_flag in ('2','3') and cross_lid='0' 
                 '''
        return self.pg.getOnlyQueryResult(sqlcmd)==0

class CCheckmainnodeid(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(1) from org_n where cross_flag in ('1','2','3') and mainnodeid='0'
                 '''
        return self.pg.getOnlyQueryResult(sqlcmd)==0

class CChecksubnodeid(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(1) from org_n where cross_flag in ('3') and subnodeid='0'
                 '''
        return self.pg.getOnlyQueryResult(sqlcmd)==0
