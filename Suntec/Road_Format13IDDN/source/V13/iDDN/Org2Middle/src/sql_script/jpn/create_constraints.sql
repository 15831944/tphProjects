--road_rnd
CREATE INDEX road_rnd_objectid_idx
  ON road_rnd
  USING btree
  (objectid);

--road_rlk
CREATE UNIQUE INDEX road_rlk_objectid_idx
  ON road_rlk
  USING btree
  (objectid);

CREATE INDEX road_rlk_fromnodeid_idx
  ON road_rlk
  USING btree
  (fromnodeid);
  
CREATE INDEX road_rlk_tonodeid_idx
  ON road_rlk
  USING btree
  (tonodeid);

CREATE INDEX temp_node_lane_nodeid_idx
  ON temp_node_lane
  USING btree
  (nodeid);

CREATE INDEX temp_node_signpost_nodeid_idx
  ON temp_node_signpost
  USING btree
  (nodeid);
  
-- temp_road_rnd
CREATE UNIQUE INDEX temp_road_rnd_objectid_idx
  ON temp_road_rnd
  USING btree
  (objectid);
  
-- temp_road_rlk
CREATE INDEX temp_road_rlk_objectid_idx
  ON temp_road_rlk
  USING btree
  (objectid);
  
CREATE UNIQUE INDEX temp_road_rlk_objectid_split_seq_num_idx
  ON temp_road_rlk
  USING btree
  (objectid, split_seq_num);
  
CREATE INDEX temp_road_rlk_fromnodeid_idx
  ON temp_road_rlk
  USING btree
  (fromnodeid);

CREATE INDEX temp_road_rlk_tonodeid_idx
  ON temp_road_rlk
  USING btree
  (tonodeid);
  
CREATE INDEX temp_node_signpost_name_gid_idx
  ON temp_node_signpost_name
  USING btree
  (gid);

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
  
CREATE INDEX rdb_admin_province_the_geom_idx
  ON rdb_admin_province
  USING gist
  (the_geom);
  
CREATE INDEX temp_boundary_the_geom_idx
  ON temp_boundary
  USING gist
  (the_geom);
  
CREATE INDEX temp_hlink_the_geom_idx
  ON temp_hlink
  USING gist
  (the_geom);
  
CREATE INDEX name_dictionary_tbl_name_id_idx
  ON name_dictionary_tbl
  USING btree
  (name_id);
------------------------------------------------------------------------
-- Facility name
------------------------------------------------------------------------
CREATE INDEX mid_fac_name_full_path_in_link_id_node_id_fac_type_idx
  ON mid_fac_name_full_path
  USING btree
  (in_link_id, node_id, fac_type);
  
CREATE INDEX mid_fac_name_node_id_idx
  ON mid_fac_name
  USING btree
  (nodeid);
  
------------------------------------------------------------------------ 
CREATE INDEX mid_road_roadpoint_node_the_geom_idx
  ON mid_road_roadpoint_node
  USING gist
  (the_geom);
  
CREATE INDEX mid_road_roadpoint_node_node_id_idx
  ON mid_road_roadpoint_node
  USING btree
  (node_id);

CREATE INDEX mid_road_roadpoint_bunnki_node_the_geom_idx
  ON mid_road_roadpoint_node
  USING gist
  (the_geom);
  
CREATE INDEX mid_road_roadpoint_bunnki_node_node_id_idx
  ON mid_road_roadpoint_node
  USING btree
  (node_id);
  
CREATE INDEX road_rnd_the_geom_idx
  ON road_rnd
  USING gist
  (the_geom);

