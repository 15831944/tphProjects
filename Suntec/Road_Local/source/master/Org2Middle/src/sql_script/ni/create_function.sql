
-------------------------------------------------------------------------------------------------------------
-- update mid_temp_force_guide_tbl
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION ni_update_mid_temp_force_guide_tbl(  ) 
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

        FOR rec IN (
            select link_array, node_array
            from (
                select mapid, condid, array_agg(linkid) as link_array
                from (
                    select b.mapid, b.condid, seq_nm, linkid
                    from org_cond a
                    left join org_cnl b
                        on 
                            a.mapid = b.mapid and
                            a.condid = b.condid
                    where a.condtype::bigint = 2 and linkid <> ''
                    order by mapid, condid, seq_nm
                ) as c
                group by mapid, condid
            ) as d
            left join (
                select mapid, condid, array_agg(nodeid) as node_array
                from (
                    select b.mapid, b.condid, seq_nm, nodeid
                    from org_cond a
                    left join org_cnl b
                        on 
                            a.mapid = b.mapid and
                            a.condid = b.condid
                    where a.condtype::bigint = 2 and nodeid <> ''
                    order by mapid, condid, seq_nm
                ) as c
                group by mapid, condid
            ) as e
            on d.mapid = e.mapid and d.condid = e.condid
        )
        LOOP
            link_count := array_upper(rec.link_array, 1);
            node_count := array_upper(rec.node_array, 1);

            IF (link_count >= 2) AND (node_count >= 1 AND rec.node_array[1] IS NOT NULL) THEN
                insert into mid_temp_force_guide_tbl (nodeid, inlinkid, outlinkid, passlid, passlink_cnt)
                values(rec.node_array[1]::bigint, rec.link_array[1]::bigint, rec.link_array[link_count]::bigint, array_to_string(rec.link_array[2:link_count-1], '|'), link_count-2);
            ELSE 
                raise INFO 'rec = %', rec;
            END IF;
        END LOOP;

        return 0;
END;
$$;

CREATE OR REPLACE FUNCTION mid_get_line_turn(line geometry)
  RETURNS smallint 
  LANGUAGE plpgsql
  AS $$ 
DECLARE
	num        smallint;
	angle_turn float;
	angle_tmp  float;
	angle_1    float;
	angle_2    float;
	i   	   smallint;
BEGIN
	num=st_npoints(line);
	angle_turn=0;
	angle_tmp=0;
	angle_1=0;
	angle_2=0;
	for i in 1..num-2 loop
		
		angle_1=st_azimuth(st_pointn(line,i),st_pointn(line,i+1))/pi()*180.0;
		angle_2=st_azimuth(st_pointn(line,i+1),st_pointn(line,i+2))/pi()*180.0;
		--raise info '%,%',angle_1,angle_2;
		angle_tmp=angle_2-angle_1;
		if angle_tmp<-180 then
			angle_tmp=angle_tmp+360;
		elseif angle_tmp>180 then
			angle_tmp=angle_tmp-360;
		end if;
		angle_turn=angle_turn+angle_tmp;
	end loop;
	return angle_turn::smallint;	
END;
$$;

CREATE OR REPLACE FUNCTION mid_make_arrowinfo7_from_link_ni(inlinkid varchar,nodeid varchar,passlid varchar,outlinkid varchar)
  RETURNS smallint 
  LANGUAGE plpgsql
  AS $$ 
DECLARE

	rec record;
	line_arr character varying[];
	line_geom_arr geometry[];
	line_merge    geometry;
	num int;
	p0 geometry;
	p1 geometry;
	angle_turn smallint;

