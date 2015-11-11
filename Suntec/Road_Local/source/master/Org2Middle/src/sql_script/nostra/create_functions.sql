
CREATE OR REPLACE FUNCTION mid_convert_weekflag(day smallint)
	RETURNS integer
	LANGUAGE plpgsql volatile
AS $$
DECLARE
	weekflag 		integer;
BEGIN
	-- every day
    if day = 0 then
        weekflag := 0;
    -- working day
    elseif day = 1 then
        weekflag := (1<<1) | (1<<2) | (1<<3) | (1<<4) | (1<<5);
    -- monday-saturday
    elseif day = 2 then
        weekflag := (1<<1) | (1<<2) | (1<<3) | (1<<4) | (1<<5) | (1<<6);
    -- saturday-sunday
    elseif day = 3 then
        weekflag := (1<<0) | (1<<6);
    -- sunday
    elseif day = 4 then
        weekflag := (1<<0);
    -- holiday
    elseif day = 5 then
        weekflag := 127 | (1<<7) | (1<<9);
    else
    	raise EXCEPTION 'org_turntable field error, day = %', day;
    end if;
    
	return weekflag;
END;
$$;


CREATE OR REPLACE FUNCTION mid_get_connect_node(link_a bigint, link_b bigint)
    RETURNS bigint
    LANGUAGE plpgsql
AS $$
DECLARE
	node_id		bigint;
BEGIN
	select	case 
			when a.start_node_id in (b.start_node_id, b.end_node_id) then a.start_node_id
			when a.end_node_id in (b.start_node_id, b.end_node_id) then a.end_node_id
			else null::bigint
			end
	from
	(select start_node_id, end_node_id from temp_topo_link where routeid = link_a)as a,
	(select start_node_id, end_node_id from temp_topo_link where routeid = link_b)as b
	into node_id;
	return node_id;
END;
$$;

CREATE OR REPLACE FUNCTION mid_convert_regulation()
    RETURNS smallint
    LANGUAGE plpgsql
AS $$
DECLARE
	rec record;
	cur_regulation_id integer;
	nIndex integer;
	nCount integer;
BEGIN
	-- regulation_id
	select (case when max(regulation_id) is null then 0 else max(regulation_id) end)
	from regulation_relation_tbl
	into cur_regulation_id;
	
    FOR rec IN
    
    	select	a.routeid, node_id, link_count, link_array, b.array_condid
    	from
    	(
    		select	routeid, link_count, link_array,
    				mid_get_connect_node(link_array[1], link_array[2]) as node_id
    		from
    		(
	    		select routeid, array_agg(arc) as link_array, count(arc) as link_count
	    		from
	    		(
		    		select routeid, arc_index, arc::bigint as arc
		    		from
		    		(
			    	select routeid, 1 as arc_index, arc1 as arc from org_turntable
			    	union
			    	select routeid, 2 as arc_index, arc2 as arc from org_turntable
			    	union
			    	select routeid, 3 as arc_index, arc3 as arc from org_turntable
			    	union
			    	select routeid, 4 as arc_index, arc4 as arc from org_turntable
			    	union
			    	select routeid, 5 as arc_index, arc5 as arc from org_turntable
			    	union
			    	select routeid, 6 as arc_index, arc6 as arc from org_turntable
			    	)as t
			    	where arc > 0
			    	order by routeid, arc_index
		    	)as t2
		    	group by routeid
	    	)as t3
    	)as a
    	left join
    	(
    		select routeid, array_agg(cond_id) as array_condid
    		from
    		(
	    		select m.routeid, n.cond_id
	    		from 
	    		(
	    			select distinct routeid, day, time
	    			from
	    			(
			    		select routeid, day, time1 as time from org_turntable where time1 is not null
						union
						select routeid, day, time2 as time from org_turntable where time2 is not null
						union
						select routeid, day, time3 as time from org_turntable where time3 is not null
						union
						select routeid, day, time4 as time from org_turntable where time4 is not null
						union
						select routeid, day, time1 as time from org_turntable where time1 is null and time2 is null and time3 is null and time4 is null
					)as t
	    		)as m
	    		left join temp_condition_regulation_tbl as n
	    		on m.day = n.day and m.time = n.time
    		)as t2
    		group by routeid
    	)as b
    	on a.routeid = b.routeid
    LOOP
		-- current regulation id
    	cur_regulation_id := cur_regulation_id + 1;
		
    	-- insert into regulation_item_tbl
		nIndex := 1;
		while nIndex <= rec.link_count LOOP
			if nIndex = 1 then
	    		insert into regulation_item_tbl
	    					("regulation_id", "linkid", "nodeid", "seq_num")
	    			VALUES	(cur_regulation_id, rec.link_array[nIndex], null, nIndex);
	    		insert into regulation_item_tbl
	    					("regulation_id", "linkid", "nodeid", "seq_num")
	    			VALUES	(cur_regulation_id, null, rec.node_id, nIndex+1);
	    	else
	    		insert into regulation_item_tbl
	    					("regulation_id", "linkid", "nodeid", "seq_num")
	    			VALUES	(cur_regulation_id, rec.link_array[nIndex], null, nIndex+1);
	    	end if;
			nIndex := nIndex + 1;
		END LOOP;
		
	   	-- insert into regulation_relation_tbl
		nCount := array_upper(rec.array_condid, 1);
		nIndex := 1;
		while nIndex <= nCount LOOP
	    	insert into regulation_relation_tbl
	    						("regulation_id", "nodeid", "inlinkid", "outlinkid", "condtype", "cond_id")
	    				VALUES	(cur_regulation_id, 
	    						 rec.node_id, rec.link_array[1], rec.link_array[rec.link_count], 
	    						 1, rec.array_condid[nIndex]);
			nIndex := nIndex + 1;
		END LOOP;    	
    	
    END LOOP;
    return 1;
