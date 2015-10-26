
CREATE OR REPLACE FUNCTION rdb_transform_4326_to_4096(shape geometry) 
  RETURNS geometry
  LANGUAGE plpgsql
AS $$
DECLARE
	point_num	integer;
	point_idx	integer;
	points_4096	geometry[];
	shape_4096	geometry;
BEGIN
	if st_geometrytype(shape) = 'ST_Point' then
		shape_4096 = rdb_move_point_pixel(shape, 14, 4096);
		
	elseif st_geometrytype(shape) = 'ST_LineString' then
		point_num = st_numpoints(shape);
		for point_idx in 1..point_num loop
			points_4096[point_idx] = rdb_move_point_pixel(st_pointn(shape,point_idx), 14, 4096);
		end loop;
		shape_4096 = st_makeline(points_4096);
	
	elseif st_geometrytype(shape) = 'ST_Polygon' then
		select ST_PolygonFromText(st_astext(st_collect(point))) as the_geom
		from
		(
			select path, rdb_move_point_pixel(geom, 14, 4096) as point
			from
			(
				select (st_dumppoints(shape)).*
			)as t
			order by path
		)as t2
		into shape_4096;
	else
		raise Exception 'unexcepted geometry type while transform 4326 to 4096.';
	end if;
	return shape_4096;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_transform_4326_to_4096_backup(shape geometry) 
  RETURNS geometry
  LANGUAGE plpgsql
AS $$
DECLARE
	shape_4096	geometry;
BEGIN
	if st_geometrytype(shape) = 'ST_LineString' then
		select st_linefrommultipoint(st_collect(point)) as the_geom
		from
		(
			select path, rdb_move_point_pixel(geom, 14, 4096) as point
			from
			(
				select (st_dumppoints(shape)).*
			)as t
			order by path
		)as t2
		into shape_4096;
	elseif st_geometrytype(shape) = 'ST_Point' then
		shape_4096	:= rdb_move_point_pixel(shape, 14, 4096);
	elsif st_geometrytype(shape) = 'ST_Polygon' then
		select ST_PolygonFromText(st_astext(st_collect(point))) as the_geom
		from
		(
			select path, rdb_move_point_pixel(geom, 14, 4096) as point
			from
			(
				select (st_dumppoints(shape)).*
			)as t
			order by path
		)as t2
		into shape_4096;
	else
		raise Exception 'unexcepted geometry type while transform 4326 to 4096.';
	end if;
	return shape_4096;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_move_point_pixel(point geometry, tz integer, size integer) 
  RETURNS geometry
  LANGUAGE plpgsql
AS $$
DECLARE
	PI			double precision;
	all_size	integer;
	lon			double precision;
	lat			double precision;
	x			double precision;
	y			double precision;
	px			integer;
	py			integer;
BEGIN
	--
    PI = 3.1415926535897931;
	all_size = (1 << tz) * size;
    
	--select st_x(point), st_y(point) into x,y;
	lon = st_x(point);
	lat = st_y(point);
	
	--select (lonlat2pixel_tile(0, 0, 0, x, y, (1<<tz) * size)).* into px,py;
    x = lon / 360.0 + 0.5;
    y = ln(tan(PI / 4.0 + lat * PI / 360.0));
    y = 0.5 - y / 2.0 / PI;
    
    px = round(x * all_size);
    py = round(y * all_size);
	
	--select (pixel2world(0, 0, 0, px, py, (1<<tz) * size)).* into x,y;
	x = px * 1.0 / all_size;
    y = py * 1.0 / all_size;
    
	--select (world2lonlat(x,y)).* into x,y;
	lon = (x - 0.5) * 360.0;
    lat = degrees(2 * atan(exp((1.0 - 2.0 * y) * PI)) - PI / 2.0);
    
	return st_setsrid(st_point(lon,lat), 4326);
END;
$$;

