-------------------------------------------------------------------------------------------------------------
-- check logic
-------------------------------------------------------------------------------------------------------------


CREATE OR REPLACE FUNCTION test_nostra_check_regulation_linkrow()
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
			select	routeid, 
					array_agg(link_id) as link_array, 
					array_agg(seq_num) as seq_array,
					array_agg(one_way) as oneway_array,
					array_agg(start_node_id) as snode_array,
					array_agg(end_node_id) as enode_array
			from
			(
				select m.routeid, m.link_id, seq_num, 1 as one_way, n.start_node_id, n.end_node_id
				from
				(
					select routeid, link_array[seq_num] as link_id, seq_num
					from
					(
						select routeid, link_array, generate_series(1,array_upper(link_array,1)) as seq_num
						from
						(
							select routeid, array_agg(arc) as link_array
							from
							(
								select *
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
			                    where arc is not null and arc > 0
			                    order by routeid, arc_index
		                    )as t2
		                    group by routeid
						)as a
						where (array_upper(link_array,1) >= 2)
					)as b
				)as m
				left join temp_topo_link as n
				on m.link_id = n.routeid
				order by m.routeid, seq_num
			)as a
			group by routeid
		)as r
	loop
		-- check linkrow continuable
		nCount		:= array_upper(rec.seq_array, 1);
		nIndex		:= 1;
		while nIndex < nCount loop
			if rec.snode_array[nIndex] in (rec.snode_array[nIndex+1], rec.enode_array[nIndex+1]) then
				if rec.oneway_array[nIndex] in (2,4) then
					raise WARNING 'regulation linkrow error: traffic flow error, routeid = %', rec.routeid;
				end if;
			elseif rec.enode_array[nIndex] in (rec.snode_array[nIndex+1], rec.enode_array[nIndex+1]) then
				if rec.oneway_array[nIndex] in (3,4) then
					raise WARNING 'regulation linkrow error: traffic flow error, routeid = %', rec.routeid;
				end if;
			else
				raise EXCEPTION 'regulation linkrow error: links have no intersection, routeid = %', rec.routeid;
			end if;
			
			nIndex		:= nIndex + 1;
		end loop;
	end loop;
	
	return 1;
END;
$$;
CREATE OR REPLACE FUNCTION check_org_junctionview_in_outlink()
  RETURNS integer 
LANGUAGE plpgsql volatile
AS $$
DECLARE
	rec			    record;
	in_s_node_id                bigint;
	in_e_node_id                bigint;
	out_s_node_id               bigint;
	out_e_node_id               bigint;
	
BEGIN	
	for rec in 
		SELECT gid, arc1, arc2, day_pic, night_pic, arrowimg, lon, lat
		FROM org_junctionview
	LOOP

		SELECT start_node_id, end_node_id
		into in_s_node_id, in_e_node_id
		FROM temp_topo_link
		where routeid = rec.arc1;
		
		SELECT start_node_id, end_node_id
		into out_s_node_id, out_e_node_id
		FROM temp_topo_link
		where routeid = rec.arc2;
		if (in_e_node_id = out_s_node_id and in_s_node_id = out_e_node_id) or (in_e_node_id = out_e_node_id and in_s_node_id = out_s_node_id ) then
			---raise INFO 'org_junctionview  gid1 = %', rec.gid;
			return 0;
		end if;
		if in_e_node_id <> out_s_node_id and in_e_node_id <> out_e_node_id and in_s_node_id <> out_s_node_id and in_s_node_id <> out_e_node_id then
			---raise INFO 'org_junctionview  gid2 = %', rec.gid;
			return 0;
		else
			
		end if;

	
	end loop;
	return 1;
END;
$$;