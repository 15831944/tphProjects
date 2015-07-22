CREATE TABLE rdb_cond_speed
(
  gid serial NOT NULL PRIMARY KEY, 
  link_id bigint NOT NULL,
  link_id_t bigint NOT NULL,
  pos_cond_speed smallint NOT NULL,
  neg_cond_speed smallint NOT NULL,
  pos_cond_speed_source smallint NOT NULL,
  neg_cond_speed_source smallint NOT NULL,
  unit smallint NOT NULL
);

CREATE TABLE rdb_cond_speed_client
(
  link_id integer NOT NULL,
  link_id_t integer NOT NULL,
  pos_multi_speed_attr smallint NOT NULL,
  neg_multi_speed_attr smallint NOT NULL,
  CONSTRAINT rdb_cond_speed_client_pkey PRIMARY KEY (link_id_t, link_id)
);

CREATE TABLE rdb_guideinfo_lane
(
  guideinfo_id serial NOT NULL,
  in_link_id bigint NOT NULL,
  in_link_id_t integer NOT NULL,
  node_id bigint NOT NULL,
  node_id_t integer NOT NULL,
  out_link_id bigint,
  out_link_id_t integer,
  lane_num smallint NOT NULL,
  lane_info smallint NOT NULL,
  arrow_info smallint NOT NULL,
  lane_num_l smallint NOT NULL,
  lane_num_r smallint NOT NULL,
  passlink_count smallint NOT NULL,
  exclusive smallint default 0,
  CONSTRAINT rdb_guideinfo_lane_pkey PRIMARY KEY (guideinfo_id)
);

CREATE TABLE rdb_guideinfo_lane_client
(
  guideinfo_id integer NOT NULL,
  in_link_id integer NOT NULL,
  in_link_id_t integer NOT NULL,
  node_id bigint NOT NULL,
  node_id_t integer NOT NULL,
  out_link_id integer,
  out_link_id_t integer,
  lane_num smallint NOT NULL,
  lane_info smallint NOT NULL,
  arrow_info smallint NOT NULL,
  passlink_count smallint NOT NULL,
  CONSTRAINT rdb_guideinfo_lane_client_lane_pkey PRIMARY KEY (guideinfo_id)
);

CREATE TABLE rdb_guideinfo_hook_turn_client
(
  guideinfo_id integer NOT NULL,
  in_link_id integer NOT NULL,
  in_link_id_t integer NOT NULL,
  node_id integer NOT NULL,
  node_id_t integer NOT NULL,
  out_link_id integer,
  out_link_id_t integer,
  passlink_count smallint NOT NULL,
  CONSTRAINT rdb_guideinfo_hook_turn_client_pkey PRIMARY KEY (guideinfo_id)
);
CREATE TABLE rdb_guideinfo_hook_turn
(
  guideinfo_id serial NOT NULL,
  in_link_id bigint NOT NULL,
  in_link_id_t integer NOT NULL,
  node_id bigint NOT NULL,
  node_id_t integer NOT NULL,
  out_link_id bigint,
  out_link_id_t integer,
  passlink_count smallint NOT NULL,
  CONSTRAINT rdb_guideinfo_hook_turn_pkey PRIMARY KEY (guideinfo_id)
);

CREATE TABLE rdb_guideinfo_pic_blob_bytea
(
  gid serial NOT NULL,
  image_id character varying,
  data bytea,
  point_list bytea,
  CONSTRAINT rdb_guideinfo_pic_blob_bytea_pkey PRIMARY KEY (gid)
);

CREATE TABLE temp_guideinfo_pic_blob_id_mapping
(
  gid bigint,
  image_id character varying,
  CONSTRAINT temp_guideinfo_pic_blob_id_mapping_pkey PRIMARY KEY (image_id)
);

CREATE TABLE rdb_guideinfo_signpost
(
  gid serial NOT NULL,
  in_link_id bigint NOT NULL,
  in_link_id_t integer NOT NULL,
  node_id bigint NOT NULL,
  node_id_t integer NOT NULL,
  out_link_id bigint,
  out_link_id_t integer,
  sp_name character varying(1024),
  passlink_count smallint NOT NULL,
  is_pattern boolean default false,
  pattern_id integer NOT NULL,
  arrow_id integer,
  CONSTRAINT rdb_guideinfo_signpost_pkey PRIMARY KEY (gid)
);

CREATE TABLE rdb_guideinfo_spotguidepoint
(
  gid serial NOT NULL,
  in_link_id bigint NOT NULL,
  in_link_id_t integer NOT NULL,
  node_id bigint NOT NULL,
  node_id_t integer NOT NULL,
  out_link_id bigint NOT NULL,
  out_link_id_t integer NOT NULL,
  "type" smallint NOT NULL,
  passlink_count smallint NOT NULL,
  pattern_id integer NOT NULL,
  arrow_id integer NOT NULL,
  point_list bytea,
  is_exist_sar boolean DEFAULT false,
  CONSTRAINT rdb_guideinfo_spotguidepoint_pkey PRIMARY KEY (gid)
);

CREATE TABLE rdb_guideinfo_natural_guidence
(
  gid serial NOT NULL primary key,
  in_link_id bigint NOT NULL,
  in_link_id_t integer NOT NULL,
  node_id bigint NOT NULL,
  node_id_t integer NOT NULL,
  out_link_id bigint NOT NULL,
  out_link_id_t integer NOT NULL,
  passlink_count smallint NOT NULL,
  feat_position smallint,
  feat_importance smallint,
  preposition_code smallint,
  feat_name character varying(4096),
  condition_id smallint
);

CREATE TABLE rdb_guideinfo_condition
(
  gid serial primary key,
  condition_id smallint,
  start_date smallint,
  end_date smallint,
  week_flag smallint,
  start_time smallint,
  end_time smallint,
  exclude_date smallint
);

CREATE TABLE rdb_guideinfo_towardname
(
  gid serial NOT NULL,
  in_link_id bigint NOT NULL,
  in_link_id_t integer NOT NULL,
  node_id bigint NOT NULL,
  node_id_t integer NOT NULL,
  --out_link_id bigint NOT NULL,
  out_link_id bigint ,
  --out_link_id_t integer NOT NULL,
  out_link_id_t integer ,
  toward_name character varying(1024),
  name_attr smallint NOT NULL,
  name_kind smallint NOT NULL,
  passlink_count smallint NOT NULL,
  CONSTRAINT rdb_guideinfo_towardname_pkey PRIMARY KEY (gid)
);

CREATE TABLE rdb_guideinfo_safetyalert
(
  gid serial not null,
  lon integer NOT NULL,
  lat integer NOT NULL,
  link_id bigint NOT NULL,
  link_id_t integer NOT NULL,
  link_id_r bigint,
  type smallint NOT NULL,
  angle smallint,
  dir smallint,
  s_dis smallint NOT NULL,
  e_dis smallint NOT NULL,
  speed smallint,
  speed_unit smallint,
  CONSTRAINT rdb_guideinfo_safetyalert_pkey PRIMARY KEY (gid)
);

CREATE TABLE rdb_guideinfo_towardname_client
(
  gid serial NOT NULL,
  in_link_id integer NOT NULL,
  in_link_id_t integer NOT NULL,
  node_id integer NOT NULL,
  node_id_t integer NOT NULL,
 -- out_link_id integer NOT NULL,
 -- out_link_id_t integer NOT NULL,
  out_link_id integer ,
  out_link_id_t integer ,
  toward_name character varying(1024),
  multi_towardname_attr smallint NOT NULL,
  CONSTRAINT rdb_guideinfo_towardname_client_pkey PRIMARY KEY (gid)
);

CREATE TABLE rdb_link
(
  gid serial not null,
  link_id bigint NOT NULL,
  link_id_t integer NOT NULL,
  iso_country_code character(3),
  display_class smallint NOT NULL,
  start_node_id bigint NOT NULL,
  start_node_id_t integer NOT NULL,
  end_node_id bigint NOT NULL,
  end_node_id_t integer NOT NULL,
  road_type smallint NOT NULL,
  one_way smallint NOT NULL,
  function_code smallint NOT NULL,
  link_length integer NOT NULL,
  road_name character varying(16384),
   road_name_id  integer not null  default 0,
  road_number character varying(2048),
  lane_id integer NOT NULL DEFAULT (-1),
  toll smallint NOT NULL,
  regulation_flag boolean NOT NULL,
  tilt_flag boolean NOT NULL,
  speed_regulation_flag boolean NOT NULL,
  link_add_info_flag boolean NOT NULL,
  fazm smallint NOT NULL,
  tazm smallint NOT NULL,
  fnlink bigint,
  fnlink_t integer,
  tnlink bigint,
  tnlink_t integer,
  link_type integer NOT NULL,
  extend_flag smallint,
  fazm_path smallint NOT NULL,
  tazm_path smallint NOT NULL, 
  shield_flag boolean not null,
  hwy_flag    smallint NOT NULL DEFAULT 0,
  bypass_flag smallint NOT NULL DEFAULT 0,
  matching_flag smallint NOT NULL DEFAULT 0,
  highcost_flag smallint NOT NULL DEFAULT 0,
  CONSTRAINT rdb_link_pkey PRIMARY KEY (gid)
); SELECT AddGeometryColumn('','rdb_link','the_geom','4326','LINESTRING',2);SELECT AddGeometryColumn('','rdb_link','the_geom_900913','3857','LINESTRING',2);
CREATE TABLE rdb_name
(
  name_id integer NOT NULL,
  name    character varying(8192)
);

