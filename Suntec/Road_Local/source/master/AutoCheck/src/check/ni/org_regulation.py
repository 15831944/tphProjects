# -*- coding: UTF8 -*-
'''
Created on 2011-12-22

@author: liuxinxing
'''

import platform.TestCase

class CCheckOnewayCRID(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_r as a
                left join org_cr as b
                on a.onewaycrid = b.crid
                where a.onewaycrid != '' and b.crid is not null
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        if rec_count == 0:
            return False
        
        sqlcmd = """
                select count(*)
                from org_r as a
                left join org_cr as b
                on a.onewaycrid = b.crid
                where a.onewaycrid != '' and b.crid is null
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        if rec_count > 0:
            return False
        
        return True

class CCheckOnewayDir(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_r as a
                left join org_cr as b
                on a.onewaycrid = b.crid
                where   (a.onewaycrid != '')
                        and 
                        (b.vpdir is null or b.vpdir not in ('2', '3'))
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckAccessCRID(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_r as a
                left join org_cr as b
                on a.accesscrid = b.crid
                where a.accesscrid != '' and b.crid is not null
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        if rec_count == 0:
            return False
        
        sqlcmd = """
                select count(*)
                from org_r as a
                left join org_cr as b
                on a.accesscrid = b.crid
                where a.accesscrid != '' and b.crid is null
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        if rec_count > 0:
            return False
        
        return True

class CCheckAccessDir(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_r as a
                left join org_cr as b
                on a.accesscrid = b.crid
                where   (a.accesscrid != '')
                        and 
                        (b.vpdir is null or b.vpdir not in ('0', '1'))
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)
    
class CCheckCExist(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_c
                where condtype = '1'
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count > 0)
    
class CCheckCID(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_c as a
                left join org_n as b
                on a.mapid = b.mapid and a.id = b.id
                where a.condtype = '1' and b.id is null
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)
    
class CCheckCInlinkid(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_c as a
                left join org_r as b
                on a.mapid = b.mapid and a.inlinkid = b.id
                where a.condtype = '1' and b.id is null
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)
    
class CCheckCOutlinkid(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_c as a
                left join org_r as b
                on a.mapid = b.mapid and a.outlinkid = b.id
                where a.condtype = '1' and b.id is null
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)
    
class CCheckCCRID(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_c as a
                left join org_cr as b
                on a.crid = b.crid
                where a.condtype = '1' and a.crid != '' and b.crid is null
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)
    
class CCheckCondExist(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_cond
                where condtype = '1'
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count > 0)
    
class CCheckCondCRID(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_cond as a
                left join org_cr as b
                on a.crid = b.crid
                where a.condtype = '1' and a.crid != '' and b.crid is null
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)
    
class CCheckCondCNL(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_cond as a
                left join org_cnl as b
                on a.mapid = b.mapid and a.condid = b.condid
                where a.condtype = '1' and b.condid is null
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        if rec_count > 0:
            return False
        
        sqlcmd = """
                select count(*)
                from org_cond as a
                left join org_cnl as b
                on a.mapid = b.mapid and a.condid = b.condid
                left join org_r as c
                on b.linkid = c.id
                left join org_n as d
                on b.nodeid = d.id
                where (a.condtype = '1')
                      and 
                      (
                          (b.linkid != '' and c.id is null)
                          or
                          (b.nodeid != '' and d.id is null)
                      )
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        if rec_count > 0:
            return False
        
        return True
    
class CCheckCrVPeriod(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = r"""
                select count(*)
                from org_cr
                where vperiod != '' 
                      and 
                      vperiod not similar to '(\\[\\((y(\\d)*)*M(\\d)*(d(\\d)*)*\\)\\((y(\\d)*)*M(\\d)*(d(\\d)*)*\\)\\])*((\\*)*\\[\\(h(\\d)*(m(\\d)*)*\\)\\(h(\\d)*(m(\\d)*)*\\)\\](\\*\\((t(\\d)*)*\\))*)*'
                """
        #sqlcmd = sqlcmd.replace('\\', '\\\\')
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckCrVPApprox(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_cr
                where vp_approx is null or vp_approx not in ('', '0', '1')
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckLinkrowContinuous(platform.TestCase.CTestCase):
    def _do(self):
        try:
            self.pg.CreateFunction_ByName('test_ni_check_regulation_linkrow')
            self.pg.callproc('test_ni_check_regulation_linkrow')
            return True
        except:
            self.logger.exception('fail to check...')
            return False

class CCheckMainnodeRegulationExist(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from
                (
                    select routeid, link_array[1] as fromlink, link_array[arc_count] as tolink
                    from
                    (
                        select routeid, array_agg(arc) as link_array, count(*) as arc_count
                        from
                        (
                            select *
                            from
                            (
                                select routeid, 1 as arc_index, arc1 as arc from org_turntable
                                union
                                select routeid, 2 as arc_index, arc2 as arc from org_turntable
                                union
                                select routeid, 3 as arc_index, arc3 as arc from org_turntable
                                union
                                select routeid, 4 as arc_index, arc4 as arc from org_turntable
                                union
                                select routeid, 5 as arc_index, arc5 as arc from org_turntable
                                union
                                select routeid, 6 as arc_index, arc6 as arc from org_turntable
                            )as t1
                            where arc is not null and arc > 0
                            order by routeid, arc_index
                        )as t2
                        group by routeid
                    )as t3
                )as m
                inner join
                (
                    select routeid, link_array[1] as fromlink, link_array[arc_count] as tolink
                    from
                    (
                        select routeid, array_agg(arc) as link_array, count(*) as arc_count
                        from
                        (
                            select *
                            from
                            (
                                select routeid, 1 as arc_index, arc1 as arc from org_turntable
                                union
                                select routeid, 2 as arc_index, arc2 as arc from org_turntable
                                union
                                select routeid, 3 as arc_index, arc3 as arc from org_turntable
                                union
                                select routeid, 4 as arc_index, arc4 as arc from org_turntable
                                union
                                select routeid, 5 as arc_index, arc5 as arc from org_turntable
                                union
                                select routeid, 6 as arc_index, arc6 as arc from org_turntable
                            )as t1
                            where arc is not null and arc > 0
                            order by routeid, arc_index
                        )as t2
                        group by routeid
                    )as t3
                )as n
                on m.routeid < n.routeid and m.fromlink = n.fromlink and m.tolink = n.tolink;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count > 0)

    