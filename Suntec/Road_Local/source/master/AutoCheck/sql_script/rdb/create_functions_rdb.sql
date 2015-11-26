-------------------------------------------------------------------------------------------------------------
-- check logic
-------------------------------------------------------------------------------------------------------------

CREATE OR REPLACE FUNCTION rdb_check_regulation_item()
	RETURNS integer
	LANGUAGE plpgsql
	AS $$ 
DECLARE
	rec				record;
	nIndex			integer;
	nCount			integer;
BEGIN
	for rec in
		select *
		from
		(
			select	record_no, 
					array_agg(link_id) as link_array, 
					array_agg(seq_num) as seq_array,
					array_agg(one_way) as oneway_array,
					array_agg(start_node_id) as snode_array,
					array_agg(end_node_id) as enode_array
			from
			(
				select record_no, m.link_id, seq_num, n.one_way, n.start_node_id, n.end_node_id
				from
				(
					select record_no, cast(link_array[seq_num] as bigint) as link_id, seq_num
					from
					(
						select record_no, link_array, generate_series(1,array_upper(link_array,1)) as seq_num
						from
						(
							select record_no, regexp_split_to_array(key_string, E'\\,+') as link_array
							from rdb_link_regulation
						)as a
						where (array_upper(link_array,1) > 2) or 
							  (array_upper(link_array,1) = 2 and link_array[1] != link_array[2])
					)as b
				)as m
				left join rdb_link as n
				on m.link_id = n.link_id
				order by record_no, seq_num
			)as a
			group by record_no
		)as r
	loop
		-- check linkrow continuable
		nCount		:= array_upper(rec.seq_array, 1);
		nIndex		:= 1;
		while nIndex < nCount loop
			if rec.snode_array[nIndex] in (rec.snode_array[nIndex+1], rec.enode_array[nIndex+1]) then
				if rec.oneway_array[nIndex] in (2,4) then
					raise WARNING 'regulation linkrow error: traffic flow error, record_no = %', rec.record_no;
				end if;
			elseif rec.enode_array[nIndex] in (rec.snode_array[nIndex+1], rec.enode_array[nIndex+1]) then
				if rec.oneway_array[nIndex] in (3,4) then
					raise WARNING 'regulation linkrow error: traffic flow error, record_no = %', rec.record_no;
				end if;
			else
				raise EXCEPTION 'regulation linkrow error: links have no intersection, record_no = %', rec.record_no;
			end if;
			
			nIndex		:= nIndex + 1;
		end loop;
	end loop;
	
	return 1;
END;
$$;


CREATE OR REPLACE FUNCTION rdb_check_regulation_condition()
	RETURNS integer
	LANGUAGE plpgsql
	AS $$ 
DECLARE
	rec				record;
	month			integer;
	day				integer;
	hour			integer;
	minute			integer;
BEGIN
	for rec in
		select * from rdb_link_cond_regulation
	loop
		-- check start_date
		month	:= rec.start_date / 100;
		day		:= rec.start_date % 100;
		if month = 2 then
			if not (day >= 0 and day <= 29) then
				raise EXCEPTION 'regulation condition error: start_date format is uncorrect, regulation_id = %', rec.regulation_id;
			end if;
		elseif month in (0,1,3,5,7,8,10,12) then
			if not (day >= 0 and day <= 31) then
				raise EXCEPTION 'regulation condition error: start_date format is uncorrect, regulation_id = %', rec.regulation_id;
			end if;
		elseif month in (4,6,9,11) then
			if not (day >= 0 and day <= 30) then
				raise EXCEPTION 'regulation condition error: start_date format is uncorrect, regulation_id = %', rec.regulation_id;
			end if;
		end if;
		
		-- check end_date
		month	:= rec.end_date / 100;
		day		:= rec.end_date % 100;
		if month = 2 then
			if not (day >= 0 and day <= 29) then
				raise EXCEPTION 'regulation condition error: end_date format is uncorrect, regulation_id = %', rec.regulation_id;
			end if;
		elseif month in (0,1,3,5,7,8,10,12) then
			if not (day >= 0 and day <= 31) then
				raise EXCEPTION 'regulation condition error: end_date format is uncorrect, regulation_id = %', rec.regulation_id;
			end if;
		elseif month in (4,6,9,11) then
			if not (day >= 0 and day <= 30) then
				raise EXCEPTION 'regulation condition error: end_date format is uncorrect, regulation_id = %', rec.regulation_id;
			end if;
		end if;
		
		-- check start_time
		if rec.start_time != 2400 then
			hour	:= rec.start_time / 100;
			minute	:= rec.start_time % 100;
			if (hour < 0 or hour >= 24) or (minute < 0 or minute >= 60) then
				raise EXCEPTION 'regulation condition error: start_time format is uncorrect, regulation_id = %', rec.regulation_id;
			end if;
		end if;
		
		-- check end_time
		if rec.end_time != 2400 then
			hour	:= rec.end_time / 100;
			minute	:= rec.end_time % 100;
			if (hour < 0 or hour >= 24) or (minute < 0 or minute >= 60) then
				raise EXCEPTION 'regulation condition error: end_time format is uncorrect, regulation_id = %', rec.regulation_id;
			end if;
		end if;
	
	end loop;
	
	return 1;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_check_regulation_flag()
	RETURNS integer
	LANGUAGE plpgsql
	AS $$ 
DECLARE
	rec				record;
BEGIN
	for rec in
		select *
		from
		(
			select a.link_id, a.regulation_flag, b.last_link_id
			from rdb_link as a
			left join
			(
				select distinct last_link_id 
				from rdb_link_regulation
			)as b
			on a.link_id = b.last_link_id
		)as c
		where (regulation_flag is true and last_link_id is null) or
			  (regulation_flag is false and last_link_id is not null)
	loop
		raise EXCEPTION 'regulation_flag error: link_id = %', rec.link_id;
	end loop;
	
	return 1;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_check_admin_zone()
	RETURNS integer
	LANGUAGE plpgsql
	AS $$ 
DECLARE
	gid_exist			integer;
BEGIN
	select a.gid
	from (select * from rdb_tile_admin_zone where ad_code != -1)as a
	left join rdb_admin_zone as b
	on a.ad_code = b.ad_code
	where b.ad_code is null
	into gid_exist;
	
	if FOUND then
		raise EXCEPTION 'rdb_tile_admin_zone ad_code error: gid = %', gid_exist;
	end if;
	
	return 1;
END;
$$;



