
-- temp_rdb_linklane_info
create table temp_rdb_linklane_info_p(
	lane_id serial not null primary key,
	old_link_id bigint not null,
	disobey_flag boolean NOT NULL,
	lane_dir smallint not null,
	lane_up_down_distingush boolean NOT NULL,
	ops_lane_number smallint not null,
	ops_width smallint not null,
	neg_lane_number smallint not null,
	neg_width smallint not null
);

create table temp_rdb_linklane_info(
	old_link_id bigint not null primary key,
	lane_id integer not null,
	disobey_flag boolean NOT NULL,
	lane_dir smallint not null,
	lane_up_down_distingush boolean NOT NULL,
	ops_lane_number smallint not null,
	ops_width smallint not null,
	neg_lane_number smallint not null,
	neg_width smallint not null
);

CREATE TABLE temp_guideinfo_crossname_dic_name
(
  gid integer not null,
  name_id integer,
  nm character varying(64),
  py character varying(64),
  en character varying(64),
  seq smallint
);

CREATE TABLE temp_guideinfo_signpost_dic_name
(
  gid integer not null,
  name_id integer,
  nm character varying(64),
  py character varying(64),
  en character varying(64),
  seq smallint
);

CREATE TABLE temp_guideinfo_towardname_dic_name
(
  gid integer not null,
  name_id integer,
  nm character varying(64),
  py character varying(64),
  en character varying(64),
  seq smallint
);

CREATE TABLE temp_kind_code
(
  kind_code integer,
  priority integer,
  name_type integer,
  kiwi_name character varying(50),
  emg_code integer,
  emg_name character varying(50),
  ni_code character varying(8),
  ni_name character varying(500),
  low_level integer,
  high_level integer,
  road_display_class integer
);

create table temp_name_sharp_road_name(
	road_name_id integer not null,
	seq_num integer not null,
	scale_type smallint not null
); SELECT AddGeometryColumn('','temp_name_sharp_road_name','the_geom','4326','LINESTRING',2);

create table temp_name_sharp_road_number(
	road_name_id integer not null,
	shield_id smallint not null,
	seq_num integer not null,
	scale_type smallint not null
); SELECT AddGeometryColumn('','temp_name_sharp_road_number','the_geom','4326','LINESTRING',2);

CREATE TABLE temp_line_name_full
(
  record_id serial NOT NULL,
  kind_code smallint NOT NULL, 
  name_type smallint NOT NULL, 
  name_prior smallint NOT NULL, 
  low_level smallint NOT NULL, 
  high_level smallint NOT NULL,
  name_id integer NOT NULL,
  name character varying(254),
  CONSTRAINT temp_line_name_fulle_pkey PRIMARY KEY (record_id)
);SELECT AddGeometryColumn('','temp_line_name_full','the_geom','4326','LINESTRING',2);

CREATE TABLE temp_line_name_cut
(
  record_id serial NOT NULL,
  kind_code smallint NOT NULL,
  name_type smallint NOT NULL,
  name_prior smallint NOT NULL,
  low_level smallint NOT NULL,
  high_level smallint NOT NULL,
  name_id integer NOT NULL
);SELECT AddGeometryColumn('','temp_line_name_cut','the_geom','4326','LINESTRING',2);

CREATE TABLE temp_line_name_cut_simplify
(
  record_id serial NOT NULL,
  kind_code smallint NOT NULL,
  name_type smallint NOT NULL,
  name_prior smallint NOT NULL,
  low_level smallint NOT NULL,
  high_level smallint NOT NULL,
  name_id integer NOT NULL
); SELECT AddGeometryColumn('','temp_line_name_cut_simplify','the_geom','4326','LINESTRING',2);

CREATE TABLE temp_name_dictionary
(
  new_name_id integer primary key NOT NULL,
  old_name_id integer not null
);

CREATE TABLE temp_point_list
(
  gid serial NOT NULL,
  image_id character varying(128),
  data bytea,
  CONSTRAINT temp_point_list_pkey PRIMARY KEY (gid)
);

CREATE TABLE temp_link_name_and_number
(
  gid serial NOT NULL,
  link_id bigint not null,                 -- tile_link_id
  name_id integer not null,                -- RDB name id
  scale_type smallint not null,            -- scale
  shield_id smallint,                      -- shield
  seq_nm smallint not null,                   
  name_type smallint not null,             -- offical name, alter name, road number.
  "name"    character varying(512),
  country_code character varying not null,
  CONSTRAINT temp_link_name_and_number_pkey PRIMARY KEY (gid)
);

create table temp_tile_level_6
(
	tile_x	integer,
	tile_y	integer
);SELECT AddGeometryColumn('','temp_tile_level_6','the_geom','4326','POLYGON',2);

create table temp_tile_level_8
(
	tile_x	integer,
	tile_y	integer
);SELECT AddGeometryColumn('','temp_tile_level_8','the_geom','4326','POLYGON',2);

create table temp_tile_level_10
(
	tile_x	integer,
	tile_y	integer
);SELECT AddGeometryColumn('','temp_tile_level_10','the_geom','4326','POLYGON',2);

create table temp_tile_level_10_admin_zone_multi
(
	tile_x	integer,
	tile_y	integer,
	ad_code	integer
);

create table temp_tile_level_10_admin_zone
(
	tile_x	integer,
	tile_y	integer,
	ad_code	integer
);

create table temp_tile_level_12
(
	tile_x	integer,
	tile_y	integer,
	ad_code	integer
);SELECT AddGeometryColumn('','temp_tile_level_12','the_geom','4326','POLYGON',2);

create table temp_tile_level_12_admin_zone
(
	gid		serial primary key,
	tile_x	integer,
	tile_y	integer,
	ad_code	integer,
	b_contain boolean
);

create table temp_tile_level_12_admin_zone_multi
(
	tile_x	integer,
	tile_y	integer,
	ad_code	integer,
	b_contain boolean
);

create table temp_tile_level_14
(
	gid		serial primary key,
	tile_id	integer,
	tile_x	integer,
	tile_y	integer
);SELECT AddGeometryColumn('','temp_tile_level_14','the_geom','4326','POLYGON',2);

create table temp_tile_level_14_intersect
(
	tile_id	integer,
	tile_x	integer,
	tile_y	integer,
	ad_code	integer
);SELECT AddGeometryColumn('','temp_tile_level_14_intersect','the_geom','4326','POLYGON',2);

create table temp_tile_level_14_admin_zone
(
	tile_id	integer,
	tile_x	integer,
	tile_y	integer,
	ad_code	integer
);SELECT AddGeometryColumn('','temp_tile_level_14_admin_zone','the_geom','4326','POLYGON',2);

create table temp_tile_level_14_admin_zone_order8_multi
(
	tile_id	integer,
	tile_x	integer,
	tile_y	integer,
	ad_code	integer,
	the_geom geometry
);

