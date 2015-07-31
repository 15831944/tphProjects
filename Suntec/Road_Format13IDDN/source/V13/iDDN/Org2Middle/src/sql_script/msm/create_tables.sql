---------------------------------------------------------------------------------------------------------
-- scripts for create tables in this file.
-- Project: Malsing
---------------------------------------------------------------------------------------------------------

create table temp_topo_link_with_attr 
as 
(
	select cast(b.new_link_id as bigint) as link_id,b.folder,b.link_id as old_link_id
		,a.type,a.route_lvl,a.one_way,a.name
		,b.start_node_id as s_node,b.end_node_id as e_node 
		,a.the_geom
	from org_processed_line a
	left join temp_topo_link b
	on a.folder = b.folder and a.link_id = b.link_id
);
------------------------------------------------------------
-- For innerlink.
------------------------------------------------------------ 
create table temp_link_innerlink 
as 
(
	select distinct a.link_id
	from temp_topo_link_with_attr a
	left join temp_topo_link_with_attr b 
	on (b.type in (1,2,3,4,5,11) and b.one_way = 1) and st_intersects(a.the_geom,b.the_geom) and a.link_id <> b.link_id
	left join temp_topo_link_with_attr c 
	on (c.type in (1,2,3,4,5,11) and c.one_way = 1) and st_intersects(a.the_geom,c.the_geom) and a.link_id <> c.link_id
	where b.link_id is not null and c.link_id is not null and b.link_id <> c.link_id
	and not (
		mid_cnv_roadname_simplify(b.name) 
		is distinct from 
		mid_cnv_roadname_simplify(c.name))
	and not (
			(
				(position('{' in b.name) = 1 or position('{' in c.name) = 1)
				and
				(
					mid_cnv_roadname_simplify(substring(b.name,position('|' in b.name) + 1,length(b.name)))
					is distinct from 
					mid_cnv_roadname_simplify(substring(c.name,position('|' in c.name) + 1,length(c.name)))
				)
			) 
			or
			(
				(position('{' in b.name) <> 1 and position('{' in c.name) <> 1) 
				and
				(
					mid_cnv_roadname_simplify(b.name)
					is distinct from  
					mid_cnv_roadname_simplify(c.name)
				)
			)
		)	
	and (a.name is distinct from b.name or a.name is distinct from c.name)
	and (a.s_node in (b.s_node,b.e_node) or a.e_node in (b.s_node,b.e_node))
	and (a.s_node in (c.s_node,c.e_node) or a.e_node in (c.s_node,c.e_node)) 
	and not st_equals(st_intersection(a.the_geom,b.the_geom),st_intersection(a.the_geom,c.the_geom)) 
	and ST_Length_Spheroid(a.the_geom,'SPHEROID("WGS_84", 6378137, 298.257223563)') < 35
	
	union
	
	select distinct link_id from temp_topo_link_with_attr 
	where mid_is_uturn(name) is true
);

------------------------------------------------------------
-- For wrong U-turn.
------------------------------------------------------------ 
create table temp_link_wrong_uturn 
as
(
	select distinct in_link_id,in_geom,in_angle,
		out_link_id,out_geom,out_angle,angle
	from (
		select m.*
		from (
			select * from (
				select in_link_id,in_angle,in_geom,
					out_link_id,out_geom,out_angle,cnt_link_id,
					case when angle >= 360 then angle - 360
						 when angle < 0 then angle + 360 
						 else angle
					end as angle
				from (
					select in_link_id,in_angle,in_geom,
						out_link_id,out_geom,out_angle,
						(out_angle - in_angle) as angle,cnt_link_id
					from (
						select a.link_id as in_link_id,
							a.tazm as in_angle,a.the_geom as in_geom,
							b.link_id as out_link_id,
							b.the_geom as out_geom,b.fazm as out_angle,
							c.link_id as cnt_link_id
						from (
							select link_id,the_geom
								,s_node,e_node,road_type,link_type
								,rdb_cal_zm_lane(the_geom, -1) as tazm
							from link_tbl
							where link_type in (2,3,5) and one_way_code in (2,3)
						) a
						left join (
							select  link_id,the_geom
								,s_node,e_node,road_type,link_type
								,rdb_cal_zm_lane(the_geom, 1) as fazm
							from link_tbl
							where link_type in (2,3,5) and one_way_code in (2,3)
						) b
						on a.e_node = b.s_node and a.link_type = b.link_type 
						and a.road_type = b.road_type
						left join (
							select * from link_tbl
							where link_type != 2
						) c
						on a.e_node in (c.s_node,c.e_node)		
						where a.link_id <> b.link_id 
						and c.link_id is not null and c.link_id not in (a.link_id,b.link_id)
					) c
				) d 
			) e where (angle >= 70 and angle <= 105)  
		) m
		left join temp_link_name n1
		on m.in_link_id = n1.link_id
		left join temp_link_name n2
		on m.out_link_id = n2.link_id
		left join temp_link_shield p1
		on m.in_link_id = p1.link_id
		left join temp_link_shield p2
		on m.out_link_id = p2.link_id
		where (p1.shield is distinct from p2.shield) 
		or (replace(replace(replace(replace(n1.name,'(N)',''),'(S)',''),'(W)',''),'(E)','') 
			is distinct from 
			replace(replace(replace(replace(n2.name,'(N)',''),'(S)',''),'(W)',''),'(E)',''))
	) q
); 

