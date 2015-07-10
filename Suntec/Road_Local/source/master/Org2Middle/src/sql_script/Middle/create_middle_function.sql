
-------------------------------------------------------------------------------------------------------------
-- common function
-------------------------------------------------------------------------------------------------------------

CREATE OR REPLACE FUNCTION mid_cnv_ramp_backup()
	RETURNS smallint
	LANGUAGE plpgsql volatile
AS $$
DECLARE
	cur1 refcursor;
	cur2 refcursor;
	cur3 refcursor;
	rec1 record;
	rec2 record;
	e bigint[];
	leng integer;
	i integer;
	idx integer;
	cnode bigint;
	csnodes bigint[];
	cenodes bigint[];
	links bigint[];
	iclinks bigint[];
	cid bigint;
	crt smallint;
	cfc smallint;
	cnode_id bigint;
	cdir smallint;
	node1 bigint;
	snode bigint;
	enode bigint;
	dir bigint;
	flag boolean;
BEGIN

	open cur1 for
		select link_id, s_node, e_node, one_way_code, road_type, function_class from link_tbl
		where road_type in (0,1) and link_type in (1,2)
		;
	
	fetch cur1 into rec1;
	while rec1.link_id is not null loop

		csnodes := e;
		cenodes := e;
		links := e;
		iclinks := e;
		
		crt := rec1.road_type;
		cfc := rec1.function_class;

		if rec1.one_way_code in (1,2) then
			csnodes := array_append(csnodes, rec1.e_node);
			cenodes := array_append(cenodes, rec1.s_node);
		end if;

		if rec1.one_way_code in (1,3) then
			csnodes := array_append(csnodes, rec1.s_node);
			cenodes := array_append(cenodes, rec1.e_node);
		end if;

		i := 1;
		while i <= array_length(csnodes,1) loop
			cnode := csnodes[i];

			open cur2 for 
				select link_id, s_node, e_node, link_type, one_way_code, road_type, function_class
				from link_tbl
				where ((s_node = cnode and one_way_code in (1,2)) or (e_node = cnode and one_way_code in (1,3)))
				and link_type in (3, 5, 7)
				and road_type in (0, 1, 2, 3, 4, 5, 6);
		
			fetch cur2 into rec2;
			while rec2.link_id is not null loop
	
				if rec2.link_id = any(links) then
					fetch cur2 into rec2;
					continue;
				end if;
	
				if rec2.link_type = 3 then
					crt := rec2.road_type;
					cfc := rec2.function_class;
				end if;
	
				if rec2.link_type = 5 then
					iclinks := array_append(iclinks, rec2.link_id);
				end if;
	
				links := array_append(links, rec2.link_id);
				
				if rec2.s_node = cnode then
					csnodes := array_append(csnodes, rec2.e_node);
				else
					csnodes := array_append(csnodes, rec2.s_node);
				end if;
	
				fetch cur2 into rec2;
			end loop;
			close cur2;
	
		i := i + 1;
		end loop;

		i := 1;
		while i <= array_length(cenodes,1) loop
			cnode := cenodes[i];
	
			open cur2 for 
				select link_id, s_node, e_node, link_type, one_way_code, road_type, function_class 
				from link_tbl
				where ((s_node = cnode and one_way_code in (1,3)) or (e_node = cnode and one_way_code in (1,2)))
				and link_type in (3, 5, 7)
				and road_type in (0, 1, 2, 3, 4, 5, 6);
	
			fetch cur2 into rec2;
			while rec2.link_id is not null loop
	
				if rec2.link_id = any(links) then
					fetch cur2 into rec2;
					continue;
				end if;
		
				if rec2.link_type = 3 then
					crt := rec2.road_type;
					cfc := rec2.function_class;
				end if;
		
				if rec2.link_type = 5 then
					iclinks := array_append(iclinks, rec2.link_id);
				end if;
		
				links := array_append(links, rec2.link_id);
				
				if rec2.s_node = cnode then
					cenodes := array_append(cenodes, rec2.e_node);
				else
					cenodes := array_append(cenodes, rec2.s_node);
				end if;
		
				fetch cur2 into rec2;
			end loop;
			close cur2;
	
			i := i + 1;
		end loop;

		leng := array_length(iclinks, 1);
		if leng is null then
			leng := 0;
		end if;
	
		for idx in 1..leng loop
			select link_id from temp_link_ramp where link_id = iclinks[idx] into cid;
			if cid is null then
				insert into temp_link_ramp(link_id, new_road_type, new_fc) 
							values(iclinks[idx], crt, cfc); 
			end if;
		end loop;

		fetch cur1 into rec1;

	end loop;
	close cur1;

	RETURN 1;
END;
$$;


CREATE OR REPLACE FUNCTION mid_cnv_ramp()
	RETURNS smallint
	LANGUAGE plpgsql volatile
AS $$
DECLARE
	rec record;
BEGIN
	for rec in
		select link_id, s_node, e_node, one_way_code, road_type, function_class
		from link_tbl
		where road_type in (0,1) and link_type in (1,2,7)
	loop
		if rec.one_way_code in (1,2) then
			perform mid_cnv_ramp_atnode(rec.e_node, rec.road_type, rec.function_class, 1);
			perform mid_cnv_ramp_atnode(rec.s_node, rec.road_type, rec.function_class, 2);
		end if;

		if rec.one_way_code in (1,3) then
			perform mid_cnv_ramp_atnode(rec.s_node, rec.road_type, rec.function_class, 1);
			perform mid_cnv_ramp_atnode(rec.e_node, rec.road_type, rec.function_class, 2);
		end if;
	end loop;
	
    insert into temp_link_ramp
                (link_id, new_road_type, new_fc)
    (
        select link_id, max(new_road_type), min(new_fc)
        from temp_link_ramp_single_path
        group by link_id
    );
	
	RETURN 1;
END;
$$;

CREATE OR REPLACE FUNCTION mid_cnv_ramp_atnode(nHwyNode bigint, nRoadTypeA integer, nFunctionClassA smallint, dir integer)
	RETURNS smallint
	LANGUAGE plpgsql volatile
AS $$
DECLARE
	rec        			record;
	bRoadEnd			boolean;
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
	count                           integer;

BEGIN	
	--rstPathArray
	tmpPathArray		:= ARRAY[null];
	tmpLastNodeArray	:= ARRAY[nHwyNode];
	tmpPathCount		:= 1;
	tmpPathIndex		:= 1;
	
	-- search
	WHILE tmpPathIndex <= tmpPathCount LOOP
		
		-- set road end flag, which means current link is road end point
		bRoadEnd	:= True;
		
		--raise INFO 'tmpPathArray = %', cast(tmpPathArray as varchar);

		for rec in
			select	(case when s_node = tmpLastNodeArray[tmpPathIndex] then e_node else s_node end) as nextnode, 
					link_id as nextlink, 
					link_type, road_type, function_class
			from link_tbl
			where	(
						(
							(dir = 1)
							and
							(
								(s_node = tmpLastNodeArray[tmpPathIndex] and one_way_code in (1,2))
								or
								(e_node = tmpLastNodeArray[tmpPathIndex] and one_way_code in (1,3))
							)
						)
						or
						(
							(dir = 2)
							and
							(
								(s_node = tmpLastNodeArray[tmpPathIndex] and one_way_code in (1,3))
								or
								(e_node = tmpLastNodeArray[tmpPathIndex] and one_way_code in (1,2))
							)
						)
					)
					and 
					(
						tmpPathArray[tmpPathIndex] is null 
						or 
						not (cast(link_id as varchar) = ANY(regexp_split_to_array(tmpPathArray[tmpPathIndex], E'\\|+')))
					)
		loop
			-- set road end flag, current link is road end point
			bRoadEnd	:= False;
			if rec.link_type in (0,3,5) then
				insert into temp_update_ramp_link_node(link_id,node_id)
				values (rec.nextlink,rec.nextnode);
			end if;
			--raise INFO 'nextnode = %', rec.nextnode;
			tmpPath		:= tmpPathArray[tmpPathIndex];
			
			-- no proper connected link, here is a complete path
			if rec.link_type not in (0,3, 5) or rec.road_type not in (0, 1, 2, 3, 4, 5, 6, 14) then
				if tmpPath is not null then
					rstPath			:= tmpPath;

					if rec.road_type < nRoadTypeA then
						nRoadType	:= rec.road_type;
						nFunctionClass	:= rec.function_class;
					else
						nRoadType	:= nRoadTypeA;
						nFunctionClass	:= nFunctionClassA;
					end if;

					link_array	:= cast(regexp_split_to_array(rstPath, E'\\|+') as bigint[]);
					nCount		:= array_upper(link_array, 1);
					for nIndex in 1..nCount loop
						insert into temp_link_ramp_single_path(link_id, new_road_type, new_fc) 
							values(link_array[nIndex], nRoadType, nFunctionClass);
					end loop;
				end if;
				continue;
			
			-- find an available link, record it as temp path and go on searching
			else
				tmpPathCount		:= tmpPathCount + 1;
				tmpLastNodeArray	:= array_append(tmpLastNodeArray, cast(rec.nextnode as bigint));
				if tmpPath is null then
					tmpPathArray	:= array_append(tmpPathArray, cast(rec.nextlink as varchar));
				else
					tmpPathArray	:= array_append(tmpPathArray, cast(tmpPath||'|'||rec.nextlink as varchar));
				end if;
			end if;
		end loop;

		
		-- road end point, here is a complete path
		if bRoadEnd then
			rstPath		:= tmpPathArray[tmpPathIndex];
			if rstPath is not null then
				link_array	:= cast(regexp_split_to_array(rstPath, E'\\|+') as bigint[]);
				nCount		:= array_upper(link_array, 1);
				for nIndex in 1..nCount loop
					insert into temp_link_ramp_single_path(link_id, new_road_type, new_fc) 
						values(link_array[nIndex], nRoadTypeA, nFunctionClassA);
				end loop;
			end if;
		end if;
		
		tmpPathIndex := tmpPathIndex + 1;
		tmpPath		:= tmpPathArray[tmpPathIndex];
	END LOOP;

	return 1;
END;
$$;

CREATE OR REPLACE FUNCTION mid_cnv_ramp_toohigh()
	RETURNS smallint
	LANGUAGE plpgsql volatile
AS $$
DECLARE
	rec record;
BEGIN
	for rec in
		select a.link_id, a.s_node, a.e_node, a.one_way_code, a.road_type, a.function_class 
		from link_tbl a 
		left join temp_update_ramp_link_node b on a.link_id = b.link_id
	    left join temp_update_ramp_link_node c on a.one_way_code=2 and a.s_node=c.node_id or a.one_way_code=3 and a.e_node=c.node_id or (a.one_way_code=1 and
		(a.s_node=c.node_id or a.e_node=c.node_id)) 
        where a.link_type in (3,5) and a.road_type in (0,1) and a.one_way_code<>4
        and b.link_id is null and c.node_id is null 
		
	loop
		if rec.one_way_code in (1,2) then
			perform mid_cnv_ramp_toohigh_search(rec.e_node, rec.link_id, rec.road_type, rec.function_class, 1);
			perform mid_cnv_ramp_toohigh_search(rec.s_node, rec.link_id, rec.road_type, rec.function_class, 2);
		end if;

		if rec.one_way_code in (1,3) then
			perform mid_cnv_ramp_toohigh_search(rec.s_node, rec.link_id, rec.road_type, rec.function_class, 1);
			perform mid_cnv_ramp_toohigh_search(rec.e_node, rec.link_id, rec.road_type, rec.function_class, 2);
		end if;
	end loop;
	
    insert into temp_link_ramp_toohigh
                (link_id, new_road_type, new_fc)
    (
        select link_id, min(new_road_type), max(new_fc)
        from temp_link_ramp_toohigh_single_path
        group by link_id
        
    );
	
	RETURN 1;
END;
$$;

CREATE OR REPLACE FUNCTION mid_cnv_ramp_toohigh_search(nodeid bigint, linkid bigint , roadtype integer, functionclass smallint, dir integer)
	RETURNS smallint
	LANGUAGE plpgsql volatile
AS $$
DECLARE
	rec        			record;
	bRoadEnd			boolean;
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
	count                           integer;

