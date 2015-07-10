
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

--------------------------------------------------------------------------------------------------------
CREATE TABLE temp_trfcsign_type_wavid
(
  trfcsign_type integer primary key,
  wav_id integer NOT NULL
); 

--------------------------------------------------------------------------------------------------------
CREATE TABLE temp_trfcsign_type_picid
(
  trfcsign_type integer primary key,
  pic_id integer NOT NULL
); 

--------------------------------------------------------------------------------------------------------
CREATE TABLE temp_trfcsign_type_data_kind
(
  trfcsign_type integer primary key,
  data_kind integer NOT NULL
); 

--------------------------------------------------------------------------------------------------------
CREATE TABLE temp_admin_wavid
(
  ad_cd integer primary key,
  wav_id integer NOT NULL
); 

--------------------------------------------------------------------------------------------------------
CREATE TABLE temp_admin_picid
(
  ad_cd integer primary key,
  pic_id integer NOT NULL
); 

-----------------------------------------------------------------
CREATE TABLE temp_trfcsign_caution_tbl
(
  inlinkid bigint,
  nodeid bigint,
  type integer,
  outlinkid bigint,
  passlid character varying(1024),
  passlink_cnt smallint default 0,
  data_kind smallint,
  voice_id integer DEFAULT 0,
  strTTS varchar(65535),
  image_id integer DEFAULT 0
); SELECT AddGeometryColumn('','temp_trfcsign_caution_tbl','node_geom','4326','POINT',2); SELECT AddGeometryColumn('','temp_trfcsign_caution_tbl','inlink_geom','4326','LINESTRING',2); 

-----------------------------------------------------------------
CREATE TABLE temp_admin_caution_tbl
(
  inlinkid bigint,
  nodeid bigint,
  outlinkid bigint,
  passlid character varying(1024),
  passlink_cnt smallint default 0,
  data_kind smallint,
  voice_id integer DEFAULT 0,
  strTTS varchar(65535),
  image_id integer DEFAULT 0
); SELECT AddGeometryColumn('','temp_admin_caution_tbl','node_geom','4326','POINT',2); SELECT AddGeometryColumn('','temp_admin_caution_tbl','inlink_geom','4326','LINESTRING',2); SELECT AddGeometryColumn('','temp_admin_caution_tbl','outlink_geom','4326','LINESTRING',2);

-----------------------------------------------------------------
CREATE TABLE temp_order8_boundary
(
  ad_code integer,
  order8_id integer,
  
  order8_name character varying(65536),
  order2_id integer,
  order2_name character varying(65536),
  order1_id integer,
  
  order1_name character varying(65536),
  order0_id integer,
  order0_name character varying(65536)
); SELECT AddGeometryColumn('','temp_order8_boundary','order8_geom','4326','MULTILINESTRING',2); 

-----------------------------------------------------------------
CREATE TABLE temp_inode
(
  link_id bigint primary key,
  s_node bigint,
  e_node bigint,
  one_way_code smallint,
  link_type smallint NOT NULL DEFAULT (-1),
  b_node bigint,
  slocate double precision,
  elocate double precision
); SELECT AddGeometryColumn('','temp_inode','the_geom','4326','POINT',2);

-----------------------------------------------------------------
CREATE TABLE temp_guideinfo_boundary
(
  inlinkid bigint,
  nodeid bigint,
  outlinkid bigint,
  innode bigint,
  outnode bigint,
  out_adcd integer
);

create table temp_poi_category
(
	u_code bigint,
	gen_0 integer,
	gen_1 integer,
	gen_2 integer,
	level smallint,
	genre_is_brand bpchar,
	importance smallint,
	org_code character(28),
	nametype character(2),
	name_lang character(3),
	name character varying(128),
	tr_lang character(3),
	tr_name character varying(128),
	chain_lang bpchar,
	chain_name character varying,
	chain_trlang bpchar,
	chain_trname character varying,
	chaincode bpchar,
	ph_type character varying(5),
	ph_lang character(3),
	ph_name character varying(128),
	logmark varchar(128) not null default ''
);

create table temp_brand_icon
(
    brandname    varchar(128) not null default ''
);

create table temp_category_priority
(
    u_code          character varying(1024)   not null,
    category_priority    int   
 
);

CREATE TABLE temp_poi_logmark
(
  poi_id character varying(13) not null, 
  kind character varying(4),
  per_code bigint not null,
  display_x character varying(12),
  display_y character varying(12),
  the_geom  geometry 
);

create table temp_poi_name
(
	poi_id character varying(13),
	poi_name varchar
);

CREATE TABLE temp_condition_regulation_tbl
(
  cond_id integer,
  vehcl_type varchar,
  vperiod varchar
);

-- link name
CREATE TABLE temp_link_name
(
  link_id bigint NOT NULL primary key,
  name    character varying(8192)
);

