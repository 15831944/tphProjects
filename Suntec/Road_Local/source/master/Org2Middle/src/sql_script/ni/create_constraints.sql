
----------------------------------------------------- 
CREATE INDEX org_cond_mapid_condid_idx
  ON org_cond
  USING btree
  (mapid, condid);

----------------------------------------------------- 
CREATE INDEX org_cnl_mapid_condid_idx
  ON org_cnl
  USING btree
  (mapid, condid);

-----------------------------------------------------   
CREATE INDEX org_r_id_idx
  ON org_r
  USING btree
  (id);
CREATE INDEX org_n_id_idx
  on org_n
  USING btree
  (id);
CREATE INDEX org_r_id_2_idx
  ON org_r
  USING btree
  (cast(id as bigint));
  
CREATE INDEX org_r_kind_idx
  ON org_r
  USING btree
  (kind);
  
CREATE INDEX org_r_snodeid_idx
  ON org_r
  USING btree
  (snodeid);
  
CREATE INDEX org_r_enodeid_idx
  ON org_r
  USING btree
  (enodeid);

CREATE INDEX temp_link_name_link_id_idx
  ON temp_link_name
  USING btree
  (link_id);
  
CREATE INDEX temp_link_shield_link_id_idx
  ON temp_link_shield
  USING btree
  (link_id);  

----------------------------------------------------- 
CREATE INDEX org_trfcsign_type_idx
  ON org_trfcsign
  USING btree
  (cast(type as integer));

----------------------------------------------------- 
CREATE INDEX mid_admin_zone_ad_order_idx
  ON mid_admin_zone
  USING btree
  (ad_order);

----------------------------------------------------- 
CREATE INDEX mid_admin_zone_order0_id_idx
  ON mid_admin_zone
  USING btree
  (order0_id);

----------------------------------------------------- 
CREATE INDEX mid_admin_zone_order1_id_idx
  ON mid_admin_zone
  USING btree
  (order1_id);

----------------------------------------------------- 
CREATE INDEX mid_admin_zone_order2_id_idx
  ON mid_admin_zone
  USING btree
  (order2_id);

  CREATE INDEX temp_poi_logmark_poi_id_idx
  ON temp_poi_logmark
  USING btree
  (poi_id);
  
CREATE INDEX temp_poi_name_poi_id_idx
  ON temp_poi_name
  USING btree
  (poi_id);
  
-----------------------------------------------------
----admin
-----------------------------------------------------
CREATE INDEX org_d_admincode_idx
  ON org_d
  USING btree
  (admincode);
  
CREATE INDEX org_admin_admincode_idx
  ON org_admin
  USING btree
  (admincode);
  
CREATE INDEX org_admin_cityadcode_idx
  ON org_admin
  USING btree
  (cityadcode);
  
CREATE INDEX org_admin_proadcode_idx
  ON org_admin
  USING btree
  (proadcode);
  
CREATE INDEX temp_admin_order8_order8_id_idx
  ON temp_admin_order8
  USING btree
  (order8_id);
  
CREATE INDEX temp_admin_order2_order2_id_idx
  ON temp_admin_order2
  USING btree
  (order2_id);
  
CREATE INDEX temp_admin_order1_order1_id_idx
  ON temp_admin_order1
  USING btree
  (order1_id);
  
CREATE INDEX temp_admin_name_admin_id_idx
  ON temp_admin_name
  USING btree
  (admin_id);

-----------------------------------------------------
--signpost_uc
CREATE INDEX temp_signpost_uc_name_sign_id_idx
  ON temp_signpost_uc_name
  USING btree
  (sign_id);
  
CREATE INDEX temp_signpost_uc_path_path_id_idx
  ON temp_signpost_uc_path
  USING btree
  (path_id);
  
CREATE INDEX temp_signpost_uc_path_id_path_id_idx
  ON temp_signpost_uc_path
  USING btree
  (path_id);
  
CREATE INDEX temp_signpost_uc_path_id_fname_idx
  ON temp_signpost_uc_path_id
  USING btree
  (fname_id,fname_type); 
  
CREATE INDEX temp_node_mapping_old_node_id_idx
  ON temp_node_mapping
  USING btree
  (old_node_id);
  
CREATE INDEX temp_node_mapping_new_node_id_idx
  ON temp_node_mapping
  USING btree
  (new_node_id);

