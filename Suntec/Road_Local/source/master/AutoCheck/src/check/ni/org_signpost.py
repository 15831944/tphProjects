# coding:utf-8
import platform.TestCase

# 检查org_signboard表不为空
class CCheckOrgSignpostCount(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd='''
                select count(*) from org_signboard;
               '''
        return self.pg.getOnlyQueryResult(sqlcmd)>0
    

