-------------------------------------------------------------------------------------------------------------
-- Full-width ==> half-width
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_translate(_name character varying)
  RETURNS character varying 
  LANGUAGE plpgsql 
  AS $$
begin
	return translate(_name, '£Á£Â£Ã£Ä£Å£Æ£Ç£È£É£Ê£Ë£Ì£Í£Î£Ï£Ð£Ñ£Ò£Ó£Ô£Ø£Ù£Ú£Õ£Ö£×£±£²£³£´£µ£¶£·£¸£¹£°', 'ABCDEFGHIJKLMNOPQRSTXYZUVW1234567890');
END;
$$;

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

-------------------------------------------------------------------------------------------------------------
-- link name
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_make_road_name_dictionary()
 RETURNS integer
    LANGUAGE plpgsql
    AS  
$$ 
DECLARE
    curs1 refcursor;
    road_lid integer[];
    road_id integer;
    _name_chn character varying; 
    _name_trd character varying; 
    _name_py character varying;
    _name_eng character varying;
    _name_ctn character varying;
    cnt integer;
    curr_name_id integer;
    i integer;
BEGIN
	curr_name_id := 0;
	cnt          := 0;
	OPEN curs1 FOR SELECT array_agg(road), name_chn, name_trd, name_py, name_eng, name_ctn
			  FROM org_roadsegment
			  where name_chn is not null
			  group by name_chn, name_trd, name_py, name_eng, name_ctn;
    
	-- Get the first record        
	FETCH curs1 INTO road_lid, _name_chn, _name_trd, _name_py, _name_eng, _name_ctn;
	while road_lid is not null LOOP
		-- Simplified Chinese and PinYin
		IF _name_chn IS NOT NULL THEN
			curr_name_id := mid_add_one_name(curr_name_id, mid_translate(_name_chn), _name_py, 1);
		END IF;
		-- Traditional Chinese and PinYin
		IF _name_trd IS NOT NULL THEN
			curr_name_id := mid_add_one_name(curr_name_id, mid_translate(_name_trd), _name_py, 2);
		END IF;
		-- English
		IF _name_eng IS NOT NULL THEN
			curr_name_id := mid_add_one_name(curr_name_id, _name_eng, null, 3);
		END IF;
		-- GuangDong
		IF _name_ctn IS NOT NULL THEN
			curr_name_id := mid_add_one_name(curr_name_id, _name_ctn, null, 54);
		END IF;
		-- They use the same name_id

		i := 1;
		while i <= array_upper(road_lid, 1) LOOP
			road_id = road_lid[i];
			insert into temp_road_name("road", "new_name_id", "seq_nm")
				values (road_id, curr_name_id, 1);
			i := i + 1;
		END LOOP;
		
		curr_name_id := curr_name_id + 1;
		cnt          := cnt + 1;
		-- Get the next record
		FETCH curs1 INTO road_lid, _name_chn, _name_trd, _name_py, _name_eng, _name_ctn;
	END LOOP;
	close curs1;

    return cnt;
END;
$$;

CREATE OR REPLACE FUNCTION mid_make_road_alias_name_dictionary()
 RETURNS integer
    LANGUAGE plpgsql
    AS  
$$ 
DECLARE
    curs1 refcursor;
    road_lid integer[];
    road_id integer;
    _name_chn character varying[]; 
    _name_trd character varying[]; 
    _name_py character varying[];
    _name_eng character varying[];
    _name_ctn character varying[];
    chn_len integer;
    trd_len integer;
    py_len integer;
    eng_len integer;
    ctn_len integer;
    cnt integer;
    curr_name_id integer;
    i integer;
    j integer;
BEGIN
	curr_name_id := 0;
	cnt          := 0;
	OPEN curs1 FOR SELECT array_agg(road), regexp_split_to_array(alias_chn, E'\\£ü+'), 
				regexp_split_to_array(alias_trd, E'\\£ü+'), 
				regexp_split_to_array(alias_py, E'\\£ü+'), 
				regexp_split_to_array(alias_eng, E'\\£ü+'), 
				regexp_split_to_array(alias_ctn, E'\\£ü+') 
			FROM org_roadsegment
			where alias_chn is not null
			group by alias_chn, alias_trd, alias_py, alias_eng, alias_ctn;
    
	-- Get the first record        
	FETCH curs1 INTO road_lid, _name_chn, _name_trd, _name_py, _name_eng, _name_ctn;
	while road_lid is not null LOOP
		i := 1;
		while i <= array_upper(_name_chn, 1) LOOP
			-- Simplified Chinese and PinYin	
			IF _name_chn[i] IS NOT NULL THEN
				curr_name_id := mid_add_one_name(curr_name_id, mid_translate(_name_chn[i]), _name_py[i], 1);
			END IF;
			-- Traditional Chinese and PinYin
			IF _name_trd[i] IS NOT NULL THEN
				curr_name_id := mid_add_one_name(curr_name_id, mid_translate(_name_trd[i]), _name_py[i], 2);
			END IF;
			-- English
			IF _name_eng[i] IS NOT NULL THEN
				curr_name_id := mid_add_one_name(curr_name_id, _name_eng[i], null, 3);
			END IF;
			-- Guang Dong
			IF _name_ctn[i] IS NOT NULL THEN
				curr_name_id := mid_add_one_name(curr_name_id, _name_ctn[i], null, 54);
			END IF;
			-- above names use the same name_id

			j := 1;
			while j <= array_upper(road_lid, 1) LOOP
				road_id = road_lid[j];
				insert into temp_road_name("road", "new_name_id", "seq_nm")
					values (road_id, curr_name_id, i+1);
				j := j + 1;
			END LOOP;
			
			curr_name_id := curr_name_id + 1;
			cnt          := cnt + 1;
			i := i + 1;
		END LOOP;

		-- Get the next record
		FETCH curs1 INTO road_lid, _name_chn, _name_trd, _name_py, _name_eng, _name_ctn;
	END LOOP;
	close curs1;

    return cnt;
