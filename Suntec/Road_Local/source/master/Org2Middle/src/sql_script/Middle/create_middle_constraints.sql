
-- link_tbl
CREATE UNIQUE INDEX link_tbl_link_id_idx
  ON link_tbl
  USING btree
  (link_id);
  
CREATE INDEX link_tbl_s_node_idx
  ON link_tbl
  USING btree
  (s_node);
  
CREATE INDEX link_tbl_e_node_idx
  ON link_tbl
  USING btree
  (e_node);

CREATE INDEX link_tbl_the_geom_idx
  ON link_tbl
  USING gist
  (the_geom);

CREATE INDEX link_tbl_s_node_e_node_idx
  ON link_tbl
  USING btree
  (s_node, e_node);
  
-- node_tbl
CREATE UNIQUE INDEX node_tbl_node_id_idx
  ON node_tbl
  USING btree
  (node_id);

CREATE INDEX node_tbl_the_geom_idx
  ON node_tbl
  USING gist
  (the_geom);

-- node_height_tbl
CREATE UNIQUE INDEX node_height_tbl_node_id_idx
  ON node_height_tbl
  USING btree
  (node_id);
  
-- lane_tbl
CREATE INDEX lane_tbl_node_id_idx
  ON lane_tbl
  USING btree
  (nodeid);
  
CREATE INDEX lane_tbl_inlinkid_idx
  ON lane_tbl
  USING btree
  (inlinkid);
  
CREATE INDEX lane_tbl_outlinkid_idx
  ON lane_tbl
  USING btree
  (outlinkid);
  
-- force_guide_tbl
CREATE INDEX force_guide_tbl_node_id_idx
  ON force_guide_tbl
  USING btree
  (nodeid);
  
-- signpost_tbl
CREATE INDEX signpost_tbl_node_id_idx
  ON signpost_tbl
  USING btree
  (nodeid);

--------------------------------------------------
-- signpost_uc
CREATE INDEX signpost_uc_tbl_node_id_idx
  ON signpost_uc_tbl
  USING btree
  (nodeid);

--------------------------------------------------
CREATE UNIQUE INDEX signpost_uc_tbl_inlinkid_nodeid_outlinkid_unique_idx
  ON signpost_uc_tbl
  USING btree
  (inlinkid, nodeid, outlinkid);

--------------------------------------------------
CREATE INDEX signpost_uc_tbl_inlinkid_nodeid_outlinkid_idx
  ON signpost_uc_tbl
  USING btree
  (inlinkid, nodeid, outlinkid);

--------------------------------------------------
-- signpost_uc
CREATE UNIQUE INDEX signpost_uc_tbl_nodeid_inlinkid_outlinkid_idx
  ON signpost_uc_tbl
  USING btree
  (nodeid, inlinkid, outlinkid);
  
-- spotguide_tbl
CREATE INDEX spotguide_tbl_node_id_idx
  ON spotguide_tbl
  USING btree
  (nodeid);

-- towardname_tbl
CREATE INDEX towardname_tbl_node_id_idx
  ON towardname_tbl
  USING btree
  (nodeid);
  
-- name_dictionary_tbl  
CREATE INDEX name_dictionary_tbl_name_id_idx
  ON name_dictionary_tbl
  USING btree
  (name_id);
  
CREATE INDEX vics_link_tbl_linkid_dir_idx
  ON vics_link_tbl
  USING BTREE
  (link_id, dir);

CREATE INDEX vics_ipc_link_tbl_linkid_dir_idx
  ON vics_ipc_link_tbl
  USING BTREE
  (link_id, dir);
  
CREATE INDEX temp_condition_direction_travel_link_id_idx
  ON temp_condition_direction_travel
  USING btree
  (link_id);

CREATE INDEX temp_node_tbl_expand_nodeid_idx
  ON temp_node_tbl_expand
  USING btree
  (node_id);


CREATE INDEX temp_merge_node_keep_node_id_idx
	on temp_merge_node_keep
	using btree
	(node_id);

