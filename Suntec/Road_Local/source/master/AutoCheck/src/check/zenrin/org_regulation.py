# -*- coding: UTF8 -*-
'''
Created on 2015-6-16

@author: liushengqiang
'''

import platform.TestCase

class CCheckorg_one_way_TableStruct(platform.TestCase.CTestCase):
    def _do(self):
        return self.pg.IsExistTable('org_one-way')
    
class CCheckorg_one_way_meshcode(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(gid)
                from "org_one-way"
                where meshcode is null
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_one_way_linkno(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(a.gid)
                from "org_one-way" a
                left join (
                    select gid, meshcode, linkno
                    from org_road
                ) b 
                    on a.meshcode = b.meshcode and a.linkno = b.linkno
                where b.gid is null
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_one_way_snodeno(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(a.gid)
                from "org_one-way" a
                left join org_node b
                    on a.meshcode = b.meshcode and a.snodeno = b.nodeno
                where b.gid is null
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_one_way_enodeno(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(a.gid)
                from "org_one-way" a
                left join org_node b
                    on a.meshcode = b.meshcode and a.enodeno = b.nodeno
                where b.gid is null
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_one_way_linkno_snodeno(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(a.gid)
                from "org_one-way" a
                left join (
                    select gid, meshcode, linkno, snodeno, enodeno
                    from org_road
                ) b 
                    on a.meshcode = b.meshcode and a.linkno = b.linkno
                where a.snodeno not in (b.snodeno, b.enodeno)
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_one_way_linkno_enodeno(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(a.gid)
                from "org_one-way" a
                left join (
                    select gid, meshcode, linkno, snodeno, enodeno
                    from org_road
                ) b 
                    on a.meshcode = b.meshcode and a.linkno = b.linkno
                where a.enodeno not in (b.snodeno, b.enodeno)
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_road_oneway(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(a.gid)
                from "org_one-way" a
                left join (
                    select gid, meshcode, linkno
                    from org_road
                    where oneway != 0
                ) b 
                    on a.meshcode = b.meshcode and a.linkno = b.linkno
                where b.gid is null
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_road_oneway2(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from (
                    select meshcode, linkno, snodeno, enodeno, oneway
                    from org_road
                    where oneway = 1
                ) a
                left join "org_one-way" b
                    on a.meshcode = b.meshcode and a.linkno = b.linkno
                where a.snodeno != b.snodeno or a.enodeno != b.enodeno
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_road_oneway3(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from (
                    select meshcode, linkno, snodeno, enodeno, oneway
                    from org_road
                    where oneway = 2
                ) a
                left join "org_one-way" b
                    on a.meshcode = b.meshcode and a.linkno = b.linkno
                where a.snodeno = b.enodeno and a.enodeno = b.snodeno
                """
        rec_count1 = self.pg.getOnlyQueryResult(sqlcmd)
        
        sqlcmd = """
                select count(*)
                from (
                    select meshcode, linkno, snodeno, enodeno, oneway
                    from org_road
                ) a
                left join "org_one-way" b
                    on a.meshcode = b.meshcode and a.linkno = b.linkno
                where a.snodeno = b.snodeno and a.enodeno = b.enodeno
                """
        rec_count2 = self.pg.getOnlyQueryResult(sqlcmd)
        
        rec_count = rec_count1 & rec_count2
        return (rec_count == 0)

class CCheckorg_one_way_day(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(gid)
                from "org_one-way"
                where day is null or day::bigint = 0 or char_length(day) != 9
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_one_way_shour(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(gid)
                from "org_one-way"
                where 
                    shour is null or 
                    char_length(shour) != 4 or 
                    (cast(substring(shour, 1, 2) as integer) < 0 or cast(substring(shour, 1, 2) as integer) > 24) or
                    (cast(substring(shour, 3, 2) as integer) < 0 or cast(substring(shour, 3, 2) as integer) > 59) or 
                    (cast(substring(shour, 1, 2) as integer) = 24 and cast(substring(shour, 3, 2) as integer) != 0)
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_one_way_ehour(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(gid)
                from "org_one-way"
                where 
                    ehour is null or 
                    char_length(ehour) != 4 or 
                    (cast(substring(ehour, 1, 2) as integer) < 0 or cast(substring(ehour, 1, 2) as integer) > 24) or
                    (cast(substring(ehour, 3, 2) as integer) < 0 or cast(substring(ehour, 3, 2) as integer) > 59) or 
                    (cast(substring(ehour, 1, 2) as integer) = 24 and cast(substring(ehour, 3, 2) as integer) != 0)
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_one_way_sdate(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(gid)
                from "org_one-way"
                where 
                    sdate is null or 
                    char_length(sdate) != 4 or 
                    (cast(substring(sdate, 1, 2) as integer) < 1 or cast(substring(sdate, 1, 2) as integer) > 12) or
                    (cast(substring(sdate, 3, 2) as integer) < 1 or cast(substring(sdate, 3, 2) as integer) > 31)
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_one_way_edate(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(gid)
                from "org_one-way"
                where 
                    edate is null or 
                    char_length(edate) != 4 or 
                    (cast(substring(edate, 1, 2) as integer) < 1 or cast(substring(edate, 1, 2) as integer) > 12) or
                    (cast(substring(edate, 3, 2) as integer) < 1 or cast(substring(edate, 3, 2) as integer) > 31)
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_one_way_cartype(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(gid)
                from "org_one-way"
                where cartype != 0
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_not_in_TableStruct(platform.TestCase.CTestCase):
    def _do(self):
        return self.pg.IsExistTable('org_not-in')

class CCheckorg_not_in_meshcode(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(gid)
                from "org_not-in"
                where meshcode is null
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_not_in_fromlinkno(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(a.gid)
                from "org_not-in" a
                left join (
                    select gid, meshcode, linkno
                    from org_road
                ) b 
                    on a.meshcode = b.meshcode and a.fromlinkno = b.linkno
                where b.gid is null
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_not_in_tolinkno(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(a.gid)
                from "org_not-in" a
                left join (
                    select gid, meshcode, linkno
                    from org_road
                ) b 
                    on a.meshcode = b.meshcode and a.tolinkno = b.linkno
                where b.gid is null
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_not_in_snodeno(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(a.gid)
                from "org_not-in" a
                left join org_node b 
                    on a.meshcode = b.meshcode and a.snodeno = b.nodeno
                where b.gid is null
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_not_in_tnodeno(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(a.gid)
                from "org_not-in" a
                left join org_node b 
                    on a.meshcode = b.meshcode and a.tnodeno = b.nodeno
                where b.gid is null
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_not_in_enodeno(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(a.gid)
                from "org_not-in" a
                left join org_node b 
                    on a.meshcode = b.meshcode and a.enodeno = b.nodeno
                where b.gid is null
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_not_in_fromlinkno_snodeno(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(a.gid)
                from "org_not-in" a
                left join (
                    select gid, meshcode, linkno, snodeno, enodeno
                    from org_road
                ) b 
                    on a.meshcode = b.meshcode and a.fromlinkno = b.linkno
                where a.snodeno not in (b.snodeno, b.enodeno)
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_not_in_fromlinkno_tnodeno(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(a.gid)
                from "org_not-in" a
                left join (
                    select gid, meshcode, linkno, snodeno, enodeno
                    from org_road
                ) b 
                    on a.meshcode = b.meshcode and a.fromlinkno = b.linkno
                where a.tnodeno not in (b.snodeno, b.enodeno)
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_not_in_tolinkno_enodeno(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(a.gid)
                from "org_not-in" a
                left join (
                    select gid, meshcode, linkno, snodeno, enodeno
                    from org_road
                ) b 
                    on a.meshcode = b.meshcode and a.tolinkno = b.linkno
                where a.enodeno not in (b.snodeno, b.enodeno)
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_not_in_tolinkno_tnodeno(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(a.gid)
                from "org_not-in" a
                left join (
                    select gid, meshcode, linkno, snodeno, enodeno
                    from org_road
                ) b 
                    on a.meshcode = b.meshcode and a.tolinkno = b.linkno
                where a.tnodeno not in (b.snodeno, b.enodeno)
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_not_in_day(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(gid)
                from "org_not-in"
                where day is null or day::bigint = 0 or char_length(day) != 9
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_not_in_shour(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(gid)
                from "org_not-in"
                where 
                    shour is null or 
                    char_length(shour) != 4 or 
                    (cast(substring(shour, 1, 2) as integer) < 0 or cast(substring(shour, 1, 2) as integer) > 24) or
                    (cast(substring(shour, 3, 2) as integer) < 0 or cast(substring(shour, 3, 2) as integer) > 59) or 
                    (cast(substring(shour, 1, 2) as integer) = 24 and cast(substring(shour, 3, 2) as integer) != 0)
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_not_in_ehour(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(gid)
                from "org_not-in"
                where 
                    ehour is null or 
                    char_length(ehour) != 4 or 
                    (cast(substring(ehour, 1, 2) as integer) < 0 or cast(substring(ehour, 1, 2) as integer) > 24) or
                    (cast(substring(ehour, 3, 2) as integer) < 0 or cast(substring(ehour, 3, 2) as integer) > 59) or 
                    (cast(substring(ehour, 1, 2) as integer) = 24 and cast(substring(ehour, 3, 2) as integer) != 0)
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_not_in_sdate(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(gid)
                from "org_not-in"
                where 
                    sdate is null or 
                    char_length(sdate) != 4 or 
                    (cast(substring(sdate, 1, 2) as integer) < 1 or cast(substring(sdate, 1, 2) as integer) > 12) or
                    (cast(substring(sdate, 3, 2) as integer) < 1 or cast(substring(sdate, 3, 2) as integer) > 31)
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_not_in_edate(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(gid)
                from "org_not-in"
                where 
                    edate is null or 
                    char_length(edate) != 4 or 
                    (cast(substring(edate, 1, 2) as integer) < 1 or cast(substring(edate, 1, 2) as integer) > 12) or
                    (cast(substring(edate, 3, 2) as integer) < 1 or cast(substring(edate, 3, 2) as integer) > 31)
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_not_in_cartype(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(gid)
                from "org_not-in"
                where cartype != 0
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_not_in_carwait(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(gid)
                from "org_not-in"
                where carwait != 0
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_not_in_carthaba(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(gid)
                from "org_not-in"
                where carthaba != 0
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_not_in_carthigh(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(gid)
                from "org_not-in"
                where carthigh != 0
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_not_in_other_TableStruct(platform.TestCase.CTestCase):
    def _do(self):
        return self.pg.IsExistTable('org_not_in_other')

class CCheckorg_not_in_other_meshcode(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_not_in_other
                where meshcode is null
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_not_in_other_fromlinkno(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(a.*)
                from org_not_in_other a
                left join (
                    select gid, meshcode, linkno
                    from org_road
                ) b 
                    on a.meshcode = b.meshcode::integer and a.fromlinkno = b.linkno
                where b.gid is null
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_not_in_other_tolinkno(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(a.*)
                from org_not_in_other a
                left join (
                    select gid, meshcode, linkno
                    from org_road
                ) b 
                    on a.meshcode = b.meshcode::integer and a.tolinkno = b.linkno
                where b.gid is null
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_not_in_other_snodeno(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(a.*)
                from org_not_in_other a
                left join org_node b 
                    on a.meshcode = b.meshcode::integer and a.snodeno = b.nodeno
                where b.gid is null
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_not_in_other_tnodeno(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(a.*)
                from org_not_in_other a
                left join org_node b 
                    on a.meshcode = b.meshcode::integer and a.tnodeno = b.nodeno
                where b.gid is null
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_not_in_other_enodeno(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(a.*)
                from org_not_in_other a
                left join org_node b 
                    on a.meshcode = b.meshcode::integer and a.enodeno = b.nodeno
                where b.gid is null
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_not_in_other_fromlinkno_snodeno(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(a.*)
                from org_not_in_other a
                left join (
                    select gid, meshcode, linkno, snodeno, enodeno
                    from org_road
                ) b 
                    on a.meshcode = b.meshcode::integer and a.fromlinkno = b.linkno
                where a.snodeno not in (b.snodeno, b.enodeno)
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_not_in_other_fromlinkno_tnodeno(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(a.*)
                from org_not_in_other a
                left join (
                    select gid, meshcode, linkno, snodeno, enodeno
                    from org_road
                ) b 
                    on a.meshcode = b.meshcode::integer and a.fromlinkno = b.linkno
                where a.tnodeno not in (b.snodeno, b.enodeno)
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_not_in_other_tolinkno_enodeno(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(a.*)
                from org_not_in_other a
                left join (
                    select gid, meshcode, linkno, snodeno, enodeno
                    from org_road
                ) b 
                    on a.meshcode = b.meshcode::integer and a.tolinkno = b.linkno
                where a.enodeno not in (b.snodeno, b.enodeno)
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_not_in_other_tolinkno_tnodeno(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(a.*)
                from org_not_in_other a
                left join (
                    select gid, meshcode, linkno, snodeno, enodeno
                    from org_road
                ) b 
                    on a.meshcode = b.meshcode::integer and a.tolinkno = b.linkno
                where a.tnodeno not in (b.snodeno, b.enodeno)
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_not_in_other_day(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_not_in_other
                where day is null or day::bigint = 0 or char_length(day) != 9
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_not_in_other_shour(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_not_in_other
                where 
                    shour::bigint < 0 or shour::bigint > 2400
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_not_in_other_ehour(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_not_in_other
                where 
                    ehour::bigint < 0 or ehour::bigint > 2400
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_not_in_other_sdate(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_not_in_other
                where 
                    sdate::bigint < 101 or sdate::bigint > 1231
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_not_in_other_edate(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_not_in_other
                where 
                    edate::bigint < 101 or edate::bigint > 1231
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_not_in_other_cartype(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_not_in_other
                where cartype != 0
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_not_in_other_carwait(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_not_in_other
                where carwait != 0
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_not_in_other_carthaba(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_not_in_other
                where carthaba != 0
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_not_in_other_carthigh(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_not_in_other
                where carthigh != 0
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)