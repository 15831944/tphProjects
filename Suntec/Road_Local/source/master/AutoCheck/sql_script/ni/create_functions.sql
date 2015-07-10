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
						m.link_id, seq_num, n.direction as one_way, n.snodeid, n.enodeid
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
								on a.mapid = b.mapid and a.condid = b.condid
								where a.condtype = '1'
								order by table_name, a.gid, b.seq_nm::integer
							)as t
							group by table_name, gid
						)as a
					)as b
				)as m
				left join org_r as n
				on m.link_id = n.id
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