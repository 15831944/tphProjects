
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
CREATE INDEX org_c_condtype_idx
  ON org_c
  USING btree
  (condtype);

-----------------------------------------------------   
CREATE INDEX org_r_id_idx
  ON org_r
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
  