CREATE OR REPLACE FUNCTION rdb_move_point_pixel_backup(point geometry, tz integer, size integer) 
  RETURNS geometry
  LANGUAGE plpgsql
AS $$
DECLARE
	x		double precision;
	y		double precision;
	px		integer;
	py		integer;
BEGIN
	select st_x(point), st_y(point) into x,y;
	select (lonlat2pixel_tile(0, 0, 0, x, y, (1<<tz) * size)).* into px,py;
	select (pixel2world(0, 0, 0, px, py, (1<<tz) * size)).* into x,y;
	select (world2lonlat(x,y)).* into x,y;
	return st_setsrid(st_point(x,y), 4326);
END;
$$;

create or replace function rdb_optimize_find_related_nodes()
returns integer
language plpgsql
as $$
declare
	rec record;
	group_id integer;
	search_link bigint;
	search_node bigint;
	
	link_array bigint[];
	node_array bigint[];
	
	next_link_array bigint[];
	s_node_array bigint[];
	e_node_array bigint[];
begin
	group_id	:= 0;
	
	for rec in 
		select * from temp_optimize_shortlink
	loop
		-- check
		select node_id 
		from temp_optimize_related_nodes
		where node_id = rec.start_node_id
		into search_node;

		if FOUND then
			continue;
		end if;
		
		--
		group_id	:= group_id + 1;

		link_array	:= ARRAY[rec.link_id];
		node_array	:= ARRAY[rec.start_node_id, rec.end_node_id];

		while True loop
			--raise INFO 'search node_array=%', node_array;
			select array_agg(link_id), array_agg(start_node_id), array_agg(end_node_id)
			from
			(
				select link_id, start_node_id, end_node_id
				from temp_optimize_shortlink
				where (start_node_id = ANY(node_array) or end_node_id = ANY(node_array)) and not (link_id = ANY(link_array))
			)as t
			into next_link_array, s_node_array, e_node_array;

			if FOUND and next_link_array is not null then
				--raise INFO 'search next_link_array=%', next_link_array;
				select array_agg(link_id)
				from
				(
					select unnest(link_array) as link_id
					union
					select unnest(next_link_array) as link_id
				)as t
				into link_array;

				select array_agg(node_id)
				from
				(
					select unnest(node_array) as node_id
					union
					select unnest(s_node_array) as node_id
					union
					select unnest(e_node_array) as node_id
				)as t
				into node_array;
			else
				insert into temp_optimize_related_nodes(group_id, node_index, node_id)
				select group_id, node_index, node_array[node_index] as node_id
				from
				(
					select generate_series(1,array_upper(node_array,1)) as node_index
				)as t;
				
				exit;
			end if;
		end loop;
	end loop;
	return 0;
end;
$$;

CREATE OR REPLACE FUNCTION rdb_get_angle_shape_point(shape_points geometry, bStartEnd boolean, INOUT t_node bigint, OUT t_geom geometry)
  LANGUAGE plpgsql
AS $$
DECLARE
	point_count		integer;
	point_start		integer;
	point_end		integer;
	point_step		integer;
	distance		double precision;
	node_point		geometry;
BEGIN
	point_count		:= ST_NPoints(shape_points);
	
    if bStartEnd then
    	point_start	:= 2;
    	point_end	:= point_count;
    	point_step	:= 1;
    	node_point	:= st_startpoint(shape_points);
    else
    	point_start	:= point_count-1;
    	point_end	:= 1;
    	point_step	:= -1;
    	node_point	:= st_endpoint(shape_points);
    end if;
    
    distance	:= 0;
    while true loop
    	t_geom	:= ST_PointN(shape_points, point_start);
    	
    	if point_start = point_end then
    		exit;
    	end if;
    	
    	if st_distance(t_geom, node_point, true) >= 1 then
    		t_node	:= -1;
    		exit;
    	end if;
    	
    	point_start	:= point_start + point_step;
    end loop;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_get_angle_for_geom(point_a geometry, point_b geometry) 
  RETURNS double precision
  LANGUAGE plpgsql
