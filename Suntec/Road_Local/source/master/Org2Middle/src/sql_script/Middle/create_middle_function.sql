
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


CREATE OR REPLACE FUNCTION mid_cnv_ramp_atnode_bak(nHwyNode bigint, nRoadTypeA integer, nFunctionClassA smallint, dir integer)
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
	tmp_link_type       integer;

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
			--raise INFO 'nextnode = %', rec.nextnode;
			tmpPath		:= tmpPathArray[tmpPathIndex];
		
			-- no proper connected link, here is a complete path
			if not (
				        (rec.road_type in (0, 1, 2, 3, 4, 5, 6, 14)) 
				            and
				        (
				            rec.link_type in (3,5) or
				            (
				                rec.link_type=0 and    
				                    rec.nextlink in (select link_id from temp_roundabout_for_searchramp)
				            )
				                    
				        )
				    )  then
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
						select link_type into tmp_link_type from link_tbl where link_id=link_array[nIndex];
						if tmp_link_type<>0 then 
						insert into temp_link_ramp_single_path(link_id, new_road_type, new_fc) 
							values(link_array[nIndex], nRoadType, nFunctionClass);
						else 
							insert into temp_roundabout_road_type(roundabout_id,new_road_type)
							select roundabout_id,nRoadType 
							from temp_roundabout_for_searchramp
							where link_id=link_array[nIndex];
						end if;
						
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


CREATE OR REPLACE FUNCTION mid_cnv_ramp_atnode(nHwyNode bigint, nRoadTypeA integer, nFunctionClassA smallint, dir integer)
	RETURNS smallint
	LANGUAGE plpgsql volatile
AS $$
DECLARE
	rec        			  record;
	bRoadEnd			    boolean;
	nRoadType			    smallint;
	nFunctionClass		smallint;
	rstPath				    varchar;
	tmpPath				    varchar;
	
	tmpPathArray		  varchar[];
	tmpLastNodeArray	bigint[];
	tmpPathCount		  integer;
	tmpPathIndex		  integer;
		
	nIndex				    integer;
	nCount				    integer;
	link_array 			  bigint[];
	count             integer;
	tmp_link_type     integer;
	LinkType          integer;   

BEGIN	
	--rstPathArray
	tmpPathArray		:= ARRAY[null];
	tmpLastNodeArray	:= ARRAY[nHwyNode];
	tmpPathCount		:= 1;
	tmpPathIndex		:= 1;
	LinkType        := 5;
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
			--raise INFO 'nextnode = %', rec.nextnode;
			tmpPath		:= tmpPathArray[tmpPathIndex];
		
			-- no proper connected link, here is a complete path
			if not (
				        (rec.road_type in (0, 1, 2, 3, 4, 5, 6, 14)) 
				            and
				        (
				            rec.link_type in (3,5) or
				            (
				                rec.link_type=0 and    
				                    rec.nextlink in (select link_id from temp_roundabout_for_searchramp)
				            )
				                    
				        )
				    )  then
				    
            if tmpPath is not null then
                rstPath			:= tmpPath;

                if rec.road_type < nRoadTypeA then
                    nRoadType	:= rec.road_type;
                    nFunctionClass	:= rec.function_class;
                else
                    nRoadType	:= nRoadTypeA;
                    nFunctionClass	:= nFunctionClassA;
                end if;

                if rec.road_type in (0,1) and rec.link_type in (1,2) then
                   LinkType := 3;
                else 
                   LinkType := 5;
                end if;
					
              --if rec.link_type > nLinkTypeA then
                --nLinkType	:= rec.link_type;
              --else
               -- nLinkType	:= nLinkTypeA;
              --end if;
				
            link_array	:= cast(regexp_split_to_array(rstPath, E'\\|+') as bigint[]);
            nCount		:= array_upper(link_array, 1);
            if LinkType = 3 then
                insert into temp_jct_link_paths(snode,path,elink)
                   values(nHwyNode,link_array,link_array[nCount]);
            end if;
            --insert into temp_ic_jct_link_paths(links)
            --  values(link_array);
            for nIndex in 1..nCount loop
                select link_type into tmp_link_type from link_tbl where link_id=link_array[nIndex];
                
                if tmp_link_type<>0 then 
                    insert into temp_link_ramp_single_path(link_id, new_road_type, new_fc, new_link_type) 
                      values(link_array[nIndex], nRoadType, nFunctionClass,LinkType);
                else 
                    insert into temp_roundabout_road_type(roundabout_id,new_road_type)
                        select roundabout_id,nRoadType 
                        from temp_roundabout_for_searchramp
                        where link_id=link_array[nIndex];
                end if;

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
      LinkType  := 5;
      rstPath		:= tmpPathArray[tmpPathIndex];
      if rstPath is not null then
          link_array	:= cast(regexp_split_to_array(rstPath, E'\\|+') as bigint[]);
          nCount		:= array_upper(link_array, 1);
          --insert into temp_ic_jct_link_paths(links)
              --values(link_array);
          for nIndex in 1..nCount loop
              insert into temp_link_ramp_single_path(link_id, new_road_type, new_fc,new_link_type) 
                  values(link_array[nIndex], nRoadTypeA, nFunctionClassA,LinkType);
          end loop;
      end if;
  end if;

  tmpPathIndex := tmpPathIndex + 1;
  tmpPath		:= tmpPathArray[tmpPathIndex];
 END LOOP;

 return 1;
END;
$$;


CREATE OR REPLACE FUNCTION mid_cnv_ramp_atnode_bak2(nHwyNode bigint, nRoadTypeA integer, nFunctionClassA smallint, dir integer)
	RETURNS smallint
	LANGUAGE plpgsql volatile
AS $$
DECLARE
	rec        			  record;
	bRoadEnd			    boolean;
	nRoadType			    smallint;
	nFunctionClass		smallint;
	rstPath				    varchar;
	tmpPath				    varchar;
	
	tmpPathArray		  varchar[];
	tmpLastNodeArray	bigint[];
	tmpPathCount		  integer;
	tmpPathIndex		  integer;
		
	nIndex				    integer;
	nCount				    integer;
	link_array 			  bigint[];
	count             integer;
	tmp_link_type     integer;
	LinkType          integer;   

BEGIN	
	--rstPathArray
	tmpPathArray		:= ARRAY[null];
	tmpLastNodeArray	:= ARRAY[nHwyNode];
	tmpPathCount		:= 1;
	tmpPathIndex		:= 1;
	LinkType        := 5;
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
			--raise INFO 'nextnode = %', rec.nextnode;
			tmpPath		:= tmpPathArray[tmpPathIndex];
		
			-- no proper connected link, here is a complete path
			if not (
				        (rec.road_type in (0, 1, 2, 3, 4, 5, 6, 14)) 
				            and
				        (
				            rec.link_type in (3,5,7) or
				            (
				                rec.link_type=0 and    
				                    rec.nextlink in (select link_id from temp_roundabout_for_searchramp)
				            )
				                    
				        )
				    )  then
				    
            if tmpPath is not null then
                rstPath			:= tmpPath;
                if rec.link_type <> 7 then
                if rec.road_type < nRoadTypeA then
                    nRoadType	:= rec.road_type;
                    nFunctionClass	:= rec.function_class;
                else
                    nRoadType	:= nRoadTypeA;
                    nFunctionClass	:= nFunctionClassA;
                end if;
                end if;

                if rec.road_type in (0,1) and rec.link_type in (1,2) then
                   LinkType := 3;
                else 
                   LinkType := 5;
                end if;
					
              --if rec.link_type > nLinkTypeA then
                --nLinkType	:= rec.link_type;
              --else
               -- nLinkType	:= nLinkTypeA;
              --end if;
				
            link_array	:= cast(regexp_split_to_array(rstPath, E'\\|+') as bigint[]);
            nCount		:= array_upper(link_array, 1);
            if LinkType = 3 then
                insert into temp_jct_link_paths(snode,path,elink)
                   values(nHwyNode,link_array,link_array[nCount]);
            end if;
            --insert into temp_ic_jct_link_paths(links)
            --  values(link_array);
            for nIndex in 1..nCount loop
                select link_type into tmp_link_type from link_tbl where link_id=link_array[nIndex];
                
                if tmp_link_type<>0 and tmp_link_type <> 7  then 
                    insert into temp_link_ramp_single_path(link_id, new_road_type, new_fc, new_link_type) 
                      values(link_array[nIndex], nRoadType, nFunctionClass,LinkType);
                end if;
                if tmp_link_type = 0 then 
                    insert into temp_roundabout_road_type(roundabout_id,new_road_type)
                        select roundabout_id,nRoadType 
                        from temp_roundabout_for_searchramp
                        where link_id=link_array[nIndex];
                end if;

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
      LinkType  := 5;
      rstPath		:= tmpPathArray[tmpPathIndex];
      if rstPath is not null then
          link_array	:= cast(regexp_split_to_array(rstPath, E'\\|+') as bigint[]);
          nCount		:= array_upper(link_array, 1);
          --insert into temp_ic_jct_link_paths(links)
              --values(link_array);
          for nIndex in 1..nCount loop
              insert into temp_link_ramp_single_path(link_id, new_road_type, new_fc,new_link_type) 
                  values(link_array[nIndex], nRoadTypeA, nFunctionClassA,LinkType);
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
		left join temp_link_ramp b on a.link_id = b.link_id
        where a.link_type in (3,5) and a.road_type in (0,1) and a.one_way_code<>4
        and b.link_id is null 
		
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
				extend_flag, bypass_flag, matching_flag, highcost_flag, ipd, urban, erp, rodizio, soi,
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
						bypass_flag, matching_flag, highcost_flag, ipd, urban, erp, rodizio, soi, extend_flag, 
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
						bypass_flag, matching_flag, highcost_flag, ipd, urban, erp, rodizio, soi, extend_flag, 
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
					and	not (rec.soi is distinct from rec2.soi)									
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
	nSubIndexForward			integer;
	nSubIndexBackward			integer;
