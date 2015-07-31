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
