------------------------------------------------------------------------------------
CREATE TABLE road_link_4326
(
  objectid integer NOT NULL primary key,
  from_node_id integer NOT NULL,
  to_node_id integer NOT NULL,
  roadclass_c integer NOT NULL,
  naviclass_c integer NOT NULL,
  linkclass_c integer NOT NULL,
  roadmanager_c integer NOT NULL,
  roadwidth_c integer NOT NULL,
  widearea_f integer NOT NULL,
  smartic_c integer NOT NULL,
  bypass_f integer NOT NULL,
  caronly_f integer NOT NULL,
  island_f integer NOT NULL,
  road_no integer NOT NULL,
  road_code integer NOT NULL,
  nopassage_c integer NOT NULL,
  oneway_c integer NOT NULL,
  lane_count integer NOT NULL
); SELECT AddGeometryColumn('','road_link_4326','the_geom','4326','MULTILINESTRING',2);

------------------------------------------------------------------------------------
CREATE TABLE road_node_4326
(
  objectid integer NOT NULL primary key,
  name_kanji character varying(132),
  name_yomi character varying(132),
  signal_f integer NOT NULL
); SELECT AddGeometryColumn('','road_node_4326','the_geom','4326','POINT',2);

------------------------------------------------------------------------------------


------------------------------------------------------------------------------------
------make guide lane temp table(inlink and outlink)--------------------------
------------------------------------------------------------------------------------
create table temp_lane_strand_tbl(
objectid integer,
lanedir_c integer,
approach_lane integer,
right_change integer,
left_change integer,
flow_lane integer,
from_lane_passable character varying(20),
from_link_id integer,
to_link_id integer,
from_linkdir_c integer,
passlink_cnt smallint,
passlid character varying(512)
);
------------------------------------------------------------------------------------------------
-- create tables for ipc about jdb
------------------------------------------------------------------------------------------------
CREATE TABLE temp_node_lane
(
  gid serial NOT NULL,
  objectid integer,
  fromlinkid integer,
  nodeid integer,
  tolinkid integer,
  passlid character varying(512),
  lanedir integer,
  applane integer,
  rightchg integer,
  leftchg integer,
  passable character varying(16),
  from_nodeid integer
);

CREATE TABLE temp_node_lane_midcount
(
  gid integer not null,
  midcount integer
);

CREATE TABLE road_dir_t
(
  gid serial NOT NULL,
  objectid integer,
  fromlinkid integer,
  tolinkid integer,
  midcount integer,
  midlinkid character varying(254),
  guideclass integer,
  name_kanji character varying(128),
  name_yomi character varying(128),
  boardptn_c integer,
  board_seq integer,
  CONSTRAINT road_dir_pkey PRIMARY KEY (gid)
);
---spotguide road illust
CREATE TABLE road_illust
(
  gid serial NOT NULL,
  objectid serial NOT NULL,
  roadname character varying(128),
  roaddir character varying(20),
  facname character varying(128),
  nodelist character varying(254),
  pictype integer,
  picname character varying(128),
  CONSTRAINT road_illust_pkey PRIMARY KEY (gid)
);
CREATE TABLE segment_attr_4326
(
  objectid integer NOT NULL primary key,
  link_id integer NOT NULL,
  linkattr_c integer NOT NULL
); SELECT AddGeometryColumn('','segment_attr_4326','the_geom','4326','MULTILINESTRING',2);

------------------------------------------------------------------------------------------------
-- SRID 4326 for facil info point
------------------------------------------------------------------------------------------------
CREATE TABLE facil_info_point_4326
(
  objectid integer NOT NULL primary key,
  road_code integer NOT NULL,
  road_seq integer NOT NULL,
  name_kanji character varying(132),
  name_yomi character varying(132),
  dummyname_f integer NOT NULL,
  facilclass_c integer NOT NULL,
  dummyfacil_f integer NOT NULL
); SELECT AddGeometryColumn('','facil_info_point_4326','the_geom','4326','POINT',2);

------------------------------------------------------------------------------------------------
-- SRID 4326 for Highway node
------------------------------------------------------------------------------------------------
CREATE TABLE highway_node_4326
(
  objectid integer NOT NULL primary key,
  hwymode_f integer NOT NULL,
  road_code integer NOT NULL,
  road_seq integer NOT NULL,
  direction_c integer NOT NULL,
  inout_c integer NOT NULL,
  approachnode_id integer NOT NULL,
  flownode_id integer NOT NULL,
  tollclass_c integer NOT NULL,
  tollfunc_c integer NOT NULL,
  dummytoll_f integer NOT NULL,
  guide_f integer NOT NULL,
  tollgate_lane character varying(36)
); SELECT AddGeometryColumn('','highway_node_4326','the_geom','4326','POINT',2);
 