BEGIN	
	--rstPathArray
	tmpPathArray		:= ARRAY[null];
	tmpLastNodeArray	:= ARRAY[nodeid];
	tmpPathCount		:= 1;
	tmpPathIndex		:= 1;
	
	-- search
	WHILE tmpPathIndex <= tmpPathCount LOOP
		
		-- set road end flag, which means current link is road end point
		bRoadEnd	:= True;
		
		--raise INFO 'tmpPathArray = %', cast(tmpPathArray as varchar);
				
		for rec in
			select	(case when s_node = tmpLastNodeArray[tmpPathIndex] then e_node else s_node end) as nextnode, 
					link_id as nextlink, 
					link_type, road_type, function_class
			from link_tbl
			where	(
						(
							(dir = 1)
							and
							(
								(s_node = tmpLastNodeArray[tmpPathIndex] and one_way_code in (1,2))
								or
								(e_node = tmpLastNodeArray[tmpPathIndex] and one_way_code in (1,3))
							)
						)
						or
						(
							(dir = 2)
							and
							(
								(s_node = tmpLastNodeArray[tmpPathIndex] and one_way_code in (1,3))
								or
								(e_node = tmpLastNodeArray[tmpPathIndex] and one_way_code in (1,2))
							)
						)
					)
					and 
					(
						tmpPathArray[tmpPathIndex] is null 
						or 
						not (cast(link_id as varchar) = ANY(regexp_split_to_array(tmpPathArray[tmpPathIndex], E'\\|+')))
					)
		loop
			-- set road end flag, current link is road end point
			bRoadEnd	:= False;
			
			raise INFO 'nextnode = %,nextlink = %,link_type= %,road_type= %', rec.nextnode,rec.nextlink,rec.link_type,rec.road_type;
			tmpPath		:= tmpPathArray[tmpPathIndex];
			
			-- no proper connected link, here is a complete path
			if rec.link_type not in (3, 5) then
					if rec.road_type > roadtype then
						nRoadType	:= rec.road_type;
					else
						nRoadType	:= roadtype;
					end if;
					
					if rec.function_class<functionclass then
						nFunctionClass	:= rec.function_class;
					else
						nFunctionClass	:= functionclass;
					end if;
					insert into temp_link_ramp_toohigh_single_path(link_id, new_road_type, new_fc) 
						values(linkid, nRoadType, nFunctionClass);
					
				continue;
			
			-- find an available link, record it as temp path and go on searching
			else
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
		tmpPath		:= tmpPathArray[tmpPathIndex];
	END LOOP;

	return 1;
END;
$$;


CREATE OR REPLACE FUNCTION mid_search_linkrow(target_tile_id integer)
	RETURNS smallint
	LANGUAGE plpgsql volatile
AS $$
DECLARE
	rec					temp_merge_link_suspect;
	rec2				record;
	pos_rec				record;
	neg_rec				record;
	dir_search_e		integer;
	dir_search_s		integer;
	start_node			bigint;
	end_node			bigint;
	link_array			bigint[];
	linkdir_array		boolean[];
	nCount				integer;
	nIndex				integer;
BEGIN
	for rec in
		select	link_id, tile_id, s_node, e_node, one_way_code, road_name, road_number, 
				link_type, road_type, toll, length, function_class, display_class,
				elevated, structure, tunnel, rail_cross, paved, uturn, disobey_flag, etc_only_flag, 
				extend_flag, bypass_flag, matching_flag, highcost_flag, ipd, urban, erp, rodizio, 
				lane_num_s2e, lane_num_e2s, width_s2e, width_e2s, 
				speed_limit_s2e, speed_limit_e2s, speed_source_s2e, speed_source_e2s,
				park_link_connect_type,park_floor,park_link_lean,park_link_toll,park_region_id
		from temp_merge_link_suspect
		where tile_id = target_tile_id
		order by tile_id,length desc
	loop
		-- mark walked_flag of current link
		select * 
		from temp_merge_link_walked
		where link_id = rec.link_id
		into rec2;
		if found then
			continue;
		else
			insert into temp_merge_link_walked ("link_id") values (rec.link_id);
		end if;
		
		-- search in positive and negative digitized direction
		--raise INFO 'link_id = %', rec.link_id;
		
		pos_rec			:= mid_search_linkrow_in_one_direction(rec, rec.s_node, rec.e_node, 2);
		end_node		:= pos_rec.reach_node;
		
		neg_rec			:= mid_search_linkrow_in_one_direction(rec, end_node, rec.s_node, 3);
		start_node		:= neg_rec.reach_node;
		
		link_array		:= neg_rec.linkrow || ARRAY[rec.link_id] || pos_rec.linkrow;
		linkdir_array	:= neg_rec.linkdirrow || ARRAY[True] || pos_rec.linkdirrow;
		
		-- insert new linkrow
		nCount			:= array_upper(link_array, 1);
		if nCount > 1 then
			insert into temp_merge_linkrow 
					   ("link_id", "s_node", "e_node", "one_way_code", "link_num", "link_array", "linkdir_array")
				values (rec.link_id, start_node, end_node, rec.one_way_code, nCount, link_array, linkdir_array);
		end if;
	end loop;
	
	RETURN 1;
END;
$$;

CREATE OR REPLACE FUNCTION mid_search_linkrow_in_one_direction(	
								rec temp_merge_link_suspect, static_node bigint, search_node bigint, dir integer, 
								OUT linkrow bigint[], OUT linkdirrow boolean[], OUT reach_node bigint, OUT reach_link bigint)
	RETURNS record
	LANGUAGE plpgsql volatile
AS $$
DECLARE
	rec2				record;
BEGIN
	-- init
	reach_link	:= rec.link_id;
	reach_node	:= search_node;
	
	-- search
	while true loop
		-- check node is not end node
		select *
		from temp_merge_node_suspect
		where node_id = reach_node
		into rec2;
		if not found then
			exit;
		end if;
		
		-- search link
		select	a.*
		from
		(
			(
				select	link_id, tile_id, 
						True as linkdir, 
						(case when (reach_node = s_node) then e_node else s_node end) as nextnode, 
						one_way_code, 
						road_name, road_number, link_type, road_type, toll, length, function_class, display_class,
						elevated, structure, tunnel, rail_cross, paved, uturn, disobey_flag, etc_only_flag, 
						bypass_flag, matching_flag, highcost_flag, ipd, urban, erp, rodizio, extend_flag, 
						lane_num_s2e, lane_num_e2s, 
						width_s2e, width_e2s, 
						speed_limit_s2e, speed_limit_e2s, 
						speed_source_s2e, speed_source_e2s,
						park_link_connect_type,park_floor,park_link_lean,park_link_toll,park_region_id
				from temp_merge_link_suspect
				where 	(
							(reach_node = s_node) and (dir = 2)
							or
							(reach_node = e_node) and (dir = 3)
						)
						and 
						(link_id != reach_link)
			)
			union
			(
				select	link_id, tile_id, 
						False as linkdir, 
						(case when (reach_node = s_node) then e_node else s_node end) as nextnode, 
						(case when one_way_code = 2 then 3 when one_way_code = 3 then 2 else one_way_code end) as one_way_code, 
						road_name, road_number, link_type, road_type, toll, length, function_class, display_class,
						elevated, structure, tunnel, rail_cross, paved, uturn, disobey_flag, etc_only_flag, 
						bypass_flag, matching_flag, highcost_flag, ipd, urban, erp, rodizio, extend_flag, 
						lane_num_e2s as lane_num_s2e, lane_num_s2e as lane_num_e2s, 
						width_e2s as width_s2e, width_s2e as width_e2s, 
						speed_limit_e2s as speed_limit_s2e, speed_limit_s2e as speed_limit_e2s, 
						speed_source_e2s as speed_source_s2e, speed_source_s2e as speed_source_e2s,
						park_link_connect_type,park_floor,park_link_lean,park_link_toll,park_region_id
				from temp_merge_link_suspect
				where 	(
							(reach_node = s_node) and (dir = 3)
							or
							(reach_node = e_node) and (dir = 2)
						)
						and 
						(link_id != reach_link)
			)
		)as a
		left join temp_merge_link_walked as b
		on a.link_id = b.link_id
		left join link_tbl as c
		on (c.s_node = static_node and c.e_node = a.nextnode) or (c.e_node = static_node and c.s_node = a.nextnode)
		left join temp_merge_linkrow as d
		on (d.s_node = static_node and d.e_node = a.nextnode) or (d.e_node = static_node and d.s_node = a.nextnode)
		where b.link_id is null and c.link_id is null and d.link_id is null and (a.nextnode != static_node)
		into rec2;
		
		if	FOUND
			and	not (rec.tile_id is distinct from rec2.tile_id)
			and 
			(
				(rec2.length <= 1)
				or
				(
						not (rec.one_way_code is distinct from rec2.one_way_code)
					and	not (rec.road_name is distinct from rec2.road_name)
					and	not (rec.road_number is distinct from rec2.road_number)
					and	not (rec.link_type is distinct from rec2.link_type)
					and	not (rec.road_type is distinct from rec2.road_type)
					and	not (rec.toll is distinct from rec2.toll)
					and	not (rec.function_class is distinct from rec2.function_class)
					and	not (rec.display_class is distinct from rec2.display_class)
					and	not (rec.elevated is distinct from rec2.elevated)
					and	not (rec.structure is distinct from rec2.structure)
					and	not (rec.tunnel is distinct from rec2.tunnel)
					and	not (rec.rail_cross is distinct from rec2.rail_cross)
					and	not (rec.paved is distinct from rec2.paved)
					and	not (rec.uturn is distinct from rec2.uturn)
					and	not (rec.disobey_flag is distinct from rec2.disobey_flag)
					and	not (rec.etc_only_flag is distinct from rec2.etc_only_flag)
					and	not (rec.bypass_flag is distinct from rec2.bypass_flag)
					and	not (rec.matching_flag is distinct from rec2.matching_flag)
					and	not (rec.highcost_flag is distinct from rec2.highcost_flag)
					and	not (rec.ipd is distinct from rec2.ipd)
					and	not (rec.erp is distinct from rec2.erp)
					and	not (rec.rodizio is distinct from rec2.rodizio)					
					and	not (rec.extend_flag is distinct from rec2.extend_flag)
					and	not (rec.lane_num_s2e is distinct from rec2.lane_num_s2e)
					and	not (rec.lane_num_e2s is distinct from rec2.lane_num_e2s)
					and	not (rec.width_s2e is distinct from rec2.width_s2e)
					and	not (rec.width_e2s is distinct from rec2.width_e2s)
					and	not (rec.speed_limit_s2e is distinct from rec2.speed_limit_s2e)
					and	not (rec.speed_limit_e2s is distinct from rec2.speed_limit_e2s)
					and	not (rec.speed_source_s2e is distinct from rec2.speed_source_s2e)
					and	not (rec.speed_source_e2s is distinct from rec2.speed_source_e2s)
					and	not (rec.park_link_connect_type is distinct from rec2.park_link_connect_type)
					and	not (rec.park_floor is distinct from rec2.park_floor)
					and	not (rec.park_link_lean is distinct from rec2.park_link_lean)
					and	not (rec.park_link_toll is distinct from rec2.park_link_toll)
					and	not (rec.park_region_id is distinct from rec2.park_region_id)
				)
			)
		then
			-- mark walked_flag of current link
			insert into temp_merge_link_walked ("link_id") values (rec2.link_id);
			
			-- walk this link
			if (dir = 2) then
				linkrow		:= array_append(linkrow, rec2.link_id);
				linkdirrow	:= array_append(linkdirrow, rec2.linkdir);
			else
				linkrow		:= array_prepend(rec2.link_id, linkrow);
				linkdirrow	:= array_prepend(rec2.linkdir, linkdirrow);
			end if;
			reach_link	:= rec2.link_id;
			reach_node	:= rec2.nextnode;
		else
			EXIT;
		end if;
	end loop;
END;
$$;

-------------------------------------------------------------------------------------------------------------
-- check logic
-------------------------------------------------------------------------------------------------------------

CREATE OR REPLACE FUNCTION mid_check_regulation_item()
	RETURNS integer
	LANGUAGE plpgsql
	AS $$ 
DECLARE
	rec			record;
	nIndex			integer;
	nCount			integer;
	nNextIndex		integer;
BEGIN
	for rec in
		select *
		from
		(
			select	regulation_id, 
				array_agg(linkid) as link_array, 
				array_agg(nodeid) as node_array,
				array_agg(seq_num) as seq_array,
				array_agg(s_node) as snode_array,
				array_agg(e_node) as enode_array
			from
			(
				select regulation_id, linkid, nodeid, seq_num, s_node, e_node
				from regulation_item_tbl as m
				left join link_tbl as n
				on m.linkid = n.link_id
				order by regulation_id, seq_num
			)as a
			group by regulation_id
		)as r
		where array_upper(link_array,1) > 1
	loop
		-- check target node
		if (rec.node_array[2] != rec.snode_array[1] and rec.node_array[2] != rec.enode_array[1])
		or (rec.node_array[2] != rec.snode_array[3] and rec.node_array[2] != rec.enode_array[3])
		then
			raise WARNING 'regulation target node error: regulation_id = %', rec.regulation_id;
		end if;
		

		-- check linkrow continuable
		nCount	:= array_upper(rec.seq_array, 1);
		nIndex	:= 3;
		while nIndex < nCount loop
			if (
				rec.snode_array[nIndex] != rec.snode_array[nIndex+1]
				and 
				rec.snode_array[nIndex] != rec.enode_array[nIndex+1]
			   )
			   and 
			   (
				rec.enode_array[nIndex] != rec.snode_array[nIndex+1] 
				and 
				rec.enode_array[nIndex] != rec.enode_array[nIndex+1]
			   )
			then
				raise EXCEPTION 'regulation linkrow error: regulation_id = %', rec.regulation_id;
			end if;
			nIndex	:= nIndex + 1;
		end loop;
	end loop;
	
	return 1;
END;
$$;



CREATE OR REPLACE FUNCTION mid_check_regulation_condition()
	RETURNS integer
	LANGUAGE plpgsql
	AS $$ 
DECLARE
	rec				record;
