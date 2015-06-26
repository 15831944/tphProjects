-- create constraints for rdb
--------------------------------------------------------------------------------------- 
-- create index 
--------------------------------------------------------------------------------------- 
CREATE UNIQUE INDEX rdb_tile_node_tile_node_id_idx
  ON rdb_tile_node
  USING btree
  (tile_node_id);

CREATE UNIQUE INDEX rdb_tile_node_old_node_id_idx
  ON rdb_tile_node
  USING btree
  (old_node_id);
  
CREATE UNIQUE INDEX rdb_tile_link_tile_link_id_idx
  ON rdb_tile_link
  USING btree
  (tile_link_id);

CREATE UNIQUE INDEX rdb_tile_link_old_link_id_idx
  ON rdb_tile_link
  USING btree
  (old_link_id);

CREATE UNIQUE INDEX rdb_node_node_id_idx
  ON rdb_node
  USING btree
  (node_id);

CREATE UNIQUE INDEX rdb_node_height_node_id_idx
  ON rdb_node_height
  USING btree
  (node_id);
  
CREATE INDEX rdb_node_the_geom_idx
  ON rdb_node
  USING gist
  (the_geom);

CREATE INDEX rdb_node_node_id_t_idx
  ON rdb_node
  USING btree
  (node_id_t);

CREATE UNIQUE INDEX rdb_link_link_id_idx
  ON rdb_link
  USING btree
  (link_id);

CREATE INDEX rdb_link_link_id_t_idx
  ON rdb_link
  USING btree
  (link_id_t);

CREATE INDEX rdb_link_the_geom_idx
  ON rdb_link
  USING gist
  (the_geom);

CREATE INDEX rdb_link_start_node_id_idx
  ON rdb_link
  USING btree
  (start_node_id);

CREATE INDEX rdb_link_end_node_id_idx
  ON rdb_link
  USING btree
  (end_node_id);

CREATE INDEX rdb_link_road_type_idx
  ON rdb_link
  USING btree
  (road_type);

CREATE INDEX rdb_link_one_way_idx
  ON rdb_link
  USING btree
  (one_way);
  
CREATE INDEX rdb_guideinfo_lane_in_link_id_node_id_idx
  ON rdb_guideinfo_lane
  USING btree
  (in_link_id, node_id);

CREATE INDEX rdb_guideinfo_lane_node_id_idx
  ON rdb_guideinfo_lane
  USING btree
  (node_id);

CREATE INDEX rdb_guideinfo_lane_node_id_t_idx
  ON rdb_guideinfo_lane
  USING btree
  (node_id_t);

CREATE INDEX temp_rdb_linklane_info_oldlinkid_idx
  ON temp_rdb_linklane_info
  USING btree
  (old_link_id);

CREATE INDEX rdb_linklane_info_lane_id_idx
  ON rdb_linklane_info
  USING btree
  (lane_id);

CREATE INDEX rdb_line_name_kind_code_idx
  ON rdb_line_name
  USING btree
  (kind_code);

CREATE INDEX rdb_line_name_low_level_high_level_idx
  ON rdb_line_name
  USING btree
  (low_level, high_level);

CREATE INDEX rdb_line_name_name_id_idx
  ON rdb_line_name
  USING btree
  (name_id);

CREATE INDEX rdb_line_name_the_geom_idx
  ON rdb_line_name
  USING gist
  (the_geom);

CREATE INDEX rdb_poi_name_display_level_idx
  ON rdb_poi_name
  USING btree
  (low_level, high_level);

CREATE INDEX rdb_poi_name_level_prior_kind_idx
  ON rdb_poi_name
  USING btree
  (low_level, high_level, name_prior, kind_code);

CREATE INDEX rdb_poi_name_the_geom_idx
  ON rdb_poi_name
  USING gist
  (the_geom);

CREATE INDEX temp_regulation_temp_regulation_id_idx
  ON temp_regulation
  USING btree
  (temp_regulation_id);

CREATE INDEX temp_link_regulation_link_id_idx
  ON temp_link_regulation
  USING btree
  (link_id);

CREATE INDEX rdb_link_regulation_idx
  ON rdb_link_regulation
  USING btree
  (record_no);

CREATE INDEX rdb_link_regulation_key_string_idx
  ON rdb_link_regulation
  USING btree
  (key_string);

CREATE INDEX rdb_link_regulation_key_string_last_link_id_idx
  ON rdb_link_regulation
  USING btree
  (key_string, last_link_id);

CREATE INDEX rdb_link_regulation_regulation_id_idx
  ON rdb_link_regulation
  USING btree
  (regulation_id);
            
CREATE INDEX rdb_link_regulation_first_link_id_idx
  ON rdb_link_regulation
  USING btree
  (first_link_id);
            
CREATE INDEX rdb_link_regulation_first_link_id_t_idx
  ON rdb_link_regulation
  USING btree
  (first_link_id_t);

CREATE INDEX rdb_link_regulation_last_link_id_idx
  ON rdb_link_regulation
  USING btree
  (last_link_id);

CREATE INDEX rdb_link_regulation_last_link_id_t_idx
  ON rdb_link_regulation
  USING btree
  (last_link_id_t);

CREATE INDEX rdb_link_cond_regulation_idx
  ON rdb_link_cond_regulation
  USING btree
  (regulation_id);

CREATE INDEX rdb_cond_speed_link_id_idx
  ON rdb_cond_speed
  USING btree
  (link_id);
  
CREATE INDEX rdb_cond_speed_link_id_t_idx
  ON rdb_cond_speed
  USING btree
  (link_id_t);

CREATE INDEX rdb_link_add_info_link_id_idx
  ON rdb_link_add_info
  USING btree
  (link_id);
  
CREATE INDEX rdb_link_add_info_link_id_t_idx
  ON rdb_link_add_info
  USING btree
  (link_id_t);
  
CREATE INDEX rdb_link_add_info2_link_id_idx
  ON rdb_link_add_info2
  USING btree
  (link_id);
  
CREATE INDEX rdb_link_client_start_node_id_idx
  ON rdb_link_client
  USING btree
  (start_node_id);

CREATE INDEX rdb_link_client_start_node_id_t_idx
  ON rdb_link_client
  USING btree
  (start_node_id_t);

CREATE INDEX rdb_link_client_end_node_id_idx
  ON rdb_link_client
  USING btree
  (end_node_id);

CREATE INDEX rdb_link_client_end_node_id_t_idx
  ON rdb_link_client
  USING btree
  (end_node_id_t);

CREATE INDEX rdb_link_client_link_id_idx
  ON rdb_link_client
  USING btree
  (link_id);

CREATE INDEX rdb_link_client_link_id_t_idx
  ON rdb_link_client
  USING btree
  (link_id_t);

CREATE INDEX rdb_link_client_link_type_idx
  ON rdb_link_client
  USING btree
  (link_type);

CREATE INDEX rdb_link_client_the_geom_idx
  ON rdb_link_client
  USING gist
  (the_geom);

