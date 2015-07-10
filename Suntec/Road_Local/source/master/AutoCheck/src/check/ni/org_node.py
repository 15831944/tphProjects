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
    
class CCheckAdjoin_nid(platform.TestCase.CTestCase):
    def _do(self):
        #确定a,b为形点相同的图框点时：a.adjoin_nid=b.id and b.adjoin_nid=a.id，即一对一的关系，不存在多个图框点重复
        sqlcmd = '''
                select count(1) from org_n a 
                join org_n b on a.adjoin_nid=b.id 
                where position('1f00' in a.kind)>0 and b.adjoin_nid<>a.id
                 '''
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckMapNodeNoInfo(platform.TestCase.CTestCase):
    def _do(self):
        #确定图框点无任何信息
        sqlcmd = '''
                SELECT count(1)
                FROM org_n
                where kind like '%1f00%' and 
                (kind_num<>'1' or kind<>'1f00' or light_flag<>'0' 
                or cross_lid<>'0' or mainnodeid<>'0' or subnodeid<>'0' or subnodeid2 <>'')
                 '''
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