CREATE OR REPLACE FUNCTION rdb_reconvert_keystring(keystring varchar)
    RETURNS varchar
    LANGUAGE plpgsql
AS $$
DECLARE
	new_keystring	varchar;
	nLength			integer;
	sub_keystring	varchar;
	array_tile_link varchar[];
	tile_link		bigint[];
	nCount			integer;
	nIndex			integer;
BEGIN
	nLength			:= length(keystring);
	sub_keystring	:= substr(keystring, 3, nLength-4);
	array_tile_link	:= regexp_split_to_array(sub_keystring, E'\\]\\,\\[');
	nCount			:= array_upper(array_tile_link, 1);
	nIndex			:= 1;
	
	new_keystring	:= '';
	while nIndex <= nCount loop
		if nIndex > 1 then
			new_keystring	:= new_keystring || ',';
		end if;
		
		tile_link	:= cast(regexp_split_to_array(array_tile_link[nIndex], E',') as bigint[]);
		new_keystring	:= new_keystring || ((tile_link[1] << 32) | tile_link[2]);
		
		nIndex	:= nIndex + 1;
	end loop;
	
    RETURN new_keystring;
END;
$$;


CREATE OR REPLACE FUNCTION CheckFunctionCodeValid()
  RETURNS integer 
  LANGUAGE plpgsql VOLATILE
AS $$ 
DECLARE
	functioncode smallint;
    curs1 refcursor;
	i smallint;
	
BEGIN    
	
	i=0;
	OPEN curs1 FOR select distinct function_code from rdb_link order by function_code;
  
    FETCH curs1 INTO functioncode;
    while functioncode is not NULL LOOP
		
		if functioncode in (1,2,3,4,5) then
				i = i + 1;
		end if;
	
	
		FETCH curs1 INTO functioncode;
    END LOOP;
	
	if i = 5 then
		return 1;
	else
		return 0;
	end if;
	
    close curs1;

END;
$$;

CREATE OR REPLACE FUNCTION CheckTheGeom()
  RETURNS integer 
  LANGUAGE plpgsql VOLATILE
AS $$ 
DECLARE
    curs1 refcursor;
	linkid bigint;
	startnodeid bigint;
	endnodeid bigint;
	linkgeom geometry;
	startnodegeom geometry;
	endnodegeom geometry;
	startgeom  geometry;
	endgeom geometry;
	num smallint;
	
BEGIN    
	
	OPEN curs1 FOR select link.link_id,link.start_node_id,link.end_node_id,
					link.the_geom as link_geom,
					node1.the_geom as start_node_geom,
					node2.the_geom as end_node_geom
					from rdb_link as link,rdb_node as node1,rdb_node as node2 
					where link.start_node_id = node1.node_id and link.end_node_id =node2.node_id;
  
    FETCH curs1 INTO linkid,startnodeid,endnodeid,linkgeom,startnodegeom,endnodegeom;
	
    while linkid is not NULL LOOP
    		
		num := ST_NPoints(linkgeom);
		
		startgeom := ST_PointN(linkgeom, 1);
		endgeom := ST_PointN(linkgeom, num);

		if ST_Equals(startgeom,startnodegeom) and ST_Equals(endgeom,endnodegeom) then 
		
		else
		
			raise exception  'link geometry error: link_id = % ', linkid;
			
		end if;
	
	
		FETCH curs1 INTO linkid,startnodeid,endnodeid,linkgeom,startnodegeom,endnodegeom;
    END LOOP;
	
	return 1;
    close curs1;

END;
$$;


CREATE OR REPLACE FUNCTION CheckToll_ETC()
  RETURNS integer 
  LANGUAGE plpgsql VOLATILE
AS $$ 
DECLARE
    nodeid bigint;
    flag integer;
    curs1 refcursor;
    curs2 refcursor;
    rec2 record;
    temp_flag smallint;
    temp_flag2 smallint;
    links bigint[];
    sort_links bigint[];
    len smallint;
    i smallint;
    
BEGIN   

    OPEN curs1 FOR SELECT node_id, extend_flag, branches FROM rdb_node;
  
    FETCH curs1 INTO nodeid,flag,links;
    while nodeid is not NULL LOOP

		temp_flag := flag >> 14;
		temp_flag := temp_flag & 1; 
				
		if temp_flag = 1 then 

			temp_flag2 := flag >> 14;
			temp_flag2 := temp_flag2 & 1; 
						
			if temp_flag2 = 1 then
			
				len := array_upper(links,1);
				sort_links := array(SELECT unnest(links) AS x ORDER BY x);
				i = 1;
				while i <= len loop

					open curs2 for execute
						'SELECT etc_lane_flag as etc_flag
						  From rdb_link_add_info where link_id = ' || @sort_links[i] ;
					
					fetch curs2 into rec2;
					if rec2.etc_flag <> 1 then
						raise exception 'extend_flag of node (Toll_ETC) error: node_id = %, extend_flag = %, not all lane are ETC lane.',nodeid,flag;
					end if;
					
					close curs2;
					
					i=i+1;
				end loop;
			
			else
			
				raise exception 'extend_flag of node (Toll_ETC) error: node_id = %, extend_flag = %',nodeid,flag;

			end if;


			
		end if;

        FETCH curs1 INTO nodeid,flag,links;
    END LOOP;
	
    return 1;
    close curs1;

END;
$$;


CREATE OR REPLACE FUNCTION CheckToll_toll()
  RETURNS integer 
  LANGUAGE plpgsql VOLATILE
AS $$ 
DECLARE
    nodeid bigint;
    flag integer;
    curs1 refcursor;
    temp_flag smallint;
    count_flag numeric;
    count_all numeric;
BEGIN    
    count_flag = 0;
    count_all = 0;
    OPEN curs1 FOR SELECT node_id, extend_flag FROM rdb_node;
  
    FETCH curs1 INTO nodeid, flag;
    while nodeid is not NULL LOOP
    
		count_all = count_all + 1;
		temp_flag := flag >> 10;
		temp_flag := temp_flag & 1; 

		if temp_flag = 1 then 
			count_flag = count_flag + 1;
		end if;

        FETCH curs1 INTO nodeid, flag;
    END LOOP;

	if count_flag = 0 or count_flag/count_all >= 0.01 then
		raise exception 'extend_flag of node (Toll station) error: all count: %, Toll station: %',count_all,count_flag;
	else
		return 1;
	end if;
	
    close curs1;

END;
$$;