CREATE INDEX rdb_node_client_node_id_t_idx
  ON rdb_node_client
  USING btree
  (node_id_t);

CREATE INDEX rdb_node_client_the_geom_idx
  ON rdb_node_client
  USING gist
  (the_geom);

CREATE INDEX rdb_link_add_info_client_link_id_idx
  ON rdb_link_add_info_client
  USING btree
  (link_id);

CREATE INDEX rdb_guideinfo_lane_client_node_id_idx
  ON rdb_guideinfo_lane_client
  USING btree
  (node_id);

CREATE INDEX rdb_guideinfo_lane_client_in_link_id_node_id_idx
  ON rdb_guideinfo_lane_client
  USING btree
  (in_link_id, node_id);

CREATE INDEX rdb_guideinfo_lane_client_node_id_t_idx
  ON rdb_guideinfo_lane_client
  USING btree
  (node_id_t);

-- rdb_guideinfo_pic_blob_bytea
CREATE UNIQUE INDEX rdb_guideinfo_pic_blob_bytea_image_id_idx
  ON rdb_guideinfo_pic_blob_bytea
  USING btree
  (image_id);

-- rdb_region
CREATE INDEX rdb_region_layer_6_the_geom_idx
  ON rdb_region_layer_6
  USING gist
  (the_geom);

CREATE INDEX rdb_region_link_layer4_tbl_link_id_idx
  ON rdb_region_link_layer4_tbl
  USING btree
  (link_id);
  
CREATE INDEX rdb_region_link_layer4_tbl_link_id_t_idx
  ON rdb_region_link_layer4_tbl
  USING btree
  (link_id_t);

CREATE INDEX rdb_region_cond_speed_layer4_tbl_link_id_idx
  ON rdb_region_cond_speed_layer4_tbl
  USING btree
  (link_id);
  
CREATE INDEX rdb_region_cond_speed_layer4_tbl_link_id_t_idx
  ON rdb_region_cond_speed_layer4_tbl
  USING btree
  (link_id_t);

CREATE INDEX rdb_region_link_add_info_layer4_tbl_link_id_idx
  ON rdb_region_link_add_info_layer4_tbl
  USING btree
  (link_id);
  
CREATE INDEX rdb_region_link_add_info_layer4_tbl_link_id_t_idx
  ON rdb_region_link_add_info_layer4_tbl
  USING btree
  (link_id_t);

CREATE INDEX rdb_region_link_lane_info_layer4_tbl_lane_id_idx
  ON rdb_region_link_lane_info_layer4_tbl
  USING btree
  (lane_id);

CREATE INDEX rdb_region_link_regulation_layer4_tbl_first_link_id_idx
  ON rdb_region_link_regulation_layer4_tbl
  USING btree
  (first_link_id);
  
CREATE INDEX rdb_region_link_regulation_layer4_tbl_first_link_id_t_idx
  ON rdb_region_link_regulation_layer4_tbl
  USING btree
  (first_link_id_t);

CREATE INDEX rdb_region_link_regulation_layer4_tbl_last_link_id_idx
  ON rdb_region_link_regulation_layer4_tbl
  USING btree
  (last_link_id);
  
CREATE INDEX rdb_region_link_regulation_layer4_tbl_last_link_id_t_idx
  ON rdb_region_link_regulation_layer4_tbl
  USING btree
  (last_link_id_t);

CREATE INDEX rdb_region_node_layer4_tbl_node_id_idx
  ON rdb_region_node_layer4_tbl
  USING btree
  (node_id);
  
CREATE INDEX rdb_region_node_layer4_tbl_node_id_t_idx
  ON rdb_region_node_layer4_tbl
  USING btree
  (node_id_t);

CREATE INDEX rdb_region_link_layer6_tbl_link_id_idx
  ON rdb_region_link_layer6_tbl
  USING btree
  (link_id);
  
CREATE INDEX rdb_region_link_layer6_tbl_link_id_t_idx
  ON rdb_region_link_layer6_tbl
  USING btree
  (link_id_t);

CREATE INDEX rdb_region_cond_speed_layer6_tbl_link_id_idx
  ON rdb_region_cond_speed_layer6_tbl
  USING btree
  (link_id);
  
CREATE INDEX rdb_region_cond_speed_layer6_tbl_link_id_t_idx
  ON rdb_region_cond_speed_layer6_tbl
  USING btree
  (link_id_t);

CREATE INDEX rdb_region_link_add_info_layer6_tbl_link_id_idx
  ON rdb_region_link_add_info_layer6_tbl
  USING btree
  (link_id);
  
CREATE INDEX rdb_region_link_add_info_layer6_tbl_link_id_t_idx
  ON rdb_region_link_add_info_layer6_tbl
  USING btree
  (link_id_t);

CREATE INDEX rdb_region_link_lane_info_layer6_tbl_lane_id_idx
  ON rdb_region_link_lane_info_layer6_tbl
  USING btree
  (lane_id);

CREATE INDEX rdb_region_link_regulation_layer6_tbl_first_link_id_idx
  ON rdb_region_link_regulation_layer6_tbl
  USING btree
  (first_link_id);
  
CREATE INDEX rdb_region_link_regulation_layer6_tbl_first_link_id_t_idx
  ON rdb_region_link_regulation_layer6_tbl
  USING btree
  (first_link_id_t);

CREATE INDEX rdb_region_link_regulation_layer6_tbl_last_link_id_idx
  ON rdb_region_link_regulation_layer6_tbl
  USING btree
  (last_link_id);
  
CREATE INDEX rdb_region_link_regulation_layer6_tbl_last_link_id_t_idx
  ON rdb_region_link_regulation_layer6_tbl
  USING btree
  (last_link_id_t);

CREATE INDEX rdb_region_node_layer6_tbl_node_id_idx
  ON rdb_region_node_layer6_tbl
  USING btree
  (node_id);
  
CREATE INDEX rdb_region_node_layer6_tbl_node_id_t_idx
  ON rdb_region_node_layer6_tbl
  USING btree
  (node_id_t);


CREATE INDEX rdb_region_link_layer8_tbl_link_id_idx
  ON rdb_region_link_layer8_tbl
  USING btree
  (link_id);
  
CREATE INDEX rdb_region_link_layer8_tbl_link_id_t_idx
  ON rdb_region_link_layer8_tbl
  USING btree
  (link_id_t);

CREATE INDEX rdb_region_cond_speed_layer8_tbl_link_id_idx
  ON rdb_region_cond_speed_layer8_tbl
  USING btree
  (link_id);
  
CREATE INDEX rdb_region_cond_speed_layer8_tbl_link_id_t_idx
  ON rdb_region_cond_speed_layer8_tbl
  USING btree
  (link_id_t);

CREATE INDEX rdb_region_link_add_info_layer8_tbl_link_id_idx
  ON rdb_region_link_add_info_layer8_tbl
  USING btree
  (link_id);
  
CREATE INDEX rdb_region_link_add_info_layer8_tbl_link_id_t_idx
  ON rdb_region_link_add_info_layer8_tbl
  USING btree
  (link_id_t);

