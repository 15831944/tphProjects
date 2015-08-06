
--------------------------------------------------------------------------------------------------------
CREATE TABLE temp_road_name_group
(
  group_id       serial not null primary key, 
  link_lid       bigint[] not null,
  street_names   character varying(512) not null,
  language_code  character(3) not null
);
--------------------------------------------------------------------------------------------------------
CREATE TABLE temp_road_name_groupid_nameid
(
  group_id    integer not null primary key,
  new_name_id integer not null
);

-- link name
CREATE TABLE temp_link_name
(
  link_id bigint NOT NULL primary key,
  name character varying(16384)
);

-- link shield
CREATE TABLE temp_link_shield
(
  link_id bigint NOT NULL primary key,
  shield character varying(2048)
);

-- eixt number
CREATE TABLE temp_sign_eixt_number_nameid
(
  sign_id integer NOT NULL,
  destination_number smallint NOT NULL,
  name_id integer NOT NULL,
  CONSTRAINT pk_temp_sign_eixt_number_nameid PRIMARY KEY (sign_id, destination_number)
 );

-- alt eixt number
CREATE TABLE temp_sign_alt_exit_number_nameid
(
  sign_id integer NOT NULL,
  destination_number smallint NOT NULL,
  name_id integer NOT NULL,
  CONSTRAINT pk_temp_sign_alt_exit_number_nameid PRIMARY KEY (sign_id, destination_number)
 );
 
 create table temp_natural_guidence_name
 (
   fp_id		bigint primary key,
   feat_name	varchar
 );
 
 create table temp_natural_guidence_asso_name
 (
   asso_id		bigint primary key,
   feat_name	varchar
 );
 
 create table temp_natural_guidence_fp_name
 (
   fp_id		bigint primary key,
   feat_name	varchar
 );
 
 create table temp_natural_guidence_fp_preposition
 (
   fp_id				bigint primary key,
   preposition			varchar
 );
 
--------------------------------------------------------------------------------------------------------
-- Cross Name
CREATE TABLE temp_node_name
(
  objectid integer primary key,
  name_id integer
);

CREATE TABLE temp_node_light
(
  node_id integer NOT NULL,
  CONSTRAINT temp_node_light_pkey PRIMARY KEY (node_id)
);

CREATE TABLE temp_node_stopsign
(
  node_id integer NOT NULL primary key
);

CREATE TABLE temp_node_bifurcation
(
  node_id integer NOT NULL primary key
);

CREATE TABLE temp_node_toll
(
  node_id integer NOT NULL,
  CONSTRAINT temp_node_toll_pkey PRIMARY KEY (node_id)
);

CREATE TABLE temp_node_lid
(
  node_id integer NOT NULL,
  lid integer[],
  CONSTRAINT temp_node_lid_pkey PRIMARY KEY (node_id)
);

CREATE TABLE temp_node_lid_ref
(
  node_id integer NOT NULL,
  lid integer[],
  CONSTRAINT temp_node_lid_ref_pkey PRIMARY KEY (node_id)
);

CREATE TABLE temp_node_lid_nonref
(
  node_id integer NOT NULL,
  lid integer[],
  CONSTRAINT temp_node_lid_nonref_pkey PRIMARY KEY (node_id)
);

CREATE TABLE temp_condition_direction_travel
(
  link_id integer NOT NULL,
  bearing integer[] NOT NULL, 
  CONSTRAINT temp_condition_direction_travel_pkey PRIMARY KEY (link_id)
);


--------------------------------------------------------------------------------------------------------
-- Regulation
CREATE TABLE temp_condition_regulation_tbl
(
  cond_id integer,
  access_id integer,
  dt_id bigint
);

--------------------------------------------------------------------------------------------------------
-- Lane

CREATE TABLE temp_lane_guide_nav_strand
(
  lane_nav_strand_id integer NOT NULL,
  seq_num integer NOT NULL,
  condition_id integer NOT NULL,
  lane_id bigint NOT NULL,
  node_id integer,
  link_id integer
);

CREATE TABLE temp_lane_guide_nav
(
  lane_nav_strand_id integer,
  condition_id integer,
  lane_id bigint,
  node_id integer,
  inlink_id integer,
  outlink_id integer,
  passcount integer,
  passlid character varying(512)
);

CREATE TABLE temp_lane_guide
(
  lane_nav_strand_id integer,
  condition_id integer,
  lane_id bigint,
  node_id integer,
  inlink_id integer,
  outlink_id integer,
  passcount integer,
  passlid character varying(512),
  lane_number integer,
  lane_type bigint,
  lane_travel_direction character(1),
  direction_category integer,
  lane_forming_ending integer,
  physical_num_lanes integer,
  bus_access_id integer
);

CREATE TABLE temp_lane_guide_distinct
(
  inlink_id integer,
  node_id integer,
  outlink_id integer,
  passcount integer,
  passlid character varying(512),
  lane_number integer,
  lane_travel_direction character(1),
  direction_category integer,
  physical_num_lanes integer,
  bus_access_id integer
);

CREATE TABLE temp_lane_tbl
(
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
  buslaneinfo character varying(17)
);


CREATE TABLE temp_lane
(
  inlink_id integer,
  node_id integer,
  outlink_id integer,
  passcount integer,
  passlid character varying(512),
  lane_number integer,
  lane_travel_direction character(1),
  direction_category integer,
  physical_num_lanes integer,
  lanenuml integer, 
  lanenumr integer,
  bus_access_id integer
);

CREATE TABLE temp_additional_lanenum_lr
(
  link_id integer,
  direction character varying(1),
  lanenuml integer,
  lanenumr integer
);

-- link
create table temp_link_through_traffic 
as 
(
	select a.link_id from temp_rdf_nav_link as a
	inner join rdf_access as b 
	on a.access_id = b.access_id
	where through_traffic = 'N'
);

create table temp_link_railcross 
as 
(
	select distinct t3.link_id from rdf_condition as t2 
	left join rdf_nav_strand as t3 
	on t2.nav_strand_id = t3.nav_strand_id    
	where t2.condition_type = 18
);

create table temp_link_pedestrians  
as 
(
	select a.link_id from temp_rdf_nav_link as a
	inner join rdf_access as b 
	on a.access_id = b.access_id
	where automobiles= 'N' and buses= 'N' and 
			taxis= 'N' and carpools= 'N' and 
			pedestrians= 'Y' and trucks= 'N' and 
			deliveries= 'N' and emergency_vehicles= 'N'
);