-----------------------------------------------------
CREATE INDEX org_poi_poi_id_idx
  ON org_poi
  USING btree
  (poi_id);

CREATE INDEX org_poi_chaincode_idx
  ON org_poi
  USING btree
  (chaincode);

CREATE INDEX org_poi_kind_idx
  ON org_poi
  USING btree
  (kind);

-----------------------------------------------------
CREATE INDEX org_poi_linkid_idx
  ON org_poi
  USING btree
  (linkid);

-----------------------------------------------------
CREATE INDEX org_poi_linkid_idx2
  ON org_poi
  USING btree
  ((linkid::bigint));

-----------------------------------------------------
CREATE INDEX org_n_the_geom_idx
  ON org_n
  USING gist
  (the_geom);

  
  
CREATE INDEX temp_poi_find_poi_id_idx
ON temp_poi_find
USING btree
((poi_id::bigint));


CREATE INDEX temp_poi_inlink_poi_id_idx
ON temp_poi_inlink
USING btree
(poi_id); 


-----------------------------------------------------
CREATE INDEX temp_node_z_level_tbl_node_id_idx
  ON temp_node_z_level_tbl
  USING btree
  (node_id);
 
CREATE INDEX org_r_lname_route_id_idx
  ON org_r_lname
  USING btree
  (route_id);
 
CREATE INDEX org_r_name_route_id_idx
  ON org_r_name
  USING btree
  (route_id);
 
CREATE INDEX org_r_phon_route_id_idx
  ON org_r_phon
  USING btree
  (route_id);
 
CREATE INDEX org_fname_featid_idx
  ON org_fname
  USING btree
  (featid);
 
CREATE INDEX org_fname_phon_featid_idx
  ON org_fname_phon
  USING btree
  (featid);
 
CREATE INDEX org_cr_vperiod_vehcl_type_idx
  ON org_cr
  USING btree
  (vperiod, vehcl_type);
 
CREATE INDEX temp_condition_regulation_tbl_vperiod_vehcl_type_idx
  ON temp_condition_regulation_tbl
  USING btree
  (vperiod, vehcl_type);
 
CREATE INDEX org_r_accesscrid_idx
  ON org_r
  USING btree
  (accesscrid);
 
CREATE INDEX org_r_onewaycrid_idx
  ON org_r
  USING btree
  (onewaycrid);
 
CREATE INDEX org_cond_crid_idx
  ON org_cond
  USING btree
  (crid);
 
CREATE INDEX org_cr_crid_idx
  ON org_cr
  USING btree
  (crid);
 
CREATE INDEX org_poi_the_geom_idx
  ON org_poi
  USING gist
  (the_geom);


CREATE INDEX temp_highway_building_json_name_poi_id_idx
ON temp_highway_building_json_name
USING btree
(poi_id); 

-----------------------------------------------------
CREATE INDEX mid_temp_hwy_exit_enter_poi_name_ni_node_id_idx
  ON mid_temp_hwy_exit_enter_poi_name_ni
  USING btree
  (node_id);
-----------------------------------------------------
CREATE INDEX org_poi_relation_poi_id1_idx
  ON org_poi_relation
  USING btree
  (poi_id1);
-----------------------------------------------------
CREATE INDEX org_poi_relation_poi_id2_idx
  ON org_poi_relation
  USING btree
  (poi_id2);
  
CREATE INDEX temp_lane_exclusive_gid_idx
  on temp_lane_exclusive
  USING btree
  (gid);
  
CREATE INDEX temp_towardname_poi_tbl_nodeid_idx
  on temp_towardname_poi_tbl
  using btree
  (nodeid);
  
CREATE INDEX temp_not_roundabout_path_id_path_id_idx
  on temp_not_roundabout_path_id
  using btree
  (path_id);
  
CREATE INDEX temp_signpost_uc_sign_id_idx
  on temp_signpost_uc
  using btree
  (sign_id);

CREATE INDEX temp_signpost_uc_path_info_id_idx
  on temp_signpost_uc_path_info
  using btree
  (id);
  
CREATE INDEX temp_roundabout_id_id_name_idx
  on temp_roundabout_id
  using btree
  (id,name);
  
CREATE INDEX temp_sp_json_name_id_idx
  on temp_sp_json_name
  using btree
  (id);
  
CREATE INDEX  temp_general_sp_name_id_idx
  on  temp_general_sp_name
  using btree
  ((id::bigint));