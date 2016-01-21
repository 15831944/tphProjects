CREATE OR REPLACE FUNCTION mid_get_day_of_week(day varchar)
	RETURNS integer
    LANGUAGE plpgsql
AS $$
DECLARE
	day_of_week      integer;
BEGIN
	day_of_week := 0;

	if char_length(day) != 9 then
		raise EXCEPTION 'day = %', day;
	end if;
	
	if day = '011111110' then
		-- if day from Mon to Sun always effect, then set 0
		return day_of_week;
	end if;
	
	if substr(day, 1, 1) = '0' then
		-- not Everyday
		-- Mon
		if substr(day, 2, 1) = '1' then
			day_of_week := day_of_week | (1 << 1);
		end if;

		-- Tue
		if substr(day, 3, 1) = '1' then
			day_of_week := day_of_week | (1 << 2);
		end if;

		-- Wed
		if substr(day, 4, 1) = '1' then
			day_of_week := day_of_week | (1 << 3);
		end if;

		-- Thu
		if substr(day, 5, 1) = '1' then
			day_of_week := day_of_week | (1 << 4);
		end if;

		-- Fri
		if substr(day, 6, 1) = '1' then
			day_of_week := day_of_week | (1 << 5);
		end if;

		-- Sat
		if substr(day, 7, 1) = '1' then
			day_of_week := day_of_week | (1 << 6);
		end if;

		-- Sun
		if substr(day, 8, 1) = '1' then
			day_of_week := day_of_week | 1;
		end if;

		-- zenrin twn do not consider holiday regulation
		-- Holiday
		--if substr(day, 9, 1) = '1' then
			--day_of_week := day_of_week | (1 << 7);
			--day_of_week := day_of_week | (1 << 9);
		--end if;
		
		--if day = '000000001' then
			-- only Holiday, Mon/Tue/Wed/Thu/Fri/Sat/Sun must set 1
			--day_of_week := day_of_week | (1 << 1); -- Mon
			--day_of_week := day_of_week | (1 << 2); -- Tue
			--day_of_week := day_of_week | (1 << 3); -- Wed
			--day_of_week := day_of_week | (1 << 4); -- Thu
			--day_of_week := day_of_week | (1 << 5); -- Fri
			--day_of_week := day_of_week | (1 << 6); -- Sat
			--day_of_week := day_of_week | 1; -- Sun
		--end if;
	end if;
	
	return day_of_week;
END;
$$;

CREATE OR REPLACE FUNCTION mid_convert_condition_regulation_tbl()
    RETURNS smallint
    LANGUAGE plpgsql
AS $$
DECLARE
	rec              record;
	cur_cond_id      integer;
	car_type         smallint;
	all_car_type     smallint;
	start_year	 integer;
	start_month	 integer;
	start_day	 integer;
	end_year	 integer;
	end_month	 integer;
	end_day		 integer;
	start_hour	 integer;
	start_minute	 integer;
	end_hour	 integer;
	end_minute	 integer;
	day_of_week	 integer;
	exclude_date	 smallint;