create table temp_tile_level_14_admin_zone_order8
(
	tile_id	integer,
	tile_x	integer,
	tile_y	integer,
	ad_code	integer,
	the_geom geometry
);

CREATE TABLE temp_tile_admin_zone
(
  gid		   serial not null primary key,
  tile_id      integer not null,
  ad_code      integer NOT NULL,
  outer_polygon_gid	   integer not null default (0),
  prior		   integer not null default (0)
); SELECT AddGeometryColumn('','temp_tile_admin_zone','the_geom','4326','POLYGON',2);

CREATE TABLE temp_tile_admin_zone_hole
(
  tmp_polygon_gid  serial not null primary key,
  outer_polygon_gid  integer,
  tile_id 	   	   integer,
  ad_code          integer,
  prior            integer
); SELECT AddGeometryColumn('','temp_tile_admin_zone_hole','the_geom','4326','POLYGON',2);

CREATE TABLE temp_tile_admin_zone_hole_tmp2old
(
  tmp_polygon_gid   integer,
  old_polygon_gid   integer
);

CREATE TABLE temp_tile_admin_zone_simple_edge
(
  tile_id integer,
  a_polygon_gid integer,
  b_polygon_gid integer,
  i_geom geometry
);

CREATE TABLE temp_tile_admin_zone_simple
(
  tile_id integer,
  ad_code integer,
  polygon_gid integer,
  the_geom geometry,
  i_geom geometry,
  i_simple_geom geometry,
  the_simple_geom geometry
);

---------------------------------------------------------------------------------
-- a different way to simplify admin zone
---------------------------------------------------------------------------------
CREATE TABLE temp_simplified_admin_zone
(
  gid		   			serial not null primary key,
  ad_code      			integer NOT NULL,
  order1_id				integer,
  the_geom				geometry
);

CREATE TABLE temp_simplified_province
(
  gid		   			serial not null primary key,
  ad_code      			integer NOT NULL,
  the_geom				geometry
);

CREATE TABLE temp_simplifying_admin_zone
(
  gid		   			serial not null primary key,
  ad_code      			integer NOT NULL,
  order1_id				integer,
  outer_polygon_gid	   	integer not null default (0),
  prior		   			integer not null default (0),
  the_geom				geometry
);

CREATE TABLE temp_simplifying_admin_zone_hole
(
  tmp_polygon_gid  		serial not null primary key,
  outer_polygon_gid  	integer,
  ad_code          		integer,
  prior            		integer,
  the_geom				geometry
);

CREATE TABLE temp_simplifying_admin_zone_hole_tmp2old
(
  tmp_polygon_gid   integer,
  old_polygon_gid   integer
);

CREATE TABLE temp_simplifying_admin_zone_edge
(
  gid serial not null primary key,
  a_polygon_gid integer,
  b_polygon_gid integer,
  i_geom geometry
);

CREATE TABLE temp_simplifying_admin_zone_edge_simple
(
  gid serial not null primary key,
  a_polygon_gid integer,
  b_polygon_gid integer,
  simplify_para float default 0.00009,
  simplify_scale float default 1.0,
  i_geom geometry,
  i_simple_geom geometry
);

CREATE TABLE temp_simplifying_admin_zone_simple
(
  ad_code integer,
  polygon_gid integer,
  the_geom geometry,
  i_geom geometry,
  i_simple_geom geometry,
  the_simple_geom geometry
);


create table temp_region_promote_link_fc
(
	link_id	bigint primary key,
	new_function_code smallint
);

create table temp_region_delete_ic
(
	link_id	bigint primary key
);

create table temp_region_orglink_level6_bak_delete_ic
as
(
	select * from temp_region_orglink_level6
);

create table temp_region_walked_link
(
	link_id	bigint primary key,
	island_id bigint
);

create table temp_region_links_sa_ramp
(
	link_id	bigint
);

create table temp_region_links_sa_path
(
	fnode		bigint,
	tnode		bigint,
	cost		float,
	link_array	bigint[]
);

create table temp_region_orglink_level6_bak_delete_link
as
(
	select * from temp_region_orglink_level6
);

create table temp_region_node_upgrade
(
	node_id bigint primary key
);

create table temp_region_merge_node_keep
as
(
	select distinct node_id
	from
	(
		select node_id
		from rdb_node
		where false --light_flag != 0 or toll_flag != 0
		
		-- ferry branch point, maybe dock
		union
		select a.node_id
		from
		(
			select distinct node_id
			from
			(
				select start_node_id as node_id from rdb_link where road_type = 10
				union
				select end_node_id as node_id from rdb_link where road_type = 10
			)as n
		)as a
		inner join rdb_node as b
		on a.node_id = b.node_id
		where b.link_num > 2
		
		-- ERP point
		union
		select distinct start_node_id from rdb_link a
		inner join rdb_link_add_info2 b
		on a.link_id = b.link_id where ((b.link_add_info2 >> 1) & 3) in (1,2)
		union
		select distinct end_node_id from rdb_link a
		inner join rdb_link_add_info2 b
		on a.link_id = b.link_id where ((b.link_add_info2 >> 1) & 3) in (1,3)
		
		-- upgrade node
		union
		select node_id
		from temp_region_node_upgrade
	)as t
);

create table temp_region_merge_link_keep
as
(
	select  distinct link_id
	from
	(
		select	unnest(link_array) as link_id
		from
		(
			select	cast(regexp_split_to_array(key_string, E',') as bigint[]) as link_array
			from
			(
				select	key_string
				from rdb_link_regulation as a
				left join
				(
					-- drop regulation: one of links is not in region
					select	distinct record_no
					from
					(		
						select	record_no, unnest(link_array) as link_id
						from
						(
							select	record_no, cast(regexp_split_to_array(key_string, E',') as bigint[]) as link_array
							from rdb_link_regulation
						)as a
					)as a
					left join temp_region_links as b
					on a.link_id = b.link_id
					where b.link_id is null
					
					union
					
					-- drop regulation: car_type is not car or small car
					select distinct record_no
					from rdb_link_regulation as a
					left join
					(
						select distinct regulation_id as keep_regulation_id
						from rdb_link_cond_regulation
						where (((car_type >> 12) & 1) > 0) or (((car_type >> 13) & 1) > 0)
					)as b
					on a.regulation_id = b.keep_regulation_id
					where a.regulation_id != 0 and b.keep_regulation_id is null
					
					union
					
					-- drop regulation: one link u-turn
					select record_no
					from rdb_link_regulation
					where link_num = 2 and last_link_id = first_link_id
				)as b
				on a.record_no = b.record_no
				where b.record_no is null
			)as t1
		)as t2
	)as t3
);

