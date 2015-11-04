
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

-----------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_insert_name_dictionary(from_tbl character varying, id_col character varying, name_col character varying, 
                                  py_col character varying, lang_code_col character varying, temp_tbl character varying, condition character varying)
 RETURNS integer
    LANGUAGE plpgsql
    AS  
$$ 
DECLARE
    curs1 refcursor;
    curr_name_id integer;
    id integer;
    _name character varying;
    py_name character varying;
    cnt integer;
    language_id integer;
    language_code character varying;
BEGIN
	curr_name_id := 0;
	cnt          := 0;

	IF py_col IS NOT NULL AND py_col <> '' THEN
		IF condition IS NOT NULL OR condition <> '' THEN
			OPEN curs1 FOR execute 'SELECT ' || id_col || ',' || name_col ||',' || py_col || ',' || lang_code_col || ' from ' || ($1) ||
				' WHERE ' || condition || ' ORDER BY ' || name_col || ',' || py_col || ',' || lang_code_col;
		ELSE
			OPEN curs1 FOR execute 'SELECT ' || id_col || ',' || name_col ||',' || py_col || ',' || lang_code_col || ' from ' || ($1) ||
				' ORDER BY ' || name_col || ',' || py_col || ',' || lang_code_col;
		END IF;
	ELSE
		IF condition IS NOT NULL OR condition <> '' THEN
			OPEN curs1 FOR execute 'SELECT ' || id_col || ', CAST(' || name_col ||' AS CHARACTER VARYING) ' || ', NULL, ' || lang_code_col || ' from ' || ($1)||
				' WHERE ' || condition || ' ORDER BY ' || name_col || ',' || lang_code_col;
		ELSE
			OPEN curs1 FOR execute 'SELECT ' || id_col || ', CAST(' || name_col ||' AS CHARACTER VARYING) ' || ', NULL, ' || lang_code_col || ' from ' || ($1) ||
				' ORDER BY ' || name_col|| ',' || lang_code_col;
		END IF;
	END IF;
    
	-- Get the first record        
	FETCH curs1 INTO id, _name, py_name, language_code;
	while id is not null LOOP
		language_id  := mid_get_language_id(language_code);
		
		curr_name_id := mid_add_one_name(curr_name_id, _name, py_name, language_id);
		
		EXECUTE 'INSERT INTO ' || temp_tbl || ' VALUES (' || id || ', ' || curr_name_id || ')';
		curr_name_id := curr_name_id + 1;
		cnt          := cnt + 1;
		-- Get the next record
		FETCH curs1 INTO id, _name, py_name, language_code;
	END LOOP;
	close curs1;

    return cnt;
END;
$$;

CREATE OR REPLACE FUNCTION mid_get_script(tbl_name character varying,  col_list character varying[], condition character varying)
    RETURNS character varying
    LANGUAGE plpgsql
    AS $$ 
DECLARE
	script character varying;
BEGIN
	if tbl_name is null THEN
		RETURN NULL;
	END IF;
	script := 'SELECT';

	col_name 
	i = 1;
	while i <= array_upper(col_list) LOOP
		if col_list[i] is not null then
			IF i = 1 then
				script := script || ' ' || col_list[i];
			else
				script := script || ', ' || col_list[i];
			end if;
		end if;
		i := i + 1;
	END LOOP;
	script := script || ' from ' || tbl_name 
	IF condition is not null then 
		script := script || ' where ' || condition;
	end if;
	
	return script;
END;
$$;


CREATE OR REPLACE FUNCTION mid_cnv_link_type( 
	intersection_category integer,
	ramp char,
	frontage char,
	multi_digitized char,
	sa_link_id integer
)
  RETURNS smallint 
  LANGUAGE plpgsql VOLATILE
  AS
$BODY$
BEGIN

    IF intersection_category = 4 THEN
    	return 0;
    ELSEIF ramp = 'Y' THEN
    	return 5;
    ELSEIF intersection_category = 1 THEN
    	return 4;
    ELSEIF intersection_category = 2 THEN
    	return 8;
    ELSEIF intersection_category = 3 THEN
    	return 9;
	ELSEIF sa_link_id is not null THEN
		return 7;
	ELSEIF frontage = 'Y' THEN
    	return 6;
    ELSEIF multi_digitized = 'Y' THEN
    	return 2;
    ELSE
    	return 1;
    END IF;
    
END;
$BODY$;
  
CREATE OR REPLACE FUNCTION mid_cnv_display_class( 
	private char,
	boat_ferry char,
	rail_ferry char,
	link_id_pedestrians integer,
	link_id_nothrough integer,
	functional_class integer,
	controlled_access char,
	public_access char,
	speed_category integer,
	four_wheel_drive char,
	buslane bigint
)
  RETURNS smallint 
  LANGUAGE plpgsql VOLATILE
  AS
$BODY$
BEGIN
    IF boat_ferry = 'Y' or rail_ferry = 'Y' THEN 
    	return 14;
    ELSEIF four_wheel_drive = 'Y' THEN
    	return 19;	
    ELSEIF buslane is not null THEN
    	return 20;		
    ELSEIF link_id_pedestrians is not null OR (private = 'Y' AND public_access = 'N') THEN
    	return 3;
    ELSEIF controlled_access = 'Y' THEN
    	return 12; 
    ELSEIF functional_class in (1, 2) THEN 
    	return 11;
    ELSEIF functional_class = 3 THEN 
    	return 7;
    ELSEIF functional_class = 4 THEN 
    	return 6;
    ELSEIF functional_class = 5 and speed_category <= 7 THEN 
    	return 5;
    ELSE
    	return 4;
    END if;
END;
$BODY$;

CREATE OR REPLACE FUNCTION mid_cnv_road_type(
	frontage char, 
	private char,
	boat_ferry char,
	rail_ferry char,
	link_id_nothrough integer,
	carpool_road char,
	link_id_public integer,
	link_id_pedestrians integer,
	functional_class integer,
	controlled_access char,
	ramp char,
	public_access char
)
  RETURNS smallint 
  LANGUAGE plpgsql VOLATILE
  AS
$BODY$
DECLARE
	roadtype smallint;
BEGIN
	
    IF boat_ferry = 'Y' THEN 
    	return 10;
	ELSEIF rail_ferry = 'Y' THEN 
    	return 11;
    ELSEIF private = 'Y' AND public_access = 'N' THEN 
    	return 7;
    ELSEIF link_id_public is not null THEN 
    	return 12;
    ELSEIF link_id_pedestrians is not null THEN
    	return 8;
    ELSEIF link_id_nothrough is not null THEN
		return 14;
	ELSEIF controlled_access = 'Y' THEN
		return 0;
    ---ELSEIF frontage = 'Y' THEN 
    ---	return 5;
    ELSEIF carpool_road = 'Y' THEN 
    	return 13;
    ELSEIF functional_class in (1, 2) THEN 
    	return 2;
    ELSEIF functional_class = 3 THEN 
    	return 3;
    ELSEIF functional_class = 4 THEN 
    	return 4;
    ELSE
    	return 6;
    END if;

END;
$BODY$;


CREATE OR REPLACE FUNCTION mid_get_lane_dir(
	travel_direction char, 
	cons_link_id integer,
	linkdir smallint, 
	linkdir_array smallint[])
  RETURNS smallint 
  LANGUAGE plpgsql VOLATILE
  AS
$$
DECLARE
	b integer;
BEGIN

	b = 0;
	if 2 = any(linkdir_array) and 3 = any(linkdir_array)  then
		b = 1;
	elsif 2 = any(linkdir_array) then
		b = 2;
	elsif 3 = any(linkdir_array) then
		b = 3;
	end if;

	RETURN CASE
		WHEN cons_link_id is not null or linkdir = 1 THEN 3
		WHEN travel_direction = 'F' and (b = 2 or b = 0) THEN 1
		WHEN travel_direction = 'T' and (b = 3 or b = 0) THEN 2
		WHEN travel_direction = 'B' 
			or b = 1 
			or (travel_direction = 'F' and b = 3)
			or (travel_direction = 'T' and b = 2)
			THEN 3
		ELSE 0
	END;	
	
END;
$$;


CREATE OR REPLACE FUNCTION mid_get_lanecount_downflow(one_way smallint, from_ref_num_lanes integer, to_ref_num_lanes integer, physical_num_lanes integer)
  RETURNS smallint 
  LANGUAGE plpgsql VOLATILE
  AS 
$$
  DECLARE 
	rtnValue   smallint;
BEGIN
	IF one_way = 3 then 
		RETURN 0;
	ELSE	
		IF physical_num_lanes is null THEN
			IF from_ref_num_lanes is not null AND to_ref_num_lanes is null THEN 
				rtnValue = from_ref_num_lanes;
			ELSIF from_ref_num_lanes is null AND to_ref_num_lanes is not null THEN 
				rtnValue = to_ref_num_lanes;
			ELSIF from_ref_num_lanes is not null AND to_ref_num_lanes is not null THEN 
				rtnValue = from_ref_num_lanes + to_ref_num_lanes;
			ELSE 
				rtnValue = 0;
			END IF;
		ELSE
			rtnValue = physical_num_lanes;
		END IF;
	END IF;

	IF rtnValue > 15 THEN 
		RETURN 15;
	ELSE
		RETURN rtnValue;
	END IF;
END;
$$;


CREATE OR REPLACE FUNCTION mid_get_lanecount_upflow(one_way smallint, from_ref_num_lanes integer, to_ref_num_lanes integer, physical_num_lanes integer)
  RETURNS smallint  
  LANGUAGE plpgsql VOLATILE
AS
$$
  DECLARE 
	rtnValue   smallint;
BEGIN
	IF one_way = 2 then 
		RETURN 0;
	ELSE	
		IF physical_num_lanes is null THEN
			IF from_ref_num_lanes is not null AND to_ref_num_lanes is null THEN 
				rtnValue = from_ref_num_lanes;
			ELSIF from_ref_num_lanes is null AND to_ref_num_lanes is not null THEN 
				rtnValue = to_ref_num_lanes;
			ELSIF from_ref_num_lanes is not null AND to_ref_num_lanes is not null THEN 
				rtnValue = from_ref_num_lanes + to_ref_num_lanes;
			ELSE 
				rtnValue = 0;
			END IF;
		ELSE
			rtnValue = physical_num_lanes;
		END IF;
	END IF;

	IF rtnValue > 15 THEN 
		RETURN 15;
	ELSE
		RETURN rtnValue;
	END IF;
END;
$$;
  

CREATE OR REPLACE FUNCTION mid_get_regulationspeed(unit character varying, speed_limit integer, speedclass integer)
	RETURNS double precision
	LANGUAGE plpgsql VOLATILE
AS 
$$
BEGIN
	IF speed_limit is not null and speed_limit > 0 and speed_limit not in (998,999) THEN
		if speed_limit > 200 then
			return -1;
		else
			return speed_limit;
		end if;
	END IF;
	
	IF unit = 'M' THEN 
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


CREATE OR REPLACE FUNCTION mid_get_speedsource(unit character varying, speed_limit integer,speed_limit_source char)
	RETURNS smallint 
    LANGUAGE plpgsql VOLATILE
	AS 
$$
BEGIN
	IF speed_limit is not null and speed_limit > 0 and speed_limit not in (998,999) THEN
		IF unit = 'M' THEN
			return ( 1 << 3) | cast(speed_limit_source as smallint);
		ELSE
			return cast(speed_limit_source as smallint);
		END IF;
	ELSE
		IF unit = 'M' THEN
			RETURN (1 << 3) | 3;
		ELSE
			return 3;
		END IF;
	END IF;
	
END;
$$;


CREATE OR REPLACE FUNCTION mid_get_oneway_code(
	travel_direction char, 
	cons_link_id integer, 
	linkdir smallint, 
	linkdir_array smallint[])
  RETURNS smallint 
  LANGUAGE plpgsql VOLATILE
  AS
$$
DECLARE
	b integer;
BEGIN
	b = 0;
	if 2 = any(linkdir_array) and 3 = any(linkdir_array)  then
		b = 1;
	elsif 2 = any(linkdir_array) then
		b = 2;
	elsif 3 = any(linkdir_array) then
		b = 3;
	end if;

	RETURN CASE
		WHEN cons_link_id is not null or linkdir = 1 THEN 4
		WHEN travel_direction = 'F' and (b = 2 or b = 0) THEN 2
		WHEN travel_direction = 'T' and (b = 3 or b = 0) THEN 3
		WHEN travel_direction = 'B' 
			or b = 1 
			or (travel_direction = 'F' and b = 3)
			or (travel_direction = 'T' and b = 2)
			THEN 1
		ELSE 0
	END;
END;
$$;

CREATE OR REPLACE FUNCTION mid_get_extend_flag
(
	pdm_flag smallint,
	driving_side char
)
  RETURNS smallint 
  LANGUAGE plpgsql VOLATILE
  AS
$$
DECLARE
	extend_flag smallint;
BEGIN
	extend_flag := 0;
	
	if driving_side = 'L' then
		extend_flag := extend_flag | 1;
	end if;
	
	if pdm_flag = 1 then
		extend_flag := extend_flag | 2;
	end if;
	
	return extend_flag;
END;
$$;

CREATE OR REPLACE FUNCTION mid_convert_condition_regulation_tbl()
    RETURNS smallint
    LANGUAGE plpgsql
AS $$
DECLARE
	rec				record;
	nCount			integer;
	nIndex			integer;
	cur_cond_id		integer;
	car_type		smallint;
	all_car_type	smallint;
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
BEGIN
	cur_cond_id := 0;
    all_car_type	:= (1<<13) | (1<<10);
	
	for rec in 
		select 	ad.access_id,
				a.automobiles,
				a.buses,
				a.taxis,
				a.carpools,
				a.pedestrians,
				a.trucks,
				a.deliveries,
				a.emergency_vehicles,
				a.through_traffic,
				a.motorcycles,
				
				ad.dt_id,
				d.datetime_type,
				d.from_end,
				d.exclude_date,
				d.start_date,
				d.end_date,
				d.start_time,
				d.end_time
		from
		(
	        select distinct access_id, dt_id
	        from
	        (
		        select x.access_id, y.dt_id
		        from (select * from rdf_condition where condition_type in (7,5,8,39)) as x
		        left join rdf_condition_dt as y
		        on x.condition_id = y.condition_id
				
				union
				
				select distinct(access_id) as access_id, cast(null as integer) as dt_id
				from rdf_access
	        )as t
        )as ad
        left join rdf_access as a
        on ad.access_id = a.access_id
        left join rdf_date_time as d
        on ad.dt_id = d.dt_id
        order by ad.access_id, ad.dt_id
    LOOP
    	-- car_type
    	car_type	:= mid_get_access(	rec.automobiles,
										rec.buses,
										rec.taxis,
										rec.carpools,
										rec.pedestrians,
										rec.trucks,
										rec.deliveries,
										rec.emergency_vehicles,
										rec.through_traffic,
										rec.motorcycles
									);
    	
    	-- datetime
    	if rec.dt_id is null then
    		-- dt does not exist, set default value
    		start_year		:= 0;
			start_month		:= 0;
			start_day		:= 0;
			end_year		:= 0;
			end_month		:= 0;
			end_day			:= 0;			
			day_of_week		:= 0;
			start_hour		:= 0;
			start_minute	:= 0;
			end_hour		:= 23;
			end_minute		:= 59;
			exclude_date	:= 0;
    	else
    		-- dt exists, convert value
	    	if rec.datetime_type = '1' then
	    		-- DAY OF WEEK
	    		start_year		:= 0;
				start_month		:= 0;
				start_day		:= 0;
				end_year		:= 0;
				end_month		:= 0;
				end_day			:= 0;			
				day_of_week		:= 0;
				nIndex			:= 1;
				nCount			:= 8;
				while nIndex <= nCount LOOP
					if substr(rec.start_date, nIndex, 1) = 'X' then
						day_of_week	:= day_of_week | (1 << (nIndex-1));
					end if;
					nIndex	:= nIndex + 1;
				END LOOP;
			elseif rec.datetime_type = '2' and upper(rec.start_date) = 'EASTER' then
				-- EASTER
				start_year		:= 0;
				start_month		:= 0;
				start_day		:= 0;
				end_year		:= 0;
				end_month		:= 0;
				end_day			:= 0;			
				day_of_week		:= 0;
				--day_of_week		:= day_of_week | 127;
				--day_of_week		:= day_of_week | (1 << 7);
				--day_of_week		:= day_of_week | (1 << 12);
			elseif rec.datetime_type = 'A' then
				-- DATE RANGES
				start_year		:= cast(rec.start_date as integer) / 10000;
				start_month		:= (cast(rec.start_date as integer) % 10000) / 100;
				start_day		:= cast(rec.start_date as integer) % 100;
				end_year		:= cast(rec.end_date as integer) / 10000;
				end_month		:= (cast(rec.end_date as integer) % 10000) / 100;
				end_day			:= cast(rec.end_date as integer) % 100;			
				day_of_week		:= 0;		
			elseif rec.datetime_type = 'C' then
				-- DAY OF MONTH
				start_year		:= 0;
				start_month		:= 0;
				start_day		:= cast(substr(rec.start_date, 1, 4) as integer);
				end_year		:= 0;
				end_month		:= 0;
				end_day			:= cast(substr(rec.end_date, 1, 4) as integer);			
				day_of_week		:= 0;		
			elseif rec.datetime_type = 'D' then
				-- DAY OF WEEK OF MONTH
				start_year		:= 0;
				start_month		:= 0;
				start_day		:= 0;
				end_year		:= 0;
				end_month		:= 0;
				end_day			:= 0;			
				day_of_week		:= 0;
				day_of_week		:= day_of_week | (1 << (cast(substr(rec.start_date, 1, 4) as integer) - 1));
				day_of_week		:= day_of_week | (2 << 7);
				day_of_week		:= day_of_week | (1 << (cast(substr(rec.start_date, 5, 4) as integer) + 8));
			elseif rec.datetime_type = 'E' then
				-- DAY OF WEEK OF YEAR
				start_year		:= 0;
				start_month		:= 0;
				start_day		:= 0;
				end_year		:= 0;
				end_month		:= 0;
				end_day			:= 0;			
				day_of_week		:= 0;
				day_of_week		:= day_of_week | (1 << (cast(substr(rec.start_date, 1, 4) as integer) - 1));
				day_of_week		:= day_of_week | (3 << 7);
				day_of_week		:= day_of_week | (cast(substr(rec.start_date, 5, 4) as integer) << 9);
			elseif rec.datetime_type = 'F' then
				-- WEEK OF MONTH
				start_year		:= 0;
				start_month		:= 0;
				start_day		:= 0;
				end_year		:= 0;
				end_month		:= 0;
				end_day			:= 0;			
				day_of_week		:= 0;
				day_of_week		:= day_of_week | 127;
				day_of_week		:= day_of_week | (2 << 7);
				day_of_week		:= day_of_week | (cast(substr(rec.start_date, 1, 4) as integer) << 9);
			elseif rec.datetime_type = 'H' then
				-- MONTH OF YEAR
				start_year		:= 0;
				start_month		:= cast(substr(rec.start_date, 1, 4) as integer) ;
				start_day		:= 0;
				end_year		:= 0;
				end_month		:= cast(substr(rec.end_date, 1, 4) as integer) ;
				end_day			:= 0;
				day_of_week		:= 0;
			elseif rec.datetime_type = 'I' then
				-- DAY OF MONTH OF YEAR
				start_year		:= 0;
				start_month		:= cast(substr(rec.start_date, 5, 4) as integer) ;
				start_day		:= cast(substr(rec.start_date, 1, 4) as integer) ;
				end_year		:= 0;
				end_month		:= cast(substr(rec.end_date, 5, 4) as integer) ;
				end_day			:= cast(substr(rec.end_date, 1, 4) as integer) ;
				day_of_week		:= 0;
			else
				raise EXCEPTION 'error, new datetime type of regulation can not be handled!';
	    	end if;
	    	
	    	if rec.start_time is null then
	    		start_hour		:= 0;
	    		start_minute	:= 0;
	    	else
				start_hour		:= cast(rec.start_time as integer) / 100;
				start_minute	:= cast(rec.start_time as integer) % 100;
			end if;
			
			if rec.end_time is null then
	    		end_hour		:= 23;
	    		end_minute		:= 59;
	    	else
				end_hour		:= cast(rec.end_time as integer) / 100;
				end_minute		:= cast(rec.end_time as integer) % 100;
			end if;
			
			if rec.exclude_date = 'Y' then
				exclude_date	:= 1;
			else
				exclude_date	:= 0;
			end if;
			
			-- update time format
	    	if end_hour >= 24 then
	    		end_hour		:= end_hour % 24;
	    	end if;
	    	if end_hour = 0 and end_minute = 0 then
	    		end_hour		:= 23;
	    		end_minute		:= 59;
	    	end if;
	    	
	    	if day_of_week = 127 then
	    		day_of_week = 0;
	    	end if;
    	end if;
    	
    	if car_type = 0 then
    		-- condition never be effect, set cond_id = -1
	    	insert into temp_condition_regulation_tbl
	    				("cond_id", "access_id", "dt_id")
	    		VALUES	(-1, rec.access_id, rec.dt_id);
		elseif
			(
    			(car_type = all_car_type) and
    			(day_of_week = 0) and
    			(start_year = 0) and
    			(start_month = 0) and
    			(start_day = 0) and
    			(start_hour = 0) and
    			(start_minute = 0) and
    			(end_year = 0) and
    			(end_month = 0) and
    			(end_day = 0) and
    			(end_hour = 23) and
    			(end_minute = 59) and
    			(exclude_date = 0)
			)
		then
			-- condition always be effect, set cond_id = null
	    	insert into temp_condition_regulation_tbl
	    				("cond_id", "access_id", "dt_id")
	    		VALUES	(null::integer, rec.access_id, rec.dt_id);
		else
			-- insert new cond if condition is useful
	    	cur_cond_id := cur_cond_id + 1;
	    	
	    	insert into temp_condition_regulation_tbl
	    				("cond_id", "access_id", "dt_id")
	    		VALUES	(cur_cond_id, rec.access_id, rec.dt_id);
			
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
	    end if;
    END LOOP;
    return 1;