CREATE TABLE rdb_link_abs
(
  link_id bigint primary key,
  max_level smallint DEFAULT 0,
  abs_link_id integer DEFAULT 0
);

CREATE TABLE rdb_link_add_info
(
  link_id bigint NOT NULL,
  link_id_t integer NOT NULL,
  struct_code smallint NOT NULL,
  shortcut_code smallint NOT NULL,
  parking_flag smallint NOT NULL,
  etc_lane_flag smallint NOT NULL,
  path_extra_info smallint NOT NULL,
  CONSTRAINT rdb_link_add_info_pkey PRIMARY KEY (link_id)
);

CREATE TABLE rdb_link_add_info2
(
  link_id bigint NOT NULL,
  link_id_t integer NOT NULL,
  link_add_info2 smallint
);

CREATE TABLE rdb_link_add_info_client
(
  link_id integer NOT NULL,
  link_id_t integer NOT NULL,
  multi_link_add_info smallint NOT NULL,
  CONSTRAINT rdb_link_add_info_client_pkey PRIMARY KEY (link_id_t, link_id)
);

CREATE TABLE rdb_link_client
(
	gid serial not null PRIMARY KEY,
	link_id integer NOT NULL,
	link_id_t integer NOT NULL,
	start_node_id integer not null,
	start_node_id_t integer NOT NULL,
	end_node_id integer NOT NULL, 
	end_node_id_t integer NOT NULL,
	link_type smallint NOT NULL, 
	link_length integer NOT NULL, 
	road_name character varying(16384),
	lane_id integer NOT NULL, 
	common_main_link_attri smallint,
	multi_link_attri smallint,
	fnlink integer,
	fnlink_t integer,
	tnlink integer,
	tnlink_t integer,
	extend_flag smallint,
	bypass_flag smallint NOT NULL DEFAULT 0,
	matching_flag smallint NOT NULL DEFAULT 0,
	highcost_flag smallint NOT NULL DEFAULT 0,
	fazm_path smallint NOT NULL,
	tazm_path smallint NOT NULL, 
	shield_flag boolean not null,
	geom_blob bytea
);SELECT AddGeometryColumn('public', 'rdb_link_client', 'the_geom', '4326', 'LINESTRING', 2);

CREATE TABLE rdb_link_cond_regulation
(
  gid serial NOT NULL,
  regulation_id integer NOT NULL,
  car_type smallint NOT NULL DEFAULT 0,
  start_date integer NOT NULL DEFAULT (-1),
  end_date integer NOT NULL DEFAULT (-1),
  month_flag smallint NOT NULL DEFAULT 0,
  day_flag integer NOT NULL DEFAULT 0,
  week_flag smallint NOT NULL DEFAULT 0,
  start_time smallint NOT NULL,
  end_time smallint NOT NULL,
  exclude_date smallint,
  CONSTRAINT rdb_link_cond_regulation_pkey PRIMARY KEY (gid)
);

CREATE TABLE rdb_link_regulation
(
  record_no serial not null,
  regulation_id integer NOT NULL,
  regulation_type smallint NOT NULL,
  is_seasonal boolean NOT NULL,
  first_link_id bigint NOT NULL,
  first_link_id_t integer NOT NULL,
  last_link_id bigint NOT NULL,
  last_link_dir smallint NOT NULL,
  last_link_id_t integer NOT NULL,
  link_num smallint NOT NULL,
  key_string character varying(1024) NOT NULL,
  CONSTRAINT rdb_link_regulation_pkey PRIMARY KEY (record_no)
);

CREATE TABLE temp_link_regulation
(
  new_regulation_id integer NOT NULL,
  old_regulation_id integer NOT NULL,
  condtype smallint,
  link_id bigint NOT NULL,
  link_dir smallint NOT NULL,
  tile_id bigint NOT NULL,
  seq_num integer
);

CREATE TABLE rdb_linklane_info
(
  lane_id integer NOT NULL,
  disobey_flag boolean NOT NULL,
  lane_dir smallint NOT NULL,
  lane_up_down_distingush boolean NOT NULL,
  ops_lane_number smallint NOT NULL,
  ops_width smallint NOT NULL,
  neg_lane_number smallint NOT NULL,
  neg_width smallint NOT NULL,
  CONSTRAINT rdb_linklane_info_pkey PRIMARY KEY (lane_id)
);

CREATE TABLE rdb_linklane_info_client
(
  lane_id integer NOT NULL,
  multi_lane_attribute_1 smallint NOT NULL,
  multi_lane_attribute_2 smallint NOT NULL,
  CONSTRAINT rdb_linklane_info_client_pkey PRIMARY KEY (lane_id)
);

CREATE TABLE rdb_name_dictionary
(
  name_id serial primary key NOT NULL,
  name character varying(512) 
);

CREATE TABLE rdb_node
(
  gid serial not null,
  node_id bigint NOT NULL,
  node_id_t integer NOT NULL,
  extend_flag integer NOT NULL,
  link_num smallint NOT NULL,
  branches bigint[] NOT NULL,
  CONSTRAINT rdb_node_pkey PRIMARY KEY (gid)
); SELECT AddGeometryColumn('','rdb_node','the_geom','4326','POINT',2);SELECT AddGeometryColumn('','rdb_node','the_geom_900913','3857','POINT',2);

CREATE TABLE rdb_node_height
(
  gid serial not null,
  node_id bigint NOT NULL,
  height smallint NOT NULL
);

CREATE TABLE rdb_node_client
(
  node_id integer NOT NULL,
  node_id_t integer NOT NULL,
  extend_flag integer NOT NULL,
  branches character varying,
  geom_blob bytea NOT NULL,
  CONSTRAINT rdb_node_client_pkey PRIMARY KEY (node_id_t, node_id)
);SELECT AddGeometryColumn('','rdb_node_client','the_geom','4326','POINT',2);

CREATE TABLE rdb_tile_manage
(
  tile_id integer NOT NULL,
  min_node_id integer NOT NULL,
  max_node_id integer NOT NULL,
  CONSTRAINT rdb_tile_manage_pkey PRIMARY KEY (tile_id)
);

CREATE TABLE rdb_tile_node
(
  tile_node_id bigint NOT NULL,
  old_node_id bigint NOT NULL,
  tile_id integer NOT NULL,
  seq_num integer not null, 
  CONSTRAINT rdb_tile_node_pkey PRIMARY KEY (old_node_id)
);

CREATE TABLE rdb_tile_link
(
  tile_link_id bigint NOT NULL,
  old_link_id bigint NOT NULL,
  tile_id integer NOT NULL,
  seq_num integer not null
);

CREATE TABLE rdb_line_name
(
  record_id serial NOT NULL,
  kind_code integer NOT NULL,
  name_type smallint,
  name_prior smallint,
  low_level smallint,
  high_level smallint,
  name_id integer,
  CONSTRAINT rdb_line_name_pkey PRIMARY KEY (record_id)
);SELECT AddGeometryColumn('','rdb_line_name','the_geom','4326','LINESTRING',2);SELECT AddGeometryColumn('','rdb_line_name','the_geom_900913','3857','LINESTRING',2);


CREATE TABLE rdb_poi_name
(
  record_id serial NOT NULL,
  kind_code integer NOT NULL,
  name_type smallint,
  name_prior smallint,
  low_level smallint,
  high_level smallint,
  name_id integer,
  CONSTRAINT rdb_poi_name_pkey PRIMARY KEY (record_id)
); SELECT AddGeometryColumn('','rdb_poi_name','the_geom','4326','POINT',2);

CREATE TABLE rdb_guideinfo_crossname
(
  guideinfo_id serial NOT NULL,
  in_link_id bigint,
  in_link_id_t integer,
  node_id bigint NOT NULL,
  node_id_t integer NOT NULL,
  out_link_id bigint,
  out_link_id_t integer,
  cross_name character varying(1024),
  CONSTRAINT rdb_guideinfo_crossname_pkey PRIMARY KEY (guideinfo_id)
);

