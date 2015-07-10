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

		-- Holiday
		if substr(day, 9, 1) = '1' then
			day_of_week := day_of_week | (1 << 7);
			day_of_week := day_of_week | (1 << 9);
		end if;
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
			union
			select distinct day, shour, ehour, sdate, edate, cartype
			from "org_not-in"
		) a
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
	return case when substr(elcode,4,1)='8' or substr(elcode,1,1)='A' then 18 
				else
		                	case Substr(elcode,2,1) when 'C' then 18
		                    when '8' then 14
		                    when 'B' then 14
		                    when 'A' then 3
		                    when '1' then 12
		                    when '2' then 11
		                    when '3' then 9
		                    when '4' then 8
		                    when '5' then 7
		                    when '6' then 6
		                    when '7' then 5
		                    else 4
		                    end 
                end;
END;
$$;

CREATE OR REPLACE FUNCTION zenrin_cnv_link_type(elcode varchar)
    RETURNS smallint
    LANGUAGE plpgsql
AS $$
BEGIN
	return 
		case when substr(elcode,1,1)='A' then 6
			 else case Substr(elcode,4,1) when '7' then 0
                    when '4' then 3
                    when '3' then 5
                    when '6' then 4
                    when '8' then 4
                    when '5' then 7
                    when '9' then 6
                    when '1' then 2
                    when '2' then 1
                    end
             end;
END;
$$;

CREATE OR REPLACE FUNCTION zenrin_cnv_road_type(elcode varchar)
    RETURNS smallint
    LANGUAGE plpgsql
AS $$
BEGIN
	return case when substr(elcode,1,1)='A' then 9  
		   else case Substr(elcode,2,1) when '8' then 10
                    when 'B' then 10
                    when 'A' then 8
                    when '1' then 0
                    when '2' then 1
                    when '3' then 3
                    when '4' then 4
                    when '5' then 4
                    when '6' then 6
                    when '7' then 6
                    when 'C' then 9
                    else 4
                    end
           end;
END;
$$;

CREATE OR REPLACE FUNCTION zenrin_cnv_toll(elcode varchar)
    RETURNS smallint
    LANGUAGE plpgsql
AS $$
BEGIN
	return case Substr(elcode,6,1) when '0' then 0
                    when '1' then 1
                    when '3' then 1
                    when '2' then 2
                    when '4' then 2 end;
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
	return case substr(elcode,3,1) when '1' then 0
                                    when '2' then 1
                                    when '3' then 2
                                    when '4' then 3
                                    when '5' then 3
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


CREATE OR REPLACE FUNCTION zenrin_findpasslink_count(passlink varchar)  

  RETURNS integer
  LANGUAGE plpgsql
  AS $$ 
  DECLARE 
BEGIN
      return array_length(regexp_split_to_array(passlink,E'\\|+'),1);
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