CREATE OR REPLACE FUNCTION CheckRamp()
	RETURNS smallint
	LANGUAGE plpgsql volatile
AS $$
DECLARE
	rec record;
BEGIN
	for rec in
		select link_id, start_node_id, end_node_id, one_way, road_type, function_code
		from rdb_link
		where road_type in (0,1) and link_type in (1,2,7)
	loop
		if rec.one_way in (1,2) then
			perform check_ramp_atnode(rec.end_node_id, rec.road_type, rec.function_code, 1);
			perform check_ramp_atnode(rec.start_node_id, rec.road_type, rec.function_code, 2);
		end if;

		if rec.one_way in (1,3) then
			perform check_ramp_atnode(rec.start_node_id, rec.road_type, rec.function_code, 1);
			perform check_ramp_atnode(rec.end_node_id, rec.road_type, rec.function_code, 2);
		end if;
	end loop;
	RETURN 1;
END;
$$;

CREATE OR REPLACE FUNCTION check_ramp_atnode(nHwyNode bigint, nRoadTypeA integer, nFunctionClassA smallint, dir integer)
	RETURNS smallint
	LANGUAGE plpgsql volatile
AS $$
DECLARE
	rec        			record;
	rec2				record;
	nRoadType			smallint;
	nFunctionClass		smallint;
	rstPath				varchar;
	tmpPath				varchar;
	
	tmpPathArray		varchar[];
	tmpLastNodeArray	bigint[];
	tmpPathCount		integer;
	tmpPathIndex		integer;
	
	nIndex				integer;
	nCount				integer;
	link_array 			bigint[];
BEGIN	
	--rstPathArray
	tmpPathArray		:= ARRAY[null];
	tmpLastNodeArray	:= ARRAY[nHwyNode];
	tmpPathCount		:= 1;
	tmpPathIndex		:= 1;
	
	-- search
	WHILE tmpPathIndex <= tmpPathCount LOOP
		
		for rec in
			select	(case when start_node_id = tmpLastNodeArray[tmpPathIndex] then end_node_id else start_node_id end) as nextnode, 
					link_id as nextlink, 
					link_type, road_type, function_code
			from rdb_link
			where	(
						(dir = 1)
						and
						(
							(start_node_id = tmpLastNodeArray[tmpPathIndex] and one_way in (1,2))
							or
							(end_node_id = tmpLastNodeArray[tmpPathIndex] and one_way in (1,3))
						)
						and road_type in (0, 1, 2, 3, 4, 5, 6)
					)
					or
					(
						(dir = 2)
						and
						(
							(start_node_id = tmpLastNodeArray[tmpPathIndex] and one_way in (1,3))
							or
							(end_node_id = tmpLastNodeArray[tmpPathIndex] and one_way in (1,2))
						)
						and road_type in (0, 1, 2, 3, 4, 5, 6)
					)
		loop
			---raise INFO 'nextnode = %', rec.nextnode;
			tmpPath		:= tmpPathArray[tmpPathIndex];
			
			-- find a complete path
			if (cast(rec.nextlink as varchar) = ANY(regexp_split_to_array(tmpPath, E'\\|+'))) then
				continue;
				
			elseif rec.link_type in (3,5)  then
				--raise info 'main node:%',nHwyNode;
				--raise info 'IC:link_id=%,function_code=%,link_type=%',cast(rec.nextlink as varchar),rec.function_code,rec.link_type;

				if rec.road_type not in (0,1) then
				
					raise exception 'ramp of link (JCT/IC) error: link_id=%,road_type=%,function_code=%,link_type=%',cast(rec.nextlink as varchar),rec.road_type,rec.function_code,rec.link_type;
				end if;
				
				tmpPathCount		:= tmpPathCount + 1;
				tmpLastNodeArray	:= array_append(tmpLastNodeArray, cast(rec.nextnode as bigint));
				if tmpPath is null then
					tmpPathArray	:= array_append(tmpPathArray, cast(rec.nextlink as varchar));
				else
					tmpPathArray	:= array_append(tmpPathArray, cast(tmpPath||'|'||rec.nextlink as varchar));
				end if;

			end if;
		end loop;
		tmpPathIndex := tmpPathIndex + 1;
	END LOOP;

	return 1;
END;
$$;
       
CREATE OR REPLACE FUNCTION byte2int(geom_blob bytea,out strx smallint,out stry smallint)
  RETURNS record
  LANGUAGE plpgsql VOLATILE
AS $$
DECLARE

	geom_bytea varchar;
	bitx int[];
	i smallint;

BEGIN

	geom_bytea := cast(geom_blob as varchar);
	strx := 0;
	stry := 0;
	i := 3;
	while i >= 0 loop
	
		bitx[i] := get_byte(geom_bytea::bytea, i);

		if (i >= 0 and i <= 1) then
			strx := strx | (bitx[i] << ( i * 8 ));
		end if;
		
		if (i > 1 and i <= 3) then
			stry := stry | (bitx[i] << ((i - 2) * 8 ));
		end if;
		
		i := i -1;
	end loop;

	
END;
$$;
 
                  
CREATE OR REPLACE FUNCTION CheckGeomBlob(link_id bigint,link_id_t integer,geom_blob bytea,the_geom geometry)
  RETURNS int
  LANGUAGE plpgsql VOLATILE
AS $$
DECLARE
	tx integer;
	ty integer;
	len integer;
	str_num integer;
	point_num integer;
	
	geom_bytea varchar;  
	i smallint;
	bitx int[];   
	strx int[];   
	stry int[];   
	x int;   
	y int;   
	seq smallint;
 	len_str int;   
	flag smallint; 
	record_lonlat record;
	lon_out float;
	lat_out float;
