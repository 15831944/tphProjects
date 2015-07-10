
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

CREATE OR REPLACE FUNCTION mid_get_passlinkcount_ni(passlid character varying,passlid2 character varying)
  RETURNS smallint 
  LANGUAGE plpgsql
  AS $$ 
BEGIN
        return length(passlid)+length(passlid2)-length(replace(passlid,'|',''))-length(replace(passlid2,'|',''));

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
		OR oneway = 0 THEN
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
		OR oneway = 0 THEN
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
		RETURN width::smallint;
	ELSE 
		RETURN 0;
	END IF;
END;
$$;


