-------------------------------------------------------------------------------------------------------------
-- check logic
-------------------------------------------------------------------------------------------------------------


CREATE OR REPLACE FUNCTION test_check_linkrow_continuable(	link_array integer[], seq_array integer[], oneway_array integer[],
															snode_array integer[], enode_array integer[]
															)
	RETURNS boolean
	LANGUAGE plpgsql
	AS $$ 
DECLARE
	nIndex			integer;
	nCount			integer;
BEGIN	
	-- check linkrow continuable
	nCount		:= array_upper(seq_array, 1);
	for nIndex in 1..nCount loop
		if nIndex < nCount then
			if snode_array[nIndex] in (snode_array[nIndex+1], enode_array[nIndex+1]) then
				if oneway_array[nIndex] in (2,4) then
					--raise WARNING 'regulation linkrow error: traffic flow error, linkrow = %', link_array;
					--return false;
				end if;
			elseif enode_array[nIndex] in (snode_array[nIndex+1], enode_array[nIndex+1]) then
				if oneway_array[nIndex] in (3,4) then
					--raise WARNING 'regulation linkrow error: traffic flow error, linkrow = %', link_array;
					--return false;
				end if;
			else
				--raise EXCEPTION 'regulation linkrow error: links have no intersection, linkrow = %', link_array;
				return false;
			end if;
		else
			if snode_array[nIndex] in (snode_array[nIndex-1], enode_array[nIndex-1]) then
				if oneway_array[nIndex] in (3,4) then
					--raise WARNING 'regulation linkrow error: traffic flow error, linkrow = %', link_array;
					--return false;
				end if;
			elseif enode_array[nIndex] in (snode_array[nIndex-1], enode_array[nIndex-1]) then
				if oneway_array[nIndex] in (2,4) then
					--raise WARNING 'regulation linkrow error: traffic flow error, linkrow = %', link_array;
					--return false;
				end if;
			else
				--raise EXCEPTION 'regulation linkrow error: links have no intersection, linkrow = %', link_array;
				return false;
			end if;
		end if;
	end loop;
	
	return true;
END;
$$;

CREATE OR REPLACE FUNCTION test_get_connect_junction(link_a integer, link_b integer)
  RETURNS integer 
LANGUAGE plpgsql volatile
AS $$
DECLARE
	junction_id	integer;
BEGIN
	select  case 
			when a.from_node_id in (b.from_node_id, b.to_node_id) then a.from_node_id 
			when a.to_node_id in (b.from_node_id, b.to_node_id) then a.to_node_id 
			else null
			end
	from
	(
		select from_node_id, to_node_id
		from road_link 
		where objectid = link_a
	)as a,
	(
		select from_node_id, to_node_id
		from road_link 
		where objectid = link_b
	)as b
	into junction_id;
	
	return junction_id;
END;
$$;