create table temp_region_merge_node_suspect
as
(
	select b.node_id
	from
	(
		select node_id
		from
		(
			select node_id, array_agg(link_id) as link_array
			from
			(
				select a.node_id, b.link_id
				from rdb_node as a
				inner join temp_region_links as b
				on a.node_id in (b.start_node_id, b.end_node_id)
			)as a
			group by node_id
		)as a
		where array_upper(link_array, 1) = 2
	)as b
	left join temp_region_merge_node_keep as c
	on b.node_id = c.node_id
	where c.node_id is null
);

create table temp_region_merge_link_suspect
as
(
	select 	distinct
			b.link_id, b.region_tile_id, b.max_level, b.abs_link_id, b.start_node_id, b.end_node_id, b.one_way, 
			b.toll, b.road_name, b.road_number, b.road_type, (b.extend_flag >> 1) & 1 as pdm_flag
			, b.bypass_flag, b.highcost_flag
			, (case when h.path_extra_info is null then 0::smallint else h.path_extra_info end) as path_extra_info
--			, b.link_type, h.etc_lane_flag
--			, i.pos_cond_speed as speed_limit_s2e, i.neg_cond_speed as speed_limit_e2s
--			, j.disobey_flag, j.lane_dir, j.lane_up_down_distingush
--			, j.ops_lane_number, j.ops_width, j.neg_lane_number, j.neg_width
	from temp_region_merge_node_suspect as a
	left join temp_region_links as b
	on a.node_id in (b.start_node_id, b.end_node_id)
	left join temp_region_merge_link_keep as c
	on b.link_id = c.link_id
	left join (
		select m.link_id
			,case when n.link_id is null then (((path_extra_info >> 4) & 1) << 6) | (path_extra_info & 7)
				else (((link_add_info2 >> 6) & 1) << 7) | (((path_extra_info >> 4) & 1) << 6) | ((link_add_info2 & 7) << 3) | (path_extra_info & 7)
			 end as path_extra_info
		from rdb_link_add_info as m
		left join rdb_link_add_info2 as n
		on m.link_id = n.link_id
	) h
	on b.link_id = h.link_id
--	left join rdb_cond_speed as i
--	on b.link_id = i.link_id
--	left join rdb_linklane_info as j
--	on b.lane_id = j.lane_id
	where c.link_id is null
);

create table temp_region_merge_link_walked
(
	link_id bigint primary key
);

create table temp_region_merge_linkrow
(
	link_id			bigint,
	start_node_id	bigint,
	end_node_id		bigint,
	one_way			smallint,
	link_num		integer,
	link_array		bigint[],
	linkdir_array	boolean[]
);

create table temp_region_merge_link_mapping
as
(
    select	proxy_link_id, 
			start_node_id, 
			end_node_id, 
			one_way, 
			link_num,
			link_array,
			linkdir_array,
			merge_index,
			link_array[merge_index] as link_id,
			linkdir_array[merge_index] as link_dir
	from
	(
		select	link_id as proxy_link_id, 
				start_node_id, 
				end_node_id, 
				one_way, 
				link_num,
				generate_series(1, link_num) as merge_index,
				link_array,
				linkdir_array
		from temp_region_merge_linkrow
	)as a
);

create table temp_region_merge_joinlink
as
(
	select	proxy_link_id, start_node_id, end_node_id, one_way, link_array, linkdir_array, fazm_path, tazm_path, 
			struct_code, shortcut_code, etc_lane_flag, disobey_flag, ops_lane_number, neg_lane_number,
			(case when ops_width is null then 4 else ops_width end) as ops_width, 
			(case when neg_width is null then 4 else neg_width end) as neg_width, 
			pos_cond_speed, neg_cond_speed, link_type, link_length, abs_linkids, the_geom
	from
	(
	    select	proxy_link_id, start_node_id, end_node_id, one_way, link_array, linkdir_array, fazm_path, tazm_path, 
	    		(case when 1 = ANY(struct_code_array) then 1 else 0 end) as struct_code,
	    		(
	    		 case 
	    		 when 3 = ANY(shortcut_code_array) then 3 
	    		 when 1 = ANY(shortcut_code_array) and 2 = ANY(shortcut_code_array) then 3 
	    		 when 1 = ANY(shortcut_code_array) then 1
	    		 when 2 = ANY(shortcut_code_array) then 2
	    		 else 0
	    		 end
	    		) as shortcut_code,
	    		(case when 1 = ANY(etc_lane_flag_array) then 1 when 2 = ANY(etc_lane_flag_array) then 2 else 0 end) as etc_lane_flag,
	    		(case when True in (disobey_flag_array[1], disobey_flag_array[link_num]) then True else False end) as disobey_flag,
	    		rdb_get_average_attr(ops_lane_number_array) as ops_lane_number,
	    		rdb_get_average_attr(ops_width_array) as ops_width,
	    		rdb_get_average_attr(neg_lane_number_array) as neg_lane_number,
	    		rdb_get_average_attr(neg_width_array) as neg_width,
	    		rdb_get_average_attr(pos_cond_speed_array) as pos_cond_speed,
	    		rdb_get_average_attr(neg_cond_speed_array) as neg_cond_speed,
	    		(
	    		 case 
	    		 when link_type_array[1] = ALL(link_type_array) then link_type_array[1]
	    		 when 7 = ANY(link_type_array) then 7 
	    		 when 2 = ANY(link_type_array) then 2 
	    		 else 1 
	    		 end
	    		) as link_type,
	    		link_length, 
	    		abs_linkids,
	    		st_linemerge(st_collect(geom_array)) as the_geom
	    from
	    (
	        select	proxy_link_id, start_node_id, end_node_id, one_way, link_array, linkdir_array, fazm_path, tazm_path, link_num, 
	        		array_agg(struct_code) as struct_code_array,
	        		array_agg(shortcut_code) as shortcut_code_array,
	        		array_agg(etc_lane_flag) as etc_lane_flag_array,
	        		array_agg(disobey_flag) as disobey_flag_array,
	        		array_agg(ops_lane_number) as ops_lane_number_array,
	        		array_agg(ops_width) as ops_width_array,
	        		array_agg(neg_lane_number) as neg_lane_number_array,
	        		array_agg(neg_width) as neg_width_array,
	        		array_agg(pos_cond_speed) as pos_cond_speed_array,
	        		array_agg(neg_cond_speed) as neg_cond_speed_array,
	        		array_agg(link_type) as link_type_array,
	        		sum(link_length) as link_length, 
	        		array_agg(abs_link_id) as abs_linkids,
	        		array_agg(the_geom) as geom_array
	        from
	        (
	        	select	proxy_link_id, start_node_id, end_node_id, one_way, link_array, linkdir_array, fazm_path, tazm_path, link_num, merge_index, 
	        			struct_code, shortcut_code, etc_lane_flag, disobey_flag, ops_lane_number, neg_lane_number, 
	        			(case when ops_width = 4 then null else ops_width end) as ops_width, 
	        			(case when neg_width = 4 then null else neg_width end) as neg_width, 
	        			(case when pos_cond_speed = 0 then null else pos_cond_speed end) as pos_cond_speed, 
	        			(case when neg_cond_speed = 0 then null else neg_cond_speed end) as neg_cond_speed, 
	        			link_type, link_length, abs_link_id, the_geom
	        	from
	        	(
		            select	a.proxy_link_id, a.start_node_id, a.end_node_id, a.one_way, a.link_array, a.linkdir_array, a.link_num, a.merge_index, 
		            		(case when b.start_node_id = a.start_node_id then b.fazm_path else b.tazm_path end) as fazm_path,
		            		(case when c.end_node_id = a.end_node_id then c.tazm_path else c.fazm_path end) as tazm_path,
		            		(case when d.struct_code = 1 then 1 else 0 end) as struct_code,
		            		(case when a.link_dir or d.shortcut_code in (0,3) then d.shortcut_code else ((d.shortcut_code % 2) + 1) end) as shortcut_code,
		            		d.etc_lane_flag,
		            		e.disobey_flag, 
		            		(case when a.link_dir then e.ops_lane_number else e.neg_lane_number end) as ops_lane_number,
		            		(case when a.link_dir then e.ops_width else e.neg_width end) as ops_width,
		            		(case when a.link_dir then e.neg_lane_number else e.ops_lane_number end) as neg_lane_number,
		            		(case when a.link_dir then e.neg_width else e.ops_width end) as neg_width,
		            		(case when a.link_dir then f.pos_cond_speed else f.neg_cond_speed end) as pos_cond_speed,
		            		(case when a.link_dir then f.neg_cond_speed else f.pos_cond_speed end) as neg_cond_speed,
		            		m.link_type,
		            		m.link_length, 
		            		m.abs_link_id,
		            		(case when a.link_dir then m.the_geom else st_reverse(m.the_geom) end) as the_geom
		            from temp_region_merge_link_mapping as a
		            left join temp_region_links as m
		            on a.link_id = m.link_id
		            left join temp_region_links as b
		            on a.link_array[1] = b.link_id
		            left join temp_region_links as c
		            on a.link_array[a.link_num] = c.link_id
		            left join rdb_link_add_info as d
					on a.link_id = d.link_id
					left join rdb_linklane_info as e
					on m.lane_id = e.lane_id
					left join rdb_cond_speed as f
					on m.link_id = f.link_id
				)as a
				order by proxy_link_id, start_node_id, end_node_id, one_way, link_array, linkdir_array, fazm_path, tazm_path, link_num, merge_index
	        )as b
	        group by proxy_link_id, start_node_id, end_node_id, one_way, link_array, linkdir_array, fazm_path, tazm_path, link_num
	    )as c
    )as t
);