AS $$
DECLARE
	deata_x		double precision;
	deata_y		double precision;
BEGIN
	deata_x		:= st_x(point_b)-st_x(point_a);
	deata_y		:= st_y(point_b)-st_y(point_a);
	if deata_x = 0 and deata_y = 0 then
		return null;
	else
		return (atan2(deata_y, deata_x) * 180.0 / pi());
	end if;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_get_angle_for_4096(px1 integer, py1 integer, px2 integer, py2 integer) 
  RETURNS double precision
  LANGUAGE plpgsql
AS $$
DECLARE
	deata_x		integer;
	deata_y		integer;
BEGIN
	deata_x		:= px2 - px1;
	deata_y		:= py1 - py2;
	if deata_x = 0 and deata_y = 0 then
		return null;
	else
		return (atan2(deata_y, deata_x) * 180.0 / pi());
	end if;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_get_node_index_array(node_array bigint[], ft_node_array bigint[]) 
  RETURNS integer[]
  LANGUAGE plpgsql
AS $$
DECLARE
	nCount			integer;
	nIndex			integer;
	ft_count		integer;
	ft_index		integer;
	ft_index_array	integer[];
BEGIN
	ft_count	:= array_upper(ft_node_array, 1);
	
	nCount		:= array_upper(node_array, 1);
	for nIndex in 1..nCount loop
		if node_array[nIndex] = -1 then
			ft_index_array	:= ft_index_array || -1;
		else
			ft_index	:= 1;
			while True loop
				if node_array[nIndex] = ft_node_array[ft_index] then
					ft_index_array	:= ft_index_array || ft_index;
					exit;
				end if;
				ft_index := ft_index + 1;
				if ft_index > ft_count then 
					ft_index_array	:= ft_index_array || -1;
					exit;
				end if;
			end loop;
		end if;
	end loop;
	
	return ft_index_array;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_get_average(val_array double precision[]) 
  RETURNS double precision
  LANGUAGE plpgsql
AS $$
DECLARE
	average double precision;
BEGIN
	select	val
	from
	(
		select unnest(val_array) as val
	)as t
	where val is null
	into average;
	
	if FOUND then
		average	:= null;
	else
		select avg(val)
		from
		(
			select unnest(val_array) as val
		)as t
		into average;
	end if;
	
	return average;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_get_maximum(val_array double precision[]) 
  RETURNS double precision
  LANGUAGE plpgsql
AS $$
DECLARE
	maximum double precision;
BEGIN
	select	val
	from
	(
		select unnest(val_array) as val
	)as t
	where val is null
	into maximum;
	
	if FOUND then
		maximum	:= null;
	else
		select max(val)
		from
		(
			select unnest(val_array) as val
		)as t
		into maximum;
	end if;
	
	return maximum;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_optimize_get_try_array(
											try_index integer, max_move integer, node_count integer, 
											OUT mx_array integer[], OUT my_array integer[]
											)
  LANGUAGE plpgsql
AS $$
DECLARE
	node_index		integer;
	node_move		integer;
	node_pos_num	integer;
	node_xy_step	integer;
BEGIN
	-- moving possibility
	node_xy_step	:= 2 * max_move + 1;						-- moving possibility of x/y dir
	node_pos_num	:= pow(node_xy_step, 2)::smallint;			-- moving possibility of (x,y)
	
	for node_index in 1..node_count loop
		node_move				:= try_index % node_pos_num;
		mx_array[node_index]	:= (node_move % node_xy_step) - max_move;
		my_array[node_index]	:= (node_move / node_xy_step) - max_move;
		try_index				:= (try_index - node_move) / node_pos_num;
	end loop;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_optimize_check_try_array(
											npx_4096_array integer[], npy_4096_array integer[], 
											mx_array integer[], my_array integer[], 
											npx_min_array integer[], npx_max_array integer[], 
											npy_min_array integer[], npy_max_array integer[]
											)
  RETURNS boolean
  LANGUAGE plpgsql