create table temp_link_under_construction
as 
(
	select a.link_id from temp_rdf_nav_link a
	inner join (
		select distinct t3.link_id from rdf_condition as t2 
		left join rdf_nav_strand as t3 
		on t2.nav_strand_id = t3.nav_strand_id    
		where t2.condition_type = 3
	) b
	on a.link_id = b.link_id
);

create table temp_link_public 
as
(
	select a.link_id from temp_rdf_nav_link as a
	inner join rdf_access as b 
	on a.access_id = b.access_id
	where automobiles= 'N' and carpools= 'N'
		  and trucks= 'N' and deliveries= 'N' 
		  and (buses= 'Y' or taxis= 'Y' 
			   or emergency_vehicles= 'Y')
);

create table temp_etc_only_link 
as 
(               
	select f.*,
		mid_make_etc_only_links_in_one_direction(inlink::bigint,inlink_nextnode::bigint) 
				as inlink_array,
		mid_make_etc_only_links_in_one_direction(outlink::bigint,outlink_nextnode::bigint) 
				as outlink_array
	from (    
		select c.nav_strand_id,c.node_id,c.inlink,c.outlink,
			case when c.node_id = d.ref_node_id then d.nonref_node_id
				 when c.node_id = d.nonref_node_id then d.ref_node_id
			end as inlink_nextnode,
			case when c.node_id = e.ref_node_id then e.nonref_node_id
				 when c.node_id = e.nonref_node_id then e.ref_node_id
			end as outlink_nextnode
		from (
			select a.nav_strand_id,a.node_id as node_id,
				a.link_id as inlink,b.link_id as outlink 
			from (
				select nav_strand_id,link_id,node_id 
				from rdf_nav_strand 
				where nav_strand_id in 
					(
					select nav_strand_id 
					from rdf_condition 
					where condition_id in 
					(
						SELECT condition_id  
						FROM rdf_condition_toll 
						where structure_type_obtain_ticket = 'N' 
								and structure_type_pay_ticket = 'N' 
								and structure_type_electronic = 'Y'
					) 
				) and seq_num = 0
			) a
			left join 
			(
				select nav_strand_id,link_id 
				from rdf_nav_strand 
				where nav_strand_id in 
					(
					select nav_strand_id from rdf_condition 
					where condition_id in 
					(
						SELECT condition_id  
						FROM rdf_condition_toll 
						where structure_type_fixed_fee = 'N' 
								and structure_type_obtain_ticket = 'N' 
								and structure_type_pay_ticket = 'N' 
								and structure_type_electronic = 'Y'
					) 
				) and seq_num = 1
			) b
			on a.nav_strand_id = b.nav_strand_id
		) c 
		left join temp_rdf_nav_link d
		on c.inlink = d.link_id
		left join temp_rdf_nav_link e
		on c.outlink = e.link_id
	) f
);

create table temp_link_lanes 
as 
(
	select a.lane_nav_strand_id,a.seq_num,c.lane_id,c.link_id,
		   c.direction_category from rdf_lane_nav_strand a        
	left join rdf_condition b
	on a.condition_id = b.condition_id
	left join rdf_lane c
	on a.lane_id = c.lane_id
	where b.condition_type = 13 
	order by a.lane_nav_strand_id,seq_num
);

create table temp_link_uturn 
as 
(
	select distinct a.link_id from temp_link_lanes a 
	left join (
		select * from temp_link_lanes 
		where direction_category in (16,2048)
	) b
	on a.lane_nav_strand_id = b.lane_nav_strand_id  
	where b.lane_nav_strand_id is not null
);

create table temp_link_ipd 
as
(
	select link_id 
	from temp_rdf_nav_link a
	left join rdf_nav_link_status b
	on a.status_id = b.status_id
	where b.in_process_data = 'Y'
);

create table temp_link_urban 
as
(
	select link_id 
	from temp_rdf_nav_link a
	left join rdf_nav_link_status b
	on a.status_id = b.status_id
	where b.urban = 'Y'
);

create table temp_link_rodizio 
(
	link_id bigint
);

create table temp_link_erp 
(
	link_id bigint,
	erp	smallint
);

create table temp_link_buslane 
(
	link_id bigint
);

CREATE TABLE temp_node_tbl_expand 
as
(
	select B.node_id, B.connect_lid, C.s_node, C.e_node, C.lane_dir from 
	(
		select node_id , cast(regexp_split_to_table(node_lids, E'\\\|+') as bigint) as connect_lid 
		 from (
			select node_id, substring(node_lid,1, length(node_lid)) as node_lids 
			  from node_tbl
		 ) as A
	) as B
	left join link_tbl as C
	on B.connect_lid = C.link_id
);


Create table temp_guideinfo_spotguide_junction_view_full
as
(
	select E.condition_id, E.condition_type, E.nav_strand_id, E.access_id,
		E.file_id, E.file_type,E.file_name, F.all_links, F.all_nodes from 
	(
		select A.condition_id, A.condition_type, A.nav_strand_id, A.access_id,
		B.file_id, C.file_type,C.file_name from 
		(
			select condition_id, condition_type, nav_strand_id, access_id
			from rdf_condition where condition_type = 20 
		) as A left join RDF_FILE_FEATURE as B
		on A.condition_id = B.feature_id left join rdf_file as C
		on B.file_id = C.file_id
		where file_type = 34
	) as E left join
	(
		select nav_strand_id, array_agg(link_id) as all_links, array_agg(node_id) as all_nodes from 
		(
			select nav_strand_id, seq_num, link_id, node_id from rdf_nav_strand where nav_strand_id in
			(
				SELECT nav_strand_id FROM rdf_condition where condition_type = 20 
			) order by nav_strand_id, seq_num
		) as D group by nav_strand_id 
	) as F on E.nav_strand_id = F.nav_strand_id
);