END;
$$;

-------------------------------------------------------------------------------------------------------------
-- node name
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_make_node_name_dictionary()
 RETURNS integer
    LANGUAGE plpgsql
    AS  
$$ 
DECLARE
    curs1 refcursor;
    road_lid integer[];
    node_id integer;
    _name_chn character varying; 
    _name_trd character varying; 
    _name_py character varying;
    _name_eng character varying;
    _name_ctn character varying;
    cnt integer;
    curr_name_id integer;
    i integer;
BEGIN
	curr_name_id := 0;
	cnt          := 0;
	OPEN curs1 FOR SELECT array_agg(node), name_chn, name_trd, name_py, name_eng, name_ctn
			  FROM org_roadnode
			  where name_chn is not null
			  group by name_chn, name_trd, name_py, name_eng, name_ctn;
    
	-- Get the first record        
	FETCH curs1 INTO road_lid, _name_chn, _name_trd, _name_py, _name_eng, _name_ctn;
	while road_lid is not null LOOP
		-- Simplified Chinese and PinYin
		IF _name_chn IS NOT NULL THEN
			curr_name_id := mid_add_one_name(curr_name_id, mid_translate(_name_chn), _name_py, 1);
		END IF;
		-- Traditional Chinese and PinYin
		IF _name_trd IS NOT NULL THEN
			curr_name_id := mid_add_one_name(curr_name_id, mid_translate(_name_trd), _name_py, 2);
		END IF;
		-- English
		IF _name_eng IS NOT NULL THEN
			curr_name_id := mid_add_one_name(curr_name_id, _name_eng, null, 3);
		END IF;
		-- GuangDong
		IF _name_ctn IS NOT NULL THEN
			curr_name_id := mid_add_one_name(curr_name_id, _name_ctn, null, 54);
		END IF;
		-- They use the same name_id

		i := 1;
		while i <= array_upper(road_lid, 1) LOOP
			node_id = road_lid[i];
			insert into temp_node_name("node", "new_name_id")
				values (node_id, curr_name_id);
			i := i + 1;
		END LOOP;
		
		curr_name_id := curr_name_id + 1;
		cnt          := cnt + 1;
		-- Get the next record
		FETCH curs1 INTO road_lid, _name_chn, _name_trd, _name_py, _name_eng, _name_ctn;
	END LOOP;
	close curs1;

    return cnt;
END;
$$;

-------------------------------------------------------------------------------------------------------------
-- road number
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_make_road_number_dictionary()
 RETURNS integer
    LANGUAGE plpgsql
    AS  
$$ 
DECLARE
    curs1 refcursor;
    gid_lid integer[];
    gid_id integer;
    _route_no character varying; 
    cnt integer;
    curr_name_id integer;
    i integer;
    temp_road_id integer;
    temp_seq_nm integer;
    tr_route_no character varying;
BEGIN
	curr_name_id := 0;
	cnt          := 0;
	OPEN curs1 FOR select array_agg(gid), route_no
			  from temp_road_number_old
			  group by route_no;

	-- Get the first record        
	FETCH curs1 INTO gid_lid, _route_no;
	while gid_lid is not null LOOP

		IF _route_no IS NOT NULL THEN
			tr_route_no  := mid_translate(_route_no)
			curr_name_id := mid_add_one_name(curr_name_id, tr_route_no, null, 1);  -- Simplified Chinese and PinYin
			curr_name_id := mid_add_one_name(curr_name_id, tr_route_no, null, 2);  -- Traditional Chinese and PinYin
			curr_name_id := mid_add_one_name(curr_name_id, tr_route_no, null, 3);  -- English
		END IF;
		
		-- They use the same name_id

		i := 1;
		while i <= array_upper(gid_lid, 1) LOOP
			gid_id = gid_lid[i];
			SELECT road, seq_nm INTO temp_road_id, temp_seq_nm from temp_road_number_old where gid = gid_id;
			insert into temp_road_number("road", "new_name_id", seq_nm)
				values (temp_road_id, curr_name_id, temp_seq_nm);
			i   := i + 1;
			cnt := cnt + 1;
		END LOOP;
		
		curr_name_id := curr_name_id + 1;
		
		-- Get the next record
		FETCH curs1 INTO  gid_lid, _route_no;
	END LOOP;
	close curs1;

    return cnt;
