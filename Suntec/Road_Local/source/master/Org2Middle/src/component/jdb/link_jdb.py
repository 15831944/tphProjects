# -*- coding: cp936 -*-
'''
Created on 2012-3-23

@author: sunyifeng
'''

import component.component_base

class comp_link_jdb(component.component_base.comp_base):
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
        
        return 0
    
            
    def _DoCreateIndex(self):
        
        self.CreateIndex2('link_tbl_link_id_idx')
        self.CreateIndex2('link_tbl_s_node_idx')
        self.CreateIndex2('link_tbl_e_node_idx')
        self.CreateIndex2('link_tbl_the_geom_idx')
        
        return 0
        
    def _DoCreateFunction(self):
        
        self.CreateFunction2('rdb_cnv_road_disp_class')       
        self.CreateFunction2('rdb_cnv_road_type')        
        self.CreateFunction2('rdb_cnv_link_type')        
        self.CreateFunction2('rdb_cnv_link_toll')        
        self.CreateFunction2('mid_linkdir')        
        self.CreateFunction2('get_lanecount_downflow')        
        self.CreateFunction2('get_lanecount_upflow')        
        self.CreateFunction2('get_width_downflow')        
        self.CreateFunction2('get_width_upflow')        
        self.CreateFunction2('rdb_linkwidth')        
        self.CreateFunction2('rdb_cnv_function_class')        
        self.CreateFunction2('rdb_make_link_name')        
        self.CreateFunction2('rdb_make_link_no')  
        self.CreateFunction2('rdb_cnv_oneway_code')
        self.CreateFunction2('rdb_cnv_oneway_condition')    
        self.CreateFunction2('rdb_cnv_pass_code')      
        self.CreateFunction2('rdb_cnv_pass_condition') 
        self.CreateFunction2('rdb_cnv_up_down_distinguish')
        self.CreateFunction2('rdb_cnv_display_class')
        self.CreateFunction2('rdb_cnv_name_suffix')
            
        return 0
        
    def _Do(self):
                   
        sqlcmd = """
        insert into link_tbl (
            link_id, s_node, e_node,display_class, link_type, road_type, toll, speed_class, length, function_class,
            lane_dir, lane_num_s2e, lane_num_e2s, elevated, structure, tunnel, rail_cross, paved, uturn, 
            speed_limit_s2e, speed_limit_e2s, speed_source_s2e, speed_source_e2s, width_s2e, width_e2s,
            one_way_code, one_way_condition, pass_code, pass_code_condition, road_name, road_number, 
            name_type, ownership, car_only, slope_code, slope_angle, disobey_flag, up_down_distinguish, 
            access, extend_flag, etc_only_flag, bypass_flag, matching_flag, highcost_flag, feature_string, feature_key, the_geom
        ) 
        select h.tile_link_id as link_id              -- link_id           
            , t.tile_node_id as s_node                -- from node
            , w.tile_node_id as e_node                -- to node
            , rdb_cnv_display_class(roadclass_c) as display_class         -- display_class
            , rdb_cnv_link_type(linkclass_c,roadclass_c) as link_type                 -- link_type
            , rdb_cnv_road_type(
                    roadclass_c, roadwidth_c, linkclass_c, naviclass_c
            ) as road_type                                                -- road type
            , rdb_cnv_link_toll(roadclass_c,e.road_code) as toll
            , null as speed_class -- speed class                          -- toll
            , ST_Length_Spheroid(
                    the_geom,'SPHEROID("WGS_84", 6378137, 298.257223563)'
            ) as length                                                   -- link_length
            , (case when roadclass_c = 401 then -1 else rdb_cnv_function_class(
                    naviclass_c, widearea_f) end) as function_class            -- function_class
            , mid_linkdir(oneway_c) as lane_dir                           -- lane dir
            , (case when roadclass_c = 401 then -1 else 
                get_lanecount_downflow(oneway_c, lane_count) end)         -- lane_num_s2e
            , (case when roadclass_c = 401 then -1 else
                get_lanecount_upflow(oneway_c,lane_count) end)                  -- lane_num_e2s
            , 0 as elevated                                               -- elevated 
            , case linkattr_c when 5 then 2 else 0 end as structure       -- structure
            , case linkattr_c when 2 then 1 else 0 end as tunnel          -- tunnel
            , case linkattr_c when 4 then 1 else 0 end as rail_cross      -- rail_cross
            , 1 as paved, 0 as uturn                                      -- paved/uturn
            , null as speed_limit_s2e                                     -- speed_limit_s2e
            , null as speed_limit_e2s                                     -- speed_limit_e2s
            , null as speed_source_s2e                                    -- speed_source_s2e
            , null as speed_source_e2s                                    -- speed_source_e2s
            , (case when roadclass_c = 401 then -1 else
                get_width_downflow(oneway_c, rdb_linkwidth(roadwidth_c)) end)   -- width_s2e
            , (case when roadclass_c = 401 then -1 else
                get_width_upflow(oneway_c, rdb_linkwidth(roadwidth_c)) end)      -- width_e2s
            , rdb_cnv_oneway_code(roadclass_c, naviclass_c, oneway_c, nopassage_c) as one_way_code     -- one_way_code
            , rdb_cnv_oneway_condition(oneway_c) as one_way_condition        -- one_way_condition
            , rdb_cnv_pass_code(nopassage_c) as pass_code                    -- pass_code
            , rdb_cnv_pass_condition(nopassage_c) as pass_code_condition     -- pass_code_condition
            , case when d.road_code is not null then
                    mid_get_json_string_for_japan_name(rdb_cnv_name_suffix(d.name_kanji),rdb_cnv_name_suffix(d.name_yomi)) 
                   when e.road_code is not null then
                    mid_get_json_string_for_japan_name(rdb_cnv_name_suffix(e.name_kanji),rdb_cnv_name_suffix(e.name_yomi))                    
                   else null
              end as road_name                                               -- road_name
            , case when road_no <> 0 then 
                    mid_get_json_string_for_japan_routeno(road_no,roadclass_c) 
                   else null
              end as road_number                                             -- road_number                                       -- road_number
            , d.lineclass_c as name_type                                     -- name_type
            , null as ownership                                              -- ownership
            , caronly_f as car_only                                          -- car only
            , null as slope_code                                             -- slope code
            , null as slope_angle                                            -- slope angle
            , case linkclass_c when 2 then 1 else 0 end as disobey_flag      -- disobey flag
            , rdb_cnv_up_down_distinguish(oneway_c) as up_down_distinguish   -- up_down_distinguish
            , null as access                                                 -- access
            , (case when roadclass_c = 401 then -1 else 1 end) as extend_flag                                               -- extend_flag
            , (case when a.smartic_c = 1 then 1 else 0 end) as etc_only_flag -- etc_only_flag
            , a.bypass_f as bypass_flag                                                   -- bypass_flag            
            , (case when a.roadclass_c in (302,303,304,305,401,402) then 1 else 0 end)    -- matching_flag
            , (case when a.road_code in (9003,209040,211050) then 1 
                    when b.org_link_id is not null then 1 else 0 end)                     -- highcost_flag
            , (
              case when a.split_seq_num >= 0 then null
                   when a.from_node_id < a.to_node_id then array_to_string(ARRAY[a.objectid,a.from_node_id,a.to_node_id],',')
                   else array_to_string(ARRAY[a.objectid,a.to_node_id,a.from_node_id],',')
              end
              )as feature_string
            , (
              case when a.split_seq_num >= 0 then null
                   when a.from_node_id < a.to_node_id then md5(array_to_string(ARRAY[a.objectid,a.from_node_id,a.to_node_id],','))
                   else md5(array_to_string(ARRAY[a.objectid,a.to_node_id,a.from_node_id],','))
              end
              )as feature_key
            , the_geom
        from temp_road_link as a
        left join org_ferry_fc_down b
        on a.objectid = b.org_link_id
        left outer join road_code_list as d
        on a.road_code = d.road_code
        left outer join road_code_list as e
        on a.road_code > 600000 and a.road_code - 400000 = e.road_code        
        left outer join middle_tile_link as h
        on a.objectid = h.old_link_id and a.split_seq_num = h.split_seq_num
        left outer join  middle_tile_node as t
        on a.from_node_id = t.old_node_id
        left outer join middle_tile_node as w
        on a.to_node_id = w.old_node_id;
        
            """
        
        self.log.info('Now it is inserting to link_tbl...')
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
            self.log.info('Inserting link succeeded')   
            
        
        return 0
        
        
        
        