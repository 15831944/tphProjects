# -*- coding: cp936 -*-
'''
Created on 2015-5-5

@author:
'''



import platform.TestCase
import time

class CCheckOrg_C_Struct(platform.TestCase.CTestCase):
    def _do(self):
        return self.pg.IsExistTable('org_c') and self.pg.IsExistTable('org_cr')

class CCheckOrg_CondStruct(platform.TestCase.CTestCase):
    def _do(self):
        return self.pg.IsExistTable('org_cond') and self.pg.IsExistTable('org_cnl') and self.pg.IsExistTable('org_cr')
    
class CCheckOrg_C_Condtype(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(*)
                    from org_c
                    where condtype::bigint = 2
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec <> 0)

class CCheckOrg_Cond_Condtype(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(*)
                    from org_cond
                    where condtype::bigint = 2
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec <> 0)

class CCheckOrg_C_Mapid(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(*)
                    from org_c 
                    where 
                        condtype::bigint = 2 and 
                        mapid = ''
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckOrg_C_Condid(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(*)
                    from org_c 
                    where 
                        condtype::bigint = 2 and 
                        condid <> ''
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckOrg_C_Id(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(b.id)
                    from org_c a
                    left join org_n b
                        on 
                            a.mapid = b.mapid and
                            a.id = b.mainnodeid
                    where 
                        condtype::bigint = 2 and 
                        b.id is null
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckOrg_C_Inlinkid(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(b.id)
                    from org_c a
                    left join org_r b
                        on 
                            a.mapid = b.mapid and
                            a.inlinkid = b.id
                    where 
                        condtype::bigint = 2 and 
                        b.id is null
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckOrg_C_Outlinkid(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(b.id)
                    from org_c a
                    left join org_r b
                        on 
                            a.mapid = b.mapid and
                            a.outlinkid = b.id
                    where 
                        condtype::bigint = 2 and 
                        b.id is null
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)
    
class CCheckOrg_C_Crid(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(b.crid)
                    from org_c a
                    left join org_cr b
                        on a.crid = b.crid
                    where 
                        condtype::bigint = 2 and 
                        a.crid <> '' and
                        b.crid = ''
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)
    
class CCheckOrg_C_Passage(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(*)
                    from org_c 
                    where 
                        condtype::bigint = 2 and 
                        passage <> ''
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)
    
class CCheckOrg_C_Slope(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(*)
                    from org_c 
                    where 
                        condtype::bigint = 2 and 
                        slope <> ''
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)
    
class CCheckOrg_C_Sgnl_Loction(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(*)
                    from org_c 
                    where 
                        condtype::bigint = 2 and 
                        sgnl_loction <> ''
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckOrg_Cond_Mapid(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(*)
                    from org_cond 
                    where 
                        condtype::bigint = 2 and 
                        mapid = ''
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckOrg_Cond_Condid(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(*)
                    from org_cond 
                    where 
                        condtype::bigint = 2 and 
                        condid = ''
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckOrg_Cond_Crid(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(b.crid)
                    from org_cond a
                    left join org_cr b
                        on a.crid = b.crid
                    where 
                        condtype::bigint = 2 and 
                        a.crid <> '' and
                        b.crid = ''
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckOrg_Cond_Passage(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(*)
                    from org_cond 
                    where 
                        condtype::bigint = 2 and 
                        passage <> ''
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckOrg_Cond_Slope(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(*)
                    from org_cond 
                    where 
                        condtype::bigint = 2 and 
                        slope <> ''
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckOrg_Cond_Sgnl_Loction(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(*)
                    from org_cond 
                    where 
                        condtype::bigint = 2 and 
                        sgnl_loction <> ''
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckOrg_Cnl_Mapid(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(*)
                    from org_cnl 
                    where mapid = ''
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckOrg_Cnl_Condid(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(b.condid)
                    from org_cnl a
                    left join org_cond b
                        on 
                            a.mapid = b.mapid and
                            a.condid = b.condid 
                    where b.condid = ''
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckOrg_Cnl_Linkid(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(b.id)
                    from org_cnl a
                    left join org_r b
                        on 
                            a.mapid = b.mapid and
                            a.linkid = b.id 
                    where 
                        a.linkid <> '' and 
                        b.id = ''
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckOrg_Cnl_Nodeid(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(b.id)
                    from org_cnl a
                    left join org_n b
                        on 
                            a.mapid = b.mapid and
                            a.nodeid = b.id 
                    where 
                        a.nodeid <> '' and 
                        b.id = ''
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckOrg_Cnl_Seq_Nm(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(*)
                    from org_cnl 
                    where seq_nm = ''
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckOrg_Cnl_Angle(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(*)
                    from org_cnl 
                    where 
                        (seq_nm::bigint in (1, 2) and angle <> '') or
                        (seq_nm::bigint not in (1, 2) and angle = '')
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckForceGuideTableStruct(platform.TestCase.CTestCase):
    def _do(self):
        return self.pg.IsExistTable('force_guide_tbl')

class CCheckForce_Guide_Tbl_Force_Guide_Id(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(gid)
                    from force_guide_tbl
                    where force_guide_id is null
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)
    
class CCheckForce_Guide_Tbl_Nodeid(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.nodeid)
                    from force_guide_tbl as a
                    left join node_tbl as b
                        on a.nodeid = b.node_id
                    where b.node_id is null
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckForce_Guide_Tbl_Inlinkid(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.inlinkid)
                    from force_guide_tbl as a
                    left join link_tbl as b
                        on a.inlinkid = b.link_id
                    where b.link_id is null
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckForce_Guide_Tbl_Inlinkid_Nodeid_Rel(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from force_guide_tbl as a
                left join link_tbl as b
                    on a.inlinkid = b.link_id
                where a.nodeid not in (b.s_node, b.e_node)
                """
        nRecCount = self.pg.getOnlyQueryResult(sqlcmd)
        return (nRecCount == 0)

class CCheckForce_Guide_Tbl_Outlinkid(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.outlinkid)
                    from force_guide_tbl as a
                    left join link_tbl as b
                        on a.outlinkid = b.link_id
                    where b.link_id is null
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckForce_Guide_Tbl_PassLink_Cnt(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from
                (
                    select (case 
                            when string_to_array(passlid, '|') is null then 0
                            else array_upper(string_to_array(passlid, '|'),1)
                            end
                           ) as lenth, passlink_cnt
                    FROM force_guide_tbl
                )as a
                where a.lenth <> passlink_cnt
                """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)
    
class CCheckForce_Guide_Tbl_Guide_Type(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(force_guide_id)
                from force_guide_tbl
                where 
                    not (guide_type >= 0 and guide_type <= 13) and 
                    not (guide_type = 255) and 
                    not (guide_type >= 32 and guide_type <= 43) and
                    not (guide_type >= 48 and guide_type <= 59) and 
                    not (guide_type >= 64 and guide_type <= 75) and 
                    not (guide_type >= 80 and guide_type <= 248)
                """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckForce_Guide_Tbl_Position_Type(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(force_guide_id)
                from force_guide_tbl
                where not (position_type >= 0 and position_type <= 9)
                """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckForce_Guide_Tbl_Inlinkid_Outlinkid_Equal(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(inlinkid)
                from force_guide_tbl
                where inlinkid = outlinkid
                """
        nRecCount = self.pg.getOnlyQueryResult(sqlcmd)
        return (nRecCount == 0)

class CCheckForce_Guide_Tbl_Inlink_Outlink_Conn(platform.TestCase.CTestCase):
    '''check guide 表单的node_id的有效性，inlink与outlink连通性'''
    
    def _do(self): 
        try:
            if self.pg.CreateFunction_ByName('mid_check_force_guide_links') == -1:
                time.sleep(60)
        except:
            self.pg.rollback()
            pass
                    
        sqlcmd = """
            select count(*) 
            from (
                select 
                    inlinkid,
                    one_way_code,
                    nodeid,outlinkid,
                    passlink_cnt,
                    dir1,
                    dir2,
                    mid_check_force_guide_links(inlinkid,nodeid,passlink_cnt,outlinkid,dir1::smallint) as pass_flag1,
                    case 
                        when dir2 is not null then mid_check_force_guide_links(inlinkid,nodeid,passlink_cnt,outlinkid,dir2::smallint) 
                    end as pass_flag2
                from (
                    select  
                        inlinkid,
                        b.one_way_code, 
                        nodeid,
                        outlinkid,
                        passlink_cnt,
                        case
                            when b.one_way_code = 1 and a.nodeid = b.s_node then 0
                            when b.one_way_code = 1 and a.nodeid = b.e_node then 1
                            when b.one_way_code  = 2 and a.nodeid = b.s_node then 0
                            when b.one_way_code  = 2 and a.nodeid = b.e_node then 1
                            when b.one_way_code  = 3 and a.nodeid = b.s_node then 1
                            when b.one_way_code  = 3 and a.nodeid = b.e_node then 0
                            else 0
                        end as dir1,
                        case 
                            when b.one_way_code = 1 and a.nodeid = b.s_node then 1
                            when b.one_way_code = 1 and a.nodeid = b.e_node then 0
                            else null
                        end as dir2
                     from force_guide_tbl a
                     left join link_tbl b
                         on a.inlinkid = b.link_id
                     where b.link_id is not null
                ) c
            ) d 
            where pass_flag1 is false and pass_flag2 is false
        """
        # 替换表名
        sqlcmd = sqlcmd.replace('[replace_table]', self.caseinfo.getTableName())
        
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row:
            if row[0] != 0:
                return False
            else:
                return True
        else:
            return False