create table temp_link_wrong_uturn_rectify 
as
(
	select m.* from temp_link_wrong_uturn m
	left join (
		select *
		from (
			select a.*,
				ST_MakePolygon(
					ST_AddPoint(
						ST_AddPoint(
							ST_MakeLine(st_pointn(in_geom,st_npoints(in_geom) - 1),st_endpoint(in_geom)),st_pointn(out_geom,2)),st_pointn(in_geom,st_npoints(in_geom) - 1))) as box
				,c.link_id as mid_link_id,c.the_geom as mid_geom
			from temp_link_wrong_uturn a
			left join link_tbl b
			on a.in_link_id = b.link_id
			left join link_tbl c
			on b.e_node in (c.s_node,c.e_node)
			and a.out_link_id <> c.link_id and a.in_link_id <> c.link_id
		) m where st_geometrytype(st_intersection(box,mid_geom)) != 'ST_Point'
	) n
	on m.in_link_id = n.in_link_id and m.out_link_id = n.out_link_id
	where n.in_link_id is null
);

------------------------------------------------------------
-- For ramp&roundabout.
------------------------------------------------------------ 
CREATE TABLE temp_link_special
(
  gid serial NOT NULL,
  link_id bigint,
  group_id  integer,
  seq    integer
);

create table temp_link_special_new_type 
as
(
	select new_link_id,(array_agg(new_type))[1] as new_type
	from (
		select a.link_id as new_link_id,h.folder,h.link_id,f.type_array[1] as new_type 
		from temp_link_special a
		left join (
		select group_id,array_agg(type) as type_array from (
			select * from (
				select a.link_id,a.group_id,c.type 
				from temp_link_special a
				left join temp_topo_link_with_attr b
				on a.link_id = b.link_id
				left join (
					select * from temp_topo_link_with_attr
					where type not in (8,9,11,12,10,22,26,27)
				) c
				on b.s_node in (c.s_node,c.e_node) or b.e_node in (c.s_node,c.e_node) 
			) d where type is not null
			order by group_id,type
		) e group by group_id
		) f on a.group_id = f.group_id
		left join temp_topo_link h
		on a.link_id = h.new_link_id
		order by a.link_id,f.type_array[1]
	) h group by new_link_id
);

create table temp_link_special_new_type_for_dsp 
as
(
	select new_link_id,(array_agg(new_type))[1] as new_type
	from (
		select a.link_id as new_link_id,h.folder,h.link_id,f.type_array[1] as new_type 
		from temp_link_special a
		left join (
		select group_id,array_agg(type) as type_array from (
			select * from (
				select a.link_id,a.group_id,c.type 
				from temp_link_special a
				left join temp_topo_link_with_attr b
				on a.link_id = b.link_id
				left join (
					select * from temp_topo_link_with_attr
					where type not in (8,9,11,12,10,22,26,27)
				) c
				on b.s_node in (c.s_node,c.e_node) or b.e_node in (c.s_node,c.e_node) 
			) d where type is not null
			order by group_id,type desc
		) e group by group_id
		) f on a.group_id = f.group_id
		left join temp_topo_link h
		on a.link_id = h.new_link_id
		order by a.link_id,f.type_array[1] desc
	) h group by new_link_id
);

create table temp_link_special_new_level 
as
(
	select new_link_id,(array_agg(new_level))[1] as new_level
	from (
		select a.link_id as new_link_id,h.folder,h.link_id
			,f.level_array[1] as new_level 
		from temp_link_special a
		left join (
			select group_id,array_agg(route_lvl) as level_array from (
				select * from (
					select a.link_id,a.group_id,c.route_lvl 
					from temp_link_special a
					left join temp_topo_link_with_attr b
					on a.link_id = b.link_id
					left join (
						select * from temp_topo_link_with_attr
						where route_lvl not in (1,2)
					) c
					on b.s_node in (c.s_node,c.e_node) or b.e_node in (c.s_node,c.e_node) 
				) d where route_lvl is not null
				order by group_id,route_lvl
			) e group by group_id
		) f on a.group_id = f.group_id
		left join temp_topo_link h
		on a.link_id = h.new_link_id
		order by a.link_id,f.level_array[1]
	) h group by new_link_id
);

