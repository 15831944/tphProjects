# -*- coding: UTF8 -*-
'''
Created on 2015-5-7

@author: zhaojie
'''

import platform.TestCase

class CCheckgradValues(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(*)
                from grad
                where grad_value not in(-40,-30,-20,-10,0,10,20,30,40);
                '''
        reg_number_count = self.pg.getOnlyQueryResult(sqlcmd)      

        return (reg_number_count == 0)
    
class CCheckgradLogic(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(*)
                from grad as a
                join link_tbl as b
                on a.link_id = b.link_id and not ST_Contains(b.the_geom, a.shape);
                '''
        reg_number_count = self.pg.getOnlyQueryResult(sqlcmd)      

        return (reg_number_count == 0)