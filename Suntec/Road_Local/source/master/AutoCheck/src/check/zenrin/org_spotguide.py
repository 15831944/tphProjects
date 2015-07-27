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
                    select distinct jct_number, org_get_meshcode(jct_number) as meshcode, from_intersetion_no, atten_intersetion_no
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

class CCheckorg_eci_jctv_TableStruct(platform.TestCase.CTestCase):
    def _do(self):
        return self.pg.IsExistTable('org_eci_jctv')

class CCheckorg_eci_jctv_eciid(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_eci_jctv
                where eciid is null
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_eci_jctv_seq(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_eci_jctv
                where seq not in (2, 3, 4, 5, 6)
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_eci_jctv_mesh1(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_eci_jctv
                where mesh1 is null
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_eci_jctv_link1(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_eci_jctv
                where link1 is null
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_eci_jctv_mesh1_link1(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_eci_jctv a
                left join org_road b
                    on a.mesh1 = b.meshcode::integer and a.link1 = b.linkno
                where mesh1 is not null and 
                    link1 is not null and 
                    b.meshcode is null
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_eci_jctv_mesh2(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_eci_jctv
                where mesh2 is null
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_eci_jctv_link2(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_eci_jctv
                where link2 is null
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_eci_jctv_mesh2_link2(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_eci_jctv a
                left join org_road b
                    on a.mesh2 = b.meshcode::integer and a.link2 = b.linkno
                where mesh2 is not null and 
                    link2 is not null and 
                    b.meshcode is null
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_eci_jctv_mesh3_link3_1(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_eci_jctv
                where (mesh3 is not null and link3 is null) or 
                    (mesh3 is null and link3 is not null)
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_eci_jctv_mesh3_link3_2(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_eci_jctv a
                left join org_road b
                    on a.mesh3 = b.meshcode::integer and a.link3 = b.linkno
                where mesh3 is not null and 
                    link3 is not null and 
                    b.meshcode is null
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_eci_jctv_mesh4_link4_1(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_eci_jctv
                where (mesh4 is not null and link4 is null) or 
                    (mesh4 is null and link4 is not null)
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_eci_jctv_mesh4_link4_2(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_eci_jctv a
                left join org_road b
                    on a.mesh4 = b.meshcode::integer and a.link4 = b.linkno
                where mesh4 is not null and 
                    link4 is not null and 
                    b.meshcode is null
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_eci_jctv_mesh5_link5_1(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_eci_jctv
                where (mesh5 is not null and link5 is null) or 
                    (mesh5 is null and link5 is not null)
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_eci_jctv_mesh5_link5_2(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_eci_jctv a
                left join org_road b
                    on a.mesh5 = b.meshcode::integer and a.link5 = b.linkno
                where mesh5 is not null and 
                    link5 is not null and 
                    b.meshcode is null
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_eci_jctv_mesh6_link6_1(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_eci_jctv
                where (mesh6 is not null and link6 is null) or 
                    (mesh6 is null and link6 is not null)
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_eci_jctv_mesh6_link6_2(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_eci_jctv a
                left join org_road b
                    on a.mesh6 = b.meshcode::integer and a.link6 = b.linkno
                where mesh6 is not null and 
                    link6 is not null and 
                    b.meshcode is null
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_eci_jctv_seq_2(platform.TestCase.CTestCase):
    def _do(self):
        self.pg.CreateFunction_ByName('org_check_seq')
        sqlcmd = """
                select org_check_seq()
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_eci_jctv_link_connectivity(platform.TestCase.CTestCase):
    def _do(self):
        self.pg.CreateFunction_ByName('org_check_seq')
        sqlcmd = """
                drop table if exists temp_node_mapping;
                CREATE TABLE temp_node_mapping 
                as
                (
                    select a.meshcode,a.nodeno,
                            case when b.nodeid_adj is not null 
                                then b.meshcode_adj::bigint*10000000+b.nodeid_adj 
                                else a.node_id end as node_id
                    from 
                    (
                        select distinct meshcode,nodeno,meshcode::bigint*10000000+nodeno as node_id 
                        from org_node
                    ) a
                    left join org_node_connect b 
                    on a.meshcode::integer=b.meshcode_self and a.nodeno=b.nodeid_self and mesh_rel in (2,4)
                );
                
                CREATE OR REPLACE FUNCTION org_check_link_seq()
                    RETURNS smallint
                    LANGUAGE plpgsql
                AS $$
                DECLARE
                    rec record;
                    targetNode bigint;
                    --count integer;
                BEGIN
                    --count := 0;
                    for rec in
                        select mesh1, link1, b1.node_id as snodeno1, b2.node_id as enodeno1, 
                            mesh2, link2, c1.node_id as snodeno2, c2.node_id as enodeno2, 
                            mesh3, link3, d1.node_id as snodeno3, d2.node_id as enodeno3, 
                            mesh4, link4, e1.node_id as snodeno4, e2.node_id as enodeno4, 
                            mesh5, link5, f1.node_id as snodeno5, f2.node_id as enodeno5, 
                            mesh6, link6, g1.node_id as snodeno6, g2.node_id as enodeno6
                        from org_eci_jctv a
                        left join org_road b
                            on a.mesh1 = b.meshcode::integer and a.link1 = b.linkno
                        left join temp_node_mapping b1
                            on b.meshcode = b1.meshcode and b.snodeno = b1.nodeno
                        left join temp_node_mapping b2
                            on b.meshcode = b2.meshcode and b.enodeno = b2.nodeno
                        left join org_road c
                            on a.mesh2 = c.meshcode::integer and a.link2 = c.linkno
                        left join temp_node_mapping c1
                            on c.meshcode = c1.meshcode and c.snodeno = c1.nodeno
                        left join temp_node_mapping c2
                            on c.meshcode = c2.meshcode and c.enodeno = c2.nodeno
                        left join org_road d
                            on a.mesh3 = d.meshcode::integer and a.link3 = d.linkno
                        left join temp_node_mapping d1
                            on d.meshcode = d1.meshcode and d.snodeno = d1.nodeno
                        left join temp_node_mapping d2
                            on d.meshcode = d2.meshcode and d.enodeno = d2.nodeno
                        left join org_road e
                            on a.mesh4 = e.meshcode::integer and a.link4 = e.linkno
                        left join temp_node_mapping e1
                            on e.meshcode = e1.meshcode and e.snodeno = e1.nodeno
                        left join temp_node_mapping e2
                            on e.meshcode = e2.meshcode and e.enodeno = e2.nodeno
                        left join org_road f
                            on a.mesh5 = f.meshcode::integer and a.link5 = f.linkno
                        left join temp_node_mapping f1
                            on f.meshcode = f1.meshcode and f.snodeno = f1.nodeno
                        left join temp_node_mapping f2
                            on f.meshcode = f2.meshcode and f.enodeno = f2.nodeno
                        left join org_road g
                            on a.mesh6 = g.meshcode::integer and a.link6 = g.linkno
                        left join temp_node_mapping g1
                            on g.meshcode = g1.meshcode and g.snodeno = g1.nodeno
                        left join temp_node_mapping g2
                            on g.meshcode = g2.meshcode and g.enodeno = g2.nodeno
                    loop
                        if rec.link1 is not null and rec.link2 is not null then 
                            if rec.snodeno2 in (rec.snodeno1, rec.enodeno1) then
                                targetNode := rec.enodeno2;
                            elseif rec.enodeno2 in (rec.snodeno1, rec.enodeno1) then
                                targetNode := rec.snodeno2;
                            else
                                --raise info '2 %', rec;
                                --count := count + 1;
                                return -1;
                            end if;
                        else 
                            return -1;
                        end if;
                        
                        if rec.link3 is not null then 
                            if targetNode = rec.snodeno3 then
                                targetNode := rec.enodeno3;
                            elseif targetNode = rec.enodeno3 then
                                targetNode := rec.snodeno3;
                            else
                                --raise info '3 % %', targetNode, rec;
                                --count := count + 1;
                                --continue;
                                return -1;
                            end if;
                        end if;
                        
                        if rec.link4 is not null then 
                            if targetNode = rec.snodeno4 then
                                targetNode := rec.enodeno4;
                            elseif targetNode = rec.enodeno4 then
                                targetNode := rec.snodeno4;
                            else
                                --raise info '4 % %', targetNode, rec;
                                --count := count + 1;
                                --continue;
                                return -1;
                            end if;
                        end if;
                        
                        if rec.link5 is not null then 
                            if targetNode = rec.snodeno5 then
                                targetNode := rec.enodeno5;
                            elseif targetNode = rec.enodeno5 then
                                targetNode := rec.snodeno5;
                            else
                                --raise info '5 % %', targetNode, rec;
                                --count := count + 1;
                                --continue;
                                return -1;
                            end if;
                        end if;
                        
                        if rec.link6 is not null then 
                            if targetNode = rec.snodeno6 then
                                targetNode := rec.enodeno6;
                            elseif targetNode = rec.enodeno6 then
                                targetNode := rec.snodeno6;
                            else
                                --raise info '6 % %', targetNode, rec;
                                --count := count + 1;
                                --continue;
                                return -1;
                            end if;
                        end if;
                    end loop;
                
                    --raise info 'end %', count;
                    return 0;
                END;
                $$;
                
                select org_check_link_seq();
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_eci_jctv_imagename(platform.TestCase.CTestCase):
    def _do(self):
        if True == self.pg.IsExistTable('org_eci_jctv'):
            sqlcmd = """
                    select count(*)
                    from org_eci_jctv
                    where imagename is null
                    """
            rec_count = self.pg.getOnlyQueryResult(sqlcmd)
            return (rec_count == 0)

class CCheckorg_eci_jctv_link3_link4(platform.TestCase.CTestCase):
    def _do(self):
        if True == self.pg.IsExistTable('org_eci_jctv'):
            sqlcmd = """
                    select count(*)
                    from org_eci_jctv
                    where link4 is not null and link3 is null
                    """
            rec_count = self.pg.getOnlyQueryResult(sqlcmd)
            return (rec_count == 0)
        
        return False

class CCheckorg_eci_jctv_link4_link5(platform.TestCase.CTestCase):
    def _do(self):
        if True == self.pg.IsExistTable('org_eci_jctv'):
            sqlcmd = """
                    select count(*)
                    from org_eci_jctv
                    where link5 is not null and link4 is null
                    """
            rec_count = self.pg.getOnlyQueryResult(sqlcmd)
            return (rec_count == 0)
        
        return False

class CCheckorg_eci_jctv_link5_link6(platform.TestCase.CTestCase):
    def _do(self):
        if True == self.pg.IsExistTable('org_eci_jctv'):
            sqlcmd = """
                    select count(*)
                    from org_eci_jctv
                    where link6 is not null and link5 is null
                    """
            rec_count = self.pg.getOnlyQueryResult(sqlcmd)
            return (rec_count == 0)
        
        return False