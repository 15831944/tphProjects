# coding:utf-8

import platform.TestCase

class CCheckorg_dr_Count(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd='''
                select count(*) from org_dr;
               '''
        return self.pg.getOnlyQueryResult(sqlcmd)>0
    
class CCheckorg_dr_fname(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd='''
                select count(*) from org_fname where nametype = '4';
               '''
        return self.pg.getOnlyQueryResult(sqlcmd)>0
    
class CCheckorg_dr_link(platform.TestCase.CTestCase):
    def _do(self):
        return self.pg.getOnlyQueryResult(CCheckid_in_link('org_dr')) == 0
    
class CCheckorg_br_Count(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd='''
                select count(*) from org_br;
               '''
        return self.pg.getOnlyQueryResult(sqlcmd)>0
    
class CCheckorg_br_fname(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd='''
                select count(*) from org_fname where nametype = '2';
               '''
        return self.pg.getOnlyQueryResult(sqlcmd)>0
    
class CCheckorg_br_link(platform.TestCase.CTestCase):
    def _do(self):
        return self.pg.getOnlyQueryResult(CCheckid_in_link('org_br')) == 0
    
class CCheckorg_ic_Count(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd='''
                select count(*) from org_ic;
               '''
        return self.pg.getOnlyQueryResult(sqlcmd)>0
    
class CCheckorg_ic_fname(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd='''
                select count(*) from org_fname where nametype = '3';
               '''
        return self.pg.getOnlyQueryResult(sqlcmd)>0
    
class CCheckorg_ic_link(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd='''
            select count(a.link_id)
            from
            (
                select unnest(string_to_array(link_lid, '|')) as link_id
                from
                (
                    select (case when passlid is not null and length(passlid)>0 then inlinkid ||'|'||outlinkid||'|'||passlid
                                else inlinkid ||'|'||outlinkid end) as link_lid
                    from org_ic
                )temp
            )a
            left join org_r as b
            on a.link_id = b.id
            where b.id is null;
           '''
        return self.pg.getOnlyQueryResult(sqlcmd) == 0
   
def CCheckid_in_link(table_name):
    sqlcmd='''
            select count(a.link_id)
            from
            (
                select unnest(string_to_array(link_lid, '|')) as link_id
                from
                (
                    select (case when passlid2 is not null and length(passlid2)>0 then inlinkid ||'|'||outlinkid||'|'||passlid||'|'||passlid2
                                when passlid is not null and length(passlid)>0 then inlinkid ||'|'||outlinkid||'|'||passlid
                                else inlinkid ||'|'||outlinkid end) as link_lid
                    from [table1]
                )temp
            )a
            left join org_r as b
            on a.link_id = b.id
            where b.id is null;
           '''
    return sqlcmd.replace('[table1]', table_name)
    