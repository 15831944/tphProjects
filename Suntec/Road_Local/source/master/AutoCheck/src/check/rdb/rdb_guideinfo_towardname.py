# -*- coding: UTF8 -*-
'''
Created on 2012-12-10

@author: hongchenzai
'''

import rdb_common_check


class CCheckPassLinkCount_TN(rdb_common_check.CCheckPassLinkCount):
    '''检查TowardName的PassLink
    '''
    def __init__(self, suite, caseinfo):
        rdb_common_check.CCheckPassLinkCount.__init__(self, suite, caseinfo, 'towardname_tbl', 'passlink_cnt')
        pass

import platform.TestCase 
class CCheckNameID(platform.TestCase.CTestCase):
    '''检查[交叉点名称]的Name ID'''
    def _do(self):
        sqlcmd = """
                SELECT count(*)
                  from towardname_tbl as a
                  left join rdb_guideinfo_towardname as b
                  on a.gid = b.gid
                  left join temp_name_dictionary AS c
                  ON a.name_id = old_name_id
                  where new_name_id <> b.name_id;
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


class CCheckUnique(platform.TestCase.CTestCase): 
    '''检查结果是否有重复''' 
    
    def _do(self):    
        sqlcmd = '''
                select count(*)
                from
                (
                    select in_link_id,node_id,toward_name
                    from rdb_guideinfo_towardname as a
                    group by in_link_id, node_id, toward_name having count(*)>1
                ) as b
                         
                '''
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)          

        return (rec_count == 0 ) 
    

    
    
    
    
    


        