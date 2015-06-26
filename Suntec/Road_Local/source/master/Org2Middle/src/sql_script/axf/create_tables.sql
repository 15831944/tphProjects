-- include road name, road alias name, node signpost, cross signpost; but exclude road number, cross name
CREATE TABLE temp_org_rass_name
(
  gid serial not null primary key,
  id integer not null,              -- It is [road] for road name, It is [sp] for signpost
  meshid integer not null,
  name_chn character varying(128),
  name_trd character varying(128),
  name_py character varying(240),
  name_eng character varying(240),
  name_ctn character varying(240), 
  seq_nm smallint,
  name_type smallint not null        -- 1: road name, 2. road alias name, 3: node signpost, 4: cross signpost
);

CREATE TABLE temp_org_road_alias_name
(
  road integer not null,
  meshid integer not null,
  alias_chn character varying(64),
  alias_trd character varying(64),
  alias_py character varying(160),
  alias_eng character varying(160),
  alias_ctn character varying(160)
);

CREATE TABLE temp_rass_name
(
  gid integer not null primary key,
  new_name_id integer
);

CREATE TABLE temp_road_alias_name
(
  road integer not null primary key,
  new_name_id integer not null
);

CREATE TABLE temp_node_signpost_dict
(
  sp integer not null,
  meshid integer not null,
  new_name_id integer not null
);

CREATE TABLE temp_cross_signpost_dict
(
  sp integer not null,
  meshid integer not null,
  new_name_id integer not null
);

CREATE TABLE temp_org_node_name
(
  new_node bigint not null primary key,
  node integer not null,
  name_chn character varying(32),
  name_trd character varying(32),
  name_py character varying(64),
  name_eng character varying(64),
  name_ctn character varying(64),
  node_type smallint not null,    --1: node, 2: cross
  meshid integer not null
);

CREATE TABLE temp_node_name
(
  new_node bigint not null primary key,
  new_name_id integer not null
);

-- HWY InterChange name
CREATE TABLE temp_interchange_name
(
  gid         integer not null primary key,
  new_name_id integer not null
);

-- HWY SA/PA name
CREATE TABLE temp_sapa_name
(
  name        integer not null primary key,
  new_name_id integer not null
);

CREATE TABLE temp_road_number_old 
(
  gid serial not null primary key,
  road integer not null,
  meshid integer not null,
  route_no character varying(48) not null, 
  road_class integer not null,
  seq_nm smallint not null
);

CREATE TABLE temp_road_number
(
  road integer not null,
  meshid integer not null,
  new_name_id integer not null,
  seq_nm smallint not null
);

CREATE TABLE temp_org_signpost_exit_no
(
  sp integer not null,
  meshid integer not null,
  exitno_chn character varying(16),
  exitno_trd character varying(16),
  exitno_py character varying(64),
  exitno_eng character varying(64),
  exitno_ctn character varying(64),
  exit_type smallint not null            -- 3: node signpost, 4: cross signpost
);

CREATE TABLE temp_org_signpost_exit_no_split
(
  gid serial not null primary key,
  sp integer not null,
  meshid integer not null,
  exitno_chn character varying(16),
  exitno_trd character varying(16),
  exitno_py character varying(64),
  exitno_eng character varying(64),
  exitno_ctn character varying(64),
  seq_nm smallint not null,
  exit_type smallint not null            -- 3: node signpost, 4: cross signpost
);

CREATE TABLE temp_node_signpost_exitno_dict
(
  sp integer not null,
  meshid integer not null,
  new_name_id integer not null,
  seq_nm smallint not null
);

CREATE TABLE temp_cross_signpost_exitno_dict
(
  sp integer not null,
  meshid integer not null,
  new_name_id integer not null,
  seq_nm smallint not null
);

CREATE TABLE temp_org_signpost_entr_no
(
  gid serial not null primary key,
  sp integer not null,
  meshid integer not null,
  entr_no character varying(16),
  entr_type smallint not null  -- 3: node signpost, 4: cross signpost
);

CREATE TABLE temp_node_signpost_entrno_dict
(
  sp integer not null,
  meshid integer not null,
  new_name_id integer not null
);

CREATE TABLE temp_cross_signpost_entrno_dict
(
  sp integer not null,
  meshid integer not null,
  new_name_id integer not null
);

CREATE TABLE temp_poi_name
(
  old_name_id integer not null,
  meshid  integer not null,
  new_name_id integer not null
);

--------------------------------------------------------------------------------------------------------
-- Regulation
CREATE TABLE temp_condition_regulation_tbl
(
  cond_id integer,
  rule_type smallint,
  vehicle character varying(8),
  rule_time character varying(200)
);

CREATE TABLE temp_node_lid
(
  node_id bigint NOT NULL,
  lid bigint[],
  CONSTRAINT temp_node_lid_pkey PRIMARY KEY (node_id)
);

