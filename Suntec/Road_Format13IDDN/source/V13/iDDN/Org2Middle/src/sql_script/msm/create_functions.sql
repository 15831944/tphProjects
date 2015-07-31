---------------------------------------------------------------------------------------------------------
-- scripts for create functions in this file.
-- Project: Malsing
---------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_cnv_roadname_simplify(name character varying) 
RETURNS character varying
    LANGUAGE plpgsql
    AS $$
    DECLARE
	new_name character varying;
	len integer;
	i integer;
	flag boolean;
    BEGIN

	IF name IS NOT NULL THEN
	
		len = char_length(name);
		new_name := '';
		flag := true;
		
		for i in 1..len loop
			if substring(name,i,1) = '(' then
				flag := false;
			elsif substring(name,i - 1,1) = ')' then
				flag := true;
			end if;

			if flag = true then
				new_name = new_name || substring(name,i,1);
			end if;
		end loop;
		
		return new_name;
	ELSE
		return name;
	END IF;	
	
    END;
$$;

CREATE OR REPLACE FUNCTION mid_is_uturn(roadname character varying)
    RETURNS BOOLEAN
    LANGUAGE plpgsql volatile
AS $$
DECLARE
    weekflag         integer;
BEGIN
    RETURN (' ' || roadname || ' ') ~* '[^[:alnum:]_]+u[^[:alnum:]_]*turns?[^[:alnum:]_]+';
END;
$$;

CREATE OR REPLACE FUNCTION rdb_cnv_display_class(type integer, new_type_roadtype integer, new_type_dsp integer, level integer, new_level integer, acc_mask character varying, exp_id bigint)
	RETURNS smallint 
	LANGUAGE plpgsql
AS $$
    BEGIN
		IF exp_id IS NOT NULL THEN RETURN 12;
		ELSE
			RETURN CASE
				WHEN type IN (26,27) THEN 14  
				WHEN type IN (10,22) OR (type = 12 and new_type_roadtype IN (10,22)) OR 
					(type IN (8,9,11) and new_type_dsp IN (10,22)) OR
					(
						substring(acc_mask,1,1) = '1' 
						and substring(acc_mask,2,1) = '1'
						and substring(acc_mask,3,1) = '1' 
						and substring(acc_mask,7,1) = '1' 
						and substring(acc_mask,9,1) = '1' 
						and substring(acc_mask,10,1) = '1'
						and substring(acc_mask,5,1) = '0'
					) THEN 3   
				WHEN type IN (1,2,3) OR (type = 12 and new_type_roadtype IN (1,2,3)) OR 
					(type IN (8,9,11) and new_type_dsp IN (1,2,3)) THEN 11 
				WHEN type IN (4,5) OR (type = 12 and new_type_roadtype IN (4,5)) OR 
					(type IN (8,9,11) and new_type_dsp IN (4,5)) THEN 7
				WHEN type IN (6) OR (type = 12 and new_type_roadtype IN (6)) OR 
					(type IN (8,9,11) and new_type_dsp IN (6)) THEN 6  
				WHEN type IN (7) OR (type = 12 and new_type_roadtype IN (7)) OR 
					(type IN (8,9,11) and new_type_dsp IN (7)) THEN 5  
				ELSE 4
			END;
		END IF;
    END;
$$;

CREATE OR REPLACE FUNCTION rdb_cnv_display_class_for_roundabout(type integer, exp_id bigint)
	RETURNS smallint 
	LANGUAGE plpgsql
AS $$
    BEGIN
		IF exp_id IS NOT NULL THEN RETURN 12;
		ELSE
			RETURN CASE   
				WHEN type IN (1,2,3) THEN 11 
				WHEN type IN (4,5) THEN 7
				WHEN type IN (6) THEN 6  
				WHEN type IN (7) THEN 5  
				ELSE 4
			END;
		END IF;
    END;
$$;

CREATE OR REPLACE FUNCTION rdb_cnv_link_type(type integer, one_way integer, inner_id bigint) 
	RETURNS smallint
    LANGUAGE plpgsql
AS $$
    BEGIN
	    RETURN CASE
			WHEN type = 12 THEN 0
	        WHEN type in (8,9,11) THEN 5
	        WHEN inner_id IS NOT NULL THEN 4
	    	WHEN type in (1,2,3,4,5,11) and one_way = 1 THEN 2
	        ELSE 1
	    END;
    END;
$$;


CREATE OR REPLACE FUNCTION rdb_cnv_road_type(type integer, new_type integer, acc_mask character varying, exp_id bigint) 
RETURNS smallint
    LANGUAGE plpgsql
    AS $$
    BEGIN
		IF exp_id IS NOT NULL THEN RETURN 0;
		ELSE	
			RETURN CASE
				WHEN type IN (26,27) THEN 10
				WHEN (
						(
							substring(acc_mask,1,1) = '1'  
							and substring(acc_mask,6,1) = '1' 
							and substring(acc_mask,7,1) = '1' 
							and substring(acc_mask,9,1) = '1'
						) 
							and (substring(acc_mask,2,1) = '0' 
							OR substring(acc_mask,3,1) = '0' 
							OR substring(acc_mask,10,1) = '0'
						)
					) THEN 12
				WHEN substring(acc_mask,8,1) = '1' THEN 14
				WHEN type IN (10,22) OR new_type IN (10,22) OR 
					(
						substring(acc_mask,1,1) = '1' 
						and substring(acc_mask,2,1) = '1'
						and substring(acc_mask,3,1) = '1' 
						and substring(acc_mask,7,1) = '1' 
						and substring(acc_mask,9,1) = '1' 
						and substring(acc_mask,10,1) = '1'
						and substring(acc_mask,5,1) = '0'
					) THEN 8
				WHEN type IN (1,2,3) OR new_type IN (1,2,3) THEN 2
				WHEN type IN (4,5) OR new_type IN (4,5) THEN 3
				WHEN type IN (6) OR new_type IN (6) THEN 4
				ELSE 6
			END;
		END IF;
    END;