BEGIN
	tx := (link_id_t >> 14) & 16383;
	ty := link_id_t & 16383;
	len := length(geom_blob);
	str_num := length(geom_blob)/4;
	point_num := st_npoints(the_geom);

	len_str := 0;
	i  := 0;
	geom_bytea := cast(geom_blob as varchar);  
	
	strx := '{}';
	stry := '{}';
	flag := 0;
	seq := 0;
	if str_num = point_num then     

		while i < len loop    
			
			strx[seq] := get_byte(geom_bytea::bytea, i) | (get_byte(geom_bytea::bytea, i+1) << 8);
			stry[seq] := get_byte(geom_bytea::bytea, i+2) | (get_byte(geom_bytea::bytea, i+3) << 8);

			if stry[seq] < 0 or stry[seq] > 4096 or strx[seq] < 0 or strx[seq] > 4096 then
				raise exception 'geom_blob of link_client check error: shapepoint out of range [ 0 - 4096 ].    link_id:%,link_id_t:%',link_id,link_id_t;
			end if; 

			record_lonlat := pixel_tile2lonlat(14::smallint,((link_id_t >> 14) & 16383), (link_id_t & 16383),strx[seq]::smallint,stry[seq]::smallint,4096::smallint);
			lon_out := record_lonlat.lon;
			lat_out := record_lonlat.lat;											

			if     (lon_out - st_x(ST_PointN(the_geom,i/4+1))) > 9.5e-6 
				or (lon_out - st_x(ST_PointN(the_geom,i/4+1))) < -9.5e-6 
				or (lat_out - st_y(ST_PointN(the_geom,i/4+1))) > 9.5e-6 
				or (lat_out - st_y(ST_PointN(the_geom,i/4+1))) < -9.5e-6 
			then
				raise exception '4096 error!    link_id:%,link_id_t:%',link_id,link_id_t;

			end if;
			
			if seq = 0 then
				x := strx[seq];
				y := stry[seq];
			else
				if strx[seq] = x and stry[seq] = y then 
					flag := flag + 1;
				end if;
				
			end if;
			
			i := i + 4;
			seq := seq + 1;
		end loop; 

		if flag = point_num - 1 then
			raise exception 'shapepoints overlays!    link_id:%,link_id_t:%',link_id,link_id_t;
		end if;
					
	elseif str_num <> point_num then  
		raise exception 'geom_blob of link_client check error: number of blob not equal to number of shapepoints.    link_id:%,link_id_t:%',link_id,link_id_t;
					
	end if;

	return 1;
END;
$$;



CREATE OR REPLACE FUNCTION pixel_tile2lonlat(tz smallint, tx int, ty int, x smallint,  y smallint,  tile_pixel smallint, out lon float, out lat float) RETURNS record
  LANGUAGE plpgsql
AS $$
DECLARE
	lleft float;
	bottom float;
	rright float;
	top float;
	box record;
	pixel record;
	lonlat record;
	lon_world float;
	lat_world float;
BEGIN 

    box := tile_bbox_check(tz, tx, ty);
    lleft := box.lleft;
    bottom := box.bottom;
    rright := box.rright;
    top := box.top;
    
    pixel := pixel2lonlat_check(x, y, lleft, top, rright, bottom, tile_pixel, tile_pixel);

    lon_world := pixel.lon;
    lat_world := pixel.lat;

     lonlat :=  world2lonlat_check(lon_world,lat_world);
     lon := lonlat.lon;
     lat := lonlat.lat;

END;
$$;


CREATE OR REPLACE FUNCTION pixel2lonlat_check(x smallint, y smallint, lleft float,top float, rright float,bottom float, width smallint, height smallint, out lon float,out lat float) RETURNS record
  LANGUAGE plpgsql
AS $$
DECLARE
	world1 record;
	world2 record;
	world3 record;
	w float;
	h float;
	wx float;
	wy float;

BEGIN 

	lon := ((rright - lleft) / 4096 )  * x + lleft;
	lat := ((bottom - top ) / 4096 )  * y + top;
	
END;
$$;


CREATE OR REPLACE FUNCTION tile_bbox_check(z smallint,x int,y int,out lleft float, out bottom float, out rright float, out top float) RETURNS record
  LANGUAGE plpgsql
AS $$
DECLARE
	world1 record;
	x1 float;
	y1 float;
	lonlat1 record;
	world2 record;
	x2 float;
	y2 float;
	lonlat2 record;
BEGIN 

    world1 := pixel2world_check(z, x, y, 0, 1, 1);
    lleft := world1.wx;
    bottom := world1.wy;

    world2 := pixel2world_check(z, x, y, 1, 0, 1);
    rright := world2.wx;
    top  := world2.wy;
    
END;
$$;


CREATE OR REPLACE FUNCTION world2lonlat_check(x float,y float, out lon float, out lat float) RETURNS record
  LANGUAGE plpgsql
AS $$
DECLARE
	PI double precision;
BEGIN 
    PI          := 3.1415926535897931;
    lon := (x - 0.5) * 360.;
    lat := degrees(2 * atan(exp((1. - 2. * y) * PI)) - PI / 2.);
    
END;
$$;



CREATE OR REPLACE FUNCTION lonlat2world_check(lon float, lat float, out x float, out y float) RETURNS record
  LANGUAGE plpgsql
AS $$
DECLARE
	PI double precision;
BEGIN 

    PI          := 3.1415926535897931;
    x := lon / 360. + 0.5;
    y := ln(tan(PI / 4. + radians(lat)/ 2.));
    y := 0.5 - y / 2. / PI;

END;
$$;


CREATE OR REPLACE FUNCTION pixel2world_check(tz smallint, tx int, ty int, px int, py int, size int ,out wx float, out wy float) RETURNS record
  LANGUAGE plpgsql
AS $$
DECLARE
	
BEGIN 

    wx := (tx * size + px) / (2 ^ tz * size);
    wy := (ty * size + py) / (2 ^ tz * size);
    
END;
$$; 

-------------------------------------------------------------------------------------------------------------
-- checkd mapping links
-------------------------------------------------------------------------------------------------------------
create or replace function rdb_check_mapping_link_Continuous(link_id_14 bigint[])
  	RETURNS boolean
	LANGUAGE plpgsql
	AS $$ 
DECLARE
	rec                record;
	i                  integer;
	num                integer;
	prev_one_way       integer;
	prev_road_type     integer;
	prev_start_node    bigint;
	prev_end_node      bigint;
	
	next_one_way       integer;
	next_road_type     integer;
	next_start_node    bigint;
	next_end_node      bigint;
	
	node_array        bigint[];
