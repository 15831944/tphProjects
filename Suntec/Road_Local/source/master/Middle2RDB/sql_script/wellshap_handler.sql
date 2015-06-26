CREATE OR REPLACE FUNCTION get_array_elem_count(flags integer[],elem integer)
  returns integer
  language plpgsql
as $$
declare
	array_len        integer;
	i                integer;
	count_f          integer;
BEGIN	
	array_len = 0;
	i = 1;
	count_f = 0;
	array_len = array_upper(flags,1);
	while i <= array_len loop
		if flags[i] = elem then
			count_f = count_f + 1;
		end if;
		i = i + 1;
	end loop;

	return count_f;
end;
$$;

CREATE OR REPLACE FUNCTION find_innerlink_shortlink_intersectnode()
  returns integer
  language plpgsql
as $$
declare
        rec        	record;
        flags           smallint[];
BEGIN
	for rec in
	  SELECT start_node_id,end_node_id,link_id,one_way
	  FROM rdb_link
	  where link_type in (8,9)
	loop
		select array_agg(exsit_flag)
		into flags
		from(
			select exsit_flag, group_flag
			from(
				select 1 as exsit_flag, 1 as group_flag
				from rdb_link
				where link_type = 4 and (start_node_id = rec.start_node_id or end_node_id = rec.start_node_id)

				union all

				select 2 as exsit_flag, 1 as group_flag
				from rdb_link
				where link_type = 4 and (start_node_id = rec.end_node_id or end_node_id = rec.end_node_id)
			) as a
		) as b
		group by group_flag;

		if get_array_elem_count(flags,1) < 2 and get_array_elem_count(flags,2) < 2 then
			if 1 = ANY(flags) and 2 = ANY(flags) then
				if rec.one_way in (1,2) then
					insert into wellshape_start_end_node_rdb_tbl(s_node,e_node)
					values(rec.start_node_id,rec.end_node_id);
				elsif rec.one_way in (1,3) then
					insert into wellshape_start_end_node_rdb_tbl(s_node,e_node)
					values(rec.end_node_id,rec.start_node_id);
				else
					raise info 'this link can not pass,linkid: %',rec.link_id;
				end if;
			elsif 1 = ANY(flags) then
				if rec.one_way in (1,2) then
					insert into wellshape_start_end_node_rdb_tbl(s_node,e_node)
					values(rec.start_node_id,find_another_shortcut_node(rec.end_node_id,rec.link_id));
				elsif rec.one_way in (1,3) then
					insert into wellshape_start_end_node_rdb_tbl(s_node,e_node)
					values(find_another_shortcut_node(rec.end_node_id,rec.link_id),rec.start_node_id);
				else
					raise info 'this link can not pass,linkid: %',rec.link_id;
				end if;
			elsif 2 = ANY(flags) then
				if rec.one_way in (1,2) then
					insert into wellshape_start_end_node_rdb_tbl(s_node,e_node)
					values(find_another_shortcut_node(rec.start_node_id,rec.link_id),rec.end_node_id);
				elsif rec.one_way in (1,3) then
					insert into wellshape_start_end_node_rdb_tbl(s_node,e_node)
					values(rec.end_node_id,find_another_shortcut_node(rec.start_node_id,rec.link_id));
				else
					raise info 'this link can not pass,linkid: %',rec.link_id;
				end if;
			end if;
		end if;
	end loop;
	return 0;
end;
$$;

CREATE OR REPLACE FUNCTION find_another_shortcut_node(nodeid bigint,linkid bigint)
  RETURNS bigint
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
	num_flag              integer;
	end_flag              integer;
