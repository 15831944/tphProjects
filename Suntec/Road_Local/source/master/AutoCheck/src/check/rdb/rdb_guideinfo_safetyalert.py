# -*- coding: UTF8 -*-
'''
Created on 2015-3-11

@author: zhengchao
'''
import platform.TestCase

class CCheckSafetyAlertLinkIDDistance(platform.TestCase.CTestCase):
    
    def _do(self):
        
        sqlcmd = '''
                select count(1)
                from safety_alert_tbl a
                left join 
                link_tbl b
                on a.link_id=b.link_id
                where st_distance(a.the_geom,b.the_geom)>1e-5
                 '''
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)

class CCheckSafetyAlertOrglinkID_Split(platform.TestCase.CTestCase):
    
    def _do(self):
        
        sqlcmd = '''
                select count(1)
                from safety_alert_tbl a
                left join 
                temp_split_newlink b
                on a.orglink_id=b.link_id
                left join 
                temp_split_newlink c
                on b.old_link_id=c.old_link_id
                left join 
                link_tbl d
                on d.link_id=a.orglink_id
                left join
                link_tbl e
                on e.link_id=c.link_id
                where a.orglink_id in 
                (
                    select distinct(link_id) from temp_split_newlink
                )
                and st_distance(a.the_geom,e.the_geom)<st_distance(a.the_geom,d.the_geom)
                  '''
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)

class CCheckSafetyAlertLinkDir(platform.TestCase.CTestCase):
    
    def _do(self):
        
        sqlcmd = '''
                select count(1)
                from safety_alert_tbl a
                left join link_tbl b
                on a.link_id=b.link_id
                where a.dir+
                 abs(((a.angle-(case when st_line_locate_point(b.the_geom,a.the_geom)<>1 then ((450-mid_cal_zm_2(st_line_substring(b.the_geom,st_line_locate_point(b.the_geom,a.the_geom),1),0))::integer)%360 
                                else ((450-mid_cal_zm_2(st_line_substring(b.the_geom,0,st_line_locate_point(b.the_geom,a.the_geom)),-1))::integer)%360 end))/180.0)::int)%2
                               <>1
                 '''
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)

class CCheckSafetyAlertDis(platform.TestCase.CTestCase):
    
    def _do(self):
        self.pg.CreateFunction_ByName('mid_get_fraction')
        
        sqlcmd = '''
                select count(1)
                from safety_alert_tbl a
                left join link_tbl b
                on a.link_id=b.link_id
                where 
                abs(s_dis-(ST_Length_Spheroid(b.the_geom,'SPHEROID("WGS_84", 6378137, 298.257223563)')*mid_get_fraction(b.the_geom,a.the_geom))::int)>1
                or
                abs(e_dis-(ST_Length_Spheroid(b.the_geom,'SPHEROID("WGS_84", 6378137, 298.257223563)')*(1-mid_get_fraction(b.the_geom,a.the_geom)))::int)>1
                '''
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)

class CCheckSafetyAlertNOTASCOrglinkID(platform.TestCase.CTestCase):
    
    def _do(self):
        
        sqlcmd='''
                select count(1)
                from safety_alert_tbl a
                left join
                mid_temp_safetyalert_link b
                on a.featid=b.featid::bigint
                left join 
                temp_split_newlink c
                on a.orglink_id=c.link_id
                where not a.type in (4,5)
                and b.shape_line_id::bigint<>c.old_link_id
                and b.shape_line_id::bigint<>a.orglink_id
                '''
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)

class CCheckSafetyAlertASCLinkID(platform.TestCase.CTestCase):
    
    def _do(self):
    
        sqlcmd='''
                select count(1)
                from safety_alert_tbl
                where "type" in (4,5)
                and link_id<>orglink_id
                '''
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)

class CCheckSafetyAlertLonLat(platform.TestCase.CTestCase):
    
    def _do(self):
    
        sqlcmd='''
                select count(1)
                from safety_alert_tbl 
                where st_distance(the_geom,st_geomfromtext('POINT('||lon/256.0/3600.0||' '||lat/256.0/3600.0||')',4326))>1e-6
                '''
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)

class CCheckSafetyAlertNOTASCTypeAngleSpeedUnit(platform.TestCase.CTestCase):
    
    def _do(self):
        
        sqlcmd='''            
                select count(1)
                from safety_alert_tbl a
                left join
                mid_temp_safetyalert b
                on a.featid=b.featid::bigint
                left join 
                (
                    with a as
                    (
                        select id,b.speed from safety_alert_tbl_bak_dupli_dealing a
                        join org_sr b on a.orglink_id::double precision = b.id
                        where (a.type not in (4,5)) and b.verified=1 and b.speedtyp='1'
                        order by id
                    )
                    select id,min(speed) from a group by id
                )
                c
                on a.orglink_id=c.id
                where not a.type in (4,5)
                and (
                a.type<>case when b.cameratype='3' then 1
                             when b.cameratype='1' then 2
                             when b.cameratype='40' then 3
                             when b.cameratype='12' then 6 end 
                or
                a.angle<>(b.drivingdirection::smallint+180) % 360
                or
                ((a.speed<>case when b.speedrestriction<>'' then b.speedrestriction::smallint else 0 end) and a.speed<>c.min)
                or
                a.speed_unit<>b.speedunitofmeasurement::smallint-1
                )

                '''
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)

