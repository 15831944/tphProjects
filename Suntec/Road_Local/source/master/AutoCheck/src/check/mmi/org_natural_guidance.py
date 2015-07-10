# -*- coding: UTF8 -*-

import platform.TestCase
import time

class CCheckLongitude(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_natural_guidance
                where longitude is null or 
                      (longitude is not null and not (longitude >= 0 and longitude <= 180))
                """
        nRecCount = self.pg.getOnlyQueryResult(sqlcmd)
        return (nRecCount == 0)

class CCheckLatitude(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_natural_guidance
                where latitude is null or 
                      (latitude is not null and not (latitude >= 0 and latitude <= 90))
                """
        nRecCount = self.pg.getOnlyQueryResult(sqlcmd)
        return (nRecCount == 0)

class CCheckPoi_nme(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_natural_guidance
                where poi_nme is null
                """
        nRecCount = self.pg.getOnlyQueryResult(sqlcmd)
        return (nRecCount == 0)
   
class CCheckPoi_prop(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_natural_guidance
                where poi_prop is not null
                """
        nRecCount = self.pg.getOnlyQueryResult(sqlcmd)
        return (nRecCount == 0)
    
class CCheckPoi_bind_link_count(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from (
                    select distinct longitude, latitude, poi_nme, poi_prop
                    from org_natural_guidance
                ) as a
                """
        nRawRecCount = self.pg.getOnlyQueryResult(sqlcmd)
        
        sqlcmd = """
                select count(*)
                from (
                    select distinct longitude, latitude, poi_nme, poi_prop
                    from org_natural_guidance a 
                    left join org_city_nw_gc_polyline b
                        on ST_DWithin(ST_SetSRID(ST_MakePoint(a.longitude, a.latitude), 4326), b.the_geom, 0.000009)
                ) as c
                """
        nAfterBindRecCount = self.pg.getOnlyQueryResult(sqlcmd)
        return (nRawRecCount == nAfterBindRecCount)
    
class CCheckNodeidValidate(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from natural_guidence_tbl as a
                left join node_tbl as b
                on a.nodeid = b.node_id
                where b.node_id is null
                """
        nRecCount = self.pg.getOnlyQueryResult(sqlcmd)
        return (nRecCount == 0)

class CCheckNodeidIsEffective(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from natural_guidence_tbl as a
                left join link_tbl as b
                on a.inlinkid = b.link_id
                where a.nodeid not in (b.s_node, b.e_node)
                """
        nRecCount = self.pg.getOnlyQueryResult(sqlcmd)
        return (nRecCount == 0)
    
class CCheckInlinkidValidate(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from natural_guidence_tbl as a
                left join link_tbl as b
                on a.inlinkid = b.link_id
                where b.link_id is null
                """
        nRecCount = self.pg.getOnlyQueryResult(sqlcmd)
        return (nRecCount == 0)
    
class CCheckInlinkTypeValidate(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from natural_guidence_tbl as a
                left join link_tbl as b
                on a.inlinkid = b.link_id
                where b.link_type = 4
                """
        nRecCount = self.pg.getOnlyQueryResult(sqlcmd)
        return (nRecCount == 0)
    
class CCheckOutlinkidValidate(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from natural_guidence_tbl as a
                left join link_tbl as b
                on a.outlinkid = b.link_id
                where b.link_id is null
                """
        nRecCount = self.pg.getOnlyQueryResult(sqlcmd)
        return (nRecCount == 0)
    
class CCheckOutlinkTypeValidate(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from natural_guidence_tbl as a
                left join link_tbl as b
                on a.outlinkid = b.link_id
                where b.link_type = 4
                """
        nRecCount = self.pg.getOnlyQueryResult(sqlcmd)
        return (nRecCount == 0)

class CCheckPassLinkCnt(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from
                (
                    select (case 
                            when string_to_array(passlid, '|') is null then 0
                            else array_upper(string_to_array(passlid, '|'),1)
                            end
                           ) as lenth, passlink_cnt
                    FROM natural_guidence_tbl
                )as a
                where a.lenth <> passlink_cnt
                """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)
    
class CCheckFeatPositionValidate(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from natural_guidence_tbl
                where feat_position not in (1, 2, 3)
                """
        nRecCount = self.pg.getOnlyQueryResult(sqlcmd)
        return (nRecCount == 0)

class CCheckFeatImportanceValidate(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from natural_guidence_tbl
                where feat_importance <> 0
                """
        nRecCount = self.pg.getOnlyQueryResult(sqlcmd)
        return (nRecCount == 0)
    
class CCheckPrepositionCodeValidate(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from natural_guidence_tbl
                where preposition_code not in (1, 3, 5)
                """
        nRecCount = self.pg.getOnlyQueryResult(sqlcmd)
        return (nRecCount == 0)

class CCheckFeatName(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from natural_guidence_tbl
                where feat_name is null
                """
        nRecCount = self.pg.getOnlyQueryResult(sqlcmd)
        return (nRecCount == 0)
    
class CCheckConditionIdValidate(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from natural_guidence_tbl
                where condition_id <> 0
                """
        nRecCount = self.pg.getOnlyQueryResult(sqlcmd)
        return (nRecCount == 0)
    
class CCheckInlinkOutlinkEqual(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from natural_guidence_tbl
                where inlinkid = outlinkid
                """
        nRecCount = self.pg.getOnlyQueryResult(sqlcmd)
        return (nRecCount == 0)

class CCheckInlinkOutlinkConnectivity(platform.TestCase.CTestCase):
    '''check guide 表单的node_id的有效性，inlink与outlink连通性'''
    
    def _do(self): 
        try:
            if self.pg.CreateFunction_ByName('mid_check_natural_guidence_links') == -1:
                time.sleep(60)
        except:
            self.pg.rollback()
            pass
                    
        sqlcmd = """
            select count(*) 
            from (
                select 
                    inlinkid,
                    one_way_code,
                    nodeid,outlinkid,
                    passlink_cnt,
                    dir1,
                    dir2,
                    mid_check_natural_guidence_links(inlinkid,nodeid,passlink_cnt,outlinkid,dir1::smallint) as pass_flag1,
                    case 
                        when dir2 is not null then mid_check_natural_guidence_links(inlinkid,nodeid,passlink_cnt,outlinkid,dir2::smallint) 
                    end as pass_flag2
                from (
                    select  
                        inlinkid,
                        b.one_way_code, 
                        nodeid,
                        outlinkid,
                        passlink_cnt,
                        case
                            when b.one_way_code = 1 and a.nodeid = b.s_node then 0
                            when b.one_way_code = 1 and a.nodeid = b.e_node then 1
                            when b.one_way_code  = 2 and a.nodeid = b.s_node then 0
                            when b.one_way_code  = 2 and a.nodeid = b.e_node then 1
                            when b.one_way_code  = 3 and a.nodeid = b.s_node then 1
                            when b.one_way_code  = 3 and a.nodeid = b.e_node then 0
                            else 0
                        end as dir1,
                        case 
                            when b.one_way_code = 1 and a.nodeid = b.s_node then 1
                            when b.one_way_code = 1 and a.nodeid = b.e_node then 0
                            else null
                        end as dir2
                     from natural_guidence_tbl a
                     left join link_tbl b
                         on a.inlinkid = b.link_id
                     where b.link_id is not null
                ) c
            ) d 
            where pass_flag1 is false and pass_flag2 is false
        """
        # 替换表名
        sqlcmd = sqlcmd.replace('[replace_table]', self.caseinfo.getTableName())
        
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row:
            if row[0] != 0:
                return False
            else:
                return True
        else:
            return False 