CREATE TABLE rdb_guideinfo_building_structure
(
  guideinfo_id serial NOT NULL PRIMARY KEY,
  in_link_id bigint,
  in_link_id_t integer,
  node_id bigint NOT NULL,
  node_id_t integer NOT NULL,
  out_link_id bigint,
  out_link_id_t integer,
  type_code integer NOT NULL,
  type_code_priority integer NOT NULL default 0,
  centroid_lontitude integer NOT NULL,
  centroid_lantitude integer NOT NULL,
  building_name character varying(2048)
);

CREATE TABLE rdb_guideinfo_caution
(
  guideinfo_id serial NOT NULL PRIMARY KEY,
  in_link_id bigint NOT NULL,
  in_link_id_t integer NOT NULL,
  node_id bigint NOT NULL,
  node_id_t integer NOT NULL,
  out_link_id bigint NOT NULL,
  out_link_id_t integer NOT NULL,
  passlink_count smallint NOT NULL,
  data_kind smallint NOT NULL,
  voice_id integer DEFAULT 0,
  strTTS varchar(65535),
  image_id integer DEFAULT 0
);

CREATE TABLE rdb_guideinfo_forceguide
(
  guideinfo_id serial NOT NULL PRIMARY KEY,
  in_link_id bigint NOT NULL,
  in_link_id_t integer NOT NULL,
  node_id bigint NOT NULL,
  node_id_t integer NOT NULL,
  out_link_id bigint NOT NULL,
  out_link_id_t integer NOT NULL,
  passlink_count smallint NOT NULL,
  guide_type smallint NOT NULL,
  position_type smallint NOT NULL
);

CREATE TABLE rdb_guideinfo_road_structure
(
  guideinfo_id serial NOT NULL PRIMARY KEY,
  in_link_id bigint NOT NULL,
  in_link_id_t integer,
  node_id bigint NOT NULL,
  node_id_t integer NOT NULL,
  out_link_id bigint NOT NULL,
  out_link_id_t integer NOT NULL,
  type_code smallint NOT NULL,
  structure_name character varying(512)
);

CREATE TABLE rdb_highway_facility_data
(
  gid serial NOT NULL PRIMARY KEY,
  facility_id integer,
  node_id bigint NOT NULL,
  node_id_t integer NOT NULL,
  separation smallint,
  name_id integer,
  junction smallint,
  pa smallint,
  sa smallint,
  ic smallint,
  ramp smallint,
  smart_ic smallint,
  tollgate smallint,
  dummy_tollgate smallint,
  tollgate_type smallint,
  service_info_flag smallint,
  in_out_type smallint  -- in out type
);

CREATE TABLE rdb_guideinfo_signpost_uc
(
  gid serial NOT NULL,
  in_link_id bigint NOT NULL,
  in_link_id_t integer NOT NULL,
  node_id bigint NOT NULL,
  node_id_t integer NOT NULL,
  out_link_id bigint NOT NULL,
  out_link_id_t integer NOT NULL,
  passlink_count smallint NOT NULL,
  sp_name      character varying(8192),
  route_no1    character varying(1024),
  route_no2    character varying(1024), 
  route_no3    character varying(1024), 
  route_no4    character varying(1024), 
  exit_no      character varying(1024),
  CONSTRAINT rdb_guideinfo_signpost_uc_pkey PRIMARY KEY (gid)
);

--------------------------------------------------------------------
-- client

CREATE TABLE rdb_link_regulation_client
(
  record_no integer NOT NULL,
  regulation_id integer NOT NULL,
  regulation_type smallint NOT NULL,
  is_seasonal boolean NOT NULL,
  first_link_id integer NOT NULL,
  first_link_id_t integer NOT NULL,
  last_link_id integer NOT NULL,
  last_link_dir smallint NOT NULL,
  last_link_id_t integer NOT NULL,
  link_num smallint NOT NULL,
  key_string character varying(1024) NOT NULL,
  CONSTRAINT rdb_link_regulation_client_pkey PRIMARY KEY (record_no)
);

CREATE TABLE rdb_guideinfo_crossname_client
(
  guideinfo_id serial NOT NULL,
  in_link_id integer,
  in_link_id_t integer,
  node_id integer NOT NULL,
  node_id_t integer NOT NULL,
  out_link_id integer,
  out_link_id_t integer,
  cross_name character varying(1024),
  CONSTRAINT rdb_guideinfo_crossname_client_pkey PRIMARY KEY (guideinfo_id)
);

CREATE TABLE rdb_guideinfo_signpost_client
(
  guideinfo_id serial NOT NULL,
  in_link_id integer not null,
  in_link_id_t integer not null,
  node_id integer not null,
  node_id_t integer not null,
  out_link_id integer,
  out_link_id_t integer,
  sp_name character varying(1024),
  passlink_count smallint,
  is_pattern boolean default false,
  pattern_id integer,
  arrow_id integer,
  CONSTRAINT rdb_guideinfo_signpost_client_pkey PRIMARY KEY (guideinfo_id)
);

CREATE TABLE rdb_guideinfo_spotguidepoint_client
(
  gid serial NOT NULL,
  in_link_id integer NOT NULL,
  in_link_id_t integer NOT NULL,
  node_id integer NOT NULL,
  node_id_t integer NOT NULL,
  out_link_id integer NOT NULL,
  out_link_id_t integer NOT NULL,
  "type" smallint NOT NULL,
  passlink_count smallint NOT NULL,
  pattern_id integer NOT NULL,
  arrow_id integer NOT NULL,
  point_list bytea,
  CONSTRAINT rdb_guideinfo_spotguidepoint_client_pkey PRIMARY KEY (gid)
);

CREATE TABLE rdb_guideinfo_natural_guidence_client
(
  gid serial NOT NULL primary key,
  in_link_id integer NOT NULL,
  in_link_id_t integer NOT NULL,
  node_id integer NOT NULL,
  node_id_t integer NOT NULL,
  out_link_id integer NOT NULL,
  out_link_id_t integer NOT NULL,
  passlink_count smallint NOT NULL,
  feat_position smallint,
  feat_importance smallint,
  preposition_code smallint,
  feat_name character varying(4096),
  condition_id smallint
);

CREATE TABLE rdb_guideinfo_building_structure_client
(
  guideinfo_id serial NOT NULL PRIMARY KEY ,
  in_link_id integer,
  in_link_id_t integer,
  node_id integer NOT NULL,
  node_id_t integer NOT NULL,
  out_link_id integer,
  out_link_id_t integer,
  type_code integer NOT NULL,
  centroid_lontitude integer NOT NULL,
  centroid_lantitude integer NOT NULL,
  building_name character varying(2048)
);

CREATE TABLE rdb_guideinfo_caution_client
(
  guideinfo_id serial NOT NULL PRIMARY KEY,
  in_link_id integer NOT NULL,
  in_link_id_t integer NOT NULL,
  node_id integer NOT NULL,
  node_id_t integer NOT NULL,
  out_link_id integer NOT NULL,
  out_link_id_t integer NOT NULL,
  passlink_count smallint NOT NULL,
  data_kind smallint NOT NULL,
  voice_id integer DEFAULT -1,
  strTTS varchar(65535),
  image_id integer DEFAULT -1
);

CREATE TABLE rdb_guideinfo_forceguide_client
(
  guideinfo_id serial NOT NULL PRIMARY KEY,
  in_link_id integer NOT NULL,
  in_link_id_t integer NOT NULL,
  node_id integer NOT NULL,
  node_id_t integer NOT NULL,
  out_link_id integer NOT NULL,
  out_link_id_t integer NOT NULL,
  passlink_count smallint NOT NULL,
  guide_type smallint NOT NULL,
  position_type smallint NOT NULL
);

CREATE TABLE rdb_guideinfo_road_structure_client
(
  guideinfo_id serial NOT NULL PRIMARY KEY,
  in_link_id integer NOT NULL,
  in_link_id_t integer NOT NULL,
  node_id integer NOT NULL,
  node_id_t integer NOT NULL,
  out_link_id integer NOT NULL,
  out_link_id_t integer NOT NULL,
  type_code smallint NOT NULL,
  structure_name character varying(512)
);

CREATE TABLE rdb_highway_facility_data_client
(
  gid serial NOT NULL PRIMARY KEY,
  facility_id integer,
  node_id bigint NOT NULL,
  node_id_t integer NOT NULL,
  separation smallint,
  name_id integer,
  junction smallint,
  pa smallint,
  sa smallint,
  ic smallint,
  ramp smallint,
  smart_ic smallint,
  tollgate smallint,
  dummy_tollgate smallint,
  tollgate_type smallint,
  service_info_flag smallint,
  in_out_type smallint  -- in out type
);

create table temp_rdb_link_name
(
  old_link_id bigint not null primary key,
  road_name_id integer not null,
  road_number_id integer
);

