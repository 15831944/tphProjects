'''
Created on 2012-9-3

@author: sunyifeng
'''

import component.component_base

class comp_link_ta(component.component_base.comp_base):
    '''
    classdocs
    '''
    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Link')

    def _DoCreateTable(self):
        
        self.CreateTable2('link_tbl')
        self.CreateTable2('temp_dir_pos')
        self.CreateTable2('temp_link_pos_cond_speed')
        self.CreateTable2('temp_link_neg_cond_speed')
        self.CreateTable2('temp_mid_iso_country_code')
        
        return 0
    
    def _DoCreateIndex(self):
        
        self.CreateIndex2('link_tbl_link_id_idx')
        self.CreateIndex2('link_tbl_s_node_idx')
        self.CreateIndex2('link_tbl_e_node_idx')
        self.CreateIndex2('link_tbl_the_geom_idx')

        return 0
        
    def _DoCreateFunction(self):
        
        self.CreateFunction2('mid_cnv_function_class')
        self.CreateFunction2('mid_cnv_link_type')
        self.CreateFunction2('mid_cnv_road_type')
        self.CreateFunction2('mid_get_lanecount_downflow')
        self.CreateFunction2('mid_get_lanecount_upflow')
        self.CreateFunction2('mid_get_oneway_code')
        self.CreateFunction2('mid_get_width')
        self.CreateFunction2('mid_getregulationspeed')
        self.CreateFunction2('mid_get_pos_dir')
        self.CreateFunction2('mid_cnv_length')
        self.CreateFunction2('mid_cnv_disp_class')

        return 0
        
    def _Do(self):

        self.CreateIndex2('nw_id_idx')
        self.CreateIndex2('nw_id_type_idx')        
        self.CreateIndex2('sr_id_idx')
        self.CreateIndex2('ta_id_idx')
        self.CreateIndex2('ta_areid_idx')
        self.CreateIndex2('ae_id_idx')
        
        self.CreateIndex2('temp_link_pos_cond_speed_id_idx')
        self.CreateIndex2('temp_link_neg_cond_speed_id_idx')
        self.CreateIndex2('temp_mid_iso_country_code_idx')
        
        sqlcmd = """
                insert into temp_mid_iso_country_code (
                    iso_country_num, iso_country_code
                    )
                    select 
                        distinct id::bigint / 10000000000 % 1000 as iso_country_num,
                        order00 as iso_country_code
                    from 
                        org_a0
                    where
                        substr(order00, 1, 1) != '$'
        """
        self.log.info('Now it is inserting to temp_mid_iso_country_code...')
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.log.info('Inserting temp_mid_iso_country_code succeeded')
        
