# -*- coding: UTF8 -*-
'''
Created on 2014-8-4

@author: yuanrui
'''
import component.component_base


class comp_link_msm(component.component_base.comp_base):
    '''
    make link_tbl
    '''
    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Link')

    def _DoCreateTable(self):

        self.CreateTable2('link_tbl')
        
    def _DoCreateFunction(self):

        self.CreateFunction2('rdb_cnv_display_class')
        self.CreateFunction2('rdb_cnv_link_type')
        self.CreateFunction2('rdb_cnv_road_type')
        self.CreateFunction2('rdb_cnv_speed_class')
        self.CreateFunction2('rdb_cnv_function_code')
        self.CreateFunction2('rdb_cnv_lane_dir')
        self.CreateFunction2('rdb_cnv_lanecount_downflow')
        self.CreateFunction2('rdb_cnv_lanecount_upflow')
        self.CreateFunction2('rdb_cnv_speedlimit_downflow')
        self.CreateFunction2('rdb_cnv_speedlimit_upflow')
        self.CreateFunction2('rdb_cnv_speedsource_downflow')
        self.CreateFunction2('rdb_cnv_speedsource_upflow')

    def _Do(self):

        # Give type to ramp&roundabout.
        self._ProcSpecialLink()
        # Pick expressway according to HERE map.
        self._procEXP()
        # Give new type to roundabout. because some of them was connected to ramp, and was given a type <MAJOR_HWY>.
        self._procCorrectRoundabout()

        
        sqlcmd = """
            insert into link_tbl (
                link_id, s_node, e_node, display_class, link_type, road_type,
                toll, speed_class, length, function_class,
                lane_dir, lane_num_s2e, lane_num_e2s, elevated,
                structure, tunnel, rail_cross, paved, uturn,
                speed_limit_s2e, speed_limit_e2s, speed_source_s2e,
                speed_source_e2s, width_s2e, width_e2s,
                one_way_code, one_way_condition, pass_code,
                pass_code_condition, road_name, road_number,
                name_type, ownership, car_only, slope_code,
                slope_angle, disobey_flag, up_down_distinguish,
                access, extend_flag, etc_only_flag, the_geom
            )
            SELECT b.new_link_id as link_id                       -- link_id
               , b.start_node_id                                  -- from node
               , b.end_node_id                                    -- to node
               , case when i.new_link_id is not null then i.display_class
                      else rdb_cnv_display_class(
                           a.type, e.new_type, h.new_type, a.route_lvl
                          , g.new_level, a.acc_mask, j.new_link_id) 
                 end as display_class                             -- display_class
               , rdb_cnv_link_type(
                       a.type, a.one_way, f.link_id)              -- link_type
               , rdb_cnv_road_type(
                       a.type, e.new_type, a.acc_mask, j.new_link_id)     -- road type
               , case when a.toll_road = 1 then 1
                      when a.toll_road = 0 then 2
                      else 0
                 end as toll                                          -- toll
               , rdb_cnv_speed_class(
                       a.spd_limit, a.route_spd)                      -- speed class
               , ST_Length_Spheroid(
                       ST_Linemerge(a.the_geom),
                       'SPHEROID("WGS_84", 6378137, 298.257223563)')
                       as link_length                                 -- link_length
               , rdb_cnv_function_code(
                       a.route_lvl, g.new_level) as function_class            -- function_class
               , rdb_cnv_lane_dir(a.one_way)                                  -- lane dir
               , rdb_cnv_lanecount_downflow(
                       a.one_way, a.acc_mask, a.lane_cnt_f, a.lane_cnt_r)     -- lane_num_s2e
               , rdb_cnv_lanecount_upflow(
                       a.one_way, a.acc_mask, a.lane_cnt_f, a.lane_cnt_r)     -- lane_num_e2s
               , 0 as elevated                                    -- elevated
               , 0 as structure                                   -- structure
               , case is_tunnel when 1 then 1 else 0 end          -- tunnel
               , 0 as rail_cross                                  -- rail_cross
               , case when a.type = 10 then 0 else 1 end          -- paved
               , case when mid_is_uturn(a.name) is true then 1
                      else 0 end as uturn                                     -- uturn
               , rdb_cnv_speedlimit_downflow(
                        a.one_way, a.acc_mask, a.spd_limit, a.route_spd)      -- speed_limit_s2e
               , rdb_cnv_speedlimit_upflow(
                        a.one_way, a.acc_mask, a.spd_limit, a.route_spd)      -- speed_limit_e2s
               , rdb_cnv_speedsource_downflow(
                        a.one_way, a.acc_mask, a.spd_limit, a.is_lgl_spd, 
                        a.route_spd, a.spd_format)          -- speed_source_s2e
               , rdb_cnv_speedsource_upflow(
                        a.one_way, a.acc_mask, a.spd_limit, a.is_lgl_spd, 
                        a.route_spd, a.spd_format)          -- speed_source_e2s
               , 4 as width_s2e                             -- width_s2e
               , 4 as width_e2s                             -- width_e2s
               , case when substring(a.acc_mask,1,1) = '1' then 4
                      when a.one_way = 1 then 2
                      else 1 
                 end as one_way_code                        -- one_way_code
               , null as one_way_condition
               , null as no_pass
               , null as no_pass_condition
               , c.name as road_name                        -- road_name
               , d.shield as road_number                    -- road_number
               , null as name_type                          -- name_type
               , null as ownership                          -- ownership
               , null as car_only                           -- car only
               , null as slope_code                         -- slope code
               , null as slope_angle                        -- slope angle
               , case when a.type in (1,2,3,4,5,11) and a.one_way = 1 then 1 
                       else 0 
                 end as disobey_flag                        -- disobey flag
               , case when a.type in (1,2,3,4,5,11) and a.one_way = 1 then 1 
                       else 0 
                 end as up_down_distinguish                 -- up_down_distinguish
               , null as accees                             -- access
               , 1 as extend_flag                           -- extend_flag
               , 0 as etc_only_flag                         -- etc_only_flag
               , st_linemerge(a.the_geom) as the_geom
           from org_processed_line as a
           left outer join temp_topo_link b
           on a.folder = b.folder and a.link_id = b.link_id
           left join temp_link_expressway j
           on b.new_link_id = j.new_link_id
           left join temp_link_special_new_type e
           on b.new_link_id = e.new_link_id
           left join temp_link_special_new_type_for_dsp h
           on b.new_link_id = h.new_link_id           
           left join temp_link_special_new_level g
           on b.new_link_id = g.new_link_id
           left join temp_link_roundabout_dsp i
           on b.new_link_id = i.new_link_id
           left join temp_link_innerlink f
           on b.new_link_id = f.link_id
           left join temp_link_name c
           on b.new_link_id = c.link_id
           left join temp_link_shield d
           on b.new_link_id = d.link_id
           where a.type <> 40;
          """

        self.log.info(': Now it is inserting to link_tbl...')
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.log.info(': end of inserting link succeeded')

    def _ProcSpecialLink(self):
        
        self.CreateTable2('temp_topo_link_with_attr')
        self.CreateIndex2('temp_topo_link_with_attr_link_id_idx')
        self.CreateIndex2('temp_topo_link_with_attr_folder_old_link_id_idx')
        self.CreateIndex2('temp_topo_link_with_attr_s_node_idx')
        self.CreateIndex2('temp_topo_link_with_attr_e_node_idx')
        self.CreateIndex2('temp_topo_link_with_attr_one_way_idx')
        self.CreateIndex2('temp_topo_link_with_attr_type_idx')
        self.CreateIndex2('temp_topo_link_with_attr_name_idx')        
        self.CreateIndex2('temp_topo_link_with_attr_route_lvl_idx')
        self.CreateIndex2('temp_topo_link_with_attr_the_geom_idx')
        
        # Inner link  
        self.CreateFunction2('mid_cnv_roadname_simplify')  
        self.CreateFunction2('mid_is_uturn')    
        self.CreateTable2('temp_link_innerlink') 
        self.CreateIndex2('temp_link_innerlink_link_id_idx')
        
        self.CreateFunction2('mid_find_special_links')
        self.CreateFunction2('mid_find_special_links_in_one_direction')
        
        # Pick ramp&roundabout together.
        self.CreateTable2('temp_link_special')
        sqlcmd ="""
            analyze temp_topo_link_with_attr;
            select mid_find_special_links(array[8,9,11,12],'temp_link_special'); 
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('temp_link_special_id_idx')
        
        # Give new type to ramp&roundabout, for road_type.
        self.CreateTable2('temp_link_special_new_type')
        self.CreateIndex2('temp_link_special_new_type_new_link_id_idx')
        self.CreateIndex2('temp_link_special_new_type_new_type_idx')               

        # Give new type to ramp&roundabout, for display_class.
        self.CreateTable2('temp_link_special_new_type_for_dsp')
        self.CreateIndex2('temp_link_special_new_type_for_dsp_new_link_id_idx')
        self.CreateIndex2('temp_link_special_new_type_for_dsp_new_type_idx') 
        
        # Give new route_lvl to ramp&roundabout, for function_code.
        self.CreateTable2('temp_link_special_new_level')
        self.CreateIndex2('temp_link_special_new_level_new_link_id_idx')
        self.CreateIndex2('temp_link_special_new_level_new_level_idx') 
    
    def _procCorrectRoundabout(self):    
        
        # Pick roundabout.
        self.CreateTable2('temp_link_roundabout')
        sqlcmd ="""
            analyze temp_topo_link_with_attr;
            select mid_find_special_links(array[12],'temp_link_roundabout'); 
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('temp_link_roundabout_link_id_idx')
        
        # Give new type to ramp&roundabout, for display_class.
        self.CreateTable2('temp_link_roundabout_new_type')
        self.CreateIndex2('temp_link_roundabout_new_type_new_link_id_idx')
        self.CreateIndex2('temp_link_roundabout_new_type_new_type_idx')  
        
        # Caculate display_class for roundabout. 
        # Correct display_class of roundabout which was connected to ramp and was given a wrong value.
        self.CreateFunction2('rdb_cnv_display_class_for_roundabout')
        self.CreateTable2('temp_link_roundabout_dsp')  
        self.CreateIndex2('temp_link_roundabout_dsp_new_link_id_idx')
    
    def _procEXP(self):
        
        # Pick expressway.
        self.CreateTable2('temp_link_expressway')  
        self.CreateIndex2('temp_link_expressway_link_id_idx')
                                                   
    def _DoCreateIndex(self):

        self.CreateIndex2('link_tbl_link_id_idx')
        self.CreateIndex2('link_tbl_s_node_idx')
        self.CreateIndex2('link_tbl_e_node_idx')
        self.CreateIndex2('link_tbl_the_geom_idx')
