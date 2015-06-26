# -*- coding: UTF8 -*-
'''
Created on 2011-12-22

@author: liuxinxing
'''

import platform.TestCase

class CCheckLinkIDFkey(platform.TestCase.CTestCase):
    def _do(self):
   
        sqlcmd = """
                select * from (
                    SELECT a.link_id as linkid1,b.link_id as linkid2 FROM rdb_cond_speed_client a 
                    left join 
                    rdb_link_client b
                    on a.link_id=b.link_id and a.link_id_t=b.link_id_t
                ) as result
                where result.linkid2 is null;
                 """
        if self.pg.execute(sqlcmd) == -1:
            return 0
        else:
            if self.pg.getcnt() == 0:
                return 1
            else:
                return 0
            
class CCheckRecordCount(platform.TestCase.CTestCase):
    def _do(self):
        
        sqlcmd = """

                    select count(*) as count1 from rdb_cond_speed_client
                    union
                    select count(*) as count2 from rdb_cond_speed

                """

        if self.pg.execute(sqlcmd) == -1:
            return 0
        else:
            if self.pg.getcnt() == 1:
                return 1
            else:
                return 0
                
class CCheckLinkID(platform.TestCase.CTestCase):
    def _do(self):

            sqlcmd = """
                      SELECT count(*)as count 
                      FROM rdb_cond_speed_client as a
                      left join rdb_cond_speed as b
                      on b.link_id_t = a.link_id_t and (b.link_id & ((1::bigint << 32) - 1)) = a.link_id
                      where b.link_id is null;
                    """
            return 0 == self.pg.getOnlyQueryResult(sqlcmd)
                
class CCheckTileID(platform.TestCase.CTestCase):
    def _do(self):

            sqlcmd = """
                select count(*) from (
                    SELECT a.link_id_t as tile1,b.link_id_t as tile2
                    FROM rdb_cond_speed_client as a
                    left join rdb_cond_speed_client as b
                    on b.link_id_t = a.link_id_t and (b.link_id & ((1::bigint << 32) - 1)) = a.link_id
                ) as c
                where tile1 <> tile2;

                """

            self.pg.execute(sqlcmd)
            row = self.pg.fetchone()
            if row:
                if row[0] == 0:
                    return True
            return False
                
class CCheckPosSpeed(platform.TestCase.CTestCase):
    def _do(self):

            sqlcmd = """
                select count(*) from (
                    select a.link_id,a.pos_cond_speed as speed1,a.unit as unit1,(b.pos_multi_speed_attr & 4095) as speed2, ((b.pos_multi_speed_attr >> 12) & 1) as unit2
                    from rdb_cond_speed a
                    inner join 
                    rdb_cond_speed_client b
                    on b.link_id_t = a.link_id_t and (b.link_id & ((1::bigint << 32) - 1)) = a.link_id
                ) as c where speed1 <>  speed2 or unit1 <> unit2;

                """

            self.pg.execute(sqlcmd)
            row = self.pg.fetchone()
            if row:
                if row[0] == 0:
                    return True
            return False
            
class CCheckNegSpeed(platform.TestCase.CTestCase):
    def _do(self):

            sqlcmd = """

                select count(*) from (
                    select a.link_id,a.neg_cond_speed as speed1,a.unit as unit1,(b.neg_multi_speed_attr & 4095) as speed2, ((b.neg_multi_speed_attr >> 12) & 1) as unit2
                    from rdb_cond_speed a
                    inner join 
                    rdb_cond_speed_client b
                    on b.link_id_t = a.link_id_t and (b.link_id & ((1::bigint << 32) - 1)) = a.link_id
                ) as c where speed1 <>  speed2 or unit1 <> unit2;

                """

            self.pg.execute(sqlcmd)
            row = self.pg.fetchone()
            if row:
                if row[0] == 0:
                    return True
            return False