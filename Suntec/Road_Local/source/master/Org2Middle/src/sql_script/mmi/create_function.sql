-------------------------------------------------------------------------------------------------------------
-- Common function
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_get_connect_junction(link_a float, link_b float)
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

CREATE OR REPLACE FUNCTION mid_is_linkrow_continuable(linkrow bigint[])
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
		select 	m.link_id, seq_num, n.one_way, 
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
		left join temp_link_regulation_traffic as n
		on m.link_id = n.link_id
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
					return false;
				end if;
			elseif rec.enode_array[nIndex] in (rec.snode_array[nIndex+1], rec.enode_array[nIndex+1]) then
				if rec.oneway_array[nIndex] in (3,4) then
					--raise WARNING 'regulation linkrow error: traffic flow error, linkrow = %', linkrow;
					return false;
				end if;
			else
				--raise EXCEPTION 'regulation linkrow error: links have no intersection, linkrow = %', linkrow;
				return false;
			end if;
		else
			if rec.snode_array[nIndex] in (rec.snode_array[nIndex-1], rec.enode_array[nIndex-1]) then
				if rec.oneway_array[nIndex] in (3,4) then
					--raise WARNING 'regulation linkrow error: traffic flow error, linkrow = %', linkrow;
					return false;
				end if;
			elseif rec.enode_array[nIndex] in (rec.snode_array[nIndex-1], rec.enode_array[nIndex-1]) then
				if rec.oneway_array[nIndex] in (2,4) then
					--raise WARNING 'regulation linkrow error: traffic flow error, linkrow = %', linkrow;
					return false;
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

-------------------------------------------------------------------------------------------------------------
-- Admin function
-------------------------------------------------------------------------------------------------------------
---get countrycode array
CREATE OR REPLACE FUNCTION mid_admin_judge_country_array()
	RETURNS character varying[]
  LANGUAGE plpgsql
AS $$
DECLARE
	rec record;
	Countryarray character varying[];
	countryidx smallint;
BEGIN
	Countryarray = null;
	countryidx := 1;
	for rec in
	(
	  select "name" as name,id
	  from org_area
	  where kind = 10
	  group by "name",id
	)
	loop
	Countryarray[countryidx] := rec.name;
	countryidx := countryidx + 1;
	Countryarray[countryidx] := cast(rec.id as character varying);
	countryidx := countryidx + 1;
	end loop;
	
	return Countryarray;
END;
$$;
---start admin
CREATE OR REPLACE FUNCTION mid_admin_get_order0_8()
	RETURNS smallint
	LANGUAGE plpgsql
AS $$
DECLARE
	rec record;
	Country_array character varying[];
	country_count smallint;
	country_code varchar;
	country_id integer;
BEGIN
	country_count := 0;
	Country_array = mid_admin_judge_country_array();
	
	if Country_array is null then
		return 0;
	end if;
	
	country_count := array_upper(Country_array,1);
	if (country_count % 2) = 1 then
		raise EXCEPTION 'country number is error, count is = %', country_count;
	end if;
	
	country_code := '';
	country_id := 0;
	for country_count_idx in 1..country_count loop
		if (country_count_idx % 2) = 1 then
			if lower(Country_array[country_count_idx]) = 'india' then
				country_code = 'IND';
				country_id = 356;
			else
				raise EXCEPTION 'No found country name = %', Country_array[country_count_idx];
			end if;
			
			perform mid_admin_add_order8_geom(country_id,Country_array[country_count_idx + 1]::double precision,country_code);
			perform mid_admin_add_order1_geom(country_id,Country_array[country_count_idx + 1]::double precision,country_code);
			perform mid_admin_add_order0_geom(country_id,Country_array[country_count_idx + 1]::double precision,country_code);
				
		else
			continue;
		end if;
	end loop;
	return 0;
END;
$$;
--order8
CREATE OR REPLACE FUNCTION mid_admin_add_order8_geom(country_idx integer,country_id double precision,
																							 country_code_temp character varying)
	RETURNS integer
	LANGUAGE plpgsql
AS $$
DECLARE
	dirst_num integer;
	New_id_temp integer;
BEGIN
	---org_data
	insert into temp_admin_order8_part(order8_id,order1_id,country_code,the_geom)
  select  a.dst_id 						 				 as order8_id, 
	        a.parent_id 				 				 as order1_id, 
	        country_code_temp  					 as country_code,
	        ST_Multi(st_union(a.the_geom)) as the_geom
	from 	org_district_region as a
	left join  org_state_region as b
	on a.parent_id = b.stt_id
	where b.parent_id = country_id
	group by a.dst_id,a.parent_id
	order by a.dst_id,a.parent_id;
		
	---Difference from state	
	insert into temp_admin_order8_part(order8_id,order1_id,country_code,the_geom)
  select order8_id, order1_id, country_code_temp, the_geom
  from temp_admin_district_new_combine
  where st_area(the_geom) > 0.0001;
  
  insert into temp_admin_order8_compare(id_old, id_new)
  SELECT a.order8_id as id_old, (array_agg(b.order8_id))[1] as id_new_array
  FROM temp_admin_district_new_combine as a
  left join temp_admin_order8_part as b
  on a.order1_id = b.order1_id and ST_Intersects(a.the_geom,b.the_geom) and b.order8_id not like '%90000%'
  where st_area(a.the_geom) < 0.0001
  group by a.order8_id;
	
	insert into temp_admin_order8(order8_id,order1_id,country_code,the_geom)
	select a.order8_id,
         a.order1_id,
         a.country_code,
         (case when st_union(c.the_geom) is null then a.the_geom 
             else ST_Multi(st_union(a.the_geom, st_union(c.the_geom))) end) as the_geom
  from temp_admin_order8_part as a
  left join temp_admin_order8_compare as b
  on a.order8_id = b.id_new
  left join temp_admin_district_new_combine as c
  on b.id_old = c.order8_id
  group by a.order8_id, a.order1_id, a.country_code, a.the_geom;
  
	--- insert into temp_adminid_newandold
	New_id_temp := country_idx*100000 + 30000;
	insert into temp_adminid_newandold(ID_old,New_ID,level,country_code)
	select order8_id as ID_old,
				 New_id_temp + row_number()over(order by order8_id) as New_ID,
				 8::smallint as level,
				 country_code_temp  as country_code
	from temp_admin_order8;
	
	return dirst_num;