AS $$
DECLARE
	node_count		smallint;
	node_index		smallint;
BEGIN
	node_count		:= array_upper(npx_4096_array, 1);
	for node_index in 1..node_count loop
		if npx_4096_array[node_index] + mx_array[node_index] < npx_min_array[node_index] then
			return false;
		end if;
		if npx_4096_array[node_index] + mx_array[node_index] > npx_max_array[node_index] then
			return false;
		end if;
		if npy_4096_array[node_index] + my_array[node_index] < npy_min_array[node_index] then
			return false;
		end if;
		if npy_4096_array[node_index] + my_array[node_index] > npy_max_array[node_index] then
			return false;
		end if;
	end loop;
	return True;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_optimize_node_to_best_position() 
  RETURNS integer
  LANGUAGE plpgsql
AS $$
DECLARE
	rec				record;
	try_move		integer;
	avg_move_cost	double precision;
	max_move_cost	double precision;
	node_index		integer;
	node_move		integer;
	node_mx			integer[];
	node_my			integer[];
	node_px			integer;
	node_py			integer;
	x				double precision;
	y				double precision;
BEGIN	
	for rec in
		select 	group_id, 
				node_count, node_array, npx_4096_array, npy_4096_array, 
				npx_min_array, npx_max_array, npy_min_array, npy_max_array, 
				link_count, link_array, tile_array, angle_array, 
				f_index_array, f_node_array, fpx_4096_array, fpy_4096_array, 
				t_index_array, t_node_array, tpx_4096_array, tpy_4096_array
		from temp_optimize_node_group
	loop
		-- calc best position
		select	try_index, avg_cost, max_cost
		from
		(
			select	try_index,
					(
					rdb_calc_move_cost(	try_index, 
										rec.node_count, rec.node_array, rec.angle_array, 
										mx_array, my_array, 
										rec.f_index_array, rec.fpx_4096_array, rec.fpy_4096_array, 
										rec.t_index_array, rec.tpx_4096_array, rec.tpy_4096_array)
					).*	--avg_cost, max_cost
			from
			(
				select	try_index, mx_array, my_array
				from temp_optimize_try_all
				where 	(node_num = rec.node_count) 
						and
						rdb_optimize_check_try_array(	rec.npx_4096_array, rec.npy_4096_array, 
													mx_array, my_array, 
													rec.npx_min_array, rec.npx_max_array, 
													rec.npy_min_array, rec.npy_max_array
						)
			)as t
		)as t
		where max_cost <= 20
		--where move_cost is not null
		order by max_cost asc, avg_cost asc
		limit 1
		into try_move, avg_move_cost, max_move_cost;
		
		-- set new position
		if not FOUND then
			--perform log('node_move', 'warning', 'legal movement does not exist, group_id = ' || rec.group_id);
			raise WARNING 'legal movement does not exist, group_id = %', rec.group_id;
		else
			if max_move_cost >= 10 then
				--perform log('node_move', 'warning', 
				--			'movement cost is too large'
				--			||', group_id = ' || rec.group_id 
				--			||', cost = '|| max_move_cost 
				--			||', nodes = '|| rec.node_array::varchar
				--			);
				raise WARNING 'movement cost is too large, group_id = %, cost = %', rec.group_id, max_move_cost;
			end if;
			
			select mx_array, my_array
			from temp_optimize_try_all
			where (node_num = rec.node_count) and (try_index = try_move)
			into node_mx, node_my;
			
			for node_index in 1..rec.node_count loop
				if node_mx[node_index] = 0 and node_my[node_index] = 0 then
					--pass;
				else
					node_px	:= rec.npx_4096_array[node_index] + node_mx[node_index];
					node_py	:= rec.npy_4096_array[node_index] + node_my[node_index];
					select (pixel2world(0, 0, 0, node_px, node_py, (1<<14) * 4096)).* into x,y;
					select (world2lonlat(x,y)).* into x,y;
					
					insert into temp_optimize_node_new_position
								(node_id, the_geom)
						values  (rec.node_array[node_index], st_setsrid(st_point(x,y), 4326));
				end if;
			end loop;
		end if;
	end loop;
	
	return 0;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_calc_move_cost(	try_move integer, 
												node_count integer, node_array bigint[], angle_array double precision[],
												mx_array integer[], my_array integer[], 
												f_index_array integer[], fpx_4096_array integer[], fpy_4096_array integer[],
												t_index_array integer[], tpx_4096_array integer[], tpy_4096_array integer[],
												OUT avg_cost double precision, OUT max_cost double precision)
  LANGUAGE plpgsql
