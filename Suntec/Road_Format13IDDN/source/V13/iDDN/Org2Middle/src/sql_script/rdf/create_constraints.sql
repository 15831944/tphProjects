
-- rdf_road_link
CREATE INDEX rdf_road_link_link_id_idx
  ON rdf_road_link
  USING btree
  (link_id);
  
CREATE INDEX rdf_road_link_road_name_id_idx
  ON rdf_road_link
  USING btree
  (road_name_id);
  
--
  
CREATE INDEX temp_rdf_nav_link_link_id_idx
  ON temp_rdf_nav_link
  USING btree
  (link_id);
  
CREATE INDEX temp_rdf_nav_link_ref_node_id_idx
  ON temp_rdf_nav_link
  USING btree
  (ref_node_id);
  
CREATE INDEX temp_rdf_nav_link_nonref_node_id_idx
  ON temp_rdf_nav_link
  USING btree
  (nonref_node_id);
  
CREATE INDEX temp_rdf_nav_node_node_id_idx
  ON temp_rdf_nav_node
  USING btree
  (node_id);
  
CREATE INDEX rdf_link_link_id_idx
  ON rdf_link
  USING btree
  (link_id);
  
CREATE INDEX rdf_nav_link_attribute_link_id_idx
  ON rdf_nav_link_attribute
  USING btree
  (link_id);
   
CREATE INDEX rdf_condition_nav_strand_id_idx
  ON rdf_condition
  USING btree
  (nav_strand_id);
  
CREATE INDEX lane_tbl_inlinkid_nodeid_idx
  ON lane_tbl
  USING btree
  (inlinkid, nodeid);
  
CREATE INDEX rdf_lane_nav_strand_lane_id_idx
  ON rdf_lane_nav_strand
  USING btree
  (lane_id);

CREATE INDEX rdf_lane_nav_strand_condition_id_idx
  ON rdf_lane_nav_strand
  USING btree
  (condition_id);
  
CREATE INDEX rdf_lane_lane_id_idx
  ON rdf_lane
  USING btree
  (lane_id);
  
CREATE INDEX rdf_lane_link_id_idx
  ON rdf_lane
  USING btree
  (link_id);
  
CREATE INDEX temp_rdf_nav_link_iso_country_code_idx
  ON temp_rdf_nav_link
  USING btree
  (iso_country_code);
  
CREATE INDEX rdb_zone_country_code_idx
  ON rdb_zone
  USING btree
  (country_code);
  
CREATE INDEX temp_rdf_nav_link_access_id_idx
  ON temp_rdf_nav_link
  USING btree
  (access_id);
  
CREATE INDEX rdf_admin_hierarchy_iso_country_code_idx
  ON rdf_admin_hierarchy
  USING btree
  (iso_country_code);
  
CREATE INDEX rdf_country_iso_country_code_idx
  ON rdf_country
  USING btree
  (iso_country_code);
  
--------------------------------------------------------------------------------
-- temp table index for link
--------------------------------------------------------------------------------  
CREATE INDEX temp_link_through_traffic_link_id_idx
  ON temp_link_through_traffic
  USING btree
  (link_id);
 
CREATE INDEX temp_link_pedestrians_link_id_idx
  ON temp_link_pedestrians
  USING btree
  (link_id);

CREATE INDEX temp_link_public_link_id_idx
  ON temp_link_public
  USING btree
  (link_id);
  
CREATE INDEX temp_link_under_construction_link_id_idx
  ON temp_link_under_construction
  USING btree
  (link_id);
  
CREATE INDEX temp_link_sa_area_link_id_idx
  ON temp_link_sa_area
  USING btree
  (link_id);

CREATE INDEX temp_node_light_node_id_idx
  ON temp_node_light
  USING btree
  (node_id);
  
CREATE INDEX temp_node_toll_node_id_idx
  ON temp_node_toll
  USING btree
  (node_id);  
--------------------------------------------------------------------------------
-- temp
--------------------------------------------------------------------------------

CREATE INDEX temp_awr_mainnode_sublink_sublink_idx
  ON temp_awr_mainnode_sublink
  USING btree
  (sublink);

CREATE INDEX temp_awr_mainnode_subnode_subnode_idx
  ON temp_awr_mainnode_subnode
  USING btree
  (subnode);

CREATE INDEX temp_awr_mainnode_mainnode_id_idx
  ON temp_awr_mainnode
  USING btree
  (mainnode_id);

CREATE INDEX temp_link_regulation_forbid_traffic_link_id_idx
  ON temp_link_regulation_forbid_traffic
  USING btree
  (link_id);

