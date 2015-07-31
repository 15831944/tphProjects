-----------------------------------------------------------------------------------------
-- script for iPC RDB (or jdb)
-----------------------------------------------------------------------------------------

CREATE INDEX road_tollnode_node_id_idx
  ON road_tollnode
  USING btree
  (node_id);
  
CREATE INDEX road_hwynode_node_id_idx
  ON road_hwynode
  USING btree
  (node_id);
  
CREATE INDEX road_dicin_road_code_seq_num_idx
  ON road_dicin
  USING btree
  (road_code, seq_num);
  
--road_rnd
CREATE INDEX road_node_4326_objectid_idx
  ON road_node_4326
  USING btree
  (objectid);

  --road_rlk
CREATE INDEX road_link_4326_objectid_idx
  ON road_link_4326
  USING btree
  (objectid);

CREATE INDEX road_link_4326_from_node_id_idx
  ON road_link_4326
  USING btree
  (from_node_id);
  
CREATE INDEX road_link_4326_to_node_id_idx
  ON road_link_4326
  USING btree
  (to_node_id);
  
CREATE INDEX middle_tile_node_tile_node_id_idx
  ON middle_tile_node
  USING btree
  (tile_node_id);

CREATE UNIQUE INDEX middle_tile_node_old_node_id_idx
  ON middle_tile_node
  USING btree
  (old_node_id);  

CREATE INDEX middle_tile_link_tile_link_id_idx
  ON middle_tile_link
  USING btree
  (tile_link_id);

CREATE INDEX middle_tile_link_old_link_id_idx
  ON middle_tile_link
  USING btree
  (old_link_id);
  
CREATE UNIQUE INDEX middle_tile_link_old_link_id_old_s_e_node_idx
  ON middle_tile_link
  USING btree
  (old_link_id, old_s_e_node);

CREATE UNIQUE INDEX middle_tile_link_old_link_id_split_seq_num_idx
  ON middle_tile_link
  USING btree
  (old_link_id, split_seq_num);  

-- temp_road_node
CREATE UNIQUE INDEX temp_road_node_objectid_idx
  ON temp_road_node
  USING btree
  (objectid);
  
-- temp_road_link
CREATE INDEX temp_road_link_objectid_idx
  ON temp_road_link
  USING btree
  (objectid);
  
CREATE UNIQUE INDEX temp_road_link_objectid_split_seq_num_idx
  ON temp_road_link
  USING btree
  (objectid, split_seq_num);
  
CREATE INDEX temp_road_link_from_node_id_idx
  ON temp_road_link
  USING btree
  (from_node_id);

CREATE INDEX temp_road_link_to_node_id_idx
  ON temp_road_link
  USING btree
  (to_node_id);

-----------------------------------------------------------------------------------------
-- for highway node
CREATE INDEX highway_node_add_nodeid_node_id_idx
  ON highway_node_add_nodeid
  USING btree
  (node_id);
-----------------------------------------------------------------------------------------
-- for highway node
CREATE INDEX highway_node_add_nodeid_the_geom_idx
  ON highway_node_add_nodeid
  USING gist
  (the_geom);
-----------------------------------------------------------------------------------------
 CREATE INDEX mid_temp_facility_full_path_node_id_idx
  ON mid_temp_facility_full_path
  USING btree
  (node_id);
  
-----------------------------------------------------------------------------------------
CREATE INDEX mid_fac_name_node_id_idx
  ON mid_fac_name
  USING btree
  (node_id);

-----------------------------------------------------------------------------------------
CREATE INDEX highway_node_add_nodeid_midid_id_idx
  ON highway_node_add_nodeid_midid
  USING btree
  (node_id);
  
-----------------------------------------------------------------------------------------
CREATE INDEX highway_node_add_nodeid_midid_name_idx
  ON highway_node_add_nodeid_midid
  USING btree
  (name_kanji);
-------------------------------------------------------------------------------------------------
-- admin
-------------------------------------------------------------------------------------------------
CREATE INDEX base_admin_4326_citycode_idx
  ON base_admin_4326
  USING btree
  (citycode);
  
CREATE INDEX base_admin_4326_geom_idx
  ON base_admin_4326
  USING gist
  (the_geom);
  
CREATE INDEX citycode_citycode_idx
  ON citycode
  USING btree
  (citycode);
  
CREATE INDEX citycode_prefname_idx
  ON citycode
  USING btree
  (prefname);
-------------------------------------------------------------------------------------------------
-- signpost TowardName
-------------------------------------------------------------------------------------------------
CREATE INDEX inf_dirguide_id_idx
  ON inf_dirguide
  USING btree
  (objectid);
CREATE INDEX lq_dirguide_inf_id_idx
  ON lq_dirguide
  USING btree
  (inf_id);
CREATE INDEX temp_inf_dirguide_id_idx
  ON temp_inf_dirguide
  USING btree
  (objectid);
CREATE INDEX temp_lq_dirguide_inf_id_idx
  ON temp_lq_dirguide
  USING btree
  (inf_id);
