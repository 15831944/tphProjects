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

class CCheckName_kind_ni(platform.TestCase.CTestCase):
    '''检查  ni 数据   ，name_attr=IC/SA/PA/TOLL时，name_kind是否为 地点名称'''
    
    def _do(self):
        sqlcmd = '''
            select count(*) from 
            (
                select * from rdb_guideinfo_towardname
                where name_attr in (1,4,5,7)
            ) as a
            where a.name_kind <> 2             
        '''
        
        return self.pg.getOnlyQueryResult(sqlcmd) == 0
 
 
class CCheckName_attr_ni(platform.TestCase.CTestCase):
    '''检查  ni 数据   ，name_kind为 地点名称时，name_attr 是否为 IC/SA/PA/TOLL'''
    
    def _do(self):
        sqlcmd = '''
            select count(*) from 
            (
                select * from rdb_guideinfo_towardname
                where name_kind = 2
            ) as a
            where a.name_attr not in (1,4,5,7)            
        '''
        
        return self.pg.getOnlyQueryResult(sqlcmd) == 0 


class CCheckInlink_ni(platform.TestCase.CTestCase):
    '''检查 当name_attr=IC/SA/PA/TOLL时，inlink是否为高速道路 Link'''
    def _do(self):
        sqlcmd = '''
            select count(*) from
            (
                select * from
                (
                    select * from rdb_guideinfo_towardname
                    where name_attr in (1,4,5,7)
                ) as a
                left join rdb_link as b
                on a.in_link_id = b.link_id 
            ) as c
            where c.road_type not in (0,1) 
        '''
        
        return self.pg.getOnlyQueryResult(sqlcmd) == 0 


class CCheckToward_name(platform.TestCase.CTestCase):
    '''  toward_name为json格式，并且名称不为空'''
    def _do(self):
        
        sqlcmd = """
                SELECT toward_name
                FROM rdb_guideinfo_towardname
                where toward_name is not null;
                """
              
        for row in self.pg.get_batch_data(sqlcmd):
            toward_name = row[0]
            name_lists = json.loads(toward_name)
            for name_list in name_lists:
                for name in name_list:
#                    if name.get('tts_type') == 'not_tts' and (not name.get('val')):
                    if not name.get('val'):
                        return False
        return True  
 



    
    
    
    
    


        