$$;

CREATE OR REPLACE FUNCTION rdb_cnv_speed_class(spd_limit integer, route_spd integer) RETURNS smallint
    LANGUAGE plpgsql
    AS $$
    BEGIN
		IF route_spd IS NOT NULL AND route_spd <> 0 THEN
			RETURN CASE
				WHEN route_spd > 130 THEN 1
				WHEN route_spd > 100 and route_spd <= 130 THEN 2
				WHEN route_spd > 90 and route_spd <= 100 THEN 3
				WHEN route_spd > 70 and route_spd <= 90 THEN 4
				WHEN route_spd > 50 and route_spd <= 70 THEN 5
				WHEN route_spd > 30 and route_spd <= 50 THEN 6
				WHEN route_spd > 11 and route_spd <= 30 THEN 7
				WHEN route_spd < 11 THEN 8
				ELSE 8
			END;
		ELSIF spd_limit IS NOT NULL AND spd_limit <> 0 THEN
			RETURN CASE
				WHEN spd_limit > 130 THEN 1
				WHEN spd_limit > 100 and spd_limit <= 130 THEN 2
				WHEN spd_limit > 90 and spd_limit <= 100 THEN 3
				WHEN spd_limit > 70 and spd_limit <= 90 THEN 4
				WHEN spd_limit > 50 and spd_limit <= 70 THEN 5
				WHEN spd_limit > 30 and spd_limit <= 50 THEN 6
				WHEN spd_limit > 11 and spd_limit <= 30 THEN 7
				WHEN spd_limit < 11 THEN 8
				ELSE 8
			END;
		ELSE
			RETURN 8;
		END IF;
    END;
$$;

CREATE OR REPLACE FUNCTION rdb_cnv_function_code(level integer, new_level integer) 
RETURNS smallint
    LANGUAGE plpgsql
    AS $$
    BEGIN
		IF new_level IS NULL THEN
			RETURN CASE
				WHEN level = 5 THEN 1
				WHEN level = 4 THEN 2
				WHEN level = 3 THEN 3
				WHEN level = 2 THEN 4
				WHEN level = 1 THEN 5
				ELSE 5
			END;
		ELSE
			RETURN CASE
				WHEN new_level = 5 THEN 1
				WHEN new_level = 4 THEN 2
				WHEN new_level = 3 THEN 3
				ELSE 3
			END;		
		END IF;
    END;
$$;

CREATE OR REPLACE FUNCTION rdb_cnv_lane_dir(one_way integer) 
RETURNS smallint
    LANGUAGE plpgsql
    AS $$
    BEGIN
	    RETURN CASE
	        WHEN one_way = 1 THEN 1
	        WHEN one_way = 0 THEN 3
	        ELSE 0
	    END;
    END;
$$; 

CREATE OR REPLACE FUNCTION rdb_cnv_lanecount_downflow(one_way integer, acc_mask character varying, lane_f integer, lane_t integer) 
RETURNS smallint
    LANGUAGE plpgsql
    AS $$
    BEGIN
	IF lane_f IS NOT NULL AND lane_t IS NOT NULL THEN
	    RETURN CASE
	        WHEN one_way = 1 OR one_way = 0 OR substring(acc_mask,1,1) = '1' THEN (lane_f + lane_t)
	        ELSE 0
	    END;
	ELSE
	    RETURN 0;
	END IF;
    END;
$$;

CREATE OR REPLACE FUNCTION rdb_cnv_lanecount_upflow(one_way integer, acc_mask character varying, lane_f integer, lane_t integer) 
RETURNS smallint
    LANGUAGE plpgsql
    AS $$
    BEGIN
	IF lane_f IS NOT NULL AND lane_t IS NOT NULL THEN
	    RETURN CASE
	        WHEN one_way = 0 OR substring(acc_mask,1,1) = '1' THEN (lane_f + lane_t)
	        ELSE 0
	    END;
	ELSE
	    RETURN 0;
	END IF;
    END;
$$;

CREATE OR REPLACE FUNCTION rdb_cnv_speedlimit_downflow(one_way integer, acc_mask character varying, spd_limit integer, route_spd integer)
  RETURNS double precision
  LANGUAGE plpgsql VOLATILE
  AS $$
DECLARE
	rtnvalue INT;
BEGIN
	IF substring(acc_mask,1,1) = '1' THEN 
		RETURN 0;
	ELSE
		IF spd_limit IS NOT NULL and spd_limit > 0 THEN
			IF spd_limit > 200 THEN
				RETURN -1;
			ELSE
				RETURN spd_limit;
			END IF;
		END IF;
		
		RETURN 
		CASE WHEN route_spd IS NOT NULL AND route_spd > 0 THEN route_spd
			ELSE 0 
		END;
	END IF;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_cnv_speedlimit_upflow(one_way integer, acc_mask character varying, spd_limit integer, route_spd integer)
  RETURNS double precision
  LANGUAGE plpgsql VOLATILE
  AS $$
DECLARE
	rtnvalue INT;
BEGIN
	IF one_way = 1 OR substring(acc_mask,1,1) = '1' THEN
		RETURN 0;
	ELSE
		IF spd_limit IS NOT NULL AND spd_limit > 0 THEN
			IF spd_limit > 200 THEN
				RETURN -1;
			ELSE
				RETURN spd_limit;
			END IF;
		END IF;
	END IF;
	
    RETURN 
	CASE WHEN route_spd IS NOT NULL AND route_spd > 0 THEN route_spd
        ELSE 0 
    END;
END;
$$;


CREATE OR REPLACE FUNCTION rdb_cnv_speedsource_downflow(one_way integer, acc_mask character varying, spd_limit integer, is_lgl_speed character varying, route_spd integer, spd_format integer)
  RETURNS SMALLINT
  LANGUAGE plpgsql VOLATILE
  AS $$
DECLARE
	rtnvalue INT;
