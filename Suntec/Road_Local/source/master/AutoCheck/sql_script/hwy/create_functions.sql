CREATE OR REPLACE FUNCTION check_is_hwy_sapa_link(nHwyNode bigint,dir integer)
	RETURNS smallint
	LANGUAGE plpgsql volatile
AS $$
DECLARE
	rec                     record;
	
	tmpLastNodeArray	bigint[];
	tmpPathCount		integer;
	tmpPathIndex		integer;
	
BEGIN	
	--rstPathArray
	tmpLastNodeArray	:= ARRAY[nHwyNode];
	tmpPathCount		:= 1;
	tmpPathIndex		:= 1;

	-- search
	WHILE tmpPathIndex <= tmpPathCount LOOP	
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
							( s_node = tmpLastNodeArray[tmpPathIndex] and
							  one_way_code in (1,2) and
							  (not e_node = ANY(tmpLastNodeArray))
							)
							or
							( e_node = tmpLastNodeArray[tmpPathIndex] and
							  one_way_code in (1,3) and
							  (not s_node =  ANY(tmpLastNodeArray))
							 )
						)
					)
					or
					(
						(dir = 2)
						and
						(
							(s_node = tmpLastNodeArray[tmpPathIndex] and
							 one_way_code in (1,3) and
							 (not e_node = ANY(tmpLastNodeArray))
							 )
							or
							(e_node = tmpLastNodeArray[tmpPathIndex] and
							 one_way_code in (1,2) and
							 (not s_node =  ANY(tmpLastNodeArray))
							 )
						)
					)
				)

		loop	
			raise INFO 'nextnode = %', rec.nextnode;	
			-- no proper connected link, here is a complete path
			if rec.link_type  in(1,2)then
				if rec.road_type = 0 then
					return 1;
				else
					return 0;
				end if;
				continue;
			-- find an available link, record it as temp path and go on searching
			else
				tmpPathCount		:= tmpPathCount + 1;
				tmpLastNodeArray	:= array_append(tmpLastNodeArray, cast(rec.nextnode as bigint));
			end if;
		end loop;

		tmpPathIndex := tmpPathIndex + 1;
	END LOOP;

	return 0;
END;
$$;

CREATE OR REPLACE FUNCTION check_sapa_link()
	RETURNS smallint
	LANGUAGE plpgsql volatile
AS $$
DECLARE
	rec record;
	a integer;
BEGIN
	for rec in
		select link_id, s_node, e_node, one_way_code, link_type, road_type, function_class
		from link_tbl
		where link_type = 7
	loop
		if rec.one_way_code in (1,2) then
			if check_is_hwy_sapa_link(rec.e_node, 1) = 1 or
			   check_is_hwy_sapa_link(rec.s_node, 2) = 1 then
				insert into check_hwy_sapa_link(link_id, link_type, road_type)
				values(rec.link_id, rec.link_type, rec.road_type);
				continue;
			end if;

		end if;

		if rec.one_way_code in (1,3) then
			if  check_is_hwy_sapa_link(rec.s_node, 1) = 1 or 
			    check_is_hwy_sapa_link(rec.e_node, 2) = 1 then
				insert into check_hwy_sapa_link(link_id, link_type, road_type)
				values(rec.link_id, rec.link_type, rec.road_type);
				continue;
			end if;

		end if;

	end loop;
	
	RETURN 1;
END;
$$;