class CCheckSafetyASCStartAngleSpeedUnit(platform.TestCase.CTestCase):
    
    def _do(self):
        
        sqlcmd='''
                select count(1)
                from safety_alert_tbl a
                left join
                mid_temp_safetyalert b
                on a.featid=b.featid::bigint
                where a.type=4
                and (
                a.angle<>(b.drivingdirection::smallint+180) % 360
                or
                a.speed<>case when b.speedrestriction<>'' then b.speedrestriction::smallint else 0 end
                or
                a.speed_unit<>b.speedunitofmeasurement::smallint-1
                )
                '''
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)

class CCheckSafetyASCEndSpeedUnit(platform.TestCase.CTestCase):
    
    def _do(self):
    
        sqlcmd='''
                select count(1)
                from safety_alert_tbl a
                left join
                mid_temp_safetyalert b
                on a.featid=b.featid::bigint
                where a.type=5
                and (
                a.speed<>case when b.speedrestriction<>'' then b.speedrestriction::smallint else 0 end
                or
                a.speed_unit<>b.speedunitofmeasurement::smallint-1
                )
                '''
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)

class CCheckSafetyAlertASCStartEndDrivingDirection(platform.TestCase.CTestCase):
    
    def _do(self):
    
        sqlcmd='''
                with a as
                (
                select a.featid,b.f_jnctid as f,b.t_jnctid as t
                from mid_temp_safetyalert_link a
                left join org_nw b
                on a.shape_line_id::numeric=b.id
                where a.cameratype='8'
                order by b.f_jnctid
                ),
                b as
                (
                select featid,(2*count(1)-count(distinct(f))-count(distinct(t)))%2 as tof
                from a
                group by featid
                ),
                c as
                (
                select a.dir as s_dir,b.dir as e_dir,a.featid
                from safety_alert_tbl a
                left join safety_alert_tbl b
                on a.featid=b.featid and a.type=4 and b.type=5
                )
                select count(1) from b left join c on b.featid::bigint=c.featid
                where (c.s_dir+b.tof)%2<>c.e_dir
                
               '''

        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)

class CCheckSafetyAlertNum(platform.TestCase.CTestCase):
    
    def _do(self):
        
        sqlcmd='''
                select 
                ( select sum(case when cameratype ='8' then 2 else 1 end) from mid_temp_safetyalert ) 
                =
                (select count(1) from safety_alert_tbl)
        '''
        rec = self.pg.getOnlyQueryResult(sqlcmd)
        return rec

class CCheckSafetyAlertAngle(platform.TestCase.CTestCase):
    
    def _do(self):
        
        sqlcmd='''
                select count(1)
                from rdb_guideinfo_safetyalert
                where angle<0 or
                angle>359 
               '''
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)

class CCheckSafetyAlertType(platform.TestCase.CTestCase):
    
    def _do(self):
        
        sqlcmd='''
                select count(1)
                from rdb_guideinfo_safetyalert
                where type<1 or
                type>6
               '''
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)

class CCheckSafetyAlertLinkID(platform.TestCase.CTestCase):
    
    def _do(self):
        
        sqlcmd = """
                    select count(a.link_id)
                    from rdb_guideinfo_safetyalert as a
                    left join rdb_link as b
                    on a.link_id = b.link_id
                    where b.link_id is null
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)   

class CCheckSafetyAlertOrglinkID(platform.TestCase.CTestCase):
    
    def _do(self):
        
        sqlcmd = """
                    select count(a.link_id_r)
                    from rdb_guideinfo_safetyalert as a
                    left join rdb_link as b
                    on a.link_id_r = b.link_id
                    where b.link_id is null
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckSafetyAlertLon(platform.TestCase.CTestCase):
    
    def _do(self):
        
        sqlcmd = """
                    select count(1)
                    from rdb_guideinfo_safetyalert
                    where abs(lon)>180*3600*256
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckSafetyAlertLat(platform.TestCase.CTestCase):
    
    def _do(self):
        
        sqlcmd = """
                    select count(1)
                    from rdb_guideinfo_safetyalert
                    where abs(lat)>90*3600*256
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckSafetyAlertSpeed(platform.TestCase.CTestCase):
    
    def _do(self):
        
        sqlcmd = """
                    select count(1)
                    from rdb_guideinfo_safetyalert
                    where speed<0
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)

class CCheckSafetyAlertSpeedUnit(platform.TestCase.CTestCase):
    
    def _do(self):
        
        sqlcmd = """
                    select count(1)
                    from rdb_guideinfo_safetyalert
                    where not speed_unit in (0,1)
                 """
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec == 0)
    
class CCheckTableCount(platform.TestCase.CTestCase):
    
    def _do(self):
        
        sqlcmd = '''
            select count(1) from rdb_guideinfo_safetyalert
                '''
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (count_rec > 0)