BEGIN
	-- search backward
	-- if tile of the backward sublinks is different, proxy point must be on the tile border.
	nSubIndexBackward	:= sub_index - 1;
	while nSubIndexBackward > 0 loop
		if x_must is null and sub_tx_array[nSubIndexBackward] != sub_tx_array[sub_index] then
			x_must			:= st_x(st_endpoint(sub_geom_array[nSubIndexBackward]));
		end if;
		if y_must is null and sub_ty_array[nSubIndexBackward] != sub_ty_array[sub_index] then
			y_must			:= st_y(st_endpoint(sub_geom_array[nSubIndexBackward]));
		end if;
			
		if not delete_flag_array[nSubIndexBackward] then
			x_proxy				:= st_x(st_endpoint(sub_geom_array[nSubIndexBackward]));
			y_proxy				:= st_y(st_endpoint(sub_geom_array[nSubIndexBackward]));
			exit;
		end if;
		nSubIndexBackward	:= nSubIndexBackward - 1;
	end loop;
	
	-- search forward
	-- if tile of the forward sublinks is different, proxy point must be on the tile border.
	nSubIndexForward	:= sub_index + 1;
	while nSubIndexForward <= sub_count loop
		if x_must is null and sub_tx_array[nSubIndexForward] != sub_tx_array[sub_index] then
			x_must			:= st_x(st_startpoint(sub_geom_array[nSubIndexForward]));
		end if;
		if y_must is null and sub_ty_array[nSubIndexForward] != sub_ty_array[sub_index] then
			y_must			:= st_y(st_startpoint(sub_geom_array[nSubIndexForward]));
		end if;
			
		if not delete_flag_array[nSubIndexForward] then
			x_proxy				:= st_x(st_startpoint(sub_geom_array[nSubIndexForward]));
			y_proxy				:= st_y(st_startpoint(sub_geom_array[nSubIndexForward]));
			exit;
		end if;
		nSubIndexForward	:= nSubIndexForward + 1;
	end loop;
	
	-- proxy point is prefered to set as the node point of link.
	if nSubIndexBackward = 0 then
		x_proxy				:= st_x(st_startpoint(sub_geom_array[1]));
		y_proxy				:= st_y(st_startpoint(sub_geom_array[1]));
	end if;
	if nSubIndexForward > sub_count then
		x_proxy				:= st_x(st_endpoint(sub_geom_array[sub_count]));
		y_proxy				:= st_y(st_endpoint(sub_geom_array[sub_count]));
	end if;
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
	return case safety_type when '1' then array['81','�����ȥ��`�֤��A���ޤ�����ע�⤯������']
		 when '4' then array['82','�������餪�褽1km�ȤޤǤϡ�����\ܞ��ע�⤷�Ƥ�������']
		 when '5' then array['83','�������餪�褽2km�ȤޤǤϡ�����\ܞ��ע�⤷�Ƥ�������']
		 when '6' then array['84','�������餪�褽3km�ȤޤǤϡ�����\ܞ��ע�⤷�Ƥ�������']
		 when '7' then array['85','�������餪�褽4km�ȤޤǤϡ�����\ܞ��ע�⤷�Ƥ�������']
		 when '8' then array['86','�������餪�褽5km�ȤޤǤϡ�����\ܞ��ע�⤷�Ƥ�������']
		 when '9' then array['87','�������餪�褽6km�ȤޤǤϡ�����\ܞ��ע�⤷�Ƥ�������']
		 when 'A' then array['88','�������餪�褽7km�ȤޤǤϡ�����\ܞ��ע�⤷�Ƥ�������']
		 when 'B' then array['89','�������餪�褽8km�ȤޤǤϡ�����\ܞ��ע�⤷�Ƥ�������']
		 when 'C' then array['90','�������餪�褽9km�ȤޤǤϡ�����\ܞ��ע�⤷�Ƥ�������']
		 when 'D' then array['91','�������餪�褽10km�ȤޤǤϡ�����\ܞ��ע�⤷�Ƥ�������']
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



CREATE OR REPLACE FUNCTION mid_find_roundabout()
 RETURNS smallint
	LANGUAGE plpgsql volatile
AS $$
DECLARE
	bool boolean;
	rec record;
	road_type smallint;
	roundabout_id integer;
	cnt   int;
BEGIN
	roundabout_id=0;
	while True loop
		select * into rec from temp_roundabout;
		if not found then
			exit;
		end if;
		select count(1) into cnt from temp_roundabout;
		raise info '%',cnt;
		execute 'delete from temp_single_roundabout where 1=1';
		execute '
		insert into temp_single_roundabout(link_id,s_node,e_node,the_geom)
		select link_id,s_node,e_node,the_geom
		from temp_roundabout 
		where link_id='||(rec.link_id)::text;
		execute 'delete from temp_roundabout a using temp_single_roundabout b where a.link_id=b.link_id';
		while True loop
			select b.* into rec from temp_single_roundabout a
			join link_tbl b
			on a.s_node=b.s_node or a.s_node=b.e_node or a.e_node=b.s_node or a.e_node=b.e_node
			left join temp_single_roundabout c
			on b.link_id=c.link_id
			where b.link_type=0 and c.link_id is null;
			if not found then
				exit;
			end if;
			execute '
			insert into temp_single_roundabout(link_id,s_node,e_node,the_geom)
			select a.link_id,a.s_node,a.e_node,a.the_geom
			from link_tbl a
			join temp_single_roundabout b
			on a.s_node=b.s_node or a.s_node=b.e_node or a.e_node=b.s_node or a.e_node=b.e_node
			left join temp_single_roundabout c
			on a.link_id=c.link_id
			where a.link_type=0 and c.link_id is null;';	
			execute 'delete from temp_roundabout a using temp_single_roundabout b where a.link_id=b.link_id';
		end loop;
		select (
			select count(1) from temp_single_roundabout
			)=
			(select count(distinct(node_id)) 
				from 
				(
					select s_node as node_id from temp_single_roundabout 
					union 
					select e_node as node_id from temp_single_roundabout
				) 
				a
			)
		into bool;
		if not bool then
			continue;
		end if;
		roundabout_id=roundabout_id+1;
		insert into temp_roundabout_for_searchramp(roundabout_id,link_id,s_node,e_node,the_geom)
		select roundabout_id,link_id,s_node,e_node,the_geom
		from temp_single_roundabout;
		execute '
			delete from temp_roundabout_for_searchramp a
			using
			(
				select distinct roundabout_id
				from temp_roundabout_for_searchramp a
				join link_tbl b
				on a.s_node=b.s_node or a.s_node=b.e_node or a.e_node=b.s_node or a.e_node=b.e_node
				where b.link_type<>0 and b.road_type not in (0,1) and b.link_type not in (3,5)
			) b
			where a.roundabout_id=b.roundabout_id;
			';
	end loop;
	return 0;
END;
$$;

CREATE OR REPLACE FUNCTION mid_get_inner_link_count() 
  RETURNS  bigint
  LANGUAGE plpgsql
AS $$
DECLARE
        rowCount bigint;
BEGIN 
        rowCount := 0;

        select count(*) into rowCount
        from temp_caution_extend_link_tbl
        where outlink_type = 4;

    return rowCount;
END;
$$;

CREATE OR REPLACE FUNCTION mid_out_link_extend() 
  RETURNS  bigint
  LANGUAGE plpgsql
AS $$
DECLARE
        rec record;
BEGIN 
        while mid_get_inner_link_count() > 0 loop
		for rec in
			select *
			from temp_caution_extend_link_tbl
			where outlink_type = 4
		loop
			--raise info 'rec = %', rec;
			insert into temp_caution_extend_link_tbl(inlinkid, nodeid, outlinkid, outlink_type, routelnklst, routenodelst, nxt_node)
			select rec.inlinkid, rec.nodeid, link_id as outlinkid, link_type as outlink_type,
				array_append(rec.routelnklst, link_id) as routelnklst, 
				array_append(rec.routenodelst, rec.nxt_node) as routenodelst,
				(case when rec.nxt_node = s_node then e_node else s_node end) as nxt_node
			from link_tbl
			where ((rec.nxt_node = s_node and one_way_code in (1, 2)) or 
				(rec.nxt_node = e_node and one_way_code in (1, 3))) and
				not (rec.nxt_node = any(rec.routenodelst)) and
				not (link_id = any(rec.routelnklst));

			delete from temp_caution_extend_link_tbl where id = rec.id;
			--exit;
		end loop;
        end loop;

    return 0;
END;
$$;

CREATE OR REPLACE FUNCTION mid_get_pass_node(node bigint, passlink_cnt smallint, passlid varchar) 
  RETURNS  bigint[]
  LANGUAGE plpgsql
AS $$
DECLARE
        passLnkLop smallint;
        nxt_node bigint;
        snode bigint;
        enode bigint;
        node_array bigint[];
		passLnkLst bigint[];
BEGIN 
	nxt_node := node;
	node_array := array[nxt_node];
	
	if 0 = passlink_cnt then
		return node_array;
	else 
		passLnkLst := string_to_array(passlid, '|');
	end if;
	
	for passLnkLop in 1..passlink_cnt loop
		select s_node, e_node into snode, enode
		from link_tbl
		where link_id = passLnkLst[passLnkLop];

		if snode = nxt_node then
			nxt_node := enode;
		else
			nxt_node := snode;
		end if;
		
		node_array := array_append(node_array, nxt_node);
	end loop;

    return node_array;
END;
$$;

CREATE OR REPLACE FUNCTION mid_get_nxt_node(node bigint, outlink bigint, passlink_cnt smallint, passlid varchar) 
  RETURNS  bigint
  LANGUAGE plpgsql
AS $$
DECLARE
        passLnkLop smallint;
        nxt_node bigint;
        snode bigint;
        enode bigint;
		passLnkLst bigint[];