BEGIN
	cur_cond_id := 0;
	all_car_type := -2;

	for rec in
		select distinct day, shour, ehour, sdate, edate, cartype
		from (
			select distinct day, shour, ehour, sdate, edate, cartype
			from "org_one-way"
			where day != '000000001'
			
			union
			
			select distinct day, shour, ehour, sdate, edate, cartype
			from "org_not-in"
			where day != '000000001'
		) a
		order by day, shour, ehour, sdate, edate, cartype
	loop
		-- set default value
		start_year	  := 0;
		start_month	  := 1;
		start_day	  := 1;
		end_year	  := 0;
		end_month	  := 12;
		end_day		  := 31;
		day_of_week	  := 0;
		start_hour	  := 0;
		start_minute      := 0;
		end_hour	  := 24;
		end_minute	  := 0;
		exclude_date	  := 0;
		
		-- car_type
		if rec.cartype = 0 then
			car_type := all_car_type;
		else
			raise EXCEPTION 'cartype = %', car_type;
		end if;

		-- day of week
		day_of_week := mid_get_day_of_week(rec.day);

		-- start month/day
		if char_length(rec.sdate) = 4 then
			start_month := substr(rec.sdate, 1, 2)::integer;
			start_day := substr(rec.sdate, 3, 2)::integer;
		else
			raise EXCEPTION 'sdate = %', rec.sdate;
		end if;

		-- end month/day
		if char_length(rec.edate) = 4 then
			end_month := substr(rec.edate, 1, 2)::integer;
			end_day := substr(rec.edate, 3, 2)::integer;
		else
			raise EXCEPTION 'edate = %', rec.edate;
		end if;

		-- start hour/minute
		if char_length(rec.shour) = 4 then
			start_hour := substr(rec.shour, 1, 2)::integer;
			start_minute := substr(rec.shour, 3, 2)::integer;
		else
			raise EXCEPTION 'shour = %', rec.shour;
		end if;

		-- end hour/minute
		if char_length(rec.ehour) = 4 then
			end_hour := substr(rec.ehour, 1, 2)::integer;
			end_minute := substr(rec.ehour, 3, 2)::integer;
		else
			raise EXCEPTION 'ehour = %', rec.ehour;
		end if;

		if (
    			(car_type = all_car_type) and
    			(day_of_week = 0) and
    			(start_year = 0) and
    			(start_month = 1) and
    			(start_day = 1) and
    			(start_hour = 0) and
    			(start_minute = 0) and
    			(end_year = 0) and
    			(end_month = 12) and
    			(end_day = 31) and
    			(end_hour = 24) and
    			(end_minute = 0) and
    			(exclude_date = 0)
		   )
		then
			-- condition always be effect, set cond_id = null
	    		insert into temp_condition_regulation_tbl(cond_id, day, shour, ehour, sdate, edate, cartype)
			values(null::integer, rec.day, rec.shour, rec.ehour, rec.sdate, rec.edate, rec.cartype);
		else
			-- insert new cond if condition is useful
			cur_cond_id := cur_cond_id + 1;

			insert into temp_condition_regulation_tbl(cond_id, day, shour, ehour, sdate, edate, cartype)
			values(cur_cond_id, rec.day, rec.shour, rec.ehour, rec.sdate, rec.edate, rec.cartype);

			insert into condition_regulation_tbl(cond_id, car_type, day_of_week, start_year, start_month, 
								start_day, start_hour, start_minute, end_year, end_month, 
								end_day, end_hour, end_minute, exclude_date)
	    		values(cur_cond_id, car_type, day_of_week, start_year, start_month, start_day, 
				start_hour, start_minute, end_year, end_month, end_day, end_hour, end_minute, exclude_date);
		end if;
	end loop;
	
	return 0;
END;
$$;

CREATE OR REPLACE FUNCTION mid_convert_regulation_oneway()
    RETURNS smallint
    LANGUAGE plpgsql
AS $$
DECLARE
	cur_regulation_id      integer;
	rec                    record;
	nCount                 integer;
	nIndex                 integer;
BEGIN
	-- regulation id
	select (case when max(regulation_id) is null then 0 else max(regulation_id) end)
	from regulation_relation_tbl
	into cur_regulation_id;

	for rec in
		select linkno, array_agg(dir) as array_dir, array_agg(cond_id) as array_condid
		from (
			select linkno, dir, cond_id
			from temp_org_one_way b
			left join temp_condition_regulation_tbl c
				on
					b.day = c.day and 
					b.shour = c.shour and 
					b.ehour = c.ehour and 
					b.sdate = c.sdate and 
					b.edate = c.edate and 
					b.cartype = c.cartype
			where cond_id is not null
			order by linkno, dir, cond_id
		) a
		group by linkno
		order by linkno
	loop
		-- current regulation id 
		cur_regulation_id := cur_regulation_id + 1;

		-- insert into regulation_item_tbl
		insert into regulation_item_tbl(regulation_id, linkid, nodeid, seq_num)
		values(cur_regulation_id, rec.linkno, null::bigint, 1);

		-- insert into regulation_relation_tbl
		nCount := array_upper(rec.array_condid, 1);
		nIndex := 1;
		while nIndex <= nCount loop
			if rec.array_dir[nIndex] = 2 then
				insert into regulation_relation_tbl(regulation_id, nodeid, inlinkid, outlinkid, condtype, cond_id)
				values(cur_regulation_id, null::bigint, rec.linkno, null::bigint, 43::smallint, rec.array_condid[nIndex]);
			elseif rec.array_dir[nIndex] = 3 then
				insert into regulation_relation_tbl(regulation_id, nodeid, inlinkid, outlinkid, condtype, cond_id)
				values(cur_regulation_id, null::bigint, rec.linkno, null::bigint, 42::smallint, rec.array_condid[nIndex]);
			end if;

			nIndex := nIndex + 1;
		end loop;
	end loop;
	
	return 0;
