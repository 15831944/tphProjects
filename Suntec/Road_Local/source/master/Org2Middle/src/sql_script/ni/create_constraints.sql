
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

----------------------------------------------------- 
CREATE INDEX temp_order8_boundary_order8_geom_idx
  ON temp_order8_boundary
  USING gist
  (order8_geom);

----------------------------------------------------- 
CREATE INDEX temp_order8_boundary_ad_code_idx
  ON temp_order8_boundary
  USING btree
  (ad_code);
  
----------------------------------------------------- 
CREATE INDEX temp_inode_b_node_idx
  ON temp_inode
  USING btree
  (b_node);

----------------------------------------------------- 
CREATE INDEX temp_inode_link_id_idx
  ON temp_inode
  USING btree
  (link_id);

----------------------------------------------------- 
CREATE INDEX temp_guideinfo_boundary_out_adcd_idx
  ON temp_guideinfo_boundary
  USING btree
  (out_adcd);

----------------------------------------------------- 
CREATE INDEX temp_guideinfo_boundary_inlinkid_idx
  ON temp_guideinfo_boundary
  USING btree
  (inlinkid);

----------------------------------------------------- 
CREATE INDEX temp_guideinfo_boundary_nodeid_idx
  ON temp_guideinfo_boundary
  USING btree
  (nodeid);

----------------------------------------------------- 
CREATE INDEX temp_guideinfo_boundary_outlinkid_idx
  ON temp_guideinfo_boundary
  USING btree
  (outlinkid);

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
 