END;
$$;
--order01
CREATE OR REPLACE FUNCTION mid_admin_add_order1_geom(country_idx integer,country_id double precision,
																							 country_code_temp character varying)
	RETURNS integer
	LANGUAGE plpgsql
AS $$
DECLARE
	rec record;
	New_id_temp integer;
BEGIN
	---Difference from district
	insert into temp_admin_order1(order1_id,order0_id,country_code,the_geom)
	select a.stt_id 				 	 	as order1_id, 
				 country_id 				 	as order0_id, 
				 country_code_temp  	as country_code,
				 ST_Multi(d.the_geom) as the_geom
	from org_state_region as a
	left join
	(
		select order1_id,ST_Multi(st_union(the_geom)) as the_geom
		from temp_admin_order8
		where country_code = country_code_temp
		group by order1_id
		order by order1_id
	) as d
	on a.stt_id = d.order1_id::double precision
	where a.parent_id = country_id
	group by a.stt_id,d.the_geom
	order by a.stt_id,d.the_geom;
	
	--- insert into temp_adminid_newandold
	New_id_temp := country_idx*100000 + 10000;
	insert into temp_adminid_newandold(ID_old,New_ID,level,country_code)
	select order1_id as ID_old,
				 New_id_temp + row_number()over(order by order1_id) as New_ID,
				 1::smallint as level,
				 country_code_temp  as country_code
	from temp_admin_order1;
	
	return 0;
END;
$$;
--country
CREATE OR REPLACE FUNCTION mid_admin_add_order0_geom(country_idx integer,country_id double precision,
																							 country_code_temp character varying)
	RETURNS smallint
	LANGUAGE plpgsql
AS $$
DECLARE
	rec record;
	New_id_temp integer;
BEGIN
	---insert into order0
	insert into temp_admin_order0(order0_id,order0_code,country_code,the_geom)
	SELECT  country_id 				 as order0_id, 
	        country_id 				 as order0_id, 
	        country_code_temp  as country_code,
	        ST_Multi(st_union(b.the_geom)) as the_geom
	from 
	(
		select id
		from org_area
		where id = country_id
		group by id
	) as a
	left join
	temp_admin_order1 as b
	on a.id = b.order0_id::double precision;
		--- insert into temp_adminid_newandold
	New_id_temp := country_idx*100000 ;
	insert into temp_adminid_newandold(ID_old,New_ID,level,country_code)
	select order0_id as ID_old,
				 New_id_temp + row_number()over(order by order0_id) as New_ID,
				 0::smallint as level,
				 country_code_temp  as country_code
	from temp_admin_order0;
	return 0;
END;
$$;
--insert into mid_admin_zone
CREATE OR REPLACE FUNCTION mid_admin_insert_mid_admin_zone()
	RETURNS integer
	LANGUAGE plpgsql
