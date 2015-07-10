# coding:utf-8

import platform.TestCase

class CCheckorg_d_Count(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd='''
                select count(*) from org_d;
               '''
        return self.pg.getOnlyQueryResult(sqlcmd)>0

class CCheckorg_d_geom(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd='''
                select count(*) from org_d where the_geom is null;
               '''
        return self.pg.getOnlyQueryResult(sqlcmd)==0

class CCheckorg_d_org_admin(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd='''
                select count(*) 
                from
                (
                    select a.admincode
                    from org_d as a
                    left join org_admin as b
                    on a.admincode = b.admincode
                    where b.admincode is null
                    
                    union

                    select a.admincode
                    from org_admin as a
                    left join org_d as b
                    on a.admincode = b.admincode
                    where b.admincode is null                                        
                )temp;
               '''
        return self.pg.getOnlyQueryResult(sqlcmd)==0

class CCheckorg_admin_count(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd='''
                select count(*) from org_admin;
               '''
        return self.pg.getOnlyQueryResult(sqlcmd)>0

class CCheckorg_admin_name(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd='''
                select count(*) 
                from 
                (
                    select unnest(array[admincode, cityadcode, proadcode]) as id
                    from org_admin
                )a
                left join org_fname as b
                on a.id = b.featid and b.nametype = '14'
                where b.featid is null;
               '''
        return self.pg.getOnlyQueryResult(sqlcmd)==0

class CCheckorg_fname_count(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd='''
                select count(*) from org_fname where nametype = '14';
               '''
        return self.pg.getOnlyQueryResult(sqlcmd)>0

class CCheckorg_fname_phon_count(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd='''
                select count(*) from org_fname_phon where nametype = '14';
               '''
        return self.pg.getOnlyQueryResult(sqlcmd)>0