BEGIN
	if link_id_14 is null then 
		return false;
	end if;
	
	num = array_upper(link_id_14, 1);
	if num < 2 then 
		return true;
	end if;

	SELECT start_node_id, end_node_id, road_type, one_way
	       into prev_start_node, prev_end_node, prev_road_type, prev_one_way
	  FROM rdb_link
	  where link_id = link_id_14[1];
	  
	i = 2;
	while i <= num loop
		SELECT start_node_id, end_node_id, road_type, one_way
		       into next_start_node, next_end_node, next_road_type, next_one_way
		  FROM rdb_link
		  where link_id = link_id_14[i] and
		       (start_node_id in (prev_start_node, prev_end_node) or end_node_id in (prev_start_node, prev_end_node));

		 if not found then 
			raise info 'prev_link: %, next_link: %', link_id_14[i-1], link_id_14[i];
			return false;
		 end if;
		-- road type
		if prev_road_type <> next_road_type then 
			raise info 'prev_link: %, prev_road_type: %, next_link: %, next_road_type: %',
			             link_id_14[i-1], prev_road_type, link_id_14[i], next_road_type;
			             
			return false;
		end if;

		if prev_start_node = next_start_node then
			if prev_one_way = 1 and next_one_way <> 1 then
				raise info '1';
				return false;
			end if;
			
			if prev_one_way = 2 and next_one_way <> 3 then 
				raise info '2';
				return false;
			end if;
			
			if prev_one_way = 3 and next_one_way <> 2 then 
				raise info '3';
				return false;
			end if;

			if i = 2 then 
				node_array = array[prev_end_node];
				node_array = node_array || array[prev_start_node];
			end if;
			node_array = node_array || array[next_end_node];
		end if;
			
		if prev_start_node = next_end_node then
			if prev_one_way <> next_one_way then 
				raise info '4';
				return false;
			end if;
			
			if i = 2 then 
				node_array = array[prev_end_node];
				node_array = node_array || array[prev_start_node];
			end if;
			node_array = node_array || array[next_start_node];
		end if;

		if prev_end_node = next_end_node then
			if prev_one_way = 1 and next_one_way <> 1 then 
				raise info '5';
				return false;
			end if;
			
			if prev_one_way = 2 and next_one_way <> 3 then 
				raise info '6';
				return false;
			end if;
			
			if prev_one_way = 3 and next_one_way <> 2 then
				raise info '7';
				return false;
			end if;
			if i = 2 then 
				node_array = array[prev_start_node];
				node_array = node_array || array[prev_end_node];
			end if;
			node_array = node_array || array[next_start_node];
		end if;

		if prev_end_node = next_start_node then
			if prev_one_way <> next_one_way then 
				raise info '8';
				return false;
			end if;
			
			if i = 2 then 
				node_array = array[prev_start_node];
				node_array = node_array || array[prev_end_node];
			end if;
			node_array = node_array || array[next_end_node];
		end if;

		prev_one_way     = next_one_way;
		prev_road_type   = next_road_type;
		prev_start_node  = next_start_node;
		prev_end_node    = next_end_node;
		i                = i + 1;
	end loop;

	-- is Circle
	PERFORM node_id
	 from (
		(SELECT unnest(node_array) AS node_id)
	 ) as a
	 group by node_id
	 having count(node_id) > 1;
	 
	if found then
		if node_array[1] = node_array[array_upper(node_array, 1)] then
			raise info 'Ring: %', node_array;
			return true;
		else	
			raise info '9: %', node_array; 
			return true;
		End if;
		
	end if;
	
	return true;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_check_guide_links(link bigint, search_node bigint, passlink_cnt int, out_link_id bigint, dir smallint)
	RETURNS boolean
	LANGUAGE plpgsql volatile
AS $$
DECLARE

	rec        			record;
	rstPath				varchar;
	tmpPath				varchar;
	bRoadEnd			boolean;	
	tmpPathArray		varchar[];
	tmpLastNodeArray	bigint[];
	tmpPathCount		integer;
	tmpPathIndex		integer;
	
	link_array 			bigint[];
	reach_link          bigint;
	reach_node			bigint;
	flag 				boolean;
	pass_array_len		smallint;
BEGIN	

	tmpPathArray		:= ARRAY[link];
	tmpLastNodeArray	:= ARRAY[search_node];
	tmpPathCount		:= 1;
	tmpPathIndex		:= 1;

	reach_node	:= search_node;
	reach_link      := link;

	flag := false;
	
	if dir = 0 then 
-- 		raise info 'unexpected direction! may be wrong node!';
 		return false;
	end if;
	
	if out_link_id is null then 
 		return true;
	end if;
	
	WHILE tmpPathIndex <= tmpPathCount LOOP
		bRoadEnd	:= True;

--  		raise INFO '   tmpPathIndex = %', tmpPathIndex;
-- 		raise INFO '   tmpPathArray = %', tmpPathArray;
-- 		raise INFO '   tmpLastNodeArray = %',tmpLastNodeArray;
		for rec in 
			select link_id as nextlink 
				,(case  when start_node_id = tmpLastNodeArray[tmpPathIndex] then end_node_id 
					when end_node_id = tmpLastNodeArray[tmpPathIndex] then start_node_id
				end) as nextnode
			from  rdb_link a 
			where 	(		
					(tmpLastNodeArray[tmpPathIndex] = start_node_id) 
					or	
					(tmpLastNodeArray[tmpPathIndex] = end_node_id)   
				)
				and 
				not (
					cast(link_id as varchar) = ANY(tmpPathArray)
				)	
		loop
-- 			raise info '         reach_link:%,find_link:%',reach_link, rec.nextlink;
			bRoadEnd	:= False;

-- 			raise info 'tmpPathIndex:%,tmpPathCount:%',tmpPathIndex,tmpPathCount;
			tmpPath		:= tmpPathArray[tmpPathIndex];
			if rec.nextlink = out_link_id then
--   				raise info '!!!find: out link:%',rec.nextlink;
				flag := true;
				return flag;
			else
				reach_link	:= rec.nextlink;
				reach_node	:= rec.nextnode;					

				if tmpPathCount >= 32767 then
					raise info 'in_link_id:%,out_link_id:%',link,out_link_id;
					return false;
				else
					tmpPathCount		:= tmpPathCount + 1;
				end if;
				
				tmpLastNodeArray	:= array_append(tmpLastNodeArray, cast(rec.nextnode as bigint));
				tmpPathArray	:= array_append(tmpPathArray, cast(rec.nextlink as varchar));
			end if;
		end loop;

		if bRoadEnd then
			rstPath		:= tmpPathArray[tmpPathIndex];
			if rstPath is not null then
				flag := false;
			end if;
		end if;
		
		tmpPathIndex := tmpPathIndex + 1;
	END LOOP;

	return flag;
END;
$$;
CREATE OR REPLACE FUNCTION get_tow_link_angle(in_angle smallint,out_angle smallint)
  RETURNS double precision 
LANGUAGE plpgsql volatile
AS $$
DECLARE
	in_angle_degree double precision;
	out_angle_degree double precision;
	angle   double precision;
