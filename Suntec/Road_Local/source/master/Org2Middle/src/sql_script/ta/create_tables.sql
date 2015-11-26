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

create table temp_mid_shield_number
(
  gid           serial not null primary key,
  link_id       bigint not null,
  --shield_id     smallint not null,
  new_name_id   integer not null,  -- shield_id and shield number
  rteprior      smallint not null,
  seqnm         smallint not null
);

-------------------------------------------------------------------------------------------------
-- road name
-------------------------------------------------------------------------------------------------
CREATE TABLE temp_mid_road_name
(
  link_id       bigint not null,
  name_type     smallint not null,
  name          character varying,
  language_code character varying,
  new_name_id   integer not null
);

-------------------------------------------------------------------------------------------------
-- link country (for road name/number)
-------------------------------------------------------------------------------------------------
CREATE TABLE temp_link_country
(
  link_id bigint not null primary key,
  country_code character varying
);

-------------------------------------------------------------------------------------------------
-- TowardName 
-------------------------------------------------------------------------------------------------
CREATE TABLE temp_towardname_si
(
  gid integer NOT NULL primary key,
  id double precision,
  seqnr integer,
  destseq integer,
  infotyp character varying(2),
  rnpart smallint,
  contyp smallint,
  new_name_id integer not null
);

-------------------------------------------------------------------------------------------------
-- TowardName Path Links
-------------------------------------------------------------------------------------------------
CREATE TABLE temp_towardname_path_links
(
  id bigint not null primary key,
  in_link_id bigint not null,
  out_link_id bigint not null,
  passlid character varying(1024),
  passlink_cnt smallint
);

--------------------------------------------------------------------------------------------------------
-- Regulation
--------------------------------------------------------------------------------------------------------
CREATE TABLE temp_condition_regulation_tbl
(
  cond_id integer,
  car_type smallint,
  timedom character varying(254)
);


-------------------------------------------------------------------------------------------------
-- link & node
-------------------------------------------------------------------------------------------------

CREATE TABLE temp_jc
(
  id double precision primary key,
  feattyp smallint,
  jncttyp smallint,
  elev smallint,
  geom geometry
);

CREATE TABLE temp_dir_pos
(
  id double precision PRIMARY KEY,
  dir_pos smallint
);

CREATE TABLE temp_node_lid
(
  node_id double precision NOT NULL,
  lid varchar,
  CONSTRAINT temp_node_lid_pkey PRIMARY KEY (node_id)
);

CREATE TABLE temp_node_lid_ref
(
  node_id double precision NOT NULL,
  lid double precision[],
  CONSTRAINT temp_node_lid_ref_pkey PRIMARY KEY (node_id)
);

CREATE TABLE temp_node_lid_nonref
(
  node_id double precision NOT NULL,
  lid double precision[],
  CONSTRAINT temp_node_lid_nonref_pkey PRIMARY KEY (node_id)
);

-------------------------------------------------------------------------------------------------
--   get speed limit.
-------------------------------------------------------------------------------------------------
create table temp_link_pos_cond_speed 
as
( 
	select sp.*,a0.munit from (
		select id,  max(speed) as speed,max(verified)  as verified,array_agg(speedtyp) as speedtyp 
		from org_sr
		where valdir in (1,2)
		group by id
	) sp
	left join (
		select distinct id,areid from org_ta where aretyp = 1111
	) gc
	on sp.id = gc.id
	left join (
		select distinct id,munit from org_a0 where name is not null and name <> 'Outer World'
	) a0
	on gc.areid = a0.id 
);

create table temp_link_neg_cond_speed 
as
(
	select sp.*,a0.munit from (                
		select id,  max(speed) as speed,max(verified)  as verified,array_agg(speedtyp) as speedtyp 
		from org_sr
		where valdir in (1,3)
		group by id
	) sp
	left join (
		select distinct id,areid from org_ta where aretyp = 1111
	) gc
	on sp.id = gc.id
	left join (
		select distinct id,munit from org_a0 where name is not null and name <> 'Outer World'
	) a0
	on gc.areid = a0.id 
);  

-------------------------------------------------------------------------------------------------
--   find record which given the path connect relationship, meanwhile
--   the start link has been given the lane arrow info
-------------------------------------------------------------------------------------------------
CREATE TABLE temp_guide_lane_needed 
as 
(
	select id , seqnr, trpelid, trpeltyp from lp where id in
	(
		  select id from lp where seqnr = 1 and trpelid in 
		  (
			select distinct(id) from ld
		  )
	) order by id, seqnr
);