------------------------------------------------------------------------------------------------
-- SRID 4326 for access point
------------------------------------------------------------------------------------------------
CREATE TABLE access_point_4326
(
  objectid integer NOT NULL,
  hwymode_f integer NOT NULL,
  road_code integer NOT NULL,
  road_seq integer NOT NULL,
  direction_c integer NOT NULL,
  inout_c integer NOT NULL,
  direction_kanji character varying(132),
  direction_yomi character varying(132),
  toilet_f integer NOT NULL,
  handicap_f integer NOT NULL,
  restaurant_f integer NOT NULL,
  cafeteria_f integer NOT NULL,
  souvenir_f integer NOT NULL,
  resting_f integer NOT NULL,
  nap_f integer NOT NULL,
  infodesk_f integer NOT NULL,
  information_f integer NOT NULL,
  shower_f integer NOT NULL,
  laundry_f integer NOT NULL,
  spa_f integer NOT NULL,
  fax_f integer NOT NULL,
  mailbox_f integer NOT NULL,
  cashservice_f integer NOT NULL,
  hwyoasis_f integer NOT NULL,
  carwash_f integer NOT NULL,
  gs_f integer NOT NULL,
  multiplestore_f integer NOT NULL,
  dogrun_f integer NOT NULL
); SELECT AddGeometryColumn('','access_point_4326','the_geom','4326','POINT',2);

------------------------------------------------------------------------------------------------
-- Add node id for highway node 
------------------------------------------------------------------------------------------------
CREATE TABLE highway_node_add_nodeid
(
  objectid integer,
  node_id integer,
  hwymode_f integer,
  road_code integer,
  road_seq integer,
  direction_c integer,
  inout_c integer,
  approachnode_id integer,
  flownode_id integer,
  tollclass_c integer,
  tollfunc_c integer,
  dummytoll_f integer,
  guide_f integer,
  tollgate_lane character varying(36),
  name_kanji character varying(132),
  name_yomi character varying(132),
  dummyname_f integer,
  facilclass_c integer,
  dummyfacil_f integer
); SELECT AddGeometryColumn('','highway_node_add_nodeid','the_geom','4326','POINT',2);

------------------------------------------------------------------------------------------------
-- create tables for force guide 
------------------------------------------------------------------------------------------------
CREATE TABLE road_gid
(
  gid serial NOT NULL,
  objectid integer,
  fromlinkid integer,
  tolinkid integer,
  midcount integer,
  midlinkid character varying(254),
  guidetype integer,
  CONSTRAINT road_gid_pkey PRIMARY KEY (gid)
);


CREATE TABLE road_hwynode
(
  mesh_code integer,
  node_id integer,
  road_code integer,
  seq_num integer,
  pos_code integer,
  updown_flag integer,
  kind integer
);

CREATE TABLE road_tollnode
(
  mesh_code integer,
  node_id integer,
  road_code integer,
  seq_num integer,
  "type" integer,
  updown_flag integer,
  kind integer,
  from_node integer,
  to_node integer,
  "name" character varying,
  name_yomi character varying
);

CREATE TABLE road_dicin
(
  road_code integer,
  seq_num integer,
  "name" character varying
);

CREATE TABLE middle_tile_node
(
  tile_node_id bigint NOT NULL,
  old_node_id bigint NOT NULL,
  tile_id integer NOT NULL,
  CONSTRAINT middle_tile_node_pkey PRIMARY KEY (old_node_id)
);

CREATE TABLE middle_tile_link
(
  tile_link_id bigint NOT NULL,
  old_link_id bigint NOT NULL,
  tile_id integer NOT NULL,
  start_node_id bigint NOT NULL,
  end_node_id bigint NOT NULL,
  old_s_e_node integer[] NOT NULL,
  split_seq_num integer NOT NULL,
  seq_num integer NOT NULL,
  CONSTRAINT middle_tile_link_pkey PRIMARY KEY (tile_link_id)
);




------------------------------------------------------------------------------------------------
-- create tables for sidefiles
------------------------------------------------------------------------------------------------
CREATE TABLE road_tollnode
(
  mesh_code integer,
  node_id integer,
  road_code integer,
  seq_num integer,
  "type" integer,
  updown_flag integer,
  kind integer,
  from_node integer,
  to_node integer,
  "name" character varying,
  name_yomi character varying
);


CREATE TABLE road_dicin
(
  road_code integer,
  seq_num integer,
  "name" character varying
);

CREATE TABLE road_highway
(
  gid serial NOT NULL,
  road_code integer,
  linkcls_c integer,
  dir_f integer,
  dir_point integer,
  road_point integer,
  next_point integer,
  next_dis integer,
  end_f smallint,
  forward_num integer,
  backward_num integer,
  branches character varying(254)
);

CREATE TABLE road_hwysame
(
  gid serial NOT NULL,
  road_code integer,
  road_point integer,
  dir_f int,
  name character varying(60),
  roads character varying(254)
);

CREATE TABLE road_tollguide
(
  gid serial NOT NULL,
  mesh_code integer,
  node_id integer,
  toll_type integer,
  toll_num smallint,
  roads character varying(254)
);

CREATE TABLE road_ferry_code
(
  gid serial NOT NULL,
  type character varying(40),
  ferry_id integer,
  ferry_name character varying(60)
);

CREATE TABLE road_gs
(
  gid serial NOT NULL,
  road_code integer,
  road_point integer,
  side_f smallint,
  chain_id integer,
  open_time character varying(20),
  sunday smallint,
  saturday smallint,
  friday smallint,
  thursday smallint,
  wednesday smallint,
  tuesday smallint,
  monday smallint,
  obon_day smallint,
  year_end smallint,
  year_start smallint,
  holiday smallint,
  shukusai smallint
);