AS $$
DECLARE
BEGIN
	insert into mid_admin_zone(ad_code,ad_order,order0_id,order1_id,order2_id,order8_id,ad_name,the_geom)
	select c.new_id 	 		as ad_code,
				 0::smallint 		as ad_order,
				 c.new_id	 			as order0_id,
				 null				 		as order1_id,
				 null				 		as order2_id,
				 null				 		as order8_id,
				 b.admin_name		as ad_name,
				 ST_Multi(a.the_geom) as the_geom
	from temp_admin_order0 as a
	left join
	temp_admin_name as b
	on a.order0_id = b.admin_id 
	left join
	(
		select id_old,new_id,country_code from temp_adminid_newandold where "level" = 0
	)as c
	on a.order0_id = c.id_old and a.country_code = c.country_code;
	
	
	insert into mid_admin_zone(ad_code,ad_order,order0_id,order1_id,order2_id,order8_id,ad_name,the_geom)
	select c.new_id 	 		as ad_code,
				 1::smallint 		as ad_order,
				 d.new_id	 			as order0_id,
				 c.new_id				as order1_id,
				 null				 		as order2_id,
				 null				 		as order8_id,
				 b.admin_name		as ad_name,
				 ST_Multi(a.the_geom) as the_geom
	from temp_admin_order1 as a
	left join
	temp_admin_name as b
	on a.order1_id = b.admin_id
	left join
	(
		select id_old,new_id,country_code from temp_adminid_newandold where "level" = 1
	)as c
	on a.order1_id = c.id_old and a.country_code = c.country_code
	left join
	(
		select id_old,new_id,country_code from temp_adminid_newandold where "level" = 0
	)as d
	on a.order0_id = d.id_old and a.country_code = d.country_code;
	
	insert into mid_admin_zone(ad_code,ad_order,order0_id,order1_id,order2_id,order8_id,ad_name,the_geom)
	select c.new_id 	 		as ad_code,
				 8::smallint 		as ad_order,
				 e.new_id	 			as order0_id,
				 d.new_id				as order1_id,
				 null				 		as order2_id,
				 c.new_id				as order8_id,
				 b.admin_name		as ad_name,
				 ST_Multi(a.the_geom) as the_geom
	from temp_admin_order8 as a
	left join
	temp_admin_name as b
	on a.order8_id = b.admin_id
	left join
	(
		select id_old,new_id,country_code from temp_adminid_newandold where "level" = 8
	)as c
	on a.order8_id = c.id_old and a.country_code = c.country_code
	left join
	(
		select id_old,new_id,country_code from temp_adminid_newandold where "level" = 1
	)as d
	on a.order1_id = d.id_old and a.country_code = d.country_code
	left join
	(
		select temp_1.order1_id as id_old,temp_0.new_id as new_id,temp_1.country_code as country_code
		from 
		temp_admin_order1 as temp_1
		left join
		(
			select id_old,new_id,country_code from temp_adminid_newandold where "level" = 0
		)as temp_0
		on temp_1.order0_id = temp_0.id_old and temp_1.country_code = temp_0.country_code
	)as e
	on a.order1_id = e.id_old and a.country_code = e.country_code;
	
	return 0;
END;
$$;

--------------------------------------------------------
-- link function
---------------------------------------------------------
CREATE OR REPLACE FUNCTION mmi_speed_class( spd_limit double precision, spd smallint ) 
  RETURNS  smallint
  LANGUAGE plpgsql
AS $$
DECLARE
     speed smallint;
BEGIN
       speed = spd;
       
	    RETURN CASE
	        WHEN speed > 130 THEN 1
	        WHEN speed > 100 and speed <= 130 THEN 2
	        WHEN speed > 90 and speed <= 100 THEN 3
	        WHEN speed > 70 and speed <= 90 THEN 4
	    	WHEN speed > 50 and speed <= 70 THEN 5
	    	WHEN speed > 30 and speed <= 50 THEN 6
	    	WHEN speed > 11 and speed <= 30 THEN 7
	    	ELSE 8
	    END;
END;
$$;

CREATE OR REPLACE FUNCTION mmi_road_type( ft smallint, privaterd smallint, frc smallint, fow smallint, 
                                          freeway smallint, ftr_cry character varying, routable character varying ) 
  RETURNS  smallint
  LANGUAGE plpgsql
AS $$
BEGIN
	    RETURN CASE
	        WHEN ft = 1 THEN 10
	        WHEN ft = 2 THEN 11
			WHEN fow in (14,15) THEN 8
			WHEN privaterd = 1 THEN 7
	    	WHEN routable = '0' THEN 14
	    	WHEN frc = 0 and freeway = 1 THEN 0
	    	---WHEN fow = 27 THEN 5
	    	WHEN frc in (0,1) or ftr_cry in('NHY','CTN','NGQ','CGQ') THEN 2
	    	WHEN frc = 2 or ftr_cry in('SHY','RDA') THEN 3
	    	WHEN frc = 3 or ftr_cry in('RDB','DHY') THEN 4
	    	ELSE 6
	    END; 
END;
$$;

CREATE OR REPLACE FUNCTION mmi_link_type( pj smallint, mult_digi double precision, fow smallint ) 
  RETURNS  smallint
  LANGUAGE plpgsql
AS $$
BEGIN
	    RETURN CASE
	        WHEN fow = 4  THEN 0
	        WHEN fow IN (10,21) THEN 5
			WHEN pj  = 1  THEN 4
			WHEN pj  = 2  THEN 8
	    	WHEN fow = 27 THEN 6
	    	WHEN mult_digi = 1 THEN 2
	    	ELSE 1
	    END; 
END;
$$;

CREATE OR REPLACE FUNCTION mmi_disp_class( ft smallint, freeway smallint, frc smallint, fow smallint,  ftr_cry character varying ) 
  RETURNS  smallint
  LANGUAGE plpgsql
AS $$
BEGIN
	    RETURN CASE
	        WHEN ft in (1,2)  THEN 14
	        WHEN fow in (14,15) THEN 3
			WHEN frc = 0 and freeway = 1 THEN 12
			WHEN frc in (0,1)  THEN 11
	    	WHEN frc = 2  THEN 7
	    	WHEN frc = 3  THEN 6
	    	WHEN frc = 4  THEN 5
	    	ELSE 4
	    END; 
END;
$$;

CREATE OR REPLACE FUNCTION mmi_function_class( frc smallint, freeway smallint, ftr_cry character varying) 
  RETURNS  smallint
  LANGUAGE plpgsql
AS $$
BEGIN
	    RETURN CASE
			WHEN frc = 0  THEN 1
			WHEN frc = 1  THEN 2
	    	WHEN frc = 2  THEN 3
	    	WHEN frc = 3  THEN 4
	    	ELSE 5
	    END; 
