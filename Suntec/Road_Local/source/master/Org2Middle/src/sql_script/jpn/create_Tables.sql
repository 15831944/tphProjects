------------------------------------------------------------------------------------------------
-- for org data
------------------------------------------------------------------------------------------------
CREATE TABLE road_rlk
(
  gid serial NOT NULL,
  objectid integer,
  fromnodeid integer,
  tonodeid integer,
  roadcls_c integer,
  navicls_c integer,
  linkcls_c integer,
  manager_c integer,
  width_c integer,
  widearea_f integer,
  island_f integer,
  bypass_f integer,
  caronly_f integer,
  roadno integer,
  roadcode integer,
  lanecount integer,
  nopass_c integer,
  oneway_c integer,
  CONSTRAINT road_rlk_pkey PRIMARY KEY (gid)
);SELECT AddGeometryColumn('','road_rlk','the_geom','4326','MULTILINESTRING',2);

CREATE TABLE road_rnd
(
  gid serial NOT NULL,
  objectid integer,
  signal_f integer,
  name_kanji character varying(128),
  name_yomi character varying(128),
  CONSTRAINT road_rnd_pkey PRIMARY KEY (gid)
);SELECT AddGeometryColumn('','road_rnd','the_geom','4326','POINT',2);

CREATE TABLE road_rsa
(
  gid serial not null,
  objectid integer,
  linkid integer,
  linkattr_c integer,
  CONSTRAINT road_rsa_pkey PRIMARY KEY (gid)
); SELECT AddGeometryColumn('','road_rsa','the_geom','4326','MULTILINESTRING',2);

CREATE TABLE topmap_ant
(
  gid serial NOT NULL,
  objectid integer,
  annocls_c integer,
  namestr1 character varying(128),
  namestr2 character varying(128),
  namestr3 character varying(128),
  CONSTRAINT topmap_ant_pkey PRIMARY KEY (gid)
); SELECT AddGeometryColumn('','topmap_ant','the_geom','4326','POINT',2);

CREATE TABLE topmap_xda
(
  gid serial NOT NULL,
  annoid integer,
  scale integer,
  nameid integer,
  lineanno_f integer,
  fontsize_c integer,
  CONSTRAINT topmap_xda_pkey PRIMARY KEY (gid)
);SELECT AddGeometryColumn('','topmap_xda','the_geom','4326','MULTILINESTRING',2);

CREATE TABLE middlemap_ant
(
  gid serial NOT NULL,
  objectid integer,
  annocls_c integer,
  namestr1 character varying(128),
  namestr2 character varying(128),
  namestr3 character varying(128),
  CONSTRAINT middlemap_ant_pkey PRIMARY KEY (gid)
); SELECT AddGeometryColumn('','middlemap_ant','the_geom','4326','POINT',2);

CREATE TABLE middlemap_xda
(
  gid serial NOT NULL,
  annoid integer,
  scale integer,
  nameid integer,
  lineanno_f integer,
  fontsize_c integer,
  CONSTRAINT middlemap_xda_pkey PRIMARY KEY (gid)
);  SELECT AddGeometryColumn('','middlemap_xda','the_geom','4326','MULTILINESTRING',2);

CREATE TABLE citymap_ant
(
  gid serial NOT NULL,
  objectid integer,
  annocls_c integer,
  namestr1 character varying(128),
  namestr2 character varying(128),
  CONSTRAINT citymap_ant_pkey PRIMARY KEY (gid)
); SELECT AddGeometryColumn('','citymap_ant','the_geom','4326','POINT',2);

CREATE TABLE citymap_xda
(
  gid serial NOT NULL,
  annoid integer,
  scale integer,
  nameid integer,
  lineanno_f integer,
  fontsize_c integer,
  CONSTRAINT citymap_xda_pkey PRIMARY KEY (gid)
);  SELECT AddGeometryColumn('','citymap_xda','the_geom','4326','MULTILINESTRING',2);