AS $$
DECLARE
	node_move	smallint;
	node_mx		smallint[];
	node_my		smallint[];
	node_index	smallint;
BEGIN
	select null, null
	from
	(
			select	angle,
					(case when f_index <= 0 then fpx_4096 else fpx_4096 + mx_array[f_index] end) as mx1,
					(case when f_index <= 0 then fpy_4096 else fpy_4096 + my_array[f_index] end) as my1,
					(case when t_index <= 0 then tpx_4096 else tpx_4096 + mx_array[t_index] end) as mx2,
					(case when t_index <= 0 then tpy_4096 else tpy_4096 + my_array[t_index] end) as my2
			from
			(
			select 	unnest(angle_array) as angle,
					unnest(f_index_array) as f_index,
					unnest(fpx_4096_array) as fpx_4096,
					unnest(fpy_4096_array) as fpy_4096,
					unnest(t_index_array) as t_index,
					unnest(tpx_4096_array) as tpx_4096,
					unnest(tpy_4096_array) as tpy_4096
			)as t0
	)as t1
	where mx1 = mx2 and my1 = my2
	into avg_cost, max_cost;
	
	if not FOUND then
		select	avg(angle_diff), max(angle_diff)
		from
		(
			select	--*,
					(
					case 
					when abs(angle - angle_new) < 180 then abs(angle - angle_new) 
					else 360 - abs(angle - angle_new)
					end
					)as angle_diff
			from
			(
				select	*, rdb_get_angle_for_4096(mx1, my1, mx2, my2) as angle_new
				from 
				(
					select	angle,
							(case when f_index <= 0 then fpx_4096 else fpx_4096 + mx_array[f_index] end) as mx1,
							(case when f_index <= 0 then fpy_4096 else fpy_4096 + my_array[f_index] end) as my1,
							(case when t_index <= 0 then tpx_4096 else tpx_4096 + mx_array[t_index] end) as mx2,
							(case when t_index <= 0 then tpy_4096 else tpy_4096 + my_array[t_index] end) as my2
					from
					(
					select 	unnest(angle_array) as angle,
							unnest(f_index_array) as f_index,
							unnest(fpx_4096_array) as fpx_4096,
							unnest(fpy_4096_array) as fpy_4096,
							unnest(t_index_array) as t_index,
							unnest(tpx_4096_array) as tpx_4096,
							unnest(tpy_4096_array) as tpy_4096
					)as t0
				)as t1
			)as t2
		)as t3
		into avg_cost, max_cost;
	end if;
END;
$$;



-------------------------------------------------------------------------
--- For promote some link to region.
-------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION rdb_find_promote_link_connector()
  RETURNS bigint 
  LANGUAGE plpgsql
  AS $$ 
DECLARE
    island_id	 bigint;
	
    nWalkedLink  bigint;
    curs1        refcursor;
    curs2        refcursor;
    rec1         record;
    rec2         record;

    listNode     bigint[];
    nIndex       bigint;
    nCurNodeID   bigint;

    nNextLink    bigint;
    nNextNode    bigint;