BEGIN	
	in_angle_degree = (in_angle + 32768) * 360.0 / 65536.0;
        out_angle_degree = (out_angle + 32768) * 360.0 / 65536.0;
        if out_angle_degree > in_angle_degree then
		angle = out_angle_degree - in_angle_degree;
	else
		angle = in_angle_degree - out_angle_degree;
	end if;
        if angle > 180 then
		angle = 360 - angle;
        end if;
	return angle;
END;
$$;
CREATE OR REPLACE FUNCTION check_guide_lane_straight_dir()
  RETURNS integer 
LANGUAGE plpgsql volatile
AS $$
DECLARE
	rec			    record;
	straight_angle              double precision;
	in_angle                    smallint;
	out_angle                   smallint;
	i                           integer;
	out_tazms_len               integer;
	one_flag                    integer;
BEGIN	
	for rec in 
		select in_fazm,in_tazm,out_fazms,out_tazms,node_id,in_snode,out_snodes,arrows,in_link_id
		from(
			select in_fazm,in_tazm,array_agg(out_fazm) as out_fazms,array_agg(out_tazm) as out_tazms,node_id,in_snode,array_agg(out_snode) as out_snodes,array_agg(arrow_info) as arrows,in_link_id
			from(
				select b.fazm as in_fazm,b.tazm as in_tazm,c.fazm as out_fazm,c.tazm as out_tazm,node_id,b.start_node_id as in_snode,c.start_node_id as out_snode,arrow_info,in_link_id
				from(
					SELECT  in_link_id, node_id,out_link_id,arrow_info
					FROM rdb_guideinfo_lane
					where passlink_count = 0
				) as a
				left join rdb_link as b
				on a.in_link_id = b.link_id
				left join rdb_link as c
				on a.out_link_id = c.link_id
				order by in_link_id,arrow_info
			) as a
			group by in_link_id, in_fazm, in_tazm, node_id, in_snode
			
		) as a
		where array_upper(arrows,1) > 1 and 1 = any(arrows)
	LOOP	
		i = 1;
		one_flag = 1;
		out_tazms_len = array_upper(rec.out_tazms,1);
		while one_flag <= out_tazms_len loop
			if (rec.arrows)[one_flag] = 1 then
				exit;
			end if;
			one_flag = one_flag + 1;
		end loop;
		if rec.in_snode = rec.node_id then
			in_angle = rec.in_fazm;
		else
			in_angle = rec.in_tazm;
		end if;
		if (rec.out_snodes)[one_flag] = rec.node_id then
			out_angle = (rec.out_fazms)[one_flag];
		else
			out_angle = (rec.out_tazms)[one_flag];
		end if;
		straight_angle = get_tow_link_angle(in_angle,out_angle);
		--raise info 'in_link_id=%',rec.in_link_id;
		--raise info 'straight=%',straight_angle;
		--raise info 'straight=%',rec.out_fazms;
		--raise info 'arrows=%',rec.arrows;
		--raise info 'straight=%',(rec.out_fazms)[1];
		--raise info 'in_angle=%,out_angle=%',in_angle,out_angle;
		while i <= out_tazms_len loop
			if i <> one_flag then
				if (rec.out_snodes)[i] = rec.node_id then
					out_angle = (rec.out_fazms)[i];
				else
					out_angle = (rec.out_tazms)[i];
				end if;
				if rec.arrows[i] <> 1 then
					if straight_angle < get_tow_link_angle(in_angle,out_angle) then
						raise info 'straight=%,other=%,one_flag=%',straight_angle,get_tow_link_angle(in_angle,out_angle),one_flag;
						return 0;
					end if;
				end if;
			end if;
			i = i + 1;
		end loop;
	
	end loop;
	return 1;
END;
$$;

-------------------------------------------------------------------------------
-- 
-------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION test_exist_branch_in_hwy_mapping(sNode bigint,
                                                            eNode bigint,
                                                            nOneWay smallint)
  RETURNS boolean
  LANGUAGE plpgsql
  AS $$
BEGIN
	PERFORM  a.link_id
	  FROM (
		SELECT link_id 
		  FROM rdb_link
		  WHERE -- in link 
			((nOneWay = 2 and -- Positive direction
			 ((start_node_id = sNode and one_way = 3)
			   or (end_node_id = sNode and one_way = 2)))
			or 
			(nOneWay = 3 and -- Reverse direction
			 ((start_node_id = eNode and one_way = 3)
			   or (end_node_id = eNode and one_way = 2))))
			or 
			-- out link 
			((nOneWay = 2 and -- Positive direction
			 ((start_node_id = eNode and one_way = 2)
			   or (end_node_id = eNode and one_way = 3)))
			or 
			 (nOneWay = 3 and -- Reverse direction
			  ((start_node_id = sNode and one_way = 2)
			    or (end_node_id = eNode and one_way = 3))))
	  ) AS a
	  INNER JOIN rdb_highway_mapping AS b
	  ON a.link_id = b.link_id;

	if found then
		return TRUE;
	end if;
	return FALSE;	
END;
$$;

-------------------------------------------------------------------------------
-- the_geom 2 lon_lat
-------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION test_check_geom_2_lonlat(the_geom geometry)
  RETURNS bigint 
  LANGUAGE plpgsql
AS $$ 
DECLARE
        x   bigint;
        y   bigint;
BEGIN
	x = round(st_x(the_geom) * 3600 * 256);
        y = round(st_y(the_geom) * 3600 * 256);
	RETURN x << 32 | y;
END;
$$;


CREATE OR REPLACE FUNCTION rdb_check_admin_summer_time()
	RETURNS integer
	LANGUAGE plpgsql
	AS $$ 
DECLARE
	rec				record;
	month			integer;
	day				integer;
	hour			integer;
	minute			integer;