create table temp_region_merge_links_oldid
as
(
	select	link_id, region_tile_id, region_level, start_node_id, end_node_id, road_type, pdm_flag, path_extra_info, one_way, function_code, link_length, link_type,
			road_name_id, road_name, toll, bypass_flag, highcost_flag, regulation_flag, fazm_path, tazm_path, struct_code, shortcut_code, etc_lane_flag, disobey_flag, 
			ops_lane_number, ops_width, neg_lane_number, neg_width, pos_cond_speed, neg_cond_speed, the_geom, 
			low_level_linkids, low_level_linkdirs, abs_linkids,
			array_to_string(low_level_linkids, ',') as feature_string, 
			md5(array_to_string(low_level_linkids, ',')) as feature_key
	from
	(
		select a.link_id, region_tile_id, region_level, a.start_node_id, a.end_node_id, 
		       road_type, pdm_flag, 
		       (case when d.path_extra_info is null then 0 else d.path_extra_info end) as path_extra_info, 
		       a.one_way, function_code, link_length, a.link_type, 
		       road_name_id, road_name, toll, bypass_flag, highcost_flag, regulation_flag, a.fazm_path, a.tazm_path, 
		       (case when d.struct_code is null then 0 else d.struct_code end) as struct_code, 
		       (case when d.shortcut_code is null then 0 else d.shortcut_code end) as shortcut_code, 
		       (case when d.etc_lane_flag is null then 0 else d.etc_lane_flag end) as etc_lane_flag, 
		       e.disobey_flag, e.ops_lane_number, e.ops_width, e.neg_lane_number, e.neg_width, 
		       f.pos_cond_speed, f.neg_cond_speed, the_geom, 
		       ARRAY[a.link_id] as low_level_linkids, 
		       ARRAY[True] as low_level_linkdirs,
		       ARRAY[a.abs_link_id] as abs_linkids
		from temp_region_links as a
		left join temp_region_merge_link_mapping as b
		on a.link_id = b.link_id
		left join (
			select m.link_id, m.struct_code, m.shortcut_code, m.etc_lane_flag
				,case when n.link_id is null then (((path_extra_info >> 4) & 1) << 6) | (path_extra_info & 7)
					else (((link_add_info2 >> 6) & 1) << 7) | (((path_extra_info >> 4) & 1) << 6) | ((link_add_info2 & 7) << 3) | (path_extra_info & 7)
				 end as path_extra_info
			from rdb_link_add_info as m
			left join rdb_link_add_info2 as n
			on m.link_id = n.link_id
		) as d
		on a.link_id = d.link_id
		left join rdb_linklane_info as e
		on a.lane_id = e.lane_id
		left join rdb_cond_speed as f
		on a.link_id = f.link_id
		where b.link_id is null
		
		union
		
		select a.link_id, region_tile_id, region_level, b.start_node_id, b.end_node_id, 
		       road_type, pdm_flag, 
		       (case when c.path_extra_info is null then 0 else c.path_extra_info end) as path_extra_info, 
		       b.one_way, function_code, b.link_length, b.link_type, 
		       road_name_id, road_name, toll, bypass_flag, highcost_flag, regulation_flag, b.fazm_path, b.tazm_path, 
		       b.struct_code, b.shortcut_code, b.etc_lane_flag, 
		       b.disobey_flag, b.ops_lane_number, b.ops_width, b.neg_lane_number, b.neg_width, 
		       b.pos_cond_speed, b.neg_cond_speed, b.the_geom, 
		       b.link_array as low_level_linkids, 
		       b.linkdir_array as low_level_linkdirs,
		       b.abs_linkids
		from temp_region_links as a
		left join temp_region_merge_joinlink as b
		on a.link_id = b.proxy_link_id
		left join (
			select m.link_id, m.struct_code, m.shortcut_code, m.etc_lane_flag
				,case when n.link_id is null then (((path_extra_info >> 4) & 1) << 6) | (path_extra_info & 7)
					else (((link_add_info2 >> 6) & 1) << 7) | (((path_extra_info >> 4) & 1) << 6) | ((link_add_info2 & 7) << 3) | (path_extra_info & 7)
				 end as path_extra_info
			from rdb_link_add_info as m
			left join rdb_link_add_info2 as n
			on m.link_id = n.link_id
		)as c
		on a.link_id = c.link_id
		where b.proxy_link_id is not null
	)as t
);