BEGIN
	island_id	:= 0;
	
	While True LOOP
		-- find a node as the start point of a new island
		select start_node_id
		from rdb_link as a
		left join temp_region_promote_connector_walked as b
		on a.link_id = b.link_id
		where a.road_type = 3 and b.link_id is null
		limit 1
		into nCurNodeID;
		
		if not FOUND then 
			exit;
		else
			island_id	:= island_id + 1;
		end if;
	
		-- walk around current island
	    nWalkedLink   := 0;
	    nIndex        := 1;
	    listNode[nIndex]:= nCurNodeID;
	    WHILE nIndex > 0 LOOP
	        nCurNodeID    := listNode[nIndex];
	        nIndex        := nIndex - 1;
	
	        open curs1 for 
	            select *
	                     from
	                     (
		                    select link_id as nextlink,
		                           end_node_id as nextnode
		                    from rdb_link
		                    where (road_type = 3 or link_type = 0)
		                    and (start_node_id = nCurNodeID)
	                     ) as a
	                     union
	                     (
		                    select link_id as nextlink,
		                           start_node_id as nextnode
		                    from rdb_link
		                    where (road_type = 3 or link_type = 0)
		                    and (end_node_id = nCurNodeID)
	                     );
	
	        fetch curs1 into rec1;
	        while rec1 is not null loop
	            nNextLink    := rec1.nextlink;
	            nNextNode    := rec1.nextnode;
	
	            open curs2 for
	                select link_id 
	                from temp_region_promote_connector_walked 
	                where link_id = nNextLink
	            ;
	            fetch curs2 into rec2;
	            if rec2 is null then
	                nWalkedLink    := nWalkedLink + 1;
	
	                nIndex        := nIndex + 1;
	                listNode[nIndex]:= nNextNode;
	
	                INSERT INTO temp_region_promote_connector_walked(link_id, island_id) VALUES (nNextLink, island_id);
	            end if;
	            close curs2;
	
	            fetch curs1 into rec1;
	        end loop;
	
	        close curs1;
	    END LOOP;
    END LOOP;
    
    return island_id;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_dispatch_region_abslinkid()
  RETURNS integer 
  LANGUAGE plpgsql
  AS $$ 
DECLARE
    rec						record;
    current_abslinkid	 	integer;
    start_abslinkid			integer;
    abs_diff				smallint;
    bNewDispatch			boolean;
BEGIN
	-- get max abs link id which has not been used yet.
	select max(abs_link_id)
	from rdb_link_abs
	into current_abslinkid;
	
	if current_abslinkid is null then
		current_abslinkid = 1;
	else
		current_abslinkid = current_abslinkid + 1;
	end if;
	
	-- dispatch new abs link id
	for rec in
		select 	link_id as old_link_id, 
				array_upper(abs_linkids, 1) as sub_link_num,
				abs_linkids
		from temp_region_merge_links_oldid
		order by old_link_id
	loop
		if rec.abs_linkids[1] > 0 then
			-- sublink has abs-link-id, that means it exists in higher region.
			-- check
			if 	(0 = ANY(rec.abs_linkids))
				or
				(abs(rec.abs_linkids[rec.sub_link_num] - rec.abs_linkids[1]) != (rec.sub_link_num - 1))
			then
				raise Exception 'region abs-link-ids are not continuous, old_link_id = %', rec.old_link_id;
			end if;
			
			-- no need to dispatch new abs-link-id
			start_abslinkid = rec.abs_linkids[1];
			abs_diff = rec.abs_linkids[rec.sub_link_num] - rec.abs_linkids[1];
			bNewDispatch = false;
		else
			-- sublink does not have abs-link-id
			-- check
			if not (0 = ALL(rec.abs_linkids)) then
				raise Exception 'region abs-link-ids are not continuous, old_link_id = %', rec.old_link_id;
			end if;
			
			-- dispatch new abs-link-id.
			start_abslinkid = current_abslinkid;
			abs_diff = rec.sub_link_num - 1;
			bNewDispatch = true;
			current_abslinkid = current_abslinkid + rec.sub_link_num;
		end if;
		
		insert into temp_region_merge_abslinkid_mapping
					(old_link_id, abs_link_id, abs_link_diff, bNewDispatch)
		values (rec.old_link_id, start_abslinkid, abs_diff, bNewDispatch);
	end loop;
	
	return current_abslinkid;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_find_divided_node()
  RETURNS  integer
  LANGUAGE plpgsql
