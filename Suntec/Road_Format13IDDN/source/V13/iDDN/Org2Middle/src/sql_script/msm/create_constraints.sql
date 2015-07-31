---------------------------------------------------------------------------------------------------------
-- scripts for create constraints in this file.
-- Project: Malsing
---------------------------------------------------------------------------------------------------------

CREATE INDEX temp_topo_link_with_attr_link_id_idx
  ON temp_topo_link_with_attr
  USING btree
  (link_id);
CREATE INDEX temp_topo_link_with_attr_folder_old_link_id_idx
  ON temp_topo_link_with_attr
  USING btree
  (folder,old_link_id);
CREATE INDEX temp_topo_link_with_attr_s_node_idx
  ON temp_topo_link_with_attr
  USING btree
  (s_node);
CREATE INDEX temp_topo_link_with_attr_e_node_idx
  ON temp_topo_link_with_attr
  USING btree
  (e_node);
CREATE INDEX temp_topo_link_with_attr_one_way_idx
  ON temp_topo_link_with_attr
  USING btree
  (one_way);
CREATE INDEX temp_topo_link_with_attr_type_idx
  ON temp_topo_link_with_attr
  USING btree
  (type);
CREATE INDEX temp_topo_link_with_attr_name_idx
  ON temp_topo_link_with_attr
  USING btree
  (name);
CREATE INDEX temp_topo_link_with_attr_route_lvl_idx
  ON temp_topo_link_with_attr
  USING btree
  (route_lvl);
CREATE INDEX temp_topo_link_with_attr_the_geom_idx
  ON temp_topo_link_with_attr
  USING gist
  (the_geom);
  
CREATE INDEX temp_link_innerlink_link_id_idx
  ON temp_link_innerlink
  USING btree
  (link_id);

------------------------------------------------------------
-- Create table for processing ramp&roundabout.
------------------------------------------------------------  
CREATE INDEX temp_link_special_id_idx
  ON temp_link_special
  USING btree
  (link_id);

CREATE INDEX temp_link_special_new_type_new_link_id_idx
  ON temp_link_special_new_type
  USING btree
  (new_link_id);
  
CREATE INDEX temp_link_special_new_type_new_type_idx
  ON temp_link_special_new_type
  USING btree
  (new_type);

CREATE INDEX temp_link_special_new_type_for_dsp_new_link_id_idx
  ON temp_link_special_new_type_for_dsp
  USING btree
  (new_link_id);
  
CREATE INDEX temp_link_special_new_type_for_dsp_new_type_idx
  ON temp_link_special_new_type_for_dsp
  USING btree
  (new_type);
  
CREATE INDEX temp_link_special_new_level_new_link_id_idx
  ON temp_link_special_new_level
  USING btree
  (new_link_id);
  
CREATE INDEX temp_link_special_new_level_new_level_idx
  ON temp_link_special_new_level
  USING btree
  (new_level);

CREATE INDEX temp_link_roundabout_link_id_idx
  ON temp_link_roundabout
  USING btree
  (link_id);

CREATE INDEX temp_link_roundabout_new_type_new_link_id_idx
  ON temp_link_roundabout_new_type
  USING btree
  (new_link_id);
  
CREATE INDEX temp_link_roundabout_new_type_new_type_idx
  ON temp_link_roundabout_new_type
  USING btree
  (new_type);

CREATE INDEX temp_link_roundabout_dsp_new_link_id_idx
  ON temp_link_roundabout_dsp
  USING btree
  (new_link_id);
  
------------------------------------------------------------
-- Create table for expressway.
------------------------------------------------------------ 
CREATE INDEX temp_link_expressway_link_id_idx
  ON temp_link_expressway
  USING btree
  (link_id);

  
CREATE INDEX temp_turn_link_reg_id
  ON temp_turn_link
  USING btree
  (reg_id);
  
CREATE INDEX temp_topo_link_org_oneway_end_node_id_idx
  ON temp_topo_link_org_oneway
  USING btree
  (end_node_id,oneway);

CREATE INDEX temp_topo_link_org_oneway_start_node_id_idx
  ON temp_topo_link_org_oneway
  USING btree
  (start_node_id,oneway);
  
CREATE INDEX org_processed_line_folder_idx
  ON org_processed_line
  USING btree
  (folder);

CREATE UNIQUE INDEX org_processed_line_folder_link_id_idx
  ON org_processed_line
  USING btree
  (folder, link_id);
  
create index temp_topo_point_3cm_ix_idx
	on temp_topo_point_3cm
	using btree
	(ix);

create index temp_topo_point_3cm_iy_idx
	on temp_topo_point_3cm
	using btree
	(iy);

create index temp_topo_point_3cm_ix_m_idx
	on temp_topo_point_3cm
	using btree
	(ix_m);

create index temp_topo_point_3cm_iy_m_idx
	on temp_topo_point_3cm
	using btree
	(iy_m);

create index temp_topo_near_point_aid_idx
	on temp_topo_near_point
	using btree
	(aid);

create index temp_topo_near_point_bid_idx
	on temp_topo_near_point
	using btree
	(bid);
create index temp_topo_node_node_id_idx
	on temp_topo_node
	using btree
	(node_id);

create index temp_topo_node_the_geom_idx
	on temp_topo_node
	using gist
	(the_geom);

create index temp_topo_point_node_x_idx
	on temp_topo_point_node
	using btree
	(x);

create index temp_topo_point_node_y_idx
	on temp_topo_point_node
	using btree
	(y);

create index temp_topo_point_node_z_idx
	on temp_topo_point_node
	using btree
	(z);
create index temp_topo_link_new_link_id_idx
	on temp_topo_link
	using btree
	(new_link_id);

create index temp_topo_link_folder_link_id_idx
	on temp_topo_link
	using btree
	(folder,link_id);
	
create index temp_topo_link_the_geom_idx
	on temp_topo_link
	using gist
	(the_geom);