create table temp_region_merge_nodes_oldid
as
(
	select 	node_id, region_tile_id, count(link_id) as link_num, array_agg(link_id) as branches, extend_flag, the_geom,
			node_id::varchar as feature_string, md5(node_id::varchar) as feature_key
	from
	(
		select node_id, region_tile_id, a.extend_flag, the_geom, angle, link_id
		from
		(
			select a.node_id, a.region_tile_id, a.extend_flag, a.the_geom, b.link_id, b.fazm_path as angle
			from temp_region_nodes as a
			left join temp_region_merge_links_oldid as b
			on a.node_id = b.start_node_id
			where b.link_id is not null
			
			union
			
			select a.node_id, a.region_tile_id, a.extend_flag, a.the_geom, b.link_id, b.tazm_path as angle
			from temp_region_nodes as a
			left join temp_region_merge_links_oldid as b
			on a.node_id = b.end_node_id
			where b.link_id is not null
		)as a
		order by node_id, region_tile_id, extend_flag, the_geom, angle desc
	)as a
	group by node_id, region_tile_id, extend_flag, the_geom
);

create table temp_region_merge_linkid_mapping
(
	region_tile_id integer,
	seq_num bigint,
	link_id bigint,
	old_link_id bigint
);
--create table temp_region_merge_linkid_mapping
--as
--(
--	select	region_tile_id, 
--			seq_num,
--			((region_tile_id::bigint << 32) + seq_num) as link_id,
--			link_array[seq_num] as old_link_id
--	from
--	(
--		select region_tile_id, link_array, generate_series(1,link_num) as seq_num
--		from
--		(
--			select region_tile_id, array_agg(link_id) as link_array, count(link_id) as link_num
--			from
--			(
--				select region_tile_id, link_id
--				from temp_region_merge_links_oldid
--				order by region_tile_id, link_id
--			)as a
--			group by region_tile_id
--		)as b
--	)as c
--);

create table temp_region_merge_abslinkid_mapping
(
	old_link_id bigint,
	abs_link_id integer,
	abs_link_diff smallint,
	bNewDispatch boolean
);

create table temp_region_merge_nodeid_mapping
(
	region_tile_id integer,
	seq_num bigint,
	node_id bigint,
	old_node_id bigint
);
--create table temp_region_merge_nodeid_mapping
--as
--(
--	select	region_tile_id, 
--			seq_num,
--			((region_tile_id::bigint << 32) + seq_num) as node_id,
--			node_array[seq_num] as old_node_id
--	from
--	(
--		select region_tile_id, node_array, generate_series(1,node_num) as seq_num
--		from
--		(
--			select region_tile_id, array_agg(node_id) as node_array, count(node_id) as node_num
--			from
--			(
--				select region_tile_id, node_id
--				from temp_region_merge_nodes_oldid
--				order by region_tile_id, node_id
--			)as a
--			group by region_tile_id
--		)as b
--	)as c
--);

create table temp_region_merge_links
as
(
	select	b.link_id, a.region_tile_id, e.abs_link_id, e.abs_link_diff,
			c.node_id as start_node_id, d.node_id as end_node_id, 
			road_type, pdm_flag, path_extra_info, one_way, function_code, link_length, link_type, road_name_id, road_name, toll, 
       		bypass_flag, highcost_flag, regulation_flag, fazm_path, tazm_path, struct_code, shortcut_code, etc_lane_flag, disobey_flag, 
       		ops_lane_number, ops_width, neg_lane_number, neg_width, pos_cond_speed, 
       		neg_cond_speed, the_geom, low_level_linkids, low_level_linkdirs
	from temp_region_merge_links_oldid as a
	left join temp_region_merge_linkid_mapping as b
	on a.link_id = b.old_link_id
	left join temp_region_merge_nodeid_mapping as c
	on a.start_node_id = c.old_node_id
	left join temp_region_merge_nodeid_mapping as d
	on a.end_node_id = d.old_node_id
	left join temp_region_merge_abslinkid_mapping as e
	on a.link_id = e.old_link_id
);

create table temp_region_merge_nodes
as
(
	select	node_id, region_tile_id, link_num, array_agg(link_id) as branches, extend_flag, the_geom, low_level_node_id
	from
	(
		select	node_id, region_tile_id, low_level_node_id, extend_flag, the_geom, link_num, link_index, link_id
		from
		(
			select	b.node_id, a.region_tile_id, a.node_id as low_level_node_id, 
					a.extend_flag, a.the_geom, a.link_num, a.link_index, c.link_id
		  	from 
		  	(
		  		select	node_id, region_tile_id, extend_flag, the_geom, 
		  				link_num, link_index, branches[link_index] as link_id
		  		from
		  		(
			  		select 	node_id, region_tile_id, link_num, branches, extend_flag, the_geom, 
			  				generate_series(1,link_num) as link_index
			  		from
			  		temp_region_merge_nodes_oldid
		  		)as t
		  	) as a
		  	left join temp_region_merge_nodeid_mapping as b
		  	on a.node_id = b.old_node_id
		  	left join temp_region_merge_linkid_mapping as c
		  	on a.link_id = c.old_link_id
	  	)as t1
	  	order by node_id, region_tile_id, low_level_node_id, extend_flag, the_geom, link_num, link_index
  	)as t2
  	group by node_id, region_tile_id, low_level_node_id, extend_flag, the_geom, link_num
);