BEGIN
	IF substring(acc_mask,1,1) = '1' THEN
		RETURN 0;
	ELSE
		IF spd_limit IS NOT NULL AND spd_limit <> 0 THEN
			IF is_lgl_speed  = 'Y' THEN
				RETURN CASE WHEN spd_format = 0 THEN (1 << 3) | 1
					ELSE 1
				END;
			ELSE
				RETURN CASE WHEN spd_format = 0 THEN (1 << 3) | 2
					ELSE 2
				END;	
			END IF;
		ELSIF route_spd IS NOT NULL AND route_spd > 0 THEN
			RETURN CASE WHEN spd_format = 0 THEN (1 << 3) | 3
				ELSE 3
			END;		
		ELSE
			RETURN 0;
		END IF;
	END IF;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_cnv_speedsource_upflow(one_way integer, acc_mask character varying, spd_limit integer, is_lgl_speed character varying, route_spd integer, spd_format integer)
  RETURNS SMALLINT
  LANGUAGE plpgsql VOLATILE
  AS $$
DECLARE
	rtnvalue INT;
BEGIN
	IF one_way = 1 OR substring(acc_mask,1,1) = '1'THEN
		RETURN 0;
	ELSE
		IF spd_limit IS NOT NULL AND spd_limit <> 0 THEN
			IF is_lgl_speed  = 'Y' THEN
				RETURN CASE WHEN spd_format = 0 THEN (1 << 3) | 1
					ELSE 1
				END;
			ELSE
				RETURN CASE WHEN spd_format = 0 THEN (1 << 3) | 2
					ELSE 2
				END;	
			END IF;
		ELSIF route_spd IS NOT NULL AND route_spd > 0 THEN
			RETURN CASE WHEN spd_format = 0 THEN (1 << 3) | 3
				ELSE 3
			END;		
		ELSE
			RETURN 0;
		END IF;
    END IF;
END;
$$;

CREATE OR REPLACE FUNCTION mid_find_special_links(s_type integer[], objTable character varying)
  RETURNS integer 
  LANGUAGE plpgsql VOLATILE
AS $$ 
DECLARE
	rec					temp_topo_link_with_attr;
	rec2				recORd;
	tmp_rec             recORd;
	pos_rec				recORd;
	neg_rec				recORd;
	start_node			bigint;
	end_node			bigint;
	link_array			bigint[];
	nCount_pos			integer;
	nCount_neg			integer;
	nCount				integer;
	nIndex				integer;
	group_id			integer;
	seq					integer;				
BEGIN

	group_id := 0;
	seq	 := 0;
	
	fOR rec in select * from temp_topo_link_with_attr where type = any(s_type) ORder by link_id
	LOOP

		pos_rec			:= mid_find_special_links_in_one_direction(rec, rec.s_node, s_type);
		end_node		:= pos_rec.reach_node;

		neg_rec			:= mid_find_special_links_in_one_direction(rec, rec.e_node, s_type);
		start_node		:= neg_rec.reach_node;

		nCount_pos		:= array_upper(pos_rec.linkrow, 1);		
		nCount_neg		:= array_upper(neg_rec.linkrow, 1);

		IF nCount_pos IS NOT NULL and nCount_neg IS NOT NULL THEN
		
			group_id 	:= group_id + 1;
			seq		:= 0;
			
			link_array		:= ARRAY[rec.link_id];
			
			fOR nIndex in 1..nCount_neg loop
				link_array	:= array_prepend(neg_rec.linkrow[nIndex], link_array);
			end loop;
			
			fOR nIndex in 1..nCount_pos loop
				link_array	:= array_append(link_array, pos_rec.linkrow[nIndex]);
			end loop;

			nCount			:= array_upper(link_array, 1);			
			nIndex := 1;
				
			while nIndex  <= nCount loop	
				
				seq := seq + 1;
				execute 'insert into ' || quote_ident(objTable) || '(link_id,group_id,seq) 
				values('||link_array[nIndex]||','||group_id||','||seq||')';
 				
				nIndex := nIndex + 1;	
			end loop;
		END IF;		
	END LOOP;

	RETURN 1;
	
END;
$$;

CREATE OR REPLACE FUNCTION mid_find_special_links_in_one_direction(	
		rec temp_topo_link_with_attr, search_node int, s_type int[], OUT linkrow bigint[], OUT reach_node bigint)
RETURNS recORd
LANGUAGE plpgsql volatile
AS $$
DECLARE
	rec2			recORd;
	reach_link              bigint;
	
	tmpLastNodeArray	bigint[];
	tmpPathArray		bigint[];
	tmpPathCount		integer;
	tmpPathIndex		integer;	

	nIndex			integer;
	nCount			integer;	
BEGIN
	reach_link	:= rec.link_id;
	reach_node	:= search_node;

	tmpLastNodeArray	:= ARRAY[reach_node];
	tmpPathArray		:= ARRAY[reach_link];
	tmpPathCount		:= 1;
	tmpPathIndex		:= 1;
	linkrow 			:= ARRAY[rec.link_id];
	
	WHILE tmpPathIndex <= tmpPathCount LOOP
		
		fOR rec2 in
			select	*
			from
			(
				(
					select	link_id, e_node as nextnode
					from temp_topo_link_with_attr
					where 	(tmpLastNodeArray[tmpPathIndex] = s_node) 
						and (link_id != reach_link)
						and type = any(s_type)
						and not (link_id = any(tmpPathArray))
						and one_way in (0,1)
				)
				union
				(
					select	link_id, s_node as nextnode
					from temp_topo_link_with_attr
					where 	(tmpLastNodeArray[tmpPathIndex] = e_node) 
						and (link_id != reach_link)
						and type = any(s_type)
						and not (link_id = any(tmpPathArray))
						and one_way in (0)
				)
			)as a ORder by link_id

		loop
		
			tmpPathCount		:= tmpPathCount + 1;
			
			linkrow		:= array_append(linkrow, rec2.link_id);
			reach_link	:= rec2.link_id;
			reach_node	:= rec2.nextnode;
			tmpLastNodeArray := array_append(tmpLastNodeArray, cast(rec2.nextnode as bigint));
			tmpPathArray	:= array_append(tmpPathArray,cast(rec2.link_id as bigint));
		end loop;

		tmpPathIndex := tmpPathIndex + 1;

	end loop;