BEGIN
	for rec in
		select * from rdb_admin_summer_time
	loop
		-- check start_date
		month	:= rec.start_date / 100;
		day		:= rec.start_date % 100;
		if month = 2 then
			if not (day >= 0 and day <= 29) then
				raise EXCEPTION 'summer time error: start_date format is uncorrect, summer_time_id = %', rec.summer_time_id;
			end if;
		elseif month in (0,1,3,5,7,8,10,12) then
			if not (day >= 0 and day <= 31) then
				raise EXCEPTION 'summer time error: start_date format is uncorrect, summer_time_id = %', rec.summer_time_id;
			end if;
		elseif month in (4,6,9,11) then
			if not (day >= 0 and day <= 30) then
				raise EXCEPTION 'summer time error: start_date format is uncorrect, summer_time_id = %', rec.summer_time_id;
			end if;
		end if;
		
		-- check end_date
		month	:= rec.end_date / 100;
		day		:= rec.end_date % 100;
		if month = 2 then
			if not (day >= 0 and day <= 29) then
				raise EXCEPTION 'summer time error: end_date format is uncorrect, summer_time_id = %', rec.summer_time_id;
			end if;
		elseif month in (0,1,3,5,7,8,10,12) then
			if not (day >= 0 and day <= 31) then
				raise EXCEPTION 'summer time error: end_date format is uncorrect, summer_time_id = %', rec.summer_time_id;
			end if;
		elseif month in (4,6,9,11) then
			if not (day >= 0 and day <= 30) then
				raise EXCEPTION 'summer time error: end_date format is uncorrect, summer_time_id = %', rec.summer_time_id;
			end if;
		end if;
		
		-- check start_time
		if rec.start_time != 2400 then
			hour	:= rec.start_time / 100;
			minute	:= rec.start_time % 100;
			if (hour < 0 or hour >= 24) or (minute < 0 or minute >= 60) then
				raise EXCEPTION 'summer time error: start_time format is uncorrect, summer_time_id = %', rec.summer_time_id;
			end if;
		end if;
		
		-- check end_time
		if rec.end_time != 2400 then
			hour	:= rec.end_time / 100;
			minute	:= rec.end_time % 100;
			if (hour < 0 or hour >= 24) or (minute < 0 or minute >= 60) then
				raise EXCEPTION 'summer time error: end_time format is uncorrect, summer_time_id = %', rec.summer_time_id;
			end if;
		end if;
		
		-- check start_weekday
		if rec.start_weekday > 0 then
			if 	(rec.start_weekday & 127) not in (1,2,4,8,16,32,64) 
				or
				(rec.start_weekday >> 7) not in (1,2,4,8,16,32) 
			then
				raise EXCEPTION 'summer time error: start_weekday format is uncorrect, summer_time_id = %', rec.summer_time_id;
			end if;
		end if;
		
		-- check end_weekday
		if rec.end_weekday > 0 then
			if 	(rec.end_weekday & 127) not in (1,2,4,8,16,32,64)
				or
				(rec.end_weekday >> 7) not in (1,2,4,8,16,32)
			then
				raise EXCEPTION 'summer time error: end_weekday format is uncorrect, summer_time_id = %', rec.summer_time_id;
			end if;
		end if;
	
	end loop;
	
	return 1;
END;
$$;


CREATE OR REPLACE FUNCTION rdb_check_guideinfo_condition()
	RETURNS integer
	LANGUAGE plpgsql
	AS $$ 
DECLARE
	rec				record;
	month			integer;
	day				integer;
	hour			integer;
	minute			integer;
BEGIN
	for rec in
		select * from rdb_guideinfo_condition
	loop
		-- check start_date
		month	:= rec.start_date / 100;
		day		:= rec.start_date % 100;
		if month = 2 then
			if not (day >= 0 and day <= 29) then
				raise EXCEPTION 'summer time error: start_date format is uncorrect, summer_time_id = %', rec.summer_time_id;
			end if;
		elseif month in (0,1,3,5,7,8,10,12) then
			if not (day >= 0 and day <= 31) then
				raise EXCEPTION 'summer time error: start_date format is uncorrect, summer_time_id = %', rec.summer_time_id;
			end if;
		elseif month in (4,6,9,11) then
			if not (day >= 0 and day <= 30) then
				raise EXCEPTION 'summer time error: start_date format is uncorrect, summer_time_id = %', rec.summer_time_id;
			end if;
		end if;
		
		-- check end_date
		month	:= rec.end_date / 100;
		day		:= rec.end_date % 100;
		if month = 2 then
			if not (day >= 0 and day <= 29) then
				raise EXCEPTION 'summer time error: end_date format is uncorrect, summer_time_id = %', rec.summer_time_id;
			end if;
		elseif month in (0,1,3,5,7,8,10,12) then
			if not (day >= 0 and day <= 31) then
				raise EXCEPTION 'summer time error: end_date format is uncorrect, summer_time_id = %', rec.summer_time_id;
			end if;
		elseif month in (4,6,9,11) then
			if not (day >= 0 and day <= 30) then
				raise EXCEPTION 'summer time error: end_date format is uncorrect, summer_time_id = %', rec.summer_time_id;
			end if;
		end if;
		
		-- check start_time
		if rec.start_time != 2400 then
			hour	:= rec.start_time / 100;
			minute	:= rec.start_time % 100;
			if (hour < 0 or hour >= 24) or (minute < 0 or minute >= 60) then
				raise EXCEPTION 'summer time error: start_time format is uncorrect, summer_time_id = %', rec.summer_time_id;
			end if;
		end if;
		
		-- check end_time
		if rec.end_time != 2400 then
			hour	:= rec.end_time / 100;
			minute	:= rec.end_time % 100;
			if (hour < 0 or hour >= 24) or (minute < 0 or minute >= 60) then
				raise EXCEPTION 'summer time error: end_time format is uncorrect, summer_time_id = %', rec.summer_time_id;
			end if;
		end if;
	end loop;
	
	return 1;
END;
$$;



CREATE OR REPLACE FUNCTION mid_get_fraction(link_geom geometry, node_geom geometry)
  RETURNS float
  LANGUAGE plpgsql
AS $$
DECLARE
	sub_string		geometry;
	sum_len			float;
	sub_len			float;
	fraction		float;
BEGIN
	fraction	:= st_line_locate_point(link_geom, node_geom);

	sub_string	:= ST_Line_Substring(link_geom,0,fraction);

	sum_len		:= ST_Length_Spheroid(link_geom,'SPHEROID("WGS_84", 6378137, 298.257223563)');
	sub_len		:= ST_Length_Spheroid(sub_string,'SPHEROID("WGS_84", 6378137, 298.257223563)');
	
	return sub_len/sum_len;
END;
$$;


CREATE OR REPLACE FUNCTION rdb_check_region_linkrow_passable(region_link_id bigint,
								                             region_one_way smallint,
								                             region_s_node bigint,
								                             region_e_node bigint,
								                             link_array bigint[],
								                             link_dir_array boolean[],
								                             one_way_array smallint[],
								                             s_node_array bigint[],
								                             e_node_array bigint[])
	RETURNS boolean
	LANGUAGE plpgsql
	AS $$ 