BEGIN
	select snodeid,enodeid,the_geom into rec from org_r where id = inlinkid;
	if rec.snodeid = nodeid then
		p1 := st_startpoint(rec.the_geom);
		p0 := st_pointn(rec.the_geom,2);				
	else 
		if rec.enodeid = nodeid then			
			num := st_npoints(rec.the_geom);
			p1 := st_endpoint(rec.the_geom);
			p0 := st_pointn(rec.the_geom,num - 1);
		end if;
	end if;

	if passlid='' or passlid is null then
		line_arr=array[outlinkid];
	else 
		line_arr=regexp_split_to_array(passlid, E'\\|+');
		line_arr=array_append(line_arr,outlinkid);
	end if;
	
	select array_agg(the_geom) into line_geom_arr from org_r where id = any(line_arr);

	line_geom_arr=array_prepend(st_makeline(p0,p1),line_geom_arr);

		
	line_merge=st_linemerge(st_collect(line_geom_arr));
	
	
	angle_turn=mid_get_line_turn(line_merge);
	if not st_equals(st_startpoint(line_merge),p0) then
		angle_turn=-angle_turn;
	end if;
	--raise info '%',st_astext(line_merge);
	--return angle_turn;
	if angle_turn<45 then
		return 16;

	end if;
	return 2048;
END;
$$;

CREATE OR REPLACE FUNCTION mid_make_arrowinfo_ni(arrow character varying)
  RETURNS smallint 
  LANGUAGE plpgsql
  AS $$ 
BEGIN
        if arrow = '1' then
            return 1;
        elseif arrow = '2' then
            return 2;
        elseif arrow = '3' then
            return 2;
        elseif arrow = '4' then
            return 4;
        elseif arrow = '5' then
            return 8;
        elseif arrow = '6' then
            return 8;
        elseif arrow = '8' then
            return 32;
        elseif arrow = '9' then
            return 32;
        elseif arrow = 'a' then
            return 64;
        elseif arrow = 'b' then
            return 128;
        elseif arrow = 'c' then
            return 128;
        end if;

END;
$$;

CREATE OR REPLACE FUNCTION mid_get_passlinkcount_ni(passlid character varying)
  RETURNS smallint 
  LANGUAGE plpgsql
  AS $$ 
BEGIN
	if passlid='' then
		return 0;
	end if;
    return length(passlid)-length(replace(passlid,'|',''))+1;

END;
$$;

-------------------------------------------------------------------------------------------------------------
-- link
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION ni_cnv_disp_class( kind varchar, vehcl_type varchar )
  RETURNS smallint
  LANGUAGE plpgsql VOLATILE
  AS $$
BEGIN

	return case
		when kind like '0a%' or kind like '%|0a%' then 14
		when substr(vehcl_type, 1, 1) = '0' 
			and substr(vehcl_type, 2, 1) = '0'
			and substr(vehcl_type, 3, 1) = '0'
			and substr(vehcl_type, 9, 1) = '0'
			and substr(vehcl_type, 14, 1) = '0'
			and substr(vehcl_type, 10, 1) = '1' then 20
		when kind like '0b%' or kind like '%|0b%' then 3 
		when kind like '00%' or kind like '%|00%' then 12 		
		when kind like '01%' or kind like '%|01%' then 11 
		when kind like '02%' or kind like '%|02%' then 9 
		when kind like '03%' or kind like '%|03%' then 8 
		when kind like '04%' or kind like '%|04%' then 7 
		when kind like '06%' or kind like '%|06%' then 6 
		when kind like '08%' or kind like '%|08%' then 5 
		when kind like '09%' or kind like '%|09%' then 18 
		else 4
	end;

END;
$$;

CREATE OR REPLACE FUNCTION ni_cnv_link_type( kind varchar)
  RETURNS smallint
  LANGUAGE plpgsql VOLATILE
  AS $$
BEGIN

	return case
		when kind like '%00' or kind like '%00|%' then 0
		when kind like '%03' or kind like '%03|%' then 3 
		when kind like '%05' or kind like '%05|%'
		     or kind like '%0b' or kind like '%0b|%' then 5 		
		when kind like '%04' or kind like '%04|%' then 4 
		when kind like '%12' or kind like '%12|%' then 8 
		when kind like '%15' or kind like '%15|%' then 9 
		when kind like '%07' or kind like '%07|%' then 7 
		when kind like '%0a' or kind like '%0a|%' then 6 
		when kind like '%02' or kind like '%02|%' then 2  
		else 1
	end;

END;
$$;

CREATE OR REPLACE FUNCTION ni_cnv_road_type( kind varchar, through varchar, unthrucrid varchar, 
	vehcl_type varchar, ownership varchar )
  RETURNS smallint
  LANGUAGE plpgsql VOLATILE
  AS $$
