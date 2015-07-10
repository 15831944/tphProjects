
CREATE INDEX temp_condition_regulation_tbl_day_shour_ehour_sdate_edate_cartype_idx
  ON temp_condition_regulation_tbl
  USING btree
  (day, shour, ehour, sdate, edate, cartype);

CREATE INDEX temp_org_one_way_day_shour_ehour_sdate_edate_cartype_idx
  ON temp_org_one_way
  USING btree
  (day, shour, ehour, sdate, edate, cartype);
 
CREATE INDEX temp_org_not_in_day_shour_ehour_sdate_edate_cartype_idx
  ON temp_org_not_in
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
  
CREATE INDEX temp_link_mapping_idx
  ON temp_link_mapping
  USING btree
  (meshcode,linkno);
  
CREATE INDEX temp_node_mapping_idx
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