-------------------------------------------------------------------------------------------------
--   get start link's all accessible lane
-------------------------------------------------------------------------------------------------
Create Table temp_access_lane_of_start_link 
as
(
	select id, min(jnctid) as jnctid,array_agg(distinct(start_link_accessible_lane)) as start_link_accessible_lanes 
	from
	(
		select gid, id, jnctid, mid_get_start_link_accessible_lane(fromto) as start_link_accessible_lane 
		from ln 
		order by id, start_link_accessible_lane
	) as A group by id 
);


Create table temp_arrow_lane_mapping_tbl 
as
(
	select id, direction, validity, mid_get_arrow_map_lane_no(validity) as get_arrow_map_lane_no
	from
	(
		select id, seqnr, direction, validity 
		from ld 
		order by id, validity desc
	) as A 
);

-------------------------------------------------------------------------------------------------
--   make complete relation table of guide_lane 
-------------------------------------------------------------------------------------------------
Create Table temp_guide_lane_tbl 
as
(
	select B.id, C.jnctid as nodeid, passlinks[1] as inlinkid, passlinks[total_pass_cnt] as outlinkid, 
		passlinks, total_pass_cnt, C.start_link_accessible_lanes, D.lanes as lanenum,
		E.direction, E.validity, E.get_arrow_map_lane_no
		from 
		(
			select id, array_agg(trpelid) as passlinks, count(id) as total_pass_cnt 
			from 
			(
				select id, trpelid 
				from temp_guide_lane_needed 
				order by id, seqnr
			) as A 
			group by id
		) as B left join temp_access_lane_of_start_link as C
			on B.id = C.id
		       left join nw as D
			on B.passlinks[1] = D.id
		       left join temp_arrow_lane_mapping_tbl as E
			on D.id = E.id and C.start_link_accessible_lanes @> E.get_arrow_map_lane_no
		order by inlinkid, outlinkid, total_pass_cnt
);


-------------------------------------------------------------------------------------------------
--   make temp_rs for direction of traffic flow and linkrow restriction
-------------------------------------------------------------------------------------------------
CREATE TABLE temp_regulation_forbid
(
  id double precision,
  dir_pos smallint,
  timedom character varying(254),
  vt_array smallint[],
  link_oneway integer
);
CREATE TABLE temp_regulation_permit
(
  id double precision,
  dir_pos smallint,
  timedom character varying(254),
  vt_array smallint[],
  link_oneway integer
);
CREATE TABLE temp_rs_df_todo
(
  id double precision,
  dir_pos smallint,
  timedom character varying(254),
  vt_array smallint[]
);
CREATE TABLE temp_rs_linkrow_todo
(
  id double precision,
  dir_pos smallint,
  timedom character varying(254),
  vt_array smallint[]
);

create table temp_country_link
as
(
	select id,cast (cast(id / 10000000000 as integer)%1000 as integer) as country_id,f_jnctid,f_jncttyp,t_jnctid,t_jncttyp,oneway
	from org_nw
	where f_jncttyp = 4 or t_jncttyp = 4
);
create table temp_country_node
as
(
	select id
	from org_jc
	where jncttyp = 4
);
CREATE TABLE temp_link_regulation_forbid_permit
(
  gid serial,
  linkid bigint,
  condtype smallint,
  cond_forbid_peumit smallint
);

CREATE TABLE temp_link_regulation_forbid_traffic
as
(
	select distinct link_id, linkdir
	from
	(
		select linkid as link_id, 1::smallint as linkdir
		from temp_link_regulation_forbid_permit
		where condtype = 4 and cond_forbid_peumit = 0
		union
		select linkid as link_id, 2::smallint as linkdir
		from temp_link_regulation_forbid_permit
		where condtype = 42 and cond_forbid_peumit = 0
		union
		select linkid as link_id, 3::smallint as linkdir
		from temp_link_regulation_forbid_permit
		where condtype = 43 and cond_forbid_peumit = 0
	)as t
	order by link_id
);

CREATE TABLE temp_link_regulation_permit_traffic
as
(
		select link_id, array_agg(linkdir) as linkdir_array
		from
		(
			select distinct link_id, linkdir
			from
			(
				select linkid as link_id, 2::smallint as linkdir
				from temp_link_regulation_forbid_permit
				where condtype = 2 and cond_forbid_peumit = 1
				union
				select linkid as link_id, 3::smallint as linkdir
				from temp_link_regulation_forbid_permit
				where condtype = 3 and cond_forbid_peumit = 1
			)as t
		)as t2
		group by link_id
);
create table temp_ta_jv_tbl
(
gid serial primary key,
image_name varchar(128),
jdn_id bigint,
pass_node_link_list varchar[]
);

