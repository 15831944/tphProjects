# -*- coding: UTF8 -*-
'''
Created on 2015-5-11

@author: liushengqiang
'''



import platform.TestCase
import time

class CCheckRdb_Guideinfo_Caution_TableStruct(platform.TestCase.CTestCase):
    def _do(self):
        return self.pg.IsExistTable('rdb_guideinfo_caution')
    
class CCheckRdb_Guideinfo_Caution_in_link_id(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(guideinfo_id)
                    from rdb_guideinfo_caution a
                    left join rdb_link b
                        on in_link_id = b.link_id
                    where 
                        (in_link_id is null) or
                        (in_link_id is not null and b.link_id is null)
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckRdb_Guideinfo_Caution_out_link_id(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(guideinfo_id)
                    from rdb_guideinfo_caution a
                    left join rdb_link b
                        on out_link_id = b.link_id
                    where out_link_id is not null and b.link_id is null
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckRdb_Guideinfo_Caution_out_link_id_ni(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(guideinfo_id)
                    from rdb_guideinfo_caution
                    where out_link_id is not null
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckRdb_Guideinfo_Caution_node_id(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(guideinfo_id)
                    from rdb_guideinfo_caution a
                    left join rdb_node b
                        on a.node_id = b.node_id
                    where 
                        (a.node_id is null) or
                        (a.node_id is not null and b.node_id is null)
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
                    select count(guideinfo_id)
                    from rdb_guideinfo_caution a
                    left join rdb_link b
                        on in_link_id = link_id
                    where in_link_id_t != link_id_t
                """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckRdb_Guideinfo_Caution_out_link_id_t(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(guideinfo_id)
                    from rdb_guideinfo_caution a
                    left join rdb_link b
                        on out_link_id = b.link_id
                    where out_link_id is not null and out_link_id_t != b.link_id_t
                """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckRdb_Guideinfo_Caution_out_link_id_t_ni(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(guideinfo_id)
                    from rdb_guideinfo_caution
                    where out_link_id is null and out_link_id_t is not null
                """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckRdb_Guideinfo_Caution_node_id_t(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(guideinfo_id)
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
                    where out_link_id is not null and in_link_id = out_link_id
                """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)
    
class CCheckRdb_Guideinfo_Caution_out_link_id_passlink_count(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(guideinfo_id)
                    from rdb_guideinfo_caution
                    where out_link_id is null and passlink_count != 0
                """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckRdb_Guideinfo_Caution_data_kind(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(guideinfo_id)
                    from rdb_guideinfo_caution
                    where data_kind not in (0, 1, 2, 3, 4, 5)
                """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckRdb_Guideinfo_Caution_voice_id(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(guideinfo_id)
                    from rdb_guideinfo_caution
                    where 
                        (voice_id is null) or
                        (voice_id is not null and voice_id = 0)
                """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckRdb_Guideinfo_Caution_image_id(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(guideinfo_id)
                    from rdb_guideinfo_caution
                    where 
                        (image_id is null) or
                        (image_id is not null and image_id = 0)
                """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckRdb_Guideinfo_Caution_strtts(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(*)
                    from rdb_guideinfo_caution
                    where strtts is not null and strtts not like '%"tts_type": "phoneme"%'
                """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckRdb_Guideinfo_Caution_strtts_ni(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(*)
                    from rdb_guideinfo_caution
                    where strtts is null
                """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckRdb_Guideinfo_Caution_in_link_id_out_link_id_connect(platform.TestCase.CTestCase):
    def _do(self): 
        try:
            if self.pg.CreateFunction_ByName('rdb_check_guide_links') == -1:
                time.sleep(60)
        except:
            self.pg.rollback()
            pass
                    
        sqlcmd = """
            select count(*) 
            from (
                select 
                    in_link_id,
                    one_way,
                    node_id,
                    out_link_id,
                    passlink_count,
                    dir1,
                    dir2,
                    rdb_check_guide_links(in_link_id, node_id, passlink_count, out_link_id, dir1::smallint) as pass_flag1,
                    case 
                        when dir2 is not null then rdb_check_guide_links(in_link_id, node_id, passlink_count, out_link_id, dir2::smallint) 
                    end as pass_flag2
                from (
                    select  
                        in_link_id,
                        one_way, 
                        node_id,
                        out_link_id,
                        passlink_count,
                        case
                            when one_way = 1 and node_id = start_node_id then 0
                            when one_way = 1 and node_id = end_node_id then 1
                            when one_way  = 2 and node_id = start_node_id then 0
                            when one_way  = 2 and node_id = end_node_id then 1
                            when one_way  = 3 and node_id = start_node_id then 1
                            when one_way  = 3 and node_id = end_node_id then 0
                            else 0
                        end as dir1,
                        case 
                            when one_way = 1 and node_id = start_node_id then 1
                            when one_way = 1 and node_id = end_node_id then 0
                            else null
                        end as dir2
                     from rdb_guideinfo_caution a
                     left join rdb_link b
                         on in_link_id = link_id
                     where link_id is not null
                ) c
            ) d 
            where pass_flag1 is false and pass_flag2 is false
        """
        
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