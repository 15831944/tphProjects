CREATE INDEX org_roadnode_meshid_idx
  ON org_roadnode
  USING btree
  (meshid);

CREATE INDEX org_roadnode_node_idx
  ON org_roadnode
  USING btree
  (node);

CREATE INDEX org_roadnode_meshid_node_idx
  ON org_roadnode
  USING btree
  (meshid, node);
  
CREATE INDEX org_roadnode_comp_node_idx
  ON org_roadnode
  USING btree
  (comp_node);
  
CREATE INDEX org_roadnode_meshid_comp_node_idx
  ON org_roadnode
  USING btree
  (meshid, comp_node);

CREATE INDEX org_roadsegment_meshid_idx
  ON org_roadsegment
  USING btree
  (meshid);

CREATE INDEX org_roadsegment_fnode_idx
  ON org_roadsegment
  USING btree
  (fnode);

CREATE INDEX org_roadsegment_meshid_fnode_idx
  ON org_roadsegment
  USING btree
  (meshid, fnode);
  
CREATE INDEX org_roadsegment_tnode_idx
  ON org_roadsegment
  USING btree
  (tnode);
  
CREATE INDEX org_roadsegment_meshid_tnode_idx
  ON org_roadsegment
  USING btree
  (meshid, tnode);
  
CREATE INDEX org_roadsegment_road_idx
  ON org_roadsegment
  USING btree
  (road);

CREATE INDEX org_roadsegment_meshid_road_idx
  ON org_roadsegment
  USING btree
  (meshid, road);
  
CREATE INDEX temp_org_rass_name_meshid_id_idx
  ON temp_org_rass_name
  USING btree
  (meshid, id);
  
CREATE INDEX temp_org_rass_name_name_type_idx
  ON temp_org_rass_name
  USING btree
  (name_type);

CREATE INDEX org_xlpath_tbl_xlpath_idx
  ON org_xlpath
  USING btree
  (xlpath);

CREATE INDEX org_roadcross_node_idx
  ON org_roadcross
  USING btree
  (node);


CREATE INDEX org_roadnodesignpost_sp_meshid_idx
  ON org_roadnodesignpost
  USING btree
  (meshid, sp);

CREATE INDEX org_roadcrosssignpost_sp_meshid_idx
  ON org_roadcrosssignpost
  USING btree
  (meshid, sp);
  
CREATE INDEX temp_node_signpost_dict_sp_meshid_idx
  ON temp_node_signpost_dict
  USING btree
  (meshid, sp);
  
CREATE INDEX temp_cross_signpost_dict_sp_meshid_idx
  ON temp_cross_signpost_dict
  USING btree
  (meshid, sp);
  
CREATE INDEX org_roadnode_meshid_node_idx
  ON org_roadnode
  USING btree
  (meshid, node);
  
CREATE INDEX org_roadsegment_meshid_road_idx
  ON org_roadsegment
  USING btree
  (meshid, road);
  
CREATE INDEX temp_node_id_meshid_idx
  ON temp_node_id
  USING btree
  (meshid);
  
CREATE INDEX temp_node_id_meshid_node_idx
  ON temp_node_id
  USING btree
  (meshid, node);

CREATE INDEX temp_node_id_g_node_idx
  ON temp_node_id
  USING btree
  (g_node);
  
CREATE INDEX temp_node_id_xy_coord_100_idx
  ON temp_node_id
  USING btree
  (x_coord_100, y_coord_100);
  
CREATE INDEX temp_node_id_xy_coord_100_m_idx
  ON temp_node_id
  USING btree
  (x_coord_100_m, y_coord_100_m);

CREATE INDEX temp_node_boundary_g_node_idx
  ON temp_node_boundary
  USING btree
  (g_node);

CREATE INDEX temp_node_mapping_meshid_node_idx
  ON temp_node_mapping
  USING btree
  (meshid, node);

CREATE INDEX temp_node_mapping_g_node_idx
  ON temp_node_mapping
  USING btree
  (g_node);
  
CREATE INDEX temp_link_mapping_meshid_road_idx
  ON temp_link_mapping
  USING btree
  (meshid, road);
  
CREATE INDEX temp_link_mapping_new_road_idx
  ON temp_link_mapping
  USING btree
  (new_road);
  
CREATE INDEX temp_node_u_meshid_node_idx
  ON temp_node_u
  USING btree
  (meshid, node);

