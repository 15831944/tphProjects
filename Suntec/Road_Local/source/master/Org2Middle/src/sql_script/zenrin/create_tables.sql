
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
	per_code          bigint,
	genre_is_brand    varchar,
	gen1           integer,
	gen2           integer,
	gen3           integer,
	genre_level       smallint,
	name              varchar,
	genre_type        varchar,
	org_code          integer,
	series            varchar,
	lang              varchar,
	filename          varchar

);
	
create table temp_category_priority
(
    u_code          bigint   not null,
    category_priority    integer   
 
);

CREATE TABLE temp_poi_logmark
(
  poi_id bigint not null, 
  per_code bigint,
  category_priority integer,
  the_geom  geometry 
);

create table temp_poi_name
(
	poi_id bigint,
	poi_name varchar
);

------------------------------------------------------------------------
CREATE TABLE mid_hwy_org_facility_node
(
  path_id        integer NOT NULL,
  dirflag        integer not null,
  facility_id    bigint NOT NULL,
  node_id        bigint NOT NULL
);

------------------------------------------------------------------------
create table temp_roundabout_all
as
(
	select geom,st_numgeometries(geom) as sum
	from (
		select meshcode_array,linkno_array,ST_linemerge(st_union(geom_array)) as geom
		from (
			select array_agg(the_geom_4326) as geom_array
				,array_agg(meshcode) as meshcode_array
				,array_agg(linkno) as linkno_array
			from (
				select * from org_road 
				where substr(elcode,4,1) = '7'
			) o
		) a
	) b
);

create table temp_roundabout_circle 
as
(
	select idx,st_geometryn(geom,idx) as geom
	from (
		select generate_series(1,sum) as idx,geom
		from temp_roundabout_all a
	) b
);

create table temp_roundabout_doublecircle 
as
(
	select a.idx as roundabout_idx,a.geom as out_geom,st_makepolygon(a.geom) as out_geom_poly
		,b.geom as in_geom,st_makepolygon(b.geom) as in_geom_poly
	from temp_roundabout_circle a
	left join temp_roundabout_circle b
	on a.idx != b.idx
	where ST_contains(st_makepolygon(a.geom),st_makepolygon(b.geom))
);

create table temp_roundabout_doublecircle_links 
as
(
	select c.link_id,a.elcode,d.node_id as s_node,e.node_id as e_node,a.oneway,a.the_geom_4326 as the_geom
		,b.roundabout_idx,2 as inout_flag
	from org_road a
	left join temp_roundabout_doublecircle b
	on st_contains(b.out_geom,a.the_geom_4326)
	left join temp_link_mapping c
	on a.meshcode = c.meshcode and a.linkno = c.linkno
	left join temp_node_mapping d
	on a.meshcode = d.meshcode and a.snodeno = d.nodeno
	left join temp_node_mapping e
	on a.meshcode = e.meshcode and a.snodeno = e.nodeno
	where b.out_geom is not null
	union
	select c.link_id,a.elcode,d.node_id as s_node,e.node_id as e_node,a.oneway,a.the_geom_4326 as the_geom
		,b.roundabout_idx,1 as inout_flag
	from org_road a
	left join temp_roundabout_doublecircle b
	on st_contains(b.in_geom,a.the_geom_4326)
	left join temp_link_mapping c
	on a.meshcode = c.meshcode and a.linkno = c.linkno
	left join temp_node_mapping d
	on a.meshcode = d.meshcode and a.snodeno = d.nodeno
	left join temp_node_mapping e
	on a.meshcode = e.meshcode and a.snodeno = e.nodeno
	where b.in_geom is not null
);

create table temp_roundabout_doublecircle_inlink 
as
(
	select distinct l.link_id,m.node_id as s_node,n.node_id as e_node,a.oneway,a.the_geom_4326 as the_geom
		,case when n.node_id in (b.s_node,b.e_node) then n.node_id
			else m.node_id
		end as node_id
		,b.roundabout_idx
		,case when d.meshcode is not null then 1
			else 0
		end as fastlane_flag
	from org_road a
	left join temp_link_mapping l
	on a.meshcode = l.meshcode and a.linkno = l.linkno
	left join temp_node_mapping m
	on a.meshcode = m.meshcode and a.snodeno = m.nodeno
	left join temp_node_mapping n
	on a.meshcode = n.meshcode and a.enodeno = n.nodeno
	left join temp_roundabout_doublecircle_links b
	on (a.oneway = 1 and n.node_id in (b.s_node,b.e_node))
	or (a.oneway = 0 and (n.node_id in (b.s_node,b.e_node) or m.node_id in (b.s_node,b.e_node)))
	left join temp_roundabout_doublecircle c
	on b.roundabout_idx = c.roundabout_idx
	left join org_fastlane_info d
	on a.meshcode = d.meshcode and a.linkno = d.linkno
	where not l.link_id = b.link_id
	and substr(a.elcode,4,1) != '7'
	and b.inout_flag = 2
	and c.out_geom is not null
	and not ST_ContainsProperly(st_buffer(st_makepolygon(c.out_geom),0.00001),a.the_geom_4326)
);