CREATE TABLE rdb_region_layer_6
(
  gid serial not null,
  link_id bigint NOT NULL,
  link_id_t integer NOT NULL,
  display_class smallint NOT NULL,
  start_node_id bigint NOT NULL,
  end_node_id bigint NOT NULL,
  end_node_id_t integer NOT NULL,
  road_type smallint NOT NULL,
  one_way smallint NOT NULL,
  function_code smallint NOT NULL,
  link_length integer NOT NULL,
  road_name character varying(4096),
  road_number character varying(2048) NOT NULL DEFAULT ''::character varying,
  lane_id integer NOT NULL DEFAULT (-1),
  toll smallint NOT NULL,
  regulation_flag boolean NOT NULL,
  tilt_flag boolean NOT NULL,
  speed_regulation_flag boolean NOT NULL,
  link_add_info_flag boolean NOT NULL,
  fazm smallint NOT NULL,
  tazm smallint NOT NULL,
  fnlink bigint,
  fnlink_t integer,
  tnlink bigint,
  tnlink_t integer,
  link_type integer NOT NULL,
  extend_flag smallint,
  bypass_flag smallint,
  highcost_flag smallint,
  fazm_path smallint NOT NULL,
  tazm_path smallint NOT NULL, 
  CONSTRAINT rdb_region_pkey PRIMARY KEY (gid)
); SELECT AddGeometryColumn('','rdb_region_layer_6','the_geom','4326','LINESTRING',2);

CREATE TABLE rdb_guideinfo_signpost_uc_client
(
  gid serial NOT NULL,
  in_link_id integer NOT NULL,
  in_link_id_t integer NOT NULL,
  node_id integer NOT NULL,
  node_id_t integer NOT NULL,
  out_link_id integer NOT NULL,
  out_link_id_t integer NOT NULL,
  passlink_count smallint NOT NULL,
  sp_name      character varying(8192),
  route_no1    character varying(1024),
  route_no2    character varying(1024), 
  route_no3    character varying(1024), 
  route_no4    character varying(1024), 
  exit_no      character varying(1024),
  CONSTRAINT rdb_guideinfo_signpost_uc_client_pkey PRIMARY KEY (gid)
);

CREATE TABLE rdb_link_shield
(
  gid          serial NOT NULL primary key,
  link_id      bigint not null,
  link_id_t    integer NOT NULL,
  name_id   integer not null
);

CREATE TABLE rdb_link_shield_client
(
  gid          serial NOT NULL primary key,
  link_id      integer not null,
  link_id_t    integer NOT NULL,
  name_id   integer not null
);

CREATE TABLE rdb_admin_zone
(
  gid serial NOT NULL,
  ad_code integer,
  ad_order smallint,
  order0_id integer,
  order1_id integer,
  order2_id integer,
  order8_id integer,
  ad_name character varying(65536),
  time_zone smallint,
  summer_time_id smallint
);

CREATE TABLE rdb_admin_summer_time
(
  gid serial primary key,
  summer_time_id smallint,
  start_date smallint,
  start_weekday smallint,
  start_time smallint,
  end_date smallint,
  end_weekday smallint,
  end_time smallint
);

CREATE TABLE rdb_link_admin_zone
(
  link_id      bigint not null,
  ad_code      integer NOT NULL
);

CREATE TABLE rdb_tile_admin_zone
(
  gid		   serial not null primary key,
  tile_id      integer not null,
  ad_code      integer NOT NULL
); SELECT AddGeometryColumn('','rdb_tile_admin_zone','the_geom','4326','POLYGON',2);SELECT AddGeometryColumn('','rdb_tile_admin_zone','the_geom_900913','3857','POLYGON',2);

create table rdb_tile_out_of_admin_zone
as
(
	select a.tile_id, (a.tile_id >> 14) & 16383 as tile_x, a.tile_id & 16383 as tile_y
	from 
	(
		select distinct tile_id
		from
		(
			select distinct node_id_t as tile_id 
			from rdb_node
			union
			select distinct link_id_t as tile_id 
			from rdb_link
		)as t
	)as a
	left join (select distinct tile_id from rdb_tile_admin_zone)as b
	on a.tile_id = b.tile_id
	where b.tile_id is null
	order by a.tile_id
);

CREATE TABLE rdb_tile_admin_zone_client
(
  gid		   serial not null primary key,
  tile_id      integer not null,
  ad_code      integer NOT NULL,
  geom_blob    bytea
);

create table rdb_region_link_add_info_layer8_tbl
as
(
    select link_id, region_tile_id as link_id_t, struct_code, shortcut_code, etc_lane_flag, 0 as parking_flag, path_extra_info
    from temp_region_merge_links
    where not (struct_code = 0 and shortcut_code = 0 and etc_lane_flag = 0 and path_extra_info = 0)
);

create table rdb_region_link_add_info_layer6_tbl
as
(
    select link_id, region_tile_id as link_id_t, struct_code, shortcut_code, etc_lane_flag, 0 as parking_flag, path_extra_info
    from temp_region_merge_links
    where not (struct_code = 0 and shortcut_code = 0 and etc_lane_flag = 0 and path_extra_info = 0)
);

create table rdb_region_link_add_info_layer4_tbl
as
(
    select link_id, region_tile_id as link_id_t, struct_code, shortcut_code, etc_lane_flag, 0 as parking_flag, path_extra_info
    from temp_region_merge_links
    where not (struct_code = 0 and shortcut_code = 0 and etc_lane_flag = 0 and path_extra_info = 0)
);

create table rdb_region_cond_speed_layer8_tbl
as
(
    select 	link_id, region_tile_id as link_id_t, 
    		(case when pos_cond_speed is not null then pos_cond_speed else 0 end) as pos_cond_speed, 
    		(case when neg_cond_speed is not null then neg_cond_speed else 0 end) as neg_cond_speed, 
    		0::smallint as unit
    from temp_region_merge_links
    where not (pos_cond_speed is null and neg_cond_speed is null)
);

create table rdb_region_cond_speed_layer6_tbl
as
(
    select 	link_id, region_tile_id as link_id_t, 
    		(case when pos_cond_speed is not null then pos_cond_speed else 0 end) as pos_cond_speed, 
    		(case when neg_cond_speed is not null then neg_cond_speed else 0 end) as neg_cond_speed, 
    		0::smallint as unit
    from temp_region_merge_links
    where not (pos_cond_speed is null and neg_cond_speed is null)
);

create table rdb_region_cond_speed_layer4_tbl
as
(
    select 	link_id, region_tile_id as link_id_t, 
    		(case when pos_cond_speed is not null then pos_cond_speed else 0 end) as pos_cond_speed, 
    		(case when neg_cond_speed is not null then neg_cond_speed else 0 end) as neg_cond_speed, 
    		0::smallint as unit
    from temp_region_merge_links
    where not (pos_cond_speed is null and neg_cond_speed is null)
);

CREATE TABLE rdb_region_node_layer8_tbl
as
(
	select a.node_id, rdb_split_to_tileid(a.node_id) as node_id_t, a.link_num, a.branches, c.seqs, 
	b.node_boundary_flag, ((a.extend_flag & (1 << 9)) > 0) as signal_flag, null::bigint as upgrade_node_id, 
	a.the_geom,  
	(
		rdb_integer_2_octal_bytea(cast(round(st_x(a.the_geom) * 256 * 3600) as int)) ||
        	rdb_integer_2_octal_bytea(cast(round(st_Y(a.the_geom) * 256 * 3600) as int))
	) as geom_blob 
	from temp_region_merge_nodes as a
	left join temp_region_node_layerx_boundary_flag as b
	on a.node_id = b.node_id
	left join 
	(
		select node_id,array_agg(find_idx) as seqs 
		from 
		(
			select node_id, seq, find_idx from temp_region_node_with_branch_link_info
			order by node_id, seq
		) as u
		group by node_id
		
	) as c
	on a.node_id = c.node_id
);

CREATE TABLE rdb_region_node_layer6_tbl
as
(
	select a.node_id, rdb_split_to_tileid(a.node_id) as node_id_t, a.link_num, a.branches, c.seqs, 
	b.node_boundary_flag, ((a.extend_flag & (1 << 9)) > 0) as signal_flag, null::bigint as upgrade_node_id, 
	a.the_geom,  
	(
		rdb_integer_2_octal_bytea(cast(round(st_x(a.the_geom) * 256 * 3600) as int)) ||
        	rdb_integer_2_octal_bytea(cast(round(st_Y(a.the_geom) * 256 * 3600) as int))
	) as geom_blob 
	from temp_region_merge_nodes as a
	left join temp_region_node_layerx_boundary_flag as b
	on a.node_id = b.node_id
	left join 
	(
		select node_id,array_agg(find_idx) as seqs 
		from 
		(
			select node_id, seq, find_idx from temp_region_node_with_branch_link_info
			order by node_id, seq
		) as u
		group by node_id
		
	) as c
	on a.node_id = c.node_id
);

