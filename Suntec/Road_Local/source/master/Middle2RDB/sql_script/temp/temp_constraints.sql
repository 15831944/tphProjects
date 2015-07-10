CREATE UNIQUE INDEX temp_name_dictionary_old_name_id_idx
  ON temp_name_dictionary
  USING btree
  (old_name_id);

-- tmp_point_list
CREATE UNIQUE INDEX temp_point_list_image_id_idx
  ON temp_point_list
  USING btree
  (image_id);
  

-- link_tbl
CREATE UNIQUE INDEX link_tbl_link_id_idx
  ON link_tbl
  USING btree
  (link_id);

CREATE INDEX link_tbl_tile_id_idx
  ON link_tbl
  USING btree
  (tile_id);
  
CREATE INDEX link_tbl_s_node_idx
  ON link_tbl
  USING btree
  (s_node);
  
CREATE INDEX link_tbl_e_node_idx
  ON link_tbl
  USING btree
  (e_node);
  
CREATE INDEX link_tbl_feature_key_idx
  ON link_tbl
  USING btree
  (feature_key);
  
CREATE INDEX link_tbl_the_geom_idx
  ON link_tbl
  USING gist
  (the_geom);
  

-- node_tbl
CREATE UNIQUE INDEX node_tbl_node_id_idx
  ON node_tbl
  USING btree
  (node_id);

CREATE INDEX node_tbl_tile_id_idx
  ON node_tbl
  USING btree
  (tile_id);
  
CREATE INDEX node_tbl_feature_key_idx
  ON node_tbl
  USING btree
  (feature_key);
  
CREATE INDEX node_tbl_the_geom_idx
  ON node_tbl
  USING gist
  (the_geom);
  
  
-- lane_tbl
CREATE INDEX lane_tbl_node_id_idx
  ON lane_tbl
  USING btree
  (node_id);
  
-- signpost_tbl
CREATE INDEX signpost_tbl_node_id_idx
  ON signpost_tbl
  USING btree
  (node_id);

-- spotguide_tbl
CREATE INDEX spotguide_tbl_node_id_idx
  ON spotguide_tbl
  USING btree
  (node_id);

-- towardname_tbl
CREATE INDEX towardname_tbl_node_id_idx
  ON towardname_tbl
  USING btree
  (node_id);
  
-- name_dictionary_tbl  
CREATE INDEX name_dictionary_tbl_name_id_idx
  ON name_dictionary_tbl
  USING btree
  (name_id);
  
-- temp_tile_admin_zone
CREATE INDEX temp_tile_level_6_the_geom_idx
  ON temp_tile_level_6
  USING gist
  (the_geom);

CREATE INDEX temp_tile_level_8_the_geom_idx
  ON temp_tile_level_8
  USING gist
  (the_geom);

CREATE INDEX temp_tile_level_10_the_geom_idx
  ON temp_tile_level_10
  USING gist
  (the_geom);

CREATE INDEX temp_tile_level_10_admin_zone_ad_code_idx
  ON temp_tile_level_10_admin_zone
  USING btree
  (ad_code);

CREATE INDEX temp_tile_level_12_ad_code_idx
  ON temp_tile_level_12
  USING btree
  (ad_code);

CREATE INDEX temp_tile_level_12_the_geom_idx
  ON temp_tile_level_12
  USING gist
  (the_geom);

CREATE INDEX temp_tile_level_12_admin_zone_ad_code_idx
  ON temp_tile_level_12_admin_zone
  USING btree
  (ad_code);

CREATE INDEX temp_tile_level_14_intersect_tile_id_idx
  ON temp_tile_level_14_intersect
  USING btree
  (tile_id);

CREATE INDEX temp_tile_level_14_intersect_tile_xy_idx
  ON temp_tile_level_14_intersect
  USING btree
  (tile_x, tile_y);

CREATE INDEX temp_tile_level_14_intersect_ad_code_idx
  ON temp_tile_level_14_intersect
  USING btree
  (ad_code);

CREATE INDEX temp_tile_level_14_intersect_b_contain_idx
  ON temp_tile_level_14_intersect
  USING btree
  (b_contain);

CREATE INDEX temp_tile_level_14_intersect_the_geom_idx
  ON temp_tile_level_14_intersect
  USING gist
  (the_geom);

