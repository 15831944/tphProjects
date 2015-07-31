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

CREATE TABLE temp_admin_order8_part
(
  order8_id character varying(10) PRIMARY KEY,
  order1_id character varying(10),
  country_code character varying(10)
);SELECT AddGeometryColumn('public', 'temp_admin_order8_part', 'the_geom', 4326, 'MULTIPOLYGON', 2);

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
	admin_name character varying(65536)
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
CREATE TABLE temp_admin_district_new_all
as
(
	select  (900000001 + row_number() over())::character varying as order8_id,
	        a.stt_id::character varying(10) 			 		   as order1_id,
	        ST_Multi(b.the_geom) as the_geom
	from org_state_region as a
	left join  temp_admin_state_difference_district as b
	on st_within(b.the_geom,a.the_geom)
	where b.the_geom is not null
	order by a.stt_id
);
CREATE TABLE temp_admin_district_new_combine
as
(
	select order8_id, order1_id, the_geom
	from temp_admin_district_new_all
	where st_area(the_geom) < 0.0001
	
	union
	
	select (array_agg(order8_id))[1] as order8_id,
         order1_id,
	       st_union(ST_Multi(the_geom)) as the_geom
	from temp_admin_district_new_all
	where st_area(the_geom) > 0.0001
	group by order1_id
);
CREATE TABLE temp_admin_order8_compare
(
	id_old varchar,
	id_new varchar
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

-------------------------------------------------------------------
CREATE TABLE mid_temp_admin_name_phoneme
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
CREATE TABLE mid_temp_admin_regional_name
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
			when e.const_st = 1 then 4
			when b.link_id is not null then 4
			when a.oneway is null then 1
			when a.oneway = 'FT' and 3 = ANY(c.linkdir_array) then 1
			when a.oneway = 'FT' then 2
			else null
			end
			) as one_way
	from org_city_nw_gc_polyline as a
	left join org_city_nw_gc_extended_polyline e
	on a.id = e.id
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
  road_lyr character varying(30),
  sign_lyr character varying(30),
  arrow character varying(30),
  "time" character varying(20)
);

-------------------------------------------------------------------------------------------------
CREATE TABLE mid_temp_hwy_poi_regional_name_group
(
  id                  bigint not null primary key,
  types               character varying[],
  name_englishs       character varying[],
  name_regionals      character varying[],
  regional_lang_types character varying[] 
);

-------------------------------------------------------------------------------------------------
CREATE TABLE mid_temp_sapa_store_info
(

);

create table temp_poi_category
(
    per_code   bigint       not null,
    gen1       int          not null,
    gen2       int          not null,
    gen3       int          not null,
    level      smallint     not null,
    name       varchar(128) not null,
    imp        smallint     not null,
    org_code   varchar(6)       not null,
    logmark    varchar(128)  default ''
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
  poi_id double precision NOT NULL,
  std_name character varying(100),
  cat_code character varying(6),
  type_code integer,
  lat numeric,
  lon numeric,
  the_geom geometry 

);

create table temp_poi_name
(
	poi_id bigint,
	poi_name varchar
);

-----------------------------------------------------------------
CREATE TABLE temp_natural_guidance_tbl
(
 gid serial primary key,
 in_link_id bigint NOT NULL,
 in_s_node bigint NOT NULL,
 in_e_node bigint NOT NULL,
 in_link_type smallint NOT NULL DEFAULT (-1),
 in_one_way_code smallint,
 in_node_id bigint NOT NULL,
 out_node_id bigint NOT NULL,
 out_link_id bigint NOT NULL,
 out_s_node bigint NOT NULL,
 out_e_node bigint NOT NULL,
 out_link_type smallint NOT NULL DEFAULT (-1),
 out_one_way_code smallint,
 poi_nme character varying(100),
 poi_prop character varying(100),
 link_array bigint[],
 node_array bigint[]
); SELECT AddGeometryColumn('','temp_natural_guidance_tbl','the_geom','4326','POINT',2);

-----------------------------------------------------------------
CREATE TABLE mid_temp_natural_guidence_name
(
 gid bigint NOT NULL,
 feat_name character varying(4096)
); 