CREATE TABLE road_multiple_store
(
  gid serial NOT NULL,
  road_code integer,
  road_point integer,
  side_f smallint,
  chain_id integer,
  open_time character varying(20),
  sunday smallint,
  saturday smallint,
  friday smallint,
  thursday smallint,
  wednesday smallint,
  tuesday smallint,
  monday smallint,
  obon_day smallint,
  year_end smallint,
  year_start smallint,
  holiday smallint,
  shukusai smallint
);

CREATE TABLE road_road_code
(
  gid serial NOT NULL,
  type character varying(40),
  road_code integer,
  road_name character varying(60),
  road_yomi character varying(60)
);

CREATE TABLE road_road_code_highway
(
  gid serial NOT NULL,
  type character varying(40),
  road_code integer,
  road_name character varying(60),
  road_yomi character varying(60)
);

CREATE TABLE road_roadpoint
(
  gid serial NOT NULL,
  point_mesh_code integer,
  point_x integer,
  point_y integer,
  mesh_id integer,
  fac_mesh_code integer,
  fac_x integer,
  fac_y integer,
  addr_id integer,
  road_point integer,
  fac_name character varying(60),
  fac_yomi character varying(60),
  road_code integer,
  fac_type integer,
  point_pos integer,
  point_name character varying(60),
  point_yomi character varying(60),
  sa_pa character varying(40),
  name_f smallint,
  fac_f smallint
);

CREATE TABLE road_roadpoint_bunnki
(
  gid serial NOT NULL,
  point_mesh_code integer,
  point_x integer,
  point_y integer,
  mesh_id integer,
  fac_mesh_code integer,
  fac_x integer,
  fac_y integer,
  addr_id integer,
  road_point integer,
  fac_name character varying(60),
  fac_yomi character varying(60),
  road_code integer,
  fac_type integer,
  point_pos integer,
  point_name character varying(60),
  point_yomi character varying(60),
  sa_pa character varying(40),
  name_f smallint,
  fac_f smallint
);

CREATE TABLE road_roadpoint_normal
(
  gid serial NOT NULL,
  point_mesh_code integer,
  point_x integer,
  point_y integer,
  mesh_id integer,
  fac_mesh_code integer,
  fac_x integer,
  fac_y integer,
  addr_id integer,
  road_point integer,
  fac_name character varying(60),
  fac_yomi character varying(60),
  road_code integer,
  fac_type integer,
  point_pos integer,
  point_name character varying(60),
  point_yomi character varying(60),
  sa_pa character varying(40),
  name_f smallint,
  fac_f smallint
);

CREATE TABLE road_store_code
(
  gid serial NOT NULL,
  chain_id integer,
  chain_name character varying(200)
);



------------------------------------------------------------------------------------------------
-- create tables for split_link
------------------------------------------------------------------------------------------------
CREATE TABLE temp_split_link
(
gid serial not null primary key,
objectid integer,
linkid integer,
linkattr_c integer,
start_fraction double precision,
end_fraction double precision
); SELECT AddGeometryColumn('','temp_split_link','the_geom','4326','LINESTRING',2);

CREATE TABLE temp_new_link
(
  gid serial NOT NULL,
  objectid integer,
  linkid integer,
  linkattr_c integer,
  split_seq_num integer,
  CONSTRAINT temp_new_link_pkey PRIMARY KEY (gid)
); SELECT AddGeometryColumn('','temp_new_link','the_geom','4326','LINESTRING',2);

CREATE TABLE temp_road_node
(
  gid serial NOT NULL,
  objectid integer,
  signal_f integer,
  name_kanji character varying(128),
  name_yomi character varying(128),
  new_add boolean DEFAULT false,
  CONSTRAINT temp_road_node_pkey PRIMARY KEY (gid)
);SELECT AddGeometryColumn('','temp_road_node','the_geom','4326','POINT',2);


CREATE TABLE temp_road_link
(
  gid serial primary key,
  objectid integer NOT NULL,
  from_node_id integer NOT NULL,
  to_node_id integer NOT NULL,
  roadclass_c integer NOT NULL,
  naviclass_c integer NOT NULL,
  linkclass_c integer NOT NULL,
  roadmanager_c integer NOT NULL,
  roadwidth_c integer NOT NULL,
  widearea_f integer NOT NULL,
  smartic_c integer NOT NULL,
  bypass_f integer NOT NULL,
  caronly_f integer NOT NULL,
  island_f integer NOT NULL,
  road_no integer NOT NULL,
  road_code integer NOT NULL,
  nopassage_c integer NOT NULL,
  oneway_c integer NOT NULL,
  lane_count integer NOT NULL,
  linkattr_c integer DEFAULT 0, 		-- 0: does not have attribute, 2: tunnel, 4: railway crossing.
  split_seq_num integer DEFAULT (-1) 	-- -1: has not been split, [0,N]: Sequence number.
); SELECT AddGeometryColumn('','temp_road_link','the_geom','4326','LINESTRING',2);

------------------------------------------------------------------------
CREATE TABLE temp_road_dir
(
  gid         serial NOT NULL primary key,
  objectid    integer,
  fromlinkid  integer,
  tolinkid    integer,
  midcount    integer,
  midlinkid   character varying(254),
  link_dir_kib   integer[],
  guideclass  integer,
  name_kanji  character varying(128),
  sp_name   character varying(1024),
  boardptn_c  integer,
  board_seq   integer,
  name_seqnm  integer
);