BEGIN
	for rec in
		select * from condition_regulation_tbl
	loop
		-- check start_date
		if rec.start_month = 2 then
			if not (rec.start_day >= 0 and rec.start_day <= 29) then
				raise EXCEPTION 'regulation condition error: start_date format is uncorrect, cond_id = %', rec.cond_id;
			end if;
		elseif rec.start_month in (0,1,3,5,7,8,10,12) then
			if not (rec.start_day >= 0 and rec.start_day <= 31) then
				raise EXCEPTION 'regulation condition error: start_date format is uncorrect, cond_id = %', rec.cond_id;
			end if;
		elseif rec.start_month in (4,6,9,11) then
			if not (rec.start_day >= 0 and rec.start_day <= 30) then
				raise EXCEPTION 'regulation condition error: start_date format is uncorrect, cond_id = %', rec.cond_id;
			end if;
		end if;
		
		-- check end_date
		if rec.end_month = 2 then
			if not (rec.end_day >= 0 and rec.end_day <= 29) then
				raise EXCEPTION 'regulation condition error: end_date format is uncorrect, cond_id = %', rec.cond_id;
			end if;
		elseif rec.end_month in (0,1,3,5,7,8,10,12) then
			if not (rec.end_day >= 0 and rec.end_day <= 31) then
				raise EXCEPTION 'regulation condition error: end_date format is uncorrect, cond_id = %', rec.cond_id;
			end if;
		elseif rec.end_month in (4,6,9,11) then
			if not (rec.end_day >= 0 and rec.end_day <= 30) then
				raise EXCEPTION 'regulation condition error: end_date format is uncorrect, cond_id = %', rec.cond_id;
			end if;
		end if;
		
		-- check start_time
		if not (rec.start_hour = 24 and rec.start_minute = 0) then
			if (rec.start_hour < 0 or rec.start_hour >= 24) or (rec.start_minute < 0 or rec.start_minute >= 60) then
				raise EXCEPTION 'regulation condition error: start_time format is uncorrect, cond_id = %', rec.cond_id;
			end if;
		end if;
		
		-- check end_time
		if not (rec.end_hour = 24 and rec.end_minute = 0) then
			if (rec.end_hour < 0 or rec.end_hour >= 24) or (rec.end_minute < 0 or rec.end_minute >= 60) then
				raise EXCEPTION 'regulation condition error: end_time format is uncorrect, cond_id = %', rec.cond_id;
			end if;
		end if;
	
	end loop;
	
	return 1;
END;
$$;


CREATE OR REPLACE FUNCTION mid_check_guide_item(the_guide_tbl varchar)
	RETURNS integer
	LANGUAGE plpgsql
	AS $$ 
DECLARE
	rec				record;
	nIndex			integer;
	nCount			integer;
	nNextIndex		integer;
BEGIN
	for rec in
		execute
		'
		select	gid, nodeid,
				array_agg(linkid) as link_array, 
				array_agg(seq_num) as seq_array,
				array_agg(one_way_code) as oneway_array,
				array_agg(s_node) as snode_array,
				array_agg(e_node) as enode_array
		from
		(
			select m.gid, linkid, nodeid, seq_num, one_way_code, s_node, e_node
			from 
			(
				select    gid, nodeid, seq_num, link_array[seq_num] as linkid
                from
                (
                    select  gid, 
                            nodeid,
                            link_num,
                            link_array,
                            generate_series(1,link_num) as seq_num
                    from
                    (
                        select  gid, 
                                nodeid,
                                (passlink_cnt + 2) as link_num,
                                (
                                case 
                                when passlink_cnt = 0 then ARRAY[inlinkid, outlinkid]
                                else ARRAY[inlinkid] || cast(string_to_array(passlid, ''|'') as bigint[]) || ARRAY[outlinkid]
                                end
                                )as link_array
                        from ' ||the_guide_tbl|| '
                    )as a
                )as b
			) as m
			left join link_tbl as n
			on m.linkid = n.link_id
			order by gid, nodeid, seq_num
		)as a
		group by gid, nodeid
		'
	loop
		-- check target node
		if rec.nodeid is null then
			raise EXCEPTION '% target node null_error: gid = %', the_guide_tbl, rec.gid;
		end if;
		
		if (rec.nodeid is distinct from rec.snode_array[1] and rec.nodeid is distinct from rec.enode_array[1])
		or (rec.nodeid is distinct from rec.snode_array[2] and rec.nodeid is distinct from rec.enode_array[2])
		then
			raise EXCEPTION '% target node intersection_error: gid = %', the_guide_tbl, rec.gid;
		end if;
		
		-- check linkrow continuable
		nCount		:= array_upper(rec.seq_array, 1);
		nIndex		:= 1;
		while nIndex < nCount loop
			if rec.snode_array[nIndex] in (rec.snode_array[nIndex+1], rec.enode_array[nIndex+1]) then
				if rec.oneway_array[nIndex] in (2,4) then
					raise EXCEPTION '% linkrow error: traffic flow error, gid = %', the_guide_tbl, rec.gid;
				end if;
			elseif rec.enode_array[nIndex] in (rec.snode_array[nIndex+1], rec.enode_array[nIndex+1]) then
				if rec.oneway_array[nIndex] in (3,4) then
					raise EXCEPTION '% linkrow error: traffic flow error, gid = %', the_guide_tbl, rec.gid;
				end if;
			else
				raise EXCEPTION '% linkrow error: links have no intersection, gid = %', the_guide_tbl, rec.gid;
			end if;
			
			nIndex		:= nIndex + 1;
		end loop;
	end loop;
	
	return 1;
END;
$$;


CREATE OR REPLACE FUNCTION mid_cnv_length(length integer) RETURNS integer
    LANGUAGE plpgsql
    AS $$
    BEGIN
	    RETURN CASE
	        WHEN length = 0 THEN 1
	        ELSE length
	    END;
    END;
$$;



CREATE OR REPLACE FUNCTION rdb_cal_lon(tile_x integer, tile_z integer)
  RETURNS float
  LANGUAGE plpgsql
AS $$
DECLARE
	world_x	float;
	lon	float;
BEGIN
	world_x	:= cast(tile_x as float) / (1 << tile_z);
	lon	:= (world_x - 0.5) * 360;
	return lon;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_cal_lat(tile_y integer, tile_z integer)
  RETURNS float
  LANGUAGE plpgsql
AS $$
DECLARE
	world_y	float;
	lat	float;
	PI	float;
BEGIN
	PI	:= 3.1415926535897931;
	world_y	:= cast(tile_y as float) / (1 << tile_z);
	lat	:= (2 * atan(exp((1 - 2 * world_y) * PI)) - PI / 2) * 180 / PI;
	return lat;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_cal_tilex(point_geom geometry, tile_z integer)
  RETURNS integer
  LANGUAGE plpgsql
AS $$
DECLARE
     x double precision;
     lon double precision;
     tile_x integer;
BEGIN
	lon = ST_X(point_geom);
	x = lon / 360. + 0.5;        
	tile_x = cast(floor(x * (1 << tile_z)) as integer);
	
	return tile_x;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_cal_tiley(point_geom geometry, tile_z integer)
  RETURNS integer
  LANGUAGE plpgsql
AS $$
DECLARE
     PI double precision;
     y double precision;
     lat double precision;
     tile_y integer;
BEGIN
	PI  = 3.1415926535897931;
	lat = ST_Y(point_geom);
	y = ln(tan(PI / 4. + radians(lat) / 2.));
	y = 0.5 - y / 2. / PI;
                                    
	tile_y = cast(floor(y * (1 << tile_z)) as integer);
	
	return tile_y;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_geom2tileid_z(point_geom geometry, z integer)
  RETURNS bigint 
  LANGUAGE plpgsql
AS $$ 
DECLARE
	 PI double precision;
     x double precision;
     y double precision;
     lon double precision;
     lat double precision;
     tile_x bigint;
     tile_y bigint;
     tile_id bigint;
BEGIN
	PI := 3.1415926535897931;
	
	--Convert LatLng to world coordinate
	--This is actually a Google Mercator projection
	if z > 14 then  
		return NULL; 
	end if;

	lon = ST_X(point_geom);
	lat = ST_Y(point_geom);
	x = lon / 360. + 0.5;
	y = ln(tan(PI / 4. + radians(lat) / 2.));
	y = 0.5 - y / 2. / PI;
                                    
	tile_x = cast(floor(x * (1<<z)) as integer);
	tile_y = cast(floor(y * (1<<z)) as integer);
                                            
	tile_id = (z << 28) | (tile_x << 14) | tile_y;
                                            
	RETURN tile_id;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_get_tile_area_byid(tile_id integer, tile_z integer)
  RETURNS geometry
  LANGUAGE plpgsql
AS $$
DECLARE
	tile_x			integer;
	tile_y			integer;
	lon_left_top		float;
	lat_left_top		float;
	lon_right_bottom	float;
	lat_right_bottom	float;
	tile_area		geometry;
	pts			geometry[];
BEGIN
	tile_x			:= (tile_id >> 14) & ((1 << 14) - 1);
	tile_y			:= tile_id & ((1 << 14) - 1);
	
	lon_left_top		:= rdb_cal_lon(tile_x, tile_z);
	lat_left_top		:= rdb_cal_lat(tile_y, tile_z);
	
	lon_right_bottom	:= rdb_cal_lon(tile_x + 1, tile_z);
	lat_right_bottom	:= rdb_cal_lat(tile_y + 1, tile_z);

	pts			:= ARRAY
				[
					st_point(lon_left_top, lat_left_top),
					st_point(lon_right_bottom, lat_left_top),
					st_point(lon_right_bottom, lat_right_bottom),
					st_point(lon_left_top, lat_right_bottom),
					st_point(lon_left_top, lat_left_top)
				];

	tile_area		:= st_polygon(ST_LineFromMultiPoint(st_collect(pts)), 4326);

	return tile_area;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_get_tile_area_byxy(tile_x integer, tile_y integer, tile_z integer)
  RETURNS geometry
  LANGUAGE plpgsql
AS $$
DECLARE
	lon_left_top		float;
	lat_left_top		float;
	lon_right_bottom	float;
	lat_right_bottom	float;
	tile_area		geometry;
	pts			geometry[];
BEGIN	
	lon_left_top		:= rdb_cal_lon(tile_x, tile_z);
	lat_left_top		:= rdb_cal_lat(tile_y, tile_z);
	
	lon_right_bottom	:= rdb_cal_lon(tile_x + 1, tile_z);
	lat_right_bottom	:= rdb_cal_lat(tile_y + 1, tile_z);

	pts			:= ARRAY
				[
					st_point(lon_left_top, lat_left_top),
					st_point(lon_right_bottom, lat_left_top),
					st_point(lon_right_bottom, lat_right_bottom),
					st_point(lon_left_top, lat_right_bottom),
					st_point(lon_left_top, lat_left_top)
				];

	tile_area		:= st_polygon(ST_LineFromMultiPoint(st_collect(pts)), 4326);

	return tile_area;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_get_tile_dummy_border(tile_x integer, tile_y integer, tile_z integer)
  RETURNS geometry
  LANGUAGE plpgsql
AS $$
DECLARE
	lon_left_top		float;
	lat_left_top		float;
	lon_right_bottom	float;
	lat_right_bottom	float;
	tile_dummy_border	geometry;
	pts					geometry[];
BEGIN	
	lon_left_top		:= rdb_cal_lon(tile_x, tile_z);
	lat_left_top		:= rdb_cal_lat(tile_y, tile_z);
	
	lon_right_bottom	:= rdb_cal_lon(tile_x + 1, tile_z);
	lat_right_bottom	:= rdb_cal_lat(tile_y + 1, tile_z);

	pts			:= ARRAY
				[
					st_point(lon_left_top, lat_right_bottom),
					st_point(lon_right_bottom, lat_right_bottom),
					st_point(lon_right_bottom, lat_left_top)
				];

	tile_dummy_border	:= ST_SetSrid(ST_LineFromMultiPoint(st_collect(pts)), 4326);

	return tile_dummy_border;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_split_circle(circle geometry)
  RETURNS geometry[]
  LANGUAGE plpgsql
AS $$
DECLARE
	sublink_array	geometry[];
	pts				geometry[];
	pt				geometry;
	nCount			integer;
	nIndex			integer;
BEGIN
--	nCount		:= st_numpoints(circle);
--	
--	-- first sublink
--	nIndex		:= 1;
--	pts			:= ARRAY[st_pointn(circle,nIndex)];
--	while nIndex < nCount loop
--		nIndex	:= nIndex + 1;
--		pt		:= st_pointn(circle, nIndex);
--		pts		:= array_append(pts, pt);
--		if not st_equals(pts[1], pt) then
--			exit;
--		end if;
--	end loop;
--	sublink_array		:= ARRAY[ST_SetSrid(ST_LineFromMultiPoint(st_collect(pts)), 4326)];
--	
--	-- second sublink
--	pts			:= ARRAY[st_pointn(circle,nIndex)];
--	while nIndex < nCount loop
--		nIndex	:= nIndex + 1;
--		pt		:= st_pointn(circle, nIndex);
--		pts		:= array_append(pts, pt);
--		if not st_equals(pts[1], pt) then
--			exit;
--		end if;
--	end loop;
--	sublink_array		:= array_append(sublink_array, ST_SetSrid(ST_LineFromMultiPoint(st_collect(pts)), 4326));
--
--	-- third sublink
--	nIndex	:= nIndex - 1;
--	while nIndex > 0 loop
--		circle	:= st_removepoint(circle, nIndex-1);
--		nIndex	:= nIndex - 1;
--	end loop;
--	sublink_array		:= array_append(sublink_array, circle);
	
	sublink_array		:= ARRAY[ST_Line_Substring(circle, 0, 0.3)];
	sublink_array		:= array_append(sublink_array, ST_Line_Substring(circle, 0.3, 0.6));
	sublink_array		:= array_append(sublink_array, ST_Line_Substring(circle, 0.6, 1));
	
	return sublink_array;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_sort_sublink(
											link_id bigint, link_geom geometry, 
											sub_tile_array integer[], sub_geom_array geometry[],
											OUT sort_tile_array integer[], OUT sort_geom_array geometry[])
  LANGUAGE plpgsql