CREATE INDEX temp_merge_link_keep_link_id_idx
	on temp_merge_link_keep
	using btree
	(link_id);

CREATE INDEX temp_merge_node_suspect_node_id_idx
	on temp_merge_node_suspect
	using btree
	(node_id);

CREATE INDEX temp_merge_link_suspect_s_node_idx
	on temp_merge_link_suspect
	using btree
	(s_node);

CREATE INDEX temp_merge_link_suspect_e_node_idx
	on temp_merge_link_suspect
	using btree
	(e_node);

CREATE INDEX temp_merge_link_suspect_link_id_idx
	on temp_merge_link_suspect
	using btree
	(link_id);
	
CREATE INDEX temp_merge_link_suspect_tile_id_length_idx
	on temp_merge_link_suspect
	using btree
	(tile_id,length);
	
CREATE INDEX temp_merge_linkrow_s_node_idx
	on temp_merge_linkrow
	using btree
	(s_node);

CREATE INDEX temp_merge_linkrow_e_node_idx
	on temp_merge_linkrow
	using btree
	(e_node);

CREATE INDEX temp_merge_linkrow_s_node_e_node_idx
	on temp_merge_linkrow
	using btree
	(s_node, e_node);

CREATE INDEX temp_merge_link_mapping_link_id_idx
	on temp_merge_link_mapping
	using btree
	(link_id);

CREATE INDEX temp_merge_link_mapping_merge_link_id_idx
	on temp_merge_link_mapping
	using btree
	(merge_link_id);

CREATE INDEX temp_merge_newlink_maybe_circle_link_id_idx
	on temp_merge_newlink_maybe_circle
	using btree
	(link_id);

CREATE INDEX temp_merge_newlink_maybe_circle_s_node_idx
	on temp_merge_newlink_maybe_circle
	using btree
	(s_node);

CREATE INDEX temp_merge_newlink_link_id_idx
	on temp_merge_newlink
	using btree
	(link_id);

CREATE INDEX temp_merge_newnode_node_id_idx
	on temp_merge_newnode
	using btree
	(node_id);

CREATE INDEX link_tbl_bak_merge_gid_idx
  ON link_tbl_bak_merge
  USING btree
  (gid);

CREATE INDEX link_tbl_bak_merge_link_id_idx
  ON link_tbl_bak_merge
  USING btree
  (link_id);

CREATE INDEX node_tbl_bak_merge_gid_idx
  ON node_tbl_bak_merge
  USING btree
  (gid);

CREATE INDEX node_tbl_bak_merge_node_id_idx
  ON node_tbl_bak_merge
  USING btree
  (node_id);

CREATE UNIQUE INDEX mid_admin_zone_ad_code_idx
  ON mid_admin_zone
  USING btree
  (ad_code);

CREATE INDEX mid_admin_zone_ad_order_idx
  ON mid_admin_zone
  USING btree
  (ad_order);

CREATE INDEX mid_admin_zone_order0_id_idx
  ON mid_admin_zone
  USING btree
  (order0_id);

CREATE INDEX mid_admin_zone_order1_id_idx
  ON mid_admin_zone
  USING btree
  (order1_id);

CREATE INDEX mid_admin_zone_order2_id_idx
  ON mid_admin_zone
  USING btree
  (order2_id);

CREATE INDEX mid_admin_zone_order8_id_idx
  ON mid_admin_zone
  USING btree
  (order8_id);

CREATE INDEX mid_admin_zone_the_geom_idx
  ON mid_admin_zone
  USING gist
  (the_geom);

CREATE INDEX mid_logmark_the_geom_idx
  ON mid_logmark
  USING gist
  (the_geom);

CREATE INDEX temp_split_link_envelope_tile_link_id_idx
  ON temp_split_link_envelope_tile
  USING btree
  (link_id);

CREATE INDEX temp_split_link_envelope_tile_tile_id_idx
  ON temp_split_link_envelope_tile
  USING btree
  (tile_id);