END;
$$;

CREATE OR REPLACE FUNCTION mmi_oneway( oneway character varying, constatus double precision, dir_f smallint, dir_p smallint[] )
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
    IF oneway = 'FT' THEN 
        ret = 1; 
    ELSE
        ret = 3 ;
    END IF;
	-- step 2
	IF 2 = any(dir_p) THEN
		ret = ret|1;
	END IF;
	IF 3 = any(dir_p) THEN
		ret = ret|2;
	END IF;

	-- step 3
	IF constatus = 1 THEN 
	   ret = 0;
    END IF;
    
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

CREATE OR REPLACE FUNCTION mmi_speed_limit( oneway smallint, spd_limit double precision, speed smallint, s2e boolean ) 
  RETURNS  smallint
  LANGUAGE plpgsql
AS $$
BEGIN
	IF (s2e = FALSE and oneway = 2) or (s2e = TRUE and oneway = 3) or oneway = 4 then
		return 0;
	END IF;
	
	IF spd_limit is not null and spd_limit > 0 THEN
		if spd_limit > 200 then
			return -1;
		else
			return spd_limit;
		end if;
	END IF;

    RETURN 
	CASE WHEN speed > 0 THEN speed
        ELSE 0 
    END;
END;
$$;

CREATE OR REPLACE FUNCTION mmi_speed_source( oneway smallint, spd_limit double precision, speed smallint, s2e boolean ) 
  RETURNS  smallint
  LANGUAGE plpgsql
AS $$
BEGIN
	IF (s2e = FALSE and oneway = 2) or (s2e = TRUE and oneway = 3) or oneway = 4 then
		return 0;
	END IF;
	
	IF spd_limit is not null and spd_limit > 0 THEN
		return 1;
	END IF;

    RETURN 
	CASE WHEN speed > 0 THEN 3 
        ELSE 0 
    END;
END;
$$;