DECLARE
	rec				record;
	nIndex			integer;
	nCount			integer;
	
	nNextNode		bigint;
BEGIN
	-- check linkrow continuable
	nCount	:= array_upper(link_array, 1);
	nIndex	:= 1;
	
	nNextNode	= region_s_node;
	while nIndex <= nCount loop
		if s_node_array[nIndex] = nNextNode then
			if 	(
					link_dir_array[nIndex] is True
				)
				and
				(
					(region_one_way = 1 and one_way_array[nIndex] = 1)
					or
					(region_one_way = 2 and one_way_array[nIndex] = 2)
					or
					(region_one_way = 3 and one_way_array[nIndex] = 3)
				)
			then
				nNextNode	= e_node_array[nIndex];
			else
				--raise EXCEPTION 'region linkrow error1: region_link_id = %', region_link_id;
				return False;
			end if;
		elseif e_node_array[nIndex] = nNextNode then
			if 	(
					link_dir_array[nIndex] is False
				)
				and
				(
					(region_one_way = 1 and one_way_array[nIndex] = 1)
					or
					(region_one_way = 2 and one_way_array[nIndex] = 3)
					or
					(region_one_way = 3 and one_way_array[nIndex] = 2)
				)
			then
				nNextNode	= s_node_array[nIndex];
			else
				--raise EXCEPTION 'region linkrow error2: region_link_id = %', region_link_id;
				return False;
			end if;
		else
			--raise EXCEPTION 'region linkrow error3: region_link_id = %', region_link_id;
			return False;
		end if;
		
		nIndex	:= nIndex + 1;
	end loop;

	return True;
END;
$$;

CREATE OR REPLACE FUNCTION find_isolated_road_network_from_tbl(isolated_tbl varchar)
  RETURNS bigint 
  LANGUAGE plpgsql
  AS $$ 
DECLARE
	island_id    bigint;
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
		SELECT s_node
		FROM temp_links_to_search
		LIMIT 1
		INTO nCurNodeID;
		
		IF not FOUND THEN 
			exit;
		ELSE
			island_id	:= island_id + 1;
		END IF;
		
		-- walk around current island
		nWalkedLink      := 0;
		nIndex           := 1;
		listNode[nIndex] := nCurNodeID;
		WHILE nIndex > 0 LOOP
			nCurNodeID    := listNode[nIndex];
			nIndex        := nIndex - 1;
			--RAISE INFO '%, %, %', nCurNodeID, island_id, nWalkedLink;
	
			open curs1 for 
				SELECT link_id as nextlink, e_node as nextnode
				FROM temp_base_link_tbl
				WHERE (s_node = nCurNodeID and one_way in (1,2,3))

				UNION

				SELECT link_id as nextlink, s_node as nextnode
				FROM temp_base_link_tbl
				WHERE (e_node = nCurNodeID and one_way in (1,2,3))
			;
	
			FETCH curs1 INTO rec1;
			WHILE rec1 is not null LOOP
				nNextLink    := rec1.nextlink;
				nNextNode    := rec1.nextnode;
	
				open curs2 for
					execute 'SELECT link_id FROM ' || isolated_tbl || ' WHERE link_id = ' || nNextLink
				;
				FETCH curs2 INTO rec2;
				IF rec2 is null THEN
					nWalkedLink    := nWalkedLink + 1;
	
					nIndex        := nIndex + 1;
					listNode[nIndex]:= nNextNode;

					EXECUTE 'INSERT INTO ' ||isolated_tbl|| '(link_id, island_id) VALUES (' || nNextLink || ', ' || island_id || ')';
					DELETE FROM temp_links_to_search WHERE link_id = nNextLink;
					
				END IF;
				CLOSE curs2;
				FETCH curs1 INTO rec1;
			END LOOP;
			CLOSE curs1;
		END LOOP;
	END LOOP;
    
    return island_id;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_check_linkrow_connectivity_by_traffic_flow()
  RETURNS smallint
  LANGUAGE plpgsql
AS $$
DECLARE
	rec		       record;
	link_loop      integer;
	search_node    bigint;
BEGIN
	FOR rec in 
		SELECT record_no, link_num, array_agg(link_id) as link_array, array_agg(start_node_id) as s_node_array, array_agg(end_node_id) as e_node_array, array_agg(one_way) as one_way_array
		FROM (
			SELECT record_no, link_num, b.link_id, seq_num, start_node_id, end_node_id, one_way
			FROM (
				SELECT record_no, link_num, unnest(link_array) as link_id, generate_series(1, link_num) as seq_num
				FROM (
					SELECT record_no, link_num, string_to_array(key_string, ',')::bigint[] as link_array
					FROM rdb_link_regulation
				) a
				ORDER BY record_no, seq_num
			) b
			LEFT JOIN rdb_link c
				ON b.link_id = c.link_id
		) d
		GROUP BY record_no, link_num
		ORDER BY record_no
	LOOP
		--raise info 'rec = %', rec;
		IF rec.link_num = 1 THEN
			IF rec.one_way_array[1] = 0 THEN
				raise EXCEPTION 'regulation linkrow traffic flow error, record_no = %', rec.record_no;
			END IF;
			
			continue;
		END IF;
		
		IF rec.s_node_array[1] in (rec.s_node_array[2], rec.e_node_array[2]) THEN
			search_node := rec.s_node_array[1];
		ELSE
			search_node := rec.e_node_array[1];
		END IF;

		--raise info 'search_node = %', search_node;
		IF (search_node = rec.s_node_array[1] and rec.one_way_array[1] not in (1, 3)) or (search_node = rec.e_node_array[1] and rec.one_way_array[1] not in (1, 2)) THEN
			raise EXCEPTION 'regulation linkrow traffic flow error, record_no = %', rec.record_no;
		END IF;
		
		FOR link_loop IN 2..rec.link_num LOOP
			IF (search_node = rec.s_node_array[link_loop] and rec.one_way_array[link_loop] not in (1, 2)) or (search_node = rec.e_node_array[link_loop] and rec.one_way_array[link_loop] not in (1, 3)) THEN
				raise EXCEPTION 'regulation linkrow traffic flow error, record_no = %', rec.record_no;
			END IF;
			
			IF search_node = rec.s_node_array[link_loop] THEN
				search_node := rec.e_node_array[link_loop];
			ELSE
				search_node := rec.s_node_array[link_loop];
			END IF;
		END LOOP;
	END LOOP;
	
	return 1;
END;
$$;