BEGIN
	return case
		when kind like '0a%' or kind like '%|0a%' then 10
		when ownership = '1' then 7
		when kind like '0b%' or kind like '%|0b%' then 8		
		when substr(vehcl_type, 1, 1) = '0' 
			and substr(vehcl_type, 2, 1) = '0'
			and substr(vehcl_type, 3, 1) = '0'
			and substr(vehcl_type, 14, 1) = '0'
			and (
				substr(vehcl_type, 8, 1) = '1'
				or substr(vehcl_type, 9, 1) = '1'
				or substr(vehcl_type, 10, 1) = '1'
			) then 12
		when through = '0' and unthrucrid = '' then 14
		when substr(vehcl_type, 14, 1) = '1' then 13 
		when kind like '00%' or kind like '%|00%' then 0 		
		when kind like '01%' or kind like '%|01%' then 1 
		when kind like '02%' or kind like '%|02%' then 2 
		when kind like '03%' or kind like '%|03%' then 3 
		when kind like '04%' or kind like '%|04%' then 4 
		when kind like '06%' or kind like '%|06%' 
			or kind like '08%' or kind like '%|08%' then 6 
		when kind like '09%' or kind like '%|09%' then 9 
		else 6
	end;

END;
$$;

CREATE OR REPLACE FUNCTION ni_cnv_speedlimit( oneway smallint, spdlmt varchar, speedclass varchar, dir boolean )
  RETURNS smallint
  LANGUAGE plpgsql VOLATILE
  AS $$
BEGIN

	IF (oneway = 2 AND dir = false) 
		OR (oneway = 3 AND dir = true)
		OR oneway = 4 THEN
		RETURN 0;
	END IF;
 
	IF spdlmt != '' THEN 
		RETURN ROUND(spdlmt::int / 10);
	ELSE
		RETURN CASE
			WHEN speedclass='1' THEN 160
			WHEN speedclass='2' THEN 130
			WHEN speedclass='3' THEN 100
			WHEN speedclass='4' THEN 90
			WHEN speedclass='5' THEN 70
			WHEN speedclass='6' THEN 50
			WHEN speedclass='7' THEN 30
			WHEN speedclass='8' THEN 11
			ELSE 0
		END;
	END IF;

END;
$$;

CREATE OR REPLACE FUNCTION ni_cnv_speed_source( oneway smallint, spdsrc varchar, dir boolean )
  RETURNS smallint
  LANGUAGE plpgsql VOLATILE
  AS $$
BEGIN

	IF (oneway = 2 AND dir = false) 
		OR (oneway = 3 AND dir = true)
		OR oneway = 4 THEN
		RETURN 0;
	END IF;
	
	IF spdsrc != '' THEN 
		RETURN spdsrc::smallint;
	ELSE
		RETURN 0;
	END IF;
END;
$$;

CREATE OR REPLACE FUNCTION ni_cnv_lane( oneway smallint, lanenums2e varchar, lanenume2s varchar, dir boolean)
  RETURNS smallint
  LANGUAGE plpgsql VOLATILE
  AS $$
BEGIN

	IF (oneway = 2 AND dir = false) 
		OR (oneway = 3 AND dir = true) THEN
		RETURN 0;
	END IF;
	
	IF lanenums2e != '' and lanenume2s != '' THEN 
		RETURN (lanenums2e::smallint + lanenume2s::smallint);
	ELSIF lanenums2e != '' and lanenume2s = '' THEN
		RETURN lanenums2e::smallint;
	ELSIF lanenume2s != '' and lanenums2e = '' THEN
		RETURN lanenume2s::smallint;
	ELSE 
		RETURN 0;
	END IF;
END;
$$;

CREATE OR REPLACE FUNCTION ni_cnv_width( oneway smallint, width varchar, dir boolean)
  RETURNS smallint
  LANGUAGE plpgsql VOLATILE
  AS $$
BEGIN

	IF (oneway = 2 AND dir = false) 
		OR (oneway = 3 AND dir = true) THEN
		RETURN 0;
	END IF;
	
	IF width != '' THEN 
		RETURN CASE
			WHEN width = '15' THEN 0
			WHEN width = '30' THEN 1
			WHEN width = '55' THEN 2
			WHEN width = '130' THEN 3
			ELSE 4
		END;
	ELSE 
		RETURN 4;
	END IF;