CREATE INDEX rdb_region_link_lane_info_layer8_tbl_lane_id_idx
  ON rdb_region_link_lane_info_layer8_tbl
  USING btree
  (lane_id);

CREATE INDEX rdb_region_link_regulation_layer8_tbl_first_link_id_idx
  ON rdb_region_link_regulation_layer8_tbl
  USING btree
  (first_link_id);
  
CREATE INDEX rdb_region_link_regulation_layer8_tbl_first_link_id_t_idx
  ON rdb_region_link_regulation_layer8_tbl
  USING btree
  (first_link_id_t);

CREATE INDEX rdb_region_link_regulation_layer8_tbl_last_link_id_idx
  ON rdb_region_link_regulation_layer8_tbl
  USING btree
  (last_link_id);
  
CREATE INDEX rdb_region_link_regulation_layer8_tbl_last_link_id_t_idx
  ON rdb_region_link_regulation_layer8_tbl
  USING btree
  (last_link_id_t);

CREATE INDEX rdb_region_node_layer8_tbl_node_id_idx
  ON rdb_region_node_layer8_tbl
  USING btree
  (node_id);
  
CREATE INDEX rdb_region_node_layer8_tbl_node_id_t_idx
  ON rdb_region_node_layer8_tbl
  USING btree
  (node_id_t);



---------------------------------------------------------------------------------------  
-- Create FOREIGN KEY
--------------------------------------------------------------------------------------- 
ALTER TABLE rdb_link
  ADD CONSTRAINT rdb_link_end_node_id_fkey FOREIGN KEY (end_node_id)
      REFERENCES rdb_node (node_id) MATCH FULL
      ON UPDATE NO ACTION ON DELETE NO ACTION;

ALTER TABLE rdb_link
  ADD CONSTRAINT rdb_link_lane_id_fkey FOREIGN KEY (lane_id)
      REFERENCES rdb_linklane_info (lane_id) MATCH FULL
      ON UPDATE NO ACTION ON DELETE NO ACTION;

ALTER TABLE rdb_link
  ADD CONSTRAINT rdb_link_start_node_id_fkey FOREIGN KEY (start_node_id)
      REFERENCES rdb_node (node_id) MATCH FULL
      ON UPDATE NO ACTION ON DELETE NO ACTION;

ALTER TABLE rdb_link_client
  ADD CONSTRAINT rdb_link_client_link_id_fkey FOREIGN KEY (link_id)
      REFERENCES rdb_link (link_id) MATCH FULL
      ON UPDATE NO ACTION ON DELETE NO ACTION;

ALTER TABLE rdb_link_client
  ADD CONSTRAINT rdb_link_client_end_node_id_fkey FOREIGN KEY (end_node_id)
      REFERENCES rdb_node (node_id) MATCH FULL
      ON UPDATE NO ACTION ON DELETE NO ACTION;
	  
ALTER TABLE rdb_line_name
  ADD CONSTRAINT rdb_line_name_name_id_fkey FOREIGN KEY (name_id)
      REFERENCES rdb_name_dictionary_chinese (name_id) MATCH FULL
      ON UPDATE NO ACTION ON DELETE NO ACTION;
	 
ALTER TABLE rdb_cond_speed
  ADD CONSTRAINT rdb_cond_speed_link_id_fkey FOREIGN KEY (link_id)
      REFERENCES rdb_link (link_id) MATCH FULL
      ON UPDATE NO ACTION ON DELETE NO ACTION;

ALTER TABLE rdb_guideinfo_lane
  ADD CONSTRAINT rdb_guideinfo_lane_node_id_fkey FOREIGN KEY (node_id)
      REFERENCES rdb_node (node_id) MATCH FULL
      ON UPDATE NO ACTION ON DELETE NO ACTION;

ALTER TABLE rdb_guideinfo_lane
  ADD CONSTRAINT rdb_guideinfo_lane_out_link_id_fkey FOREIGN KEY (out_link_id)
      REFERENCES rdb_link (link_id) MATCH FULL
      ON UPDATE NO ACTION ON DELETE NO ACTION;
	
ALTER TABLE rdb_link_regulation
  ADD CONSTRAINT rdb_link_regulation_last_link_id_fkey FOREIGN KEY (last_link_id)
      REFERENCES rdb_link (link_id) MATCH FULL
      ON UPDATE NO ACTION ON DELETE NO ACTION;

--------------------------------------------------------------------------------------- 	  
-- Client
-- Create FOREIGN KEY 
--------------------------------------------------------------------------------------- 	
ALTER TABLE rdb_cond_speed_client
  ADD CONSTRAINT rdb_cond_speed_client_link_id_fkey FOREIGN KEY (link_id)
      REFERENCES rdb_link (link_id) MATCH FULL
      ON UPDATE NO ACTION ON DELETE NO ACTION;
	  
	  
ALTER TABLE rdb_guideinfo_lane_client
  ADD CONSTRAINT rdb_guideinfo_lane_client_in_link_id_fkey FOREIGN KEY (in_link_id)
      REFERENCES rdb_link (link_id) MATCH FULL
      ON UPDATE NO ACTION ON DELETE NO ACTION;

ALTER TABLE rdb_guideinfo_lane_client
  ADD CONSTRAINT rdb_guideinfo_lane_client_node_id_fkey FOREIGN KEY (node_id)
      REFERENCES rdb_node (node_id) MATCH FULL
      ON UPDATE NO ACTION ON DELETE NO ACTION;

ALTER TABLE rdb_guideinfo_lane_client
  ADD CONSTRAINT rdb_guideinfo_lane_client_out_link_id_fkey FOREIGN KEY (out_link_id)
      REFERENCES rdb_link (link_id) MATCH FULL
      ON UPDATE NO ACTION ON DELETE NO ACTION;
	
ALTER TABLE rdb_guideinfo_signpost
  ADD CONSTRAINT rdb_guideinfo_signpost_pattern_id_fkey FOREIGN KEY (pattern_id)
      REFERENCES rdb_guideinfo_pic_blob_bytea (gid) MATCH FULL
      ON UPDATE NO ACTION ON DELETE NO ACTION;

ALTER TABLE rdb_guideinfo_signpost
  ADD CONSTRAINT rdb_guideinfo_signpost_arrow_id_fkey FOREIGN KEY (arrow_id)
      REFERENCES rdb_guideinfo_pic_blob_bytea (gid) MATCH FULL
      ON UPDATE NO ACTION ON DELETE NO ACTION;

ALTER TABLE rdb_guideinfo_signpost
  ADD CONSTRAINT rdb_guideinfo_signpost_in_link_id_fkey FOREIGN KEY (in_link_id)
      REFERENCES rdb_link (link_id) MATCH FULL
      ON UPDATE NO ACTION ON DELETE NO ACTION;
	  
ALTER TABLE rdb_guideinfo_signpost
  ADD CONSTRAINT rdb_guideinfo_signpost_node_id_fkey FOREIGN KEY (node_id)
      REFERENCES rdb_node (node_id) MATCH FULL
      ON UPDATE NO ACTION ON DELETE NO ACTION;
	  