END;
$$;


CREATE OR REPLACE FUNCTION rdb_cnv_display_class(rdlnclass smallint, level smallint, tollway character varying, c_access character varying, fw smallint)
	RETURNS smallint 
	LANGUAGE plpgsql
AS $$
    BEGIN
        RETURN CASE
            WHEN RDLNCLASS in (71) THEN 14  
            WHEN RDLNCLASS in (61,63,72) THEN 3   
            WHEN (C_ACCESS = 'Y' AND LEVEL = 5)  OR RDLNCLASS in (11,14) THEN 12 
            WHEN (RDLNCLASS IN (12,13,21,22,23,24,26,27,28,41,42,43,44,46,51,62) OR FW = 1)  AND LEVEL = 5 THEN 11  
            WHEN (RDLNCLASS IN (12,13,21,22,23,24,26,27,28,41,42,43,44,46,51,62)  OR FW = 1 ) AND LEVEL = 4 THEN 7  
            WHEN  RDLNCLASS IN (12,13,21,22,23,24,26,27,28,41,42,43,44,46,51,62) AND LEVEL = 3 THEN 6  
            WHEN (RDLNCLASS IN (12,13,21,22,23,24,26,27,28,41,42,43,44,46,51,62) AND LEVEL = 2) OR (RDLNCLASS IN (12,13,21,26,27,28,46) AND LEVEL = 1) THEN 5  
            ELSE 4
        END;
    END;
$$;

CREATE OR REPLACE FUNCTION rdb_cnv_link_type(fw smallint, rdlnclass smallint, oneway character varying, level smallint, dtype smallint) 
	RETURNS smallint
    LANGUAGE plpgsql
AS $$
    BEGIN
	    RETURN CASE
			WHEN fw = 4 THEN 0
	        WHEN rdlnclass = 14 THEN 3
	        WHEN rdlnclass in (12,13) THEN 5
	        WHEN fw IN (5,6) THEN 8
	        WHEN fw = 9 THEN 4
	    	WHEN fw = 7 THEN 7
	    	WHEN rdlnclass in (26,46) THEN 6
	    	WHEN dtype = 1 THEN 2
	        ELSE 1
	    END;
    END;
$$;

CREATE OR REPLACE FUNCTION rdb_cnv_road_type(rdlnclass smallint, level smallint, c_access character varying, fw smallint) 
RETURNS smallint
    LANGUAGE plpgsql
    AS $$
    BEGIN
	    RETURN CASE
	        WHEN rdlnclass IN (71) THEN 10
	        WHEN rdlnclass = 63 THEN 7
			WHEN rdlnclass IN (61,72) THEN 8
			WHEN rdlnclass IN (24,44) THEN 14
	    	WHEN (c_access = 'Y' and level = 5 and fw = 1) or rdlnclass IN (11,12,13,14) THEN 0
	    	---WHEN rdlnclass IN (26,27,28,46) THEN 5
	    	WHEN (rdlnclass IN (21,22,23,26,27,28,41,43,46,51) OR fw = 1) AND level = 5  THEN 2
	    	WHEN (rdlnclass IN (21,22,23,26,27,28,41,42,43,46,51) OR fw = 1) AND level = 4 THEN 3
	    	WHEN rdlnclass IN (21,22,23,26,27,28,41,42,43,46,51) AND level IN (2,3) THEN 4
	    	ELSE 6
	    END;
    END;