CREATE INDEX temp_split_link_span_tile_link_id_idx
  ON temp_split_link_span_tile
  USING btree
  (link_id);

CREATE INDEX temp_split_link_span_tile_tile_id_idx
  ON temp_split_link_span_tile
  USING btree
  (tile_id);

CREATE INDEX temp_split_tile_tile_id_idx
  ON temp_split_tile
  USING btree
  (tile_id);

CREATE INDEX temp_split_tile_the_geom_idx
  ON temp_split_tile
  USING gist
  (the_geom);

CREATE INDEX temp_split_tile_dummy_border_geom_idx
  ON temp_split_tile
  USING gist
  (dummy_border_geom);

CREATE INDEX temp_split_sublink_phase1_tile_id_idx
  ON temp_split_sublink_phase1
  USING btree
  (tile_id);

CREATE INDEX temp_split_sublink_phase1_link_id_idx
  ON temp_split_sublink_phase1
  USING btree
  (link_id);

CREATE INDEX temp_split_sublink_phase1_the_geom_idx
  ON temp_split_sublink_phase1
  USING gist
  (the_geom);

CREATE INDEX temp_split_sublink_phase2_gid_phase1_idx
  ON temp_split_sublink_phase2
  USING btree
  (gid_phase1);

CREATE INDEX temp_split_sublink_phase2_link_id_idx
  ON temp_split_sublink_phase2
  USING btree
  (link_id);

CREATE INDEX temp_split_sublink_phase2_the_geom_idx
  ON temp_split_sublink_phase2
  USING gist
  (the_geom);

CREATE INDEX temp_split_sublink_phase3_link_id_idx
  ON temp_split_sublink_phase3
  USING btree
  (link_id);

CREATE INDEX temp_split_sublink_phase3_the_geom_idx
  ON temp_split_sublink_phase3
  USING gist
  (the_geom);

CREATE INDEX temp_split_sublink_phase4_gid_phase3_idx
  ON temp_split_sublink_phase4
  USING btree
  (gid_phase3);

CREATE INDEX temp_split_sublink_phase4_link_id_idx
  ON temp_split_sublink_phase4
  USING btree
  (link_id);

CREATE INDEX temp_split_sublink_phase4_the_geom_idx
  ON temp_split_sublink_phase4
  USING gist
  (the_geom);

CREATE INDEX temp_split_sublink_phase5_link_id_idx
  ON temp_split_sublink_phase5
  USING btree
  (link_id);

CREATE INDEX temp_split_sublink_phase5_the_geom_idx
  ON temp_split_sublink_phase5
  USING gist
  (the_geom);

CREATE INDEX temp_split_sublink_phase6_link_id_idx
  ON temp_split_sublink_phase6
  USING btree
  (link_id);

CREATE INDEX temp_split_sublink_phase6_the_geom_idx
  ON temp_split_sublink_phase6
  USING gist
  (the_geom);

CREATE INDEX temp_split_short_sublink_link_id_idx
  ON temp_split_short_sublink
  USING btree
  (link_id);

CREATE INDEX temp_split_short_sublink_s_node_idx
  ON temp_split_short_sublink
  USING btree
  (s_node);

CREATE INDEX temp_split_short_sublink_e_node_idx
  ON temp_split_short_sublink
  USING btree
  (e_node);

CREATE INDEX temp_split_keep_sublink_link_id_idx
  ON temp_split_keep_sublink
  USING btree
  (link_id);

CREATE INDEX temp_split_short_node_degree_node_id_idx
  ON temp_split_short_node_degree
  USING btree
  (node_id);

CREATE INDEX temp_split_delete_sublink_link_id_idx
  ON temp_split_delete_sublink
  USING btree
  (link_id);

CREATE INDEX temp_split_delete_sublink_environment_link_id_idx
  ON temp_split_delete_sublink_environment
  USING btree
  (link_id);

CREATE INDEX temp_split_delete_sublink_point_link_id_idx
  ON temp_split_delete_sublink_point
  USING btree
  (link_id);