CREATE TABLE rdb_region_node_layer4_tbl
as
(
	select a.node_id, rdb_split_to_tileid(a.node_id) as node_id_t, a.link_num, a.branches, c.seqs, 
	b.node_boundary_flag, ((a.extend_flag & (1 << 9)) > 0) as signal_flag, q.layer6_node_id as upgrade_node_id,
	a.the_geom, 
	(
		rdb_integer_2_octal_bytea(cast(round(st_x(a.the_geom) * 256 * 3600) as int)) ||
        	rdb_integer_2_octal_bytea(cast(round(st_Y(a.the_geom) * 256 * 3600) as int))
	) as geom_blob 
	from temp_region_merge_nodes as a
	left join temp_region_node_layerx_boundary_flag as b
	on a.node_id = b.node_id
	left join 
	(
		select node_id,array_agg(find_idx) as seqs 
		from 
		(
			select node_id, seq, find_idx from temp_region_node_with_branch_link_info
			order by node_id, seq
		) as u
		group by node_id
	) as c
	on a.node_id = c.node_id
	left join
	(
		select m.region_node_id as layer4_node_id, n.region_node_id as layer6_node_id
		from rdb_region_layer4_node_mapping as m
		left join rdb_region_layer6_node_mapping as n
		on m.node_id_14= n.node_id_14
	) as q
	on a.node_id = q.layer4_node_id
);

-------------------------------------------------------------------------------------------
-- HWY SA/PA
-------------------------------------------------------------------------------------------
CREATE TABLE rdb_highway_sa_pa
(
  gid                   serial NOT NULL primary key,
  sapa_id               INTEGER NOT NULL,
  node_id               BIGINT  NOT NULL,
  node_id_t             integer NOT NULL,
  name_id               INTEGER,
  public_telephone      smallint,
  vending_machine       smallint,
  handicapped_telephone smallint,
  toilet                smallint,
  handicapped_toilet    smallint,
  gas_station           smallint,
  natural_gas           smallint,
  nap_rest_area         smallint,
  rest_area             smallint,
  nursing_room          smallint,
  dinning               smallint,
  repair                smallint,
  shop                  smallint,
  launderette           smallint,
  fax_service           smallint,
  coffee                smallint,
  post	                smallint,
  hwy_infor_Terminal    smallint,
  hwy_tot_springs       smallint,
  shower                smallint,
  image_id              INTEGER
);

-------------------------------------------------------------------------------------------
-- HWY SA/PA For Client
-------------------------------------------------------------------------------------------
CREATE TABLE rdb_highway_sa_pa_client
(
  gid                   serial NOT NULL primary key,
  sapa_id               INTEGER NOT NULL,
  node_id               INTEGER NOT NULL,
  node_id_t             integer NOT NULL,
  name_id               INTEGER,
  public_telephone      smallint,
  vending_machine       smallint,
  handicapped_telephone smallint,
  toilet                smallint,
  handicapped_toilet    smallint,
  gas_station           smallint,
  natural_gas           smallint,
  nap_rest_area         smallint,
  rest_area             smallint,
  nursing_room          smallint,
  dinning               smallint,
  repair                smallint,
  shop                  smallint,
  launderette           smallint,
  fax_service           smallint,
  coffee                smallint,
  post	                smallint,
  hwy_infor_Terminal    smallint,
  hwy_tot_springs       smallint,
  shower                smallint,
  image_id              INTEGER
);


-------------------------------------------------------------------------------------------
-- id fund (incremental updating)
-------------------------------------------------------------------------------------------
create table id_fund_link
(
	link_id			bigint,
	tile_id			integer,
	seq_num			integer,
	feature_string	character varying,
	feature_key		character varying(32),
	the_geom		geometry,
	version			character varying
);

create table id_fund_node
(
	node_id			bigint,
	tile_id			integer,
	seq_num			integer,
	feature_string	character varying,
	feature_key		character varying(32),
	the_geom		geometry,
	version			character varying
);

-------------------------------------------------------------------------------------------
-- VICS process (axf & jpn)
-------------------------------------------------------------------------------------------
create table temp_vics_org2rdb_prepare_axf 
as 
(
	select a.loc_code
		,a.dir,a.service_id
		,b.type
		,b.meshid as loc_code_mesh
		,d.road,
		case when e.flag = 't' and b.road_dir = 0 then 1
			 when e.flag = 't' and b.road_dir = 1 then 0
			 else b.road_dir
		end as link_dir,d.meshid,
		case when e.s_fraction > e.e_fraction then e.e_fraction 
		     else e.s_fraction
	    end as s_fraction,
		case when e.s_fraction > e.e_fraction then e.s_fraction 
		     else e.e_fraction
		end as e_fraction,
		case when e.s_fraction > e.e_fraction then e.e_point 
		     else e.s_point
	    end as s_point,
		case when e.s_fraction > e.e_fraction then e.s_point 
		     else e.e_point
		end as e_point	
		,e.target_link_id
		,e.flag
		from temp_tmc_chn_rdstmc a 
	left join 
		temp_tmc_chn_rdslinkinfo b
		on b.link >= a.link and b.link < (a.link + a.link_cnt) and b.meshid = a.meshid
	left join 
		mesh_mapping_tbl c
		on b.mesh = c.meshid_str
	left join
		temp_link_mapping d
		on d.road_id = b.road_id and d.meshid = c.globl_mesh_id
	left join 
		temp_link_org_rdb e
		on d.road = e.org_link_id and d.meshid = e.org_mesh_id
);
	
create table temp_vics_org2rdb_seq_axf
as
( 
	select loc_code_mesh,loc_code,dir,service_id,type,target_link_id,
		link_dir,
		case when link_dir = 1 then count - seq 
			 else seq
		end as seq,
		s_fraction,e_fraction,s_point,e_point 
	from (
		select loc_code_mesh,loc_code,dir,
			unnest(service_id_array) as service_id,
			unnest(type_array) as type,
			target_link_id,
			unnest(link_dir_array) as link_dir,
			unnest(s_fraction_array) as s_fraction,
			unnest(e_fraction_array) as e_fraction,
			unnest(s_point_array) as s_point,
			unnest(e_point_array) as e_point,		
			generate_series(1,count) as seq,count 
		from (
			select loc_code_mesh,loc_code,dir,
				array_agg(service_id) as service_id_array,
				array_agg(type) as type_array,
				target_link_id,
				array_agg(link_dir) as link_dir_array,
				array_agg(s_fraction) as s_fraction_array,
				array_agg(e_fraction) as e_fraction_array,
				array_agg(s_point) as s_point_array,
				array_agg(e_point) as e_point_array,			
				count(*) as count 
			from (
				select * from (
					select loc_code,dir,service_id,type,loc_code_mesh,
						s_fraction,
						e_fraction,
						s_point,
						e_point,
						target_link_id,link_dir,flag
					from temp_vics_org2rdb_prepare_axf
				) org 
				order by loc_code_mesh,loc_code,dir,target_link_id,s_fraction
			) a
			group by loc_code_mesh,loc_code,dir,target_link_id
		) b
	) c
);
	

CREATE TABLE rdb_vics_org2rdb_axf
(
  gid serial,
  loc_code integer,
  dir smallint,
  "type" smallint,
  loc_code_mesh integer,
  service_id character varying(10),
  s_fraction double precision,
  e_fraction double precision,
  s_point smallint,
  e_point smallint,
  target_link_id bigint,
  link_dir integer,
  group_id smallint,
  seq smallint
);

create table temp_vics_org2rdb_prepare_jpn 
as 
( 
	select  a.link_id as linkid,
		a.sequence as seq,
		case when c.flag  is true and a.linkdir_c = 1 then 2
		     when c.flag  is true and a.linkdir_c = 2 then 1
		     else a.linkdir_c
		end as linkdir,
		b.vics_meshcode as meshcode,
		b.vicsclass_c as vicsclass,
		b.vics_link_id as vicsid,
		case when c.s_fraction > c.e_fraction then c.e_fraction 
		     else c.s_fraction
	    end as s_fraction,
		case when c.s_fraction > c.e_fraction then c.s_fraction 
		     else c.e_fraction
		end as e_fraction,
		case when c.s_fraction > c.e_fraction then c.e_point 
		     else c.s_point
	    end as s_point,
		case when c.s_fraction > c.e_fraction then c.s_point 
		     else c.e_point
		end as e_point,		
		target_link_id,flag 	
	from lq_vics a
	left join inf_vics b
	on a.inf_id = b.objectid
	left join temp_link_org_rdb c
	on a.link_id = c.org_link_id  
);