END;
$$;

CREATE OR REPLACE FUNCTION mid_convert_regulation_linkrow()
    RETURNS smallint
    LANGUAGE plpgsql
AS $$
DECLARE
	rec                    record;
	cur_regulation_id      integer;
	nCount                 integer;
	nIndex                 integer;
BEGIN
	-- regulation_id
	select (case when max(regulation_id) is null then 0 else max(regulation_id) end)
	from regulation_relation_tbl
	into cur_regulation_id;

	for rec in
		select fromlinkno, tnodeno, tolinkno, array_agg(cond_id) as array_condid
		from (
			select distinct fromlinkno, tnodeno, tolinkno, cond_id
			from temp_org_not_in b
			left join temp_condition_regulation_tbl c
				on
					b.day = c.day and 
					b.shour = c.shour and 
					b.ehour = c.ehour and 
					b.sdate = c.sdate and 
					b.edate = c.edate and 
					b.cartype = c.cartype
			order by fromlinkno, tnodeno, tolinkno, cond_id
		) a
		group by fromlinkno, tnodeno, tolinkno
		order by fromlinkno, tnodeno, tolinkno
	loop
		-- current regulation id 
		cur_regulation_id := cur_regulation_id + 1;

		-- insert into regulation_item_tbl
		insert into regulation_item_tbl(regulation_id, linkid, nodeid, seq_num)
		values(cur_regulation_id, rec.fromlinkno, null::bigint, 1::smallint);
		insert into regulation_item_tbl(regulation_id, linkid, nodeid, seq_num)
		values(cur_regulation_id, null::bigint, rec.tnodeno, 2::smallint);
		insert into regulation_item_tbl(regulation_id, linkid, nodeid, seq_num)
		values(cur_regulation_id, rec.tolinkno, null::bigint, 3::smallint);

		-- insert into regulation_relation_tbl
		nCount := array_upper(rec.array_condid, 1);
		nIndex := 1;
		while nIndex <= nCount loop
			insert into regulation_relation_tbl(regulation_id, nodeid, inlinkid, outlinkid, condtype, cond_id)
			values(cur_regulation_id, rec.tnodeno, rec.fromlinkno, rec.tolinkno, 1::smallint, rec.array_condid[nIndex]);

			nIndex := nIndex + 1;
		end loop;
	end loop;

	return 0;
END;
$$;


CREATE OR REPLACE FUNCTION zenrin_cnv_disp_class(elcode varchar)
    RETURNS smallint
    LANGUAGE plpgsql
AS $$
BEGIN
	return case when substr(elcode,4,1)='8' then 4
				when substr(elcode,1,1)='A' then 18 
				when substr(elcode,2,1)='C' then 18
				when substr(elcode,3,1)='7' then 18
                when substr(elcode,2,1)='8' then 14
                when substr(elcode,2,1)='B' then 14
                when substr(elcode,2,1)='A' then 3
                when substr(elcode,2,1)='1' then 12
                when substr(elcode,2,1)='2' then 11
                when substr(elcode,2,1)='3' then 9
                when substr(elcode,2,1)='4' then 8
                when substr(elcode,2,1)='5' then 7
                when substr(elcode,2,1)='6' then 6
                when substr(elcode,2,1)='7' then 5
                end;
END;
$$;

CREATE OR REPLACE FUNCTION zenrin_cnv_link_type(elcode varchar)
    RETURNS smallint
    LANGUAGE plpgsql
AS $$
BEGIN
	return 
		case when substr(elcode,4,1)='7' then 0
			 when substr(elcode,4,1)='2' then 1
			 when substr(elcode,1,1)='A' then 1
			 when substr(elcode,4,1)='1' then 2
             when substr(elcode,4,1)='4' then 3
             when substr(elcode,4,1)='6' then 4
             when substr(elcode,4,1)='8' then 4
             when substr(elcode,4,1)='3' then 5
             when substr(elcode,4,1)='9' then 6
             when substr(elcode,4,1)='5' then 7
             end;
END;
$$;

CREATE OR REPLACE FUNCTION zenrin_cnv_road_type(elcode varchar)
    RETURNS smallint
    LANGUAGE plpgsql