AS $$
DECLARE
	sub_geom			geometry;
	startpt				geometry;
	nSubCount			integer;
	nSubIndex			integer;
	fraction_a			float;
	fraction_b			float;
BEGIN
	nSubCount		:= array_upper(sub_geom_array, 1);
	
	if nSubCount = 1 then
		select sub_tile_array, sub_geom_array 
		into sort_tile_array, sort_geom_array;
	else
		select  array_agg(the_sub_tile) as the_sub_tile_array,
				array_agg(the_sub_geom) as the_sub_geom_array
		from
		(
			select 	the_link_geom, the_sub_tile, the_sub_geom, 
					rdb_get_fraction(the_link_geom, the_sub_geom) as fraction
			from
			(
				select	the_link_geom,
						the_sub_tile_array[sub_index] as the_sub_tile,
						the_sub_geom_array[sub_index] as the_sub_geom
				from
				(
					select 	link_geom as the_link_geom, 
							sub_tile_array as the_sub_tile_array,
							sub_geom_array as the_sub_geom_array,
							generate_series(1,nSubCount) as sub_index
				)as t
			)as a
			order by fraction
		)as b
		group by the_link_geom
		into sort_tile_array, sort_geom_array;
	end if;

	startpt		:= st_startpoint(link_geom);
	for nSubIndex in 1..nSubCount loop
		sub_geom	:= sort_geom_array[nSubIndex];
		if st_isclosed(sub_geom) then
			fraction_a	:= st_line_locate_point(link_geom, st_pointn(sub_geom,2));
			fraction_b	:= st_line_locate_point(link_geom, st_pointn(sub_geom,3));
			if fraction_a > fraction_b then
				sort_geom_array[nSubIndex]	:= st_reverse(sub_geom);
			end if;
		end if;
		
		if st_equals(startpt, st_startpoint(sub_geom)) then
			startpt		:= st_endpoint(sub_geom);
		elseif st_equals(startpt, st_endpoint(sub_geom)) then
			startpt		:= st_startpoint(sub_geom);
			sort_geom_array[nSubIndex]	:= st_reverse(sub_geom);
		else
			raise EXCEPTION 'geom error while spliting link, link_id = %', link_id;
		end if;
	end loop;
END;
$$;


CREATE OR REPLACE FUNCTION rdb_get_fraction(link_geom geometry, sub_geom geometry)
  RETURNS float
  LANGUAGE plpgsql
AS $$
DECLARE
	startpt			geometry;
	nextpt			geometry;
	fraction_pt		geometry;
	nCount			integer;
	nIndex			integer;
	fraction		float;
BEGIN
	startpt		:= st_startpoint(sub_geom);
	nCount		:= st_numpoints(sub_geom);
	for nIndex in 2..nCount loop
		nextpt	:= st_pointn(sub_geom, nIndex);
		if not st_equals(startpt, nextpt) then 
			exit;
		end if;
	end loop;
	
	fraction_pt	:= st_line_interpolate_point(st_linefrommultipoint(st_collect(ARRAY[startpt,nextpt])), 0.5);
	fraction	:= st_line_locate_point(link_geom, fraction_pt);
	
	return fraction;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_merge_sublink_in_tile(
											link_id bigint, link_geom geometry, 
											sub_tile_array integer[], sub_geom_array geometry[],
											OUT merge_tile_array integer[], OUT merge_geom_array geometry[])
  LANGUAGE plpgsql
AS $$
DECLARE
	sort_tile_array		integer[];
	sort_geom_array 	geometry[];
	merge_geom			geometry;
	startpt				geometry;
	
	nSubCount			integer;
	nMergeCount			integer;
	
	nFromIndex			integer;
	nToIndex			integer;
	nFromTile			integer;
	nToTile				integer;
BEGIN
	select (rdb_sort_sublink(link_id, link_geom, sub_tile_array, sub_geom_array)).*
	into sort_tile_array, sort_geom_array;
	
	select (rdb_merge_sorted_sublink_in_tile(sort_tile_array, sort_geom_array)).*
	into merge_tile_array, merge_geom_array;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_merge_sorted_sublink_in_tile(
											sort_tile_array integer[], sort_geom_array geometry[],
											OUT merge_tile_array integer[], OUT merge_geom_array geometry[])
  LANGUAGE plpgsql
AS $$
DECLARE
	merge_geom			geometry;
	startpt				geometry;
	
	nSubCount			integer;
	nMergeCount			integer;
	
	nFromIndex			integer;
	nToIndex			integer;
	nFromTile			integer;
	nToTile				integer;
BEGIN
	nMergeCount			:= 0;
	merge_tile_array	:= sort_tile_array[1:1];
	merge_geom_array	:= sort_geom_array[1:1];
	
	nFromIndex		:= 1;
	nFromTile		:= sort_tile_array[1];
	nSubCount		:= array_upper(sort_geom_array, 1);
	nToIndex		:= 1;
	while nToIndex <= nSubCount loop
		nToIndex	:= nToIndex + 1;
		if (sort_tile_array[nToIndex] is distinct from nFromTile) then
			-- merge
			nMergeCount		:= nMergeCount + 1;
			merge_tile_array[nMergeCount]	:= sort_tile_array[nFromIndex];
			if nFromIndex = nToIndex - 1 then
				merge_geom_array[nMergeCount]	:= sort_geom_array[nFromIndex];
			else
				select st_linefrommultipoint(st_collect(geom_point)) as the_geom
				from
				(
					select path, geom as geom_point
					from
					(
						select (st_dumppoints(geom_multiline)).*
						from
						(
							select st_collect(line_geom) as geom_multiline
							from
							(
								select line_geom_array[sub_index] as line_geom, sub_index
								from
								(
									select sort_geom_array as line_geom_array, generate_series(nFromIndex, nToIndex - 1) as sub_index
								)as a
								order by sub_index
							)as b
						)as c
					)as d
					where not (path[1] > 1 and path[2] = 1)
					order by path
				)as e
				into merge_geom;
				merge_geom_array[nMergeCount]	:= merge_geom;
			end if;
			
			-- new search
			nFromTile		:= sort_tile_array[nToIndex];
			nFromIndex		:= nToIndex;
		end if;
	end loop;
END;
$$;


CREATE OR REPLACE FUNCTION rdb_get_split_link(target_link_id bigint, adj_node_id bigint)
  RETURNS bigint
  LANGUAGE plpgsql
AS $$
DECLARE
	new_link_id		bigint;
BEGIN
	if adj_node_id is null then
		select link_id
		from temp_split_newlink
		where old_link_id = target_link_id and sub_index = sub_count
		into new_link_id;
	else
		select link_id
		from temp_split_newlink
		where old_link_id = target_link_id and adj_node_id in (s_node, e_node)
		into new_link_id;
	end if;
	
	if new_link_id is null then
		new_link_id	:= target_link_id;
	end if;
	
	return new_link_id;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_get_split_link_array(target_link_id bigint, from_node_id bigint, bReverse integer)
  RETURNS bigint[]
  LANGUAGE plpgsql
AS $$
DECLARE
	new_link_array		bigint[];
	bDir				integer;
	strOrder			varchar;
BEGIN
	if from_node_id is null then
		bDir	:= 0;
	else
		select dir
		from
		(
			select 0 as dir
			from temp_split_newlink
			where (old_link_id = target_link_id) and (sub_index = 1) and (s_node = from_node_id)
			union
			select 1 as dir
			from temp_split_newlink
			where (old_link_id = target_link_id) and (sub_index = sub_count) and (e_node = from_node_id)
		)as a
		into bDir;
	end if;
	
	if bDir is null then
		new_link_array	:= ARRAY[target_link_id];
	else
		if bDir = bReverse then
			select array_agg(link_id) as link_array
			from
			(
				select old_link_id, link_id, sub_index
				from temp_split_newlink
				where old_link_id = target_link_id
				order by sub_index asc
			)as a
			group by old_link_id
			into new_link_array;
		else
			select array_agg(link_id) as link_array
			from
			(
				select old_link_id, link_id, sub_index
				from temp_split_newlink
				where old_link_id = target_link_id
				order by sub_index desc
			)as a
			group by old_link_id
			into new_link_array;
		end if;
	end if;
	
	return new_link_array;
END;
$$;


CREATE OR REPLACE FUNCTION rdb_get_junction_node_list(start_node_id bigint, s_array bigint[], e_array bigint[])
  RETURNS bigint[]
  LANGUAGE plpgsql
AS $$
DECLARE
	junction_node_list	bigint[];
	junction_node_id	bigint;
	nIndex				integer;
	nCount				integer;
BEGIN
	junction_node_id	:= start_node_id;
	junction_node_list	:= ARRAY[start_node_id];
	
	nCount				:= array_upper(s_array,1);
	for nIndex in 2..(nCount-1) loop
		if junction_node_id = s_array[nIndex] then
			junction_node_id	:= e_array[nIndex];
		else
			junction_node_id	:= s_array[nIndex];
		end if;
		junction_node_list	:= array_append(junction_node_list, junction_node_id);
	end loop;
	
	return junction_node_list;
END;
$$;


CREATE OR REPLACE FUNCTION rdb_get_split_linkrow(start_node_id bigint, link_array bigint[], s_array bigint[], e_array bigint[])
  RETURNS bigint[]
  LANGUAGE plpgsql
AS $$
DECLARE
	new_linkrow	bigint[];
BEGIN
	select  array_agg(linkid) as new_link_array
	from
	(
		select	link_index, sub_index, split_link_array[sub_index] as linkid
		from
		(
			select	link_index, split_link_array, generate_series(1,sub_count) as sub_index
			from
			(
				select  link_index, split_link_array, array_upper(split_link_array,1) as sub_count
				from
				(
					select  link_num, 
							link_index, 
							(
							case 
							when link_index = 1 then ARRAY[rdb_get_split_link(old_link_array[link_index], jnc_node_array[1])]
							when link_index = link_num then ARRAY[rdb_get_split_link(old_link_array[link_index], jnc_node_array[link_index-1])]
							else rdb_get_split_link_array(old_link_array[link_index], jnc_node_array[link_index-1], 0)
							end
							)as split_link_array
					from
					(
						select  link_num, old_link_array, jnc_node_array,
								generate_series(1,link_num) as link_index
						from
						(
							select  array_upper(link_array, 1) as link_num, 
									link_array as old_link_array, 
									rdb_get_junction_node_list(start_node_id, s_array, e_array) as jnc_node_array
						)as a
					)as b
				)as c
			)as d
		)as e
		order by link_index, sub_index
	)as f
	into new_linkrow;
	
	return new_linkrow;
END;
$$;


CREATE OR REPLACE FUNCTION mid_cal_zm(shape_point geometry, dir integer) RETURNS smallint
  LANGUAGE plpgsql
AS $$
DECLARE
    num     integer;
    i       integer;
    MIN_DIFF_CRD double precision;
    
    org_lon double precision;
    org_lat double precision;
    dst_lon double precision;
    dst_lat double precision;
    delta_lat double precision; 
    delta_lon double precision;
    
    A_WGS84  double precision;
    E2_WGS84 double precision;
    PI double precision;
    
	ref_lat double precision;
	real_delta_lat double precision;
    real_delta_lon double precision;

    sinlat double precision;
    c double precision;
    lat_radi double precision;
    
    M2SecLon double precision;
    M2SecLat double precision;
    
    real_dir double precision;
    
    direction smallint;
    direction_32 integer;
BEGIN 
    PI          := 3.1415926535897932;
    A_WGS84     := 6378137.0;
    E2_WGS84    := 6.69437999013e-3;
    MIN_DIFF_CRD := 31.0; -- 0x1f

    num := ST_NPoints(shape_point);  
    IF (num < 2) THEN
        RETURN NULL;
    END IF;

    IF (dir = -1) THEN
		org_lon := ST_X(ST_PointN(shape_point, num)) * 256 * 3600;
		org_lat := ST_Y(ST_PointN(shape_point, num)) * 256 * 3600;
    else
		org_lon := ST_X(ST_PointN(shape_point, 1)) * 256 * 3600;
		org_lat := ST_Y(ST_PointN(shape_point, 1)) * 256 * 3600;
    end if;
    
    i := 1;
    while (num - 1) >= i LOOP
		IF (dir = -1) THEN
			dst_lon := ST_X(ST_PointN(shape_point, num - i)) * 256 * 3600;
			dst_lat := ST_Y(ST_PointN(shape_point, num - i)) * 256 * 3600;
		else
			dst_lon := ST_X(ST_PointN(shape_point, 1 + i)) * 256 * 3600;
			dst_lat := ST_Y(ST_PointN(shape_point, 1 + i)) * 256 * 3600;
		end if;

		-- Get the next shape point
		delta_lon := dst_lon - org_lon;
		delta_lat := dst_lat - org_lat;
		if (abs(delta_lon) + abs(delta_lat)) < MIN_DIFF_CRD then
			i := i + 1;
		else
			exit;
		end if;
    END LOOP;
    
    if delta_lon = 0 and delta_lat = 0 then
    	real_dir  := 0.0;
	else
	    ref_lat   := (org_lat + delta_lat / 2.0) / 256.0;
	    lat_radi  := ref_lat / 3600.0 * PI / 180.0;
	    sinlat    := sin(lat_radi);
	    c         := 1.0 - (E2_WGS84 * sinlat * sinlat);
	    M2SecLon  := 3600.0 / ((PI/180.0) * A_WGS84 * cos(lat_radi) / sqrt(c));
	    M2SecLat  := 1.0 / ((PI/648000.0) * A_WGS84 * (1 - E2_WGS84)) * sqrt(c*c*c);
	    real_delta_lat := delta_lat / M2SecLat / 256.0;
	    real_delta_lon := delta_lon / M2SecLon / 256.0;
        real_dir := (atan2(real_delta_lat, real_delta_lon) * 180.0 / PI);
    end if;
    
    direction_32 := 0;
    if (real_dir > 0.0) then
        direction_32 := round(real_dir * 65536 / 360.0);
    else 
        --max := 65535;  --0xffff;
        direction_32 := round(65535 - abs(real_dir * 65536 / 360.0));
    end if;
    
    direction := direction_32 - 32768;
    
    return direction;