BEGIN
	--rstPath
	nFromLink               := linkid;
	rstPathCount		:= 0;
	tmpPathArray		:= ARRAY[cast(nFromLink as varchar)];
	tmpPathCount		:= 1;
	tmpPathIndex		:= 1;
	nStartNode          := nodeid;
	tmpLastNodeArray	:= ARRAY[nStartNode];
	tmpLastLinkArray	:= ARRAY[nFromLink];
	num_flag = 0;
	-- search
        WHILE tmpPathIndex <= tmpPathCount LOOP
                --raise INFO 'tmpPathArray = %', tmpPathArray[tmpPathIndex];
                -----stop if tmpPath has been more than layer given
                if array_upper(regexp_split_to_array(tmpPathArray[tmpPathIndex], E'\\,+'),1) < 10 then
                
				select count(*)
				into num_flag
				from rdb_link
				where link_type = 4 and (start_node_id = tmpLastNodeArray[tmpPathIndex] or end_node_id = tmpLastNodeArray[tmpPathIndex]);
				
				--select count(*)
				--into end_flag
				--from rdb_link
				--where link_type in (0,1,2,3,5,6,7,10,11,12,13) and (start_node_id = tmpLastNodeArray[tmpPathIndex] or end_node_id = tmpLastNodeArray[tmpPathIndex]);
				
                        if  num_flag is not null and num_flag <> 0 then
                                return tmpLastNodeArray[tmpPathIndex];
                        --elsif end_flag is not null and end_flag <> 0  then
				--return null;
                        else
                                for rec in 
						select nextroad,nextnode,dir
						from(
							SELECT link_id as nextroad,'(2)' as dir,end_node_id as nextnode
							FROM rdb_link
							where link_type in (8,9) and start_node_id = tmpLastNodeArray[tmpPathIndex]  and one_way in (1,2)
							 
							union

							SELECT link_id as nextroad, '(3)' as dir,start_node_id as nextnode
							FROM rdb_link
							where link_type in (8,9) and end_node_id = tmpLastNodeArray[tmpPathIndex]  and one_way in (1,3)
						) as a
                                loop
                                        
                                        if 		not (rec.nextroad in (nFromLink, tmpLastLinkArray[tmpPathIndex]))
                                                and	not ((rec.nextroad||rec.dir) = ANY(regexp_split_to_array(tmpPathArray[tmpPathIndex], E'\\,+')))
                                                and 	not (rec.nextnode = ANY(tmpLastNodeArray))
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
    return null;
        
END;
$$;

select find_innerlink_shortlink_intersectnode();

delete from wellshape_start_end_node_rdb_tbl
WHERE s_node is null or e_node is null;


CREATE OR REPLACE FUNCTION get_mid_postion_node(startnode bigint,endnode bigint)
  RETURNS bigint
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
	flag                  integer;
	count_num             integer;
BEGIN
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
	flag                    := 0;
	-- search
        WHILE tmpPathIndex <= tmpPathCount LOOP
                --raise INFO 'tmpPathArray = %', tmpPathArray[tmpPathIndex];
                -----stop if tmpPath has been more than layer given
                if array_upper(regexp_split_to_array(tmpPathArray[tmpPathIndex], E'\\,+'),1) < 10 then

			SELECT count(*)
			into count_num
			FROM rdb_link
			where link_type = 4 and (start_node_id = tmpLastNodeArray[tmpPathIndex] or end_node_id = tmpLastNodeArray[tmpPathIndex]);
			if count_num >= 3 then
				return tmpLastNodeArray[tmpPathIndex];
			end if;
			if flag < 2 then
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
								SELECT link_id as nextroad,'(2)' as dir,end_node_id as nextnode
								from rdb_link
								where link_type = 4 and start_node_id = tmpLastNodeArray[tmpPathIndex] and one_way in (1,2,0)

								union

								SELECT link_id as nextroad,'(2)' as dir,start_node_id as nextnode
								from rdb_link
								where link_type = 4 and end_node_id = tmpLastNodeArray[tmpPathIndex] and one_way in (1,3,0)
							) as e
					loop
						
						if 		not (rec.nextroad in (nFromLink, tmpLastLinkArray[tmpPathIndex]))
							and	not ((rec.nextroad||rec.dir) = ANY(regexp_split_to_array(tmpPathArray[tmpPathIndex], E'\\,+')))
							and 	not (rec.nextnode = ANY(tmpLastNodeArray))
						then
							tmpPathCount		:= tmpPathCount + 1;
							tmpPathArray		:= array_append(tmpPathArray, cast(tmpPathArray[tmpPathIndex]||','||rec.nextroad||rec.dir as varchar));
							tmpLastNodeArray	:= array_append(tmpLastNodeArray, cast(rec.nextnode as bigint));
							tmpLastLinkArray	:= array_append(tmpLastLinkArray, cast(rec.nextroad as bigint));
						end if;
						
					end loop;
				end if;
			else
				flag = flag - 1;
			end if;
                else
                        return null;
                end if;
                tmpPathIndex := tmpPathIndex + 1;
        END LOOP;
       return null;
END;
$$;

UPDATE wellshape_start_end_node_rdb_tbl
SET postion_node=get_mid_postion_node(s_node,e_node);

DELETE FROM wellshape_start_end_node_rdb_tbl
WHERE postion_node is null;



CREATE OR REPLACE FUNCTION test_get_expand_box(nodeid bigint,range float)
  returns geometry
  language plpgsql
as $$
declare
        box_geom  geometry;