AS $$
BEGIN
	return case when substr(elcode,2,1)='A' then 8
				when substr(elcode,6,1) in ('1','2') or substr(elcode,2,1)='7' then 14
				when substr(elcode,1,1)='A' or substr(elcode,2,1)='C' or substr(elcode,3,1)='7' then 9  
		        when substr(elcode,2,1)='1' then 0
                when substr(elcode,2,1)='2' then 1
                when substr(elcode,2,1)='3' then 2
                when substr(elcode,2,1)='4' then 3
                when substr(elcode,2,1)='5' then 4
                when substr(elcode,2,1)='6' then 6
                when substr(elcode,2,1)='8' or substr(elcode,2,1)='B' then 10
           		end;
END;
$$;

CREATE OR REPLACE FUNCTION zenrin_cnv_toll(elcode varchar)
    RETURNS smallint
    LANGUAGE plpgsql
AS $$
BEGIN
	return case when substr(elcode,1,1)='A' then 2
                when substr(elcode,6,1)='2' then 2
                when substr(elcode,6,1)='4' then 2 
				when substr(elcode,6,1)='1' then 1 
                when substr(elcode,6,1)='3' then 1
    			end;
END;
$$;

CREATE OR REPLACE FUNCTION zenrin_cnv_speed_class(speed smallint)
    RETURNS smallint
    LANGUAGE plpgsql
AS $$
BEGIN
	return case WHEN speed > 130 THEN 1
                WHEN speed > 100 and speed <= 130 THEN 2
                WHEN speed > 90 and speed <= 100 THEN 3
                WHEN speed > 70 and speed <= 90 THEN 4
                WHEN speed > 50 and speed <= 70 THEN 5
                WHEN speed > 30 and speed <= 50 THEN 6
                WHEN speed > 11 and speed <= 30 THEN 7
                ELSE 8 END;
END;
$$;

CREATE OR REPLACE FUNCTION zenrin_cnv_width(elcode varchar)
    RETURNS smallint
    LANGUAGE plpgsql
AS $$
BEGIN
	return case when substr(elcode,3,1)='1' then 0
                when substr(elcode,3,1)='2' then 1
                when substr(elcode,3,1)='3' then 2
                when substr(elcode,3,1)='4' then 3
                when substr(elcode,3,1)='5' then 3
                else 4 end;
END;
$$;




CREATE OR REPLACE FUNCTION zenrin_findpasslink(mesh varchar, tnodeno integer, ysnodeno integer, passno_array integer[])  

  RETURNS varchar
  LANGUAGE plpgsql
  AS $$ 
  DECLARE 
      passnode_array integer[];
      i integer;
      meshcode1 varchar;
      link integer;
      snode integer;
      enode integer;
      linkid bigint;
      passlink varchar; 
BEGIN
      passnode_array := array_append(passno_array,ysnodeno);
      passnode_array := array_prepend(tnodeno,passnode_array);
      passlink := '0';
      for i in 1..(array_length(passnode_array,1) - 1) loop
         snode := passnode_array[i];
         if snode = 0 then
            continue;
         end if;
         enode := passnode_array[i+1];
         if enode = 0 then
            enode := passnode_array[i+2];
         end if;
         
         select linkno from org_road as a
            where ( a.snodeno = snode and a.enodeno = enode
                   or
                    a.snodeno = enode and a.enodeno = snode
                  )
                  and meshcode = mesh
         into  link;

         select link_id from temp_link_mapping as t
         where t.meshcode = mesh and t.linkno = link
         into linkid;
         if passlink = '0' then
            passlink := linkid::varchar;
         else
            passlink := passlink ||'|'|| (linkid::varchar);
         end if;
      end loop;

      return passlink;	
END;
$$;


CREATE OR REPLACE FUNCTION zenrin_findpasslink_count_bak(passlink varchar)  

  RETURNS integer
  LANGUAGE plpgsql
  AS $$ 
  DECLARE 
BEGIN
      return array_length(regexp_split_to_array(passlink,E'\\|+'),1);
END;
$$;


CREATE OR REPLACE FUNCTION zenrin_findpasslink_count(passlink varchar,separator varchar)  

  RETURNS integer
  LANGUAGE plpgsql
  AS $$ 
DECLARE 
	pass_array varchar[];
	length 		integer;