END;
$$;


-------------------------------------------------------------------------------------------------------------
-- lonlat / world / pixel
-------------------------------------------------------------------------------------------------------------

CREATE OR REPLACE FUNCTION lonlat2pixel_tile(tz int, tx int, ty int, lon float, lat float, tile_pixel int, out px int, out py int)
  LANGUAGE plpgsql
AS $$
DECLARE
	lleft float;
	bottom float;
	rright float;
	top float;
	box record;
	pixel record;
BEGIN 

    box := tile_bbox(tz, tx, ty);
    lleft := box.lleft;
    bottom := box.bottom;
    rright := box.rright;
    top := box.top;
--     raise info 'lleft:%,bottom:%,rright:%,top:%',lleft,bottom,rright,top;
    
    pixel := lonlat2pixel(lon, lat, lleft, top, rright, bottom, tile_pixel, tile_pixel);
    px := round(pixel.wx_out);
    py := round(pixel.wy_out);
    
END;
$$;


CREATE OR REPLACE FUNCTION lonlat2pixel(lon float,lat float, lleft float,top float, rright float,bottom float, width int, height int,out wx_out float,out wy_out float ) RETURNS record
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

    ---""" Convert lonlat to pixel coordinate """
    world1 := lonlat2world(lleft,top);
    lleft := world1.x;
    top := world1.y;
    world2 := lonlat2world(rright,bottom);
    rright := world2.x;
    bottom := world2.y;
--     raise info 'lleft:%,bottom:%,rright:%,top:%',lleft,bottom,rright,top;
    
    w := abs(rright - lleft) / width;
    h := abs(bottom - top) / height;
    world3 := lonlat2world(lon,lat);
    wx := world3.x;
    wy := world3.y;
--     raise info 'wx:%,wy:%',wx,wy;
    
    wx_out := (wx - lleft) / w;
    wy_out := (wy - top) / h;
--     raise info 'wx_out:%,wy_out:%',wx_out,wy_out;
    
END;
$$;


CREATE OR REPLACE FUNCTION tile_bbox(z int,x int,y int,out lleft float, out bottom float, out rright float, out top float) RETURNS record
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

--     """ Get tile bounding box """;
    world1 := pixel2world(z, x, y, 0, 1, 1);
    x1 := world1.wx;
    y1 := world1.wy;
    lonlat1 :=  world2lonlat(x1,y1);
    lleft := lonlat1.lon;
    bottom := lonlat1.lat;

    world2 := pixel2world(z, x, y, 1, 0, 1);
    x2 := world2.wx;
    y2 := world2.wy;
    lonlat2 :=  world2lonlat(x2,y2);
    rright := lonlat2.lon;
    top := lonlat2.lat;

    
END;
$$;

CREATE OR REPLACE FUNCTION world2lonlat(x float,y float, out lon float, out lat float) RETURNS record
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


CREATE OR REPLACE FUNCTION lonlat2world(lon float, lat float, out x float, out y float) RETURNS record
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


CREATE OR REPLACE FUNCTION pixel2world(tz int, tx int, ty int, px int, py int, size int, out wx float, out wy float) RETURNS record
  LANGUAGE plpgsql
AS $$
DECLARE
	
BEGIN 
    --""" Convert pixel coordinate of a tile to world coordinate """
--     size := 1;
    wx := (tx * size + px) / (2 ^ tz * size);
    wy := (ty * size + py) / (2 ^ tz * size);
    
END;
$$; 


CREATE OR REPLACE FUNCTION rdb_move_point_pixel(point geometry, tz integer, size integer) 
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

CREATE OR REPLACE FUNCTION rdb_get_proxy_xy_advice(
											sub_count integer, sub_index integer,
											sub_geom_array geometry[], delete_flag_array boolean[],
											sub_tx_array integer[], sub_ty_array integer[], 
											OUT x_must float, OUT y_must float, OUT x_proxy float, OUT y_proxy float)
  LANGUAGE plpgsql
AS $$
DECLARE
	nSubCount			integer;
	nSubIndex			integer;
BEGIN
	nSubIndex	:= sub_index - 1;
	while nSubIndex > 0 loop
		if not delete_flag_array[nSubIndex] then
			x_proxy				:= st_x(st_endpoint(sub_geom_array[nSubIndex]));
			y_proxy				:= st_y(st_endpoint(sub_geom_array[nSubIndex]));
			
			if sub_tx_array[nSubIndex] != sub_tx_array[sub_index] then
				x_must			:= st_x(st_endpoint(sub_geom_array[nSubIndex]));
			end if;
			if sub_ty_array[nSubIndex] != sub_ty_array[sub_index] then
				y_must			:= st_y(st_endpoint(sub_geom_array[nSubIndex]));
			end if;
			
			exit;
		end if;
		nSubIndex	:= nSubIndex - 1;
	end loop;
	
	nSubIndex	:= sub_index + 1;
	while nSubIndex <= sub_count loop
		if not delete_flag_array[nSubIndex] then
			x_proxy				:= st_x(st_startpoint(sub_geom_array[nSubIndex]));
			y_proxy				:= st_y(st_startpoint(sub_geom_array[nSubIndex]));
			
			if sub_tx_array[nSubIndex] != sub_tx_array[sub_index] then
				x_must			:= st_x(st_startpoint(sub_geom_array[nSubIndex]));
			end if;
			if sub_ty_array[nSubIndex] != sub_ty_array[sub_index] then
				y_must			:= st_y(st_startpoint(sub_geom_array[nSubIndex]));
			end if;
			
			exit;
		end if;
		nSubIndex	:= nSubIndex + 1;
	end loop;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_find_proxy_point_of_short_sublink(
											start_proxy_point geometry, end_proxy_point geometry,
											sub_count integer, sub_index integer,
											sub_geom_array geometry[], delete_flag_array boolean[],
											sub_tx_array integer[], sub_ty_array integer[])
  returns geometry
  LANGUAGE plpgsql
AS $$
DECLARE
	nSubCount			integer;
	nSubIndex			integer;
	x_must				float;
	y_must				float;
	x_proxy				float;
	y_proxy				float;
BEGIN
	if start_proxy_point is not null then
		nSubIndex	:= sub_index - 1;
		while nSubIndex > 0 loop
			if not delete_flag_array[nSubIndex] then
				exit;
			end if;
			nSubIndex	:= nSubIndex - 1;
		end loop;
		if nSubIndex = 0 then
			return start_proxy_point;
		end if;
	end if;
	
	if end_proxy_point is not null then
		nSubIndex	:= sub_index + 1;
		while nSubIndex <= sub_count loop
			if not delete_flag_array[nSubIndex] then
				exit;
			end if;
			nSubIndex	:= nSubIndex + 1;
		end loop;
		if nSubIndex > sub_count then
			return end_proxy_point;
		end if;
	end if;
	
	select 	(rdb_get_proxy_xy_advice(sub_count, sub_index, sub_geom_array, delete_flag_array, sub_tx_array, sub_ty_array)).*
			into x_must, y_must, x_proxy, y_proxy;
	return 	(
			case 
			when x_must is not null and y_must is not null then st_setsrid(st_point(x_must,y_must), 4326)
			when x_must is not null then st_setsrid(st_point(x_must,y_proxy), 4326)
			when y_must is not null then st_setsrid(st_point(x_proxy,y_must), 4326)
			else st_setsrid(st_point(x_proxy,y_proxy), 4326)
			end
			);
END;
$$;

CREATE OR REPLACE FUNCTION rdb_recalc_tile_for_move_sublink(
											sub_count integer, sub_index integer, sub_geom geometry, 
											sub_geom_array geometry[], delete_flag_array boolean[],
											sub_tile_array integer[], sub_tx_array integer[], sub_ty_array integer[])
  returns integer
  LANGUAGE plpgsql
AS $$
DECLARE
	nSubCount			integer;
	nSubIndex			integer;
	found_flag			boolean;
	connect_point		geometry;
BEGIN
	if sub_index = 1 then
		nSubIndex	:= 2;
		while nSubIndex <= sub_count loop
			if not delete_flag_array[nSubIndex] then
				connect_point	:= st_startpoint(sub_geom_array[nSubIndex]);
				exit;
			end if;
			nSubIndex	:= nSubIndex + 1;
		end loop;
	end if;
	
	if sub_index = sub_count then
		nSubIndex	:= sub_count - 1;
		while nSubIndex > 0 loop
			if not delete_flag_array[nSubIndex] then
				connect_point	:= st_endpoint(sub_geom_array[nSubIndex]);
				exit;
			end if;
			nSubIndex	:= nSubIndex - 1;
		end loop;
	end if;
	
	if sub_tile_array[sub_index] != sub_tile_array[nSubIndex] then
		if sub_tx_array[sub_index] = sub_tx_array[nSubIndex] then
			select true
			from
			(
				select (st_dumppoints(sub_geom)).*
			)as t
			where st_y(geom) != st_y(connect_point)
			limit 1
			into found_flag;
			if not FOUND then
				return sub_tile_array[nSubIndex];
			end if;
		end if;
		if sub_ty_array[sub_index] = sub_ty_array[nSubIndex] then
			select true
			from
			(
				select (st_dumppoints(sub_geom)).*
			)as t
			where st_x(geom) != st_x(connect_point)
			limit 1
			into found_flag;
			if not FOUND then
				return sub_tile_array[nSubIndex];
			end if;
		end if;
	end if;
	
	return sub_tile_array[sub_index];
END;
$$;

CREATE OR REPLACE FUNCTION rdb_delete_short_sublink(
											sub_tile_array integer[], sub_geom_array geometry[],
											delete_flag_array integer[], new_pt_array geometry[], 
											OUT keep_tile_array integer[], OUT keep_geom_array geometry[])
  LANGUAGE plpgsql
AS $$
DECLARE
	nSubCount			integer;
	nSubIndex			integer;
	nKeepCount			integer;
	nKeepIndex			integer;
	keep_index_array	integer[];
BEGIN
	nSubCount		:= array_upper(sub_geom_array, 1);
	select  array_agg(the_sub_tile) as the_sub_tile_array,
			array_agg(the_sub_geom) as the_sub_geom_array,
			array_agg(sub_index) as index_array
	from
	(
		select 	sub_index, the_sub_tile, the_sub_geom
		from
		(
			select	sub_index,
					the_sub_tile_array[sub_index] as the_sub_tile,
					the_sub_geom_array[sub_index] as the_sub_geom,
					the_flag_array[sub_index] as the_flag
			from
			(
				select 	sub_tile_array as the_sub_tile_array,
						sub_geom_array as the_sub_geom_array,
						delete_flag_array as the_flag_array,
						generate_series(1,nSubCount) as sub_index
			)as t
		)as a
		where the_flag = 0
		order by sub_index asc
	)as b
	into keep_tile_array, keep_geom_array, keep_index_array;

	nKeepCount		:= array_upper(keep_tile_array, 1);
	for nKeepIndex in 1..nKeepCount loop
		nSubIndex	:= keep_index_array[nKeepIndex];
		if nSubIndex > 1 and delete_flag_array[nSubIndex-1] = 1 then
			keep_geom_array[nKeepIndex]	:= st_setpoint(keep_geom_array[nKeepIndex], 0, new_pt_array[nSubIndex-1]);
		end if;
		if nSubIndex < nSubCount and delete_flag_array[nSubIndex+1] = 1 then
			keep_geom_array[nKeepIndex]	:= st_setpoint(keep_geom_array[nKeepIndex], st_numpoints(keep_geom_array[nKeepIndex])-1, new_pt_array[nSubIndex+1]);
		end if;
	end loop;
	
	select (rdb_merge_sorted_sublink_in_tile(keep_tile_array, keep_geom_array)).*
	into keep_tile_array, keep_geom_array;
END;
$$;


CREATE OR REPLACE FUNCTION mid_calc_new_pos_for_vertex() 
  RETURNS integer
  LANGUAGE plpgsql
AS $$
DECLARE
	rec			record;
	node_id		bigint;
	old_x		double precision;
	old_y		double precision;
	px			smallint;
	py			smallint;
	x			double precision;
	y			double precision;
