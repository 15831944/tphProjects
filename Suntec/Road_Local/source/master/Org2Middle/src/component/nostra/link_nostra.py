# -*- coding: UTF8 -*-
'''
Created on 2012-12-5

@author: yuanrui
'''
import component.component_base


class comp_link_nostra(component.component_base.comp_base):
    '''
    make link_tbl
    '''
    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Link')

    def _DoCreateTable(self):

        if self.CreateTable2('link_tbl') == -1:
            return -1

        return 0

    def _DoCreateFunction(self):

        if self.CreateFunction2('rdb_cnv_display_class') == -1:
            return -1

        if self.CreateFunction2('rdb_cnv_link_type') == -1:
            return -1

        if self.CreateFunction2('rdb_cnv_road_type') == -1:
            return -1

        if self.CreateFunction2('rdb_cnv_speed_class') == -1:
            return -1

        if self.CreateFunction2('rdb_cnv_function_code') == -1:
            return -1

        if self.CreateFunction2('rdb_cnv_lane_dir') == -1:
            return -1

        if self.CreateFunction2('rdb_cnv_lanecount_downflow') == -1:
            return -1

        if self.CreateFunction2('rdb_cnv_lanecount_upflow') == -1:
            return -1

        if self.CreateFunction2('rdb_cnv_speedlimit_downflow') == -1:
            return -1

        if self.CreateFunction2('rdb_cnv_speedlimit_upflow') == -1:
            return -1

        if self.CreateFunction2('rdb_cnv_speedsource_downflow') == -1:
            return -1

        if self.CreateFunction2('rdb_cnv_speedsource_upflow') == -1:
            return -1

        if self.CreateFunction2('get_width_downflow') == -1:
            return -1

        if self.CreateFunction2('get_width_upflow') == -1:
            return -1

        return 0

    def _Do(self):

        self.CreateIndex2('org_l_tran_routeid_idx')
      
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
            SELECT a.routeid as link_id                         -- link_id
               , b.start_node_id                                -- from node
               , b.end_node_id                                  -- to node
               , rdb_cnv_display_class(rdlnclass, level,
                       tollway, c_access, fw) as display_class  --display_class
               , rdb_cnv_link_type(fw, rdlnclass,
                                   oneway, level, dtype) as link_type         -- link_type
               , rdb_cnv_road_type(rdlnclass, level,
                               c_access, fw) as road_type       -- road type
               , case when tollway = 'Y' then 1
                      when tollway = 'N' then 2
                      else 0
                 end as toll                                   -- toll
               , rdb_cnv_speed_class(speed) as speed_class     -- speed class
               , ST_Length_Spheroid(
                       ST_Linemerge(a.the_geom),
                       'SPHEROID("WGS_84", 6378137, 298.257223563)')
                       as link_length                          -- link_length
               , rdb_cnv_function_code(level) as function_class-- function_class
               , rdb_cnv_lane_dir(oneway)                      -- lane dir
               , rdb_cnv_lanecount_downflow(oneway, rdlnlane)  -- lane_num_s2e
               , rdb_cnv_lanecount_upflow(oneway, rdlnlane)    -- lane_num_e2s
               , case fw when 1 then 1 else 0 end as elevated  -- elevated
               , case when rdlnclass in (41,42,43,44,46) then 1 else 0 end as structure                             -- structure
               , case rdlnclass when 51 then 1 else 0 end   -- tunnel
               , 0 as rail_cross                            -- rail_cross
               , case when rdlnclass = 62 then 0 else 1 end as paved   -- paved
               , case when fw in (5, 21) then 1 else 0 end as uturn    -- uturn
               , rdb_cnv_speedlimit_downflow(oneway, spd_limit,
                        speed) as speed_limit_s2e           -- speed_limit_s2e
               , rdb_cnv_speedlimit_upflow(oneway, spd_limit,
                       speed) as speed_limit_e2s            -- speed_limit_e2s
               , rdb_cnv_speedsource_downflow(oneway, spd_limit,
                       speed) as speed_source_s2e           -- speed_source_s2e
               , rdb_cnv_speedsource_upflow(oneway, spd_limit,
                       speed) as speed_source_e2s           -- speed_source_e2s
               , get_width_downflow(oneway, rdlnwidth)      -- width_s2e
               , get_width_upflow(oneway, rdlnwidth)        -- width_e2s
               , (case oneway
                  when 'FT' then 2
                  when 'TF' then 3
                  else 1 end) as one_way_code               --one_way_code
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
               , dtype as disobey_flag                      -- disobey flag
               , dtype as up_down_distinguish               -- up_down_distinguish
               , null as accees                             --access
               , 1 as extend_flag                           --extend_flag
               , 0 as etc_only_flag                         --etc_only_flag
               , b.the_geom as the_geom
           from org_l_tran as a
           left outer join temp_topo_link b
           on a.routeid = b.routeid
           left join temp_link_name c
           on a.routeid = c.link_id
           left join temp_link_shield d
           on a.routeid = d.link_id;
          """

        self.log.info(': Now it is inserting to link_tbl...')
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
            self.log.info(': end of inserting link succeeded')

        return 0

    def _DoCreateIndex(self):

        self.CreateIndex2('link_tbl_link_id_idx')
        self.CreateIndex2('link_tbl_s_node_idx')
        self.CreateIndex2('link_tbl_e_node_idx')
        self.CreateIndex2('link_tbl_the_geom_idx')

        return 0
