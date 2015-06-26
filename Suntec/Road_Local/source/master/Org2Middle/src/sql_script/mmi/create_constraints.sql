CREATE INDEX org_state_region_geom_idx
	on org_state_region
	using gist
	(the_geom);
CREATE INDEX org_state_district_geom_idx
	on org_district_region
	using gist
	(the_geom);
CREATE INDEX org_area_id_idx
	on org_area
	using btree
	(id);
CREATE INDEX org_state_islands_region_geom_idx
	on org_state_islands_region
	using gist
	(the_geom);
CREATE INDEX temp_admin_state_difference_district_geom_idx
	on temp_admin_state_difference_district
	using gist
	(the_geom);

	
CREATE INDEX temp_link_regulation_forbid_traffic_link_id_idx
  ON temp_link_regulation_forbid_traffic
  USING btree
  (link_id);

CREATE INDEX temp_link_regulation_permit_traffic_link_id_idx
  ON temp_link_regulation_permit_traffic
  USING btree
  (link_id);

CREATE INDEX temp_link_regulation_traffic_link_id_idx
  ON temp_link_regulation_traffic
  USING btree
  (link_id);

-----------------------------------------------------
CREATE INDEX org_city_nw_gc_polyline_id_idx
  ON org_city_nw_gc_polyline
  USING btree
  (id);
CREATE INDEX org_city_nw_gc_polyline_id_bigint_idx
  ON org_city_nw_gc_polyline
  USING btree
  (cast(id as bigint));  
CREATE INDEX org_city_nw_gc_extended_polyline_id_idx
  ON org_city_nw_gc_extended_polyline
  USING btree
  (id);  

----------------------------------------------------- 
CREATE INDEX org_city_jc_point_id_feattyp_jncttyp_idx
  ON org_city_jc_point
  USING btree
  (id, feattyp, jncttyp);
  
-----------------------------------------------------  
CREATE INDEX temp_link_name_link_id_idx
  ON temp_link_name
  USING btree
  (link_id);  
CREATE INDEX temp_link_shield_link_id_idx
  ON temp_link_shield
  USING btree
  (link_id);  

-----------------------------------------------------   
CREATE INDEX org_toll_booth_point_fm_edge_to_edge_idx
  ON org_toll_booth_point
  USING btree
  (fm_edge, to_edge); 

-----------------------------------------------------   
CREATE INDEX org_bifurcation_junctionid_idx
  ON org_bifurcation
  USING btree
  (junctionid);  

-----------------------------------------------------   
 CREATE INDEX org_regional_data_id_bigint_idx
  ON org_regional_data
  USING btree
  ((id::bigint));

-----------------------------------------------------   

CREATE INDEX org_regional_data_id_idx
  ON org_regional_data
  USING btree
  (id);

-----------------------------------------------------   

CREATE INDEX org_regional_data_table_idx
  ON org_regional_data
  USING btree
  ("table");
  

-----------------------------------------------------   
 CREATE INDEX org_poi_point_uid_bigint_idx
  ON org_poi_point
  USING btree
  ((uid::bigint));


-----------------------------------------------------  
CREATE INDEX mid_temp_sapa_link_poi_id_idx
  ON mid_temp_sapa_link
  USING btree
  (poi_id);

CREATE INDEX temp_poi_name_poi_id_idx
  ON temp_poi_name
  USING btree
  (poi_id);

CREATE INDEX temp_poi_logmark_poi_id_idx
  ON temp_poi_logmark
  USING btree
  (uid);