END;
$$;

-------------------------------------------------------------------------------------------------------------
-- sign post
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_make_node_signpost_dictionary()
 RETURNS integer
    LANGUAGE plpgsql
    AS  
$$ 
DECLARE
    curs1 refcursor;
    sp_lid integer[];
    sp_id integer;
    _name_chn character varying; 
    _name_trd character varying; 
    _name_py character varying;
    _name_eng character varying;
    _name_ctn character varying;
    cnt integer;
    curr_name_id integer;
    i integer;
BEGIN
	curr_name_id := 0;
	cnt          := 0;
	OPEN curs1 FOR SELECT  array_agg(sp), name_chn, name_trd, name_py, name_eng, name_ctn
			  FROM org_roadnodesignpost
			  where name_chn is not null
			  group by name_chn, name_trd, name_trd, name_eng, name_ctn;
    
	-- Get the first record        
	FETCH curs1 INTO sp_lid, _name_chn, _name_trd, _name_py, _name_eng, _name_ctn;
	while sp_lid is not null LOOP
		-- Simplified Chinese and PinYin
		IF _name_chn IS NOT NULL THEN
			curr_name_id := mid_add_one_name(curr_name_id, mid_translate(_name_chn), _name_py, 1);
		END IF;
		-- Traditional Chinese and PinYin
		IF _name_trd IS NOT NULL THEN
			curr_name_id := mid_add_one_name(curr_name_id, mid_translate(_name_trd), _name_py, 2);
		END IF;
		-- English
		IF _name_eng IS NOT NULL THEN
			curr_name_id := mid_add_one_name(curr_name_id, _name_eng, null, 3);
		END IF;
		-- GuangDong
		IF _name_ctn IS NOT NULL THEN
			curr_name_id := mid_add_one_name(curr_name_id, _name_ctn, null, 54);
		END IF;
		-- They use the same name_id

		i := 1;
		while i <= array_upper(sp_lid, 1) LOOP
			sp_id = sp_lid[i];
			insert into temp_node_signpost_dict("sp", "new_name_id")
				values (sp_id, curr_name_id);
			i := i + 1;
		END LOOP;
		
		curr_name_id := curr_name_id + 1;
		cnt          := cnt + 1;
		-- Get the next record
		FETCH curs1 INTO sp_lid, _name_chn, _name_trd, _name_py, _name_eng, _name_ctn;
	END LOOP;
	close curs1;

    return cnt;
END;
$$;

CREATE OR REPLACE FUNCTION mid_make_cross_signpost_dictionary()
 RETURNS integer
    LANGUAGE plpgsql
    AS  
$$ 
DECLARE
    curs1 refcursor;
    sp_lid integer[];
    sp_id integer;
    _name_chn character varying; 
    _name_trd character varying; 
    _name_py character varying;
    _name_eng character varying;
    _name_ctn character varying;
    cnt integer;
    curr_name_id integer;
    i integer;
BEGIN
	curr_name_id := 0;
	cnt          := 0;
	OPEN curs1 FOR SELECT  array_agg(sp), name_chn, name_trd, name_py, name_eng, name_ctn
			  FROM org_roadcrosssignpost
			  where name_chn is not null
			  group by name_chn, name_trd, name_py, name_eng, name_ctn;
    
	-- Get the first record        
	FETCH curs1 INTO sp_lid, _name_chn, _name_trd, _name_py, _name_eng, _name_ctn;
	while sp_lid is not null LOOP
		-- Simplified Chinese and PinYin
		IF _name_chn IS NOT NULL THEN
			curr_name_id := mid_add_one_name(curr_name_id, mid_translate(_name_chn), _name_py, 1);
		END IF;
		-- Traditional Chinese and PinYin
		IF _name_trd IS NOT NULL THEN
			curr_name_id := mid_add_one_name(curr_name_id, mid_translate(_name_trd), _name_py, 2);
		END IF;
		-- English
		IF _name_eng IS NOT NULL THEN
			curr_name_id := mid_add_one_name(curr_name_id, _name_eng, null, 3);
		END IF;
		-- GuangDong
		IF _name_ctn IS NOT NULL THEN
			curr_name_id := mid_add_one_name(curr_name_id, _name_ctn, null, 54);
		END IF;
		-- They use the same name_id

		i := 1;
		while i <= array_upper(sp_lid, 1) LOOP
			sp_id = sp_lid[i];
			insert into temp_cross_signpost_dict("sp", "new_name_id")
				values (sp_id, curr_name_id);
			i := i + 1;
		END LOOP;
		
		curr_name_id := curr_name_id + 1;
		cnt          := cnt + 1;
		-- Get the next record
		FETCH curs1 INTO sp_lid, _name_chn, _name_trd, _name_py, _name_eng, _name_ctn;
	END LOOP;
	close curs1;

    return cnt;
END;
$$;