# -*- coding: UTF8 -*-
'''
Created on 2015-3-19

@author: zhaojie
'''
import platform.TestCase
from check.rdb import rdb_common_check

class CCheckPassLinkCount(platform.TestCase.CTestCase):
    '''check PassLink'''       
    def _do(self):
        sqlcmd = """
                select count(*)
                  from hook_turn_tbl
                  left join rdb_guideinfo_hook_turn
                  ON hook_turn_tbl.gid = rdb_guideinfo_hook_turn.guideinfo_id
                  where passlink_cnt <> passlink_count;
                """
        
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row:
            if row[0] != 0:
                return False
            else:
                return True
        else:
            return False