BEGIN
	pass_array = string_to_array(passlink,separator);
	length = array_length(pass_array,1);
	if length is null then
		return 0;
	else
		return length;
	end if;
      
END;
$$;

CREATE OR REPLACE FUNCTION zenrin_get_meshcode(jct_number varchar)
	RETURNS varchar
    LANGUAGE plpgsql
AS $$
DECLARE
	meshcode            varchar;
	secondMeshCode      integer;
BEGIN
	if jct_number is null then
		raise EXCEPTION 'jct_number is null';
	end if;

	secondMeshCode = substr(jct_number, 5, 2)::integer;

	if secondMeshCode <= 8 and secondMeshCode >= 1 then 
		secondMeshCode := secondMeshCode - 1;
	elseif secondMeshCode <= 16 and secondMeshCode >= 9 then
		secondMeshCode := secondMeshCode + 1;
	elseif secondMeshCode <= 24 and secondMeshCode >= 17 then
		secondMeshCode := secondMeshCode + 3;
	elseif secondMeshCode <= 32 and secondMeshCode >= 25 then
		secondMeshCode := secondMeshCode + 5;
	elseif secondMeshCode <= 40 and secondMeshCode >= 33 then
		secondMeshCode := secondMeshCode + 7;
	elseif secondMeshCode <= 48 and secondMeshCode >= 41 then
		secondMeshCode := secondMeshCode + 9;
	elseif secondMeshCode <= 56 and secondMeshCode >= 49 then
		secondMeshCode := secondMeshCode + 11;
	elseif secondMeshCode <= 64 and secondMeshCode >= 57 then
		secondMeshCode := secondMeshCode + 13;
	else 
		raise EXCEPTION 'jct_number(%s) is error', jct_number;
	end if;

	meshcode := substr(jct_number, 1, 4) || lpad(secondMeshCode::varchar, 2, '0');
	
	return meshcode;
END;
$$;

CREATE OR REPLACE FUNCTION zenrin_join(strings varchar[])  

  RETURNS varchar
  LANGUAGE plpgsql
  AS $$ 
  DECLARE
      i integer;
      s varchar;
BEGIN
	s := strings[1];
	if array_length(strings,1) > 1 then
		for i in 2..array_length(strings,1)
		loop
		   if strings[i] is not null then
            s := s ||strings[i];
	     end if;
		end loop;
	end if; 

  return s;
END;
$$;

CREATE OR REPLACE FUNCTION zenrin_make_lanenum_lr(data smallint[])  
  RETURNS smallint[]
  LANGUAGE plpgsql
  AS $$ 
  DECLARE
      i integer;
      lanenum_l smallint;
      lanenum_r smallint;
BEGIN
	lanenum_l = 0;
	lanenum_r = 0;
	
	i := 1;
	while i <=  array_length(data,1) 
	loop
	    if data[i] = 1 then
          lanenum_l := lanenum_l + 1;
	    else
          exit;
	    end if;
	    i := i + 1;
	end loop;

	i := array_length(data,1);
	if i <> 1 then
      while i > 0
      loop
          if data[i] = 1 then
             lanenum_r := lanenum_r + 1;
          else
             exit;
          end if;
          i := i - 1;
      end loop;
	end if;
		
  return array[lanenum_l,lanenum_r];
END;
$$;

CREATE OR REPLACE FUNCTION zenrin_find_tnode(mesh varchar,fromnode integer,tonode integer)  
  RETURNS integer[]
  LANGUAGE plpgsql
  AS $$ 
  DECLARE
      rec integer;
      fnode integer;
      tnode integer;
      node1 integer;
      node2 integer;
BEGIN
	fnode := fromnode;
	tnode := tonode;
	while True loop
		select count(*) from
		(
			select * from org_road as a
			where a.meshcode = mesh
				and (a.snodeno = tnode or a.enodeno = tnode)
		) as c
		into rec;
		if rec > 2 then
       exit;
		end if;
		select snodeno,enodeno from org_road as r
		where r.meshcode = mesh 
			and 
			(  r.snodeno = tnode and r.enodeno <> fnode 
			or 
			   r.enodeno = tnode and r.snodeno <> fnode
			)
			into node1,node2;		
		if node1 = tnode then 
       fnode := node1;
       tnode := node2;
		else
       fnode := node2;
       tnode := node1;
		end if;
	end loop;
	return array[fnode,tnode];	
END;
$$;


