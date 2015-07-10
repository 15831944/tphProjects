-------------------------------------------------------------------------------------------------------------
-- check logic
-------------------------------------------------------------------------------------------------------------


CREATE OR REPLACE FUNCTION test_ni_check_regulation_linkrow()
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
			select	table_name, gid, node_array, 
					array_agg(link_id) as link_array, 
					array_agg(seq_num) as seq_array,
					array_agg(one_way) as oneway_array,
					array_agg(snodeid) as snode_array,
					array_agg(enodeid) as enode_array
			from
			(
				select 	m.table_name, m.gid, m.node_array, 
						m.link_id, seq_num, n.direction as one_way, 
						(case when s.adjoin_nid != '0' and s.id > s.adjoin_nid then s.adjoin_nid else s.id end) as snodeid, 
						(case when e.adjoin_nid != '0' and e.id > s.adjoin_nid then e.adjoin_nid else e.id end) as enodeid
				from
				(
					select table_name, gid, node_array, link_array[seq_num] as link_id, seq_num
					from
					(
						select 	table_name, gid, link_array, node_array, 
								generate_series(1,array_upper(link_array,1)) as seq_num
						from
						(
							--select 	'org_c' as table_name, gid,
							--		ARRAY[inlinkid,null,outlinkid] as link_array,
							--		ARRAY[null,id,null] as node_array
							--from org_c
							--where condtype = '1'
							--
							--union
							select 	table_name, gid,
									array_agg(linkid) as link_array,
									array_agg(nodeid) as node_array
							from
							(
								select 'org_cond' as table_name, a.gid, b.linkid, b.nodeid
								from org_cond as a
								left join org_cnl as b
								on a.folder = b.folder and a.mapid = b.mapid and a.condid = b.condid
								where a.condtype = '1'
								order by table_name, a.gid, b.seq_nm::integer
							)as t
							group by table_name, gid
						)as a
					)as b
				)as m
				left join org_r as n
				on m.link_id = n.id
				left join org_n as s
				on n.snodeid = s.id
				left join org_n as e
				on n.enodeid = e.id
				order by m.table_name, m.gid, seq_num
			)as a
			group by table_name, gid, node_array
		)as r
	loop
		-- check linkrow continuable
		nCount		:= array_upper(rec.seq_array, 1);
		nIndex		:= 1;
		while nIndex <= nCount loop
			if nIndex = 1 then
				if rec.snode_array[nIndex] = rec.node_array[nIndex+1] then
					if rec.oneway_array[nIndex] = '2' then
						raise WARNING '% regulation linkrow warning, gid = %', rec.table_name, rec.gid;
					end if;
				elseif rec.enode_array[nIndex] = rec.node_array[nIndex+1] then
					if rec.oneway_array[nIndex] = '3' then
						raise WARNING '% regulation linkrow warning, gid = %', rec.table_name, rec.gid;
					end if;
				else
					raise EXCEPTION '% regulation linkrow warning, gid = %', rec.table_name, rec.gid;
				end if;
			elseif nIndex = 2 then
				--
			elseif nIndex = 3 then
				if rec.snode_array[nIndex] = rec.node_array[nIndex-1] then
					if rec.oneway_array[nIndex] = '3' then
						raise WARNING '% regulation linkrow warning, gid = %', rec.table_name, rec.gid;
					end if;
				elseif rec.enode_array[nIndex] = rec.node_array[nIndex-1] then
					if rec.oneway_array[nIndex] = '2' then
						raise WARNING '% regulation linkrow warning, gid = %', rec.table_name, rec.gid;
					end if;
				else
					raise EXCEPTION '% regulation linkrow warning, gid = %', rec.table_name, rec.gid;
				end if;
			else
				if nIndex < nCount then
					if rec.snode_array[nIndex] in (rec.snode_array[nIndex+1], rec.enode_array[nIndex+1]) then
						if rec.oneway_array[nIndex] = '2' then
						raise WARNING '% regulation linkrow warning, gid = %', rec.table_name, rec.gid;
						end if;
					elseif rec.enode_array[nIndex] in (rec.snode_array[nIndex+1], rec.enode_array[nIndex+1]) then
						if rec.oneway_array[nIndex] = '3' then
						raise WARNING '% regulation linkrow warning, gid = %', rec.table_name, rec.gid;
						end if;
					else
					raise EXCEPTION '% regulation linkrow warning, gid = %', rec.table_name, rec.gid;
					end if;
				end if;
				
				if True then
					if rec.snode_array[nIndex] in (rec.snode_array[nIndex-1], rec.enode_array[nIndex-1]) then
						if rec.oneway_array[nIndex] = '3' then
						raise WARNING '% regulation linkrow warning, gid = %', rec.table_name, rec.gid;
						end if;
					elseif rec.enode_array[nIndex] in (rec.snode_array[nIndex-1], rec.enode_array[nIndex-1]) then
						if rec.oneway_array[nIndex] = '2' then
						raise WARNING '% regulation linkrow warning, gid = %', rec.table_name, rec.gid;
						end if;
					else
					raise EXCEPTION '% regulation linkrow warning, gid = %', rec.table_name, rec.gid;
					end if;
				end if;
			end if;
			
			nIndex		:= nIndex + 1;
		end loop;
	end loop;
	
	return 1;
