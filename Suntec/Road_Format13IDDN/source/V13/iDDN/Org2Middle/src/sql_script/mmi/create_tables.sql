-------------------------------------------------------------------------------------------------
-- admin
-------------------------------------------------------------------------------------------------
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
  order1_id character varying(10),
  country_code character varying(10)
);SELECT AddGeometryColumn('public', 'temp_admin_order8', 'the_geom', 4326, 'MULTIPOLYGON', 2);

CREATE TABLE temp_adminid_newandold
(
  ID_old character varying(10) not null primary key,
  New_ID integer,
  level smallint,
  country_code character varying(10)
);
CREATE TABLE temp_admin_name
(
	admin_id character varying(10) not null primary key,
	admin_name character varying(65536),
	country_code character varying(10)
);
CREATE TABLE temp_admin_state_difference_district
as
(
	select (st_dump(st_difference(
																(
																	select st_union(a.the_geom,b.the_geom)
																	from
																	(select st_union(the_geom) as the_geom from org_state_region) as a,
																	(select st_union(the_geom) as the_geom from org_state_islands_region) as b
																),											
																(select st_union(the_geom) from org_district_region)))).geom as the_geom
);
-----------------------------------------------------------------
CREATE TABLE temp_link_name
(
  link_id bigint NOT NULL primary key,
  name    character varying(4096)
);

-----------------------------------------------------------------
-- link shield
CREATE TABLE temp_link_shield
(
  link_id bigint NOT NULL primary key,
  shield  character varying(2048)
);

-----------------------------------------------------------------
CREATE TABLE mid_temp_link_name_phoneme
(
  id            BIGINT NOT NULL primary key,
  types         character varying[] NOT NULL,
  name_englishs character varying[] NOT NULL,
  name_nuances  character varying[] NOT NULL
);

-----------------------------------------------------------------
CREATE TABLE mid_temp_link_regional_name
(
  id                  BIGINT NOT NULL primary key,
  types               character varying[] NOT NULL,
  name_englishs       character varying[] NOT NULL,
  name_regionals      character varying[] NOT NULL,
  regional_lang_types character varying[] NOT NULL 
);

-----------------------------------------------------------------
CREATE TABLE mid_temp_signpost_element
(
  folder             character varying,
  sign_id            bigint NOT NULL,
  signpost_name      character varying(4096),
  route_no1          character varying(512),
  route_no2          character varying(512),
  route_no3          character varying(512),
  route_no4          character varying(512),
  exit_no            character varying(512),
  CONSTRAINT mid_temp_signpost_element_pkey PRIMARY KEY (folder, sign_id)
);

-------------------------------------------------------------------------------------------------
-- SignPost Path Links
-------------------------------------------------------------------------------------------------
CREATE TABLE mid_temp_signpost_passlink
(
  folder          character varying,
  sign_id         bigint not null,
  in_link_id      bigint not null,
  node_id         bigint not null,
  out_link_id     bigint not null,
  passlid         character varying(1024),
  passlink_cnt    smallint,
  CONSTRAINT mid_temp_signpost_passlink_pkey PRIMARY KEY (folder, sign_id)
);

-------------------------------------------------------------------------------------------------
-- Regulation Path Links
-------------------------------------------------------------------------------------------------
CREATE TABLE temp_condition_regulation_tbl
(
  cond_id integer,		-- new cond id
  dt_id integer			-- org_date_time.gid
);


create table temp_link_regulation_forbid_traffic
as
(
	select distinct inlinkid as link_id, 1::smallint as linkdir
	from regulation_relation_tbl
	where condtype = 4 and cond_id is null
);

create table temp_link_regulation_permit_traffic
as
(
	select link_id, array_agg(linkdir) as linkdir_array
	from
	(
		select distinct link_id, linkdir
		from
		(
			select inlinkid as link_id, 2::smallint as linkdir
			from regulation_relation_tbl
			where condtype = 2
			union
			select inlinkid as link_id, 3::smallint as linkdir
			from regulation_relation_tbl
			where condtype = 3
		)as t
	)as t2
	group by link_id
);

create table temp_link_regulation_traffic
as
(
	select 	a.id::bigint as link_id, a.f_jnctid::bigint, a.t_jnctid::bigint, 
			(
			case 
			when a.const_st = 1 then 4
			when b.link_id is not null then 4
			when a.oneway is null then 1
			when a.oneway = 'FT' and 3 = ANY(c.linkdir_array) then 1
			when a.oneway = 'FT' then 2
			else null
			end
			) as one_way
	from org_city_nw_gc_polyline as a
	left join temp_link_regulation_forbid_traffic as b
	on a.id = b.link_id
	left join temp_link_regulation_permit_traffic as c
	on a.id = c.link_id
);

CREATE TABLE temp_junction_tbl
(
  gid serial NOT NULL,
  inlinkid bigint,
  outlinkid bigint,
  nodeid bigint,
  sky_lyr character varying(30),
  arrow character varying(30),
  "time" character varying(20)
);