END;
$$;

CREATE OR REPLACE FUNCTION mid_get_access(
	automobiles char,
	buses char,
	taxis char,
	carpools char,
	pedestrians char,
	trucks char,
	deliveries char,
	emergency_vehicles char,
	through_traffic char,
	motorcycles char
	)
	RETURNS smallint
    LANGUAGE plpgsql
AS $$
DECLARE
	x smallint;
BEGIN
	x = 0; 
	IF automobiles = 'Y' THEN
		x = x | (1 << 13);
	END IF;
	IF carpools = 'Y' THEN
		x = x | (1 << 10);
	END IF;
	
	--IF buses = 'Y' THEN
	--	x = x | (1 << 7);
	--END IF;
	--IF taxis = 'Y' THEN
	--	x = x | (1 << 8);
	--END IF;
	--IF pedestrians = 'Y' THEN
	--	x = x | (1 << 4);
	--END IF;
	--IF trucks = 'Y' THEN
	--	x = x | (1 << 14);
	--END IF;
	--IF deliveries = 'Y' THEN
	--	x = x | (1 << 6);
	--END IF;
	--IF emergency_vehicles = 'Y' THEN
	--	x = x | (1 << 9);
	--END IF;
	--IF motorcycles = 'Y' THEN
	--	x = x | (1 << 11);
	--END IF;
	--IF through_traffic = 'N' THEN
	--	x = x & (1 << 4);
	--END IF;
	
	return x;
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
				nodeid_array
    	from
    	(
			select	array_condid,
					linkid_array,
					nodeid_array
			from
			(
	    		select condition_id, array_agg(cond_id) as array_condid
	    		from
	    		(
	    			select condition_id, cond_id
	    			from
	    			(
		    			-- regulation with condition
						select condition_id, cond_id
						from
						(
							-- Improve query efficiency: select effect rdf_nav_strand info
					        select a.condition_id, a.access_id, a.dt_id
					        from
					        (
						        select x.condition_id, nav_strand_id, access_id, dt_id
						        from (select * from rdf_condition where condition_type = 7) as x
						        left join rdf_condition_dt as y
						        on x.condition_id = y.condition_id
					        )as a
					        inner join (select * from rdf_nav_strand where seq_num = 0) as b
					        on a.nav_strand_id = b.nav_strand_id
						)as cad
						left join temp_condition_regulation_tbl as cr
						on cad.access_id = cr.access_id and (cad.dt_id is not distinct from cr.dt_id)
						where cr.cond_id is null or cr.cond_id > 0
					)as cad2
					order by condition_id, cond_id
				)as c
				group by condition_id
			)as cc
			inner join
			(
				select	condition_id,
						array_agg(link_id) as linkid_array,
						array_agg(node_id) as nodeid_array
				from
				(
					select condition_id, link_id, node_id, seq_num
					from (select * from rdf_condition where condition_type = 7) as m
					left join rdf_nav_strand as n
					on m.nav_strand_id = n.nav_strand_id
					order by condition_id asc, seq_num asc
				)as nl
				group by condition_id
			)as cl
			on cc.condition_id = cl.condition_id
			
			-- add divider manoeuvre
			union	
			select 	ARRAY[null::integer] as array_condid,
					ARRAY[from_link_id, to_link_id] as linkid_array,
					ARRAY[node_id, null::bigint] as nodeid_array
			from rdf_condition_divider
		)as t
		order by linkid_array, nodeid_array, array_condid
		
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
	    			VALUES	(cur_regulation_id, null, rec.nodeid_array[nIndex], nIndex+1);
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
	    						 rec.nodeid_array[1], rec.linkid_array[1], rec.linkid_array[array_upper(rec.linkid_array,1)], 
	    						 1, rec.array_condid[nIndex]);
			nIndex := nIndex + 1;
		END LOOP;    	
    	
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
	nCount integer;
	nIndex integer;
BEGIN
	-- regulation_id
	select (case when max(regulation_id) is null then 0 else max(regulation_id) end)
	from regulation_relation_tbl
	into cur_regulation_id;
	
    FOR rec IN
    	select	a.linkid, a.array_linkdir, a.array_condid, b.travel_direction
    	from (
	    	select linkid, array_agg(linkdir) as array_linkdir, array_agg(cond_id) as array_condid
	    	from (
	    		select distinct linkid, linkdir, cond_id
	    		from (
				select	cond_id, link_id as linkid, bearing as linkdir
				from (
					-- regulation with condition
					select condition_id, cond_id, link_id
					from (
						select a.condition_id, a.access_id, a.dt_id, b.link_id
						from (
							select x.condition_id, nav_strand_id, access_id, dt_id
							from (select * from rdf_condition where condition_type = 5) as x
							left join rdf_condition_dt as y
								on x.condition_id = y.condition_id
							left join rdf_condition_access z
								on x.condition_id = z.condition_id
						)as a
						inner join (select * from rdf_nav_strand where seq_num = 0) as b
							on a.nav_strand_id = b.nav_strand_id
					)as cad
					left join temp_condition_regulation_tbl as cr
						on cad.access_id = cr.access_id and (cad.dt_id is not distinct from cr.dt_id)
					where cr.cond_id is null or cr.cond_id > 0
				)as cc
				left join rdf_condition_direction_travel as ct
					on cc.condition_id = ct.condition_id
			)as c
			order by linkid, linkdir, cond_id
	    	)as t
	    	group by linkid
    	)as a
    	left join temp_rdf_nav_link as b
		on a.linkid = b.link_id
    	order by a.linkid
    LOOP
		-- current regulation id
    	cur_regulation_id := cur_regulation_id + 1;
    	
    	-- insert into regulation_item_tbl
    	insert into regulation_item_tbl
    				("regulation_id", "linkid", "nodeid", "seq_num")
    		VALUES 	(cur_regulation_id, rec.linkid, null, 1);
		
		-- insert into regulation_relation_tbl
		nCount := array_upper(rec.array_condid, 1);
		nIndex := 1;
		-- bearing(linkdir): 1-From Reference Node/ 2-To Reference Node/ 3-Both Directions
		-- travel_direction: B-Both Directions/ F-From Reference Node/ T-To Reference Node
		while nIndex <= nCount LOOP
			if rec.travel_direction = 'F' then
				if rec.array_linkdir[nIndex] = 1 then
					--pass
				elseif rec.array_linkdir[nIndex] = 2 then
		    		insert into regulation_relation_tbl
		    					("regulation_id", "nodeid", "inlinkid", "outlinkid", "condtype", "cond_id")
		    			VALUES 	(cur_regulation_id, null, rec.linkid, null, 42, rec.array_condid[nIndex]);
		    		insert into regulation_relation_tbl
		    					("regulation_id", "nodeid", "inlinkid", "outlinkid", "condtype", "cond_id")
		    			VALUES 	(cur_regulation_id, null, rec.linkid, null, 3, rec.array_condid[nIndex]);
				else--if rec.array_linkdir[nIndex] = 3 then
		    		insert into regulation_relation_tbl
		    					("regulation_id", "nodeid", "inlinkid", "outlinkid", "condtype", "cond_id")
		    			VALUES 	(cur_regulation_id, null, rec.linkid, null, 3, rec.array_condid[nIndex]);
				end if;
			elseif rec.travel_direction = 'T' then
				if rec.array_linkdir[nIndex] = 1 then
		    		insert into regulation_relation_tbl
		    					("regulation_id", "nodeid", "inlinkid", "outlinkid", "condtype", "cond_id")
		    			VALUES 	(cur_regulation_id, null, rec.linkid, null, 43, rec.array_condid[nIndex]);
		    		insert into regulation_relation_tbl
		    					("regulation_id", "nodeid", "inlinkid", "outlinkid", "condtype", "cond_id")
		    			VALUES 	(cur_regulation_id, null, rec.linkid, null, 2, rec.array_condid[nIndex]);
				elseif rec.array_linkdir[nIndex] = 2 then
					--pass
				else--if rec.array_linkdir[nIndex] = 3 then
		    		insert into regulation_relation_tbl
		    					("regulation_id", "nodeid", "inlinkid", "outlinkid", "condtype", "cond_id")
		    			VALUES 	(cur_regulation_id, null, rec.linkid, null, 2, rec.array_condid[nIndex]);
				end if;
			else--if rec.travel_direction = 'B' then
				if rec.array_linkdir[nIndex] = 1 then
		    		insert into regulation_relation_tbl
		    					("regulation_id", "nodeid", "inlinkid", "outlinkid", "condtype", "cond_id")
		    			VALUES 	(cur_regulation_id, null, rec.linkid, null, 43, rec.array_condid[nIndex]);
				elseif rec.array_linkdir[nIndex] = 2 then
		    		insert into regulation_relation_tbl
		    					("regulation_id", "nodeid", "inlinkid", "outlinkid", "condtype", "cond_id")
		    			VALUES 	(cur_regulation_id, null, rec.linkid, null, 42, rec.array_condid[nIndex]);
				else--if rec.array_linkdir[nIndex] = 3 then
					--pass
				end if;
			end if;
    		nIndex := nIndex + 1;
    	END LOOP;
    END LOOP;
    return 1;
END;
$$;

CREATE OR REPLACE FUNCTION mid_convert_regulation_through_link()
    RETURNS smallint
    LANGUAGE plpgsql
AS $$
DECLARE
	rec record;
	cur_regulation_id integer;
	nCount integer;
	nIndex integer;
BEGIN
	-- regulation_id
	select (case when max(regulation_id) is null then 0 else max(regulation_id) end)
	from regulation_relation_tbl
	into cur_regulation_id;
	
    FOR rec IN
    	select	linkid, is_seasonal, 
    			array_agg(cond_id) as array_condid
    	from
    	(
    		select distinct linkid, is_seasonal, cond_id
    		from
    		(
				-- regulation with condition
				select condition_id, cond_id, link_id as linkid, is_seasonal
				from
				(
			        select a.condition_id, a.access_id, a.dt_id, b.link_id, a.is_seasonal
			        from
			        (
				        select 	x.condition_id, nav_strand_id, access_id, dt_id, 
				        		(case when z.seasonal_closure = 'Y' then true else false end) as is_seasonal
				        from (select * from rdf_condition where condition_type = 8) as x
				        left join rdf_condition_dt as y
				        on x.condition_id = y.condition_id
						left join rdf_condition_access as z
						on x.condition_id = z.condition_id
			        )as a
			        inner join (select * from rdf_nav_strand where seq_num = 0) as b
			        on a.nav_strand_id = b.nav_strand_id
				)as cad
				left join temp_condition_regulation_tbl as cr
				on cad.access_id = cr.access_id and (cad.dt_id is not distinct from cr.dt_id)
				where cr.cond_id is null or cr.cond_id > 0
			)as c
			order by linkid, is_seasonal, cond_id
    	)as t
    	group by linkid, is_seasonal
    	order by linkid, is_seasonal
    LOOP
		-- current regulation id
    	cur_regulation_id := cur_regulation_id + 1;
    	
    	-- insert into regulation_item_tbl
    	insert into regulation_item_tbl("regulation_id", "linkid", "nodeid", "seq_num")
    							VALUES (cur_regulation_id, rec.linkid, null, 1);
		
    	-- insert into regulation_relation_tbl
		nCount := array_upper(rec.array_condid, 1);
		nIndex := 1;
		while nIndex <= nCount LOOP
    		insert into regulation_relation_tbl
    					("regulation_id", "nodeid", "inlinkid", "outlinkid", "condtype", "is_seasonal", "cond_id")
    			VALUES	(cur_regulation_id, null, rec.linkid, null, 4, rec.is_seasonal, rec.array_condid[nIndex]);
    		nIndex := nIndex + 1;
    	END LOOP;
    END LOOP;
    return 1;
END;
$$;

CREATE OR REPLACE FUNCTION mid_convert_regulation_pdm()
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
				nodeid_array
		from
		(
    		select condition_id, array_agg(cond_id) as array_condid
    		from
    		(
    			select condition_id, cond_id
    			from
    			(
	    			-- regulation with condition
					select condition_id, cond_id
					from
					(
				        select a.condition_id, a.access_id, a.dt_id
				        from
				        (
					        select x.condition_id, nav_strand_id, access_id, dt_id
					        from (select * from rdf_condition where condition_type = 39) as x
					        left join rdf_condition_dt as y
					        on x.condition_id = y.condition_id
				        )as a
				        inner join (select * from rdf_nav_strand where seq_num = 0) as b
				        on a.nav_strand_id = b.nav_strand_id
					)as cad
					left join temp_condition_regulation_tbl as cr
					on cad.access_id = cr.access_id and (cad.dt_id is not distinct from cr.dt_id)
					where cr.cond_id is null or cr.cond_id > 0
				)as cad2
				order by condition_id, cond_id
			)as c
			group by condition_id
		)as cc
		inner join
		(
			select	condition_id,
					array_agg(link_id) as linkid_array,
					array_agg(node_id) as nodeid_array
			from
			(
				select condition_id, link_id, node_id, seq_num
				from (select * from rdf_condition where condition_type = 39) as m
				left join rdf_nav_strand as n
				on m.nav_strand_id = n.nav_strand_id
				order by condition_id asc, seq_num asc
			)as nl
			group by condition_id
			having count(*) > 1 -- mainnode u-turn, and one link u-turn is thrown away
		)as cl
		on cc.condition_id = cl.condition_id
		order by cc.condition_id
		
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
	    			VALUES	(cur_regulation_id, null, rec.nodeid_array[nIndex], nIndex+1);
	    	else
	    		insert into regulation_item_tbl
	    					("regulation_id", "linkid", "nodeid", "seq_num")
	    			VALUES	(cur_regulation_id, rec.linkid_array[nIndex], null, nIndex+1);
	    	end if;
			nIndex := nIndex + 1;
		END LOOP;
		
		-- for one link u-turn case
		--if nCount = 1 then
		--    insert into regulation_item_tbl
		--			("regulation_id", "linkid", "nodeid", "seq_num")
		--	VALUES	(cur_regulation_id, rec.linkid_array[1], null, 3);
		--end if;
		
	   	-- insert into regulation_relation_tbl
		nCount := array_upper(rec.array_condid, 1);
		nIndex := 1;
		while nIndex <= nCount LOOP
	    	insert into regulation_relation_tbl
	    						("regulation_id", "nodeid", "inlinkid", "outlinkid", "condtype", "cond_id")
	    				VALUES	(cur_regulation_id, 
	    						 rec.nodeid_array[1], rec.linkid_array[1], rec.linkid_array[array_upper(rec.linkid_array,1)], 
	    						 10, rec.array_condid[nIndex]);
			nIndex := nIndex + 1;
		END LOOP;    	
    	
    END LOOP;
    return 1;
END;
$$;

CREATE OR REPLACE FUNCTION mid_convert_regulation_access_link()
    RETURNS smallint
    LANGUAGE plpgsql
AS $$
DECLARE
	rec record;
	cur_regulation_id integer;
	nCount integer;
	nIndex integer;
BEGIN
	-- regulation_id
	select (case when max(regulation_id) is null then 0 else max(regulation_id) end)
	from regulation_relation_tbl
	into cur_regulation_id;
	
    FOR rec IN
    		select linkid, cond_id
			from
			(
				-- RDF_NAV_LINK.ACCESS_ID identifies what vehicle types can use a link.
				-- as follows identifies what vehicle types can not use a link.(as opposite_access_id is said)
				select link_id as linkid, opposite_access_id, cast(null as integer) as dt_id
				from rdf_nav_link as m
		        left join
		        (
		        	select a.access_id as access_id, b.access_id as opposite_access_id
		        	from rdf_access as a, rdf_access as b
		        	where	a.automobiles != b.automobiles and
		        			a.buses != b.buses and
		        			a.taxis != b.taxis and
		        			a.carpools != b.carpools and
		        			a.pedestrians != b.pedestrians and
		        			a.trucks != b.trucks and
		        			a.deliveries != b.deliveries and
		        			a.emergency_vehicles != b.emergency_vehicles and
		        			a.through_traffic != b.through_traffic and
		        			a.motorcycles != b.motorcycles
		        )as n
		        on m.access_id = n.access_id
			)as cad
			left join temp_condition_regulation_tbl as cr
			on cad.opposite_access_id = cr.access_id and (cad.dt_id is not distinct from cr.dt_id)
			where cr.cond_id is null or cr.cond_id > 0
			order by linkid asc
    LOOP
		-- current regulation id
    	cur_regulation_id := cur_regulation_id + 1;
    	
    	-- insert into regulation_item_tbl
    	insert into regulation_item_tbl("regulation_id", "linkid", "nodeid", "seq_num")
    							VALUES (cur_regulation_id, rec.linkid, null, 1);
		
    	-- insert into regulation_relation_tbl
		insert into regulation_relation_tbl
					("regulation_id", "nodeid", "inlinkid", "outlinkid", "condtype", "cond_id")
			VALUES	(cur_regulation_id, null, rec.linkid, null, 4, rec.cond_id);
    END LOOP;
    return 1;