create table temp_region_merge_regulation
as
(
	select	record_no, regulation_id, regulation_type, is_seasonal, first_link_id, first_link_id_t, 
			last_link_id, last_link_dir, last_link_id_t, link_num, array_to_string(link_array, ',') as key_string
	from
	(
		select	record_no, regulation_id, regulation_type, is_seasonal, first_link_id, first_link_id_t, 
				last_link_id, last_link_dir, last_link_id_t, link_num, array_agg(link_id) as link_array
		from
		(
			select	record_no, regulation_id, regulation_type, is_seasonal, 
					first_link_id, first_link_id_t, last_link_id, last_link_dir, last_link_id_t, 
					link_num, link_index, link_id
			from
			(
				select	record_no, regulation_id, regulation_type, is_seasonal, 
						c.link_id as first_link_id, c.region_tile_id as first_link_id_t, 
						d.link_id as last_link_id, last_link_dir, d.region_tile_id as last_link_id_t, 
						link_num, link_index, (case when b.link_id is null then 0 else b.link_id end) as link_id
				from
				(
					select	record_no, regulation_id, regulation_type, is_seasonal, 
							first_link_id, first_link_id_t, last_link_id, last_link_dir, last_link_id_t, 
							link_num, link_index, link_array[link_index] as link_id
					from
					(
						select 	record_no, regulation_id, regulation_type, is_seasonal, link_num, 
								first_link_id, first_link_id_t, last_link_id, last_link_dir, last_link_id_t, 
					       		cast(regexp_split_to_array(key_string, E'\\,+') as bigint[]) as link_array,
					       		generate_series(1,link_num) as link_index
						from 
						(
							select * from rdb_link_regulation 
							where not (link_num = 2 and first_link_id = last_link_id)
						)as a
						left join
						(
							select distinct regulation_id as keep_regulation_id
							from rdb_link_cond_regulation
							where (((car_type >> 12) & 1) > 0) or (((car_type >> 13) & 1) > 0)
						)as b
						on a.regulation_id = b.keep_regulation_id
						where a.regulation_id = 0 or b.keep_regulation_id is not null
					)as t
				)as a
				left join temp_region_merge_linkid_mapping as b
				on a.link_id = b.old_link_id
				left join temp_region_merge_linkid_mapping as c
				on a.first_link_id = c.old_link_id
				left join temp_region_merge_linkid_mapping as d
				on a.last_link_id = d.old_link_id
			)as a
			order by record_no, regulation_id, regulation_type, is_seasonal, first_link_id, first_link_id_t, 
					 last_link_id, last_link_dir, last_link_id_t, link_num, link_index
		)as b
		group by record_no, regulation_id, regulation_type, is_seasonal, first_link_id, first_link_id_t, 
				 last_link_id, last_link_dir, last_link_id_t, link_num
	)as c
	where not (0 = ANY(link_array))
);

create table temp_merge_link_lane_info_unnest
as
(
	select lane_id, unnest(relavent_links) as link_id
	from temp_merge_link_lane_info
);

create table temp_link_tbl 
as
(
	SELECT    a.link_id
			, a.s_node
			, a.e_node
			, a.tile_id
			, rdb_merge_link_common_attri((extend_flag & 1)::smallint,toll::smallint,function_class::smallint,road_type::smallint,display_class::smallint) 
			as common_main_link_attri, a.display_class
	from link_tbl as a
	left join rdb_tile_link as b
	on a.link_id = b.old_link_id
	where b.tile_link_id is null
	order by a.tile_id, common_main_link_attri,display_class
);

CREATE TABLE temp_link_tbl_walked
 (
   link_id bigint
 );
 
CREATE TABLE temp_link_tbl_newid
(
  gid serial NOT NULL,
  link_id bigint,
  s_node bigint,
  e_node bigint,
  tile_id bigint,
  common_main_link_attri smallint,
  display_class smallint
);

CREATE TABLE temp_link_tbl_newid_park
(
  gid serial NOT NULL,
  link_id bigint,
  s_node bigint,
  e_node bigint,
  tile_id bigint,
  common_main_link_attri smallint
);

create table temp_region_layerx_link_with_regulation_status
as
(
	select link_id, rdb_split_tileid(link_id) as link_id_32, rdb_split_to_tileid(link_id) as link_id_32_t, 
	case when array_upper(regulation_exist_state_array,1) = 2 then 3 else regulation_exist_state_array[1] end as regulation_exist_state
	from 
	(
		select link_id, array_agg(regulation_exist_state) as regulation_exist_state_array from
		(
			select link_id, regulation_exist_state from 
			(
				select first_link_id as link_id, 1 as regulation_exist_state from temp_region_merge_regulation
				union 
				select last_link_id as link_id, 2 as regulation_exist_state from temp_region_merge_regulation
			) as a
			order by link_id, regulation_exist_state
		) as b
		group by link_id
	) as c
);

create table temp_merge_link_lane_info
(
  lane_id serial primary key NOT NULL,
  disobey_flag boolean NOT NULL,
  lane_dir smallint NOT NULL,
  lane_up_down_distingush boolean NOT NULL,
  ops_lane_number smallint NOT NULL,
  ops_width smallint NOT NULL,
  neg_lane_number smallint NOT NULL,
  neg_width smallint NOT NULL,
  relavent_links bigint[]
);

create table temp_region_node_with_branch_link_info
as
(
	select e.*, rdb_get_specify_link_idx(e.cmp_link, e.branches) as find_idx from
	(
		select node_id, low_level_node_id, seq, branch_link , case when node_id = start_node_id then low_level_linkids[1] 
		when node_id = end_node_id then low_level_linkids[low_links_cnt] else null end as cmp_link, branches from
		(
			select a.node_id, a.low_level_node_id , a.seq, a.branch_link, b.start_node_id, b.end_node_id, b.low_level_linkids, 
			array_upper(b.low_level_linkids,1) as low_links_cnt, c.branches from
			(
				select node_id, low_level_node_id, seq, branches[seq] as branch_link, branches from
				(
				select node_id, low_level_node_id, generate_series(1, array_upper(branches,1)) as seq, branches from temp_region_merge_nodes 
				) as q
			) as a
			left join temp_region_merge_links as b
			on a.branch_link = b.link_id
			left join rdb_node as c
			on a.low_level_node_id = c.node_id
		) as d
	) as e
);

create table temp_region_node_layerx_boundary_flag
as
(
	select node_id, case when array_upper(array_agg(tile_id),1) > 1 then True else False end as node_boundary_flag from 
	(
		select distinct node_id, tile_id from
		(
			select node_id, rdb_split_to_tileid(connect_link_id) as tile_id from
			(
				select node_id, unnest(branches) as connect_link_id from temp_region_merge_nodes 
			) as a
		) as b
	) as c
	group by node_id
);





create table temp_optimize_shortlink
as
(
	select link_id, start_node_id, end_node_id
	from temp_optimize_rdb_link_backup
	where link_length <= 3
);

create table temp_optimize_related_nodes
(
	group_id	integer,
	node_index	integer,
	node_id		bigint
);