create table temp_guideinfo_spotguide_junction_view_chn
as
(
	select n.condition_id,n.condition_type,n.nav_strand_id,n.access_id
		,n.pattern_array[1] as pattern
		,n.arrow_array[1] as arrow
		,n.file_type_pattern[1] as pattern_file_type
		,n.file_type_arrow[1] as arrow_file_type
		,l.all_links
		,l.all_nodes		
	from 
	(
		select condition_id,condition_type,nav_strand_id,access_id
			,array_agg(file_name_pattern) as pattern_array
			,array_agg(file_name_arrow) as arrow_array
			,array_agg(file_type_pattern) as file_type_pattern
			,array_agg(file_type_arrow) as file_type_arrow
		from (
			select distinct a.condition_id, a.condition_type, a.nav_strand_id, a.access_id
				,b.file_id as file_id_pattern, c.file_type as file_type_pattern,c.file_name as file_name_pattern
				,d.file_id as file_id_arrow, e.file_type as file_type_arrow,e.file_name as file_name_arrow
			from 
			(
				select condition_id, condition_type, nav_strand_id, access_id
				from rdf_condition 
				where condition_type = 20 
			) as a 
			left join rdf_file_feature as b
			on a.condition_id = b.feature_id 
			left join rdf_file as c
			on b.file_id = c.file_id
			left join rdf_file_feature as d
			on a.condition_id = d.feature_id 
			left join rdf_file as e
			on d.file_id = e.file_id
			where c.file_type in (3,13) and e.file_type in (4,14)
			and e.file_type = c.file_type + 1
			order by condition_id,file_type_pattern desc
		) m
		group by condition_id,condition_type,nav_strand_id,access_id
	) n
	left join
	(
		select nav_strand_id, array_agg(link_id) as all_links, array_agg(node_id) as all_nodes 
		from 
		(
			select nav_strand_id, seq_num, link_id, node_id 
			from rdf_nav_strand 
			where nav_strand_id in
			(
				select nav_strand_id from rdf_condition 
				where condition_type = 20 
			) order by nav_strand_id, seq_num
		) as d 
		group by nav_strand_id 
	) l on n.nav_strand_id = l.nav_strand_id
);

create table temp_guideinfo_signpost_chn
as
(
	select n.condition_id,n.condition_type,n.nav_strand_id,n.access_id
		,n.pattern_array[1] as pattern
		,n.arrow_array[1] as arrow
		,l.all_links
		,l.all_nodes		
	from 
	(
		select condition_id,condition_type,nav_strand_id,access_id
			,array_agg(file_name_pattern) as pattern_array
			,array_agg(file_name_arrow) as arrow_array
		from (
			select distinct a.condition_id, a.condition_type, a.nav_strand_id, a.access_id
				,b.file_id as file_id_pattern, c.file_type as file_type_pattern,c.file_name as file_name_pattern
				,d.file_id as file_id_arrow, e.file_type as file_id_arrow,e.file_name as file_name_arrow
			from 
			(
				select condition_id, condition_type, nav_strand_id, access_id
				from rdf_condition 
				where condition_type = 20 
			) as a 
			left join rdf_file_feature as b
			on a.condition_id = b.feature_id 
			left join rdf_file as c
			on b.file_id = c.file_id
			left join rdf_file_feature as d
			on a.condition_id = d.feature_id 
			left join rdf_file as e
			on d.file_id = e.file_id
			where c.file_type in (36) and e.file_type in (37)
			and e.file_type = c.file_type + 1
			order by condition_id,file_type_pattern desc
		) m
		group by condition_id,condition_type,nav_strand_id,access_id
	) n
	left join
	(
		select nav_strand_id, array_agg(link_id) as all_links, array_agg(node_id) as all_nodes 
		from 
		(
			select nav_strand_id, seq_num, link_id, node_id 
			from rdf_nav_strand 
			where nav_strand_id in
			(
				select nav_strand_id from rdf_condition 
				where condition_type = 20 
			) order by nav_strand_id, seq_num
		) as d 
		group by nav_strand_id 
	) l on n.nav_strand_id = l.nav_strand_id
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
			select inlinkid as link_id, 2::smallint as linkdir
			from regulation_relation_tbl
			where condtype = 42 and cond_id is not null
			union
			select inlinkid as link_id, 3::smallint as linkdir
			from regulation_relation_tbl
			where condtype = 3
			union
			select inlinkid as link_id, 3::smallint as linkdir
			from regulation_relation_tbl
			where condtype = 43 and cond_id is not null
		)as t
	)as t2
	group by link_id
);

create table temp_rdf_nav_link_traffic
as
(
	select 	a.link_id, a.iso_country_code, a.ref_node_id, a.nonref_node_id, 
			(
			case 
			when b.link_id is not null then 4
			when a.travel_direction = 'B' then 1
			when a.travel_direction = 'F' and 3 = ANY(c.linkdir_array) then 1
			when a.travel_direction = 'T' and 2 = ANY(c.linkdir_array) then 1
			when a.travel_direction = 'F' then 2
			when a.travel_direction = 'T' then 3
			else null
			end
			) as one_way
	from temp_rdf_nav_link as a
	left join temp_link_regulation_forbid_traffic as b
	on a.link_id = b.link_id
	left join temp_link_regulation_permit_traffic as c
	on a.link_id = c.link_id
);

create table temp_node_nation_boundary
as
(
	select node_id
	from
	(
	  SELECT distinct a.node_id, b.iso_country_code
	  FROM temp_rdf_nav_node as a
	  left join temp_rdf_nav_link as b
	  on a.node_id in (b.ref_node_id, b.nonref_node_id)
	)as t
	group by node_id having count(*) > 1
);

create table temp_regulation_admin
as
(
	select b.admin_place_id
	from rdf_admin_attribute as a
	inner join rdf_admin_hierarchy as b
	on 	(a.admin_wide_regulations = 1) 
		and 
		a.admin_place_id in (b.country_id, b.order1_id, b.order2_id, b.order8_id, b.builtup_id)
);

create table temp_regulation_admin_link
as
(
	select 	m.link_id, 
			m.travel_direction, 
			t.one_way,
			m.ref_node_id as s_node, 
			m.nonref_node_id as e_node, 
			m.intersection_category,
			(case when m.intersection_category = 1 then true else false end) as bInner,
			m.multi_digitized,
			(case when m.multi_digitized = 'Y' then true else false end) as bDivide,
			n.name as road_name,
			rdb_get_link_angle(m.the_geom, true) as f_angle,
			rdb_get_link_angle(m.the_geom, false) as t_angle,
			m.the_geom
	from
	(
		select distinct b.*
		from temp_regulation_admin as a
		inner join temp_rdf_nav_link as b
		on a.admin_place_id in (b.left_admin_place_id, b.right_admin_place_id)
	)as m
	left join temp_link_name as n
    on m.link_id = n.link_id
    left join temp_rdf_nav_link_traffic as t
    on m.link_id = t.link_id
);

