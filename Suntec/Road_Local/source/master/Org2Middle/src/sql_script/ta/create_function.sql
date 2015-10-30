-------------------------------------------------------------------------------------------------------------
-- Dictionary
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_add_one_name(n_id integer, _name character varying, py character varying, lang_type integer)
  RETURNS integer 
  LANGUAGE plpgsql 
  AS $$
DECLARE
	max_name_id integer;
BEGIN
	IF n_id IS NULL or n_id = 0 THEN 
		-- Get the Max Name ID
		SELECT max(name_id) into max_name_id FROM name_dictionary_tbl;
		IF max_name_id IS NULL THEN
			n_id := 1;
		ELSE
			n_id := max_name_id + 1;
		END IF;
	END IF;
	
	INSERT INTO name_dictionary_tbl("name_id", "name", "py", "language")
		values(n_id, _name, py, lang_type);
    
	return n_id;
END;
$$;

CREATE OR REPLACE FUNCTION mid_get_language_id(lang_code character varying)
    RETURNS integer
    LANGUAGE plpgsql
    AS $$ 
DECLARE
	temp_language_id integer;
BEGIN
	IF lang_code IS NULL THEN
		RETURN NULL;
	END IF;
	
	SELECT language_id into temp_language_id FROM temp_language_tbl where language_code = lang_code;
	IF FOUND THEN
		RETURN temp_language_id;
	END IF;
	return NULL;
END;
$$;

-------------------------------------------------------------------------------------------------------------
-- shield number dictionary (for Guide)
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_make_shield_num_dictionary()
 RETURNS integer
    LANGUAGE plpgsql
    AS  
$$ 
DECLARE
    curs1 refcursor;
    gid_lid integer[];
    gid_id integer;
    cnt integer;
    curr_name_id integer;
    i integer;
    temp_link_id bigint;
    temp_shield_id smallint;
    temp_rteprior smallint;
    temp_seqnm smallint;
    temp_shield_num character varying;

BEGIN
	curr_name_id := 0;
	cnt          := 0;
	OPEN curs1 FOR select gid_array, shield_id_num
					  from (
						select array_agg(gid) as gid_array, 
							   (shield_id::varchar || E'\t' || shield_num::varchar) as shield_id_num
						  from temp_mid_link_shield
						  group by (shield_id::varchar || E'\t' || shield_num::varchar)
					  ) as a
					  order by gid_array, shield_id_num;

	-- Get the first record        
	FETCH curs1 INTO gid_lid, temp_shield_num;
	while gid_lid is not null LOOP
		IF temp_shield_num IS NOT NULL THEN
			curr_name_id := mid_add_one_name(curr_name_id, temp_shield_num, null, 3);  -- English
		
		
			-- They use the same name_id
			i := 1;
			while i <= array_upper(gid_lid, 1) LOOP
				gid_id = gid_lid[i];
				SELECT link_id, shield_id, rteprior, seqnm
				  INTO temp_link_id, temp_shield_id, temp_rteprior, temp_seqnm
				  FROM temp_mid_link_shield where gid = gid_id;
				
				insert into temp_mid_shield_number(
						"link_id", "rteprior", "new_name_id", "seqnm")
					values (temp_link_id, temp_rteprior, curr_name_id, temp_seqnm);
				i   := i + 1;
				cnt := cnt + 1;
			END LOOP;
			
			curr_name_id := curr_name_id + 1;
		END IF;
		
		-- Get the next record
		FETCH curs1 INTO  gid_lid, temp_shield_num;
	END LOOP;
	close curs1;

    return cnt;
END;
$$;
-------------------------------------------------------------------------------------------------------------
-- link name
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_make_link_name_dictionary()
 RETURNS integer
    LANGUAGE plpgsql
    AS  
$$ 
DECLARE
	curs1 refcursor;
	link_lid bigint[];
	temp_link_id bigint;
	temp_road_name character varying; 
	cnt integer;
	curr_name_id integer;
	i integer;
	temp_shield_id smallint;
	temp_nametyp smallint;
	language_id integer;
	language_code character varying;
BEGIN
	curr_name_id := 0;
	cnt          := 0;
	OPEN curs1 FOR SELECT link_array, name, lang_type
					  from (
						SELECT array_agg(link_id) AS link_array, name, lang_type
						  FROM temp_mid_link_merged_name
						  group by name, lang_type
					  ) as a
					  order by name, lang_type
					  ;

	-- Get the first record        
	FETCH curs1 INTO link_lid, temp_road_name, language_code;
	while link_lid is not null LOOP
		IF temp_road_name IS NOT NULL THEN
			language_id  := mid_get_language_id(language_code);
			curr_name_id := mid_add_one_name(curr_name_id, temp_road_name, null, language_id); 
		else 
			FETCH curs1 INTO link_lid, temp_road_name, language_code;
			continue;
		END IF;
		
		-- They use the same name_id
		i := 1;
		while i <= array_upper(link_lid, 1) LOOP
			temp_link_id = link_lid[i];

			insert into temp_mid_road_name("link_id", "name_type", "name", "language_code", "new_name_id")
				values (temp_link_id, 1, temp_road_name, language_code, curr_name_id);
			i   := i + 1;
			cnt := cnt + 1;
		END LOOP;
		
		curr_name_id := curr_name_id + 1;
		
		-- Get the next record
		FETCH curs1 INTO  link_lid, temp_road_name, language_code;
	END LOOP;
	close curs1;

	return cnt;
END;
$$;

-------------------------------------------------------------------------------------------------------------
-- towardname Route number
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_make_towardname_rn_dictionary()
 RETURNS integer
    LANGUAGE plpgsql
    AS  
$$ 
DECLARE
    curs1 refcursor;
    gid_lid integer[];
    cnt integer;
    curr_name_id integer;
    temp_road_number character varying; 
BEGIN
	curr_name_id := 0;
	cnt          := 0;
	OPEN curs1 FOR SELECT array_agg(gid) as g_ids, txtcont as road_number
			FROM si
			where infotyp = 'RN' -- Route Number [RN]; 
			group by txtcont
			;

	-- Get the first record        
	FETCH curs1 INTO gid_lid, temp_road_number;
	while gid_lid is not null LOOP
		IF temp_road_number IS NOT NULL THEN
			curr_name_id     := mid_add_one_name(curr_name_id, temp_road_number, null, 3); 			

			INSERT INTO temp_towardname_si(
				gid, id, seqnr, destseq, infotyp, rnpart, contyp, new_name_id)
			(
			SELECT gid, id, seqnr, destseq, infotyp, rnpart, contyp, curr_name_id
			  FROM si
			  where gid = any(gid_lid) 
			);
			
			curr_name_id     := curr_name_id + 1;
			cnt = cnt + array_upper(gid_lid, 1);
		END IF;
		
		-- Get the next record
		FETCH curs1 INTO  gid_lid, temp_road_number;
	END LOOP;
	close curs1;

    return cnt;
END;
$$;

-------------------------------------------------------------------------------------------------------------
-- towardname (Route Name, Other Destination, Place Name, Street Name)
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_make_towardname_name_dictionary()
 RETURNS integer
    LANGUAGE plpgsql
    AS  
$$ 
DECLARE
	curs1 refcursor;
	gid_lid integer[];
	gid_id integer;
	cnt integer;
	curr_name_id integer;
	temp_toward_name character varying; 
	temp_language_code character varying; 
	language_id integer;
BEGIN
	curr_name_id := 0;
	cnt          := 0;
	OPEN curs1 FOR SELECT array_agg(gid) as g_ids, txtcont as toward_name, txtcontlc as language_code
					FROM si
					where infotyp in ('RJ', '4I', '9D', '6T', '4G')   -- Route Name [RJ], Other Destination [4I], Place Name [9D], Street Name [6T], Exit Name [4G]
					group by txtcont, txtcontlc
					;

	-- Get the first record       
	FETCH curs1 INTO gid_lid, temp_toward_name, temp_language_code;
	while gid_lid is not null LOOP
		IF temp_toward_name IS NOT NULL THEN
			language_id      := mid_get_language_id(temp_language_code);
			curr_name_id     := mid_add_one_name(curr_name_id, temp_toward_name, null, language_id); 

			INSERT INTO temp_towardname_si(
				gid, id, seqnr, destseq, infotyp, rnpart, contyp, new_name_id)
			(
			SELECT gid, id, seqnr, destseq, infotyp, rnpart, contyp, curr_name_id
			  FROM si
			  where gid = any(gid_lid) 
			);

			curr_name_id := curr_name_id + 1;
			cnt          := cnt + array_upper(gid_lid, 1);
		END IF;
		
		-- Get the next record
		FETCH curs1 INTO  gid_lid, temp_toward_name, temp_language_code;
	END LOOP;
	close curs1;

    return cnt;
END;
$$;
-------------------------------------------------------------------------------------------------------------
-- make link number
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_make_link_number()
    RETURNS integer
    LANGUAGE plpgsql
    AS $$ 
DECLARE
	cnt integer;
	curs1 refcursor;
	curr_link_id bigint;
	pre_link_id bigint;
	name_id integer;
	is_roadsign character varying;
	seq_nm smallint;
	temp_route_type smallint;
	temp_scale_type smallint;
	temp_country_code character varying;
	--temp_hierarchy_id integer;
	temp_shield_id smallint;
BEGIN
	cnt := 0;
	pre_link_id  := 0;
	
	OPEN curs1 FOR SELECT temp_road_number.link_id, new_name_id, rteprior, shield_id, country_code
					  FROM temp_road_number
					  left join name_dictionary_tbl
					  on new_name_id = name_dictionary_tbl.name_id
					  left join temp_link_country
					  on temp_road_number.link_id = temp_link_country.link_id
					  order by link_id, rteprior, length(name), name;
	
	-- Get the first record        
	FETCH curs1 INTO curr_link_id, name_id, temp_scale_type, temp_shield_id, temp_country_code;
	while curr_link_id is not null LOOP
		IF curr_link_id <> pre_link_id THEN 
			seq_nm := 1;
			pre_link_id := curr_link_id;
		ELSE 
			seq_nm := seq_nm + 1;
		END IF;
		
		INSERT INTO link_name_relation_tbl(
			    "link_id", "name_id", "scale_type", "shield_id", "seq_nm", "name_type", "country_code", "hierarchy_id")
		    VALUES (curr_link_id, name_id, temp_scale_type, temp_shield_id, seq_nm, 3, temp_country_code, 1);

		cnt  := cnt + 1;
		-- Get the next record
		FETCH curs1 INTO curr_link_id, name_id, temp_scale_type, temp_shield_id, temp_country_code;
	END LOOP;
	close curs1;
	
	return cnt;
END;
$$;

-------------------------------------------------------------------------------------------------------------
-- make link name
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_make_link_name()
    RETURNS integer
    LANGUAGE plpgsql
    AS $$ 
DECLARE
	cnt integer;
	curs1 refcursor;
	curr_link_id bigint;
	pre_link_id bigint;
	temp_name_id integer;
	seq_nm smallint;
	temp_country_code character varying;
	temp_name_type smallint;
	name_type smallint;
BEGIN
	cnt := 0;
	pre_link_id  := 0;
	
	OPEN curs1 FOR SELECT a.link_id, new_name_id, country_code, a.nametyp,
						   nw.id is null as nw_flag -- for sort
					  FROM temp_road_name as a
					  left join name_dictionary_tbl
					  on new_name_id = name_id
					  left join org_nw as nw
					  on a.link_id = id and a.nametyp = nw.nametyp and name_dictionary_tbl."name" = nw."name"
					  left join temp_link_country
					  on a.link_id = temp_link_country.link_id
					  order by link_id, nametyp, nw_flag;
	
	-- Get the first record        
	FETCH curs1 INTO curr_link_id, temp_name_id, temp_country_code, temp_name_type;
	while curr_link_id is not null LOOP
		IF curr_link_id <> pre_link_id THEN 
			seq_nm := 1;
			pre_link_id := curr_link_id;
		ELSE 
			seq_nm := seq_nm + 1;
		END IF;

		IF (temp_name_type & 1) = 1 THEN -- Official Name
			name_type := 1;
		ELSE
			name_type := 2;
		END IF;
		
		INSERT INTO link_name_relation_tbl(
			    "link_id", "name_id", "scale_type", "seq_nm", "name_type", "country_code", "hierarchy_id")
		    VALUES (curr_link_id, temp_name_id, -1, seq_nm, name_type, temp_country_code, 1);

		cnt  := cnt + 1;
		-- Get the next record
		FETCH curs1 INTO curr_link_id, temp_name_id, temp_country_code, temp_name_type;
	END LOOP;
	close curs1;
	
	return cnt;
END;
$$;

-------------------------------------------------------------------------------------------------------------
-- regulation
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_country_condition()
	RETURNS boolean
    LANGUAGE plpgsql
AS $$
DECLARE
	node_rec record;
	innode_rec record;
	outnode_rec record;
	cur_regulation_id integer;
BEGIN
	cur_regulation_id := 0;
		-- regulation_id
	select (case when max(regulation_id) is null then 0 else max(regulation_id) end)
	from regulation_relation_tbl
	into cur_regulation_id;
	--node
	for node_rec in 
	(
		select id from temp_country_node group by id
	) loop
		--into node
		for innode_rec in
		(
			select id,country_id
			from temp_country_link 
			where f_jnctid = node_rec.id and (oneway is null or oneway = 'TF')
			union
			select id,country_id
			from temp_country_link 
			where t_jnctid = node_rec.id and (oneway is null or oneway = 'FT')
		)	loop
			--out node
			for outnode_rec in 
			(
				select id,country_id
				from temp_country_link 
				where f_jnctid = node_rec.id and (oneway is null or oneway = 'FT') and country_id != innode_rec.country_id
				union
				select id,country_id
				from temp_country_link 
				where t_jnctid = node_rec.id and (oneway is null or oneway = 'TF') and country_id != innode_rec.country_id
			)loop
				cur_regulation_id := cur_regulation_id + 1;
				---into regulation_relation_tbl
	    	insert into regulation_relation_tbl("regulation_id", "nodeid", "inlinkid", "outlinkid", "condtype")
				VALUES	(cur_regulation_id, node_rec.id, innode_rec.id,outnode_rec.id,11);
	    	---into regulation_item_tbl
    	  insert into regulation_item_tbl("regulation_id", "linkid", "nodeid", "seq_num")
    		VALUES	(cur_regulation_id, innode_rec.id, null, 1);
    		insert into regulation_item_tbl("regulation_id", "linkid", "nodeid", "seq_num")
    		VALUES	(cur_regulation_id, null, node_rec.id, 2);
    		insert into regulation_item_tbl("regulation_id", "linkid", "nodeid", "seq_num")
    		VALUES	(cur_regulation_id, outnode_rec.id, null, 3);				
			end loop;
		end loop;
	end loop;	
	return true;
END;
$$;

CREATE OR REPLACE FUNCTION mid_Jude_Cartype_Array(CarArray smallint[])
	RETURNS boolean
    LANGUAGE plpgsql
AS $$
DECLARE
BEGIN
	if(0 = any(CarArray) or 11 = any(CarArray) or -1 = any(CarArray)) then
		return true;
	else
		return false;
	end if;		
END;
$$;
	
CREATE OR REPLACE FUNCTION mid_org_into_temp_regulation_permit()
	RETURNS smallint
    LANGUAGE plpgsql
AS $$
DECLARE
BEGIN
	insert into temp_regulation_permit
	select * from
	(
		select n.id, n.dir_pos, n.timedom,n.vt_array,
		(
			case when oneway = 'FT' then 2 -- Open in Positive Direction
			 when oneway = 'TF' then 3 -- Open in Negative Direction
			 when oneway = 'N' then 0 -- Closed in Both Directions
			 else 1 -- Open in Both Directions
			end
		)as link_oneway
		from
		(
			select id, dir_pos, timedom, array_agg(vt) as vt_array
			from
			(
				select a.id, a.seqnr, a.dir_pos, a.vt, b.timedom
				from
				(select * from org_rs where restrtyp = 'DF') as a
				left join org_td as b
				on a.id = b.id and a.seqnr = b.seqnr
				order by a.id, a.dir_pos, b.timedom, a.vt
			)as m
			group by id, dir_pos, timedom
		)as n
		left join org_nw as nw
		on n.id = nw.id
		where	not
			(
				(timedom is null) and mid_Jude_Cartype_Array(vt_array)
			)
	)as permit;
	return 0;
END;
$$;
CREATE OR REPLACE FUNCTION mid_org_into_temp_regulation_forbid()
	RETURNS smallint
    LANGUAGE plpgsql
AS $$
DECLARE
BEGIN
	insert into temp_regulation_forbid
	select * from
	(
		select n.id, n.dir_pos, n.timedom,n.vt_array,
		(
			case when oneway = 'FT' then 2 -- Open in Positive Direction
			 when oneway = 'TF' then 3 -- Open in Negative Direction
			 when oneway = 'N' then 0 -- Closed in Both Directions
			 else 1 -- Open in Both Directions
			end
		)as link_oneway
		from
		(
			select id, dir_pos, timedom, array_agg(vt) as vt_array
			from
			(
				select a.id, a.seqnr, a.dir_pos, a.vt, b.timedom
				from
				(select * from org_rs where restrtyp = 'DF') as a
				left join org_td as b
				on a.id = b.id and a.seqnr = b.seqnr
				order by a.id, a.dir_pos, b.timedom, a.vt
			)as m
			group by id, dir_pos, timedom
		)as n
		left join org_nw as nw
		on n.id = nw.id
		where (timedom is null) and mid_Jude_Cartype_Array(vt_array)
	)as forbid;
	return 0;
END;
$$;
CREATE OR REPLACE FUNCTION mid_org_into_temp_rs_df_todo()
	RETURNS smallint
    LANGUAGE plpgsql
AS $$
DECLARE
BEGIN
	insert into temp_rs_df_todo
	select * from
	(
		select * from
		(
			select id, dir_pos, timedom, array_agg(vt) as vt_array
			from
			(
				select a.id, a.seqnr, a.dir_pos, a.vt, b.timedom
				from
				(select * from org_rs where restrtyp = 'DF') as a
				left join org_td as b
				on a.id = b.id and a.seqnr = b.seqnr
				order by a.id, a.dir_pos, b.timedom, a.vt
			)as m
			group by id, dir_pos, timedom
		)as n
		where (timedom is not null) and mid_Jude_Cartype_Array(vt_array)
	)as linkrow;
	return 0;