CREATE TABLE temp_link_roundabout
(
  gid serial NOT NULL,
  link_id bigint,
  group_id  integer,
  seq    integer
);

create table temp_link_roundabout_new_type  
as
(
	select new_link_id,(array_agg(new_type))[1] as new_type
	from (
		select a.link_id as new_link_id,h.folder,h.link_id,f.type_array[1] as new_type 
		from temp_link_roundabout a
		left join (
		select group_id,array_agg(type) as type_array from (
			select * from (
				select a.link_id,a.group_id,
				case when c.type in (8,9,11) then cc.new_type
				     when c.type in (12,10,22,26,27) then null
					 else c.type
				end as type
				from temp_link_roundabout a
				left join temp_topo_link_with_attr b
				on a.link_id = b.link_id
				left join temp_topo_link_with_attr c
				on b.s_node in (c.s_node,c.e_node) or b.e_node in (c.s_node,c.e_node) 
				left join temp_link_special_new_type_for_dsp cc
				on a.link_id = cc.new_link_id
			) d where type is not null
			order by group_id,type
		) e group by group_id
		) f on a.group_id = f.group_id
		left join temp_topo_link h
		on a.link_id = h.new_link_id
		order by a.link_id,f.type_array[1]
	) h group by new_link_id
);

create table temp_link_roundabout_dsp 
as
(
	select a.new_link_id,rdb_cnv_display_class_for_roundabout(new_type, b.new_link_id) as display_class
	from temp_link_roundabout_new_type a
	left join temp_link_expressway b
	on a.new_link_id = b.new_link_id
);


------------------------------------------------------------
-- For expressway.
------------------------------------------------------------ 
create table temp_link_expressway 
as
(
	select * from (
		select b.new_link_id as new_link_id,a.folder,a.link_id as link_id
			,a.name,a.toll_road,a.spd_limit,a.the_geom
			,a.type as type
		from org_processed_line a
		left join temp_topo_link b
		on a.folder = b.folder and a.link_id = b.link_id
	) d
	where 
	(
		((
			(
				((name like '{ME%') and spd_limit >= 80)
				or (lower(name) like '%expressway%' and type in (1,2,3))
				or (name in ('{O26|Lebuhraya K.L.I.A.'))
				or (toll_road = 1 and type = 1)	
				or
				( type = 1 and spd_limit >= 80 
				and st_intersects(ST_Boundary(the_geom),ST_GeomFromText('POLYGON((101.645387 3.096518,101.679773 3.096518,101.679773 3.117174,101.635387 3.117174,101.635387 3.096518,101.645387 3.096518))',4326)))
				or
				( type = 1 and spd_limit >= 80 
				and st_intersects(ST_Boundary(the_geom),ST_GeomFromText('POLYGON((101.71982 3.07952,101.74415 3.07952,101.74415 3.10887,101.71982 3.10887,101.71982 3.07952))',4326)))
			)
			and not
			(
				st_intersects(ST_Boundary(the_geom),ST_GeomFromText('POLYGON((101.55586790 3.07532721,101.5806670 3.07532721,101.5806670 3.0823906,101.55586790 3.0823906,101.55586790 3.07532721))',4326))
				or st_intersects(ST_Boundary(the_geom),ST_GeomFromText('POLYGON((101.5795017 3.0695571,101.624861 3.0695571,101.624861 3.080691,101.5795017 3.080691,101.5795017 3.0695571))',4326))
				or st_intersects(ST_Boundary(the_geom),ST_GeomFromText('POLYGON((101.6044065 3.1126864,101.625011 3.1126864,101.625011 3.130063,101.6044065 3.130063,101.6044065 3.1126864))',4326))		
			)
		) and folder not in ('SG'))
		or (folder = 'SG' and type in (1))
	)
	and not (folder = 'NPM1' and (name like '{ME19%' or name like '{ME28%'))
);