create table temp_regulation_admin_node
as
(
	select distinct b.node_id
	from temp_regulation_admin_link as a
	left join temp_rdf_nav_node as b
	on b.node_id in (a.s_node, a.e_node)
);

create table temp_awr_mainnode_sublink
(
	mainnode_id integer,
	sublink bigint
);

create table temp_awr_mainnode_subnode
(
	mainnode_id integer,
	subnode bigint
);

create table temp_awr_mainnode
as
(
	select mainnode_id, array_agg(subnode) as node_array
	from temp_awr_mainnode_subnode
	group by mainnode_id
);

create table temp_awr_mainnode_uturn
as
(
	select	*
	from
	(
		select	a.mainnode_id,
				b.road_name, 
				b.link_id as in_link,
				(case when b.s_node = ANY(a.node_array) then b.s_node else b.e_node end) as in_node,
				(case when b.s_node = ANY(a.node_array) then 'F' else 'T' end) as in_node_flag,
				(
				case 
				when b.s_node = ANY(a.node_array) then b.f_angle
				else b.t_angle
				end
				) as in_angle,
				c.link_id as out_link,
				(case when c.s_node = ANY(a.node_array) then c.s_node else c.e_node end) as out_node,
				(case when c.s_node = ANY(a.node_array) then 'F' else 'T' end) as out_node_flag,
				(
				case 
				when c.s_node = ANY(a.node_array) then c.f_angle
				else c.t_angle
				end
				) as out_angle
		from temp_awr_mainnode as a
		inner join temp_regulation_admin_link as b
		on 	(b.bInner is false)
			and
			(
	    		(b.s_node = ANY(a.node_array) and b.one_way in (1,3))
	    		or 
	    		(b.e_node = ANY(a.node_array) and b.one_way in (1,2))
			)
		inner join temp_regulation_admin_link as c
		on 	(c.bInner is false)
			and
			(
	    		(c.s_node = ANY(a.node_array) and c.one_way in (1,2))
	    		or 
	    		(c.e_node = ANY(a.node_array) and c.one_way in (1,3))
			)
		where 	(b.road_name is not distinct from c.road_name and b.bDivide is true and c.bDivide is true)
				or
				(b.link_id = c.link_id and b.bDivide is false)
	)as t
	where 	((road_name is not null) and (rdb_get_angle_diff(in_angle,out_angle) <= 85))
			or
			((road_name is null) and (rdb_get_angle_diff(in_angle,out_angle) <= 60))
);

create table temp_awr_node_uturn
as
(
	select	*
	from
	(
		select	a.node_id as node_id,
				b.road_name,
				b.link_id as in_link,
				(
				case 
				when b.s_node = a.node_id then b.f_angle
				else b.t_angle
				end
				) as in_angle,
				c.link_id as out_link,
				(
				case 
				when c.s_node = a.node_id then c.f_angle
				else c.t_angle
				end
				) as out_angle
		from temp_regulation_admin_node as a
		inner join temp_regulation_admin_link as b
		on 	(b.bDivide is true and b.bInner is false)
			and
			(
	    		(b.s_node = a.node_id and b.one_way in (1,3))
	    		or 
	    		(b.e_node = a.node_id and b.one_way in (1,2))
			)
		inner join temp_regulation_admin_link as c
		on 	(c.bDivide is true and c.bInner is false)
			and
			(
	    		(c.s_node = a.node_id and c.one_way in (1,2))
	    		or 
	    		(c.e_node = a.node_id and c.one_way in (1,3))
			)
		where (b.road_name is not distinct from c.road_name)
	)as t
	where 	((road_name is not null) and (rdb_get_angle_diff(in_angle,out_angle) <= 85))
			or
			((road_name is null) and (rdb_get_angle_diff(in_angle,out_angle) <= 60))
);

create table temp_awr_pdm_linkrow
as
(
	select	regulation_id, 
			link_count, 
			link_array,
			link_array[1] as first_link,
			array_to_string(link_array, ',') as linkrow
	from
	(
		select	regulation_id, 
				count(*) as link_count, 
				array_agg(linkid) as link_array
		from
		(
			select b.regulation_id, b.linkid
			from (select distinct regulation_id from regulation_relation_tbl where condtype = 10 and outlinkid is not null) as a
			inner join regulation_item_tbl as b
			on a.regulation_id = b.regulation_id
			where b.seq_num != 2
			order by b.regulation_id, b.seq_num
		)as t
		group by regulation_id
	)as t
);

create table temp_link_regulation_pdm_flag
as
(
	select a.link_id, 1::smallint as pdm_flag
	from temp_regulation_admin_link as a
	left join regulation_relation_tbl as b
	on (b.condtype = 10) and a.link_id in (b.inlinkid, b.outlinkid)
	where b.regulation_id is null
);

----------------------------------------------------------------------------------------------------------
-- temp 
----------------------------------------------------------------------------------------------------------
CREATE TABLE temp_wkt_link
(
  link_id integer NOT NULL PRIMARY KEY
); SELECT AddGeometryColumn('','temp_wkt_link','the_geom','4326','LINESTRING',2);

CREATE TABLE temp_wkt_node
(
  node_id integer NOT NULL PRIMARY KEY
); SELECT AddGeometryColumn('','temp_wkt_node','the_geom','4326','POINT',2);

CREATE TABLE temp_wkt_building
(
  building_id integer NOT NULL  PRIMARY KEY
); SELECT AddGeometryColumn('','temp_wkt_building','the_geom','4326','POLYGON',2);

CREATE TABLE temp_wkt_location
(
  location_id bigint NOT NULL PRIMARY KEY
); SELECT AddGeometryColumn('','temp_wkt_location','the_geom','4326','POINT',2);

CREATE TABLE temp_wkt_face
(
  face_id integer NOT NULL PRIMARY KEY
);SELECT AddGeometryColumn('','temp_wkt_face','the_geom','4326','POLYGON',2);