CREATE INDEX temp_tile_level_14_tile_id_idx
  ON temp_tile_level_14
  USING btree
  (tile_id);

CREATE INDEX temp_tile_level_14_tile_xy_idx
  ON temp_tile_level_14
  USING btree
  (tile_x, tile_y);

CREATE INDEX temp_tile_level_14_b_contain_idx
  ON temp_tile_level_14
  USING btree
  (b_contain);

CREATE INDEX temp_tile_level_14_the_geom_idx
  ON temp_tile_level_14
  USING gist
  (the_geom);

CREATE INDEX temp_tile_level_14_admin_zone_tile_id_idx
  ON temp_tile_level_14_admin_zone
  USING btree
  (tile_id);

CREATE INDEX temp_tile_level_14_admin_zone_ad_code_idx
  ON temp_tile_level_14_admin_zone
  USING btree
  (ad_code);

CREATE INDEX temp_tile_level_14_admin_zone_the_geom_idx
  ON temp_tile_level_14_admin_zone
  USING gist
  (the_geom);

CREATE INDEX temp_tile_level_14_admin_zone_order8_multi_tile_id_idx
  ON temp_tile_level_14_admin_zone_order8_multi
  USING btree
  (ad_code);

CREATE INDEX temp_tile_level_14_admin_zone_order8_tile_id_idx
  ON temp_tile_level_14_admin_zone_order8
  USING btree
  (ad_code);

CREATE INDEX temp_tile_level_14_admin_zone_order8_ad_code_idx
  ON temp_tile_level_14_admin_zone_order8
  USING btree
  (ad_code);

CREATE INDEX temp_tile_level_14_admin_zone_order8_tile_xy_idx
  ON temp_tile_level_14_admin_zone_order8
  USING btree
  (tile_x, tile_y);

CREATE INDEX temp_tile_admin_zone_tile_id_idx
  ON temp_tile_admin_zone
  USING btree
  (tile_id);

CREATE INDEX temp_tile_admin_zone_outer_polygon_gid_idx
  ON temp_tile_admin_zone
  USING btree
  (outer_polygon_gid);

CREATE INDEX temp_tile_admin_zone_hole_outer_polygon_gid_idx
  ON temp_tile_admin_zone_hole
  USING btree
  (outer_polygon_gid);

CREATE INDEX temp_tile_admin_zone_hole_tmp2old_tmp_polygon_gid_idx
  ON temp_tile_admin_zone_hole_tmp2old
  USING btree
  (tmp_polygon_gid);

CREATE INDEX temp_tile_admin_zone_hole_tile_id_idx
  ON temp_tile_admin_zone_hole
  USING btree
  (tile_id);

CREATE INDEX temp_tile_admin_zone_simple_edge_a_polygon_gid_idx
  ON temp_tile_admin_zone_simple_edge
  USING btree
  (a_polygon_gid );

CREATE INDEX temp_tile_admin_zone_simple_edge_b_polygon_gid_idx
  ON temp_tile_admin_zone_simple_edge
  USING btree
  (b_polygon_gid );

CREATE INDEX temp_tile_admin_zone_simple_tile_id_ad_code_polygon_gid_idx
  ON temp_tile_admin_zone_simple
  USING btree
  (tile_id, ad_code, polygon_gid);




CREATE INDEX temp_simplified_admin_zone_gid_idx
  ON temp_simplified_admin_zone
  USING btree
  (gid);

CREATE INDEX temp_simplified_admin_zone_ad_code_idx
  ON temp_simplified_admin_zone
  USING btree
  (ad_code);

CREATE INDEX temp_simplified_admin_zone_order1_id_idx
  ON temp_simplified_admin_zone
  USING btree
  (order1_id);

CREATE INDEX temp_simplified_admin_zone_the_geom_idx
  ON temp_simplified_admin_zone
  USING gist
  (the_geom);

CREATE INDEX temp_simplified_province_gid_idx
  ON temp_simplified_province
  USING btree
  (gid);

CREATE INDEX temp_simplified_province_ad_code_idx
  ON temp_simplified_province
  USING btree
  (ad_code);

CREATE INDEX temp_simplified_province_the_geom_idx
  ON temp_simplified_province
  USING gist
  (the_geom);