--regulation
CREATE TABLE temp_turn_rstrs
(
  gid serial not null,
  f_link_id bigint not null,
  node_id bigint not null,
  t_link_id character varying(80) not null,
  s_e character varying not null,
  folder character varying not null
);
CREATE TABLE temp_turn_link
(
  reg_id integer NOT NULL,
  in_link_id bigint NOT NULL,
  node_id bigint NOT NULL,
  out_link_id bigint NOT NULL,
  pass_link character varying(80),
  pass_link_count integer,
  s_e character varying NOT NULL
);
CREATE TABLE temp_acc_mask
(
  gid serial not null,
  link_id bigint not null,
  one_way integer,
  cond_id integer
);
CREATE TABLE temp_node_country
as
(
	select node_id,country_array
	from
	(
		select node_id,array_agg(l_country) as country_array
		from
		(
			select start_node_id as node_id,c.l_country
			from temp_topo_link as b
			left join org_processed_line as c
			on b.folder = c.folder and b.link_id = c.link_id
		
			union
		
			select end_node_id as node_id,c.l_country
			from temp_topo_link as b
			left join org_processed_line as c
			on b.folder = c.folder and b.link_id = c.link_id
		)temp
		group by node_id
	)temp1
	where array_upper(country_array,1) > 1
);

create table temp_link_country
as
(
	select new_link_id as link_id, start_node_id as s_node, end_node_id as e_node, b.one_way, l_country as country
	from temp_topo_link as a
	left join org_processed_line as b
	on a.folder = b.folder and a.link_id = b.link_id
	left join temp_acc_mask as c
	on a.new_link_id = c.link_id
	where c.link_id is null
);

-----------------------------------------------------------------
CREATE TABLE temp_link_name
(
  link_id            bigint NOT NULL primary key,
  name               character varying(4096)
);

-----------------------------------------------------------------
-- link shield
CREATE TABLE temp_link_shield
(
  link_id            bigint NOT NULL primary key,
  shield             character varying(2048)
);

-----------------------------------------------------------------
CREATE TABLE mid_temp_signpost
(
  in_link_id         bigint not null,
  out_link_id        bigint not null,
  folder             character varying not null,
  sign_id            bigint NOT NULL,
  signpost_name      character varying(4096),
  route_no1          character varying(512),
  route_no2          character varying(512),
  route_no3          character varying(512),
  route_no4          character varying(512),
  exit_no            character varying(512)
);
-------------------------------------------------------------------------------------------
--test
-------------------------------------------------------------------------------------------
create table temp_topo_folderid 
as
(
	select row_number() over (order by folder) as id_cls,folder
	from (
		select distinct folder from org_processed_line 
	) a
);


create table temp_topo_link_point
as
(
	select	((b.id_cls * 10000000) + a.link_id)::bigint as new_link_id,
		a.folder,
		a.link_id,
		st_linemerge(the_geom) as the_geom,
		st_startpoint(the_geom) as geom_start, 
		st_x(st_startpoint(the_geom)) as x_start,
		st_y(st_startpoint(the_geom)) as y_start,
		0 as z_start,
		st_endpoint(the_geom) as geom_end,
		st_x(st_endpoint(the_geom)) as x_end,
		st_y(st_endpoint(the_geom)) as y_end,
		0 as z_end
	from org_processed_line a
	left join temp_topo_folderid b
	on a.folder = b.folder
);

create table temp_topo_point
as
(
	select	(row_number() over (order by x,y,z))::integer as gid, *
	from
	(
		select	distinct x, y, z, the_geom
		from
		(
			select	geom_start as the_geom, x_start as x, y_start as y, z_start as z
			from temp_topo_link_point
			union
			select	geom_end as the_geom, x_end as x, y_end as y, z_end as z
			from temp_topo_link_point
		)as t
	)as t
);

create table temp_topo_point_3cm
as
(
	select	*,
		(x*3600*1000)::integer as ix, (y*3600*1000)::integer as iy,
		(x*3600*1000-0.5)::integer as ix_m, (y*3600*1000-0.5)::integer as iy_m
	from temp_topo_point
);

create table temp_topo_near_point
as
(
	select a.gid as aid, b.gid as bid, st_distance(a.the_geom, b.the_geom, true) as distance
	from temp_topo_point_3cm as a
	inner join temp_topo_point_3cm as b
	on 	(a.gid < b.gid)
		and
		(
			(a.ix = b.ix or a.ix_m = b.ix_m)
			and
			(a.iy = b.iy or a.iy_m = b.iy_m)
		)
);
create table temp_topo_node
(
	node_id		integer,
	x		double precision,
	y		double precision,
	z		integer,
	the_geom	geometry
);

create table temp_topo_point_node
(
	gid		integer,
	x		double precision,
	y		double precision,
	z		integer,
	node_id		integer,
	new_geom	geometry
);

create table temp_topo_walk_point
(
	gid	integer primary key
);