$$;

CREATE OR REPLACE FUNCTION rdb_cnv_speed_class(speed smallint) RETURNS smallint
    LANGUAGE plpgsql
    AS $$
    BEGIN
	    RETURN CASE
	        WHEN speed > 130 THEN 1
	        WHEN speed > 100 and speed <= 130 THEN 2
	        WHEN speed > 90 and speed <= 100 THEN 3
	        WHEN speed > 70 and speed <= 90 THEN 4
	    	WHEN speed > 50 and speed <= 70 THEN 5
	    	WHEN speed > 30 and speed <= 50 THEN 6
	    	WHEN speed > 11 and speed <= 30 THEN 7
	    	WHEN speed < 11 THEN 8
	    	ELSE 8
	    END;
    END;
$$;

CREATE OR REPLACE FUNCTION rdb_cnv_function_code(level smallint) 
RETURNS smallint
    LANGUAGE plpgsql
    AS $$
    BEGIN
	    RETURN CASE
	        WHEN level = 5 THEN 1
	        WHEN level = 4 THEN 2
	        WHEN level = 3 THEN 3
	        WHEN level = 2 THEN 4
	        WHEN level = 1 THEN 5
	        ELSE 5
	    END;
    END;
$$;

CREATE OR REPLACE FUNCTION rdb_cnv_lane_dir(oneway character varying) 
RETURNS smallint
    LANGUAGE plpgsql
    AS $$
    BEGIN
	    RETURN CASE
	        WHEN oneway = 'FT' THEN 1
	        WHEN oneway = 'TF' THEN 2
	        WHEN oneway is null THEN 3
	        ELSE 0
	    END;
    END;
$$; 

CREATE OR REPLACE FUNCTION rdb_cnv_lanecount_downflow(oneway character varying, lanecount smallint) 
RETURNS smallint
    LANGUAGE plpgsql
    AS $$
    BEGIN
	    RETURN CASE
	        WHEN oneway = 'FT' or oneway is null THEN lanecount
	        ELSE 0
	    END;
    END;
$$;

CREATE OR REPLACE FUNCTION rdb_cnv_lanecount_upflow(oneway character varying, lanecount smallint) 
RETURNS smallint
    LANGUAGE plpgsql
    AS $$
    BEGIN
	    RETURN CASE
	        WHEN oneway = 'TF' or oneway is null THEN lanecount
	        ELSE 0
	    END;
    END;
$$;

CREATE OR REPLACE FUNCTION rdb_cnv_speedlimit_downflow(oneway character varying, spd_limit smallint, speed smallint)
  RETURNS double precision
  LANGUAGE plpgsql VOLATILE
  AS $$
DECLARE
	rtnvalue INT;
BEGIN
	IF oneway = 'TF' then
		return 0;
	ELSE
		IF spd_limit is not null and spd_limit > 0 THEN
			if spd_limit > 200 then
				return -1;
			else
				return spd_limit;
			end if;
		END IF;
	END IF;
	
    RETURN 
	CASE WHEN speed > 0 THEN speed
        ELSE 0 
    END;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_cnv_speedlimit_upflow(oneway character varying, spd_limit smallint, speed smallint)
  RETURNS double precision
  LANGUAGE plpgsql VOLATILE
  AS $$
DECLARE
	rtnvalue INT;
BEGIN
	IF oneway = 'FT' then
		return 0;
	ELSE
		IF spd_limit is not null and spd_limit > 0 THEN
			if spd_limit > 200 then
				return -1;
			else
				return spd_limit;
			end if;
		END IF;
	END IF;
	
    RETURN 
	CASE WHEN speed > 0 THEN speed
        ELSE 0 
    END;
END;
$$;


CREATE OR REPLACE FUNCTION rdb_cnv_speedsource_downflow(oneway character varying, spd_limit smallint, speed smallint)
  RETURNS SMALLINT
  LANGUAGE plpgsql VOLATILE
  AS $$