ALTER TABLE rdb_guideinfo_signpost
  ADD CONSTRAINT rdb_guideinfo_signpost_out_link_id_fkey FOREIGN KEY (out_link_id)
      REFERENCES rdb_link (link_id) MATCH FULL
      ON UPDATE NO ACTION ON DELETE NO ACTION;	  
	
ALTER TABLE rdb_guideinfo_spotguidepoint
  ADD CONSTRAINT rdb_guideinfo_spotguidepoint_in_link_id_fkey FOREIGN KEY (in_link_id)
      REFERENCES rdb_link (link_id) MATCH FULL
      ON UPDATE NO ACTION ON DELETE NO ACTION;

ALTER TABLE rdb_guideinfo_spotguidepoint
  ADD CONSTRAINT rdb_guideinfo_spotguidepoint_pattern_id_fkey FOREIGN KEY (pattern_id)
      REFERENCES rdb_guideinfo_pic_blob_bytea (gid) MATCH FULL
      ON UPDATE NO ACTION ON DELETE NO ACTION;

ALTER TABLE rdb_guideinfo_spotguidepoint
  ADD CONSTRAINT rdb_guideinfo_spotguidepoint_arrow_id_fkey FOREIGN KEY (arrow_id)
      REFERENCES rdb_guideinfo_pic_blob_bytea (gid) MATCH FULL
      ON UPDATE NO ACTION ON DELETE NO ACTION;
	  
ALTER TABLE rdb_guideinfo_spotguidepoint
  ADD CONSTRAINT rdb_guideinfo_spotguidepoint_node_id_fkey FOREIGN KEY (node_id)
      REFERENCES rdb_node (node_id) MATCH FULL
      ON UPDATE NO ACTION ON DELETE NO ACTION;
	  
ALTER TABLE rdb_guideinfo_spotguidepoint
  ADD CONSTRAINT rdb_guideinfo_spotguidepoint_out_link_id_fkey FOREIGN KEY (out_link_id)
      REFERENCES rdb_link (link_id) MATCH FULL
      ON UPDATE NO ACTION ON DELETE NO ACTION;

ALTER TABLE rdb_guideinfo_towardname
  ADD CONSTRAINT rdb_guideinfo_towardname_in_link_id_fkey FOREIGN KEY (in_link_id)
      REFERENCES rdb_link (link_id) MATCH FULL
      ON UPDATE NO ACTION ON DELETE NO ACTION;

ALTER TABLE rdb_guideinfo_towardname
  ADD CONSTRAINT rdb_guideinfo_towardname_node_id_fkey FOREIGN KEY (node_id)
      REFERENCES rdb_node (node_id) MATCH FULL
      ON UPDATE NO ACTION ON DELETE NO ACTION;
	  
ALTER TABLE rdb_guideinfo_towardname
  ADD CONSTRAINT rdb_guideinfo_towardname_out_link_id_fkey FOREIGN KEY (out_link_id)
      REFERENCES rdb_link (link_id) MATCH FULL
      ON UPDATE NO ACTION ON DELETE NO ACTION;
	  
ALTER TABLE rdb_guideinfo_towardname
  ADD CONSTRAINT rdb_guideinfo_towardname_name_id_fkey FOREIGN KEY (name_id)
      REFERENCES rdb_name_dictionary_chinese (name_id) MATCH FULL
      ON UPDATE NO ACTION ON DELETE NO ACTION;
	
ALTER TABLE rdb_guideinfo_towardname_client
  ADD CONSTRAINT rdb_guideinfo_towardname_client_in_link_id_fkey FOREIGN KEY (in_link_id)
      REFERENCES rdb_link (link_id) MATCH FULL
      ON UPDATE NO ACTION ON DELETE NO ACTION;

ALTER TABLE rdb_guideinfo_towardname_client
  ADD CONSTRAINT rdb_guideinfo_towardname_client_node_id_fkey FOREIGN KEY (node_id)
      REFERENCES rdb_node (node_id) MATCH FULL
      ON UPDATE NO ACTION ON DELETE NO ACTION;
	  
ALTER TABLE rdb_guideinfo_towardname_client
  ADD CONSTRAINT rdb_guideinfo_towardname_client_out_link_id_fkey FOREIGN KEY (out_link_id)
      REFERENCES rdb_link (link_id) MATCH FULL
      ON UPDATE NO ACTION ON DELETE NO ACTION;
	  
ALTER TABLE rdb_guideinfo_towardname_client
  ADD CONSTRAINT rdb_guideinfo_towardname_client_name_id_fkey FOREIGN KEY (name_id)
      REFERENCES rdb_name_dictionary_chinese (name_id) MATCH FULL
      ON UPDATE NO ACTION ON DELETE NO ACTION;	  

ALTER TABLE rdb_link_add_info
  ADD CONSTRAINT rdb_link_add_info_link_id_fkey FOREIGN KEY (link_id)
      REFERENCES rdb_link (link_id) MATCH FULL
      ON UPDATE NO ACTION ON DELETE NO ACTION;
	  
ALTER TABLE rdb_link_add_info_client
  ADD CONSTRAINT rdb_link_add_info_client_link_id_fkey FOREIGN KEY (link_id)
      REFERENCES rdb_link (link_id) MATCH FULL
      ON UPDATE NO ACTION ON DELETE NO ACTION;
	
ALTER TABLE rdb_linklane_info_client
  ADD CONSTRAINT rdb_linklane_info_client_lane_id_fkey FOREIGN KEY (lane_id)
      REFERENCES rdb_linklane_info (lane_id) MATCH FULL
      ON UPDATE NO ACTION ON DELETE NO ACTION;

ALTER TABLE rdb_node_client
  ADD CONSTRAINT rdb_node_client_node_id_fkey FOREIGN KEY (node_id)
      REFERENCES rdb_node (node_id) MATCH FULL
      ON UPDATE NO ACTION ON DELETE NO ACTION;

CREATE INDEX rdb_guideinfo_crossname_node_id_idx
  ON rdb_guideinfo_crossname
  USING btree
  (node_id);
  
CREATE INDEX rdb_guideinfo_crossname_node_id_t_idx
  ON rdb_guideinfo_crossname
  USING btree
  (node_id_t);


 
-- rdb_link_regulation_client

CREATE INDEX rdb_link_regulation_client_key_string_idx
  ON rdb_link_regulation_client
  USING btree
  (key_string);

CREATE INDEX rdb_link_regulation_client_key_string_last_link_id_idx
  ON rdb_link_regulation_client
  USING btree
  (key_string, last_link_id);

CREATE INDEX rdb_link_regulation_client_last_link_id_idx
  ON rdb_link_regulation_client
  USING btree
  (last_link_id);

CREATE INDEX rdb_link_regulation_client_last_link_id_t_idx
  ON rdb_link_regulation_client
  USING btree
  (last_link_id_t);