END;
$$;

CREATE OR REPLACE FUNCTION mid_convert_regulation_awr()
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
    	select	a.linkid_array, a.node_id
    	from
    	(
    		-- mainnode uturn
	    	select 	(ARRAY[in_link::bigint] || string_to_array(inner_path, ',')::bigint[] || ARRAY[out_link::bigint]) as linkid_array,
	    			in_node as node_id
	    	from
	    	(
		    	select 	*, 
		    			unnest(mid_find_awr_inner_path(in_link, out_link, in_node, out_node)) as inner_path
		    	from temp_awr_mainnode_uturn
	    	)as t
	    	where (inner_path is not null) and (inner_path != '')
	    	
	    	-- node uturn
	    	union
	    	select ARRAY[in_link, out_link] as linkid_array, node_id
	    	from temp_awr_node_uturn
    	)as a
    	left join temp_awr_pdm_linkrow as b
    	on 	b.first_link = ANY(a.linkid_array) 
    		and 
    		rdb_contains(a.linkid_array, b.link_array) is true
    	where b.regulation_id is null
    
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
	    			VALUES	(cur_regulation_id, null, rec.node_id, nIndex+1);
	    	else
	    		insert into regulation_item_tbl
	    					("regulation_id", "linkid", "nodeid", "seq_num")
	    			VALUES	(cur_regulation_id, rec.linkid_array[nIndex], null, nIndex+1);
	    	end if;
			nIndex := nIndex + 1;
		END LOOP;
		
	   	-- insert into regulation_relation_tbl
    	insert into regulation_relation_tbl
    						("regulation_id", "nodeid", "inlinkid", "outlinkid", "condtype", "cond_id")
    				VALUES	(cur_regulation_id, 
    						 rec.node_id, rec.linkid_array[1], rec.linkid_array[array_upper(rec.linkid_array,1)], 
    						 1, null);
    END LOOP;
    return 1;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_get_angle_diff(angle1 float, angle2 float)
    RETURNS float
    LANGUAGE plpgsql
AS $$
DECLARE
	angle_diff float;
BEGIN
	angle_diff = abs(angle1 - angle2);
	if angle_diff > 180 then
		angle_diff = 360 - angle_diff;
	end if;
	return angle_diff;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_contains(big_link_array bigint[], small_link_array bigint[])
    RETURNS boolean
    LANGUAGE plpgsql
AS $$
DECLARE
	nCount 		integer;
	nStartIndex integer;
BEGIN
	select nIndex, big_link_array[nIndex] as link_id
	from
	(
	select generate_series(1, array_upper(big_link_array, 1)) as nIndex
	)as t
	where big_link_array[nIndex] = small_link_array[1]
	into nStartIndex;
	
	if (big_link_array[nStartIndex: nStartIndex + array_upper(small_link_array,1) - 1]) = small_link_array then
		return true;
	else
		return false;
	end if;
END;
$$;

CREATE OR REPLACE FUNCTION mid_convert_regulation_nation_boundary()
    RETURNS smallint
    LANGUAGE plpgsql
AS $$
DECLARE
	rec record;
	cur_regulation_id integer;
	nCount integer;
	nIndex integer;
BEGIN
	-- regulation_id
	select (case when max(regulation_id) is null then 0 else max(regulation_id) end)
	from regulation_relation_tbl
	into cur_regulation_id;
	
    FOR rec IN
    		select linkid_array, nodeid_array, cast(null as integer) as cond_id
			from
			(
				select ARRAY[b.link_id, c.link_id] as linkid_array, ARRAY[a.node_id] as nodeid_array
				from temp_node_nation_boundary as a
				inner join temp_rdf_nav_link_traffic as b
				on 	(
						(a.node_id = b.ref_node_id) 
						and 
						b.one_way in (1,3)
					)
					or  
					(
						(a.node_id = b.nonref_node_id) 
						and 
						b.one_way in (1,2)
					)
				inner join temp_rdf_nav_link_traffic as c
				on 	(
						(a.node_id = c.ref_node_id) 
						and 
						c.one_way in (1,2)
					)
					or  
					(
						(a.node_id = c.nonref_node_id) 
						and 
						c.one_way in (1,3)
					)
				where b.iso_country_code != c.iso_country_code
			) as t
			order by linkid_array, nodeid_array asc
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
	    			VALUES	(cur_regulation_id, null, rec.nodeid_array[nIndex], nIndex+1);
	    	else
	    		insert into regulation_item_tbl
	    					("regulation_id", "linkid", "nodeid", "seq_num")
	    			VALUES	(cur_regulation_id, rec.linkid_array[nIndex], null, nIndex+1);
	    	end if;
			nIndex := nIndex + 1;
		END LOOP;
		
	   	-- insert into regulation_relation_tbl
    	insert into regulation_relation_tbl
    						("regulation_id", "nodeid", "inlinkid", "outlinkid", "condtype", "cond_id")
    				VALUES	(cur_regulation_id, 
    						 rec.nodeid_array[1], rec.linkid_array[1], rec.linkid_array[array_upper(rec.linkid_array,1)], 
    						 11, rec.cond_id);
    END LOOP;
    return 1;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_get_link_angle(link_shape geometry, dir boolean) 
  RETURNS double precision
  LANGUAGE plpgsql
AS $$
DECLARE
	nPointNum		integer;
	nStartIndex		integer;
	nEndIndex		integer;
	nStep			integer;
	point_a			geometry;
	point_b			geometry;
	deata_x			double precision;
	deata_y			double precision;
BEGIN
	nPointNum	= st_numpoints(link_shape);
	if nPointNum < 2 then
		return null;
	end if;
	
	if dir is true then
		point_a		= st_startpoint(link_shape);
		nStartIndex	= 1;
		nEndIndex	= nPointNum;
		nStep		= 1;
	else
		point_a		= st_endpoint(link_shape);
		nStartIndex	= nPointNum;
		nEndIndex	= 1;
		nStep		= -1;
	end if;

	while true loop
		nStartIndex	:= nStartIndex + nStep;
		
		point_b		:= st_pointn(link_shape, nStartIndex);
		deata_x		:= st_x(point_b)-st_x(point_a);
		deata_y		:= st_y(point_b)-st_y(point_a);
		
		if nStartIndex = nEndIndex then
			exit;
		end if;
		
		if abs(deata_x) + abs(deata_y) > 0.00009 then
			exit;
		end if;
	end loop;
	
	if deata_x = 0 and deata_y = 0 then
		return null;
	else
		return (atan2(deata_y, deata_x) * 180.0 / pi());
	end if;
END;
$$;

create or replace function mid_find_awr_mainnode()
returns integer
language plpgsql
as $$
declare
	rec record;
	mainnode_id integer;
	search_link bigint;
	link_array bigint[];
	node_array bigint[];
	next_link_array bigint[];
	s_node_array bigint[];
	e_node_array bigint[];
begin
	mainnode_id	:= 0;
	for rec in select * from temp_regulation_admin_link where bInner is true
	loop
		-- check
		select sublink 
		from temp_awr_mainnode_sublink 
		where sublink = rec.link_id
		into search_link;

		if FOUND then
			continue;
		end if;
		
		--
		mainnode_id	:= mainnode_id + 1;

		link_array	:= ARRAY[rec.link_id];
		node_array	:= ARRAY[rec.s_node, rec.e_node];

		while True loop
			--raise INFO 'search node_array=%', node_array;
			select array_agg(link_id), array_agg(s_node), array_agg(e_node)
			from
			(
				select link_id, s_node, e_node
				from temp_regulation_admin_link
				where 		(s_node = ANY(node_array) or e_node = ANY(node_array)) 
						and (bInner is true)
						and not (link_id = ANY(link_array))
			)as t
			into next_link_array, s_node_array, e_node_array;

			if FOUND and next_link_array is not null then
				--raise INFO 'search next_link_array=%', next_link_array;
				select array_agg(link_id)
				from
				(
					select unnest(link_array) as link_id
					union
					select unnest(next_link_array) as link_id
				)as t
				into link_array;

				select array_agg(node_id)
				from
				(
					select unnest(node_array) as node_id
					union
					select unnest(s_node_array) as node_id
					union
					select unnest(e_node_array) as node_id
				)as t
				into node_array;
			else
				insert into temp_awr_mainnode_sublink(mainnode_id, sublink)
				select mainnode_id, unnest(link_array);

				insert into temp_awr_mainnode_subnode(mainnode_id, subnode)
				select mainnode_id, unnest(node_array);
				
				exit;
			end if;
		end loop;
	end loop;
	return 0;
end;
$$;

create or replace function mid_find_awr_inner_path(inlink bigint, outlink bigint, innode bigint, outnode bigint)
returns varchar[]
language plpgsql
as $$
declare
	rstPathArray		varchar[];
	rstPathCount		integer;
	rstPath				varchar;
	
	tmpPathArray		varchar[];
	tmpLastNodeArray	bigint[];
	tmpLastLinkArray	bigint[];
	tmpPathCount		integer;
	tmpPathIndex		integer;
	
	rec        			record;
begin	
	--rstPathArray
	rstPathCount		:= 0;
	tmpPathArray		:= ARRAY[''];
	tmpLastNodeArray	:= ARRAY[innode];
	tmpLastLinkArray	:= ARRAY[inlink];
	tmpPathCount		:= 1;
	tmpPathIndex		:= 1;

	-- search
	WHILE tmpPathIndex <= tmpPathCount LOOP
		--raise INFO '%', cast(tmpPathArray as varchar);
		for rec in 
			 select nextroad, dir, nextnode, bInner
			 from
			 (
				 select link_id as nextroad, '(2)' as dir, e_node as nextnode, bInner
				 from temp_regulation_admin_link
				 where (s_node = tmpLastNodeArray[tmpPathIndex]) and (travel_direction in ('B','F'))
				 union
				 select link_id as nextroad, '(3)' as dir, s_node as nextnode, bInner
				 from temp_regulation_admin_link
				 where (e_node = tmpLastNodeArray[tmpPathIndex]) and (travel_direction in ('B','T'))
			 )as a
			 where (nextroad != tmpLastLinkArray[tmpPathIndex])	-- can not one-link-u-turn
			 order by nextroad, dir, nextnode
		loop
			if (rec.nextroad = outlink) and (tmpLastNodeArray[tmpPathIndex] = outnode) then
				rstPathCount		:= rstPathCount + 1;
				rstPath				:= tmpPathArray[tmpPathIndex];
				rstPath				:= replace(rstPath, '(2)', '');
				rstPath				:= replace(rstPath, '(3)', '');
				if rstPathCount = 1 then
					rstPathArray	:= ARRAY[rstPath];
				else
					rstPathArray	:= array_append(rstPathArray, rstPath);
				end if;
				continue;
			end if;
			
			if 	(rec.nextroad not in (inlink, outlink))
				and (rec.bInner is true)
				and not ((rec.nextroad||rec.dir) = ANY(regexp_split_to_array(tmpPathArray[tmpPathIndex], E',+'))) 
			then
				tmpPathCount		:= tmpPathCount + 1;
				if tmpPathArray[tmpPathIndex] = '' then
					tmpPathArray	:= array_append(tmpPathArray, cast(rec.nextroad||rec.dir as varchar));
				else
					tmpPathArray	:= array_append(tmpPathArray, cast(tmpPathArray[tmpPathIndex]||','||rec.nextroad||rec.dir as varchar));
				end if;
				tmpLastNodeArray	:= array_append(tmpLastNodeArray, cast(rec.nextnode as bigint));
				tmpLastLinkArray	:= array_append(tmpLastLinkArray, cast(rec.nextroad as bigint));
			end if;
		end loop;
		tmpPathIndex := tmpPathIndex + 1;
	END LOOP;

	return rstPathArray;
end;
$$;

CREATE OR REPLACE FUNCTION mid_make_link_name()
    RETURNS integer
    LANGUAGE plpgsql
    AS $$ 
DECLARE
	cnt integer;
	curs1 refcursor;
	curr_link_id integer;
	pre_link_id integer;
	name_id integer;
	is_roadsign character varying;
	seq_nm smallint;
	name_type smallint;
	temp_function_class smallint;
	temp_country_code character varying;
	temp_hierarchy_id integer;
BEGIN
	cnt := 0;
	pre_link_id  := 0;
	
	OPEN curs1 FOR SELECT a.link_id, new_name_id, functional_class, is_name_on_roadsign, c.iso_country_code, case when d.order1_id is not null then d.order1_id else admin_place_id end
			      FROM rdf_road_link as a
			      LEFT JOIN temp_road_name as b
			      ON a.road_name_id = b.road_name_id
			      LEFT JOIN temp_rdf_nav_link as c
			      ON a.link_id = c.link_id
			      left join rdf_admin_hierarchy as d
			      ON right_admin_place_id = d.admin_place_id
			      where a.road_name_id is not null
				    and is_junction_name <> 'Y'    -- not junction name
				    and is_exit_name <> 'Y'    -- not exit name
				    and explicatable <> 'N'    -- Not Explicatable names are published to assist in destination selection but should never be used during route guidance.
				    and is_postal_name <> 'Y'  -- Postal Names can be used to assist in destination selection but should never be used for route guidance or map display
				    and a.road_name_id in (    
				    SELECT road_name_id    
				      FROM rdf_road_name
				      WHERE route_type is null  -- road name
				    )
				    and a.link_id in (
				    SELECT link_id
				      FROM rdf_nav_link
				    )
				    and a.link_id is not null
			      order by link_id, is_name_on_roadsign desc, is_vanity_name, is_scenic_name, a.road_name_id;
	
	-- Get the first record        
	FETCH curs1 INTO curr_link_id, name_id, temp_function_class, is_roadsign, temp_country_code, temp_hierarchy_id;
	while curr_link_id is not null LOOP
		IF curr_link_id <> pre_link_id THEN 
			seq_nm := 1;
			pre_link_id := curr_link_id;
		ELSE 
			seq_nm := seq_nm + 1;
		END IF;

		IF is_roadsign = 'Y' THEN -- Official Name (RDF_ROAD_LINK.IS_NAME_ON_ROADSIGN = Y)
			name_type := 1;
		ELSE
			name_type := 2;
		END IF;
		
		INSERT INTO link_name_relation_tbl(
			    "link_id", "name_id", "scale_type", "seq_nm", "name_type", "country_code", "hierarchy_id")
		    VALUES (curr_link_id, name_id, temp_function_class, seq_nm, name_type, temp_country_code, temp_hierarchy_id);

		cnt  := cnt + 1;
		-- Get the next record
		FETCH curs1 INTO curr_link_id, name_id, temp_function_class, is_roadsign, temp_country_code, temp_hierarchy_id;
	END LOOP;
	close curs1;
	
	return cnt;
END;
$$;


CREATE OR REPLACE FUNCTION mid_make_link_number()
    RETURNS integer
    LANGUAGE plpgsql
    AS $$ 
DECLARE
	cnt integer;
	curs1 refcursor;
	curr_link_id integer;
	pre_link_id integer;
	name_id integer;
	is_roadsign character varying;
	seq_nm smallint;
	temp_route_type smallint;
	temp_function_class smallint;
	temp_country_code character varying;
	temp_hierarchy_id integer;
BEGIN
	cnt := 0;
	pre_link_id  := 0;
	
	OPEN curs1 FOR SELECT a.link_id, new_name_id, functional_class, route_type, is_name_on_roadsign, d.iso_country_code, (case when e.order1_id is not null then e.order1_id else admin_place_id end) as hierarchy_id
			      FROM rdf_road_link as a
			      LEFT JOIN temp_road_name as b
			      ON a.road_name_id = b.road_name_id
			      LEFT JOIN rdf_road_name AS c
			      ON a.road_name_id = c.road_name_id
			      LEFT JOIN temp_rdf_nav_link as d
			      ON a.link_id = d.link_id
			      left join rdf_admin_hierarchy as e
			      ON right_admin_place_id = e.admin_place_id
			      where a.road_name_id is not null
				    and is_junction_name <> 'Y'    -- not junction name
				    and is_exit_name <> 'Y'    -- not exit name
				    and explicatable <> 'N'    -- Not Explicatable names are published to assist in destination selection but should never be used during route guidance.
				    and is_postal_name <> 'Y'  -- Postal Names can be used to assist in destination selection but should never be used for route guidance or map display
				    and a.road_name_id in (      
				    SELECT road_name_id    
				      FROM rdf_road_name       -- road number
				      WHERE route_type is not null and name_type = 'B'
				    )
				    and a.link_id is not null
			      order by link_id, is_name_on_roadsign desc, is_vanity_name, is_scenic_name, a.road_name_id;
	
	-- Get the first record        
	FETCH curs1 INTO curr_link_id, name_id, temp_function_class, temp_route_type, is_roadsign, temp_country_code, temp_hierarchy_id;
	while curr_link_id is not null LOOP
		IF curr_link_id <> pre_link_id THEN 
			seq_nm := 1;
			pre_link_id := curr_link_id;
		ELSE 
			seq_nm := seq_nm + 1;
		END IF;
		
		INSERT INTO link_name_relation_tbl(
			    "link_id", "name_id", "scale_type", "shield_id", "seq_nm", "name_type", "country_code", "hierarchy_id")
		    VALUES (curr_link_id, name_id, temp_function_class, temp_route_type, seq_nm, 3, temp_country_code, temp_hierarchy_id);

		cnt  := cnt + 1;
		-- Get the next record
		FETCH curs1 INTO curr_link_id, name_id, temp_function_class, temp_route_type, is_roadsign, temp_country_code, temp_hierarchy_id;
	END LOOP;
	close curs1;
	
	return cnt;
END;
$$;

CREATE OR REPLACE FUNCTION mid_get_arrowinfo(direction_category integer)
RETURNS smallint AS
$$
    BEGIN
	    RETURN CASE
	        WHEN direction_category = 1 THEN 0
	        WHEN direction_category = 2 THEN 45
	        WHEN direction_category = 4 THEN 90
	    	WHEN direction_category = 8 THEN 135
	        WHEN direction_category = 16 THEN 180
	        WHEN direction_category = 32 THEN 225
	        WHEN direction_category = 64 THEN 270
	        WHEN direction_category = 128 THEN 315
	    	WHEN direction_category = 256 THEN 0
	    	WHEN direction_category = 512 THEN 0
	    	WHEN direction_category = 1024 THEN 0
	    	WHEN direction_category = 2048 THEN 180
	    	WHEN direction_category = 4096 THEN 90
	    	WHEN direction_category = 8192 THEN 225
	        ELSE null
	    END;
    END;