create table temp_roundabout_doublecircle_outlink 
as
(
	select distinct l.link_id,m.node_id as s_node,n.node_id as e_node,a.oneway,a.the_geom_4326 as the_geom
		,case when n.node_id in (b.s_node,b.e_node) then n.node_id
			else m.node_id
		end as node_id
		,b.roundabout_idx
		,case when d.meshcode is not null then 1
			else 0
		end as fastlane_flag
	from org_road a
	left join temp_link_mapping l
	on a.meshcode = l.meshcode and a.linkno = l.linkno
	left join temp_node_mapping m
	on a.meshcode = m.meshcode and a.snodeno = m.nodeno
	left join temp_node_mapping n
	on a.meshcode = n.meshcode and a.enodeno = n.nodeno
	left join temp_roundabout_doublecircle_links b
	on (a.oneway = 1 and m.node_id in (b.s_node,b.e_node))
	or (a.oneway = 0 and (n.node_id in (b.s_node,b.e_node) or m.node_id in (b.s_node,b.e_node)))
	left join temp_roundabout_doublecircle c
	on b.roundabout_idx = c.roundabout_idx
	left join org_fastlane_info d
	on a.meshcode = d.meshcode and a.linkno = d.linkno
	where not l.link_id = b.link_id
	and substr(a.elcode,4,1) != '7'
	and b.inout_flag = 2
	and c.out_geom is not null
	and not ST_ContainsProperly(st_buffer(st_makepolygon(c.out_geom),0.00001),a.the_geom_4326)
);

create table temp_roundabout_doublecircle_connect_links 
as
(
	select b.link_id, c.node_id as s_node, d.node_id as e_node, a.oneway
		,case when f.inout_flag is not null then f.inout_flag
			else 0
		end as inout_flag
	from  org_road a 
	left join temp_link_mapping b
	on a.meshcode = b.meshcode and a.linkno = b.linkno
	left join temp_node_mapping c
	on a.meshcode = c.meshcode and a.snodeno = c.nodeno
	left join temp_node_mapping d
	on a.meshcode = d.meshcode and a.enodeno = d.nodeno
	left join temp_roundabout_doublecircle e
	on 1 = 1
	left join temp_roundabout_doublecircle_links f
	on b.link_id = f.link_id
	where ST_intersects(e.out_geom_poly,a.the_geom_4326)
);

create table temp_roundabout_doublecircle_paths 
as
(
	select nextval('temp_regulation_id_seq') as id,* 
	from (
		select in_link,in_node,out_link,unnest(paths) as path
		from (
			select *
				,zenrin_cal_doubleroundabout_path(in_link,in_node,out_link,out_node) as paths
			from (
				select a.link_id as in_link,a.node_id as in_node,a.s_node as in_s_node,a.e_node as in_e_node,a.oneway as in_oneway,a.fastlane_flag as in_faselane_flag
					,b.link_id as out_link,b.node_id as out_node,b.s_node as out_s_node,b.e_node as out_e_node,b.oneway as out_oneway,b.fastlane_flag as out_faselane_flag
					,zenrin_cal_doubleroundabout_angle(a.the_geom,(case when a.node_id = a.s_node then 1 else -1 end),b.the_geom,(case when b.node_id = b.s_node then 1 else -1 end)) as turn_angle
					,case when a.node_id = a.s_node then st_pointn(a.the_geom,2)
						else st_pointn(a.the_geom,st_npoints(a.the_geom) - 1)
					end as p0
					,case when a.node_id = a.s_node then st_startpoint(a.the_geom)
						else st_endpoint(a.the_geom)
					end as p1
					,case when b.node_id = b.s_node then st_startpoint(b.the_geom)
						else st_endpoint(b.the_geom)
					end as p2
				from temp_roundabout_doublecircle_inlink a
				left join temp_roundabout_doublecircle_outlink b
				on not a.link_id = b.link_id
				and a.roundabout_idx = b.roundabout_idx
				where a.fastlane_flag = 0 and a.oneway = 1
				order by a.link_id
			) c 
			where (turn_angle > 65 and turn_angle < 125)
			or (turn_angle > -295 and turn_angle < -235)
		) d
	) e
	where array_upper(string_to_array(path,'|'),1) > 2
	order by id
);

create table temp_link_dummy_walked
(
    meshcode varchar,
    linkno   integer
);

create table temp_dummy_link
(
    meshcode varchar,
    linkno integer,
    snodeno integer,
    enodeno integer,
    oneway  integer,
    elcode  varchar,
    the_geom geometry
);

create table temp_dummy_node 
as
(
    select distinct meshcode,snodeno as nodeno
    from temp_dummy_link
    union
    select distinct meshcode,enodeno as nodeno
    from temp_dummy_link
);

create table mid_dummy_reg
(
	reg_id int,
	link_id bigint,
	seq smallint
);

create table mid_dummy_todelete
(
	meshcode varchar(10),
	linkno   int
);

create table org_notin_gid
(
	gid integer
);

CREATE TABLE temp_signpost_uc_deal_with_dummy_link
(
  gid          serial,
  nodeid       bigint not null,
  inlinkid     bigint not null,
  outlinkid    bigint not null,
  passlid      character varying(1024),
  passlink_cnt smallint not null,
  sp_name      character varying(8192),
  route_no1    character varying(1024),
  route_no2    character varying(1024), 
  route_no3    character varying(1024), 
  route_no4    character varying(1024), 
  exit_no      character varying(1024)
);

CREATE TABLE temp_spotguide_tbl1
(
  gid serial primary key,
  firstlink bigint,
  node bigint,
  secondlink bigint,
  thirdlink bigint,
  patternno varchar,
  arrowno varchar
);

CREATE TABLE temp_spotguide_tbl2
(
  gid serial primary key,
  inlink bigint,
  nodeid bigint,
  passlid character varying,
  passlink_cnt smallint,
  outlink bigint,
  imagename varchar
);