CREATE INDEX temp_simplifying_admin_zone_outer_polygon_gid_idx
  ON temp_simplifying_admin_zone
  USING btree
  (outer_polygon_gid);

CREATE INDEX temp_simplifying_admin_zone_the_geom_idx
  ON temp_simplifying_admin_zone
  USING gist
  (the_geom);

CREATE INDEX temp_simplifying_admin_zone_hole_outer_polygon_gid_idx
  ON temp_simplifying_admin_zone_hole
  USING btree
  (outer_polygon_gid);

CREATE INDEX temp_simplifying_admin_zone_hole_tmp2old_tmp_polygon_gid_idx
  ON temp_simplifying_admin_zone_hole_tmp2old
  USING btree
  (tmp_polygon_gid);

CREATE INDEX temp_simplifying_admin_zone_edge_a_polygon_gid_idx
  ON temp_simplifying_admin_zone_edge
  USING btree
  (a_polygon_gid );

CREATE INDEX temp_simplifying_admin_zone_edge_b_polygon_gid_idx
  ON temp_simplifying_admin_zone_edge
  USING btree
  (b_polygon_gid );

CREATE INDEX temp_simplifying_admin_zone_edge_simple_a_polygon_gid_idx
  ON temp_simplifying_admin_zone_edge_simple
  USING btree
  (a_polygon_gid );

CREATE INDEX temp_simplifying_admin_zone_edge_simple_b_polygon_gid_idx
  ON temp_simplifying_admin_zone_edge_simple
  USING btree
  (b_polygon_gid );

CREATE INDEX temp_simplifying_admin_zone_simple_ad_code_polygon_gid_idx
  ON temp_simplifying_admin_zone_simple
  USING btree
  (ad_code, polygon_gid);


CREATE INDEX temp_region_orglink_level4_link_id_idx
	on temp_region_orglink_level4
	using btree
	(link_id);

CREATE INDEX temp_region_orglink_level4_start_node_id_idx
	on temp_region_orglink_level4
	using btree
	(start_node_id);

CREATE INDEX temp_region_orglink_level4_end_node_id_idx
	on temp_region_orglink_level4
	using btree
	(end_node_id);

CREATE INDEX temp_region_orgnode_level4_node_id_idx
	on temp_region_orgnode_level4
	using btree
	(node_id);

CREATE INDEX temp_region_orglink_level6_link_id_idx
	on temp_region_orglink_level6
	using btree
	(link_id);

CREATE INDEX temp_region_orglink_level6_start_node_id_idx
	on temp_region_orglink_level6
	using btree
	(start_node_id);

CREATE INDEX temp_region_orglink_level6_end_node_id_idx
	on temp_region_orglink_level6
	using btree
	(end_node_id);

CREATE INDEX temp_region_orgnode_level6_node_id_idx
	on temp_region_orgnode_level6
	using btree
	(node_id);

CREATE INDEX temp_region_orglink_level8_link_id_idx
	on temp_region_orglink_level8
	using btree
	(link_id);

CREATE INDEX temp_region_orglink_level8_start_node_id_idx
	on temp_region_orglink_level8
	using btree
	(start_node_id);

CREATE INDEX temp_region_orglink_level8_end_node_id_idx
	on temp_region_orglink_level8
	using btree
	(end_node_id);

CREATE INDEX temp_region_orgnode_level8_node_id_idx
	on temp_region_orgnode_level8
	using btree
	(node_id);

CREATE INDEX temp_region_delete_ic_link_id_idx
	on temp_region_delete_ic
	using btree
	(link_id);

CREATE INDEX temp_region_walked_link_link_id_idx
	on temp_region_walked_link
	using btree
	(link_id);

CREATE INDEX temp_region_links_sa_path_ftnode_idx
	on temp_region_links_sa_path
	using btree
	(fnode, tnode);

CREATE INDEX temp_region_links_sa_ramp_link_id_idx
	on temp_region_links_sa_ramp
	using btree
	(link_id);

CREATE INDEX temp_region_orglink_level6_bak_delete_ic_link_id_idx
	on temp_region_orglink_level6_bak_delete_ic
	using btree
	(link_id);

CREATE INDEX temp_region_orglink_level6_bak_delete_link_link_id_idx
	on temp_region_orglink_level6_bak_delete_link
	using btree
	(link_id);