BEGIN 
	nxt_node := node;
	
	if 0 != passlink_cnt then
		passLnkLst := string_to_array(passlid, '|');
	end if;
	
	-- pass link 
	for passLnkLop in 1..passlink_cnt loop
		select s_node, e_node into snode, enode
		from link_tbl
		where link_id = passLnkLst[passLnkLop];

		if snode = nxt_node then
			nxt_node := enode;
		else
			nxt_node := snode;
		end if;
	end loop;
	
	select s_node, e_node into snode, enode
	from link_tbl
	where link_id = outlink;
	
	-- out link
	if snode = nxt_node then
		nxt_node := enode;
	else 
		nxt_node := snode;
	end if;

    return nxt_node;
END;
$$;

-------------------------------------------------------------------------------------------------------------
-- get start/end node of a link
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_get_s_e_node(nlinkid bigint)
  RETURNS bigint[]
  LANGUAGE plpgsql AS
$$
DECLARE
	n_s_node  bigint;
	e_s_node  bigint;
BEGIN
	n_s_node = NULL;
	e_s_node = NULL;
	select s_node, e_node into n_s_node, e_s_node
	from link_tbl
	where link_id = nlinkid;
	return array[n_s_node, e_s_node];
END;
$$;

-------------------------------------------------------------------------------------------------------------
-- link lid ==> node lid
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_linklid_2_nodelid(
    inlinkid bigint,
    outlinkid bigint,
    passlid character varying)
  RETURNS bigint[]
  LANGUAGE plpgsql
AS $$
DECLARE
        passlid_array   bigint[];
        node_lid        bigint[];
        link_cnt        int;
        prev_nodes      bigint[];
        next_nodes      bigint[];
BEGIN
	link_cnt = 1;
	node_lid = NULL::bigint[];
	passlid_array = regexp_split_to_array(passlid, E'\\|+')::bigint[];
	passlid_array = passlid_array || array[outlinkid];
	prev_nodes = mid_get_s_e_node(inlinkid);
	while link_cnt <= array_upper(passlid_array, 1) loop
		--raise info 'link_id=%', passlid_array[link_cnt];
		next_nodes = mid_get_s_e_node(passlid_array[link_cnt]);
		if prev_nodes[1] = any(next_nodes) then
			if link_cnt = 1 then
				node_lid = array[prev_nodes[2]];
			end if;
			node_lid = array_append(node_lid, prev_nodes[1]);
		elseif prev_nodes[2] = any(next_nodes) then
			if link_cnt = 1 then
				node_lid = array[prev_nodes[1]];
			end if;
			node_lid = array_append(node_lid, prev_nodes[2]);
		else
			raise EXCEPTION 'No Inter Node (%, %), inlink=%,outlink=% ', 
			      prev_nodes, next_nodes, inlinkid, outlinkid;
			return NULL;
		end if;
		prev_nodes := next_nodes;
		link_cnt := link_cnt + 1;
	end loop;

	if next_nodes[1] <> node_lid[array_upper(node_lid, 1)] then
		node_lid = array_append(node_lid, next_nodes[1]);
	else
		node_lid = array_append(node_lid, next_nodes[2]);
	end if;
	return node_lid;
END;
$$;

CREATE OR REPLACE FUNCTION mid_find_sapa_links( link bigint, step integer)
	RETURNS varchar[]
        LANGUAGE plpgsql
	AS $$
	DECLARE        
		links varchar[];
		i integer;
		snode  bigint;
		enode  bigint;
		slink bigint;
		link_type1 integer;
		road_type1 integer; 
		temp_link bigint;
		temp_node bigint;
		direction integer;          
		link_searched bigint[];
		rec record;
		steps integer;
		pass_links varchar[];
		step_array integer[];
		link_array varchar[];
		
BEGIN
    links := array_append(links,link::varchar);
    i := 1;
    steps = 0;
    step_array := array_append(step_array,steps);
    while i <= array_length(links,1) 

    
    loop
        link_array = string_to_array(links[i],'|');
        slink := (link_array[array_upper(link_array,1)])::bigint;
        steps :=  array_upper(link_array,1);
        
        --raise info 'steps, steps = %', steps;
        --raise info 'links, passlink = %', links[i];
        
        if steps > step then
            exit;
        end if ;
	      --raise info 'slink = %', slink;
	 
        select link_type, road_type, link_id, s_node, e_node, one_way_code
        from link_tbl as a
        where a.link_id = slink 
        into  link_type1, road_type1, temp_link, snode, enode, direction;

        if direction = 4 then
           exit;
        end if;
        if direction = 3 then
          temp_node = snode;
          snode = enode;
          enode = temp_node;
        end if;

        if link_type1 = 7 and i <> 1 then
            pass_links := array_append(pass_links,links[i]);
            insert into temp_find_sapa_passlinks(passlink)
            values(links[i]);
        else 
            for rec in 
                select  link_id
                from link_tbl as a
                where (	  ( (a.s_node in (snode,enode) and a.one_way_code in(1,2))
                            or
                               (a.e_node in (snode,enode) and a.one_way_code in(1,3))
                             )
                            and 
                            direction = 1

                        or
                            (
                              (a.s_node = enode  and a.one_way_code in(1,2))
                             or 
                              (a.e_node = enode  and a.one_way_code in (1,3))
                            )
                            and
                            direction <> 1 
                      )
                      and not (a.road_type in (0,1) and a.link_type in (1,2))
                    
            loop
                if rec.link_id is not null then
                    if rec.link_id = any(link_searched) then
                      continue;
                    else
                      link_searched := array_append(link_searched,rec.link_id);
                      links := array_append(links,(links[i] || '|' || (rec.link_id::varchar))::varchar);
                    end if;
                else
                --change
                       raise info 'change links = %',links[i];
                       insert into temp_find_sapa_passlinks(passlink)
                          values(links[i]);
                end if;

           end loop;

      end if;
	 
	 i := i + 1;

   end loop;

return links ;
END;
$$;


CREATE OR REPLACE FUNCTION mid_delete_not_sapa_link()
	RETURNS integer
        LANGUAGE plpgsql
	AS $$
	DECLARE        
	
		rec record;
		i integer;
		num integer;
			
BEGIN
	i := 0;
	while True loop
      raise info 'loop, loop = %', i;
      for rec in 
          select  distinct a.link_id
          from temp_find_sapa_link_suspect as a
          join link_tbl as b 
          --on ST_Intersects(a.the_geom, b.the_geom) and a.link_id <> b.link_id
          on a.link_id <> b.link_id and ( a.s_node in (b.s_node, b.e_node) or a.e_node in (b.s_node, b.e_node) )
          left join temp_find_sapa_link_suspect as c
          on b.link_id =  c.link_id
          where b.link_type <> 7 and c.link_id is null
      loop
          if rec.link_id is not null then
              delete from temp_find_sapa_link_suspect as a
              where a.link_id  = rec.link_id;
              raise info 'delete link, link = %', rec.link_id;
          else
              return 0;
          end if;

      end loop;

      i := i + 1;
      select count(*)
      from 
      (
          select  distinct a.link_id
          from temp_find_sapa_link_suspect as a
          join link_tbl as b 
          --on ST_Intersects(a.the_geom, b.the_geom) and a.link_id <> b.link_id
          on a.link_id <> b.link_id and ( a.s_node in (b.s_node, b.e_node) or a.e_node in (b.s_node, b.e_node) )
          left join temp_find_sapa_link_suspect as c
          on b.link_id =  c.link_id
          where b.link_type <> 7 and c.link_id is null
      ) as k  into num;
      
      if num = 0 then
         exit;
         return 0;
      end if;
      
  end loop;
  
 return 0;
END;
$$;

CREATE OR REPLACE FUNCTION mid_jct_linktype_modify_bak(node bigint,path bigint[],node_s bigint, node_e bigint,step integer)
	RETURNS integer
	LANGUAGE plpgsql volatile
AS $$
DECLARE
	rec 		    record;
	slink 		  bigint;
	link_array 	bigint[];
	i 		      integer;
	temp_node	  bigint;
	snode		    bigint;
	enode		    bigint;
	direction	  integer;
BEGIN
	select link_id 
	from link_tbl as a
	where node in (a.s_node,a.e_node) and a.link_type in (1,2) and a.road_type in (0,1) limit 1
	into slink;

	link_array := array_append(link_array,slink);
	i := 1;
	while i < step loop
		slink := link_array[i];
		raise info 'link = %',slink;
		select  s_node, e_node, one_way_code
		from link_tbl as a
		where a.link_id = slink  into   snode, enode, direction;

		if direction = 4 then
			exit;
		end if;
		
		if direction = 3 then
			temp_node = snode;
			snode = enode;
			enode = temp_node;
		end if;
		
		if node_s in (snode,enode) or node_e in (snode,enode) then
			insert into temp_jct_change_to_ic(links)
				values(path);
		        return 0;
		        --exit;
		else
			for rec in 
          select  link_id
          from link_tbl as a
          where (	  
                    ( a.s_node = enode and a.one_way_code = 2 )
                      or
                    ( a.e_node = enode and a.one_way_code = 3 )
                 )
                  
                and a.road_type in (0,1) and a.link_type in (1,2)
		  loop
          if rec.link_id is not null then
            if rec.link_id = any(link_array) then
              continue;
            else
              link_array := array_append(link_array,rec.link_id);
            end if;
          end if;
          
     end loop;
		        	
		end if;

	i := i + 1;
		
	end loop;
	return 1;

END;
$$;

CREATE OR REPLACE FUNCTION mid_jct_linktype_modify(node bigint,path bigint[],node_s bigint, node_e bigint)
	RETURNS integer
	LANGUAGE plpgsql volatile
AS $$
DECLARE
	flag1 integer;
	flag2 integer;
	flag3 integer;