-- Table: temp_rdf_nav_link
CREATE TABLE temp_rdf_nav_link
(
  link_id integer NOT NULL PRIMARY KEY,
  iso_country_code character(3) NOT NULL,
  access_id integer NOT NULL,
  status_id integer NOT NULL,
  functional_class integer NOT NULL,
  controlled_access character(1) NOT NULL,
  travel_direction character(1) NOT NULL,
  boat_ferry character(1) NOT NULL,
  rail_ferry character(1) NOT NULL,
  multi_digitized character(1) NOT NULL,
  divider character(1) NOT NULL,
  divider_legal character(1) NOT NULL,
  frontage character(1) NOT NULL,
  paved character(1) NOT NULL,
  ramp character(1) NOT NULL,
  private character(1) NOT NULL,
  tollway character(1) NOT NULL,
  poi_access character(1) NOT NULL,
  intersection_category integer,
  speed_category integer NOT NULL,
  lane_category integer NOT NULL,
  coverage_indicator character(2),
  from_ref_num_lanes integer,
  to_ref_num_lanes integer,
  physical_num_lanes integer,
  from_ref_speed_limit integer,
  to_ref_speed_limit integer,
  speed_limit_source character(2),
  low_mobility integer NOT NULL,
  public_access character(1) NOT NULL,
  ref_node_id integer NOT NULL,
  nonref_node_id integer NOT NULL,
  left_admin_place_id integer NOT NULL,
  right_admin_place_id integer NOT NULL,
  left_postal_area_id integer,
  right_postal_area_id integer,
  bridge character(1) NOT NULL,
  tunnel character(1) NOT NULL
); SELECT AddGeometryColumn('','temp_rdf_nav_link','the_geom','4326','LINESTRING',2);

-- nav node
CREATE TABLE temp_rdf_nav_node
(
  node_id integer NOT NULL PRIMARY KEY,
  lat integer NOT NULL,
  lon integer NOT NULL,
  z_coord integer,
  zlevel integer NOT NULL,
  is_aligned character(1) NOT NULL
); SELECT AddGeometryColumn('','temp_rdf_nav_node','the_geom','4326','POINT',2);

CREATE TABLE rdb_admin_country
(
  country_id integer PRIMARY KEY,
  iso_country_code character(3),
  country_name character varying(100)
); SELECT AddGeometryColumn('public','rdb_admin_country','the_geom','4326','MULTIPOLYGON',2);

CREATE TABLE rdb_admin_order1
(
  order1_id integer PRIMARY KEY,
  country_id integer,
  order1_name character varying(100)
); SELECT AddGeometryColumn('public','rdb_admin_order1','the_geom','4326','MULTIPOLYGON',2);

----------------------------------------------------------------------------------------------------------
-- temp 900913
----------------------------------------------------------------------------------------------------------
CREATE TABLE temp_wkt_link_900913
(
  link_id integer NOT NULL PRIMARY KEY
); SELECT AddGeometryColumn('','temp_wkt_link_900913','the_geom','900913','LINESTRING',2);

CREATE TABLE temp_wkt_building_900913
(
  building_id integer NOT NULL  PRIMARY KEY
); SELECT AddGeometryColumn('','temp_wkt_building_900913','the_geom','900913','POLYGON',2);

CREATE TABLE temp_wkt_location_900913
(
  location_id integer NOT NULL PRIMARY KEY
); SELECT AddGeometryColumn('','temp_wkt_location_900913','the_geom','900913','POINT',2);

CREATE TABLE temp_wkt_face_900913
(
  face_id integer NOT NULL PRIMARY KEY
);SELECT AddGeometryColumn('','temp_wkt_face_900913','the_geom','900913','POLYGON',2);

CREATE TABLE temp_nav_node
(
  node_id integer NOT NULL primary key
);


----------------------------------------------------------------------------
-- temp link name
CREATE TABLE mid_temp_link_name
(
  link_id bigint,
  road_link_id bigint,
  left_address_range_id bigint,
  right_address_range_id bigint,
  address_type integer,
  is_exit_name character(1),
  explicatable character(1),
  is_junction_name character(1),
  is_name_on_roadsign character(1),
  is_postal_name character(1),
  is_stale_name character(1),
  is_vanity_name character(1),
  is_scenic_name character(1),
  road_name_id bigint,
  route_type integer,
  attached_to_base character(1),
  precedes_base character(1),
  prefix character varying(10),
  street_type character varying(30),
  suffix character varying(10),
  base_name character varying(250),
  language_code character(3),
  is_exonym character(1),
  name_type character(1),
  direction_on_sign character(1),
  street_name character varying(250)
);

----------------------------------------------------------------------------
-- name merged
CREATE TABLE mid_temp_link_name_merged
(
  link_id bigint,
  street_names text,
  language_code_group character varying[]
);

----------------------------------------------------------------------------
-- temp link number
CREATE TABLE mid_temp_link_number
(
  link_id bigint,
  road_link_id bigint,
  left_address_range_id bigint,
  right_address_range_id bigint,
  address_type integer,
  is_exit_name character(1),
  explicatable character(1),
  is_junction_name character(1),
  is_name_on_roadsign character(1),
  is_postal_name character(1),
  is_stale_name character(1),
  is_vanity_name character(1),
  is_scenic_name character(1),
  road_name_id bigint,
  route_type integer,
  attached_to_base character(1),
  precedes_base character(1),
  prefix character varying(10),
  street_type character varying(30),
  suffix character varying(10),
  base_name character varying(60),
  language_code character(3),
  is_exonym character(1),
  name_type character(1),
  direction_on_sign character(1),
  street_name character varying(100)
);

----------------------------------------------------------------------------
-- temp link merged number 
CREATE TABLE mid_temp_link_number_merged
(
  link_id       bigint not null,
  route_type    smallint not null,
  shield_number character varying(24) not null,
  language_code character(3) not null,
  seq_num       smallint not null
);

--------------------------------------------------------------------------------------------------------
CREATE TABLE temp_link_number_group
(
  group_id         serial not null primary key,
  link_lid         bigint[] not null,
  route_type_array smallint[] not null,
  shield_number    character varying(100) NOT NULL,
  language_code    character(3) not null,
  seq_num_array    smallint[] not null
);

--------------------------------------------------------------------------------------------------------
CREATE TABLE temp_link_number_groupid_nameid
(
  group_id integer not null primary key,
  new_name_id integer not null
);

----------------------------------------------------------------------------
-- toward name merged
CREATE TABLE mid_temp_toward_name_merged
(
  sign_id               bigint not null,
  destination_number    integer not null,
  toward_names          character varying(512) not null,
  language_code_group   character varying[]
);

