'''
Created on 2014-3-12

@author: zhangpeng
'''

import component.component_base

class comp_link_mmi(component.component_base.comp_base):
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
        #self.CreateTable2('temp_dir_pos')
        
        return 0
    
    def _DoCreateIndex(self):
        
        self.CreateIndex2('link_tbl_link_id_idx')
        self.CreateIndex2('link_tbl_s_node_idx')
        self.CreateIndex2('link_tbl_e_node_idx')
        self.CreateIndex2('link_tbl_the_geom_idx')

        return 0
        
    def _DoCreateFunction(self):
        
        self.CreateFunction2('mmi_speed_class')
        self.CreateFunction2('mmi_road_type')
        self.CreateFunction2('mmi_link_type')
        self.CreateFunction2('mmi_disp_class')
        self.CreateFunction2('mmi_function_class')
        self.CreateFunction2('mmi_oneway')
        self.CreateFunction2('mmi_speed_limit')
        self.CreateFunction2('mmi_speed_source')
        return 0
        
    def _Do(self):

        self.CreateIndex2('org_city_nw_gc_polyline_id_idx')
        self.CreateIndex2('org_city_nw_gc_polyline_id_bigint_idx')
        self.CreateIndex2('org_city_nw_gc_extended_polyline_id_idx')
        
        self.CreateIndex2('temp_link_name_link_id_idx')
        self.CreateIndex2('temp_link_shield_link_id_idx')
       
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
                    road_name, road_number, name_type, 
                    ownership, 
                    car_only, 
                    slope_code, slope_angle, 
                    disobey_flag, 
                    up_down_distinguish, 
                    access, 
                    extend_flag, 
                    etc_only_flag, 
                    the_geom
                    ) 
                select 
                        link_id, 
                        'IND' as iso_country_code,
                        s_node, e_node, display_class, link_type, road_type, 
                        toll, speed_class, length, function_class,
                        0 as lane_dir, 0 as lane_num_s2e, 0 as lane_num_e2s,  
                        elevated, 
                        structure, 
                        tunnel, 
                        0 as rail_cross, 
                        paved, 
                        uturn,
                        mmi_speed_limit( one_way_code, spd_limit, kph, TRUE)   as speed_limit_s2e,
                        mmi_speed_limit( one_way_code, spd_limit, kph, FALSE)  as speed_limit_e2s, 
                        mmi_speed_source(one_way_code, spd_limit, kph, TRUE)   as speed_source_s2e, 
                        mmi_speed_source(one_way_code, spd_limit, kph, FALSE)  as speed_source_e2s,
                        4 as width_s2e, 4 as width_e2s, 
                        one_way_code,  0 as one_way_condition, 
                        null as pass_code,  null as pass_code_condition, 
                        road_name,  road_number, 0 as name_type, 
                        ownership, 
                        1 as car_only, 
                        null as slope_code,  null as slope_angle, 
                        disobey_flag, 
                        up_down_distinguish, 
                        null as access, 
                        1 as extend_flag, 
                        0 as etc_only_flag, 
                        the_geom
                  from 
                  (
                 select  
                        a.id         as link_id, 
                        a.f_jnctid   as s_node, 
                        a.t_jnctid   as e_node, 
                        mmi_disp_class( ft, freeway, frc, fow, ftr_cry )                    as display_class,
                        mmi_link_type ( pj, mult_digi, fow)                        as link_type, 
                        mmi_road_type (ft, privaterd, frc, fow, freeway, ftr_cry, routable )  as road_type,
                        mmi_function_class( frc, freeway, ftr_cry)                 as function_class,
                        mmi_oneway(oneway, e.const_st, f.linkdir, p.linkdir_array)   as one_way_code,
                        CASE when tollrd in ('B','FT') or e.toll_rd = 1 then 1 else 0 end as toll, 
                        CASE when fow = 22 then 1 else 0 END             as elevated,
                        CASE when fow in (24)     then 1
                             when fow = 25           then 2
                             else 0
                        END                                              as structure,
                        CASE when a.fow    = 26       then 1 else 0 END  as tunnel,
                        CASE when a.rdcond = 1        then 1 else 0 END  as paved,
                        CASE when a.fow    = 23       then 1 else 0 END  as uturn,
                        CASE when a.privaterd = 1     then 1 else 0 END  as ownership,
                        CASE when e.div_typ in (1, 2) then 1 else 0 END  as disobey_flag, 
                        CASE when a.mult_digi = 1     then 1 else 0 END  as up_down_distinguish,
                        mmi_speed_class( spd_limit, kph )                as speed_class,
                        spd_limit,
                        kph,
                        n.name     as road_name,                          -- road_name
                        s.shield   as road_number,                        -- road_number
                        ST_Length_Spheroid(a.the_geom,'SPHEROID("WGS_84", 6378137, 298.257223563)') as length,
                        ST_LineMerge(a.the_geom) as the_geom
                   from org_city_nw_gc_polyline             as a
              left join org_city_nw_gc_extended_polyline    as e
                     on a.id = e.id
              left join temp_link_name                      as n
                     on a.id::bigint = n.link_id
              left join temp_link_shield                    as s
                     on a.id::bigint = s.link_id 
              left join temp_link_regulation_permit_traffic as p
                     on a.id::bigint = p.link_id
              left join temp_link_regulation_forbid_traffic as f
                     on a.id = f.link_id
                ) as t;
            """
        
        self.log.info('Now it is inserting to link_tbl...')
        self.pg.do_big_insert2(sqlcmd)
        self.log.info('Inserting link succeeded')   
            
        return 0
        