BEGIN

  --raise info 'calc the jct angle';
  select mid_filter_the_large_angle_jct(path,120) into flag3;


	if flag3 = 1 then
      raise info 'the jct angle have problem';
      insert into temp_jct_change_to_ic(links,link_type)
          values(path,5);
  else
       raise info 'the jct angle is ok';
      select mid_highway_connect(node, path, node_s, node_e,1) into flag1;
      select mid_highway_connect(node, path, node_s, node_e,2) into flag2;
      if flag1 = 1 or flag2 = 1 then
        insert into temp_jct_change_to_ic(links,link_type)
          values(path,5);
      else
        insert into temp_jct_change_to_ic(links,link_type)
          values(path,3);
      end if;
   end if;
	return 0;

END;
$$;

CREATE OR REPLACE FUNCTION mid_jct_linktype_modify_bak(node bigint,path bigint[],node_s bigint, node_e bigint)
	RETURNS integer
	LANGUAGE plpgsql volatile
AS $$
DECLARE
	rec 		    record;
	slink 		  bigint;
	link_array 	bigint[];
	i 		      integer;
	temp_node	  bigint;
	snode		    bigint;
	enode		    bigint;
	direction	  integer;
	path_length  bigint;
	highway_length bigint;
	length        bigint;
	geom          geometry;
BEGIN
  path_length := 0;
  highway_length := 0;
  for rec in 
      select distinct the_geom from link_tbl as a
      where a.link_id = any(path)
  loop
      if rec.the_geom is not null then
          select ST_Length_Spheroid(rec.the_geom,'SPHEROID("WGS_84", 6378137, 298.257223563)')::bigint  into length;
          path_length := path_length + length;
      end if;
  end loop;
  
  
	select link_id 
	from link_tbl as a
	where node in (a.s_node,a.e_node) and a.link_type in (1,2) and a.road_type in (0,1) limit 1
	into slink;

	link_array := array_append(link_array,slink);
	i := 1;
	while highway_length < path_length loop
		slink := link_array[i];
		raise info 'link = %',slink;
		select  s_node, e_node, one_way_code,the_geom
		from link_tbl as a
		where a.link_id = slink  into   snode, enode, direction, geom;
    select ST_Length_Spheroid(geom,'SPHEROID("WGS_84", 6378137, 298.257223563)')::bigint  into length;
		if direction = 4 then
			exit;
		end if;
		
		if direction = 3 then
			temp_node = snode;
			snode = enode;
			enode = temp_node;
		end if;
		
		if node_s in (snode,enode) or node_e in (snode,enode) then
			insert into temp_jct_change_to_ic(links,link_type)
				values(path,5);
		        return 0;
		        --exit;
		else
			for rec in 
          select  link_id
          from link_tbl as a
          where (	  
                    ( a.s_node = enode and a.one_way_code = 2 )
                      or
                    ( a.e_node = enode and a.one_way_code = 3 )
                 )
                  
                and a.road_type in (0,1) and a.link_type in (1,2)
		  loop
          if rec.link_id is not null then
            if rec.link_id = any(link_array) then
              continue;
            else
              link_array := array_append(link_array,rec.link_id);
            end if;
          end if;
          
     end loop;
		        	
		end if;
		
  highway_length := highway_length + length;
	i := i + 1;
		
	end loop;
	
	
  insert into temp_jct_change_to_ic(links,link_type)
    values(path,3);
	
	return 1;

END;
$$;



CREATE OR REPLACE FUNCTION mid_find_sapa_links_rev2( link bigint, max_num integer,dir integer)
	RETURNS integer
        LANGUAGE plpgsql
	AS $$
	DECLARE        	
		slink 		bigint;
		temp_link 	bigint;
		temp_node	bigint;
		snode 	 	bigint;
		enode  		bigint;
		direction 	integer; 
		rec 		record;
		link_arrayA	bigint[];
		link_arrayB	bigint[];
		link_arrayC	bigint[];
		flag		integer;
		link_num	integer;
		i		integer;
		leng		integer;
		link_searched	bigint[];	
BEGIN
	--flag := 1;
	link_num := 0;
	link_arrayB := array_append(link_arrayB,link);
	while link_num < max_num loop
		leng := array_length(link_arrayB,1);
		if leng is null then
			leng := 0;
		end if;
		raise info 'link_arrayA = %',link_arrayA;
		raise info 'link_arrayB = %',link_arrayB;
		for i in 1..leng loop
			
			slink := link_arrayB[i];
			if not (slink = any(link_searched)) or array_length(link_searched,1) is null then
				link_searched := array_append(link_searched,slink);
			else
				continue;
			end if;
			if not (slink = any(link_arrayA))  or array_length(link_arrayA,1) is null then
				link_arrayA := array_append(link_arrayA,slink);
			end if;
			raise info 'slink = %',slink;
			select link_id, s_node, e_node, one_way_code
			from link_tbl as a
			where a.link_id = slink
			into  temp_link, snode, enode, direction;
			if slink = link then
				direction = dir;
			--end if;
			if direction = 4 then
				exit;
			end if;
			if direction = 3 then
				temp_node = snode;
				snode = enode;
				enode = temp_node;
			end if;
			--if slink = link then
			--	direction = dir;
			end if; 
			for rec in 
				select link_id,link_type
				from link_tbl as a
				where 	( ( a.s_node = enode or a.e_node = enode ) and slink = link
                  or
                  ( a.s_node in (snode, enode) or a.e_node in (snode,enode ) ) and slink <> link
                )
				
              and (
                not (a.road_type in (0,1) and a.link_type in (1,2))
                  )
					    
              and a.link_id <> temp_link
					
			loop
				if rec.link_id is not null then
					--raise info 'rec is not null';
					--raise info 'rec = %',rec;
					raise info 'link_searched = %',link_searched;
					if not (rec.link_id = any(link_searched)) and rec.link_type <> 7 then
						link_arrayC := array_append(link_arrayC, rec.link_id);
						--raise info 'rec.link_id = %',rec.link_id;
	
					end if;
				end if;
			end loop;
			
			
			
		end loop;
		flag := 1;
		raise info 'link_arrayC = %',link_arrayC;
		leng := array_length(link_arrayC,1);
		if leng is null then
			flag := 1;
		else
			for rec in 
				select distinct link_type from link_tbl as a 
				where a.link_id = any(link_arrayC)
			loop
				raise info 'rec_C = %', rec.link_type;
				if rec.link_type <> 7 then
					flag := 0;
				end if;
			end loop;
		end if;

		raise info 'flag = %', flag;
		if flag = 1 then
			insert into temp_sapa_link(link_array)
				values(link_arrayA);
			return 0;
			
		else
			link_arrayB = link_arrayC;
			link_arrayC = array[]::bigint[];
		end if;


		link_num := array_length(link_arrayA,1);
		
	end loop;

	return 1;

   
END;
$$;

CREATE OR REPLACE FUNCTION mid_highway_connect_bak(node bigint,path bigint[],node_s bigint, node_e bigint,dir integer)
	RETURNS integer
	LANGUAGE plpgsql volatile
AS $$
DECLARE
	rec 		    	record;
	slink 		   	bigint;
	link_array 		bigint[];
	i 		      	integer;
	temp_node	  	bigint;
	snode		    	bigint;
	enode		    	bigint;
	direction	 	  integer;
	path_length  		bigint;
	highway_length 		bigint;
	length        		bigint;
	geom          		geometry;
	path_count       integer;
BEGIN
  path_length := 0;
  highway_length := 0;
  for rec in 
      select distinct the_geom from link_tbl_bak_for_linktype_test_bak as a
      where a.link_id = any(path)
  loop
      if rec.the_geom is not null then
          select ST_Length_Spheroid(rec.the_geom,'SPHEROID("WGS_84", 6378137, 298.257223563)')::bigint  into length;
          path_length := path_length + length;
      end if;
  end loop;
  
  path_count := array_length(path,1);
  if path_count > 1 and (node = node_s or node = node_e) then
      return 1;
  end if;
	select link_id 
	from link_tbl_bak_for_linktype_test_bak as a
	where node in (a.s_node,a.e_node) and a.link_type in (1,2) and a.road_type in (0,1) limit 1
	into slink;

	link_array := array_append(link_array,slink);
	i := 1;
	while highway_length < path_length loop
		slink := link_array[i];
		raise info 'link = %',slink;
		select  s_node, e_node, one_way_code,the_geom
		from link_tbl_bak_for_linktype_test_bak as a
		where a.link_id = slink  into   snode, enode, direction, geom;
		select ST_Length_Spheroid(geom,'SPHEROID("WGS_84", 6378137, 298.257223563)')::bigint  into length;
		if direction = 4 then
			exit;
		end if;
		
		if direction = 3 then
			temp_node = snode;
			snode = enode;
			enode = temp_node;
		end if;
		
		if (node_s in (snode,enode) and node <> node_s )  or  (node_e in (snode,enode) and node <> node_e ) then   
			return 1;       
		else
			for rec in 
				  select  link_id
				  from link_tbl_bak_for_linktype_test_bak as a
				  where (	  
					   (
					    ( a.s_node = enode and a.one_way_code in (1,2) )
					      or
					    ( a.e_node = enode and a.one_way_code in (1,3) )
					   ) 
					   and dir = 1
					   
					   or 
					   
					   (
					    ( a.e_node = snode and a.one_way_code in (1,2) )
					      or
					    ( a.s_node = snode and a.one_way_code in (1,3) )
					   ) 
					   and dir = 2
					    
					    
					)
					  
					and a.road_type in (0,1) and a.link_type in (1,2)
			loop
				  if rec.link_id is not null then
              raise info 'rec.link_id = %',rec.link_id;
              if rec.link_id = any(link_array) then
                continue;
              else
                link_array := array_append(link_array,rec.link_id);
              end if;
				  else
              return 0;
				  end if;
          
			end loop;
		        	
		end if;
		if i <> 1 then
        highway_length := highway_length + length;
    end if;

	i := i + 1;
		
	end loop;
	
	return 0;
END;
$$;
				