AS $$ 
DECLARE
	rec               record;
	i                 integer;
	nCount            integer;
	in_count          integer;
	out_count         integer;
	one_way           smallint;
	X                 double precision;
	Y                 double precision;
	polygon           geometry;
	point_array       geometry[];
	distance_x        double precision;  -- 250m -> longitude
	distance_y        double precision;  -- 250m -> latitude
	dis               double precision[];
	PI			      double precision;
	RADIUS_EARTH      integer;
BEGIN
	PI = pi();  -- 3.14159265358979
	RADIUS_EARTH = 6370986;
	distance_y = 45000.0/(PI*RADIUS_EARTH);  -- 0.00224830895535959347341504252442
	for rec in
		select 	node_id,
			(array_agg(the_geom))[1] as the_geom,
			array_agg(s_node) as s_node_array,
			array_agg(e_node) as e_node_array,
			array_agg(one_way_code) as one_way_array
		from
		(
			select a.node_id, a.the_geom, b.s_node, b.e_node, b.one_way_code
			from node_tbl as a
			inner join link_tbl as b
			on b.one_way_code <> 4 and (a.node_id = b.s_node or a.node_id = b.e_node)
		)as t
		group by node_id having count(*) > 2
	loop
		nCount = array_upper(rec.s_node_array, 1);
		in_count = 0;
		out_count = 0;
		for i in 1..nCount
		loop
			one_way = rec.one_way_array[i]; 
			if rec.node_id = rec.e_node_array[i] then
				if one_way = 3 then
					out_count = out_count + 1;
				elsif one_way = 2 then
					in_count = in_count + 1;
				elsif one_way = 1 then
					out_count = out_count + 1;
					in_count = in_count + 1;
				end if;
			else
				if one_way = 3 then
					in_count = in_count + 1;
				elsif one_way = 2 then
					out_count = out_count + 1;
				elsif one_way = 1 then
					out_count = out_count + 1;
					in_count = in_count + 1;
				end if;
			end if;
	    end loop;
	    
	    if out_count >= 2 and in_count >= 1 then
--		    X = ST_X(rec.the_geom);
			Y = ST_Y(rec.the_geom);
			distance_x = 45000.0/(PI*RADIUS_EARTH*cos(radians(Y)));
			
			point_array[1] = ST_Translate(rec.the_geom, -distance_x,  distance_y);
			point_array[2] = ST_Translate(rec.the_geom,  distance_x,  distance_y);
			point_array[3] = ST_Translate(rec.the_geom,  distance_x, -distance_y);
			point_array[4] = ST_Translate(rec.the_geom, -distance_x, -distance_y);
			point_array[5] = ST_Translate(rec.the_geom, -distance_x,  distance_y);
			
			