#        self.pg.callproc('mid_get_pos_dir')
#        self.pg.commit2()
               
        sqlcmd = """
                insert into link_tbl (
                    link_id, iso_country_code, s_node, e_node, display_class, link_type, road_type, 
                    toll, speed_class, length, function_class,
                    lane_dir, lane_num_s2e, lane_num_e2s, 
                    elevated, 
                    structure, 
                    tunnel, 
                    rail_cross, 
                    paved, 
                    uturn,
                    speed_limit_s2e, speed_limit_e2s, speed_source_s2e, speed_source_e2s, 
                    width_s2e,  width_e2s,
                    one_way_code, one_way_condition, 
                    pass_code, pass_code_condition, 
                    road_name, 
                    road_number, 
                    name_type, 
                    ownership, 
                    car_only, 
                    slope_code, slope_angle, 
                    disobey_flag, 
                    up_down_distinguish, 
                    access, 
                    extend_flag, 
                    etc_only_flag,
                    ipd, 
                    the_geom
                    ) 
                    select 
                        link_id, iso_country_code, s_node, e_node, display_class, link_type, road_type, 
                        toll, speed_class, length, function_class,
                        null as lane_dir, 
                        mid_get_lanecount_downflow(one_way_code, lanes) as lane_num_s2e, 
                        mid_get_lanecount_upflow(one_way_code, lanes) as lane_num_e2s,  
                        null as elevated, 
                        structure, 
                        tunnel, 
                        0 as rail_cross, 
                        paved, 
                        0 as uturn,
                        mid_getregulationspeed(one_way_code, unitf, speedf, speedcat, true) as speed_limit_s2e,
                        mid_getregulationspeed(one_way_code, unitt, speedt, speedcat, false) as speed_limit_e2s, 
                        case when unitf = 2 and verifiedf = 1 and '1' = any(speedtypf) then (1 << 3) | 1
                             when unitf <> 2 and verifiedf = 1 and '1' = any(speedtypf) then 1
                             when unitf = 2 then (1 << 3) | 3
                             else 3
                        end as speed_source_s2e, 
                        case when unitt = 2 and verifiedt = 1 and '1' = any(speedtypt)  then (1 << 3) | 1
                             when unitt <> 2 and verifiedt = 1 and '1' = any(speedtypt)  then 1
                             when unitt = 2 then (1 << 3) | 3
                             else 3
                        end  as speed_source_e2s, 
                        4 as width_s2e, 
                        4 as width_e2s, 
                        one_way_code, 
                        0 as one_way_condition, 
                        null as pass_code, 
                        0 as pass_code_condition, 
                        road_name, 
                        road_number, 
                        0 as name_type, 
                        ownership, 
                        car_only, 
                        null as slope_code, 
                        null as slope_angle, 
                        disobey_flag, 
                        up_down_distinguish, 
                        null as access, 
                        extend_flag, 
                        etc_only_flag, 
                        ipd,
                        the_geom
                    from 
                    (
                    select  
                        a.id as link_id, 
                        t.iso_country_code as iso_country_code,
                        a.f_jnctid as s_node, 
                        a.t_jnctid as e_node, 
                        mid_cnv_disp_class(a.freeway, a.frc,a.feattyp, a.fow, a.roughrd) as display_class,
                        mid_cnv_link_type(a.fow, a.ramp, a.pj, a.sliprd) as link_type, 
                        mid_cnv_road_type(freeway, frc, ft, fow, privaterd, backrd, procstat, carriage, nthrutraf, sliprd, stubble) as road_type, 
                        case when tollrd is not null then 1 else 2 end as toll,
                        speedcat as speed_class,
                        ST_Length_Spheroid(a.the_geom,'SPHEROID("WGS_84", 6378137, 298.257223563)') as length, 
                        mid_cnv_function_class(net2class) as function_class,
                        case when partstruc = 2 then 3 else 0 end as structure, 
                        case when partstruc = 1 then 1 else 0 end as tunnel, 
                        case when a.rdcond in (2, 3) then 0 else 1 end as paved,
                        mid_get_oneway_code( oneway, constatus, f.linkdir, p.linkdir_array ) as one_way_code,
                        case when a.privaterd = 0 then 0 else 1 end        as ownership,
                        case when fow in (1, 2) then 1 else 0 end          as disobey_flag, 
                        case when fow in (1, 2) then 1 else 0 end          as up_down_distinguish,
                        case when e.attvalue = '1' then 1 else 0 end       as extend_flag, 
                        case when tollrd in (21, 22, 23) then 1 else 0 end as etc_only_flag,
                        case when a.PROCSTAT in (2,8,9) then 1 else 0 end as ipd,
                        roughrd    as car_only,
                        n.name     as road_name,                           -- road_name
                        s.shield   as road_number,                         -- road_number
                        b.speed    as speedf,
                        c.speed    as speedt,
                        b.verified as verifiedf,
                        c.verified as verifiedt,
                        b.speedtyp as speedtypf,
                        c.speedtyp as speedtypt,
			            b.munit    as unitf,
			            c.munit    as unitt,
                        speedcat,
                        lanes,
                        ST_LineMerge(the_geom) as the_geom
                      from org_nw as a
                        left join temp_link_regulation_permit_traffic as p
                            on a.id = p.link_id
                        left join temp_link_regulation_forbid_traffic as f
                            on a.id = f.link_id
                        left join temp_link_pos_cond_speed as b
                            on a.id = b.id
                        left join temp_link_neg_cond_speed as c
                            on a.id = c.id
                        left join org_ta as d
                            on a.id = d.id and d.aretyp = 1111
                        left join(
                               select distinct id, feattyp, atttyp, attvalue from org_ae where atttyp = '3D'
                        ) as e
                            on d.areid = e.id 
                        left join temp_link_name n
                            on a.id = n.link_id
                        left join temp_link_shield s
                            on a.id = s.link_id 
                        left join temp_mid_iso_country_code as t
                            on a.id::bigint / 10000000000 % 1000 = t.iso_country_num
                      where a.frc != -1 
                    ) as t;
            """
        
        self.log.info('Now it is inserting to link_tbl...')
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.log.info('Inserting link succeeded')   
            
        return 0
        
        
        
        