CREATE INDEX rdb_link_regulation_client_regulation_id_idx
  ON rdb_link_regulation_client
  USING btree
  (regulation_id);
  
CREATE INDEX rdb_guideinfo_crossname_client_node_id_t_node_id_idx
  ON rdb_guideinfo_crossname_client
  USING btree
  (node_id_t, node_id);
  
CREATE INDEX rdb_guideinfo_crossname_client_node_id_t_idx
  ON rdb_guideinfo_crossname_client
  USING btree
  (node_id_t);
  
CREATE INDEX rdb_guideinfo_signpost_in_link_id_node_id_idx
  ON rdb_guideinfo_signpost
  USING btree
  (in_link_id, node_id);

CREATE INDEX rdb_guideinfo_signpost_node_id_idx
  ON rdb_guideinfo_signpost
  USING btree
  (node_id);

CREATE INDEX rdb_guideinfo_signpost_node_id_t_idx
  ON rdb_guideinfo_signpost
  USING btree
  (node_id_t);

CREATE INDEX rdb_guideinfo_signpost_client_in_link_id_node_id_idx
  ON rdb_guideinfo_signpost_client
  USING btree
  (in_link_id, node_id);

CREATE INDEX rdb_guideinfo_signpost_client_node_id_idx
  ON rdb_guideinfo_signpost_client
  USING btree
  (node_id);

CREATE INDEX rdb_guideinfo_signpost_client_node_id_t_idx
  ON rdb_guideinfo_signpost_client
  USING btree
  (node_id_t);
  
CREATE INDEX rdb_guideinfo_towardname_node_id_idx
  ON rdb_guideinfo_towardname
  USING btree
  (node_id);
                                                             
CREATE INDEX rdb_guideinfo_towardname_in_link_id_node_id_idx
 ON rdb_guideinfo_towardname
 USING btree
 (in_link_id, node_id);

CREATE INDEX rdb_guideinfo_towardname_node_id_t_idx
  ON rdb_guideinfo_towardname
  USING btree
  (node_id_t);

CREATE INDEX rdb_guideinfo_towardname_client_node_id_idx
  ON rdb_guideinfo_towardname_client
  USING btree
  (node_id);

CREATE INDEX rdb_guideinfo_towardname_client_in_link_id_node_id_idx
  ON rdb_guideinfo_towardname_client
  USING btree
  (in_link_id, node_id);            

CREATE INDEX rdb_guideinfo_towardname_client_node_id_t_idx
  ON rdb_guideinfo_towardname_client
  USING btree
  (node_id_t); 
  
CREATE INDEX rdb_guideinfo_spotguidepoint_in_link_id_node_id_idx
  ON rdb_guideinfo_spotguidepoint
  USING btree
  (in_link_id, node_id);

CREATE INDEX rdb_guideinfo_spotguidepoint_node_id_idx
  ON rdb_guideinfo_spotguidepoint
  USING btree
  (node_id);

CREATE INDEX rdb_guideinfo_spotguidepoint_node_id_t_idx
  ON rdb_guideinfo_spotguidepoint
  USING btree
  (node_id_t); 
  
CREATE INDEX rdb_guideinfo_spotguidepoint_client_in_link_id_node_id_idx
  ON rdb_guideinfo_spotguidepoint_client
  USING btree
  (in_link_id, node_id);

CREATE INDEX rdb_guideinfo_spotguidepoint_client_node_id_idx
  ON rdb_guideinfo_spotguidepoint_client
  USING btree
  (node_id);

CREATE INDEX rdb_guideinfo_spotguidepoint_client_node_id_t_idx
  ON rdb_guideinfo_spotguidepoint_client
  USING btree
  (node_id_t);
  
CREATE INDEX rdb_guideinfo_condition_condition_id_idx
  ON rdb_guideinfo_condition
  USING btree
  (condition_id);
  
CREATE INDEX rdb_guideinfo_natural_guidence_in_link_id_node_id_idx
  ON rdb_guideinfo_natural_guidence
  USING btree
  (in_link_id, node_id);

CREATE INDEX rdb_guideinfo_natural_guidence_node_id_idx
  ON rdb_guideinfo_natural_guidence
  USING btree
  (node_id);

CREATE INDEX rdb_guideinfo_natural_guidence_node_id_t_idx
  ON rdb_guideinfo_natural_guidence
  USING btree
  (node_id_t); 
  
CREATE INDEX rdb_guideinfo_natural_guidence_client_in_link_id_node_id_idx
  ON rdb_guideinfo_natural_guidence_client
  USING btree
  (in_link_id, node_id);

CREATE INDEX rdb_guideinfo_natural_guidence_client_node_id_idx
  ON rdb_guideinfo_natural_guidence_client
  USING btree
  (node_id);

CREATE INDEX rdb_guideinfo_natural_guidence_client_node_id_t_idx
  ON rdb_guideinfo_natural_guidence_client
  USING btree
  (node_id_t);
  
-------------------------------------------------------------------------------------
CREATE INDEX rdb_guideinfo_hook_turn_in_link_id_node_id_idx
  ON rdb_guideinfo_hook_turn
  USING btree
  (in_link_id, node_id);

CREATE INDEX rdb_guideinfo_hook_turn_node_id_idx
  ON rdb_guideinfo_hook_turn
  USING btree
  (node_id);

CREATE INDEX rdb_guideinfo_hook_turn_node_id_t_idx
  ON rdb_guideinfo_hook_turn
  USING btree
  (node_id_t); 
  
CREATE INDEX rdb_guideinfo_hook_turn_client_in_link_id_node_id_idx
  ON rdb_guideinfo_hook_turn_client
  USING btree
  (in_link_id, node_id);

CREATE INDEX rdb_guideinfo_hook_turn_client_node_id_idx
  ON rdb_guideinfo_hook_turn_client
  USING btree
  (node_id);

CREATE INDEX rdb_guideinfo_hook_turn_client_node_id_t_idx
  ON rdb_guideinfo_hook_turn_client
  USING btree
  (node_id_t);
-------------------------------------------------------------------------------------

CREATE INDEX rdb_cond_speed_client_link_id_t_idx
  ON rdb_cond_speed_client
  USING btree
  (link_id_t);

CREATE INDEX rdb_cond_speed_client_link_id_idx
  ON rdb_cond_speed_client
  USING btree
  (link_id);

CREATE INDEX rdb_guideinfo_building_structure_in_link_id_node_id_idx
  ON rdb_guideinfo_building_structure
  USING btree
  (in_link_id, node_id);

CREATE INDEX rdb_guideinfo_building_structure_node_id_idx
  ON rdb_guideinfo_building_structure
  USING btree
  (node_id);

CREATE INDEX rdb_guideinfo_building_structure_node_id_t_idx
  ON rdb_guideinfo_building_structure
  USING btree
  (node_id_t);
  
CREATE INDEX rdb_guideinfo_building_structure_client_in_link_id_node_id_idx
  ON rdb_guideinfo_building_structure_client
  USING btree
  (in_link_id, node_id);