BEGIN
        select ST_Expand(exten_box::geometry, range) into box_geom
        from (
                select st_envelope(ST_Collect(the_geom)) as exten_box
                FROM rdb_node
                WHERE node_id = nodeid
        ) as a;
        
        IF FOUND THEN
                RETURN ST_SETSRID(box_geom, 4326);
        END IF;
        RETURN NULL;
end;
$$;


CREATE OR REPLACE FUNCTION test_innerlink_shortlink(nodeid bigint)
  returns integer
  language plpgsql
as $$
declare
        box_geom  geometry;
	sum_num     integer;
BEGIN
	sum_num = 0;
	box_geom = ST_ASEWKT(test_get_expand_box(nodeid,0.0007));
	select count(*)
	into sum_num
	from(
		select distinct postion_node
		from wellshape_start_end_node_rdb_tbl as a
		left join rdb_node as b
		on a.postion_node = b.node_id
		where ST_Intersects(box_geom,b.the_geom)
	) as a;
	if sum_num >= 4 then
		return 1;
	else
		return 0;
	end if;
end;
$$;

delete from wellshape_start_end_node_rdb_tbl
 WHERE test_innerlink_shortlink(postion_node) = 0;


CREATE OR REPLACE FUNCTION test_innerlink_shortlink_intersect_num(nodeid bigint)
  returns integer
  language plpgsql
as $$
declare
        box_geom  geometry;
	sum_num     integer;
BEGIN
	sum_num = 0;
	box_geom = ST_ASEWKT(test_get_expand_box(nodeid,0.0007));
	SELECT count(*)
	into sum_num
	FROM rdb_link
	where link_type = 4 and ST_Intersects(box_geom,the_geom);
	
	if sum_num >= 11 then
		return 1;
	else
		return 0;
	end if;
end;
$$;

delete from wellshape_start_end_node_rdb_tbl
WHERE test_innerlink_shortlink_intersect_num(postion_node) = 0;


CREATE OR REPLACE FUNCTION test_findpasslink_two_node(startnode bigint, endnode bigint)
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
	flag                  integer;
	count_num             integer;
BEGIN
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
	flag                    := 0;
	-- search
        WHILE tmpPathIndex <= tmpPathCount LOOP
                --raise INFO 'tmpPathArray = %', tmpPathArray[tmpPathIndex];
                -----stop if tmpPath has been more than layer given
                if array_upper(regexp_split_to_array(tmpPathArray[tmpPathIndex], E'\\,+'),1) < 10 then

			SELECT count(*)
			into count_num
			FROM rdb_link
			where link_type = 4 and (start_node_id = tmpLastNodeArray[tmpPathIndex] or end_node_id = tmpLastNodeArray[tmpPathIndex]);
			if count_num >= 3 then
				flag = flag + 1;
			end if;
			if flag < 2 then
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
								SELECT link_id as nextroad,'(2)' as dir,end_node_id as nextnode
								from rdb_link
								where link_type = 4 and start_node_id = tmpLastNodeArray[tmpPathIndex] and one_way in (1,2,0)

								union

								SELECT link_id as nextroad,'(2)' as dir,start_node_id as nextnode
								from rdb_link
								where link_type = 4 and end_node_id = tmpLastNodeArray[tmpPathIndex] and one_way in (1,3,0)
							) as e
					loop
						
						if 		not (rec.nextroad in (nFromLink, tmpLastLinkArray[tmpPathIndex]))
							and	not ((rec.nextroad||rec.dir) = ANY(regexp_split_to_array(tmpPathArray[tmpPathIndex], E'\\,+')))
							and 	not (rec.nextnode = ANY(tmpLastNodeArray))
						then
							tmpPathCount		:= tmpPathCount + 1;
							tmpPathArray		:= array_append(tmpPathArray, cast(tmpPathArray[tmpPathIndex]||','||rec.nextroad||rec.dir as varchar));
							tmpLastNodeArray	:= array_append(tmpLastNodeArray, cast(rec.nextnode as bigint));
							tmpLastLinkArray	:= array_append(tmpLastLinkArray, cast(rec.nextroad as bigint));
						end if;
						
					end loop;
				end if;
			else
				flag = flag - 1;
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


----drop table test_result_innerlink
insert into wellshape_innerlink_rdb_tbl
select distinct innerlink::bigint
from(
	select unnest(regexp_split_to_array(a.passlink, E'\\,+')) as innerlink
	from(
	select s_node,e_node, test_findpasslink_two_node(s_node,e_node) as passlink
	from wellshape_start_end_node_rdb_tbl
	) as a
	where a.passlink is not null
) as a;