CREATE INDEX temp_split_move_node_node_id_idx
  ON temp_split_move_node
  USING btree
  (node_id);

CREATE INDEX temp_split_move_link_link_id_idx
  ON temp_split_move_link
  USING btree
  (link_id);

CREATE INDEX temp_split_move_sublink_link_id_idx
  ON temp_split_move_sublink
  USING btree
  (link_id);


CREATE INDEX temp_split_newnode_node_id_idx
  ON temp_split_newnode
  USING btree
  (node_id);

CREATE INDEX temp_split_newnode_old_link_id_idx
  ON temp_split_newnode
  USING btree
  (old_link_id);

CREATE INDEX temp_split_newnode_sub_index_idx
  ON temp_split_newnode
  USING btree
  (sub_index);

CREATE INDEX temp_split_newnode_the_geom_idx
  ON temp_split_newnode
  USING gist
  (the_geom);

CREATE INDEX temp_split_newlink_link_id_idx
  ON temp_split_newlink
  USING btree
  (link_id);

CREATE INDEX temp_split_newlink_s_node_idx
  ON temp_split_newlink
  USING btree
  (s_node);

CREATE INDEX temp_split_newlink_e_node_idx
  ON temp_split_newlink
  USING btree
  (e_node);

CREATE INDEX temp_split_newlink_old_link_id_idx
  ON temp_split_newlink
  USING btree
  (old_link_id);

CREATE INDEX temp_split_newlink_sub_index_idx
  ON temp_split_newlink
  USING btree
  (sub_index);

CREATE INDEX temp_split_newlink_the_geom_idx
  ON temp_split_newlink
  USING gist
  (the_geom);


CREATE INDEX regulation_relation_tbl_gid_idx
  ON regulation_relation_tbl
  USING btree
  (gid);

CREATE INDEX regulation_relation_tbl_regulation_id_idx
  ON regulation_relation_tbl
  USING btree
  (regulation_id);

CREATE INDEX regulation_relation_tbl_inlinkid_idx
  ON regulation_relation_tbl
  USING btree
  (inlinkid);

CREATE INDEX regulation_relation_tbl_outlinkid_idx
  ON regulation_relation_tbl
  USING btree
  (outlinkid);
  
CREATE INDEX regulation_item_tbl_regulation_id
  ON regulation_item_tbl
  USING btree
  (regulation_id);
  
CREATE INDEX condition_regulation_tbl_cond_id
  ON condition_regulation_tbl
  USING btree
  (cond_id);

CREATE INDEX regulation_relation_tbl_bak_splitting_regulation_id_idx
  ON regulation_relation_tbl_bak_splitting
  USING btree
  (regulation_id);

CREATE INDEX regulation_item_tbl_bak_splitting_regulation_id_idx
  ON regulation_item_tbl_bak_splitting
  USING btree
  (regulation_id);

CREATE INDEX regulation_item_tbl_bak_splitting_linkid_idx
  ON regulation_item_tbl_bak_splitting
  USING btree
  (linkid);

CREATE INDEX temp_split_update_linkrow_regulation_regulation_id_idx
  ON temp_split_update_linkrow_regulation
  USING btree
  (regulation_id);

CREATE INDEX link_tbl_bak_splitting_gid_idx
  ON link_tbl_bak_splitting
  USING btree
  (gid);

CREATE INDEX link_tbl_bak_splitting_link_id_idx
  ON link_tbl_bak_splitting
  USING btree
  (link_id);

CREATE INDEX link_tbl_bak_splitting_s_node_idx
  ON link_tbl_bak_splitting
  USING btree
  (s_node);

CREATE INDEX link_tbl_bak_splitting_e_node_idx
  ON link_tbl_bak_splitting
  USING btree
  (e_node);

CREATE INDEX node_tbl_bak_splitting_gid_idx
  ON node_tbl_bak_splitting
  USING btree
  (gid);