create table temp_optimize_node_group
as
(
	select	m.group_id, 
			m.node_count, m.node_array, m.npx_4096_array, m.npy_4096_array, 
			m.npx_min_array, m.npy_min_array, m.npx_max_array, m.npy_max_array, 
			n.link_count, n.link_array, n.tile_array, n.angle_array, 
			rdb_get_node_index_array(n.f_node_array, m.node_array) as f_index_array, 
			n.f_node_array, n.fpx_4096_array, n.fpy_4096_array,
			rdb_get_node_index_array(n.t_node_array, m.node_array) as t_index_array, 
			n.t_node_array, n.tpx_4096_array, n.tpy_4096_array
	from 
	(
		select 	group_id, 
				count(*)::integer as node_count,
				array_agg(node_id) as node_array,
				array_agg(npx_4096) as npx_4096_array,
				array_agg(npy_4096) as npy_4096_array,
				array_agg(npx_min) as npx_min_array,
				array_agg(npy_min) as npy_min_array,
				array_agg(npx_max) as npx_max_array,
				array_agg(npy_max) as npy_max_array
		from
		(
			select	group_id, 
					node_index, 
					node_id, 
					npx_4096,
					npy_4096,
					(case when tx_array[1] = ALL(tx_array) then tx_array[1]*4096 else npx_4096 end) as npx_min,
					(case when ty_array[1] = ALL(ty_array) then ty_array[1]*4096 else npy_4096 end) as npy_min,
					(case when tx_array[1] = ALL(tx_array) then (tx_array[1]+1)*4096 else npx_4096 end) as npx_max,
					(case when ty_array[1] = ALL(ty_array) then (ty_array[1]+1)*4096 else npy_4096 end) as npy_max
			from
			(
				select	group_id, 
						node_index, 
						node_id, 
						px(node_4096) as npx_4096,
						py(node_4096) as npy_4096,
						array_agg(tx) as tx_array, 
						array_agg(ty) as ty_array
				from
				(
					select	a.group_id,
							a.node_index,
							a.node_id,
							lonlat2pixel_tile(0, 0, 0, st_x(b.the_geom), st_y(b.the_geom), (1<<14) * 4096) as node_4096,
							(c.link_id_t >> 14) & ((1 << 14) - 1) as tx, 
							c.link_id_t & ((1 << 14) - 1) as ty
					from temp_optimize_related_nodes as a
					left join temp_optimize_rdb_node_backup as b
					on a.node_id = b.node_id
					left join temp_optimize_rdb_link_backup as c
					on a.node_id in (c.start_node_id, c.end_node_id)
				)as t1
				group by group_id, node_index, node_id, node_4096
			)as t2
			order by group_id, node_index
		)as t3
		group by group_id having count(*) <= 5	-- added to avoid too long time
	)as m
	inner join
	(
		select *
		from
		(
			select	group_id,
					count(*) as link_count,
					array_agg(link_id) as link_array,
					array_agg(tile_id) as tile_array,
					array_agg(angle) as angle_array,
					array_agg(f_node) as f_node_array,
					array_agg(fpx_4096) as fpx_4096_array,
					array_agg(fpy_4096) as fpy_4096_array,
					array_agg(t_node) as t_node_array,
					array_agg(tpx_4096) as tpx_4096_array,
					array_agg(tpy_4096) as tpy_4096_array,
					array_agg(angle_same) as angle_same_array
			from
			(
				select	*,
						(angle_4096 is not null) and (abs(angle-angle_4096) <= 10 or abs(angle-angle_4096) >= 350) as angle_same
				from
				(
					select	*,
							px(f_4096) as fpx_4096,
							py(f_4096) as fpy_4096,
							px(t_4096) as tpx_4096,
							py(t_4096) as tpy_4096,
							rdb_get_angle_for_4096(px(f_4096), py(f_4096), px(t_4096), py(t_4096)) as angle_4096
					from
					(
						select	group_id,
								link_id,
								tile_id,
								rdb_get_angle_for_geom(f_geom, t_geom) as angle,
								f_node,
								lonlat2pixel_tile(0, 0, 0, st_x(f_geom), st_y(f_geom), (1<<14) * 4096) as f_4096,
								t_node,
								lonlat2pixel_tile(0, 0, 0, st_x(t_geom), st_y(t_geom), (1<<14) * 4096) as t_4096
						from
						(
							select 	a.group_id,
									b.link_id,
									b.link_id_t as tile_id,
									a.node_id as f_node, 
									(case when (a.node_id = b.start_node_id) then st_startpoint(b.the_geom) else st_endpoint(b.the_geom) end) as f_geom,
									(
										rdb_get_angle_shape_point(	b.the_geom, 
																	(a.node_id = b.start_node_id), 
																	(case when (a.node_id = b.start_node_id) then end_node_id else start_node_id end)
										)
									).*
							from temp_optimize_related_nodes as a
							left join temp_optimize_rdb_link_simplify_4326 as b
							on a.node_id in (b.start_node_id, b.end_node_id)
						)as t1
					)as t2
				)as t3
				order by group_id, f_node, link_id
			)as t4
			group by group_id
		)as t
		where False = ANY(angle_same_array)
	)as n
	on m.group_id = n.group_id
);


create table temp_optimize_try_all
as
(
	select	node_num, try_index, (rdb_optimize_get_try_array(try_index, max_move, node_num)).*
	from
	(
		select	node_num, 
				max_move,
				generate_series(0::integer, pow(node_pos_num, node_num)::integer - 1) as try_index
		from
		(
			select	node_num, 
					max_move,
					pow(2 * max_move + 1, 2)::smallint as node_pos_num				-- moving possibility of (x,y)
			from
			(
				select	distinct(node_count) as node_num,
						(case when node_count <= 4 then 2 else 1 end) as max_move		-- max moving distance(pixel) of node
				from temp_optimize_node_group
			)as t1
		)as t2
	)as t3
);

create table temp_optimize_node_new_position
(
	gid serial NOT NULL,
	node_id		bigint
); SELECT AddGeometryColumn('','temp_optimize_node_new_position','the_geom','4326','POINT',2);

create table temp_tile_level_14_park
(
	tile_id	integer,
	tile_x	integer,
	tile_y	integer
);SELECT AddGeometryColumn('','temp_tile_level_14_park','the_geom','4326','POLYGON',2);

CREATE TABLE temp_org_slope
(
  rdb_link_id 	bigint NOT NULL,
  org_link_id 	bigint NOT NULL,
  slope_value 	integer NOT NULL
);SELECT AddGeometryColumn('','temp_org_slope','slope_shape','4326','LINESTRING',2);SELECT AddGeometryColumn('','temp_org_slope','link_geom','4326','LINESTRING',2);

CREATE TABLE temp_rdb_slope
(
  rdb_link_id 	bigint NOT NULL,
  link_pos_s	double precision NOT NULL,
  link_pos_e	double precision NOT NULL,
  slope_value	integer NOT NULL
);SELECT AddGeometryColumn('','temp_rdb_slope','slope_shape','4326','LINESTRING',2);

