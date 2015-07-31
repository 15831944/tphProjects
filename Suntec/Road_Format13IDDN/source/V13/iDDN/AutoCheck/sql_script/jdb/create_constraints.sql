--------------------------------------------------------------------------------------- 
-- create index 
--------------------------------------------------------------------------------------- 

CREATE INDEX road_link_objectid_idx
  ON road_link
  USING btree
  (objectid);


CREATE INDEX inf_turnreg_objectid_idx
  ON inf_turnreg
  USING btree
  (objectid);

CREATE INDEX lq_turnreg_inf_id_idx
  ON lq_turnreg
  USING btree
  (inf_id);