-- link shield
CREATE TABLE temp_link_shield
(
  link_id bigint NOT NULL primary key,
  shield  character varying(2048)
);
------------------------------------------------------------------------------
CREATE TABLE temp_admin_order0
(
  order0_id character varying(10) PRIMARY KEY,
  order0_code character varying(10),
  country_code character varying(10)
);SELECT AddGeometryColumn('public', 'temp_admin_order0', 'the_geom', 4326, 'MULTIPOLYGON', 2);

CREATE TABLE temp_admin_order1
(
  order1_id character varying(10) PRIMARY KEY,
  order0_id character varying(10),
  country_code character varying(10)
);SELECT AddGeometryColumn('public', 'temp_admin_order1', 'the_geom', 4326, 'MULTIPOLYGON', 2);

CREATE TABLE temp_admin_order2
(
  order2_id character varying(10) PRIMARY KEY,
  order1_id character varying(10),
  country_code character varying(10)
);SELECT AddGeometryColumn('public', 'temp_admin_order2', 'the_geom', 4326, 'MULTIPOLYGON', 2);

CREATE TABLE temp_admin_order8
(
  order8_id character varying(10) PRIMARY KEY,
  order2_id character varying(10),
  country_code character varying(10)
);SELECT AddGeometryColumn('public', 'temp_admin_order8', 'the_geom', 4326, 'MULTIPOLYGON', 2);

CREATE TABLE temp_admin_name
(
	admin_id character varying(10) PRIMARY KEY,
	admin_name character varying(65536)
);
CREATE TABLE temp_admin_municipalities
(
	admin_id character varying(10) PRIMARY KEY
);
-----------------------------------------------------------------------------------------------------
--signpost_uc
CREATE TABLE temp_signpost_uc_path
as
(
	select row_number() over(order by nodeid, inlinkid, outlinkid, passlid ) as path_id, 
		nodeid, inlinkid, outlinkid, passlid, array_agg(id||'|'||type) as id_array
	from
	(
		select id, nodeid, inlinkid, outlinkid, 
			(case when passlid2 is null or length(passlid2) < 1 then passlid
			else passlid ||'|'||passlid2 end) as passlid, 2 as type
		from org_br
		
		union
		
		select id, nodeid, inlinkid, outlinkid, passlid, 3 as type
		from org_ic
		
		union
		
		select id, nodeid, inlinkid, outlinkid, 
		 (case when passlid2 is null or length(passlid2) < 1 then passlid
			else passlid ||'|'||passlid2 end) as passlid, 4 as type
		from org_dr
	)temp
	group by nodeid, inlinkid, outlinkid, passlid
	order by nodeid, inlinkid, outlinkid, passlid 
);

CREATE TABLE temp_signpost_uc_path_id
as
(
	select id_array[1] as fname_id, id_array[2] as fname_type, path_id
	from
	(
		select path_id, string_to_array(unnest(id_array),'|') as id_array
		from temp_signpost_uc_path
	)temp
);
CREATE TABLE temp_signpost_uc_name
(
  sign_id            bigint NOT NULL PRIMARY KEY,
  signpost_name      character varying(8192),
  route_no1          character varying(1024),
  route_no2          character varying(1024),
  route_no3          character varying(1024),
  route_no4          character varying(1024),
  exit_no            character varying(1024)
);

CREATE TABLE temp_node_mapping
as
(
	select id as old_node_id , adjoin_nid as new_node_id
	from org_n where position('1f00' in kind)<>0
	and id::bigint>adjoin_nid::bigint
);

------------------------------------------------------------------------
CREATE TABLE temp_node_z_level_tbl
(
 node_id character varying NOT NULL PRIMARY KEY,
 z_level smallint default 0
); 

------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------
CREATE TABLE mid_temp_hwy_exit_enter_poi_name_ni
(
   node_id         bigint not null,
   poi_id          bigint not null,
   kind            character varying(8) not null,     -- Enter/Exit
   name            character varying(1024) not null   --JSON
);


CREATE TABLE temp_poi_inlink
(
poi_id  bigint,
inlink  bigint,
node    bigint
);

create table temp_towardname_name
(
	poi_id bigint,
	toward_name varchar
);

create table temp_lane_exclusive
as
(
	select gid,case when length(replace(laneinfo_all,'0',''))=length(replace(laneinfo_part,'0','')) then 0
		else 1 end as exclusive from
	(
		select *,lpad((laneinfo_all::bigint-laneinfo::bigint)::text,lanenum,'0') as laneinfo_part from 
		(
			select nodeid,inlinkid,lanenum,lpad(sum(laneinfo::bigint)::text,lanenum,'0') as laneinfo_all from (
				select distinct nodeid,inlinkid,arrowinfo,laneinfo,lanenum from lane_tbl
			) a group by nodeid,inlinkid,lanenum
		)  a
		join lane_tbl b
		using(nodeid,inlinkid,lanenum)
	) a
);