------------------------------------------------------------------------------
-- Middle route number 
------------------------------------------------------------------------------
create table temp_mid_link_route_number
(
  gid             serial not null primary key,
  link_id         bigint not null,
  route_num       character varying,
  lang_type       character varying(3), -- language type
  rteprior        smallint not null,
  new_name_prior  smallint not null,
  second_prior    smallint not null
);

------------------------------------------------------------------------------
-- shield table
------------------------------------------------------------------------------
create table temp_mid_link_shield
(
  gid           serial not null primary key,
  link_id       bigint not null,
  shield_id     smallint not null,
  shield_num    character varying,
  route_num     character varying,
  rteprior      smallint not null,
  seqnm         smallint not null
);

------------------------------------------------------------------------------
-- Middle link name (exclude Route Number)
------------------------------------------------------------------------------
create table temp_mid_link_name
(
  gid              serial not null primary key,
  link_id          bigint not null,
  name             character varying,
  lang_type        character varying(3), -- language type
  new_name_prior   smallint not null,
  second_prior     smallint not null
);

------------------------------------------------------------------------------
-- Middle link name (include Route Number)
------------------------------------------------------------------------------
create table temp_mid_link_name_number
(
  gid              serial not null primary key,
  link_id          bigint not null,
  name             character varying,
  lang_type        character varying(3), -- language type
  new_name_prior   smallint not null,
  second_prior     smallint not null,
  seq_num          smallint not null
);

------------------------------------------------------------------------------
-- Street Name and Link Name are merged
------------------------------------------------------------------------------
create table temp_mid_link_merged_name
(
 link_id     bigint not null primary key, 
 "name"      character varying(512), 
 lang_type   character varying(3)
);

------------------------------------------------------------------------------
-- SignPost Exit NO.
------------------------------------------------------------------------------
create table temp_mid_signpost_exit_no
(
  id bigint not null primary key,
  exit_no integer not null
);

------------------------------------------------------------------------------
-- Merged SignPost Name
------------------------------------------------------------------------------
create table temp_mid_merged_signpost_name
(
  id bigint     not null primary key,
  signpost_name character varying(512) not null,
  language_type character varying(3)
);

------------------------------------------------------------------------------
-- SignPost Name
------------------------------------------------------------------------------
create table temp_mid_signpost_name
(
  id bigint     not null primary key,
  signpost_name integer not null
);

------------------------------------------------------------------------------
-- SignPost Merged Route NO
------------------------------------------------------------------------------
create table temp_mid_signpost_merged_route_no
(
  gid              serial not null primary key,
  id               bigint not null,
  shield_id        character varying(4),
  shield_num       character varying(8),
  direction        character varying(2),
  merged_route_no  character varying(16)
);

------------------------------------------------------------------------------
-- SignPost Route NO
------------------------------------------------------------------------------
create table temp_mid_signpost_route_no
(
  id               bigint not null primary key,
  join_route_no    character varying(512) not null,
  route_no         integer not null
);

-------------------------------------------------------------------------------------------------
-- SignPost Path Links
-------------------------------------------------------------------------------------------------
CREATE TABLE mid_temp_signpost_passlink
(
  sign_id bigint not null primary key,
  in_link_id bigint not null,
  out_link_id bigint not null,
  passlid character varying(1024),
  passlink_cnt smallint
);

CREATE TABLE jv_si
(
  gid serial NOT NULL primary key,
  id bigint,
  seqnr integer,
  destseq integer,
  infotyp character varying(10),
  shield int,
  txtcont character varying(70)
);

CREATE TABLE jv
(
  gid serial NOT NULL primary key,
  id bigint,
  jvtyp smallint,
  complex smallint,
  lat double precision,
  lon double precision,
  f_trpeltyp character varying(10),
  f_trpelid_mnl bigint,
  f_trpeldset_gdf character varying(20),
  f_trpelsect_gdf character varying(20),
  f_trpelid_gdf character varying(20),
  jnctid_mnl bigint,
  jnctdset_gdf character varying(20),
  jnctsect_gdf character varying(20),
  jnctid_gdf character varying(20),
  t_trpeltyp character varying(10),
  t_trpelid_mnl bigint,
  t_trpeldset_gdf character varying(20),
  t_trpelsect_gdf character varying(20),
  t_trpelid_gdf character varying(20),
  background character varying(30),
  mirroring smallint,
  road character varying(30),
  arrow character varying(30),
  sign_color smallint,
  text_color smallint
);