END;
$$;
-------------------------------------------------------------------------------------------------------------
-- regulation
-------------------------------------------------------------------------------------------------------------

CREATE OR REPLACE FUNCTION mid_get_acc_mask(stracc_mask varchar)
	RETURNS smallint
	LANGUAGE plpgsql
AS $$
DECLARE
	car_type smallint;
BEGIN
	car_type := 0;
	
	if stracc_mask is null then
		return car_type;
	end if;
	
	if substring(stracc_mask,1,1) = '1' then--Automobiles
		car_type := car_type | 1 << 13;
	end if;
	
	if substring(stracc_mask,4,1) = '1' then --Carpools
		car_type := car_type | 1 << 10;
	end if;
	
	return car_type;
END;
$$;

CREATE OR REPLACE FUNCTION mid_get_link_array(link_lib varchar, strfolder varchar)
  RETURNS bigint[] 
  LANGUAGE plpgsql
AS $$
DECLARE
	cur refcursor;
	rec record;
	link_lib_tmp bigint[];
	link_lib_tmp_num integer;
	link_new bigint[];
BEGIN
	if link_lib is null then
		return null;
	end if;

	link_lib_tmp := (regexp_split_to_array(link_lib,E'\\\,+'))::bigint[];
	link_lib_tmp_num := array_upper(link_lib_tmp,1);

	for link_lib_tmp_num_idx in 1..link_lib_tmp_num loop
		open cur for select new_link_id
			from temp_topo_link
			where link_id = link_lib_tmp[link_lib_tmp_num_idx] and folder = strfolder;
		fetch cur into rec;
		if rec.new_link_id is null then
			close cur;
			return null;
		end if;
		link_new[link_lib_tmp_num_idx] := rec.new_link_id::bigint;
		close cur;
	end loop;

	return link_new;
END;
$$;

CREATE OR REPLACE FUNCTION mid_convert_regulation_from_one_link()
  RETURNS integer
  LANGUAGE plpgsql
AS $$
DECLARE
	cur refcursor;
	rec record;
	curr_reg_id bigint;
	icondtype integer;
BEGIN
	curr_reg_id := 0;
	-- get the max regulation_id
	OPEN cur FOR SELECT max(regulation_id) as reg_id FROM regulation_relation_tbl;
	FETCH cur INTO rec;
	if rec.reg_id is not null then
		curr_reg_id := rec.reg_id;
	end if;
	close cur;
	
	OPEN cur FOR SELECT gid, link_id, one_way, cond_id
	FROM temp_acc_mask;
	FETCH cur INTO rec;
	
	while rec.gid is not null loop
		if rec.one_way = 0 then
			icondtype := 4;
		else
			icondtype := 42;
		end if;
		
		insert into regulation_relation_tbl(regulation_id, nodeid, inlinkid, outlinkid, condtype, cond_id)
		values(rec.gid+curr_reg_id,null,rec.link_id,null,icondtype,rec.cond_id);
		
		insert into regulation_item_tbl(regulation_id, linkid, nodeid, seq_num)
		values(rec.gid+curr_reg_id, rec.link_id,null,1);
		
		FETCH cur INTO rec;
	end loop;
	
	close cur;
	
	return 1;
END;
$$;
CREATE OR REPLACE FUNCTION mid_convert_regulation_from_link_array()
  RETURNS integer
  LANGUAGE plpgsql
AS $$
DECLARE
	cur refcursor;
	rec record;
	curr_reg_id bigint;
	icondtype integer;
	
	iseq_num integer;
	link_array bigint[];
BEGIN
	curr_reg_id := 0;
	-- get the max regulation_id
	OPEN cur FOR SELECT max(regulation_id) as reg_id FROM regulation_relation_tbl;
	FETCH cur INTO rec;
	if rec.reg_id is not null then
		curr_reg_id := rec.reg_id;
	end if;
	close cur;
	
	OPEN cur FOR SELECT reg_id, in_link_id, node_id, out_link_id, pass_link, pass_link_count, s_e
	FROM temp_turn_link;
	FETCH cur INTO rec;
	
	while rec.reg_id is not null loop
		
		--if rec.in_link_id = rec.out_link_id then
			--FETCH cur INTO rec;
			--continue;
		--end if;
		
		insert into regulation_relation_tbl(regulation_id, nodeid, inlinkid, outlinkid, condtype)
		values(rec.reg_id+curr_reg_id,rec.node_id,rec.in_link_id,rec.out_link_id,1);
		
		iseq_num := 1;
		insert into regulation_item_tbl(regulation_id, linkid, nodeid, seq_num)
		values(rec.reg_id+curr_reg_id, rec.in_link_id,null,iseq_num);
		iseq_num := iseq_num + 1;
		insert into regulation_item_tbl(regulation_id, linkid, nodeid, seq_num)
		values(rec.reg_id+curr_reg_id,null, rec.node_id,iseq_num);
		
		--passlink
		if rec.pass_link_count > 0 then
			link_array := regexp_split_to_array(rec.pass_link,E'\\|+')::bigint[];
			for idx in 1..rec.pass_link_count loop
				iseq_num := iseq_num + 1;
				insert into regulation_item_tbl(regulation_id, linkid, nodeid, seq_num)
				values(rec.reg_id+curr_reg_id,link_array[idx],null,iseq_num);
			end loop;
		end if;
		
		iseq_num := iseq_num + 1;
		insert into regulation_item_tbl(regulation_id, linkid, nodeid, seq_num)
		values(rec.reg_id+curr_reg_id,rec.out_link_id,null,iseq_num);
		
		FETCH cur INTO rec;
	end loop;
	
	close cur;
	
	return 1;
END;
$$;

CREATE OR REPLACE FUNCTION mid_get_country_regulation()
  RETURNS integer 
  LANGUAGE plpgsql AS
