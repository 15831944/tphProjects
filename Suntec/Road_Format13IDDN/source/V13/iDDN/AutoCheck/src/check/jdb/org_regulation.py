# -*- coding: UTF8 -*-
'''
Created on 2014-4-11

@author: liuxinxing
'''

import platform.TestCase

class CCheckLqTurnregNumber(platform.TestCase.CTestCase):
    def _do(self):
        rec_count = self.pg.getOnlyQueryResult("select count(*) from lq_turnreg")
        return (rec_count > 0)

class CCheckLqTurnregLinkID(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from lq_turnreg as a
                left join road_link as b
                on a.link_id = b.objectid
                where b.objectid is null
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckLqTurnregLinkDir(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from lq_turnreg as a
                inner join road_link as b
                on a.link_id = b.objectid
                where  (a.linkdir_c = 1 and b.oneway_c in (2))
                    or (a.linkdir_c = 2 and b.oneway_c in (1))
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckLqTurnregSequence(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select  count(*)
                from
                (
                    select  inf_id, 
                            array_agg(sequence) as seq_array,
                            generate_series(1, count(*)) as seq
                    from
                    (
                        select *
                        from lq_turnreg
                        order by inf_id, sequence
                    )as t
                    group by inf_id
                )as t2
                where seq_array[seq] != seq
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckLqTurnregInfID(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from lq_turnreg
                where inf_id is null or inf_id <= 0
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)


class CCheckInfTurnregNumber(platform.TestCase.CTestCase):
    def _do(self):
        rec_count = self.pg.getOnlyQueryResult("select count(*) from inf_turnreg")
        return (rec_count > 0)

class CCheckInfTurnregObjectID(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from inf_turnreg as a
                left join lq_turnreg as b
                on a.objectid = b.inf_id
                where b.inf_id is null
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)


class CCheckTimeOnewayNumber(platform.TestCase.CTestCase):
    def _do(self):
        rec_count = self.pg.getOnlyQueryResult("select count(*) from time_oneway")
        return (rec_count > 0)

class CCheckTimeOnewayLinkID(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from time_oneway as a
                left join road_link as b
                on a.link_id = b.objectid
                where b.objectid is null
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckTimeOnewayLinkDir(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from time_oneway as a
                inner join road_link as b
                on a.link_id = b.objectid
                where  (a.linkdir_c = 1 and b.oneway_c in (2,4))
                    or (a.linkdir_c = 2 and b.oneway_c in (1,3))
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)


class CCheckTimeNoPassageNumber(platform.TestCase.CTestCase):
    def _do(self):
        rec_count = self.pg.getOnlyQueryResult("select count(*) from time_nopassage")
        return (rec_count > 0)

class CCheckTimeNoPassageLinkID(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from time_nopassage as a
                left join road_link as b
                on a.link_id = b.objectid
                where b.objectid is null
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)


class CCheckTime(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from
                (
                select start_month, start_day, start_hour, start_min, end_month, end_day, end_hour, end_min, dayofweek_c
                from time_nopassage
                union 
                select start_month, start_day, start_hour, start_min, end_month, end_day, end_hour, end_min, dayofweek_c
                from time_oneway
                union
                select start_month1, start_day1, start_hour1, start_min1, end_month1, end_day1, end_hour1, end_min1, dayofweek1_c
                from inf_turnreg
                union
                select start_month2, start_day2, start_hour2, start_min2, end_month2, end_day2, end_hour2, end_min2, dayofweek2_c
                from inf_turnreg
                union
                select start_month3, start_day3, start_hour3, start_min3, end_month3, end_day3, end_hour3, end_min3, dayofweek3_c
                from inf_turnreg
                union
                select start_month4, start_day4, start_hour4, start_min4, end_month4, end_day4, end_hour4, end_min4, dayofweek4_c
                from inf_turnreg
                )as t
                where   (t.start_month is not null)
                    and (
                            start_month < 0
                            or 
                            start_month > 12
                            or
                            start_day < 0
                            or 
                            start_day > 31
                            or
                            end_month < 0
                            or 
                            end_month > 12
                            or
                            end_day < 0
                            or 
                            end_day > 31
                            or 
                            start_hour < 0
                            or 
                            start_hour > 24
                            or 
                            start_min < 0
                            or 
                            start_min > 59
                            or 
                            end_hour < 0
                            or 
                            end_hour > 24
                            or 
                            end_min < 0
                            or 
                            end_min > 59
                            or
                            (start_month in (4,6,9,11) and start_day > 30)
                            or
                            (start_month = 2 and start_day > 29)
                            or
                            (end_month in (4,6,9,11) and end_day > 30)
                            or
                            (end_month = 2 and end_day > 29)
                            or
                            (start_hour = 24 and start_min > 0)
                            or
                            (end_hour = 24 and end_min > 0)
                        )
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckDayOfWeek(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from
                (
                select start_month, start_day, start_hour, start_min, end_month, end_day, end_hour, end_min, dayofweek_c
                from time_nopassage
                union 
                select start_month, start_day, start_hour, start_min, end_month, end_day, end_hour, end_min, dayofweek_c
                from time_oneway
                union
                select start_month1, start_day1, start_hour1, start_min1, end_month1, end_day1, end_hour1, end_min1, dayofweek1_c
                from inf_turnreg
                union
                select start_month2, start_day2, start_hour2, start_min2, end_month2, end_day2, end_hour2, end_min2, dayofweek2_c
                from inf_turnreg
                union
                select start_month3, start_day3, start_hour3, start_min3, end_month3, end_day3, end_hour3, end_min3, dayofweek3_c
                from inf_turnreg
                union
                select start_month4, start_day4, start_hour4, start_min4, end_month4, end_day4, end_hour4, end_min4, dayofweek4_c
                from inf_turnreg
                )as t
                where   (t.dayofweek_c is not null) and (dayofweek_c < 1 or dayofweek_c > 17)
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)


class CCheckLinkrowContinuous(platform.TestCase.CTestCase):
    def _do(self):
        self.pg.CreateFunction_ByName('test_check_linkrow_continuable')
        sqlcmd = """
                select count(*)
                from
                (
                    select  inf_id, 
                            array_agg(link_id) as link_array,
                            array_agg(sequence) as seq_array,
                            array_agg(one_way) as oneway_array,
                            array_agg(from_node_id) as snode_array,
                            array_agg(to_node_id) as enode_array
                    from
                    (
                        select  a.inf_id, a.link_id, a.sequence, 
                                (
                                case 
                                when b.oneway_c in (0,5) then 1
                                when b.oneway_c in (1,3) then 2
                                when b.oneway_c in (2,4) then 3
                                else 4
                                end
                                ) as one_way, 
                                b.from_node_id, b.to_node_id
                        from lq_turnreg as a
                        left join road_link as b
                        on a.link_id = b.objectid
                        order by a.inf_id, a.sequence
                    )as t
                    group by inf_id
                )as t
                where not test_check_linkrow_continuable(link_array, seq_array, oneway_array, snode_array, enode_array)
                ;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)

class CCheckMainnodeRegulationExist(platform.TestCase.CTestCase):
    def _do(self):
        self.pg.CreateFunction_ByName('test_get_connect_junction')
        sqlcmd = """
                create temp table temp_regulation_link_array
                as
                select link_array[1] as fromlink, link_array[link_count] as tolink, link_array
                from
                (
                    select  count(*) as link_count,
                            array_agg(link_id) as link_array
                    from
                    (
                        select *
                        from lq_turnreg
                        order by inf_id, sequence
                    )as r
                    group by inf_id
                )as t;
                
                select count(*)
                from
                (
                    select  test_get_connect_junction(m.link_array[1], m.link_array[2]) as m_jnct,
                            test_get_connect_junction(n.link_array[1], n.link_array[2]) as n_jnct
                    from temp_regulation_link_array as m
                    inner join temp_regulation_link_array as n
                    on m.fromlink = n.fromlink and m.tolink = n.tolink and m.link_array != n.link_array
                )as t
                where m_jnct = n_jnct;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)