CREATE INDEX rdb_guideinfo_building_structure_client_node_id_idx
  ON rdb_guideinfo_building_structure_client
  USING btree
  (node_id);

CREATE INDEX rdb_guideinfo_caution_in_link_id_node_id_idx
  ON rdb_guideinfo_caution
  USING btree
  (in_link_id, node_id);

CREATE INDEX rdb_guideinfo_caution_node_id_idx
  ON rdb_guideinfo_caution
  USING btree
  (node_id);

CREATE INDEX rdb_guideinfo_caution_node_id_t_idx
  ON rdb_guideinfo_caution
  USING btree
  (node_id_t);
  
CREATE INDEX rdb_guideinfo_caution_client_node_id_idx
  ON rdb_guideinfo_caution_client
  USING btree
  (node_id);

CREATE INDEX rdb_guideinfo_caution_client_in_link_id_node_id_idx
  ON rdb_guideinfo_caution_client
  USING btree
  (in_link_id, node_id);
  
CREATE INDEX rdb_guideinfo_forceguide_in_link_id_node_id_idx
  ON rdb_guideinfo_forceguide
  USING btree
  (in_link_id, node_id);

CREATE INDEX rdb_guideinfo_forceguide_node_id_idx
  ON rdb_guideinfo_forceguide
  USING btree
  (node_id);

CREATE INDEX rdb_guideinfo_forceguide_node_id_t_idx
  ON rdb_guideinfo_forceguide
  USING btree
  (node_id_t);
  
CREATE INDEX rdb_guideinfo_forceguide_client_node_id_idx
  ON rdb_guideinfo_forceguide_client
  USING btree
  (node_id);

CREATE INDEX  rdb_guideinfo_forceguide_client_in_link_id_node_id_idx
  ON rdb_guideinfo_forceguide_client
  USING btree
  (in_link_id, node_id);
 
CREATE INDEX rdb_guideinfo_road_structure_in_link_id_node_id_idx
  ON rdb_guideinfo_road_structure
  USING btree
  (in_link_id, node_id);

CREATE INDEX rdb_guideinfo_road_structure_node_id_idx
  ON rdb_guideinfo_road_structure
  USING btree
  (node_id);

CREATE INDEX rdb_guideinfo_road_structure_node_id_t_idx
  ON rdb_guideinfo_road_structure
  USING btree
  (node_id_t);

CREATE INDEX rdb_guideinfo_road_structure_client_in_link_id_node_id_idx
  ON rdb_guideinfo_road_structure_client
  USING btree
  (in_link_id, node_id);

CREATE INDEX rdb_guideinfo_road_structure_client_node_id_idx
  ON rdb_guideinfo_road_structure_client
  USING btree
  (node_id);

CREATE INDEX rdb_highway_facility_data_node_id_idx
  ON rdb_highway_facility_data
  USING btree
  (node_id);

CREATE INDEX rdb_highway_facility_data_node_id_t_idx
  ON rdb_highway_facility_data
  USING btree
  (node_id_t);
 
CREATE INDEX rdb_highway_facility_data_client_node_id_idx
  ON rdb_highway_facility_data_client
  USING btree
  (node_id);

CREATE INDEX rdb_highway_sa_pa_node_id_idx
  ON rdb_highway_sa_pa
  USING btree
  (node_id);

CREATE INDEX rdb_highway_sa_pa_client_node_id_idx
  ON rdb_highway_sa_pa
  USING btree
  (node_id);
  
CREATE INDEX temp_link_name_and_number_link_id_idx
  ON temp_link_name_and_number
  USING btree
  (link_id);
  
CREATE INDEX temp_link_name_and_number_name_id_idx
  ON temp_link_name_and_number
  USING btree
  (name_id);

CREATE INDEX rdb_link_shield_link_id_idx
  ON rdb_link_shield
  USING btree
  (link_id);

CREATE INDEX rdb_link_shield_link_id_t_idx
  ON rdb_link_shield
  USING btree
  (link_id_t);
  
CREATE INDEX rdb_link_shield_client_link_id_idx
  ON rdb_link_shield_client
  USING btree
  (link_id);

CREATE UNIQUE INDEX rdb_admin_zone_ad_code_idx
  ON rdb_admin_zone
  USING btree
  (ad_code);

CREATE UNIQUE INDEX rdb_admin_summer_time_summer_time_id_idx
  ON rdb_admin_summer_time
  USING btree
  (summer_time_id);

CREATE INDEX rdb_link_admin_zone_link_id_idx
  ON rdb_link_admin_zone
  USING btree
  (link_id);

CREATE INDEX rdb_tile_admin_zone_tile_id_idx
  ON rdb_tile_admin_zone
  USING btree
  (tile_id);

CREATE INDEX rdb_tile_admin_zone_the_geom_idx
  ON rdb_tile_admin_zone
  USING gist
  (the_geom);ALTER TABLE rdb_tile_admin_zone CLUSTER ON rdb_tile_admin_zone_the_geom_idx;

CREATE INDEX rdb_tile_out_of_admin_zone_tile_id_idx
  ON rdb_tile_out_of_admin_zone
  USING btree
  (tile_id);

CREATE INDEX rdb_tile_out_of_admin_zone_tile_x_tile_y_idx
  ON rdb_tile_out_of_admin_zone
  USING btree
  (tile_x, tile_y);

CREATE INDEX rdb_guideinfo_signpost_uc_in_link_id_node_id_idx
  ON rdb_guideinfo_signpost_uc
  USING btree
  (in_link_id, node_id);

CREATE INDEX rdb_guideinfo_signpost_uc_node_id_idx
  ON rdb_guideinfo_signpost_uc
  USING btree
  (node_id);

CREATE INDEX rdb_guideinfo_signpost_uc_node_id_t_idx
  ON rdb_guideinfo_signpost_uc
  USING btree
  (node_id_t);
  
CREATE INDEX rdb_guideinfo_signpost_uc_client_in_link_id_node_id_idx
  ON rdb_guideinfo_signpost_uc_client
  USING btree
  (in_link_id, node_id);

CREATE INDEX rdb_guideinfo_signpost_uc_client_node_id_idx
  ON rdb_guideinfo_signpost_uc_client
  USING btree
  (node_id);

CREATE INDEX rdb_guideinfo_signpost_uc_client_node_id_t_idx
  ON rdb_guideinfo_signpost_uc_client
  USING btree
  (node_id_t);


--------------------------------------------------------------------------------------- 
-- create index for id_fund
--------------------------------------------------------------------------------------- 

CREATE UNIQUE INDEX id_fund_node_node_id_idx
  ON id_fund_node
  USING btree
  (node_id);

CREATE INDEX id_fund_node_tile_id_idx
  ON id_fund_node
  USING btree
  (tile_id);

CREATE UNIQUE INDEX id_fund_node_feature_key_tile_id_idx
  ON id_fund_node
  USING btree
  (feature_key, tile_id);

CREATE INDEX id_fund_node_the_geom_idx
  ON id_fund_node
  USING gist
  (the_geom);
  

