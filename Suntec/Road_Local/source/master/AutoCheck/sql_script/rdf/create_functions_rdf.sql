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