CREATE TABLE basemap_ant
(
  gid serial NOT NULL,
  objectid integer,
  annocls_c integer,
  namestr1 character varying(128),
  namestr2 character varying(128),
  namestr3 character varying(128),
  CONSTRAINT basemap_ant_pkey PRIMARY KEY (gid)
); SELECT AddGeometryColumn('','basemap_ant','the_geom','4326','POINT',2);

CREATE TABLE basemap_xda
(
  gid serial NOT NULL,
  annoid integer,
  scale integer,
  nameid integer,
  lineanno_f integer,
  fontsize_c integer,
  CONSTRAINT basemap_xda_pkey PRIMARY KEY (gid)
); SELECT AddGeometryColumn('','basemap_xda','the_geom','4326','MULTILINESTRING',2);


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

CREATE TABLE road_etc_sp
(
  gid serial PRIMARY KEY,
  objectid serial NOT NULL,
  roadname character varying(128),
  roaddir character varying(20),
  facname character varying(128),
  nodeid integer,
  picname character varying(128)
);

CREATE TABLE road_etc_link
(
  gid serial PRIMARY KEY,
  objectid serial NOT NULL,
  typecode integer,
  snodeid integer,
  midnodeid character varying(128),
  enodeid integer
);

------------------------------------------------------------------------------------------------
-- create tables for ipc
------------------------------------------------------------------------------------------------
CREATE TABLE temp_node_lane
(
  gid integer not null,
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

CREATE TABLE temp_node_signpost
(
  gid integer not null,
  fromlinkid integer,
  nodeid integer,
  tolinkid integer,
  midcount integer,
  midlinkid character varying(254),
  guideclass integer,
  name_id integer,
  name_kanji character varying(128),
  name_yomi character varying(128),
  boardptn_c integer,
  board_seq integer,
  from_nodeid integer
);

CREATE TABLE temp_node_signpost_midcount
(
  gid integer not null,
  midcount integer
);

CREATE TABLE temp_node_signpost_name
(
  gid integer not null,
  name_id integer,
  name_jp character varying(64),
  name_yomi character varying(64)
);

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
  guideclass integer,
  name_kanji character varying(128),
  name_yomi character varying(128),
  boardptn_c integer,
  board_seq integer,
  from_nodeid integer
);


CREATE TABLE temp_routeid_nameid
(
  route_id bigint NOT NULL,
  name_id bigint NOT NULL,
  CONSTRAINT temp_routeid_nameid_pkey PRIMARY KEY (route_id)
);

CREATE TABLE temp_tid_nameid
(
  maptype character varying(10) NOT NULL,
  ant_id integer NOT NULL,
  inner_name_id integer,
  name_id bigint
);

CREATE TABLE temp_name_shape
as (
    SELECT  roadcode
      , generate_series(1, rdb_NumGeometries(shape)) as seq_num
      , rdb_GeometryN(shape, generate_series(1, rdb_NumGeometries(shape))) AS the_geom --MultiLineString ==> LineString
      , roadtypes[1] as roadtype
      , func_classes
      FROM (
        SELECT   roadcode
               , ST_LineMerge(ST_Union(array_agg(geom))) as shape     -- Union the LineString which has the same name
               , array_agg(roadtype) as roadtypes
               , array_agg(zl_test_function_type(navicls_c, widearea_f)) as func_classes
         FROM (
            SELECT road_roadcode.roadcode, road_roadcode.roadtype, linkcls_c, navicls_c, ST_LineMerge(the_geom) as geom
              FROM road_roadcode
              INNER JOIN road_rlk
              ON road_roadcode.roadcode = road_rlk.roadcode
              ) AS a
         WHERE linkcls_c != 4 -- exclude the Intersection Link
         group by roadcode, roadtype
      ) AS b
);

