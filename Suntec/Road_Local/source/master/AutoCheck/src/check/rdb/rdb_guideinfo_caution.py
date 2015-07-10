# -*- coding: UTF8 -*-
'''
Created on 2015-5-11

@author: liushengqiang
'''



import platform.TestCase

class CCheckRdb_Guideinfo_Caution_TableStruct(platform.TestCase.CTestCase):
    def _do(self):
        return self.pg.IsExistTable('rdb_guideinfo_caution')
    
class CCheckRdb_Guideinfo_Caution_in_link_id(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.in_link_id)
                    from rdb_guideinfo_caution a
                    left join rdb_link b
                        on a.in_link_id = b.link_id
                    where b.link_id is null
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckRdb_Guideinfo_Caution_out_link_id(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.out_link_id)
                    from rdb_guideinfo_caution a
                    left join rdb_link b
                        on a.out_link_id = b.link_id
                    where b.link_id is null
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckRdb_Guideinfo_Caution_node_id(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.node_id)
                    from rdb_guideinfo_caution a
                    left join rdb_node b
                        on a.node_id = b.node_id
                    where b.node_id is null
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckRdb_Guideinfo_Caution_passlink_count(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(guideinfo_id)
                from rdb_guideinfo_caution
                where not (passlink_count >= 0 and passlink_count <= 20)
                """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckRdb_Guideinfo_Caution_in_link_id_t(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.guideinfo_id)
                    from rdb_guideinfo_caution a
                    left join rdb_link b
                        on a.in_link_id = b.link_id
                    where a.in_link_id_t != b.link_id_t
                """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckRdb_Guideinfo_Caution_out_link_id_t(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.guideinfo_id)
                    from rdb_guideinfo_caution a
                    left join rdb_link b
                        on a.out_link_id = b.link_id
                    where a.out_link_id_t != b.link_id_t
                """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckRdb_Guideinfo_Caution_node_id_t(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.guideinfo_id)
                    from rdb_guideinfo_caution a
                    left join rdb_node b
                        on a.node_id = b.node_id
                    where a.node_id_t != b.node_id_t
                """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckRdb_Guideinfo_Caution_in_link_id_out_link_id_diff(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(guideinfo_id)
                    from rdb_guideinfo_caution
                    where in_link_id = out_link_id
                """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)