$$
LANGUAGE plpgsql VOLATILE
COST 100;
ALTER FUNCTION mid_get_arrowinfo(integer) OWNER TO postgres;

CREATE OR REPLACE FUNCTION mid_make_temp_lane_nav()
  RETURNS smallint AS
$$
DECLARE
	cur refcursor;
	rec record;
	lane_nav_strand_id_o integer;
	c integer;
	inlinkid bigint;
	nodeid bigint;
	outlinkid bigint;
	passlink_cnt integer;
	passlid character varying(512);
	laneid bigint;
	midlinkids1 character varying(512);
	midlinkids2 character varying(512);
	conditionid integer;
BEGIN
    open cur for
    	SELECT lane_nav_strand_id, seq_num, condition_id, lane_id, node_id, link_id
  		FROM temp_lane_nav_strand
    	order by lane_nav_strand_id, seq_num;
	
	lane_nav_strand_id_o := 0;
	c := 0;
	fetch cur into rec;
	if rec is null then
		return 0;
	end if;
	while rec.lane_nav_strand_id is not null loop
	
		if rec.lane_nav_strand_id != lane_nav_strand_id_o then
		
			if lane_nav_strand_id_o != 0 then
                INSERT INTO temp_lane_nav(
		            lane_nav_strand_id, condition_id, lane_id, node_id, inlink_id, 
		            outlink_id, passcount, passlid)
		    	VALUES (lane_nav_strand_id_o, conditionid, laneid, nodeid, inlinkid, 
		            outlinkid, c-1, midlinkids1);
			end if;
			
			lane_nav_strand_id_o := rec.lane_nav_strand_id;
			laneid := rec.lane_id;
			c := 0;
			passlid := '';
			inlinkid := rec.link_id;
			nodeid := rec.node_id;
			midlinkids1 := null;
			midlinkids2 := null;
			conditionid := rec.condition_id;
			
		else

			outlinkid := rec.link_id;
			midlinkids1 := midlinkids2;
			if c = 0 then 
				midlinkids2 := cast(outlinkid as character varying);
			else
				midlinkids2 := midlinkids2 || '|' || cast(outlinkid as character varying);
			end if;
			c := c + 1;
			
		end if;
		
		fetch cur into rec;
		
	end loop;
	
	if lane_nav_strand_id_o != 0 then
	    INSERT INTO temp_lane_nav(
	        lane_nav_strand_id, condition_id, lane_id, node_id, inlink_id, 
	        outlink_id, passcount, passlid)
		VALUES (lane_nav_strand_id_o, conditionid, laneid, nodeid, inlinkid, 
	        outlinkid, c-1, midlinkids1);
	end if;
	
	close cur;
    
    return 1;
END;
$$
  LANGUAGE plpgsql VOLATILE
  COST 100;
ALTER FUNCTION mid_make_temp_lane_nav() OWNER TO postgres;

CREATE OR REPLACE FUNCTION mid_make_lanenumlr()
  RETURNS smallint AS
$$
DECLARE
	cur refcursor;
	rec record;
	linkid_o integer;
	dir_o character varying(1);
	lane_num_l integer;
	lane_num_r integer;
	pre_lane integer;
	bf boolean;
	bt boolean;
BEGIN
    open cur for
		SELECT lane_id, link_id, lane_number, lane_travel_direction, lane_type, 
		       access_id, lane_divider_marker, center_divider_marker, direction_category, 
		       transition_area, lane_forming_ending, from_speed_limit, to_speed_limit, 
		       height_restriction, width, lane_crossing_restriction
		  FROM rdf_lane order by link_id, lane_number;
	
	linkid_o := 0;
	dir_o := null;

	fetch cur into rec;
	if rec is null then
		return 0;
	end if;
	
	while rec.link_id is not null loop
	
		if rec.link_id != linkid_o or rec.lane_travel_direction != dir_o then
		
			if linkid_o != 0 and dir_o is not null then
				INSERT INTO temp_lanenumlr(
				            link_id, direction, lanenuml, lanenumr)
				    VALUES (linkid_o, dir_o, lane_num_l, lane_num_r);
			end if;
			
			linkid_o := rec.link_id;
			dir_o := rec.lane_travel_direction;
			lane_num_l := 0;
			lane_num_r := 0;
			bf := false;
			bt := false;
			
			if (rec.lane_travel_direction = 'F' or rec.lane_travel_direction = 'B') then
				if rec.lane_forming_ending = 1 then
					lane_num_l := lane_num_l + 1;
				elseif rec.lane_forming_ending = 2 then
					lane_num_l := lane_num_l - 1;
				end if;
				if rec.lane_forming_ending is null then
					bf := true;
				end if;
			else
				bf := true;
			end if;
			
			if (rec.lane_travel_direction = 'T' or rec.lane_travel_direction = 'B') then
				if (rec.lane_forming_ending = 1) then
					lane_num_r := lane_num_r + 1;
				elseif rec.lane_forming_ending = 2 then
					lane_num_r := lane_num_r - 1;
				end if;
				if rec.lane_forming_ending is null then
					bt := true;
				end if;
			else
				bt := true;
			end if;
			
		else
			
			if (rec.lane_travel_direction = 'F' or rec.lane_travel_direction = 'B') then
				if bf = false then
					if (rec.lane_forming_ending = 1) then
						lane_num_l := lane_num_l + 1;
					elseif (rec.lane_forming_ending = 2) then
						lane_num_l := lane_num_l - 1;
					end if;
				else
					if (rec.lane_forming_ending = 1) then
						lane_num_r := lane_num_r + 1;
					elseif (rec.lane_forming_ending = 2) then
						lane_num_r := lane_num_r - 1;
					end if;
				end if;
				if rec.lane_forming_ending is null then
					bf := true;
				end if;
			else
				bf := true;
			end if;
			
			if (rec.lane_travel_direction = 'T' or rec.lane_travel_direction = 'B') then
				if bt = false then
					if (rec.lane_forming_ending = 1) then
						lane_num_r := lane_num_r + 1;
					elseif (rec.lane_forming_ending = 2) then
						lane_num_r := lane_num_r - 1;
					end if;
				else
					if (rec.lane_forming_ending = 1) then
						lane_num_l := lane_num_l + 1;
					elseif (rec.lane_forming_ending = 2) then
						lane_num_l := lane_num_l - 1;
					end if;
				end if;
				if rec.lane_forming_ending is null then
					bt := true;
				end if;
			else
				bt := true;
			end if;
			
		end if;
		
		fetch cur into rec;
		
	end loop;
	
	if linkid_o != 0 and dir_o is not null then
		INSERT INTO temp_lanenumlr(
		            link_id, direction, lanenuml, lanenumr)
		    VALUES (linkid_o, dir_o, lane_num_l, lane_num_r);
	end if;
	
	close cur;
    
    return 1;
END;
$$
  LANGUAGE plpgsql VOLATILE
  COST 100;
ALTER FUNCTION mid_make_lanenumlr() OWNER TO postgres;

CREATE OR REPLACE FUNCTION mid_make_guideinfo_lane()
  RETURNS smallint AS
$$
DECLARE
	cur refcursor;
	rec record;
	sLaneinfo character varying(17);
	sBusinfo character varying(17);
	iId integer;
	iLen integer;
	iLane integer;
	aLaneinfo character[];
	aBusinfo character[];
	i integer;
	clens integer;
	e integer[];
	
BEGIN
    open cur for
		SELECT inlink_id, node_id, outlink_id, passcount, passlid,
		       lane_travel_direction, direction_category, physical_num_lanes, b.lanenuml, b.lanenumr,
		       array_agg(lane_number) as array_lane, array_agg(bus_access_id) as array_bus
		  FROM temp_lane_guide_distinct as a left join temp_additional_lanenum_lr as b 
		  	on a.inlink_id = b.link_id and a.lane_travel_direction = b.direction
		  group by inlink_id, node_id, outlink_id, passcount, passlid,  
		       lane_travel_direction, direction_category, physical_num_lanes, b.lanenuml, b.lanenumr;
	
	iId := 0;
	fetch cur into rec;
	if rec is null then
		return 0;
	end if;
	
	while rec.inlink_id is not null loop
	
		iId := iId + 1;
		
		clens := rec.physical_num_lanes;
		aLaneinfo := e;
		aBusinfo := e;
		for i in 1..clens loop
			aLaneinfo := array_append(aLaneinfo, '0');
			aBusinfo := array_append(aBusinfo, '0');
		end loop;
		
		iLen := array_length(rec.array_lane, 1);
		i := 1;
		while i <= iLen loop
			iLane := rec.array_lane[i];
			aLaneinfo[iLane] := '1';
			if aBusinfo[iLane] is not null then 
				rec.array_bus[iLane] := '1';
			end if;
			i := i + 1;
		end loop;
		
		
		sLaneinfo := array_to_string(aLaneinfo, '');
		sBusinfo := array_to_string(aBusinfo, '');
		
		
		INSERT INTO lane_tbl(
		            id, nodeid, inlinkid, outlinkid, passlid, passlink_cnt, 
		            lanenum, laneinfo, arrowinfo, lanenuml, lanenumr, buslaneinfo)
		    VALUES (iId, rec.node_id, rec.inlink_id, rec.outlink_id, rec.passlid, rec.passcount,
		            rec.physical_num_lanes, sLaneinfo, (rec.direction_category), 
		            rec.lanenuml, rec.lanenumr, sBusinfo);

		fetch cur into rec;
		
	end loop;
	
	close cur;
    return 1;
END;
$$
  LANGUAGE plpgsql VOLATILE
  COST 100;
ALTER FUNCTION mid_make_guideinfo_lane() OWNER TO postgres;


CREATE OR REPLACE FUNCTION mid_make_rdb_zone()
RETURNS smallint AS
$$
DECLARE
	id smallint;
	cur refcursor;
	rec record;
	drive_flag smallint;
BEGIN
	id := 0;
	
	open cur for
		select iso_country_code, driving_side from rdf_country;
		
	fetch cur into rec;
	WHILE rec.iso_country_code is not null LOOP
		id := id + 1;
		if rec.driving_side = 'L' then 
			drive_flag := 1;
		else
			drive_flag := 0;
		end if;
		
		INSERT INTO rdb_zone(
            zoneid, country_code, sub_country_code, travel_flag)
    	VALUES (id, rec.iso_country_code, null, drive_flag);
    	
    	fetch cur into rec;
	END LOOP;
	
	return 1;
END;
$$
LANGUAGE plpgsql VOLATILE
COST 100;
ALTER FUNCTION mid_make_rdb_zone() OWNER TO postgres;


CREATE OR REPLACE FUNCTION mid_get_connected_nodeid_uc(fromnodeid1 bigint, tonodeid1 bigint, fromnodeid2 bigint, tonodeid2 bigint)
  RETURNS integer 
  LANGUAGE plpgsql
  AS
$$ 
BEGIN
	IF fromnodeid1 = fromnodeid2 THEN
		return fromnodeid1;
	END IF;
	
	IF fromnodeid1 = tonodeid2 THEN
		return fromnodeid1;
	END IF;

	IF tonodeid1 = fromnodeid2 THEN
		return tonodeid1;
	END IF;

	IF tonodeid1 = tonodeid2 THEN
		return tonodeid1;
	END IF;
	
	return NULL;
END;
$$;

--------------------------------------------------------------------------
-- make a part of guide lane data
--------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_make_temp_lane_guide_nav()
  RETURNS bigint
  LANGUAGE plpgsql volatile
AS $$
DECLARE
	strand_id_array         integer[];
	temp_lane_nav_strand_id integer;
	strand_id_array_length  integer;
	strand_id_index         integer;
	link_array              bigint[];
	link_index              integer;
	seq_num_array           bigint[];
	lane_id_array           bigint[];
	conditionid_array       integer[];
	nodeid_array            bigint[];
	inlinkid                bigint;
	nodeid                  bigint;
	outlinkid               bigint;
	passlink_cnt            integer;
	passlinkids             character varying(512);
	laneid                  bigint;
	conditionid             integer;
	seq_num_array_length    integer;
BEGIN
	SELECT array_agg(distinct lane_nav_strand_id)
	into strand_id_array
	FROM temp_lane_guide_nav_strand;
	
	strand_id_array_length = array_upper(strand_id_array, 1);
	
	if strand_id_array_length < 1 then
		return 0;
	end if;
	strand_id_index = 1;
	while strand_id_index <= strand_id_array_length loop
		passlinkids = null;
		SELECT array_agg(seq_num), array_agg(condition_id), array_agg(lane_id), array_agg(node_id), array_agg(link_id)
		into seq_num_array, conditionid_array, lane_id_array, nodeid_array, link_array
		FROM (
			select * 
			from temp_lane_guide_nav_strand
			where lane_nav_strand_id = strand_id_array[strand_id_index]
			order by seq_num
			) as a;
		
		seq_num_array_length = array_upper(seq_num_array, 1);
		
		if seq_num_array_length > 1 then
			inlinkid = link_array[1];
			outlinkid = link_array[seq_num_array_length];
			nodeid = nodeid_array[1];
			passlink_cnt = seq_num_array_length - 2;
			laneid = lane_id_array[1];
			conditionid = conditionid_array[1];
			if passlink_cnt = 0 then
				passlinkids = null;
			else
				link_index = 2;
				passlinkids = cast(link_array[link_index] as character varying);
				link_index = link_index + 1;
				while link_index <= seq_num_array_length - 1 loop
					passlinkids := passlinkids || '|' || cast(link_array[link_index] as character varying);
					link_index = link_index + 1;	
				end loop;
			end if;
		elseif seq_num_array_length = 1 then
			inlinkid = link_array[1];
			outlinkid = inlinkid;
			nodeid = nodeid_array[1];
			passlink_cnt = -1;
			laneid = lane_id_array[1];
			conditionid = conditionid_array[1];
			passlinkids = null;
		else
			raise INFO '';
			EXIT;
		end if;

		INSERT INTO temp_lane_guide_nav(
			lane_nav_strand_id, condition_id, lane_id, node_id, inlink_id, 
			outlink_id, passcount, passlid)
		    	VALUES (strand_id_array[strand_id_index], conditionid, laneid, nodeid, inlinkid, 
		            outlinkid, passlink_cnt, passlinkids);
		strand_id_index	= strand_id_index + 1;
	end loop;
	return 1;
end;
$$;
--------------------------------------------------------------------------
-- make additional lane number
--------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION _mid_notuse_make_additional_lanenum()
  RETURNS smallint
LANGUAGE plpgsql volatile
AS $$
DECLARE
	linkid_array          integer[];
	linkid_array_length   integer;
	linkid_array_index    integer;
	temp_linkid           integer;
	lane_tra_dir_array    character[];
	lane_tra_dir_index    integer;
	lane_tra_dir_length   integer;
	lane_form_end_array   integer[]; 
	f_index               integer;
	t_index               integer;
	lane_num_l            integer;
	lane_num_r            integer;
	---Previous traveral this lanes, if not meet the conditions 
	bf                    boolean;
	bt                    boolean;
	lane_dir              character;
BEGIN
	--get all link_id
	SELECT array_agg(distinct link_id)
	into linkid_array
	FROM rdf_lane;
	
	linkid_array_length = array_upper(linkid_array, 1);
	linkid_array_index = 1;
	if linkid_array_length < 1 then
		return 0;
	end if;
	--traverals all links to get addtional lanes
	while linkid_array_index <= linkid_array_length loop
		bf = false;
		bt = false;
		lane_dir = null;
		lane_num_l = 0;
		lane_num_r = 0;
		temp_linkid = linkid_array[linkid_array_index];
		--get lanes ordered by lane_number about every link
		SELECT array_agg(lane_travel_direction), array_agg(lane_forming_ending)
		into lane_tra_dir_array, lane_form_end_array
		from(
			select * 
			FROM rdf_lane
			where link_id = temp_linkid
			order by lane_number
		) as a;
		
		lane_tra_dir_length = array_upper(lane_tra_dir_array, 1);
		lane_tra_dir_index = 1;
		
		if lane_tra_dir_length < 1 then
			EXIT;
		end if;
		--different lanes on the same link 
		while lane_tra_dir_index <= lane_tra_dir_length loop
			
			if lane_tra_dir_array[lane_tra_dir_index] <> lane_dir or  lane_dir is null then
				--raise INFO 'fgiokgkdktokgro-----%', lane_dir;
				--lane direction change(first group) 
				if lane_dir is not null then
					INSERT INTO temp_additional_lanenum_lr(link_id, direction, lanenuml, lanenumr)
					VALUES (temp_linkid, lane_dir, lane_num_l, lane_num_r);
				end if;
				
				lane_num_l = 0;
				lane_num_r = 0;
				lane_dir = lane_tra_dir_array[lane_tra_dir_index];
				bf = false;
				bt = false;
				----first lane(the same direction)
				if (lane_tra_dir_array[lane_tra_dir_index] = 'F' or lane_tra_dir_array[lane_tra_dir_index] = 'B') then
					if lane_form_end_array[lane_tra_dir_index] = 1 then
						lane_num_l = lane_num_l + 1;
					elseif lane_form_end_array[lane_tra_dir_index] = 2 then
						lane_num_l = lane_num_l - 1;
					elseif lane_form_end_array[lane_tra_dir_index] is null then
						bf = true;
					end if;	
				else
					bf = true;	
				end if;
				if (lane_tra_dir_array[lane_tra_dir_index] = 'T' or lane_tra_dir_array[lane_tra_dir_index] = 'B') then
					if lane_form_end_array[lane_tra_dir_index] = 1 then
						lane_num_r = lane_num_r + 1;
					elseif lane_form_end_array[lane_tra_dir_index] = 2 then
						lane_num_r = lane_num_r - 1;
					elseif lane_form_end_array[lane_tra_dir_index] is null then
						bt = true;
					end if;	
				else
					bt = true;
				end if;
			else
				if (lane_tra_dir_array[lane_tra_dir_index] = 'F' or lane_tra_dir_array[lane_tra_dir_index] = 'B') then
					if bf <> true then
						if lane_form_end_array[lane_tra_dir_index] = 1 then
							lane_num_l = lane_num_l + 1;
						elseif lane_form_end_array[lane_tra_dir_index] = 2 then
							lane_num_l = lane_num_l - 1;
						end if;	
					else
						if lane_form_end_array[lane_tra_dir_index] = 1 then
						lane_num_r = lane_num_r + 1;
						elseif lane_form_end_array[lane_tra_dir_index] = 2 then
						lane_num_r = lane_num_r - 1;
						end if;		
					end if;
					if lane_form_end_array[lane_tra_dir_index] is null then
						bf := true;
					end if;
				else
					bf := true;
					
				end if;
				if (lane_tra_dir_array[lane_tra_dir_index] = 'T' or lane_tra_dir_array[lane_tra_dir_index] = 'B') then
					if bt <> true then
						if lane_form_end_array[lane_tra_dir_index] = 1 then
							lane_num_r = lane_num_r + 1;
						elseif lane_form_end_array[lane_tra_dir_index] = 2 then
							lane_num_r = lane_num_r - 1;
						end if;	
					else
						if lane_form_end_array[lane_tra_dir_index] = 1 then
						lane_num_l = lane_num_l + 1;
						elseif lane_form_end_array[lane_tra_dir_index] = 2 then
						lane_num_l = lane_num_l - 1;
						end if;		
					end if;
					if lane_form_end_array[lane_tra_dir_index] is null then
						bt := true;
					end if;
				else
					bt := true;
				end if;
			end if;
			
			lane_tra_dir_index = lane_tra_dir_index + 1;
		end loop;
		--second group
		if lane_dir is not null then
			INSERT INTO temp_additional_lanenum_lr(link_id, direction, lanenuml, lanenumr)
			VALUES (temp_linkid, lane_dir, lane_num_l, lane_num_r);
		end if;
		linkid_array_index = linkid_array_index + 1;
	end loop;
	
    
    return 1;