create table temp_ipc_vics_org2rdb_prepare_jpn 
as 
(
	select  a.link_id as org_link_id,
		a.sequence as seq,
		case when c.flag  is true and a.linkdir_c = 1 then 2
		     when c.flag  is true and a.linkdir_c = 2 then 1
		     else a.linkdir_c
		end as linkdir,
		b.vics_meshcode as meshcode,
		b.vicsclass_c as vicsclass,
		b.vics_link_id as vicsid,
		case when c.s_fraction > c.e_fraction then c.e_fraction 
		     else c.s_fraction
	    end as s_fraction,
		case when c.s_fraction > c.e_fraction then c.s_fraction 
		     else c.e_fraction
		end as e_fraction,
		case when c.s_fraction > c.e_fraction then c.e_point 
		     else c.s_point
	    end as s_point,
		case when c.s_fraction > c.e_fraction then c.s_point 
		     else c.e_point
		end as e_point,	
		target_link_id,flag 	
	from lq_ipcvics a
	left join inf_ipcvics b
	on a.inf_id = b.objectid
	left join temp_link_org_rdb c
	on a.link_id = c.org_link_id 		
);

create table temp_vics_org2rdb_seq_jpn 
as 
(
	select meshcode,vicsclass,vicsid,target_link_id,
		linkdir,
		case when linkdir = 1 then count - seq 
			 else seq
		end as seq,
		s_fraction,e_fraction,s_point,e_point 
	from (
		select meshcode,vicsclass,vicsid,
			target_link_id,
			unnest(linkdir_array) as linkdir,
			unnest(s_fraction_array) as s_fraction,
			unnest(e_fraction_array) as e_fraction,
			unnest(s_point_array) as s_point,
			unnest(e_point_array) as e_point,		
			generate_series(1,count) as seq,count 
		from (
			select meshcode,vicsclass,vicsid,
				target_link_id,
				array_agg(linkdir) as linkdir_array,
				array_agg(s_fraction) as s_fraction_array,
				array_agg(e_fraction) as e_fraction_array,
				array_agg(s_point) as s_point_array,
				array_agg(e_point) as e_point_array,			
				count(*) as count 
			from (
				select * from (
					select meshcode,vicsclass,vicsid,seq,
						s_fraction,
						e_fraction,
						s_point,
						e_point,
						target_link_id,linkdir,flag
					from temp_vics_org2rdb_prepare_jpn
				) org 
				order by meshcode,vicsclass,vicsid,target_link_id,s_fraction
			) a
			group by meshcode,vicsclass,vicsid,target_link_id
		) b
	) c
);

create table temp_ipc_vics_org2rdb_seq_jpn 
as 
(
	select meshcode,vicsclass,vicsid,target_link_id,
		linkdir,
		case when linkdir = 1 then count - seq 
			 else seq
		end as seq,
		s_fraction,e_fraction,s_point,e_point 
	from (
		select meshcode,vicsclass,vicsid,
			target_link_id,
			unnest(linkdir_array) as linkdir,
			unnest(s_fraction_array) as s_fraction,
			unnest(e_fraction_array) as e_fraction,
			unnest(s_point_array) as s_point,
			unnest(e_point_array) as e_point,		
			generate_series(1,count) as seq,count 
		from (
			select meshcode,vicsclass,vicsid,
				target_link_id,
				array_agg(linkdir) as linkdir_array,
				array_agg(s_fraction) as s_fraction_array,
				array_agg(e_fraction) as e_fraction_array,
				array_agg(s_point) as s_point_array,
				array_agg(e_point) as e_point_array,			
				count(*) as count 
			from (
				select * from (
					select meshcode,vicsclass,vicsid,seq,
						s_fraction,
						e_fraction,
						s_point,
						e_point,
						target_link_id,linkdir,flag
					from temp_ipc_vics_org2rdb_prepare_jpn
				) org 
				order by meshcode,vicsclass,vicsid,target_link_id,s_fraction
			) a
			group by meshcode,vicsclass,vicsid,target_link_id
		) b
	) c
);

CREATE TABLE rdb_vics_org2rdb_jpn
(
  gid serial,
  meshcode integer,
  vicsclass integer,
  vicsid integer,
  linkdir integer,
  target_link_id bigint,
  group_id smallint,
  seq smallint,
  s_fraction double precision,
  e_fraction double precision,
  s_point smallint,
  e_point smallint
);

CREATE TABLE rdb_ipc_vics_org2rdb_jpn
(
  gid serial,
  meshcode integer,
  vicsclass integer,
  vicsid integer,
  linkdir integer,
  target_link_id bigint,
  group_id smallint,
  seq smallint,
  s_fraction double precision,
  e_fraction double precision,
  s_point smallint,
  e_point smallint
);

CREATE TABLE rdb_pic_tile_info
as
(
	select 	pic_id, 
			((pic_tile_id >> 28) & 15) as tile_z, 
			((pic_tile_id >> 14) & 16383) as tile_x, 
			(pic_tile_id & 16383) as tile_y, 
			type 
	from
	(
		select distinct(pic_id), pic_tile_id, type from
		(
			select pattern_id as pic_id, node_id_t as pic_tile_id, type from rdb_guideinfo_spotguidepoint
			union
			select arrow_id as pic_id, node_id_t as pic_tile_id, type from rdb_guideinfo_spotguidepoint
		) as a
	) as b
	where b.pic_id <> 0
);

---------------------------------------------------------------------
CREATE TABLE temp_vics_org2rdb_with_node
(
  class0 integer,
  class1 integer,
  class2 smallint,
  class3 smallint,
  class4 character varying(10),
  link_id bigint,
  linkdir integer,
  s_fraction double precision,
  e_fraction double precision,
  s_point smallint,
  e_point smallint,
  s_node bigint,
  e_node bigint,
  first_flag integer DEFAULT 1
);

CREATE TABLE temp_vics_link_seq
(
   class0 integer,
   class1 integer,
   class2 integer, 
   class3 smallint,
   class4 character varying,
   link_id bigint,
   linkdir integer,
   seq smallint,
   group_id smallint
);

CREATE TABLE temp_vics_link_walked
 (
   class0 integer,
   class1 integer,
   class2 integer, 
   class3 smallint,
   class4 character varying,
   link_id bigint,
   linkdir integer
 );
 
create table rdb_vics_org2rdb_jpn_bak 
as 
(
	select * from rdb_vics_org2rdb_jpn
); 

create table rdb_ipc_vics_org2rdb_jpn_bak 
as 
(
	select * from rdb_ipc_vics_org2rdb_jpn
); 

create table rdb_vics_org2rdb_axf_bak 
as 
(
	select * from rdb_vics_org2rdb_axf
); 

---------------------------------------------------------------------
CREATE TABLE rdb_language
(
  language_id smallint primary key NOT NULL,
  l_full_name character varying(40) NOT NULL,
  l_talbe character varying(80) NOT NULL,
  pronunciation character varying(40),
  p_table character varying(80),
  language_code character varying(3),
  language_code_client character varying(3) NOT NULL,
  language_id_client   smallint NOT NULL
);

---------------------------------------------------------------------
CREATE TABLE rdb_link_with_all_attri_view
(
  gid                    integer NOT NULL primary key,
  link_id                bigint,
  link_id_t              integer,
  display_class          smallint,
  start_node_id          bigint,
  start_node_id_t        integer,
  end_node_id            bigint,
  end_node_id_t          integer,
  road_type              smallint,
  one_way                smallint,
  function_code          smallint,
  link_length            integer,
  road_name              character varying(16384),
  road_name_id            integer,
  road_number            character varying(2048),
  lane_id                integer,
  toll                   smallint,
  regulation_flag        boolean,
  tilt_flag              boolean,
  speed_regulation_flag  boolean,
  link_add_info_flag     boolean,
  fazm                   smallint,
  tazm                   smallint,
  fnlink                 bigint,
  fnlink_t               integer,
  tnlink                 bigint,
  tnlink_t               integer,
  link_type              integer,
  extend_flag            smallint,
  bypass_flag			 smallint,
  matching_flag			 smallint,
  highcost_flag		 smallint,  
  fazm_path              smallint,
  tazm_path              smallint,
  shield_flag            boolean,
  regulation_exist_state integer,
  link_length_modify     integer,
  link_length_unit       integer,
  geom_blob              bytea,
  pdm_flag               boolean,
  common_main_link_attri smallint,
  pass_side              integer,
  admin_wide_regulation  integer,
  region_cost            integer,
  reserve                integer,
  width                  smallint,
  hwy_flag               smallint NOT NULL DEFAULT 0,
  abs_link_id			 integer NOT NULL DEFAULT 0,
  s_sequence_link_id		 bigint NOT NULL DEFAULT -1,
  e_sequence_link_id		 bigint NOT NULL DEFAULT -1,
  forecast_flag			 boolean
);SELECT AddGeometryColumn('','rdb_link_with_all_attri_view','the_geom','4326','LINESTRING',2);