CREATE TABLE temp_node_lid_ref
(
  node_id bigint NOT NULL,
  lid bigint[],
  CONSTRAINT temp_node_lid_ref_pkey PRIMARY KEY (node_id)
);

CREATE TABLE temp_node_lid_nonref
(
  node_id bigint NOT NULL,
  lid bigint[],
  CONSTRAINT temp_node_lid_nonref_pkey PRIMARY KEY (node_id)
);

CREATE TABLE temp_crosslane_tbl_full_table
as 
(
	select maat, maat_id, node, from_road, to_road, lane_info_target_direction, all_lane_info, lane_no,
	lane_div, xlpath, xlpath_cnt, meshid from
	(
		select maat, maat_id, node, from_road, to_road, lane_info_target_direction, all_lane_info, lane_no,
		lane_div, xlpath, xlpath_cnt, meshid from
		(
			select A.maat, A.maat_id, A.node, A.from_road,A.to_road,A.lane_info as lane_info_target_direction, B.laneinfo as all_lane_info, C.lane_no,
			C.lane_div, C.xlpath, C.xlpath_cnt, A.meshid from org_roadcrossmaat as A
			left join org_roadcrosssaat as B on A.from_road = B.road and A.node = B.node and A.meshid = B.meshid
			left join org_extendlanecross as C on A.maat = C.maat and A.meshid = C.meshid
		) as D where lane_info_target_direction is not null
	) as E
);

CREATE TABLE temp_nodelane_tbl_full_table
as 
(
	select maat, maat_id, node, from_road, to_road, lane_info_target_direction, all_lane_info, lane_no,
	lane_div, xlpath, xlpath_cnt, meshid from
	(
		select maat, maat_id, node, from_road, to_road, lane_info_target_direction, all_lane_info, lane_no,
		lane_div, xlpath, xlpath_cnt, meshid from
		(
			select A.maat, A.maat_id, A.node, A.from_road,A.to_road,A.lane_info as lane_info_target_direction, B.laneinfo as all_lane_info, C.lane_no,
			C.lane_div, C.xlpath, C.xlpath_cnt, A.meshid from org_roadnodemaat as A
			left join org_roadnodesaat as B on A.from_road = B.road and A.node = B.node and A.meshid = B.meshid
			left join org_extendlanenode as C on A.maat = C.maat and A.meshid = C.meshid
		) as D where lane_info_target_direction is not null
	) as E
);