END;
$$;
--------------------------------------------------------------------------
-- get lane number
--------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_get_lanenum(inlinkid integer, lanenum integer)
  RETURNS smallint
LANGUAGE plpgsql volatile
AS $$
DECLARE
	lanes    integer;
	
BEGIN	lanes = 0;
	select count(*)
	into lanes
	from rdf_lane
	where link_id = inlinkid 
		and lane_travel_direction in 
				(select lane_travel_direction
				from rdf_lane
				where link_id = inlinkid and lane_number = lanenum);
	
	
    
    return lanes;
END;
$$;
--------------------------------------------------------------------------
-- get left and right additional lane number
--------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_make_additional_lanenum(inlinkid integer,nodeid bigint,direction character)
  RETURNS smallint[]
LANGUAGE plpgsql volatile
AS $$
DECLARE
	result                integer[];
	lanes_array_length    integer;
	linkid_array_index    integer;
	lane_number_array     integer[];
	lane_number_index     integer;
	lane_tra_dir_length   integer;
	lane_form_end_array   integer[]; 
	lane_num_l            integer;
	lane_num_r            integer;
	not_ref_node_id       bigint;
	link_ref_dir          boolean;
	lanenum               integer;
	cnt                   integer;
	rec                   record;
BEGIN	

	link_ref_dir = false;
	lane_num_l = 0;
	lane_num_r = 0;
	result = ARRAY[0,0];
	lane_number_index = 1;
	--get Data under given linkid and direction (the same dir lane data)
	SELECT array_agg(lane_forming_ending),array_agg(lane_number)
	into  lane_form_end_array, lane_number_array
	from(
		select * 
		FROM rdf_lane
		where link_id = inlinkid and lane_travel_direction = direction
		order by lane_number
	) as a;

	lanes_array_length = array_upper(lane_number_array, 1);
	
	if lanes_array_length < 1 then
		return 0;
	end if;
	
	select nonref_node_id
	into not_ref_node_id
	from rdf_link 
	where link_id = inlinkid;
	
	if not_ref_node_id = nodeid then
		link_ref_dir = true;
	end if;

	lanenum = lane_number_array[1] + round(lanes_array_length::numeric / 2::numeric, 0) - 1;
	
	while lane_number_index <= lanes_array_length loop
		if lane_form_end_array[lane_number_index] is not null then
			if lane_form_end_array[lane_number_index] in (2,3) then
				
				if lane_number_array[lane_number_index]>lanenum then
					if link_ref_dir then
						lane_num_r = lane_num_r - 1;
					else
						lane_num_l = lane_num_l - 1;
					end if;
					
				else
					if link_ref_dir then
						lane_num_l = lane_num_l - 1;
					else
						lane_num_r = lane_num_r - 1;
					end if;
				end if;
			end if;
		end if;
		lane_number_index = lane_number_index + 1;
	end loop;
	
	select count(1) into cnt 
	from rdf_link where nodeid in (ref_node_id,nonref_node_id)
	and link_id<>inlinkid;
	
	if cnt=1 then
		for rec in 
			select link_id,lane_number,lane_travel_direction,case when ref_node_id=nodeid then 1 else 0 end as direction,
			(select round(avg(lane_number),0) from rdf_lane where link_id=a.link_id and lane_travel_direction=b.lane_travel_direction) as midlanenum
			from rdf_link a left join rdf_lane b using(link_id) 
			where nodeid in (ref_node_id,nonref_node_id) and
			b.lane_forming_ending in (1,3) and
			link_id<>inlinkid and
			( (lane_travel_direction='F' and ref_node_id=nodeid) or 
			(lane_travel_direction='T' and nonref_node_id=nodeid))
		loop
		
		if rec.lane_number>=rec.midlanenum then
			if rec.lane_travel_direction='T' then
				lane_num_l = lane_num_l + 1;
			else
				lane_num_r = lane_num_r + 1;
			end if;
		else
			if rec.lane_travel_direction='T' then
				lane_num_r = lane_num_r + 1;
			else
				lane_num_l = lane_num_l + 1;
			end if;
		end if;
		end loop;
	end if;
	
	result[1] = lane_num_l;
	result[2] = lane_num_r;
    return result;
END;
$$;



--------------------------------------------------------------------------
-- get every lane traveral information
--------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_make_guidelane_info(param_inlinkid integer,param_nodeid bigint, physical_lane_num integer,lane_travel_dir character)
  RETURNS integer LANGUAGE plpgsql
  AS
$$ 
DECLARE
	
	not_ref_node_id  bigint;
	link_ref_dir     boolean;
	lanenum_array    integer[];
	existlan_array   integer[];
	dir_ctg_array    integer[];
	dir_ctg_length   integer;
	existlan_index   integer;
	lanenum_index    integer;
	i                integer;
	j                integer;
	sm_aLaneinfo     character[];
	ex_aLaneinfo     character[];
	null_flag        boolean;
	businfo_id_array integer[];
	businfo_id_index integer;
	businfo          integer[];
BEGIN   
	link_ref_dir = false;
	dir_ctg_length = 0;
	lanenum_index = 1;
	businfo_id_index = 1;
	existlan_index = 1;
	
	j=1;
	--null_flag = true;
	
	select nonref_node_id
	into not_ref_node_id
	from rdf_link 
	where link_id = param_inlinkid;
	--lane direction is or isn't same with direction or link(FROM TO)
	if not_ref_node_id = param_nodeid then
		link_ref_dir = true;
	end if;
	
	if link_ref_dir then
		--get lane_travel_direction and direction_category\lane_no
		select array_agg(direction_category),array_agg(lane_number),array_agg(c.businfo_id)
		into dir_ctg_array,lanenum_array,businfo_id_array
		from(
			select a.direction_category,a.lane_number,  
			(
			case 
				when b.bus_access_id is NULL then -1 
				else b.bus_access_id 
			end
			) as businfo_id
			from rdf_lane as a
			left join (
				    select access_id as bus_access_id 
				    from rdf_access 
				    where automobiles= 'N' and buses= 'Y' 
				    and taxis= 'N' and carpools= 'N' 
				    and pedestrians= 'N' and trucks= 'N' 
				    and deliveries= 'N' and emergency_vehicles= 'N' 
				    and through_traffic= 'N' and motorcycles= 'N'
				) as b 
				on a.access_id = b.bus_access_id
			where a.link_id = param_inlinkid and a.lane_travel_direction = lane_travel_dir
			order by lane_number 
		) as c;	

		SELECT array_agg(lane_number)
		into existlan_array
		from(
			select distinct lane_number
			FROM temp_lane_guide_distinct 
			where inlink_id = param_inlinkid and node_id = param_nodeid
			order by lane_number
		) as a; 
		
	else
		select array_agg(direction_category),array_agg(lane_number),array_agg(c.businfo_id)
		into dir_ctg_array,lanenum_array,businfo_id_array
		from(
			select a.direction_category,a.lane_number, 
			(
			case 
				when b.bus_access_id is NULL then -1 
				else b.bus_access_id 
			end
			) as businfo_id
			from rdf_lane as a
			left join (
				    select access_id as bus_access_id 
				    from rdf_access 
				    where automobiles= 'N' and buses= 'Y' 
				    and taxis= 'N' and carpools= 'N' 
				    and pedestrians= 'N' and trucks= 'N' 
				    and deliveries= 'N' and emergency_vehicles= 'N' 
				    and through_traffic= 'N' and motorcycles= 'N'
				) as b 
				on a.access_id = b.bus_access_id
			where a.link_id = param_inlinkid and a.lane_travel_direction = lane_travel_dir
			order by lane_number desc
		) as c;	

		SELECT array_agg(lane_number)
		into existlan_array
		from(
			select distinct lane_number
			FROM temp_lane_guide_distinct 
			where inlink_id = param_inlinkid and node_id = param_nodeid
			order by lane_number desc
		) as a; 
	end if;

	--raise INFO 'existlan_array = %', existlan_array;
	--raise INFO 'lanenum_array = %', lanenum_array;
	
	---lanes
	dir_ctg_length = array_upper(dir_ctg_array, 1);
	
	if dir_ctg_length is null or dir_ctg_length < 1 then
		return 0;
	end if;

	---[0000000],not data
	--while j <= dir_ctg_length loop
		--if dir_ctg_array[j] is not null then
		--	null_flag = false;
		--	exit;
		--end if;
		--j = j + 1;
	--end loop;
	
	------get businfo
	while businfo_id_index <= dir_ctg_length loop
		if businfo_id_array[businfo_id_index] = -1 then
			businfo = array_append(businfo, 0);
		else
			businfo = array_append(businfo, 1);
		end if;
		businfo_id_index = businfo_id_index + 1;
	end loop;
	


	---raise INFO 'dir_ctg_length = %', dir_ctg_length;
	
	---init lane info
	---ex_aLaneinfo not
	
	for i in 1..physical_lane_num loop
		sm_aLaneinfo := array_append(sm_aLaneinfo, '0');
		ex_aLaneinfo := array_append(ex_aLaneinfo, '0');
	end loop;

	
	--insert data
	--while lanenum_index <= dir_ctg_length and null_flag = false loop
	while lanenum_index <= dir_ctg_length  loop
		if lanenum_array[lanenum_index] = existlan_array[existlan_index]then
			sm_aLaneinfo[lanenum_index] = '1';
			insert into temp_lane_tbl(nodeid,inlinkid, outlinkid,passlid,passlink_cnt,lanenum,laneinfo,arrowinfo,lanenuml,lanenumr,buslaneinfo)
			(
				select node_id, inlink_id, outlink_id, passlid, passcount, physical_num_lanes,array_to_string(sm_aLaneinfo, ''), 
					case when direction_category = 0 then null
						else direction_category
					end,(mid_make_additional_lanenum(param_inlinkid,param_nodeid,lane_travel_dir))[1],
					(mid_make_additional_lanenum(param_inlinkid,param_nodeid,lane_travel_dir))[2],array_to_string(businfo, '')
				from temp_lane_guide_distinct
				where inlink_id = param_inlinkid and node_id = param_nodeid and lane_number = lanenum_array[lanenum_index]
			);
			
			sm_aLaneinfo[lanenum_index] = '0';
			existlan_index = existlan_index + 1;
			lanenum_index = lanenum_index + 1;
		else	
			if dir_ctg_array[lanenum_index] is not null then
				ex_aLaneinfo[lanenum_index] = '1';
				insert into temp_lane_tbl(nodeid,inlinkid, outlinkid,passlid,passlink_cnt,lanenum,laneinfo,arrowinfo,lanenuml,lanenumr,buslaneinfo)
				(
				select param_nodeid,param_inlinkid, NULL, NULL, 0, physical_lane_num, array_to_string(ex_aLaneinfo, ''), 
					case when dir_ctg_array[lanenum_index] = 0 then null
						else dir_ctg_array[lanenum_index]
					end, 
					(mid_make_additional_lanenum(param_inlinkid,param_nodeid,lane_travel_dir))[1], 
				        (mid_make_additional_lanenum(param_inlinkid,param_nodeid,lane_travel_dir))[2], array_to_string(businfo, '')
				);
			end if;
			ex_aLaneinfo[lanenum_index] = '0';
			lanenum_index = lanenum_index + 1;
		end if;
		
	end loop;
	
	--raise INFO 'strSubTime-ccccccc';
    return 0;
END;
$$;

--------------------------------------------------------------------------
-- make lane_tbl table
--------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_make_lane_tbl()
  RETURNS integer LANGUAGE plpgsql
  AS
$$ 
DECLARE
	rec              record;
	ID               integer;
	lstr_array       text[];
	lstr_array_len   integer;
	lane_num         integer;
	lstr_index       integer;
	char_index       integer;
	laneinfo_result  character[];
	temp_char        character;
	lane_index       integer;
	laneinfo_str     text;
BEGIN   
	ID = 1;
	lstr_array_len = 1;
	temp_char = '0';
	
	FOR rec IN
		select array_agg(laneinfo) as laneinfo_list, lanenum, nodeid, inlinkid, outlinkid, passlid, passlink_cnt, arrowinfo, lanenuml, lanenumr, buslaneinfo
		from temp_lane_tbl 
		group by nodeid, inlinkid, outlinkid, passlid, passlink_cnt, lanenum, arrowinfo, lanenuml, lanenumr, buslaneinfo 
	loop
		lstr_array = rec.laneinfo_list;
		lane_num = rec.lanenum;
		lstr_array_len = array_upper(lstr_array,1);
		lane_index = 1;
		char_index = 1;
		laneinfo_result = null;
		while lane_index <= lane_num loop
			laneinfo_result[lane_index] = '0';
			lane_index = lane_index + 1;
		end loop;
		---raise INFO 'laneinfo_result = %', laneinfo_result;
		if lstr_array_len > 1 then
			while char_index <= lane_num loop
				lstr_index = 1;
				while lstr_index <= lstr_array_len loop
					temp_char = substr(lstr_array[lstr_index], char_index, 1)::character;
					if temp_char = '1' then
						laneinfo_result[char_index] = '1'
						exit;
					end if;
					lstr_index = lstr_index + 1;
				end loop;
				char_index = char_index + 1;
			end loop;
			laneinfo_str = array_to_string(laneinfo_result,'');
		else
			laneinfo_str = lstr_array[1];
		end if;
		
		insert into lane_tbl(id,nodeid,inlinkid, outlinkid,passlid, passlink_cnt,lanenum, laneinfo,arrowinfo, lanenuml,lanenumr,buslaneinfo)
			values(ID, rec.nodeid, rec.inlinkid, rec.outlinkid, rec.passlid, rec.passlink_cnt, rec.lanenum, laneinfo_str, rec.arrowinfo, rec.lanenuml, rec.lanenumr, rec.buslaneinfo);
		ID  = ID  + 1;
	END LOOP;
    return 0;
END;
$$;



CREATE OR REPLACE FUNCTION mid_make_etc_only_links_in_one_direction( link bigint, search_node bigint, OUT linkrow bigint[])
	RETURNS bigint[]
	LANGUAGE plpgsql volatile
AS $$
DECLARE
	rec				record;
	reach_link                      bigint;
	reach_node			bigint;
	linkid				bigint;
	nodeid				bigint;
BEGIN

	reach_node	:= search_node;
	reach_link      := link;
	
	-- search
	while true loop
		
		-- search link
		select	count(*) as count,array_agg(link_id) as links,array_agg(nextnode) as nodes
		from
		(
			(
				select	link_id, nonref_node_id as nextnode
				from temp_rdf_nav_link
				where 	(reach_node = ref_node_id) 
					and link_id <> reach_link
			)
			union
			(
				select	link_id, ref_node_id as nextnode
				from temp_rdf_nav_link
				where 	(reach_node = nonref_node_id) 
					and link_id <> reach_link
			)
		)as a
		into rec;

		if rec.count = 1 then
			
			linkid = unnest(rec.links);
			nodeid = unnest(rec.nodes);
			linkrow		:= array_append(linkrow,linkid );
			reach_link	:= linkid;
			reach_node	:= nodeid;
		else
			EXIT;
		end if;
	end loop;
END;
$$;

--------------------------------------------------------------------------
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

CREATE OR REPLACE FUNCTION mid_findpasslinkbybothlinks(nfromlink bigint, ntolink bigint, startnode bigint, endnode bigint, dir integer,max_pathcnt integer)
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

	temp_count          integer;

	temp_snode          bigint;
	temp_enode          bigint;
	oneway_c            integer;
BEGIN
	--rstPath
	rstPathCount		:= 0;
	tmpPathArray		:= ARRAY[cast(nFromLink as varchar)];
	tmpPathCount		:= 1;
	tmpPathIndex		:= 1;
	nStartNode          := endNode;
	nScondNode          := startNode;
	
	select count(*)
	into temp_count
	from link_tbl
	where link_id in (nfromlink,ntolink) and one_way_code = 4;
	
	if temp_count <> 0 then
                return null;
	end if;

	SELECT s_node, e_node, one_way_code
        into temp_snode,temp_enode,oneway_c
        FROM link_tbl
        where link_id = nfromlink;
                
	if dir = 1 then
                if oneway_c = 1 then
                        tmpLastNodeArray	:= ARRAY[nStartNode,nScondNode];
                        tmpLastLinkArray	:= ARRAY[nFromLink,nFromLink];
                        tmpPathArray		:= array_append(tmpPathArray,cast(nFromLink as varchar));
                        tmpPathCount		:= 2;
                elseif oneway_c = 2 then
                        tmpLastNodeArray	:= ARRAY[temp_enode];
                        tmpLastLinkArray	:= ARRAY[nFromLink];
                elseif oneway_c = 3 then
                        tmpLastNodeArray	:= ARRAY[temp_snode];
                        tmpLastLinkArray	:= ARRAY[nFromLink];
                        ---raise INFO 'node = %', tmpLastNodeArray;
                else
                        return null;
                end if;
	elseif dir = 0 then
                if (oneway_c = 2 and nStartNode = temp_enode) or (oneway_c = 3 and nStartNode = temp_snode) or (oneway_c = 1 and nStartNode in (temp_snode,temp_enode))then
                        tmpLastNodeArray	:= ARRAY[nStartNode];
                        tmpLastLinkArray	:= ARRAY[nFromLink];
                else
                        return null;
                end if;
	else 
		return null;
	end if;
	raise INFO 'node = %', tmpLastNodeArray;
	
	-- search
		
	WHILE tmpPathIndex <= tmpPathCount LOOP
			--raise INFO 'tmpPathArray = %', tmpPathArray[tmpPathIndex];
			-----stop if tmpPath has been more than layer given
		if array_upper(regexp_split_to_array(tmpPathArray[tmpPathIndex], E'\\|+'),1) < max_pathcnt then
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
				   SELECT a.link_id as nextroad,'(2)' as dir,a.e_node as nextnode
                                    FROM link_tbl as a
                                    where a.s_node = tmpLastNodeArray[tmpPathIndex] and a.one_way_code in (1,2)

                                    union

                                    SELECT b.link_id as nextroad,'(3)' as dir,b.s_node as nextnode
                                    FROM link_tbl as b
                                    where b.e_node = tmpLastNodeArray[tmpPathIndex] and b.one_way_code in (1,3)
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