CREATE TABLE temp_name_shape_intersectionlink
as 
(
   SELECT  roadcode
      , generate_series(1, rdb_NumGeometries(shape)) as seq_num
      , rdb_GeometryN(shape, generate_series(1, rdb_NumGeometries(shape))) AS the_geom 
      , roadtypes[1] as roadtype
      , func_classes
      FROM 
      (    
	        SELECT   roadcode   
	               , ST_AsText(ST_LineMerge((ST_Union(array_agg(geom))))) as shape     
	               , array_agg(roadtype) as roadtypes
	               , array_agg(zl_test_function_type(navicls_c, widearea_f)) as func_classes
	        FROM 
	        (
	               SELECT in_link.roadcode, in_link.roadtype, navicls_c, ST_LineMerge(the_geom) as geom
	               FROM 
	               ( 
	                   SELECT roadcode, roadtype
	                   FROM road_roadcode
	                   WHERE roadcode not in 
	                   					  (
	                                          SELECT distinct(roadcode)
	                                          FROM temp_name_shape
	                                      )
	                ) as in_link
	                INNER JOIN road_rlk
	                ON in_link.roadcode = road_rlk.roadcode
	        ) AS a
	        group by roadcode, roadtype
      ) AS b
);

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

CREATE TABLE temp_road_rnd
(
  gid serial NOT NULL,
  objectid integer,
  signal_f integer,
  name_kanji character varying(128),
  name_yomi character varying(128),
  new_add boolean DEFAULT false,
  CONSTRAINT temp_road_rnd_pkey PRIMARY KEY (gid)
);SELECT AddGeometryColumn('','temp_road_rnd','the_geom','4326','POINT',2);

CREATE TABLE temp_road_rlk
(
  gid serial NOT NULL,
  objectid integer,
  fromnodeid integer,
  tonodeid integer,
  roadcls_c integer,
  navicls_c integer,
  linkcls_c integer,
  manager_c integer,
  width_c integer,
  widearea_f integer,
  island_f integer,
  bypass_f integer,
  caronly_f integer,
  roadno integer,
  roadcode integer,
  lanecount integer,
  nopass_c integer,
  oneway_c integer,
  linkattr_c integer DEFAULT 0, -- 0: does not have attribute, 2: tunnel, 4: railway crossing.
  split_seq_num integer DEFAULT (-1), -- -1: has not been split, [0,N]: Sequence number.
  CONSTRAINT temp_road_rlk_gid_pkey PRIMARY KEY (gid)
); SELECT AddGeometryColumn('','temp_road_rlk','the_geom','4326','LINESTRING',2);

CREATE TABLE temp_link_name_dictionary_tbl
(
	name_id integer,
	name character varying,
	py character varying,
	seq_nm smallint,
	language smallint,
	roadcode integer
);

CREATE TABLE temp_link_name_dictionary_tbl
(
  name_id integer,
  "name" character varying,
  py character varying,
  seq_nm smallint,
  "language" smallint,
  roadcode integer
);

CREATE TABLE temp_link_no_dictionary_tbl
(
  name_id integer,
  "name" character varying,
  py character varying,
  seq_nm smallint,
  "language" smallint,
  objectid integer
);