CREATE INDEX temp_link_regulation_permit_traffic_link_id_idx
  ON temp_link_regulation_permit_traffic
  USING btree
  (link_id);

CREATE INDEX temp_rdf_nav_link_traffic_link_id_idx
  ON temp_rdf_nav_link_traffic
  USING btree
  (link_id);

CREATE INDEX temp_rdf_nav_link_traffic_ref_node_id_idx
  ON temp_rdf_nav_link_traffic
  USING btree
  (ref_node_id);

CREATE INDEX temp_rdf_nav_link_traffic_nonref_node_id_idx
  ON temp_rdf_nav_link_traffic
  USING btree
  (nonref_node_id);


CREATE INDEX temp_node_nation_boundary_node_id_idx
  ON temp_node_nation_boundary
  USING btree
  (node_id);

CREATE INDEX rdf_admin_hierarchy_admin_place_id_idx
  ON rdf_admin_hierarchy
  USING btree
  (admin_place_id);

CREATE INDEX rdf_admin_hierarchy_country_id_idx
  ON rdf_admin_hierarchy
  USING btree
  (country_id);

CREATE INDEX rdf_admin_hierarchy_order1_id_idx
  ON rdf_admin_hierarchy
  USING btree
  (order1_id);

CREATE INDEX rdf_admin_hierarchy_order2_id_idx
  ON rdf_admin_hierarchy
  USING btree
  (order2_id);

CREATE INDEX rdf_admin_hierarchy_order8_id_idx
  ON rdf_admin_hierarchy
  USING btree
  (order8_id);

CREATE INDEX rdf_admin_hierarchy_builtup_id_idx
  ON rdf_admin_hierarchy
  USING btree
  (builtup_id);

CREATE INDEX temp_regulation_admin_admin_place_id_idx
  ON temp_regulation_admin
  USING btree
  (admin_place_id);

CREATE INDEX temp_regulation_admin_link_link_id_idx
  ON temp_regulation_admin_link
  USING btree
  (link_id);

CREATE INDEX temp_regulation_admin_link_s_node_idx
  ON temp_regulation_admin_link
  USING btree
  (s_node);

CREATE INDEX temp_regulation_admin_link_e_node_idx
  ON temp_regulation_admin_link
  USING btree
  (e_node);

CREATE INDEX temp_regulation_admin_node_node_id_idx
  ON temp_regulation_admin_node
  USING btree
  (node_id);

CREATE INDEX temp_awr_mainnode_uturn_mainnode_id_idx
  ON temp_awr_mainnode_uturn
  USING btree
  (mainnode_id);

CREATE INDEX temp_awr_node_uturn_node_id_idx
  ON temp_awr_node_uturn
  USING btree
  (node_id);

CREATE INDEX temp_awr_pdm_linkrow_regulation_id_idx
  ON temp_awr_pdm_linkrow
  USING btree
  (regulation_id);

CREATE INDEX temp_awr_pdm_linkrow_first_link_idx
  ON temp_awr_pdm_linkrow
  USING btree
  (first_link);



CREATE INDEX temp_link_regulation_pdm_flag_link_id_idx
  ON temp_link_regulation_pdm_flag
  USING btree
  (link_id);



CREATE INDEX temp_rdf_nav_link_the_geom_idx
  ON temp_rdf_nav_link
  USING gist
  (the_geom);

 CREATE INDEX temp_rdf_nav_link_nonref_node_id_idx
  ON temp_rdf_nav_link
  USING btree
  (nonref_node_id);

-- Index: temp_rdf_nav_link_ref_node_id_idx

-- DROP INDEX temp_rdf_nav_link_ref_node_id_idx;

CREATE INDEX temp_rdf_nav_link_ref_node_id_idx
  ON temp_rdf_nav_link
  USING btree
  (ref_node_id);

CREATE INDEX temp_rdf_nav_link_left_admin_place_id_idx
  ON temp_rdf_nav_link
  USING btree
  (left_admin_place_id);

CREATE INDEX temp_rdf_nav_link_right_admin_place_id_idx
  ON temp_rdf_nav_link
  USING btree
  (right_admin_place_id);
  
CREATE INDEX temp_rdf_nav_node_the_geom_idx
  ON temp_rdf_nav_node
  USING gist
  (the_geom);

CREATE INDEX temp_wkt_building_the_geom_idx
  ON temp_wkt_building
  USING gist
  (the_geom);

CREATE INDEX temp_wkt_face_the_geom_idx
  ON temp_wkt_face
  USING gist
  (the_geom);

CREATE INDEX temp_wkt_link_the_geom_idx
  ON temp_wkt_link
  USING gist
  (the_geom);