CREATE OR REPLACE FUNCTION mid_transtwd67totwd97(geom geometry)
  RETURNS geometry
  LANGUAGE plpgsql
  AS $$ 
DECLARE
    wkt text;
    substr1 text;
    substr2 text;
    strtmp text;
    result text;
    lon numeric;
    lat numeric;
    result_arr text[];
    point_tmp geometry;
    
BEGIN
    select st_astext(geom) into wkt;
    --raise info '%',wkt;
    strtmp = wkt;
    result_arr=array[''];
    while True loop
        
        select substring(str,1,pos-1),str1,substring(str,pos+len),str
        into substr1,substr2,strtmp
        from
        (
            select position(arr[1] in str) as pos,length(arr[1]) as len,str,arr[1] as str1 from
            (
                select regexp_matches(strtmp,E'[0-9\.]+ [0-9\.]+') as arr,strtmp as str
            ) a
        )a;
        lon = substring(substr2,1,position(' ' in substr2)-1)::numeric;
        lat = substring(substr2,position(' ' in substr2)+1)::numeric;
        point_tmp=mid_transtwd67totwd97_point(st_point(lon,lat));
        substr2=st_x(point_tmp)::text||' '||st_y(point_tmp)::text;
        result_arr=array_append(result_arr,substr1);
        result_arr=array_append(result_arr,substr2);
        if not strtmp ~ E'[0-9\.]+ [0-9\.]+' then
            result_arr=array_append(result_arr,strtmp);
            result=array_to_string(result_arr,'');
            return st_geomfromtext(result);
        end if;
    end loop;
END;
$$;
  
CREATE OR REPLACE FUNCTION mid_transtwd67totwd97_point(geom geometry)
  RETURNS geometry
  LANGUAGE plpgsql
  AS $$ 
DECLARE
    E67 numeric;
    N67 numeric;
    E97 numeric;
    N97 numeric;
    A   numeric;
    B   numeric;
    
BEGIN
    A= 0.00001549;
    B= 0.000006521;
    E67=st_x(geom);
    N67=st_y(geom);
    E97 = E67 + 807.8 + A * E67 + B * N67;
    N97 = N67 - 248.6 + A * N67 + B * E67;
    return st_point(E97,N97);
END;
$$;

  
CREATE OR REPLACE FUNCTION mid_transtwd67totwd97_bentu(geom geometry)
  RETURNS geometry
  LANGUAGE plpgsql
  AS $$ 
BEGIN
    return st_transform(st_setsrid(geom,999900),4326);
END;
$$;

CREATE OR REPLACE FUNCTION mid_transtwd67totwd97_jinmen(geom geometry)
  RETURNS geometry
  LANGUAGE plpgsql
  AS $$ 
BEGIN
    return st_transform(st_setsrid(geom,999902),4326);
END;
$$;


CREATE OR REPLACE FUNCTION mid_transtwd67totwd97_lianjiang(geom geometry)
  RETURNS geometry
  LANGUAGE plpgsql
  AS $$ 
BEGIN
    return st_transform(st_setsrid(geom,999901),4326);
END;
$$;

---------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION zenrin_cal_doubleroundabout_angle(
	in_geom geometry, in_dir integer, out_geom geometry, out_dir integer) RETURNS float
  LANGUAGE plpgsql
AS $$
DECLARE
	in_angle int;
	out_angle int;
	
	in_angle_f float;
	out_angle_f float;
BEGIN 
	in_angle := mid_cal_zm(in_geom,in_dir);
	out_angle := mid_cal_zm(out_geom,out_dir);
	
	in_angle := in_angle + 32768;
	in_angle_f := in_angle * 360.0 / 65535.0;

   	out_angle := out_angle + 32768;
   	out_angle_f := out_angle * 360.0 / 65535.0;

	return out_angle_f - in_angle_f;
    
END;
$$;

CREATE OR REPLACE FUNCTION zenrin_cal_doubleroundabout_path(nfromlink bigint, startnode bigint, ntolink bigint,  endnode bigint)
RETURNS varchar[]
LANGUAGE plpgsql volatile
AS $$
DECLARE
	rstPathCount		integer;
	rstPath				varchar;
	rstPathArray		varchar[];
	
	tmpPathArray		varchar[];
	tmpPathAttrArray	varchar[];
	tmpLastNodeArray	bigint[];
	tmpLastLinkArray	bigint[];
	tmpPathCount		integer;
	tmpPathIndex		integer;
	rec        			record;
	nStartNode          bigint;
	nScondNode          bigint;
	rec_out				record;