END;
$$;
CREATE OR REPLACE FUNCTION mid_org_into_temp_rs_linkrow_todo()
	RETURNS smallint
    LANGUAGE plpgsql
AS $$
DECLARE
BEGIN
	insert into temp_rs_linkrow_todo
	select * from
	(
		select * from 
		(
			select id, dir_pos, timedom, array_agg(vt) as vt_array
			from
			(
				select r.id, r.seqnr, r.dir_pos, t.timedom, r.vt
				from
				(
					select b.id, b.seqnr, b.dir_pos, b.vt
					from
					(
						select *
						from org_mn
						where feattyp in (2101, 2103)
					)as a
					left join org_rs as b
					on a.id = b.id
				)as r
				left join org_td as t
				on r.id = t.id and r.seqnr = t.seqnr
				order by r.id, r.dir_pos, t.timedom, r.vt
			)as t1
			group by id, dir_pos, timedom
		)as t2
		where mid_Jude_Cartype_Array(vt_array)
	)as df;
	return 0;
END;
$$;
---
CREATE OR REPLACE FUNCTION mid_permit_into_regulation()
	RETURNS smallint
    LANGUAGE plpgsql
AS $$
DECLARE
	rec record;
	iDir_Pos smallint[];
BEGIN
	for rec in 
	(
		select id,array_agg(dir_pos) as iDir_Pos,link_oneway
	  from temp_regulation_permit GROUP BY id,link_oneway
	)
	loop
		-- dir_pos---regulation dir(1: both line directions / 2: positive line direction / 3: negative line direction)
		if 1 = any(rec.iDir_Pos) or (2 = any(rec.iDir_Pos) and 3 = any(rec.iDir_Pos)) then
			if rec.link_oneway in (0,2,3) then
				insert into temp_link_regulation_forbid_permit("linkid","condtype","cond_forbid_peumit")
				VALUES(rec.id,2,1);
				insert into temp_link_regulation_forbid_permit("linkid","condtype","cond_forbid_peumit")
				VALUES(rec.id,3,1);
			end if;
		elsif 2 = any(rec.iDir_Pos) then
			if rec.link_oneway in (0,3) then
				insert into temp_link_regulation_forbid_permit("linkid","condtype","cond_forbid_peumit")
				VALUES(rec.id,2,1);
			end if;
		elsif 3 = any(rec.iDir_Pos) then
			if rec.link_oneway in (0,2) then
				insert into temp_link_regulation_forbid_permit("linkid","condtype","cond_forbid_peumit")
				VALUES(rec.id,3,1);
			end if;
		end if;
	end loop;
	
	return 0;
END;
$$;

CREATE OR REPLACE FUNCTION mid_forbid_into_regulation()
	RETURNS smallint
    LANGUAGE plpgsql
AS $$
DECLARE
	rec record;
	iDir_Pos smallint[];
BEGIN
	for rec in 
	(
		select id,array_agg(dir_pos) as iDir_Pos,link_oneway
	  from temp_regulation_forbid GROUP BY id,link_oneway
	)
	loop
		-- dir_pos---regulation dir(1: both line directions / 2: positive line direction / 3: negative line direction)
		if 1 = any(rec.iDir_Pos) or (2 = any(rec.iDir_Pos) and 3 = any(rec.iDir_Pos)) then
			if rec.link_oneway in (1,2,3) then
				insert into temp_link_regulation_forbid_permit("linkid","condtype","cond_forbid_peumit")
				VALUES(rec.id,4,0);
			end if;
		elsif 2 = any(rec.iDir_Pos) then
			if rec.link_oneway in (1,2) then
				insert into temp_link_regulation_forbid_permit("linkid","condtype","cond_forbid_peumit")
				VALUES(rec.id,42,0);
			end if;
		elsif 3 = any(rec.iDir_Pos) then
			if rec.link_oneway in (1,3) then
				insert into temp_link_regulation_forbid_permit("linkid","condtype","cond_forbid_peumit")
				VALUES(rec.id,43,0);
			end if;
		end if;	
	end loop;
	
	return 0;
END;
$$;

CREATE OR REPLACE FUNCTION mid_Jude_Cartype(vt_array smallint)
	RETURNS boolean
    LANGUAGE plpgsql
AS $$
DECLARE

BEGIN
	return vt_array & (1 << 13) = (1 << 13);
END;
$$;

CREATE OR REPLACE FUNCTION mid_get_cartype(vt_array smallint[])
	RETURNS smallint
    LANGUAGE plpgsql
AS $$
DECLARE
	cartype		smallint;
	nIndex		integer;
	nCount		integer;
	vt			smallint;
BEGIN
	cartype := 0;
	
	nCount	:= array_upper(vt_array, 1);
	nIndex	:= 1;
	while nIndex <= nCount loop
		vt	:= vt_array[nIndex];
		
		---Passenger Cars fro tomtom_vnm
		if vt in(-1,0,11) then
			cartype := cartype | (1 << 13);
		end if;
	
		-- -1: Not Applicable
		--if vt = -1 then
			--cartype	:= cartype | (1 << 13) | (1 << 8) | (1 << 7) | (1 << 1);
		--end if;
		-- 0: All Vehicle Types
		--if vt = 0 then
			--cartype	:= cartype | (1 << 13) | (1 << 8) | (1 << 7) | (1 << 1);
		--end if;
		-- 11: Passenger Cars
		--if vt = 11 then
			--cartype	:= cartype | (1 << 13);
		--end if;
		-- 12: Residential Vehicles
		--if vt = 12 then
			--cartype	:= cartype | (1 << 1);
		--end if;
		-- 16: Taxi
		--if vt = 16 then
			--cartype	:= cartype | (1 << 8);
		--end if;
		-- 17: Public Bus
		--if vt = 17 then
			--cartype	:= cartype | (1 << 7);
		--end if;
		-- 24: Bicycle
		--if vt = 24 then
			--cartype	:= cartype | (1 << 5);
		--end if;
		
		nIndex	:= nIndex + 1;
	end loop;
	
	return cartype;
END;
$$;


CREATE OR REPLACE FUNCTION mid_update_datetime(datetime_array integer[])
	RETURNS integer[]
    LANGUAGE plpgsql
AS $$
DECLARE
	start_year		integer;
	start_month		integer;
	start_day		integer;
	start_hour		integer;
	start_minute	integer;
	end_year		integer;
	end_month		integer;
	end_day			integer;
	end_hour		integer;
	end_minute		integer;
	
	temp_array		integer[];
BEGIN
	-- init
	start_year		:= datetime_array[1];
	start_month 	:= datetime_array[2];
	start_day		:= datetime_array[3];
	start_hour		:= datetime_array[4];
	start_minute	:= datetime_array[5];
	end_year		:= datetime_array[6];
	end_month		:= datetime_array[7];
	end_day			:= datetime_array[8];
	end_hour		:= datetime_array[9];
	end_minute		:= datetime_array[10];
	
	-- update hour/minute
	if end_minute >= 60 then
		end_minute	:= end_minute - 60;
		end_hour	:= end_hour + 1;
	end if;
	if end_hour = 24 and end_minute = 0 then
		end_hour	:= 23;
		end_minute	:= 59;
	end if;
	if end_hour >= 24 then
		end_hour	:= end_hour - 24;
	end if;
	
	-- update start_date
	temp_array		:= mid_update_date(ARRAY[start_year, start_month, start_day]);
	start_year		:= temp_array[1];
	start_month 	:= temp_array[2];
	start_day		:= temp_array[3];
	
	-- update end_date
	temp_array		:= mid_update_date(ARRAY[end_year, end_month, end_day]);
	end_year		:= temp_array[1];
	end_month 		:= temp_array[2];
	end_day			:= temp_array[3];
	
	return ARRAY[start_year, start_month, start_day, start_hour, start_minute, end_year, end_month, end_day, end_hour, end_minute];
END;
$$;

CREATE OR REPLACE FUNCTION mid_update_date(date_array integer[])
	RETURNS integer[]
    LANGUAGE plpgsql
AS $$
DECLARE
	temp_year integer;
	temp_month integer;
	temp_day integer;
BEGIN
	-- init
	temp_year		:= date_array[1];
	temp_month 		:= date_array[2];
	temp_day		:= date_array[3];
	
	-- update month
	if temp_month > 12 then
		temp_month		:= temp_month - 12;
		if temp_year != 0 then
			temp_year	:= temp_year + 1;
		end if;
	end if;
	
	-- update month/day	
	if temp_month = 2 then
		if temp_day > 28 then
			temp_month		:= temp_month + 1;
			temp_day		:= temp_day - 28;
		end if;
	elseif temp_month in (4,6,9,11) then
		if temp_day > 30 then
			temp_month		:= temp_month + 1;
			temp_day		:= temp_day - 30;
		end if;
	elseif temp_month in (1,3,5,7,8,10,12) then
		if temp_day > 31 then
			temp_month		:= temp_month + 1;
			temp_day		:= temp_day - 31;
		end if;
	end if;
	
	-- update month again
	if temp_month > 12 then
		temp_month		:= temp_month - 12;
		if temp_year != 0 then
			temp_year	:= temp_year + 1;
		end if;
	end if;
	
	return ARRAY[temp_year, temp_month, temp_day];
END;
$$;

CREATE OR REPLACE FUNCTION mid_insert_condtion_record(	cur_cond_id integer, car_type smallint, timedom varchar, 
														datetime_array integer[], day_of_week integer, exclude_date smallint)
	RETURNS integer
    LANGUAGE plpgsql
AS $$
DECLARE
	start_year		integer;
	start_month		integer;
	start_day		integer;
	start_hour		integer;
	start_minute	integer;
	end_year		integer;
	end_month		integer;
	end_day			integer;
	end_hour		integer;
	end_minute		integer;
BEGIN
	-- update
	datetime_array	:= mid_update_datetime(datetime_array);
	
	start_year		:= datetime_array[1];
	start_month 	:= datetime_array[2];
	start_day		:= datetime_array[3];
	start_hour		:= datetime_array[4];
	start_minute	:= datetime_array[5];
	end_year		:= datetime_array[6];
	end_month		:= datetime_array[7];
	end_day			:= datetime_array[8];
	end_hour		:= datetime_array[9];
	end_minute		:= datetime_array[10];
	
	insert into temp_condition_regulation_tbl
				(
				"cond_id", "car_type", "timedom"
				)
		VALUES	(
				cur_cond_id, car_type, timedom
				);
	
	insert into condition_regulation_tbl
				(
				"cond_id", "car_type", "day_of_week",
				"start_year", "start_month", "start_day", "start_hour", "start_minute",
				"end_year", "end_month", "end_day", "end_hour", "end_minute", "exclude_date"
				)
		VALUES	(
				cur_cond_id, car_type, day_of_week,
				start_year, start_month, start_day, start_hour, start_minute,
				end_year, end_month, end_day, end_hour, end_minute, exclude_date
				);
	
	return 1;
END;
$$;

CREATE OR REPLACE FUNCTION mid_convert_condition_regulation_tbl()
    RETURNS smallint
    LANGUAGE plpgsql
AS $$
DECLARE
	rec				record;
	
	cur_cond_id		integer;
	start_year		integer;
	start_month		integer;
	start_day		integer;
	end_year		integer;
	end_month		integer;
	end_day			integer;
	start_hour		integer;
	start_minute	integer;
	end_hour		integer;
	end_minute		integer;
	day_of_week		integer;
	exclude_date	smallint;
	dt_array		integer[];
	
	temp1_array		varchar[];
	temp2_array		varchar[];
	nTemp1Count		integer;
	nTemp1Index		integer;
	strTemp1		varchar;
	nTemp2Count		integer;
	nTemp2Index		integer;
	strTemp2		varchar;
	
	temp_array		varchar[];
	nTempCount		integer;
	nTempIndex		integer;
	strTemp			varchar;
	
	time_array		varchar[];
	nTimeCount		integer;
	nTimeIndex		integer;
	strDateTime		varchar;
	
	subtime_array	varchar[];
	nSubTimeCount	integer;
	nSubTimeIndex	integer;
	strSubTime		varchar;
	strStartTime	varchar;
	strLastTime		varchar;
	strEndTime		varchar;
	
	nTargetPos		integer;
	nTempPos		integer;
	nHourPos		integer;
	nMinPos			integer;
	strChar			varchar;
	
	nStartYear		integer;
	nEndYear		integer;
	nStartWeek		integer;
	nStartWeekDay	integer;
	nEndWeek		integer;
	nEndWeekDay		integer;
	nFirstWeekDay	integer;
	
	nWeek			integer;
	nWeekDay		integer;
