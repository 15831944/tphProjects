# -*- coding: UTF8 -*-
'''
Created on 2013-3-21

@author: hongchenzai
'''

import platform.TestCase

class CCheckRdbLinkAttriView(platform.TestCase.CTestCase):
    '''check rdb_link_with_all_attri_view: 条目'''
    def _do(self):
        sqlcmd = """
        select 
            (SELECT count(gid)
              FROM rdb_link_with_all_attri_view ) as view_link_num,
            (SELECT count(gid) from rdb_link) as link_num
        """

        self.pg.query(sqlcmd)
        row = self.pg.fetchone()
        if row:
            if row[0] != row[1]:
                self.logger.error('Number of rdb_link_with_all_attri_view.')
                return False
            else:
                pass
        else:
            return False
            
        return True
    
class CCheckRdbNodeAttriView(platform.TestCase.CTestCase):
    '''check rdb_node_with_all_attri_view: 条目'''
    def _do(self):
        sqlcmd = """
        select 
            (SELECT count(gid)
              FROM rdb_node_with_all_attri_view ) as view_node_num,
            (SELECT count(gid) from rdb_node) as node_num
        """

        self.pg.query(sqlcmd)
        row = self.pg.fetchone()
        if row:
            if row[0] != row[1]:
                self.logger.error('Number of rdb_node_with_all_attri_view.')
                return False
            else:
                pass
        else:
            return False
            
        return True