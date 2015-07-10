
--------------------------------------------------------------------------------------------------------
CREATE TABLE mid_temp_force_guide_tbl
(
  gid serial primary key,
  nodeid bigint,
  inlinkid bigint,
  outlinkid bigint,
  passlid character varying(1024),
  passlink_cnt smallint
); SELECT AddGeometryColumn('','mid_temp_force_guide_tbl','node_geom','4326','POINT',2); SELECT AddGeometryColumn('','mid_temp_force_guide_tbl','inlink_geom','4326','LINESTRING',2); SELECT AddGeometryColumn('','mid_temp_force_guide_tbl','outlink_geom','4326','LINESTRING',2);