CREATE INDEX temp_road_dir_id_idx
  ON temp_road_dir
  USING btree
  (objectid);
CREATE INDEX temp_boundary_the_geom_idx
  ON temp_boundary
  USING gist
  (the_geom);
  
CREATE INDEX temp_hlink_the_geom_idx
  ON temp_hlink
  USING gist
  (the_geom);
CREATE INDEX highway_node_4326_the_geom_idx
  ON highway_node_4326
  USING gist
  (the_geom);
CREATE INDEX road_node_4326_the_geom_idx
  ON road_node_4326
  USING gist
  (the_geom);

-------------------------------------------------------------------------
CREATE UNIQUE INDEX temp_language_tbl_l_talbe_idx
  ON temp_language_tbl
  USING btree
  (l_talbe);

-------------------------------------------------------------------------
CREATE UNIQUE INDEX temp_language_tbl_language_code_idx
  ON temp_language_tbl
  USING btree
  (language_code);
  
-------------------------------------------------------------------------
CREATE INDEX road_node_4326_the_geom
   ON road_node_4326 
   USING gist (the_geom);
   
-------------------------------------------------------------------------
CREATE INDEX highway_node_4326_the_geom_idx
  ON highway_node_4326
  USING gist
  (the_geom);
  
CREATE INDEX mid_road_hwynode_middle_nodeid_node_id_idx
  ON mid_road_hwynode_middle_nodeid
  USING btree
  (node_id);
  

------------------------------------------------------------------------
CREATE UNIQUE INDEX mid_hwy_main_link_path_link_id_idx
  ON mid_hwy_main_link_path
  USING btree
  (link_id);
  
CREATE INDEX mid_hwy_main_link_path_node_id_idx
  ON mid_hwy_main_link_path
  USING btree
  (node_id);
------------------------------------------------------------------------
CREATE INDEX mid_hwy_ic_no_road_code_road_point_idx
  ON mid_hwy_ic_no
  USING btree
  (road_code, road_point);
 
CREATE UNIQUE INDEX mid_hwy_ic_no_node_id_road_code_road_point_updown_idx
  ON mid_hwy_ic_no
  USING btree
  (node_id, road_code, road_point, updown);
------------------------------------------------------------------------
CREATE INDEX mid_hwy_facility_id_road_code_road_point_idx
  ON mid_hwy_facility_id
  USING btree
  (road_code, road_point);
 
------------------------------------------------------------------------
CREATE UNIQUE INDEX mid_hwy_road_no_roadcode_idx
  ON mid_hwy_road_no
  USING btree
  (roadcode);

------------------------------------------------------------------------
CREATE INDEX mid_hwy_ic_path_s_road_code_s_road_point_s_updown_s_node_id_idx
  ON mid_hwy_ic_path
  USING btree
  (s_road_code, s_road_point, s_updown, s_node_id, s_facilcls, s_inout_c);
  
------------------------------------------------------------------------
CREATE INDEX mid_hwy_ic_path_t_road_code_t_road_point_t_updown_t_node_id_idx
  ON mid_hwy_ic_path
  USING btree
  (t_road_code, t_road_point, t_updown, t_node_id, t_facilcls, t_inout_c);

------------------------------------------------------------------------
CREATE INDEX mid_access_point_middle_nodei_road_code_road_seq_direction__idx
  ON mid_access_point_middle_nodeid
  USING btree
  (road_code, road_seq, direction_c);
  (the_geom);

------------------------------------------------------------------------
CREATE INDEX highway_node_4326_the_geom_idx
  ON highway_node_4326
  USING gist
  (the_geom);

------------------------------------------------------------------------
CREATE INDEX road_node_4326_the_geom_idx
  ON road_node_4326
  USING gist
  (the_geom);

------------------------------------------------------------------------ 
CREATE INDEX road_link_4326_road_code_idx
  ON road_link_4326
  USING btree
  (road_code);

------------------------------------------------------------------------
CREATE INDEX road_code_list_road_code_idx
  ON road_code_list
  USING btree
  (road_code);
  
------------------------------------------------------------------------
CREATE INDEX mid_temp_full_path_nodeid_idx
  ON mid_temp_full_path
  USING btree
  (node_id);
  
CREATE INDEX mid_fac_name_temp_nodeid_idx
  ON mid_fac_name_temp
  USING btree
  (node_id);
  
CREATE INDEX mid_fac_name_temp_shortpath_nodeid_idx
  ON mid_fac_name_temp_shortpath
  USING btree
  (node_id);
  
CREATE INDEX highway_node_add_nodeid_midid_nodeid_idx
  ON highway_node_add_nodeid_midid
  USING btree
  (node_id);

CREATE INDEX temp_node_num_for_towardname_node_id_idx
	ON temp_node_num_for_towardname
	USING btree
	(node_id);

------------------------------------------------------------------------
CREATE UNIQUE INDEX mid_hwy_facility_name_for_tow_nodeid_inlinkid_outlinkid_pas_idx
  ON mid_hwy_facility_name_for_towardname
  USING btree
  (nodeid, inlinkid, outlinkid, passlid);