--			dis[1] = ST_Distance_Sphere(ST_SetSRID(point_array[1], 4326), rec.the_geom);
--			dis[2] = ST_Distance_Sphere(ST_SetSRID(point_array[2], 4326), rec.the_geom);
--			dis[3] = ST_Distance_Sphere(ST_SetSRID(point_array[3], 4326), rec.the_geom);
--			dis[4] = ST_Distance_Sphere(ST_SetSRID(point_array[4], 4326), rec.the_geom);
--			dis[5] = ST_Distance_Sphere(ST_SetSRID(point_array[5], 4326), rec.the_geom);
			polygon = ST_SetSRID(ST_MakePolygon(ST_MakeLine(point_array)), 4326);
			INSERT INTO temp_guidence_node
    		VALUES 
    		(rec.node_id, rec.the_geom, polygon); --
		end if;	
	end loop;
                                            
	RETURN 0;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_cal_forecast_time(free_time integer, time_list_array text[])
  RETURNS integer[]
  LANGUAGE plpgsql VOLATILE
AS $$ 
DECLARE
	i		integer;
	j		integer;
	len		integer;
	num		integer;
	
	cur_time_list		character varying;	
	next_time_list		character varying;		
	time_array		character varying[];

	slot_seq		int;
	next_time		int;
	time_dst		int;
	time_dst_array		int[];

BEGIN
	i := 1;

	--- loop.for different parts of the same link, combine time in the same time slot, when its time exceeds 5min, fetch the next time slot's time.
	time_dst_array := '{}';
	
	len := array_upper(time_list_array,1);
	for i in 1..len loop
		cur_time_list := time_list_array[i];
		
		time_array := string_to_array(cur_time_list,'|');

		time_dst := 0;

		num := array_upper(time_array,1);
		for j in 1..num loop	
-- 			raise info '-----seq:%',j;

			slot_seq := time_dst / 300;
-- 				raise info '	cur_time:%,slot_seq:%',cast(time_array[j] as int),slot_seq;
			next_time_list := time_list_array[i + slot_seq];
			if slot_seq > 0 and (string_to_array(next_time_list,'|'))[j] is not null then
				next_time := (string_to_array(next_time_list,'|'))[j]::int;
-- 					raise info '		add next slot time:%',(string_to_array(next_time_list,'|'))[j]::int;	
			else
				if time_array[j] is not null then
					next_time := time_array[j]::int;
				else 
					time_dst := free_time;
				end if;
-- 					raise info '		add cur slot time:%',time_array[j]::int;
			end if;

			time_dst := time_dst + next_time;

			j := j + 1;

		end loop;

		--- get time array for current time_slot
		if round(time_dst - free_time) >= 0 then 
			time_dst_array := array_append(time_dst_array,round(time_dst - free_time)::int);
		else 
			time_dst_array := array[NULL];
		end if;
		
		i := i + 1;
	end loop;


	return time_dst_array;	
END;
$$;

CREATE OR REPLACE FUNCTION rdb_locate_to_tile(	point_geom geometry, z integer,
												out tx integer, out ty integer)
  LANGUAGE plpgsql
AS $$ 
DECLARE
	 PI double precision;
     x double precision;
     y double precision;
     lon double precision;
     lat double precision;
BEGIN
	PI := 3.1415926535897931;
	
	lon = ST_X(point_geom);
	lat = ST_Y(point_geom);
	
	x = lon / 360. + 0.5;
	y = ln(tan(PI / 4. + radians(lat) / 2.));
	y = 0.5 - y / 2. / PI;

	tx = cast(floor(x * (1<<z)) as integer);
	ty = cast(floor(y * (1<<z)) as integer);
END;
$$;

create or replace function rdb_get_meshid_by_zlevel(tx_16 int, ty_16 int, z_level int, tile_offset int)
	returns bigint
	LANGUAGE plpgsql
AS $$ 
DECLARE
	target_tile_x int;
	target_tile_y int;
	target_tile_id bigint;
BEGIN
	target_tile_x := (tx_16 + tile_offset) >> (16 - z_level);
	target_tile_y := (ty_16 + tile_offset) >> (16 - z_level);
	
	target_tile_id := (z_level::bigint << 32) | (target_tile_x << 16) | target_tile_y;
	
	return target_tile_id;
END;
$$;