-------------------------------------------------------------------------------------------------------------
-- Regulation function
-------------------------------------------------------------------------------------------------------------
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
	
	for rec in 
		select 	array_agg(gid) as dt_id_array, 
				type, excl_date, f_end, ref_date, exp_date, str_time, end_time
		from org_date_time
		group by type, excl_date, f_end, ref_date, exp_date, str_time, end_time
    LOOP
		-- car_type
    	car_type	:= -2;
    	
    	-- datetime
    	if rec.type = '1' then
    		-- DAY OF WEEK
    		start_year		:= 0;
			start_month		:= 0;
			start_day		:= 0;
			end_year		:= 0;
			end_month		:= 0;
			end_day			:= 0;			
			day_of_week		:= 0;
			for nIndex in 1..7 LOOP
				if substr(rec.ref_date, nIndex, 1) = 'Y' then
					day_of_week	:= day_of_week | (1 << (nIndex-1));
				end if;
			END LOOP;
			if day_of_week = 127 then
				day_of_week	:= 0;
			end if;
		elseif rec.type = 'A' then
			-- DATE RANGES
			start_year		:= cast(rec.ref_date as integer) / 10000;
			start_month		:= (cast(rec.ref_date as integer) % 10000) / 100;
			start_day		:= cast(rec.ref_date as integer) % 100;
			end_year		:= cast(rec.exp_date as integer) / 10000;
			end_month		:= (cast(rec.exp_date as integer) % 10000) / 100;
			end_day			:= cast(rec.exp_date as integer) % 100;			
			day_of_week		:= 0;		
		elseif rec.datetime_type = 'C' then
			-- DAY OF MONTH
			start_year		:= 0;
			start_month		:= 0;
			start_day		:= cast(substr(rec.ref_date, 1, 4) as integer);
			end_year		:= 0;
			end_month		:= 0;
			end_day			:= cast(substr(rec.exp_date, 1, 4) as integer);			
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
			day_of_week		:= day_of_week | (1 << (cast(substr(rec.ref_date, 1, 4) as integer) - 1));
			day_of_week		:= day_of_week | (2 << 7);
			day_of_week		:= day_of_week | (1 << (cast(substr(rec.ref_date, 5, 4) as integer) + 8));
		elseif rec.datetime_type = 'E' then
			-- DAY OF WEEK OF YEAR
			start_year		:= 0;
			start_month		:= 0;
			start_day		:= 0;
			end_year		:= 0;
			end_month		:= 0;
			end_day			:= 0;			
			day_of_week		:= 0;
			day_of_week		:= day_of_week | (1 << (cast(substr(rec.ref_date, 1, 4) as integer) - 1));
			day_of_week		:= day_of_week | (3 << 7);
			day_of_week		:= day_of_week | (cast(substr(rec.ref_date, 5, 4) as integer) << 9);
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
			day_of_week		:= day_of_week | (cast(substr(rec.ref_date, 1, 4) as integer) << 9);
		elseif rec.datetime_type = 'H' then
			-- MONTH OF YEAR
			start_year		:= 0;
			start_month		:= cast(substr(rec.ref_date, 1, 4) as integer) ;
			start_day		:= 0;
			end_year		:= 0;
			end_month		:= cast(substr(rec.exp_date, 1, 4) as integer) ;
			end_day			:= 0;
			day_of_week		:= 0;
		elseif rec.datetime_type = 'I' then
			-- DAY OF MONTH OF YEAR
			start_year		:= 0;
			start_month		:= cast(substr(rec.ref_date, 5, 4) as integer) ;
			start_day		:= cast(substr(rec.ref_date, 1, 4) as integer) ;
			end_year		:= 0;
			end_month		:= cast(substr(rec.exp_date, 5, 4) as integer) ;
			end_day			:= cast(substr(rec.exp_date, 1, 4) as integer) ;
			day_of_week		:= 0;
		else
			raise EXCEPTION 'error, new datetime type of regulation can not be handled!';
    	end if;
    	
    	if rec.str_time is null then
    		start_hour		:= 0;
    		start_minute	:= 0;
    	else
			start_hour		:= cast(rec.str_time as integer) / 100;
			start_minute	:= cast(rec.str_time as integer) % 100;
		end if;
		
		if rec.end_time is null then
    		end_hour		:= 23;
    		end_minute		:= 59;
    	else
			end_hour		:= cast(rec.end_time as integer) / 100;
			end_minute		:= cast(rec.end_time as integer) % 100;
		end if;
		
		-- update time format
    	if end_hour >= 24 then
    		end_hour		:= end_hour % 24;
    	end if;
    	if end_hour = 0 and end_minute = 0 then
    		end_hour		:= 23;
    		end_minute		:= 59;
    	end if;
		
		if rec.excl_date = 'Y' then
			exclude_date	:= 1;
		else
			exclude_date	:= 0;
		end if;
    	
    	if
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
	    	insert into temp_condition_regulation_tbl("cond_id", "dt_id")
	    	select null::integer, unnest(rec.dt_id_array);
		else
			-- insert new cond if condition is useful
	    	cur_cond_id := cur_cond_id + 1;
	    	
	    	insert into temp_condition_regulation_tbl("cond_id", "dt_id")
	    	select cur_cond_id, unnest(rec.dt_id_array);
			
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
    	select linkid, array_agg(linkdir) as array_linkdir, array_agg(cond_id) as array_condid
    	from
    	(
    		select 	a.edge_id as linkid, 
    				b.value::integer as linkdir, 
    				c.cond_id
    		from org_date_time as a
    		inner join org_condition_type as b
    		on a.cond_id = b.cond_id and b.type = '5'
    		left join temp_condition_regulation_tbl as c
    		on a.gid = c.dt_id
    		order by linkid, linkdir, cond_id
    	)as t
    	group by linkid
    	order by linkid
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
		while nIndex <= nCount LOOP
			if rec.array_linkdir[nIndex] in (1,3) then
	    		insert into regulation_relation_tbl
	    					("regulation_id", "nodeid", "inlinkid", "outlinkid", "condtype", "cond_id")
	    			VALUES 	(cur_regulation_id, null, rec.linkid, null, 2, rec.array_condid[nIndex]);
			end if;
			if rec.array_linkdir[nIndex] in (2,3) then
	    		insert into regulation_relation_tbl
	    					("regulation_id", "nodeid", "inlinkid", "outlinkid", "condtype", "cond_id")
	    			VALUES 	(cur_regulation_id, null, rec.linkid, null, 3, rec.array_condid[nIndex]);
	    	end if;
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
		select	link_array::bigint[] as linkid_array, junction::bigint as nodeid
		from
		(
    		select ARRAY[frmedgeid,toedgeid] as link_array, junction
    		from org_turn_table
    		
    		union
    		
    		select link_array, mid_get_connect_junction(link_array[1],link_array[2]) as junction
    		from
    		(
	    		select 	parent_id || array_agg(child_id) as link_array
	    		from
	    		(
		    		select *
		    		from org_complex_turn_restriction
		    		order by id, seq
	    		)as r
	    		group by id, parent_id
    		)as s
		)as t
		group by linkid_array, junction
		order by linkid_array, junction
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
	    			VALUES	(cur_regulation_id, null, rec.nodeid, nIndex+1);
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
    						 rec.nodeid, 
    						 rec.linkid_array[1], 
    						 rec.linkid_array[array_upper(rec.linkid_array,1)], 
    						 1, null::integer);
    
    END LOOP;
    return 1;
END;
$$;

-------------------------------------------------------------------------------------------------------------
-- calculate can pass link count
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mmi_get_canPassLink_count( linkid bigint, nodeid bigint ) 
  RETURNS  bigint
  LANGUAGE plpgsql
AS $$
DECLARE
        rowCnt bigint;
BEGIN
        rowCnt := 0;
	IF (linkid IS NULL) or (nodeid IS NULL) then
	    return 0;
	END IF;

        select count(*) into rowCnt
        from (
            select link_id
            from link_tbl
            where (nodeid = s_node and one_way_code in (1, 2)
                  or nodeid = e_node and one_way_code in (1, 3)) --out from node
                  and (link_id <> linkid) --out link do not in link
        ) as d;

    return rowCnt;
END;
$$;

-------------------------------------------------------------------------------------------------------------
-- when select link_type=4 from temp_natural_guidance_tbl once update execute
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mmi_one_sub_update_temp_natural_guidance_tbl( in_gid int, in_link bigint, in_node bigint, in_linkArray bigint[], in_flag boolean ) 
  RETURNS  boolean
  LANGUAGE plpgsql