BEGIN
	cur_cond_id := 0;
	
	for rec in 
		select distinct car_type, timedom
		from
		(
			select id, dir_pos, timedom, mid_get_cartype(vt_array) as car_type
			from temp_rs_df_todo
			union
			select id, dir_pos, timedom, mid_get_cartype(vt_array) as car_type
			from temp_rs_linkrow_todo
		)as t
		order by car_type, timedom
    LOOP
        -- special logic: delete condition for all car and all time
        if mid_Jude_Cartype(rec.car_type) and rec.timedom is null then
        	continue;
        end if;
    	
    	-- split datetime into array of separate datetime if datetime exists '+'
    	-- example:
    	-- rec.timedom = "[[(h16){h8}]*[[(d1){d28}] + [(d29){d1}] + [(d30){d1}] + [(d31){d1}]]*[(M7){M2}]]"
    	-- array of separate datetime = {
    	--									"[[(h16){h8}]* [(d23){d6}] *[(M7){M2}]]",
    	--									"[[(h16){h8}]* [(d29){d1}] *[(M7){M2}]]",
    	--									"[[(h16){h8}]* [(d30){d1}] *[(M7){M2}]]",
    	--									"[[(h16){h8}]* [(d31){d1}] *[(M7){M2}]]"
    	--								}
    	--
    	if rec.timedom is null or strpos(rec.timedom, '+') = 0 then
    		time_array	:= ARRAY[rec.timedom];
    	else
    		--raise INFO 'datetime = %', rec.timedom;
    		temp1_array	:= regexp_split_to_array(rec.timedom, E'\\*+');
    		nTemp1Count	:= array_upper(temp1_array, 1);
    		for nTemp1Index in 1..nTemp1Count loop
	    		strTemp1	:= temp1_array[nTemp1Index];
	    		temp2_array	:= regexp_split_to_array(strTemp1, E'\\++');
    			if nTemp1Index = 1 then
    				time_array = temp2_array;
    			else
    				temp_array	:= time_array;
	    			nTempCount	:= array_upper(temp_array, 1);
		    		nTemp2Count	:= array_upper(temp2_array, 1);
		    		
		    		for nTemp2Index in 1..nTemp2Count loop
		    			strTemp2	:= temp2_array[nTemp2Index];
	    				for nTempIndex in 1..nTempCount loop
		    				time_array[nTempCount * (nTemp2Index-1) + nTempIndex]	:= (temp_array[nTempIndex] || '*' || strTemp2);
		    			end loop;
    				end loop;
    			end if;
    		end loop;
    		--raise INFO 'time_array = %', time_array;
    	end if;
    	
    	-- process every separate datetime
    	nTimeCount	:= array_upper(time_array, 1);
    	for nTimeIndex in 1..nTimeCount loop
	    	-- example of strDateTime: "[[(h16){h8}]*[[(d1){d28}]*[(M7){M2}]]"
	    	strDateTime		:= time_array[nTimeIndex];
	    	
			-- for different datetime type (1.start/end, 2.start/last)
		   	-- 0.datetime is null
		    if strDateTime is null then
	    		-- init datetime
				start_hour		:= 0;
				start_minute	:= 0;
				end_hour		:= 0;
				end_minute		:= 0;
				start_year		:= 0;
				start_month		:= 0;
				start_day		:= 0;
				end_year		:= 0;
				end_month		:= 0;
				end_day			:= 0;
				day_of_week		:= 0;
				exclude_date	:= 0;
				
		    	cur_cond_id := cur_cond_id + 1;
		    	perform mid_insert_condtion_record(	cur_cond_id, 
				    								rec.car_type, 
				    								rec.timedom,
				    								ARRAY[
				    								start_year, start_month, start_day, start_hour, start_minute,
				    								end_year, end_month, end_day, end_hour, end_minute],
				    								day_of_week,
				    								exclude_date);
			
		   	-- 1.start/end datetime type
		    elseif strpos(strDateTime,')(') > 0 then
	    		-- init datetime
				start_hour		:= 0;
				start_minute	:= 0;
				end_hour		:= 0;
				end_minute		:= 0;
				start_year		:= 0;
				start_month		:= 1;
				start_day		:= 1;
				end_year		:= 0;
				end_month		:= 1;
				end_day			:= 1;
				day_of_week		:= 0;
				exclude_date	:= 0;
				nStartWeek		:= null;
				
		    	-- start/end datetime type
		    	if strpos(strDateTime, '*') > 0 then
		    		raise EXCEPTION 'datetime error: start/end datetime with intersection, timedom = %', rec.timedom;
		    	else
		    		strSubTime		:= strDateTime;
		    	end if;
		    	
		    	-- start time
		    	strStartTime	:= substring(strSubTime, strpos(strSubTime,'(')+1, strpos(strSubTime,')')-strpos(strSubTime,'(')-1);
				nTargetPos	:= 1;
				nTempPos	:= 2;
				while nTempPos <= length(strStartTime)+1 loop
					strChar	:= substring(strStartTime, nTempPos, 1);
					if not ((strChar >= '0') and (strChar <= '9')) then
						if substring(strStartTime, nTargetPos, 1) = 'y' then
							start_year		:= cast(substring(strStartTime, nTargetPos+1, nTempPos-nTargetPos-1) as integer);
						elseif substring(strStartTime, nTargetPos, 1) = 'M' then
							start_month		:= cast(substring(strStartTime, nTargetPos+1, nTempPos-nTargetPos-1) as integer);
						elseif substring(strStartTime, nTargetPos, 1) = 'd' then
							start_day		:= cast(substring(strStartTime, nTargetPos+1, nTempPos-nTargetPos-1) as integer);
						elseif substring(strStartTime, nTargetPos, 1) = 'h' then
							start_hour		:= cast(substring(strStartTime, nTargetPos+1, nTempPos-nTargetPos-1) as integer);
						elseif substring(strStartTime, nTargetPos, 1) = 'm' then
							start_minute	:= cast(substring(strStartTime, nTargetPos+1, nTempPos-nTargetPos-1) as integer);
						elseif substring(strStartTime, nTargetPos, 1) = 'f' then
							nStartWeek		:= cast(substring(strStartTime, nTargetPos+1, 1) as integer);
							nStartWeekDay	:= cast(substring(strStartTime, nTargetPos+2, 1) as integer);
						else
							raise EXCEPTION 'unhandled timedom type1, timedom = %', rec.timedom;
						end if;
						nTargetPos	:= nTempPos;
					end if;
					nTempPos	:= nTempPos + 1;
		    	end loop;
		    	
		    	-- end time
		    	strTemp			:= substring(strSubTime, strpos(strSubTime,')(')+2);
		    	strEndTime		:= substring(strTemp, 1, strpos(strTemp,')')-1);
				nTargetPos	:= 1;
				nTempPos	:= 2;
				while nTempPos <= length(strEndTime)+1 loop
					strChar	:= substring(strEndTime, nTempPos, 1);
					if not ((strChar >= '0') and (strChar <= '9')) then
						if substring(strEndTime, nTargetPos, 1) = 'y' then
							end_year		:= cast(substring(strEndTime, nTargetPos+1, nTempPos-nTargetPos-1) as integer);
						elseif substring(strEndTime, nTargetPos, 1) = 'M' then
							end_month		:= cast(substring(strEndTime, nTargetPos+1, nTempPos-nTargetPos-1) as integer);
						elseif substring(strEndTime, nTargetPos, 1) = 'd' then
							end_day			:= cast(substring(strEndTime, nTargetPos+1, nTempPos-nTargetPos-1) as integer);
						elseif substring(strEndTime, nTargetPos, 1) = 'h' then
							end_hour		:= cast(substring(strEndTime, nTargetPos+1, nTempPos-nTargetPos-1) as integer);
						elseif substring(strEndTime, nTargetPos, 1) = 'm' then
							end_minute		:= cast(substring(strEndTime, nTargetPos+1, nTempPos-nTargetPos-1) as integer);
						elseif substring(strEndTime, nTargetPos, 1) = 'f' then
							nEndWeek		:= cast(substring(strEndTime, nTargetPos+1, 1) as integer);
							nEndWeekDay		:= cast(substring(strEndTime, nTargetPos+2, 1) as integer);
						else
							raise EXCEPTION 'unhandled timedom type1, timedom = %', rec.timedom;
						end if;
						nTargetPos	:= nTempPos;
					end if;
					nTempPos	:= nTempPos + 1;
		    	end loop;
		    	
		    	if nStartWeek is null then
					nStartYear	:= 0;
					nEndYear	:= 0;
				else
					nStartYear	:= extract(year from now());
					nEndYear	:= nStartYear + 4;
				end if;
				
				for nYear in nStartYear..nEndYear loop
					if nStartWeek is not null then
						--year
						start_year		:= nYear;
						-- weekday of the first day of the month
						nFirstWeekDay	:= extract(dow from to_timestamp(start_year || '-' || start_month || '-1', 'FXYYYY-MM-DD')) + 1;
						if nFirstWeekDay <= nStartWeekDay then
							start_day		:= 1 + ((nStartWeek - 1) * 7) + (nStartWeekDay - nFirstWeekDay);
						else
							start_day		:= 1 + (nStartWeek * 7) + (nStartWeekDay - nFirstWeekDay);
						end if;
						
						if start_month <= end_month then
							end_year	:= nYear;
						else
							end_year	:= nYear + 1;
						end if;
						nFirstWeekDay	:= extract(dow from to_timestamp(end_year || '-' || end_month || '-1', 'FXYYYY-MM-DD')) + 1;
						if nFirstWeekDay <= nEndWeekDay then
							end_day			:= 1 + ((nEndWeek - 1) * 7) + (nEndWeekDay - nFirstWeekDay);
						else
							end_day			:= 1 + (nEndWeek * 7) + (nEndWeekDay - nFirstWeekDay);
						end if;
					end if;
					
					-- insert first result
					if start_hour != 0 or start_minute != 0 then
				    	cur_cond_id := cur_cond_id + 1;
				    	perform mid_insert_condtion_record(	cur_cond_id, 
						    								rec.car_type, 
						    								rec.timedom,
						    								ARRAY[
						    								start_year, start_month, start_day, start_hour, start_minute, 
						    								start_year, start_month, start_day, 23, 59],
						    								day_of_week,
						    								exclude_date);
					end if;
					
					-- insert second result
					if start_hour != 0 or start_minute != 0 then
				    	cur_cond_id := cur_cond_id + 1;
				    	perform mid_insert_condtion_record(	cur_cond_id, 
						    								rec.car_type, 
						    								rec.timedom,
						    								ARRAY[
						    								start_year, start_month, start_day+1, 0, 0,
						    								end_year, end_month, end_day, 0, 0],
						    								day_of_week,
						    								exclude_date);
					else
				    	cur_cond_id := cur_cond_id + 1;
				    	perform mid_insert_condtion_record(	cur_cond_id, 
						    								rec.car_type, 
						    								rec.timedom,
						    								ARRAY[
						    								start_year, start_month, start_day, 0, 0,
						    								end_year, end_month, end_day, 0, 0],
						    								day_of_week,
						    								exclude_date);
					end if;
					
					-- insert third result
					if end_hour != 0 or end_minute != 0 then
				    	cur_cond_id := cur_cond_id + 1;
				    	perform mid_insert_condtion_record(	cur_cond_id, 
						    								rec.car_type, 
						    								rec.timedom,
						    								ARRAY[
						    								end_year, end_month, end_day, 0, 0,
						    								end_year, end_month, end_day, end_hour, end_minute],
						    								day_of_week,
						    								exclude_date);
					end if;
				end loop;
				
		    -- 2.start/last datetime type
		    elseif strpos(strDateTime,'){') > 0 then
	    		-- init datetime
				start_year		:= 0;
				start_month		:= 0;
				start_day		:= 0;
				start_hour		:= 0;
				start_minute	:= 0;
				end_year		:= 0;
				end_month		:= 0;
				end_day			:= 0;
				end_hour		:= 0;
				end_minute		:= 0;
				day_of_week		:= 0;
				exclude_date	:= 0;
				
		    	-- start/last datetime type
		    	subtime_array	:= regexp_split_to_array(strDateTime, E'\\*+');
		    	if subtime_array is null then
		    		nSubTimeCount	:= 0;
		    	else
		    		nSubTimeCount	:= array_upper(subtime_array, 1);
		    	end if;
		    	for nSubTimeIndex in 1..nSubTimeCount loop
			    	strSubTime		:= subtime_array[nSubTimeIndex];
			    	
			    	-- start time
			    	strStartTime	:= substring(strSubTime, strpos(strSubTime,'(')+1, strpos(strSubTime,')')-strpos(strSubTime,'(')-1);
					nTargetPos	:= 1;
					nTempPos	:= 2;
					while nTempPos <= length(strStartTime)+1 loop
						strChar	:= substring(strStartTime, nTempPos, 1);
						if not ((strChar >= '0') and (strChar <= '9')) then
							if substring(strStartTime, nTargetPos, 1) = 'y' then
								start_year		:= cast(substring(strStartTime, nTargetPos+1, nTempPos-nTargetPos-1) as integer);
							elseif substring(strStartTime, nTargetPos, 1) = 'M' then
								start_month		:= cast(substring(strStartTime, nTargetPos+1, nTempPos-nTargetPos-1) as integer);
							elseif substring(strStartTime, nTargetPos, 1) = 'd' then
								start_day		:= cast(substring(strStartTime, nTargetPos+1, nTempPos-nTargetPos-1) as integer);
							elseif substring(strStartTime, nTargetPos, 1) = 'h' then
								start_hour		:= cast(substring(strStartTime, nTargetPos+1, nTempPos-nTargetPos-1) as integer);
							elseif substring(strStartTime, nTargetPos, 1) = 'm' then
								start_minute	:= cast(substring(strStartTime, nTargetPos+1, nTempPos-nTargetPos-1) as integer);
							elseif substring(strStartTime, nTargetPos, 1) = 't' then
								nWeekDay	:= cast(substring(strStartTime, nTargetPos+1, nTempPos-nTargetPos-1) as integer);
								if nWeekDay >= 7 then
									day_of_week	:= day_of_week | (1<<(nWeekDay-7));
								else 
									day_of_week	:= day_of_week | (1<<nWeekDay);
								end if;
							elseif substring(strStartTime, nTargetPos, 1) = 'f' then
								nWeek		:= cast(substring(strStartTime, nTargetPos+1, 1) as integer);
								nWeekDay	:= cast(substring(strStartTime, nTargetPos+2, 1) as integer);
								day_of_week	:= day_of_week | (2 << 7);
								day_of_week	:= day_of_week | (1 << (nWeek+8));
								if nWeekDay >= 7 then
									day_of_week	:= day_of_week | (1<<(nWeekDay-7));
								else 
									day_of_week	:= day_of_week | (1<<nWeekDay);
								end if;
							elseif substring(strStartTime, nTargetPos, 1) = 'l' then
								nWeek		:= cast(substring(strStartTime, nTargetPos+1, 1) as integer);
								nWeekDay	:= cast(substring(strStartTime, nTargetPos+2, 1) as integer);
								day_of_week	:= day_of_week | (2 << 7);
								day_of_week	:= day_of_week | (1 << (6-nWeek+8));
								if nWeekDay >= 7 then
									day_of_week	:= day_of_week | (1<<(nWeekDay-7));
								else 
									day_of_week	:= day_of_week | (1<<nWeekDay);
								end if;
							elseif substring(strStartTime, nTargetPos, 2) = 'z4' then
								day_of_week		:= day_of_week | 127;
								day_of_week		:= day_of_week | (1<<7);
								day_of_week		:= day_of_week | (1<<9);
							elseif substring(strStartTime, nTargetPos, 2) = 'z1' then
								--
							elseif substring(strStartTime, nTargetPos, 2) = 'z2' then
								--
							elseif substring(strStartTime, nTargetPos, 3) = 'z17' then
								--
							elseif substring(strStartTime, nTargetPos, 3) = 'z25' then
								--
							elseif substring(strStartTime, nTargetPos, 3) = 'z26' then
								--
							else
								raise EXCEPTION 'unhandled timedom type2, timedom = %', rec.timedom;
							end if;
							nTargetPos	:= nTempPos;
						end if;
						nTempPos	:= nTempPos + 1;
			    	end loop;
			    	
			    	if start_year != 0 and start_month = 0 then
			    		start_month	:= 1;
			    	end if;
			    	if start_month != 0 and start_day = 0 then
			    		start_day	:= 1;
			    	end if;
			    	
			    	-- last time
			    	if strpos(strSubTime,'){') > 0 then
			    		strTemp			:= substring(strSubTime, strpos(strSubTime,'){')+2);
			    		strLastTime		:= substring(strTemp, 1, strpos(strTemp,'}')-1);
			    	end if;
					nTargetPos	:= 1;
					nTempPos	:= 2;
					while nTempPos <= length(strLastTime)+1 loop
						strChar	:= substring(strLastTime, nTempPos, 1);
						if not ((strChar >= '0') and (strChar <= '9')) then
							if substring(strLastTime, nTargetPos, 1) = 'y' then
								end_year	:= cast(substring(strLastTime, nTargetPos+1, nTempPos-nTargetPos-1) as integer);
							elseif substring(strLastTime, nTargetPos, 1) = 'M' then
								end_month	:= cast(substring(strLastTime, nTargetPos+1, nTempPos-nTargetPos-1) as integer);
							elseif substring(strLastTime, nTargetPos, 1) = 'd' then
								end_day		:= cast(substring(strLastTime, nTargetPos+1, nTempPos-nTargetPos-1) as integer);
							elseif substring(strLastTime, nTargetPos, 1) = 'h' then
								end_hour	:= cast(substring(strLastTime, nTargetPos+1, nTempPos-nTargetPos-1) as integer);
							elseif substring(strLastTime, nTargetPos, 1) = 'm' then
								end_minute	:= cast(substring(strLastTime, nTargetPos+1, nTempPos-nTargetPos-1) as integer);
							elseif substring(strLastTime, nTargetPos, 3) = 'z54' then
								--
							elseif substring(strLastTime, nTargetPos, 3) = 'z51' then
								--
							elseif substring(strLastTime, nTargetPos, 3) = 'z52' then
								--
							elseif substring(strLastTime, nTargetPos, 3) = 'z67' then
								--
							elseif substring(strLastTime, nTargetPos, 3) = 'z75' then
								--
							elseif substring(strLastTime, nTargetPos, 3) = 'z76' then
								--
							else
								raise EXCEPTION 'unhandled timedom type2, timedom = %', rec.timedom;
							end if;
							nTargetPos	:= nTempPos;
						end if;
						nTempPos	:= nTempPos + 1;
			    	end loop;
		    	end loop;	-- end subdatetime
		    	
		    	-- calc end time
				end_year		:= start_year + end_year;
				end_month		:= start_month + end_month;
				end_day			:= start_day + end_day;
				end_hour		:= start_hour + end_hour;
				end_minute		:= start_minute + end_minute;
			    
				-- day_of_week = 127 means that from mon to sun have regulation, then day_of_week set 0
				if day_of_week = 127 then
					day_of_week := 0;
				end if;
				
				-- insert result
		    	cur_cond_id := cur_cond_id + 1;
		    	perform mid_insert_condtion_record(	cur_cond_id, 
				    								rec.car_type, 
				    								rec.timedom,
				    								ARRAY[
				    								start_year, start_month, start_day, start_hour, start_minute,
				    								end_year, end_month, end_day, end_hour, end_minute],
				    								day_of_week,
				    								exclude_date);
		    else
		    	raise EXCEPTION 'datetime type error, timedom = %', rec.timedom;
	    	end if;	-- end datetime type
    	end loop;	-- end datetime
    	
    END LOOP;
    return 1;
END;
$$;


CREATE OR REPLACE FUNCTION mid_convert_regulation_oneway_link()
    RETURNS smallint
    LANGUAGE plpgsql
AS $$
DECLARE
	rec record;
	cur_regulation_id integer;
	nIndex integer;
	nCount integer;
	cond_type integer;
BEGIN
	-- regulation_id
	select (case when max(regulation_id) is null then 0 else max(regulation_id) end)
	from regulation_relation_tbl
	into cur_regulation_id;
	
    FOR rec IN
    
    	select linkid, dir_pos, array_agg(cond_id) as array_condid
    	from
    	(
			select 	linkid, dir_pos, cond_id
			from
			(
				select	id as linkid, dir_pos, timedom, mid_get_cartype(vt_array) as car_type
				from temp_rs_df_todo
			)as r
			left join temp_condition_regulation_tbl as t
			on (r.car_type = t.car_type) and 
			   ((r.timedom = t.timedom) or (r.timedom is null and t.timedom is null))
			where t.cond_id is not null
			order by linkid, dir_pos, cond_id
    	)as t
    	group by linkid, dir_pos
    	order by linkid, dir_pos
    
    LOOP
		-- current regulation id
    	cur_regulation_id := cur_regulation_id + 1;
    	
    	-- insert into regulation_item_tbl
    	insert into regulation_item_tbl
    				("regulation_id", "linkid", "nodeid", "seq_num")
    		VALUES 	(cur_regulation_id, rec.linkid, null, 1);
		
		-- cond type
		if rec.dir_pos = 1 then
			cond_type	:= 4;
		elseif rec.dir_pos = 2 then
			cond_type	:= 42;
		elseif rec.dir_pos = 3 then
			cond_type	:= 43;
		else
			raise EXCEPTION 'regulation error: unknown dir_pos of df';
		end if;
		
		-- insert into regulation_relation_tbl
		nCount := array_upper(rec.array_condid, 1);
		nIndex := 1;
		while nIndex <= nCount LOOP
    		insert into regulation_relation_tbl
    					("regulation_id", "nodeid", "inlinkid", "outlinkid", "condtype", "cond_id")
    			VALUES 	(cur_regulation_id, null, rec.linkid, null, cond_type, rec.array_condid[nIndex]);
    		nIndex := nIndex + 1;
    	END LOOP;
    END LOOP;
    return 1;
END;
$$;


CREATE OR REPLACE FUNCTION mid_convert_regulation_linkrow()
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
    
		select	array_condid,
				linkid_array,
				jnctid
		from
		(
    		select condition_id, jnctid, array_agg(cond_id) as array_condid
    		from
    		(
				select 	id as condition_id, jnctid, cond_id
				from
				(
					select a.id, b.jnctid, a.dir_pos, a.timedom, mid_get_cartype(a.vt_array) as car_type
					from temp_rs_linkrow_todo as a
					left join org_mn as b
					on a.id = b.id
				)as r
				left join temp_condition_regulation_tbl as t
				on (r.car_type = t.car_type) and 
				   ((r.timedom = t.timedom) or (r.timedom is null and t.timedom is null))
				order by condition_id, jnctid, cond_id
			)as c
			group by condition_id, jnctid
		)as cc
		left join
		(
			select	condition_id,
					array_agg(link_id) as linkid_array
			from
			(
				select	id as condition_id, 
						seqnr as seq_num, 
						trpelid as link_id
				from
				(
					select distinct id, seqnr, trpelid
					from org_mp
				)as a
				order by id asc, seqnr asc
			)as nl
			group by condition_id
		)as cl
		on cc.condition_id = cl.condition_id
		order by jnctid, linkid_array
		
    LOOP
		-- current regulation id
    	cur_regulation_id := cur_regulation_id + 1;
		
    	-- insert into regulation_item_tbl
		nCount := array_upper(rec.linkid_array, 1);
		nIndex := 1;
		while nIndex <= nCount LOOP
			if nIndex = 1 then
	    		insert into regulation_item_tbl
	    					("regulation_id", "linkid", "nodeid", "seq_num")
	    			VALUES	(cur_regulation_id, rec.linkid_array[nIndex], null, nIndex);
	    		insert into regulation_item_tbl
	    					("regulation_id", "linkid", "nodeid", "seq_num")
	    			VALUES	(cur_regulation_id, null, rec.jnctid, nIndex+1);
	    	else
	    		insert into regulation_item_tbl
	    					("regulation_id", "linkid", "nodeid", "seq_num")
	    			VALUES	(cur_regulation_id, rec.linkid_array[nIndex], null, nIndex+1);
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
	    						 rec.jnctid, rec.linkid_array[1], rec.linkid_array[array_upper(rec.linkid_array,1)], 
	    						 1, rec.array_condid[nIndex]);
			nIndex := nIndex + 1;
		END LOOP;    	
    	
    END LOOP;
    return 1;