CREATE UNIQUE INDEX id_fund_link_link_id_idx
  ON id_fund_link
  USING btree
  (link_id);

CREATE INDEX id_fund_link_tile_id_idx
  ON id_fund_link
  USING btree
  (tile_id);

CREATE UNIQUE INDEX id_fund_link_feature_key_tile_id_idx
  ON id_fund_link
  USING btree
  (feature_key, tile_id);

CREATE INDEX id_fund_link_the_geom_idx
  ON id_fund_link
  USING gist
  (the_geom);

----------------------------------------------------
-- Vics
  
CREATE INDEX temp_tmc_chn_rdstmc_link_cnt_idx
  ON temp_tmc_chn_rdstmc
  USING btree
  (link_cnt);

CREATE INDEX temp_tmc_chn_rdstmc_link_idx
  ON temp_tmc_chn_rdstmc
  USING btree
  (link);

CREATE INDEX temp_tmc_chn_rdstmc_meshid_idx
  ON temp_tmc_chn_rdstmc
  USING btree
  (meshid);

CREATE INDEX temp_tmc_chn_rdslinkinfo_link_idx
  ON temp_tmc_chn_rdslinkinfo
  USING btree
  (link);

CREATE INDEX temp_tmc_chn_rdslinkinfo_mesh_idx
  ON temp_tmc_chn_rdslinkinfo
  USING btree
  (mesh);

CREATE INDEX temp_tmc_chn_rdslinkinfo_meshid_idx
  ON temp_tmc_chn_rdslinkinfo
  USING btree
  (meshid);

CREATE INDEX temp_tmc_chn_rdslinkinfo_road_id_idx
  ON temp_tmc_chn_rdslinkinfo
  USING btree
  (road_id);  

CREATE INDEX temp_tmc_chn_rdslinkinfo_meshid_link_idx
  ON temp_tmc_chn_rdslinkinfo
  USING btree
  (meshid,link); 

CREATE INDEX temp_link_mapping_meshid_road_id_numeric_idx
  ON temp_link_mapping
  USING btree
  (meshid,cast(road_id as numeric(10,0)));

CREATE INDEX temp_link_org_rdb_mesh_id_link_id_idx
  ON temp_link_org_rdb
  USING btree
  (org_mesh_id,org_link_id);

CREATE INDEX lq_vics_inf_id_idx
  ON lq_vics
  USING btree
  (inf_id);

CREATE INDEX lq_vics_link_id_idx
  ON lq_vics
  USING btree
  (link_id);

CREATE INDEX inf_vics_objectid_idx
  ON inf_vics
  USING btree
  (objectid);
  
CREATE INDEX lq_ipcvics_inf_id_idx
  ON lq_ipcvics
  USING btree
  (inf_id);

CREATE INDEX lq_ipcvics_link_id_idx
  ON lq_ipcvics
  USING btree
  (link_id);

CREATE INDEX inf_ipcvics_objectid_idx
  ON inf_ipcvics
  USING btree
  (objectid); 

CREATE INDEX temp_vics_org2rdb_with_node_linkdir_idx
  ON temp_vics_org2rdb_with_node
  USING btree
  (linkdir);  
CREATE INDEX temp_vics_org2rdb_with_node_s_node_idx
  ON temp_vics_org2rdb_with_node
  USING btree
  (s_node);
CREATE INDEX temp_vics_org2rdb_with_node_e_node_idx
  ON temp_vics_org2rdb_with_node
  USING btree
  (e_node);

CREATE INDEX temp_vics_org2rdb_with_node_classes_idx
  ON temp_vics_org2rdb_with_node
  USING btree
  (class0, class1, class2, class3, link_id);

CREATE INDEX temp_vics_link_walked_classes_idx
  ON temp_vics_link_walked
  USING btree
  (class0, class1, class2, class3, class4, link_id);

CREATE INDEX rdb_vics_org2rdb_jpn_bak_attr_idx
  ON rdb_vics_org2rdb_jpn_bak
  USING btree
  (meshcode, vicsclass, vicsid, target_link_id, linkdir);

CREATE INDEX rdb_ipc_vics_org2rdb_jpn_bak_attr_idx
  ON rdb_ipc_vics_org2rdb_jpn_bak
  USING btree
  (meshcode, vicsclass, vicsid, target_link_id, linkdir);

CREATE INDEX rdb_vics_org2rdb_axf_bak_attr_idx
  ON rdb_vics_org2rdb_axf_bak
  USING btree
  (loc_code_mesh, loc_code, dir, "type", service_id, target_link_id, link_dir);
  
CREATE INDEX temp_vics_link_seq_attr_idx
  ON temp_vics_link_seq
  USING btree
  (class0, class1, class2, link_id, linkdir);

----------------------------------------------------
-- language
CREATE UNIQUE INDEX rdb_language_language_code_idx
  ON rdb_language
  USING btree
  (language_code);
  
--------------------------------------------------------
-- link attri
CREATE INDEX rdb_link_with_all_attri_view_link_id_idx
  ON rdb_link_with_all_attri_view
  USING btree
  (link_id);

CREATE INDEX rdb_link_with_all_attri_view_link_id_t_idx
  ON rdb_link_with_all_attri_view
  USING btree
  (link_id_t);
  
--------------------------------------------------------
-- node attri
CREATE INDEX rdb_node_with_all_attri_view_node_id_idx
  ON rdb_node_with_all_attri_view
  USING btree
  (node_id);
--------------------------------------------------------
CREATE INDEX rdb_node_with_all_attri_view_node_id_t_idx
  ON rdb_node_with_all_attri_view
  USING btree
  (node_id_t);
  
--------------------------------------------------------
CREATE INDEX rdb_highway_mapping_link_id_idx
  ON rdb_highway_mapping
  USING btree
  (link_id);

--------------------------------------------------------
CREATE INDEX rdb_highway_mapping_tile_id_idx
  ON rdb_highway_mapping
  USING btree
  (tile_id);

--------------------------------------------------------
CREATE INDEX rdb_highway_mapping_the_geom_idx
  ON rdb_highway_mapping
  USING gist
  (the_geom);
--------------------------------------------------------
CREATE UNIQUE INDEX rdb_highway_mapping_link_id_forward_ic_no_backward_ic_no_idx
  ON rdb_highway_mapping
  USING btree
  (link_id, forward_ic_no, backward_ic_no);
  
--------------------------------------------------------
CREATE INDEX rdb_highway_ic_info_ic_no_idx
  ON rdb_highway_ic_info
  USING btree
  (ic_no);

--------------------------------------------------------
CREATE INDEX rdb_highway_ic_info_tile_id_idx
  ON rdb_highway_ic_info
  USING btree
  (tile_id);
  
--------------------------------------------------------
CREATE INDEX rdb_highway_ic_info_up_down_facility_id_idx
  ON rdb_highway_ic_info
  USING btree
  (up_down, facility_id);