----------------------------------------------------------
CREATE TABLE rdb_node_with_all_attri_view
(
  gid integer NOT NULL primary key,
  node_id bigint,
  node_id_t integer,
  extend_flag integer,
  height smallint,
  link_num smallint,
  branches bigint[],
  upgrade_node_id bigint,
  upgrade_node_id_t integer,
  geom_blob bytea
);SELECT AddGeometryColumn('','rdb_node_with_all_attri_view','the_geom','4326','POINT',2);
----------------------------------------------------------------
---park
----------------------------------------------------------------
CREATE TABLE temp_rdb_link_add_info_contain_park
as
(
	select * from rdb_link_add_info
);
CREATE TABLE temp_rdb_linklane_info_contain_park
as
(
	select * from rdb_linklane_info
);
CREATE TABLE temp_rdb_link_contain_park
as
(
	select * from rdb_link
);
CREATE TABLE temp_rdb_node_contain_park
as
(
	select * from rdb_node
);
create table temp_park_node_id
(
	base_node_id bigint not null,
	new_park_node_id bigint not null
);

CREATE TABLE rdb_park_region
(
  park_region_id integer NOT NULL,
  park_region_tile integer,
  park_region_name character varying(1024),
  park_type smallint,
  park_toll smallint,
  park_map_ID bigint,
  geom_blob bytea
);SELECT AddGeometryColumn('','rdb_park_region','the_geom','4326','POLYGON',2);SELECT AddGeometryColumn('','rdb_park_region','the_geom_4096','4326','POLYGON',2);

CREATE TABLE rdb_park_point
(
  park_poi_id bigint NOT NULL primary key,
  park_region_id integer,
  park_name character varying(1024)
);SELECT AddGeometryColumn('','rdb_park_point','the_geom','4326','POINT',2);

CREATE TABLE rdb_park_node
(
  park_node_id bigint NOT NULL primary key,
  park_node_id_t integer not null,
  park_base_id bigint,
  park_node_type smallint,
  park_connect_link_num smallint,
  park_connect_link_array bigint[],
  park_link_num smallint,
  park_branches bigint[],
  park_region_id integer,
  geom_blob bytea
);SELECT AddGeometryColumn('','rdb_park_node','the_geom','4326','POINT',2);SELECT AddGeometryColumn('','rdb_park_node','the_geom_4096','4326','POINT',2);

CREATE TABLE rdb_park_link
(
  park_link_id bigint NOT NULL primary key,
  park_link_id_t integer not null,
  park_link_oneway smallint,
  park_s_node_id bigint,
  park_e_node_id bigint,
  park_link_length integer,
  park_link_type smallint,
  park_link_connect_type smallint,
  park_floor smallint,
  park_link_lean boolean,
  park_link_toll smallint,
  park_link_add1 smallint,
  park_link_add2 smallint,
  park_region_id integer,
  geom_blob bytea
);SELECT AddGeometryColumn('','rdb_park_link','the_geom','4326','LINESTRING',2);SELECT AddGeometryColumn('','rdb_park_link','the_geom_4096','4326','LINESTRING',2);

------------------------------------------------------------------------
-- highway ic info
CREATE TABLE rdb_highway_ic_info
(
  gid              serial not null primary key,
  ic_no            integer not null,
  up_down          smallint not null,
  facility_id      integer not null,
  between_distance smallint not null,
  inside_distance  smallint not null,
  enter            smallint not null,
  exit             smallint not null,
  tollgate         smallint not null,
  jct              smallint not null,
  pa               smallint not null,
  sa               smallint not null,
  ic               smallint not null,
  ramp             smallint not null,
  smart_ic         smallint not null,
  barrier          smallint not null,
  dummy            smallint not null,
  boundary         smallint not null,
  "new"            smallint not null,
  unopen           smallint not null,
  forward_flag     smallint not null,
  reverse_flag     smallint not null,
  toll_count       smallint not null,
  enter_direction  smallint not null,
  path_count       smallint not null,
  vics_count       smallint not null,
  conn_count       smallint not null,
  illust_count     smallint not null,
  store_count      smallint not null,
  servise_flag     smallint not null,
  toll_index       smallint not null,
  vics_index       smallint not null,
  road_no          integer not null,
  road_index       integer,
  name             character varying(4096),
  conn_tile_id     integer not null,
  tile_id          integer not null,
  index            smallint not null
);
------------------------------------------------------------------------
create table rdb_highway_road_info
(
  gid             serial not null primary key,
  road_no         integer not null,
  iddn_road_kind  smallint not null, 
  road_kind       smallint not null, 
  road_attr       smallint not null,
  loop            smallint not null,
  new             smallint not null,
  un_open         smallint not null,
  start_ic_no     smallint not null,
  ic_count        smallint not null,
  name            character varying(1024),
  up_down         smallint not null,
  tile_id         integer not null,
  tile_index      smallint not null,
  road_number     character varying  -- Just for Service DB
);

------------------------------------------------------------------------
CREATE TABLE rdb_highway_conn_info
(
  gid                 serial not null primary key, 
  ic_no               smallint not null,
  road_attr           smallint not null,
  conn_direction      smallint not null,
  same_road_flag      smallint not null,
  tile_change_flag    smallint not null,
  uturn_flag          smallint not null,
  new_flag            smallint not null,
  unopen_flag         smallint not null,
  vics_flag           smallint not null,
  toll_flag           smallint not null,
  conn_road_no        smallint not null,
  conn_ic_no          smallint not null,
  conn_link_length    smallint not null,
  conn_tile_id        integer not null,
  toll_index          smallint not null,    
  toll_count          smallint not null,
  vics_index          smallint not null,
  vics_count          smallint not null,
  tile_id             integer not null,
  index               smallint not null
);

------------------------------------------------------------------------
-- toll info
CREATE TABLE rdb_highway_toll_info
(
  toll_no          serial not null primary key,
  ic_no            INTEGER not null,
  conn_ic_no       INTEGER,
  toll_class       smallint not null,
  class_name       character varying(5),
  up_down          SMALLINT not null,
  facility_id	   SMALLINT not null,
  tollgate_count   smallint not null,
  etc_antenna      smallint not null,
  enter            smallint not null,
  exit             smallint not null,
  jct              smallint not null,
  sa_pa            smallint not null,
  gate             smallint not null,
  unopen           smallint not null,
  dummy            smallint not null,
  non_ticket_gate  smallint not null,
  check_gate       smallint not null,
  single_gate      smallint not null,
  cal_gate         smallint not null,
  ticket_gate      smallint not null,
  nest             smallint not null,
  uturn            smallint not null,
  not_guide        smallint not null,
  normal_toll      smallint not null,
  etc_toll         smallint not null,
  etc_section      smallint not null,
  name             character varying(132),
  old_toll_no      integer not null,
  tile_id          integer not null,
  tile_index       smallint not null
);

------------------------------------------------------------------------
-- store info
CREATE TABLE rdb_highway_store_info
(
  gid             serial not null primary key,
  ic_no           integer not null,
  bis_time_flag   smallint not null,
  bis_time_num    smallint not null,
  store_kind      INTEGER not null,
  open_hour       smallint not null,
  open_min        smallint not null,
  close_hour      smallint not null,
  close_min       smallint not null,
  holiday         smallint not null,
  goldenWeek      smallint not null,
  newyear         smallint not null,
  yearend         smallint not null,
  bonfestival     smallint not null,
  Sunday          smallint not null,
  Saturday        smallint not null,
  Friday          smallint not null,
  Thursday        smallint not null,
  Wednesday       smallint not null,
  Tuesday         smallint not null,
  Monday          smallint not null,
  seq_nm          smallint not null,
  tile_id         integer not null,
  tile_index      smallint not null,
  store_name      character varying
);

-----------------------------------------------------------------------------
CREATE TABLE rdb_highway_store_picture
(
  store_kind      integer NOT NULL primary key,
  picture_name    character varying(60) NOT NULL
);

-----------------------------------------------------------------------------
CREATE TABLE rdb_highway_store_name
(
  gid              serial not null primary key,
  store_kind       integer not null,
  name             character varying(100),
  language_code    character(3)
);

-----------------------------------------------------------------------------
CREATE TABLE rdb_highway_path_point
(
  gid          serial not null primary key,
  ic_no        integer not null,
  enter_flag   smallint not null,
  exit_flag    smallint not null,
  main_flag    smallint not null,
  center_flag  smallint not null,
  new_flag     smallint not null,
  unopen_flag  smallint not null,
  link_id      bigint not null,
  lon          INTEGER not null,
  lat          INTEGER not null,
  tile_id      integer not null,
  index        smallint not null,
  node_id      bigint not null
);SELECT AddGeometryColumn('','rdb_highway_path_point','the_geom','4326','POINT',2); SELECT AddGeometryColumn('','rdb_highway_path_point','link_geom','4326','LINESTRING',2);

