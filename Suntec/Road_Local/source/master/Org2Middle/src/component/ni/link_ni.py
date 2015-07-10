# -*- coding: cp936 -*-

import component.component_base

class comp_link_ni(component.component_base.comp_base):
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
    
    def _DoCreateIndex(self):
        
        self.CreateIndex2('link_tbl_link_id_idx')
        self.CreateIndex2('link_tbl_s_node_idx')
        self.CreateIndex2('link_tbl_e_node_idx')
        self.CreateIndex2('link_tbl_the_geom_idx')
        
    def _DoCreateFunction(self):
        
        self.CreateFunction2('ni_cnv_disp_class')
        self.CreateFunction2('ni_cnv_link_type')
        self.CreateFunction2('ni_cnv_road_type')
        self.CreateFunction2('ni_cnv_speedlimit')
        self.CreateFunction2('ni_cnv_speed_source')
        self.CreateFunction2('ni_cnv_lane')
        self.CreateFunction2('ni_cnv_width')
        self.CreateFunction2('mid_get_access')
        
    def _Do(self):

        self.CreateIndex2('org_r_id_idx')
        self.CreateIndex2('org_r_id_2_idx')
        self.CreateIndex2('org_r_kind_idx')
        self.CreateIndex2('org_r_snodeid_idx')
        self.CreateIndex2('org_r_enodeid_idx')
        
        self.CreateIndex2('temp_link_name_link_id_idx')
        self.CreateIndex2('temp_link_shield_link_id_idx')
       
        sqlcmd = """
            insert into link_tbl (
                link_id, iso_country_code, s_node, e_node, display_class, link_type, road_type, 
                toll, speed_class, length, function_class, lane_dir, lane_num_s2e, lane_num_e2s, 
                elevated, structure, tunnel, rail_cross, paved, uturn,
                speed_limit_s2e, speed_limit_e2s, speed_source_s2e, speed_source_e2s, 
                width_s2e,  width_e2s, one_way_code, one_way_condition, pass_code, pass_code_condition, 
                road_name, road_number, name_type, ownership, car_only, slope_code, slope_angle, 
                disobey_flag, up_down_distinguish, access, extend_flag, etc_only_flag, urban, the_geom
            )
              select 
                    link_id, 'CHN' as iso_country_code, s_node, e_node, 
                    display_class, link_type, road_type, toll, speed_class::smallint, 
                    length, function_class, 0 as lane_dir, 
                    ni_cnv_lane( one_way_code, lanenums2e, lanenume2s, true) as lane_num_s2e, 
                    ni_cnv_lane( one_way_code, lanenums2e, lanenume2s, false) as lane_num_e2s, 
                    elevated, structure, tunnel, 
                    0 as rail_cross, paved, 0 as uturn,
                    ni_cnv_speedlimit( one_way_code, spdlmts2e, speedclass, true ) as speed_limit_s2e,
                    ni_cnv_speedlimit( one_way_code, spdlmte2s, speedclass, false ) as speed_limit_e2s, 
                    ni_cnv_speed_source( one_way_code, spdsrcs2e, true ) as speed_source_s2e, 
                    ni_cnv_speed_source( one_way_code, spdsrce2s, false ) as speed_source_e2s,
                    ni_cnv_width( one_way_code, width, true ) as width_s2e, 
                    ni_cnv_width( one_way_code, width, false ) as width_e2s, 
                    one_way_code, 0 as one_way_condition, 
                    null as pass_code, null as pass_code_condition, 
                    road_name, road_number, 0 as name_type, 
                    ownership, 1 as car_only, null as slope_code, null as slope_angle, 
                    disobey_flag, up_down_distinguish, null as access, 0 as extend_flag, 
                    0 as etc_only_flag, urban, the_geom
              from 
              (
                  select  
                        a.id::bigint as link_id,
                        (case when b.new_node_id is null then a.snodeid else b.new_node_id end)::bigint as s_node, 
                        (case when c.new_node_id is null then a.enodeid else c.new_node_id end)::bigint as e_node, 
                        ni_cnv_disp_class( kind, vehcl_type ) as display_class,
                        ni_cnv_link_type ( kind ) as link_type, 
                        ni_cnv_road_type ( kind, through, unthrucrid, vehcl_type, ownership ) as road_type,
                        funcclass::smallint as function_class,
                        case when const_st = '4'
                            or mid_get_access(vehcl_type) = 0 then 4::smallint
                            else direction::smallint 
                        end as one_way_code,
                        toll::smallint as toll, 
                        elevated::smallint as elevated,
                        case when structure in ('1','2') then structure::smallint
                            when (kind like '%08' or kind like '%08|%') then 3
                            else 0
                        end as structure,
                        case when kind like '%0f' or kind like '%0f|%' then 1
                            else 0
                        end as tunnel,
                        case when road_cond = '0' then 1 else 0 end as paved,
                        ownership::smallint as ownership,
                        case when kind like '%02' or kind like '%02|%' then 1
                            else 0
                        end as disobey_flag, 
                        case when kind like '%02' or kind like '%02|%' then 1
                            else 0
                        end as up_down_distinguish,
                        speedclass::smallint as speed_class,
                        spdlmts2e, spdlmte2s, speedclass, spdsrcs2e, spdsrce2s,
                        lanenums2e, lanenume2s, width,
                        n.name as road_name, 
                        s.shield as road_number, 
                        uflag::smallint as urban,
                        ST_Length_Spheroid( 
                            a.the_geom,'SPHEROID("WGS_84", 6378137, 298.257223563)' 
                        ) as length,
                        ST_LineMerge( a.the_geom ) as the_geom
                  from org_r a
                  left join temp_link_name n
                  on a.id::bigint = n.link_id
                  left join temp_link_shield s
                  on a.id::bigint = s.link_id 
                  left join temp_node_mapping b
                  on a.snodeid=b.old_node_id
                  left join temp_node_mapping c
                  on a.enodeid=c.old_node_id
            ) as t;
            """
        
        self.log.info('Now it is inserting to link_tbl...')
        self.pg.do_big_insert2(sqlcmd)
        self.log.info('Inserting link succeeded')
        