CREATE TABLE temp_node_name_dictionary_tbl
(
  name_id integer,
  "name" character varying,
  py character varying,
  seq_nm smallint,
  "language" smallint,
  objectid integer
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

CREATE TABLE middle_tile_node
(
  tile_node_id bigint NOT NULL,
  old_node_id bigint NOT NULL,
  tile_id integer NOT NULL,
  CONSTRAINT middle_tile_node_pkey PRIMARY KEY (old_node_id)
);

-- Dictionary and relation table
CREATE TABLE temp_link_name
(
  road_code integer,
  name_id integer
);

CREATE TABLE temp_link_no
(
  link_id bigint not null primary key,
  name_id integer not null
);

CREATE TABLE temp_node_name
(
  objectid integer primary key,
  name_id integer
);

CREATE TABLE temp_toward_name
(
  gid integer,
  name_id integer
);

CREATE TABLE temp_signpost_name
(
  gid integer,
  name_id integer
);

CREATE TABLE temp_poi_name
(
  layer_name character varying(30),
  objectid integer,
  seq_nm integer,
  name_id integer
);

CREATE TABLE temp_link_number_group
(
  gid serial not null primary key,
  linkid_array bigint[] not null,
  shield_id_num character varying(30) not null
);

CREATE TABLE temp_link_number_groupid_nameid
(
  gid integer not null primary key,
  name_id integer not null
);

CREATE TABLE temp_condition_regulation_tbl
(
  cond_id integer,
  from_month integer,
  from_day integer,
  to_month integer,
  to_day integer,
  from_hour integer,
  from_min integer,
  to_hour integer,
  to_min integer,
  dayofweek integer
);

CREATE TABLE temp_node_name_and_illust_name
(
  node_id integer not null primary key,
  name_kanji character varying(128),
  name_yomi character varying(128)
);

CREATE TABLE temp_node_name_and_illust_name_groupby
(
  groupid serial not null,
  node_ids integer[],
  name_kanji character varying(128),
  name_yomi character varying(128)
);

CREATE TABLE temp_node_name_and_illust_name_groupid_nameid
(
  groupid integer,
  name_id integer
);

CREATE TABLE temp_road_illust
(
  gid integer,
  objectid integer,
  roadname character varying(128),
  roaddir character varying(20),
  facname character varying(128),
  nodelist character varying(254),
  pictype integer,
  picname character varying(128),
  node_id integer
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

CREATE TABLE temp_admin_province
(
  ad_cd integer,
  ad_name character varying(64),
  ad_py character varying(64)
);

CREATE TABLE rdb_admin_province
(
  ad_cd integer,
  ad_name character varying(64),
  ad_py character varying(64)
);SELECT AddGeometryColumn('','rdb_admin_province','the_geom','4326','POLYGON',2);

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
--------------------------------------------------------------------------------------
CREATE TABLE mid_road_roadpoint_node
(
  gid integer NOT NULL primary key,
  node_id integer,
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
); SELECT AddGeometryColumn('','mid_road_roadpoint_node','the_geom','4326','POINT',2);

--------------------------------------------------------------------------------------
CREATE TABLE mid_road_roadpoint_bunnki_node
(
  gid integer NOT NULL primary key,
  node_id integer,
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
); SELECT AddGeometryColumn('','mid_road_roadpoint_bunnki_node','the_geom','4326','POINT',2);

-----------------------------------------------------------------------------------------------
-- fac name and full path
CREATE TABLE mid_fac_name_full_path
(
  gid serial NOT NULL primary key,
  node_id integer,
  mesh_code integer,
  road_code integer,
  seq_num integer,
  pos_code integer,
  updown_flag integer,
  fac_type integer  not null,
  fac_name character varying(60) not null,
  in_link_id bigint,
  path character varying[],
  to_node_id bigint,
  path_seq_num integer
);

------------------------------------------------------------------------------------------------
-- fac name and shoot path
CREATE TABLE mid_fac_name
(
  gid serial NOT NULL primary key,
  id integer[],
  nodeid bigint not null,
  inlinkid bigint not null,
  outlinkid bigint not null,
  passlid character varying(512),
  passlink_cnt smallint,
  pos_code integer not null,
  fac_type integer not null,
  fac_name character varying(60) not null,
  tonode_array bigint[] not null
);

------------------------------------------------------------------------------------------------
-- facility name id table
CREATE TABLE mid_temp_fac_name_id
(
  gid integer NOT NULL primary key,
  new_name_id integer
);

---------------------------------------------------------------------------
-- toll node name group 
---------------------------------------------------------------------------
create table mid_temp_toll_node_name_group
(
  gid serial not null primary key,
  node_array bigint[],
  name character varying,
  name_yomi character varying
);

---------------------------------------------------------------------------
-- toll node name 
---------------------------------------------------------------------------
CREATE TABLE mid_temp_toll_node_name
(
  gid integer not null primary key,
  new_name_id integer not null
);

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