CREATE TABLE jv_ld
(
  gid serial NOT NULL primary key,
  id bigint,
  seqnr integer,
  lanediv character varying(50)
);

CREATE TABLE jv_nw
(
  gid serial NOT NULL primary key,
  id bigint,
  seqnr integer,
  trpeltyp character varying(10),
  trpelid_mnl bigint,
  trpeldset_gdf character varying(20),
  trpelsect_gdf character varying(20),
  trpelid_gdg character varying(20)
);

create table temp_jv_link_walked 
(
  id bigint,
  link bigint
);

-------------------------------------------------------------------------------------------------
-- admin
-------------------------------------------------------------------------------------------------
CREATE TABLE temp_admin_order0
(
  order0_id integer PRIMARY KEY,
  order0_code integer,
  name_id bigint,
  country_code character varying(10)
);SELECT AddGeometryColumn('public', 'temp_admin_order0', 'the_geom', 4326, 'MULTIPOLYGON', 2);

CREATE TABLE temp_admin_order1
(
  order1_id integer PRIMARY KEY,
  order0_id integer,
  name_id bigint,
  country_code character varying(10)
);SELECT AddGeometryColumn('public', 'temp_admin_order1', 'the_geom', 4326, 'MULTIPOLYGON', 2);

CREATE TABLE temp_admin_order2
(
  order2_id integer PRIMARY KEY,
  order1_id integer,
  name_id bigint,
  country_code character varying(10)
);SELECT AddGeometryColumn('public', 'temp_admin_order2', 'the_geom', 4326, 'MULTIPOLYGON', 2);

CREATE TABLE temp_admin_order8
(
  order8_id integer PRIMARY KEY,
  order2_id integer,
  name_id bigint,
  country_code character varying(10)
);SELECT AddGeometryColumn('public', 'temp_admin_order8', 'the_geom', 4326, 'MULTIPOLYGON', 2);

CREATE TABLE temp_adminid_newandold
(
	gid serial not null primary key,
  id_old double precision,
  level smallint,
  id_new integer
);

create table temp_admin_timedom_mapping
(
	timedom varchar,
	summer_time_id smallint
);

-------------------------------------------------------------------------
-- Phoneme
-------------------------------------------------------------------------
CREATE TABLE org_vm_pt_label
(
  pt_label      INTEGER not null primary key,
  description   CHAR(15)
);

CREATE TABLE org_vm_pt
(
  ptid          integer not null,
  pt_label      integer not null,
  alphabet      integer,
  transcription character varying(254),
  lanphonset    char(3),
  preference    integer,
  lanpron       char(3)  
);

CREATE TABLE org_vm_geovariant
(
  ptid          integer not null,
  lanphonset    char(3),
  preference    integer,
  areacode      character varying(50),
  codetype      integer
);

CREATE TABLE org_vm_nefa
(
  nameitemid     integer,
  featdsetid     integer,
  featsectid     integer,
  featlayerid    integer,
  featitemid     integer,
  featcat        integer,
  featclass      integer,
  sideofline     integer,
  attitemid      integer,
  attseqnr       integer,
  langcode       char(3),
  nametype       char(2),
  namesubtype    integer
);

CREATE TABLE org_vm_foa
(
  featdsetid     integer,
  featsectid     integer,
  featlayerid    integer,
  featitemid     integer,
  featcat        integer,
  featclass      integer,
  shapeid        bigint,
  CONSTRAINT org_vm_foa_pkey PRIMARY KEY (featdsetid, featsectid, 
                      featlayerid, featitemid, featcat, featclass)
);

CREATE TABLE org_vm_ne
(
  namedsetid     integer,
  namesectid     integer,
  namelayerid    integer,
  nameitemid     integer,
  langcode       char(3),
  name           character varying(100),
  normname       character varying(254),
  ptid           integer,
  CONSTRAINT org_vm_ne_pkey PRIMARY KEY (namedsetid, namesectid, namelayerid, nameitemid)
);

CREATE TABLE org_vm_foa_area
(
  featdsetid     integer,
  featsectid     integer,
  featlayerid    integer,
  featitemid     integer,
  feactcat       integer,
  featclass      integer,
  areacode       character varying(50),
  codetype       integer
);

CREATE TABLE org_vm_alphabet_classification
(
  alphabet       integer,
  description    character varying(30)
);

CREATE TABLE org_vm_comp
(
  namedsetid     integer,
  namesectid     integer,
  namelayerid    integer,
  nameitemid     integer,
  langcode       char(3),
  namecompontenttype  integer,
  namecomponenttext   character varying(100),
  ptid                integer
);