AS $$
BEGIN
	IF (in_gid IS NULL) or (in_link IS NULL) or (in_node IS NULL) or (in_linkArray IS NULL) or (in_flag IS NULL) then
		return false;
	END IF;

        IF true = in_flag then
            insert into temp_natural_guidance_tbl (
                in_link_id, 
                in_s_node, 
                in_e_node, 
                in_link_type,
                in_one_way_code,
                in_node_id,
                out_node_id,
                out_link_id,
                out_s_node,
                out_e_node,
                out_link_type,
                out_one_way_code,
                poi_nme,
                poi_prop,
                link_array,
                node_array,
                the_geom
                ) 
            select 
                a.link_id as in_link_id,
                a.s_node as in_s_node,
                a.e_node as in_e_node,
                a.link_type as in_link_type,
                a.one_way_code as in_one_way_code,
                in_node as in_node_id,
                b.out_node_id,
                b.out_link_id,
                b.out_s_node,
                b.out_e_node,
                b.out_link_type,
                b.out_one_way_code,
                b.poi_nme,
                b.poi_prop,
                array_prepend(in_link, b.link_array) as link_array,
                array_prepend(in_node, b.node_array) as node_array,
                b.the_geom
            from (
                select *
                from link_tbl
                where link_id = ANY(in_linkArray)
            ) as a,
            (
                select *
                from temp_natural_guidance_tbl
                where gid = in_gid and
                      NOT (in_node = ANY(node_array))
            ) as b;
        ELSE
            insert into temp_natural_guidance_tbl (
                in_link_id, 
                in_s_node, 
                in_e_node, 
                in_link_type,
                in_one_way_code,
                in_node_id,
                out_node_id,
                out_link_id,
                out_s_node,
                out_e_node,
                out_link_type,
                out_one_way_code,
                poi_nme,
                poi_prop,
                link_array,
                node_array,
                the_geom
                ) 
            select
                b.in_link_id,
                b.in_s_node,
                b.in_e_node,
                b.in_link_type,
                b.in_one_way_code,
                b.in_node_id,
                in_node as out_node_id,
                a.link_id as out_link_id,
                a.s_node as out_s_node,
                a.e_node as out_e_node,
                a.link_type as out_link_type,
                a.one_way_code as out_one_way_code,
                b.poi_nme,
                b.poi_prop,
                array_append(b.link_array, in_link) as link_array,
                array_append(b.node_array, in_node) as node_array,
                b.the_geom
            from (
                select *
                from link_tbl
                where link_id = ANY(in_linkArray)
            ) as a, 
            (
                select *
                from temp_natural_guidance_tbl
                where gid = in_gid and
                      NOT (in_node = ANY(node_array))
            ) as b;
        END IF;

        delete from temp_natural_guidance_tbl where gid = in_gid;
        --delete from temp_natural_guidance_tbl where in_link_id = out_link_id;
        
    return true;
END;
$$;

-------------------------------------------------------------------------------------------------------------
-- calculate can pass link list
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mmi_get_canPassLink_list( in_link_id bigint, nodeid bigint, out_link_id bigint ) 
  RETURNS  bigint[]
  LANGUAGE plpgsql
AS $$
DECLARE
        rec record;
        rec_idx bigint;
        link_array bigint[];
BEGIN
        link_array = NULL;
        rec_idx := 1;
	IF nodeid IS NULL then
	    return link_array;
	END IF;
        
        IF in_link_id IS NOT NULL THEN
            FOR rec in (
            select *
            from (
                    select *
                    from link_tbl
                    where
                        (nodeid = s_node and one_way_code in (1, 3) or
                         nodeid = e_node and one_way_code in (1, 2)) and 
                        (link_id <> in_link_id)
                ) as d
            )  
            LOOP
                link_array[rec_idx] := rec.link_id;
                rec_idx := rec_idx + 1;
            END LOOP;
        ELSEIF out_link_id IS NOT NULL THEN
            FOR rec in (
            select *
            from (
                    select *
                    from link_tbl
                    where 
                        (nodeid = s_node and one_way_code in (1, 2) or
                         nodeid = e_node and one_way_code in (1, 3)) and
                        (link_id <> out_link_id)
                ) as d
            )  
            LOOP
                link_array[rec_idx] := rec.link_id;
                rec_idx := rec_idx + 1;
            END LOOP;
        END IF;

    return link_array;
END;
$$;

-------------------------------------------------------------------------------------------------------------
-- when select link_type=4 from temp_natural_guidance_tbl once update interface
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mmi_one_update_temp_natural_guidance_tbl( in_flag boolean ) 
  RETURNS  boolean
  LANGUAGE plpgsql
AS $$
DECLARE
        rec record;
