
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