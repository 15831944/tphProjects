# -*- coding: UTF8 -*-
'''
Created on 2015-3-19

@author: zhengchao
'''
import platform.TestCase
from check.rdb import rdb_common_check

class CCheckSafetyZoneLinkID(platform.TestCase.CTestCase):
    
    def _do(self):
        
        sqlcmd = """
                    select count(a.link_id)
                    from rdb_guideinfo_safety_zone as a
                    left join rdb_link as b
                    on a.link_id = b.link_id
                    where b.link_id is null
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CChecklinkidTileSame(platform.TestCase.CTestCase):
    def _do(self):
        checkobject = rdb_common_check.CCheckItemTileIDSame(self.pg, 'rdb_guideinfo_safety_zone', 'link_id', 'link_id_t')
        return checkobject.do()

def check_values_sql(strcolumn, str_values):
    sqlcmd = """
            select count(*)
            from rdb_guideinfo_safety_zone
            where [column_name] not in [values_array]
         """
    sqlcmd = sqlcmd.replace('[column_name]', strcolumn)
    sqlcmd = sqlcmd.replace('[values_array]', str_values)
    
    return sqlcmd

class CCheckspeedunit_code_values(platform.TestCase.CTestCase):
    
    def _do(self):
        count_rec = self.pg.getOnlyQueryResult(check_values_sql('speedunit_code', '(0,1,2)'))
        return (count_rec == 0)
class CCheckdirection_values(platform.TestCase.CTestCase):
    
    def _do(self):
        count_rec = self.pg.getOnlyQueryResult(check_values_sql('direction', '(1,2,3)'))
        return (count_rec == 0)
class CChecksafety_type_values(platform.TestCase.CTestCase):
    
    def _do(self):
        count_rec = self.pg.getOnlyQueryResult(check_values_sql('safety_type', '(0,1,2,3)'))
        return (count_rec == 0)
    
class CCheckspeedlimit_values(platform.TestCase.CTestCase):
    
    def _do(self):
        
        sqlcmd = """
            select count(*)
            from rdb_guideinfo_safety_zone
            where (speedlimit <= 0 or speedlimit > 260) and safety_type in (1,3)
            
            union
            select count(*)
            from rdb_guideinfo_safety_zone
            where (speedlimit < 0 or speedlimit > 260) and safety_type = 2           
            
         """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckLinkSequence(platform.TestCase.CTestCase):
    
    def _do(self):
        sqlcmd = '''
            select safetyzone_id,array_agg(case direction when 2 then start_node_id else end_node_id end),
                    array_agg(case direction when 3 then start_node_id else end_node_id end)
            from rdb_guideinfo_safety_zone a 
            join rdb_link b using(link_id)
            where safety_type<>3
            group by safetyzone_id
            order by safetyzone_id
                '''
        results=self.pg.get_batch_data(sqlcmd)
        for result in results:
            start_node_arr=result[1]
            end_node_arr=result[2]
            num=len(start_node_arr)
            if len(set(start_node_arr))<>len(start_node_arr) or len(set(end_node_arr))<>len(end_node_arr) \
                or len(set(start_node_arr)-set(end_node_arr))<>1:
                return False
            node_id=list(set(start_node_arr)-set(end_node_arr))[0]
            idx=start_node_arr.index(node_id)
            for i in range(num-1):
                node_id_next=end_node_arr[idx]
                if node_id_next not in start_node_arr:
                    return False
                idx=start_node_arr.index(node_id_next)
                node_id=node_id_next
        return True

class CCheckDrivingDirection(platform.TestCase.CTestCase):
    
    def _do(self):
        sqlcmd='''
                select count(*) from rdb_guideinfo_safety_zone a
                join rdb_link b
                using(link_id)
                where a.direction=1 and b.one_way<>1
                    or a.direction=2 and b.one_way<>1 and b.one_way<>2
                    or a.direction=3 and b.one_way<>1 and b.one_way<>3
                '''
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

    
class CCheckTableCount(platform.TestCase.CTestCase):
    
    def _do(self):
        
        sqlcmd = '''
            select count(1) from rdb_guideinfo_safety_zone
                '''
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec > 0)