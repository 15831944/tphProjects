# -*- coding: UTF8 -*-
'''
Created on 2011-12-22

@author: liuxinxing
'''

import platform.TestCase

class CCheckTurnTableNumber(platform.TestCase.CTestCase):
    def _do(self):
        rec_count = self.pg.getOnlyQueryResult("select count(*) from org_turn_table")
        return (rec_count > 0)

class CCheckTurnTableFromEdge(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_turn_table as a
                left join org_city_nw_gc_polyline as b
                on a.frmedgeid = b.id
                where b.id is null
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckTurnTableToEdge(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_turn_table as a
                left join org_city_nw_gc_polyline as b
                on a.toedgeid = b.id
                where b.id is null
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckTurnTableJunction(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_turn_table as a
                left join org_city_jc_point as b
                on a.junction = b.id
                where b.id is null
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckComplexTurnNumber(platform.TestCase.CTestCase):
    def _do(self):
        rec_count = self.pg.getOnlyQueryResult("select count(*) from org_complex_turn_restriction")
        return (rec_count > 0)

class CCheckComplexTurnID(platform.TestCase.CTestCase):
    def _do(self):
        rec_count = self.pg.getOnlyQueryResult("select count(*) from org_complex_turn_restriction where id is null")
        return (rec_count == 0)

class CCheckComplexTurnSeq(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = "select count(*) from org_complex_turn_restriction where seq is null or seq < 1"
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        if rec_count > 0:
            return False
        
        sqlcmd = """
                select count(*)
                from
                (
                    select id, seq_array, generate_series(1, seq_count) as seq_index
                    from
                    (
                        select id, count(seq) as seq_count, array_agg(seq) as seq_array
                        from
                        (
                            select id, seq
                            from org_complex_turn_restriction
                            order by id, seq
                        )as a
                        group by id
                    )as b
                )as c
                where seq_array[seq_index] != seq_index
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckComplexTurnParentID(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_complex_turn_restriction as a
                left join org_city_nw_gc_polyline as b
                on a.parent_id = b.id
                where b.id is null
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckComplexTurnChildID(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_complex_turn_restriction as a
                left join org_city_nw_gc_polyline as b
                on a.child_id = b.id
                where b.id is null
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckDateTimeNumber(platform.TestCase.CTestCase):
    def _do(self):
        rec_count = self.pg.getOnlyQueryResult("select count(*) from org_date_time")
        return (rec_count > 0)

class CCheckDateTimeEdgeID(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_date_time as a
                left join org_city_nw_gc_polyline as b
                on a.edge_id = b.id
                where b.id is null
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckDateTimeToEdgeID(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_date_time as a
                --left join org_city_nw_gc_polyline as b
                --on a.to_edge = b.id
                --where (a.to_edge is null) or (a.to_edge != 0 and b.id is null)
                where to_edge is not null and to_edge > 0
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckDateTimeCondID(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_date_time as a
                left join org_condition_type as b
                on a.cond_id = b.cond_id
                where b.cond_id is null
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckDateTimeType(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_date_time
                where type is null or type not in ('A', 'C', 'D', 'E', 'F', 'H', 'I', '1')
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckDateTimeExclDate(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_date_time
                where excl_date is null or excl_date not in ('Y', 'N')
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckDateTimeFEnd(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_date_time
                where type not in ('C', 'E', 'H') and (f_end is null or f_end not in ('Y', 'N'))
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckDateTimeRefDate(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = r"""
                select count(*)
                from org_date_time
                where (ref_date is null)
                      or 
                      (type = '1' and ref_date !~ '(Y|N){7}')
                      or 
                      (type != '1' and ref_date !~ '(\\d){8}')
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckDateTimeExpDate(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = r"""
                select count(*)
                from org_date_time
                where (type = '1' and exp_date is not null)
                      or 
                      (f_end = 'N' and exp_date is not null)
                      or 
                      (f_end = 'Y' and exp_date !~ '(\\d){8}')
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckDateTimeStrTime(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = r"""
                select count(*)
                from org_date_time
                where (str_time is null)
                    or (str_time !~ '(\\d){4}')
                    or (str_time::integer / 100 > 24)
                    or (str_time::integer % 100 > 60)
                    or ((str_time::integer / 100 = 24) and (str_time::integer % 100 > 0))
                ;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)

class CCheckDateTimeEndTime(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = r"""
                select count(*)
                from org_date_time
                where (end_time is null)
                    or (end_time !~ '(\\d){4}')
                    or (end_time::integer / 100 > 24)
                    or (end_time::integer % 100 > 60)
                    or ((end_time::integer / 100 = 24) and (end_time::integer % 100 > 0))
                ;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)

class CCheckConditionTypeNumber(platform.TestCase.CTestCase):
    def _do(self):
        rec_count = self.pg.getOnlyQueryResult("select count(*) from org_condition_type")
        return (rec_count > 0)

class CCheckConditionTypeCondID(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from
                (
                    select cond_id
                    from org_condition_type
                    group by cond_id having count(*) > 1
                )as t;
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckConditionTypeType(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_condition_type
                where type is null or type not in ('5','7')
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckConditionTypeValue(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_condition_type
                where (value is null)
                    or (type = '5' and value not in ('1','2','3'))
                    or (type = '7' and value not in ('1'))
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckLinkrowContinuous(platform.TestCase.CTestCase):
    def _do(self):
        self.pg.CreateFunction_ByName('test_mmi_check_linkrow_continuable')
        sqlcmd = """
                select count(*)
                from
                (
                    select ARRAY[frmedgeid, toedgeid] as link_array
                    from org_turn_table
                    union
                    select ARRAY[parent_id] || array_agg(child_id) as link_array
                    from
                    (
                        select *
                        from org_complex_turn_restriction
                        order by id, parent_id, seq
                    )as r
                    group by id, parent_id
                )as t
                where not test_mmi_check_linkrow_continuable(link_array)
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
                    select  2 as link_count, 
                            ARRAY[frmedgeid, toedgeid] as link_array
                    from org_turn_table
                    union
                    select  (count(*) + 1) as link_count,
                            ARRAY[parent_id] || array_agg(child_id) as link_array
                    from
                    (
                        select *
                        from org_complex_turn_restriction
                        order by id, parent_id, seq
                    )as r
                    group by id, parent_id
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