CREATE INDEX temp_region_links_link_id_idx
	on temp_region_links
	using btree
	(link_id);

CREATE INDEX temp_region_links_start_node_id_idx
	on temp_region_links
	using btree
	(start_node_id);

CREATE INDEX temp_region_links_end_node_id_idx
	on temp_region_links
	using btree
	(end_node_id);

CREATE INDEX temp_region_nodes_node_id_idx
	on temp_region_nodes
	using btree
	(node_id);

CREATE INDEX temp_region_merge_node_keep_node_id_idx
	on temp_region_merge_node_keep
	using btree
	(node_id);

CREATE INDEX temp_region_merge_link_keep_link_id_idx
	on temp_region_merge_link_keep
	using btree
	(link_id);

CREATE INDEX temp_region_merge_node_suspect_node_id_idx
	on temp_region_merge_node_suspect
	using btree
	(node_id);

CREATE INDEX temp_region_merge_link_suspect_start_node_id_idx
	on temp_region_merge_link_suspect
	using btree
	(start_node_id);

CREATE INDEX temp_region_merge_link_suspect_end_node_id_idx
	on temp_region_merge_link_suspect
	using btree
	(end_node_id);

CREATE INDEX temp_region_merge_linkrow_start_node_id_idx
	on temp_region_merge_linkrow
	using btree
	(start_node_id);

CREATE INDEX temp_region_merge_linkrow_end_node_id_idx
	on temp_region_merge_linkrow
	using btree
	(end_node_id);

CREATE INDEX temp_region_merge_linkrow_start_node_id_end_node_id_idx
	on temp_region_merge_linkrow
	using btree
	(start_node_id, end_node_id);

CREATE INDEX temp_region_merge_link_mapping_link_id_idx
	on temp_region_merge_link_mapping
	using btree
	(link_id);

CREATE INDEX temp_region_merge_joinlink_proxy_link_id_idx
	on temp_region_merge_joinlink
	using btree
	(proxy_link_id);

CREATE INDEX temp_region_merge_links_oldid_link_id_idx
	on temp_region_merge_links_oldid
	using btree
	(link_id);

CREATE INDEX temp_region_merge_links_oldid_start_node_id_idx
	on temp_region_merge_links_oldid
	using btree
	(start_node_id);

CREATE INDEX temp_region_merge_links_oldid_end_node_id_idx
	on temp_region_merge_links_oldid
	using btree
	(end_node_id);

CREATE INDEX temp_region_merge_links_oldid_the_geom_idx
  ON temp_region_merge_links_oldid
  USING gist
  (the_geom);ALTER TABLE temp_region_merge_links_oldid CLUSTER ON temp_region_merge_links_oldid_the_geom_idx;

CREATE INDEX temp_region_merge_nodes_oldid_node_id_idx
	on temp_region_merge_nodes_oldid
	using btree
	(node_id);

CREATE INDEX temp_region_merge_nodes_oldid_the_geom_idx
  ON temp_region_merge_nodes_oldid
  USING gist
  (the_geom);ALTER TABLE temp_region_merge_nodes_oldid CLUSTER ON temp_region_merge_nodes_oldid_the_geom_idx;

CREATE INDEX temp_region_merge_linkid_mapping_link_id_idx
	on temp_region_merge_linkid_mapping
	using btree
	(link_id);

CREATE INDEX temp_region_merge_linkid_mapping_old_link_id_idx
	on temp_region_merge_linkid_mapping
	using btree
	(old_link_id);

CREATE INDEX temp_region_merge_abslinkid_mapping_old_link_id_idx
	on temp_region_merge_abslinkid_mapping
	using btree
	(old_link_id);

CREATE INDEX temp_region_merge_nodeid_mapping_node_id_idx
	on temp_region_merge_nodeid_mapping
	using btree
	(node_id);

CREATE INDEX temp_region_merge_nodeid_mapping_old_node_id_idx
	on temp_region_merge_nodeid_mapping
	using btree
	(old_node_id);

CREATE INDEX temp_region_merge_links_link_id_idx
	on temp_region_merge_links
	using btree
	(link_id);

CREATE INDEX temp_region_merge_nodes_node_id_idx
	on temp_region_merge_nodes
	using btree
	(node_id);