--------------------------------------------------------------------------------------------------------
CREATE TABLE mid_temp_toward_name_group
(
  group_id       serial not null primary key,
  sign_lid       bigint[] not null,
  dest_num_array integer[] not null,
  toward_names   character varying(512) not null,
  language_code  character(3) not null
);

--------------------------------------------------------------------------------------------------------
CREATE TABLE mid_temp_toward_name_groupid_nameid
(
  group_id           integer not null primary key,
  name_id            integer NOT NULL
);

--------------------------------------------------------------------------------------------------------
-- Toward Name
CREATE TABLE temp_toward_name
(
  sign_id            bigint NOT NULL,
  destination_number integer not null,
  name_id            integer NOT NULL,
  CONSTRAINT temp_toward_name_pkey PRIMARY KEY (sign_id, destination_number)
);

--------------------------------------------------------------------------------------------------------
--Unconnect Toward Name
CREATE TABLE temp_unconnect_toward_guide
(
  sign_id            bigint not null,
  in_link_id         bigint,
  in_direct          character(1),
  in_start_node_id   bigint,
  in_end_node_id     bigint,
  out_link_id        bigint,
  out_direct         character(1),
  out_start_node_id  bigint,
  out_end_node_id    bigint,
  destination_number smallint
);

--------------------------------------------------------------------------------------------------------
-- store pass_link
CREATE TABLE temp_towardname_guide_middle_layer
(
  sign_id            integer NOT NULL,
  inlinkid           integer NOT NULL,
  nodeid             integer NOT NULL,
  outlinkid          integer NOT NULL,
  passlink_cnt       integer,
  passlink           integer[],
  destination_number smallint,
  CONSTRAINT temp_towardname_guide_middle_layer_pkey PRIMARY KEY (sign_id, destination_number)
);

--------------------------------------------------------------------------------------------------------
-- 
CREATE TABLE mid_temp_sign_element_trans_group
(
  sign_element_id bigint not null primary key,
  trans_types text not null,
  trans_names text not null
);

--------------------------------------------------------------------------------------------------------
CREATE TABLE mid_temp_sign_destination_trans_group
(
  sign_id bigint,
  destination_number integer,
  trans_exit_langs text,
  trans_exit_nums text,
  CONSTRAINT mid_temp_sign_destination_trans_group_pkey PRIMARY KEY 
       (sign_id, destination_number)
);

--------------------------------------------------------------------------------------------------------
CREATE TABLE mid_temp_feature_name
(
  feature_id     bigint not null primary key,
  feature_name   character varying(65536)
);

--------------------------------------------------------------------------------------------------------
CREATE TABLE mid_temp_road_name_trans_group
(
  road_name_id  bigint not null primary key,
  trans_types   character varying(1024),
  trans_names   character varying(1024)
);

--------------------------------------------------------------------------------------------------------
CREATE TABLE mid_temp_road_name
(
  gid     serial not null primary key,
  link_id bigint not null,
  road_link_id bigint,
  road_name_id bigint,
  left_address_range_id bigint,
  right_address_range_id bigint,
  address_type integer,
  is_exit_name character(1),
  explicatable character(1),
  is_junction_name character(1),
  is_name_on_roadsign character(1),
  is_postal_name character(1),
  is_stale_name character(1),
  is_vanity_name character(1),
  is_scenic_name character(1),
  route_type integer,
  street_type character varying(30),
  language_code character varying(3),
  street_name character varying(100)
);

--------------------------------------------------------------------------------------------------------
CREATE TABLE mid_temp_signpost_element
(
  sign_id            bigint NOT NULL,
  destination_number integer NOT NULL,
  signpost_name      character varying(8192),
  route_no1          character varying(1024),
  route_no2          character varying(1024),
  route_no3          character varying(1024),
  route_no4          character varying(1024),
  CONSTRAINT mid_temp_signpost_element_pkey PRIMARY KEY (sign_id, destination_number)
);

--------------------------------------------------------------------------------------------------------
CREATE TABLE mid_temp_signpost_exit_no
(
  sign_id            bigint NOT NULL,
  destination_number integer NOT NULL,
  exit_no            character varying(1024),
  CONSTRAINT mid_temp_signpost_exit_no_pkey PRIMARY KEY (sign_id, destination_number)
);

--------------------------------------------------------------------------------------------------------
CREATE TABLE mid_temp_signpost_passlink
(
  sign_id            bigint NOT NULL,
  destination_number integer NOT NULL,
  nodeid             bigint NOT NULL,
  inlinkid           bigint NOT NULL,
  outlinkid          bigint NOT NULL,
  passlid            character varying(1024),
  passlink_cnt       smallint NOT NULL,
  CONSTRAINT mid_temp_signpost_passlink_pkey PRIMARY KEY (sign_id, destination_number)
);

--------------------------------------------------------------------------------------------------------
CREATE TABLE mid_temp_signpost_passlink_shortest_distance
(
  sign_id            bigint NOT NULL,
  destination_number integer NOT NULL,
  nodeid             bigint NOT NULL,
  inlinkid           bigint NOT NULL,
  outlinkid          bigint NOT NULL,
  passlid            character varying(1024),
  passlink_cnt       smallint NOT NULL,
  CONSTRAINT mid_temp_signpost_passlink_shortest_distance_pkey PRIMARY KEY (sign_id, destination_number)
);
--------------------------------------------------------------------------------------------------------
CREATE TABLE temp_gjv_junctionview_info
(
  gid serial NOT NULL,
  nodeid bigint,
  inlink bigint,
  outlink bigint,
  jv_type integer,
  filename character varying(800),
  side character(1),
  pathlink character varying,
  ramp character(1),
  bif character(1),
  ca character(1)
);
CREATE TABLE temp_ejv_junctionview_info
(
  gid serial NOT NULL,
  nodeid bigint,
  inlink bigint,
  outlink bigint,
  jv_type integer,
  filename character varying(800),
  side character(1),
  pathlink character varying,
  ramp character(1),
  bif character(1),
  ca character(1)
);
--------------------------------------------------------------------------------------------------------
CREATE TABLE mid_temp_road_phonetic
(
  road_name_id            bigint primary key not null,
  phonetic_ids            bigint[] not null,
  phonetic_strings        character varying[] not null,
  phonetic_language_codes character varying[] not null
);

--------------------------------------------------------------------------------------------------------
CREATE TABLE mid_temp_feature_phontetic
(
  name_id                  bigint not null primary key,
  phonetic_ids             character varying[],
  phonetic_strings         character varying[],
  phonetic_language_codes  character varying[]
);

