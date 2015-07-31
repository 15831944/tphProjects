'''
Created on 2012-3-23

@author: sunyifeng
'''

import base

class comp_link_jpn(base.component_base.comp_base):
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
        
        self.CreateIndex2('link_tbl_link_id_idx')
        self.CreateIndex2('link_tbl_s_node_idx')
        self.CreateIndex2('link_tbl_e_node_idx')
        self.CreateIndex2('link_tbl_the_geom_idx')
        
        return 0
        
    def _DoCreateFunction(self):
        
        if self.CreateFunction2('rdb_cnv_road_disp_class') == -1:
            return -1
        
        if self.CreateFunction2('rdb_cnv_road_type') == -1:
            return -1
        
        if self.CreateFunction2('rdb_cnv_link_type') == -1:
            return -1
        
        if self.CreateFunction2('rdb_cnv_link_toll') == -1:
            return -1
        
        if self.CreateFunction2('mid_linkdir') == -1:
            return -1
        
        if self.CreateFunction2('get_lanecount_downflow') == -1:
            return -1
        
        if self.CreateFunction2('get_lanecount_upflow') == -1:
            return -1
        
        if self.CreateFunction2('get_width_downflow') == -1:
            return -1
        
        if self.CreateFunction2('get_width_upflow') == -1:
            return -1
        
        if self.CreateFunction2('rdb_linkwidth') == -1:
            return -1
        
        if self.CreateFunction2('zl_test_function_type') == -1:
            return -1
        
        if self.CreateFunction2('rdb_make_link_name') == -1:
            return -1
        
        if self.CreateFunction2('rdb_make_link_no') == -1:
            return -1
        
        if self.CreateFunction2('rdb_cnv_oneway_code') == -1:
            return -1
        
        if self.CreateFunction2('rdb_cnv_oneway_code_hcz') == -1:
            return -1
        
        if self.CreateFunction2('rdb_cnv_oneway_condition') == -1:
            return -1
        
        if self.CreateFunction2('rdb_cnv_pass_code') == -1:
            return -1
        
        if self.CreateFunction2('rdb_cnv_pass_condition') == -1:
            return -1
        
        if self.CreateFunction2('rdb_cnv_up_down_distinguish') == -1:
            return -1
        
        if self.CreateFunction2('rdb_cnv_display_class') == -1:
            return -1
                
        return 0
        
    def _Do(self):
                   
        sqlcmd = """
                insert into link_tbl (
                    link_id, s_node, e_node,display_class, link_type, road_type, toll, speed_class, length, function_class,
                    lane_dir, lane_num_s2e, lane_num_e2s, elevated, structure, tunnel, rail_cross, 
                    speed_limit_s2e, speed_limit_e2s, speed_source_s2e, speed_source_e2s, width_s2e, width_e2s,
                    one_way_code, one_way_condition, pass_code, pass_code_condition, road_name_id, road_number_id, 
                    name_type, ownership, car_only, slope_code, slope_angle, disobey_flag, up_down_distinguish, 
                    access, extend_flag, etc_only_flag, feature_string, feature_key, the_geom
                    ) 
                    SELECT       h.tile_link_id as link_id                        -- link_id
                               , t.tile_node_id            -- from node
                               , w.tile_node_id              -- to node
                               , rdb_cnv_display_class(roadcls_c) as display_class          --display_class
                               , rdb_cnv_link_type(linkcls_c) as link_type                  -- link_type
                               , rdb_cnv_road_type(roadcls_c, width_c, linkcls_c, navicls_c) as road_type                 -- road type
                               , rdb_cnv_link_toll(roadcls_c) as toll                      -- toll
                               , null as speed_class -- speed class
                               , ST_Length_Spheroid(the_geom,'SPHEROID("WGS_84", 6378137, 298.257223563)') as link_length               -- link_length
                               , zl_test_function_type(navicls_c, widearea_f) as function_class    -- function_class
                               , mid_linkdir(oneway_c) -- lane dir
                               , get_lanecount_downflow(oneway_c, lanecount) -- lane_num_s2e
                               , get_lanecount_upflow(oneway_c,lanecount) -- lane_num_e2s
                               , 0 as elevated  -- elevated
                               , case linkattr_c when 5 then 2 else 0 end   -- structure
                               , case linkattr_c when 2 then 1 else 0 end   -- tunnel
                               , case linkattr_c when 4 then 1 else 0 end   -- rail_cross
                               , null as speed_limit_s2e -- speed_limit_s2e
                               , null as speed_limit_e2s -- speed_limit_e2s
                               , null as speed_source_s2e -- speed_source_s2e
                               , null as speed_source_e2s -- speed_source_e2s
                               , get_width_downflow(oneway_c, rdb_linkwidth(width_c)) -- width_s2e
                               , get_width_upflow(oneway_c, rdb_linkwidth(width_c)) -- width_e2s
                               , rdb_cnv_oneway_code_hcz(oneway_c, nopass_c)
                               , rdb_cnv_oneway_condition(oneway_c)
                               , rdb_cnv_pass_code(nopass_c)
                               , rdb_cnv_pass_condition(nopass_c)
                               , e.name_id -- road_name_id
                               , f.name_id -- road_number_id
                               , d.roadtype -- name_type
                               , null as ownership -- ownership
                               , caronly_f -- car only
                               , null as slope_code -- slope code
                               , null as slope_angle -- slope angle
                               , case linkcls_c when 2 then 1 else 0 end -- disobey flag
                               , rdb_cnv_up_down_distinguish(oneway_c) as up_down_distinguish -- up_down_distinguish
                               , null as access
                               , 1 as extend_flag
                               , (case when i.gid is null then 0 else 1 end) as etc_only_flag
                               , (
                                 case when a.split_seq_num >= 0 then null
                                      when a.fromnodeid < a.tonodeid then array_to_string(ARRAY[a.objectid,a.fromnodeid,a.tonodeid],',')
                                      else array_to_string(ARRAY[a.objectid,a.tonodeid,a.fromnodeid],',')
                                 end
                                 )as feature_string
                               , (
                                 case when a.split_seq_num >= 0 then null
                                      when a.fromnodeid < a.tonodeid then md5(array_to_string(ARRAY[a.objectid,a.fromnodeid,a.tonodeid],','))
                                      else md5(array_to_string(ARRAY[a.objectid,a.tonodeid,a.fromnodeid],','))
                                 end
                                 )as feature_key
                               , the_geom
                               from temp_road_rlk as a
                               left outer join road_roadcode as d
                               on a.roadcode = d.roadcode
                               left outer join temp_link_name as e
                               on a.roadcode = e.road_code
                               left outer join temp_link_no as f
                               on a.objectid = f.link_id
                               left outer join middle_tile_link as h
                               on a.objectid = h.old_link_id and a.split_seq_num = h.split_seq_num
                               left join (select * from road_etc_link where typecode = 1) as i
                               on (a.fromnodeid = i.snodeid and a.tonodeid = i.enodeid) or (a.tonodeid = i.snodeid and a.fromnodeid = i.enodeid)
                               left outer join middle_tile_node as t
                               on a.fromnodeid = t.old_node_id
                               left outer join middle_tile_node as w
                               on a.tonodeid = w.old_node_id
            """
        
        self.log.info('Now it is inserting to link_tbl...')
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
            self.log.info('Inserting link succeeded')   
            
        
        return 0
        
        
        
        