CREATE INDEX node_tbl_bak_splitting_node_id_idx
  ON node_tbl_bak_splitting
  USING btree
  (node_id);

CREATE INDEX temp_move_shortlink_link_id_idx
  ON temp_move_shortlink
  USING btree
  (link_id);

CREATE INDEX temp_move_shortlink_newshape_link_id_idx
  ON temp_move_shortlink_newshape
  USING btree
  (link_id);

CREATE INDEX temp_move_shortlink_vertex_newpos_node_id_idx
  ON temp_move_shortlink_vertex_newpos
  USING btree
  (node_id);

CREATE INDEX link_tbl_bak_moving_link_id_idx
  ON link_tbl_bak_moving
  USING btree
  (link_id);

CREATE INDEX node_tbl_bak_moving_node_id_idx
  ON node_tbl_bak_moving
  USING btree
  (node_id);

CREATE INDEX link_tbl_bak_dupli_dealing_link_id_idx
  ON link_tbl_bak_dupli_dealing
  USING btree
  (link_id);

CREATE INDEX temp_dupli_link_aid_idx
  ON temp_dupli_link
  USING btree
  (aid);

CREATE INDEX dupli_link_gid_idx
  ON dupli_link
  USING btree
  (gid);

CREATE INDEX dupli_link_link_id_idx
  ON dupli_link
  USING btree
  (link_id);

CREATE INDEX dupli_link_the_geom_idx
  ON dupli_link
  USING gist
  (the_geom);
  
CREATE INDEX dupli_link_s_node_e_node_idx
  ON dupli_link
  USING btree
  (s_node, e_node);
  
CREATE INDEX temp_circle_link_ageom_idx
  ON temp_circle_link
  USING gist
  (ageom);
  
CREATE INDEX temp_circle_link_aid_idx
  ON temp_circle_link
  USING btree
  (aid);
 
CREATE INDEX regulation_relation_tbl_bak_circle_dealing_regulation_id_idx
  ON regulation_relation_tbl_bak_circle_dealing
  USING btree
  (regulation_id); 

CREATE INDEX regulation_item_tbl_bak_circle_dealing_linkid_idx
  ON regulation_item_tbl_bak_circle_dealing
  USING btree
  (linkid);

CREATE INDEX regulation_item_tbl_bak_circle_dealing_regulation_id_idx
  ON regulation_item_tbl_bak_circle_dealing
  USING btree
  (regulation_id);
  
CREATE INDEX link_tbl_bak_circle_dealing_link_id_idx
  ON link_tbl_bak_circle_dealing
  USING btree
  (link_id);
  
CREATE INDEX highway_facility_tbl_facility_id_idx
  ON highway_facility_tbl
  USING btree
  (facility_id);
  
CREATE INDEX highway_facility_tbl_node_idx
  ON highway_facility_tbl
  USING btree
  (node_id);
  
CREATE INDEX highway_sa_pa_tbl_sapa_id_idx
  ON highway_sa_pa_tbl
  USING btree
  (sapa_id);
  
  CREATE INDEX highway_sa_pa_tbl_node_id_idx
  ON highway_sa_pa_tbl
  USING btree
  (node_id);


CREATE INDEX temp_mainnode_sublink_sublink_idx
  ON temp_mainnode_sublink
  USING btree
  (sublink);

CREATE INDEX temp_mainnode_subnode_subnode_idx
  ON temp_mainnode_subnode
  USING btree
  (subnode);

CREATE INDEX temp_mainnode_regulation_new_linkrow_regulation_id_idx
  ON temp_mainnode_regulation_new_linkrow
  USING btree
  (regulation_id);


CREATE INDEX temp_mainnode_update_regulation_old_regulation_id_idx
  ON temp_mainnode_update_regulation
  USING btree
  (old_regulation_id);

CREATE INDEX regulation_relation_tbl_bak_mainnode_regulation_id_idx
  ON regulation_relation_tbl_bak_mainnode
  USING btree
  (regulation_id);