END;
$$;

CREATE OR REPLACE FUNCTION mid_check_force_guide_links(link bigint, search_node bigint, passlink_cnt int, out_link_id bigint, dir smallint)
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
				,(case  when s_node = tmpLastNodeArray[tmpPathIndex] then e_node 
					when e_node = tmpLastNodeArray[tmpPathIndex] then s_node
				end) as nextnode
			from  link_tbl a 
			where 	(		
					(tmpLastNodeArray[tmpPathIndex] = s_node) 
					or	
					(tmpLastNodeArray[tmpPathIndex] = e_node)   
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

CREATE OR REPLACE FUNCTION test_link_shape_turn_number(shape_point geometry)
RETURNS integer
LANGUAGE plpgsql
AS $$
DECLARE
    PI                double precision;
    turn_number        integer;
    point_count        integer;
    point_index        integer;
    angle_array        double precision[];
    angle_diff        double precision;
   
    org_lon            double precision;
    org_lat            double precision;
    dst_lon            double precision;
    dst_lat            double precision;
    delta_lat        double precision;
    delta_lon        double precision;
BEGIN
    --
    PI          := 3.1415926535897932;
   
    --
    point_count := ST_NPoints(shape_point);
    if (point_count <= 2) then
        RETURN 0;
    end if;
   
    --
    for point_index in 2..point_count loop
        org_lon := ST_X(ST_PointN(shape_point, point_index-1));
        org_lat := ST_Y(ST_PointN(shape_point, point_index-1));
       
        dst_lon := ST_X(ST_PointN(shape_point, point_index));
        dst_lat := ST_Y(ST_PointN(shape_point, point_index));
       
        delta_lon := dst_lon - org_lon;
        delta_lat := dst_lat - org_lat;   
       
        if delta_lon = 0 and delta_lat = 0 then
            angle_array[point_index]  := 0.0;
        else
            angle_array[point_index] := (atan2(delta_lat, delta_lon) * 180.0 / PI);
        end if;
    end loop;
   
    --
    turn_number    := 0;
    for point_index in 3..point_count loop
        angle_diff    := abs(angle_array[point_index] - angle_array[point_index-1]);
        if angle_diff >= 175 and angle_diff <= 185 then
            turn_number    := turn_number + 1;
            raise INFO 'point = %', st_astext(ST_PointN(shape_point, point_index-1));
        end if;
    end loop;
   
    return turn_number;
END;
$$;

CREATE OR REPLACE FUNCTION ni_cnv_link_type( kind varchar)
  RETURNS smallint
  LANGUAGE plpgsql VOLATILE
  AS $$
BEGIN

	return case
		when kind like '%00' or kind like '%00|%' then 0
		when kind like '%03' or kind like '%03|%' then 3 
		when kind like '%05' or kind like '%05|%'
		    or kind like '%0b' or kind like '%0b|%' then 5 		
		when kind like '%04' or kind like '%04|%' then 4 
		when kind like '%12' or kind like '%12|%' then 8 
		when kind like '%15' or kind like '%15|%' then 9 
		when kind like '%06' or kind like '%06|%'
			or kind like '%07' or kind like '%07|%' then 7 
		when kind like '%0a' or kind like '%0a|%' then 6 
		when kind like '%02' or kind like '%02|%' then 2  
		else 1
	end;
END;
$$;

CREATE OR REPLACE FUNCTION ni_cnv_road_type( kind varchar, through varchar, unthrucrid varchar, 
	vehcl_type varchar, ownership varchar )
  RETURNS smallint
  LANGUAGE plpgsql VOLATILE
  AS $$
BEGIN
	return case
		when kind like '0a%' or kind like '%|0a%' then 10
		when ownership = '1' then 7
		when kind like '0b%' or kind like '%|0b%' then 8		
		when substr(vehcl_type, 1, 1) = '0' 
			and substr(vehcl_type, 2, 1) = '0'
			and substr(vehcl_type, 3, 1) = '0'
			and substr(vehcl_type, 14, 1) = '0'
			and (
				substr(vehcl_type, 8, 1) = '1'
				or substr(vehcl_type, 9, 1) = '1'
				or substr(vehcl_type, 10, 1) = '1'
			) then 12
		when (through = '0' and unthrucrid = '')
			or (kind like '08%' or kind like '%|08%') then 14
		when substr(vehcl_type, 14, 1) = '1' then 13 
		when kind like '00%' or kind like '%|00%' then 0 		
		when kind like '01%' or kind like '%|01%' then 1 
		when kind like '02%' or kind like '%|02%' then 2 
		when kind like '03%' or kind like '%|03%' then 3 
		when kind like '04%' or kind like '%|04%' then 4 
		when kind like '06%' or kind like '%|06%' then 6 
		when kind like '09%' or kind like '%|09%' then 9 
		else 6
	end;
END;
$$;

CREATE OR REPLACE FUNCTION org_CheckRamp()
	RETURNS smallint
	LANGUAGE plpgsql volatile
AS $$
DECLARE
	rec record;
BEGIN
	for rec in
		select id, snodeid, enodeid, direction, ni_cnv_road_type(kind,through,unthrucrid,vehcl_type,ownership) as road_type, funcclass
		from org_r
		where ni_cnv_road_type(kind,through,unthrucrid,vehcl_type,ownership) in (0,1) and ni_cnv_link_type(kind) in (1,2,7)
	loop
		if rec.direction in ('1','2') then
			perform org_check_ramp_atnode(rec.enodeid, rec.road_type, rec.funcclass, 1);
			perform org_check_ramp_atnode(rec.snodeid, rec.road_type, rec.funcclass, 2);
		end if;

		if rec.direction in ('1','3') then
			perform org_check_ramp_atnode(rec.snodeid, rec.road_type, rec.funcclass, 1);
			perform org_check_ramp_atnode(rec.enodeid, rec.road_type, rec.funcclass, 2);
		end if;
	end loop;
	RETURN 1;
END;
$$;

CREATE OR REPLACE FUNCTION org_check_ramp_atnode(nHwyNode varchar, nRoadTypeA integer, nFunctionClassA varchar, dir integer)
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
	tmpLastNodeArray	varchar[];
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
			select	(case when snodeid = tmpLastNodeArray[tmpPathIndex] then enodeid else snodeid end) as nextnode, 
					id as nextlink, 
					ni_cnv_link_type(kind) as link_type, ni_cnv_road_type(kind,through , unthrucrid , 
	vehcl_type , ownership ) as road_type, funcclass as function_code
			from org_r
			where	(
						(dir = 1)
						and
						(
							(snodeid = tmpLastNodeArray[tmpPathIndex] and direction in ('1','2'))
							or
							(enodeid = tmpLastNodeArray[tmpPathIndex] and direction in ('1','3'))
						)
						and ni_cnv_road_type(kind,through,unthrucrid,vehcl_type,ownership) in (0, 1, 2, 3, 4, 5, 6)
					)
					or
					(
						(dir = 2)
						and
						(
							(snodeid = tmpLastNodeArray[tmpPathIndex] and direction in ('1','3'))
							or
							(enodeid = tmpLastNodeArray[tmpPathIndex] and direction in ('1','2'))
						)
						and ni_cnv_road_type(kind,through,unthrucrid,vehcl_type,ownership) in (0, 1, 2, 3, 4, 5, 6)
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
				tmpLastNodeArray	:= array_append(tmpLastNodeArray, cast(rec.nextnode as varchar));
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


CREATE OR REPLACE FUNCTION ni_check_lane_linkrow()
	RETURNS smallint
	LANGUAGE plpgsql volatile
AS $$
DECLARE
	rec record;
	node_bef varchar[];
	node_aft varchar[];
	gid_now  integer;
BEGIN
	for gid_now in
		select gid from org_ln 
	loop
		for rec in 
			select idx,case when b.id is null then array[snodeid] else array[b.id,b.adjoin_nid] end as snode_arr,
			case when c.id is null then array[enodeid] else array[c.id,c.adjoin_nid] end as enode_arr,direction from
			(
				select a.*,b.snodeid,b.enodeid,b.direction from
				(
					select link_arr[idx] as link_id,idx from
					(
						select link_arr,generate_series(1,array_upper(link_arr,1)) as idx
						from
						(
							select array[inlinkid::text]||string_to_array(passlid,'|')||string_to_array(passlid2,'|')||array[outlinkid::text] as link_arr
							from org_ln 
							where gid=gid_now
						) a
					) a
				) a
				join org_r b
				on a.link_id=b.id 
			) a
			left join org_n b
			on a.snodeid=b.id and b.kind like '%1f00%'
			left join org_n c
			on a.enodeid=c.id and c.kind like '%1f00%'
			order by idx
		loop
			raise info 'gid=%',gid_now;
			if rec.idx<>1 then
				if rec.direction in ('0','1') then
					node_aft=rec.snode_arr||rec.enode_arr;
				elsif rec.direction='2' then
					node_aft=rec.snode_arr;
				elsif rec.direction='3' then
					node_aft=rec.enode_arr;
				end if;
				
				if not (node_bef && node_aft) then
					raise exception '%,%',gid_now,rec.idx;
				end if;
			end if;
				
			if rec.direction in ('0','1') then
				node_bef=rec.snode_arr||rec.enode_arr;
			elsif rec.direction='2' then
				node_bef=rec.enode_arr;
			elsif rec.direction='3' then
				node_bef=rec.snode_arr;
			end if;
		end loop;
				
	end loop;
	RETURN 1;
END;
$$;