BEGIN
	for rec in 
		select 	link_id, tile_id, tz, tx, ty, size, 
				s_node, e_node, b_flag1, b_flag2,
				x1, y1, x2, y2, 
				px1, py1, px2, py2, 
				the_geom
		from temp_move_shortlink 
	loop
		old_x	:= null;
		old_y	:= null;
		
		-- calc new px/py
		if rec.x1 = rec.x2 or (rec.py1 in (0,4096) and rec.y1 != rec.y2) then
			-- move y
			if rec.y1 < rec.y2 then
				if rec.py1 = 0 then
					node_id		:= rec.s_node;
					px			:= rec.px1;
					old_x		:= rec.x1;
					py			:= rec.py1 + 1;
				else
					node_id		:= rec.e_node;
					px			:= rec.px2;
					old_x		:= rec.x2;
					py			:= rec.py2 - 1;
				end if;
			else--if rec.y1 > rec.y2 then
				if rec.py1 = 0 then
					node_id		:= rec.e_node;
					px			:= rec.px2;
					old_x		:= rec.x2;
					py			:= rec.py2 + 1;
				else
					node_id		:= rec.s_node;
					px			:= rec.px1;
					old_x		:= rec.x1;
					py			:= rec.py1 - 1;
				end if;
			end if;
		else
			-- move x
			if rec.x1 < rec.x2 then
				if rec.px1 = 4096 then
					node_id		:= rec.s_node;
					px			:= rec.px1 - 1;
					py			:= rec.py1;
					old_y		:= rec.y1;
				else
					node_id		:= rec.e_node;
					px			:= rec.px2 + 1;
					py			:= rec.py2;
					old_y		:= rec.y2;
				end if;
			else--if rec.x1 > rec.x2 then
				if rec.px1 = 4096 then
					node_id		:= rec.e_node;
					px			:= rec.px2 - 1;
					py			:= rec.py2;
					old_y		:= rec.y2;
				else
					node_id		:= rec.s_node;
					px			:= rec.px1 + 1;
					py			:= rec.py1;
					old_y		:= rec.y1;
				end if;
			end if;
		end if;
		
		-- calc new x/y
		select (pixel2world(rec.tz, rec.tx, rec.ty, px, py, rec.size)).* into x,y;
		select (world2lonlat(x,y)).* into x,y;
		if old_x is not null then 
			x	:= old_x;
		end if;
		if old_y is not null then 
			y	:= old_y;
		end if;
		
		insert into temp_move_shortlink_vertex_newpos
					(node_id, the_geom)
			values  (node_id, st_setsrid(st_point(x,y), 4326));
		
	end loop;
	return 1;
END;
$$;


CREATE OR REPLACE FUNCTION mid_split_circle_link(geom geometry)
  RETURNS geometry[]
  LANGUAGE plpgsql
AS $$
DECLARE
	sublink_array	geometry[];

BEGIN

	sublink_array		:= ARRAY[ST_Line_Substring(geom, 0, 0.5)];
	sublink_array		:= array_append(sublink_array, ST_Line_Substring(geom, 0.5, 1));
	
	return sublink_array;
END;
$$;


CREATE OR REPLACE FUNCTION rdb_get_split_linkrow_circle_dealing(start_node_id bigint, link_array bigint[], s_array bigint[], e_array bigint[])
  RETURNS bigint[]
  LANGUAGE plpgsql
AS $$
DECLARE
	new_linkrow	bigint[];
BEGIN
	select  array_agg(linkid) as new_link_array
	from
	(
		select	link_index, sub_index, split_link_array[sub_index] as linkid
		from
		(
			select	link_index, split_link_array, generate_series(1,sub_count) as sub_index
			from
			(
				select  link_index, split_link_array, array_upper(split_link_array,1) as sub_count
				from
				(
					select  link_num, 
							link_index, 
							(
							case 
							when link_index = 1 then ARRAY[rdb_get_split_link_circle_dealing(old_link_array[link_index], jnc_node_array[1])]
							when link_index = link_num then ARRAY[rdb_get_split_link_circle_dealing(old_link_array[link_index], jnc_node_array[link_index-1])]
							else rdb_get_split_link_array_circle_dealing(old_link_array[link_index], jnc_node_array[link_index-1], 0)
							end
							)as split_link_array
					from
					(
						select  link_num, old_link_array, jnc_node_array,
								generate_series(1,link_num) as link_index
						from
						(
							select  array_upper(link_array, 1) as link_num, 
									link_array as old_link_array, 
									rdb_get_junction_node_list(start_node_id, s_array, e_array) as jnc_node_array
						)as a
					)as b
				)as c
			)as d
		)as e
		order by link_index, sub_index
	)as f
	into new_linkrow;
	
	return new_linkrow;
END;
$$;


CREATE OR REPLACE FUNCTION rdb_get_split_link_circle_dealing(target_link_id bigint, adj_node_id bigint)
  RETURNS bigint
  LANGUAGE plpgsql
AS $$
DECLARE
	new_link_id		bigint;
BEGIN
	if adj_node_id is null then
		select link_id
		from temp_circle_link_new_seq_link
		where old_link_id = target_link_id and sub_index = sub_count
		into new_link_id;
	else
		select link_id
		from temp_circle_link_new_seq_link
		where old_link_id = target_link_id and adj_node_id in (s_node, e_node)
		into new_link_id;
	end if;
	
	if new_link_id is null then
		new_link_id	:= target_link_id;
	end if;
	
	return new_link_id;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_get_split_link_array_circle_dealing(target_link_id bigint, from_node_id bigint, bReverse integer)
  RETURNS bigint[]
  LANGUAGE plpgsql
AS $$
DECLARE
	new_link_array		bigint[];
	bDir				integer;
	strOrder			varchar;
BEGIN
	if from_node_id is null then
		bDir	:= 0;
	else
		select dir
		from
		(
			select 0 as dir
			from temp_circle_link_new_seq_link
			where (old_link_id = target_link_id) and (sub_index = 1) and (s_node = from_node_id)
			union
			select 1 as dir
			from temp_circle_link_new_seq_link
			where (old_link_id = target_link_id) and (sub_index = sub_count) and (e_node = from_node_id)
		)as a
		into bDir;
	end if;
	
	if bDir is null then
		new_link_array	:= ARRAY[target_link_id];
	else
		if bDir = bReverse then
			select array_agg(link_id) as link_array
			from
			(
				select old_link_id, link_id, sub_index
				from temp_circle_link_new_seq_link
				where old_link_id = target_link_id
				order by sub_index asc
			)as a
			group by old_link_id
			into new_link_array;
		else
			select array_agg(link_id) as link_array
			from
			(
				select old_link_id, link_id, sub_index
				from temp_circle_link_new_seq_link
				where old_link_id = target_link_id
				order by sub_index desc
			)as a
			group by old_link_id
			into new_link_array;
		end if;
	end if;
	
	return new_link_array;
END;
$$;


create or replace function mid_construct_substitude_link_array(old_link_array bigint[], substitude_link_array bigint[])
RETURNS bigint[]
LANGUAGE plpgsql
AS $$ 
DECLARE
	new_sub_link_array bigint[];
	old_link_cnt int;
	nindex int;
	current_old_link_id bigint;
	current_substituede_link_id bigint;
BEGIN
	current_old_link_id := 0;
	current_substituede_link_id := 0;
	
	old_link_cnt := array_upper(old_link_array,1);
	
	if old_link_cnt <= 2 then 
		return substitude_link_array;
	end if;

	for nindex in 1..old_link_cnt loop
		if current_substituede_link_id <> substitude_link_array[nindex] then
			new_sub_link_array = array_append(new_sub_link_array, substitude_link_array[nindex]);
		else
			if old_link_array[nindex] = current_old_link_id then
				new_sub_link_array = array_append(new_sub_link_array, substitude_link_array[nindex]);
			end if;
		end if;
		
		current_old_link_id := old_link_array[nindex];
		current_substituede_link_id := substitude_link_array[nindex];
	end loop;
	
	return new_sub_link_array;
END;
$$;


create or replace function mid_find_mainnode()
returns integer
language plpgsql
as $$
declare
	rec record;
	mainnode_id integer;
	search_link bigint;
	link_array bigint[];
	node_array bigint[];
	next_link_array bigint[];
	s_node_array bigint[];
	e_node_array bigint[];