--------------------------------------------------------
CREATE INDEX rdb_highway_conn_info_ic_no_idx
  ON rdb_highway_conn_info
  USING btree
  (ic_no);
  
--------------------------------------------------------
CREATE INDEX rdb_highway_conn_info_tile_id_idx
  ON rdb_highway_conn_info
  USING btree
  (tile_id);
  
--------------------------------------------------------
CREATE INDEX rdb_highway_path_point_ic_no_idx
  ON rdb_highway_path_point
  USING btree
  (ic_no);

--------------------------------------------------------
CREATE INDEX rdb_highway_road_info_tile_id_idx
  ON rdb_highway_road_info
  USING btree
  (tile_id);

--------------------------------------------------------
CREATE INDEX rdb_highway_ic_mapping_up_down_facility_id_idx
  ON rdb_highway_ic_mapping
  USING btree
  (up_down, facility_id);
  
--------------------------------------------------------
CREATE UNIQUE INDEX rdb_highway_service_info_ic_no_idx
  ON rdb_highway_service_info
  USING btree
  (ic_no);

--------------------------------------------------------
CREATE INDEX rdb_highway_node_add_info_link_geom_idx
  ON rdb_highway_node_add_info
  USING gist
  (link_geom);

--------------------------------------------------------
CREATE UNIQUE INDEX rdb_highway_node_add_info_link_id_idx
  ON rdb_highway_node_add_info
  USING btree
  (link_id);

--------------------------------------------------------
CREATE INDEX rdb_highway_node_add_info_node_geom_idx
  ON rdb_highway_node_add_info
  USING gist
  (node_geom);

--------------------------------------------------------
CREATE INDEX rdb_highway_path_point_link_geom_idx
  ON rdb_highway_path_point
  USING gist
  (link_geom);
--------------------------------------------------------
CREATE INDEX rdb_highway_path_point_the_geom_idx
  ON rdb_highway_path_point
  USING gist
  (the_geom);

--------------------------------------------------------
CREATE UNIQUE INDEX rdb_highway_fee_toll_info_facility_id_up_down_idx
  ON rdb_highway_fee_toll_info
  USING btree
  (facility_id, up_down);

--------------------------------------------------------
CREATE UNIQUE INDEX rdb_highway_fee_alone_toll_facility_id_up_down_idx
  ON rdb_highway_fee_alone_toll
  USING btree
  (facility_id, up_down);

--------------------------------------------------------
CREATE UNIQUE INDEX rdb_highway_fee_ticket_toll_from_facility_id_to_facility_id_idx
  ON rdb_highway_fee_ticket_toll
  USING btree
  (from_facility_id, to_facility_id);

--------------------------------------------------------
CREATE UNIQUE INDEX rdb_highway_fee_free_toll_from_facility_id_to_facility_id_idx
  ON rdb_highway_fee_free_toll
  USING btree
  (from_facility_id, to_facility_id);

CREATE UNIQUE INDEX rdb_highway_fee_same_facility_from_facility_id_to_facility_id_idx
  ON rdb_highway_fee_same_facility
  USING btree
  (from_facility_id, to_facility_id);

--------------------------------------------------------
CREATE INDEX temp_link_sequence_dir_link_id_idx
  ON temp_link_sequence_dir
  USING btree
  (link_id);
CREATE INDEX rdb_link_sequence_link_id_idx
  ON rdb_link_sequence
  USING btree
  (link_id);
CREATE INDEX temp_region_link_layer4_sequence_dir_link_id_idx
  ON temp_region_link_layer4_sequence_dir
  USING btree
  (link_id);
CREATE INDEX rdb_region_link_layer4_sequence_link_id_idx
  ON rdb_region_link_layer4_sequence
  USING btree
  (link_id);
CREATE INDEX temp_region_link_layer6_sequence_dir_link_id_idx
  ON temp_region_link_layer6_sequence_dir
  USING btree
  (link_id);
CREATE INDEX rdb_region_link_layer6_sequence_link_id_idx
  ON rdb_region_link_layer6_sequence
  USING btree
  (link_id);
CREATE INDEX temp_region_link_layer8_sequence_dir_link_id_idx
  ON temp_region_link_layer8_sequence_dir
  USING btree
  (link_id);
CREATE INDEX rdb_region_link_layer8_sequence_link_id_idx
  ON rdb_region_link_layer8_sequence
  USING btree
  (link_id);
CREATE INDEX rdb_node_gid_idx
  ON rdb_node
  USING btree
  (gid);
CREATE INDEX rdb_region_node_layer4_tbl_gid_idx
  ON rdb_region_node_layer4_tbl
  USING btree
  (gid);
CREATE INDEX rdb_region_node_layer6_tbl_gid_idx
  ON rdb_region_node_layer6_tbl
  USING btree
  (gid);
CREATE INDEX rdb_region_node_layer8_tbl_gid_idx
  ON rdb_region_node_layer8_tbl
  USING btree
  (gid);
CREATE INDEX rdb_link_gid_idx
  ON rdb_link
  USING btree
  (gid);
CREATE INDEX link_tbl_gid_idx
  ON link_tbl
  USING btree
  (gid);
CREATE INDEX rdb_region_link_layer4_tbl_gid_idx
  ON rdb_region_link_layer4_tbl
  USING btree
  (gid);
CREATE INDEX rdb_region_link_layer6_tbl_gid_idx
  ON rdb_region_link_layer6_tbl
  USING btree
  (gid);
CREATE INDEX rdb_region_link_layer8_tbl_gid_idx
  ON rdb_region_link_layer8_tbl
  USING btree
  (gid);
CREATE INDEX rdb_region_link_layer4_tbl_start_node_id_idx
  ON rdb_region_link_layer4_tbl
  USING btree
  (start_node_id);
CREATE INDEX rdb_region_link_layer4_tbl_end_node_id_idx
  ON rdb_region_link_layer4_tbl
  USING btree
  (end_node_id);
CREATE INDEX rdb_region_link_layer6_tbl_start_node_id_idx
  ON rdb_region_link_layer6_tbl
  USING btree
  (start_node_id);
CREATE INDEX rdb_region_link_layer6_tbl_end_node_id_idx
  ON rdb_region_link_layer6_tbl
  USING btree
  (end_node_id);
CREATE INDEX rdb_region_link_layer8_tbl_start_node_id_idx
  ON rdb_region_link_layer8_tbl
  USING btree
  (start_node_id);
CREATE INDEX rdb_region_link_layer8_tbl_end_node_id_idx
  ON rdb_region_link_layer8_tbl
  USING btree
  (end_node_id);
CREATE INDEX rdb_guideinfo_safety_zone_link_id_idx
  ON rdb_guideinfo_safety_zone
  USING btree
  (link_id);
-------------------------------------------------------------
CREATE INDEX rdb_trf_tile_area_no_tile_id_idx
  ON rdb_trf_tile_area_no
  USING btree
  (tile_id);
CREATE INDEX rdb_link_add_info2_link_id_t_idx
  ON rdb_link_add_info2
  USING btree
  (link_id_t);