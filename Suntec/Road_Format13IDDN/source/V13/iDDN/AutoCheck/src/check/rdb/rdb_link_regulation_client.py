# -*- coding: UTF8 -*-
'''
Created on 2011-12-22

@author: liuxinxing
'''

import platform.TestCase

class CCheckTableStruct(platform.TestCase.CTestCase):
    def _do(self):
        return self.pg.IsExistTable('rdb_link_regulation_client')

class CCheckLastLinkIDEffective(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.last_link_id)
                    from rdb_link_regulation_client as a
                    left join rdb_link_client as b
                    on a.last_link_id = b.link_id
                    where b.link_id is null
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckRegulationNumber(platform.TestCase.CTestCase):
    def _do(self):
        srv_count = self.pg.getOnlyQueryResult("select count(record_no) from rdb_link_regulation")
        clt_count = self.pg.getOnlyQueryResult("select count(record_no) from rdb_link_regulation_client")
        return (srv_count == clt_count)

class CCheckRegulationType(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.record_no)
                    from rdb_link_regulation_client as a
                    left join rdb_link_regulation as b
                    on a.record_no = b.record_no
                    where a.regulation_type != b.regulation_type
                """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckLastLinkID(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.record_no)
                    from rdb_link_regulation_client as a
                    left join rdb_link_regulation as b
                    on a.record_no = b.record_no
                    where a.last_link_id != (b.last_link_id & ((cast(1 as bigint) << 32) - 1))
                """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckLastTileID(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.record_no)
                    from rdb_link_regulation_client as a
                    left join rdb_link_regulation as b
                    on a.record_no = b.record_no
                    where a.last_link_id_t != b.last_link_id_t
                """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckLastLinkDir(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.record_no)
                    from rdb_link_regulation_client as a
                    left join rdb_link_regulation as b
                    on a.record_no = b.record_no
                    where a.last_link_dir != b.last_link_dir
                """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckLinknum(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.record_no)
                    from rdb_link_regulation_client as a
                    left join rdb_link_regulation as b
                    on a.record_no = b.record_no
                    where a.link_num != b.link_num
                """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckKeyString(platform.TestCase.CTestCase):
    def _do(self):
        self.pg.CreateFunction_ByName('rdb_reconvert_keystring')
        sqlcmd = """
                    select count(a.record_no)
                    from rdb_link_regulation_client as a
                    left join rdb_link_regulation as b
                    on a.record_no = b.record_no
                    where rdb_reconvert_keystring(a.key_string) != b.key_string
                """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)