DECLARE
	rtnvalue INT;
BEGIN
	IF oneway = 'TF' then
		return 0;
	ELSE
		IF spd_limit is not null and spd_limit > 0 THEN
			if spd_limit > 200 then
				return 0;
			else
				return 3;
			end if;
		END IF;
	END IF;
	
    RETURN 
	CASE WHEN speed > 0 THEN 3 
        ELSE 0 
    END;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_cnv_speedsource_upflow(oneway character varying, spd_limit smallint, speed smallint)
  RETURNS SMALLINT
  LANGUAGE plpgsql VOLATILE
  AS $$
DECLARE
	rtnvalue INT;
BEGIN
	IF oneway = 'FT' then
		return 0;
	ELSE
		IF spd_limit is not null and spd_limit > 0 THEN
			if spd_limit > 200 then
				return 0;
			else
				return 3;
			end if;
		END IF;
	END IF;
	
    RETURN 
	CASE WHEN speed > 0 THEN 3
        ELSE 0 
    END;
END;
$$;


CREATE OR REPLACE FUNCTION get_width_downflow(oneway character varying, width integer) 
RETURNS smallint
    LANGUAGE plpgsql
    AS $$
    BEGIN
		IF oneway = 'TF' THEN 
			RETURN 4;
		ELSE
			RETURN CASE
				WHEN width > 0 and width <= 3 THEN 0
				WHEN width > 3 and width <= 5.5 THEN 1
				WHEN width > 5.5 and width <= 13 THEN 2
				WHEN width > 13 THEN 3
				ELSE 4
			END;
		END IF;
    END;
$$;

CREATE OR REPLACE FUNCTION get_width_upflow(oneway character varying, width integer) 
RETURNS smallint
    LANGUAGE plpgsql
    AS $$
    BEGIN
		IF oneway = 'FT' THEN 
			RETURN 4;
		ELSE
			RETURN CASE
				WHEN width > 0 and width <= 3 THEN 0
				WHEN width > 3 and width <= 5.5 THEN 1
				WHEN width > 5.5 and width <= 13 THEN 2
				WHEN width > 13 THEN 3
				ELSE 4
			END;
		END IF;
    END;
$$;

CREATE OR REPLACE FUNCTION make_temp_org_junctionview_node(arc1 bigint,arc2 bigint)
  RETURNS bigint 
LANGUAGE plpgsql volatile
AS $$
DECLARE
	rec			    record;
	in_s_node_id                bigint;
	in_e_node_id                bigint;
	out_s_node_id               bigint;
	out_e_node_id               bigint;
	
BEGIN	
	
	SELECT start_node_id, end_node_id
	into in_s_node_id, in_e_node_id
	FROM temp_topo_link
	where routeid = arc1;

	SELECT start_node_id, end_node_id
	into out_s_node_id, out_e_node_id
	FROM temp_topo_link
	where routeid = arc2;
	
	if in_e_node_id = out_s_node_id or in_e_node_id = out_e_node_id then
		return in_e_node_id;
	end if;
	if in_s_node_id = out_s_node_id or in_s_node_id = out_e_node_id then
		return in_s_node_id;
	end if;
	return 0;
END;
$$;

CREATE OR REPLACE FUNCTION mid_get_laneinfo(lane integer,lane_list integer[])
  RETURNS text 
LANGUAGE plpgsql volatile
AS $$
DECLARE
	laneInfoArray               smallint[];
	laneInfoStr                 text;
	lane_list_len               integer;
	lane_num                    integer;
	p_sum                       integer;
BEGIN	
		laneInfoStr = '';
		lane_num = lane;
		if lane < 1 then
			return laneInfoStr;
		end if;
		p_sum = 1;
		while p_sum <= lane_num loop
			if p_sum = any(lane_list) then
				laneInfoStr = laneInfoStr || 1;
			else
				laneInfoStr = laneInfoStr || 0;
			end if; 
			p_sum = p_sum + 1;
		end loop;
	
	return laneInfoStr;
END;
$$;

CREATE OR REPLACE FUNCTION mid_findpasslinkbybothlinks(nfromlink bigint, ntolink bigint, startnode integer, endnode integer, dir integer)
  RETURNS character varying
