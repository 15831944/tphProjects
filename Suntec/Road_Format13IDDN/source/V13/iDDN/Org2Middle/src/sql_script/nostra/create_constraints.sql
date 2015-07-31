--temp_condition_regulation_tbl
CREATE INDEX temp_condition_regulation_tbl_day_time_idx
  ON temp_condition_regulation_tbl
  USING btree
  (day, time);
---lane
CREATE INDEX lane_tbl_outlinkid_idx
  ON lane_tbl
  USING btree
  (outlinkid);
  
CREATE INDEX lane_tbl_inlinkid_idx
  ON lane_tbl
  USING btree
  (inlinkid);
  
CREATE INDEX temp_topo_link_end_node_idx
  ON temp_topo_link
  USING btree
  (end_node_id);
 
CREATE INDEX temp_topo_link_start_node_idx
  ON temp_topo_link
  USING btree
  (start_node_id);

CREATE INDEX temp_topo_link_routeid_idx
  ON temp_topo_link
  USING btree
  (routeid);

CREATE INDEX org_l_tran_oneway_idx
  ON org_l_tran
  USING btree
  (oneway);
  
CREATE INDEX org_l_tran_routeid_idx
  ON org_l_tran
  USING btree
  (routeid);
  
----------------------------------------------------
-- language
CREATE UNIQUE INDEX language_tbl_language_code_idx
  ON language_tbl
  USING btree
  (language_code);