END;
$$;
CREATE OR REPLACE FUNCTION linkdir_str_in(stroneway character varying)
	RETURNS smallint
	LANGUAGE plpgsql
as $$
BEGIN
	if stroneway = 'FT' then
		return 2;
	elsif stroneway = 'TF' then
		return 3;
	elsif stroneway = 'N' then
		return 0;
	else
		return 1;
	end if;
END;
$$;

CREATE OR REPLACE FUNCTION Get_new_dir(oneway smallint,linkdir_array smallint[])
	RETURNS smallint
	LANGUAGE plpgsql
as $$
DECLARE
	ioneway smallint;
BEGIN
	if oneway = 1 then
		return 1;
	elsif oneway = 2 then
		if 3 = any(linkdir_array) then
			return 1;
		else
			return 2;
		end if;
	elsif oneway = 3 then
		if 2 = any(linkdir_array) then
			return 1;
		else
			return 3;
		end if;
	elsif oneway = 0 then
		if 2 = any(linkdir_array) and  3 = any(linkdir_array) then
			return 1;
		elsif 2 = any(linkdir_array) then
			return 2;
		elsif 3 = any(linkdir_array) then
			return 3;
		else
			return 0;
		end if;
	end if;
END;
$$;
CREATE OR REPLACE FUNCTION mid_delete_item_relation_table(linkdir smallint,F_T integer,re_id integer)
	RETURNS boolean
	LANGUAGE plpgsql
AS $$
BEGIN
	if ((F_T = 2) and (linkdir in (0,3))) or ((F_T = 3) and (linkdir in (0,2))) then
			delete from regulation_item_tbl where regulation_id = re_id;
			delete from regulation_relation_tbl where regulation_id = re_id;
	end if;
	return true;
END;
$$;
CREATE OR REPLACE FUNCTION mid_get_seq_numidx(allidx smallint,idx smallint,idx_arry smallint[])
	RETURNS smallint
	LANGUAGE plpgsql
AS $$
DECLARE
	iNumidx smallint;
BEGIN
	for iNumidx in 1..allidx loop
		if idx_arry[iNumidx] = idx then
			return  iNumidx;
		end if;
	end loop;
END;
$$;
CREATE OR REPLACE FUNCTION mid_delete_linkrow_from_link()
	RETURNS boolean
	LANGUAGE plpgsql
AS $$
DECLARE
	rec record;
	ilinknum smallint;
	outJClinkidx smallint;
	outlinkidx smallint;
BEGIN
	for rec in
	(
		select a.regulation_id,
					 array_agg(a.nodeid) as node_array,
					 array_agg(a.linkid) as linkid_array,
					 array_agg(a.seq_num) as seqidx_array,
					 array_agg(b.f_jnctid) as Fid_array,
					 array_agg(b.t_jnctid) as Tid_array,
					 array_agg(Get_new_dir(linkdir_str_in(b.oneway),c.linkdir_array)) as linkdir_array 
					 from
		(select * from regulation_item_tbl order by seq_num) as a
		left join org_nw as b
		on a.linkid = b.id
		left join temp_link_regulation_permit_traffic as c
		on a.linkid = c.link_id
		group by a.regulation_id
	)
	LOOP
		ilinknum := 0;
		outlinkidx := 0;
		outJClinkidx := 0;

		ilinknum := array_upper(rec.linkid_array,1);
		outlinkidx := mid_get_seq_numidx(ilinknum,ilinknum,rec.seqidx_array);
		if ilinknum = 1 then
			continue;
		elsif ilinknum = 2 then
			raise EXCEPTION 'ilinknum = 2,regulation_item_tbl error, regulation_id = %', rec.regulation_id;
		elsif ilinknum = 3 then
			if rec.Fid_array[outlinkidx] = any(rec.node_array) then
				perform mid_delete_item_relation_table(rec.linkdir_array[outlinkidx],2,rec.regulation_id);
			elsif rec.Tid_array[outlinkidx] = any(rec.node_array) then
				perform mid_delete_item_relation_table(rec.linkdir_array[outlinkidx],3,rec.regulation_id);
			else
				raise EXCEPTION 'outlink node no find,regulation_item_tbl error, regulation_id = %', rec.regulation_id;
			end if;
		elsif	ilinknum > 3 then
			outJClinkidx := mid_get_seq_numidx(ilinknum,(ilinknum - 1)::smallint,rec.seqidx_array);
			if rec.Fid_array[outlinkidx] in (rec.Fid_array[outJClinkidx], rec.Tid_array[outJClinkidx]) then
				perform mid_delete_item_relation_table(rec.linkdir_array[outlinkidx],2,rec.regulation_id);
			elsif rec.Tid_array[outlinkidx] in (rec.Fid_array[outJClinkidx], rec.Tid_array[outJClinkidx]) then
				perform mid_delete_item_relation_table(rec.linkdir_array[outlinkidx],3,rec.regulation_id);
			else
				raise EXCEPTION 'outlink node no find,regulation_item_tbl error, regulation_id = %', rec.regulation_id;
			end if;
		else
			raise EXCEPTION 'regulation_item_tbl error, regulation_id = %', rec.regulation_id;
		end if;
	
	END LOOP;
	RETURN TRUE;
END;
$$;
-------------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------------

CREATE OR REPLACE FUNCTION mid_get_lanecount_downflow(oneway smallint, lanes smallint)
RETURNS smallint 
LANGUAGE plpgsql
AS
$$
BEGIN
	IF lanes is null THEN
		return 0;
	END IF;
	RETURN CASE
		WHEN oneway in (1,2,4) THEN lanes
		ELSE 0
	END;
END;
$$;

CREATE OR REPLACE FUNCTION mid_get_lanecount_upflow(oneway smallint, lanes smallint)
RETURNS smallint 
LANGUAGE plpgsql
AS
$$
BEGIN
	IF lanes is null THEN
		return 0;
	END IF;
	RETURN CASE
		WHEN oneway in (1,3,4) THEN lanes
		ELSE 0
	END;
END;
$$;

CREATE OR REPLACE FUNCTION mid_cnv_function_class( 
net2class smallint
)
  RETURNS smallint 
  LANGUAGE plpgsql
  AS
$$
BEGIN

    IF net2class in (0,1) THEN
    	return 1;
    ELSEIF net2class = 2 THEN
    	return 2;
    ELSEIF net2class = 3 THEN
    	return 3;
    ELSEIF net2class in (4,5) THEN
    	return 4;
    ELSE
    	return 5;
    END IF;
    
END;
$$;

CREATE OR REPLACE FUNCTION mid_cnv_road_type(
freeway smallint,
frc smallint,
ft smallint,
fow smallint,
privaterd smallint,
backrd smallint,
procstat smallint,
carriage character varying,
nthrutraf smallint,
sliprd smallint,
stubble smallint
)
RETURNS smallint
LANGUAGE plpgsql
AS
$$

BEGIN

    IF ft = 1 THEN 
    	return 10;
    ELSEIF ft = 2 THEN
    	return 11;
    ELSEIF procstat in (8,9) or stubble = 1 or backrd in (1, 2, 3, 4) THEN
    	return 9;
    ELSEIF fow in (14, 15, 18, 19) THEN
    	return 8;
    ELSEIF nthrutraf = 1 THEN
        return 14;
    ELSEIF fow = 20 THEN
    	return 12;
    ELSEIF carriage = '1' THEN
    	return 13;
    ELSEIF privaterd != 0 THEN
        return 7;
    ELSEIF freeway = 1 or fow = 1 THEN
	    return 0;
    ELSEIF frc in (0, 1, 2) THEN
        return 2;
    ELSEIF frc = 3 THEN 
    	return 3;
    ELSEIF frc in (4, 5) THEN
    	return 4;
    ---ELSEIF sliprd = 1 THEN
    ---	return 5;         -- when the frc is higher, we don't set it to frontage road
    ELSEIF frc in (6,7) THEN
    	return 6;
    ELSE
    	return 6;
    END if;
END;
$$;

CREATE OR REPLACE FUNCTION mid_cnv_link_type( 
fow smallint, ramp smallint, pj smallint, sliprd smallint, frc smallint
)
  RETURNS smallint 
  LANGUAGE plpgsql
  AS
$$
BEGIN

	IF pj = 1 THEN
    	return 4;
    ELSEIF pj = 3 THEN
    	return 8;
    ELSEIF pj = 2 THEN
    	return 9;
    ELSEIF (fow = 12 or fow = 6 or fow = 7) THEN
    	return 7;
    ELSEIF fow = 4 THEN
    	return 0;
    ELSEIF (fow = 10 or (ramp != 0 and frc < 3)) THEN
    	return 5;
    ELSEIF fow = 11 THEN
    	return 6;
    ELSEIF fow in (1, 2) THEN
    	return 2;
    ELSE
    	return 1;
    END IF;
    
END;
$$;

CREATE OR REPLACE FUNCTION mid_get_width(oneway smallint, roadtype smallint, dir boolean)
  RETURNS smallint
  LANGUAGE plpgsql
  AS
$$
DECLARE
	
BEGIN
	
	if dir = true then
		if oneway = 3 then
			return 0;
		end if;
	else
		if oneway = 2 then
			return 0;
		end if;
	end if;
	
	if roadtype = 2 then
		return 3;
	elseif roadtype in (0,1,3,13) then
		return 2;
	else
		return 1;
	end if;
	
END;
$$;


CREATE OR REPLACE FUNCTION mid_getregulationspeed
(
oneway smallint,
unit   smallint,
speed_limit integer,
speedclass integer,
dir boolean
)
  RETURNS double precision
  LANGUAGE plpgsql VOLATILE
  AS $$
DECLARE
	
BEGIN
	
	IF (oneway = 2 and dir = false) 
			or (oneway = 3 and dir = true)
			or oneway = 4 THEN
		return 0;
	END IF;

	IF speed_limit is not null and speed_limit > 0 and speed_limit < 200 THEN
		return speed_limit;
	END IF;
		
	IF unit = 2 THEN 
		RETURN CASE WHEN speedclass=1 THEN 99
			WHEN speedclass=2 THEN 80
			WHEN speedclass=3 THEN 62
			WHEN speedclass=4 THEN 55
			WHEN speedclass=5 THEN 43
			WHEN speedclass=6 THEN 31
			WHEN speedclass=7 THEN 18
			WHEN speedclass=8 THEN 6
			ELSE 0 
		END;
	ELSE 
		RETURN CASE WHEN speedclass=1 THEN 160
			WHEN speedclass=2 THEN 130
			WHEN speedclass=3 THEN 100
			WHEN speedclass=4 THEN 90
			WHEN speedclass=5 THEN 70
			WHEN speedclass=6 THEN 50
			WHEN speedclass=7 THEN 30
			WHEN speedclass=8 THEN 11
			ELSE 0 
		END;
	END IF;
	

END;
$$;

CREATE OR REPLACE FUNCTION mid_get_oneway_code( travel_direction character varying, constatus character varying, dir_f smallint, dir_p smallint[] )
RETURNS smallint 
LANGUAGE plpgsql VOLATILE
AS
$$
DECLARE
	ret smallint;
BEGIN
    -- outside 1:both,  2: ft,  3: tf    4: close
    -- inside  1:  ft,  2: tf,  3: both  0: close
    ret = 0;
    -- step 1
    CASE travel_direction
	    WHEN 'FT' THEN ret = 1; 
        WHEN 'TF' THEN ret = 2;
        WHEN 'N'  THEN ret = 0;
        ELSE ret = 3 ;
    END CASE;
	-- step 2
	IF 2 = any(dir_p) THEN
		ret = ret|1;
	END IF;
	IF 3 = any(dir_p) THEN
		ret = ret|2;
	END IF;

	-- step 3
	CASE constatus
	    WHEN 'FT' THEN ret = ret&2;
        WHEN 'TF' THEN ret = ret&1;
        WHEN 'N'  THEN ret = ret&0;
        ELSE ret = ret;
    END CASE;
    
    -- step 4
    CASE dir_f
	    WHEN 2 THEN ret = ret&2;
        WHEN 3 THEN ret = ret&1;
        WHEN 1 THEN ret = ret&0;
        ELSE ret = ret;
    END CASE;
    
    -- return the value
	RETURN CASE ret
	    WHEN 1 THEN 2
	    WHEN 2 THEN 3
	    WHEN 3 THEN 1
	    ELSE 4
	END;
END;
$$;

CREATE OR REPLACE FUNCTION mid_get_pos_dir()
RETURNS smallint 
LANGUAGE plpgsql VOLATILE
AS
$$
DECLARE
	cur refcursor;
	rec record;
	cdir smallint;
	cid double precision;
BEGIN
	
	open cur for
		select distinct id, dir_pos from temp_rs_df_with_condition 
        	where dir_pos is not null
        	order by id;
        	
    fetch cur into rec;
    cid := -1;
    while rec.id is not null loop
    
    	if rec.id != cid then
    		if cid != -1 then
    			insert into temp_dir_pos(id, dir_pos) values (cid, cdir);
    		end if;
    		cid := rec.id;
    		cdir := rec.dir_pos;
    	elseif cdir = 1 
    			or rec.dir_pos = 1 
    			or (cdir = 2 and rec.dir_pos = 3) 
    			or (cdir = 3 and rec.dir_pos = 2) then
    		cdir := 1;
    	end if;
    	
    	fetch cur into rec;
    	
    end loop;
    
    insert into temp_dir_pos(id, dir_pos) values (cid, cdir);
    
    return 1;
	
END;
$$;

CREATE OR REPLACE FUNCTION mid_cnv_node_kind(jncttyp smallint)
  RETURNS character varying
  LANGUAGE plpgsql VOLATILE
AS $$

BEGIN
	
	IF jncttyp = 3 THEN
		return '1301';
	ELSE
		return null;
	END IF;
	
END;
$$;

CREATE OR REPLACE FUNCTION mid_cnv_disp_class(freeway smallint, frc smallint, feattyp smallint, fow smallint ,roughrd smallint)
  RETURNS smallint
  LANGUAGE plpgsql AS
$$
BEGIN
    IF feattyp in (4130) THEN
        return 14;   --100;
    ELSEIF roughrd = 1 THEN   -- 4-wheel drive
        return 19;         
    ELSEIF  freeway = 1 or fow = 1 THEN   -- Free Way
		return 12;
    ELSEIF frc in (0, 1) THEN   -- HighWay
        return 9; 
    ELSEIF frc = 2 THEN   
        return 8; 
    ELSEIF frc = 3 THEN 
    	return 7;
    ELSEIF frc = 4 THEN 
        return 6;  
    ELSEIF frc = 5 THEN 
        return 5;		
    ELSEIF frc in (6,7) THEN 
        return 4;
    END IF;
    
    return 3;   -- Other
END;
$$;

-------------------------------------------------------------------------------------------------------------
-- split start link's accessible lane
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_get_start_link_accessible_lane(lane_fromto character varying)
 RETURNS integer
    LANGUAGE plpgsql
    AS  
$$ 
DECLARE
BEGIN
    IF lane_fromto is null THEN
	return -1;
    END IF;

    return (substring(lane_fromto, 1, position('/' in lane_fromto) - 1))::INT;
END;
$$;

-------------------------------------------------------------------------------------------------------------
-- 
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_get_arrow_map_lane_no(validity varchar)
  RETURNS integer[] 
  LANGUAGE plpgsql 
  AS 
$$
DECLARE
        lane_num int;
        pos int;
	array_lane_no_for_arrow integer[];
BEGIN
	pos := 2;
	lane_num := length(validity);
	while pos <= lane_num loop
		if substring(validity, pos, 1) = '1' then
			--array_lane_no_for_arrow[pos-1] := pos -1;
			array_lane_no_for_arrow := array_append(array_lane_no_for_arrow, pos-1);
		end if;

		pos := pos + 1;
	end loop; 

	return array_lane_no_for_arrow;
END;
$$;
-------------------------------------------------------------------------------------------------------------
-- towardname path links(in_link_id, out_link_id, pass links, pass link count)
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_make_towardname_path_links()
 RETURNS integer
    LANGUAGE plpgsql
    AS  
$$ 
DECLARE
    curs1 refcursor;
    cur_rec record;
    pre_rec record;
    in_link_id bigint;
    out_link_id bigint;
    passlid character varying(1024);
    passlink_cnt smallint;
    cnt integer;
    
BEGIN
	in_link_id   := NULL;
	out_link_id  := NULL;
	passlid      := NULL;
	passlink_cnt := 0;
	cnt          := 0;
	OPEN curs1 FOR SELECT id, seqnr, trpelid as link_id
			  FROM sp
			  order by id, seqnr;

	-- first record  
	FETCH curs1 INTO pre_rec;
	if pre_rec is not null then
		in_link_id  := pre_rec.link_id;
	else
		return 0;
	end if;
	
	FETCH curs1 INTO cur_rec;
	while cur_rec is not null LOOP
		if pre_rec.id = cur_rec.id then
			if cur_rec.seqnr > 2 then
				if passlid is null then
					passlid      := pre_rec.link_id::character varying;
					passlink_cnt := 1;
				else
					passlid      := passlid || '|' || pre_rec.link_id::character varying;
					passlink_cnt := passlink_cnt + 1;
				end if;
			end if;
		
		elsif pre_rec.id <> cur_rec.id then
			out_link_id := pre_rec.link_id;
			
			--insert into
			INSERT INTO temp_towardname_path_links("id", "in_link_id", "out_link_id", "passlid", "passlink_cnt")
				values(pre_rec.id, in_link_id, out_link_id, passlid, passlink_cnt);
			cnt          := cnt + 1;	
			passlid      := NULL;
			passlink_cnt := 0;
			in_link_id   := cur_rec.link_id;
			out_link_id  := NULL;
		end if;
		
		pre_rec := cur_rec;
		-- Get the next record
		FETCH curs1 INTO cur_rec;
	END LOOP;
	close curs1;

	--insert into [the last record]
	out_link_id := pre_rec.link_id;
	INSERT INTO temp_towardname_path_links("id", "in_link_id", "out_link_id", "passlid", "passlink_cnt")
		values(pre_rec.id, in_link_id, out_link_id, passlid, passlink_cnt);
	cnt          := cnt + 1;

    return cnt;