LANGUAGE plpgsql volatile
AS $$
DECLARE
	rstPathStr  		varchar;
	rstPathCount		integer;
	rstPath				varchar;
	
	tmpPathArray		varchar[];
	tmpLastNodeArray	bigint[];
	tmpLastLinkArray	bigint[];
	tmpPathCount		integer;
	tmpPathIndex		integer;
	rec        			record;
	nStartNode          integer;
	nScondNode          integer;
BEGIN
	--rstPath
	rstPathCount		:= 0;
	tmpPathArray		:= ARRAY[cast(nFromLink as varchar)];
	tmpPathCount		:= 1;
	tmpPathIndex		:= 1;
	nStartNode          := endNode;
	nScondNode          := startNode;
	if dir = 1 then
		tmpLastNodeArray	:= ARRAY[nStartNode,nScondNode];
		tmpLastLinkArray	:= ARRAY[nFromLink,nFromLink];
		tmpPathArray		:= array_append(tmpPathArray,cast(nFromLink as varchar));
		tmpPathCount		:= 2;
	elseif dir = 0 then
		tmpLastNodeArray	:= ARRAY[nStartNode];
		tmpLastLinkArray	:= ARRAY[nFromLink];
	else 
		return null;
	end if;
	
	-- search
		
		WHILE tmpPathIndex <= tmpPathCount LOOP
			--raise INFO 'tmpPathArray = %', tmpPathArray[tmpPathIndex];
			-----stop if tmpPath has been more than layer given
			if array_upper(regexp_split_to_array(tmpPathArray[tmpPathIndex], E'\\|+'),1) < 16 then
				if tmpLastLinkArray[tmpPathIndex] = nToLink then
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
								SELECT a.routeid as nextroad,'(2)' as dir,b.end_node_id as nextnode
								FROM org_l_tran as a
								left join temp_topo_link as b
								on a.routeid = b.routeid
								where b.start_node_id = tmpLastNodeArray[tmpPathIndex] and (a.oneway is null or a.oneway = 'FT')

								union

								SELECT c.routeid as nextroad,'(3)' as dir,d.start_node_id as nextnode
								FROM org_l_tran as c
								left join temp_topo_link as d
								on c.routeid = d.routeid
								where d.end_node_id = tmpLastNodeArray[tmpPathIndex] and (c.oneway is null or c.oneway = 'TF')
							) as e
					loop
						
						if 		not (rec.nextroad in (nFromLink, tmpLastLinkArray[tmpPathIndex]))
							and	not ((rec.nextroad||rec.dir) = ANY(regexp_split_to_array(tmpPathArray[tmpPathIndex], E'\\|+')))
						then
							tmpPathCount		:= tmpPathCount + 1;
							tmpPathArray		:= array_append(tmpPathArray, cast(tmpPathArray[tmpPathIndex]||'|'||rec.nextroad||rec.dir as varchar));
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
		if array_upper(regexp_split_to_array(rstPathStr,E'\\|+'),1) > 1 then
			return rstPathStr;
		else
			return null;
		end if;
END;
$$;


CREATE OR REPLACE FUNCTION mid_junctionview_innerlink_handle(nfromlink bigint, startnode integer)
  RETURNS character varying[]
LANGUAGE plpgsql volatile
AS $$
DECLARE
	rstPathStr  		varchar;
	rstPathCount		integer;
	rstPath			varchar;
	innerlink		integer;
	tmpPathArray		varchar[];
	tmpLastNodeArray	bigint[];
	tmpLastLinkArray	bigint[];
	tmpPathCount		integer;
	tmpPathIndex		integer;
	rec        		record;
	nStartNode          	integer;
	flag                    integer;
	pathlength              integer;
	result                  character varying[];