CREATE OR REPLACE FUNCTION mid_change_sapa(max_num integer)
	RETURNS smallint
	LANGUAGE plpgsql volatile
AS $$
DECLARE
	rec record;
BEGIN
	for rec in
		select link_id, one_way_code
		from temp_highway_sapa_link	
	loop
		if rec.one_way_code = 1 then
			perform mid_find_sapa_links_rev2(rec.link_id::bigint, max_num, 2);
			perform mid_find_sapa_links_rev2(rec.link_id::bigint, max_num, 3);
			
		end if;

		if rec.one_way_code in (2,3) then
			perform mid_find_sapa_links_rev2(rec.link_id::bigint, max_num, rec.one_way_code);
			
		end if;
	end loop;
	
	
	RETURN 1;
END;
$$;

CREATE OR REPLACE FUNCTION mid_convert_regulation_from_patch()
    RETURNS smallint
    LANGUAGE plpgsql
AS $$
DECLARE
	rec record;
	cur_regulation_id integer;
	link_index integer;
	link_len integer;
BEGIN
	-- regulation_id
	select (case when max(regulation_id) is null then 0 else max(regulation_id) end)
	from regulation_relation_tbl
	into cur_regulation_id;
	

    FOR rec IN
		SELECT a.link_id_list as link_list, (case when a.first_node_id = d.s_node then d.e_node else d.s_node end) as node_id, b.regulation_type, c.cond_id, e.single_link_flag
		FROM mid_temp_patch_link_tbl a
		LEFT JOIN temp_regulation_patch_tbl b
			ON a.gid = b.gid
		LEFT JOIN condition_regulation_tbl c
			ON b.car_type = c.car_type and 
				b.start_year = c.start_year and
				b.start_month = c.start_month and
				b.start_day = c.start_day and
				b.start_hour = c.start_hour and
				b.start_minute = c.start_minute and
				b.end_year = c.end_year and
				b.end_month = c.end_month and
				b.end_day = c.end_day and
				b.end_hour = c.end_hour and
				b.end_minute = c.end_minute and 
				b.day_of_week = c.day_of_week and 
				b.exclude_date = c.exclude_date
		LEFT JOIN link_tbl d
			ON a.link_id_list[1] = d.link_id
		LEFT JOIN mid_temp_patch_node_tbl e
			ON a.gid = e.gid
    LOOP
		cur_regulation_id := cur_regulation_id + 1;
		
		link_len := array_upper(rec.link_list, 1);
		IF rec.single_link_flag = true THEN
			FOR link_index IN 1..link_len LOOP
				INSERT INTO regulation_item_tbl(regulation_id, linkid, nodeid, seq_num) 
					VALUES(cur_regulation_id, rec.link_list[link_index], null::bigint, 1::smallint);
				INSERT INTO regulation_relation_tbl(regulation_id, nodeid, inlinkid, outlinkid, condtype, cond_id)
					VALUES(cur_regulation_id, NULL::bigint, rec.link_list[link_index], NULL::bigint, rec.regulation_type, rec.cond_id);
			END LOOP;
		ELSE
			FOR link_index IN 1..link_len LOOP
				IF link_index = 1 THEN
					INSERT INTO regulation_item_tbl(regulation_id, linkid, nodeid, seq_num)
						VALUES(cur_regulation_id, rec.link_list[link_index], null::bigint, link_index::smallint);
					INSERT INTO regulation_item_tbl(regulation_id, linkid, nodeid, seq_num)
						VALUES(cur_regulation_id, null::bigint, rec.node_id, (link_index+1)::smallint);
				ELSE
					INSERT INTO regulation_item_tbl(regulation_id, linkid, nodeid, seq_num)
						VALUES(cur_regulation_id, rec.link_list[link_index], null::bigint, (link_index+1)::smallint);
				END IF;
				
				link_index := link_index + 1;
			END LOOP;
			
			INSERT INTO regulation_relation_tbl(regulation_id, nodeid, inlinkid, outlinkid, condtype, cond_id)
				VALUES(cur_regulation_id, rec.node_id, rec.link_list[1], rec.link_list[link_len], rec.regulation_type, rec.cond_id);
		END IF;
    END LOOP;
    return 1;
END;
$$;

CREATE OR REPLACE FUNCTION mid_update_temp_patch_node_tbl( )
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
	single_link boolean;
BEGIN
	FOR rec IN 
		SELECT gid, the_geom_list, z_level_list
		FROM temp_patch_node_tbl
	LOOP
		geom_list_len := array_upper(rec.the_geom_list, 1);
		z_level_list_len := array_upper(rec.z_level_list, 1);
		idx := 1;
		deata := 0.03;

		IF geom_list_len != z_level_list_len THEN
			raise EXCEPTION '% the_geom list length error: not equal to % z_level list length', rec.the_geom_list, rec.z_level_list;
		END IF;
		
		WHILE idx <= geom_list_len LOOP
			SELECT node_id, the_geom, ST_Distance_Sphere(the_geom, (rec.the_geom_list)[idx]) as node_distance INTO temp_node, temp_geom, distance
			FROM node_tbl
			WHERE z_level = (rec.z_level_list)[idx] and ST_DWithin(the_geom, (rec.the_geom_list)[idx], deata, True)
			ORDER BY node_distance
			LIMIT 1;

			IF temp_node is null THEN
				deata := deata + 0.2;
				
				IF deata >= 10.0 THEN
					raise EXCEPTION 'the_geom: %s z_level: %s can not bound to node', (rec.the_geom_list)[idx], (rec.z_level_list)[idx];
				ELSE
					continue;
				END IF;
			ELSE
				IF idx = 1 THEN
					res_node_list := Array[temp_node];
				ELSE
					res_node_list := array_append(res_node_list, temp_node);
				END IF;

				idx := idx + 1;
				deata := 0.03;
			END IF;
		END LOOP;
		
		IF geom_list_len = 2 THEN
			single_link := true;
		ELSE
			single_link := false;
		END IF;
		
		INSERT INTO mid_temp_patch_node_tbl(gid, node_id_list, single_link_flag)
		VALUES(rec.gid, res_node_list, single_link);
	END LOOP;

	return true;
END;
$$;

CREATE OR REPLACE FUNCTION mid_update_temp_patch_link_tbl( )
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
		from mid_temp_patch_node_tbl
	loop
		node_idx := 1;
		node_list_len := array_upper(rec.node_id_list, 1);
		if node_list_len < 2 then
			raise EXCEPTION 'mid_temp_patch_node_tbl node_id_list % length error', rec.node_id_list;
		end if;

		while node_idx <= (node_list_len - 1) loop
			in_node := (rec.node_id_list)[node_idx];
			out_node := (rec.node_id_list)[node_idx + 1];
			res_string := mid_findpasslinkbybothnodes(in_node, out_node);
			if res_string is null then
				raise EXCEPTION 'in_node % out_node % can not calc path', in_node, out_node;
			end if;
			
			if array_upper(string_to_array(res_string, ','), 1) > 3 then
				raise EXCEPTION 'The path from in_node % to out_node % maybe error!!!', in_node, out_node;
			end if;
			
			temp_link_array := string_to_array(res_string, ',')::bigint[];
			if node_idx = 1 then
				res_link_array := temp_link_array;--Array[temp_link_array[array_upper(temp_link_array, 1)]];
			else
				res_link_array := array_cat(res_link_array, temp_link_array);
			end if;
			--raise info '%', temp_link_array;
			--raise info '%', res_link_array;
			node_idx := node_idx + 1;
		end loop;
		
		res_link_array_len := array_upper(res_link_array, 1);
		if res_link_array_len >= 1 then
			insert into mid_temp_patch_link_tbl(gid, first_node_id, link_id_list)
			values(rec.gid, rec.node_id_list[1], res_link_array);
		else
			raise EXCEPTION 'node list % convert to link list % error', rec.node_id_list, res_link_array;
		end if;
	end loop;

	return true;
END;
$$;

CREATE OR REPLACE FUNCTION mid_convert_regulation_from_patch()
    RETURNS smallint
    LANGUAGE plpgsql
AS $$
DECLARE
	rec record;
	cur_regulation_id integer;
	link_index integer;
	link_len integer;
BEGIN
	-- regulation_id
	select (case when max(regulation_id) is null then 0 else max(regulation_id) end)
	from regulation_relation_tbl
	into cur_regulation_id;
	

    FOR rec IN
		SELECT a.link_id_list as link_list, (case when a.first_node_id = d.s_node then d.e_node else d.s_node end) as node_id, b.regulation_type, c.cond_id, e.single_link_flag
		FROM mid_temp_patch_link_tbl a
		LEFT JOIN temp_regulation_patch_tbl b
			ON a.gid = b.gid
		LEFT JOIN condition_regulation_tbl c
			ON b.car_type = c.car_type and 
				b.start_year = c.start_year and
				b.start_month = c.start_month and
				b.start_day = c.start_day and
				b.start_hour = c.start_hour and
				b.start_minute = c.start_minute and
				b.end_year = c.end_year and
				b.end_month = c.end_month and
				b.end_day = c.end_day and
				b.end_hour = c.end_hour and
				b.end_minute = c.end_minute and 
				b.day_of_week = c.day_of_week and 
				b.exclude_date = c.exclude_date
		LEFT JOIN link_tbl d
			ON a.link_id_list[1] = d.link_id
		LEFT JOIN mid_temp_patch_node_tbl e
			ON a.gid = e.gid
    LOOP
		cur_regulation_id := cur_regulation_id + 1;
		
		link_len := array_upper(rec.link_list, 1);
		IF rec.single_link_flag = true THEN
			FOR link_index IN 1..link_len LOOP
				INSERT INTO regulation_item_tbl(regulation_id, linkid, nodeid, seq_num) 
					VALUES(cur_regulation_id, rec.link_list[link_index], null::bigint, 1::smallint);
				INSERT INTO regulation_relation_tbl(regulation_id, nodeid, inlinkid, outlinkid, condtype, cond_id)
					VALUES(cur_regulation_id, NULL::bigint, rec.link_list[link_index], NULL::bigint, rec.regulation_type, rec.cond_id);
			END LOOP;
		ELSE
			FOR link_index IN 1..link_len LOOP
				IF link_index = 1 THEN
					INSERT INTO regulation_item_tbl(regulation_id, linkid, nodeid, seq_num)
						VALUES(cur_regulation_id, rec.link_list[link_index], null::bigint, link_index::smallint);
					INSERT INTO regulation_item_tbl(regulation_id, linkid, nodeid, seq_num)
						VALUES(cur_regulation_id, null::bigint, rec.node_id, (link_index+1)::smallint);
				ELSE
					INSERT INTO regulation_item_tbl(regulation_id, linkid, nodeid, seq_num)
						VALUES(cur_regulation_id, rec.link_list[link_index], null::bigint, (link_index+1)::smallint);
				END IF;
				
				link_index := link_index + 1;
			END LOOP;
			
			INSERT INTO regulation_relation_tbl(regulation_id, nodeid, inlinkid, outlinkid, condtype, cond_id)
				VALUES(cur_regulation_id, rec.node_id, rec.link_list[1], rec.link_list[link_len], rec.regulation_type, rec.cond_id);
		END IF;
    END LOOP;
    return 1;