END;
$$;

CREATE OR REPLACE FUNCTION mid_make_passlinkid( inlinkid varchar, passlid varchar, passlid2 varchar , outlinkid varchar)
  RETURNS varchar
  LANGUAGE plpgsql VOLATILE
  AS $$
DECLARE
	passlid_arr     varchar[];
	passlid_arr_new varchar[];
	num             int;
	linkid          varchar;
	i               int;
BEGIN
	if passlid2<>'' then
		passlid=passlid||'|'||passlid2;
	end if;
	if substr(passlid,1,length(inlinkid))=inlinkid::text then
		passlid=substr(passlid,length(inlinkid)+2)::varchar;
	end if;
	if substr(passlid,length(passlid)-length(outlinkid)+1)=outlinkid::text then
		passlid=substr(passlid,1,length(passlid)-length(outlinkid)-1);
	end if;
	passlid_arr=regexp_split_to_array(passlid,E'\\|+');
	num=array_upper(passlid_arr,1);
	linkid=passlid_arr[1];
	passlid_arr_new=array_append(passlid_arr_new,linkid);
	for i in 2..num loop
		if linkid<>passlid_arr[i] then
			passlid_arr_new=array_append(passlid_arr_new,passlid_arr[i]);
			linkid=passlid_arr[i];
		end if;
	end loop;
	return array_to_string(passlid_arr_new,'|');
END;
$$;

-------------------------------------------------------------------------------------------------------------
-- regulation
-------------------------------------------------------------------------------------------------------------


CREATE OR REPLACE FUNCTION mid_get_access(vehicle_type varchar)
	RETURNS smallint
    LANGUAGE plpgsql
AS $$
DECLARE
	car_type		smallint;
BEGIN
	car_type = 0;
	
	-- all car type
	if vehicle_type is null or vehicle_type = '' then
		car_type 	= car_type | (1<<10) | (1<<13);
	end if;
	
	-- passenger car
	if substr(vehicle_type, 1, 1) = '1' then
		car_type	= car_type | (1<<13);
	end if;
	
	-- deliveries
	if substr(vehicle_type, 2, 1) = '1' then
		car_type	= car_type | (1<<13);
	end if;
	
	-- local car
	if substr(vehicle_type, 12, 1) = '1' then
		car_type	= car_type | (1<<13);
	end if;
	
	-- self-own car
	if substr(vehicle_type, 13, 1) = '1' then
		car_type	= car_type | (1<<13);
	end if;
	
	-- carpools
	if substr(vehicle_type, 14, 1) = '1' then
		car_type	= car_type | (1<<10);
	end if;
	
	return car_type;
END;
$$;

CREATE OR REPLACE FUNCTION mid_convert_condition_regulation_tbl()
    RETURNS smallint
    LANGUAGE plpgsql
AS $$
DECLARE
	rec				record;
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
	
	nCount			integer;
	nIndex			integer;
	nTargetPos		integer;
	nTempPos		integer;
	strChar			varchar;
	strNumber		integer;
	
	strTime			varchar;
	strStartTime	varchar;
	strEndTime		varchar;
	strWeek			varchar;
