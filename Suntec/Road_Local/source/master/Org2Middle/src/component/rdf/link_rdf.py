'''
Created on 2012-3-23

@author: sunyifeng
'''

import component.component_base

class comp_link_rdf(component.component_base.comp_base):
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

        # create temp table for through traffic link.
        self.CreateTable2('temp_link_through_traffic')        
            
        # create temp table for railway cross link.         
        self.CreateTable2('temp_link_railwaycross')     

        # create temp table for walkway.   
        self.CreateTable2('temp_link_pedestrians')                 

        # create temp table for under construction link.   
        self.CreateTable2('temp_link_under_construction') 
        
        # create temp table for pulic lane road. 
        self.CreateTable2('temp_link_public')        
        
        # create temp table for etc only road.
        self.CreateFunction2('mid_make_etc_only_links_in_one_direction')
        self.CreateTable2('temp_etc_only_link')        

        # create temp table for SA road.
        self.CreateFunction2('mid_find_sa_link_in_diff_direction')
        self.CreateFunction2('mid_find_sa_link')

        sqlcmd = """
            drop table if exists temp_link_sa_area;
            CREATE TABLE temp_link_sa_area
            (
             link_id integer
            ); 
                        
            select mid_find_sa_link();
            """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
      
#        # create temp table for u-turn link.
#        self.CreateTable2('temp_link_lanes') 
#        self.CreateTable2('temp_link_uturn') 

        # create temp table for IPD road. 
        self.CreateTable2('temp_link_ipd') 

        # create temp table for Urban road. 
        self.CreateTable2('temp_link_urban')
        
        # create temp table for ERP road.
        self.CreateTable2('temp_link_erp') 
        self.pg.execute2("""
            insert into temp_link_erp(link_id, erp)
            select link_id
                ,case when 1 = any(flag_array) and 2 = any(flag_array) then 1
                    when 1 = any(flag_array) then 2
                    else 3 end as flag
            from (
                select array_agg(node_id) as node_array, link_id, array_agg(flag) as flag_array
                from(
                    select node_id,link_array[1] as link_id,flag_array[1] as flag
                    from (
                        select node_id,array_agg(link_id) as link_array,array_agg(flag) as flag_array
                        from (
                            select a.condition_id,a.nav_strand_id
                                ,b.seq_num,b.link_id,b.node_id
                                ,case when b.node_id = c.ref_node_id then 1
                                    when b.node_id = c.nonref_node_id then 2
                                 end as flag
                            from rdf_condition a
                            left join rdf_condition_toll a1
                            on a.condition_id = a1.condition_id
                            left join rdf_nav_strand b
                            on a.nav_strand_id = b.nav_strand_id
                            left join temp_rdf_nav_link c
                            on b.link_id = c.link_id
                            where c.iso_country_code in ('SGP','ARE') and a.condition_type = 1 and a1.structure_type_electronic = 'Y'
                        ) m group by node_id
                    ) n 
                ) o group by link_id
            ) p;                       
        """)

        # create temp table for Rodizio road. 
        self.CreateTable2('temp_link_rodizio')
        
        self.pg.execute2("""
            insert into temp_link_rodizio(link_id)
            select distinct  b.link_id
            from rdf_condition a
            left join rdf_nav_strand b
            on a.nav_strand_id = b.nav_strand_id
            left join temp_rdf_nav_link c
            on b.link_id = c.link_id
            where a.condition_type = 34 and c.iso_country_code = 'BRA'                       
        """)

        # create temp table for buslane. 
        self.CreateTable2('temp_link_buslane')
        
        if self.isCountry('ase'):
            self.pg.execute2("""
                insert into temp_link_buslane(link_id)
                SELECT distinct a.link_id
                FROM temp_rdf_nav_link a
                left join rdf_access b
                on a.access_id = b.access_id
                where b.automobiles = 'N' and b.buses = 'Y' and b.taxis = 'N' and b.carpools = 'N'
                and b.trucks = 'N' and b.deliveries = 'N';                        
            """)
            
        return 0

    def _DoCreateIndex(self):
        
        self.CreateIndex2('link_tbl_link_id_idx')
        self.CreateIndex2('link_tbl_s_node_idx')
        self.CreateIndex2('link_tbl_e_node_idx')
        self.CreateIndex2('link_tbl_the_geom_idx')
         
        return 0
        
    def _DoCreateFunction(self):
        
        self.CreateFunction2('mid_cnv_road_type')
        self.CreateFunction2('mid_cnv_link_type')
        self.CreateFunction2('mid_cnv_display_class')
        self.CreateFunction2('mid_get_lane_dir')
        self.CreateFunction2('mid_get_lanecount_downflow')
        self.CreateFunction2('mid_get_lanecount_upflow')
        self.CreateFunction2('mid_get_oneway_code')
        self.CreateFunction2('mid_get_access')
        self.CreateFunction2('mid_get_extend_flag')
        self.CreateFunction2('mid_cnv_length')
        self.CreateFunction2('mid_get_regulationspeed')
        self.CreateFunction2('mid_get_speedsource')
                        
        return 0
        
    def _Do(self):
        
        self.CreateIndex2('temp_rdf_nav_link_link_id_idx')
        self.CreateIndex2('rdf_link_link_id_idx')
        self.CreateIndex2('rdf_nav_link_attribute_link_id_idx')
        self.CreateIndex2('rdf_condition_nav_strand_id_idx')
        self.CreateIndex2('temp_rdf_nav_link_access_id_idx')
        self.CreateIndex2('temp_rdf_nav_link_iso_country_code_idx')
        self.CreateIndex2('rdf_country_iso_country_code_idx')
        
        self.CreateIndex2('temp_link_through_traffic_link_id_idx')
        self.CreateIndex2('temp_link_pedestrians_link_id_idx')
        self.CreateIndex2('temp_link_public_link_id_idx')
        self.CreateIndex2('temp_link_under_construction_link_id_idx')
        self.CreateIndex2('temp_link_sa_area_link_id_idx')                                          
        self.CreateIndex2('temp_link_railwaycross_link_id_idx')
        self.CreateIndex2('temp_link_ipd_link_id_idx')                                          
        self.CreateIndex2('temp_link_urban_link_id_idx')
        self.CreateIndex2('temp_link_erp_link_id_idx')                                          
        self.CreateIndex2('temp_link_rodizio_link_id_idx')
        self.CreateIndex2('temp_link_buslane_link_id_idx')
                        
        self.CreateTable2('temp_link_category6_roundabout')
        self.__Add_roundabout()
                                             
        sqlcmd = """
        insert into link_tbl (
            link_id, iso_country_code, s_node, e_node, display_class, link_type, road_type, toll,  
            speed_class,length, function_class,lane_dir, lane_num_s2e,  
            lane_num_e2s, elevated, structure, tunnel, rail_cross, paved, uturn,
            speed_limit_s2e, speed_limit_e2s, speed_source_s2e, speed_source_e2s,
            width_s2e, width_e2s,one_way_code, one_way_condition, 
            pass_code, pass_code_condition, road_name, road_number, name_type,
            ownership, car_only, slope_code, slope_angle, disobey_flag, 
            up_down_distinguish, access, extend_flag, etc_only_flag, ipd, urban,
            erp, rodizio, the_geom
        ) 
        SELECT    link_id, iso_country_code, s_node, e_node, display_class, link_type, road_type, toll, 
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
                  etc_only_flag, ipd, urban, erp, rodizio, the_geom                                 
        from (                
                select   a.link_id as link_id                          -- link_id
                   , a.iso_country_code as iso_country_code            --iso_country_code
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
                        speed_category,
                        c.four_wheel_drive,
                        bus.link_id
                      ) as display_class                           -- display_class
                   , (case when category6.link_id is not null then 0 else
                       mid_cnv_link_type(
                        intersection_category, 
                        ramp, 
                        frontage,
                        multi_digitized,
                        sa.link_id
                      ) end) as link_type                               -- link_type
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
                   , case a.bridge when 'Y' then 3 
                          else 0 
                     end as structure                                -- structure
                   , case a.tunnel when 'Y' then 1 
                          else 0 
                     end as tunnel                                   -- tunnel
                   , case tlr.flag when 2 then 1
                       when 0 then 2
                       when 1 then 3
                       else 0
                     end as rail_cross                               -- rail_cross
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
                   , case when ipd.link_id is not null then 1
                          else 0
                     end as ipd                                       --ipd
                   , case when urban.link_id is not null then 1
                          else 0
                     end as urban                                     --urban
                   , case when erp.link_id is not null then erp.erp
                          else 0
                     end as erp                                       --erp 
                   , case when rodizio.link_id is not null then 1
                          else 0
                     end as rodizio                                   --rodizio                                                                
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
                left join temp_link_category6_roundabout as category6
                    on a.link_id = category6.link_id
				left join temp_link_railwaycross as tlr
                    on a.link_id = tlr.link_id
                left join temp_link_ipd as ipd
                    on a.link_id = ipd.link_id
                left join temp_link_urban as urban
                    on a.link_id = urban.link_id
                left join temp_link_erp as erp
                    on a.link_id = erp.link_id
                left join temp_link_rodizio as rodizio
                    on a.link_id = rodizio.link_id
                left join temp_link_buslane as bus
                    on a.link_id = bus.link_id                    
        ) a;
        """
        
        self.log.info('Now it is inserting to link_tbl...')
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
            self.log.info('Inserting link succeeded')   
                   
        return 0
    
    def __Add_roundabout(self):
        self.log.info('start add roundabout!')
        
        if self.CreateFunction2('mid_cal_zm') == -1:
            return -1
        if self.CreateFunction2('mid_jude_link_angle') == -1:
            return -1
        if self.CreateFunction2('mid_get_round_link') == -1:
            return -1
        
        if self.CreateTable2('temp_link_category6') == -1:
            return -1
        if self.CreateTable2('temp_node_connect_acute_link') == -1:
            return -1
        
        sqlcmd = '''
            delete from temp_link_category6
            where link_id in (
                select link_id
                from temp_link_category6 as a
                left join  temp_node_connect_acute_link as b
                on b.node_id in (a.s_node,a.e_node)
                where b.node_id is not null
            );
            '''
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
        
        sqlcmd = '''
            insert into temp_link_category6_roundabout
            (
                select link_id,the_geom
                from temp_link_category6
                where mid_get_round_link(link_id,s_node,e_node,one_way_code)
            );
            '''
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
            
        if self.CreateIndex2('temp_link_category6_roundabout_link_id_idx') == -1:
            return -1
        
        self.log.info('end add roundabout!')
           
