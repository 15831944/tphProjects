# -*- coding: UTF8 -*-
'''
Created on 2015-5-11

@author: liushengqiang
'''



import platform.TestCase

class CCheckRdb_Guideinfo_Caution_Client_TableStruct(platform.TestCase.CTestCase):
    def _do(self):
        return self.pg.IsExistTable('rdb_guideinfo_caution_client')

class CCheckRdb_Guideinfo_Caution_Client_in_link_id(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.guideinfo_id)
                    from rdb_guideinfo_caution_client a
                    left join rdb_guideinfo_caution b
                        on a.guideinfo_id = b.guideinfo_id
                    where a.in_link_id != (b.in_link_id & ((cast(1 as bigint) << 32) - 1))
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckRdb_Guideinfo_Caution_Client_out_link_id(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.guideinfo_id)
                    from rdb_guideinfo_caution_client a
                    left join rdb_guideinfo_caution b
                        on a.guideinfo_id = b.guideinfo_id
                    where a.out_link_id is not null and a.out_link_id != (b.out_link_id & ((cast(1 as bigint) << 32) - 1))
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckRdb_Guideinfo_Caution_Client_node_id(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.guideinfo_id)
                    from rdb_guideinfo_caution_client a
                    left join rdb_guideinfo_caution b
                        on a.guideinfo_id = b.guideinfo_id
                    where a.node_id != (b.node_id & ((cast(1 as bigint) << 32) - 1))
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckRdb_Guideinfo_Caution_Client_in_link_id_t(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.guideinfo_id)
                    from rdb_guideinfo_caution_client a
                    left join rdb_guideinfo_caution b
                        on a.guideinfo_id = b.guideinfo_id
                    where a.in_link_id_t != b.in_link_id_t
                """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckRdb_Guideinfo_Caution_Client_out_link_id_t(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.guideinfo_id)
                    from rdb_guideinfo_caution_client a
                    left join rdb_guideinfo_caution b
                        on a.guideinfo_id = b.guideinfo_id
                    where a.out_link_id_t is not null and a.out_link_id_t != b.out_link_id_t
                """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckRdb_Guideinfo_Caution_Client_node_id_t(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.guideinfo_id)
                    from rdb_guideinfo_caution_client a
                    left join rdb_guideinfo_caution b
                        on a.guideinfo_id = b.guideinfo_id
                    where a.node_id_t != b.node_id_t
                """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckRdb_Guideinfo_Caution_Client_passlink_count(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.guideinfo_id)
                    from rdb_guideinfo_caution_client a
                    left join rdb_guideinfo_caution b
                        on a.guideinfo_id = b.guideinfo_id
                    where a.passlink_count != b.passlink_count
                """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckRdb_Guideinfo_Caution_Client_data_kind(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.guideinfo_id)
                    from rdb_guideinfo_caution_client a
                    left join rdb_guideinfo_caution b
                        on a.guideinfo_id = b.guideinfo_id
                    where a.data_kind != b.data_kind
                """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckRdb_Guideinfo_Caution_Client_voice_id(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.guideinfo_id)
                    from rdb_guideinfo_caution_client a
                    left join rdb_guideinfo_caution b
                        on a.guideinfo_id = b.guideinfo_id
                    where a.voice_id is not null and a.voice_id != b.voice_id
                """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)
    
class CCheckRdb_Guideinfo_Caution_Client_strtts(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.guideinfo_id)
                    from rdb_guideinfo_caution_client a
                    left join rdb_guideinfo_caution b
                        on a.guideinfo_id = b.guideinfo_id
                    where a.strtts is not null and a.strtts != b.strtts
                """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)
    
class CCheckRdb_Guideinfo_Caution_Client_image_id(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.guideinfo_id)
                    from rdb_guideinfo_caution_client a
                    left join rdb_guideinfo_caution b
                        on a.guideinfo_id = b.guideinfo_id
                    where a.image_id is not null and a.image_id != b.image_id
                """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)