# -*- coding: UTF8 -*-
'''
Created on 2014-3-5

@author: tangpinghui
'''

import platform.TestCase

#确保元数据中org_jv_loaction表非空
#有效值检查、条目数检查、逻辑性检查
class CCheckJunctionViewNumber(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(*)
                from org_jv_location
                '''
        reg_number_count = self.pg.getOnlyQueryResult(sqlcmd)      
        return (reg_number_count > 0)
    
#确保
class CCheckJunctionViewType(platform.TestCase.CTestCase):
    def _do(self):    
        sqlcmd = '''
                select count(*)
                from org_area
                where kind not in(1,2,9,10);
                '''
        reg_type_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_type_count == 0)
    
class CCheckJunctionViewName(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(*)
                from org_area
                where "name" is null;
                '''
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)
class CCheckStateNumber(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(*)
                from org_state_region
                '''
        reg_number_count = self.pg.getOnlyQueryResult(sqlcmd)            
        return (reg_number_count > 0)
class CCheckStateType(platform.TestCase.CTestCase):
    def _do(self):    
        sqlcmd = '''
                select count(*)
                from org_state_region
                where kind <> 9
                '''
        reg_type_count = self.pg.getOnlyQueryResult(sqlcmd)
        
        return (reg_type_count == 0)
class CCheckStateName(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(*)
                from org_state_region
                where stt_nme is null;
                '''
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)
class CCheckStategeom(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(*)
                from org_state_region
                where ST_IsEmpty(the_geom) or (GeometryType(the_geom) <> 'MULTIPOLYGON');
                '''
        reg_count_state = self.pg.getOnlyQueryResult(sqlcmd)         
        return (reg_count_state == 0)
class CCheckStatetoArea(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(*)
                from
                (
                    select stt_id,parent_id,id
                    from
                    (
                        select stt_id,parent_id
                        from org_state_region
                        group by stt_id,parent_id
                    )as a
                    left join
                    (
                        select id
                        from org_area
                        where kind = 10
                        group by id
                    )as b
                    on a.parent_id = b.id
                ) as c
                where c.id is null;
                '''
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)
class CCheckDistrictNumber(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(*)
                from org_district_region
                '''
        reg_number_count = self.pg.getOnlyQueryResult(sqlcmd)       
        return (reg_number_count > 0)
class CCheckDistrictType(platform.TestCase.CTestCase):
    def _do(self):     
        sqlcmd = '''
                select count(*)
                from org_district_region
                where kind <> 2;
                '''
        reg_type_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_type_count == 0)
class CCheckDistrictName(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(*)
                from org_district_region
                where dst_nme is null;
                '''
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)
class CCheckDistrictgeom(platform.TestCase.CTestCase):
    def _do(self):     
        sqlcmd = '''
                select count(*)
                from org_district_region
                where ST_IsEmpty(the_geom) or (GeometryType(the_geom) <> 'MULTIPOLYGON');
                '''
        reg_count_district = self.pg.getOnlyQueryResult(sqlcmd)     
        return (reg_count_district == 0)
class CCheckDistrictToState(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(*)
                from
                (
                    select dst_id,parent_id,stt_id
                    from
                    (
                        select dst_id,parent_id
                        from org_district_region
                        group by dst_id,parent_id
                    )as a
                    left join
                    (
                        select stt_id
                        from org_state_region
                        group by stt_id
                    )as b
                    on a.parent_id = b.stt_id
                ) as c
                where c.stt_id is null;
                '''
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)
class CCheckTownNumber(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(*)
                from org_town_region
                '''
        reg_number_count = self.pg.getOnlyQueryResult(sqlcmd)       
        return (reg_number_count > 0)
class CCheckTownType(platform.TestCase.CTestCase):
    def _do(self):     
        sqlcmd = '''
                select count(*)
                from org_town_region
                where kind <> 2;
                '''
        reg_type_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_type_count == 0)
class CCheckLocalitiesNumber(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(*)
                from org_localities_region
                '''
        reg_number_count = self.pg.getOnlyQueryResult(sqlcmd)       
        return (reg_number_count > 0)
class CCheckLocalitiesType(platform.TestCase.CTestCase):
    def _do(self):     
        sqlcmd = '''
                select count(*)
                from org_localities_region
                where kind <> 1;
                '''
        reg_type_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_type_count == 0)    