create table temp_trf_names 
as
(
	select cid,nid
		,array_agg(lid) as lid_array
		,array_agg(language) as language_array
		,array_agg(name) as name_array
	from (
		select c.cid,c.nid,c.lid_org as lid,c.language_org as language
			,case when c.lid = c.lid_org then name
				else d.ntranslation 
			end as name
		from 
		(
			select a.cid,a.nid,a.lid,a.name
				,unnest(b.lid_array_org) as lid_org
				,unnest(b.language_array) as language_org
			from (
				select a.cid,a.nid,a.name,a.lid
					,b.language
				from gewi_names a
				left join gewi_languages b
				on a.cid = b.cid and a.lid = b.lid
			) a
			left join temp_trf_languages b
			on a.cid = b.cid 
			order by cid,nid,lid,name,language_org
		) c
		left join gewi_nametranslations d
		on c.cid = d.cid and c.nid = d.nid and c.lid_org = d.lid
		order by cid,nid,lid
	) e group by cid,nid
);

create table temp_trf_languages 
as
(
	select cid
		,array_agg(lid) as lid_array_org
		,array_agg(language) as language_array
	from (
		select a.cid,a.lid
			,case when b.l_full_name is not null then b.language_code_client
				else a.language
			end as language
		from (
			select m.cid,m.lid
				,case when n.language is not null then n.language
					else m.language
				end as language
			from gewi_languages m
			left join (
				select distinct lid,language
				from gewi_languages 
				where lower(language) != 'default'
			) n
			on m.lid = n.lid
		) a
		left join rdb_language b
		on lower(a.language) = lower(b.l_full_name)
	) a group by cid
);

CREATE TABLE temp_locationtable 
AS
(
	---point
	select b.ecc, b.ccd as cc, b.cname as country
		,a.tabcd::integer as table_no, a.lcd::integer as location_code
		,a.class::character varying as location_type, a.tcd::smallint as type, a.stcd::smallint as subtype
		,n1.name_array as road_name, n4.language_array as road_lang
		,n2.name_array as first_name, n4.language_array as first_lang
		,n3.name_array as second_name, n4.language_array as second_lang
		,c.roadnumber as road_number, n4.language_array as roadnumber_lang 
		,a.pol_lcd::integer as area_ref, a.roa_lcd::integer as line_ref
		,case when d.neg_off_lcd is null then 0
			  else d.neg_off_lcd::integer 
		 end as neg_offset
		,case when d.pos_off_lcd is null then 0
			  else d.pos_off_lcd::integer 
		 end as pos_offset
		,a.urban::integer as urban, 0 as veran, 0 as tern
		,e.int_lcd::integer as intersection_code
		,inpos::integer as in_pos,outpos::integer as out_pos,inneg::integer as in_neg,outneg::integer as out_neg
		,presentpos::integer as cur_pos,presentneg::integer as cur_neg, null::integer as exit_no
		,diversionpos as div_pos, diversionneg as div_neg
	from gewi_points a
	left join gewi_countries b
	on a.cid = b.cid
	left join gewi_roads c
	on a.cid = c.cid and a.tabcd = c.tabcd and a.lcd = c.lcd and a.class = c.class 
	and a.tcd = c.tcd and a.stcd = c.stcd
	left join gewi_poffsets d
	on a.cid = d.cid and a.tabcd = d.tabcd and a.lcd = d.lcd
	left join gewi_intersections e
	on a.cid = e.cid and a.tabcd = e.tabcd and a.lcd = e.lcd
	left join temp_trf_names n1
	on a.cid = n1.cid and a.rnid = n1.nid
	left join temp_trf_names n2
	on a.cid = n2.cid and a.n1id = n2.nid
	left join temp_trf_names n3
	on a.cid = n3.cid and a.n2id = n3.nid
	left join temp_trf_languages n4
	on a.cid = n4.cid	

	union
	---line
	select b.ecc, b.ccd as cc, b.cname as country
		,a.tabcd::integer as table_no, a.lcd::integer as location_code
		,a.class::character varying as location_type, a.tcd::smallint as type, a.stcd::smallint as subtype
		,n1.name_array as road_name, n4.language_array as road_lang
		,n2.name_array as first_name, n4.language_array as first_lang
		,n3.name_array as second_name, n4.language_array as second_lang
		,a.roadnumber as road_number, n4.language_array as roadnumber_lang
		,a.pol_lcd::integer as area_ref, null::integer as line_ref
		,null::integer as neg_offset
		,null::integer as pos_offset
		,0::integer as urban, 0 as veran, 0 as tern
		,null::integer as intersection_code
		,null::integer as in_pos,null::integer as out_pos,null::integer as in_neg,null::integer as out_neg
		,null::integer as cur_pos,null::integer as cur_neg, null::integer as exit_no
		,null as div_pos, null as div_neg
	from gewi_roads a
	left join gewi_countries b
	on a.cid = b.cid
	left join temp_trf_names n1
	on a.cid = n1.cid and a.rnid = n1.nid
	left join temp_trf_names n2
	on a.cid = n2.cid and a.n1id = n2.nid
	left join temp_trf_names n3
	on a.cid = n3.cid and a.n2id = n3.nid
	left join temp_trf_languages n4
	on a.cid = n4.cid
	
	union
	---area
	select b.ecc, b.ccd as cc, b.cname as country
		,a.tabcd::integer as table_no, a.lcd::integer as location_code
		,a.class::character varying as location_type, a.tcd::smallint as type, a.stcd::smallint as subtype
		,n.name_array as road_name, n4.language_array as road_lang
		,null::character varying[] as first_name, n4.language_array as first_lang
		,null::character varying[] as second_name, n4.language_array as second_lang
		,null::character varying as road_number, n4.language_array as roadnumber_lang
		,null::integer as area_ref, null::integer as line_ref
		,null::integer as neg_offset
		,null::integer as pos_offset
		,0::integer as urban, 0 as veran, 0 as tern
		,null::integer as intersection_code
		,null::integer as in_pos,null::integer as out_pos,null::integer as in_neg,null::integer as out_neg
		,null::integer as cur_pos,null::integer as cur_neg, null::integer as exit_no
		,null as div_pos, null as div_neg
	from gewi_administrativearea a
	left join gewi_countries b
	on a.cid = b.cid
	left join temp_trf_names n
	on a.cid = n.cid and a.nid = n.nid
	left join temp_trf_languages n4
	on a.cid = n4.cid where n4.cid is null	
	order by table_no,location_code
);

create table temp_region_promote_link
(
	link_id bigint primary key
);

create table temp_region_promote_path
(
	from_node_id bigint,
	to_node_id bigint,
	link_array bigint[]
);

create table temp_region_promote_connector_walked
(
	link_id	bigint primary key,
	island_id bigint
);

CREATE TABLE temp_guidence_node
(
	node_id bigint NOT NULL,
	the_geom geometry NOT NULL,
	polygon geometry
);

create table temp_link_deleted
(
	link_id	bigint
);

CREATE TABLE temp_rdb_name
(
  link_id bigint NOT NULL,
  name_id integer NOT NULL,
  name    character varying(8192)
);