CREATE INDEX temp_region_merge_regulation_record_no_idx
	on temp_region_merge_regulation
	using btree
	(record_no);
    
CREATE INDEX temp_link_tbl_tile_id_idx
  ON temp_link_tbl
  USING btree
  (tile_id);	
    
CREATE INDEX temp_link_tbl_link_id_idx
  ON temp_link_tbl
  USING btree
  (link_id);		

CREATE INDEX temp_link_tbl_tile_id_common_idx
  ON temp_link_tbl
  USING btree
  (tile_id,common_main_link_attri);

CREATE INDEX temp_link_tbl_s_node_idx
  ON temp_link_tbl
  USING btree
  (s_node);

CREATE INDEX temp_link_tbl_e_node_idx
  ON temp_link_tbl
  USING btree
  (e_node);

CREATE INDEX temp_link_tbl_common_attri_idx
  ON temp_link_tbl
  USING btree
  (common_main_link_attri);  

CREATE INDEX temp_link_tbl_walked_link_id
  ON temp_link_tbl_walked
  USING btree
  (link_id);

CREATE INDEX temp_link_tbl_newid_tile_id_idx
  ON temp_link_tbl_newid
  USING btree
  (tile_id);


CREATE INDEX temp_merge_link_lane_info_lane_id_idx
  ON temp_merge_link_lane_info
  USING btree
  (lane_id);

CREATE INDEX temp_region_layerx_link_with_regulation_status_linkid_idx
  ON temp_region_layerx_link_with_regulation_status
  USING btree
  (link_id);

CREATE INDEX temp_merge_link_lane_info_unnest_link_id_idx
  ON temp_merge_link_lane_info_unnest
  USING btree
  (link_id);



CREATE INDEX rdb_region_layer8_node_mapping_node_id_14_idx
  ON rdb_region_layer8_node_mapping
  USING btree
  (node_id_14);

CREATE INDEX rdb_region_layer6_node_mapping_node_id_14_idx
  ON rdb_region_layer6_node_mapping
  USING btree
  (node_id_14);

CREATE INDEX rdb_region_layer4_node_mapping_node_id_14_idx
  ON rdb_region_layer4_node_mapping
  USING btree
  (node_id_14);

  

CREATE INDEX temp_optimize_shortlink_link_id_idx
  ON temp_optimize_shortlink
  USING btree
  (link_id);

CREATE INDEX temp_optimize_shortlink_start_node_id_idx
  ON temp_optimize_shortlink
  USING btree
  (start_node_id);

CREATE INDEX temp_optimize_shortlink_end_node_id_idx
  ON temp_optimize_shortlink
  USING btree
  (end_node_id);

create index temp_optimize_try_all_node_num_idx
	on temp_optimize_try_all
	using btree
	(node_num);
create index temp_optimize_try_all_try_index_idx
	on temp_optimize_try_all
	using btree
	(try_index);

CREATE INDEX temp_optimize_illegal_shortlink_link_id_idx
  ON temp_optimize_illegal_shortlink
  USING btree
  (link_id);

CREATE INDEX temp_optimize_node_group_group_id_idx
  ON temp_optimize_node_group
  USING btree
  (group_id);

CREATE INDEX temp_optimize_related_nodes_node_id_idx
  ON temp_optimize_related_nodes
  USING btree
  (node_id);

CREATE INDEX temp_optimize_node_new_position_node_id_idx
  ON temp_optimize_node_new_position
  USING btree
  (node_id);

CREATE INDEX rdb_link_the_geom_4096_idx
  ON rdb_link
  USING gist
  (the_geom_4096);

CREATE INDEX rdb_node_the_geom_4096_idx
  ON rdb_node
  USING gist
  (the_geom_4096);
  
CREATE INDEX temp_node_connect_link_num_node_id_idx
	ON temp_node_connect_link_num
	USING btree
	(node_id);
	
CREATE INDEX temp_node_connect_link_num_num_idx
	ON temp_node_connect_link_num
	USING btree
	(num);
	
CREATE INDEX temp_org_add_region_link_id_idx
	on temp_org_add_region
	using btree
	(link_id);

CREATE INDEX temp_org_add_region_start_node_id_idx
	on temp_org_add_region
	using btree
	(start_node_id);

CREATE INDEX temp_org_add_region_end_node_id_idx
	on temp_org_add_region
	using btree
	(end_node_id);

