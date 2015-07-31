-------------------------------------------------------------------------------------------------------------
-- check logic
-------------------------------------------------------------------------------------------------------------


CREATE OR REPLACE FUNCTION test_mmi_check_linkrow_continuable(linkrow float[])
	RETURNS boolean
	LANGUAGE plpgsql
	AS $$ 
DECLARE
	rec				record;
	nIndex			integer;
	nCount			integer;
BEGIN	
	select	array_agg(link_id) as link_array, 
			array_agg(seq_num) as seq_array,
			array_agg(one_way) as oneway_array,
			array_agg(start_node_id) as snode_array,
			array_agg(end_node_id) as enode_array
	from
	(
		select 	m.link_id, seq_num, 
				(case when n.oneway = 'FT' then 2 else 1 end) as one_way, 
				n.f_jnctid as start_node_id, 
				n.t_jnctid as end_node_id
		from
		(
			select link_array[seq_num] as link_id, seq_num
			from
			(
				select linkrow as link_array, generate_series(1,array_upper(linkrow,1)) as seq_num
			)as s
		)as m
		left join org_city_nw_gc_polyline as n
		on m.link_id = n.id
		order by seq_num
	)as t
	into rec;
	
	-- check linkrow continuable
	nCount		:= array_upper(rec.seq_array, 1);
	for nIndex in 1..nCount loop
		if nIndex < nCount then
			if rec.snode_array[nIndex] in (rec.snode_array[nIndex+1], rec.enode_array[nIndex+1]) then
				if rec.oneway_array[nIndex] in (2,4) then
					--raise WARNING 'regulation linkrow error: traffic flow error, linkrow = %', linkrow;
					--return false;
				end if;
			elseif rec.enode_array[nIndex] in (rec.snode_array[nIndex+1], rec.enode_array[nIndex+1]) then
				if rec.oneway_array[nIndex] in (3,4) then
					--raise WARNING 'regulation linkrow error: traffic flow error, linkrow = %', linkrow;
					--return false;
				end if;
			else
				--raise EXCEPTION 'regulation linkrow error: links have no intersection, linkrow = %', linkrow;
				return false;
			end if;
		else
			if rec.snode_array[nIndex] in (rec.snode_array[nIndex-1], rec.enode_array[nIndex-1]) then
				if rec.oneway_array[nIndex] in (3,4) then
					--raise WARNING 'regulation linkrow error: traffic flow error, linkrow = %', linkrow;
					--return false;
				end if;
			elseif rec.enode_array[nIndex] in (rec.snode_array[nIndex-1], rec.enode_array[nIndex-1]) then
				if rec.oneway_array[nIndex] in (2,4) then
					--raise WARNING 'regulation linkrow error: traffic flow error, linkrow = %', linkrow;
					--return false;
				end if;
			else
				--raise EXCEPTION 'regulation linkrow error: links have no intersection, linkrow = %', linkrow;
				return false;
			end if;
		end if;
	end loop;
	
	return true;
END;
$$;

CREATE OR REPLACE FUNCTION test_get_connect_junction(link_a float, link_b float)
  RETURNS float 
LANGUAGE plpgsql volatile
AS $$
DECLARE
	junction_id	float;
BEGIN
	select  case 
			when a.f_jnctid in (b.f_jnctid, b.t_jnctid) then a.f_jnctid 
			when a.t_jnctid in (b.f_jnctid, b.t_jnctid) then a.t_jnctid 
			else null
			end
	from
	(
		select f_jnctid, t_jnctid
		from org_city_nw_gc_polyline 
		where id = link_a
	)as a,
	(
		select f_jnctid, t_jnctid
		from org_city_nw_gc_polyline 
		where id = link_b
	)as b
	into junction_id;
	
	return junction_id;
END;
$$;