------------------------------------------------------------------------
CREATE TABLE temp_inf_dirguide
(
  gid          serial not null primary key,
  objectid     integer not null,
  guideclass_c integer not null,
  name_kanji   character varying(68),
  name_yomi    character varying(132),
  boardptn_c   integer not null,
  board_seq    integer not null,
  name_seqnm   smallint not null
);

------------------------------------------------------------------------
CREATE TABLE temp_lq_dirguide
(
  objectid    integer not null primary key,
  fromlinkid  integer not null,
  tolinkid    integer not null,
  midcount    integer not null,
  midlinkid   character varying(254),
  link_dir_lib		integer[] not null
);
------------------------------------------------------------------------
CREATE TABLE temp_language_tbl
(
  language_id smallint NOT NULL,
  l_full_name character varying(40) NOT NULL,
  l_talbe character varying(80) NOT NULL,
  pronunciation character varying(40),
  p_table character varying(80),
  language_code character varying(3),
  CONSTRAINT temp_language_tbl_pkey PRIMARY KEY (language_id)
);

------------------------------------------------------------------------
-- Dictionary and relation table
CREATE TABLE temp_link_name
(
  road_code integer,
  name_id integer
);

------------------------------------------------------------------------
CREATE TABLE temp_link_no
(
  link_id bigint not null primary key,
  name_id integer not null
);

------------------------------------------------------------------------
CREATE TABLE temp_node_name
(
  objectid integer primary key,
  name_id integer
);

------------------------------------------------------------------------
CREATE TABLE temp_toward_name
(
  gid integer,
  name_id integer
);

------------------------------------------------------------------------
CREATE TABLE temp_signpost_name
(
  gid integer,
  name_id integer
);

------------------------------------------------------------------------
CREATE TABLE temp_poi_name
(
  layer_name character varying(30),
  objectid integer,
  seq_nm integer,
  name_id integer
);

------------------------------------------------------------------------
CREATE TABLE temp_link_number_group
(
  gid serial not null primary key,
  linkid_array bigint[] not null,
  shield_id_num character varying(30) not null
);

------------------------------------------------------------------------
CREATE TABLE temp_link_number_groupid_nameid
(
  gid integer not null primary key,
  name_id integer not null
);

------------------------------------------------------------------------
CREATE TABLE temp_node_name_and_illust_name
(
  node_id integer not null primary key,
  name_kanji character varying(128),
  name_yomi character varying(128)
);

------------------------------------------------------------------------
CREATE TABLE temp_node_name_and_illust_name_groupby
(
  groupid serial not null,
  node_ids integer[],
  name_kanji character varying(128),
  name_yomi character varying(128)
);

------------------------------------------------------------------------
CREATE TABLE temp_node_name_and_illust_name_groupid_nameid
(
  groupid integer,
  name_id integer
);

------------------------------------------------------------------------
-- for toward name
CREATE TABLE temp_node_towardname
(
  gid integer not null PRIMARY KEY,
  objectid integer,
  fromlinkid integer,
  nodeid integer,
  tolinkid integer,
  midcount integer,
  midlinkid character varying(254),
  link_dir_kib integer[],
  guideclass integer,
  name_kanji character varying(128),
  toward_name character varying(1024),
  boardptn_c integer,
  board_seq integer,
  from_nodeid integer
);

------------------------------------------------------------------------
-- for ficility name
CREATE TABLE mid_temp_facil_info_point_name_group
(
  group_id      serial not null primary key,
  objectid_lid  integer[] not null,
  name_kanji    character varying(132) not null,
  name_yomi     character varying(132) not null
);

------------------------------------------------------------------------
CREATE TABLE  mid_temp_facil_info_point_name_groupid_nameid
(
  group_id     integer not null primary key,
  name_id      integer not null
);

------------------------------------------------------------------------
-- facil and name id
CREATE TABLE  mid_temp_facil_point_name
(
  objectid     integer not null primary key,
  name_id      integer not null
);


------------------------------------------------------------------------
CREATE TABLE mid_temp_facility_full_path
(
  node_id          bigint not null,
  in_link_id       bigint not null,
  full_pass_node   character varying[],
  to_node          bigint not null,
  type						 smallint not null
);
CREATE TABLE mid_temp_full_path
(
  node_id          bigint not null,
  in_link_id       bigint not null,
  full_pass_node   varchar[]
);

------------------------------------------------------------------------
CREATE TABLE mid_temp_facility_full_path_jct
(
  node_id          bigint not null,
  in_link_id       bigint not null,
  full_pass_link   character varying[],
  to_node          bigint not null
);
------------------------------------------------------------------------
CREATE TABLE mid_temp_facilit_matched
(
  gid            serial NOT NULL primary key,
  node_id        bigint not null,
  in_link_id     bigint not null,
  to_node        bigint not null,
  road_code      integer not null,
  road_seq       integer not null,
  full_pass_link character varying[] not null,
  facilclass_c   integer not null,
  inout_c        integer not null
);

------------------------------------------------------------------------
CREATE TABLE mid_temp_facilit_matched_add_nameid
(
  gid            serial NOT NULL primary key,
  node_id        bigint not null,
  in_link_id     bigint not null,
  to_node        bigint not null,
  road_code      integer not null,
  road_seq       integer not null,
  full_pass_link character varying[] not null,
  facilclass_c   integer not null,
  inout_c        integer not null,
  sp_name        character varying(1024) not null
);