BEGIN 
        IF in_flag IS NULL then
		return false;
	END IF;
	
        FOR rec in (
            select *
            from temp_natural_guidance_tbl
            where in_link_type = 4 or
                  out_link_type = 4
        )  
        LOOP
            
            IF 4 = rec.in_link_type AND true = in_flag THEN
                IF rec.in_node_id = rec.in_s_node THEN
                    perform mmi_one_sub_update_temp_natural_guidance_tbl(rec.gid, rec.in_link_id, rec.in_e_node, mmi_get_canPassLink_list(rec.in_link_id, rec.in_e_node, NULL), in_flag);
                ELSEIF rec.in_node_id = rec.in_e_node THEN
                    perform mmi_one_sub_update_temp_natural_guidance_tbl(rec.gid, rec.in_link_id, rec.in_s_node, mmi_get_canPassLink_list(rec.in_link_id, rec.in_s_node, NULL), in_flag);
                END IF;
            END IF;

            IF 4 = rec.out_link_type AND false = in_flag THEN
                IF rec.out_node_id = rec.out_s_node THEN
                    perform mmi_one_sub_update_temp_natural_guidance_tbl(rec.gid, rec.out_link_id, rec.out_e_node, mmi_get_canPassLink_list(NULL, rec.out_e_node, rec.out_link_id), in_flag);
                ELSEIF rec.out_node_id = rec.out_e_node THEN
                    perform mmi_one_sub_update_temp_natural_guidance_tbl(rec.gid, rec.out_link_id, rec.out_s_node, mmi_get_canPassLink_list(NULL, rec.out_s_node, rec.out_link_id), in_flag);
                END IF;
            END IF;
            
        END LOOP;

    return true;
END;
$$;

-------------------------------------------------------------------------------------------------------------
-- Get link count from temp_natural_guidance_tbl where link_type=4
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mmi_get_inner_link_count(  in_flag boolean ) 
  RETURNS  bigint
  LANGUAGE plpgsql
AS $$
DECLARE
        rowCount bigint;
BEGIN 
        rowCount := 0;
        IF in_flag IS NULL THEN
            return rowCount;
        END IF;

        IF true = in_flag THEN
            select count(*) into rowCount
            from temp_natural_guidance_tbl
            where in_link_type = 4;
        ELSEIF false = in_flag THEN
            select count(*) into rowCount
            from temp_natural_guidance_tbl
            where out_link_type = 4;

            --raise INFO 'rowCount = %', rowCount;
        END IF;

    return rowCount;
END;
$$;

-------------------------------------------------------------------------------------------------------------
-- when select link_type=4 from temp_natural_guidance_tbl once update interface
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mmi_update_temp_natural_guidance_tbl( ) 
  RETURNS  boolean
  LANGUAGE plpgsql
AS $$
BEGIN 
        --Initialization temp_natural_guidance_tbl
        delete from temp_natural_guidance_tbl;
        insert into temp_natural_guidance_tbl (
            in_link_id, 
            in_s_node, 
            in_e_node, 
            in_link_type,
            in_one_way_code,
            in_node_id,
            out_node_id,
            out_link_id,
            out_s_node,
            out_e_node,
            out_link_type,
            out_one_way_code,
            poi_nme,
            poi_prop,
            node_array,
            the_geom
            )
        select 
            a.in_link_id,
            a.in_s_node,
            a.in_e_node,
            a.in_link_type,
            a.in_one_way_code,
            a.node_id as in_node_id,
            a.node_id as out_node_id,
            a.out_link_id,
            a.out_s_node,
            a.out_e_node,
            a.out_link_type,
            a.out_one_way_code,
            a.poi_nme,
            a.poi_prop,
            array_append(a.node_array, a.node_id) as node_array,
            a.the_geom
        from 
            temp_natural_guidance_in_out_link_rel a;

        --in link calc route
        WHILE mmi_get_inner_link_count(true) > 0 LOOP
            PERFORM mmi_one_update_temp_natural_guidance_tbl(true);
        END LOOP;

        --out link calc route
        WHILE mmi_get_inner_link_count(false) > 0 LOOP
            PERFORM mmi_one_update_temp_natural_guidance_tbl(false);
        END LOOP;

    return true;
END;
$$;

-------------------------------------------------------------------------------------------------------------
-- delete array element
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mmi_array_delete( array_rec bigint[], id bigint ) 
  RETURNS  bigint[]
  LANGUAGE plpgsql
AS $$
DECLARE
        recIdx bigint;
        array_rec_t bigint[];
        rec_count bigint;
        rec_count_idx bigint;
BEGIN
        array_rec_t = NULL;
        recIdx := 1;
        
	IF (array_rec IS NULL) or (id IS NULL) then
		return array_rec_t;
	END IF;

	rec_count := array_upper(array_rec, 1);
        FOR rec_count_idx in 1..rec_count
        LOOP
            IF array_rec[rec_count_idx] <> id THEN
                array_rec_t[recIdx] := array_rec[rec_count_idx];
                recIdx := recIdx + 1;
            END IF;
        END LOOP;
        
    return array_rec_t;
END;
$$;

-------------------------------------------------------------------------------------------------------------
-- Gets the node list by node id
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mmi_get_inner_node_by_node_id( nodeid bigint ) 
  RETURNS  bigint[]
  LANGUAGE plpgsql
AS $$
DECLARE
        rec record;
        recIdx bigint;
        node_array bigint[];
        node_idx bigint;
        node_array_t bigint[];
        node_idx_t bigint;