CREATE INDEX temp_wkt_location_the_geom_idx
  ON temp_wkt_location
  USING gist
  (the_geom);

CREATE INDEX temp_wkt_node_the_geom_idx
  ON temp_wkt_node
  USING gist
  (the_geom);
  
--------------------------------------------------------------------------------
-- 900913
--------------------------------------------------------------------------------
CREATE INDEX temp_wkt_building_900913_the_geom_idx
  ON temp_wkt_building_900913
  USING gist
  (the_geom);

CREATE INDEX temp_wkt_face_900913_the_geom_idx
  ON temp_wkt_face_900913
  USING gist
  (the_geom);

CREATE INDEX temp_wkt_link_900913_the_geom_idx
  ON temp_wkt_link_900913
  USING gist
  (the_geom);

CREATE INDEX temp_wkt_location_900913_the_geom_idx
  ON temp_wkt_location_900913
  USING gist
  (the_geom);
  
--------------------------------------------------------------------------------
-- signpost
--------------------------------------------------------------------------------
CREATE INDEX towardname_tbl_in_and_out_link_index
  ON towardname_tbl
  USING btree
  (inlinkid, outlinkid);
  
  --------------------------------------------------------------------------------
-- guide lane
--------------------------------------------------------------------------------
CREATE INDEX temp_lane_guide_nav_strand_on_lane_nav_strand_id_index
  ON temp_lane_guide_nav_strand
  USING btree
  (lane_nav_strand_id);
  
  
 CREATE INDEX temp_lane_guide_distinct_inlink_id_and_nodeid_index
  ON temp_lane_guide_distinct
  USING btree
  (inlink_id,node_id);
  
  CREATE INDEX rdf_lane_link_id_and_lane_travel_direction_index
  ON rdf_lane
  USING btree
  (link_id,lane_travel_direction);
  
  CREATE INDEX rdf_access_allcolum_index
  ON rdf_access
  USING btree
  (
  automobiles,
  buses,
  taxis,
  carpools,
  pedestrians,
  trucks,
  deliveries,
  emergency_vehicles,
  through_traffic,
  motorcycles
  );
  
 

CREATE INDEX wkt_building_building_id_idx
  ON wkt_building
  USING btree
  (building_id);

CREATE INDEX wkt_face_face_id_idx
  ON wkt_face
  USING btree
  (face_id);

CREATE INDEX wkt_link_link_id_idx
  ON wkt_link
  USING btree
  (link_id);

CREATE INDEX wkt_location_location_id_idx
  ON wkt_location
  USING btree
  (location_id);

CREATE INDEX wkt_node_node_id_idx
  ON wkt_node
  USING btree
  (node_id);

-------------------------------------------------------------------------------------

CREATE INDEX temp_link_name_link_id_idx
  ON temp_link_name
  USING btree
  (link_id);

CREATE INDEX mid_temp_link_name_link_id_idx
  ON mid_temp_link_name
  USING btree
  (link_id);
  
-------------------------------------------------------------------------------------
CREATE INDEX temp_link_shield_link_id_idx
  ON temp_link_shield
  USING btree
  (link_id);

CREATE INDEX mid_temp_link_number_link_id_idx
  ON mid_temp_link_number
  USING btree
  (link_id);
  
-------------------------------------------------------------------------------------
CREATE INDEX mid_temp_road_name_link_id_idx
  ON mid_temp_road_name
  USING btree
  (link_id);

-------------------------------------------------------------------------------------
CREATE INDEX mid_temp_road_name_route_type_idx
  ON mid_temp_road_name
  USING btree
  (route_type);

--------------------------------------------------------------------------------------
----add by xuwenbo
--------------------------------------------------------------------------------------
CREATE INDEX rdf_country_country_id_idx
  ON rdf_country
  USING btree
  (country_id);

CREATE INDEX rdf_admin_hierarchy_admin_place_id_idx
  ON rdf_admin_hierarchy
  USING btree
  (admin_place_id);

CREATE INDEX rdf_feature_names_feature_id_idx
  ON rdf_feature_names
  USING btree
  (feature_id);

CREATE INDEX rdf_feature_name_name_id_idx
  ON rdf_feature_name
  USING btree
  (name_id);

CREATE INDEX rdf_carto_named_place_id_idx
  ON rdf_carto
  USING btree
  (named_place_id);

CREATE INDEX rdf_carto_face_carto_id_idx
  ON rdf_carto_face
  USING btree
  (carto_id);

CREATE INDEX temp_wkt_face_face_id_idx
  ON temp_wkt_face
  USING btree
  (face_id);

CREATE INDEX mid_temp_feature_name_feature_id_idx
  ON mid_temp_feature_name
  USING btree
  (feature_id);