$$ 
DECLARE
	cur1 refcursor;
	rec1 record;
	cur2 refcursor;
	rec2 record;
	cur3 refcursor;
	rec3 record;
	
	start_link_array integer[];
	start_link_array_num integer;
	end_link_array integer[];
	end_link_array_num integer;

	icountry_num integer;
	curr_reg_id integer;
BEGIN
	curr_reg_id := 0;
	-- get the max regulation_id
	OPEN cur1 FOR SELECT max(regulation_id) as reg_id FROM regulation_relation_tbl;
	FETCH cur1 INTO rec1;
	if rec1.reg_id is not null then
		curr_reg_id := rec1.reg_id;
	end if;
	close cur1;
	
	open cur1 for select node_id,country_array from temp_node_country;
	FETCH cur1 INTO rec1;
	
	while rec1.node_id is not null loop
		icountry_num := array_upper(rec1.country_array,1);
		for icountry_num_idx in 1..icountry_num loop
			--start_link
			open cur2 for select array_agg(link_id) as link_array
										from temp_link_country
										where ((one_way = 0 and (s_node = rec1.node_id or e_node = rec1.node_id)) or
													(one_way = 1 and e_node = rec1.node_id)) and country = rec1.country_array[icountry_num_idx]
										group by country;
			FETCH cur2 INTO rec2;
			--end_link
			open cur3 for select array_agg(link_id) as link_array
										from temp_link_country
										where ((one_way = 0 and (s_node = rec1.node_id or e_node = rec1.node_id)) or
													(one_way = 1 and s_node = rec1.node_id)) and country <> rec1.country_array[icountry_num_idx]
										group by country;
			FETCH cur3 INTO rec3;
			if rec2.link_array is null or rec3.link_array is null then
				--
			else
				start_link_array := rec2.link_array;
				start_link_array_num := array_upper(start_link_array,1);
				for start_link_array_num_idx in 1..start_link_array_num loop
					while rec3.link_array is not null loop
						end_link_array := rec3.link_array;
						end_link_array_num := array_upper(end_link_array,1);
						for end_link_array_num_idx in 1..end_link_array_num loop
							
							curr_reg_id := curr_reg_id + 1;
							insert into regulation_relation_tbl(regulation_id, nodeid, inlinkid, outlinkid, condtype)
							values(curr_reg_id,rec1.node_id,start_link_array[start_link_array_num_idx],end_link_array[end_link_array_num_idx],11);
					
							insert into regulation_item_tbl(regulation_id, linkid, nodeid, seq_num)
							values(curr_reg_id, start_link_array[start_link_array_num_idx],null,1);
							insert into regulation_item_tbl(regulation_id, linkid, nodeid, seq_num)
							values(curr_reg_id,null, rec1.node_id,2);
							insert into regulation_item_tbl(regulation_id, linkid, nodeid, seq_num)
							values(curr_reg_id, end_link_array[end_link_array_num_idx], null,3);
						end loop;
						FETCH cur3 INTO rec3;
					end loop;
				end loop;
			
			end if;
			
			close cur2;
			close cur3;
			
		end loop;
		FETCH cur1 INTO rec1;
	end loop;
	close cur1;

	return 0;
END;
$$;

CREATE OR REPLACE FUNCTION mid_get_node_fromtwolink(fromlink bigint, tolink bigint)
  RETURNS bigint 
  LANGUAGE plpgsql AS
$$ 
DECLARE
	fromnodeid1	bigint;
	tonodeid1	bigint;
	fromnodeid2	bigint;
	tonodeid2	bigint;
BEGIN
	if fromLink is null or toLink is null then
		return null;
	end if;
  
	select start_node_id, end_node_id into fromnodeid1, tonodeid1 
	    from temp_topo_link 
	    where new_link_id = fromLink;
	if not found then
		return NULL;
	end if;
	
	select start_node_id, end_node_id into fromnodeid2, tonodeid2 
	   from temp_topo_link 
	   where new_link_id = toLink;
	if not found then
		return NULL;
	end if;

	return (
	case 
	when fromnodeid1 = fromnodeid2 then fromnodeid1
	when fromnodeid1 = tonodeid2 then fromnodeid1
	when tonodeid1 = fromnodeid2 then tonodeid1
	when tonodeid1 = tonodeid2 then tonodeid1
	else NULL end
	);
END;
$$;


-------------------------------------------------------------------------------
-- Get connected Node of two links.
CREATE OR REPLACE FUNCTION mid_get_connected_node(fromlink bigint, tolink bigint)
  RETURNS bigint 
  LANGUAGE plpgsql AS
$$ 
DECLARE
	fromnodeid1	bigint;
	tonodeid1	bigint;
	fromnodeid2	bigint;
	tonodeid2	bigint;
BEGIN
	if fromLink is null or toLink is null then
		return null;
	end if;
  
	select s_node, e_node into fromnodeid1, tonodeid1 
	    from link_tbl 
	    where link_id = fromLink;
	if not found then
		return NULL;
	end if;
	
	select s_node, e_node into fromnodeid2, tonodeid2 
	   from link_tbl 
	   where link_id = toLink;
	if not found then
		return NULL;
	end if;

	return (
	case 
	when fromnodeid1 = fromnodeid2 then fromnodeid1
	when fromnodeid1 = tonodeid2 then fromnodeid1
	when tonodeid1 = fromnodeid2 then tonodeid1
	when tonodeid1 = tonodeid2 then tonodeid1
	else NULL end
	);
END;
$$;

CREATE OR REPLACE FUNCTION make_temp_lane_info()
    RETURNS smallint
    LANGUAGE plpgsql
AS $$
DECLARE
	rec					record;
	length                                  integer;
	i                                       integer;
	temp_lane_num                           integer;
	lanes                                   text[];
	j                                       integer;