BEGIN
        recIdx := 1;
        node_array := NULL;
        node_idx := 1;
        node_array_t := NULL;
        node_idx_t := 1;
	IF nodeid IS NULL then
	    return node_array;
	END IF;
	
        node_array[node_idx] := nodeid;
        node_idx := node_idx + 1;
        node_array_t[node_idx_t] := nodeid;
        node_idx_t := node_idx_t + 1;
        WHILE node_array_t IS NOT NULL LOOP
            
            FOR rec in (
                select *
                from link_tbl
                where 
                    node_array_t[1] in (s_node, e_node) and 
                    link_type = 4
            )
            LOOP
                IF NOT (rec.s_node = ANY(node_array)) THEN
                    node_array_t[node_idx_t] := rec.s_node;
                    node_idx_t := node_idx_t + 1;
                    
                    node_array[node_idx] := rec.s_node;
                    node_idx := node_idx + 1;
                END IF;

                IF NOT (rec.e_node = ANY(node_array)) THEN
                    node_array_t[node_idx_t] := rec.e_node;
                    node_idx_t := node_idx_t + 1;
                    
                    node_array[node_idx] := rec.e_node;
                    node_idx := node_idx + 1;
                END IF;

                
            END LOOP;

            node_array_t := mmi_array_delete( node_array_t, node_array_t[1] );
            node_idx_t := node_idx_t - 1;
        END LOOP;

    return node_array;
END;
$$;

-------------------------------------------------------------------------------------------------------------
-- Get preposition_code
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mmi_get_preposition_code( in_gid bigint ) 
  RETURNS  smallint
  LANGUAGE plpgsql
AS $$
DECLARE
        rec record;
        preposition_code smallint;
        rec_count bigint;
BEGIN
        preposition_code := 3; --default value(at)
        rec_count := 0;
	
	IF in_gid IS NULL THEN
	    return preposition_code;
	END IF;

	select a.in_link_id, a.the_geom as poi_geom, b.the_geom as node_geom into rec
	from temp_natural_guidance_tbl a
	inner join node_tbl b
	    on a.in_node_id = b.node_id
	where a.gid = in_gid;

	IF rec IS NOT NULL THEN
	    IF ST_Distance_Sphere(rec.poi_geom, rec.node_geom) < 10.0 THEN
	        --poi point and guide point within 10m
	        preposition_code := 3; --at
	    ELSE
	        --poi point and guide point between 10m and 50m
               select count(*) into rec_count
               from temp_natural_guidance_poi_link_tbl
               where poi_lid = rec.in_link_id;

               IF rec_count <> 0 THEN
                   preposition_code = 1; --after
               ELSE
                   preposition_code = 5; --before
               END IF;
	    END IF;
	END IF;
        
    return preposition_code;
END;
$$;

-------------------------------------------------------------------------------------------------------------
-- Get feat_position
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mmi_get_feat_position( in_gid int ) 
  RETURNS  smallint
  LANGUAGE plpgsql
AS $$
DECLARE
        rec record;
        link_array bigint[];
        link_idx bigint;
        rec_loop_idx bigint;
        node_id bigint;
        link_rec record;
        route_array geometry[];
        route_array_idx bigint;
        route_geometry geometry;
        poi_guide_geometry geometry;
        poi_guide_rec record;
        feat_position smallint;
BEGIN
        feat_position := 0;
        select * into rec
        from temp_natural_guidance_tbl
        where gid = in_gid;

        --raise INFO 'rec = %', rec;

        IF rec.gid IS NOT NULL THEN
            link_idx := 1;
            link_array[link_idx] := rec.in_link_id;
            link_idx := link_idx + 1;

            IF rec.link_array IS NOT NULL THEN
                FOR rec_loop_idx in 1..array_upper(rec.link_array, 1) LOOP
                    link_array[link_idx] := rec.link_array[rec_loop_idx];
                    link_idx := link_idx + 1;
                END LOOP;
            END IF;

            link_array[link_idx] := rec.out_link_id;
            link_idx := link_idx + 1;

            IF rec.in_node_id = rec.in_s_node THEN
                node_id := rec.in_e_node;
            ELSE
                node_id := rec.in_s_node;
            END IF;

            route_array_idx := 1;
            FOR rec_loop_idx in 1..array_upper(link_array, 1) LOOP
                select * into link_rec
                from link_tbl
                where link_id = link_array[rec_loop_idx];

                IF link_rec.link_id IS NOT NULL THEN
                    IF node_id = link_rec.s_node THEN
                        route_array[route_array_idx] := link_rec.the_geom;
                        node_id = link_rec.e_node;
                    ELSEIF node_id = link_rec.e_node THEN
                        route_array[route_array_idx] := ST_Reverse(link_rec.the_geom);
                        node_id = link_rec.s_node;
                    END IF;
                    route_array_idx := route_array_idx + 1;
                ELSE
                    return 0;
                END IF;
            END LOOP;
            
            route_geometry := ST_LineMerge(ST_Collect(route_array));

            select a.the_geom as poi_geom, b.the_geom as node_geom into poi_guide_rec
            from temp_natural_guidance_tbl a
            inner join node_tbl b
                on a.in_node_id = b.node_id
            where a.gid = in_gid;

            IF poi_guide_rec IS NOT NULL THEN
                poi_guide_geometry := ST_MakeLine(poi_guide_rec.node_geom, poi_guide_rec.poi_geom);
            END IF;
        END IF;
        
        IF route_geometry IS NOT NULL and
           poi_guide_geometry IS NOT NULL THEN
           feat_position := ST_LineCrossingDirection(route_geometry, poi_guide_geometry);
        END IF;

        RETURN CASE
            WHEN feat_position < 0 THEN 1 --left
            WHEN feat_position > 0 THEN 2 --right
            ELSE 3 --middle
        END;
END;
$$;