END;
$$;

CREATE OR REPLACE FUNCTION mid_make_laneinfo( get_arrow_map_lane_no integer[], total_lane_num integer, validity character varying, convert_flag boolean)
  RETURNS character varying 
  LANGUAGE plpgsql 
  AS 
$$
DECLARE
        lane_num int;
        pos int;
	rtn_access_lane character varying;
BEGIN
	lane_num := length(validity) - 1;

	if lane_num != total_lane_num then
		raise exception 'lane num is not matching';
	end if;

	if convert_flag = False then
		pos := total_lane_num;
		rtn_access_lane := '';

		while pos > 0 loop
			if (ARRAY[pos] <@ get_arrow_map_lane_no) = TRUE then
				rtn_access_lane :=  rtn_access_lane || '1';
			else
				rtn_access_lane := rtn_access_lane  || '0';
			end if;
	
			pos := pos -1;
		end loop;
	else
		pos := 1;
		rtn_access_lane := '';

		while pos <= total_lane_num loop
			if (ARRAY[pos] <@ get_arrow_map_lane_no) = TRUE then
				rtn_access_lane :=  rtn_access_lane || '1';
			else
				rtn_access_lane := rtn_access_lane  || '0';
			end if;
	
			pos := pos + 1;
		end loop;
	end if;

	return rtn_access_lane;
END;
$$;

CREATE OR REPLACE FUNCTION mid_make_arrowinfo(direction smallint)
  RETURNS smallint 
  LANGUAGE plpgsql 
  AS 
$$
DECLARE
	temp_mid_value smallint;
	rtn_value smallint;
BEGIN
	-- get arrow except bit 8(U-Turn Right)
	temp_mid_value :=  direction & (255::smallint);
	
	rtn_value := ( direction >> 8) << 11 | temp_mid_value ;

	return rtn_value;
END;
$$;


CREATE OR REPLACE FUNCTION mid_get_all_links_array(pass_node_link_list varchar[])
  RETURNS varchar[]
  LANGUAGE plpgsql 
  AS 
$$
DECLARE
	list_cnt int;
	pos int;
	rtn_value varchar[];
BEGIN
	pos := 1;
	list_cnt := array_length(pass_node_link_list, 1);

        while pos <= list_cnt loop
		if pos % 2 = 0 then
 			rtn_value := array_append(rtn_value,pass_node_link_list[pos]);
		end if;

		pos := pos + 1;
	end loop;

	return rtn_value;
END;
$$;

-------------------------------------------------------------------------------------------------------------
-- Is multi shiled number.
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_is_multi_shieldnum(shieldnum character varying, special_chars character varying)
 RETURNS Boolean
    LANGUAGE plpgsql
    AS  
$$ 
DECLARE
	i integer;
	sp_pos integer;
	sp_ch character varying;
	prev_ch character varying;
	next_ch character varying;
BEGIN
	i := 1;
	
	if shieldnum is null then 
		return false;
	end if;
	
	if special_chars is null then 
		return false;
	end if;

	while i <= length(special_chars) loop
		sp_ch  := substr(special_chars, i, 1);
		sp_pos := strpos(shieldnum, sp_ch);
		if sp_pos > 1 then 
			-- like ['60 70'], ['17/92']
			prev_ch := substr(shieldnum, sp_pos - 1, 1);
			next_ch := substr(shieldnum, sp_pos + 1, 1);
			if prev_ch >= E'0' and prev_ch <= E'9' and next_ch >= E'0' and next_ch <= E'9' then
				return TRUE;
			end if;
		end if;
		i := i + 1;
	end loop;
	return false;
END;
$$;

-------------------------------------------------------------------------------------------------------------
-- has sub_shield_num
-- when the number of sub_shield_num more than 2, returns TRUE;
-- else returns FALSE.
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION  mid_has_multi_sub_shield_num(link_id bigint, multi_shieldnum character varying)
 RETURNS boolean
    LANGUAGE plpgsql
    AS  
$$ 
DECLARE
	cnt integer;
begin
	if link_id is null or multi_shieldnum is null then
		return false;
	end if;
	
	SELECT count(id) into cnt
	  FROM rn
	  where id = link_id and shieldnum <> multi_shieldnum and strpos(multi_shieldnum, shieldnum) > 0;

	if cnt is null then 
		return false;
	end if;

	if cnt >= 2 then 
		return TRUE;
	end if;

	return false;
end;
$$;

-------------------------------------------------------------------------------------------------------------
-- Makes route rumber, shield and shield number.
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_make_route_number()
 RETURNS integer
    LANGUAGE plpgsql
    AS  
$$ 
DECLARE
    curs1 refcursor;
    rec record;
    new_name_prior integer;
    second_prior integer;
    shield_seqnm integer;
    prev_id bigint;
BEGIN
	prev_id := 0;
	OPEN curs1 FOR SELECT id, 
			       rtetyp as shield_id, 
			       shieldnum, 
			       routenum, 
			       rteprior,
			       (routenam is not null or extrtnam is not null) as route_name_flg,
			       mid_is_multi_shieldnum(shieldnum, E'-/ ') as multi_num_flg
			  FROM rn
			  order by id, rteprior, octet_length(routenum), routenum, shield_id
			  ;

	-- Get the first record        
	FETCH curs1 INTO rec;
	while rec.id is not null LOOP
		if rec.id <> prev_id then
			second_prior := 1;
			shield_seqnm := 1;
			prev_id      := rec.id;
		end if;
		
		-- multi shield numbers: [17/92], [60 70]
		if rec.multi_num_flg = true then
			if  mid_has_multi_sub_shield_num(rec.id::bigint, rec.shieldnum) = true then 
				FETCH curs1 INTO rec;
				continue;
			end if;
		end if;
		
		-- Get the name prior for route_num
		new_name_prior := mid_get_new_name_prior_number(rec.rteprior, rec.shieldnum, rec.route_name_flg);

		-- route number
		INSERT INTO temp_mid_link_route_number(
			    "link_id", "route_num", "lang_type", "rteprior", "new_name_prior", "second_prior")
		    VALUES (rec.id, rec.routenum, 'ENG', rec.rteprior, new_name_prior, second_prior);

		second_prior := second_prior + 1;

		-- shield and shield number
		-- legth of shield number must be <= 5
		-- not Multi shield number([60 70])
		if rec.multi_num_flg = FALSE and 
		   new_name_prior <> 31 and 
		   new_name_prior <> 81 and
		   length(rec.shieldnum) <= 5 then
			INSERT INTO temp_mid_link_shield(
				    link_id, "shield_id", shield_num, route_num, "rteprior", seqnm)
			    VALUES (rec.id, rec.shield_id, rec.shieldnum, rec.routenum, rec.rteprior, shield_seqnm);

			shield_seqnm := shield_seqnm + 1;
		end if;
		
		-- Get the next record
		FETCH curs1 INTO rec;
	END LOOP;
	close curs1;
	
	return 0;
END;
$$;

-------------------------------------------------------------------------------------------------------------
-- get new name prior for Street Name
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_get_new_name_prior_for_name(nametyp smallint)
 RETURNS smallint
    LANGUAGE plpgsql
    AS  
$$ 
DECLARE
BEGIN	
	-- Link Name
	RETURN
	case  
		WHEN nametyp = 1  then 11  -- officer name
		WHEN nametyp = 17 then 21 -- Street Name(officer)
		when nametyp = 65 then 41 -- Postal Street Name(officer)
		when nametyp = 66 then 42 -- Postal Street Name(Alternate)
		when nametyp = 33 then 51 -- Locality Name (officer)
		when nametyp = 34 then 52 -- Locality Name (Alternate)
		when nametyp = 18 then 61 -- Street Name (Alternate)
		else 9999
	end;
END;
$$;

-------------------------------------------------------------------------------------------------------------
-- get new name prior for Route Number
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_get_new_name_prior_number(rteprior smallint, shieldnum character varying, route_name_flg boolean)
 RETURNS smallint
    LANGUAGE plpgsql
    AS  
$$ 
DECLARE
	new_name_prior smallint;
BEGIN
	new_name_prior := 9999;

	IF shieldnum is null then
		new_name_prior = 31;
	elseif rteprior = 1 or rteprior = 2 then
		new_name_prior = 1;
	elseif rteprior = 3 and route_name_flg = false then 
		new_name_prior = 1;
	elseif rteprior = 3 and route_name_flg = true then 
		new_name_prior = 71;
	else
		new_name_prior = 81;
	end if;

	return new_name_prior;
END;
$$;

-------------------------------------------------------------------------------------------------------------
--Select Link Name from gc (exclude Route Number)
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_make_link_name_gc()
 RETURNS integer
    LANGUAGE plpgsql
    AS  
$$ 
DECLARE
    curs1 refcursor;
    rec record;
    second_prior integer;
    prev_id bigint;
    cnt integer;
BEGIN
	prev_id := 0;
	cnt     := 0;
	OPEN curs1 FOR SELECT   id, fullname, namelc as lang_type, nametyp, 
				mid_get_new_name_prior_for_name(nametyp) as new_name_prior
			  FROM gc
			  where nametyp & 4 = 0 and nametyp <> 8
			  order by id, new_name_prior, sol, fullname
			  ;

	-- Get the first record        
	FETCH curs1 INTO rec;
	while rec.id is not null LOOP
		if rec.id <> prev_id then
			second_prior := 1;
			prev_id      := rec.id;
		end if;
		
		-- Street Number
		INSERT INTO temp_mid_link_name(
			    "link_id", "name", "lang_type", "new_name_prior", "second_prior")
		    VALUES (rec.id, rec.fullname, rec.lang_type, rec.new_name_prior, second_prior);

		second_prior := second_prior + 1;
		cnt := cnt + 1;
		-- Get the next record
		FETCH curs1 INTO rec;
	END LOOP;
	close curs1;
	
	return cnt;
END;
$$;

-------------------------------------------------------------------------------------------------------------
--Union the Street Name and the Route Number.
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_union_link_name_number()
 RETURNS integer
    LANGUAGE plpgsql
    AS  
$$ 
DECLARE
    curs1 refcursor;
    rec record;
    prev_id bigint;
    seq_num smallint;
    cnt integer;
BEGIN
	prev_id := 0;
	cnt     := 0;
	OPEN curs1 FOR SELECT link_id, name, lang_type, new_name_prior, second_prior
				FROM (
				SELECT link_id, route_num as "name", lang_type, new_name_prior, second_prior
				  FROM temp_mid_link_route_number

				union 

				SELECT link_id, "name", lang_type, new_name_prior, second_prior
				  FROM temp_mid_link_name
				) AS a
				order by link_id, new_name_prior, second_prior
				;

	-- Get the first record        
	FETCH curs1 INTO rec;
	while rec.link_id is not null LOOP
		if rec.link_id <> prev_id then
			seq_num  := 1;
			prev_id  := rec.link_id;
		end if;
		
		-- Street Number
		INSERT INTO temp_mid_link_name_number(
			    "link_id", "name", "lang_type", "new_name_prior", "second_prior", "seq_num")
		    VALUES (rec.link_id, rec.name, rec.lang_type, rec.new_name_prior, rec.second_prior, seq_num);

		seq_num := seq_num + 1;
		cnt     := cnt + 1;
		-- Get the next record
		FETCH curs1 INTO rec;
	END LOOP;
	close curs1;
	
	return cnt;
END;
$$;

-------------------------------------------------------------------------------------------------------------
--Merge the Street Name and the Route Number.
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_merge_name()
 RETURNS integer
    LANGUAGE plpgsql
    AS  
$$ 
DECLARE
    curs1 refcursor;
    rec record;
    prev_id bigint;
    cnt integer;
    merged_name character varying;
    temp_lang_type character varying(3);
BEGIN
	prev_id := 0;
	cnt     := 0;
	merged_name := '';
	OPEN curs1 FOR SELECT link_id, name, lang_type, seq_num
			  FROM temp_mid_link_name_number
			  order by link_id, seq_num
			;

	-- Get the first record        
	FETCH curs1 INTO rec;
	while rec.link_id is not null LOOP
		if rec.link_id <> prev_id then
			if prev_id <> 0 then
				-- Insert the Merged Name and Number into [temp_mid_link_merged_name]
				INSERT INTO temp_mid_link_merged_name(
					    "link_id", "name", "lang_type")
				    VALUES (prev_id, merged_name, temp_lang_type);

				cnt := cnt + 1;
			end if;
			temp_lang_type := rec.lang_type;
			merged_name    := rec.name;
			prev_id        := rec.link_id;
		else
			merged_name := merged_name || E'\t' || rec.name;   -- names are splitted by TAB word.
		end if;

		
		-- Get the next record
		FETCH curs1 INTO rec;
	END LOOP;
	close curs1;

	-- the last
	INSERT INTO temp_mid_link_merged_name(
					  "link_id", "name", "lang_type")
	    VALUES (prev_id, merged_name, temp_lang_type);
	cnt := cnt + 1;
	
	return cnt;
END;
$$;

-------------------------------------------------------------------------------------------------------------
--Make SignPost Exit NO.
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_make_signpost_exit_no()
  RETURNS integer 
  LANGUAGE plpgsql
  AS
$$
DECLARE
	curs1 refcursor;
	curr_name_id integer;
	cnt integer;
	sp_id bigint;
	id_lid bigint[];
	exit_no character varying;
	lang_type character varying;
	i integer;
BEGIN
	curr_name_id := 0;
	cnt          := 0;
	OPEN curs1 FOR select id_array, exit_nos
				   from (
					SELECT array_agg(id) id_array, array_to_string(exit_nos, E'\t') as exit_nos  -- multi exit no split by 'TAB' keyword
					 FROM (
						SELECT id, array_agg(txtcont) as exit_nos
						  FROM (
							select id, seqnr, destseq, txtcont 
							  from si
							  where infotyp = '4E'
							  order by id, seqnr, destseq
						  ) as a
						  group by id
					  ) AS A
					  group by exit_nos
				  ) a 
				  order by exit_nos;

	-- Get the first record        
	FETCH curs1 INTO id_lid, exit_no;
	WHILE id_lid is not null LOOP

		curr_name_id := mid_add_one_name(curr_name_id, exit_no, null, 3);  -- Use English language type
		
		i := 1;
		while i <= array_upper(id_lid, 1) LOOP
			sp_id = id_lid[i];
				
			insert into temp_mid_signpost_exit_no("id", "exit_no")
				values (sp_id, curr_name_id);
			i   := i + 1;
			cnt := cnt + 1;
		END LOOP;
		
		curr_name_id := curr_name_id + 1;
		-- Get the next record
		FETCH curs1 INTO  id_lid, exit_no;
	END LOOP;
	
	return cnt;
END;
$$;

-------------------------------------------------------------------------------------------------------------
-- Get SignPost Name temp priority
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_get_singpost_name_prior(infotyp character varying (2))
  RETURNS smallint 
  LANGUAGE plpgsql
  AS
$$
DECLARE
	
BEGIN
	return case
	when infotyp = '9D' then 1
	when infotyp = '6T' then 1
	when infotyp = '4I' then 3
	else 99
	end;
END;
$$;

-------------------------------------------------------------------------------------------------------------
--Merge SignPost Name(4G,4I, 6T, 9D)
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_merge_signpost_name()
  RETURNS integer 
  LANGUAGE plpgsql
  AS
$$
DECLARE
	curs1 refcursor;
	cnt integer;
	prev_sp_id bigint;
	curr_sp_id bigint;
	signpost_name character varying;
	lang_type character varying;
	merged_sp_name character varying;
	first_lang_type character varying;
BEGIN
	cnt          := 0;
	prev_sp_id   := 0;
	curr_sp_id   := 0;
	merged_sp_name := '';
	
	OPEN curs1 FOR select id, txtcont, txtcontlc
			  from (
				SELECT id, seqnr, destseq, txtcont, txtcontlc
				  FROM si
				  -- Exit Name [4G], Other Destination [4I], Street Name [6T], Place Name [9D]
				  where infotyp in ('4G', '4I', '6T', '9D')  
			  ) as a
			  order by id, seqnr, destseq;

	-- Get the first record
	FETCH curs1 INTO curr_sp_id, signpost_name, lang_type;
	WHILE curr_sp_id is not null LOOP
		IF curr_sp_id <> prev_sp_id then
			if prev_sp_id <> 0 then
				INSERT into temp_mid_merged_signpost_name("id", "signpost_name", "language_type")
					values(prev_sp_id, merged_sp_name, first_lang_type);
				cnt := cnt + 1;
			end if;
			
			merged_sp_name  := signpost_name;
			prev_sp_id      := curr_sp_id;
			first_lang_type := lang_type;
		else 
			merged_sp_name  := merged_sp_name || E'/' || signpost_name;
		end if;
		
		-- Get next record
		FETCH curs1 INTO curr_sp_id, signpost_name, lang_type;
	END LOOP;

	-- store the last record
	INSERT into temp_mid_merged_signpost_name("id", "signpost_name", "language_type")
		values(prev_sp_id, merged_sp_name, first_lang_type);
	cnt := cnt + 1;
	
	return cnt;
END;
$$;

-------------------------------------------------------------------------------------------------------------
--Make Signpost Name dictionary
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_make_signpost_name_dictionary()
  RETURNS integer 
  LANGUAGE plpgsql
  AS
$$
DECLARE
	curs1 refcursor;
	cnt integer;
	prev_sp_id bigint;
	curr_name_id integer;
	temp_signpost_name character varying;
	temp_lang_type character varying;
	language_id smallint;
	i integer;
	sp_id bigint;
	id_lid bigint[];