------------------------------------------------------------------------
CREATE TABLE mid_temp_facility_full_path_enter_no_inlink
(
  node_id        bigint not null,
  full_pass_link text[] not null,
  to_node        bigint not null
);

------------------------------------------------------------------------
CREATE TABLE mid_temp_facility_full_path_enter
(
  node_id        bigint not null,
  in_link_id     bigint not null,
  full_pass_link character varying[] not null,
  to_node        bigint not null,
  road_code      integer not null,
  road_seq       integer not null,
  facilclass_c   integer not null,
  inout_c        integer not null
);

------------------------------------------------------------------------
CREATE TABLE mid_fac_name
(
  gid          serial NOT NULL primary key,
  node_id      bigint,
  in_link_id   bigint,
  out_link_id  bigint,
  pass_link    text,
  passlink_cnt integer,
  facilclass_c integer,
  inout_c      integer,
  sp_name      character varying(1024)
);
CREATE TABLE mid_fac_name_temp
(
  gid          serial NOT NULL primary key,
  node_id      bigint,
  in_link_id   bigint,
  facilclass_c integer,
  inout_c      integer,
  node_array    varchar,
  sp_name      character varying(1024)
);
CREATE TABLE mid_fac_name_temp_shortpath
(
  gid          serial NOT NULL primary key,
  node_id      bigint,
  in_link_id   bigint,
  facilclass_c integer,
  inout_c      integer,
  node_array    varchar,
  sp_name      character varying(1024)
);
CREATE TABLE highway_node_add_nodeid_midid
as
(
	select b.tile_node_id as node_id, hwymode_f, road_code, road_seq, direction_c, inout_c, approachnode_id, flownode_id, tollclass_c,
		tollfunc_c, dummytoll_f, guide_f, tollgate_lane, name_kanji, name_yomi, dummyname_f, facilclass_c, dummyfacil_f,the_geom
	from highway_node_add_nodeid as a
	left join middle_tile_node as b
	on a.node_id = b.old_node_id
);

------------------------------------------------------------------------
-- regulation
------------------------------------------------------------------------
CREATE TABLE temp_condition_regulation_tbl
(
  cond_id integer,
  start_month integer,
  start_day integer,
  end_month integer,
  end_day integer,
  start_hour integer,
  start_min integer,
  end_hour integer,
  end_min integer,
  dayofweek_c integer
);

-------------------------------------------------------------------------------------------------
-- admin
-------------------------------------------------------------------------------------------------
CREATE TABLE base_admin_4326
(
  objectid integer NOT NULL primary key,
  citycode character varying(9) NOT NULL
); SELECT AddGeometryColumn('','base_admin_4326','the_geom',4326,'MULTIPOLYGON',2);

CREATE TABLE temp_admin_order0
(
  order0_id character varying(10) PRIMARY KEY,
  order0_code character varying(10),
  name_kanji character varying(254),
  name_yomi character varying(254)
);SELECT AddGeometryColumn('public', 'temp_admin_order0', 'the_geom', 4326, 'MULTIPOLYGON', 2);

CREATE TABLE temp_admin_order1
(
  order1_id character varying(10) PRIMARY KEY,
  order0_id character varying(10),
  name_kanji character varying(254),
  name_yomi character varying(254)
);SELECT AddGeometryColumn('public', 'temp_admin_order1', 'the_geom', 4326, 'MULTIPOLYGON', 2);

CREATE TABLE temp_admin_order2
(
  order2_id character varying(10) PRIMARY KEY,
  order1_id character varying(10),
  name_kanji character varying(254),
  name_yomi character varying(254)
);SELECT AddGeometryColumn('public', 'temp_admin_order2', 'the_geom', 4326, 'MULTIPOLYGON', 2);

CREATE TABLE temp_admin_order8
(
  order8_id character varying(10) PRIMARY KEY,
  order1_id character varying(10),
  name_kanji character varying(254),
  name_yomi character varying(254)
);SELECT AddGeometryColumn('public', 'temp_admin_order8', 'the_geom', 4326, 'MULTIPOLYGON', 2);
------------------------------------------------------------
----forceguide_patch
--------------------------------------------------------
CREATE TABLE road_force_guide_append_patch_tbl
(
  gid serial not null primary key,
  guide_code varchar,
  guide_code_int int,
  meshcode1 int,
  node1 int,
  meshcode2 int,
  node2 int,
  meshcode3 int,
  node3 int,
  meshcode4 int,
  node4 int,
  meshcode5 int,
  node5 int,
  meshcode6 int,
  node6 int,
  meshcode7 int,
  node7 int
);


CREATE TABLE road_force_guide_update_patch_tbl
(
  gid serial not null primary key,
  guide_code varchar,
  guide_code_int int,
  meshcode1 int,
  node1 int,
  meshcode2 int,
  node2 int,
  meshcode3 int,
  node3 int,
  meshcode4 int,
  node4 int,
  meshcode5 int,
  node5 int,
  meshcode6 int,
  node6 int,
  meshcode7 int,
  node7 int
);

CREATE TABLE road_force_guide_friendly_append_patch_tbl
(
  gid serial not null primary key,
  guide_code varchar,
  guide_code_int int,
  meshcode1 int,
  node1 int,
  meshcode2 int,
  node2 int,
  meshcode3 int,
  node3 int,
  meshcode4 int,
  node4 int,
  meshcode5 int,
  node5 int,
  meshcode6 int,
  node6 int,
  meshcode7 int,
  node7 int
);

