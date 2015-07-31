DROP INDEX IF EXISTS org_processed_line_folder_idx;
CREATE INDEX org_processed_line_folder_idx
  ON org_processed_line
  USING btree
  (folder);

DROP INDEX IF EXISTS org_processed_line_folder_link_id_idx;
CREATE UNIQUE INDEX org_processed_line_folder_link_id_idx
  ON org_processed_line
  USING btree
  (folder, link_id);
  
drop table if exists temp_topo_folderid;
create table temp_topo_folderid as
select row_number() over (order by folder) as id_cls,folder
from (
	select distinct folder from org_processed_line 
) a;


drop table if exists temp_topo_link_point;
create table temp_topo_link_point
as
select	(b.id_cls * 10000000) + a.link_id as new_link_id,
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
on a.folder = b.folder;


drop table if exists temp_topo_point;
create table temp_topo_point
as
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
)as t;

drop table if exists temp_topo_point_3cm;
create table temp_topo_point_3cm
as
select	*,
	(x*3600*1000)::integer as ix, (y*3600*1000)::integer as iy,
	(x*3600*1000-0.5)::integer as ix_m, (y*3600*1000-0.5)::integer as iy_m
from temp_topo_point;

create index temp_topo_point_3cm_ix_idx
	on temp_topo_point_3cm
	using btree
	(ix);

create index temp_topo_point_3cm_iy_idx
	on temp_topo_point_3cm
	using btree
	(iy);

create index temp_topo_point_3cm_ix_m_idx
	on temp_topo_point_3cm
	using btree
	(ix_m);

create index temp_topo_point_3cm_iy_m_idx
	on temp_topo_point_3cm
	using btree
	(iy_m);

drop table if exists temp_topo_near_point;
create table temp_topo_near_point
as
select a.gid as aid, b.gid as bid, st_distance(a.the_geom, b.the_geom, true) as distance
from temp_topo_point_3cm as a
inner join temp_topo_point_3cm as b
on 	(a.gid < b.gid)
	and
	(
		(a.ix = b.ix or a.ix_m = b.ix_m)
		and
		(a.iy = b.iy or a.iy_m = b.iy_m)
	);

create index temp_topo_near_point_aid_idx
	on temp_topo_near_point
	using btree
	(aid);

create index temp_topo_near_point_bid_idx
	on temp_topo_near_point
	using btree
	(bid);

drop table if exists temp_topo_node;
create table temp_topo_node
(
	node_id		integer,
	x		double precision,
	y		double precision,
	z		integer,
	the_geom	geometry
);

drop table if exists temp_topo_point_node;
create table temp_topo_point_node
(
	gid		integer,
	x		double precision,
	y		double precision,
	z		integer,
	node_id		integer,
	new_geom	geometry
);

drop table if exists temp_topo_walk_point;
create table temp_topo_walk_point
(
	gid	integer primary key
);

create or replace function temp_make_malsing_node()
returns integer
language plpgsql
AS $$
DECLARE
	rec			record;
	rec2			record;
	node_id			integer;
	walk_gid		integer;

	search_gid_array	integer[];
	search_index		integer;
	search_count		integer;
BEGIN
	node_id	:= 0;
	for rec in select * from temp_topo_point order by gid
	loop
		-- check whether currend point has been walked
		select gid from temp_topo_walk_point where gid = rec.gid into walk_gid;
		if FOUND then continue; end if;

		-- new node
		node_id	:= node_id + 1;
		insert into temp_topo_node(node_id, x, y, z, the_geom) values (node_id, rec.x, rec.y, rec.z, rec.the_geom);
	
		-- record current point
		insert into temp_topo_walk_point(gid) values (rec.gid);
		insert into temp_topo_point_node(gid, x, y, z, node_id) values (rec.gid, rec.x, rec.y, rec.z, node_id);

		-- search near point
		search_gid_array	:= ARRAY[rec.gid];
		search_count		:= 1;
		search_index		:= 0;
		while search_index < search_count loop
			search_index	:= search_index + 1;
			for rec2 in 
				select n.*
				from
				(
					select aid as gid 
					from temp_topo_near_point 
					where search_gid_array[search_index] = bid
					union
					select bid as gid 
					from temp_topo_near_point 
					where search_gid_array[search_index] = aid
				)as m
				left join temp_topo_point as n
				on m.gid = n.gid
			loop
				-- current point has been walked, then pass
				if rec2.gid = ANY(search_gid_array) then 
					continue;
				end if;

				-- record current point, and add it into search_array
				insert into temp_topo_walk_point(gid) values (rec2.gid);
				if st_equals(rec.the_geom, rec2.the_geom) then
					insert into temp_topo_point_node(gid, x, y, z, node_id) 
						values (rec2.gid, rec2.x, rec2.y, rec2.z, node_id);
				else
					insert into temp_topo_point_node(gid, x, y, z, node_id,new_geom) 
						values (rec2.gid, rec2.x, rec2.y, rec2.z, node_id,rec.the_geom);
				end if;
				
				search_gid_array	:= search_gid_array || rec2.gid;
				search_count		:= search_count + 1;
			end loop;
		end loop;
	end loop;
	return node_id;
END
$$;

select temp_make_malsing_node();

create index temp_topo_node_node_id_idx
	on temp_topo_node
	using btree
	(node_id);

create index temp_topo_node_the_geom_idx
	on temp_topo_node
	using gist
	(the_geom);

create index temp_topo_point_node_x_idx
	on temp_topo_point_node
	using btree
	(x);

create index temp_topo_point_node_y_idx
	on temp_topo_point_node
	using btree
	(y);

create index temp_topo_point_node_z_idx
	on temp_topo_point_node
	using btree
	(z);

drop table if exists temp_topo_link;
create table temp_topo_link
as
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
on t.x_end = b.x and t.y_end = b.y and t.z_end = b.z;

create index temp_topo_link_new_link_id_idx
	on temp_topo_link
	using btree
	(new_link_id);

create index temp_topo_link_folder_link_id_idx
	on temp_topo_link
	using btree
	(folder,link_id);
	
create index temp_topo_link_the_geom_idx
	on temp_topo_link
	using gist
	(the_geom);