CREATE INDEX regulation_item_tbl_bak_mainnode_regulation_id_idx
  ON regulation_item_tbl_bak_mainnode
  USING btree
  (regulation_id);
  
----------------------------------------------------
-- language
CREATE UNIQUE INDEX language_tbl_language_code_idx
  ON language_tbl
  USING btree
  (language_code);
 
----------------------------------------------------
CREATE UNIQUE INDEX language_tbl_l_full_name_idx
  ON language_tbl
  USING btree
  (l_full_name);

----------------------------------------------------
CREATE UNIQUE INDEX language_tbl_l_talbe_idx
  ON language_tbl
  USING btree
  (l_talbe);
----park link\node\ploygon
CREATE INDEX park_link_id_idx
  ON park_link_tbl
  USING btree
  (park_link_id);
CREATE INDEX park_node_id_idx
  ON park_node_tbl
  USING btree
  (park_node_id);
CREATE INDEX park_polygon_id_idx
  ON park_polygon_tbl
  USING btree
  (park_region_id);

----------------------------------------------------
CREATE UNIQUE INDEX highway_mapping_link_id_forward_ic_no_backward_ic_no_idx
  ON highway_mapping
  USING btree
  (link_id, forward_ic_no, backward_ic_no);
  
----------------------------------------------------
CREATE INDEX highway_mapping_forward_ic_no_idx
  ON highway_mapping
  USING btree
  (forward_ic_no);

----------------------------------------------------
CREATE INDEX highway_mapping_backward_ic_no_idx
  ON highway_mapping
  USING btree
  (backward_ic_no);

----------------------------------------------------
CREATE INDEX highway_mapping_link_id_idx
  ON highway_mapping
  USING btree
  (link_id);

----------------------------------------------------
CREATE INDEX highway_ic_info_ic_no_idx
  ON highway_ic_info
  USING btree
  (ic_no);
----------------------------------------------------
CREATE UNIQUE INDEX highway_conn_info_ic_no_conn_ic_no_conn_direction_idx
  ON highway_conn_info
  USING btree
  (ic_no, conn_ic_no, conn_direction);

-------------------------------------------------
CREATE UNIQUE INDEX highway_toll_info_ic_no_conn_ic_no_node_id_idx
  ON highway_toll_info
  USING btree
  (ic_no, conn_ic_no, node_id);
-------------------------------------------------
CREATE UNIQUE INDEX park_link_tbl_bak_splitting_park_link_id_idx
  ON park_link_tbl_bak_splitting
  USING btree
  (park_link_id);

CREATE UNIQUE INDEX park_node_tbl_bak_splitting_park_node_id_idx
  ON park_node_tbl_bak_splitting
  USING btree
  (park_node_id);
-------------------------------------------------
CREATE UNIQUE INDEX park_link_tbl_bak_merge_park_link_id_idx
  ON park_link_tbl_bak_merge
  USING btree
  (park_link_id);

CREATE UNIQUE INDEX park_node_tbl_bak_merge_park_node_id_idx
  ON park_node_tbl_bak_merge
  USING btree
  (park_node_id);

-------------------------------------------------
CREATE UNIQUE INDEX highway_node_add_info_node_id_link_id_idx
  ON highway_node_add_info
  USING btree
  (node_id, link_id);
-------------------------------------------------
CREATE UNIQUE INDEX highway_node_add_info_link_id_idx
  ON highway_node_add_info
  USING btree
  (link_id);
-------------------------------------------------
CREATE INDEX toll_from_to_table_ex_code1_code2_idx
  ON toll_from_to_table_ex
  USING btree
  (code1, code2);
  
-------------------------------------------------
CREATE INDEX highway_node_add_info_node_geom_idx
  ON highway_node_add_info
  USING gist
  (node_geom);

-------------------------------------------------
CREATE INDEX highway_node_add_info_link_geom_idx
  ON highway_node_add_info
  USING gist
  (link_geom);

