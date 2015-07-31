# -*- coding: UTF8 -*-
'''
Created on 2013-12-26

@author: minghongtu
'''
import platform.TestCase

class CCheckSpotGuideInLinkType(platform.TestCase.CTestCase):
    
    def _do(self):
        
        sqlcmd = """
                select count(*)
                from spotguide_tbl as a left join link_tbl as b
                on a.inlinkid = b.link_id
                where b.link_type = 4 
                
                """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)
                