CREATE OR REPLACE FUNCTION mid_find_sa_link_in_diff_direction( link bigint, search_node bigint,dir smallint)
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
	reach_link                      bigint;
	reach_node			bigint;
	flag 				boolean;
BEGIN	

	tmpPathArray		:= ARRAY[link];
	tmpLastNodeArray	:= ARRAY[search_node];
	tmpPathCount		:= 1;
	tmpPathIndex		:= 1;

	reach_node	:= search_node;
	reach_link      := link;

	flag := false;
		
	WHILE tmpPathIndex <= tmpPathCount LOOP
		bRoadEnd	:= True;

--  	raise INFO '   tmpPathIndex = %', tmpPathIndex;
-- 		raise INFO '   tmpPathArray = %', tmpPathArray;
-- 		raise info '   tmpLastNodeArray = %', cast(tmpLastNodeArray as varchar);

		for rec in 
			select link_id as nextlink 
				,(case  when ref_node_id = tmpLastNodeArray[tmpPathIndex] then nonref_node_id 
					when nonref_node_id = tmpLastNodeArray[tmpPathIndex] then ref_node_id
				end) as nextnode
				,controlled_access
				,ramp
				,poi_access 
			from (
				select	link_id  
					,ref_node_id 
					,nonref_node_id
					,case when travel_direction = 'B' then 1
					      when travel_direction = 'F' then 2
					      when travel_direction = 'T' then 3
					      else 4
					end as one_way
					,controlled_access
					,ramp 
					,poi_access
				from temp_rdf_nav_link 
			) a 
			where 	(	dir = 2
					and (		
						(tmpLastNodeArray[tmpPathIndex] = ref_node_id) and one_way in (1,3)
						or	
						(tmpLastNodeArray[tmpPathIndex] = nonref_node_id)   and one_way in (1,2)
					)
					and 
					not (
						cast(link_id as varchar) = ANY(tmpPathArray)
					)
				)
				or 
				(	dir = 1
					and (		
						(tmpLastNodeArray[tmpPathIndex] = ref_node_id) and one_way in (1,2)
						or	
						(tmpLastNodeArray[tmpPathIndex] = nonref_node_id)   and one_way in (1,3)
					)
					and 
					not (
						cast(link_id as varchar) = ANY(tmpPathArray)
					)
				)
			
		loop
		
			bRoadEnd	:= False;
			
			tmpPath		:= tmpPathArray[tmpPathIndex];
-- 			raise INFO 'tmpPath = %', tmpPath;
			if rec.controlled_access = 'Y' or rec.ramp = 'Y' then
--   				raise info ' ||| highway:current link:%,ca:%,pa:%',rec.nextlink,rec.controlled_access,rec.poi_access;
				flag := true;
				return flag;
			elseif rec.poi_access = 'Y' then
				tmpPathCount		:= tmpPathCount + 1;
				tmpLastNodeArray	:= array_append(tmpLastNodeArray, cast(rec.nextnode as bigint));
				if tmpPath is null then
--  					raise info '        new array rec.nextlink:%',rec.nextlink;
					tmpPathArray	:= array_append(tmpPathArray, cast(rec.nextlink as varchar));
				else
--  					raise info '        append array rec.nextlink:%,ca:%,pa:%',rec.nextlink,rec.controlled_access,rec.poi_access;
					tmpPathArray	:= array_append(tmpPathArray, cast(rec.nextlink as varchar));
				end if;
			else
-- 				raise info ' ||| not poi access road/highway:current link:%,ca:%,pa:%',rec.nextlink,rec.controlled_access,rec.poi_access;
			end if;
		end loop;
		
		-- road end point, here is a complete path
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
		
					
		
					
CREATE OR REPLACE FUNCTION mid_find_sa_link()
	RETURNS smallint
	LANGUAGE plpgsql volatile
AS $$
DECLARE
	rec record;
	highway_flag_pos_dir1 boolean;
	highway_flag_neg_dir1 boolean;
	highway_flag_pos_dir2 boolean;
	highway_flag_neg_dir2 boolean;	
BEGIN
	
	for rec in
		select link_id, ref_node_id as start_node_id, nonref_node_id as end_node_id
		,case when travel_direction = 'B' then 1
		      when travel_direction = 'F' then 2
		      when travel_direction = 'T' then 3
		      else 4
		end as one_way
		,the_geom 
		from temp_rdf_nav_link 
		where poi_access = 'Y' 
	loop

		highway_flag_pos_dir1 := false;
		highway_flag_neg_dir1 := false;
		highway_flag_pos_dir2 := false;
		highway_flag_neg_dir2 := false;
				
		if rec.one_way = 4 then 
			raise info 'this is an non-accessible road.';
		else
			if rec.one_way in (1,2) then
				highway_flag_pos_dir1 = mid_find_sa_link_in_diff_direction(rec.link_id,rec.start_node_id,2::smallint);
-- 				raise info ' *** highway_flag_pos_dir1:%',highway_flag_pos_dir1;
				highway_flag_neg_dir1 = mid_find_sa_link_in_diff_direction(rec.link_id,rec.end_node_id,1::smallint);
-- 				raise info ' *** highway_flag_neg_dir1:%',highway_flag_neg_dir1;
			end if;

			if rec.one_way in (1,3) then
				highway_flag_pos_dir2 = mid_find_sa_link_in_diff_direction(rec.link_id,rec.start_node_id,1::smallint);
--  				raise info ' *** highway_flag_pos_dir2:%',highway_flag_pos_dir2;
				highway_flag_neg_dir2 = mid_find_sa_link_in_diff_direction(rec.link_id,rec.end_node_id,2::smallint);
--  				raise info ' *** highway_flag_neg_dir2:%',highway_flag_neg_dir2;
			end if;
			
			if (rec.one_way = 1 and (
				(highway_flag_pos_dir1 is true or highway_flag_neg_dir1 is true) 
				and 
				(highway_flag_pos_dir2 is true or highway_flag_neg_dir2 is true)
				)
			   )
				or (rec.one_way = 2 and (highway_flag_pos_dir1 is true and highway_flag_neg_dir1 is true))
				or (rec.one_way = 3 and (highway_flag_pos_dir2 is true and highway_flag_neg_dir2 is true))
			then 
-- 				raise info 'this is sa link. ===> link_id:%',rec.link_id;
 				insert into temp_link_sa_area(link_id) select rec.link_id;
			else
-- 				raise info 'this is not sa link. ===> link_id:%',rec.link_id;
			end if;		
		end if;
	end loop;
	
	RETURN 1;
END;
$$;


---------------------------------------------------------------------------------------------------
--
CREATE OR REPLACE FUNCTION mid_push_forward_inlink(nInLink bigint, nOutLink bigint, 
                                            nNode bigint, passlink character varying,
                                            passlink_cnt integer)
  returns BIGINT[]
  language plpgsql
as $$
declare
        path_array         bigint[];
        nPrevLinkType       integer;
        nCurrLinkType       integer;
        link_cnt            integer;
        rst_passlink_array  bigint[];
        other_branches      bigint[];
begin
        -- No PassLink
        IF passlink is null or passlink = '' then
                return passlink;
        END IF;
        -- Get link_type of inlink
        nPrevLinkType = mid_get_link_type(nInLink);
        if nPrevLinkType is null then
                return null;
        end if;
        
        path_array    := array[nInLink] || regexp_split_to_array(passlink, E'\\|+')::bigint[];
        passlink_cnt  := passlink_cnt + 1;
        
        link_cnt := 2;
        while link_cnt <= passlink_cnt LOOP
                nCurrLinkType = mid_get_link_type(path_array[link_cnt]);
                -- Inner Link
                IF nCurrLinkType = 4 then 
                        raise info 'InLink=%, CurrLink=%, nOutLink=%, nCurrLinkType=%', 
                                 nInLink, path_array[link_cnt], nCurrLinkType, nOutLink;
                        exit;
                end if;
                
                other_branches = mid_get_other_branch_links(path_array[link_cnt - 1], path_array[link_cnt]);
                -- exist othere branch
                if other_branches is not null then
                        if mid_has_same_name(path_array[link_cnt - 1], path_array[link_cnt]) = True then
                                NULL;
                                raise info 'InLink=%, CurrLink=%, nOutLink=%, SAME NAME.', 
                                            nInLink, path_array[link_cnt], nOutLink;
                        else
                                raise info 'InLink=%, CurrLink=%, nOutLink=%, other_branches=%', 
                                            nInLink, path_array[link_cnt], nOutLink, other_branches;
                                exit;
                        END IF;
                end if;
                
                --  link type change
                if nPrevLinkType in (1, 2) and nCurrLinkType in (3, 5, 7) then
                        raise info 'InLink=%, CurrLink=%, nOutLink=%, nPrevLinkType=%, nCurrLinkType=%', 
                                  nInLink, path_array[link_cnt], nOutLink, nPrevLinkType, nCurrLinkType;
                        exit;
                end if;

                if nPrevLinkType <> nCurrLinkType then
                        raise info 'InLink=%, CurrLink=%, nOutLink=%, nPrevLinkType=%, nCurrLinkType=%. link type changed.=======', 
                                   nInLink, path_array[link_cnt], nOutLink, nPrevLinkType, nCurrLinkType;
                        null;
                end if;
                
                nPrevLinkType := nCurrLinkType;
                
                link_cnt := link_cnt + 1;
        END LOOP;
        
        IF link_cnt = 2 THEN -- Not change
                RETURN NULL;
        ELSE
                return path_array[(link_cnt-1):passlink_cnt];
        END IF;
end;
$$;

-------------------------------------------------------------------------------
-- 
CREATE OR REPLACE FUNCTION mid_has_same_name(nLink1 bigint, nLink2 bigint)
  returns BOOLEAN
  language plpgsql
as $$
declare
        name1  character varying;
        name2  character varying;
begin
        select road_name into name1 from link_tbl where link_id = nLink1;
        IF NOT FOUND THEN 
                RETURN False;
        END IF;
        select road_name into name2 from link_tbl where link_id = nLink2;
        IF NOT FOUND THEN 
                RETURN False;
        END IF;
        IF name1 = name2 then
                return TRUE;
        end if;
        return False;
end;
$$;

-------------------------------------------------------------------------------
-- 
CREATE OR REPLACE FUNCTION mid_get_other_branch_links(nInLink bigint, nOutLink bigint)
  returns bigint[]
  language plpgsql
as $$
declare
        branches    bigint[];
        nNode       bigint;
        rec         record;
        num         integer;
begin
        num      := 0;
        branches := array[]::bigint[];
        nNode := mid_get_connected_node(nInLink, nOutLink);
        if nNode is null then
                return NULL;
        end if;

        -- Get Out Links
        for rec in 
                SELECT link_id 
                  FROM link_tbl
                  WHERE ((s_node = nNode )
                         OR (e_node = nNode))
                        AND link_id not in (nInLink, nOutLink)
        LOOP
                num := num + 1;
                branches = branches || array[rec.link_id];
        END LOOP;
        if num > 0 then 
                return branches;
        else
                return NULL;
        end if;        
end;
$$;

-------------------------------------------------------------------------------
-- 
CREATE OR REPLACE FUNCTION mid_get_link_type(nLinkID bigint)  
  returns integer
  language plpgsql
as $$
declare
        nLinkType    integer;
begin
        SELECT  link_type into  nLinkType
          FROM link_tbl WHERE link_id = nLinkID;
        
        IF FOUND THEN
                RETURN nLinkType;
        END IF;
        return nLinkType;
end;
$$;

----------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_delete_shortlink_for_roundabout()
	RETURNS integer
	LANGUAGE PLPGSQL
AS $$
DECLARE
	iNum 	integer;
BEGIN
	
	while true loop
	
		drop table if exists temp_roundabout_add_delete;
		create table temp_roundabout_add_delete
		as
		(
			select link_id,unnest(node_array) as node_id
			from
			(
				select link_id,array[a.s_node,a.e_node] as node_array
				from temp_link_category6 as a
				join temp_node_category6_num as b
				on a.s_node = b.node_id or a.e_node = b.node_id
				where b.num = 1
			)temp
		);
		
		select count(link_id) into iNum
		from temp_roundabout_add_delete;
		
		if iNum = 0 then
			exit;
		end if;
		
		update temp_node_category6_num
		set num = num - 1
		where node_id in (
			select distinct node_id
			from temp_roundabout_add_delete
		);
		
		delete from temp_link_category6
		where link_id in (
			select distinct link_id from temp_roundabout_add_delete
		);
	end loop;
	
	return 0;
END;
$$;
CREATE OR REPLACE FUNCTION mid_get_round_link(s_link bigint,s_s_node bigint,s_e_node bigint,s_one_way smallint)
	RETURNS BOOLEAN
	LANGUAGE PLPGSQL
AS $$
DECLARE
	cur1	refcursor;
	rec1	record;
	
	first_node bigint;
	end_node bigint;
	cur_node bigint;
	
	end_node_array bigint[];
	end_node_array_num integer;
BEGIN

	if s_one_way = 2 then
		first_node = s_s_node;
		end_node = s_e_node;
	else
		first_node = s_e_node;
		end_node = s_s_node;
	end if;
	end_node_array_num := 1;
	end_node_array[end_node_array_num] := end_node;
	
	cur_node := 1;
	while cur_node <= end_node_array_num loop
		open cur1 for select link_id,s_node,e_node,one_way_code
							from temp_link_category6
							where (end_node_array[cur_node] = s_node and one_way_code = 2) or 
								(end_node_array[cur_node] = e_node and one_way_code = 3);
		fetch cur1 into rec1;
		while rec1.link_id is not null loop
			if rec1.one_way_code = 2 then
				end_node := rec1.e_node;
			else
				end_node := rec1.s_node;
			end if;
			
			if end_node = first_node then
				close cur1;
				return true;
			end if;
			
			if end_node = any(end_node_array) then
				--
			else
				end_node_array_num := end_node_array_num + 1;
				end_node_array[end_node_array_num] := end_node;
			end if;
			
			fetch cur1 into rec1;
		end loop;
		close cur1;
		cur_node := cur_node + 1;
	end loop;
	
	return false;
END;
$$;
CREATE OR REPLACE FUNCTION mid_jude_link_angle(angle_array smallint[])
	RETURNS boolean
	LANGUAGE PLPGSQL
AS $$
DECLARE
	angle_array_num integer;
	in_angle1 integer;
	in_angle2 integer;
BEGIN
	angle_array_num := array_upper(angle_array,1);
	for angle_array_num_idx in 1..(angle_array_num - 1) loop
		in_angle1 := ((angle_array[angle_array_num_idx] + 32768) * 360.0 / 65536.0)::integer;
		in_angle2 := ((angle_array[angle_array_num_idx + 1] + 32768) * 360.0 / 65536.0)::integer;
		if abs(in_angle1 - in_angle2) > 70 and abs(in_angle1 - in_angle2) < 290 then
			return false;
		end if;
	end loop;
	
	return true;
END;
$$;
------------------------------------------------------------------------------------------------------
--caution
------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_get_caution_passlid(pass_array bigint[],in_link_array bigint[],out_link_array bigint[])
	RETURNS bigint[]
	LANGUAGE PLPGSQL
AS $$
DECLARE
BEGIN
	if array_upper(in_link_array,1) > 1 then
		pass_array := in_link_array[2:array_upper(in_link_array,1)] || pass_array;
	end if;
	
	if array_upper(out_link_array,1) > 1 then
		pass_array := pass_array || out_link_array[1:array_upper(out_link_array,1)-1];
	end if;
	return pass_array;
END;
$$;

CREATE OR REPLACE FUNCTION mid_get_caution_in_link(ilink_id bigint,inode_id bigint)
	RETURNS varchar[]
	LANGUAGE plpgsql
AS $$
DECLARE
	curs1 refcursor;
	rec1 record;
	curs2 refcursor;
	rec2 record;
	
	link_array varchar[];
	link_array_one bigint[];
	link_array_one_num integer;

	node_temp bigint;
	ilast_num integer;
	icurrent_num integer;
	
	link_appear_array bigint[];
	link_array_temp varchar[];
	link_array_temp_num integer;
	
BEGIN
	link_appear_array := array[ilink_id];
	
	icurrent_num := 1;
	link_array[icurrent_num] := inode_id::varchar;

	link_array_temp_num := 1;
	ilast_num := 1;
	while ilast_num <= icurrent_num loop
		link_array_one := (regexp_split_to_array(link_array[ilast_num],E'\\|+'))::bigint[];
		link_array_one_num := array_upper(link_array_one,1);
		
		if ilast_num > 1 then
			open curs2 for select node_id from temp_node_in_admin where node_id = link_array_one[link_array_one_num];
			fetch curs2 into rec2;
			if rec2.node_id is null then
				close curs2;
				link_array_temp[link_array_temp_num] := array_to_string(link_array_one[1:link_array_one_num-1],'|');
				link_array_temp_num := link_array_temp_num + 1;
				ilast_num := ilast_num + 1;
				continue;
			end if;
			close curs2;
		end if;
		
		open curs1 for select link_id,s_node,e_node
				 				from link_tbl
				where ((s_node = link_array_one[link_array_one_num] and one_way_code in (1,3))
								or (e_node = link_array_one[link_array_one_num] and one_way_code in (1,2)))
								and road_type in (0,1) and link_type in(1,2);
								
		fetch curs1 into rec1;
		while rec1.link_id is not null loop	
			if rec1.link_id = any(link_appear_array) then
				fetch curs1 into rec1;
				continue;
			else
				link_appear_array := link_appear_array || array[rec1.link_id];
			end if;
			
			if link_array_one[link_array_one_num] = rec1.s_node then
				node_temp := rec1.e_node;
			else
				node_temp := rec1.s_node;
			end if;
			
			icurrent_num := icurrent_num + 1;	
			if link_array_one_num = 1 then
				link_array[icurrent_num] := rec1.link_id::varchar || '|' || node_temp::varchar;
			else
				link_array[icurrent_num] := rec1.link_id::varchar || '|' || array_to_string(link_array_one[1:link_array_one_num-1],'|')
														|| '|' || node_temp::varchar;
			end if;
			fetch curs1 into rec1;
		end loop;
		close curs1;
		ilast_num := ilast_num + 1;
	end loop;
	 				
	return link_array_temp;
END;
$$;

CREATE OR REPLACE FUNCTION mid_get_caution_out_link(ilink_id bigint,inode_id bigint)
	RETURNS varchar[]
	LANGUAGE plpgsql
AS $$
DECLARE
	curs1 refcursor;
	rec1 record;
	curs2 refcursor;
	rec2 record;
	
	link_array varchar[];
	link_array_one bigint[];
	link_array_one_num integer;

	node_temp bigint;
	ilast_num integer;
	icurrent_num integer;
	
	link_appear_array bigint[];
	link_array_temp varchar[];
	link_array_temp_num integer;
	