-------------------------------------------------
CREATE INDEX highway_fee_ticket_toll_from_facility_id_to_facility_id_idx
  ON highway_fee_ticket_toll
  USING btree
  (from_facility_id, to_facility_id);

-------------------------------------------------
CREATE UNIQUE INDEX highway_fee_ticket_toll_from_facility_id_to_facility_id_etc_idx
  ON highway_fee_ticket_toll
  USING btree
  (from_facility_id, to_facility_id, etc_discount_c);

-------------------------------------------------
CREATE UNIQUE INDEX highway_fee_same_facility_from_facility_id_to_facility_id_idx
  ON highway_fee_same_facility
  USING btree
  (from_facility_id, to_facility_id);

CREATE INDEX caution_tbl_inlinkid_idx
  ON caution_tbl
  USING btree
  (inlinkid);

CREATE INDEX caution_tbl_nodeid_idx
  ON caution_tbl
  USING btree
  (nodeid);

CREATE INDEX caution_tbl_outlinkid_idx
  ON caution_tbl
  USING btree
  (outlinkid);

-------------------------------------------------
-- For splitting link by tollgates.
CREATE INDEX org_mn_pi_feattyp_idx
  ON org_mn_pi
  USING btree
  (feattyp);
  
CREATE INDEX temp_link_after_split_old_link_id_idx
  ON temp_link_after_split
  USING btree
  (old_link_id);

CREATE INDEX mid_temp_hwy_main_link_the_geom_idx
  ON mid_temp_hwy_main_link
  USING gist
  (the_geom);

CREATE INDEX mid_temp_hwy_inout_link_the_geom_idx
  ON mid_temp_hwy_inout_link
  USING gist
  (the_geom);

CREATE INDEX mid_temp_hwy_ic_link_the_geom_idx
  ON mid_temp_hwy_ic_link
  USING gist
  (the_geom);

CREATE INDEX mid_temp_hwy_main_path_path_id_idx
  ON mid_temp_hwy_main_path
  USING btree
  (path_id);

CREATE UNIQUE INDEX mid_temp_hwy_main_path_link_id_idx
  ON mid_temp_hwy_main_path
  USING btree
  (link_id);

CREATE INDEX hwy_link_road_code_info_the_geom_idx
  ON hwy_link_road_code_info
  USING gist
  (the_geom);

------------------------------------------------------------------------
CREATE INDEX hwy_node_the_geom_idx
  ON hwy_node
  USING gist
  (the_geom);

CREATE UNIQUE INDEX hwy_node_road_code_road_seq_facilcls_c_inout_c_node_id_idx
  ON hwy_node
  USING btree
  (road_code, road_seq, facilcls_c, inout_c, node_id);
 
------------------------------------------------------------------------
CREATE INDEX mid_hwy_facility_id_road_code_road_seq_idx
  ON mid_hwy_facility_id
  USING btree
  (road_code, road_seq);
  
------------------------------------------------------------------------
CREATE INDEX mid_temp_hwy_exit_name_link_id_idx
  ON mid_temp_hwy_exit_name
  USING btree
  (link_id);

------------------------------------------------------------------------
CREATE UNIQUE INDEX mid_temp_hwy_exit_name_link_id_is_exit_name_idx
  ON mid_temp_hwy_exit_name
  USING btree
  (link_id, is_exit_name);

------------------------------------------------------------------------
CREATE UNIQUE INDEX mid_temp_hwy_exit_name_link_id_is_junction_name_idx
  ON mid_temp_hwy_exit_name
  USING btree
  (link_id, is_junction_name);

------------------------------------------------------------------------
CREATE INDEX mid_temp_hwy_facil_name_road_code_road_seq_idx
  ON mid_temp_hwy_facil_name
  USING btree
  (road_code, road_seq);

------------------------------------------------------------------------
CREATE INDEX mid_link_mapping_link_id_idx
  ON mid_link_mapping
  USING btree
  (link_id);

------------------------------------------------------------------------
CREATE INDEX mid_link_mapping_org_link_id_idx
  ON mid_link_mapping
  USING btree
  (org_link_id);

