# -*- coding: UTF8 -*-
'''
Created on 2015-3

@author: zhengchao
'''
from component.component_base import comp_base
import common


class comp_guideinfo_safety_alert_ta(comp_base):
    '''
    classdocs
    '''
    
    def __init__(self):
        '''
        Constructor
        '''
        comp_base.__init__(self, 'Guideinfo_Safety_Alert')
        
    def _DoCreateTable(self):
        
        if self.CreateTable2('safety_alert_tbl') == -1:
            return -1
        return 0
    
    def _DoCreateIndex(self):
        'create index.'
        return 0
    
    def _DoCreateFunction(self):
        self.CreateFunction2('mid_cal_zm_2')
        self.CreateFunction2('mid_sc_nw_id')
        self.CreateFunction2('mid_sc_link_id')
        self.CreateFunction2('mid_get_fraction')
        return 0

    def _Do(self):
        if not (self.pg.IsExistTable("scpoint") and self.pg.IsExistTable("scpoint_ext") and
                self.pg.IsExistTable("scpoint_ll") and self.pg.IsExistTable("scpoint_ext_ll") and
                self.pg.IsExistTable("scpoint_status") and self.pg.IsExistTable("scpoint_ext_status")):
            self.log.warning('table is not exist!!!')
            return 0
        safetyalert_flag = common.common_func.GetPath('safetyalert_flag')
        if safetyalert_flag == 'true':
            self._organize_org_data()
        return
    
    def _organize_org_data(self):
        #print '-----------'
        if self.CreateTable2('mid_temp_safetyalert') == -1:
            return -1
        if self.CreateTable2('mid_temp_safetyalert_link') == -1:
            return -1
        #对应average speed camera的start_link和end_link
        sqlcmd = '''
            alter table mid_temp_safetyalert drop column if exists start_link;
            alter table mid_temp_safetyalert drop column if exists end_link;
            alter table mid_temp_safetyalert add column start_link numeric;
            alter table mid_temp_safetyalert add column end_link numeric;
            
            update mid_temp_safetyalert  a
            set start_link =e.start_link::numeric , end_link=e.end_link::numeric
            from 
            (
                with a as
                (
                    select a.featid,b.shape_line_id,a.the_geom,a.the_Geom_endpoint,b.geom,st_distance(a.the_geom,b.geom) as dist_start,st_distance(a.the_geom_endpoint,b.geom) as dist_end
                    from mid_temp_safetyalert a
                    left join mid_temp_safetyalert_link b
                    on a.featid=b.featid
                    where a.cameratype='8'
                    ),
                b as
                (
                    select featid,min(dist_start) as s_min,min(dist_end) as e_min
                    from a
                    group by featid
                    ),
                c as
                (
                    select a.featid,a.shape_line_id
                    from a
                    left join
                    b
                    on a.featid=b.featid
                    where a.dist_start=b.s_min
                    ),
                d as
                (
                    select a.featid,a.shape_line_id
                    from a
                    left join
                    b
                    on a.featid=b.featid
                    where a.dist_end=b.e_min
                )
                select c.featid,c.shape_line_id as start_link,d.shape_line_id as end_link
                from c
                left join d
                on
                c.featid=d.featid
            )
            e
            where a.featid=e.featid

        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        sqlcmd='''
                alter table mid_temp_safetyalert drop column if exists start_link_geom;
                alter table mid_temp_safetyalert drop column if exists end_link_geom;
                select addgeometrycolumn('mid_temp_safetyalert','start_link_geom',4326,'MULTILINESTRING',2);
                select addgeometrycolumn('mid_temp_safetyalert','end_link_geom',4326,'MULTILINESTRING',2);
                
                update mid_temp_safetyalert a
                set start_link_geom=b.the_geom
                from org_nw b
                where a.start_link=b.id;
                update mid_temp_safetyalert a
                set end_link_geom=b.the_geom
                from org_nw b
                where a.end_link=b.id;
              '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        #补全average speed camera区域断开的link
        sqlcmd='''
                insert into mid_temp_safetyalert_link(featid,cameratype,shape_line_id,geom)
                select * from
                (
                    with ac
                    as
                    (
                        select featid
                        from
                        (
                            select featid,(select count(1) from mid_temp_safetyalert_link where featid=ac.featid) as count1,
                            (
                                select count
                                from
                                (
                                    select count(1),count as num
                                    from
                                    (
                                        select count(1)
                                        from
                                        (
                                            select (select f_jnctid from org_nw where id=a.shape_line_id::numeric) as node
                                            from mid_temp_safetyalert_link a
                                            where featid=ac.featid
                                            union all
                                            select (select t_jnctid from org_nw where id=a.shape_line_id::numeric) as node
                                            from mid_temp_safetyalert_link a
                                            where featid=ac.featid
                                        )
                                        a
                                        group by node
                                    )
                                    b
                                    group by count
                                )
                                c
                                where num=2
                            ) as count2
                            from mid_temp_safetyalert ac
                            where cameratype='8'
                            order by count1
                        )
                        a
                        where count1-count2<>1
                    ),
                    bd as
                    (
                        select featid,(select f_jnctid from org_nw where id=a.shape_line_id::numeric) as node
                        from mid_temp_safetyalert_link a
                        where featid in (select featid from ac)
                        union all
                        select featid,(select t_jnctid from org_nw where id=a.shape_line_id::numeric) as node
                        from mid_temp_safetyalert_link a
                        where featid in (select featid from ac)
                    ),
                    ce as
                    (
                        select featid,node,count(1)
                        from bd
                        group by featid,node
                    ),
                    df as
                    (
                        select featid,node
                        from ce 
                        where count=1
                    ),
                    eg as
                    (
                        select distinct(featid),array(select node from df where featid=a.featid) as arr from df a
                    ),                
                    fh as
                    (
                        select featid,unnest(array(select id from org_nw where a.arr@>array[f_jnctid::double precision] and a.arr@>array[t_jnctid::double precision]))
                        from eg a
                    )
                    select a.featid,'8' as cameratype,a.unnest as shape_line_id,bc.the_geom
                    from fh a
                    left join
                    org_nw bc
                    on a.unnest=bc.id
                )
                a
        
               '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        #average speed camera终点drivingdirection
        sqlcmd='''
                alter table mid_temp_safetyalert drop column if exists enddrivingdirection;
                alter table mid_temp_safetyalert add column enddrivingdirection integer;
                
                update mid_temp_safetyalert a
                set enddrivingdirection=
                (c.end_angle+c.angle_diff+180*c.tof+720) %360
                from
                (
                    with a as
                    (
                        select a.featid,b.f_jnctid as f,b.t_jnctid as t
                        from mid_temp_safetyalert_link a
                        left join org_nw b
                        on a.shape_line_id::numeric=b.id
                        where a.cameratype='8'
                       -- where a.featid in 
                       -- (
                       -- select featid from mid_temp_safetyalert
                       -- where cameratype='8'
                       -- )
                        order by b.f_jnctid
                    ),
                    b as
                    (
                        select featid,(2*count(1)-count(distinct(f))-count(distinct(t)))%2 as tof
                        from a
                        group by featid
                    )
                    --select * from b
                    select 
                    --drivingdirection,
                    ((450-mid_cal_zm_2(st_line_substring(st_geometryn(start_link_geom,1),st_line_locate_point(st_geometryn(start_link_geom,1),the_geom),1),0))::integer)%360 as angle,
                    featid,drivingdirection::integer-((450-mid_cal_zm_2(st_line_substring(st_geometryn(start_link_geom,1),st_line_locate_point(st_geometryn(start_link_geom,1),the_geom),1),0))::integer)%360 as angle_diff,b.tof,
                    case when st_line_locate_point(st_geometryn(end_link_geom,1),the_geom_endpoint)<>1 then ((450-mid_cal_zm_2(st_line_substring(st_geometryn(end_link_geom,1),st_line_locate_point(st_geometryn(end_link_geom,1),the_geom_endpoint),1),0))::integer)%360 
                    else ((450-mid_cal_zm_2(st_line_substring(st_geometryn(end_link_geom,1),0,st_line_locate_point(st_geometryn(end_link_geom,1),the_geom_endpoint)),-1))::integer)%360 end as end_angle
                    from mid_temp_safetyalert
                    left join b
                    using(featid)
                    where cameratype='8'
                )
                c
                where a.featid=c.featid
               '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        #求所在link
        sqlcmd='''
                alter table mid_temp_safetyalert drop column if exists nw_id;
                alter table mid_temp_safetyalert add column nw_id numeric;
                
                alter table mid_temp_safetyalert drop column if exists link_id;
                alter table mid_temp_safetyalert add column link_id numeric;
                
                select mid_sc_nw_id();
                select mid_sc_link_id();
               '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        #camera所在地改变到所在link上
        sqlcmd='''
                update mid_temp_safetyalert
                set the_geom=ST_Line_Interpolate_Point(st_geometryn(start_link_geom,1),st_line_locate_point(st_geometryn(start_link_geom,1),the_geom)),
                the_geom_endpoint=ST_Line_Interpolate_Point(st_geometryn(end_link_geom,1),st_line_locate_point(st_geometryn(end_link_geom,1),the_geom_endpoint))
                where cameratype='8';
                
                update mid_temp_safetyalert a
                set the_geom=ST_Line_Interpolate_Point(st_geometryn((select the_geom from org_nw where id=a.link_id),1),st_line_locate_point(st_geometryn((select the_geom from org_nw where id=a.link_id),1),the_geom))
                where cameratype<>'8';
               '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        #link 切割合并前的最终表
        sqlcmd='''
                
                insert into safety_alert_tbl(featid,lon,lat,link_id,orglink_id,type,angle,dir,s_dis,e_dis,speed,speed_unit,the_geom)
                select 
                    a.featid::bigint,
                    (st_x(a.the_geom)*3600*256)::int,
                    (st_y(a.the_geom)*3600*256)::int,
                    a.link_id::bigint,
                    b.shape_line_id::bigint,
                    case when a.cameratype='3' then 1
                    when a.cameratype='1' then 2
                    when a.cameratype='40' then 3
                    when a.cameratype='12' then 6 end, 
                    (a.drivingdirection::smallint+180)%360,
                    abs(((a.drivingdirection::int-
                        (case when st_line_locate_point(st_geometryn(c.the_geom,1),a.the_geom)<>1 then 
                                ((450-mid_cal_zm_2(st_line_substring(st_geometryn(c.the_geom,1),st_line_locate_point(st_geometryn(c.the_geom,1),a.the_geom),1),0))::integer)%360 
                            else 
                                ((450-mid_cal_zm_2(st_line_substring(st_geometryn(c.the_geom,1),0,st_line_locate_point(st_geometryn(c.the_geom,1),a.the_geom)),-1))::integer)%360 end)
                          )/180.0)::int)%2 ,
                    (ST_Length_Spheroid(c.the_geom,'SPHEROID("WGS_84", 6378137, 298.257223563)')*mid_get_fraction(st_geometryn(c.the_geom,1),a.the_geom))::int,
                    (ST_Length_Spheroid(c.the_geom,'SPHEROID("WGS_84", 6378137, 298.257223563)')*(1-mid_get_fraction(st_geometryn(c.the_geom,1),a.the_geom)))::int,
                    case when a.speedrestriction<>'' then a.speedrestriction::smallint else 0 end,
                    a.speedunitofmeasurement::smallint-1,
                    a.the_geom
                from mid_temp_safetyalert a
                left join mid_temp_safetyalert_link b
                using(featid)
                left join org_nw c
                on a.link_id=c.id
                where a.cameratype<>'8'
                
                union
                
                select
                    featid::bigint,
                    (st_x(the_geom)*3600*256)::int,
                    (st_y(the_geom)*3600*256)::int,
                    start_link::bigint,
                    start_link::bigint,
                    4,
                    (drivingdirection::smallint+180)%360,
                    abs(((drivingdirection::int-
                    (case when st_line_locate_point(st_geometryn(start_link_geom,1),the_geom)<>1 then ((450-mid_cal_zm_2(st_line_substring(st_geometryn(start_link_geom,1),st_line_locate_point(st_geometryn(start_link_geom,1),the_geom),1),0))::integer)%360 
                    else ((450-mid_cal_zm_2(st_line_substring(st_geometryn(start_link_geom,1),0,st_line_locate_point(st_geometryn(start_link_geom,1),the_geom)),-1))::integer)%360 end))/180.0)::int)%2,
                    (ST_Length_Spheroid(start_link_geom,'SPHEROID("WGS_84", 6378137, 298.257223563)')*mid_get_fraction(st_geometryn(start_link_geom,1),the_geom))::int,
                    (ST_Length_Spheroid(start_link_geom,'SPHEROID("WGS_84", 6378137, 298.257223563)')*(1-mid_get_fraction(st_geometryn(start_link_geom,1),the_geom)))::int,
                    case when speedrestriction<>'' then speedrestriction::smallint else 0 end,
                    speedunitofmeasurement::smallint-1,
                    the_geom
                from mid_temp_safetyalert
                where cameratype='8'
                
                union
                
                select
                    featid::bigint,
                    (st_x(the_geom_endpoint)*3600*256)::int,
                    (st_y(the_geom_endpoint)*3600*256)::int,
                    end_link::bigint,
                    end_link::bigint,
                    5,
                    (enddrivingdirection::smallint+180)%360,
                    abs(((enddrivingdirection::int-
                    (case when st_line_locate_point(st_geometryn(end_link_geom,1),the_geom_endpoint)<>1 then ((450-mid_cal_zm_2(st_line_substring(st_geometryn(end_link_geom,1),st_line_locate_point(st_geometryn(end_link_geom,1),the_geom_endpoint),1),0))::integer)%360 
                    else ((450-mid_cal_zm_2(st_line_substring(st_geometryn(end_link_geom,1),0,st_line_locate_point(st_geometryn(end_link_geom,1),the_geom_endpoint)),-1))::integer)%360 end))/180.0)::int)%2,
                    (ST_Length_Spheroid(end_link_geom,'SPHEROID("WGS_84", 6378137, 298.257223563)')*mid_get_fraction(st_geometryn(end_link_geom,1),the_geom_endpoint))::int,
                    (ST_Length_Spheroid(end_link_geom,'SPHEROID("WGS_84", 6378137, 298.257223563)')*(1-mid_get_fraction(st_geometryn(end_link_geom,1),the_geom_endpoint)))::int,
                    case when speedrestriction<>'' then speedrestriction::smallint else 0 end,
                    speedunitofmeasurement::smallint-1,
                    the_geom_endpoint
                from mid_temp_safetyalert
                where cameratype='8'

               '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        #从org_sr补全速度限制
        sqlcmd='''
                update safety_alert_tbl a
                set speed=b.min
                
                from
                (
                    with a as
                    (
                        select id,b.speed from safety_alert_tbl a
                        join org_sr b on a.orglink_id::double precision = b.id
                        where a.speed = 0 and (a.type not in (4,5)) and b.verified=1 and b.speedtyp='1'
                        order by id
                    )
                    select id,min(speed) from a group by id
                )
                b
                where a.orglink_id=b.id::bigint and a.speed=0 and (a.type not in (4,5))

       '''

        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        return

    