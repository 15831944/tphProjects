# -*- coding: UTF8 -*-
'''
Created on 2011-12-22

@author: liuxinxing
'''

import platform.TestCase

class CCheckRegulationNumber(platform.TestCase.CTestCase):
    def _do(self):
        reg_count = self.pg.getOnlyQueryResult("select count(*) from org_turntable")
        return (reg_count > 0)

class CCheckRouteIDUnique(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from
                (
                    select routeid
                    from org_turntable
                    group by routeid having count(*) > 1
                )as t;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)

class CCheckArcs(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_turntable
                where   (arc1 is null or arc1 = 0) 
                    and (arc2 is null or arc1 = 0) 
                    and (arc3 is null or arc1 = 0) 
                    and (arc4 is null or arc1 = 0) 
                    and (arc5 is null or arc1 = 0) 
                    and (arc6 is null or arc1 = 0) 
                ;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        if reg_count > 0:
            return False
        
        sqlcmd = """
                select count(*)
                from
                (
                    select routeid, count(*) as arc_num, max(arc_index) as max_arc_index
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
                        )as t
                        where arc is not null and arc > 0
                    )as t2
                    group by routeid
                )as t3
                where max_arc_index != arc_num
                ;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        if reg_count > 0:
            return False
        
        sqlcmd = """
                select count(*)
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
                )as a
                left join org_l_tran as b
                on a.arc = b.routeid
                where a.arc is not null and a.arc > 0 and b.routeid is null
                ;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        if reg_count > 0:
            return False

        return True

class CCheckNodeID(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_turntable
                where nodeid > 0
                ;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)

class CCheckTurn(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_turntable
                --where turn not in (0,1)
                where turn not in (0)
                ;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)

class CCheckTime(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_turntable
                where ((time1 is not null) and not (time1 ~ '(\\d){4}-(\\d){4}'))
                      or 
                      ((time2 is not null) and not (time2 ~ '(\\d){4}-(\\d){4}'))
                      or 
                      ((time3 is not null) and not (time3 ~ '(\\d){4}-(\\d){4}'))
                      or 
                      ((time4 is not null) and not (time4 ~ '(\\d){4}-(\\d){4}'))
                ;
                """
        sqlcmd = sqlcmd.replace('\\', '\\\\')
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)

class CCheckDay(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_turntable
                where day not in (0,1,2,3,4,5,9)
                ;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)

class CCheckTurnType(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_turntable
                where turn_type not in (1,2,3,4,5,9)
                ;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)

class CCheckTimeLegal(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from
                (
                    select time, unnest(subtime_array::integer[]) as subtime
                    from
                    (
                        select time, string_to_array(time, '-') as subtime_array
                        from 
                        (
                            select time1 as time from org_turntable
                            union
                            select time2 as time from org_turntable
                            union
                            select time3 as time from org_turntable
                            union
                            select time4 as time from org_turntable
                        )as t
                        where time is not null
                    )as t2
                )as t3
                where  (subtime / 100 > 24)
                    or (subtime % 100 > 60)
                    or ((subtime / 100 = 24) and (subtime % 100 > 0))
                ;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)

class CCheckLinkrowContinuous(platform.TestCase.CTestCase):
    def _do(self):
        try:
            self.pg.CreateFunction_ByName('test_nostra_check_regulation_linkrow')
            self.pg.callproc('test_nostra_check_regulation_linkrow')
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