--------------------------------------------------------------------------------------------------------
CREATE TABLE mid_temp_sign_element_phonetic
(
  sign_element_id            bigint not null primary key,
  phonetic_ids               bigint[] not null,
  phonetic_strings           character varying[],
  phonetic_language_codes    character varying[]
);

--------------------------------------------------------------------------------------------------------
CREATE TABLE mid_temp_sign_destination_phonetic
(
  sign_id                  BIGINT NOT NULL,
  destination_number       integer,
  phonetic_ids             BIGINT[],
  phonetic_strings         CHARACTER VARYING[],
  phonetic_language_codes  CHARACTER VARYING[],
  CONSTRAINT mid_temp_sign_destination_phonetic_pkey PRIMARY KEY (sign_id, destination_number)
);
CREATE TABLE rdfpl_gjv_lat_display_org
(
  gid serial NOT NULL,
  dp_node_id bigint,
  originating_link_id bigint,
  dest_link_id bigint,
  ramp character(1),
  bif character(1),
  ca character(1),
  filename character varying(800),
  side character(1),
  sign_dest bigint,
  jv_origin bigint,
  iso_country_code character(3),
  gms_svg character varying(800),
  gms_template character varying(255),
  mdps character(1),
  mdps_approx character(4),
  tunnel_origin_link character(4),
  tunnel_dest_link character(4),
  origin_bearing character varying(10),
  dest_bearing character varying(10),
  dp2_node_id bigint,
  latitude character varying(4000),
  longitude character varying(4000)
);

--------------------------------------------------------------------------------------------------------
CREATE TABLE signpost_uc_tbl_bak_push_inlink
(
  gid          integer NOT NULL primary key,
  id           bigint,
  nodeid       bigint NOT NULL,
  inlinkid     bigint NOT NULL,
  outlinkid    bigint NOT NULL,
  passlid      character varying(1024),
  passlink_cnt smallint NOT NULL,
  sp_name      character varying(8192),
  route_no1    character varying(1024),
  route_no2    character varying(1024),
  route_no3    character varying(1024),
  route_no4    character varying(1024),
  exit_no      character varying(1024)
);
-----------------------------------------------------------------------------------------------------------------
CREATE TABLE rdfpl_all_jv_lat_display_org
(
  gid serial NOT NULL,
  dp_node_id bigint,
  originating_link_id bigint,
  dest_link_id bigint,
  ramp character(1),
  bif character(1),
  ca character(1),
  filename character varying(800),
  side character(1),
  sign_dest bigint,
  jv_origin bigint,
  iso_country_code character(3),
  gms_svg character varying(800),
  gms_template character varying(255),
  mdps character(1),
  mdps_approx character(4),
  tunnel_origin_link character(4),
  tunnel_dest_link character(4),
  origin_bearing character varying(10),
  dest_bearing character varying(10),
  dp2_node_id bigint,
  latitude character varying(4000),
  longitude character varying(4000),
  ejv_filename character varying(800),
  sar_filename character varying(800),
  dp1_dest_link bigint,
  jv_dest_link bigint
);

------------------------------------------------
CREATE TABLE temp_link_railwaycross
AS
select link_id
	,case when 0 = any(flag_array) and 1 = any(flag_array) then 2
		when 0 = any(flag_array) then 0
		else 1 end as flag
from (
	select array_agg(node_id) as node_array, link_id, array_agg(flag) as flag_array
	from(
		select node_id,link_array[1] as link_id,flag_array[1] as flag
		from (
			select node_id,array_agg(link_id) as link_array,array_agg(flag) as flag_array
			from (
				select a.condition_id,a.nav_strand_id
					,b.seq_num,b.link_id,b.node_id
					,case when b.node_id = c.ref_node_id then 0
						when b.node_id = c.nonref_node_id then 1
					end as flag
				from rdf_condition a
				left join rdf_nav_strand b
				on a.nav_strand_id = b.nav_strand_id
				left join temp_rdf_nav_link c
				on b.link_id = c.link_id
				where a.condition_type = 18
			) m group by node_id
		) n 
	) o group by link_id
) p;

create table temp_link_category6
as
(
	select link_id,s_node,e_node,one_way_code,0 as round_num, false as flage,fazm,tazm,the_geom
	from
	(
		select a.link_id, ref_node_id as s_node, nonref_node_id as e_node, 
			mid_get_oneway_code(travel_direction,b.link_id, c.linkdir,d.linkdir_array) as one_way_code, 
			mid_cal_zm(a.the_geom, 1) as fazm, mid_cal_zm(a.the_geom, -1) as tazm, the_geom
		from temp_rdf_nav_link as a
	  left join temp_link_under_construction b
	  on a.link_id = b.link_id
	  left outer join temp_link_regulation_forbid_traffic as c 
	  on a.link_id = c.link_id                                               
	  left outer join temp_link_regulation_permit_traffic as d 
	  on a.link_id = d.link_id
	  where intersection_category = 6
  )temp
  where one_way_code in (2,3)
);
create table temp_node_connect_acute_link
as
(
	select node_id
	from
	(
		select node_id,array_agg(angle) as angle_array
		from
		(
			select link_id,fazm as angle,s_node as node_id
			from temp_link_category6
			
			union
			
			select link_id,tazm as angle,e_node as node_id
			from temp_link_category6
		)temp
		group by node_id
	)temp1
	where mid_jude_link_angle(angle_array)
);
create table temp_link_category6_roundabout
(
	link_id bigint
);SELECT AddGeometryColumn('','temp_link_category6_roundabout','the_geom','4326','LINESTRING',2);
----------------------------------------------------------------------------------------------------
---caution
----------------------------------------------------------------------------------------------------
CREATE TABLE temp_admin_line
(
	ad_code integer not null
);SELECT AddGeometryColumn('','temp_admin_line','the_geom','4326','LINESTRING',2);
CREATE TABLE temp_link_caution
(
	link_id bigint not null,
	s_node bigint not null,
	e_node bigint not null,
	one_way_code smallint
);SELECT AddGeometryColumn('','temp_link_caution','the_geom','4326','LINESTRING',2);

