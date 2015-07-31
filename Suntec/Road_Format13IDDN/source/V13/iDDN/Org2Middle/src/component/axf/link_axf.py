'''
Created on 2012-3-23

@author: sunyifeng
'''

import base

class comp_link_axf(base.component_base.comp_base):
    '''
    classdocs
    '''
    def __init__(self):
        '''
        Constructor
        '''
        base.component_base.comp_base.__init__(self, 'Link')

    def _DoCreateTable(self):
        
        if self.CreateTable2('link_tbl') == -1:
            return -1
        
        
        return 0
    
            
    def _DoCreateIndex(self):
        
        if self.CreateIndex2('link_tbl_link_id_idx') == -1:
            return -1
        
        if self.CreateIndex2('link_tbl_s_node_idx') == -1:
            return -1
        
        if self.CreateIndex2('link_tbl_e_node_idx') == -1:
            return -1
        
        if self.CreateIndex2('link_tbl_the_geom_idx') == -1:
            return -1
         
        return 0
        
    def _DoCreateFunction(self):
        
        if self.CreateFunction2('mid_cnv_road_type') == -1:
            return -1
        
        if self.CreateFunction2('mid_cnv_link_type') == -1:
            return -1
        
        if self.CreateFunction2('mid_cnv_function_class') == -1:
            return -1
        
        if self.CreateFunction2('mid_get_lanecount_downflow') == -1:
            return -1
        
        if self.CreateFunction2('mid_get_lanecount_upflow') == -1:
            return -1
        
        if self.CreateFunction2('mid_getspeedclass') == -1:
            return -1
        
        if self.CreateFunction2('mid_getregulationspeed') == -1:
            return -1
        
        if self.CreateFunction2('mid_getregulationspeed_source') == -1:
            return -1
                
        if self.CreateFunction2('mid_get_width') == -1:
            return -1
        
        if self.CreateFunction2('mid_get_width_downflow') == -1:
            return -1
        
        if self.CreateFunction2('mid_get_width_upflow') == -1:
            return -1
        
        if self.CreateFunction2('mid_cnv_length') == -1:
            return -1

        if self.CreateFunction2('mid_cnv_display_class') == -1:
            return -1
                
        return 0
        
    def _Do(self):

        if self.CreateIndex2('org_roadsegment_road_idx') == -1:
            return -1
                   
        sqlcmd = """
                insert into link_tbl (
                    link_id, s_node, e_node, display_class, link_type, road_type, toll, speed_class, 
                    length, 
                    function_class,
                    lane_dir, lane_num_s2e, lane_num_e2s, elevated, structure, tunnel, rail_cross, 
                    speed_limit_s2e, speed_limit_e2s, speed_source_s2e, speed_source_e2s, width_s2e, width_e2s,
                    one_way_code, one_way_condition, pass_code, pass_code_condition, road_name_id, road_number_id, 
                    name_type, ownership, car_only, slope_code, slope_angle, disobey_flag, 
                    up_down_distinguish, access, extend_flag, feature_string, feature_key, the_geom
                    ) 
                    SELECT 
                        link_id, 
                        s_node, 
                        e_node, 
                        display_class,
                        link_type, 
                        road_type, 
                        toll, 
                        speed_class, 
                        ST_Length_Spheroid(the_geom,'SPHEROID("WGS_84", 6378137, 298.257223563)') as length, 
                        function_class,
                        lane_dir, 
                        lane_num_s2e, 
                        lane_num_e2s, 
                        elevated, 
                        0 as structure, 
                        tunnel, 
                        0 as rail_cross, 
                        speed_limit_s2e,  
                        speed_limit_e2s,
                        speed_source_s2e,
                        speed_source_e2s, 
                        width_s2e, 
                        width_e2s,
                        one_way_code, 
                        0 as one_way_condition, 
                        null as pass_code, 
                        0 as pass_code_condition, 
                        null as road_name_id, 
                        null as road_number_id, 
                        null as name_type, 
                        ownership, 
                        null as car_only, 
                        null as slope_code, 
                        null as slope_angle, 
                        disobey_flag, 
                        up_down_distinguish, 
                        null as access, 
                        0 as extend_flag, 
                        feature_string,
                        feature_key,
                        the_geom
                    FROM
                       (
                        SELECT  
                            b.new_road as link_id, 
                            b.new_fnode as s_node, 
                            b.new_tnode as e_node,
                            mid_cnv_display_class(road_class,fc,link_type) as display_class, 
                            mid_cnv_link_type(form_way) as link_type, 
                            mid_cnv_road_type(
                                        road_class,
                                        form_way,
                                        link_type,
                                        status,
                                        ownership,
                                        name_chn,
                                        direction
                                        ) as road_type, 
                            case when toll_flag in (1,2) then toll_flag else 0 end as toll,
                            mid_getspeedclass(road_class) as speed_class,
                            mid_cnv_function_class(fc) as function_class,
                            direction as lane_dir,
                            mid_get_lanecount_downflow(direction, max_lanes) as lane_num_s2e, 
                            mid_get_lanecount_upflow(direction, max_lanes) as lane_num_e2s, 
                            case when over_head = 1 then 1 else 0 end as elevated, 
                            case when link_type = 2 then 1 else 0 end as tunnel, 
                            mid_getregulationspeed(direction, c.speed_array::int[], max_speed, mid_getspeedclass(road_class), true) as speed_limit_s2e,
                            mid_getregulationspeed(direction, c.speed_array::int[], max_speed, mid_getspeedclass(road_class), false) as speed_limit_e2s,                
                            mid_getregulationspeed_source(direction, c.speed_array::int[], max_speed, mid_getspeedclass(road_class), true) as speed_source_s2e,
                            mid_getregulationspeed_source(direction, c.speed_array::int[], max_speed, mid_getspeedclass(road_class), false) as speed_source_e2s,
                            mid_get_width_downflow(direction, width) as width_s2e,
                            mid_get_width_upflow(direction, width) as width_e2s,
                            direction as one_way_code,
                            case when a.ownership = 3 then 1 else 0 end as ownership,
                            case when form_way = 1 then 1 else 0 end as disobey_flag, 
                            case when form_way = 1 then 1 else 0 end as up_down_distinguish,
                            array_to_string(ARRAY[a.meshid, a.road_id],',') as feature_string,
                            md5(array_to_string(ARRAY[a.meshid, a.road_id],',')) as feature_key,
                            ST_LineMerge(st_scale(b.the_geom, 1/3600.0, 1/3600.0)) as the_geom
                          FROM org_roadsegment as a
                              left outer join temp_link_mapping as b
                              on a.meshid = b.meshid and a.road = b.road
                              left join
                              (
                                SELECT meshid,road,array_agg(speed_limit) as speed_array from (
                                      select road,meshid,rf_side,cast(min(speed) as double precision) as speed_limit from (
                                          select * from (
                                            select road,meshid,unnest(string_to_array(rf_info,'|'))  as speed,rf_side 
                                            from org_roadfurniture 
                                            where rf_type = 1
                                          ) a where speed <> '' and speed is not null
                                      ) a group by meshid,road,rf_side
                                      order by meshid,road,rf_side
                                ) a group by meshid,road
                              ) c
                              on a.meshid = c.meshid and a.road = c.road
                          ) as t;
            """
        
        self.log.info('Now it is inserting to link_tbl...')
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
            self.log.info('Inserting link succeeded')   
            
        
        return 0
        
        
        
        