CREATE TABLE org_vm_sh
(
  namedsetid     integer,
  namesectid     integer,
  namelayerid    integer,
  nameitemid     integer,
  langcode       char(3),
  shortname      character varying(254),
  shortnametype  integer,
  ptid           integer
);

CREATE TABLE org_vm_area
(
  areacode       character varying(50),
  codetype       integer,
  owner          character varying(50),
  ownercodetype  integer
);
---------------------------------------------------------------------------------------------
--poi Phoneme
---------------------------------------------------------------------------------------------
CREATE TABLE org_poi_pt_label
(
  pt_label      INTEGER not null primary key,
  description   CHAR(15)
);

CREATE TABLE org_poi_pt
(
  ptid          integer not null,
  pt_label      integer not null,
  alphabet      integer,
  transcription character varying(254),
  lanphonset    char(3),
  preference    integer,
  lanpron       char(3)  
);

CREATE TABLE org_poi_geovariant
(
  ptid          integer not null,
  lanphonset    char(3),
  preference    integer,
  areacode      character varying(50),
  codetype      integer
);

CREATE TABLE org_poi_nefa
(
  nameitemid     integer,
  featdsetid     integer,
  featsectid     integer,
  featlayerid    integer,
  featitemid     integer,
  featcat        integer,
  featclass      integer,
  sideofline     integer,
  attitemid      integer,
  attseqnr       integer,
  langcode       char(3),
  nametype       char(2),
  namesubtype    integer
);

CREATE TABLE org_poi_foa
(
  featdsetid     integer,
  featsectid     integer,
  featlayerid    integer,
  featitemid     integer,
  featcat        integer,
  featclass      integer,
  shapeid        bigint,
  CONSTRAINT org_poi_foa_pkey PRIMARY KEY (featdsetid, featsectid, 
                      featlayerid, featitemid, featcat, featclass)
);

CREATE TABLE org_poi_ne
(
  namedsetid     integer,
  namesectid     integer,
  namelayerid    integer,
  nameitemid     integer,
  langcode       char(3),
  name           character varying(100),
  normname       character varying(254),
  ptid           integer,
  CONSTRAINT org_poi_ne_pkey PRIMARY KEY (namedsetid, namesectid, namelayerid, nameitemid)
);

CREATE TABLE org_poi_foa_area
(
  featdsetid     integer,
  featsectid     integer,
  featlayerid    integer,
  featitemid     integer,
  feactcat       integer,
  featclass      integer,
  areacode       character varying(50),
  codetype       integer
);

CREATE TABLE org_poi_alphabet_classification
(
  alphabet       integer,
  description    character varying(30)
);

CREATE TABLE org_poi_comp
(
  namedsetid     integer,
  namesectid     integer,
  namelayerid    integer,
  nameitemid     integer,
  langcode       char(3),
  namecompontenttype  integer,
  namecomponenttext   character varying(100),
  ptid                integer
);

CREATE TABLE org_poi_sh
(
  namedsetid     integer,
  namesectid     integer,
  namelayerid    integer,
  nameitemid     integer,
  langcode       char(3),
  shortname      character varying(254),
  shortnametype  integer,
  ptid           integer
);

CREATE TABLE org_poi_area
(
  areacode       character varying(50),
  codetype       integer,
  owner          character varying(50),
  ownercodetype  integer
);
-- link name
CREATE TABLE temp_link_name
(
  link_id bigint NOT NULL primary key,
  name    character varying(4096)
);

-- link shield
CREATE TABLE temp_link_shield
(
  link_id bigint NOT NULL primary key,
  shield  character varying(2048)
);

-- link route num
CREATE TABLE temp_link_route_num
(
  gid      serial not null primary key,
  link_id  bigint NOT NULL,
  routenum character varying(2048)
);

--------------------------------------------------------------------------------------------------------
CREATE TABLE mid_temp_admin_name
(
  admin_id     bigint not null primary key,
  admin_name   character varying(65536)
);

--------------------------------------------------------------------------------------------------------
CREATE TABLE mid_temp_signpost_element
(
  sign_id            bigint NOT NULL,
  signpost_name      character varying(8192),
  route_no1          character varying(1024),
  route_no2          character varying(1024),
  route_no3          character varying(1024),
  route_no4          character varying(1024),
  exit_no            character varying(1024),
  CONSTRAINT mid_temp_signpost_element_pkey PRIMARY KEY (sign_id)
);
----lane_tbl related
CREATE TABLE temp_lane_link_info
(
  gid serial NOT NULL,
  id bigint,
  inlink bigint,
  outlink bigint,
  passlink character varying(100),
  CONSTRAINT temp_lane_link_info_pkey PRIMARY KEY (gid)
);