BEGIN
	--rstPath
	rstPathCount		:= 0;
	tmpPathArray		:= ARRAY[cast(nFromLink as varchar)];
	tmpPathAttrArray	:= ARRAY[cast(0 as varchar)];
	tmpPathCount		:= 1;
	tmpPathIndex		:= 1;
	nStartNode         	:= startNode;
	nScondNode          := endNode;

	tmpLastNodeArray	:= ARRAY[nStartNode];
	tmpLastLinkArray	:= ARRAY[nFromLink];
		
	WHILE tmpPathIndex <= tmpPathCount LOOP
-- 		raise INFO 'tmpPathIndex:%,tmpPathArray = %', tmpPathIndex,tmpPathArray[tmpPathIndex];
		-----stop if out link is found.
		if array_upper(regexp_split_to_array(tmpPathArray[tmpPathIndex], E'\\|+'),1) < 10 then
			
			if tmpLastLinkArray[tmpPathIndex] = nToLink then
				rstPathCount	:= rstPathCount + 1;
				rstPath			:= cast(tmpPathArray[tmpPathIndex] as varchar);
				
				if rstPathCount = 1 then
						rstPathArray		:= ARRAY[rstPath];
				else
-- 					raise info 'tmpPathAttrArray[tmpPathIndex]:%',tmpPathAttrArray[tmpPathIndex];
					if not ('1' = ANY(string_to_array(tmpPathAttrArray[tmpPathIndex],'|'))) then
						rstPathArray		:= array_append(rstPathArray, rstPath);
					end if;
				end if;
				
				tmpPathIndex := tmpPathIndex + 1;
				
				continue;
			else
				for rec in 
					select link_id as nextlink,inout_flag
						,(case  when s_node = tmpLastNodeArray[tmpPathIndex] then e_node 
							when e_node = tmpLastNodeArray[tmpPathIndex] then s_node
						end) as nextnode
					from  temp_roundabout_doublecircle_connect_links 
					where (
						(		
							((tmpLastNodeArray[tmpPathIndex] = s_node and oneway = 1) 
							or	
							((tmpLastNodeArray[tmpPathIndex] = s_node or tmpLastNodeArray[tmpPathIndex] = e_node) and oneway in (0,2)))   
						)
						and 
						not (
							cast(link_id as varchar) = ANY(tmpPathArray)
						)
						and (inout_flag = 2 or link_id in (nfromlink,ntolink))
					)
				loop
					
					if not (rec.nextlink in (nFromLink, tmpLastLinkArray[tmpPathIndex]))
						and not ((rec.nextlink)::varchar = ANY(regexp_split_to_array(tmpPathArray[tmpPathIndex], E'\\|+'))) 
					then
-- 						raise info ' find:%,%',rec.nextlink,rec.inout_flag;
						tmpPathCount		:= tmpPathCount + 1;
						tmpPathArray		:= array_append(tmpPathArray, cast(tmpPathArray[tmpPathIndex]||'|'||rec.nextlink as varchar));
						tmpPathAttrArray	:= array_append(tmpPathAttrArray, cast(tmpPathAttrArray[tmpPathIndex]||'|'||rec.inout_flag as varchar));
						tmpLastNodeArray	:= array_append(tmpLastNodeArray, cast(rec.nextnode as bigint));
						tmpLastLinkArray	:= array_append(tmpLastLinkArray, cast(rec.nextlink as bigint));
					end if;
					
				end loop;
			end if;
		end if;
		tmpPathIndex := tmpPathIndex + 1;
	END LOOP;

	return rstPathArray;
END;
$$;

CREATE OR REPLACE FUNCTION zenrin_update_inlink_to_no_dummy_link_bak(node bigint)
    RETURNS bigint[]
    LANGUAGE plpgsql
AS $$
DECLARE 
	rec 			          record;
	node_array 		      bigint[];
	result_link_array 	bigint[];
	i 			            integer;
   