------------------------------------------------------------------------
CREATE INDEX mid_temp_sapa_store_info_poi_id_idx
  ON mid_temp_sapa_store_info
  USING btree
  (poi_id);

------------------------------------------------------------------------
CREATE INDEX mid_temp_hwy_sapa_name_poi_id_idx
  ON mid_temp_hwy_sapa_name_group
  USING btree
  (name_id);

------------------------------------------------------------------------
CREATE INDEX mid_temp_poi_link_poi_id_idx
  ON mid_temp_poi_link
  USING btree
  (poi_id);

CREATE INDEX mid_temp_poi_link_link_id_idx
  ON mid_temp_poi_link
  USING btree
  (link_id);

------------------------------------------------------------------------
CREATE INDEX org_ig_elemid_idx
  ON org_ig
  USING btree
  (elemid);

------------------------------------------------------------------------
CREATE INDEX org_ig_id_idx
  ON org_ig
  USING btree
  (id);

CREATE INDEX org_is_id_idx
  ON org_is
  USING btree
  (id);
------------------------------------------------------------------------
CREATE INDEX hook_turn_tbl_inlinkid_idx
  ON hook_turn_tbl
  USING btree
  (inlinkid);
  
CREATE INDEX hook_turn_tbl_nodeid_idx
  ON hook_turn_tbl
  USING btree
  (nodeid);

--------------------------------------------------------------
CREATE INDEX mid_temp_hwy_sapa_info_road_code_road_seq_idx
  ON mid_temp_hwy_sapa_info
  USING btree
  (road_code, road_seq);

------------------------------------------------------------------------  
CREATE INDEX hwy_same_info_road_code_road_seq_updown_c_node_id_idx
  ON hwy_same_info
  USING btree
  (road_code, road_seq, updown_c, node_id);

------------------------------------------------------------------------
CREATE INDEX hwy_same_info_road_code_p_road_seq_updown_c_p_node_id_idx
  ON hwy_same_info
  USING btree
  (road_code, p_road_seq, updown_c, p_node_id);
  
------------------------------------------------------------------------    
CREATE INDEX stopsign_tbl_link_id_idx
  ON stopsign_tbl
  USING btree
  (link_id);
  
------------------------------------------------------------------------    
CREATE INDEX temp_force_guide_patch_link_tbl_objectid
  ON temp_force_guide_patch_link_tbl
  USING btree
  (objectid);
  
------------------------------------------------------------------------ 
CREATE INDEX towardname_tbl_nodeid_idx
  ON towardname_tbl
  USING btree
  (nodeid);

CREATE INDEX temp_poi_category_org_code_idx
  ON temp_poi_category
  USING btree
  (org_code);

CREATE INDEX temp_brand_icon_brandname_idx
  ON temp_brand_icon
  USING btree
  (brandname);

------------------------------------------------------------------------ 
CREATE INDEX mid_temp_hwy_ic_path_path_type_idx
  ON mid_temp_hwy_ic_path
  USING btree
  (path_type);

------------------------------------------------------------------------ 
CREATE INDEX mid_temp_hwy_ic_path_road_code_road_seq_node_id_inout_c_fac_idx
  ON mid_temp_hwy_ic_path
  USING btree
  (road_code, road_seq, node_id, inout_c, facilcls_c);

------------------------------------------------------------------------ 
CREATE INDEX mid_hwy_ic_no_node_id_idx
  ON mid_hwy_ic_no
  USING btree
  (node_id);

------------------------------------------------------------------------ 
CREATE INDEX mid_hwy_ic_no_node_id_road_code_idx
  ON mid_hwy_ic_no
  USING btree
  (node_id, road_code);

------------------------------------------------------------------------
CREATE INDEX mid_hwy_ic_no_node_id_road_code_road_seq_updown_idx
  ON mid_hwy_ic_no
  USING btree
  (node_id, road_code, road_seq, updown);