END;
$$;

CREATE OR REPLACE FUNCTION mid_update_force_guide_temp_patch_link_tbl( )
	RETURNS boolean
	LANGUAGE plpgsql volatile
AS $$
DECLARE
	rec record;
	link_list_len integer;
	link_index integer;
	link_s_node bigint;
	link_e_node bigint;
	find_link boolean;
	first_node_id bigint;
	res_link_array bigint[];
BEGIN	
	FOR rec IN 
		SELECT a.gid, a.link_id_list, b.node_id_list[2] as node_id
		FROM mid_temp_patch_link_tbl_bak a
		LEFT JOIN mid_temp_patch_node_tbl b
			ON a.gid = b.gid
	LOOP
		find_link := false;
		link_list_len := array_upper(rec.link_id_list, 1);
		
		FOR link_index IN 1..link_list_len LOOP
			SELECT s_node, e_node INTO link_s_node, link_e_node
			FROM link_tbl
			WHERE link_id = rec.link_id_list[link_index];
			
			IF find_link = false THEN
				IF link_s_node = rec.node_id or link_e_node = rec.node_id THEN
					find_link := true;
					IF link_s_node = rec.node_id THEN 
						first_node_id := link_e_node;
					ELSE
						first_node_id := link_s_node;
					END IF;
					res_link_array := ARRAY[rec.link_id_list[link_index]];
				END IF;
			ELSE
				res_link_array := array_append(res_link_array, rec.link_id_list[link_index]);
			END IF;
		END LOOP;
		
		INSERT INTO mid_temp_patch_link_tbl(gid, first_node_id, link_id_list)
			VALUES(rec.gid, first_node_id, res_link_array);
	END LOOP;

	return true;
END;
$$;

CREATE OR REPLACE FUNCTION  mid_find_jct_link_paths_bak(link bigint, dir integer,len integer)
	RETURNS smallint
        LANGUAGE plpgsql
	AS $$
	DECLARE 
		rec record;
		link_array bigint[];
		temp_link bigint;
		snode  bigint;
		enode  bigint;
		temp_node bigint;
		direction integer;
		link_type1 integer;
		road_type1 integer;
		flag  smallint;
		link_path_array varchar[];
		i integer;
		slink bigint;
		length integer;	
		sapa_count integer;	
               		
BEGIN
    link_path_array := array_append(link_path_array,link::varchar);
    i := 1;
    flag := 0;
    while i <= array_length(link_path_array,1) loop
        link_array := string_to_array(link_path_array[i],'|');
        sapa_count := 0;
        for rec in 
            select distinct link_id,link_type from link_tbl as a
            where a.link_id = any(link_array)
        loop
            if rec.link_type = 7 then
                sapa_count := sapa_count + 1;
            end if;
        end loop;
        raise info 'sapa count = %',sapa_count;
        if sapa_count > len then
            i := i + 1;
            continue;
        end if;
        raise info 'link_path_array =  %', link_path_array[i];
        slink := (link_array[array_upper(link_array,1)])::bigint;	
        select link_type,road_type,link_id, s_node, e_node, one_way_code
        from link_tbl as a
        where a.link_id = slink  into  link_type1, road_type1, temp_link, snode, enode, direction;
        if direction = 4 then
           exit;
        end if;
        if direction = 3 then
          temp_node = snode;
          snode = enode;
          enode = temp_node;
        end if;

        if link_type1 in (1,2) and road_type1 in (0,1) or link_type1 = 3  then
              for rec in 
                  select distinct link_type from link_tbl as a
                  where a.link_id = any(link_array)
              loop
                  if rec.link_type = 7 then
                      flag = 1;
                  end if;	
              end loop;
              if flag = 1 and link_type1 in (1,2) then
                  insert into temp_suspect_jct_paths_link(link_path) 
                        --values(link_path_array[i]);
                        values((string_to_array(link_path_array[i],'|'))::bigint[] );	
              end if;
          
              if flag = 1 and link_type1 = 3 then
                    insert into temp_jct_paths_link(link_path) 
                          --values(link_path_array[i]);
                          values((string_to_array(link_path_array[i],'|'))::bigint[] );
              end if;
		
        else	 
              for rec in 
                  select  link_id
                  from link_tbl as a
                  where (	  ( (a.s_node in (snode,enode) and a.one_way_code in(1,2))
                              or
                                 (a.e_node in (snode,enode) and a.one_way_code in(1,3))
                               )
                              and 
                              direction = 1 and dir = 1
                              
                          or
                            ( (a.e_node in (snode,enode) and a.one_way_code in(1,2))
                              or
                                 (a.s_node in (snode,enode) and a.one_way_code in(1,3))
                               )
                              and 
                              direction = 1 and dir = 2

                          or
                              (
                                (a.s_node = enode  and a.one_way_code in(1,2))
                               or 
                                (a.e_node = enode  and a.one_way_code in (1,3))
                              )
                              and
                              direction <> 1 and dir = 1
                              
                          or
                              (
                                (a.e_node = snode  and a.one_way_code in(1,2))
                               or 
                                (a.s_node = snode  and a.one_way_code in (1,3))
                              )
                              and
                              direction <> 1 and dir = 2
                        )
                        and 
                       ( a.link_type in (3,5,7) or (a.link_type in (1,2) and a.road_type in (0,1)))
	
              loop
                  if rec.link_id is not null then
                      if rec.link_id = any(link_array) then
                            continue;
                      else
                            link_path_array := array_append(link_path_array,(link_path_array[i] || '|' || (rec.link_id::varchar))::varchar);
                      end if;
                  end if;
              end loop;
	
        end if;
				
    i := i + 1;
				
    end loop;
  return 0;
END;
$$;

CREATE OR REPLACE FUNCTION  mid_find_suspect_jct_link(len integer)
	RETURNS smallint
        LANGUAGE plpgsql
	AS $$
	DECLARE 
		rec record;	
               		
BEGIN
    for rec in 
        select distinct a.link_id
        from
        (
          select * from link_tbl_bak_for_linktype_test_bak
          where link_type = 5
        ) as a
        join
        ( 
          select * from link_tbl_bak_for_linktype_test_bak 
          where link_type in (1,2) and road_type in (0,1)
        ) as b
        on a.s_node in (b.s_node,b.e_node) or a.e_node in (b.s_node,b.e_node)
    loop
        perform mid_find_jct_link_paths(rec.link_id,1,len);
        perform mid_find_jct_link_paths(rec.link_id,2,len);
    end loop;

return 0;
END;
$$;

CREATE OR REPLACE FUNCTION mid_jct_link_paths_filter(node bigint,path bigint[],node_s bigint, node_e bigint)
	RETURNS integer
	LANGUAGE plpgsql volatile
AS $$
DECLARE
	flag1 integer;
	flag2 integer;
	flag3 integer;
BEGIN
    select mid_filter_the_large_angle_jct(path,120) into flag3;
    if flag3 = 0 then
      raise info 'the jct path angle is ok';
      select mid_highway_connect(node, path, node_s, node_e,1) into flag1;
      select mid_highway_connect(node, path, node_s, node_e,2) into flag2;
      if flag1 <> 1 and flag2 <> 1 then
        raise info 'the jct path is not connect to the same highway';
        insert into temp_jct_paths_link(link_path)
          values(path);
       end if;
    end if;
	return 0;

END;
$$;


CREATE OR REPLACE FUNCTION  mid_find_jct_link_paths(link bigint, dir integer,len integer)
	RETURNS smallint
        LANGUAGE plpgsql
	AS $$
	DECLARE 
		rec record;
		link_array bigint[];
		temp_link bigint;
		snode  bigint;
		enode  bigint;
		temp_node bigint;
		direction integer;
		link_type1 integer;
		road_type1 integer;
		flag  smallint;
		link_path_array varchar[];
		i integer;
		slink bigint;
		length integer;	
		sapa_count integer;
		sapa_link_array bigint[];	
               		
