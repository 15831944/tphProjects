-- link name
CREATE TABLE temp_link_name
(
  link_id bigint NOT NULL primary key,
  name character varying(4096)
);

-- link name were deleted
CREATE TABLE temp_link_name_delete
(
  link_id bigint NOT NULL primary key
);

-- link shield
CREATE TABLE temp_link_shield
(
  link_id bigint NOT NULL primary key,
  shield character varying(1024)
);

create table temp_condition_regulation_tbl
as
(
	select 	(row_number() over (order by day, time)) as cond_id,
			day, time
	from
	(
		select distinct day, time
		from
		(
			select day, time1 as time from org_turntable
			union
			select day, time2 as time from org_turntable
			union
			select day, time3 as time from org_turntable
			union
			select day, time4 as time from org_turntable
		)as t
		where not (day = 0 and time is null)
	)as t2
	order by day, time
);

--temp_mid_lane_table
CREATE TABLE temp_mid_lane_table
(
  gid serial NOT NULL,
  arcin double precision,
  "no" smallint,
  sign_id integer,
  change smallint,
  lane smallint,
  direction smallint,
  arcout double precision,
  "type" smallint,
  CONSTRAINT temp_mid_lane_table_pkey PRIMARY KEY (gid)
);
----temp_mid_laneinfo_table
CREATE TABLE temp_mid_laneinfo_table
(
  gid serial NOT NULL,
  arcin double precision,
  arcout double precision,
  laneinfo text,
  direction smallint,
  lane smallint,
  CONSTRAINT temp_mid_laneinfo_table_pkey PRIMARY KEY (gid)
);

CREATE TABLE temp_org_junctionview
(
  gid serial NOT NULL,
  arc1 double precision,
  arc2 double precision,
  nodeid bigint,
  day_pic character varying(80),
  night_pic character varying(80),
  arrowimg character varying(80),
  passlib character varying(256)
);
CREATE TABLE temp_mid_junctionview_tbl
(
  gid integer,
  arc1 double precision,
  arc2 double precision,
  nodeid bigint,
  patternno character varying(80),
  arrowno character varying(80),
  passlib character varying(256)
);
CREATE TABLE temp_mid_pathlinks_lane_tbl
(
  gid serial NOT NULL,
  arcin double precision,
  arcout double precision,
  pathlink text,
  pathlinkcount integer,
  laneinfo text,
  direction smallint,
  lane smallint,
  nodeid integer,
  CONSTRAINT temp_mid_pathlinks_lane_tbl_pkey PRIMARY KEY (gid)
);

--------------------------------------------------------------------
CREATE TABLE temp_toll_plaza_name
(
  gid          integer primary key,
  nodeid       bigint not null,
  inlinkid     bigint not null,
  outlinkid    bigint not null,
  passlid      character varying(1024),
  passlink_cnt smallint not null,
  sp_name      character varying(1024) not null
);
--------------------------------------------------------------------
CREATE TABLE temp_jv_name
(
  gid          integer primary key,
  nodeid       bigint not null,
  inlinkid     bigint not null,
  outlinkid    bigint not null,
  passlid      character varying(1024),
  passlink_cnt smallint not null,
  sp_name      character varying(1024) not null
);
------------------------------------------------------------
CREATE TABLE temp_junctionview_name_sort
(
  gid serial NOT NULL,
  pic_name character varying(256),
  first_arrow_name character varying(256),
  second_arrow_name character varying(256),
  en_roadname1 character varying(256),
  en_roadname2 character varying(256),
  th_roadname1 character varying(256),
  th_roadname2 character varying(256)
);

CREATE TABLE mid_temp_guidelane_passlink_shortest_distance
(
  inlinkid           bigint NOT NULL,
  outlinkid          bigint NOT NULL,
  nodeid             bigint NOT NULL,
  lane_num           integer NOT NULL,
  direction          integer,
  laneinfo           character varying(16),
  passlid            character varying(1024),
  passlink_cnt       smallint NOT NULL
);
CREATE TABLE temp_park_node
(
  park_node_id bigint NOT NULL,
  park_node_type smallint,
  park_node_old_id bigint,
  base_node_id bigint
);SELECT AddGeometryColumn('public', 'temp_park_node', 'the_geom', 4326, 'POINT', 2);
CREATE TABLE temp_park_polygon
(
  gid serial NOT NULL,
  park_old_id integer NOT NULL
);SELECT AddGeometryColumn('public', 'temp_park_polygon', 'the_geom', 4326, 'MULTIPOLYGON', 2);
CREATE TABLE temp_park_link
(
  park_link_id bigint NOT NULL,
  park_link_old_id bigint,
  park_s_node bigint,
  park_e_node bigint,
  park_oneway smallint
);SELECT AddGeometryColumn('public', 'temp_park_link', 'the_geom', 4326, 'LINESTRING', 2);

CREATE TABLE temp_patch_force_guide_tbl
(
  objectid integer,
  fromlinkid bigint,
  tolinkid bigint,
  midcount bigint,
  midlinkid character varying(254),
  guidetype integer,
  nodeid    integer
);
CREATE TABLE new_force_guide_patch
(
  gid serial NOT NULL,
  guide_type smallint,
  node1 bigint,
  node2 bigint,
  node3 bigint,
  node4 bigint,
  node5 bigint
);
CREATE TABLE update_junctionview_tbl
(
  gid serial NOT NULL,
  old_inlinkid bigint,
  old_outlinkid bigint,
  new_inlinkid bigint,
  new_outlinkid bigint,
  patterno character varying(128),
  arrow_name character varying(128)
);
CREATE TABLE temp_update_org_junctionview_tbl
(
  gid serial NOT NULL,
  arc1 double precision,
  arc2 double precision,
  day_pic character varying(80),
  night_pic character varying(80),
  arrowimg character varying(80),
  lon double precision,
  lat double precision
);