CREATE INDEX temp_guidence_node_node_polygon_idx
	on temp_guidence_node
	using gist
	(polygon);

CREATE INDEX temp_asso_node_logmark_node_id_idx
	on temp_asso_node_logmark
	using btree
	(node_id);
	
	CREATE INDEX temp_rdb_name_name_id_idx
  ON temp_rdb_name
  USING btree
  (name_id);
  CREATE INDEX temp_rdb_name_link_id_idx
  ON temp_rdb_name
  USING btree
  (link_id);
  
-- forecast data.
CREATE INDEX org_hsnp_network_id_idx1
  ON org_hsnp
  USING btree
  (network_id);
CREATE INDEX org_hspr_forecast_id_idx
  ON org_hspr
  USING btree
  (profile_id);
CREATE INDEX org_hspr_time_slot_idx
  ON org_hspr
  USING btree
  (time_slot);  

CREATE INDEX temp_forecast_link_link_id_idx
  ON temp_forecast_link
  USING btree
  (link_id);
CREATE INDEX temp_forecast_link_profile_flag_idx
  ON temp_forecast_link
  USING btree
  (profile_flag);
  
CREATE INDEX temp_link_with_length_org_link_id_idx
  ON temp_link_with_length
  USING btree
  (org_link_id);
CREATE INDEX temp_link_with_length_rdb_link_id_idx
  ON temp_link_with_length
  USING btree
  (rdb_link_id);
CREATE INDEX temp_link_with_length_s_fraction_idx
  ON temp_link_with_length
  USING btree
  (s_fraction);
CREATE INDEX temp_link_with_length_e_fraction_idx
  ON temp_link_with_length
  USING btree
  (e_fraction);  

CREATE INDEX temp_forecast_link_rdb_id_link_id_idx
  ON temp_forecast_link_rdb_id
  USING btree
  (link_id);
CREATE INDEX temp_forecast_link_rdb_id_dir_idx
  ON temp_forecast_link_rdb_id
  USING btree
  (dir);  
  
CREATE INDEX temp_forecast_time_org_forecast_id_idx1
  ON temp_forecast_time_org
  USING btree
  (profile_id);
CREATE INDEX temp_forecast_time_org_time_slot_idx1
  ON temp_forecast_time_org
  USING btree
  (time_slot);

CREATE INDEX temp_forecast_link_with_slot_link_id_dir_idx1
  ON temp_forecast_link_with_slot
  USING btree
  (link_id,dir);
CREATE INDEX temp_forecast_link_with_slot_profile_1_idx1
  ON temp_forecast_link_with_slot
  USING btree
  (profile_1);
CREATE INDEX temp_forecast_link_with_slot_profile_2_idx1
  ON temp_forecast_link_with_slot
  USING btree
  (profile_2);
CREATE INDEX temp_forecast_link_with_slot_profile_3_idx1
  ON temp_forecast_link_with_slot
  USING btree
  (profile_3);
CREATE INDEX temp_forecast_link_with_slot_profile_4_idx1
  ON temp_forecast_link_with_slot
  USING btree
  (profile_4);
CREATE INDEX temp_forecast_link_with_slot_profile_5_idx1
  ON temp_forecast_link_with_slot
  USING btree
  (profile_5);
CREATE INDEX temp_forecast_link_with_slot_profile_6_idx1
  ON temp_forecast_link_with_slot
  USING btree
  (profile_6);
CREATE INDEX temp_forecast_link_with_slot_profile_7_idx1
  ON temp_forecast_link_with_slot
  USING btree
  (profile_7);
  
CREATE INDEX temp_forecast_time_slot_time_slot_idx1
  ON temp_forecast_time_slot
  USING btree
  (time_slot);
  
CREATE INDEX temp_forecast_link_with_slot_main_link_id_dir_time_slot_idx1
  ON temp_forecast_link_with_slot_main
  USING btree
  (link_id,dir,time_slot);
  
CREATE INDEX temp_forecast_link_with_slot_merge_link_id_dir_idx
  ON temp_forecast_link_with_slot_merge
  USING btree
  (link_id, dir);
  
CREATE INDEX temp_forecast_control_link_id_dir_idx
  ON temp_forecast_control
  USING btree
  (link_id, dir);