BEGIN
    link_path_array := array_append(link_path_array,link::varchar);
    i := 1;
    flag := 0;
    while i <= array_length(link_path_array,1) loop
        link_array := string_to_array(link_path_array[i],'|');
        sapa_count := 0;
        for rec in 
            select distinct link_id,link_type from link_tbl_bak_for_linktype_test_bak as a
            where a.link_id = any(link_array)
        loop
            if rec.link_type = 7 then
                sapa_count := sapa_count + 1;
            end if;
        end loop;
        raise info 'sapa count = %',sapa_count;
        if sapa_count > len then
            i := i + 1;
            continue;
        end if;
        raise info 'link_path_array =  %', link_path_array[i];
        slink := (link_array[array_upper(link_array,1)])::bigint;	
        select link_type,road_type,link_id, s_node, e_node, one_way_code
        from link_tbl_bak_for_linktype_test_bak as a
        where a.link_id = slink  into  link_type1, road_type1, temp_link, snode, enode, direction;
        if direction = 4 then
           exit;
        end if;
        if direction = 3 then
          temp_node = snode;
          snode = enode;
          enode = temp_node;
        end if;

        if link_type1 in (1,2) and road_type1 in (0,1) or link_type1 = 3  then
              for rec in 
                  select distinct link_type from link_tbl_bak_for_linktype_test_bak as a
                  where a.link_id = any(link_array)
              loop
                  if rec.link_type = 7 then
                      flag = 1;
                  end if;	
              end loop;
              if flag = 1 and link_type1 in (1,2) then
                  insert into temp_suspect_jct_paths_link(link_path) 
                        --values(link_path_array[i]);
                        values((string_to_array(link_path_array[i],'|'))::bigint[] );	
              end if;
          
              if flag = 1 and link_type1 = 3 then
                    insert into temp_jct_paths_link(link_path) 
                          --values(link_path_array[i]);
                          values((string_to_array(link_path_array[i],'|'))::bigint[] );
              end if;
		
        else	 
              for rec in 
                  select  link_id,link_type
                  from link_tbl_bak_for_linktype_test_bak as a
                  where (	  ( (a.s_node in (snode,enode) and a.one_way_code in(1,2))
                              or
                                 (a.e_node in (snode,enode) and a.one_way_code in(1,3))
                               )
                              and 
                              direction = 1 and dir = 1
                              
                          or
                            ( (a.e_node in (snode,enode) and a.one_way_code in(1,2))
                              or
                                 (a.s_node in (snode,enode) and a.one_way_code in(1,3))
                               )
                              and 
                              direction = 1 and dir = 2

                          or
                              (
                                (a.s_node = enode  and a.one_way_code in(1,2))
                               or 
                                (a.e_node = enode  and a.one_way_code in (1,3))
                              )
                              and
                              direction <> 1 and dir = 1
                              
                          or
                              (
                                (a.e_node = snode  and a.one_way_code in(1,2))
                               or 
                                (a.s_node = snode  and a.one_way_code in (1,3))
                              )
                              and
                              direction <> 1 and dir = 2
                        )
                        and 
                       ( a.link_type in (3,5,7) or (a.link_type in (1,2) and a.road_type in (0,1)))
	
              loop
                  if rec.link_id is not null then
                      if rec.link_type = 7 then
                        if rec.link_id = any(sapa_link_array) then
                          continue;
                        else
                          sapa_link_array := array_append(sapa_link_array,rec.link_id);	
                        end if;
                      end if;
			
                      if rec.link_id = any(link_array) then
                            continue;
                      else
                            link_path_array := array_append(link_path_array,(link_path_array[i] || '|' || (rec.link_id::varchar))::varchar);
                      end if;
                 else
                      insert into temp_suspect_jct_paths_link(link_path) 
                            values((string_to_array(link_path_array[i],'|'))::bigint[] );	
                end if;
             end loop;
	
       end if;
		
    i := i + 1;
				
    end loop;
  return 0;
END;
$$;

CREATE OR REPLACE FUNCTION mid_calc_jct_steering_angle(slink bigint, elink bigint, join_node bigint)
	RETURNS double precision
	LANGUAGE plpgsql volatile
AS $$
DECLARE
	rec record;
	angle1 double precision;
	angle2 double precision;
	result double precision;

BEGIN
	for rec in
		select ( case when a.e_node = join_node then mid_cal_zm(a.the_geom,-1)
                    when a.s_node = join_node then mid_cal_zm(a.the_geom,1) 
                    else 70000 end) as angle
		from link_tbl_bak_for_linktype_test_bak as a
		where link_id = slink
	loop
		if rec.angle is not null then
			angle1 = rec.angle/65536.0 * 360 ;
		else 
			raise info 'error happened !!!';
		end if;
	end loop;

	for rec in
		select ( case when a.e_node = join_node then mid_cal_zm(a.the_geom,-1)
                    when a.s_node = join_node then mid_cal_zm(a.the_geom,1) 
                    else 70000 end) as angle
		from link_tbl_bak_for_linktype_test_bak as a
		where link_id = elink
	loop
		if rec.angle is not null then
			angle2 = rec.angle/65536.0 * 360 ;
		else 
			raise info 'error happened !!!';
		end if;
	end loop;
	
	if angle1 > 360 or angle2 > 360 then
      return 180 ;
	else
      result := angle2 - angle1;
      
      if result > 180 then
          result := 360 - result;
      end if;
      
      if result < -180 then
          result := 360 + result;
      end if;
      
      return abs(result);
   end if;
END;
$$;

CREATE OR REPLACE FUNCTION mid_filter_the_large_angle_jct(path bigint[],angle double precision)
	RETURNS integer
	LANGUAGE plpgsql volatile
AS $$
DECLARE
	i integer;
	link bigint;
	rec record;
	steering_angle double precision;
	join_node_array bigint[];

BEGIN
	i = 1;
	while i <= array_length(path,1)
	loop
		link := path[i];
		for rec in 
			select distinct b.link_id, c.link_id as other_link,
				(case when b.s_node in (c.s_node,c.e_node) then b.s_node
				      when b.e_node in (c.s_node,c.e_node) then b.e_node end ) as join_node
			from
			(
				select * from link_tbl_bak_for_linktype_test_bak as a
				where link_id = link  and a.link_type <> 7
			) as b
			left join link_tbl_bak_for_linktype_test_bak as c
			on b.s_node in (c.s_node,c.e_node) or b.e_node in (c.s_node,c.e_node)
			where (not (c.link_id = any(path)) )
			      and c.one_way_code <> 4
			      and  not ( c.link_type in (1,2) and c.road_type in (0,1) )
		loop

			if rec.other_link is not null then
          if (not rec.join_node = any(join_node_array) ) or array_length(join_node_array,1) is null then
              join_node_array := array_append(join_node_array,rec.join_node);
            
              select mid_calc_jct_steering_angle(path[i], path[i+1], rec.join_node) into steering_angle;
              raise info 'join_node_array = %',join_node_array;
              raise info 'link1 = %',path[i];
              raise info 'link2 = %',path[i+1];
              raise info 'join_node = %',rec.join_node;
              raise info 'the angle = %', steering_angle;
              if steering_angle < angle then
                raise info 'steering angle has problem';
                return 1;
              end if;
           end if;
			end if;

		end loop;
		
	i := i + 1;

	end loop;
  raise info 'steering angle is ok';
	return 0;

END;
$$;



CREATE OR REPLACE FUNCTION mid_find_ic_surround_by_jct(link bigint, dir integer)
	RETURNS smallint
	LANGUAGE plpgsql volatile
AS $$
DECLARE
	rec 			record;
	ic_path_array 		varchar[];
	i 			integer;
	link_type1 		smallint;
	temp_link 		bigint;
	snode 			bigint;
	enode 			bigint;
	temp_node 		bigint;
	direction 		integer;
	link_array		bigint[];
	slink			bigint;
	len			integer;
	j			integer;
	temp_ic_path_array	bigint[];
	flag			integer;
	
	
BEGIN

	i := 1;
	ic_path_array := array_append(ic_path_array,link::varchar);
	flag := 0;
	while i <= array_length(ic_path_array,1) loop
		link_array := string_to_array(ic_path_array[i],'|');
		slink := (link_array[array_upper(link_array,1)])::bigint; 

		select link_type,link_id, s_node, e_node, one_way_code
		from link_tbl_bak_for_linktype_test_bak as a
		where a.link_id = slink  into  link_type1, temp_link, snode, enode, direction;
		if direction = 4 then
		   continue;
		end if;

		if direction = 1 and slink = link then
			direction = dir;
		end if;
		
		if direction = 3 then
		  temp_node = snode;
		  snode = enode;
		  enode = temp_node;
		end if;

		if link_type1  = 3  then
			len := array_length(link_array,1);
			if len > 2 then
				raise info 'insert into----';
				insert into temp_ic_surround_by_jct_link(links)
					values(link_array);
			end if;
			
			if len = 2 then
				raise info 'the path only two links';
				for rec in 
					select distinct a.link_id, b.link_id as l_link, c.link_id as r_link
					from link_tbl_bak_for_linktype_test_bak as a
					left join 
					(
						select * from link_tbl_bak_for_linktype_test_bak
						where link_type = 3
					) as b
					on a.s_node in (b.s_node, b.e_node)
					left join 
					(
						select * from link_tbl_bak_for_linktype_test_bak
						where link_type = 3
					) as c
					on a.e_node in (c.s_node, c.e_node)
					where a.link_id = link_array[1]
				loop
					if rec.l_link is not null and rec.r_link is not null then
						insert into temp_ic_surround_by_jct_link(links)
							values(link_array);
					end if;
				end loop;

			end if;		
		
		else
			temp_ic_path_array := (array[])::bigint[];
		      for rec in 
			  select  distinct link_id, link_type
			  from link_tbl_bak_for_linktype_test_bak as a
			  where (     ( (a.s_node in (snode,enode) and a.one_way_code in(1,2))
				      or
					 (a.e_node in (snode,enode) and a.one_way_code in(1,3))
				       )
				      and 
				      direction = 1 
				      
				  or
				      (
					(a.s_node = enode  and a.one_way_code in(1,2))
				       or 
					(a.e_node = enode  and a.one_way_code in (1,3))
				      )
				      and
				      direction <> 1 
				)
				--and a.link_type in (3,5) 
		      loop
			  if rec.link_id is not null then
				if rec.link_type not in (3,5) then
					raise info 'the ic path connect to general raod';
					flag := 1;
					exit;
				end if;
				
			      if rec.link_id = any(link_array) then
				    continue;
			      else
				    temp_ic_path_array := array_append(temp_ic_path_array,rec.link_id);
				    
			      end if;	
			  end if;
		     end loop;

		     len := array_length(temp_ic_path_array,1);
		     if flag = 0 and len is not null then
			for j in 1..len loop
				ic_path_array := array_append(ic_path_array,(ic_path_array[i] || '|' || ((temp_ic_path_array[j])::varchar))::varchar);	

			end loop;
		     end if;

		end if;
		
	i := i + 1;
	flag := 0;			
	end loop;
  return 0;