CREATE TABLE temp_lane_link_node
(
  gid serial NOT NULL,
  id bigint,
  inlink bigint,
  outlink bigint,
  passlink character varying(100),
  nodeid bigint,
  laneno integer,
  CONSTRAINT temp_lane_link_node_pkey PRIMARY KEY (gid)
);

CREATE TABLE lane_tbl
(
  gid serial NOT NULL,
  id integer,
  nodeid bigint,
  inlinkid bigint,
  outlinkid bigint,
  passlid character varying(1024),
  passlink_cnt smallint,
  lanenum smallint,
  laneinfo character varying(17),
  arrowinfo smallint,
  lanenuml smallint,
  lanenumr smallint,
  buslaneinfo character varying(17),
  CONSTRAINT lane_tbl_pkey PRIMARY KEY (gid)
);


-------------------------------------------------------------------------------------------
CREATE TABLE mid_vm_phoneme_relation
(
  id            bigint,
  --nameitemid  integer,
  --featdsetid  integer,
  --featsectid  integer,
  --featlayerid integer,
  --featitemid  integer,
  --featcat     integer,
  featclass     integer,
  sideofline    integer,
  attitemid     integer,
  attseqnr      integer,
  langcode      character(3),
  nametype      character(2),
  namesubtype   integer,
  "name"        character varying(100),
  normname      character varying(254),
  ptid          integer
);

-------------------------------------------------------------------------------------------
CREATE TABLE mid_vm_pt
(
  ptid             integer not null primary key,
  pt_labels        integer[],
  alphabets        integer[],
  transcriptions   character varying[],
  lanphonsets      character varying[],
  preferences      integer[],
  lanprons         character varying[]
);

-------------------------------------------------------------------------------------------
CREATE TABLE mid_temp_link_lang_code
(
   id          bigint not null primary key,
   l_laxonlc   character varying(3),
   r_laxonlc   character varying(3),
   l_order00   character varying(3),
   r_order00   character varying(3)
);
-------------------------------------------------------------------------------------------
CREATE TABLE mid_temp_safetyalert 
as
(
select a.*,ST_GeomFromtext('POINT(' || a.xcoordinate || ' ' || a.ycoordinate || ')',4326) as the_geom
 ,case when a.endxcoordinate != '' then
  ST_GeomFromtext('POINT(' || a.endxcoordinate || ' ' || a.endycoordinate || ')',4326) 
 else null end as the_geom_endpoint
from (
 select * from scpoint
 union
 select * from scpoint_ext
) a
left join 
(
 select * from scpoint_status 
 union
 select * from scpoint_ext_status
)
b
on a.featid=b.featid
where cameratype in ('1','3','8','12','40')
and b.status<>'UNVERIFIED'
order by cameratype
);


-------------------------------------------------------------------------------------------

CREATE TABLE mid_temp_safetyalert_link 
as
(
select a.featid,a.cameratype,b.shape_line_id,c.the_geom as geom
from mid_temp_safetyalert a
left join (
 select * from scpoint_ll
 union
 select * from scpoint_ext_ll
) b
on a.featid = b.featid
left join org_nw c
on cast(b.shape_line_id as double precision) = c.id
left join (
 select * from scpoint_status 
 union
 select * from scpoint_ext_status
) d
on a.featid = d.featid
where lower(d.status) != 'unverified'
);

-------------------------------------------------------------------------------------------

CREATE TABLE mid_temp_safetyzone
as
(
select a.*,ST_GeomFromtext('POINT(' || a.xcoordinate || ' ' || a.ycoordinate || ')',4326) as the_geom
 ,case when a.endxcoordinate != '' then
  ST_GeomFromtext('POINT(' || a.endxcoordinate || ' ' || a.endycoordinate || ')',4326) 
 else null end as the_geom_endpoint
from scpoint a
left join scpoint_status b
on a.featid=b.featid
where cameratype in ('11','8')
and b.status<>'UNVERIFIED'
order by cameratype
);


-------------------------------------------------------------------------------------------

CREATE TABLE mid_temp_safetyzone_link 
as
(
select a.featid,a.cameratype,b.shape_line_id,c.the_geom as geom
from mid_temp_safetyzone a
left join scpoint_ll b
on a.featid = b.featid
left join org_nw c
on cast(b.shape_line_id as double precision) = c.id
left join scpoint_status d
on a.featid = d.featid
where lower(d.status) != 'unverified'
);