BEGIN
	temp_lane_num = 0;
	i = 0;
	length = 0;
	for rec in 
		SELECT gid,link_id,
		array[SUBSTRING(rstr1 from '[0-9]+'),
		SUBSTRING(rstr2 from '[0-9]+'),
		SUBSTRING(rstr3 from '[0-9]+'),
		SUBSTRING(rstr4 from '[0-9]+'),
		SUBSTRING(rstr5 from '[0-9]+')] as outlinks,
		array[SUBSTRING(SUBSTRING(rstr1,position('>' in rstr1)-1) from '[0-9>,]+'),
		SUBSTRING(SUBSTRING(rstr2,position('>' in rstr2)-1) from '[0-9>,]+'),
		SUBSTRING(SUBSTRING(rstr3,position('>' in rstr3)-1) from '[0-9>,]+'),
		SUBSTRING(SUBSTRING(rstr4,position('>' in rstr4)-1) from '[0-9>,]+'),
		SUBSTRING(SUBSTRING(rstr5,position('>' in rstr5)-1) from '[0-9>,]+')] conns,
	       lane_cnt_f, lane_cnt_r,folder
		from(
			  select gid,link_id,replace(trim(both ',' from rstr1),' ','') as rstr1,replace(trim(both ',' from rstr2),' ','') as rstr2,
				replace(trim(both ',' from rstr3),' ','') as rstr3,replace(trim(both ',' from rstr4),' ','') as rstr4,
				replace(trim(both ',' from rstr5),' ','') as rstr5,lane_cnt_f, lane_cnt_r,folder
			  FROM org_processed_line
			  where rstr1 is not null
		) as a
	LOOP
		i = 1;
		length = array_upper(rec.outlinks,1);
		while i <= length and rec.outlinks[i] is not null loop
			lanes = regexp_split_to_array(rec.conns[i],',');
			temp_lane_num = array_upper(lanes,1);
			j = 1;
			while j <= temp_lane_num and lanes[j] is not null loop
				INSERT INTO temp_lane_info
				("gid", 
				 "link_id", "outlinkid", 
				 "laneno",
				 "lane_cnt_f", "lane_cnt_r",
				 "folder"
				 )
				VALUES
				(
				rec.gid,rec.link_id,rec.outlinks[i]::integer,SUBSTRING(lanes[j],1,1),rec.lane_cnt_f,rec.lane_cnt_r,rec.folder
				);
				j = j + 1;
			end loop;
			i = i + 1;
		end loop;
		
	END LOOP;
	return 1;
END;
$$;
CREATE OR REPLACE FUNCTION mid_findpasslinkbybothlinks(nfromlink bigint, ntolink bigint, startnode integer, endnode integer, dir integer)
  RETURNS character varying