BEGIN
	link_appear_array := array[ilink_id];
	
	icurrent_num := 1;
	link_array[icurrent_num] := inode_id::varchar;

	link_array_temp_num := 1;
	ilast_num := 1;
	while ilast_num <= icurrent_num loop
		link_array_one := (regexp_split_to_array(link_array[ilast_num],E'\\|+'))::bigint[];
		link_array_one_num := array_upper(link_array_one,1);
		
		if ilast_num > 1 then
			open curs2 for select node_id from temp_node_in_admin where node_id = link_array_one[link_array_one_num];
			fetch curs2 into rec2;
			if rec2.node_id is null then
				close curs2;
				link_array_temp[link_array_temp_num] := array_to_string(link_array_one[1:link_array_one_num-1],'|');
				link_array_temp_num := link_array_temp_num + 1;
				ilast_num := ilast_num + 1;
				continue;
			end if;
			close curs2;
		end if;
		
		open curs1 for select link_id,s_node,e_node
				 				from link_tbl
				where ((s_node = link_array_one[link_array_one_num] and one_way_code in (1,2))
								or (e_node = link_array_one[link_array_one_num] and one_way_code in (1,3)))
								and road_type in (0,1) and link_type in(1,2);
								
		fetch curs1 into rec1;
		while rec1.link_id is not null loop	
			if rec1.link_id = any(link_appear_array) then
				fetch curs1 into rec1;
				continue;
			else
				link_appear_array := link_appear_array || array[rec1.link_id];
			end if;
			
			if link_array_one[link_array_one_num] = rec1.s_node then
				node_temp := rec1.e_node;
			else
				node_temp := rec1.s_node;
			end if;
			
			icurrent_num := icurrent_num + 1;
			if link_array_one_num = 1 then
				link_array[icurrent_num] := rec1.link_id::varchar || '|' || node_temp::varchar;
			else
				link_array[icurrent_num] := array_to_string(link_array_one[1:link_array_one_num-1],'|') || '|' || rec1.link_id::varchar
														|| '|' || node_temp::varchar;
			end if;
			fetch curs1 into rec1;
		end loop;
		close curs1;
		ilast_num := ilast_num + 1;
	end loop;
	 				
	return link_array_temp;
END;
$$;


CREATE OR REPLACE FUNCTION mid_convert_summer_time()
    RETURNS smallint
    LANGUAGE plpgsql
AS $$
DECLARE
	rec					record;
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
		select 	dst_id, dst_observed,
				dst_start_weekday, dst_start_day, dst_start_month, dst_start_time, 
				dst_end_weekday, dst_end_day, dst_end_month, dst_end_time
		from rdf_admin_dst
		order by dst_id
    LOOP
    	if rec.dst_observed = 'N' then
			insert into temp_admin_dst_mapping(dst_id, summer_time_id)
				values(rec.dst_id, 0);
    	else
	    	-- summer_time_id
	    	summer_time_id = summer_time_id + 1;
	    	
	    	-- weekday
	    	if rec.dst_start_weekday = 8 then
	    		start_weekday = 0;
	    	else
	    		start_weekday = (1::smallint) << (rec.dst_start_weekday - 1);
	    	end if;
	    	
	    	if rec.dst_end_weekday = 8 then
	    		end_weekday = 0;
	    	else
	    		end_weekday = (1::smallint) << (rec.dst_end_weekday - 1);
	    	end if;
	    	
	    	-- date
	    	if rec.dst_start_day <= 31 then
	    		start_date = rec.dst_start_month * 100 + rec.dst_start_day;
	    	else
	    		start_date = rec.dst_start_month * 100;
	    		start_weekday = start_weekday | ((1::smallint) << (rec.dst_start_day - 34));
	    	end if;
	    	
	    	if rec.dst_end_day <= 31 then
	    		end_date = rec.dst_end_month * 100 + rec.dst_end_day;
	    	else
	    		end_date = rec.dst_end_month * 100;
	    		end_weekday = end_weekday | ((1::smallint) << (rec.dst_end_day - 34));
	    	end if;
	    	
	    	-- time
	    	start_time = rec.dst_start_time;
	    	end_time = rec.dst_end_time;
	    	
	    	-- set records
	    	insert into mid_admin_summer_time
	    				(summer_time_id, start_weekday, start_date, start_time, end_weekday, end_date, end_time)
	    		values	(summer_time_id, start_weekday, start_date, start_time, end_weekday, end_date, end_time);
	    	
			insert into temp_admin_dst_mapping(dst_id, summer_time_id)
				values(rec.dst_id, summer_time_id);
    	end if;
    END LOOP;
    
	return summer_time_id;
END;
$$;


CREATE OR REPLACE FUNCTION mid_convert_guidence_condition()
    RETURNS smallint
    LANGUAGE plpgsql
AS $$
DECLARE
	rec					record;
	nCount				integer;
	nIndex				integer;
	condition_id		smallint;
	day_of_week			smallint;
	start_date			smallint;
	end_date			smallint;
	start_time			smallint;
	end_time			smallint;
	exclude_date		smallint;
BEGIN
	condition_id = 0;
	for rec in
		select 	distinct b.*
		from rdf_asso_dt as a
		left join rdf_date_time as b
		on a.dt_id = b.dt_id
		order by b.dt_id
    LOOP
    	condition_id	:= condition_id + 1;
    	
    	if rec.datetime_type = '1' then
    		-- DAY OF WEEK
    		start_date		:= 0;
			end_date		:= 0;
			day_of_week		:= 0;
			nIndex			:= 1;
			nCount			:= 8;
			while nIndex <= nCount LOOP
				if substr(rec.start_date, nIndex, 1) = 'X' then
					day_of_week	:= day_of_week | (1 << (nIndex-1));
				end if;
				nIndex	:= nIndex + 1;
			END LOOP;
		elseif rec.datetime_type = '2' and upper(rec.start_date) = 'EASTER' then
			-- EASTER
    		start_date		:= 0;
			end_date		:= 0;
			day_of_week		:= 0;
			--day_of_week		:= day_of_week | 127;
			--day_of_week		:= day_of_week | (1 << 7);
			--day_of_week		:= day_of_week | (1 << 12);
		elseif rec.datetime_type = 'A' then
			-- DATE RANGES
			start_date		:= cast(rec.start_date as integer);
			end_date		:= cast(rec.end_date as integer);
			day_of_week		:= 0;		
		elseif rec.datetime_type = 'C' then
			start_date		:= cast(substr(rec.start_date, 1, 4) as integer);
			end_date		:= cast(substr(rec.end_date, 1, 4) as integer);			
			day_of_week		:= 0;		
		elseif rec.datetime_type = 'D' then
			-- DAY OF WEEK OF MONTH
			start_date		:= 0;
			end_date		:= 0;	
			day_of_week		:= 0;
			day_of_week		:= day_of_week | (1 << (cast(substr(rec.start_date, 1, 4) as integer) - 1));
			day_of_week		:= day_of_week | (2 << 7);
			day_of_week		:= day_of_week | (1 << (cast(substr(rec.start_date, 5, 4) as integer) + 8));
		elseif rec.datetime_type = 'E' then
			-- DAY OF WEEK OF YEAR
			start_date		:= 0;
			end_date		:= 0;
			day_of_week		:= 0;
			day_of_week		:= day_of_week | (1 << (cast(substr(rec.start_date, 1, 4) as integer) - 1));
			day_of_week		:= day_of_week | (3 << 7);
			day_of_week		:= day_of_week | (cast(substr(rec.start_date, 5, 4) as integer) << 9);
		elseif rec.datetime_type = 'F' then
			-- WEEK OF MONTH
			start_date		:= 0;
			end_date		:= 0;
			day_of_week		:= 0;
			day_of_week		:= day_of_week | 127;
			day_of_week		:= day_of_week | (2 << 7);
			day_of_week		:= day_of_week | (cast(substr(rec.start_date, 1, 4) as integer) << 9);
		elseif rec.datetime_type = 'H' then
			-- MONTH OF YEAR
			start_date		:= cast(substr(rec.start_date, 1, 4) as integer) * 100 ;
			end_date		:= cast(substr(rec.end_date, 1, 4) as integer) * 100;
			day_of_week		:= 0;
		elseif rec.datetime_type = 'I' then
			-- DAY OF MONTH OF YEAR
			start_date		:= cast(substr(rec.start_date, 5, 4) as integer) * 100 + cast(substr(rec.start_date, 1, 4) as integer);
			end_date		:= cast(substr(rec.end_date, 5, 4) as integer) * 100 + cast(substr(rec.end_date, 1, 4) as integer);
			day_of_week		:= 0;
		else
			raise EXCEPTION 'error, new datetime type of regulation can not be handled!';
    	end if;
    	
    	if rec.start_time is null then
    		start_time		:= 0;
    	else
			start_time		:= cast(rec.start_time as integer);
		end if;
		
		if rec.end_time is null then
    		end_time		:= 0;
    	else
			end_time		:= cast(rec.end_time as integer);
		end if;
		
		if rec.exclude_date = 'Y' then
			exclude_date	:= 1;
		else
			exclude_date	:= 0;
		end if;
		
    	if day_of_week = 127 then
    		day_of_week = 0;
    	end if;
	    
    	-- set records
    	insert into guidence_condition_tbl
    				(condition_id, week_flag, start_date, start_time, end_date, end_time)
    		values	(summer_time_id, day_of_week, start_date, start_time, end_date, end_time);
    	
		insert into temp_guidence_datetime_mapping(dt_id, condition_id)
			values(rec.dt_id, condition_id);
    END LOOP;
    
	return condition_id;
END;
$$;
CREATE OR REPLACE FUNCTION mid_get_outlink_for_admin(ilink_id bigint,inode_id bigint, admin bigint)
	RETURNS varchar[]
	LANGUAGE plpgsql
AS $$
DECLARE
	curs1 refcursor;
	rec1 record;
	curs2 refcursor;
	rec2 record;
	
	link_array varchar[];
	link_array_one bigint[];
	link_array_one_num integer;

	node_temp bigint;
	ilast_num integer;
	icurrent_num integer;
	
	link_appear_array bigint[];
	link_array_temp varchar[];
	link_array_temp_num integer;
	
BEGIN
	link_appear_array := array[ilink_id];
	
	icurrent_num := 1;
	link_array[icurrent_num] := inode_id::varchar;

	link_array_temp_num := 1;
	ilast_num := 1;
	while ilast_num <= icurrent_num loop
		link_array_one := (regexp_split_to_array(link_array[ilast_num],E'\\|+'))::bigint[];
		link_array_one_num := array_upper(link_array_one,1);
		
		open curs1 for select link_id, ref_node_id, nonref_node_id, left_admin, right_admin
				 				from temp_rdf_nav_link_admin_need
				where ((ref_node_id = link_array_one[link_array_one_num] and one_way_code in (1,2))
								or (nonref_node_id = link_array_one[link_array_one_num] and one_way_code in (1,3)));
								
		fetch curs1 into rec1;
		while rec1.link_id is not null loop	
			if rec1.link_id = any(link_appear_array) or (rec1.left_admin = admin and  rec1.right_admin = rec1.left_admin) then
				fetch curs1 into rec1;
				continue;
			else
				link_appear_array := link_appear_array || array[rec1.link_id::bigint];
			end if;
			
			if link_array_one[link_array_one_num] = rec1.ref_node_id then
				node_temp := rec1.nonref_node_id;
			else
				node_temp := rec1.ref_node_id;
			end if;
			
			if rec1.left_admin = rec1.right_admin then
				if link_array_one_num = 1 then
					link_array_temp[link_array_temp_num] := rec1.link_id::varchar;
				else
					link_array_temp[link_array_temp_num] := array_to_string(link_array_one[1:link_array_one_num-1],'|') || '|' || rec1.link_id::varchar;	
				end if;
				link_array_temp_num := link_array_temp_num + 1;
			else
				icurrent_num := icurrent_num + 1;
				if link_array_one_num = 1 then
					link_array[icurrent_num] := rec1.link_id::varchar || '|' || node_temp::varchar;
				else
					link_array[icurrent_num] := array_to_string(link_array_one[1:link_array_one_num-1],'|') || '|' || rec1.link_id::varchar
															|| '|' || node_temp::varchar;
				end if;
			end if;

			fetch curs1 into rec1;
		end loop;
		close curs1;
		ilast_num := ilast_num + 1;
	end loop;
	 				
	return link_array_temp;
END;
$$;
-----------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_adas_link_delete_addad_node(seq_array bigint[],lat_array bigint[], lon_array bigint[],
										z_coord_array bigint[], flag_array varchar[], slope_array bigint[])
	RETURNS varchar[]
	LANGUAGE plpgsql
AS $$
DECLARE
	flag_array_num integer;
	real_node_num integer;
	geom_array geometry[];
	geom_array_num integer;
	geom_point geometry;
	
	slope_temp integer;
	slope_one integer;

	all_array bigint[];
	
	link_array_temp varchar[];
	link_array_temp_num integer;
BEGIN
	--alter slope
	flag_array_num := array_upper(flag_array,1);	
	for flag_array_num_idx in 1..flag_array_num loop
		if flag_array[flag_array_num_idx] = 'Y' then
			if flag_array_num_idx = 1 then
				return null;
			end if;
			if flag_array[flag_array_num_idx - 1] = 'Y' then
				continue;
			else
				real_node_num := mid_get_adas_real_num(flag_array_num_idx,flag_array);
				slope_array[flag_array_num_idx - 1] = mid_get_adas_slope(flag_array_num_idx - 1, real_node_num, lat_array, lon_array, z_coord_array, slope_array);
			end if;
		end if;
	end loop;
	
	--get array
	slope_temp := -1000;
	geom_array_num := 0;
	link_array_temp_num := 0;
	for flag_array_num_idx in 1..flag_array_num loop
		if flag_array[flag_array_num_idx] = 'Y' then
			continue;
		end if;
		
		slope_one := mid_transform_slope_value(slope_array[flag_array_num_idx]/1000.0);
		geom_point := ST_GeogFromText('SRID=4326;POINT(' || lon_array[flag_array_num_idx]/10000000.0 || ' ' || lat_array[flag_array_num_idx]/10000000.0 || ')');
		
		geom_array_num := geom_array_num + 1;
		geom_array[geom_array_num] := geom_point;
			
		if flag_array_num_idx = 1 then
			slope_temp := slope_one;
		elsif flag_array_num_idx = flag_array_num then
			link_array_temp_num := link_array_temp_num + 1;
			link_array_temp[link_array_temp_num] := slope_temp::varchar || '|' || array_to_string(geom_array,'|');
		else
			if slope_temp <> slope_one then
				link_array_temp_num := link_array_temp_num + 1;
				link_array_temp[link_array_temp_num] := slope_temp::varchar || '|' || array_to_string(geom_array,'|');
				geom_array_num := 0;
				geom_array = null;
				geom_array_num := geom_array_num + 1;
				geom_array[geom_array_num] := geom_point;
				slope_temp := slope_one;
			end if;
		end if;

	end loop;
		
	return link_array_temp;
END;
$$;

CREATE OR REPLACE FUNCTION mid_get_adas_slope(first_num integer, end_num integer, lat_array bigint[], lon_array bigint[], z_coord_array bigint[], slope_array bigint[])
	RETURNS bigint
	LANGUAGE plpgsql
AS $$
DECLARE
	len double precision;
	len_all double precision;
	slope_all double precision;
BEGIN
	
	len_all = 0.0;
	slope_all = 0.0;
	
	for idx in first_num..(end_num - 1) loop
		len = mid_get_adas_len_3D(lon_array[idx], lat_array[idx], z_coord_array[idx], lon_array[idx + 1], lat_array[idx + 1], z_coord_array[idx + 1]);
		len_all := len_all + len;
		slope_all := slope_all + slope_array[idx] * len;	
	end loop;
	
	return (slope_all/len_all)::bigint;
END;
$$;
CREATE OR REPLACE FUNCTION mid_get_adas_len_3D(s_x bigint, s_y bigint, s_z bigint, e_x bigint, e_y bigint, e_z bigint)
	RETURNS double precision
	LANGUAGE plpgsql
AS $$
DECLARE
	s_x_d double precision;
	s_y_d double precision;
	s_z_d double precision;
	e_x_d double precision;
	e_y_d double precision;
	e_z_d double precision;
BEGIN
	s_x_d = s_x/10000000.0;
	s_y_d = s_y/10000000.0;
	s_z_d = s_z/100.0;
	e_x_d = e_x/10000000.0;
	e_y_d = e_y/10000000.0;
	e_z_d = e_z/100.0;
	
	return ST_Length_Spheroid(ST_MakeLine(ST_SetSRID(ST_MakePoint(s_x_d,s_y_d,s_z_d),4326), ST_SetSRID(ST_MakePoint(e_x_d,e_y_d,e_z_d),4326)), 'SPHEROID("WGS_84", 6378137, 298.257223563)');

END;
$$;


CREATE OR REPLACE FUNCTION mid_get_adas_real_num(seq_num integer, flag_array varchar[])
	RETURNS integer
	LANGUAGE plpgsql
AS $$
DECLARE
	flag_array_num integer;
BEGIN
	flag_array_num := array_upper(flag_array,1);	
	for flag_array_num_idx in seq_num..flag_array_num loop
		if flag_array[flag_array_num_idx] = 'N' then
			return flag_array_num_idx;
		end if;
	end loop;
	
	return -1;
END;
$$;

CREATE OR REPLACE FUNCTION mid_transform_slope_value(slope double precision)
	RETURNS integer
	LANGUAGE plpgsql
AS $$
DECLARE
BEGIN
	RETURN CASE
		WHEN slope >= -0.5 and  slope <= 0.5 THEN 0
		WHEN slope > 0.5 and  slope <= 1.5 THEN 10
		WHEN slope > 1.5 and  slope <= 2.5 THEN 20
		WHEN slope > 2.5 and  slope <= 3.5 THEN 30
		WHEN slope > 3.5 THEN 40
		WHEN slope >= -1.5 and  slope < -0.5 THEN -10
		WHEN slope >= -2.5 and  slope < -1.5 THEN -20
		WHEN slope >= -3.5 and  slope < -2.5 THEN -30
		WHEN slope < -3.5 THEN -40
		ELSE 0
	END;
END;
$$;


-----------------------------------------------------------------------------------------------

CREATE OR REPLACE FUNCTION mid_find_highaccid_links(orgTable character varying, objTable character varying)
  RETURNS integer 
  LANGUAGE plpgsql VOLATILE
AS $$ 
DECLARE
	rec				record;
	curs 				refcursor;
	tmp_rec            		record;
	pos_rec				record;
	neg_rec				record;
	start_node			bigint;
	end_node			bigint;
	link_array			bigint[];
	nCount_pos			integer;
	nCount_neg			integer;
	nCount				integer;
	nIndex				integer;
	group_id			integer;
	seq				integer;
	array_walked			bigint[];				
