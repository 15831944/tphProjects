# -*- coding: UTF8 -*-
'''
Created on 2015-6-26

@author: liushengqiang
'''

import platform.TestCase

class CCheckorg_spjcty_TableStruct(platform.TestCase.CTestCase):
    def _do(self):
        return self.pg.IsExistTable('org_spjcty')

class CCheckorg_spjcty_mesh_layer(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_spjcty
                where mesh_layer != '0'
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_spjcty_data_ident(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_spjcty
                where data_ident != '0'
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_spjcty_jct_number(platform.TestCase.CTestCase):
    def _do(self):
        self.pg.CreateFunction_ByName('org_check_jct_number')
        sqlcmd = """
                select count(*)
                from org_spjcty
                where 0 != org_check_jct_number(jct_number)
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_spjcty_atten_intersetion_no(platform.TestCase.CTestCase):
    def _do(self):
        self.pg.CreateFunction_ByName('org_get_meshcode')
        sqlcmd = """
                select count(*)
                from (
                    select distinct jct_number, org_get_meshcode(jct_number) as meshcode, atten_intersetion_no
                    from org_spjcty
                ) a
                left join org_node b
                    on a.meshcode = b.meshcode and atten_intersetion_no = b.nodeno
                where b.gid is null
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_spjcty_atten_intersetion_bifurcation(platform.TestCase.CTestCase):
    def _do(self):
        self.pg.CreateFunction_ByName('org_get_meshcode')
        sqlcmd = """
                select count(*)
                from (
                    select meshcode, nodeno, array_agg(linkno) as linklist
                    from (
                    select b.meshcode, b.nodeno, b.linkno
                    from (
                        select distinct jct_number, org_get_meshcode(jct_number) as meshcode, atten_intersetion_no
                        from org_spjcty
                    ) a
                    left join org_node b
                        on a.meshcode = b.meshcode and atten_intersetion_no = b.nodeno
                    where b.gid is not null
                    ) c
                    group by meshcode, nodeno
                ) d
                where array_upper(linklist, 1) <= 2
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_spjcty_atten_intersetion_onmeshborder(platform.TestCase.CTestCase):
    def _do(self):
        self.pg.CreateFunction_ByName('org_get_meshcode')
        sqlcmd = """
                select count(*)
                from (
                    select distinct jct_number, org_get_meshcode(jct_number) as meshcode, atten_intersetion_no
                    from org_spjcty
                ) a
                left join org_node b
                    on a.meshcode = b.meshcode and atten_intersetion_no = b.nodeno
                where b.gid is not null and b.crosskind = 3
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_spjcty_from_intersetion_no(platform.TestCase.CTestCase):
    def _do(self):
        self.pg.CreateFunction_ByName('org_get_meshcode')
        sqlcmd = """
                select count(*)
                from (
                    select distinct jct_number, org_get_meshcode(jct_number) as meshcode, from_intersetion_no
                    from org_spjcty
                ) a
                left join org_node b
                    on a.meshcode = b.meshcode and from_intersetion_no = b.nodeno
                where b.gid is null
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_spjcty_mng_no_bkg(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_spjcty
                where mng_no_bkg is null or char_length(mng_no_bkg) != 11
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_spjcty_to_road_number(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_spjcty
                where to_road_number is null
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_spjcty_to_road_number_to_int_no11(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_spjcty
                where to_road_number = 0 and to_int_no11 != '      '
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_spjcty_to_int_no11(platform.TestCase.CTestCase):
    def _do(self):
        self.pg.CreateFunction_ByName('org_get_meshcode')
        sqlcmd = """
                select count(*)
                from (
                    select distinct jct_number, org_get_meshcode(jct_number) as meshcode, to_int_no11
                    from org_spjcty
                    where to_int_no11 != '      '
                ) a
                left join org_node b
                    on a.meshcode = b.meshcode and to_int_no11::integer = b.nodeno
                where b.gid is null
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_spjcty_to_int_no11_to_int_no12(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_spjcty
                where to_int_no11 = '      ' and to_int_no12 != '      ' 
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_spjcty_to_int_no12(platform.TestCase.CTestCase):
    def _do(self):
        self.pg.CreateFunction_ByName('org_get_meshcode')
        sqlcmd = """
                select count(*)
                from (
                    select distinct jct_number, org_get_meshcode(jct_number) as meshcode, to_int_no12
                    from org_spjcty
                    where to_int_no12 != '      '
                ) a
                left join org_node b
                    on a.meshcode = b.meshcode and to_int_no12::integer = b.nodeno
                where b.gid is null
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_spjcty_to_int_no11_to_int_no12_2(platform.TestCase.CTestCase):
    def _do(self):
        self.pg.CreateFunction_ByName('org_get_meshcode')
        sqlcmd = """
                select count(*)
                from (
                    select distinct jct_number, org_get_meshcode(jct_number) as meshcode, to_int_no11, to_int_no12
                    from org_spjcty
                    where to_int_no11 != '      ' and to_int_no12 != '      '
                ) a
                left join org_road b
                    on a.meshcode = b.meshcode and to_int_no11::integer = b.snodeno and to_int_no12::integer = b.enodeno
                where b.gid is null
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_spjcty_mng_no_arrow1(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_spjcty
                where (to_int_no11 != '      ' and mng_no_arrow1 is null) or
                    (to_int_no11 != '      ' and mng_no_arrow1 is not null and char_length(mng_no_arrow1) != 11)
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_spjcty_to_road_number_to_int_no21(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_spjcty
                where to_road_number <= 1 and to_int_no21 != '      '
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)
    
class CCheckorg_spjcty_to_int_no21(platform.TestCase.CTestCase):
    def _do(self):
        self.pg.CreateFunction_ByName('org_get_meshcode')
        sqlcmd = """
                select count(*)
                from (
                    select distinct jct_number, org_get_meshcode(jct_number) as meshcode, to_int_no21
                    from org_spjcty
                    where to_int_no21 != '      '
                ) a
                left join org_node b
                    on a.meshcode = b.meshcode and to_int_no21::integer = b.nodeno
                where b.gid is null
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_spjcty_to_int_no21_to_int_no22(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_spjcty
                where to_int_no21 = '      ' and to_int_no22 != '      ' 
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_spjcty_to_int_no22(platform.TestCase.CTestCase):
    def _do(self):
        self.pg.CreateFunction_ByName('org_get_meshcode')
        sqlcmd = """
                select count(*)
                from (
                    select distinct jct_number, org_get_meshcode(jct_number) as meshcode, to_int_no22
                    from org_spjcty
                    where to_int_no22 != '      '
                ) a
                left join org_node b
                    on a.meshcode = b.meshcode and to_int_no22::integer = b.nodeno
                where b.gid is null
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_spjcty_to_int_no21_to_int_no22_2(platform.TestCase.CTestCase):
    def _do(self):
        self.pg.CreateFunction_ByName('org_get_meshcode')
        sqlcmd = """
                select count(*)
                from (
                    select distinct jct_number, org_get_meshcode(jct_number) as meshcode, to_int_no21, to_int_no22
                    from org_spjcty
                    where to_int_no21 != '      ' and to_int_no22 != '      '
                ) a
                left join org_road b
                    on a.meshcode = b.meshcode and to_int_no21::integer = b.snodeno and to_int_no22::integer = b.enodeno
                where b.gid is null
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_spjcty_mng_no_arrow2(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_spjcty
                where (to_int_no21 != '      ' and mng_no_arrow2 is null) or
                    (to_int_no21 != '      ' and mng_no_arrow2 is not null and char_length(mng_no_arrow2) != 11)
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_spjcty_from_intersetion_no_atten_intersetion_no(platform.TestCase.CTestCase):
    def _do(self):
        self.pg.CreateFunction_ByName('org_get_meshcode')
        sqlcmd = """
                select count(*)
                from (
                    select distinct jct_number, org_get_meshcode(jct_number) as meshcode, from_intersetion_no, atten_intersetion_no
                    from org_spjcty
                ) a
                left join org_road b
                    on a.meshcode = b.meshcode and from_intersetion_no = b.snodeno and atten_intersetion_no = b.enodeno
                where b.gid is null
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_spjcty_atten_intersetion_no_to_int_no11(platform.TestCase.CTestCase):
    def _do(self):
        self.pg.CreateFunction_ByName('org_get_meshcode')
        sqlcmd = """
                select count(*)
                from (
                    select distinct jct_number, org_get_meshcode(jct_number) as meshcode, atten_intersetion_no, to_int_no11
                    from org_spjcty
                ) a
                left join org_road b
                    on a.meshcode = b.meshcode and atten_intersetion_no = b.snodeno and to_int_no11::integer = b.enodeno
                where b.gid is null
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_spjcty_atten_intersetion_no_to_int_no21(platform.TestCase.CTestCase):
    def _do(self):
        self.pg.CreateFunction_ByName('org_get_meshcode')
        sqlcmd = """
                select count(*)
                from (
                    select distinct jct_number, org_get_meshcode(jct_number) as meshcode, atten_intersetion_no, to_int_no21
                    from org_spjcty
                ) a
                left join org_road b
                    on a.meshcode = b.meshcode and atten_intersetion_no = b.snodeno and to_int_no21::integer = b.enodeno
                where b.gid is null
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_spjcty_from_intersetion_atten_intersetion_road(platform.TestCase.CTestCase):
    def _do(self):
        self.pg.CreateFunction_ByName('org_get_meshcode')
        sqlcmd = """
                select count(*)
                from (
                    select distinct jct_number, mid_get_meshcode(jct_number) as meshcode, from_intersetion_no, atten_intersetion_no
                    from org_spjcty
                ) a
                left join org_road b
                    on a.meshcode = b.meshcode and from_intersetion_no = snodeno and atten_intersetion_no = enodeno
                where b.gid is not null and substr(elcode, 2, 1) != '1' and substr(elcode, 2, 1) != '2'
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_spjcty_atten_intersetion_jct(platform.TestCase.CTestCase):
    def _do(self):
        rec_count = 0.0
        rec_count1 = 0
        rec_count2 = 0
        self.pg.CreateFunction_ByName('org_get_meshcode')
        sqlcmd = """
                select count(*)
                from (
                    select meshcode, from_intersetion_no, nodeno, to_int_no11, to_int_no21, array_agg(linkno) as linklist, array_agg(linktype) as typelist
                    from (
                        select b.meshcode, from_intersetion_no, b.nodeno, to_int_no11, to_int_no21, b.linkno, substr(c.elcode, 4, 1) as linktype
                        from (
                            select distinct jct_number, org_get_meshcode(jct_number) as meshcode, from_intersetion_no, atten_intersetion_no, to_int_no11, to_int_no21
                            from org_spjcty
                        ) a
                        left join org_node b
                            on a.meshcode = b.meshcode and atten_intersetion_no = b.nodeno
                        left join org_road c
                            on b.meshcode = c.meshcode and b.linkno = c.linkno
                        where b.gid is not null and c.gid is not null
                    ) c
                    group by meshcode, from_intersetion_no, nodeno, to_int_no11, to_int_no21
                ) d
                """
        rec_count1 = self.pg.getOnlyQueryResult(sqlcmd)
        
        sqlcmd = """
                select count(*)
                from (
                    select meshcode, from_intersetion_no, nodeno, to_int_no11, to_int_no21, array_agg(linkno) as linklist, array_agg(linktype) as typelist
                    from (
                        select b.meshcode, from_intersetion_no, b.nodeno, to_int_no11, to_int_no21, b.linkno, substr(c.elcode, 4, 1) as linktype
                        from (
                            select distinct jct_number, org_get_meshcode(jct_number) as meshcode, from_intersetion_no, atten_intersetion_no, to_int_no11, to_int_no21
                            from org_spjcty
                        ) a
                        left join org_node b
                            on a.meshcode = b.meshcode and atten_intersetion_no = b.nodeno
                        left join org_road c
                            on b.meshcode = c.meshcode and b.linkno = c.linkno
                        where b.gid is not null and c.gid is not null
                    ) c
                    group by meshcode, from_intersetion_no, nodeno, to_int_no11, to_int_no21
                ) d
                where not ('4' = any(typelist))
                """
        rec_count2 = self.pg.getOnlyQueryResult(sqlcmd)
        
        rec_count = rec_count2 * 1.0 / rec_count1 * 100.0
        return (rec_count < 2.0)