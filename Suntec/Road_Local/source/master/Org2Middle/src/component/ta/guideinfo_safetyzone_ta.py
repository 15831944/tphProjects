# -*- coding: UTF8 -*-
'''
Created on 2015-3

@author: zhengchao
'''
from component.component_base import comp_base
import common


class comp_guideinfo_safety_zone_ta(comp_base):
    '''
    classdocs
    '''
    
    def __init__(self):
        '''
        Constructor
        '''
        comp_base.__init__(self, 'Guideinfo_Safety_Zone')
        
    def _DoCreateTable(self):
        if not (self.pg.IsExistTable("scpoint") and self.pg.IsExistTable("scpoint_ext") and
                self.pg.IsExistTable("scpoint_ll") and self.pg.IsExistTable("scpoint_ext_ll") and
                self.pg.IsExistTable("scpoint_status") and self.pg.IsExistTable("scpoint_ext_status")):
            self.log.warning('table is not exist!!!')
            return 0
        if self.CreateTable2('mid_temp_safetyzone') == -1:
            return -1
        if self.CreateTable2('mid_temp_safetyzone_link') == -1:
            return -1
        if self.CreateTable2('safety_zone_tbl') == -1:
            return -1
        return 0
    
    def _DoCreateIndex(self):
        'create index.'
        return 0
    
    def _DoCreateFunction(self):
        self.CreateFunction2('mid_sz_dir')
        self.CreateFunction2('mid_cal_zm_2')
        return 0

    def _Do(self):
        safetyalert_flag = common.common_func.GetPath('safetyzone_flag')
        if safetyalert_flag == 'true':
            self._organize_org_data()
        return
    
    def _organize_org_data(self):
        #print '-----------'
        
        #补全safety zone区域断开的link
        sqlcmd = '''
              insert into mid_temp_safetyzone_link(featid,cameratype,shape_line_id,geom)
                select * from
                (
                    with ac
                    as
                    (
                        select featid,cameratype
                        from
                        (
                            select featid,cameratype,(select count(1) from mid_temp_safetyzone_link where featid=ac.featid) as count1,
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
                                            from mid_temp_safetyzone_link a
                                            where featid=ac.featid
                                            union all
                                            select (select t_jnctid from org_nw where id=a.shape_line_id::numeric) as node
                                            from mid_temp_safetyzone_link a
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
                            from mid_temp_safetyzone ac
                            order by count1
                        )
                        a
                        where count1-count2<>1
                    ),
                    bd as
                    (
                        select featid,cameratype,(select f_jnctid from org_nw where id=a.shape_line_id::numeric) as node
                        from mid_temp_safetyzone_link a
                        where featid in (select featid from ac)
                        union all
                        select featid,cameratype,(select t_jnctid from org_nw where id=a.shape_line_id::numeric) as node
                        from mid_temp_safetyzone_link a
                        where featid in (select featid from ac)
                    ),
                    ce as
                    (
                        select featid,node,cameratype,count(1)
                        from bd
                        group by featid,node,cameratype
                    ),
                    df as
                    (
                        select featid,cameratype,node
                        from ce 
                        where count=1
                    ),
                    eg as
                    (
                        select featid,cameratype,array(select node from df where featid=a.featid) as arr from df a
                        group by featid,cameratype
                    ),
                    fh as
                    (
                        select featid,cameratype,unnest(array(select id from org_nw where a.arr@>array[f_jnctid::double precision] and a.arr@>array[t_jnctid::double precision]))
                        from eg a
                    )
                    select a.featid,cameratype,a.unnest as shape_line_id,bc.the_geom
                    from fh a
                    left join
                    org_nw bc
                    on a.unnest=bc.id
                )
                a

                 '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        sqlcmd='''
                alter table mid_temp_safetyzone drop column if exists nw_id;
                alter table mid_temp_safetyzone add column nw_id numeric;
                
                alter table mid_temp_safetyzone drop column if exists dir;
                alter table mid_temp_safetyzone add column dir smallint;
                
                alter table mid_temp_safetyzone drop column if exists flag;
                alter table mid_temp_safetyzone add column flag boolean;
                
                update mid_temp_safetyzone a
                set 
                nw_id=d.shape_line_id::numeric,
                flag=d.flag
                from
                (
                    with a as
                    (
                        select featid,drivingdirection,the_geom,geom,shape_line_id,st_distance(the_geom,geom) as dist
                        from mid_temp_safetyzone a
                        left join
                        mid_temp_safetyzone_link b
                        using(featid)
                    ),
                    b as
                    (
                        select featid,min(dist)
                        from a
                        group by featid
                    ),
                    c as
                    (
                        select featid,drivingdirection,a.shape_line_id,
                        (450-
                        case when st_line_locate_point(st_geometryn(geom,1),the_geom)<>1 then
                        mid_cal_zm_2(st_line_substring(st_geometryn(geom,1),st_line_locate_point(st_geometryn(geom,1),the_geom),1),0)
                        else
                        mid_cal_zm_2(st_line_substring(st_geometryn(geom,1),0,st_line_locate_point(st_geometryn(geom,1),the_geom)),-1)
                        end)::integer %360 as angle,
                        st_distance(the_geom,geom),the_geom,geom
                        from a 
                        left join b
                        using(featid)
                        where dist=min
                    )
                    select featid,shape_line_id,
                    (abs((((drivingdirection::integer-angle)/180.0-((drivingdirection::integer-angle)/180.0)::integer)*180)::int)>1
                    and (st_line_locate_point(st_geometryn(geom,1),the_geom)>0.99
                    or
                    st_line_locate_point(st_geometryn(geom,1),the_geom)<0.01)) as flag
                    from c
                )d
                where 
                a.featid =d.featid;
                
                update mid_temp_safetyzone a
                set 
                dir=
                abs(((a.drivingdirection::int-
                (450-
                case when st_line_locate_point(st_geometryn(b.the_geom,1),a.the_geom)<>1 then
                mid_cal_zm_2(st_line_substring(st_geometryn(b.the_geom,1),st_line_locate_point(st_geometryn(b.the_geom,1),a.the_geom),1),0)
                else
                mid_cal_zm_2(st_line_substring(st_geometryn(b.the_geom,1),0,st_line_locate_point(st_geometryn(b.the_geom,1),a.the_geom)),-1)
                end)::integer %360)/180.0)::int % 2)+1
                from
                org_nw b
                where a.nw_id::double precision=b.id and not a.flag;

                update mid_temp_safetyzone a
                set 
                dir=st_line_locate_point(st_geometryn(b.the_geom,1),a.the_geom)::int+1
                from
                org_nw b
                where a.nw_id::double precision=b.id and a.flag;
               '''

        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        sqlcmd='''
                alter table mid_temp_safetyzone_link drop column if exists dir;
                alter table mid_temp_safetyzone_link add column dir smallint;
                
                update mid_temp_safetyzone_link a
                set dir=b.dir
                from mid_temp_safetyzone b
                where a.featid=b.featid and a.shape_line_id::numeric=b.nw_id;
                
                alter table mid_temp_safetyzone_link drop column if exists f_jnctid;
                alter table mid_temp_safetyzone_link drop column if exists t_jnctid;
                
                alter table mid_temp_safetyzone_link add column f_jnctid numeric;
                alter table mid_temp_safetyzone_link add column t_jnctid numeric;
                
                update mid_temp_safetyzone_link a
                set f_jnctid=b.f_jnctid::numeric,
                t_jnctid=b.t_jnctid::numeric
                from org_nw b
                where a.shape_line_id::double precision=b.id;
                
                select mid_sz_dir();

               '''

        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        sqlcmd='''
                drop sequence if exists safetyzone_id_seq;
                create sequence safetyzone_id_seq;
           
                
                INSERT INTO safety_zone_tbl(
                    safetyzone_id,linkid, speedlimit, speedunit_code, direction, safety_type)
 
                select c.safetyzone_id,b.shape_line_id::bigint as linkid,case when a.speedrestriction='' then 0 else a.speedrestriction::smallint end as speedlimit,
                    3-a.speedunitofmeasurement::smallint as speedunit_code,b.dir+1 as direction,
                    case when a.cameratype='8' then 1 when a.cameratype='11' then 2 end as safety_type
                    from mid_temp_safetyzone a
                    left join
                    mid_temp_safetyzone_link b
                    using(featid)
                    left join 
                    (
                        select nextval('safetyzone_id_seq') as safetyzone_id,featid
                        from 
                        (
                            select featid
                            from mid_temp_safetyzone 
                            order by cameratype,featid
                        ) a
                    ) c
                    on a.featid=c.featid
                    order by safetyzone_id

               '''

        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        #从org_sr中补全speedlimit
        sqlcmd ='''
                update safety_zone_tbl a
                set speedlimit=b.min
                from
                (        
                    select a.linkid,min(b.speed) from safety_zone_tbl a
                    inner join org_sr b on b.id::bigint=a.linkid
                    where a.speedlimit=0 and a.safety_type=2 and b.verified=1 and b.speedtyp='1' 
                    and (a.direction=b.valdir or b.valdir=1)
                    group by a.linkid
                ) b
                where a.linkid=b.linkid and a.safety_type=2
                '''
        
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        sqlcmd='''
                delete from safety_zone_tbl
                where speedlimit=0 and safety_type=1
               '''
        
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        return 0

    