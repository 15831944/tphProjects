-- link_tbl
CREATE TABLE link_tbl
(
  gid serial primary key,
  link_id bigint NOT NULL,
  iso_country_code character(3),
  tile_id integer,
  s_node bigint NOT NULL,
  e_node bigint NOT NULL,
  display_class smallint,
  link_type smallint NOT NULL default -1,
  road_type smallint NOT NULL default -1,
  toll smallint NOT NULL,
  speed_class smallint,
  length double precision NOT NULL,
  function_class smallint NOT NULL default -1,
  lane_dir smallint,
  lane_num_s2e smallint default -1,
  lane_num_e2s smallint default -1,
  elevated smallint,
  structure smallint,
  tunnel smallint,
  rail_cross smallint default 0,
  paved smallint default 1,
  uturn smallint default 0,
  speed_limit_s2e double precision,
  speed_limit_e2s double precision,
  speed_source_s2e smallint,
  speed_source_e2s smallint,
  width_s2e smallint default -1,
  width_e2s smallint default -1,
  one_way_code smallint,
  one_way_condition smallint,
  pass_code smallint,
  pass_code_condition smallint,
  road_name character varying(16384),
  road_number character varying(2048),
  name_type smallint,
  ownership smallint,
  car_only smallint,
  slope_code smallint,
  slope_angle smallint,
  disobey_flag smallint default -1,
  up_down_distinguish smallint,
  access smallint,
  extend_flag smallint default -1,
  etc_only_flag smallint default -1,
  bypass_flag smallint default 0,
  matching_flag smallint default 0,
  highcost_flag smallint default 0,
  ipd smallint default 0,
  urban smallint default 0,
  erp smallint default 0,
  rodizio smallint default 0,
  soi smallint default 0,
  fazm integer,
  tazm integer,
  feature_string character varying,
  feature_key character varying(32)
); SELECT AddGeometryColumn('','link_tbl','the_geom','4326','LINESTRING',2);

-- node_tbl
CREATE TABLE node_tbl
(
 gid serial primary key,
 node_id bigint not null,
 tile_id integer,
 kind character varying(23),
 light_flag smallint,
 stopsign_flag smallint,
 toll_flag smallint,
 bifurcation_flag smallint,
 org_boundary_flag smallint default 0,
 tile_boundary_flag smallint default 0,
 mainnodeid bigint,
 node_lid character varying(512),
 node_name character varying(1024),
 feature_string character varying,
 feature_key character varying(32),
 z_level smallint default 0
); SELECT AddGeometryColumn('','node_tbl','the_geom','4326','POINT',2);

-- node_height_tbl
CREATE TABLE node_height_tbl
(
 gid serial primary key,
 node_id bigint not null,
 height smallint
);

-- regulation_relation_tbl
CREATE TABLE regulation_relation_tbl
(
  gid serial,
  regulation_id integer,
  nodeid bigint,
  inlinkid bigint,
  outlinkid bigint,
  condtype smallint,
  cond_id integer,
  gatetype smallint,
  slope smallint,
  is_seasonal boolean default false
);

-- regulation_item_tbl
CREATE TABLE regulation_item_tbl
(
  gid serial,
  regulation_id integer,
  linkid bigint,
  nodeid bigint,
  seq_num smallint
);

-- condition_regulation_tbl
CREATE TABLE condition_regulation_tbl
(
  gid serial,
  cond_id integer,
  car_type smallint,
  start_year integer,
  start_month integer,
  start_day integer,
  end_year integer,
  end_month integer,
  end_day integer,
  start_hour integer,
  start_minute integer,
  end_hour integer,
  end_minute integer,
  day_of_week integer,
  exclude_date smallint
);

-- temp_signpost_tbl
CREATE TABLE temp_signpost_tbl
(
  gid serial primary key,
  id serial,
  nodeid bigint,
  inlinkid bigint,
  outlinkid bigint,
  passlid character varying(1024),
  passlink_cnt smallint,
  direction smallint,
  guideattr smallint,
  namekind smallint,
  guideclass smallint,
  sp_name character varying(1024),
  patternno character varying(128),
  arrowno character varying(128)
);
-- signpost_tbl
CREATE TABLE signpost_tbl
(
  gid serial primary key,
  id serial,
  nodeid bigint,
  inlinkid bigint,
  outlinkid bigint,
  passlid character varying(1024),
  passlink_cnt smallint,
  direction smallint,
  guideattr smallint,
  namekind smallint,
  guideclass smallint,
  sp_name character varying(1024),
  is_pattern boolean default false,
  patternno character varying(128),
  arrowno character varying(128)
);

