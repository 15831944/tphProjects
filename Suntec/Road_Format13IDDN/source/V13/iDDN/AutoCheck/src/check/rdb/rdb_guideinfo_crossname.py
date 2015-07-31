# -*- coding: UTF8 -*-
'''
Created on 2012-12-10
CrossName类
@author: hongchenzai
'''
import platform.TestCase

class CCheckNumber(platform.TestCase.CTestCase):
    '''检查[交叉点名称]的条目数。'''
    def _do(self):
        sqlcmd = """
                SELECT 
                 (
                 SELECT COUNT(gid)
                    from crossname_tbl
                    where namestr is not null
                 ) as mid_num,
                  (
                  SELECT COUNT(guideinfo_id)
                    from rdb_guideinfo_crossname
                    ) as rdb_num;
                """
                
        self.pg.query(sqlcmd)
        row = self.pg.fetchone()
        if row:
            if row[0] != row[1]:
                return False
            else:
                return True
        else:
            return False
   
class CCheckNameID(platform.TestCase.CTestCase):
    '''检查[交叉点名称]的Name ID'''
    def _do(self):
        sqlcmd = """
            SELECT count(a.*)
              FROM rdb_guideinfo_crossname as a
              left join temp_name_dictionary AS b
              ON a.name_id = new_name_id
              left join rdb_tile_node as c
              ON a.node_id = tile_node_id
              left join node_tbl as d
              on c.old_node_id = d.node_id
              where old_name_id <> d.name_id;
        """
        self.pg.query(sqlcmd)
        row = self.pg.fetchone()
        if row:
            if row[0] != 0:
                return False
            else:
                return True
        else:
            return False
        