CREATE TABLE mid_sensis_jv_tbl
(
   Dataset varchar(10),
   JunctionView_ID varchar(100),
   Junction_ID varchar(100),
   Junction_ID_Shape varchar(100),
   Junction_ID_X varchar(100),
   Junction_ID_Y varchar(100),
   Left_ARC_ID varchar(100),
   Left_ARC_ID_Shape varchar(100),
   L_ARC_X varchar(100),
   L_ARC_Y varchar(100),
   Straight_ARC_ID varchar(100),
   Straight_ARC_ID_Shape varchar(100),
   S_ARC_X varchar(100),
   S_ARC_Y varchar(100),
   Right_ARC_ID varchar(100),
   Right_ARC_ID_Shape varchar(100),
   R_ARC_X varchar(100),
   R_ARC_Y varchar(100),
   Left_Left_ARC_ID varchar(100),
   Left_Left_ARC_ID_Shape varchar(100),
   LL_ARC_X varchar(100),
   LL_ARC_Y varchar(100),
   Left_Right_ARC_ID varchar(100),
   Left_Right_ARC_ID_Shape varchar(100),
   LR_ARC_X varchar(100),
   LR_ARC_Y varchar(100),
   Right_Left_ARC_ID varchar(100),
   Right_Left_ARC_ID_Shape varchar(100),
   RL_ARC_X varchar(100),
   RL_ARC_Y varchar(100),
   Right_Right_ARC_ID varchar(100),
   Right_Right_ARC_ID_Shape varchar(100),
   RR_ARC_X varchar(100),
   RR_ARC_Y varchar(100),
   Straight_Left_ARC_ID varchar(100),
   Straight_Left_ARC_ID_Shape varchar(100),
   SL_ARC_X varchar(100),
   SL_ARC_Y varchar(100),
   Straight_Right_ARC_ID varchar(100),
   Straight_Right_ARC_ID_Shape varchar(100),
   SR_ARC_X varchar(100),
   SR_ARC_Y varchar(100)
);

create table temp_extend_jv_tbl
(
	in_node_id bigint ,
	out_link_id bigint ,
	picture_type smallint,
	picture_name varchar(100)
);
--------------------------------------------------------------------------------------------
CREATE TABLE temp_school_zone_speed
(
   id        bigint not null primary key,
   dataset   character varying(10) not null,
   speed 		 smallint not null,
   valdir    smallint not null
);
CREATE TABLE temp_school_zone_a0
(
   id    bigint not null primary key,
   munit smallint not null
);
CREATE TABLE temp_school_zone_guide_id
(
   id   		bigint not null primary key,
   guide_id integer not null
);
CREATE TABLE temp_school_zone_s_e_node
as
(
	select shp_id::bigint as id,
				 f_jnctid::bigint as s_node,
				 t_jnctid::bigint as e_node,
				 c.speed
	from org_school_zones as a
	left join org_nw as b
	on a.shp_id::bigint = b.id
	left join temp_school_zone_speed as c
	on a.shp_id::bigint = c.id
);
--------------------------------------------------------------------------------------------
CREATE TABLE temp_scpoint_ext_link
(
   id          		 bigint not null primary key,
   link_id_array 	 bigint[] not null,
   s_node_id_array bigint[] not null,
   e_node_id_array bigint[] not null,
   oneway_array 	 smallint[] not null,
   link_num   		 smallint
);
CREATE TABLE temp_scpoint_ext_link_order
(
   id         		bigint not null primary key,
   inlink 				bigint not null,
   outlink 				bigint not null,
   nodeid 				bigint not null,
   link_array 		bigint[],
   link_num				smallint
);
--------------------------------------------------------------------------------------------
--genre_id;genre_is_brand;gen_0;gen_1;gen_2;genre_level;name;genre_type;org_code;sub_category;brand_name;lang
create table temp_poi_category1
(
  org_code bigint NOT NULL,
  ucode integer NOT NULL,
  gen1 integer NOT NULL,
  gen2 integer NOT NULL,
  gen3 integer NOT NULL,
  "level" smallint NOT NULL,
  al smallint NOT NULL,
  brand character varying(1) NOT NULL,
  imp smallint NOT NULL,
  lang character varying(3) NOT NULL,
  nametype character varying(2) NOT NULL,
  table_name character varying(20) DEFAULT ''::character varying,
  condition character varying(250),
  "name" character varying(128) NOT NULL,
  logmark character varying(1)
 );

create table temp_poi_category_code
(
  per_code   bigint       not null,
  is_brand   varchar,
  gen1       int          not null,
  gen2       int          not null,
  gen3       int          not null,
  level      smallint     not null,
  genre_type varchar,
  org_code   bigint,
  sub_category        integer,
  brand_name varchar,
  filename varchar
);