CREATE INDEX temp_node_u_new_node_idx
  ON temp_node_u
  USING btree
  (new_node);

CREATE INDEX org_roadfurnituresignpost_meshid_idx
  ON org_roadfurnituresignpost
  USING btree
  (meshid);

CREATE INDEX org_roadfurnituresignpost_node_idx
  ON org_roadfurnituresignpost
  USING btree
  (node);

CREATE INDEX org_rfsppath_meshid_idx
  ON org_rfsppath
  USING btree
  (meshid);

CREATE INDEX org_rfsppath_link_idx
  ON org_rfsppath
  USING btree
  (link);

CREATE INDEX org_rfsppathlink_meshid_link_idx
  ON org_rfsppathlink
  USING btree
  (meshid, link);

CREATE INDEX mesh_mapping_tbl_meshid_str_idx
  ON mesh_mapping_tbl
  USING btree
  (meshid_str);


CREATE INDEX org_roadnodesaat_meshid_idx
  ON org_roadnodesaat
  USING btree
  (meshid);

CREATE INDEX org_roadnodesaat_node_idx
  ON org_roadnodesaat
  USING btree
  (node);

CREATE INDEX org_roadnodesaat_road_idx
  ON org_roadnodesaat
  USING btree
  (road);

CREATE INDEX org_roadnodemaat_meshid_idx
  ON org_roadnodemaat
  USING btree
  (meshid);

CREATE INDEX org_roadnodemaat_node_idx
  ON org_roadnodesaat
  USING btree
  (node);

CREATE INDEX org_roadnodemaat_from_road_idx
  ON org_roadnodemaat
  USING btree
  (from_road);

  
CREATE INDEX org_roadcrosssaat_meshid_idx
  ON org_roadcrosssaat
  USING btree
  (meshid);

CREATE INDEX org_roadcrosssaat_node_idx
  ON org_roadcrosssaat
  USING btree
  (node);

CREATE INDEX org_roadcrosssaat_road_idx
  ON org_roadcrosssaat
  USING btree
  (road);

CREATE INDEX org_roadcrossmaat_meshid_idx
  ON org_roadcrossmaat
  USING btree
  (meshid);

CREATE INDEX org_roadcrossmaat_node_idx
  ON org_roadcrossmaat
  USING btree
  (node);

CREATE INDEX org_roadcrossmaat_from_road_idx
  ON org_roadcrossmaat
  USING btree
  (from_road);

CREATE INDEX temp_poi_name_meshid_old_name_id_idx
  ON temp_poi_name
  USING btree
  (meshid, old_name_id);
  
CREATE INDEX org_poi_meshid_poi_idx
  ON org_poi
  USING btree
  (meshid, poi);

CREATE INDEX org_poiname_meshid_name_idx
  ON org_poiname
  USING btree
  (meshid, name);
  
 CREATE INDEX org_interchange_ic_idx
  ON org_interchange
  USING btree
  (ic);
 
 CREATE INDEX org_sapa_sapa_idx
  ON org_sapa
  USING btree
  (sapa);


CREATE INDEX org_interchange_ic_id_idx
  ON org_interchange
  USING btree
  (ic_id);


CREATE INDEX org_interchange_ic_type_idx
  ON org_interchange
  USING btree
  (ic_type);


CREATE INDEX org_interchange_mesh_node_idx
  ON org_interchange
  USING btree
  (mesh, node);

CREATE INDEX temp_node_mapping_new_node_idx
  ON temp_node_mapping
  USING btree
  (new_node);


CREATE INDEX temp_node_mapping_node_idx
  ON temp_node_mapping
  USING btree
  (node);


create index org_highwaysingle_mesh_idx
	on org_highwaysingle
	using btree
	(mesh);

create index org_highwaysingle_node_idx
	on org_highwaysingle
	using btree
	(node);

create index org_highwaysingle_from_road_idx
	on org_highwaysingle
	using btree
	(from_road);

create index org_highwaysingle_to_road_idx
	on org_highwaysingle
	using btree
	(to_road);

create index org_highwaysingle_image_id_idx
	on org_highwaysingle
	using btree
	(image_id);

create index org_highwaydouble_mesh_idx
	on org_highwaydouble
	using btree
	(mesh);

create index org_highwaydouble_node_idx
	on org_highwaydouble
	using btree
	(node);

create index org_highwaydoublepath_hwp_idx
	on org_highwaydoublepath
	using btree
	(hwp);