BEGIN
	cnt          := 0;
	sp_id        := 0;
	curr_name_id := 0;
	
	OPEN curs1 FOR 	
			select id_array, signpost_name, language_type
			from (
				select array_agg(id) id_array, signpost_name, language_type
				  from temp_mid_merged_signpost_name
				  group by signpost_name, language_type
			) a 
			order by signpost_name,language_type;

	-- Get the first record
	FETCH curs1 INTO id_lid, temp_signpost_name, temp_lang_type;
	WHILE id_lid is not null LOOP
		language_id  := mid_get_language_id(temp_lang_type);
		curr_name_id := mid_add_one_name(curr_name_id, temp_signpost_name, null, language_id); 

		i := 1;
		while i <= array_upper(id_lid, 1) LOOP
			sp_id = id_lid[i];
			
			insert into temp_mid_signpost_name("id", "signpost_name")
				values(sp_id, curr_name_id);
			
			cnt := cnt + 1;
			i   := i + 1;
		end LOOP;
		
		curr_name_id := curr_name_id + 1;
		-- Get next record
		FETCH curs1 INTO id_lid, temp_signpost_name, temp_lang_type;
	END LOOP;
	
	return cnt;
END;
$$;

-------------------------------------------------------------------------------------------------------------
--Get SignPost Direction
--1==>N, 2==>E, 3==>S, 4==>W, Others==>NULL
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_get_signpost_direction(inDirection character varying)
  RETURNS character varying 
  LANGUAGE plpgsql
  AS
$$
DECLARE
BEGIN
	return case
	when inDirection = '1' then 'N'
	when inDirection = '2' then 'E'
	when inDirection = '3' then 'S'
	when inDirection = '4' then 'W'
	else NULL
	end;
END;
$$;

-------------------------------------------------------------------------------------------------------------
--Merge SignPost Route NO
--shield_id,shield_num, direction ==> [shield_id,shield_num,direction]
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_merge_signpost_route_no(shield_id character varying, 
														shield_num character varying, 
														direction character varying)
  RETURNS character varying 
  LANGUAGE plpgsql
  AS
$$
DECLARE
	route_no_info character varying;
BEGIN
	route_no_info := '';
	
	IF (shield_id IS NULL) OR (shield_num IS NULL) THEN
		RETURN NULL;
	END IF;

	route_no_info := '[' || shield_id || ',' || shield_num;

	IF direction is not null then
		route_no_info := route_no_info || ',' || direction || ']';
	else
		route_no_info := route_no_info || ']';
	end if;	
	return route_no_info;
END;
$$;

-------------------------------------------------------------------------------------------------------------
--Make Signpost Route NO Dictionary
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_make_signpost_route_no_dictionary()
  RETURNS integer 
  LANGUAGE plpgsql
  AS
$$
DECLARE
	curs1 refcursor;
	cnt integer;
	temp_join_route_no character varying;
	i integer;
	sp_id bigint;
	id_lid bigint[];
	curr_name_id integer;
BEGIN
	curr_name_id := 0;
	cnt          := 0;
	
	OPEN curs1 FOR 	
		select id_array, join_route_no
		  from (
			select array_agg(id) id_array, ('[' || array_to_string(route_no_array, ',') || ']') as join_route_no
			  from (
				SELECT id, array_agg(merged_route_no) as route_no_array
				  FROM (
					SELECT id, merged_route_no
					  from (
						SELECT min(gid) as gid, id, merged_route_no
						  FROM temp_mid_signpost_merged_route_no
						  WHERE merged_route_no is not null
						  group by id, merged_route_no
					  ) as a
					  order by gid
				  ) as a
				  WHERE merged_route_no is not null
				  group by id
			  ) as b
			 group by route_no_array
		  ) a 
		  order by join_route_no;

	-- Get the first record
	FETCH curs1 INTO id_lid, temp_join_route_no;
	WHILE id_lid is not null LOOP
		curr_name_id := mid_add_one_name(curr_name_id, temp_join_route_no, null, 3); 
		i := 1;
		WHILE i <= array_upper(id_lid, 1) LOOP
			sp_id = id_lid[i];
			INSERT INTO temp_mid_signpost_route_no("id", "join_route_no", "route_no")
				values(sp_id, temp_join_route_no, curr_name_id);
			i := i + 1;
		END LOOP;

		cnt          := cnt + i - 1;
		curr_name_id := curr_name_id + 1;
		-- Get next record
		FETCH curs1 INTO id_lid, temp_join_route_no;
	END LOOP;
	
	return cnt;
END;
$$;


CREATE OR REPLACE FUNCTION mid_get_jv_passlink_array( jvid bigint, inlink bigint, nodeid bigint, outlink bigint, OUT linkrow bigint[])
	RETURNS bigint[]
	LANGUAGE plpgsql volatile
AS $$
DECLARE
	cur				refcursor;
	rec				record;
 	tmp_rec 			record;
	reach_link                      bigint;
	reach_node			bigint;
	linkfrc			        smallint;
	i 				smallint;
	tolerance			smallint;
	flag 				smallint;
BEGIN
	reach_link	:= inlink;
	reach_node	:= nodeid;
	
	linkrow		:= array[]::bigint[];
	tolerance := 10;
	i := 0;

	while true loop
		
		open cur for select a.*
		from
		(
			(
				select	id, t_jnctid  as nextnode
				from nw
				where 	( f_jnctid = reach_node ) 
					and (id != reach_link)
					and oneway in ('','FT')
			)
			union
			(
				select	id, f_jnctid as nextnode
				from nw
				where 	( t_jnctid = reach_node ) 
					and (id != reach_link)
					and oneway in ('','TF')
			)
		)as a
		left join temp_jv_link_walked as b
		on b.id = jvid and b.link = a.id
		where b.link is null;
		
		fetch cur into rec;

		i := i + 1;
		
		while rec.id is not null loop
			if rec.id = outlink then
				flag = 1;
				exit;				
			end if;
			
			fetch cur into rec;
		end loop;

		if flag = 1 then
			exit;
		end if;
		
		if FOUND then
		
			if i = 1 then
				insert into temp_jv_link_walked ("id","link") values (jvid,cast(rec.id as bigint));
			end if;

			if i > tolerance then
				reach_link 	:= inlink;
				reach_node 	:= nodeid;
				linkrow		:= array[]::bigint[];
				i 		:= 0;
			else 

				if rec.id = outlink then
					exit;
				else
					linkrow		:= array_append(linkrow, cast(rec.id as bigint));
					reach_link	:= rec.id;
					reach_node	:= rec.nextnode;
				end if;
			end if;
		else
			
			select * from temp_jv_link_walked where id = jvid into tmp_rec;
			if FOUND then
				tolerance 	:= tolerance + 10;
				reach_link 	:= inlink;
				reach_node 	:= nodeid;
				linkrow		:= array[]::bigint[];
				i 		:= 0;
			else
				exit;
			end if;			
			
		end if;

		close cur;
	end loop;
END;
$$;	

-------------------------------------------------------------------------------------------------------------
-- admin
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_Get_VN_UpOrder(country_code_temp character varying, order_code character varying ,leval integer)
	RETURNS integer
    LANGUAGE plpgsql
AS $$
DECLARE
	rce  record;
	order_id integer;

BEGIN
	order_id = 0;
	if leval = 0 then
		for rce in
		(
			select mid_admin_GetNewID(cast(id as bigint),0) as order_id_temp  
			from org_a0 
			where order00 = order_code and order00 = country_code_temp
			group by id
		)
		loop
		order_id = rce.order_id_temp;
		exit;
		end loop;
	elsif leval = 1 then
		for rce in
		(
			select mid_admin_GetNewID(cast(id as bigint),1) as order_id_temp  
			from org_a1 
			where order01 = order_code and order00 = country_code_temp 
			group by id
		)
		loop
		order_id = rce.order_id_temp;
		exit;
		end loop;
	elsif leval = 3 or leval = 5 then
		for rce in
		(
			select mid_admin_GetNewID(cast(id as bigint),leval%4) as order_id_temp  
			from org_a7 
			where order07 = order_code and order00 = country_code_temp 
			group by id
		)
		loop
		order_id = rce.order_id_temp;
		exit;
		end loop;
	end if;

	return order_id;
END;
$$;
------get admin new ID
CREATE OR REPLACE FUNCTION mid_admin_GetNewID(id_old_temp bigint,level_temp integer)
	RETURNS integer
	LANGUAGE plpgsql
AS $$
DECLARE
	rce  record;
	Get_id integer;
BEGIN
	Get_id := 0;
	for rce in
	(
		select New_ID as order_id_temp  
		from temp_adminid_newandold 
		where ID_old = id_old_temp and level = level_temp 
		group by New_ID
	)
	loop
	Get_id = rce.order_id_temp;
	exit;
	end loop;
	
	return Get_id;
END;
$$;

-----alter admin ID
CREATE OR REPLACE FUNCTION mid_admin_SetNewID(id_old bigint,gid integer,level integer)
	RETURNS integer
	LANGUAGE plpgsql
AS $$
DECLARE
BEGIN
	return 100000*cast(cast (id_old / 10000000000 as integer) % 1000 as integer) + level*10000 + gid;
END;
$$;
---insert into ID_New_Old
CREATE OR REPLACE FUNCTION mid_into_ID_NewOLd(country_code_temp character varying)
	RETURNS smallint
	LANGUAGE plpgsql
AS $$
DECLARE
BEGIN

  INSERT INTO temp_adminid_newandold(ID_old,new_id , level)
  (
	  select unnest(id_array), mid_admin_SetNewID(id_array[1]::bigint,1,0),0
		from
		(
			select array_agg(gid) as gid_array, array_agg(id) as id_array
			from
			(
				select gid, id, order00
				from org_a0
				where order00 = country_code_temp
				order by id
			)temp
		)temp1
	);

  ---order1
  if country_code_temp = 'LSO' or country_code_temp = 'SWZ' then
  	INSERT INTO temp_adminid_newandold(ID_old,new_id, level)
		select ID_old, mid_admin_SetNewID(ID_old, (row_number() over(order by ID_old))::integer,1),1
		from
		(
			select id::bigint as ID_old
			from org_a7
			where order00 = country_code_temp  
			group by ID_old
			order by ID_old
		)temp;
  
  else
   
		INSERT INTO temp_adminid_newandold(ID_old,new_id, level)
		select ID_old, mid_admin_SetNewID(ID_old, (row_number() over(order by ID_old))::integer,1),1
		from
		(
			select id::bigint as ID_old
			from org_a1
			where order00 = country_code_temp  
			group by ID_old
			order by ID_old
		)temp;
	end if;
  ---order2
  if country_code_temp <> 'LSO' and country_code_temp <> 'SWZ' then 
	INSERT INTO temp_adminid_newandold(ID_old, new_id, level)
	select ID_old, mid_admin_SetNewID(ID_old, (row_number() over(order by ID_old))::integer,3),3
	from
	(
		select id::bigint as ID_old
		from org_a7
		where order00 = country_code_temp  
		group by ID_old
		order by ID_old
	)temp;
	end if;
  ---order8
  if country_code_temp <> 'VNM' then
	  INSERT INTO temp_adminid_newandold(ID_old, new_id, level)
		select ID_old, mid_admin_SetNewID(ID_old, (row_number() over(order by ID_old))::integer,4),4
		from
		(
			select id::bigint as ID_old
			from org_a8
			where order00 = country_code_temp  
			group by ID_old
			order by ID_old
		)temp;
 	end if;
   
   return 0;
END;
$$;
---creat country table
CREATE OR REPLACE FUNCTION mid_alter_VN_arder0_8(country_code_temp character varying)
	RETURNS smallint
    LANGUAGE plpgsql
AS $$
DECLARE

BEGIN
	
	---order0
	INSERT INTO temp_admin_order0(order0_id, order0_code, name_id, country_code, the_geom)
	(
		select order0_id, order0_code, name_id_array[1],country_code, the_geom
		from
		(
			SELECT  order0_id, 
		          order0_code, 
		          array_agg(name_id) as name_id_array,
		          country_code, 
		          st_union(the_geom) as the_geom
	    FROM
	    (
	        select  mid_admin_GetNewID(cast(id as bigint),0) as order0_id,
	                mid_admin_GetNewID(cast(id as bigint),0) as order0_code, 
	                id as name_id, 
	                order00 as country_code,
	                the_geom
	        from org_a0 
	        where order00 = country_code_temp
	    )as a
	    group by order0_id, order0_code,country_code
	    order by order0_id, order0_code,country_code
		)temp 
	);
    
  ---order1
  if country_code_temp = 'LSO' or country_code_temp = 'SWZ' then
  	INSERT INTO temp_admin_order1(order1_id, order0_id, name_id, country_code,the_geom)
  	SELECT  order1_id, 
          	order0_id, 
          	name_id,
          	country_code, 
          	st_union(the_geom) as the_geom
    FROM
    (
        select  mid_admin_GetNewID(cast(id as bigint),1) as order1_id,
                mid_Get_VN_UpOrder(country_code_temp,order00,0) as order0_id, 
                (case when name is null then null else id end) as name_id,
                order00 as country_code, 
                the_geom
        from org_a7
        where order00 = country_code_temp 
    )as a
    group by order1_id, order0_id, name_id,country_code
    order by order1_id, order0_id, name_id,country_code;  
  else
  	INSERT INTO temp_admin_order1(order1_id, order0_id, name_id, country_code,the_geom)
  	SELECT  order1_id, 
          	order0_id, 
          	name_id,
          	country_code, 
          	st_union(the_geom) as the_geom
    FROM
    (
        select  mid_admin_GetNewID(cast(id as bigint),1) as order1_id,
                mid_Get_VN_UpOrder(country_code_temp,order00,0) as order0_id, 
                (case when name is null then null else id end) as name_id,
                order00 as country_code, 
                the_geom
        from org_a1
        where order00 = country_code_temp 
    )as a
    group by order1_id, order0_id, name_id,country_code
    order by order1_id, order0_id, name_id,country_code;
	end if;
    
  ---order8
  if country_code_temp = 'VNM' then
		INSERT INTO temp_admin_order8(order8_id, order2_id, name_id, country_code,the_geom)
	  SELECT  order8_id, 
	          order2_id, 
	          name_id,
	          country_code,
	          st_union(the_geom) as the_geom
	    FROM
	    (
	        select  mid_admin_GetNewID(cast(id as bigint),3) as order8_id,
	                mid_Get_VN_UpOrder(country_code_temp,order01,1) as order2_id, 
	                (case when name is null then null else id end) as name_id,
	                order00 as country_code, 
	                the_geom
	        from org_a7 
	        where order00 = country_code_temp
	    )as a
	    group by order8_id, order2_id, name_id,country_code
	    order by order8_id, order2_id, name_id,country_code;
	    
	elseif country_code_temp = 'LSO' or country_code_temp = 'SWZ' then
	
		INSERT INTO temp_admin_order8(order8_id, order2_id, name_id, country_code,the_geom)
	  SELECT  order8_id, 
	          order2_id, 
	          name_id,
	          country_code,
	          st_union(the_geom) as the_geom
	    FROM
	    (
	        select  mid_admin_GetNewID(cast(id as bigint),4) as order8_id,
	                mid_Get_VN_UpOrder(country_code_temp,order07,5) as order2_id, 
	                (case when name is null then null else id end) as name_id,
	                order00 as country_code, 
	                the_geom
	        from org_a8 
	        where order00 = country_code_temp
	    )as a
	    group by order8_id, order2_id, name_id,country_code
	    order by order8_id, order2_id, name_id,country_code;
	    
	else 
	
		INSERT INTO temp_admin_order2(order2_id, order1_id, name_id, country_code,the_geom)
	  SELECT  order2_id, 
	          order1_id, 
	          name_id,
	          country_code,
	          st_union(the_geom) as the_geom
	    FROM
	    (
	        select  mid_admin_GetNewID(cast(id as bigint),3) as order2_id,
	                mid_Get_VN_UpOrder(country_code_temp,order01,1) as order1_id, 
	                (case when name is null then null else id end) as name_id,
	                order00 as country_code, 
	                the_geom
	        from org_a7 
	        where order00 = country_code_temp
	    )as a
	    group by order2_id, order1_id, name_id,country_code
	    order by order2_id, order1_id, name_id,country_code;
	    
		INSERT INTO temp_admin_order8(order8_id, order2_id, name_id, country_code,the_geom)
	  SELECT  order8_id, 
	          order2_id, 
	          name_id,
	          country_code,
	          st_union(the_geom) as the_geom
	    FROM
	    (
	        select  mid_admin_GetNewID(cast(id as bigint),4) as order8_id,
	                mid_Get_VN_UpOrder(country_code_temp,order07,3) as order2_id, 
	                (case when name is null then null else id end) as name_id,
	                order00 as country_code, 
	                the_geom
	        from org_a8 
	        where order00 = country_code_temp
	    )as a
	    group by order8_id, order2_id, name_id,country_code
	    order by order8_id, order2_id, name_id,country_code;
	    
	end if;
	
	return 0;
END;
$$;

---insert into mid_admin_zone
CREATE OR REPLACE FUNCTION mid_insert_mid_admin_zone_VN(country_code_temp character varying)
	RETURNS integer
    LANGUAGE plpgsql
AS $$
DECLARE