create table temp_poi_category
(
  per_code   bigint       not null,
  is_brand   varchar,
  gen1       int          not null,
  gen2       int          not null,
  gen3       int          not null,
  level      smallint     not null,
  name        varchar,
  genre_type varchar,
  org_code   bigint,
  sub_category        integer,
  brand_name varchar,
  lang       varchar,
  filename varchar
);

create table temp_category_priority
(
    per_code          character varying(1024)   not null,
    category_priority    int   
 
);

CREATE TABLE temp_poi_logmark
(
  id                bigint NOT NULL,
  feattyp           smallint,
  per_code          bigint,
  category_priority integer,
  the_geom          geometry 
);

CREATE TABLE temp_mnpoi_logmark
(
  id                bigint NOT NULL,
  feattyp           smallint,
  per_code          bigint,
  category_priority integer,
  the_geom          geometry 
);

CREATE TABLE temp_mn_logmark
(
  id                bigint NOT NULL,
  feattyp           smallint,
  per_code          bigint,
  category_priority integer,
  the_geom          geometry 

);

create table temp_poi_name
(
	poi_id bigint,
	poi_name varchar
);

------------------------------------------------------------------------
--
CREATE TABLE mid_temp_hwy_exit_name_ta
(
  node_id            bigint not null primary key,
  is_exit_name       character(1),
  is_junction_name   character(1),
  name               character varying(4096)
);

------------------------------------------------------------------------
--
CREATE TABLE mid_temp_hwy_ic_out_nearby_poi
(
  gid            serial not null primary key,
  road_code      integer not null,
  road_seq       integer not null,
  updown_c       integer not null,
  node_id        bigint not null,
  poi_id         bigint not null,
  cat_id         smallint not null,
  subcat         integer,
  name           character varying(240),
  brandname      character varying(75),
  distance       float
); SELECT AddGeometryColumn('','mid_temp_hwy_ic_out_nearby_poi','the_geom','4326','POINT',2);

-------------------------------------------------------------------------------------------------
-- iso country code
-------------------------------------------------------------------------------------------------
CREATE TABLE temp_mid_iso_country_code
(
  iso_country_num        bigint not null,
  iso_country_code       character varying(3)
);

-------------------------------------------------------------------------------------------------
create table temp_stopsign 
as
(
	select a.featid,d.id,d.f_jnctid,d.t_jnctid
		,ST_GeomFromEWKT('SRID=4326' || chr(59) || 'POINT(' || xcoordinate || ' ' || ycoordinate || ')') as the_geom
		,st_linemerge(d.the_geom) as the_geom_link
	from scpoint_ext a
	left join scpoint_ext_ll b
	on a.featid = b.featid
	left join org_nw d
	on cast(b.shape_line_id as double precision) = d.id 
	where a.cameratype in ('68')
);

create table temp_brand_icon
(
    brandname    varchar(128) not null default ''
);


CREATE TABLE temp_lane_tbl
(
  gid serial primary key,
  id integer,
  nodeid bigint,
  inlinkid bigint,
  outlinkid bigint,
  passlid character varying(1024),
  passlink_cnt smallint,
  lanenum smallint,
  laneinfo character varying(17),
  arrowinfo smallint,
  lanenuml smallint,
  lanenumr smallint,
  buslaneinfo character varying(17),
  exclusive smallint default 0
);

CREATE TABLE org_poi_ne
(
  namedsetid integer NOT NULL,
  namesectid integer NOT NULL,
  namelayerid integer NOT NULL,
  nameitemid integer NOT NULL,
  langcode character(3),
  "name" character varying(100),
  normname character varying(254),
  ptid integer

);

CREATE TABLE org_poi_nefa
(
  nameitemid integer,
  featdsetid integer,
  featsectid integer,
  featlayerid integer,
  featitemid integer,
  featcat integer,
  featclass integer,
  sideofline integer,
  attitemid integer,
  attseqnr integer,
  langcode character(3),
  nametype character(2),
  namesubtype integer
);

CREATE TABLE org_poi_pt
(
  ptid integer NOT NULL,
  pt_label integer NOT NULL,
  alphabet integer,
  transcription character varying(254),
  lanphonset character(3),
  preference integer,
  lanpron character(3)
);

CREATE TABLE org_poi_foa
(
  featdsetid integer NOT NULL,
  featsectid integer NOT NULL,
  featlayerid integer NOT NULL,
  featitemid integer NOT NULL,
  featcat integer NOT NULL,
  featclass integer NOT NULL,
  shapeid bigint

);