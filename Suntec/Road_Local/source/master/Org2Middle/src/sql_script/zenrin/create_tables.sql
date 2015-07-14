
CREATE TABLE temp_condition_regulation_tbl
(
  cond_id integer,
  day character varying(9),
  shour character varying(4),
  ehour character varying(4),
  sdate character varying(4),
  edate character varying(4),
  cartype integer
);

CREATE TABLE temp_org_one_way
(
  gid serial primary key,
  linkno bigint not null,
  dir smallint not null,
  day character varying(9),
  shour character varying(4),
  ehour character varying(4),
  sdate character varying(4),
  edate character varying(4),
  cartype integer
);

CREATE TABLE temp_org_not_in
(
  gid serial primary key,
  fromlinkno bigint not null,
  tolinkno bigint not null,
  snodeno bigint not null,
  tnodeno bigint not null,
  enodeno bigint not null,
  day character varying(9),
  shour character varying(4),
  ehour character varying(4),
  sdate character varying(4),
  edate character varying(4),
  cartype integer,
  carwait smallint,
  carthaba smallint,
  carthigh smallint
);


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



CREATE TABLE temp_link_mapping 
as
(
	select meshcode,linkno,meshcode::bigint*10000000+linkno as link_id from org_road
);

CREATE TABLE temp_node_mapping 
as
(
	select a.meshcode,a.nodeno,
			case when b.nodeid_adj is not null 
				then b.meshcode_adj::bigint*10000000+b.nodeid_adj 
				else a.node_id end as node_id
	from 
	(
		select distinct meshcode,nodeno,meshcode::bigint*10000000+nodeno as node_id 
		from org_node
	) a
	left join org_node_connect b 
	on a.meshcode::integer=b.meshcode_self and a.nodeno=b.nodeid_self and mesh_rel in (2,4)
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
			where condtype = 42 and cond_id is not null
			union
			select inlinkid as link_id, 3::smallint as linkdir
			from regulation_relation_tbl
			where condtype = 43 and cond_id is not null
		)as t
	)as t2
	group by link_id
);

CREATE TABLE temp_lane_tbl
(
  gid serial primary key,
  nodeid bigint,
  inlinkid bigint,
  outlinkid bigint,
  passlid character varying(1024),
  passlink_cnt smallint,
  lanenum smallint,
  laneinfo character varying(17),
  arrowinfo smallint,
  lanenuml smallint,
  lanenumr smallint
);

CREATE TABLE temp_hwlane_tbl
(
  nodeid integer,
  meshcode varchar,
  inlinkid integer,
  outlinkid integer,
  passlid character varying(1024),
  passlink_cnt smallint,
  lanenum smallint,
  laneinfo character varying(17),
  arrowinfo smallint,
  lanenuml smallint,
  lanenumr smallint
);

CREATE TABLE temp_org_spjcty
(
  gid serial primary key,
  atten_intersetion_no bigint,
  from_intersetion_no bigint,
  mng_no_bkg character varying(64),
  to_int_no11 bigint,
  to_int_no12 bigint,
  mng_no_arrow1 character varying(64),
  to_int_no21 bigint,
  to_int_no22 bigint,
  mng_no_arrow2 character varying(64)
);

CREATE TABLE temp_spotguide_tbl
(
  gid serial primary key,
  firstLink bigint,
  node bigint,
  secondLink bigint,
  thirdLink bigint,
  patternno character varying(128),
  arrowno character varying(128)
);


CREATE TABLE temp_sp_name
(
 id bigint,
 sp_name varchar
);

CREATE TABLE temp_intersection_name
(
 id bigint,
 intersection_name varchar
);

create table temp_brand_icon
(
    brandname    varchar(128) not null default ''
);

create table temp_poi_category
(
	genre_0 integer,
	genre_1 integer,
	genre_2 integer,
	genre_level smallint,
	genre_is_brand varchar,
	importance smallint,
	org_code character(6),
	chain_code varchar,
	u_code bigint
);
	
create table temp_category_priority
(
    u_code          bigint   not null,
    category_priority    integer   
 
);

CREATE TABLE temp_poi_logmark
(
  poi_id bigint not null, 
  sub_cat integer,
  u_code bigint not null,
  the_geom  geometry 
);

create table temp_poi_name
(
	poi_id bigint,
	poi_name varchar
);

------------------------------------------------------------------------
CREATE TABLE temp_prepare_node
(
	path_id   integer,
	ic_no     bigint,
	node_id   bigint
);