BEGIN
	---order00
	insert into mid_admin_zone(ad_code, ad_order, order0_id, order1_id, order2_id, order8_id, ad_name, the_geom)
            select  order0_id as ad_code,
                    0 as ad_order,
                    order0_id as order0_id,
                    null as order1_id,
                    null as order2_id,
                    null as order8_id,
                    cast(name_id as character varying) as ad_name,
                    st_multi(the_geom) as the_geom
            from temp_admin_order0
            where country_code = country_code_temp
            order by ad_code;
	---order01
	insert into mid_admin_zone(ad_code, ad_order, order0_id, order1_id, order2_id, order8_id, ad_name, the_geom)
            select  order1_id as ad_code,
                    1 as ad_order,
                    order0_id as order0_id,
                    order1_id as order1_id,
                    null as order2_id,
                    null as order8_id,
                    cast(name_id as character varying) as ad_name,
                    st_multi(the_geom) as the_geom
            from temp_admin_order1
            where country_code = country_code_temp
            order by ad_code;
            
  if country_code_temp = 'VNM' or country_code_temp = 'LSO' or country_code_temp = 'SWZ' then
	---order08
	insert into mid_admin_zone(ad_code, ad_order, order0_id, order1_id, order2_id, order8_id, ad_name, the_geom)
            select  a.order8_id as ad_code,
                    8 as ad_order,
                    b.order0_id as order0_id,
                    a.order2_id as order1_id,
                    null as order2_id,
                    a.order8_id as order8_id,
                    cast(a.name_id as character varying) as ad_name,
                    st_multi(the_geom) as the_geom
            from temp_admin_order8 as a,
            (
            	select order0_id,order1_id from temp_admin_order1
            )as b
            where a.order2_id = b.order1_id and country_code = country_code_temp
            order by ad_code; 
            
	else
	---order02
		insert into mid_admin_zone(ad_code, ad_order, order0_id, order1_id, order2_id, order8_id, ad_name, the_geom)
		      select  a.order2_id as ad_code,
		              2 as ad_order,
		              b.order0_id as order0_id,
		              a.order1_id as order1_id,
		              a.order2_id as order2_id,
		              null as order8_id,
		              cast(a.name_id as character varying) as ad_name,
		              st_multi(the_geom) as the_geom
		      from temp_admin_order2 as a
		      left join
		      (
		      	select order0_id,order1_id from temp_admin_order1
		      )as b
		      on a.order1_id = b.order1_id
		      where country_code = country_code_temp
		      order by ad_code; 
		---order08      
		insert into mid_admin_zone(ad_code, ad_order, order0_id, order1_id, order2_id, order8_id, ad_name, the_geom)
		      select  a.order8_id as ad_code,
		              8 as ad_order,
		              c.order0_id as order0_id,
		              b.order1_id as order1_id,
		              a.order2_id as order2_id,
		              a.order8_id as order8_id,
		              cast(a.name_id as character varying) as ad_name,
		              st_multi(a.the_geom) as the_geom
		      from temp_admin_order8 as a
		      left join temp_admin_order2 as b
		      on a.order2_id = b.order2_id
		      left join temp_admin_order1 as c
		      on b.order1_id = c.order1_id
		      where a.country_code = country_code_temp
		      order by ad_code; 
	end if;            
	return 0;
END;
$$;

---get countrycode array
CREATE OR REPLACE FUNCTION mid_judge_country_array()
	RETURNS character varying[]
    LANGUAGE plpgsql
AS $$
DECLARE
	rec record;
	Countryarray character varying[];
	countryidx smallint;
BEGIN
	countryidx := 1;
	for rec in
	(
		select order00 
		from org_a0
		where name is not null and name <> 'Outer World'
		group by order00
	)
	loop
	Countryarray[countryidx] := rec.order00;
	countryidx := countryidx + 1;
	end loop;
	
	return Countryarray;
END;
$$;
---creat country_array table
CREATE OR REPLACE FUNCTION mid_alter_arder0_8()
	RETURNS smallint
    LANGUAGE plpgsql
AS $$
DECLARE
	Countryarray character varying[];
	countryidx smallint;
	countrycount smallint;
BEGIN
	Countryarray := mid_judge_country_array();
	countrycount := 0;
	countrycount := array_upper(Countryarray,1);
	
	for countryidx in 1..countrycount loop
		--if Countryarray[countryidx] in ('VNM', 'AUS', 'NZL') then
			perform mid_into_ID_NewOLd(Countryarray[countryidx]);
			perform mid_alter_VN_arder0_8(Countryarray[countryidx]);
		--end if;
	end loop;
	return 0;
END;
$$;
---insert array into mid_admin_zone
CREATE OR REPLACE FUNCTION mid_insert_mid_admin_zone()
	RETURNS integer
    LANGUAGE plpgsql
AS $$
DECLARE
	Countryarray character varying[];
	countryidx smallint;
	countrycount smallint;
BEGIN
	Countryarray := mid_judge_country_array();
	countrycount := 0;
	countrycount := array_upper(Countryarray,1);
	
	for countryidx in 1..countrycount loop
		--if Countryarray[countryidx] in ('VNM', 'AUS', 'NZL') then
			perform mid_insert_mid_admin_zone_VN(Countryarray[countryidx]);
		--end if;
	end loop;
		return 0;
END;
$$;

CREATE OR REPLACE FUNCTION mid_convert_summer_time()
    RETURNS smallint
    LANGUAGE plpgsql
AS $$
DECLARE
	rec					record;
	strTemp				varchar;
	strDateTime			varchar;
	strChar				varchar;
	nTargetPos			smallint;
	nTempPos			smallint;
	nStartWeek			smallint;
	nStartWeekDay		smallint;
	nEndWeek			smallint;
	nEndWeekDay			smallint;
	
	summer_time_id		smallint;
	start_weekday		smallint;
	end_weekday			smallint;
	start_date			smallint;
	end_date			smallint;
	start_time			smallint;
	end_time			smallint;
BEGIN
	summer_time_id = 0;
	for rec in
		select distinct attvalue as timedom
		from org_ae
		where atttyp = 'VP'
		order by timedom
    LOOP
    	-- init
    	summer_time_id = summer_time_id + 1;
    	start_weekday = 0;
    	end_weekday = 0;
    	start_date = 0;
    	end_date = 0;
    	start_time = 0;
    	end_time = 0;
    	
    	-- "[(M9l11)(M4f11)]"
    	strTemp = rec.timedom;
    	
		if not strpos(strTemp,')(') > 0 then
			raise EXCEPTION 'unhandled timedom type1, timedom = %', rec.timedom;
		else
	    	-- start date time
	    	strDateTime	:= substring(strTemp, strpos(strTemp,'(')+1, strpos(strTemp,')')-strpos(strTemp,'(')-1);
			nTargetPos	:= 1;
			nTempPos	:= 2;
			while nTempPos <= length(strDateTime)+1 loop
				strChar	:= substring(strDateTime, nTempPos, 1);
				if not ((strChar >= '0') and (strChar <= '9')) then
					if substring(strDateTime, nTargetPos, 1) = 'M' then
						start_date		:= cast(substring(strDateTime, nTargetPos+1, nTempPos-nTargetPos-1) as integer) * 100;
					elseif substring(strDateTime, nTargetPos, 1) = 'f' then
						nStartWeek		:= cast(substring(strDateTime, nTargetPos+1, 1) as integer);
						nStartWeekDay	:= cast(substring(strDateTime, nTargetPos+2, 1) as integer);
	    				start_weekday 	= ((1::smallint) << (nStartWeekDay-1));
	    				start_weekday 	= start_weekday | ((1::smallint) << (nStartWeek+6));
					elseif substring(strDateTime, nTargetPos, 1) = 'l' then
						nStartWeek		:= cast(substring(strDateTime, nTargetPos+1, 1) as integer);
						nStartWeekDay	:= cast(substring(strDateTime, nTargetPos+2, 1) as integer);
						if nStartWeek = 1 then
		    				start_weekday 	= ((1::smallint) << (nStartWeekDay-1));
		    				start_weekday 	= start_weekday | ((1::smallint) << 12);
		    			else
							raise EXCEPTION 'unhandled timedom type3, timedom = %', rec.timedom;
		    			end if;
					else
						raise EXCEPTION 'unhandled timedom type2, timedom = %', rec.timedom;
					end if;
					nTargetPos	:= nTempPos;
				end if;
				nTempPos	:= nTempPos + 1;
	    	end loop;
	    	
	    	strTemp			:= substring(strTemp, strpos(strTemp,')')+1);
	    	strDateTime		:= substring(strTemp, strpos(strTemp,'(')+1, strpos(strTemp,')')-strpos(strTemp,'(')-1);
			nTargetPos		:= 1;
			nTempPos		:= 2;
			while nTempPos <= length(strDateTime)+1 loop
				strChar	:= substring(strDateTime, nTempPos, 1);
				if not ((strChar >= '0') and (strChar <= '9')) then
					if substring(strDateTime, nTargetPos, 1) = 'M' then
						end_date		:= cast(substring(strDateTime, nTargetPos+1, nTempPos-nTargetPos-1) as integer) * 100;
					elseif substring(strDateTime, nTargetPos, 1) = 'f' then
						nEndWeek		:= cast(substring(strDateTime, nTargetPos+1, 1) as integer);
						nEndWeekDay		:= cast(substring(strDateTime, nTargetPos+2, 1) as integer);
	    				end_weekday 	= ((1::smallint) << (nEndWeekDay-1));
	    				end_weekday 	= end_weekday | ((1::smallint) << (nEndWeek+6));
					elseif substring(strDateTime, nTargetPos, 1) = 'l' then
						nEndWeek		:= cast(substring(strDateTime, nTargetPos+1, 1) as integer);
						nEndWeekDay		:= cast(substring(strDateTime, nTargetPos+2, 1) as integer);
						if nEndWeek = 1 then
		    				end_weekday 	= ((1::smallint) << (nEndWeekDay-1));
		    				end_weekday 	= end_weekday | ((1::smallint) << 12);
		    			else
							raise EXCEPTION 'unhandled timedom type3, timedom = %', rec.timedom;
		    			end if;
					else
						raise EXCEPTION 'unhandled timedom type2, timedom = %', rec.timedom;
					end if;
					nTargetPos	:= nTempPos;
				end if;
				nTempPos	:= nTempPos + 1;
	    	end loop;
	    end if;
    	    	
    	-- set records
    	insert into mid_admin_summer_time
    				(summer_time_id, start_weekday, start_date, start_time, end_weekday, end_date, end_time)
    		values	(summer_time_id, start_weekday, start_date, start_time, end_weekday, end_date, end_time);
    	
		insert into temp_admin_timedom_mapping(timedom, summer_time_id)
			values(rec.timedom, summer_time_id);
    END LOOP;
    
	return summer_time_id;
END;
$$;

----------------------------------------------------------------------------------------------
CREATE or replace FUNCTION mid_compare_nametype(nametyp character varying, 
                                                phone_nametyp character varying)
  RETURNS boolean  
  LANGUAGE plpgsql
  AS $$
DECLARE 
        nNametype     integer;
begin 
        if nametyp is null or phone_nametyp is null then
                return false;
        end if;

        BEGIN
                nNametype = nametyp::integer;
                if nNametype & 1 > 0 and phone_nametyp = 'ON' then  -- Official name
                        return true;
                end if;
                
                if nNametype & 2 > 0 and phone_nametyp = 'AN' then  -- alter name
                        return true;
                end if;
                
                if nNametype & 4 > 0 and phone_nametyp = 'RN' then  -- route number
                        return true;
                end if;
                
                if nNametype & 8 > 0 and phone_nametyp = 'BU' then  -- bru
                        return true;
                end if;
                -- RJ: Route Name       
        EXCEPTION when SQLSTATE '22P02' then 
                return nametyp = phone_nametyp;
        END;
        return False;
end;
$$;

-------------------------------------------------------------------------------------------
-- featclass: 4110--road, 2128--sign post
CREATE or replace FUNCTION mid_get_phoneme(nID bigint, 
                                           nFeatClass integer, 
                                           sol integer, 
                                           fullname character varying,
                                           namelc character varying, 
                                           nametyp character varying
                                           )
  RETURNS CHARACTER VARYING 
LANGUAGE plpgsql volatile
AS $$
DECLARE  
        rec         record;
        cnt         integer;
        nNametype   integer;
        scriptions_lang_codes  CHARACTER VARYING; -- phoneme script and lang code
        tmp_scrption           CHARACTER VARYING; 
begin 
        if nID is null then
                return null;
        end if;
        cnt := 0;
        scriptions_lang_codes := NUll;
        
        for rec in
	(	
                SELECT transcriptions, lanphonsets
                  FROM mid_vm_phoneme_relation AS r
                  LEFT JOIN mid_vm_pt as pt
                  ON r.ptid = pt.ptid
                  where id = nID 
                      AND featclass = nFeatClass
                      AND langcode = namelc
                      AND sideofline = sol
                      AND name = fullname
                      AND mid_compare_nametype(nametyp, r.nametype)
                  order by nametype = 'ON' desc  -- official name
        )
        LOOP 
                if cnt = 0 then
                        if rec.transcriptions::character varying ~ E'\\[|\\]|\\|' then
                                raise exception 'id=%, transcriptions=%, transcriptions include: [ ] |', 
                                                 nID, rec.transcriptions;
                                return null;
                        end if;
                        tmp_scrption := rec.transcriptions::character varying;
                        scriptions_lang_codes ='[' || array_to_string(rec.transcriptions, '|') || ']';
                        scriptions_lang_codes = scriptions_lang_codes || '[' || 
                                                array_to_string(rec.lanphonsets, '|') || ']';
                else
                        if tmp_scrption <> rec.transcriptions::character varying then
                                raise exception 'id=%, Phoneme Script dose not same.', nID;
                        end if;
                end if;
                cnt := cnt + 1;
        END LOOP;
        
        if cnt > 1 then
                BEGIN
                        nNametype = nametyp::integer;
                        if nNametype & 1 > 0 and nNametype & 8 > 0 then -- official and Bruunel
                                NULL;  
                        else
                                raise notice 'id = %, cnt=%, nametyp=%, namelc=%', nID, cnt, nNametype, namelc;
                        end if;
                EXCEPTION when SQLSTATE '22P02' then 
                        NULL;
                END;
        end if;
        return scriptions_lang_codes;
end;
$$;

-------------------------------------------------------------------------------------------

CREATE OR REPLACE FUNCTION mid_sc_nw_id()
  RETURNS integer 
  LANGUAGE plpgsql 
  AS $$
DECLARE
    i       integer;
    dist numeric;
    sql text;
    sql_1 text;
    sql_pol_count text;
    sql_pol text;
    sql_update text;
    sql_nearest text;
    cur1 refcursor;
    count integer;
    count_pol smallint;
    count_1 integer;
    featid text;
    point_x numeric;
    point_y numeric;
    pol_text text;
    --direction smallint;
    tf boolean;
    link_id numeric;
    
