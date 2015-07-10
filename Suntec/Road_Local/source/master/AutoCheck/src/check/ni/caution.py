# -*- coding: cp936 -*-
'''
Created on 2015-5-5

@author:
'''



import platform.TestCase

class CCheckOrg_Trfcsign_TableStruct(platform.TestCase.CTestCase):
    def _do(self):
        return self.pg.IsExistTable('org_trfcsign') and self.pg.IsExistTable('org_cr')
    
class CCheckOrg_Trfcsign_mapid(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(*)
                    from org_trfcsign
                    where mapid = ''
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckOrg_Trfcsign_inlinkid(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(gid)
                    from org_trfcsign
                    where inlinkid = ''
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)
    
class CCheckOrg_Trfcsign_mapid_inlinkid(platform.TestCase.CTestCase):
    def _do(self):
        self.pg.CreateIndex_ByName('org_trfcsign_mapid_inlinkid_idx')
        self.pg.CreateIndex_ByName('org_r_mapid_id_idx')
        
        sqlcmd = """
                    select count(b.id)
                    from org_trfcsign a
                    left join org_r b
                        on 
                            a.mapid::bigint = b.mapid::bigint and
                            a.inlinkid::bigint = id::bigint
                    where b.id is null
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckOrg_Trfcsign_nodeid(platform.TestCase.CTestCase):
    def _do(self):
        self.pg.CreateIndex_ByName('org_trfcsign_mapid_nodeid_idx')
        self.pg.CreateIndex_ByName('org_n_mapid_id_idx')
        
        sqlcmd = """
                    select count(gid)
                    from org_trfcsign
                    where nodeid = ''
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)
    
class CCheckOrg_Trfcsign_mapid_nodeid(platform.TestCase.CTestCase):
    def _do(self):
        self.pg.CreateIndex_ByName('org_trfcsign_mapid_nodeid_idx')
        self.pg.CreateIndex_ByName('org_n_mapid_id_idx')
        
        sqlcmd = """
                    select count(b.id)
                    from org_trfcsign a
                    left join org_n b
                        on 
                            a.mapid::bigint = b.mapid::bigint and
                            a.nodeid::bigint = id::bigint
                    where b.id is null
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)
    
class CCheckOrg_Trfcsign_type(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(type)
                    from org_trfcsign
                    where type::bigint < 1 or type::bigint > 47
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)
    
class CCheckOrg_Trfcsign_validdist(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(validdist)
                    from org_trfcsign
                    where validdist::bigint < 0 or validdist::bigint > 4294967295
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)
    
class CCheckOrg_Trfcsign_predist(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(predist)
                    from org_trfcsign
                    where predist::bigint < 0 or predist::bigint > 4294967295
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)
    
class CCheckOrg_Trfcsign_crid(platform.TestCase.CTestCase):
    def _do(self):
        self.pg.CreateIndex_ByName('org_trfcsign_crid_idx')
        self.pg.CreateIndex_ByName('org_cr_crid_idx')
        
        sqlcmd = """
                    select count(b.crid)
                    from org_trfcsign a
                    left join org_cr b
                        on a.crid::bigint = b.crid::bigint
                    where a.crid <> '' and b.crid is null
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)
    
class CCheckOrg_Trfcsign_inlinkid_nodeid_Rel(platform.TestCase.CTestCase):
    def _do(self):
        self.pg.CreateIndex_ByName('org_trfcsign_mapid_inlinkid_idx')
        self.pg.CreateIndex_ByName('org_r_mapid_id_idx')
        
        sqlcmd = """
                    select count(b.id)
                    from org_trfcsign a
                    left join org_r b
                        on 
                            a.mapid::bigint = b.mapid::bigint and
                            a.inlinkid::bigint = b.id::bigint
                    where a.nodeid not in (b.snodeid, b.enodeid)
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckCautionTableStruct(platform.TestCase.CTestCase):
    def _do(self):
        return self.pg.IsExistTable('caution_tbl')

class CCheckCaution_Tbl_Id(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(gid)
                    from caution_tbl
                    where id is null
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckCaution_Tbl_Inlinkid(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.inlinkid)
                    from caution_tbl as a
                    left join link_tbl as b
                        on a.inlinkid = b.link_id
                    where b.link_id is null
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)
    
class CCheckCaution_Tbl_Nodeid(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.nodeid)
                    from caution_tbl as a
                    left join node_tbl as b
                        on a.nodeid = b.node_id
                    where b.node_id is null
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckCaution_Tbl_Inlinkid_Nodeid_Rel(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from caution_tbl as a
                left join link_tbl as b
                    on a.inlinkid = b.link_id
                where a.nodeid not in (b.s_node, b.e_node)
                """
        nRecCount = self.pg.getOnlyQueryResult(sqlcmd)
        return (nRecCount == 0)

class CCheckCaution_Tbl_Outlinkid(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.outlinkid)
                    from caution_tbl as a
                    left join link_tbl as b
                        on a.outlinkid = b.link_id
                    where a.outlinkid is not null and b.link_id is null
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckCaution_Tbl_PassLink_Cnt(platform.TestCase.CTestCase):
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
                    FROM caution_tbl
                )as a
                where a.lenth <> passlink_cnt
                """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)

class CCheckCaution_Tbl_Data_Kind(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(id)
                from caution_tbl
                where 
                    not (data_kind >= 0 and data_kind <= 5) 
                """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)