CREATE TABLE crossname_tbl
(
  gid serial NOT NULL,
  inlinkid bigint,
  nodeid bigint,
  outlinkid bigint,
  passlid character(256),
  passlink_cnt integer,
  namestr character(1024)
);

CREATE TABLE temp_boundary
(
  ad_cd integer
);SELECT AddGeometryColumn('','temp_boundary','the_geom','4326','LINESTRING',2);

CREATE TABLE temp_hlink
(
  link_id bigint,
  s_node bigint,
  e_node bigint,
  one_way_code smallint,
  CONSTRAINT temp_hlink_pkey PRIMARY KEY (link_id)
);SELECT AddGeometryColumn('','temp_hlink','the_geom','4326','LINESTRING',2);

CREATE TABLE temp_inode
(
  link_id bigint,
  s_node bigint,
  e_node bigint,
  one_way_code smallint,
  CONSTRAINT temp_inode_pkey PRIMARY KEY (link_id)
);SELECT AddGeometryColumn('','temp_inode','the_geom','4326','POINT',2);

CREATE TABLE temp_bnode
(
  link_id bigint,
  s_node bigint,
  e_node bigint,
  one_way_code smallint,
  bnode bigint,
  link_id2 bigint,
  s_node2 bigint,
  e_node2 bigint,
  one_way_code2 smallint,
  CONSTRAINT temp_bnode_pkey PRIMARY KEY (link_id, bnode, link_id2)
);SELECT AddGeometryColumn('','temp_bnode','the_geom','4326','POINT',2);

CREATE TABLE temp_guideinfo_boundary
(
  gid serial primary key,
  id serial,
  inlinkid bigint,
  nodeid bigint,
  outlinkid bigint,
  innode bigint,
  outnode bigint,
  out_adcd integer
);
CREATE TABLE temp_admin_wavid
(
  ad_cd integer NOT NULL,
  wav_id integer NOT NULL,
  CONSTRAINT temp_admin_wavid_pkey PRIMARY KEY (ad_cd)
);
CREATE TABLE temp_admin_wavid_strtts
(
  wav_id integer NOT NULL,
  strTTS varchar(254) NOT NULL
);
CREATE TABLE temp_admin_wavid_strtts_json
(
  wav_id integer NOT NULL,
  strTTS_json varchar(65535) NOT NULL
);

--------------------------------------------------------------------
CREATE TABLE mid_road_hwynode_middle_nodeid
(
  objectid   integer not null primary key,
  node_id    bigint,
  hwymode_f  integer,
  road_code   integer,
  road_seq    integer,
  direction_c integer,
  inout_c integer,
  approachnode_id integer,
  flownode_id integer,
  tollclass_c integer,
  tollfunc_c integer,
  dummytoll_f integer,
  guide_f integer,
  tollgate_lane character varying(36),
  name_kanji character varying(132),
  name_yomi character varying(132),
  dummyname_f integer,
  facilclass_c integer,
  dummyfacil_f integer
); SELECT AddGeometryColumn('','mid_road_hwynode_middle_nodeid','the_geom','4326','POINT',2);


--------------------------------------------------------------------
CREATE TABLE mid_access_point_middle_nodeid
(
  objectid         integer primary key,
  node_id          bigint not null,
  old_node_id      bigint not null,
  hwymode_f        integer,
  road_code        integer,
  road_seq         integer,
  direction_c      integer,
  inout_c          integer,
  direction_kanji  character varying(132),
  direction_yomi   character varying(132),
  toilet_f         integer,
  handicap_f       integer,
  restaurant_f     integer,
  cafeteria_f      integer,
  souvenir_f       integer,
  resting_f        integer,
  nap_f            integer,
  infodesk_f       integer,
  information_f    integer,
  shower_f         integer,
  laundry_f        integer,
  spa_f            integer,
  fax_f            integer,
  mailbox_f        integer,
  cashservice_f    integer,
  hwyoasis_f       integer,
  carwash_f        integer,
  gs_f             integer,
  multiplestore_f  integer,
  dogrun_f         integer
);SELECT AddGeometryColumn('','mid_access_point_middle_nodeid','the_geom','4326','POINT',2);

--------------------------------------------------------------------
CREATE TABLE mid_hwy_main_link
(
  link_id       bigint primary key,
  s_node        bigint,
  e_node        bigint,
  display_class smallint,
  link_type     smallint,
  road_type     smallint,
  toll          smallint,
  one_way_code  smallint,
  lineclass_c   integer,
  roadcode      integer,
  fazm          smallint,
  tazm          smallint,
  hwymode_f     smallint,
  tile_id       integer
);

--------------------------------------------------------------------
CREATE TABLE mid_hwy_main_link_no_toll
(
  link_id       bigint primary key,
  s_node        bigint,
  e_node        bigint,
  display_class smallint,
  link_type     smallint,
  road_type     smallint,
  toll          smallint,
  one_way_code  smallint,
  roadcode      integer,
  fazm          smallint,
  tazm          smallint,
  hwymode_f     smallint,
  tile_id       integer
);

