
CREATE INDEX temp_condition_regulation_tbl_day_shour_ehour_sdate_edate_cartype_idx
  ON temp_condition_regulation_tbl
  USING btree
  (day, shour, ehour, sdate, edate, cartype);
  
CREATE INDEX temp_link_name_link_id_idx
  ON temp_link_name
  USING btree
  (link_id);
  
CREATE INDEX temp_link_shield_link_id_idx
  ON temp_link_shield
  USING btree
  (link_id);  
  
CREATE INDEX org_road_meshcode_linkno_idx
  ON org_road
  USING btree
  (meshcode,linkno);

CREATE INDEX org_road_elcode_idx
  ON org_road
  USING btree
  (elcode);
  
CREATE INDEX org_underpass_meshcode_linkno_idx
  ON org_underpass
  USING btree
  (meshcode,linkno);  
  
CREATE INDEX org_tunnelname_meshcode_roadno_idx
  ON org_tunnelname
  USING btree
  (meshcode,roadno);  
  
CREATE INDEX org_divider_meshcode_linkno_idx
  ON org_divider
  USING btree
  (meshcode,linkno);  
  
CREATE INDEX org_lanenum_meshcode_roadno_idx
  ON org_lanenum
  USING btree
  (meshcode,roadno);  
  
CREATE INDEX org_speed_meshcode_roadno_idx
  ON org_speed
  USING btree
  (meshcode,roadno);  
  
CREATE INDEX temp_link_mapping_meshcode_linkno_idx
  ON temp_link_mapping
  USING btree
  (meshcode,linkno);
  
CREATE INDEX temp_node_mapping_meshcode_nodeno_idx
  ON temp_node_mapping
  USING btree
  (meshcode,nodeno);
  
CREATE INDEX org_node_connect_meshcode_nodeid_idx
  ON org_node_connect
  USING btree
  (meshcode_self,nodeid_self);

CREATE INDEX org_road_oneway_idx
  ON org_road
  USING btree
  (oneway);

CREATE INDEX org_one_way_meshcode_linkno_idx
  ON "org_one-way"
  USING btree
  (meshcode, linkno);

CREATE INDEX org_not_in_meshcode_fromlinkno_idx
  ON "org_not-in"
  USING btree
  (meshcode, fromlinkno);

CREATE INDEX org_not_in_meshcode_tolinkno_idx
  ON "org_not-in"
  USING btree
  (meshcode, tolinkno);

CREATE INDEX org_not_in_meshcode_snodeno_idx
  ON "org_not-in"
  USING btree
  (meshcode, snodeno);
 
CREATE INDEX org_not_in_meshcode_tnodeno_idx
  ON "org_not-in"
  USING btree
  (meshcode, tnodeno);
  
CREATE INDEX org_not_in_meshcode_enodeno_idx
  ON "org_not-in"
  USING btree
  (meshcode, enodeno);

CREATE INDEX temp_hwlane_tbl_meshcode_idx
  ON temp_hwlane_tbl
  USING btree
  (meshcode);
  
CREATE INDEX temp_hwlane_tbl_inlinkid_idx
  ON temp_hwlane_tbl
  USING btree
  (inlinkid);

CREATE INDEX temp_hwlane_tbl_outlinkid_idx
  ON temp_hwlane_tbl
  USING btree
  (outlinkid);
  
CREATE INDEX temp_signpost_id_idx
  ON temp_signpost
  USING btree
  (id);
  
CREATE INDEX temp_signpost_inlink_idx
  ON temp_signpost
  USING btree
  (inlink); 
   
CREATE INDEX temp_signpost_uc_path_after_deal_with_dummy_link_id_idx
  ON temp_signpost_uc_path_after_deal_with_dummy_link
  USING btree
  (id);  
  
CREATE INDEX temp_sp_name_id_idx
  ON temp_sp_name
  USING btree
  (id);
  
  
CREATE INDEX temp_crossname_id_idx
  ON temp_crossname
  USING btree
  (id);
   
CREATE INDEX temp_intersection_name_id_idx
  ON temp_intersection_name
  USING btree
  (id); 
  
CREATE INDEX temp_poi_logmark_poi_id_idx
  ON temp_poi_logmark
  USING btree
  (poi_id);

CREATE INDEX temp_poi_name_poi_id_idx
  ON temp_poi_name
  USING btree
  (poi_id);
  
CREATE INDEX temp_lane_passlink_meshcode_tnodeno_snodeno_enodeno_ysnodeno_yenodeno_idx
 ON temp_lane_passlink
 USING btree
 (meshcode, tnodeno, snodeno, enodeno, ysnodeno, yenodeno);
 
CREATE INDEX temp_lane_lane_arrow_info_meshcode_tnodeno_snodeno_enodeno_idx
 ON temp_lane_lane_arrow_info
 USING btree
 (meshcode, tnodeno, snodeno, enodeno);
 
CREATE INDEX temp_lane_lane_arrow_info_meshcode_tnodeno_snodeno_enodeno_ysnodeno_yenodeno_idx
 ON temp_lane_lane_arrow_info
 USING btree
 (meshcode, tnodeno, snodeno, enodeno, ysnodeno, yenodeno);
 
 CREATE INDEX temp_lane_numlr_meshcode_tnodeno_snodeno_enodeno_idx
 ON temp_lane_numlr
 USING btree
 (meshcode, tnodeno, snodeno, enodeno);
 
-------------------------------------------------------------------------------
--
CREATE UNIQUE INDEX mid_hwy_org_facility_node_path_id_facility_id_node_id_idx
  ON mid_hwy_org_facility_node
  USING btree
  (path_id, facility_id, node_id);
  
CREATE INDEX temp_poi_category_mapping_org_id1_idx
  ON temp_poi_category_mapping
  USING btree
  (org_id1);

CREATE INDEX org_fastlane_info_meshcode_linkno_idx
  ON org_fastlane_info
  USING btree
  (meshcode, linkno); 

CREATE INDEX temp_zenrin_dummy_new_inlink_node_signpost_uc_id_idx
  ON temp_zenrin_dummy_new_inlink_node_signpost_uc
  USING btree
  (id);
  
CREATE INDEX temp_zenrin_dummy_new_inlink_node_crossname_id_idx
  ON temp_zenrin_dummy_new_inlink_node_crossname
  USING btree
  (id);
 
 CREATE INDEX temp_crossname_path_after_delete_dummy_link_id_idx
  ON temp_crossname_path_after_delete_dummy_link
  USING btree
  (id);
  
CREATE INDEX temp_link_walked_link_id_idx
ON temp_link_walked
USING btree
(link_id);  