-----------------------------------------------------------------------------
create table rdb_highway_mapping
(
  gid              serial not null primary key,
  road_kind        smallint not null,
  ic_count         smallint not null,
  road_no          smallint not null,
  display_class    smallint not null,
  link_id          bigint not null,
  forward_tile_id  integer,
  forward_ic_no    smallint not null,
  backward_tile_id integer,
  backward_ic_no   smallint not null,
  path_type        character varying(10) not null,
  tile_id          integer not null
);SELECT AddGeometryColumn('','rdb_highway_mapping','the_geom','4326','LINESTRING',2);

------------------------------------------------------------------------
-- Link, which's road_type is 0, but are not included in hwy model.
CREATE TABLE rdb_highway_not_hwy_model_link
(
   link_id   bigint not null primary key
);

------------------------------------------------------------------------
-- highway illust info
create table rdb_highway_illust_info
(
  gid          serial not null,
  ic_no        integer not null primary key,
  image_id     integer not null,
  tile_id      integer not null
);

------------------------------------------------------------------------
-- highway service info
create table rdb_highway_service_info
(
  gid                     serial not null primary key,
  ic_no                   integer not null,
  dog_run                 smallint not null,
  hwy_oasis               smallint not null,
  public_telephone        smallint not null,
  vending_machine         smallint not null,
  handicapped_telephone   smallint not null,
  handicapped_toilet      smallint not null,
  information             smallint not null,
  snack_corner            smallint not null,
  nursing_room            smallint not null,
  gas_station             smallint not null,
  launderette             smallint not null,
  coin_shower             smallint not null,
  toilet                  smallint not null,
  rest_area               smallint not null,
  shopping_corner         smallint not null,
  fax_service             smallint not null,
  postbox                 smallint not null,
  hwy_infor_terminal      smallint not null,
  atm                     smallint not null,
  hwy_hot_springs         smallint not null,
  nap_rest_area           smallint not null,
  restaurant              smallint not null,
  tile_id                 integer not null
);

------------------------------------------------------------------------
CREATE TABLE rdb_highway_ic_mapping
(
  gid           serial not null primary key,
  up_down       smallint NOT NULL,
  facility_id   smallint NOT NULL,
  ic_no         integer NOT NULL,
  tile_id       integer NOT NULL,
  ic_index      smallint NOT NULL
);

------------------------------------------------------------------------
CREATE TABLE rdb_highway_node_add_info
(
  gid              SERIAL PRIMARY KEY not null,
  link_id          BIGINT not null,
  node_id          BIGINT not null,
  start_end_flag   SMALLINT not null,
  toll_flag        SMALLINT not null,
  no_toll_money    SMALLINT not null,
  ---------------------------------------------
  facility_num     SMALLINT not null,
  up_down          SMALLINT not null,
  facility_id      SMALLINT not null,
  seq_num          SMALLINT not null,
  etc_antenna      SMALLINT not null,
  enter            SMALLINT not null,
  exit             SMALLINT not null,
  jct              SMALLINT not null,
  sapa             SMALLINT not null,
  gate             SMALLINT not null,
  un_open          SMALLINT not null,
  dummy            SMALLINT not null,
  ---------------------------------------------
  toll_type_num    SMALLINT not null,
  non_ticket_gate  smallint not null,
  check_gate       smallint not null,
  single_gate      smallint not null,
  cal_gate         smallint not null,
  ticket_gate      smallint not null,
  nest             smallint not null,
  uturn            smallint not null,
  not_guide        smallint not null,
  normal_toll      smallint not null,
  etc_toll         smallint not null,
  etc_section      smallint not null,
  name             CHARACTER VARYING(1024),
  tile_id          INTEGER not null
);

------------------------------------------------------------------------
CREATE TABLE rdb_highway_fee_toll_info
(
  gid              SERIAL NOT NULL primary key,
  up_down          INTEGER NOT NULL,
  facility_id      INTEGER NOT NULL,
  about_flag       SMALLINT NOT NULL,
  over_flag        SMALLINT NOT NULL,
  ticket_flag	   SMALLINT NOT NULL,
  alone_flag       SMALLINT NOT NULL,
  free_flag        SMALLINT NOT NULL,
  tile_id          INTEGER NOT NULL
);

------------------------------------------------------------------------
CREATE TABLE rdb_highway_fee_alone_toll
(
  gid              SERIAL NOT NULL primary key,
  up_down          INTEGER NOT NULL,
  facility_id      INTEGER NOT NULL,
  k_money_idx      SMALLINT NOT NULL,
  s_money_idx      SMALLINT NOT NULL,
  m_money_idx      SMALLINT NOT NULL,
  l_money_idx      SMALLINT NOT NULL,
  vl_money_idx     SMALLINT NOT NULL,
  tile_id          INTEGER NOT NULL
);

------------------------------------------------------------------------
CREATE TABLE rdb_highway_fee_ticket_toll
(
  gid              SERIAL NOT NULL primary key,
  from_facility_id INTEGER NOT NULL,
  to_facility_id   INTEGER NOT NULL,
  k_money_idx      SMALLINT NOT NULL,
  s_money_idx      SMALLINT NOT NULL,
  m_money_idx      SMALLINT NOT NULL,
  l_money_idx      SMALLINT NOT NULL,
  vl_money_idx     SMALLINT NOT NULL,
  from_tile_id     INTEGER NOT NULL
);

------------------------------------------------------------------------
CREATE TABLE rdb_highway_fee_free_toll
(
  gid              SERIAL NOT NULL primary key,
  from_facility_id INTEGER NOT NULL,
  to_facility_id   INTEGER NOT NULL,
  from_tile_id     INTEGER NOT NULL
);

------------------------------------------------------------------------
create table rdb_highway_fee_same_facility
(
  gid                serial NOT NULL primary key,
  from_up_down       integer NOT NULL,
  from_facility_id   integer NOT NULL,
  to_up_down         integer NOT NULL,
  to_facility_id     integer NOT NULL
);

------------------------------------------------------------------------
CREATE TABLE rdb_highway_fee_money
(
  money_idx        INTEGER NOT NULL ,
  money            INTEGER NOT NULL primary key
);

------------------------------------------------------------------------
CREATE TABLE rdb_slope_tbl
(
  link_id 		bigint NOT NULL,
  link_id_t 	integer NOT NULL,
  slope_pos_s	double precision NOT NULL,
  slope_pos_e	double precision NOT NULL,
  slope_value	integer NOT NULL
);
-------------------------------------------------------------------------
create table wellshape_start_end_node_rdb_tbl
(
	s_node bigint,
	e_node bigint,
	postion_node bigint
);
CREATE TABLE wellshape_innerlink_rdb_tbl
(
  innerlink bigint
);
-------------------------------------------------------------------------------
create table temp_link_sequence_dir
(
	link_id bigint not null,
	dir smallint not null,
	sequence_id smallint not null,
	sequence_link_id bigint not null,
	node_id bigint not null
);
create table rdb_link_sequence
(
	link_id bigint not null,
	sequence smallint not null,
	s_link_id bigint,
	e_link_id bigint
);
create table temp_region_link_layer4_sequence_dir
(
	link_id bigint not null,
	dir smallint not null,
	sequence_id smallint not null,
	sequence_link_id bigint not null,
	node_id bigint not null
);
create table rdb_region_link_layer4_sequence
(
	link_id bigint not null,
	sequence smallint not null,
	s_link_id bigint,
	e_link_id bigint
);
create table temp_region_link_layer6_sequence_dir
(
	link_id bigint not null,
	dir smallint not null,
	sequence_id smallint not null,
	sequence_link_id bigint not null,
	node_id bigint not null
);
create table rdb_region_link_layer6_sequence
(
	link_id bigint not null,
	sequence smallint not null,
	s_link_id bigint,
	e_link_id bigint
);
create table temp_region_link_layer8_sequence_dir
(
	link_id bigint not null,
	dir smallint not null,
	sequence_id smallint not null,
	sequence_link_id bigint not null,
	node_id bigint not null
);
create table rdb_region_link_layer8_sequence
(
	link_id bigint not null,
	sequence smallint not null,
	s_link_id bigint,
	e_link_id bigint
);
CREATE TABLE rdb_guideinfo_safety_zone
(
  gid serial NOT NULL,
  safetyzone_id integer,
  link_id bigint NOT NULL,
  link_id_t integer NOT NULL,
  speedlimit smallint,
  speedunit_code smallint,
  direction smallint,
  safety_type smallint
);

--- Forecast data.
CREATE TABLE rdb_forecast_link
(
  link_id bigint,
  link_id_t integer,
  dir smallint,
  free_time smallint,
  weekday_time smallint,
  weekend_time smallint,
  average_time smallint,
  info_id integer default 0,
  type smallint
);

CREATE TABLE rdb_forecast_control
(
  info_id integer,
  weather_type smallint default 0,
  day_type smallint default 0,
  start_day smallint default 0,
  end_day smallint default 0,
  time_id_weekday integer,  
  time_id_weekend integer  
);

CREATE TABLE rdb_forecast_time
(
  time_id integer,
  time_slot smallint,
  time smallint
);