BEGIN

	group_id := 0;
	
	open curs for execute 'select * from ' || quote_ident(orgTable);	
	fetch curs into rec;
	
	while rec.link_id is not null 
	LOOP
		select link_id from temp_link_walked where link_id = rec.link_id into tmp_rec;
		
		if found then
			fetch curs into rec;
			continue;
		else 
			insert into temp_link_walked values (rec.link_id);
		end if;	

		pos_rec			:= mid_find_highaccid_links_in_one_direction(orgTable, rec.link_id, rec.s_node);
		end_node		:= pos_rec.reach_node;

		neg_rec			:= mid_find_highaccid_links_in_one_direction(orgTable, rec.link_id, rec.e_node);
		start_node		:= neg_rec.reach_node;

		nCount_pos		:= array_upper(pos_rec.linkrow, 1);		
		nCount_neg		:= array_upper(neg_rec.linkrow, 1);

		IF nCount_pos IS NOT NULL and nCount_neg IS NOT NULL THEN
		
			group_id 	:= group_id + 1;
			
			link_array		:= ARRAY[rec.link_id];
			
			fOR nIndex in 2..nCount_neg loop
				link_array	:= array_prepend(neg_rec.linkrow[nIndex], link_array);
			end loop;
			
			fOR nIndex in 2..nCount_pos loop
				link_array	:= array_append(link_array, pos_rec.linkrow[nIndex]);
			end loop;

			nCount			:= array_upper(link_array, 1);			
			nIndex := 1;
				
			while nIndex  <= nCount loop	
				execute 'insert into ' || quote_ident(objTable) || '(link_id,group_id) 
				values('||link_array[nIndex]||','||group_id||')';
				nIndex := nIndex + 1;	
			end loop;
		END IF;	

	fetch curs into rec;	
	END LOOP;

	close curs;
	RETURN 1;
	
END;
$$;


CREATE OR REPLACE FUNCTION mid_find_highaccid_links_in_one_direction( 
	orgTable character varying, search_link bigint,
	search_node int, OUT linkrow bigint[], OUT reach_node bigint)
RETURNS record
LANGUAGE plpgsql volatile
AS $$
DECLARE
	rec2			record;
	curs2 			refcursor;
	reach_link              bigint;
	
	tmpLastNodeArray	bigint[];
	tmpPathArray		bigint[];
	tmpPathCount		integer;
	tmpPathIndex		integer;	

	nIndex			integer;
	nCount			integer;
	sqlcmd			character varying;	
BEGIN
	reach_link	:= search_link;
	reach_node	:= search_node;

	tmpLastNodeArray	:= ARRAY[reach_node];
	tmpPathArray		:= ARRAY[reach_link];
	tmpPathCount		:= 1;
	tmpPathIndex		:= 1;
	linkrow 		:= ARRAY[reach_link];

	WHILE tmpPathIndex <= tmpPathCount LOOP

		open curs2 for execute
		'select	*
		from
		(
			(
				select	link_id, e_node as nextnode
				from '|| quote_ident(orgTable) || '
				where 	(' || tmpLastNodeArray[tmpPathIndex] || '= s_node) 
					and (link_id != ' || reach_link || ')
					and not (link_id = any(' || quote_literal(tmpPathArray) || '))
			)
			union
			(
				select	link_id, s_node as nextnode
				from '|| quote_ident(orgTable) || '
				where 	(' || tmpLastNodeArray[tmpPathIndex] || ' = e_node) 
					and (link_id != ' || reach_link || ')
					and not (link_id = any(' || quote_literal(tmpPathArray) || '))
			)
		)as a ';

		fetch curs2 into rec2;
	
		while rec2.link_id is not null 
		LOOP
			insert into temp_link_walked values (rec2.link_id);
			tmpPathCount	:= tmpPathCount + 1;
			linkrow		:= array_append(linkrow, rec2.link_id);
			reach_link	:= rec2.link_id;
			reach_node	:= rec2.nextnode;
			tmpLastNodeArray := array_append(tmpLastNodeArray, cast(rec2.nextnode as bigint));
			tmpPathArray	:= array_append(tmpPathArray,cast(rec2.link_id as bigint));
		fetch curs2 into rec2;
		end loop;

		tmpPathIndex := tmpPathIndex + 1;

		close curs2;
	end loop;
END;
$$;

-------------------------------------------------------------------------------------------------------------
-- update mid_temp_force_guide_tbl
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION rdf_update_mid_temp_force_guide_tbl(  ) 
  RETURNS  smallint
  LANGUAGE plpgsql
AS $$
DECLARE
        rec record;
        link_count bigint;
        node_count bigint;
BEGIN
        link_count := 0;
        node_count := 0;

        for rec in (
            select condition_id, array_agg(link_id) as link_array, array_agg(node_id) as node_array
            from (
                select condition_id, seq_num, link_id, node_id
                from rdf_condition a
                inner join rdf_nav_strand b
                    on a.nav_strand_id = b.nav_strand_id
                where condition_type = 14
                order by condition_id, seq_num
            ) as c
            group by condition_id
        )
        loop
            link_count := array_upper(rec.link_array, 1);
            node_count := array_upper(rec.node_array, 1);

	    -- forceguide at least 2 link and 1 node, and first node in node_array must be not null
            if (link_count >= 2) and (node_count >= 1 and rec.node_array[1] is not null) then
				if link_count = 2 then
					insert into mid_temp_force_guide_tbl (nodeid, inlinkid, outlinkid, passlid, passlink_cnt, guide_type)
					values(rec.node_array[1]::bigint, rec.link_array[1]::bigint, rec.link_array[link_count]::bigint, null, link_count-2, 0);
				elseif link_count > 2 then
					insert into mid_temp_force_guide_tbl (nodeid, inlinkid, outlinkid, passlid, passlink_cnt, guide_type)
					values(rec.node_array[1]::bigint, rec.link_array[1]::bigint, rec.link_array[link_count]::bigint, array_to_string(rec.link_array[2:link_count-1], '|'), link_count-2, 0);
				end if;
            else 
                raise INFO 'rec = %', rec;
            end if;
        end loop;

        return 0;
END;
$$;

-----------------------------------------------------------------------------------------------
-- deal with bug: no sapa link in sapa   --main function
-----------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_hwy_premote_sapa_link()
	RETURNS smallint
	LANGUAGE plpgsql volatile
AS $$
DECLARE
	rec record;
BEGIN
	for rec in 
		select  a.link_id , a.one_way_code,
		        case
				  when a.s_node = b.s_node or a.s_node = b.e_node then a.s_node
				  else a.e_node
				end as node
		from (select  link_id, s_node, e_node, one_way_code
				from link_tbl
				where link_type = 7
			 )as a
		left join (
			 select link_id, road_type, link_type,s_node, e_node, one_way_code
			 from link_tbl
			 where road_type <>0 and link_type not in (3,5,7)
			 )as b
		on
		(
			(a.one_way_code in (1, 2) and
				(
					( a.s_node = b.e_node and b.one_way_code in (1, 2))
					or
					( a.s_node = b.s_node and b.one_way_code in (1, 3))
					or
					( a.e_node = b.s_node and b.one_way_code in (1, 2))
					or
					( a.e_node = b.e_node and b.one_way_code in (1, 3))
				)
			)
			or
			( a.one_way_code in (1, 3) and
				(
					( a.s_node = b.e_node and b.one_way_code in (1, 3))
					or
					( a.s_node = b.s_node and b.one_way_code in (1, 2))
					or
					( a.e_node = b.s_node and b.one_way_code in (1, 3))
					or
					( a.e_node = b.e_node and b.one_way_code in (1, 2))
				)
			)
		)
		where b.link_id is not null
	loop 
		perform premote_link_type_sapa(rec.link_id, rec.node, 1);
		perform premote_link_type_sapa(rec.link_id, rec.node, 2);
	end loop;
	return 1;
END;
$$;
-----------------------------------------------------------------------------------------------
-- premote_link_type_sapa
-----------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION premote_link_type_sapa(link_id bigint, node bigint, dir integer)
	RETURNS smallint
	LANGUAGE plpgsql volatile
AS $$
DECLARE
	rec record;
	isolated_path varchar[];
BEGIN
    for rec in 
	    select link
		from (
		     select distinct regexp_split_to_table(
			        array_to_string(
						get_isolated_sapa_path(link_id::bigint, node::bigint, dir::integer),'|'),E'\\|+'
									)AS link
			 )AS isolated_link
		where isolated_link.link <> ''
    loop
		if check_connected_main_path(cast(rec.link as bigint)) = 1 then
			perform premote_link(cast(rec.link as bigint));
		else
			continue;
		end if;
    end loop;
    return 1;
END;
$$;
-----------------------------------------------------------------------------------------------
--get_isolated_sapa_path
-----------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION get_isolated_sapa_path(link bigint,node bigint, dir integer)
	RETURNS character varying[]
	LANGUAGE plpgsql volatile
AS $$
DECLARE
	rec record;
	temp_path varchar;
	temp_path_array varchar[];
	visited_node_array bigint[];

	path_index integer;
	path_index_count integer;

BEGIN
	path_index := 1;
	path_index_count := 1;
	visited_node_array := ARRAY[node];
	temp_path_array := ARRAY[null];


	--search
	WHILE path_index <= path_index_count LOOP
		temp_path := temp_path_array[path_index];--search from current path
		for rec in
			select
				case 
				  when s_node =visited_node_array[path_index]  then e_node
				  else s_node
				end as next_node,
			        link_id as next_link ,
			        road_type, link_type, one_way_code, s_node, e_node
			from link_tbl
			where road_type <> 0 and link_type not in (3,5,7)
			and 
			 (
				(
					(dir = 1) and
					(
						(s_node = visited_node_array[path_index] and one_way_code in (1,2))
						or
						(e_node = visited_node_array[path_index] and one_way_code in (1,3))	
					)
				)
				or
				(
					(dir = 2) and
					(
						(s_node = visited_node_array[path_index] and one_way_code in (1,3))
						or
						(e_node = visited_node_array[path_index] and one_way_code in (1,2))
					)
				)
			 )
			 and
			 (
				temp_path_array[path_index] is null
			        or
				not (cast(link_id as varchar) = ANY(regexp_split_to_array(temp_path_array[path_index], E'\\|+')))
			 )
		loop
			

			temp_path := temp_path_array[path_index];

			visited_node_array := array_append(visited_node_array, cast(rec.next_node as bigint));
				
			if temp_path is not null then
				temp_path_array :=  array_append(temp_path_array, cast(temp_path||'|'||rec.next_link as varchar));
			else 
				temp_path_array :=  array_append(temp_path_array, cast(rec.next_link as varchar));
			end if;

			if array_upper(regexp_split_to_array(temp_path, E'\\|+'), 1) > 8 then	
				return ARRAY[null];
			end if;
				
			path_index_count := path_index_count +1;


		end loop;
			
		path_index := path_index + 1 ;
		temp_path  := temp_path_array[path_index];

	END LOOP;
	return temp_path_array ;
END;
$$;
-----------------------------------------------------------------------------------------------
--check_connected_main_path
-----------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION check_connected_main_path(link bigint)
	RETURNS smallint
	LANGUAGE plpgsql volatile
AS $$
DECLARE
	rec record;
BEGIN
	for rec in
		select link_id, s_node, e_node, link_type, road_type, one_way_code
		from link_tbl
		where link_id = link
	loop
		if rec.one_way_code in (1,2) then
			if is_connected_main_path(rec.e_node, 1) = 1 or is_connected_main_path(rec.s_node, 2) = 1 then
				return 1;
			end if;
		end if;

		if rec.one_way_code in (1,3) then
		    if is_connected_main_path(rec.s_node, 1) = 1 or is_connected_main_path(rec.e_node, 2) = 1 then
				return 1;
			end if;
		end if;
	end loop;
	return 0;
END;
$$;
-----------------------------------------------------------------------------------------------
--premote_link
-----------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION premote_link(link bigint)
	RETURNS smallint
	LANGUAGE plpgsql volatile
AS $$
DECLARE
	rec record;
BEGIN
	insert into temp_zh values(link);
	return 1;
END;
$$;
-----------------------------------------------------------------------------------------------
--is_connected_main_path
-----------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION is_connected_main_path(node bigint, dir integer)
	RETURNS smallint
	LANGUAGE plpgsql volatile
AS $$
DECLARE
	rec record;
	temp_path varchar;
	visited_node_array bigint[];
	temp_path_array varchar[];
	path_index_count integer;
	path_index integer;
BEGIN
	visited_node_array := ARRAY[node];
	temp_path_array := ARRAY[null];
	path_index := 1;
	path_index_count := 1;
	
	WHILE path_index <= path_index_count LOOP
		for rec in
			select 
			  case 
			    when s_node = visited_node_array[path_index] then e_node
			    else s_node
			  end as next_node,
			  link_id as next_link, link_type, road_type
			from link_tbl
			where 
			(
				(
					(dir = 1) and
					(
						(s_node = visited_node_array[path_index] and one_way_code in (1, 2))
						or
						(e_node = visited_node_array[path_index] and one_way_code in (1, 3))
					)
				)
				or
				(
					(dir = 2)and
					(
						(e_node = visited_node_array[path_index] and one_way_code in (1, 2))
						or
						(s_node = visited_node_array[path_index] and one_way_code in (1, 3))
					)
				)
			)
			and
			(
				temp_path_array[path_index] is null
				or
				not (cast(link_id as varchar) = ANY(regexp_split_to_array(temp_path_array[path_index], E'\\|+')))
			)
		loop
			if rec.road_type = 0 and rec.link_type in (1, 2)then
				return 1;
			else
				path_index_count := path_index_count + 1;
				visited_node_array := array_append(visited_node_array, cast(rec.next_node as bigint));
				if temp_path is null then
					temp_path_array := array_append(temp_path_array, cast(rec.next_link as varchar));
				else
					temp_path_array := array_append(temp_path_array, cast(temp_path||'|'||rec.next_link as varchar));
				end if;
			end if;
		end loop;
		
		path_index := path_index + 1;
		temp_path := temp_path_array[path_index];
	END LOOP;
	return 0;
END;
$$;



--------

create or replace function mid_make_lanenum_lr()
returns smallint 
language plpgsql
as $$
declare
	rec record;
	rec2 record;
	tmprec record;
	lane_num_t int;
	lane_num_f int;
	lane_num_b int;
	lane_num int;
	total int;
	node_id_t bigint;
	lanenum_left int;
	form_end int;
	lanenum_right int;
	lane_add_num int;
	link_id_list bigint[];
	link_id_list_laneright bigint[];
	angle_list int[];
	idx int;
	node_id_1 bigint;
	node_0 geometry;
	node_1 geometry;
	node_2 geometry;
	angle_turn int;
	in_link_id bigint;
	lanenum_bef int;
	cnt int;
	angle_m1 int;
	angle_m2 int;
	num smallint;
	
begin
	execute ' drop table if exists temp_lanenum_lr;
	          create table temp_lanenum_lr
	          (
			link_id bigint,
			node_id bigint,
			total_lane_number int,
			lane_number int,
			forming_ending smallint
	          );

		  drop table if exists mid_lanenum_lr;
		  create table mid_lanenum_lr
		  (
			link_id bigint,
			node_id bigint,
			lanenum_l smallint,
			lanenum_r smallint
		  );
	          ';
	for rec in 
	select * from rdf_lane where lane_forming_ending in (1,2) --lane_forming_ending in (2,3)
	loop
		select count(*) into lane_num_t from rdf_lane where link_id=rec.link_id
						and lane_travel_direction='T';
		if lane_num_t is null then
			lane_num_t=0;
		end if;

		select count(*) into lane_num_f from rdf_lane where link_id=rec.link_id
						and lane_travel_direction='F';
		if lane_num_f is null then
			lane_num_f=0;
		end if;

		select count(*) into lane_num_b from rdf_lane where link_id=rec.link_id
						and lane_travel_direction='B';
		if lane_num_b is null then
			lane_num_b=0;
		end if;

		if rec.lane_travel_direction in ('T','F') then
		
			if rec.lane_travel_direction='T' then
				lane_num=lane_num_t+lane_num_b+1-rec.lane_number;
				total=lane_num_t+lane_num_b;
			else
				lane_num=rec.lane_number-lane_num_t;
				total=lane_num_f+lane_num_b;
			end if;

			--raise info '%,%,%,%',rec.link_id,lane_num_t,lane_num_b,lane_num_f;
			
			select case when rec.lane_travel_direction ='T' then ref_node_id else nonref_node_id end 
			into node_id_t
			from rdf_link
			where link_id=rec.link_id;

			insert into temp_lanenum_lr
			values(rec.link_id,node_id_t,total,lane_num,rec.lane_forming_ending);
		else ---rec.lane_travel_direction='B'
			lane_num=lane_num_t+lane_num_b+1-rec.lane_number;
			total=lane_num_t+lane_num_b;

			select ref_node_id  
			into node_id_t
			from rdf_link
			where link_id=rec.link_id;

			insert into temp_lanenum_lr
			values(rec.link_id,node_id_t,total,lane_num,rec.lane_forming_ending);

			lane_num=rec.lane_number-lane_num_t-lane_num_b;
				total=lane_num_f+lane_num_b;
				
			select nonref_node_id  
			into node_id_t
			from rdf_link
			where link_id=rec.link_id;

			insert into temp_lanenum_lr
			values(rec.link_id,node_id_t,total,lane_num,rec.lane_forming_ending);
		end if;
	end loop;

	for rec2 in
	
		select link_id,node_id,total_lane_number,array_agg(lane_number) as lane_number_arr,array_agg(forming_ending) as forming_ending_Arr 
		from 
		(
			select * from temp_lanenum_lr
			order by link_id,node_id,total_lane_number,lane_number
		) a
		group by link_id,node_id,total_lane_number
	loop
		lanenum_left=0;
		lanenum_right=0;
		lane_add_num=array_upper(rec2.lane_number_arr,1);
		if lane_add_num=rec2.total_lane_number then
			raise info 'lane_add_num=total_lane_number,link_id=%',rec2.link_id;
			continue;
		end if;

		
		if rec2.lane_number_arr[1]=1 then
			idx=1;
			form_end=rec2.forming_ending_arr[1];
			while true loop
				idx=idx+1;
				if idx>lane_add_num or rec2.lane_number_arr[idx]<>rec2.lane_number_arr[idx-1]+1 or rec2.forming_ending_arr[idx]<>rec2.forming_ending_arr[idx-1] then
					exit;
				end if;
			end loop;
			lanenum_left=case when form_end = 1 then idx-1 else 1-idx end;
		end if;
		if rec2.lane_number_arr[lane_add_num]=rec2.total_lane_number then
			idx=lane_add_num;
			form_end=rec2.forming_ending_arr[lane_add_num];
			while true loop
				idx=idx-1;
				if idx<1 or rec2.lane_number_arr[idx]<>rec2.lane_number_arr[idx+1]-1 or rec2.forming_ending_arr[idx]<>rec2.forming_ending_arr[idx+1] then
					exit;
				end if;
			end loop;
			lanenum_right=case when form_end = 1 then lane_add_num-idx else idx-lane_add_num end;
		end if;
		
		if lane_add_num<>abs(lanenum_left)+abs(lanenum_right) then
			raise info 'error:  mid lane----. link_id=%',rec2.link_id;
			continue;
		end if;
		
		insert into mid_lanenum_lr--total_lane_number
		values(rec2.link_id,rec2.node_id,lanenum_left,lanenum_right);
	end loop;

	return 0;
end;
$$;