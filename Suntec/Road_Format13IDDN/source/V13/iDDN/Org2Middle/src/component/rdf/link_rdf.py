'''
Created on 2012-3-23

@author: sunyifeng
'''

import base

class comp_link_rdf(base.component_base.comp_base):
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

        # create temp table for through traffic link.
        if self.CreateTable2('temp_link_through_traffic') == -1:
            return -1        
#            
#        # create temp table for railway cross link.
#        if self.CreateTable2('temp_link_railcross') == -1:
#            return -1         
        
        # create temp table for walkway.   
        if self.CreateTable2('temp_link_pedestrians') == -1:
            return -1                  

        # create temp table for under construction link.   
        if self.CreateTable2('temp_link_under_construction') == -1:
            return -1 
        
        # create temp table for pulic lane road. 
        if self.CreateTable2('temp_link_public') == -1:
            return -1         
        
        # create temp table for etc only road.

        if self.CreateFunction2('mid_make_etc_only_links_in_one_direction') == -1:
            return -1

        if self.CreateTable2('temp_etc_only_link') == -1:
            return -1          

        # create temp table for SA road.

        if self.CreateFunction2('mid_find_sa_link_in_diff_direction') == -1:
            return -1

        if self.CreateFunction2('mid_find_sa_link') == -1:
            return -1

        sqlcmd = """
            drop table if exists temp_link_sa_area;
            CREATE TABLE temp_link_sa_area
            (
             link_id integer
            ); 
                        
            select mid_find_sa_link();
            """

        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
                                    