--------------------------------------------------------------------
CREATE TABLE mid_hwy_main_link_path
(
  gid           serial not null primary key,
  road_code     integer not null,
  updown_flag   integer not null,
  node_id       bigint not null,
  link_id       bigint null,
  seqnum        integer not null,
  cycle_flag    integer not null,
  lineclass_c   integer not null
);

----------------------------------------------------------------------------
create table mid_hwy_ic_path
(
  gid            serial not null primary key,
  s_road_code    integer not null,
  s_road_point   integer not null,
  s_updown       integer not null,
  s_node_id      bigint not null,
  s_facilcls     integer not null,
  s_inout_c      integer not null,
  t_road_code    integer,
  t_road_point   integer,
  t_updown       integer,
  t_node_id      bigint,
  t_facilcls     integer,
  t_inout_c      integer,
  inlinkid       bigint,
  outlinkid      bigint,
  link_lid       character varying(2048) not null, -- split by |
  node_lid       character varying(2048) not null, -- split by |
  path_type      character varying(8) not null,    -- UTURN, JCT, IC, SAPA
  toll_flag      smallint not null                 -- 1: toll, 0: No Toll
);

----------------------------------------------------------------------------
CREATE TABLE mid_hwy_ic_no
(
  gid            serial not null primary key,
  ic_no          integer not null,
  node_id        bigint not null,
  road_code      integer not null,
  road_point     integer not null,
  updown         integer not null,
  facilclass_c   integer not null,
  inout_c        integer not null,
  name_kanji     character varying(132),
  facility_id    integer
);

----------------------------------------------------------------------------
CREATE TABLE mid_hwy_facility_id
(
  facility_id     serial not null primary key,
  road_code       integer not null,
  road_point      integer not null
);

----------------------------------------------------------------------------
create table mid_hwy_road_no
(
  road_no        serial not null primary key,
  roadcode       integer not null,
  lineclass_c    integer not null,
  road_type      integer not null,
  linedirtype_c  integer not null
);

------------------------------------------------------------------------
CREATE TABLE mid_hwy_ic_link_temp_mapping
(
  gid               serial not null primary key,
  backward_node_id  bigint,
  backward_ic_no    integer,
  bwd_facility_id   integer,
  forward_node_id   bigint,
  forward_ic_no     integer,
  fwd_facility_id   integer,
  link_id           bigint not null,
  path_type         character varying(8),
  toll_flag         smallint not null  -- 1: toll, 0: No Toll
);

------------------------------------------------------------------------
CREATE TABLE mid_hwy_node_add_info
(
  ic_no          integer,              -- NULL, when it is no toll facility
  updown         integer not null,
  facility_id    integer not null,
  facilcls       smallint not null,
  in_out         smallint not null,
  link_lid       character varying(2048),
  node_lid       character varying(2048),
  add_node_id    bigint NOT NULL,
  add_link_id    bigint not null,
  name_kanji     character varying(132),
  name_yomi      character varying(132),
  pos_type       integer not null,
  pos_type_name  character varying(32),
  toll_flag      smallint not null     -- 1: toll, 0: No Toll
);

---------------------------------------------------------------------------------------------------------
--parking
---------------------------------------------------------------------------------------------------------
CREATE TABLE temp_park_node
(
  node_id integer NOT NULL,
  parking_id integer not null
);SELECT AddGeometryColumn('','temp_park_node','the_geom','4326','POINT',2);
CREATE TABLE temp_park_link
(
  link_id integer NOT NULL,
  parking_id integer not null
);
CREATE TABLE temp_park_node_connect
as
(
	select a.node_id
	from temp_park_node as a
	left join road_link_4326 as b
	on a.node_id = b.from_node_id or a.node_id = b.to_node_id
	left join temp_park_link as c
	on b.objectid = c.link_id
	where c.link_id is null
	group by a.node_id
);
CREATE TABLE parking_shape_4326
(
  objectid integer NOT NULL,
  parkingclass_c integer NOT NULL
); SELECT AddGeometryColumn('','parking_shape_4326','the_geom','4326','MULTIPOLYGON',2);