LANGUAGE plpgsql volatile
AS $$
DECLARE
	rstPathStr  		varchar;
	rstPathCount		integer;
	rstPath			varchar;
	
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
	tmpPathArray		:= ARRAY[cast('(2)'||nFromLink as varchar)];
	tmpPathCount		:= 1;
	tmpPathIndex		:= 1;
	nStartNode          := endNode;
	nScondNode          := startNode;
	if dir = 1 then
		tmpLastNodeArray	:= ARRAY[nStartNode,nScondNode];
		tmpLastLinkArray	:= ARRAY[nFromLink,nFromLink];
		tmpPathArray		:= array_append(tmpPathArray,cast('(3)'||nFromLink as varchar));
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
			if array_upper(regexp_split_to_array(tmpPathArray[tmpPathIndex], E'\\|+'),1) < 7 then
				if tmpLastLinkArray[tmpPathIndex] = nToLink then
					rstPathCount	:= rstPathCount + 1;
					rstPath		:= cast(tmpPathArray[tmpPathIndex] as varchar);
					if SUBSTRING(rstPath,2,1) = '2' then
						rstPath		:= replace(rstPath, '(2)', '');
						rstPath		:= replace(rstPath, '(3)', '');
						rstPath = rstPath || '(2)';
					elsif SUBSTRING(rstPath,2,1) = '3' then
						rstPath		:= replace(rstPath, '(3)', '');
						rstPath		:= replace(rstPath, '(2)', '');
						rstPath = rstPath || '(3)';
					end if;
					rstPathStr	:=  rstPath;
					exit;
					
				else
					for rec in 
						select e.nextroad,e.dir,e.nextnode
							from
							(
								SELECT link_id as nextroad,'(2)' as dir,end_node_id as nextnode
								FROM temp_topo_link_org_oneway
								where start_node_id = tmpLastNodeArray[tmpPathIndex] and (oneway = 0 or oneway = 1)

								union

								SELECT link_id as nextroad,'(3)' as dir,start_node_id as nextnode
								FROM temp_topo_link_org_oneway
								where end_node_id = tmpLastNodeArray[tmpPathIndex] and oneway = 0
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
CREATE OR REPLACE FUNCTION make_nodeid_by_passlink(passlink text)
    RETURNS integer
    LANGUAGE plpgsql
AS $$
DECLARE
	rec					record;
	passlinkarray                           text[];
	start_node1                             integer;
	end_node1                               integer;
	start_node2                             integer;
	end_node2                               integer;
BEGIN
	passlinkarray = regexp_split_to_array(passlink, E'\\|+');
	SELECT start_node_id, end_node_id
	into  start_node1,end_node1
	FROM temp_topo_link
	where new_link_id = (passlinkarray[1]::bigint);
	
	SELECT start_node_id, end_node_id
	into  start_node2,end_node2
	FROM temp_topo_link
	where new_link_id = (passlinkarray[2]::bigint);
	if start_node1 in (start_node2,end_node2) then
		return start_node1;
	else
		return end_node1;
	end if;
END;
$$;
CREATE OR REPLACE FUNCTION make_temp_spotguide_info()
    RETURNS smallint
    LANGUAGE plpgsql
AS $$
DECLARE
	rec					record;
	length                                  integer;
	i                                       integer;
	names_len                               integer;
	name_array                              text[];
BEGIN
	i = 0;
	length = 0;
	for rec in 
		SELECT gid,link_id,
			array[SUBSTRING(rstr6 from '[0-9]+'),
			SUBSTRING(rstr7 from '[0-9]+'),
			SUBSTRING(rstr8 from '[0-9]+')] as outlinks,
			array[spath6,spath7,spath8] as names,
			array[SUBSTRING(slyr6_2 from '[0-9]+'),
			SUBSTRING(slyr7_2 from '[0-9]+'),
			SUBSTRING(slyr8_2 from '[0-9]+')] as arrowids,
			array[SUBSTRING(slyr6_3 from '[0-9]+'),
			SUBSTRING(slyr7_3 from '[0-9]+'),
			SUBSTRING(slyr8_3 from '[0-9]+')] as signs,
		       folder
		from(
			SELECT gid, link_id, 
				replace(rstr6,' ','') as rstr6, replace(rstr7,' ','') as rstr7, replace(rstr8,' ','') as rstr8, 
				replace(spath6,' ','') as spath6, replace(spath7,' ','') as spath7, replace(spath8,' ','') as spath8, 
				replace(slyr6_1,' ','') as slyr6_1, replace(slyr6_2,' ','') as slyr6_2, replace(slyr6_3,' ','') as slyr6_3, replace(slyr6_4,' ','') as slyr6_4, 
				replace(slyr7_1,' ','') as slyr7_1, replace(slyr7_2,' ','') as slyr7_2, replace(slyr7_3,' ','') as slyr7_3, replace(slyr7_4,' ','') as slyr7_4, 
				replace(slyr8_1,' ','') as slyr8_1, replace(slyr8_2,' ','') as slyr8_2, replace(slyr8_3,' ','') as slyr8_3, replace(slyr8_4,' ','') as slyr8_4, 
				folder
			FROM org_processed_line
			where rstr6 is not null
		) as a
	LOOP
		i = 1;
		names_len =0;
		length = array_upper(rec.outlinks,1);
		while i <= length and rec.outlinks[i] is not null loop
			name_array = regexp_split_to_array(rec.names[i], E'\\\\+');
			names_len = array_upper(name_array,1);
			INSERT INTO temp_spotguide_info
			("gid", 
			 "link_id", "outlinkid", 
			 "illustname",
			 "arrowid",
			 "signid",
			 "folder"
			 )
			VALUES
			(
			rec.gid,rec.link_id,rec.outlinks[i]::integer,name_array[names_len],rec.arrowids[i],rec.signs[i],rec.folder
			);
			i = i + 1;
		end loop;
		
	END LOOP;
	return 1;
END;
$$;
create or replace function temp_make_malsing_node()
returns integer
language plpgsql
AS $$
DECLARE
	rec			record;
	rec2			record;
	node_id			integer;
	walk_gid		integer;

	search_gid_array	integer[];
	search_index		integer;
	search_count		integer;
BEGIN
	node_id	:= 0;
	for rec in select * from temp_topo_point order by gid
	loop
		-- check whether currend point has been walked
		select gid from temp_topo_walk_point where gid = rec.gid into walk_gid;
		if FOUND then continue; end if;

		-- new node
		node_id	:= node_id + 1;
		insert into temp_topo_node(node_id, x, y, z, the_geom) values (node_id, rec.x, rec.y, rec.z, rec.the_geom);
	
		-- record current point
		insert into temp_topo_walk_point(gid) values (rec.gid);
		insert into temp_topo_point_node(gid, x, y, z, node_id) values (rec.gid, rec.x, rec.y, rec.z, node_id);

		-- search near point
		search_gid_array	:= ARRAY[rec.gid];
		search_count		:= 1;
		search_index		:= 0;
		while search_index < search_count loop
			search_index	:= search_index + 1;
			for rec2 in 
				select n.*
				from
				(
					select aid as gid 
					from temp_topo_near_point 
					where search_gid_array[search_index] = bid
					union
					select bid as gid 
					from temp_topo_near_point 
					where search_gid_array[search_index] = aid
				)as m
				left join temp_topo_point as n
				on m.gid = n.gid
			loop
				-- current point has been walked, then pass
				if rec2.gid = ANY(search_gid_array) then 
					continue;
				end if;

				-- record current point, and add it into search_array
				insert into temp_topo_walk_point(gid) values (rec2.gid);
				if st_equals(rec.the_geom, rec2.the_geom) then
					insert into temp_topo_point_node(gid, x, y, z, node_id) 
						values (rec2.gid, rec2.x, rec2.y, rec2.z, node_id);
				else
					insert into temp_topo_point_node(gid, x, y, z, node_id,new_geom) 
						values (rec2.gid, rec2.x, rec2.y, rec2.z, node_id,rec.the_geom);
				end if;
				
				search_gid_array	:= search_gid_array || rec2.gid;
				search_count		:= search_count + 1;
			end loop;
		end loop;
	end loop;
	return node_id;
END
$$;
-----------------------------------------------------------------------
------admin
-----------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_insert_into_state()
	RETURNS INTEGER
	LANGUAGE PLPGSQL
AS $$
DECLARE
	rec record;
BEGIN	
	for rec in select code,name from temp_country
	loop
		insert into temp_state(code, country_code, name, the_geom)
		(
			select rec.code + (ROW_NUMBER() OVER(ORDER BY state_name))*1000,rec.code,state_name,muti_geom
			from
			(
				select name_lib[1] as state_name,ST_Multi(st_union(the_geom)) as muti_geom
				from
				(
					select regexp_split_to_array("name",E'\\\,+')::varchar[] as name_lib,the_geom
					from "org_Country_State_Admin_Area_20"
				)temp
				where name_lib[3] = rec.name
				group by name_lib[1]
			)temp1
		);
	end loop;
	
	return 0;
	
END;
$$;

CREATE OR REPLACE FUNCTION mid_insert_into_city()
	RETURNS INTEGER
	LANGUAGE PLPGSQL
AS $$
DECLARE
	rec record;
	imax_code integer;
	name_array varchar[];
BEGIN
	insert into temp_code_city(city_name, the_geom)
	(
		select (regexp_split_to_array("name",E'\\\,+'))[1]::varchar as strname, the_geom
		from "org_District_City_Subcity_Taman_Admin_Area_20"
		where datalevel = 3
		
		union
		
		select city_name as strname, the_geom
		from temp_district_different_city
		
		union
		
		select city_name as strname, the_geom
		from temp_state_different_city
		
		union
		
		select "name" as strname, the_geom
		from sgp_builtup_region
	);
	 
	for rec in select code,the_geom
						from temp_state
	loop
		insert into temp_code_city_temp(city_id, code, the_geom)
		(
			select gid, rec.code, st_multi(ST_Difference(the_geom,rec.the_geom)) as new_geom
			from
			(
				select gid,the_geom
				from temp_code_city
				where ST_Intersects(the_geom,rec.the_geom)
			)temp
			where (ST_GeometryType(ST_Difference(the_geom,rec.the_geom)) = 'ST_MultiPolygon')
         or (ST_GeometryType(ST_Difference(the_geom,rec.the_geom)) = 'ST_Polygon')
		);
	end loop;
	
	for rec in select code,the_geom
						from temp_state
	loop
		insert into temp_city(code, state_code, name, the_geom)
		(
			select rec.code + ROW_NUMBER() OVER(ORDER BY city_name),rec.code,city_name,ST_Multi(st_union(the_geom)) as muti_geom
			from
			(
				select city_name, the_geom
				from temp_code_city as a
				where ST_Intersects(rec.the_geom,the_geom) and 
							((ST_Area(ST_Intersection(the_geom,rec.the_geom))/ST_Area(the_geom)) > 0.8)
			)temp
      group by city_name
		);	
	end loop;

	return 0;
	
END;
$$;

CREATE OR REPLACE FUNCTION rdb_cal_zm(shape_point geometry, dir integer) RETURNS smallint
  LANGUAGE plpgsql
AS $$
DECLARE
	ref_lat double precision;
	real_delta_lat double precision;
    real_delta_lon double precision;
    real_delta_d double precision; 
    real_dir double precision; 
    delta_lat bigint; 
    delta_lon bigint; 
    --ret_value smallint;
    
    A_WGS84  double precision;
    E2_WGS84 double precision;
    PI double precision;
    --DEG_TO_NAVI double precision;
    
    org_lon bigint;
    org_lat bigint;
    dst_lon bigint;
    dst_lat bigint;

    sinlat double precision;
    c double precision;
    lat_radi double precision;
    
    M2SecLon double precision;
    M2SecLat double precision;
    --max      SMALLINT;
    
    direction smallint;
    direction_32 integer;
    num     integer;
    i       integer;
BEGIN 
    PI          := 3.1415926535897932;
    A_WGS84     := 6378137.0;
    E2_WGS84    := 6.69437999013e-3;
    --DEG_TO_NAVI := (65536 / 360.0);  -- 0x10000/360.0

    num := ST_NPoints(shape_point);  
    IF (num < 2) THEN
        RETURN NULL;
    END IF;

    IF (dir = -1) THEN
	org_lon := round(ST_X(ST_PointN(shape_point, num)) * 256 * 3600);
	org_lat := round(ST_Y(ST_PointN(shape_point, num)) * 256 * 3600);
    else
	org_lon := round(ST_X(ST_PointN(shape_point, 1)) * 256 * 3600);
	org_lat := round(ST_Y(ST_PointN(shape_point, 1)) * 256 * 3600);
    end if;
    i := 1;
    while (num - 1) >= i LOOP
	IF (dir = -1) THEN
		dst_lon := round(ST_X(ST_PointN(shape_point, num - i)) * 256 * 3600);
		dst_lat := round(ST_Y(ST_PointN(shape_point, num - i)) * 256 * 3600);
	else
		dst_lon := round(ST_X(ST_PointN(shape_point, 1 + i)) * 256 * 3600);
		dst_lat := round(ST_Y(ST_PointN(shape_point, 1 + i)) * 256 * 3600);
	end if;

	delta_lon := dst_lon - org_lon;
	delta_lat := dst_lat - org_lat;
	-- Get the next shape point
	if delta_lon = 0 and delta_lat = 0 then
		if num - 1 = i then
			return null;
		end if;
		i := i + 1;
	else
		exit;
	end if;
    END LOOP;
    
    ref_lat   := (org_lat + delta_lat / 2.0) / 256.0;
    lat_radi  := ref_lat / 3600.0 * PI / 180.0;
    sinlat    := sin(lat_radi);
    c         := 1.0 - (E2_WGS84 * sinlat * sinlat);
    M2SecLon  := 3600.0 / ((PI/180.0) * A_WGS84 * cos(lat_radi) / sqrt(c));
    M2SecLat  := 1.0 / ((PI/648000.0) * A_WGS84 * (1 - E2_WGS84)) * sqrt(c*c*c);
    real_delta_lat := delta_lat / M2SecLat / 256.0;
    real_delta_lon := delta_lon / M2SecLon / 256.0;
  
    real_delta_d := sqrt((real_delta_lat*real_delta_lat) + (real_delta_lon*real_delta_lon));
    
    if (real_delta_d = 0.0) then
        real_dir  := 0.0;
    else 
        real_dir := (atan2(real_delta_lat, real_delta_lon) * 180.0 / PI);
    END IF;
    
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

CREATE OR REPLACE FUNCTION rdb_cal_zm_lane(geom geometry, dir integer) RETURNS float
  LANGUAGE plpgsql
AS $$
DECLARE
	angle_int int;

	angle_f float;

BEGIN 
    angle_int := rdb_cal_zm(geom,dir);
    
    if angle_int > 0 then
		angle_int := angle_int + 32768;
		angle_int := angle_int - 65535;
		angle_f := angle_int * 360.0 / 65535.0;
	
    else
		angle_int := angle_int + 32768;
		angle_f := angle_int * 360.0 / 65535.0;
	
    end if;
    return angle_f;
END;
$$;