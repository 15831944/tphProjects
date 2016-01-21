# -*- coding: UTF8 -*-
'''
Created on 2015-8-10

@author: lsq
'''

import platform.TestCase

class CCheckProhibitedManeuverLinkList(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from (
                    select id
                    from org_mn
                    where feattyp in (2101, 2103)
                ) a1
                left join (
                    select condition_id, array_agg(link_id) as linkid_array 
                    from (
                        select id as condition_id, seqnr as seq_num, trpelid as link_id
                        from (
                            select distinct id, seqnr, trpelid
                            from org_mp
                        ) a
                        order by id asc, seqnr asc
                    ) b
                    group by condition_id
                ) a2
                    on a1.id = a2.condition_id
                where a2.condition_id is null or array_upper(linkid_array, 1) <= 1
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0) 

class CCheckProhibitedManeuverLinkList_Continuity(platform.TestCase.CTestCase):
    def _do(self):
        self.pg.CreateFunction_ByName('org_check_linklist_continuity')
        
        sqlcmd = """
                select count(*)
                from (
                    select id
                    from org_mn
                    where feattyp in (2101, 2103)
                ) a1
                left join (
                    select condition_id, array_agg(link_id) as linkid_array 
                    from (
                        select id as condition_id, seqnr as seq_num, trpelid as link_id
                        from (
                            select distinct id, seqnr, trpelid
                            from org_mp
                        ) a
                        order by id asc, seqnr asc
                    ) b
                    group by condition_id
                ) a2
                    on a1.id = a2.condition_id
                where -1 = org_check_linklist_continuity(linkid_array)
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckProhibitedManeuverJnctidIsEffect(platform.TestCase.CTestCase):
    def _do(self):
        self.pg.CreateFunction_ByName('org_check_linklist_continuity')
        
        sqlcmd = """
                select count(*)
                from (
                    select id, jnctid
                    from org_mn
                    where feattyp in (2101, 2103)
                ) a1
                left join (
                    select condition_id, array_agg(link_id) as linkid_array, array_agg(f_jnctid) as f_jnctid_array, array_agg(t_jnctid) as t_jnctid_array
                    from (
                        select a.id as condition_id, seqnr as seq_num, trpelid as link_id, f_jnctid, t_jnctid
                        from (
                            select distinct id, seqnr, trpelid
                            from org_mp
                            order by id asc, seqnr asc
                        ) a
                        left join org_nw b
                            on a.trpelid = b.id
                        where b.id is not null and seqnr in (1, 2)
                    ) c
                    group by condition_id
                ) a2
                    on a1.id = a2.condition_id
                where not (jnctid = any(f_jnctid_array) or jnctid = any(t_jnctid_array))
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckUnderConstructionRoadIsExists(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                SELECT COUNT(*)
                FROM org_rs
                WHERE restrtyp = '6z'
            """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)