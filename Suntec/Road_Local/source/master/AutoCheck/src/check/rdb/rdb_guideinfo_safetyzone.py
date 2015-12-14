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
                    array_agg(case direction when 3 then start_node_id else end_node_id end),
                    array_agg(a.gid) as gid_arr
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
            gid_arr=result[3]
            num=len(start_node_arr)
            if len(set(start_node_arr))<>len(start_node_arr) or len(set(end_node_arr))<>len(end_node_arr) \
                or len(set(start_node_arr)-set(end_node_arr))<>1:
                print result[0]
                return False
            node_id=list(set(start_node_arr)-set(end_node_arr))[0]
            idx=start_node_arr.index(node_id)
            gid=gid_arr[idx]
            for i in range(num-1):
                node_id_next=end_node_arr[idx]
                if start_node_arr.count(node_id_next)<>1:
                    print result[0]
                    return False
                idx=start_node_arr.index(node_id_next)
                if gid_arr[idx]<>gid+1:
                    print result[0]
                    return False
                else:
                    gid+=1
                node_id=node_id_next
                
        return True

class CCheckDrivingDirection(platform.TestCase.CTestCase):
    
    def _do(self):
        sqlcmd='''
                select count(*) from rdb_guideinfo_safety_zone a
                join rdb_link b
                using(link_id)
                where (a.direction=1 and b.one_way<>1
                    or a.direction=2 and b.one_way<>1 and b.one_way<>2
                    or a.direction=3 and b.one_way<>1 and b.one_way<>3) and a.safety_type<>3
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
    
class CCheckSchool_zone_safetyzone_id(platform.TestCase.CTestCase):
    
    def _do(self):
        sqlcmd = '''
                select count(*)
                from
                (
                    select a.link_id, a.safetyzone_id,a.speedlimit,
                           s_school.safetyzone_id as s_id, s_school.speedlimit as s_speed,
                           e_school.safetyzone_id as e_id, e_school.speedlimit as e_speed
                    from rdb_guideinfo_safety_zone as a
                    left join rdb_link as b
                    on a.link_id = b.link_id
                    left join rdb_link as s_link
                    on b.start_node_id in (s_link.start_node_id, s_link.end_node_id)
                    left join rdb_guideinfo_safety_zone as s_school
                    on s_link.link_id = s_school.link_id and s_school.safety_type = 3
                    left join rdb_link as e_link
                    on b.end_node_id in (e_link.start_node_id, e_link.end_node_id)
                    left join rdb_guideinfo_safety_zone as e_school
                    on e_link.link_id = e_school.link_id and e_school.safety_type = 3                
                    where a.safety_type = 3
                )temp
                where (s_id is not null and safetyzone_id <> s_id and speedlimit = s_speed) or 
                      (e_id is not null and safetyzone_id <> e_id and speedlimit = e_speed);
                '''
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)
    
class CCheckSafetyZone_Camera_Start(platform.TestCase.CTestCase):
    
    def _do(self):
        '所有起点camera都在safety zone第一条link上'
        sqlcmd = '''
            select * from rdb_guideinfo_safetyalert a
            left join (
                select link_id_array[1] as link_id
                from (
                    select safetyzone_id,array_agg(link_id) as link_id_array
                    from (
                        select * from rdb_guideinfo_safety_zone
                        order by safetyzone_id,gid
                    ) m 
                    group by safetyzone_id
                ) n
            ) b
            on a.link_id = b.link_id
            where a.type in (4)
            and b.link_id is null;
                '''
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)    
    
class CCheckSafetyZone_Camera_End(platform.TestCase.CTestCase):
    
    def _do(self):
        '所有终点camera都在safety zone最后一条link上'
        sqlcmd = '''
            select * from rdb_guideinfo_safetyalert a
            left join (
                select link_id_array[array_upper(link_id_array,1)] as link_id
                from (
                    select safetyzone_id,array_agg(link_id) as link_id_array
                    from (
                        select * from rdb_guideinfo_safety_zone
                        order by safetyzone_id,gid
                    ) m 
                    group by safetyzone_id
                ) n
            ) b
            on a.link_id = b.link_id
            where a.type in (5)
            and b.link_id is null;
                '''
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)
    
class CCheckSafetyZone_Distance(platform.TestCase.CTestCase):
    
    def _do(self):
        '所有dis不为-1的字段都是第一条link或者最后一条link'
        sqlcmd = '''
            select * from (
                select gid from rdb_guideinfo_safety_zone
                where dis != -1
            ) a
            left join (
                select gid_array[1] as gid
                from (
                    select safetyzone_id,array_agg(gid) as gid_array
                    from (
                        select * from rdb_guideinfo_safety_zone
                        order by safetyzone_id,gid
                    ) m 
                    group by safetyzone_id
                ) n
                union
                select gid_array[array_upper(gid_array,1)] as gid
                from (
                    select safetyzone_id,array_agg(gid) as gid_array
                    from (
                        select * from rdb_guideinfo_safety_zone
                        order by safetyzone_id,gid
                    ) m 
                    group by safetyzone_id
                ) n
                
            ) b
            on a.gid = b.gid
            where b.gid is null;
                '''
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)
    
class CCheckSafetyZone_Camera_Position(platform.TestCase.CTestCase):
    
    def _do(self):
        '所有safety zone的第一条link和最后一条link上肯定都有camera'
        sqlcmd = '''
            select * from (
                select link_id from rdb_guideinfo_safety_zone
                where dis != -1 and safety_type = 1
            ) a
            left join rdb_guideinfo_safetyalert b
            on a.link_id = b.link_id
            where b.link_id is null;
                '''
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)            