#        # create temp table for u-turn link.
#        if self.CreateTable2('temp_link_lanes') == -1:
#            return -1  
#        if self.CreateTable2('temp_link_uturn') == -1:
#            return -1  
                
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

        if self.CreateFunction2('mid_cnv_display_class') == -1:
            return -1
                
        if self.CreateFunction2('mid_get_lane_dir') == -1:
            return -1
        
        if self.CreateFunction2('mid_get_lanecount_downflow') == -1:
            return -1
        
        if self.CreateFunction2('mid_get_lanecount_upflow') == -1:
            return -1
        
        if self.CreateFunction2('mid_get_oneway_code') == -1:
            return -1
        
        if self.CreateFunction2('mid_get_access') == -1:
            return -1
        
        if self.CreateFunction2('mid_get_extend_flag') == -1:
            return -1
        
        if self.CreateFunction2('mid_cnv_length') == -1:
            return -1
        
        if self.CreateFunction2('mid_get_regulationspeed') == -1:
            return -1
        
        if self.CreateFunction2('mid_get_speedsource') == -1:
            return -1
                        
        return 0
        
    def _Do(self):
        
        if self.CreateIndex2('temp_rdf_nav_link_link_id_idx') == -1:
            return -1
        
        if self.CreateIndex2('rdf_link_link_id_idx') == -1:
            return -1
        
        if self.CreateIndex2('rdf_nav_link_attribute_link_id_idx') == -1:
            return -1
        
        if self.CreateIndex2('rdf_condition_nav_strand_id_idx') == -1:
            return -1
        
        if self.CreateIndex2('temp_rdf_nav_link_access_id_idx') == -1:
            return -1
        
        if self.CreateIndex2('temp_rdf_nav_link_iso_country_code_idx') == -1:
            return -1
        
        if self.CreateIndex2('rdf_country_iso_country_code_idx') == -1:
            return -1

        if self.CreateIndex2('temp_link_through_traffic_link_id_idx') == -1:
            return -1
        
        if self.CreateIndex2('temp_link_pedestrians_link_id_idx') == -1:
            return -1
        
        if self.CreateIndex2('temp_link_public_link_id_idx') == -1:
            return -1

        if self.CreateIndex2('temp_link_under_construction_link_id_idx') == -1:
            return -1
                            
        if self.CreateIndex2('temp_link_sa_area_link_id_idx') == -1:
            return -1                                          
        sqlcmd = """
        insert into link_tbl (
            link_id, s_node, e_node, display_class, link_type, road_type, toll,  
            speed_class,length, function_class,lane_dir, lane_num_s2e,  
            lane_num_e2s, elevated, structure, tunnel, rail_cross, paved, uturn,
            speed_limit_s2e, speed_limit_e2s, speed_source_s2e, speed_source_e2s,
            width_s2e, width_e2s,one_way_code, one_way_condition, 
            pass_code, pass_code_condition, road_name, road_number, name_type,
            ownership, car_only, slope_code, slope_angle, disobey_flag, 
            up_down_distinguish, access, extend_flag, etc_only_flag, the_geom
        ) 
        SELECT    link_id, s_node, e_node, display_class, link_type, road_type, toll, 
                  speed_class, length, function_class, lane_dir
                , mid_get_lanecount_downflow(
                        one_way_code, 
                        from_ref_num_lanes,
                        to_ref_num_lanes,
                        physical_num_lanes
                    ) as lane_num_s2e                             -- lane_num_s2e
                , mid_get_lanecount_upflow(
                        one_way_code,
                        from_ref_num_lanes,
                        to_ref_num_lanes,
                        physical_num_lanes
                    ) as lane_num_e2s                             -- lane_num_e2s                       
                , elevated, structure, tunnel, rail_cross, paved, uturn
                , case when one_way_code in (4,3) then 0
                       else speed_limit_s2e 
                  end as speed_limit_s2e                             -- speed_limit_s2e
                , case when one_way_code in (4,2) then 0
                       else speed_limit_e2s 
                  end as speed_limit_e2s                             -- speed_limit_e2s
                , case when one_way_code in (4,3) and speed_limit_unit = 'M' then (1 << 3) | 0
                       when one_way_code in (4,3) then 0
                       else speed_source_s2e  
                  end as speed_source_s2e                            -- speed_source_s2e
                , case when one_way_code in (4,2) and speed_limit_unit = 'M' then (1 << 3) | 0
                       when one_way_code in (4,2) then 0
                       else speed_source_e2s 
                  end as speed_source_e2s                            -- speed_source_e2s
                , width_s2e
                , width_e2s
                , one_way_code, one_way_condition, pass_code, pass_code_condition, 
                  road_name, road_number, name_type, ownership, car_only, slope_code, 
                  slope_angle, disobey_flag, up_down_distinguish, access, extend_flag, 
                  etc_only_flag, the_geom                                 
        from (                
                select   a.link_id as link_id                          -- link_id
                   , a.ref_node_id  as s_node                      -- from node
                   , a.nonref_node_id   as e_node                  -- to node
                   , mid_cnv_display_class(
                        private,
                        boat_ferry,
                        rail_ferry,
                        t.link_id,
                        n.link_id,
                        functional_class,
                        controlled_access,
                        public_access,
                        speed_category
                      ) as display_class                           -- display_class
                   , mid_cnv_link_type(
                        intersection_category, 
                        ramp, 
                        frontage,
                        multi_digitized,
                        sa.link_id
                      ) as link_type                               -- link_type
                   , mid_cnv_road_type(
                        frontage, 
                        private,
                        boat_ferry,
                        rail_ferry,
                        n.link_id,
                        carpool_road,
                        i.link_id,
                        t.link_id,
                        functional_class,
                        controlled_access,
                        ramp,
                        public_access
                      ) as road_type                                  -- road type
                   , case tollway when 'Y' then 1 
                          else 2 
                     end as toll                                      -- toll
                   , speed_category as speed_class                    -- speed class
                   , ST_Length_Spheroid(a.the_geom,
                           'SPHEROID("WGS_84", 6378137, 298.257223563)'
                       ) as length                                    -- link_length
                   , functional_class as function_class               -- function_class
                   , mid_get_lane_dir(
                        travel_direction,
                        con.link_id, 
                        j.linkdir,
                        k.linkdir_array
                       ) as lane_dir                                 -- lane dir
                   , from_ref_num_lanes
                   , to_ref_num_lanes    
                   , physical_num_lanes                       
                   , 0 as elevated                                   -- elevated
                   , case a.bridge when 'Y' then 1 
                          else 0 
                     end as structure                                -- structure
                   , case a.tunnel when 'Y' then 1 
                          else 0 
                     end as tunnel                                   -- tunnel
                   , 0 as rail_cross                                 -- rail_cross
                   , case when a.paved = 'Y' then 1
                          else 0
                     end as paved                                    --paved
                   , 0 as uturn                                      --uturn
                   , b.speed_limit_unit
                   , mid_get_regulationspeed(
                        b.speed_limit_unit,
                        a.from_ref_speed_limit, 
                        a.speed_category
                       ) as speed_limit_s2e 
                   , mid_get_regulationspeed(
                        b.speed_limit_unit,
                        a.to_ref_speed_limit, 
                        a.speed_category
                       ) as speed_limit_e2s 
                   , mid_get_speedsource(
                        b.speed_limit_unit,
                        from_ref_speed_limit, 
                        speed_limit_source
                       ) as speed_source_s2e
                   , mid_get_speedsource(
                        b.speed_limit_unit,
                        to_ref_speed_limit, 
                        speed_limit_source
                       ) as speed_source_e2s 
                   , 4 as width_s2e                                     -- width_s2e
                   , 4 as width_e2s                                     -- width_e2s                       
                   , mid_get_oneway_code(
                        travel_direction,
                        con.link_id, 
                        j.linkdir,
                        k.linkdir_array
                       ) as one_way_code                             --one_way_code
                   , 0 as one_way_condition
                   , case m.through_traffic when 'N' then 0 
                        else 1 
                     end as pass_code
                   , 0 as pass_code_condition
                   , d.name as road_name                             -- road_name
                   , e.shield as road_number                         -- road_number
                   , 0 as name_type -- name_type
                   , case private when 'Y' then 1 
                        else 0 
                     end as ownership                                -- ownership
                   , null::smallint as car_only                      -- car only
                   , null::smallint as slope_code                    -- slope code
                   , null::smallint as slope_angle                   -- slope angle
                   , case divider when 'N' then 0 
                        else 1 
                     end as disobey_flag                             -- disobey flag
                   , case multi_digitized when 'Y' then 1 
                        else 0 
                     end as up_down_distinguish                      -- up_down_distinguish
                   , mid_get_access(
                        m.automobiles,
                        m.buses,
                        m.taxis,
                        m.carpools,
                        m.pedestrians,
                        m.trucks,
                        m.deliveries,
                        m.emergency_vehicles,
                        m.through_traffic,
                        m.motorcycles
                       ) as access                                    -- car access
                   , mid_get_extend_flag(
                        pdm.pdm_flag,
                        rc.driving_side
                       ) as extend_flag                               -- extend flag
                   , case when etc_links.link_id is not null then 1
                          else 0
                     end as etc_only_flag                             --etc_only_flag
                   , a.the_geom as the_geom                           --the_geom
                from temp_rdf_nav_link as a 
                left join rdf_country b
                    on a.iso_country_code = b.iso_country_code
                left outer join rdf_nav_link_attribute as c 
                    on a.link_id = c.link_id
                left join temp_link_name d
                    on a.link_id = d.link_id
                left join temp_link_shield e
                    on a.link_id = e.link_id 
                left outer join temp_link_regulation_forbid_traffic as j 
                    on a.link_id = j.link_id                                               
                left outer join temp_link_regulation_permit_traffic as k 
                    on a.link_id = k.link_id
                left outer join rdf_access as m 
                    on a.access_id = m.access_id
                left outer join temp_link_public as i 
                    on a.link_id = i.link_id
                left outer join temp_link_pedestrians as t 
                    on a.link_id = t.link_id
                left outer join temp_link_through_traffic as n 
                    on a.link_id = n.link_id
                left join temp_link_under_construction con
                    on a.link_id = con.link_id
                left join temp_link_sa_area sa
                    on a.link_id = sa.link_id
                left outer join (
                    select distinct iso_country_code, driving_side from rdf_country
                ) as rc
                    on a.iso_country_code = rc.iso_country_code                         
                left join temp_link_regulation_pdm_flag as pdm                   
                    on a.link_id = pdm.link_id
                left join
                (
                    select inlink as link_id from temp_etc_only_link
                    union
                    select outlink as link_id  from temp_etc_only_link
                    union
                    select unnest(inlink_array) as link_id  from temp_etc_only_link
                    union
                    select unnest(outlink_array) as link_id  from temp_etc_only_link
                ) as etc_links
                    on a.link_id = etc_links.link_id        
        ) a;
        """
        
        self.log.info('Now it is inserting to link_tbl...')
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
            self.log.info('Inserting link succeeded')   
                   
        return 0
           