BEGIN
	node_array = array_append(node_array,node);
	
	i := 1;
	while i <= array_length(node_array,1) loop
        raise info 'node = %',node_array[i];

        for rec in
            select link_id,link_type,
                    (case when a.s_node = node_array[1] then a.e_node
                          else a.s_node end ) as node_id
            from link_tbl as a
            where (  a.s_node = node_array[i] and a.one_way_code in (1,3)
                    or 
                      a.e_node = node_array[i] and a.one_way_code in (1,2)
                    )
                    and a.link_id not in (select link_id from temp_dummy_link_todelete )
            
        loop
              if rec.link_id is not null then
                    if rec.link_type <> 4  then
                          result_link_array = array_append(result_link_array,rec.link_id);
                     else 
                          if not rec.node_id = any(node_array) then
                            node_array = array_append(node_array,rec.node_id);
                          end if;
                    end if;	

              end if;

        end loop;
        i = i+ 1;
			
	end loop;

	return result_link_array;
		
END;
$$;

CREATE OR REPLACE FUNCTION zenrin_update_inlink_to_no_dummy_link(flag integer,id bigint, node bigint,outlink bigint)
    RETURNS integer
    LANGUAGE plpgsql
AS $$
DECLARE 
	rec 			        record;
	node_array 		      	bigint[];
	i 			        integer;
   
BEGIN
	node_array = array_append(node_array,node);
	
	i := 1;
	while i <= array_length(node_array,1) loop
        raise info 'node = %',node_array[i];

        for rec in
            select link_id,link_type,
                    (case when a.s_node = node_array[i] then a.e_node
                          else a.s_node end ) as node_id
            from link_tbl as a
            where (  a.s_node = node_array[i] and a.one_way_code in (1,3)
                    or 
                      a.e_node = node_array[i] and a.one_way_code in (1,2)
                    )
                    and a.link_id <> outlink
        loop
              if rec.link_id is not null then
                    if rec.link_type <> 4  then
                          if flag = 1 then
                              insert into temp_zenrin_dummy_new_inlink_node_signpost_uc(id,new_inlink,new_node)
                                  values(id,rec.link_id,node_array[i]);
                           end if;
                          if flag = 2 then
                              insert into temp_zenrin_dummy_new_inlink_node_crossname(id,new_inlink,new_node)
                                  values(id,rec.link_id,node_array[i]);
                           end if;
                     else 
                          if not rec.node_id = any(node_array) then
                            node_array = array_append(node_array,rec.node_id);
                          end if;
                    end if;	

              end if;

        end loop;
        i = i+ 1;
			
	end loop;

	return 0;
		
END;
$$;

CREATE OR REPLACE FUNCTION mid_find_intersectionlink(linkid bigint)
RETURNS integer
LANGUAGE plpgsql
AS $$
DECLARE
	rec record;
	cnt integer;
BEGIN
	delete from temp_intersection_link;
	
	insert into temp_intersection_link(link_id,s_node,e_node,one_way_code,link_type)
	select link_id,s_node,e_node,one_way_code,link_type
	from link_tbl
	where link_id=linkid;
	
	insert into temp_link_walked(link_id)
	values(linkid);
	
	while True loop
		cnt = 0;
		for rec in
			select distinct a.* from link_tbl a
			join temp_intersection_link b
			on (a.s_node in (b.s_node,b.e_node) or a.e_node in (b.s_node,b.e_node)) and a.link_type=4
			left join temp_intersection_link c
			on a.link_id=c.link_id
			where c.link_id is null
			order by a.link_id
		loop
			cnt = cnt+1;
			
			insert into temp_intersection_link(link_id,s_node,e_node,one_way_code,link_type)
			values(rec.link_id,rec.s_node,rec.e_node,rec.one_way_code,rec.link_type);
			
			insert into temp_link_walked(link_id)
			values(rec.link_id);
			
		end loop;
		if cnt = 0 then
			exit;
		end if;
	end loop;
	
	drop table if exists temp_dummy_node;
	create table temp_dummy_node 
	as
	(
	    select distinct s_node as node_id
	    from temp_intersection_link
	    union
	    select distinct e_node as node_id
	    from temp_intersection_link
	);	
	
	insert into temp_intersection_link
    (link_id,s_node,e_node,one_way_code,link_type)
    select distinct a.link_id,a.s_node,a.e_node,a.one_way_code,a.link_type 
    from link_tbl a
    join 
    temp_intersection_link b
    on b.s_node in (a.s_node,a.e_node) or b.e_node in (a.s_node,a.e_node)
    left join temp_intersection_link c
    on a.link_id=c.link_id
    where c.link_id is null
    order by a.link_id;
	
	return 0;
END;
$$;