CREATE TABLE temp_node_in_admin
as
(
	select node_id
	from 
	(
		select b.node_id,b.the_geom
		from
		(
			SELECT s_node as node_id
		  FROM temp_link_caution
		  	  
			union
			
			SELECT e_node as node_id
		  FROM temp_link_caution
		) as a
		left join node_tbl as b
		on a.node_id = b.node_id
	)temp
	join temp_admin_line as temp2
	on ST_Contains(temp2.the_geom,temp.the_geom)
);

create table temp_natural_guidence
as
(
	select	*,
			st_setsrid(st_makeline(f_geom, t_geom), 4326) as the_geom
	from
	(
		select	a.asso_id, 
				a.asso_type,
				b.link_id,
				c.direction,
				c.seasonal_dependency,
				c.visibility,
				c.calc_importance,
				d.dt_id,
				e.poi_id,
				fp.fp_id as f_fp_id,
				fp.link_id as f_link_id,
				fp.side as f_side,
				tp.fp_id as t_fp_id,
				tp.link_id as t_link_id,
				tp.side as t_side,
				tpp.preposition_code,
				st_setsrid(st_point(fp.lon / 100000.0, fp.lat / 100000.0), 4326) as f_geom,
				st_setsrid(st_point(tp.lon / 100000.0, tp.lat / 100000.0), 4326) as t_geom,
				tpps.preposition,
				n.guidence_name
		from rdf_asso as a
		left join rdf_asso_feat_point as fa
			on a.asso_id = fa.asso_id
		left join rdf_feature_point as fp
			on fa.fp_id = fp.fp_id
		left join rdf_asso_feat_point as ta
			on a.asso_id = ta.asso_id
		left join rdf_feature_point as tp
			on ta.fp_id = tp.fp_id
		left join rdf_feature_point_prep as tpp
			on tp.fp_id = tpp.fp_id and tpp.preposition_code like 'ENG%'
		left join rdf_asso_link as b
			on a.asso_id = b.asso_id
		left join rdf_asso_natural_guidance as c
			on a.asso_id = c.asso_id
		left join rdf_asso_dt as d
			on a.asso_id = d.asso_id
		left join rdf_asso_poi as e
			on a.asso_id = e.asso_id
		left join rdf_meta_preposition as tpps
			on tpp.preposition_code = tpps.preposition_code
		left join
		(
			select a.asso_id, array_to_string(array_agg(b.name), ' / ') as guidence_name
			from rdf_asso_names as a
			inner join rdf_asso_name as b
			on a.name_id = b.name_id and b.language_code = 'ENG'
			group by a.asso_id
		)as n
			on a.asso_id = n.asso_id
		where fp.fp_type = 'GP' and tp.fp_type = 'RG' and c.visibility != 3
	)as t
	order by asso_type, link_id, direction, t_link_id, asso_id
);alter table temp_natural_guidence add column gid serial primary key;

create table temp_asso_feat_name
(
	asso_id			bigint,
	name_string		varchar
);

create table temp_natural_guidence_paths
(
	gid				bigint,
	link_array		bigint[]
);

create table temp_meta_preposition_code
as
(
	select 	row_number() over (order by preposition_code) as preposition_code,
			preposition_code as preposition_code_string
	from
	(
		select distinct preposition_code
		from temp_natural_guidence
		order by preposition_code
	)as t
);

create table temp_admin_dst_mapping
(
	dst_id bigint,
	summer_time_id smallint
);

create table temp_guidence_datetime_mapping
(
	dt_id bigint,
	condition_id smallint
);
CREATE TABLE org_sign_as_real
(
  gid serial NOT NULL,
  filename character varying(128),
  inlinkid integer
);

create table temp_adas_link_geometry
(
	gid serial not null primary key,
	link_id bigint not null,
	slope_lib varchar
);

create table temp_poi_logmark
(
	poi_id bigint
);

create table temp_poi_name
(
	poi_id bigint,
	poi_name varchar
);

create table temp_category_priority
(
    per_code          character varying(1024)   not null,
    category_priority    int   
 
);

create table temp_poi_category1
(
    per_code   bigint       not null,
    gen1       int          not null,
    gen2       int          not null,
    gen3       int          not null,
    level      smallint     not null,
    name       varchar(128) not null,
    imp        smallint     not null,
    org_code   bigint       not null,
    tr_name    varchar(128) not null default ''
);


create table temp_poi_category
(
    per_code   bigint       not null,
    is_brand   varchar,
    gen1       int          not null,
    gen2       int          not null,
    gen3       int          not null,
    level      smallint     not null,
    name       varchar(128) not null,
    genre_type varchar,
    org_code   bigint,
    sub        integer,
    chain      bigint,
    cuisine    integer,
    building   integer,    
    three       bigint,
    lang        varchar,
    filename    varchar
);

--genre_id,is_brand,g0,g1,g2,level,name,genre_type,cat_id,sub,chain,cuisine,building,three,lang,filename
--------------------------------------------------------------------------------------------------------
CREATE TABLE mid_temp_hwy_sapa_name_group
(
  name_id          bigint NOT NULL,
  trans_types      character varying(1024),
  trans_names      character varying(1024)
);

--------------------------------------------------------------------------------------------------------
CREATE TABLE mid_temp_hwy_exit_poi_name
(
   node_id         bigint not null primary key,
   poi_id          bigint not null,
   name            character varying(1024)   --JSON
);

--------------------------------------------------------------------------------------------------------
CREATE TABLE mid_temp_hwy_exit_poi_name_trans_group
(
   name_id        bigint not null primary key,
   trans_type     character(3),
   trans_name     character varying(255)
);

--------------------------------------------------------------------------------------------------------
CREATE TABLE mid_temp_hwy_exit_poi
(  
   gid             serial not null primary key,
   poi_id          bigint not null,
   cat_id          bigint
); SELECT AddGeometryColumn('','mid_temp_hwy_exit_poi','the_geom','4326','POINT',2);

--------------------------------------------------------------------------------------------------------
create table temp_highaccid_links 
as
(
	select c.link_id, 
		case when a.direction = 1 then 2
			when a.direction = 2 then 3
			when a.direction = 3 then 1
			else 0
		end as dir, 
		d.ref_node_id as s_node, d.nonref_node_id as e_node
	FROM (
		select *
		from rdf_condition
		where condition_type = 38
	) as b
	left join rdf_condition_blackspot as a
	on b.condition_id = a.condition_id
	left join rdf_nav_strand as c
	on b.nav_strand_id = c.nav_strand_id
	left join temp_rdf_nav_link d
	on c.link_id = d.link_id
	where c.node_id is null
);

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