BEGIN
	cur_cond_id := 0;
    all_car_type	:= (1<<13) | (1<<10);
	
	for rec in 
		select distinct vperiod, vehcl_type
		from org_cr
        order by vperiod, vehcl_type
    LOOP
    	-- car_type
    	car_type	:= mid_get_access(rec.vehcl_type);
    	
		-- set default value
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
		
		-- for example, "[(h15)(h17)]*(t2t3t4t5t6)"
		if rec.vperiod is null or rec.vperiod = '' then
			-- all time
		else
			-- split time and week
			if strpos(rec.vperiod, '*') > 0 then
				strTime		= substr(rec.vperiod, 1, strpos(rec.vperiod, '*') - 1);
				strWeek		= substr(rec.vperiod, strpos(rec.vperiod, '*') + 1);
			else
				strTime		= rec.vperiod;
				strWeek		= null;
			end if;
			strStartTime	= substr(strTime, 
									 strpos(strTime, '[') + 1, 
									 strpos(strTime, ')(') - strpos(strTime, '['));
			strEndTime		= substr(strTime, 
									 strpos(strTime, ')(') + 1, 
									 strpos(strTime, ']') - strpos(strTime, ')(') - 1);
			--raise EXCEPTION 'strStartTime = %, strEndTime = %, strWeek = %', strStartTime, strEndTime, strWeek;
			
			-- start time
			if strStartTime is not null then
				nTargetPos	:= 2;
				nTempPos	:= 3;
				while nTempPos <= length(strStartTime) loop
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
						else
							raise EXCEPTION 'unhandled vperiod type1, vperiod = %', rec.vperiod;
						end if;
						nTargetPos	:= nTempPos;
					end if;
					nTempPos	:= nTempPos + 1;
		    	end loop;
	    	end if;
			
			-- end time
			if strEndTime is not null then
				nTargetPos	:= 2;
				nTempPos	:= 3;
				while nTempPos <= length(strEndTime) loop
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
							end_minute		:= 0;
						elseif substring(strEndTime, nTargetPos, 1) = 'm' then
							end_minute		:= cast(substring(strEndTime, nTargetPos+1, nTempPos-nTargetPos-1) as integer);
						else
							raise EXCEPTION 'unhandled vperiod type1, vperiod = %', rec.vperiod;
						end if;
						nTargetPos	:= nTempPos;
					end if;
					nTempPos	:= nTempPos + 1;
		    	end loop;
	    	end if;
			
			-- week
			if strWeek is not null then
				nTargetPos	:= 2;
				nTempPos	:= 3;
				while nTempPos <= length(strWeek) loop
					strChar	:= substring(strWeek, nTempPos, 1);
					if not ((strChar >= '1') and (strChar <= '7')) then
						if substring(strWeek, nTargetPos, 1) = 't' then
							strNumber		:= substring(strWeek, nTargetPos+1, nTempPos-nTargetPos-1);
							day_of_week		:= day_of_week | (1 << (strNumber::integer - 1));
						else
							raise EXCEPTION 'unhandled vperiod type2, vperiod = %', rec.vperiod;
						end if;
						nTargetPos	:= nTempPos;
					end if;
					nTempPos	:= nTempPos + 1;
		    	end loop;
			end if;
		end if;
    	
    	-- set cond_id -1 if no car restriction
    	if car_type = 0 then
	    	insert into temp_condition_regulation_tbl
	    				(
	    				"cond_id", "vperiod", "vehcl_type"
						)
	    		VALUES	(
	    				-1, rec.vperiod, rec.vehcl_type
	    				);
		-- insert new cond if condition is useful
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
		-- condition always be effect, set cond_id = null
		then
	    	insert into temp_condition_regulation_tbl
	    				(
	    				"cond_id", "vperiod", "vehcl_type"
						)
	    		VALUES	(
	    				null::integer, rec.vperiod, rec.vehcl_type
	    				);
		-- insert new cond if condition is useful
		else
	    	cur_cond_id := cur_cond_id + 1;
	    	
	    	insert into temp_condition_regulation_tbl
	    				(
	    				"cond_id", "vperiod", "vehcl_type"
						)
	    		VALUES	(
	    				cur_cond_id, rec.vperiod, rec.vehcl_type
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
	    end if;
	end loop;

    return 1;
END;
$$;

CREATE OR REPLACE FUNCTION mid_convert_regulation_access()
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
    	select 	link_id, 
    			array_agg(cond_id) as array_condid,
    			array_agg(seasonal_flag) as array_seasonal
    	from
    	(
	    	select 	a.id::bigint as link_id, c.cond_id, 
	    			(case when b.vp_approx = '1' then True else False end) as seasonal_flag
	    	from org_r as a
	    	left join org_cr as b
	    	on a.accesscrid = b.crid
	    	left join temp_condition_regulation_tbl as c
	    	on b.vperiod = c.vperiod and b.vehcl_type = c.vehcl_type
	    	where b.gid is not null and c.cond_id is distinct from -1
	    	order by a.id, c.cond_id
    	)as t
    	group by link_id
    	order by link_id
    LOOP
		-- current regulation id
    	cur_regulation_id := cur_regulation_id + 1;
    	
    	-- insert into regulation_item_tbl
    	insert into regulation_item_tbl("regulation_id", "linkid", "nodeid", "seq_num")
    							VALUES (cur_regulation_id, rec.link_id, null, 1);
		
    	-- insert into regulation_relation_tbl
		nCount := array_upper(rec.array_condid, 1);
		nIndex := 1;
		while nIndex <= nCount LOOP
    		insert into regulation_relation_tbl
    					("regulation_id", "nodeid", "inlinkid", "outlinkid", "condtype", "cond_id", "is_seasonal")
    			VALUES	(cur_regulation_id, null, rec.link_id, null, 4, rec.array_condid[nIndex], rec.array_seasonal[nIndex]);
    		nIndex := nIndex + 1;
    	END LOOP;
    END LOOP;
    return 1;
END;
$$;

CREATE OR REPLACE FUNCTION mid_convert_regulation_oneway()
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
    	select 	link_id, 
    			array_agg(vpdir) array_linkdir, 
    			array_agg(cond_id) as array_condid,
    			array_agg(seasonal_flag) as array_seasonal
    	from
    	(
	    	select 	a.id::bigint as link_id, b.vpdir, c.cond_id, 
	    			(case when b.vp_approx = '1' then True else False end) as seasonal_flag
	    	from org_r as a
	    	left join org_cr as b
	    	on a.onewaycrid = b.crid
	    	left join temp_condition_regulation_tbl as c
	    	on b.vperiod = c.vperiod and b.vehcl_type = c.vehcl_type
	    	where b.gid is not null and c.cond_id is distinct from -1
	    	order by a.id, b.vpdir, c.cond_id
    	)as t
    	group by link_id
    	order by link_id
    LOOP
		-- current regulation id
    	cur_regulation_id := cur_regulation_id + 1;
    	
    	-- insert into regulation_item_tbl
    	insert into regulation_item_tbl
    				("regulation_id", "linkid", "nodeid", "seq_num")
    		VALUES 	(cur_regulation_id, rec.link_id, null, 1);
		
		-- insert into regulation_relation_tbl
		nCount := array_upper(rec.array_condid, 1);
		nIndex := 1;
		while nIndex <= nCount LOOP
			if rec.array_linkdir[nIndex] = '2' then
	    		insert into regulation_relation_tbl
	    					("regulation_id", "nodeid", "inlinkid", "outlinkid", "condtype", "cond_id")
	    			VALUES 	(cur_regulation_id, null, rec.link_id, null, 43::smallint, rec.array_condid[nIndex]);
    		elseif rec.array_linkdir[nIndex] = '3' then
	    		insert into regulation_relation_tbl
	    					("regulation_id", "nodeid", "inlinkid", "outlinkid", "condtype", "cond_id")
	    			VALUES 	(cur_regulation_id, null, rec.link_id, null, 42::smallint, rec.array_condid[nIndex]);
	    	else
	    		raise EXCEPTION 'oneway vpdir error, link_id = %', rec.link_id;
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
	
	linkid_array bigint[];
	nLinkid integer;
	nConnectedLinkid integer;
	nTargetLinkid bigint;
	
	nodeid_array bigint[];
	nTileLinkid bigint;
	nTileConnectedLinkid bigint;
	nTargetNodeid bigint;
BEGIN
	-- regulation_id
	select (case when max(regulation_id) is null then 0 else max(regulation_id) end)
	from regulation_relation_tbl
	into cur_regulation_id;
	
    FOR rec IN
    	select	mapid, condid, link_array, node_array,
    			array_agg(cond_id) as array_condid,
    			array_agg(seasonal_flag) as array_seasonal
    	from
    	(
			select 	a.mapid, a.condid, d.link_array, d.node_array, 
					(case when b.vp_approx = '1' then True else False end) as seasonal_flag,
					c.cond_id
			from org_cond as a
	    	left join org_cr as b
	    	on a.crid = b.crid
	    	left join temp_condition_regulation_tbl as c
	    	on b.vperiod = c.vperiod and b.vehcl_type = c.vehcl_type
	    	left join
	    	(
	    		select 	mapid, condid,
	    				array_agg(linkid) as link_array,
	    				array_agg(nodeid) as node_array
	    		from
	    		(
		    		select 	mapid, condid, 
		    				(case when linkid = '' then null::bigint else linkid::bigint end) as linkid, 
		    				(case when nodeid = '' then null::bigint else nodeid::bigint end) as nodeid
		    		from org_cnl
		    		order by mapid, condid, seq_nm::integer
	    		)as t
	    		group by mapid, condid
	    	)as d
	    	on a.mapid = d.mapid and a.condid = d.condid
	    	where 	a.condtype = '1'
	    			and
	    			--b.gid is not null 
	    			--and 
	    			c.cond_id is distinct from -1
	    			and
	    			d.mapid is not null
	    	order by a.mapid, a.condid, d.link_array, d.node_array, c.cond_id
    	)as t
    	group by mapid, condid, link_array, node_array
    	order by mapid, condid, link_array, node_array
    LOOP
		-- current regulation id
    	cur_regulation_id := cur_regulation_id + 1;
		
    	-- insert into regulation_item_tbl
		nCount := array_upper(rec.link_array, 1);
		nIndex := 1;
		while nIndex <= nCount LOOP
    		insert into regulation_item_tbl
    					("regulation_id", "linkid", "nodeid", "seq_num")
    			VALUES	(cur_regulation_id, rec.link_array[nIndex], rec.node_array[nIndex], nIndex);
			nIndex := nIndex + 1;
		END LOOP;
		
	   	-- insert into regulation_relation_tbl
		nCount := array_upper(rec.array_condid, 1);
		nIndex := 1;
		while nIndex <= nCount LOOP
	    	insert into regulation_relation_tbl
	    						("regulation_id", "nodeid", "inlinkid", "outlinkid", "condtype", "cond_id")
	    				VALUES	(cur_regulation_id, 
	    						 rec.node_array[2], rec.link_array[1], rec.link_array[array_upper(rec.link_array,1)], 
	    						 1, rec.array_condid[nIndex]);
			nIndex := nIndex + 1;
		END LOOP;    	
    	
    END LOOP;
    return 1;
END;
$$;

CREATE OR REPLACE FUNCTION mid_cnv_shield_ni(route_kind character varying, name character varying)
  RETURNS character varying 
  LANGUAGE plpgsql
  AS $$ 
  DECLARE
	shield character varying;
BEGIN
	
 	IF route_kind = '1' then shield := '5100';
	ELSIF route_kind = '2' then shield := '5101';
	ELSIF route_kind = '3' then shield := '5102'; 		
	ELSIF route_kind = '4' then shield := '5103'; 
	ELSIF route_kind = '5' then shield := '5104';
	ELSIF route_kind = '6' then shield := '5105'; 
	ELSIF route_kind = '7' then shield := '5106';  
	ELSE shield := '';
	END IF;

	RETURN (shield || chr(9) || name);
END;
$$;


-- Convert between DBC case & SBC case.
-- text: convert string  
-- integer:1-SBC case  Others-DBC case  
CREATE OR REPLACE FUNCTION mid_convertstring(text, integer)  
  RETURNS character varying 
  LANGUAGE plpgsql
  AS $$ 
  DECLARE len INTEGER;  
  DECLARE retval text;  
  DECLARE tmp text;  
  DECLARE asciival INT4;  
    
BEGIN  
    --string length  
    SELECT textlen($1) INTO len;  
      
    SELECT '' INTO retval;  
      
    -- loop 
    FOR i IN 1..len LOOP  
      -- get single string  
      SELECT substring($1, i, 1) INTO tmp;  
      SELECT ascii(tmp) INTO asciival;  
        
      IF ($2 = 1) THEN  
        BEGIN  
          --SBC case to  DBC case
          IF (asciival = 12288) THEN  --space  
            asciival = 32;  
          END IF;  
          IF (asciival > 65280) AND (asciival < 65375) THEN  
            SELECT asciival-65248 INTO asciival;  
          END IF;  
        END;  
      ELSE  
        BEGIN  
          --DBC case to  SBC case 
          IF (asciival = 32) THEN --space  
            asciival = 12288;  
          END IF;  
          IF (asciival < 127) THEN  
            SELECT asciival+65248 INTO asciival;  
          END IF;  
        END;  
      END IF;  
        
      SELECT chr(asciival) INTO tmp;  
        
      SELECT textcat(retval, tmp) INTO retval;  
    END LOOP;  
      
    RETURN retval;  
  END;  
  $$;  