BEGIN
	--rstPath
	rstPathCount		:= 0;
	tmpPathArray		:= ARRAY[cast(nFromLink as varchar)];
	tmpPathCount		:= 1;
	tmpPathIndex		:= 1;
	nStartNode            := startnode;
	tmpLastNodeArray	:= ARRAY[nStartNode];
	tmpLastLinkArray	:= ARRAY[nFromLink];
	flag 			:= 0;
	
	-- search
		
		WHILE tmpPathIndex <= tmpPathCount LOOP
			--raise INFO 'tmpPathArray = %', tmpPathArray[tmpPathIndex];
			-----stop if tmpPath has been more than layer given
			if array_upper(regexp_split_to_array(tmpPathArray[tmpPathIndex], E'\\|+'),1) < 16 then
				select fw
				into innerlink
				from org_l_tran
				where routeid = tmpLastLinkArray[tmpPathIndex];
				if innerlink != 9 and flag = 0 then
					flag = 1;
					rstPathCount	:= rstPathCount + 1;
					rstPath		:= cast(tmpPathArray[tmpPathIndex] as varchar);
					rstPath		:= replace(rstPath, '(2)', '');
					rstPath		:= replace(rstPath, '(3)', '');
					rstPathStr	:=  rstPath;
					pathlength = array_upper(regexp_split_to_array(rstPathStr,E'\\|+'),1);
				elsif innerlink != 9 and flag = 1 then
					rstPathCount	:= rstPathCount + 1;
					rstPath		:= cast(tmpPathArray[tmpPathIndex] as varchar);
					rstPath		:= replace(rstPath, '(2)', '');
					rstPath		:= replace(rstPath, '(3)', '');
					rstPathStr	:=  rstPath;
				elsif flag = 0 then
					for rec in 
						select e.nextroad,e.dir,e.nextnode
							from
							(
								SELECT a.routeid as nextroad,'(2)' as dir,b.start_node_id as nextnode
								FROM org_l_tran as a
								left join temp_topo_link as b
								on a.routeid = b.routeid
								where b.end_node_id = tmpLastNodeArray[tmpPathIndex] and (a.oneway is null or a.oneway = 'FT')

								union

								SELECT c.routeid as nextroad,'(3)' as dir,d.end_node_id as nextnode
								FROM org_l_tran as c
								left join temp_topo_link as d
								on c.routeid = d.routeid
								where d.start_node_id = tmpLastNodeArray[tmpPathIndex] and (c.oneway is null or c.oneway = 'TF')
							) as e
					loop
						
						if 		not (rec.nextroad in (nFromLink, tmpLastLinkArray[tmpPathIndex]))
							and	not ((rec.nextroad||rec.dir) = ANY(regexp_split_to_array(tmpPathArray[tmpPathIndex], E'\\|+')))
						then
							tmpPathCount		:= tmpPathCount + 1;
							tmpPathArray		:= array_append(tmpPathArray, cast(tmpPathArray[tmpPathIndex]||'|'||rec.nextroad||rec.dir as varchar));
							tmpLastNodeArray	:= array_append(tmpLastNodeArray, cast(rec.nextnode as bigint));
							tmpLastLinkArray	:= array_append(tmpLastLinkArray, cast(rec.nextroad as bigint));
						end if;
						
					end loop;
				end if;
			else
				return null;
			end if;
			if pathlength = array_upper(regexp_split_to_array(rstPathStr,E'\\|+'),1) then
				result = array_append(result,rstPathStr);
				rstPathStr = null;
			end if;
			tmpPathIndex := tmpPathIndex + 1;
		END LOOP;
	return result;
END;
$$;
---------------------------------------------------------------------
---mid_make_link_list_by_nodes
--------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_make_link_list_by_nodes(nodeid_list bigint[])
  RETURNS bigint[]
  LANGUAGE plpgsql AS
$$ 
DECLARE
rtn_link_list bigint[];
valid_node_cnt bigint;
iterator bigint;
linkid bigint;
node_link_str varchar;
pos bigint;
BEGIN
	rtn_link_list := array[]::bigint[]; 
	
	if nodeid_list is null then
		return rtn_link_list;
	end if;

	iterator := 1;

	node_link_str := array_to_string(nodeid_list, ',');
	--raise info 'node_link_str = %', node_link_str;
	 
	valid_node_cnt := array_upper(string_to_array(node_link_str,','),1);

	while iterator < valid_node_cnt loop
		select routeid into linkid
		from temp_topo_link 
		where ((start_node_id = nodeid_list[iterator] and end_node_id = nodeid_list[iterator +1]) 
		OR (end_node_id = nodeid_list[iterator] and start_node_id = nodeid_list[iterator +1]));

		if found then
			rtn_link_list := array_append(rtn_link_list,linkid);
		else 
			raise info 'It does not found link with node1 = % and node2 =%', nodeid_list[iterator],nodeid_list[iterator+1];
		end if;
		
		iterator := iterator + 1;
	end loop;

	-- test whether found link cnt is correct
	if array_upper(rtn_link_list,1) <> (valid_node_cnt - 1) then
		raise info 'Found link error, may be node dis-connect';
		rtn_link_list := array[NULL];
	end if;

	return rtn_link_list;
END;
$$;