BEGIN 
    sql='select featid,xcoordinate::numeric,ycoordinate::numeric from mid_temp_safetyalert where nw_id is null and cameratype<>''8''';
    open cur1 for execute sql;
    sql_1='select count(1) from mid_temp_safetyalert where nw_id is null and cameratype<>''8''';
    
    execute sql_1 into count;
    while count>0 loop
         count:=count-1;
         dist=1e-7;
         fetch cur1 into featid,point_x,point_y;
         pol_text='POLYGON(('||(point_x+dist)::text||' '||(point_y+dist)::text||','||
                   (point_x+dist)::text||' '||(point_y-dist)::text||','||
                   (point_x-dist)::text||' '||(point_y-dist)::text||','||
                   (point_x-dist)::text||' '||(point_y+dist)::text||','||
                   (point_x+dist)::text||' '||(point_y+dist)::text||'))';
         --sql_pol='select id from org_nw where st_intersects(st_geomfromtext('|pol_text|',4326),the_geom)';
         sql_pol_count='select count(1) from org_nw where st_intersects(st_geomfromtext('''||pol_text||''',4326),the_geom)';
         
         tf:=True;
         count_1:=0;
         while tf loop
                  count_1:=count_1+1;
                  execute sql_pol_count into count_pol;
                  if count_pol=1 then
			sql_pol='select id from org_nw where st_intersects(st_geomfromtext('''||pol_text||''',4326),the_geom)';
			execute sql_pol into link_id;
			sql_update='update mid_temp_safetyalert set nw_id='||link_id::text||' where featid='''||featid||'''';
			execute sql_update;
			tf=False;
		  elsif count_pol=0 then
		         dist:=dist*2;
		         pol_text='POLYGON(('||(point_x+dist)::text||' '||(point_y+dist)::text||','||
			(point_x+dist)::text||' '||(point_y-dist)::text||','||
			(point_x-dist)::text||' '||(point_y-dist)::text||','||
			(point_x-dist)::text||' '||(point_y+dist)::text||','||
			(point_x+dist)::text||' '||(point_y+dist)::text||'))';
		         sql_pol_count='select count(1) from org_nw where st_intersects(st_geomfromtext('''||pol_text||''',4326),the_geom)';
		  else 
		         dist:=dist/2;
		         pol_text='POLYGON(('||(point_x+dist)::text||' '||(point_y+dist)::text||','||
			(point_x+dist)::text||' '||(point_y-dist)::text||','||
			(point_x-dist)::text||' '||(point_y-dist)::text||','||
			(point_x-dist)::text||' '||(point_y+dist)::text||','||
			(point_x+dist)::text||' '||(point_y+dist)::text||'))';
		        
		         sql_pol_count='select count(1) from org_nw where st_intersects(st_geomfromtext('''||pol_text||''',4326),the_geom)';
                  end if;
                  if count_1>100 then
                         sql_nearest='select id from org_nw order by st_distance(the_geom,(select the_geom from mid_temp_safetyalert where featid='''||featid||''')) limit 1';
                         execute sql_nearest into link_id;
                         sql_update='update mid_temp_safetyalert set nw_id='||link_id::text||' where featid='''||featid||'''';
			 execute sql_update;
			 tf=False;
	          end if;
         end loop;
     end loop;
     close cur1;
     return 0;
     
END;
$$;

-------------------------------------------------------------------------------------------


CREATE OR REPLACE FUNCTION mid_sc_link_id()
  RETURNS integer 
  LANGUAGE plpgsql 
  AS $$
DECLARE
    i       integer;
    dist numeric;
    sql text;
    sql_1 text;
    sql_pol_count text;
    sql_pol text;
    sql_update text;
    sql_nearest text;
    sql_angle text;
    sql_dd text;
    cur1 refcursor;
    count integer;
    count_pol smallint;
    count_1 integer;
    featid text;
    point_x numeric;
    point_y numeric;
    pol_text text;
    --direction smallint;
    tf boolean;
    id text;
    angle_dd integer;
    angle1 integer;
    angle2 integer;
    
    
    link_id numeric;
    cur2 refcursor;
    
BEGIN 
    sql='select featid,xcoordinate::numeric,ycoordinate::numeric from mid_temp_safetyalert where link_id is null and cameratype<>''8''';
    open cur1 for execute sql;
    sql_1='select count(1) from mid_temp_safetyalert where link_id is null and cameratype<>''8''';
    
    execute sql_1 into count;
    while count>0 loop
         count:=count-1;
         dist=1e-7;
         fetch cur1 into featid,point_x,point_y;
         tf=True;
         while tf loop
              pol_text='POLYGON(('||(point_x+dist)::text||' '||(point_y+dist)::text||','||
                   (point_x+dist)::text||' '||(point_y-dist)::text||','||
                   (point_x-dist)::text||' '||(point_y-dist)::text||','||
                   (point_x-dist)::text||' '||(point_y+dist)::text||','||
                   (point_x+dist)::text||' '||(point_y+dist)::text||'))';
		sql_pol='select id::text from org_nw where st_intersects(st_geomfromtext('''||pol_text||''',4326),the_geom)';
		sql_pol_count='select count(1) from org_nw where st_intersects(st_geomfromtext('''||pol_text||''',4326),the_geom)';
		open cur2 for execute sql_pol;
		execute sql_pol_count into count_1;
		while count_1>0 loop
			count_1:=count_1-1;
			fetch cur2 into id;
			sql_angle='select 
			((450-mid_cal_zm_2(st_geomfromtext(st_astext(st_line_substring(st_geometryn((select the_geom from org_nw where id='||id||'),1),0,st_line_locate_point(st_geometryn((select the_geom from org_nw where id='||id||'),1),a.the_geom))),4326),-1))::integer)%360,
			((450-mid_cal_zm_2(st_geomfromtext(st_astext(st_line_substring(st_geometryn((select the_geom from org_nw where id='||id||'),1),st_line_locate_point(st_geometryn((select the_geom from org_nw where id='||id||'),1),a.the_geom),1)),4326),0))::integer)%360
			from mid_temp_safetyalert a  
			where a.featid='''||featid||'''';
			execute sql_angle into angle1,angle2;
			sql_dd='select drivingdirection::integer from mid_temp_safetyalert where featid='''||featid||'''';
			execute sql_dd into angle_dd;
			if (angle1 is not null and (360+angle1-angle_dd) % 180 in (179,0,1)) or (angle2 is not null and (360+angle2-angle_dd) % 180 in (179,0,1)) then 
				sql_update='update mid_temp_safetyalert
					set link_id='||id||'
					where featid='''||featid||'''';
				execute sql_update;
				tf=False;
			end if;
		end loop;
		close cur2;
          dist:=dist*2;
          end loop;    
     end loop;
     close cur1;
     return 0;
END;
$$;

-------------------------------------------------------------------------------------------

CREATE OR REPLACE FUNCTION mid_cal_zm_2(shape_point geometry, dir integer)
  RETURNS double precision 
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

    i=1;
    IF (dir = -1) THEN
        dst_lon := ST_X(ST_PointN(shape_point, num)) * 256 * 3600;
        dst_lat := ST_Y(ST_PointN(shape_point, num)) * 256 * 3600;
        org_lon := ST_X(ST_PointN(shape_point, num - 1)) * 256 * 3600;
        org_lat := ST_Y(ST_PointN(shape_point, num - 1)) * 256 * 3600;

    else
        org_lon := ST_X(ST_PointN(shape_point, 1)) * 256 * 3600;
        org_lat := ST_Y(ST_PointN(shape_point, 1)) * 256 * 3600;
        dst_lon := ST_X(ST_PointN(shape_point, 2)) * 256 * 3600;
        dst_lat := ST_Y(ST_PointN(shape_point, 2)) * 256 * 3600;
    
    end if;

    while dst_lon=org_lon and dst_lat=org_lat loop
                i:=i+1;
        IF (dir = -1) THEN
            org_lon := ST_X(ST_PointN(shape_point, num - i)) * 256 * 3600;
            org_lat := ST_Y(ST_PointN(shape_point, num - i)) * 256 * 3600;
        else
            dst_lon := ST_X(ST_PointN(shape_point, 1 + i)) * 256 * 3600;
            dst_lat := ST_Y(ST_PointN(shape_point, 1 + i)) * 256 * 3600;
        end if;
    end loop;

    delta_lon := dst_lon - org_lon;
    delta_lat := dst_lat - org_lat;
    
    real_dir := (atan2(delta_lat, delta_lon) * 180.0 / PI);
    
    return real_dir;
END;
$$;

-------------------------------------------------------------------------------------------

CREATE OR REPLACE FUNCTION mid_sz_dir()
  RETURNS integer
  LANGUAGE plpgsql 
  AS $$
DECLARE
    i       integer;
    sql  text;
    sql_1  text;
    sql_2  text;
    sql_3  text;
    sql_4  text;
    sql_5  text;
    featid text;
    nodeid numeric;
    dir     smallint;
    cur1 refcursor;
    count int;
    count1 int;
    linknum int;
    link_id_f text;
    link_id_t text;
    
BEGIN 
    sql:='select featid,count(1) from mid_temp_safetyzone_link group by featid';
    open cur1 for execute sql;
    sql_1:='select count(distinct(featid)) from mid_temp_safetyzone_link';
    
    execute sql_1 into count;
    while count>0 loop
         count:=count-1;
         fetch cur1 into featid,linknum;
         sql_2:='select shape_line_id,dir,case when dir=2 then f_jnctid else t_jnctid end from mid_temp_safetyzone_link where featid='''||featid||''' and dir is not null';
         execute sql_2 into link_id_f,dir,nodeid;
         for i in 1..linknum-1 loop
             sql_3:='select shape_line_id,case when f_jnctid='||nodeid::text|| ' then t_jnctid else f_jnctid end
                    from mid_temp_safetyzone_link
                    where '||nodeid::text||' in (f_jnctid,t_jnctid) and dir is null and featid='''||featid||'''';
             execute sql_3 into link_id_t,nodeid;
             sql_4:='    update mid_temp_safetyzone_link
      			set dir=case when
			(select 4-count(distinct(f_jnctid))-count(distinct(t_jnctid))
			from mid_temp_safetyzone_link
			where featid='''||featid||''' and shape_line_id in ('''||link_id_f||''','''||link_id_t||'''))=1 then '||(3-dir)::text||' else '||dir::text||' end
			where 
			featid='''||featid||'''
			and 
			shape_line_id='''||link_id_t||'''';
	     execute sql_4;
	     sql_5='select dir from mid_temp_safetyzone_link
	            where featid='''||featid||''' and shape_line_id='''||link_id_t||'''';
	     execute sql_5 into dir;
	     link_id_f:=link_id_t;
         end loop;
     end loop;
     close cur1;
     return 0;
END;
$$;

-----------------------------------------------------------------------------------------
--Sort on link
CREATE OR REPLACE FUNCTION mid_get_link_order(links bigint[], s_nodes bigint[],
																						e_nodes bigint[], oneways smallint[], num smallint)
RETURNS BIGINT[]
LANGUAGE PLPGSQL
AS $$
DECLARE
    node_id bigint;
BEGIN
	
--	if 1 = all(oneways) then
--		return null
--	end if;
	
	--initialisation data array
--	for num_idx in 1..(num+num) loop
--		s_e_idx[num_idx] := 0;
--	end loop;
	
	--get oneway is 2 or 3
--	for num_idx in 1..num loop
--		s_e_idx[num_idx] := 0;
--	end loop;
		node_id := 0;
		for num_idx in 1..num loop
			if mid_in_array_count(e_nodes[num_idx], e_nodes||s_nodes, num+num) not in (1,2) then
				raise notice 'id = %, oneway=% error!',links[num_idx], oneways[num_idx];
			end if;
		end loop;
    
    for num_idx in 1..num loop
    	if oneways[num_idx] = 4 then
    		raise notice 'id = %, oneway=% error!',links[num_idx], oneways[num_idx];
    	end if;
    	--shuang xiang
    	if oneways[num_idx] = 1 then
    		if num_idx = 1 then
    		
	    		if (s_nodes[num_idx] in (s_nodes[num_idx+1], e_nodes[num_idx+1])) and 
	    			 (mid_in_array_count(e_nodes[num_idx], e_nodes||s_nodes, num+num) = 1) then
	    			 node_id = s_nodes[num_idx];
	    		elsif (e_nodes[num_idx] in (s_nodes[num_idx+1], e_nodes[num_idx+1])) and 
	    			 (mid_in_array_count(s_nodes[num_idx], e_nodes||s_nodes, num+num) = 1) then
	    			node_id = e_nodes[num_idx];
	    		else
	    			raise notice 'id = %, oneway=% error!',links[num_idx], oneways[num_idx];
	    		end if;
	    		
	    	elsif num_idx = num then
	    	
	    		if (s_nodes[num_idx] in (s_nodes[num_idx-1], e_nodes[num_idx-1])) and 
	    			 (mid_in_array_count(e_nodes[num_idx], e_nodes||s_nodes, num+num) = 1) then
	    			 --
	    		elsif (e_nodes[num_idx] in (s_nodes[num_idx-1], e_nodes[num_idx-1])) and 
	    			 (mid_in_array_count(s_nodes[num_idx], e_nodes||s_nodes, num+num) = 1) then
	    			--
	    		else
	    			raise notice 'id = %, oneway=% error!',links[num_idx], oneways[num_idx];
	    		end if;
	    		
	    	else
	    		
	    		if (s_nodes[num_idx] in (s_nodes[num_idx-1], e_nodes[num_idx-1])) and 
	    			 (e_nodes[num_idx] in (s_nodes[num_idx+1], e_nodes[num_idx+1])) then
	    			 --
	    		elsif (e_nodes[num_idx] in (s_nodes[num_idx-1], e_nodes[num_idx-1])) and 
	    			 (s_nodes[num_idx] in (s_nodes[num_idx+1], e_nodes[num_idx+1])) then
	    			--
	    		else
	    			raise notice 'id = %, oneway=% error!',links[num_idx], oneways[num_idx];
	    		end if;
	    		
	    	end if;
	    --zheng xiang	
	    elsif oneways[num_idx] = 2 then
	    	if num_idx = 1 then
    
	    		if (e_nodes[num_idx] in (s_nodes[num_idx+1], e_nodes[num_idx+1])) and 
	    			 (mid_in_array_count(s_nodes[num_idx], e_nodes||s_nodes, num+num) = 1) then
	    			node_id = e_nodes[num_idx];
	    		else
	    			raise notice 'id = %, oneway=% error!',links[num_idx], oneways[num_idx];
	    		end if;
	    		
	    	elsif num_idx = num then
	    	
	    		if (s_nodes[num_idx] in (s_nodes[num_idx-1], e_nodes[num_idx-1])) and 
	    			 (mid_in_array_count(e_nodes[num_idx], e_nodes||s_nodes, num+num) = 1) then
	    			 --
	    		else
	    			raise notice 'id = %, oneway=% error!',links[num_idx], oneways[num_idx];
	    		end if;
	    		
	    	else
	    		
	    		if (s_nodes[num_idx] in (s_nodes[num_idx-1], e_nodes[num_idx-1])) and 
	    			 (e_nodes[num_idx] in (s_nodes[num_idx+1], e_nodes[num_idx+1])) then
	    			 --
	    		else
	    			raise notice 'id = %, oneway=% error!',links[num_idx], oneways[num_idx];
	    		end if;
	    		
	    	end if;
	    --fanxiang	
	    elsif oneways[num_idx] = 3 then
	    	if num_idx = 1 then
    
	    		if (s_nodes[num_idx] in (s_nodes[num_idx+1], e_nodes[num_idx+1])) and 
	    			 (mid_in_array_count(e_nodes[num_idx], e_nodes||s_nodes, num+num) = 1) then
	    			node_id = s_nodes[num_idx];
	    		else
	    			raise notice 'id = %, oneway=% error!',links[num_idx], oneways[num_idx];
	    		end if;
	    		
	    	elsif num_idx = num then
	    	
	    		if (e_nodes[num_idx] in (s_nodes[num_idx-1], e_nodes[num_idx-1])) and 
	    			 (mid_in_array_count(s_nodes[num_idx], e_nodes||s_nodes, num+num) = 1) then
	    			 --
	    		else
	    			raise notice 'id = %, oneway=% error!',links[num_idx], oneways[num_idx];
	    		end if;
	    		
	    	else
	    		
	    		if (s_nodes[num_idx] in (s_nodes[num_idx+1], e_nodes[num_idx+1])) and 
	    			 (e_nodes[num_idx] in (s_nodes[num_idx-1], e_nodes[num_idx-1])) then
	    			 --
	    		else
	    			raise notice 'id = %, oneway=% error!',links[num_idx], oneways[num_idx];
	    		end if;
	    		
	    	end if;
	    else
	    	raise notice 'id = %, oneway=% error!',links[num_idx], oneways[num_idx];
    	end if;
  	end loop;
  	
  	return links || array[node_id];
END;
$$;
--get link appear num
CREATE OR REPLACE FUNCTION mid_in_array_count(id bigint, id_array bigint[], array_num smallint)
RETURNS smallint
LANGUAGE PLPGSQL
AS $$
DECLARE
    num_appear integer;
BEGIN
	num_appear := 0;
	for array_num_idx in 1..array_num loop
		if id = id_array[array_num_idx] then
			num_appear := num_appear + 1;
		end if;
	end loop;
	
  return num_appear;
END;
$$;
--get school zone link 
CREATE OR REPLACE FUNCTION mid_get_school_zone_guide_id()
RETURNS smallint
LANGUAGE PLPGSQL
AS $$
DECLARE
	cur_guide_id integer;
	curs1 refcursor;
	rec1 record;
	
	id_array bigint[];
	id_array_all bigint[];
BEGIN	
	select (case when max(safetyzone_id) is null then 0 else max(safetyzone_id) end)
	from safety_zone_tbl
	where safety_type <> 3
	into cur_guide_id;
	
	id_array_all := array[]::bigint[];
	
	open curs1 for select id, s_node, e_node, speed from temp_school_zone_s_e_node;
	fetch curs1 into rec1;
	
	while rec1.id is not null loop
	
		if rec1.id = any(id_array_all) then
			fetch curs1 into rec1;
			continue;
		end if;
		
		cur_guide_id := cur_guide_id + 1;
		id_array := mid_get_school_zone_connect_link(rec1.id, rec1.s_node, rec1.e_node, rec1.speed);
		
		insert into temp_school_zone_guide_id(id, guide_id)
		(select unnest(id_array), cur_guide_id);
		
		id_array_all := id_array_all || id_array;
		
		fetch curs1 into rec1;
	end loop;

	
	return 0;
	
END;
$$;

CREATE OR REPLACE FUNCTION mid_get_school_zone_connect_link(temp_id bigint, temp_s_node bigint, temp_e_node bigint, temp_speed smallint)
RETURNS bigint[]
LANGUAGE PLPGSQL
AS $$
DECLARE
	icur_id integer;
	iall_id integer;
	curs1 refcursor;
	rec1 record;
	
	link_id_array bigint[];
	link_snode_array bigint[];
	link_enode_array bigint[];
	
BEGIN	

	icur_id := 1;
	iall_id := 1;
	
	link_id_array[iall_id] := temp_id;
	link_snode_array[iall_id] := temp_s_node;
	link_enode_array[iall_id] := temp_e_node;	
	
	while icur_id <= iall_id loop
	
		open curs1 for select id, s_node, e_node
										from temp_school_zone_s_e_node
										where id <> link_id_array[icur_id] and speed = temp_speed and 
										      (s_node in (link_snode_array[icur_id], link_enode_array[icur_id]) or
										       e_node in (link_snode_array[icur_id], link_enode_array[icur_id]));
		fetch curs1 into rec1;
		while rec1.id is not null loop
			if rec1.id = any(link_id_array) then
				fetch curs1 into rec1;
				continue;
			end if;
			
			iall_id := iall_id + 1;
			link_id_array[iall_id] := rec1.id;
			link_snode_array[iall_id] := rec1.s_node;
			link_enode_array[iall_id] := rec1.e_node;
			
			fetch curs1 into rec1;
		end loop;
		close curs1;
		
		icur_id := icur_id + 1;
	end loop;
	
	return link_id_array;
	
END;
$$;

CREATE OR REPLACE FUNCTION cat_condition_judge(condi character varying, subcat integer, brandname character varying)
RETURNS boolean 
LANGUAGE PLPGSQL
AS $$ 
DECLARE
s1 smallint;
e1 smallint;
s2 smallint;
e2 smallint;
BEGIN
	if condi = '' then
		return TRUE;
	else
		-- brandname Sub-Category
		if (position('brandname' in condi) <> 0) and (position('Sub-Category' in condi) <> 0) then
			s1 = position('=' in condi) + 1;
			e1 = position('&' in condi) - s1;
			s2 = position('''' in condi) + 1;
			e2 = char_length(condi) - s2;
			if subcat = cast(substring(condi from s1 for e1) AS integer)
			   and brandname = substring(condi from s2 for e2 ) then
				return TRUE;
			else
				return FALSE;
			end if;
		end if;
		--brandname
		if (position('brandname'in condi) <> 0) and (position('Sub-Category' in condi) = 0) then		
			s2 = position('''' in condi) + 1;
			e2 = char_length(condi) - s2 ;
			if brandname = substring(condi from s2 for e2 ) then
				return TRUE;
			else
				return FALSE;
			end if;
		end if;
		--Sub-Category
		if (position('Sub-Category' in condi) <> 0) and (position('brandname' in condi) = 0) then		
			s1 = position('=' in condi) + 1;
			e1 = char_length(condi) - s1 + 1;
			if subcat = cast(substring(condi from s1 for e1) AS integer) then
				return TRUE;	
			else
				return FALSE;
			end if;
		end if;
	end if;
	return FALSE;
END;
$$;

CREATE OR REPLACE FUNCTION temp_lane_info_merge(laneinfo_list varchar[], lane_cnt int)
  returns varchar
  language plpgsql
as $$
declare
	lane_info_cnt int;
	i int;
	rtn_merge_laneinfo bit varying(16);
BEGIN
	rtn_merge_laneinfo := laneinfo_list[1]::bit(16);
	
        lane_info_cnt := array_upper(laneinfo_list, 1);

	if lane_info_cnt >= 2 then
		for i in 2..lane_info_cnt loop
			rtn_merge_laneinfo := rtn_merge_laneinfo | laneinfo_list[i]::bit(16);
		end loop;
	end if;

        return substring(rtn_merge_laneinfo::varchar, 1, lane_cnt);
end;
$$;