begin
	mainnode_id	:= 0;
	for rec in select * from link_tbl where link_type = 4
	loop
		-- check
		select sublink 
		from temp_mainnode_sublink 
		where sublink = rec.link_id
		into search_link;

		if FOUND then
			continue;
		end if;
		
		--
		mainnode_id	:= mainnode_id + 1;

		link_array	:= ARRAY[rec.link_id];
		node_array	:= ARRAY[rec.s_node, rec.e_node];

		while True loop
			--raise INFO 'search node_array=%', node_array;
			select array_agg(link_id), array_agg(s_node), array_agg(e_node)
			from
			(
				select link_id, s_node, e_node
				from link_tbl
				where 		(s_node = ANY(node_array) or e_node = ANY(node_array)) 
						and (link_type = 4)
						and not (link_id = ANY(link_array))
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
				insert into temp_mainnode_sublink(mainnode_id, sublink)
				select mainnode_id, unnest(link_array);

				insert into temp_mainnode_subnode(mainnode_id, subnode)
				select mainnode_id, unnest(node_array);
				
				exit;
			end if;
		end loop;
	end loop;
	return 0;
end;
$$;

create or replace function mid_get_inner_path_array(link_array bigint[], node_id bigint)
returns varchar[]
language plpgsql
as $$
declare
	path_array varchar[];
	path_num integer;
	path_index integer;
	
	link_num integer;
	link_index integer;
	
	inner_linkrow_array varchar[];
	inner_num integer;
	inner_index integer;
	
	inlink bigint;
	outlink bigint;
	innode bigint;
	outnode bigint;

	current_node bigint;
	next_node bigint;
	next_link_type smallint;
	subnode_mainnode_id	bigint;
begin
	--raise INFO 'link_array = %, node_id = %', link_array, node_id;
	-- init
	inlink			:= link_array[1];
	innode			:= node_id;
	outnode			:= node_id;
	path_array		:= ARRAY[inlink::varchar];
	path_num		:= 1;

	-- find inner paths
	link_num		:= array_upper(link_array, 1);
	current_node	:= node_id;
	for link_index in 2..link_num loop
		-- set outlink
		outlink	:= link_array[link_index];
		
		if link_index < link_num then
			select (case when s_node = current_node then e_node else s_node end), link_type
			from link_tbl 
			where link_id = outlink
			into next_node, next_link_type;
			
			if next_link_type = 4 then
				current_node	:= next_node;
				outnode			:= next_node;
				continue;
			end if;
		end if;
		
		-- check if current node is mainnode
		select mainnode_id
		from temp_mainnode_subnode
		where subnode = current_node
		into subnode_mainnode_id;

		-- find inner path between two adj_link
		if not FOUND then
			-- not mainnode
			for path_index in 1..path_num loop
				path_array[path_index]	:= path_array[path_index] || ',' || outlink::varchar;
			end loop;
		else
			-- mainnode, then find inner path
			inner_linkrow_array	:= mid_find_inner_path(inlink, outlink, innode, outnode);
			inner_num			:= array_upper(inner_linkrow_array, 1);
			if inner_num = 0 or inner_num is null then
				-- inner path not found, which means linkrow is unwalkable.
				path_array	:= ARRAY[array_to_string(link_array, ',')];
				exit;
			else
				inner_index			:= inner_num;
				while inner_index >= 1 loop
					for path_index in 1..path_num loop
						--raise INFO 'path_num*(inner_num-1)+path_index = %', path_num*(inner_num-1)+path_index;
						if inner_linkrow_array[inner_index] = '' then
							path_array[path_num*(inner_index-1)+path_index]	:= path_array[path_index] || ',' || outlink::varchar;
						else
							path_array[path_num*(inner_index-1)+path_index]	:= path_array[path_index] || ',' || inner_linkrow_array[inner_index] || ',' || outlink::varchar;
						end if;
					end loop;
					inner_index	:= inner_index - 1;
				end loop;
				path_num	:= path_num * inner_num;
			end if;
		end if;

		inlink			:= outlink;
		innode			:= next_node;
		outnode			:= next_node;
		current_node	:= next_node;
	end loop;

	return path_array;
end;
$$;

create or replace function mid_find_inner_path(inlink bigint, outlink bigint, innode bigint, outnode bigint)
returns varchar[]
language plpgsql
as $$
declare
	rstPathArray		varchar[];
	rstPathCount		integer;
	rstPath				varchar;
	
	tmpPathArray		varchar[];
	tmpLastNodeArray	bigint[];
	tmpLastLinkArray	bigint[];
	tmpPathCount		integer;
	tmpPathIndex		integer;
	
	rec        			record;
begin	
	--rstPathArray
	rstPathCount		:= 0;
	tmpPathArray		:= ARRAY[''];
	tmpLastNodeArray	:= ARRAY[innode];
	tmpLastLinkArray	:= ARRAY[inlink];
	tmpPathCount		:= 1;
	tmpPathIndex		:= 1;

	-- search
	WHILE tmpPathIndex <= tmpPathCount LOOP
		--raise INFO '%', cast(tmpPathArray as varchar);
		for rec in 
			 select nextroad, dir, nextnode, link_type
			 from
			 (
				 select link_id as nextroad, '(2)' as dir, e_node as nextnode, link_type
				 from link_tbl
				 where	(s_node = tmpLastNodeArray[tmpPathIndex]) and 
						(one_way_code in (1,2))-- and (road_type not in (8,9))
				 union
				 
				 select link_id as nextroad, '(3)' as dir, s_node as nextnode, link_type
				 from link_tbl
				 where	(e_node = tmpLastNodeArray[tmpPathIndex]) and 
						(one_way_code in (1,3))-- and (road_type not in (8,9))
			 )as a
			 where (nextroad != tmpLastLinkArray[tmpPathIndex])	-- can not one-link-u-turn
			 order by nextroad, dir, nextnode
		loop
			if (rec.nextroad = outlink) and (tmpLastNodeArray[tmpPathIndex] = outnode) then
				rstPathCount		:= rstPathCount + 1;
				rstPath				:= tmpPathArray[tmpPathIndex];
				rstPath				:= replace(rstPath, '(2)', '');
				rstPath				:= replace(rstPath, '(3)', '');
				if rstPathCount = 1 then
					rstPathArray	:= ARRAY[rstPath];
				else
					rstPathArray	:= array_append(rstPathArray, rstPath);
				end if;
				continue;
			end if;
			
			if 	(rec.nextroad not in (inlink, outlink))
				and (rec.link_type = 4)
				and not ((rec.nextroad||rec.dir) = ANY(regexp_split_to_array(tmpPathArray[tmpPathIndex], E',+'))) 
			then
				tmpPathCount		:= tmpPathCount + 1;
				if tmpPathArray[tmpPathIndex] = '' then
					tmpPathArray	:= array_append(tmpPathArray, cast(rec.nextroad||rec.dir as varchar));
				else
					tmpPathArray	:= array_append(tmpPathArray, cast(tmpPathArray[tmpPathIndex]||','||rec.nextroad||rec.dir as varchar));
				end if;
				tmpLastNodeArray	:= array_append(tmpLastNodeArray, cast(rec.nextnode as bigint));
				tmpLastLinkArray	:= array_append(tmpLastLinkArray, cast(rec.nextroad as bigint));
			end if;
		end loop;
		tmpPathIndex := tmpPathIndex + 1;
	END LOOP;

	return rstPathArray;
end;
$$;


-- input format:  'THA', 'left_right', 'office_name', 'not_tts', 'name'
-- output format: '{"lang": "THA", "left_right": "left_right", "type": "office_name", "tts_type": "not_tts", "val": "name"}',
create or replace function mid_get_json_string_for_name(language varchar, left_right varchar, type varchar, tts_type varchar, value varchar)
	returns varchar
	language plpgsql
as $$
begin
	return '{"lang": "'||language||'", "left_right": "'||left_right||'", "type": "'||type||'", "tts_type": "'||tts_type||'", "val": "'||value||'"}';
end;
$$;

-- convert list to string
-- input format:  [a, b, c], output format: '[a, b, c]'
create or replace function mid_get_json_string_for_list(lang_name_list varchar[])
	returns varchar
	language plpgsql
as $$
begin
	return '['||array_to_string(lang_name_list,', ')||']';
end;
$$;

create or replace function mid_get_json_string_for_thailand_name(name_th varchar, name_en varchar)
	returns varchar
	language plpgsql
as $$
declare
	string_th		varchar;
	string_en		varchar;
	string_name		varchar;
	string_namelist	varchar;
begin
	string_th		:= mid_get_json_string_for_name('THA', 'left_right', 'office_name', 'not_tts', name_th);
	string_en		:= mid_get_json_string_for_name('THE', 'left_right', 'office_name', 'not_tts', name_en);
	string_name		:= mid_get_json_string_for_list(ARRAY[string_th,string_en]);
	string_namelist	:= mid_get_json_string_for_list(ARRAY[string_name]);
	return string_namelist;
end;
$$;
--------------------------------------------------------------
------------
CREATE OR REPLACE FUNCTION mid_get_expand_box(nLink1 bigint, 
                                              nLink2 bigint, 
                                              units_to_expand float)
  returns geometry
  language plpgsql
as $$
declare
        box_geom  geometry;
BEGIN
        select ST_Expand(exten_box::geometry, units_to_expand) into box_geom
        from (
                select st_envelope(ST_Collect(the_geom)) as exten_box
                FROM link_tbl
                WHERE link_id in (nLink1, nLink2)
        ) as a;
        
        IF FOUND THEN
                RETURN ST_SETSRID(box_geom, 4326);
        END IF;
        RETURN NULL;
end;
$$;

--------------------------------------------------------------
--
CREATE OR REPLACE FUNCTION mid_get_node_expand_box(nnode1 bigint, nnode2 bigint, units_to_expand double precision)
  returns geometry
  language plpgsql
as $$
declare
        box_geom  geometry;
BEGIN
        select ST_Expand(exten_box::geometry, units_to_expand) into box_geom
        from (
                select st_envelope(ST_Collect(the_geom)) as exten_box
                FROM node_tbl
                WHERE node_id in (nNode1, nNode2)
        ) as a;
        
        IF FOUND THEN
                RETURN ST_SETSRID(box_geom, 4326);
        END IF;
        RETURN NULL;
end;
$$;

--------------------------------------------------------------
--
CREATE OR REPLACE FUNCTION mid_check_guide_item_new(the_guide_tbl varchar)
	RETURNS integer
	LANGUAGE plpgsql
	AS $$ 
DECLARE
	rec				record;
	nIndex			integer;
	nCount			integer;
	nNextIndex		integer;
	error_count             integer;
BEGIN
        error_count = 0;
	for rec in
		execute
		'
		select	gid, nodeid,
				array_agg(linkid) as link_array, 
				array_agg(seq_num) as seq_array,
				array_agg(one_way_code) as oneway_array,
				array_agg(s_node) as snode_array,
				array_agg(e_node) as enode_array
		from
		(
			select m.gid, linkid, nodeid, seq_num, one_way_code, s_node, e_node
			from 
			(
				select    gid, nodeid, seq_num, link_array[seq_num] as linkid
                from
                (
                    select  gid, 
                            nodeid,
                            link_num,
                            link_array,
                            generate_series(1,link_num) as seq_num
                    from
                    (
                        select  gid, 
                                nodeid,
                                (passlink_cnt + 2) as link_num,
                                (
                                case 
                                when passlink_cnt = 0 then ARRAY[inlinkid, outlinkid]
                                else ARRAY[inlinkid] || cast(string_to_array(passlid, ''|'') as bigint[]) || ARRAY[outlinkid]
                                end
                                )as link_array
                        from ' ||the_guide_tbl|| '
                    )as a
                )as b
			) as m
			left join link_tbl as n
			on m.linkid = n.link_id
			order by gid, nodeid, seq_num
		)as a
		group by gid, nodeid
		'
	loop
		-- check target node
		if rec.nodeid is null then
                        error_count = error_count + 1;
			---raise EXCEPTION '% target node null_error: gid = %', the_guide_tbl, rec.gid;
		end if;
		
		if (rec.nodeid is distinct from rec.snode_array[1] and rec.nodeid is distinct from rec.enode_array[1])
		or (rec.nodeid is distinct from rec.snode_array[2] and rec.nodeid is distinct from rec.enode_array[2])
		then
                        error_count = error_count + 1;
			---raise EXCEPTION '% target node intersection_error: gid = %', the_guide_tbl, rec.gid;
		end if;
		
		-- check linkrow continuable
		nCount		:= array_upper(rec.seq_array, 1);
		nIndex		:= 1;
		while nIndex < nCount loop
			if rec.snode_array[nIndex] in (rec.snode_array[nIndex+1], rec.enode_array[nIndex+1]) then
				if rec.oneway_array[nIndex] in (2,4) then
                                        error_count = error_count + 1;
					---raise EXCEPTION '% linkrow error: traffic flow error, gid = %', the_guide_tbl, rec.gid;
				end if;
			elseif rec.enode_array[nIndex] in (rec.snode_array[nIndex+1], rec.enode_array[nIndex+1]) then
				if rec.oneway_array[nIndex] in (3,4) then
                                        error_count = error_count + 1;
					---raise EXCEPTION '% linkrow error: traffic flow error, gid = %', the_guide_tbl, rec.gid;
				end if;
			else
                                error_count = error_count + 1;
				---raise EXCEPTION '% linkrow error: links have no intersection, gid = %', the_guide_tbl, rec.gid;
			end if;
			
			nIndex		:= nIndex + 1;
		end loop;
	end loop;
	
	return error_count;
END;
$$;

CREATE OR REPLACE FUNCTION mid_findpasslinkbybothnodes(startnode bigint, endnode bigint, threshould_steps int default 10)
  RETURNS character varying
LANGUAGE plpgsql volatile
AS $$
DECLARE
	rstPathStr  		varchar;
	rstPathCount		integer;
	rstPath		        varchar;
	nFromLink               bigint;
	tmpPathArray		varchar[];
	tmpLastNodeArray	bigint[];
	tmpLastLinkArray	bigint[];
	tmpPathCount		integer;
	tmpPathIndex		integer;
	rec        		record;
	nStartNode              bigint;
	nEndNode              bigint;
BEGIN
	if startnode is null or endnode is null then 
		return null;
	end if;
	
	--rstPath
	nFromLink               := -1;
	rstPathCount		:= 0;
	tmpPathArray		:= ARRAY[cast(nFromLink as varchar)];
	tmpPathCount		:= 1;
	tmpPathIndex		:= 1;
	nStartNode          := startnode;
	nEndNode          := endnode;
	tmpLastNodeArray	:= ARRAY[nStartNode];
	tmpLastLinkArray	:= ARRAY[nFromLink];
	-- search
        WHILE tmpPathIndex <= tmpPathCount LOOP
                --raise INFO 'tmpPathArray = %', tmpPathArray[tmpPathIndex];
                -----stop if tmpPath has been more than layer given
                if array_upper(regexp_split_to_array(tmpPathArray[tmpPathIndex], E'\\,+'),1) < threshould_steps then
                        if tmpLastNodeArray[tmpPathIndex] = nEndNode then
                                rstPathCount	:= rstPathCount + 1;
                                rstPath		:= cast(tmpPathArray[tmpPathIndex] as varchar);
                                rstPath		:= replace(rstPath, '(2)', '');
                                rstPath		:= replace(rstPath, '(3)', '');
                                rstPathStr	:=  rstPath;
                                exit;
                                
                        else
                                for rec in 
                                        select e.nextroad,e.dir,e.nextnode
                                                from
                                                (
                                                        SELECT link_id as nextroad,'(2)' as dir,e_node as nextnode
                                                        FROM link_tbl
                                                        where s_node = tmpLastNodeArray[tmpPathIndex] and one_way_code in (1,2) 

                                                        union

                                                        SELECT link_id as nextroad,'(3)' as dir,s_node as nextnode
                                                        FROM link_tbl
                                                        where e_node = tmpLastNodeArray[tmpPathIndex] and one_way_code in (1,3)
                                                ) as e
                                loop
                                        
                                        if 		not (rec.nextroad in (nFromLink, tmpLastLinkArray[tmpPathIndex]))
                                                and	not ((rec.nextroad||rec.dir) = ANY(regexp_split_to_array(tmpPathArray[tmpPathIndex], E'\\,+')))
                                        then
                                                tmpPathCount		:= tmpPathCount + 1;
                                                tmpPathArray		:= array_append(tmpPathArray, cast(tmpPathArray[tmpPathIndex]||','||rec.nextroad||rec.dir as varchar));
                                                tmpLastNodeArray	:= array_append(tmpLastNodeArray, cast(rec.nextnode as bigint));
                                                tmpLastLinkArray	:= array_append(tmpLastLinkArray, cast(rec.nextroad as bigint));
                                        end if;
                                        
                                end loop;
                        end if;
                else
                        return null;
                end if;
                tmpPathIndex := tmpPathIndex + 1;
        END LOOP;
        if array_upper(regexp_split_to_array(rstPathStr,E'\\,+'),1) > 1 then
                return substring(rstPathStr,4);
        else
                return null;
        end if;
END;
$$;

create or replace function mid_get_caution_info(safety_type varchar)
  RETURNS varchar[]
LANGUAGE plpgsql volatile
AS $$
DECLARE
BEGIN
	return case safety_type when '1' then array['81','この先カーブが続きます。ご注意ください']
		 when '4' then array['82','ここからおよそ1km先までは、充分運転に注意してください']
		 when '5' then array['83','ここからおよそ2km先までは、充分運転に注意してください']
		 when '6' then array['84','ここからおよそ3km先までは、充分運転に注意してください']
		 when '7' then array['85','ここからおよそ4km先までは、充分運転に注意してください']
		 when '8' then array['86','ここからおよそ5km先までは、充分運転に注意してください']
		 when '9' then array['87','ここからおよそ6km先までは、充分運転に注意してください']
		 when 'A' then array['88','ここからおよそ7km先までは、充分運転に注意してください']
		 when 'B' then array['89','ここからおよそ8km先までは、充分運転に注意してください']
		 when 'C' then array['90','ここからおよそ9km先までは、充分運転に注意してください']
		 when 'D' then array['91','ここからおよそ10km先までは、充分運転に注意してください']
		END;	
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

CREATE OR REPLACE FUNCTION mid_transformlat(x numeric,y numeric)
  RETURNS numeric
  LANGUAGE plpgsql 
  AS $$
DECLARE
	ret numeric;
BEGIN
	ret = -100.0 + 2.0 * x + 3.0 * y + 0.2 * y * y + 0.1 * x * y + 0.2 * sqrt(abs(x));
	ret = ret+(20.0 * sin(6.0 * x * pi()) + 20.0 *sin(2.0 * x * pi())) * 2.0 / 3.0;
	ret = ret+(20.0 * sin(y * pi()) + 40.0 * sin(y / 3.0 * pi())) * 2.0 / 3.0;
	ret = ret+(160.0 * sin(y / 12.0 * pi()) + 320 * sin(y * pi() / 30.0)) * 2.0 / 3.0;
	return ret;
END;
$$;

CREATE OR REPLACE FUNCTION mid_transformlon(x numeric,y numeric)
  RETURNS numeric
  LANGUAGE plpgsql 
  AS $$
DECLARE
	ret numeric;
BEGIN
	ret = 300.0 + x + 2.0 * y + 0.1 * x * x + 0.1 * x * y + 0.1 * sqrt(abs(x));
	ret = ret+ (20.0 * sin(6.0 * x * pi()) + 20.0 * sin(2.0 * x * pi())) * 2.0 / 3.0;
	ret = ret+ (20.0 * sin(x * pi()) + 40.0 * sin(x / 3.0 * pi())) * 2.0 / 3.0;
	ret = ret+ (150.0 * sin(x / 12.0 * pi()) + 300.0 * sin(x / 30.0 * pi())) * 2.0 / 3.0;
	return ret;
END;
$$;

CREATE OR REPLACE FUNCTION mid_transWGS2GCJ_point(geom geometry)
  RETURNS geometry
  LANGUAGE plpgsql 
  AS $$
DECLARE
	dlat numeric;
	dlon numeric;
	radlat numeric;
	magic numeric;
	sqrtmagic numeric;
	elat numeric;
	elon numeric;
	ee numeric;
	a numeric;
	lon numeric;
	lat numeric;
BEGIN
	ee = 0.00669342162296594323;
	a = 6378245.0;
	lon=st_x(geom);
	lat=st_y(geom);
	dlat = mid_transformlat(lon-105.0,lat-35.0);
	dlon = mid_transformlon(lon-105.0,lat-35.0);
	radlat = lat/180.0*pi();
	magic = sin(radlat);
	magic = 1 - ee * magic * magic;
	sqrtmagic = sqrt(magic);
	dlat = (dlat * 180.0) / ((a * (1-ee))/ (magic*sqrtmagic)*pi());
	dlon = (dlon * 180.0) / (a / sqrtmagic * cos(radlat) * pi());
	elat = lat + dlat;
	elon = lon + dlon;
	return st_point(elon,elat);
END;
$$;

CREATE OR REPLACE FUNCTION mid_transGCJ2WGS_point(geom geometry)
  RETURNS geometry
  LANGUAGE plpgsql 
  AS $$
DECLARE
	wlat numeric;
	wlon numeric;
	dlat numeric;
	dlon numeric;	
	clat numeric;
	clon numeric;
	lon  numeric;
	lat  numeric;
BEGIN
	lon=st_x(geom);
	lat=st_y(geom);
	wlon =lon;
	wlat =lat;
	while True loop
		clon=st_x(mid_transWGS2GCJ_point(st_point(wlon,wlat)));
		clat=st_y(mid_transWGS2GCJ_point(st_point(wlon,wlat)));
		dlat=lat-clat;
		dlon=lon-clon;
		if (abs(dlat)<1e-7 and abs(dlon)<1e-7) then
		    return st_point(wlon,wlat);
		end if;
		wlat=wlat+dlat;
		wlon=wlon+dlon;
	end loop;
END;
$$;

CREATE OR REPLACE FUNCTION mid_transgcj2wgs(geom geometry)
  RETURNS geometry 
  LANGUAGE plpgsql 
  AS $$
DECLARE
	wkt text;
	substr1 text;
	substr2 text;
	strtmp text;
	result text;
	lon numeric;
	lat numeric;
	result_arr text[];
	point_tmp geometry;
	
BEGIN
	select st_astext(geom) into wkt;
	--raise info '%',wkt;
	strtmp = wkt;
	result_arr=array[''];
	while True loop
		
		select substring(str,1,pos-1),str1,substring(str,pos+len),str
		into substr1,substr2,strtmp
		from
		(
			select position(arr[1] in str) as pos,length(arr[1]) as len,str,arr[1] as str1 from
			(
				select regexp_matches(strtmp,E'[0-9\.]+ [0-9\.]+') as arr,strtmp as str
			) a
		)a;
		lon = substring(substr2,1,position(' ' in substr2)-1)::numeric;
		lat = substring(substr2,position(' ' in substr2)+1)::numeric;
		point_tmp=mid_transgcj2wgs_point(st_point(lon,lat));
		substr2=st_x(point_tmp)::text||' '||st_y(point_tmp)::text;
		result_arr=array_append(result_arr,substr1);
		result_arr=array_append(result_arr,substr2);
		if not strtmp ~ E'[0-9\.]+ [0-9\.]+' then
			result_arr=array_append(result_arr,strtmp);
			result=array_to_string(result_arr,'');
			return st_geomfromtext(result,4326);
		end if;
	end loop;
END;
$$;

CREATE OR REPLACE FUNCTION mid_update_temp_force_guide_patch_node_tbl( )
	RETURNS boolean
	LANGUAGE plpgsql volatile
AS $$
DECLARE
	rec record;
	z_level_list_len bigint;
	geom_list_len bigint;
	idx bigint;
	deata double precision;
	res_node_list bigint[];
	temp_node bigint;
	temp_geom geometry;
	distance double precision;
BEGIN
	for rec in 
		select guide_type, (string_to_array(geom_text, '|'))::geometry[] as the_geom_list, (string_to_array(z_text, '|'))::smallint[] as z_level_list
		from temp_force_guide_patch_tbl
	loop
		geom_list_len := array_upper(rec.the_geom_list, 1);
		z_level_list_len := array_upper(rec.z_level_list, 1);
		idx := 1;
		deata := 0.03;

		if geom_list_len != z_level_list_len then
			raise EXCEPTION '% the_geom list length error: not equal to % z_level list length', rec.the_geom_list, rec.z_level_list;
		end if;
		
		while idx <= geom_list_len loop
			select node_id, the_geom, ST_Distance_Sphere(the_geom, (rec.the_geom_list)[idx]) as node_distance into temp_node, temp_geom, distance
			from node_tbl
			where z_level = (rec.z_level_list)[idx] and ST_DWithin(the_geom, (rec.the_geom_list)[idx], deata, True)
			order by node_distance
			limit 1;

			if temp_node is null then
				deata := deata + 0.2;
				
				if deata >= 10.0 then
					raise EXCEPTION 'the_geom: %s z_level: %s can not bound to node', (rec.the_geom_list)[idx], (rec.z_level_list)[idx];
				else
					continue;
				end if;
			else
				if idx = 1 then
					res_node_list := Array[temp_node];
				else
					res_node_list := array_append(res_node_list, temp_node);
				end if;

				idx := idx + 1;
				deata := 0.03;
			end if;
		end loop;
		
		insert into temp_force_guide_patch_node_tbl(guide_type, node_id_list)
		values(rec.guide_type, res_node_list);
	end loop;

	return true;
END;
$$;

CREATE OR REPLACE FUNCTION mid_update_temp_force_guide_patch_link_tbl( )
	RETURNS boolean
	LANGUAGE plpgsql volatile
AS $$
DECLARE
	rec record;
	node_list_len bigint;
	node_idx bigint;
	in_node bigint;
	out_node bigint;
	res_string character varying;
	temp_link_array bigint[];
	res_link_array bigint[];
	res_link_array_len bigint;
BEGIN	
	for rec in 
		select *
		from temp_force_guide_patch_node_tbl
	loop
		node_idx := 1;
		node_list_len := array_upper(rec.node_id_list, 1);
		if node_list_len < 2 then
			raise EXCEPTION 'temp_force_guide_patch_node_tbl node_id_list % length error', rec.node_id_list;
		end if;

		while node_idx <= (node_list_len - 1) loop
			in_node := (rec.node_id_list)[node_idx];
			out_node := (rec.node_id_list)[node_idx + 1];
			res_string := mid_findpasslinkbybothnodes(in_node, out_node, 30);
			if res_string is null then
				raise EXCEPTION 'in_node % out_node % can not calc path', in_node, out_node;
			end if;
			
			temp_link_array := string_to_array(res_string, ',');
			if node_idx = 1 then
				res_link_array := Array[temp_link_array[array_upper(temp_link_array, 1)]];
			else
				res_link_array := array_cat(res_link_array, temp_link_array);
			end if;
			--raise info '%', temp_link_array;
			--raise info '%', res_link_array;
			node_idx := node_idx + 1;
		end loop;
		
		res_link_array_len := array_upper(res_link_array, 1);
		if res_link_array_len >= 2 then
			insert into temp_force_guide_patch_link_tbl(objectid, guide_type, link_id_list)
			values(rec.id, rec.guide_type, res_link_array);
		else
			raise EXCEPTION 'node list % convert to link list % error', rec.node_id_list, res_link_array;
		end if;
	end loop;

	return true;
END;
$$;

CREATE OR REPLACE FUNCTION mid_get_connectnode_by_links( inlink bigint, outlink bigint )
	RETURNS bigint
	LANGUAGE plpgsql volatile
AS $$
DECLARE
	start_node_id bigint;
	end_node_id bigint;
	temp_link_id bigint;
	res_node_id bigint;
BEGIN	
	if inlink is null or outlink is null then
		return -1;
	end if;

	select a.s_node, a.e_node, b.link_id into start_node_id, end_node_id, temp_link_id
	from (
		select link_id, s_node, e_node
		from link_tbl
		where link_id = inlink
	) a
	left join (
		select link_id, s_node, e_node
		from link_tbl
		where link_id = outlink
	) b
	on a.s_node = b.s_node or a.s_node = b.e_node;

	if temp_link_id is not null then
		res_node_id := start_node_id;
	else 
		res_node_id := end_node_id;
	end if;

	return res_node_id;
END;
$$;

CREATE OR REPLACE FUNCTION mid_get_position_type_by_links( inlink bigint, outlink bigint )
	RETURNS smallint
	LANGUAGE plpgsql volatile
AS $$
DECLARE
	linkType1 smallint;
	roadType1 smallint;
	linkType2 smallint;
	roadType2 smallint;
	res_position_type smallint;
BEGIN	
	if inlink is null or outlink is null then
		return -1;
	end if;

	select link_type, road_type into linkType1, roadType1
	from link_tbl
	where link_id = inlink;

	select link_type, road_type into linkType2, roadType2
	from link_tbl
	where link_id = outlink;
	
	if roadType1 in (0, 1) and linkType1 in (1, 2) and linkType2 = 5 then
		res_position_type := 5; --gao su chu kou
	elseif roadType1 not in (0, 1) and linkType2 = 5 and roadType2 in (0, 1) then
		res_position_type := 4; --gao su ru kou
	elseif roadType1 in (0, 1) and linkType1 in (1, 2) and roadType2 in (0, 1) and linkType2 = 3 then
		res_position_type := 6; --gao su ben xian fen qi
	elseif roadType1 in (0, 1) and linkType1 = 3 and roadType2 in (0, 1) and linkType2 in (1, 2) then
		res_position_type := 7; --gao su ben xian he liu
	else
		res_position_type := 0;
	end if;
	
	return res_position_type;
END;
$$;

CREATE OR REPLACE FUNCTION mid_adjust_guidence_node(old_node_id bigint, old_link_array bigint[], guide_table_name varchar,
													out new_nodeid bigint, out new_inlinkid bigint,out new_passlid varchar)
	LANGUAGE plpgsql volatile
AS $$
DECLARE
	nCount				integer;
	nIndex				integer;
	search_link_id		bigint;
	search_link_type	smallint;
	search_s_node		bigint;
	search_e_node		bigint;
	search_node			bigint;
	search_node_branch	integer;
	search_gid			integer;
BEGIN
	nCount			= array_upper(old_link_array, 1);
	search_node		= old_node_id;
	nIndex			= 2;
	while nIndex <= nCount loop
		-- if guideninfo exists on this node, stay at old position and exit 
		if nIndex > 2 then
			execute
			'
			select gid
			from '|| guide_table_name || '
			where nodeid = ' || search_node || '
			limit 1
			'
			into search_gid;
			
			if search_gid is not null then
				nIndex = 2;
				exit;
			end if;
		end if;
		
		-- if node branch, exit
		select array_upper(string_to_array(node_lid, '|'), 1)
		from node_tbl
		where node_id = search_node
		into search_node_branch;
		
		if search_node_branch > 2 then
			exit;
		end if;
		
		-- if inner link, exit
		select link_id, link_type, s_node, e_node
		from link_tbl
		where link_id = old_link_array[nIndex]
		into search_link_id, search_link_type, search_s_node, search_e_node;
		
		if search_link_type = 4 then
			exit;
		end if;
		
		-- search next
		nIndex	= nIndex + 1;
		if search_s_node = search_node then
			search_node	= search_e_node;
		else
			search_node = search_s_node;
		end if;
	end loop;
	
	if nIndex in (2, nCount+1) then
		new_nodeid		= old_node_id;
		new_inlinkid	= old_link_array[1];
		new_passlid		= array_to_string(old_link_array[2:nCount-1], '|');
	else
		new_nodeid		= search_node;
		new_inlinkid	= old_link_array[nIndex-1];
		new_passlid		= array_to_string(old_link_array[nIndex:nCount-1], '|');
	end if;
END;
$$;