CREATE TABLE temp_lane_dircect_mapping
(
  lane_direction smallint,
  lane_array character(1)[]
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

-- Construct globl new_road value
create table temp_org_xlpath 
as
(
	select C.gid, C.xlpath, C.xlpath_id,C.globl_mesh_id,D.new_road, C.area_flag,C.reserved, C.meshid from
	(
	select A.gid, A.xlpath, A.xlpath_id, B.globl_mesh_id, A.road, A.area_flag, A.reserved, A.meshid from org_xlpath as A
	left join mesh_mapping_tbl as B
	on A.mesh = B.meshid_str
	) as C left join temp_link_mapping as D
	on C.road = D.road and C.globl_mesh_id = D.meshid
);

CREATE TABLE temp_node_id
(
  meshid integer,
  node integer,
  g_node bigint,
  x_coord double precision,
  y_coord double precision,
  x_coord_100 bigint,
  y_coord_100 bigint,
  x_coord_100_m bigint,
  y_coord_100_m bigint
); SELECT AddGeometryColumn('','temp_node_id','the_geom','4326','POINT',2);

CREATE TABLE temp_node_boundary
(
  g_node bigint,
  new_node bigint
);

CREATE TABLE temp_node_mapping
(
  meshid integer,
  node integer,
  node_id integer,
  g_node bigint,
  new_node bigint,
  boundary_flag smallint
); SELECT AddGeometryColumn('','temp_node_mapping','the_geom','4326','POINT',2);

CREATE TABLE temp_link_mapping
(
  meshid integer,
  road integer,
  road_id integer,
  new_road bigint,
  new_fnode bigint,
  new_tnode bigint
); SELECT AddGeometryColumn('','temp_link_mapping','the_geom','4326','LINESTRING',2);


CREATE TABLE temp_node_u
(
  meshid integer,
  node integer,
  new_node bigint
);

CREATE TABLE temp_node_boundary_patch
(
  mid integer,
  meshstr varchar,
  node integer,
  x_coord double precision,
  y_coord double precision,
  boundary integer
);

CREATE TABLE temp_admin_province
(
  ad_cd integer NOT NULL,
  ad_name character varying(254),
  ad_py character varying(254),
  CONSTRAINT temp_admin_province_pkey PRIMARY KEY (ad_cd)
);
    
CREATE TABLE temp_admin_city
(
  ad_cd integer NOT NULL,
  ad_name character varying(254),
  ad_py character varying(254),
  CONSTRAINT temp_admin_city_pkey PRIMARY KEY (ad_cd)
);

CREATE TABLE rdb_admin_province
(
  gid serial NOT NULL,
  ad_cd integer,
  ad_name character varying(254),
  ad_py character varying(254),
  CONSTRAINT rdb_admin_province_pkey PRIMARY KEY (gid),
  CONSTRAINT rdb_admin_province_ad_cd_key UNIQUE (ad_cd)
); 

CREATE TABLE rdb_admin_city
(
  gid serial NOT NULL,
  ad_cd integer,
  ad_name character varying(254),
  ad_py character varying(254),
  CONSTRAINT rdb_admin_city_pkey PRIMARY KEY (gid),
  CONSTRAINT rdb_admin_city_ad_cd_key UNIQUE (ad_cd)
);

--------------------------------------------------------
-- temp road and node info
create table temp_road_newid
as
(
	select	a.meshid, cast(a.road as integer), cast(a.road_id as integer),
			(cast(a.meshid as bigint) << 32) | cast(a.road as bigint) as new_road_id,
			cast(fnode as integer), cast(tnode as integer),
			direction as oneway_c,
			toll_flag as toll,
			status,
			road_class as road_class,
			form_way as roadtype_c,
			link_type as linktype_c,
			fc as function_class,
			navi_flag as navigable,
			ownership as ownership,
			speed_cate as speed_limit,
			length as link_length,
			width as link_width,
			urban as city_flag, over_head, 
			ad_bnd, ad_code, ad_codes,
			name_chn, alias_chn, route_no, 
			min_lanes, max_lanes, lane_wide,
			min_speed, max_speed, ave_speed,
			the_geom
	from org_roadsegment as a
	left join org_roadsegmentplus as b
	on a.meshid = b.meshid and a.road = b.road
);

create table temp_node_newid
as
(
	select	meshid, cast(node as integer), 
			(cast(meshid as bigint) << 32) | cast(node as bigint) as new_node_id,
			boundary, ad_bnd,
			cast(realnode as bigint) 
			| (cast(tollgate as bigint) << 16) 
			| (cast(nodetype as bigint) << 32)
			| (cast(signlight as bigint) << 48)
			as comp_attr,
			cast(comp_node as integer),
			name_chn, alias_chn,
			x_coord, y_coord,
			the_geom
	from org_roadnode
);

create table temp_crosslane_exception_lane_with_maat
as
(
select meshid,maat,array_agg(lane_no) as exception_lane_no_array from
(
select meshid, maat, lane_no 
from temp_crosslane_tbl_full_table order by meshid, maat, lane_no
) as A group by meshid, maat
);


create table temp_nodelane_exception_lane_with_maat
as
(
select meshid,maat,array_agg(lane_no) as exception_lane_no_array from
(
select meshid, maat, lane_no 
from temp_nodelane_tbl_full_table order by meshid, maat, lane_no
) as A group by meshid, maat
);



---------------------------------------------------------------------------
-- SAPA_IC (SAME)
---------------------------------------------------------------------------
CREATE TABLE mid_temp_sapa_ic_same
(
  ic              integer NOT NULL primary key,
  ic_no           integer NOT NULL,
  in_out_type     smallint,
  sapa            integer not null
);

---------------------------------------------------------------------------
-- SAPA_IC (NOT SAME)
---------------------------------------------------------------------------
CREATE TABLE mid_temp_sapa_ic_not_same
(
  ic              integer NOT NULL primary key,
  ic_no           integer NOT NULL,
  in_out_type     smallint,
  sapa            integer not null
);

---------------------------------------------------------------------------
-- SAPA_IC (NOT SAME)
---------------------------------------------------------------------------
CREATE TABLE mid_temp_sapa_ic_not_same_all
(
  ic              integer NOT NULL primary key,
  ic_no           integer NOT NULL,
  in_out_type     smallint,
  sapa            integer not null
);

---------------------------------------------------------------------------
-- HIGHWAY_IC (ic\mainnode\path\direct)
---------------------------------------------------------------------------
CREATE TABLE temp_from_ic_to_highwaymainline_path
(
  gid serial NOT NULL,
  icnode_id bigint,
  path character varying,
  mainnode bigint,
  direct integer
);

---------------------------------------------------------------------------
-- HIGHWAY_IC (ic\mainnode\type)
---------------------------------------------------------------------------
CREATE TABLE mid_mainline_ic_node_info
(
  gid serial NOT NULL,
  ic_id bigint,
  ic_node bigint,
  mainline_node bigint,
  ic_type smallint
);