------------------------------------------------------------------------------
-- SignPost UC
------------------------------------------------------------------------------
CREATE TABLE signpost_uc_tbl
(
  gid          serial primary key,
  id           bigint,
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

-- spotguide_tbl
CREATE TABLE spotguide_tbl
(
  gid serial primary key,
  id serial,
  nodeid bigint,
  inlinkid bigint,
  outlinkid bigint,
  passlid character varying(1024),
  passlink_cnt smallint,
  direction smallint,
  guideattr smallint,
  namekind smallint,
  guideclass smallint,
  patternno character varying(128),
  arrowno character varying(128),
  type smallint,
  is_exist_sar boolean default false
);

-- lane_tbl
CREATE TABLE lane_tbl
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

-- towardname_tbl
CREATE TABLE towardname_tbl
(
  gid serial primary key,
  id bigint,
  nodeid bigint not null,
  inlinkid bigint not null,
  outlinkid bigint,
  passlid character varying(1024),
  passlink_cnt smallint,
  direction smallint,
  guideattr smallint,
  namekind smallint,
  namekind2 smallint,
  --toward_name character varying(1024) not null,
  toward_name varchar not null,
  type smallint
);

-- force_guide_tbl
CREATE TABLE force_guide_tbl
(
  gid serial primary key,
  force_guide_id integer,
  nodeid bigint,
  inlinkid bigint,
  outlinkid bigint,
  passlid character varying(1024),
  passlink_cnt smallint,
  guide_type smallint,
  position_type smallint
);

CREATE TABLE caution_tbl
(
  gid serial primary key,
  id serial,
  inlinkid bigint,
  nodeid bigint,
  outlinkid bigint,
  passlid character varying(1024),
  passlink_cnt smallint default 0,
  data_kind smallint,
  voice_id integer DEFAULT 0,
  strTTS varchar(65535),
  image_id integer DEFAULT 0
);

-- spotguide_tbl
CREATE TABLE spotguide_tbl
(
  gid serial primary key,
  id serial,
  nodeid bigint,
  inlinkid bigint,
  outlinkid bigint,
  passlid character varying(1024),
  passlink_cnt smallint,
  direction smallint,
  guideattr smallint,
  namekind smallint,
  guideclass smallint,
  patternno character varying(128),
  arrowno character varying(128),
  type smallint,
  is_exist_sar boolean default false
);

-- natural_guidence_tbl
CREATE TABLE natural_guidence_tbl
(
  gid serial primary key,
  nodeid bigint,
  inlinkid bigint,
  outlinkid bigint,
  passlid character varying(1024),
  passlink_cnt smallint,
  feat_position smallint,
  feat_importance smallint,
  preposition_code smallint,
  feat_name character varying(4096),
  condition_id smallint
);

CREATE TABLE guidence_condition_tbl
(
  gid serial primary key,
  condition_id smallint,
  start_date smallint,
  end_date smallint,
  week_flag smallint,
  start_time smallint,
  end_time smallint,
  exclude_date smallint
);

--name_dictionary_tbl
CREATE TABLE name_dictionary_tbl
(
  gid serial primary key,
  name_id integer not null,
  name character varying(512) not null,
  py character varying(512),
  language smallint not null
);


-- zone

CREATE TABLE rdb_zone
(
  zoneid smallint NOT NULL,
  country_code character(3),
  sub_country_code smallint,
  travel_flag smallint,
  CONSTRAINT rdb_zone_pkey PRIMARY KEY (zoneid)
);


CREATE TABLE poi_tbl
(
   gid serial NOT NULL primary key,
   poi_id integer not null,
   name_id integer not null,
   poi_type character varying(32)
); SELECT AddGeometryColumn('','poi_tbl','the_geom','4326','POINT',2);

CREATE TABLE temp_link_ramp
(
  link_id bigint NOT NULL,
  new_road_type smallint not null ,
  new_fc smallint,
  CONSTRAINT temp_link_ramp_pkey PRIMARY KEY (link_id)
);

CREATE TABLE temp_link_ramp_single_path
(
  link_id bigint NOT NULL,
  new_road_type smallint not null ,
  new_fc smallint,
  new_link_type smallint
);

CREATE TABLE temp_link_ramp_toohigh
(
  link_id bigint NOT NULL,
  new_road_type smallint not null ,
  new_fc smallint,
  CONSTRAINT temp_link_ramp_toohigh_pkey PRIMARY KEY (link_id)
);

CREATE TABLE temp_link_ramp_toohigh_single_path
(
  link_id bigint NOT NULL,
  new_road_type smallint not null ,
  new_fc smallint
);

create table temp_merge_node_keep
as
(
	select distinct node_id
	from
	(
		select node_id
		from node_tbl
		where light_flag != 0 or stopsign_flag != 0 or toll_flag != 0 
		or bifurcation_flag != 0 or node_name is not null or kind in('101','102','103')
		union
		select node_id
		from mid_all_highway_node
	)as t
	union
	select nodeid 
	from 
	(
		select nodeid
		from signpost_tbl
		union
		select nodeid
		from signpost_uc_tbl
		union
		select nodeid
		from spotguide_tbl
		union
		select nodeid
		from force_guide_tbl
		union
		select nodeid
		from lane_tbl
		union
		select nodeid
		from towardname_tbl
		union
		select nodeid
		from caution_tbl
		union
		select nodeid
		from crossname_tbl		
		union
		select nodeid
		from natural_guidence_tbl
		union
		select nodeid
		from hook_turn_tbl
		union
		select node_id as nodeid
		from stopsign_tbl
	) as b
	union
	select s_node from link_tbl where erp in (1,2)
	union
	select e_node from link_tbl where erp in (1,3)
	union
	select nodeid
	from regulation_item_tbl
	where nodeid is not null
);

create table temp_merge_link_keep
as
(
	select distinct link_id
	from
	(
		select inlinkid as link_id
		from regulation_relation_tbl
		where outlinkid is null
		
		union
		
		select linkid as link_id
		from safety_zone_tbl
		
		union
		
		select link_id
		from safety_alert_tbl
		
		union
		
		select orglink_id as link_id
		from safety_alert_tbl
		
		--union
		
		--select	unnest(link_array) as link_id
		--from
		--(
		--	select	(
		--			case when passlid is null or passlid = '' then ARRAY[inlinkid, outlinkid]
		--			else ARRAY[inlinkid, outlinkid] || cast(regexp_split_to_array(passlid, E'\\|+') as bigint[])
		--			end
		--			)as link_array
		--	from
		--	(
		--		select inlinkid, outlinkid, passlid
		--		from signpost_tbl
		--		union
		--		select inlinkid, outlinkid, passlid
		--		from signpost_uc_tbl
		--		union
		--		select inlinkid, outlinkid, passlid
		--		from spotguide_tbl
		--		union
		--		select inlinkid, outlinkid, passlid
		--		from force_guide_tbl
		--		union
		--		select inlinkid, outlinkid, passlid
		--		from lane_tbl
		--		union
		--		select inlinkid, outlinkid, passlid
		--		from towardname_tbl
		--		union
		--		select inlinkid, outlinkid, null as passlid
		--		from caution_tbl
		--	)as a
		--)as b
	)as x
);

create table temp_merge_node_suspect
as
(
	select b.node_id
	from
	(
		select node_id
		from
		(
			select node_id, regexp_split_to_array(node_lid, E'\\|+') as link_array
			from node_tbl
		)as a
		where array_upper(link_array, 1) = 2
	)as b
	left join temp_merge_node_keep as c
	on b.node_id = c.node_id
	where c.node_id is null
);

create table temp_merge_link_suspect
as
(
	select 	distinct
			b.link_id, b.tile_id, s_node, e_node, one_way_code, road_name, road_number, 
			link_type, road_type, toll, length, function_class, b.display_class, 
			elevated, structure, tunnel, rail_cross, paved, uturn, disobey_flag, etc_only_flag, extend_flag,
			bypass_flag, matching_flag, highcost_flag, ipd, urban, erp, rodizio, soi, lane_num_s2e, lane_num_e2s, width_s2e, width_e2s, speed_limit_s2e, speed_limit_e2s, speed_source_s2e, speed_source_e2s,
			park_link_connect_type,park_floor,park_link_lean,park_link_toll,park_region_id
	from temp_merge_node_suspect as a
	left join link_tbl as b
	on a.node_id in (b.s_node, b.e_node)
	left join temp_merge_link_keep as c
	on b.link_id = c.link_id
	left join park_link_tbl as d
	on b.link_id = d.park_link_id
	where c.link_id is null --and road_type = 10
);

create table temp_merge_link_walked
(
	link_id bigint primary key
);

create table temp_merge_linkrow
(
	link_id			bigint,
	s_node			bigint,
	e_node			bigint,
	one_way_code	smallint,
	link_num		integer,
	link_array		bigint[],
	linkdir_array	boolean[]
);

create table temp_merge_link_mapping
as
(
	select	link_id, 
			s_node, 
			e_node, 
			one_way_code, 
			link_num,
			merge_index,
			link_array[merge_index] as merge_link_id,
			linkdir_array[merge_index] as merge_link_dir
	from
	(
		select	link_id, 
				s_node, 
				e_node, 
				one_way_code, 
				link_num,
				generate_series(1, link_num) as merge_index,
				link_array,
				linkdir_array
		from temp_merge_linkrow
	)as a
);

create table temp_merge_newlink_maybe_circle
as
(
    select link_id, s_node, e_node, one_way_code, st_linemerge(st_collect(the_geom)) as the_geom
    from
    (
	select	a.link_id, a.s_node, a.e_node, a.one_way_code, a.merge_index,
		(case when a.merge_link_dir then b.the_geom else st_reverse(b.the_geom) end) as the_geom
	from temp_merge_link_mapping as a
	left join link_tbl as b
	on a.merge_link_id = b.link_id
	order by a.link_id, a.s_node, a.e_node, a.one_way_code, a.merge_index
    )as a
    group by link_id, s_node, e_node, one_way_code
);

create table temp_merge_newlink
as
(
	select	link_id, s_node, e_node, one_way_code,
			ST_Length_Spheroid(the_geom,'SPHEROID("WGS_84", 6378137, 298.257223563)') as length, 
			mid_cal_zm(the_geom, 1) as fazm,
			mid_cal_zm(the_geom, -1) as tazm,
			the_geom
	from
	(
		select 	link_id, s_node, e_node, one_way_code,
				(case when st_equals(st_startpoint(line_geom), node_geom) then line_geom else st_reverse(line_geom) end) as the_geom
		from
		(
			select a.link_id, a.s_node, a.e_node, a.one_way_code, a.the_geom as line_geom, b.the_geom as node_geom
			from temp_merge_newlink_maybe_circle as a
			left join node_tbl as b
			on a.s_node = b.node_id
			where st_geometrytype(a.the_geom) = 'ST_LineString' and not st_isclosed(a.the_geom)
		)as a
		
		union
		
    	select	link_id, s_node, e_node, one_way_code, 
    			st_linefrommultipoint(st_collect(geom_point)) as the_geom
    	from
    	(
    		select	link_id, s_node, e_node, one_way_code, path, geom as geom_point
    		from
    		(
		        select	link_id, s_node, e_node, one_way_code, (st_dumppoints(geom_multiline)).*
		        from
		        (
		            select link_id, s_node, e_node, one_way_code, st_collect(the_geom) as geom_multiline
		            from
		            (
		                select	a.link_id, a.s_node, a.e_node, a.one_way_code, a.merge_index,
		                		(case when a.merge_link_dir then b.the_geom else st_reverse(b.the_geom) end) as the_geom
		                from 
		                (
		                	select * from temp_merge_newlink_maybe_circle 
		                	where st_geometrytype(the_geom) != 'ST_LineString' or st_isclosed(the_geom)
		                )as nlc
		                inner join temp_merge_link_mapping as a
		                on nlc.link_id = a.link_id
		                left join link_tbl as b
		                on a.merge_link_id = b.link_id
		                order by a.link_id, a.s_node, a.e_node, a.one_way_code, a.merge_index
		            )as a
		            group by link_id, s_node, e_node, one_way_code
		        )as b
	        )as c
	        where not (path[1] > 1 and path[2] = 1)
			order by link_id, s_node, e_node, one_way_code, path
        )as d
	    group by link_id, s_node, e_node, one_way_code
    )as e
);

create table temp_merge_newnode
as
(
	select	node_id, 
			array_to_string(link_array, '|') as node_lid
    from
    (
        select	node_id, 
        		array_agg(link_id) as link_array
        from
        (
            select    b.node_id, c.link_id
            from
            (
                select distinct node_id
                from
                (
	                select s_node as node_id from temp_merge_linkrow
	                union
	                select e_node as node_id from temp_merge_linkrow
                )as a
            )as b
            left join link_tbl as c
            on b.node_id in (c.s_node, c.e_node)
        )as d
        group by node_id
    )as e
);

create table link_tbl_bak_merge
as
(
	select * from link_tbl
);

create table node_tbl_bak_merge
as
(
	select * from node_tbl
);

CREATE TABLE mid_admin_zone
(
  gid serial primary key,
  ad_code integer,
  ad_order smallint,
  order0_id integer,
  order1_id integer,
  order2_id integer,
  order8_id integer,
  ad_name_single character varying(65536),
  ad_name character varying(65536),
  time_zone smallint,
  summer_time_id smallint
);SELECT AddGeometryColumn('','mid_admin_zone','the_geom','4326','MULTIPOLYGON',2);

CREATE TABLE mid_admin_summer_time
(
  gid serial primary key,
  summer_time_id smallint,
  start_date smallint,
  start_weekday smallint,
  start_time smallint,
  end_date smallint,
  end_weekday smallint,
  end_time smallint
);

CREATE TABLE mid_logmark
(
  gid serial primary key,
  poi_id bigint,
  type_code integer,
  type_code_priority integer not null  default 0,
  building_name varchar,
  the_geom geometry
);

-- table related to splitting link
create table temp_split_link_envelope_tile
(
	link_id		bigint,
	tile_z		integer,
	x_min		integer,
	x_max		integer,
	y_min		integer,
	y_max		integer,
	tile_x		integer,
	tile_y		integer,
	tile_id		integer
);

create table temp_split_link_span_tile
(
	gid			serial primary key,
	link_id		bigint,
	tile_id		integer
);

create table temp_split_tile
(
	tile_id		integer,
	tile_x				integer,
	tile_y				integer,
	tile_z				integer,
	dummy_border_geom	geometry
); SELECT AddGeometryColumn('','temp_split_tile','the_geom','4326','POLYGON',2);

create table temp_split_sublink_phase1
(
	gid 		serial PRIMARY KEY,
	link_id 	bigint,
	tile_id		integer
); SELECT AddGeometryColumn('','temp_split_sublink_phase1','the_geom','4326','LINESTRING',2);

create table temp_split_sublink_phase2
(
	gid 		serial PRIMARY KEY,
	gid_phase1	integer,
	link_id 	bigint,
	tile_id		integer
); SELECT AddGeometryColumn('','temp_split_sublink_phase2','the_geom','4326','LINESTRING',2);

create table temp_split_sublink_phase3
(
	gid 		serial PRIMARY KEY,
	link_id 	bigint,
	tile_id		integer
); SELECT AddGeometryColumn('','temp_split_sublink_phase3','the_geom','4326','LINESTRING',2);

create table temp_split_sublink_phase4
(
	gid 		serial PRIMARY KEY,
	gid_phase3	integer,
	link_id 	bigint,
	tile_id		integer
); SELECT AddGeometryColumn('','temp_split_sublink_phase4','the_geom','4326','LINESTRING',2);

create table temp_split_sublink_phase5
(
	gid 		serial PRIMARY KEY,
	link_id 	bigint,
	sub_count	integer,
	sub_index	integer,
	tile_id		integer
); SELECT AddGeometryColumn('','temp_split_sublink_phase5','the_geom','4326','LINESTRING',2);

create table temp_split_sublink_phase6
(
	gid 		serial PRIMARY KEY,
	link_id 	bigint,
	sub_count	integer,
	sub_index	integer,
	tile_id		integer
); SELECT AddGeometryColumn('','temp_split_sublink_phase6','the_geom','4326','LINESTRING',2);


create table temp_split_short_sublink
as
(
	select a.link_id, b.s_node, b.e_node, a.sub_count, a.sub_index
	from temp_split_sublink_phase5 as a
	left join link_tbl_bak_splitting as b
	on a.link_id = b.link_id
	where ST_Length_Spheroid(a.the_geom,'SPHEROID("WGS_84", 6378137, 298.257223563)') <= 1
);

create table temp_split_short_node_degree
as
(
	select node_id, count(*) as degree
	from
	(
		-- related nodes
		select distinct node_id
		from
		(
			select unnest(ARRAY[b.s_node, b.e_node]) as node_id
			from temp_split_short_sublink as a
			left join link_tbl_bak_splitting as b
			on a.link_id = b.link_id
		)as t
	)as a
	left join link_tbl_bak_splitting as b
	on a.node_id in (b.s_node, b.e_node)
	group by a.node_id
);

create table temp_split_keep_sublink
as
(
	select	distinct link_id, sub_count, unnest(sub_index_array) as sub_index
	from
	(
		select	t.link_id, t.sub_count, 
				(
				case 
				when a.degree = 2 and b.degree = 2 then ARRAY[1, t.sub_count]
				when a.degree = 2 then ARRAY[1]
				when b.degree = 2 then ARRAY[t.sub_count]
				else ARRAY[1, t.sub_count]
				end
				)as sub_index_array
		from
		(
			select link_id, s_node, e_node, sub_count
			from temp_split_short_sublink
			group by link_id, s_node, e_node, sub_count
			having count(*) = sub_count
		)as t
		left join temp_split_short_node_degree as a
		on t.s_node = a.node_id
		left join temp_split_short_node_degree as b
		on t.e_node = b.node_id
	)as t
);

create table temp_split_delete_sublink
as
(
	select a.link_id, a.s_node, a.e_node, a.sub_count, a.sub_index
	from temp_split_short_sublink as a
	left join temp_split_keep_sublink as b
	on a.link_id = b.link_id and a.sub_index = b.sub_index
	where b.link_id is null
);

create table temp_split_delete_sublink_environment
as
(
	select 	link_id, sub_count, 
			array_agg(tile_id) as tile_array, 
			array_agg(tx) as tx_array, 
			array_agg(ty) as ty_array, 
			array_agg(delete_flag) as delete_flag_array,
			array_agg(the_geom) as geom_array
	from
	(
		select 	b.link_id, b.sub_count, b.sub_index, b.the_geom, 
				b.tile_id, (b.tile_id >> 14) & ((1 << 14) - 1) as tx, b.tile_id & ((1 << 14) - 1) as ty, 
				(c.link_id is not null) as delete_flag
		from
		(
			select distinct b.link_id
			from temp_split_short_node_degree as a
			left join link_tbl_bak_splitting as b
			on a.node_id in (b.s_node, b.e_node)
		)as a
		left join temp_split_sublink_phase5 as b
		on a.link_id = b.link_id
		left join temp_split_delete_sublink as c
		on b.link_id = c.link_id and b.sub_index = c.sub_index
		order by b.link_id, b.sub_count, b.sub_index
	)as t
	group by link_id, sub_count
);

create table temp_split_move_node
as
(
	select	node_id,
			(
			case 
			when x_must is not null and y_must is not null then st_setsrid(st_point(x_must,y_must), 4326)
			when x_must is not null then st_setsrid(st_point(x_must,y_proxy), 4326)
			when y_must is not null then st_setsrid(st_point(x_proxy,y_must), 4326)
			else st_setsrid(st_point(x_proxy,y_proxy), 4326)
			end
			)as the_geom
	from
	(
		select	node_id,
				max(x_must) as x_must, 
				max(y_must) as y_must,
				max(x_proxy) as x_proxy, 
				max(y_proxy) as y_proxy
		from
		(
			select 	a.node_id, 
					(rdb_get_proxy_xy_advice(b.sub_count, b.sub_index, c.geom_array, c.delete_flag_array, c.tx_array, c.ty_array)).*
			from temp_split_short_node_degree as a
			inner join temp_split_delete_sublink as b
			on (a.node_id = b.s_node and b.sub_index = 1) or (a.node_id = b.e_node and b.sub_index = b.sub_count)
			inner join temp_split_delete_sublink_environment as c
			on b.link_id = c.link_id
		)as t
		group by node_id
	)as t
);

create table temp_split_delete_sublink_point
as
(
	select 	a.link_id, a.sub_count, a.sub_index, 
			rdb_find_proxy_point_of_short_sublink(	c.the_geom, d.the_geom, 
													a.sub_count, a.sub_index, 
													b.geom_array, b.delete_flag_array, 
													b.tx_array, b.ty_array
			) as proxy_point
	from temp_split_delete_sublink as a
	left join temp_split_delete_sublink_environment as b
	on a.link_id = b.link_id
	left join temp_split_move_node as c
	on a.s_node = c.node_id
	left join temp_split_move_node as d
	on a.e_node = d.node_id
);

create table temp_split_move_link
as
(
	select	a.link_id, 
			(
            case 
            when c.node_id is null then st_setpoint(a.the_geom, 0, b.the_geom)
            when b.node_id is null then st_setpoint(a.the_geom, st_numpoints(a.the_geom)-1, c.the_geom)
            else st_setpoint(st_setpoint(a.the_geom, 0, b.the_geom), st_numpoints(a.the_geom)-1, c.the_geom)
            end
            )as the_geom
	from link_tbl_bak_splitting as a
	left join temp_split_move_node as b
	on a.s_node = b.node_id
	left join temp_split_move_node as c
	on a.e_node = c.node_id
	where b.node_id is not null or c.node_id is not null
);

create table temp_split_move_sublink
as
(
	select	a.link_id, a.sub_count, a.sub_index, a.the_geom,
			rdb_recalc_tile_for_move_sublink(	a.sub_count, a.sub_index, a.the_geom, 
												b.geom_array, b.delete_flag_array, 
												b.tile_array, b.tx_array, b.ty_array
			)as tile_id
	from
	(
		select 	a.link_id, a.sub_count, a.sub_index, 
				(
				case
				when f.node_id is null then st_setpoint(a.the_geom, 0, d.the_geom)
				when d.node_id is null then st_setpoint(a.the_geom, st_numpoints(a.the_geom)-1, f.the_geom)
				else st_setpoint(st_setpoint(a.the_geom, 0, d.the_geom), st_numpoints(a.the_geom)-1, f.the_geom)
				end
				) as the_geom
		from temp_split_sublink_phase5 as a
		left join temp_split_delete_sublink as b
		on a.link_id = b.link_id and a.sub_index = b.sub_index
		left join link_tbl_bak_splitting as c
		on a.link_id = c.link_id and a.sub_index = 1
		left join temp_split_move_node as d
		on c.s_node = d.node_id
		left join link_tbl_bak_splitting as e
		on a.link_id = e.link_id and a.sub_index = a.sub_count
		left join temp_split_move_node as f
		on e.e_node = f.node_id
		where b.link_id is null and (d.node_id is not null or f.node_id is not null)
	)as a
	left join temp_split_delete_sublink_environment as b
	on a.link_id = b.link_id
);

create table temp_split_newnode
(
	gid 			serial PRIMARY KEY,
	node_id			bigint,
	old_link_id 	bigint,
	sub_index		integer,
	tile_id			integer
); SELECT AddGeometryColumn('','temp_split_newnode','the_geom','4326','POINT',2);

create table temp_split_newlink
(
	gid 			serial PRIMARY KEY,
	link_id			bigint,
	s_node			bigint,
	e_node			bigint,
	old_link_id 	bigint,
	sub_count		integer,
	sub_index		integer,
	tile_id			integer
); SELECT AddGeometryColumn('','temp_split_newlink','the_geom','4326','LINESTRING',2);

-- backup table for splitting link
CREATE TABLE regulation_relation_tbl_bak_splitting
as
(
	select * from regulation_relation_tbl
);

CREATE TABLE regulation_item_tbl_bak_splitting
as
(
	select * from regulation_item_tbl
);

create table temp_split_update_linkrow_regulation
as
(
	select  regulation_id, 
			nodeid, 
			link_array[1] as inlinkid, 
			(case when link_num = 1 then null else link_array[link_num] end) as outlinkid, 
			link_num, 
			link_array
	from
	(
		select  regulation_id, nodeid, link_array, array_upper(link_array, 1) as link_num
		from
		(
			select  regulation_id, nodeid, 
					rdb_get_split_linkrow(nodeid, link_array, s_array, e_array) as link_array
			from
			(
				select 	regulation_id, 
						nodeid,
						array_agg(linkid) as link_array, 
						array_agg(s_node) as s_array,
						array_agg(e_node) as e_array
				from
				(
					select 	a.regulation_id, b.nodeid, c.linkid, c.seq_num, d.s_node, d.e_node
					from
					(
						select distinct a.regulation_id
						from regulation_item_tbl_bak_splitting as a
						inner join temp_split_newlink as b
						on a.linkid = b.old_link_id and b.sub_index = 1
					)as a
					inner join 
					(
						select distinct regulation_id, nodeid
						from regulation_relation_tbl_bak_splitting
					)as b
					on a.regulation_id = b.regulation_id
					inner join regulation_item_tbl_bak_splitting as c
					on a.regulation_id = c.regulation_id and c.seq_num != 2
					left join link_tbl_bak_splitting as d
					on c.linkid = d.link_id
					order by c.regulation_id, b.nodeid, c.seq_num
				)as a
				group by regulation_id, nodeid
			)as b
		)as c
	)as d
);

CREATE TABLE link_tbl_bak_splitting
as
(
	select * from link_tbl
);

CREATE TABLE node_tbl_bak_splitting
as
(
	select * from node_tbl
);

-- backup table for merging link
CREATE TABLE regulation_relation_tbl_bak_merge
as
(
	select * from regulation_relation_tbl
);

CREATE TABLE regulation_item_tbl_bak_merge
as
(
	select * from regulation_item_tbl
);

create table temp_merge_update_linkrow_regulation
as
(
	select  regulation_id, 
			nodeid, 
			link_array[1] as inlinkid, 
			(case when link_num = 1 then null else link_array[link_num] end) as outlinkid, 
			link_num, 
			link_array
	from
	(
		select  regulation_id, nodeid, link_array, array_upper(link_array, 1) as link_num
		from
		(
			select  regulation_id, nodeid, 
					mid_construct_substitude_link_array(old_link_array, new_link_array) as link_array
			from
			(
				select 	regulation_id, 
						nodeid,
						array_agg(old_link_id) as old_link_array,
						array_agg(new_link_id) as new_link_array
				from
				(
					select 	a.regulation_id, b.nodeid, c.seq_num, 
							c.linkid as old_link_id,
							(case when d.link_id is not null then d.link_id else c.linkid end) as new_link_id 
					from
					(
						select distinct a.regulation_id
						from regulation_item_tbl_bak_merge as a
						inner join temp_merge_link_mapping as b
						on a.linkid = b.merge_link_id
					)as a
					inner join 
					(
						select distinct regulation_id, nodeid
						from regulation_relation_tbl_bak_merge
					)as b
					on a.regulation_id = b.regulation_id
					inner join regulation_item_tbl_bak_merge as c
					on a.regulation_id = c.regulation_id and c.seq_num != 2
					left join temp_merge_link_mapping as d
					on c.linkid = d.merge_link_id
					order by c.regulation_id, b.nodeid, c.seq_num
				)as a
				group by regulation_id, nodeid
			)as b
		)as c
	)as d
);

create table temp_move_shortlink
as
(
	select	link_id, tile_id, tz, tx, ty, size, 
			s_node, e_node, b_flag1, b_flag2,
			x1, y1, x2, y2, 
			px1, py1, px2, py2, 
			the_geom
	from
	(
		select	link_id, tile_id, tz, tx, ty, size, the_geom, 
				s_node, e_node, b_flag1, b_flag2,
				x1, y1, x2, y2,
				px(pxy1) as px1, py(pxy1) as py1,
				px(pxy2) as px2, py(pxy2) as py2
		from
		(
			select	link_id, a.tile_id, tz, tx, ty, size, a.the_geom, 
					s_node, e_node, 
					n1.tile_boundary_flag as b_flag1,
					n2.tile_boundary_flag as b_flag2,
					st_x(n1.the_geom) as x1, st_y(n1.the_geom) as y1, 
					st_x(n2.the_geom) as x2, st_y(n2.the_geom) as y2, 
					lonlat2pixel_tile(tz, tx, ty, st_x(n1.the_geom), st_y(n1.the_geom), size) as pxy1,
					lonlat2pixel_tile(tz, tx, ty, st_x(n2.the_geom), st_y(n2.the_geom), size) as pxy2
			from
			(
				select 	link_id, tile_id, s_node, e_node, the_geom,
						14 as tz, ((tile_id >> 14) & ((1 << 14) - 1)) as tx, (tile_id & ((1 << 14) - 1)) as ty, 4096 as size
				from link_tbl 
				where length <= 1
			)as a
			left join node_tbl as n1
			on a.s_node = n1.node_id
			left join node_tbl as n2
			on a.e_node = n2.node_id
		)as a
	)as b
	where px1 = px2 and py1 = py2
);


create table temp_move_shortlink_newshape
(
	gid serial NOT NULL,
	link_id		bigint
); SELECT AddGeometryColumn('','temp_move_shortlink_newshape','the_geom','4326','LINESTRING',2);

create table temp_move_shortlink_vertex_newpos
(
	gid serial NOT NULL,
	node_id		bigint
); SELECT AddGeometryColumn('','temp_move_shortlink_vertex_newpos','the_geom','4326','POINT',2);


CREATE TABLE link_tbl_bak_moving
as
(
	select * from link_tbl
);

CREATE TABLE node_tbl_bak_moving
as
(
	select * from node_tbl
);

create table dupli_link 
as
( 
	select a.* from link_tbl a
	inner join link_tbl b
	on ((a.s_node = b.s_node and a.e_node = b.e_node) or (a.s_node = b.e_node and a.e_node = b.s_node))
	and a.gid <> b.gid 
);

create table temp_dupli_link_tmp 
as
(
	select a.link_id as aid,a.the_geom as ageom,b.link_id as bid,b.the_geom as bgeom
		,case when (a.s_node = b.s_node and a.e_node = b.e_node) then 0 
		      when (a.s_node = b.e_node and a.e_node = b.s_node) then 1 
		 end as node_inverse
	from dupli_link a
	inner join dupli_link b
	on  ST_Equals(a.the_geom , b.the_geom)
	and  a.link_id  > b.link_id
);

create table temp_dupli_link 
as
(
	select a.* from temp_dupli_link_tmp a
	left join (
		select aid,min(bid) as bid from temp_dupli_link_tmp group by aid
	) b
	on a.aid = b.aid and a.bid = b.bid
	where b.aid is not null
);

create table temp_dupli_name_shield 
as 
(	
	select a.aid as link_id_a, b.road_name as name_a, b.road_number as shield_a,
		a.bid as link_id_b, c.road_name as name_b, c.road_number as shield_b
	from temp_dupli_link  a
	left join link_tbl_bak_dupli_dealing b
	on a.aid = b.link_id
	left join link_tbl_bak_dupli_dealing c
	on a.bid = c.link_id			
);

create table temp_circle_link 
as
(
	select aid,ageom,bid[1] as bid,bgeom[1] as bgeom from (
		select aid,ageom,array_agg(bid) as bid,array_agg(bgeom) as bgeom from (
			select aid,ageom,bid,bgeom from (
				select a.link_id as aid,a.the_geom as ageom,b.link_id as bid,b.the_geom as bgeom,c.aid as cid from dupli_link a
				inner  join dupli_link b
				on (a.s_node = b.s_node and a.e_node = b.e_node) or  (a.s_node = b.e_node and a.e_node = b.s_node)
				left join temp_dupli_link c
				on a.link_id = c.aid
			) a where aid > bid and cid is null
			and not ST_Equals(ageom , bgeom)
		) a group by aid,ageom
	) a
);

create table temp_circle_link_new_seq_link 
as 
(
	select  aid as old_link_id,nextval('temp_new_link_id_seq') as link_id
		,link_array[sub_index] as the_geom
		,sub_index as sub_index
		,2 as sub_count
		,case when sub_index = 1 then old_s_node when sub_index = 2 then currval('temp_new_node_id_seq') end as s_node
		,case when sub_index = 1 then nextval('temp_new_node_id_seq') when sub_index = 2 then old_e_node end as e_node
		,case when sub_index = 1 then st_endpoint(link_array[sub_index]) when sub_index = 2 then st_startpoint(link_array[sub_index]) end as e_geom
		,tile_id,bid,bgeom 
	from (
		select aid,generate_series(1, array_upper(link_array, 1)) as sub_index,link_array,bid,bgeom,old_s_node,old_e_node,tile_id from (
			select aid,mid_split_circle_link(ageom) as link_array,bid,bgeom,b.s_node as old_s_node,b.e_node as old_e_node,b.tile_id as tile_id 
				from temp_circle_link a
			inner join link_tbl_bak_dupli_dealing b
			on a.aid = b.link_id
		) a order by aid,sub_index
	) a
);

create table temp_circle_link_update_linkrow_regulation
as
(
	select  regulation_id, 
			nodeid, 
			link_array[1] as inlinkid, 
			(case when link_num = 1 then null else link_array[link_num] end) as outlinkid, 
			link_num, 
			link_array
	from
	(
		select  regulation_id, nodeid, link_array, array_upper(link_array, 1) as link_num
		from
		(
			select  regulation_id, nodeid, 
					rdb_get_split_linkrow_circle_dealing(nodeid, link_array, s_array, e_array) as link_array
			from
			(
				select 	regulation_id, 
						nodeid,
						array_agg(linkid) as link_array, 
						array_agg(s_node) as s_array,
						array_agg(e_node) as e_array
				from
				(
					select 	a.regulation_id, b.nodeid, c.linkid, c.seq_num, d.s_node, d.e_node
					from
					(
						select distinct a.regulation_id
						from regulation_item_tbl_bak_circle_dealing as a
						inner join temp_circle_link_new_seq_link as b
						on a.linkid = b.old_link_id and b.sub_index = 1
					)as a
					inner join 
					(
						select distinct regulation_id, nodeid
						from regulation_relation_tbl_bak_circle_dealing 
					)as b
					on a.regulation_id = b.regulation_id
					inner join regulation_item_tbl_bak_circle_dealing as c
					on a.regulation_id = c.regulation_id and c.seq_num != 2
					left join link_tbl_bak_circle_dealing as d
					on c.linkid = d.link_id
					order by c.regulation_id, b.nodeid, c.seq_num
				)as a
				group by regulation_id, nodeid
			)as b
		)as c
	)as d
);

CREATE TABLE highway_facility_tbl
(
  gid serial NOT NULL,
  facility_id integer,
  node_id bigint NOT NULL,
  separation smallint DEFAULT 2,
  name_id integer,
  junction smallint,
  pa smallint,
  sa smallint,
  ic smallint,
  ramp smallint DEFAULT 0,
  smart_ic smallint DEFAULT 0,
  tollgate smallint,
  dummy_tollgate smallint DEFAULT 0,
  tollgate_type smallint,
  service_info_flag smallint,
  in_out_type smallint,  -- in out type
  CONSTRAINT interchange_tbl_pkey PRIMARY KEY (gid)
); 

CREATE TABLE highway_sa_pa_tbl

(
  gid serial NOT NULL,
  sapa_id integer NOT NULL,
  node_id bigint,
  name_id integer,
  public_telephone smallint,
  vending_machine smallint,
  handicapped_telephone smallint DEFAULT 0,
  toilet smallint,
  handicapped_toilet smallint DEFAULT 0,
  gas_station smallint,
  natural_gas smallint,
  nap_rest_area smallint DEFAULT 0,
  rest_area smallint,
  nursing_room smallint DEFAULT 0,
  dinning smallint,
  repair smallint,
  shop smallint,
  launderette smallint DEFAULT 0,
  fax_service smallint DEFAULT 0,
  coffee smallint,
  post smallint DEFAULT 0,
  hwy_infor_terminal smallint DEFAULT 0,
  hwy_tot_springs smallint DEFAULT 0,
  shower smallint,
  image_id integer DEFAULT -1,
  CONSTRAINT highway_sa_pa_tbl_pkey PRIMARY KEY (gid)
); 

------------------------------------------------------------------------
create table mid_all_highway_node
(
  node_id  bigint not null primary key
);

create table regulation_relation_tbl_bak_mainnode
as
select * from regulation_relation_tbl;

create table regulation_item_tbl_bak_mainnode
as
select * from regulation_item_tbl;

create table temp_mainnode_sublink
(
	mainnode_id integer,
	sublink bigint
);

create table temp_mainnode_subnode
(
	mainnode_id integer,
	subnode bigint
);

create table temp_mainnode_regulation_new_linkrow
as
(
	select	regulation_id, nodeid, link_num, link_array, inner_link_array
	from
	(
		select	regulation_id, nodeid, link_num, link_array,
				string_to_array(unnest(inner_path_array), ',')::bigint[] as inner_link_array
		from
		(
			select 	a.regulation_id, link_num, a.link_array, b.nodeid, 
					mid_get_inner_path_array(a.link_array, b.nodeid) as inner_path_array
			from
			(
				select regulation_id, count(*) as link_num, array_agg(linkid) as link_array
				from
				(
					select *
					from regulation_item_tbl
					where seq_num != 2
					order by regulation_id, seq_num
				)as t1
				group by regulation_id having count(*) >= 2
			)as a
			left join regulation_item_tbl as b
			on a.regulation_id = b.regulation_id and b.seq_num = 2
		)as t1
	)as t2
	where link_array is distinct from inner_link_array
);

CREATE TABLE language_tbl
(
  language_id    smallint NOT NULL,
  l_full_name    character varying(40) NOT NULL,
  l_talbe        character varying(80) NOT NULL,
  pronunciation  character varying(40),
  p_table        character varying(80),
  language_code  character varying(3) NOT NULL,
  language_code_client character varying(3) NOT NULL,
  language_id_client   smallint NOT NULL,
  exist_flag           boolean DEFAULT false,
  CONSTRAINT language_tbl_pkey PRIMARY KEY (language_id)
);

create table temp_update_patch_force_guide_tbl
(
	objectid integer,
 	fromlinkid integer,
	tolinkid integer,
	midcount integer,
	midlinkid character varying(254),
	guidetype integer
);

create table temp_append_patch_force_guide_tbl
(
	objectid integer,
 	fromlinkid integer,
	tolinkid integer,
	midcount integer,
	midlinkid character varying(254),
	guidetype integer
);

CREATE TABLE temp_update_path_force_guide_tbl_matching_tbl
(
  new_objectid integer,
  new_fromlinkid integer,
  new_tolinkid integer,
  new_midcount integer,
  new_midlinkid character varying(254),
  new_guidetype integer,
  old_objectid integer,
  old_fromlinkid integer,
  old_tolinkid integer,
  old_midcount integer,
  old_midlinkid character varying(254),
  old_guidetype integer
);

create table temp_friendly_append_patch_force_guide_tbl
(
	objectid integer,
 	fromlinkid integer,
	tolinkid integer,
	midcount integer,
	midlinkid character varying(254),
	guidetype integer
);

CREATE TABLE temp_append_update_path_force_guide_tbl_matching_tbl
(
  new_objectid integer,
  new_fromlinkid integer,
  new_tolinkid integer,
  new_midcount integer,
  new_midlinkid character varying(254),
  new_guidetype integer,
  old_objectid integer,
  old_fromlinkid integer,
  old_tolinkid integer,
  old_midcount integer,
  old_midlinkid character varying(254),
  old_guidetype integer
);

CREATE TABLE temp_friendly_update_path_force_guide_tbl_matching_tbl
(
  new_objectid integer,
  new_fromlinkid integer,
  new_tolinkid integer,
  new_midcount integer,
  new_midlinkid character varying(254),
  new_guidetype integer,
  old_objectid integer,
  old_fromlinkid integer,
  old_tolinkid integer,
  old_midcount integer,
  old_midlinkid character varying(254),
  old_guidetype integer
);
CREATE TABLE crossname_tbl
(
  gid serial NOT NULL,
  inlinkid bigint,
  nodeid bigint,
  outlinkid bigint,
  passlid character(256),
  passlink_cnt integer,
  namestr character(1024)
);
------------------------------------------------------------------------
-- highway node add_info
CREATE TABLE highway_node_add_info
(
  gid              SERIAL PRIMARY KEY not null,
  link_id          BIGINT not null,
  node_id          BIGINT not null,
  toll_flag        SMALLINT not null DEFAULT 0,
  no_toll_money    SMALLINT not null DEFAULT 0,
  ---------------------------------------------
  facility_num     SMALLINT not null,
  up_down          SMALLINT not null,
  facility_id      INTEGER not null,
  seq_num          SMALLINT not null DEFAULT 0,
  dir_s_node       BIGINT not null,
  dir_e_node       BIGINT not null,
  etc_antenna      SMALLINT not null DEFAULT 0,
  enter            SMALLINT not null DEFAULT 0,
  exit             SMALLINT not null DEFAULT 0,
  jct              SMALLINT not null DEFAULT 0,
  sapa             SMALLINT not null DEFAULT 0,
  gate             SMALLINT not null DEFAULT 0,
  un_open          SMALLINT not null DEFAULT 0,
  dummy            SMALLINT not null DEFAULT 0,
  ---------------------------------------------
  toll_type_num    SMALLINT not null DEFAULT 0,
  non_ticket_gate  smallint not null DEFAULT 0,
  check_gate       smallint not null DEFAULT 0,
  single_gate      smallint not null DEFAULT 0,
  cal_gate         smallint not null DEFAULT 0,
  ticket_gate      smallint not null DEFAULT 0,
  nest             smallint not null DEFAULT 0,
  uturn            smallint not null DEFAULT 0,
  not_guide        smallint not null DEFAULT 0,
  normal_toll      smallint not null DEFAULT 0,
  etc_toll         smallint not null DEFAULT 0,
  etc_section      smallint not null DEFAULT 0,
  name             CHARACTER VARYING(1024),
  tile_id          INTEGER not null,
  no_toll_flag     smallint not null  -- 1: no toll, 0: toll
);

------------------------------------------------------------------------
-- highway ic info
CREATE TABLE highway_ic_info
(
  gid              serial not null,
  ic_no            integer not null,
  up_down          smallint not null,
  facility_id      integer not null,
  between_distance float not null,
  inside_distance  float not null,
  enter            smallint not null,
  exit             smallint not null,
  tollgate         smallint not null,
  jct              smallint not null,
  pa               smallint not null,
  sa               smallint not null,
  ic               smallint not null,
  ramp             smallint not null,
  smart_ic         smallint not null,
  barrier          smallint not null,
  dummy            smallint not null,
  boundary         smallint not null,
  "new"            smallint not null,
  unopen           smallint not null,
  forward_flag     smallint not null,
  reverse_flag     smallint not null,
  toll_count       smallint not null,
  enter_direction  smallint not null,
  path_count       smallint not null,
  vics_count       smallint not null,
  conn_count       smallint not null,
  illust_count     smallint not null,
  store_count      smallint not null,
  servise_flag     smallint not null,
  road_no          integer not null,
  name             character varying(4096),
  conn_tile_id     integer not null,
  tile_id          integer not null,
  node_id          bigint
);

------------------------------------------------------------------------
create table highway_road_info
(
  gid             serial not null primary key,
  road_no         integer not null,
  iddn_road_kind  smallint not null, 
  road_kind       smallint not null, 
  road_attr       smallint not null,
  loop            smallint not null,
  new             smallint not null,
  un_open         smallint not null,
  name            character varying(4096),
  up_down         integer not null,
  road_number     character varying  -- Just for Service DB
);

------------------------------------------------------------------------
CREATE TABLE highway_conn_info
(
  gid                 serial not null primary key, 
  ic_no               integer not null,
  road_attr           smallint not null,
  conn_direction      smallint not null,
  same_road_flag      smallint not null,
  tile_change_flag    smallint not null,
  uturn_flag          smallint not null,
  new_flag            smallint not null,
  unopen_flag         smallint not null,
  vics_flag           smallint not null,
  toll_flag           smallint not null,
  conn_road_no        smallint not null,
  conn_ic_no          integer not null,
  conn_link_length    float not null,
  conn_tile_id        integer not null,
  --toll_index          smallint not null,    
  toll_count          smallint not null,
  --vics_index          smallint not null,
  vics_count          smallint not null,
  tile_id             integer not null,
  ic_name             character varying(4096),
  conn_ic_name        character varying(4096)
);

------------------------------------------------------------------------
-- toll info
CREATE TABLE highway_toll_info
(
  toll_no          serial not null primary key,
  ic_no            INTEGER not null,
  conn_ic_no       INTEGER,
  toll_class       smallint not null,
  class_name       character varying(5),
  up_down          SMALLINT not null,
  facility_id	   INTEGER not null,
  tollgate_count   smallint not null,
  etc_antenna      smallint not null,
  enter            smallint not null,
  exit             smallint not null,
  jct              smallint not null,
  sa_pa            smallint not null,
  gate             smallint not null,
  unopen           smallint not null,
  dummy            smallint not null,
  non_ticket_gate  smallint not null,
  check_gate       smallint not null,
  single_gate      smallint not null,
  cal_gate         smallint not null,
  ticket_gate      smallint not null,
  nest             smallint not null,
  uturn            smallint not null,
  not_guide        smallint not null,
  normal_toll      smallint not null,
  etc_toll         smallint not null,
  etc_section      smallint not null,
  node_id          bigint not null,
  road_code        integer not null,
  road_seq         integer not null,
  name             character varying(4096),
  dummy_toll_node  bigint
);

------------------------------------------------------------------------
CREATE TABLE mid_hwy_node_add_info
(
  ic_no          integer,              -- NULL, when it is no toll facility
  updown         integer not null,
  facility_id    integer not null,
  facilcls       smallint not null,
  in_out         smallint not null,
  link_lid       character varying(2048),
  node_lid       character varying(2048),
  add_node_id    bigint NOT NULL,
  add_link_id    bigint not null,
  dir_s_node     bigint not null,
  dir_e_node     bigint not null,
  name_kanji     character varying(4096),
  name_yomi      character varying(4096),
  pos_type       integer not null,
  pos_type_name  character varying(32),
  toll_flag      smallint not null     -- 1: toll, 0: No Toll
);

------------------------------------------------------------------------
CREATE TABLE mid_hwy_node_add_info_merged
(
  ic_no          integer,              -- NULL, when it is no toll facility
  updown         integer not null,
  facility_id    integer not null,
  facilcls       smallint not null,
  in_out         smallint not null,
  link_lid       character varying(2048),
  node_lid       character varying(2048),
  add_node_id    bigint NOT NULL,
  add_link_id    bigint not null,
  dir_s_node     bigint not null,
  dir_e_node     bigint not null,
  facil_name     character varying(4096),
  pos_type       integer not null,
  pos_type_name  character varying(32),
  toll_flag      smallint not null     -- 1: toll, 0: No Toll
);

------------------------------------------------------------------------
-- store info
create table highway_store_info 
(
  gid             serial not null primary key,
  ic_no           integer not null,
  bis_time_flag   smallint not null,
  bis_time_num    smallint not null,
  store_kind      INTEGER not null,
  open_hour       smallint not null,
  open_min        smallint not null,
  close_hour      smallint not null,
  close_min       smallint not null,
  holiday         smallint not null,
  goldenWeek      smallint not null,
  newyear         smallint not null,
  yearend         smallint not null,
  bonfestival     smallint not null,
  Sunday          smallint not null,
  Saturday        smallint not null,
  Friday          smallint not null,
  Thursday        smallint not null,
  Wednesday       smallint not null,
  Tuesday         smallint not null,
  Monday          smallint not null,
  seq_nm          smallint not null,
  store_name      character varying,
  priority        double precision,
  service_kind    character varying
);

------------------------------------------------------------------------
CREATE TABLE highway_store_picture
(
  store_kind      integer NOT NULL primary key,
  picture_name    character varying(60) NOT NULL
);

------------------------------------------------------------------------
CREATE TABLE highway_store_name
(
  gid              serial not null primary key,
  store_kind       integer not null,
  name             character varying(100),
  language_code    character(3)
);

-----------------------------------------------------------------------------
CREATE TABLE highway_path_point
(
  gid          serial not null primary key,
  ic_no        integer not null,
  enter_flag   smallint not null,
  exit_flag    smallint not null,
  main_flag    smallint not null,
  center_flag  smallint not null,
  new_flag     smallint not null,
  unopen_flag  smallint not null,
  pos_flag     smallint not null,
  link_id      bigint,
  node_id      bigint not null,
  tile_id      integer not null
);
-----------------------------------------------------------------------------
create table highway_mapping
(
  gid            serial not null primary key,
  road_kind      smallint,
  ic_count       smallint,
  road_no        smallint,
  display_class  smallint,
  link_id        bigint not null,
  forward_ic_no  integer,
  backward_ic_no integer,
  path_type      character varying(10),
  tile_id        integer not null
);

------------------------------------------------------------------------
-- highway illust info
create table highway_illust_info
(
  gid          serial not null,
  ic_no        integer not null primary key,
  image_id     character varying(60) not null
);

------------------------------------------------------------------------
-- highway service info
create table highway_service_info
(
  gid                     serial not null primary key,
  ic_no                   integer not null,
  dog_run                 smallint,
  hwy_oasis               smallint,
  public_telephone        smallint,
  vending_machine         smallint,
  handicapped_telephone   smallint,
  handicapped_toilet      smallint,
  information             smallint,
  snack_corner            smallint,
  nursing_room            smallint,
  gas_station             smallint,
  launderette             smallint,
  coin_shower             smallint,
  toilet                  smallint,
  rest_area               smallint,
  shopping_corner         smallint,
  fax_service             smallint,
  postbox                 smallint,
  hwy_infor_terminal      smallint,
  atm                     smallint,
  hwy_hot_springs         smallint,
  nap_rest_area           smallint,
  restaurant              smallint
);

------------------------------------------------------------------------
CREATE TABLE highway_fee_toll_info
(
  gid                serial not null primary key,
  up_down            integer not null,
  facility_id        integer not null,
  ticket_flag        smallint not null,
  alone_flag         smallint not null,
  free_flag          smallint not null
);

------------------------------------------------------------------------
CREATE TABLE highway_fee_free_toll
(
  gid                serial not null primary key,
  from_facility_id   INTEGER NOT NULL,
  to_facility_id     INTEGER NOT NULL
);

------------------------------------------------------------------------
CREATE TABLE highway_fee_alone_toll
(
  gid             SERIAL NOT NULL PRIMARY KEY,
  up_down         SMALLINT NOT NULL,  -- 0: down, 1: up, 3: up money == down money.
  facility_id     INTEGER NOT NULL,
  etc_discount_c  INTEGER NOT NULL,
  k_money         integer NOT NULL,
  s_money         integer NOT NULL,
  m_money         integer NOT NULL,
  l_money         integer NOT NULL,
  vl_money        integer NOT NULL
);

------------------------------------------------------------------------
CREATE TABLE highway_fee_ticket_toll
(
  gid                serial not null primary key,
  from_facility_id   integer not null,
  to_facility_id     integer not null,
  etc_discount_c     integer not null,
  k_money            integer NOT NULL,
  s_money            integer NOT NULL,
  m_money            integer NOT NULL,
  l_money            integer NOT NULL,
  vl_money           integer NOT NULL
);

------------------------------------------------------------------------
CREATE TABLE highway_fee_temp_toll
(
  gid                 serial not null primary key,
  facility_id         INTEGER NOT NULL,
  temptollclass_c     smallint NOT NULL,  -- 1: about, 2: above
  etc_discount_c      smallint NOT NULL
);

------------------------------------------------------------------------
-- toll same facility
CREATE TABLE highway_fee_same_facility
(
  gid                 serial not null primary key,
  from_up_down        integer NOT NULL,
  from_facility_id    integer not null,
  to_up_down          integer NOT NULL,
  to_facility_id      integer not null
);

------------------------------------------------------------------------
-- Link, which's road_type is hwy, but are not included in hwy model.
CREATE TABLE highway_not_hwy_model_link
(
   link_id   bigint not null primary key
);

------------------------------------------------------------------------------
----park link\node\ploygon
CREATE TABLE park_link_tbl
(
	park_link_id bigint not null,
	park_link_oneway smallint,
	park_s_node_id bigint,
	park_e_node_id bigint,
	park_link_length integer,
	park_link_type smallint,
	park_link_connect_type smallint,
	park_floor smallint,
	park_link_lean smallint,
	park_link_toll smallint,
	park_link_add1 smallint,
	park_link_add2 smallint,
	park_region_id integer
);SELECT AddGeometryColumn('','park_link_tbl','the_geom','4326','LINESTRING',2);

CREATE TABLE park_link_tbl_bak_splitting
as
(
	select * from park_link_tbl
);
create table park_link_tbl_bak_merge
as
(
	select * from park_link_tbl
);

------------------------------------------------------------------------
CREATE TABLE park_node_tbl
(
	park_node_id bigint not null,
	base_node_id bigint,
	park_node_type smallint,
	node_name character varying(1024),
	park_region_id bigint
);SELECT AddGeometryColumn('','park_node_tbl','the_geom','4326','POINT',2);

CREATE TABLE park_node_tbl_bak_splitting
as
(
	select * from park_node_tbl
);
create table park_node_tbl_bak_merge
as
(
	select * from park_node_tbl
);

------------------------------------------------------------------------
CREATE TABLE park_polygon_tbl
(
	park_region_id integer not null,
	park_type smallint,
	park_toll smallint,
	park_map_ID bigint,
	park_name character varying(1024)
);SELECT AddGeometryColumn('','park_polygon_tbl','the_geom','4326','MULTIPOLYGON',2);
------------------------------------------------------------------------
create table temp_dupli_name_shield_comp
(
  id          bigint not null primary key,
  name        character varying,
  shield      character varying
);
create table building_structure
(
gid serial not null,
id serial not null,
inlinkid bigint,
nodeid bigint,
outlinkid bigint,
type_code integer,
centroid_lontitude integer,
centroid_lantitude integer,
building_name character varying
);
CREATE TABLE safety_zone_tbl
(
  gid serial NOT NULL,
  safetyzone_id integer,
  linkid bigint,
  speedlimit smallint,
  speedunit_code smallint,
  direction smallint,
  safety_type smallint
);
CREATE TABLE safety_alert_tbl
(
gid serial not null,
featid bigint,
lon integer,
lat integer,
link_id bigint,
orglink_id bigint,
type smallint,
angle smallint,
dir smallint,
s_dis int,
e_dis int,
speed smallint,
speed_unit smallint
); SELECT AddGeometryColumn('','safety_alert_tbl','the_geom','4326','POINT',2);

CREATE TABLE grad
(
  objectid integer NOT NULL,
  link_id bigint NOT NULL,
  grad_value integer NOT NULL
); SELECT AddGeometryColumn('','grad','shape','4326','MULTILINESTRING',2);

-------------------------------------------------
-- For splitting link by tollgates.

create table temp_link_after_split 
as
(
	select m.link_id as old_link_id,m.link_id ,m.s_node,m.e_node
		,1 as sub_count,1 as sub_index,r.tile_id
		,case when p.link_id is not null then p.the_geom
			else m.the_geom
		end as the_geom 
	from link_tbl_bak_splitting m
	left join temp_split_newlink n
	on m.link_id = n.old_link_id
	left join temp_split_link_envelope_tile r
	on m.link_id = r.link_id
	left join temp_split_move_link p
	on m.link_id = p.link_id	
	where n.old_link_id is null
union
	select n.old_link_id,n.link_id,n.s_node,n.e_node
		,n.sub_count,n.sub_index,n.tile_id,n.the_geom
	from link_tbl_bak_splitting m
	left join temp_split_newlink n
	on m.link_id = n.old_link_id
	where n.old_link_id is not null
);

create table temp_tollgate_before_split 
as
(
	select id,d.old_link_id,d.link_id,d.s_node,d.e_node,d.sub_count
		,d.sub_index,d.tile_id,a.the_geom as node_geom,d.the_geom as link_geom 
	from (
		--- Get links after dupli&circle dealing.
		select id_temp as id
			,case when r.link_id is not null then r.link_id
				else p.cltrpelid 
			end as link_id
			,p.the_geom 
		from (
					select (array_agg(id))[1] as id_temp, cltrpelid, the_geom
					from
					(
						select id,cltrpelid,the_geom from org_mn_pi
							where feattyp = 7375
						union
						select id,cltrpelid,the_geom from org_mnpoi_pi
							where feattyp = 7375
					)temp
					group by cltrpelid, the_geom
		) p
		left join temp_dupli_link q
		on p.cltrpelid = q.aid
		left join temp_circle_link_new_seq_link r
		on p.cltrpelid = r.old_link_id and ST_DWithin(p.the_geom, r.the_geom, 0.000000694945852358566745)
		where q.aid is null
	) a
	left join temp_link_after_split d
	on a.link_id = d.old_link_id
	order by id,d.old_link_id,d.sub_index
);

create table temp_tollgate_not_split 
as
(
	select *
		,case when length * fraction <= 5 then s_node
			when length * (1 - fraction) <= 5 then e_node
			else null
		end as node_id
	from (
		select *,ST_Length_Spheroid(link_geom,'SPHEROID("WGS_84", 6378137, 298.257223563)') as length
			,mid_get_fraction(link_geom,node_geom) as fraction
		from temp_tollgate_before_split
		where ST_DWithin(node_geom, link_geom, 0.000000694945852358566745)
	) a where length * fraction < 5 or length * (1 - fraction) < 5
);

create table temp_tollgate_split_link 
as
(
	select old_link_id,link_id,s_node,e_node,sub_count,sub_index,tile_id,link_geom
		,index,sum
		,nextval('new_linkid_for_tollgate') as new_link_id
		,case when index = 1 then ST_Line_Substring(link_geom,0,fraction_array[index])
			when index = sum then ST_Line_Substring(link_geom,fraction,1)
			else ST_Line_Substring(link_geom,fraction,fraction_array[index])
		end as new_geom 
	from (
		select unnest(array_prepend(null,id_array)) as id
			,old_link_id,link_id,s_node,e_node,sub_count,sub_index,tile_id,link_geom
			,unnest(array_prepend(null,node_geom_array)) as node_geom
			,unnest(array_prepend(0::double precision,fraction_array)) as fraction
			,fraction_array
			,sum
			,generate_series(1,sum) as index
		from (
			select old_link_id,link_id,s_node,e_node,sub_count,sub_index,tile_id,link_geom
				,array_agg(id) as id_array
				,array_agg(node_geom) as node_geom_array
				,array_agg(fraction) as fraction_array
				,array_upper(array_agg(id),1) + 1 as sum
			from (
				select a.*,ST_Line_Locate_Point(a.link_geom,a.node_geom) as fraction 
				from temp_tollgate_before_split a
				left join temp_tollgate_not_split b
				on a.id = b.id 
				where ST_DWithin(a.node_geom, a.link_geom, 0.000000694945852358566745) 
				and b.id is null
				order by a.old_link_id,a.sub_index,fraction
			) c group by old_link_id,link_id,s_node,e_node,sub_count,sub_index,tile_id,link_geom
		) d
	) e
);

create table temp_tollgate_split_node 
as
(
	select  nextval('new_nodeid_for_tollgate') as new_node_id
		,old_link_id
		,link_id
		,(index - 1) as index
		,tile_id
		,st_startpoint(new_geom) as the_geom
	from temp_tollgate_split_link
	where index > 1 order by old_link_id,link_id,index
);

create table temp_split_newlink_bak 
as 
(
	select * from temp_split_newlink
);

create table temp_split_newnode_bak 
as
(
	select * from temp_split_newnode
);

CREATE TABLE temp_tollgate
(
  node_id bigint
);

CREATE TABLE temp_tollgate_electronic
(
  old_link_id bigint,
  s_node bigint,
  e_node bigint,
  sum integer,
  index integer,
  tile_id integer,
  new_link_id bigint,
  one_way_code smallint,
  etc_only_flag smallint,
  erp_flag integer
);
------------------------------------------------------------------------
--
CREATE TABLE mid_temp_hwy_main_link
(
  link_id       bigint NOT NULL,
  s_node        bigint,
  e_node        bigint,
  one_way_code  smallint,
  link_type     smallint,
  road_type     smallint,
  display_class smallint,
  toll          smallint,
  fazm          integer,
  tazm          integer,
  tile_id       integer,
  length        double precision,
  road_name     character varying(16384),
  road_number   character varying(2048)
); SELECT AddGeometryColumn('','mid_temp_hwy_main_link','the_geom','4326','LINESTRING',2);

------------------------------------------------------------------------
--
CREATE TABLE mid_temp_hwy_ic_link
(
  link_id       bigint NOT NULL,
  s_node        bigint,
  e_node        bigint,
  one_way_code  smallint,
  link_type     smallint,
  road_type     smallint,
  display_class smallint,
  toll          smallint,
  fazm          integer,
  tazm          integer,
  tile_id       integer,
  length        double precision,
  road_name     character varying(16384),
  road_number   character varying(2048)
); SELECT AddGeometryColumn('','mid_temp_hwy_ic_link','the_geom','4326','LINESTRING',2);

------------------------------------------------------------------------
--
CREATE TABLE mid_temp_hwy_inout_link
(
  link_id       bigint NOT NULL,
  s_node        bigint,
  e_node        bigint,
  one_way_code  smallint,
  link_type     smallint,
  road_type     smallint,
  display_class smallint,
  toll          smallint,
  fazm          integer,
  tazm          integer,
  tile_id       integer,
  length        double precision,
  road_name     character varying(16384),
  road_number   character varying(2048)
); SELECT AddGeometryColumn('','mid_temp_hwy_inout_link','the_geom','4326','LINESTRING',2);

------------------------------------------------------------------------
--
CREATE TABLE mid_temp_hwy_relation_link
(
  link_id bigint NOT NULL PRIMARY KEY,
  s_node bigint,
  e_node bigint,
  one_way_code smallint,
  link_type smallint,
  road_type smallint,
  display_class smallint,
  toll smallint,
  fazm integer,
  tazm integer,
  tile_id integer,
  length double precision,
  road_name character varying(16384),
  road_number character varying(2048)
);

------------------------------------------------------------------------
--
create table mid_temp_hwy_relation_regulation
(
  regulation_id    integer not null,
  cond_id          integer,
  link_lid         bigint[] not null,
  edges            text[]
);

------------------------------------------------------------------------
--
CREATE TABLE mid_temp_hwy_main_path
(
  path_id     integer NOT NULL,
  updown      integer DEFAULT 1,
  node_id     bigint NOT NULL,
  link_id     bigint,
  seq_nm      integer NOT NULL
); --SELECT AddGeometryColumn('','mid_temp_hwy_main_path','the_geom','4326','LINESTRING',2);

------------------------------------------------------------------------
--
CREATE TABLE mid_temp_hwy_main_path_attr
(
  path_id        integer NOT NULL primary key,
  length         double precision NOT NULL,
  road_name      character varying,
  road_number    character varying,
  sort_name      character varying,
  sort_number    character varying,
  fazm           integer,
  tazm           integer,
  side_path_flg  integer NOT NULL DEFAULT 0,
  delete_flag    integer NOT NULL DEFAULT 0
);

------------------------------------------------------------------------
--
CREATE TABLE mid_temp_hwy_main_cut_path_attr
(
  path_id        integer NOT NULL primary key,
  length         double precision NOT NULL,
  road_name      character varying,
  road_number    character varying,
  sort_name      character varying,
  sort_number    character varying,
  start_idx      integer NOT NULL,
  end_idx        integer NOT NULL
);

------------------------------------------------------------------------
--
CREATE TABLE road_code_info
(
  road_code     serial NOT NULL primary key,
  road_name     character varying,
  road_number   character varying,
  path_id       integer,
  sort_name     character varying,
  sort_number   character varying
);

------------------------------------------------------------------------
--
CREATE TABLE hwy_link_road_code_info
(
  gid           serial NOT NULL primary key,
  road_code     integer NOT NULL,
  updown        integer NOT NULL DEFAULT 1,
  node_id       bigint NOT NULL,
  link_id       bigint,
  seq_nm        integer NOT NULL
); SELECT AddGeometryColumn('','hwy_link_road_code_info','the_geom','4326','LINESTRING',2);

------------------------------------------------------------------------
--
CREATE TABLE mid_temp_hwy_ic_path
(
   gid            serial not null primary key,
   road_code      integer not null,
   road_seq       integer not null,
   facilcls_c     integer not null,
   inout_c        integer not null,
   updown_c       integer NOT NULL,
   node_id        bigint not null,
   to_node_id     bigint not null,
   node_lid       character varying,
   link_lid       character varying,
   path_type      character varying,
   facil_name     character varying
);

------------------------------------------------------------------------
--
CREATE TABLE mid_temp_hwy_jct_uturn_del
(
  gid             integer NOT NULL primary key,
  road_code       integer,
  road_seq        integer,
  facilcls_c      integer,
  inout_c         integer,
  node_id         bigint,
  to_node_id      bigint,
  node_lid        character varying,
  link_lid        character varying,
  path_type       character varying
);

------------------------------------------------------------------------
--
CREATE TABLE mid_temp_hwy_ic_del
(
  gid             integer NOT NULL primary key,
  road_code       integer,
  road_seq        integer,
  facilcls_c      integer,
  inout_c         integer,
  node_id         bigint,
  to_node_id      bigint,
  node_lid        character varying,
  link_lid        character varying,
  path_type       character varying
);

------------------------------------------------------------------------
--
CREATE TABLE mid_temp_hwy_sapa_del
(
  gid             integer NOT NULL primary key,
  road_code       integer,
  road_seq        integer,
  facilcls_c      integer,
  inout_c         integer,
  node_id         bigint,
  to_node_id      bigint,
  node_lid        character varying,
  link_lid        character varying,
  path_type       character varying
);

------------------------------------------------------------------------
--
CREATE TABLE mid_temp_hwy_service_road_path1
(
   gid             serial not null primary key,
   inout_c         integer not null,
   node_id         bigint not null,
   to_node_id      bigint not null,
   node_lid        character varying not null,
   link_lid        character varying not null,
   road_code       integer not null,
   updown_c        integer NOT NULL
);

------------------------------------------------------------------------
--
CREATE TABLE mid_temp_hwy_service_road_path2
(
   gid             serial not null primary key,
   inout_c         integer not null,
   node_id         bigint not null,
   to_node_id      bigint not null,
   node_lid        character varying not null,
   link_lid        character varying not null,
   road_code       integer not null,
   updown_c        integer NOT NULL
);


------------------------------------------------------------------------
CREATE TABLE mid_temp_hwy_service_road_path_del
(
gid        integer not null primary key,
node_lid   character varying
);

------------------------------------------------------------------------
CREATE TABLE mid_temp_hwy_jct_service_road_del
(
gid        integer not null primary key,
node_lid   character varying
);

------------------------------------------------------------------------
--
CREATE TABLE hwy_same_info
(
  gid            serial NOT NULL PRIMARY KEY,
  road_code      integer NOT NULL,
  road_seq       integer NOT NULL,
  updown_c       integer NOT NULL,
  inout_c        integer NOT NULL,
  node_id        bigint NOT NULL,
  p_node_id      bigint NOT NULL,
  p_road_seq     integer NOT NULL
);

----------------------------------------------------------------------------
CREATE TABLE mid_hwy_ic_no
(
  gid            serial not null primary key,
  ic_no          integer not null,
  node_id        bigint not null,
  road_code      integer not null,
  road_seq       integer not null,
  updown         integer not null,
  facilclass_c   integer not null,
  inout_c        integer not null,
  facil_name     character varying(4096),
  facility_id    integer
);

------------------------------------------------------------------------
--
CREATE TABLE mid_hwy_facility_id
(
  facility_id     serial not null primary key,
  road_code       integer not null,
  updown_c        integer not null default 1,
  road_seq        integer not null
);

------------------------------------------------------------------------
--
create table mid_hwy_road_no
(
  road_no        serial not null primary key,
  road_code      integer not null,
  road_type      integer not null
);

------------------------------------------------------------------------
--
CREATE TABLE hwy_node
(
  gid            serial not null primary key,
  road_code      integer not null,
  road_seq       integer not null,
  updown_c       integer not null,
  facilcls_c     integer not null,
  inout_c        integer not null,
  node_id        bigint not null,
  tollcls_c      integer,
  facil_name     character varying
); SELECT AddGeometryColumn('','hwy_node','the_geom','4326','POINT',2);

------------------------------------------------------------------------
--
CREATE TABLE mid_temp_hwy_ic_link_mapping
(
  gid               serial not null primary key,
  bwd_node_id       bigint,
  bwd_ic_no         integer,
  bwd_facility_id   integer,
  fwd_node_id       bigint,
  fwd_ic_no         integer,
  fwd_facility_id   integer,
  link_id           bigint not null,
  path_type         character varying(12),
  toll_flag         smallint  -- 1: toll, 0: No Toll
);

------------------------------------------------------------------------
--
CREATE TABLE mid_temp_hwy_facil_name
(
  gid          serial not null primary key,
  road_code    INTEGER NOT NULL,
  updown_c     INTEGER NOT NULL default 1,
  road_seq     INTEGER NOT NULL, 
  facil_name   CHARACTER VARYING(4096)
);

------------------------------------------------------------------------
--
CREATE TABLE mid_temp_hwy_exit_name
(
  link_id            bigint not null,
  ref_node_id        bigint,
  nonref_node_id     bigint,
  is_exit_name       character(1),
  is_junction_name   character(1),
  name               character varying(4096)
);

------------------------------------------------------------------------
--
CREATE TABLE mid_temp_hwy_sapa_name
(
  poi_id             bigint not null,
  name               character varying(4096)
);

------------------------------------------------------------------------
--
CREATE TABLE mid_temp_poi_link
(
  poi_id             bigint not null,
  link_id            bigint not null
);

------------------------------------------------------------------------
--
CREATE TABLE mid_temp_poi_closest_link
(
  poi_id        bigint not null primary key,
  link_id       bigint not null,
  dist          double precision not null,
  s_length      double precision not null,
  e_length      double precision not null
);

------------------------------------------------------------------------
--
CREATE TABLE mid_temp_sapa_store_info
(
  poi_id             bigint not null,   -- SAPA poi id
  child_poi_id       bigint not null,
  chain_id           character varying(13),
  cat_id             character varying(4) not null,
  subcat             bigint,
  chain_name         character varying
);

------------------------------------------------------------------------
--
CREATE TABLE mid_link_mapping
(
  link_id      bigint not null,
  org_link_id  bigint not null
);

------------------------------------------------------------------------
--
CREATE TABLE hwy_store
(
  gid               serial not null primary key,
  road_code         INTEGER NOT NULL,
  road_seq          INTEGER NOT NULL,
  updown_c          INTEGER NOT NULL,
  store_cat_id      character varying(4) DEFAULT '' NOT NULL,  -- '': No category
  sub_cat           character varying(8) DEFAULT '' NOT NULL,  -- '': No sub category
  store_chain_id    character varying(13) DEFAULT '' NOT NULL, -- '': No chain id
  chain_name        character varying(254) DEFAULT '',
  priority          double precision,
  service_kind      character varying
);

------------------------------------------------------------------------
-- chain name or store name
CREATE TABLE hwy_chain_name
(
  gid               serial not null primary key,
  u_code            bigint,
  cat_id            character varying(4) DEFAULT '' NOT NULL,   -- '': No category
  sub_cat           character varying(8) DEFAULT '' NOT NULL,   -- '': No sub category
  chain_id          character varying(13) DEFAULT '' NOT NULL,  -- '': No chain id
  chain_name        character varying(254),
  language_code     character(3)
);

------------------------------------------------------------------------
--
CREATE TABLE hwy_service
(
  gid               serial not null primary key,
  road_code         INTEGER NOT NULL,
  road_seq          INTEGER NOT NULL,
  updown_c          INTEGER NOT NULL,
  gas_station       INTEGER NOT NULL,
  information       INTEGER NOT NULL,
  rest_area         INTEGER NOT NULL,
  shopping_corner   INTEGER NOT NULL,
  postbox           INTEGER NOT NULL,
  atm               INTEGER NOT NULL,
  restaurant        INTEGER NOT NULL,
  toilet            INTEGER NOT NULL
);

------------------------------------------------------------------------
--
CREATE TABLE hwy_ic_service
(
  gid               serial not null primary key,
  road_code         INTEGER NOT NULL,
  road_seq          INTEGER NOT NULL,
  updown_c          INTEGER NOT NULL,
  node_id           bigint NOT NULL,
  gas_station       INTEGER NOT NULL,
  information       INTEGER NOT NULL,
  rest_area         INTEGER NOT NULL,
  shopping_corner   INTEGER NOT NULL,
  postbox           INTEGER NOT NULL,
  atm               INTEGER NOT NULL,
  restaurant        INTEGER NOT NULL,
  toilet            INTEGER NOT NULL
);

------------------------------------------------------------------------
--
CREATE TABLE hwy_ic_store
(
  gid               serial not null primary key,
  road_code         INTEGER NOT NULL,
  road_seq          INTEGER NOT NULL,
  updown_c          INTEGER NOT NULL,
  node_id           bigint NOT NULL,
  store_cat_id      character varying(4) DEFAULT '' NOT NULL,  -- '': No category
  sub_cat           character varying(8) DEFAULT '' NOT NULL,  -- '': No sub category
  store_chain_id    character varying(13) DEFAULT '' NOT NULL, -- '': No chain id
  chain_name        character varying(254) DEFAULT '',
  priority          double precision,
  service_kind      character varying
);
------------------------------------------------------------------------
CREATE TABLE hook_turn_tbl
(
	gid             serial primary key,
	id              bigint,
	nodeid          bigint not null,
	inlinkid        bigint not null,
	outlinkid 		bigint not null,
	passlid         varchar(254),
	passlink_cnt    smallint default 0
);
------------------------------------------------------------------------
-- All links of SAPA Pathes
CREATE TABLE mid_temp_sapa_path_link(
  link_id      bigint not null primary key,
  link_type    smallint not null,
  toll_flag    smallint DEFAULT 1   -- 1: Hwy, 0: No toll section
);

------------------------------------------------------------------------
-- All links of jct Pathes
CREATE TABLE mid_temp_jct_path_link(
  link_id      bigint not null primary key,
  link_type    smallint not null,
  toll_flag    smallint DEFAULT 1   -- 1: Hwy, 0: No toll section
);

------------------------------------------------------------------------
-- All links of IC UTurn Pathes
CREATE TABLE mid_temp_ic_turn_path_link(
  link_id      bigint not null primary key,
  link_type    smallint not null,
  toll_flag    smallint  DEFAULT 1  -- 1: Hwy, 0: No toll section
);

------------------------------------------------------------------------
-- All links of sevice road Pathes
CREATE TABLE mid_temp_sevice_road_path_link(
  link_id      bigint not null primary key,
  link_type    smallint not null
);

------------------------------------------------------------------------
--
CREATE TABLE mid_node_facil
(
  node_id           bigint not null primary key,
  facil_id          bigint not null
);

------------------------------------------------------------------------
CREATE TABLE mid_temp_hwy_sapa_info
(
 gid           serial not null primary key,
 road_code     integer not null,
 road_seq      integer not null,
 facilcls_c    integer not null,
 updown_c      integer not null,
 poi_id        bigint,
 sapa_name     CHARACTER VARYING(4096)
);

------------------------------------------------------------------------
CREATE TABLE hwy_facil_position
(
  road_code      INTEGER NOT NULL,
  road_seq       INTEGER NOT NULL,
  updown_c       INTEGER NOT NULL,
  facilcls_c     INTEGER,
  inout_c        INTEGER,
  tollcls_c      INTEGER,
  facil_name     character varying,
  link_id        bigint,
  node_id        bigint
);

--------------------------------------------------------------------------------------------------------
CREATE TABLE mid_temp_hwy_exit_poi_name
(
   node_id         bigint not null primary key,
   poi_id          bigint not null,
   name            character varying(1024)   --JSON
);

------------------------------------------------------------------------
CREATE TABLE stopsign_tbl
(
 id    		   serial not null primary key,
 link_id       bigint not null,
 node_id       bigint not null,
 pos_flag      smallint not null
);SELECT AddGeometryColumn('','stopsign_tbl','the_geom','4326','POINT',2);

create table stopsign_tbl_bak_merge 
as
(
	select * from stopsign_tbl
);

------------------------------------------------------------------------
CREATE TABLE temp_force_guide_patch_tbl
(
 gid serial not null primary key,
 guide_type bigint not null,
 str_geom varchar not null,
 str_z varchar not null
); 

CREATE TABLE temp_org_category_priority
(
  org_code  character varying(6),
  priority  integer
);      


create table temp_single_roundabout
( 
	link_id bigint,
	s_node bigint,
	e_node bigint
);select addgeometrycolumn('','temp_single_roundabout','the_geom','4326','LINESTRING',2);

create table temp_roundabout
as
(
	select distinct a.* from link_tbl a
	join link_tbl b
	on a.s_node=b.s_node or a.s_node=b.e_node or a.e_node=b.s_node or a.e_node=b.e_node
	where a.link_type=0 and ((b.road_type in (0,1) and b.link_type in (1,2,7)) or b.link_type in (3,5))
);

create table temp_roundabout_for_searchramp
(
	roundabout_id 	integer,
	link_id       	bigint,
	s_node        	bigint,
	e_node        	bigint
);select addgeometrycolumn('','temp_roundabout_for_searchramp','the_geom','4326','LINESTRING',2);

create table temp_roundabout_road_type
(
	roundabout_id   integer,
	new_road_type   smallint
);     

CREATE TABLE rdb_admin_order0_iso_country_mapping
(
  order0_id integer PRIMARY KEY,
  iso_country_code character varying(10)
);

create table temp_ramp_update_dispclass
(
    link_id bigint,
    display_class smallint
);

create table link_tbl_bak_dispclass
as
(
	select * from link_tbl
);

------------------------------------------------------------------------
create table hwy_service_category_mapping
(
   service        character varying(1024),
   genre          character varying(1024),
   service_name   character varying(1024),
   category_id    bigint,
   add_cat_id     integer
);

create table temp_jct_link_paths
(
snode bigint,
path  bigint[],
elink bigint,
direction integer
);


create table temp_jct_change_to_ic
(
links bigint[],
link_type  smallint
);

create table temp_sapa_link
(
gid           serial,
link_array    bigint[]
);

CREATE TABLE hwy_tollgate
(
  road_code      integer not null,
  road_seq       integer not null,
  updown_c       integer not null,
  facilcls_c     integer not null,
  inout_c        integer,
  node_id        bigint not null,
  link_id        bigint not null, -- out link of tollgate
  tollcls_c      integer,
  facil_name     character varying(1024),
  toll_name_flg  integer default(0),  -- 1: tollgate node name, 0: facililty name
  path_type      character varying(10)
);SELECT AddGeometryColumn('','hwy_tollgate','the_geom','4326','POINT',2)

------------------------------------------------------------------------
CREATE TABLE mid_temp_hwy_ic_tollgate
(
  road_code    integer not null,
  road_seq     integer not null,
  updown_c     integer not null,
  facilcls_c   integer not null,
  inout_c      integer,
  prev_link_id bigint,
  node_id      bigint not null,
  link_id      bigint not null, -- out link of toll
  tollcls_c    integer,
  facil_name   character varying(1024),
  path_type    character varying(10)
);SELECT AddGeometryColumn('','mid_temp_hwy_ic_tollgate','the_geom','4326','POINT',2)

------------------------------------------------------------------------
CREATE TABLE mid_temp_hwy_ic_path_expand_node
(
  road_code     integer not null,
  road_seq      integer not null,
  facilcls_c    integer not null,
  inout_c       integer not null,
  node_id       bigint not null,
  pass_node_id  bigint not null,
  path_type     character varying
);

------------------------------------------------------------------------
CREATE TABLE mid_temp_hwy_ic_path_expand_link
(
  road_code      integer NOT NULL,
  road_seq       integer NOT NULL,
  updown_c       integer NOT NULL,
  facilcls_c     integer NOT NULL,
  inout_c        integer NOT NULL,
  node_id        bigint NOT NULL,
  pass_link_id   bigint NOT NULL,
  path_type      character varying
);

------------------------------------------------------------------------
CREATE TABLE mid_temp_hwy_inout_join_node
(
  node_id   bigint not null primary key
);

------------------------------------------------------------------------
CREATE TABLE mid_temp_hwy_sapa_link
(
 gid           serial not null primary key,
 road_code     integer not null,
 road_seq      integer not null,
 updown_c      integer not null,
 link_ids      bigint[],
 link_lid      character varying,
 poi_id        bigint,
 distance      double precision
);

------------------------------------------------------------------------
CREATE TABLE temp_hwy_sapa_first_node
(
  road_code     integer,
  road_seq      integer,
  updown_c      integer,
  node_id       bigint
);

CREATE TABLE temp_regulation_patch_tbl
(
 gid serial not null primary key,
 str_geom varchar not null,
 str_z varchar not null,
 regulation_type integer,
 car_type integer,
 start_year integer,
 start_month integer,
 start_day integer,
 start_hour integer,
 start_minute integer,
 end_year integer,
 end_month integer,
 end_day integer,
 end_hour integer,
 end_minute integer,
 day_of_week integer,
 exclude_date smallint
);

CREATE TABLE mid_temp_patch_node_tbl
(
 gid integer not null,
 node_id_list bigint[],
 single_link_flag boolean default false
);

CREATE TABLE mid_temp_patch_link_tbl
(
 gid integer not null,
 first_node_id bigint,
 link_id_list bigint[]
);

CREATE TABLE mid_temp_force_guide_tbl
(
  gid serial primary key,
  nodeid bigint,
  inlinkid bigint,
  outlinkid bigint,
  passlid character varying(1024),
  passlink_cnt smallint,
  guide_type smallint
);

create table temp_suspect_jct_paths_link
(
link_path bigint[],
direction integer
);

create table temp_jct_paths_link
(
link_path bigint[]
); 

create table temp_ic_surround_by_jct_link
(
links bigint[]

);

create table temp_ic_surround_by_sapa_link
(
links bigint[]

);

create table temp_poi_category_name
(
  per_code  integer NOT NULL,
  name_id   integer,
  nametype  varchar,
  name_lang varchar,
  name      varchar,
  tr_lang   varchar,
  tr_name   varchar,
  ph_flag   varchar,
  phoneme_id  integer
);
