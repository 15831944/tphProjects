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
BEGIN
	country_count := 0;
	Country_array = mid_admin_judge_country_array();
	country_count := array_upper(Country_array,1);
	if (country_count % 2) = 1 then
		raise EXCEPTION 'country number is error, count is = %', country_count;
	end if;
	
	for country_count_idx in 1..country_count loop
		if (country_count_idx % 2) = 1 then
			if Country_array[country_count_idx] = 'India' then
				perform mid_admin_add_order8_geom(356,Country_array[country_count_idx + 1],'IND'::character varying);
				perform mid_admin_add_order1_geom(356,Country_array[country_count_idx + 1],'IND'::character varying);
				perform mid_admin_add_order0_geom(356,Country_array[country_count_idx + 1],'IND'::character varying);
			else
				raise EXCEPTION 'No found country name = %', Country_array[country_count_idx];
			end if;
		else
			continue;
		end if;
	end loop;
	return 0;
END;
$$;
--order8
CREATE OR REPLACE FUNCTION mid_admin_add_order8_geom(country_idx integer,country_id character varying,
																							 country_code_temp character varying)
	RETURNS integer
	LANGUAGE plpgsql
AS $$
DECLARE
	dirst_num integer;
	New_id_temp integer;
BEGIN
	---org_data
	insert into temp_admin_order8(order8_id,order1_id,country_code,the_geom)
  select  a.dst_id 						 				 as order8_id, 
	        a.parent_id 				 				 as order1_id, 
	        country_code_temp  					 as country_code,
	        ST_Multi(st_union(a.the_geom)) as the_geom
	from 	org_district_region as a
	join
	org_state_region as b
	on a.parent_id = b.stt_id
	where b.parent_id = country_id
	group by a.dst_id,a.parent_id
	order by a.dst_id,a.parent_id;
		
	---Difference from state	
	dirst_num := 900000001;
	insert into temp_admin_order8(order8_id,order1_id,country_code,the_geom)
  select  (dirst_num + (a.stt_id :: integer)) :: character varying			as order8_id, 
	        a.stt_id 			 		 as order1_id, 
	        country_code_temp  as country_code,
	        st_union(ST_Multi(b.the_geom)) as the_geom
	from org_state_region as a
	join
	temp_admin_state_difference_district as b
	on st_within(b.the_geom,a.the_geom)
	where a.parent_id = country_id
	group by a.stt_id
	order by a.stt_id;
	
	
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
CREATE OR REPLACE FUNCTION mid_admin_add_order1_geom(country_idx integer,country_id character varying,
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
	on a.stt_id = d.order1_id
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
CREATE OR REPLACE FUNCTION mid_admin_add_order0_geom(country_idx integer,country_id character varying,
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
	on a.id = b.order0_id;
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
	on a.order0_id = b.admin_id and a.country_code = b.country_code 
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
	on a.order1_id = b.admin_id and a.country_code = b.country_code
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
	on a.order8_id = b.admin_id and a.country_code = b.country_code
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
	    	WHEN routable = '0' THEN 7
	    	WHEN frc = 0 or freeway = 1 or ftr_cry in('NGQ','CGQ') THEN 0
	    	---WHEN fow = 27 THEN 5
	    	WHEN frc = 1 or ftr_cry in('NHY','CTN') THEN 2
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

CREATE OR REPLACE FUNCTION mmi_disp_class( ft smallint, frc smallint, fow smallint,  ftr_cry character varying ) 
  RETURNS  smallint
  LANGUAGE plpgsql
AS $$
BEGIN
	    RETURN CASE
	        WHEN ft in (1,2)  THEN 14
	        WHEN fow in (14,15) THEN 3
			WHEN frc = 0 or ftr_cry in('NGQ','CGQ')  THEN 12
			WHEN frc = 1  THEN 9
	    	WHEN frc = 2  THEN 8
	    	WHEN frc = 3  THEN 7
	    	WHEN frc = 4  THEN 6
	    	WHEN frc = 5  THEN 5
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