-------------------------------------------------------------
CREATE TABLE road_sapa_illust
(
  illust_name  character varying(30) NOT NULL,
  road_name    character varying(30) NOT NULL,
  fac_name     character varying(30) NOT NULL,
  updown_flag  character varying(10) NOT NULL,
  mesh_id      bigint NOT NULL,
  node_id      bigint NOT NULL
);
-------------------------------------------------------------
CREATE TABLE access_point_4326_middle
as
(
	select distinct c.tile_node_id, a.hwymode_f, a.road_code, a.road_seq, a.inout_c,a.the_geom
	from access_point_4326 as a 
	left join road_node_4326 as b
	on ST_Equals(a.the_geom,b.the_geom)
	left join middle_tile_node as c
	on b.objectid = c.old_node_id
);
CREATE TABLE mid_temp_multi_highway_node
as
(
		select node_id, array_agg(facilclass_c) as type_array, count(*) as num
		from mid_road_hwynode_middle_nodeid
		where facilclass_c != 6
		group by node_id
);
CREATE TABLE temp_node_num_for_towardname
AS
(
	select node_id,count(*) as num
	from
	(
		select link_id,s_node as node_id
		from link_tbl
		where one_way_code in(1,2)
		union
		select link_id,e_node as node_id
		from link_tbl
		where one_way_code in(1,3)
	)temp
	group by node_id
);
-------------------------------
CREATE TABLE base_site_4326
(
	objectid integer NOT NULL,
  sc1bgclass_c integer NOT NULL,
  sc2bgclass_c integer NOT NULL,
  sc3bgclass_c integer NOT NULL,
  sc4bgclass_c integer NOT NULL
);SELECT AddGeometryColumn('','base_site_4326','the_geom','4326','MULTIPOLYGON',2);
CREATE TABLE base_annotation_4326
(
	objectid integer NOT NULL,
  name_string1 character varying(132),
  name_string2 character varying(132),
  name_string3 character varying(132),
  annoclass_c integer NOT NULL,
  sc1disptype_c integer NOT NULL,
  sc2disptype_c integer NOT NULL,
  sc3disptype_c integer NOT NULL,
  sc4disptype_c integer NOT NULL
);SELECT AddGeometryColumn('','base_annotation_4326','the_geom','4326','POINT',2);
CREATE TABLE city_annotation_4326
(
	objectid integer NOT NULL,
  name_string1 character varying(132),
  name_string2 character varying(132),
  annoclass_c integer NOT NULL,
  disptype_c integer NOT NULL
);SELECT AddGeometryColumn('','city_annotation_4326','the_geom','4326','POINT',2);=======
);

------------------------------------------------------------------------
-- store code mapping table
CREATE TABLE store_code_mapping
(
  gid                   serial,
  org_store_code        integer not null primary key,
  org_store_name        character varying(64) not null,
  ddn_store_code        integer,
  ddn_store_code_hex    character varying(6),
  ddn_store_name        character varying(64),
  picture_name          character varying(64)
);

CREATE TABLE test_lane_tbl
(
  gid integer NOT NULL,
  nodeid bigint,
  inlinkid bigint,
  outlinkid bigint,
  arrowinfo smallint,
  inlink_rdb bigint,
  outlink_rdb bigint,
  turndir smallint
);

------------------------------------------------------------------------
-- facility name
CREATE TABLE mid_hwy_facility_name_for_towardname
(
  gid           serial NOT NULL primary key,
  nodeid        bigint NOT NULL,
  inlinkid      bigint NOT NULL,
  outlinkid     bigint NOT NULL,
  passlid       character varying(1024),
  passlink_cnt  smallint NOT NULL,
  facilcls      smallint not null,
  sp_name       character varying(4096)
);

------------------------------------------------------------------------
-- toll same facility
CREATE TABLE toll_same_facility
(
  gid             serial not null primary key,
  road_code1      integer not null,
  road_seq1       smallint not null,
  road_code2      integer not null,
  road_seq2       smallint not null
);

-- All links of SAPA Pathes
CREATE TABLE mid_temp_sapa_path_link(
  link_id      bigint not null primary key,
  link_type    smallint,
  toll_flag    smallint    -- 1: Hwy, 0: No toll section
);

-- SAPA & IC & UTURN links
CREATE TABLE temp_link_uturn_ic_sapa
(
  link_id bigint,
  type_array character varying[]
);

CREATE TABLE road_guidance_caution_tbl
(
gid serial,
Lon bigint not null,
Lat bigint not null,
in_node_meshid int not null, 
in_node_id int not null,
out_node_meshid int not null, 
out_node_id int not null,
safety_type varchar not null, 
road_name_dir varchar(1024), 
road_name_dir_yomi varchar(1024),
address varchar(1024), 
description varchar(1024)
);
CREATE TABLE temp_building_mapping_node
(
  gid serial NOT NULL,
  node_id bigint NOT NULL,
  the_geom geometry,
  country_road_f smallint,
  the_geom_circ_buffer geometry,
  CONSTRAINT enforce_dims_the_geom CHECK (st_ndims(the_geom) = 2),
  CONSTRAINT enforce_geotype_the_geom CHECK (geometrytype(the_geom) = 'POINT'::text OR the_geom IS NULL),
  CONSTRAINT enforce_srid_the_geom CHECK (st_srid(the_geom) = 4326)
);
CREATE TABLE t_logomark_4326
(
  gid serial NOT NULL,
  usml_genre_key_s integer,
  usml_data_grouping_code integer,
  usml_store_code integer,
  uint_sequence integer,
  geo_geom character varying,
  utny_poi_sequence integer,
  attr_code character varying,
  priority smallint,
  the_geom geometry,
  CONSTRAINT enforce_dims_the_geom CHECK (st_ndims(the_geom) = 2),
  CONSTRAINT enforce_geotype_the_geom CHECK (geometrytype(the_geom) = 'POINT'::text OR the_geom IS NULL),
  CONSTRAINT enforce_srid_the_geom CHECK (st_srid(the_geom) = 4326)
);
CREATE TABLE org_logmark_code_mapping
(
 gid serial NOT NULL,
 attr_code character varying,
 kiwicode character varying,
 name_str character varying
);
create table building_structure
(
gid serial not null,
id serial not null,
inlinkid bigint,
nodeid bigint,
outlinkid bigint,
type_code integer,
centroid_lontitude integer,
centroid_lantitude integer,
building_name character varying
);
create table t_logmark_priority_tbl(
	attr_code character varying,
	priority smallint
);

create table road_roundabout_link_tbl(
	mesh_id int,
	node1 int,
	node2 int
);