END;
$$;


CREATE OR REPLACE FUNCTION mid_find_ic_surround_by_jct_exec()
	RETURNS smallint
	LANGUAGE plpgsql volatile
AS $$
DECLARE
	rec 			record;	
BEGIN

	for rec in 
		select distinct b.link_id, b.one_way_code
		from
		(
			select * from link_tbl_bak_for_linktype_test_bak as a
			where a.link_type = 5
		) as b
		join 
		(
			select * from link_tbl_bak_for_linktype_test_bak as a
			where a.link_type = 3
		) as c
		on b.s_node in (c.s_node,c.e_node) and b.one_way_code in (1,2)
		or  b.e_node in (c.s_node,c.e_node) and b.one_way_code in (1,3)
	loop
		if rec is not null then
			if rec.one_way_code = 1 then
				perform mid_find_ic_surround_by_jct(rec.link_id, 2);
				perform mid_find_ic_surround_by_jct(rec.link_id, 3);
			end if;
			if rec.one_way_code in (2,3) then
				perform mid_find_ic_surround_by_jct(rec.link_id, rec.one_way_code);
			end if;

		end if;

	end loop;

	return 0;

END;
$$;


CREATE OR REPLACE FUNCTION mid_convert_mainnode_regulation_linkrow()
    RETURNS smallint
    LANGUAGE plpgsql
AS $$
DECLARE
	rec record;
	cur_regulation_id integer;
	nIndex integer;
	nCount integer;
	exist_regulation_id integer[];
	exist_regulation_id_len integer;
	exist_regulation_id_idx integer;
	exist_linkids varchar;
	exist_linkids_flag boolean;
BEGIN
	-- regulation_id
	SELECT (case when max(regulation_id) is null then 0 else max(regulation_id) end)
	FROM regulation_relation_tbl
	INTO cur_regulation_id;
	
	FOR rec IN 
		SELECT a.new_regulation_id, a.new_link_array, a.new_link_array::varchar as linkids, b.nodeid, b.inlinkid, b.outlinkid, b.condtype, b.cond_id
		FROM temp_mainnode_update_regulation a
		LEFT JOIN regulation_relation_tbl b
			ON a.old_regulation_id = b.regulation_id
		ORDER BY a.new_regulation_id
	LOOP
		-- avoid redundancy regulation
		exist_linkids_flag := false;
		SELECT array_agg(regulation_id) INTO exist_regulation_id
		FROM regulation_relation_tbl
		WHERE nodeid IS NOT DISTINCT FROM rec.nodeid and 
			inlinkid IS NOT DISTINCT FROM rec.inlinkid and
			outlinkid IS NOT DISTINCT FROM rec.outlinkid and
			condtype IS NOT DISTINCT FROM rec.condtype and 
			cond_id IS NOT DISTINCT FROM rec.cond_id;
		
		IF FOUND and (exist_regulation_id IS NOT NULL) THEN
			exist_regulation_id_len := array_upper(exist_regulation_id, 1);
			FOR exist_regulation_id_idx IN 1..exist_regulation_id_len LOOP
				SELECT array_agg(linkid)::varchar INTO exist_linkids
				FROM (
					SELECT *
					FROM regulation_item_tbl
					WHERE regulation_id = exist_regulation_id[exist_regulation_id_idx] and seq_num != 2
					ORDER BY regulation_id, seq_num
				) a
				GROUP BY regulation_id;
				
				IF exist_linkids IS NOT DISTINCT FROM rec.linkids THEN
					exist_linkids_flag := true;
					exit;
				END IF;
			END LOOP;
			
			IF exist_linkids_flag = true THEN
				continue;
			END IF;
		END IF;
		
		-- current regulation id
    	cur_regulation_id := cur_regulation_id + 1;
		
		-- insert into regulation_item_tbl
		nCount := array_upper(rec.new_link_array, 1);
		nIndex := 1;
		WHILE nIndex <= nCount LOOP
			IF nIndex = 1 THEN
	    		INSERT INTO regulation_item_tbl("regulation_id", "linkid", "nodeid", "seq_num")
	    			VALUES	(cur_regulation_id, rec.new_link_array[nIndex], null, nIndex);
	    		INSERT INTO regulation_item_tbl("regulation_id", "linkid", "nodeid", "seq_num")
	    			VALUES	(cur_regulation_id, null, rec.nodeid, nIndex+1);
	    	ELSE
	    		INSERT INTO regulation_item_tbl("regulation_id", "linkid", "nodeid", "seq_num")
	    			VALUES	(cur_regulation_id, rec.new_link_array[nIndex], null, nIndex+1);
	    	END IF;
			nIndex := nIndex + 1;
		END LOOP;
		
		-- insert into regulation_relation_tbl
    	INSERT INTO regulation_relation_tbl("regulation_id", "nodeid", "inlinkid", "outlinkid", "condtype", "cond_id")
    				VALUES	(cur_regulation_id, rec.nodeid, rec.inlinkid, rec.outlinkid, rec.condtype, rec.cond_id);
	END LOOP;
	
    return 1;
END;
$$;

CREATE OR REPLACE FUNCTION mid_highway_connect(node bigint,path bigint[],node_s bigint, node_e bigint,dir integer)
	RETURNS integer
	LANGUAGE plpgsql volatile
AS $$
DECLARE
	rec 		    	record;
	slink 		   	bigint;
	link_array 		bigint[];
	i 		      	integer;
	temp_node	  	bigint;
	snode		    	bigint;
	enode		    	bigint;
	direction	 	  integer;
	path_length  		bigint;
	highway_length 		bigint;
	length        		bigint;
	geom          		geometry;
	path_count       	integer;
	highway_path_array	varchar[];
	flag			integer;
BEGIN
	flag := 0;
  path_length := 0;
  highway_length := 0;
  for rec in 
      select distinct the_geom from link_tbl_bak_for_linktype_test_bak as a
      where a.link_id = any(path)
  loop
      if rec.the_geom is not null then
          select ST_Length_Spheroid(rec.the_geom,'SPHEROID("WGS_84", 6378137, 298.257223563)')::bigint  into length;
          path_length := path_length + length;
      end if;
  end loop;
  
  path_count := array_length(path,1);
  if path_count > 1 and (node = node_s or node = node_e) then
      return 1;
  end if;
  
	select link_id 
	from link_tbl_bak_for_linktype_test_bak as a
	where node in (a.s_node,a.e_node) and a.link_type in (1,2) and a.road_type in (0,1) limit 1
	into slink;

	highway_path_array := array_append(highway_path_array,slink::varchar);
	
	i := 1;
	while i <= array_length(highway_path_array,1) loop
		raise info 'paths = %',highway_path_array[i];
		link_array := string_to_array(highway_path_array[i],'|');

		highway_length := 0;
		for rec in 
			select distinct the_geom from link_tbl_bak_for_linktype_test_bak as a
			where a.link_id = any(link_array) and a.link_id <> (highway_path_array[1])::bigint
		loop
			if rec.the_geom is not null then
				select ST_Length_Spheroid(rec.the_geom,'SPHEROID("WGS_84", 6378137, 298.257223563)')::bigint  into length;
				highway_length := highway_length + length;
			end if;
		end loop;
		
		if highway_length > path_length then
			flag := flag + 1;
			
		end if;

		if flag > 5 then
			exit;
		end if;
		slink := (link_array[array_upper(link_array,1)])::bigint;
		--raise info 'link = %',slink;
		select  s_node, e_node, one_way_code
		from link_tbl_bak_for_linktype_test_bak as a
		where a.link_id = slink  into   snode, enode, direction;
		
		if direction = 4 then
			exit;
		end if;
		
		if direction = 3 then
			temp_node = snode;
			snode = enode;
			enode = temp_node;
		end if;
		
		if (node_s in (snode,enode) and node <> node_s )  or  (node_e in (snode,enode) and node <> node_e ) then   
			return 1;       
		else
			for rec in 
				  select  link_id
				  from link_tbl_bak_for_linktype_test_bak as a
				  where (	  
						( (a.s_node in (snode,enode) and a.one_way_code in(1,2))
						or
						 (a.e_node in (snode,enode) and a.one_way_code in(1,3))
						)
						and 
						direction = 1 and dir = 1
					      
						or
						( (a.e_node in (snode,enode) and a.one_way_code in(1,2))
						or
						 (a.s_node in (snode,enode) and a.one_way_code in(1,3))
						)
						and 
						direction = 1 and dir = 2

						or
					      (
						(a.s_node = enode  and a.one_way_code in(1,2))
					       or 
						(a.e_node = enode  and a.one_way_code in (1,3))
					      )
					      and
					      direction <> 1 and dir = 1
					      
						or
					      (
						(a.e_node = snode  and a.one_way_code in(1,2))
					       or 
						(a.s_node = snode  and a.one_way_code in (1,3))
					      )
					      and
					      direction <> 1 and dir = 2   
					    
					)
					  
					and a.road_type in (0,1) and a.link_type in (1,2)
			loop
				if rec.link_id is not null then
					raise info 'rec.link_id = %',rec.link_id;
					if rec.link_id = any(link_array) then
						continue;
					else
				highway_path_array := array_append(highway_path_array,(highway_path_array[i] || '|' || (rec.link_id::varchar))::varchar);
					end if;
				  else
					return 0;
				  end if;
          
			end loop;
		        	
		end if;
	i := i + 1;
		
	end loop;
	return 0;

END;
$$;