create table temp_topo_link
as
(
	select 	t.new_link_id, 
		t.folder,
		t.link_id,
		a.node_id as start_node_id, 
		b.node_id as end_node_id,
		(
		case 
		when a.new_geom is not null and b.new_geom is not null then 
		     st_setpoint(st_setpoint(t.the_geom, 0, a.new_geom), st_numpoints(t.the_geom)-1, b.new_geom)
		when a.new_geom is not null then
		     st_setpoint(t.the_geom, 0, a.new_geom)
		when b.new_geom is not null then
		     st_setpoint(t.the_geom, st_numpoints(t.the_geom)-1, b.new_geom)
		else t.the_geom
		end
		)as the_geom
	from temp_topo_link_point as t
	left join temp_topo_point_node as a
	on t.x_start = a.x and t.y_start = a.y and t.z_start = a.z
	left join temp_topo_point_node as b
	on t.x_end = b.x and t.y_end = b.y and t.z_end = b.z
);

-----------------------------------------------------------------
create table road_replaced_name
(
  gid            serial not null primary key,
  org_name       character varying(100),
  replace_name   character varying(100),
  reference      character varying(100)
);
------------------------------------------------------------------
CREATE TABLE temp_lane_info
(
  gid integer,
  link_id integer,
  outlinkid integer,
  laneno text,
  lane_cnt_f integer,
  lane_cnt_r integer,
  folder text
);
CREATE TABLE temp_lane_in_topolink
(
  link_id bigint,
  outlinkid integer,
  laneno text,
  lane_cnt_f integer,
  lane_cnt_r integer
);
CREATE TABLE temp_topo_link_org_oneway
(
  link_id bigint,
  start_node_id integer,
  end_node_id integer,
  oneway integer
);
CREATE TABLE temp_lane_info_passlink_dir
(
  link_id bigint,
  outlinkid bigint,
  lanenos integer[],
  lane_cnt_f integer,
  lane_cnt_r integer,
  passlink text,
  passlink_dir smallint,
  nodeid integer
);
create table temp_spotguide_info
("gid" integer, 
 "link_id" integer, "outlinkid" integer, 
 "illustname" text,
 "arrowid" text,
 "signid" text,
 "folder" text
 );
CREATE TABLE temp_spotguide_in_topolink
(
  link_id bigint,
  outlinkid integer,
  illustname text,
  arrowid text,
  signid text,
  folder text
);
CREATE TABLE temp_spotguide_info_passlink_dir
(
  link_id bigint,
  outlinkid bigint,
  illustname text,
  arrowid text,
  signid text,
  passlink text,
  passlink_dir smallint,
  nodeid integer
);
-----------------------------------------------------------------------
------admin
-----------------------------------------------------------------------
CREATE TABLE temp_country
(
	code integer,
	name varchar(100)
);SELECT AddGeometryColumn('','temp_country','the_geom','4326','MULTIPOLYGON',2);

CREATE TABLE temp_state
(
	code integer,
	country_code integer,
	name varchar(100)
);SELECT AddGeometryColumn('','temp_state','the_geom','4326','MULTIPOLYGON',2);

CREATE TABLE temp_city
(
	code integer,
	state_code integer,
	name varchar(100)
);SELECT AddGeometryColumn('','temp_city','the_geom','4326','MULTIPOLYGON',2);

CREATE TABLE temp_code_city
(
	gid serial not null,
	city_name varchar(100)
);SELECT AddGeometryColumn('','temp_code_city','the_geom','4326','MULTIPOLYGON',2);

CREATE TABLE temp_code_city_temp
(
	city_id integer,
	code integer
);SELECT AddGeometryColumn('','temp_code_city_temp','the_geom','4326','MULTIPOLYGON',2);

CREATE TABLE temp_district_different_city
(
	city_name varchar(100)
);SELECT AddGeometryColumn('','temp_district_different_city','the_geom','4326','MULTIPOLYGON',2);

CREATE TABLE temp_state_different_city
(
	city_name varchar(100)
);SELECT AddGeometryColumn('','temp_state_different_city','the_geom','4326','MULTIPOLYGON',2);

CREATE TABLE temp_admin_name
(
	admin_id character varying(10) not null primary key,
	admin_name character varying(65536),
	country_code character varying(10)
);
CREATE TABLE org_force_guide_patch
(
  gid serial NOT NULL,
  inlinkid integer,
  outlinkid integer,
  passlid character varying(1024),
  passlink_cnt smallint,
  guide_type smallint,
  in_folder character varying(128),
  out_folder character varying(128),
  CONSTRAINT org_force_guide_patch_pkey PRIMARY KEY (gid)
);
