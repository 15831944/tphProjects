-----------------------------------------------------------------------------------------
-- script for iPC RDB (or jdb)
-----------------------------------------------------------------------------------------


create or replace function mid_get_json_string_for_japan_routeno(road_number integer, road_class integer)
	returns varchar
	language plpgsql
as $$
declare
	shield_type		varchar;
	shield_no		varchar;
	string_name		varchar;
	string_namelist	varchar;
begin
	IF road_class = 1 or road_class = 103 THEN
		 -- Country road's roadno
		shield_type	:= '496';
	ELSE  -- roadcls: 2, 4 , 104, 106
		-- Prefectural road's roadno
		shield_type	:= '503';
	END IF;
	shield_no		:= road_number::varchar;
	string_name		:= mid_get_json_string_for_routeno_japan(shield_type, shield_no);
	string_namelist	:= mid_get_json_string_for_list(ARRAY[string_name]);
	return string_namelist;
end;
$$;

create or replace function mid_get_json_string_for_japan_names(name_kanji_array varchar[], name_yomi_array varchar[])
	returns varchar
	language plpgsql
as $$
declare
	string_name			varchar;
	string_name_array	varchar[];
	string_namelist		varchar;
	nIndex				integer;
	nCount				integer;
begin
	nCount	:= array_upper(name_kanji_array,1);
	for nIndex in 1..nCount loop
		string_name			:= mid_get_json_string_for_name_japan(name_kanji_array[nIndex], name_yomi_array[nIndex]);
		string_name_array	:= string_name_array || string_name;
	end loop;
	string_namelist	:= mid_get_json_string_for_list(string_name_array);
	return string_namelist;
end;
$$;

create or replace function mid_get_json_string_for_japan_name(name_kanji varchar, name_yomi varchar)
	returns varchar
	language plpgsql
as $$
declare
	string_name		varchar;
	string_namelist	varchar;
begin
	string_name		:= mid_get_json_string_for_name_japan(name_kanji, name_yomi);
	string_namelist	:= mid_get_json_string_for_list(ARRAY[string_name]);
	return string_namelist;
end;
$$;

create or replace function mid_get_json_string_for_name_japan(name_kanji varchar, name_yomi varchar)
	returns varchar
	language plpgsql
as $$
declare
	string_kanji		varchar;
	string_yomi		varchar;
	string_name		varchar;
begin
	string_kanji		:= mid_get_json_string_for_name('JPN', 'left_right', 'office_name', 'not_tts', name_kanji);
	if name_yomi is null then
		string_name		:= mid_get_json_string_for_list(ARRAY[string_kanji]);
	else
		string_yomi		:= mid_get_json_string_for_name('JPY', 'left_right', 'office_name', 'not_tts', name_yomi);
		string_name		:= mid_get_json_string_for_list(ARRAY[string_kanji,string_yomi]);
	end if;
	return string_name;
end;
$$;

create or replace function mid_get_json_string_for_routeno_japan(shield_type varchar, shield_no varchar)
	returns varchar
	language plpgsql
as $$
declare
	string_shield	varchar;
	string_routeno	varchar;
	string_name		varchar;
begin
	string_shield	:= shield_type || '	' || shield_no;
	string_routeno	:= mid_get_json_string_for_name('JPN', 'left_right', 'shield', 'not_tts', string_shield);
	string_name		:= mid_get_json_string_for_list(ARRAY[string_routeno]);
	return string_name;
end;
$$;

-----------------------------------------------------------------------------------------
-- convert shield id
-----------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION rdb_cnv_shield_id_jdb(roadcls integer) 
    RETURNS smallint
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF roadcls = 1 or roadcls = 103 THEN  -- Country road's roadno
		RETURN 496;
	ELSE  -- roadcls: 2, 4 , 104, 106
		RETURN 503;                       -- Prefectural road's roadno
	END IF;
END;
$$;


-----------------------------------------------------------------------------------------
-- import sidefiles
-----------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION deal_type_of_sidefile(tablename varchar) RETURNS smallint
  LANGUAGE plpgsql
AS $$
DECLARE
	cur refcursor;
	rec record;
	i int;
	name varchar[];
	id int[];
	type_value varchar;
	gid_value int;
BEGIN 
	i := 0;
	
	open cur for execute

		'SELECT gid,type FROM ' || quote_ident(tablename) || ' where type is not null order by gid';
	fetch cur into rec;
	
	while rec.gid is not null loop

		raise info 'rec.gid:%',rec.gid;
		if i <> 0 then
		
			type_value := name[i - 1];
			gid_value := id[i - 1];

			execute 'update ' || quote_ident(tablename) || ' set "type" = ' || chr(39) ||  type_value || chr(39) || ' where gid < ' || rec.gid || ' and gid > ' || gid_value;

		end if;
		
		name[i] := rec.type;
		id[i] := rec.gid;		
		i := i + 1;
		
		fetch cur into rec;
	end loop;

	close cur;

	execute 'update ' || quote_ident(tablename) || ' set "type" = ' || chr(39) || name[i - 1] || chr(39) || ' where gid > ' || id[i - 1];	
	
    return 1;
END;
$$;


CREATE OR REPLACE FUNCTION create_geom_of_sidefile(meshcode int, x int, y int) RETURNS geometry
  LANGUAGE plpgsql
AS $$
DECLARE
	unitx float;
	unity float;
	Px smallint;
	Py smallint;
	Pnum smallint;
	col smallint;
	row smallint;
	lon float;
	lat float;
BEGIN 
        unitx := 1.0/8.0;
        unity := 40.0/60.0/8.0;

        Py := round(meshcode/10000);  
        Px := round((meshcode - py * 10000)/100);  
        Pnum := meshcode - Py * 10000 - Px * 100;  
        
        col := round(Pnum / 10); 
        row := Pnum - col * 10; 

	Py := Py * 8 + col;
	Px := Px * 8 + row;
	
        lon := unitx*Px + unitx/1000000.0 * x + 100; 
        lat := unity*Py + unity/1000000.0 * y; 

	return  st_transform(ST_SetSRID(ST_Point(lon, lat),4301),4326);
	
END;
$$;


-----------------------------------------------------------------------------------------
-- split link by attr
-----------------------------------------------------------------------------------------

CREATE OR REPLACE FUNCTION rdb_split_link_by_attr()
  RETURNS integer
  LANGUAGE plpgsql
  AS $$ 
DECLARE
	curs1 refcursor;
	temp_objectid integer;
	temp_linkid   integer;
	temp_linkattr_c integer;
	temp_attr_geom  geometry;
	temp_start_fraction double precision;
	temp_end_fraction double precision;
	temp_link_geom geometry;

	objectid_a integer[];
	linkattr_a integer[];
	attr_geom_a geometry[];
	s_fraction_a double precision[];
	e_fraction_a double precision[];
	num integer;
	curr_linkid integer;
	curr_link_geom geometry;
	cnt integer;
BEGIN
	OPEN curs1 FOR	SELECT objectid, link_id, linkattr_c, 
                               rdb_line_reverse(the_geom, start_fraction, end_fraction) as attr_geom,
                               rdb_min(start_fraction, end_fraction) as start_fraction,
                               rdb_max(start_fraction, end_fraction) as end_fraction,
                               link_geom
                          FROM (
                               SELECT A.objectid, link_id, linkattr_c, A.the_geom, 
                                      mid_get_fraction(ST_LineMerge(road_link_4326.the_geom), ST_StartPoint(A.the_geom)) as start_fraction,
                                      mid_get_fraction(ST_LineMerge(road_link_4326.the_geom), ST_EndPoint(A.the_geom)) as end_fraction,
                                      ST_LineMerge(road_link_4326.the_geom) as link_geom
                                FROM (
                                      SELECT objectid, link_id, linkattr_c, ST_LineMerge(the_geom) as the_geom
                                        FROM segment_attr_4326
                                        -- Tunnel, Railway Crossing
                                        WHERE linkattr_c IN (2, 4) 
                                ) as A
                                LEFT JOIN road_link_4326
                                ON link_id = road_link_4326.objectid
                          ) AS B
                          ORDER BY link_id, start_fraction, linkattr_c;

	num := 0;
	cnt := 0;
	-- Get the first record	
	FETCH curs1 INTO temp_objectid, temp_linkid, temp_linkattr_c, temp_attr_geom, temp_start_fraction, temp_end_fraction, temp_link_geom;
	curr_linkid    := temp_linkid;
	curr_link_geom := temp_link_geom;

	while temp_objectid is not NULL LOOP
		IF curr_linkid <> temp_linkid THEN
			cnt := cnt + rdb_split_a_link_by_attr(num, objectid_a, curr_linkid, linkattr_a,
			                     attr_geom_a, s_fraction_a, e_fraction_a, curr_link_geom);
                     
			num               := 0;
			curr_linkid       := temp_linkid;
			curr_link_geom    := temp_link_geom;
		END IF;
		
		num               := num + 1;
		objectid_a[num]   := temp_objectid;
		linkattr_a[num]   := temp_linkattr_c;
		attr_geom_a[num]  := temp_attr_geom;
		s_fraction_a[num] := temp_start_fraction;
		e_fraction_a[num] := temp_end_fraction;

		FETCH curs1 INTO temp_objectid, temp_linkid, temp_linkattr_c, temp_attr_geom, temp_start_fraction, temp_end_fraction, temp_link_geom;
	END LOOP;
	
	-- last link
	-- split and insert
	cnt := cnt + rdb_split_a_link_by_attr(num, objectid_a, curr_linkid, linkattr_a, 
			           attr_geom_a, s_fraction_a, e_fraction_a, curr_link_geom);
			           
	close curs1;
	
	return cnt;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_split_a_link_by_attr(num integer, objectid_a integer[], linkid integer, linkattr_a integer[], attr_geom_a geometry[], s_fraction_a double precision[], e_fraction_a double precision[], link_geom geometry)
  RETURNS integer
  LANGUAGE plpgsql
  AS $$ 
DECLARE
	i integer;
	j integer;
	start_fraction double precision;
	end_fraction double precision;
	last_fraction double precision;
	temp_geom geometry;
	start_point geometry;
	end_point geometry;
	last_point geometry;
	seq_num integer;
BEGIN
	last_fraction := 0;
	last_point    := ST_StartPoint(link_geom);
	
	seq_num := 0;
	i       := 1;
	while i <= num Loop
		start_fraction := s_fraction_a[i];
		start_point    := ST_StartPoint(attr_geom_a[i]);
		
		if s_fraction_a[i] > last_fraction THEN
			temp_geom    := rdb_line_substring(link_geom, 
			                               last_point, 
			                               start_point, 
			                               last_fraction, 
			                               start_fraction);
			-- insert 
			INSERT INTO temp_split_link(
				"objectid", "linkid", "linkattr_c", "start_fraction", "end_fraction", "the_geom")
				VALUES (0, linkid, 0, last_fraction, start_fraction, ST_SetSrid(temp_geom, 4326));
			seq_num := seq_num + 1;
			last_point    := start_point;
			last_fraction := start_fraction;

		END IF;
		
		if s_fraction_a[i] <> e_fraction_a[i] then
			end_fraction := e_fraction_a[i];
			end_point    := ST_EndPoint(attr_geom_a[i]);
			
			j := i + 1;
			while j <= num loop
				-- doesn't Overlap
				if s_fraction_a[j] >= end_fraction or e_fraction_a[j] <= start_fraction THEN
					j := j + 1;
					continue;
				END IF;
				
				-- Complete Overlap
				if e_fraction_a[j] <= end_fraction then
					if linkattr_a[j] <= linkattr_a[i] then
						s_fraction_a[j] := e_fraction_a[j];
					else 
						if start_fraction < s_fraction_a[j] then
							temp_geom := rdb_line_substring(link_geom, 
								       start_point, 
								       ST_StartPoint(attr_geom_a[j]), 
								       start_fraction, 
								       s_fraction_a[j]);
							
							-- insert
							INSERT INTO temp_split_link(
								"objectid", "linkid", "linkattr_c", "start_fraction", "end_fraction", "the_geom")
								VALUES (objectid_a[i], linkid, linkattr_a[i], start_fraction, s_fraction_a[j], ST_SetSrid(temp_geom, 4326));
							seq_num       := seq_num + 1;
						end if;
						start_fraction := e_fraction_a[j];
						start_point    := ST_EndPoint(attr_geom_a[j]);
					end if;
				end if;

				-- Part Overlap
				if e_fraction_a[j] > end_fraction then
					-------------------------------------------------
					--INSERT INTO temp_test("linkid") values(linkid);
					-------------------------------------------------
					if linkattr_a[j] <= linkattr_a[i] then
						s_fraction_a[j] := end_fraction;
						attr_geom_a[j]  := rdb_line_substring(link_geom, 
								       ST_EndPoint(attr_geom_a[i]), 
								       ST_EndPoint(attr_geom_a[j]), 
								       e_fraction_a[i], 
								       e_fraction_a[j]);
					else 
						if start_fraction <> s_fraction_a[j] then
							end_fraction := s_fraction_a[j];
							end_point    := ST_StartPoint(attr_geom_a[j]);
						end if;
						exit ;
					end if;
				end if;
				
				j := j + 1;
			end loop;
		END IF;
		
		if start_fraction < end_fraction then
			temp_geom = rdb_line_substring(link_geom, 
			                               start_point, 
			                               end_point, 
			                               start_fraction, 
			                               end_fraction);
			-- insert
			INSERT INTO temp_split_link(
					"objectid", "linkid", "linkattr_c", "start_fraction", "end_fraction", "the_geom")
					VALUES (objectid_a[i], linkid, linkattr_a[i], start_fraction, end_fraction, ST_SetSrid(temp_geom, 4326));
			seq_num       := seq_num + 1; 
				
			if last_fraction < end_fraction then 
				last_point    := end_point;
				last_fraction := end_fraction;
			end if;
		end if;
		
		if i = num and last_fraction < 1 THEN
			temp_geom = rdb_line_substring(link_geom, 
						last_point, 
						ST_EndPoint(link_geom), 
						last_fraction, 
						1);
			-- insert
			INSERT INTO temp_split_link(
					"objectid", "linkid", "linkattr_c", "start_fraction", "end_fraction", "the_geom")
					VALUES (0, linkid, 0, last_fraction, 1, ST_SetSrid(temp_geom, 4326));
			seq_num := seq_num + 1; 
		END IF;
		i := i + 1;
	end loop;
	
	return seq_num;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_line_substring(line_geom geometry, start_point geometry, end_point geometry, startfraction double precision, endfraction double precision)
  RETURNS geometry
  LANGUAGE plpgsql
  AS $$ 
DECLARE
	sub_string geometry;
	temp_point geometry;
	temp_fraction double precision;
BEGIN
	IF startfraction > endfraction THEN
		temp_fraction := startfraction;
		startfraction := endfraction;
		endfraction   := temp_fraction;

		temp_point  := start_point;
		start_point := end_point;
		end_point   := temp_point;
	END IF;

	sub_string := ST_Line_Substring(line_geom, startfraction, endfraction);
	
	if start_point is not null and startfraction <> 0 then
		IF ST_Equals(ST_PointN(sub_string, 2), start_point) THEN        -- 1-based
			sub_string := ST_RemovePoint(sub_string, 0);  -- 0-based
		else
			-- replace the first point
			sub_string := ST_SetPoint(sub_string, 0, start_point);
		END IF;
	end if;
		
	if end_point is not null and endfraction <> 1 then
		IF ST_Equals(ST_PointN(sub_string, (ST_NumPoints(sub_string) - 1)), end_point) THEN       -- 1-based
			sub_string := ST_RemovePoint(sub_string, ST_NumPoints(sub_string) - 1); -- 0-based
		else
			-- replace the first point
			sub_string := ST_SetPoint(sub_string, ST_NumPoints(sub_string) - 1, end_point);
		end if;
	end if;

	return sub_string;
END;
$$;


CREATE OR REPLACE FUNCTION rdb_make_new_link_by_attr()
  RETURNS integer
  LANGUAGE plpgsql
  AS $$ 
DECLARE
	curs1 refcursor;
	pre_objectid integer;
	pre_linkid   integer;
	pre_linkattr_c integer;
	pre_geom geometry;
	pre_length double precision;

	curr_objectid integer;
	curr_linkid   integer;
	curr_linkattr_c integer;
	curr_geom geometry;
	curr_length double precision;
	
	seq_num integer;
	pre_merge_flag smallint;
BEGIN
	OPEN curs1 FOR	SELECT objectid, linkid, linkattr_c, 
			       the_geom, ST_Length_Spheroid(the_geom,'SPHEROID("WGS_84", 6378137, 298.257223563)') as length
			  FROM temp_split_link 
			  order by linkid, start_fraction;

	FETCH curs1 INTO curr_objectid, curr_linkid, curr_linkattr_c,  curr_geom, curr_length;

	seq_num     := 0;
	
	while curr_linkid is not NULL LOOP
		
		IF pre_linkid is null THEN
			pre_objectid       := curr_objectid;
			pre_linkid         := curr_linkid;
			pre_linkattr_c     := curr_linkattr_c;
			pre_geom 	   := curr_geom;
			pre_length 	   := curr_length;
		
		ELSIF pre_linkid <> curr_linkid THEN
			INSERT INTO temp_new_link(
					objectid, linkid, linkattr_c, "split_seq_num", the_geom)
				VALUES (pre_objectid, pre_linkid, pre_linkattr_c, seq_num, pre_geom);
				
			seq_num            := 0;
			
			pre_objectid       := curr_objectid;
			pre_linkid         := curr_linkid;
			pre_linkattr_c     := curr_linkattr_c;
			pre_geom 	   := curr_geom;
			pre_length 	   := curr_length;

		else 	
			if curr_linkattr_c = 0 and curr_length <= 1 then					
				pre_geom  := ST_LineMerge(ST_union(pre_geom, curr_geom));
				
				pre_length 	   := curr_length + pre_length;

			else
				if pre_linkattr_c = 0 and pre_length <= 1 then
					pre_geom := ST_LineMerge(ST_union(pre_geom, curr_geom)); 
					pre_linkid         := curr_linkid;
					pre_linkattr_c     := curr_linkattr_c;
					pre_length 	   := curr_length + pre_length;					
	
				else
					if curr_linkattr_c = pre_linkattr_c then 
						pre_geom := ST_LineMerge(ST_union(pre_geom, curr_geom)); 
						pre_linkid         := curr_linkid;
						pre_linkattr_c     := curr_linkattr_c;
						pre_length 	   := curr_length  + pre_length;
					else
						INSERT INTO temp_new_link(
							objectid, linkid, linkattr_c, "split_seq_num", the_geom)
						VALUES (pre_objectid, pre_linkid, pre_linkattr_c, seq_num, pre_geom);
						
						seq_num  := seq_num + 1;

						pre_linkid         := curr_linkid;
						pre_linkattr_c     := curr_linkattr_c;
						pre_geom           := curr_geom;
						pre_length 	   := curr_length;
					end if;			
				end if;			
			end if;
			
		END IF;
		
		FETCH curs1 INTO curr_objectid, curr_linkid, curr_linkattr_c,  curr_geom, curr_length;
	END LOOP;
	
	INSERT INTO temp_new_link(
		objectid, linkid, linkattr_c, "split_seq_num", the_geom)
	VALUES (pre_objectid, pre_linkid, pre_linkattr_c, seq_num, pre_geom);	
	 
	close curs1;
	
	return 1;
END;
$$;

-- CREATE OR REPLACE FUNCTION rdb_make_new_link_by_attr()
--   RETURNS integer
--   LANGUAGE plpgsql
--   AS $$ 
-- DECLARE
-- 	curs1 refcursor;
-- 	curr_objectid integer;
-- 	curr_linkid   integer;
-- 	curr_linkattr_c integer;
-- 	curr_delt_fraction double precision;
-- 	curr_geom geometry;
-- 
-- 	next_objectid integer;
-- 	next_linkid   integer;
-- 	next_linkattr_c integer;
-- 	next_delt_fraction double precision;
-- 	next_geom geometry;
-- 	
-- 	seq_num integer;
-- 	insert_flg boolean;
-- 	merger_flg boolean;
-- 	cnt integer;
-- BEGIN
-- 	OPEN curs1 FOR	SELECT objectid, linkid, linkattr_c, (end_fraction - start_fraction) as delt_fraction, 
-- 			       the_geom
-- 			  FROM temp_split_link
-- 			  order by linkid, start_fraction;
-- 
-- 	cnt := 0;
-- 	-- Get the first record	
-- 	FETCH curs1 INTO curr_objectid, curr_linkid, curr_linkattr_c, curr_delt_fraction, curr_geom;
-- 	FETCH curs1 INTO next_objectid, next_linkid, next_linkattr_c, next_delt_fraction, next_geom;
-- 	seq_num     := 0;
-- 	insert_flg  := TRUE;
-- 	while curr_linkid is not NULL LOOP
-- 		IF next_linkid is null or curr_linkid <> next_linkid THEN
-- 			INSERT INTO temp_new_link(
-- 					objectid, linkid, linkattr_c, "split_seq_num", the_geom)
-- 				VALUES (curr_objectid, curr_linkid, curr_linkattr_c, seq_num, curr_geom);
-- 			cnt                 := cnt + 1;
-- 			seq_num             := 0;
-- 			curr_objectid       := next_objectid;
-- 			curr_linkid         := next_linkid;
-- 			curr_linkattr_c     := next_linkattr_c;
-- 			curr_delt_fraction  := next_delt_fraction;
-- 			curr_geom           := next_geom;
-- 		else 	
-- 			insert_flg := true;
-- 			merger_flg := false;
-- 			if next_linkattr_c = curr_linkattr_c then 
-- 				-- error
-- 			end if;
-- 			
-- 			-- normal link ==> railway crossing
-- 			--if next_linkattr_c = 4 and curr_linkattr_c = 0 and next_delt_fraction > (curr_delt_fraction * 2) then
-- 			--	curr_geom  := ST_LineMerge(ST_union(curr_geom, next_geom));
-- 			--	insert_flg := FALSE;
-- 			--	merger_flg := true;
-- 			--end if;
-- 			
-- 			-- railway crossing ==> normal link
-- 			--if next_linkattr_c = 0 and curr_linkattr_c = 4 and next_delt_fraction < (curr_delt_fraction / 2) then
-- 			--	curr_geom  := ST_LineMerge(ST_union(curr_geom, next_geom));
-- 			--	merger_flg := true;
-- 			--end if;
-- 			
-- 			-- normal link ==> tunnel
-- 			--if next_linkattr_c = 2 and curr_linkattr_c = 0 and next_delt_fraction > (curr_delt_fraction * 20) then
-- 			--	curr_geom  := ST_LineMerge(ST_union(curr_geom, next_geom));
-- 			--	insert_flg := FALSE;
-- 			--	merger_flg := true;
-- 			--end if;
-- 			
-- 			-- tunnel ==> normal link
-- 			--if next_linkattr_c = 0 and curr_linkattr_c = 2 and next_delt_fraction < (curr_delt_fraction / 20) then
-- 			--	curr_geom  := ST_LineMerge(ST_union(curr_geom, next_geom));
-- 			--	merger_flg := true;
-- 			--end if;
-- 			
-- 			if insert_flg = TRUE then 
-- 				INSERT INTO temp_new_link(
-- 						    objectid, linkid, linkattr_c, "split_seq_num", the_geom)
-- 					    VALUES (curr_objectid, curr_linkid, curr_linkattr_c, seq_num, curr_geom);
-- 					
-- 				cnt      := cnt + 1;
-- 				seq_num  := seq_num + 1;
-- 				if merger_flg = FALSE THEN 
-- 					curr_objectid       := next_objectid;
-- 					curr_linkattr_c     := next_linkattr_c;
-- 					curr_delt_fraction  := next_delt_fraction;
-- 					curr_geom           := next_geom; 
-- 				ELSE
-- 					FETCH curs1 INTO curr_objectid, curr_linkid, curr_linkattr_c, curr_delt_fraction, curr_geom;
-- 					if curr_linkid <> next_linkid then 
-- 						seq_num  := 0;						
-- 					end if;
-- 				END IF;
-- 			else
-- 				curr_objectid       := next_objectid;
-- 				curr_linkattr_c     := next_linkattr_c;
-- 				curr_delt_fraction  := next_delt_fraction;
-- 			end if;	
-- 		END IF;
-- 		
-- 		
-- 		FETCH curs1 INTO next_objectid, next_linkid, next_linkattr_c, next_delt_fraction, next_geom;
-- 	END LOOP;			           
-- 	close curs1;
-- 	
-- 	return cnt;
-- END;
-- $$;



CREATE OR REPLACE FUNCTION rdb_line_reverse(the_geom geometry, startfraction double precision, endfraction double precision)
  RETURNS geometry
  LANGUAGE plpgsql
  AS $$
DECLARE
	temp_geom geometry;
BEGIN
	IF startfraction <= endfraction THEN
		temp_geom := the_geom;
	ELSE
		temp_geom := ST_Reverse(the_geom);
	END IF;

	return temp_geom;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_min(startfraction double precision, endfraction double precision)
  RETURNS double precision
  LANGUAGE plpgsql
  AS $$
DECLARE
BEGIN
	IF startfraction <= endfraction THEN
		return startfraction;
	ELSE
		return endfraction;
	END IF;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_max(startfraction double precision, endfraction double precision)
  RETURNS double precision
  LANGUAGE plpgsql
  AS $$
DECLARE
BEGIN
	IF startfraction <= endfraction THEN
		return endfraction;
	ELSE
		return startfraction;
	END IF;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_make_new_node_link()
  RETURNS integer
  LANGUAGE plpgsql
  AS $$ 
DECLARE
	curs1 refcursor;
	curs2 refcursor;
	temp_link record;
	curr_link record;
	curr_node_id integer;
	cnt2 integer;
BEGIN
	-- get the max node id
	OPEN curs2 FOR SELECT max(objectid)
		    FROM road_node;
	FETCH curs2 INTO curr_node_id;
	if curr_node_id is null then
		curr_node_id := 0;
	end if;
	close curs2;
	
	OPEN curs1 FOR 
			SELECT linkid, from_node_id, to_node_id, roadclass_c, naviclass_c, 
                   linkclass_c, roadmanager_c, roadwidth_c, widearea_f, smartic_c, 
                   bypass_f, caronly_f, island_f, road_no, road_code, nopassage_c, 
                   oneway_c, lane_count, linkattr_c, 
			       split_seq_num, ST_linemerge(B.the_geom) as the_geom
			FROM (
			    SELECT linkid, linkattr_c, split_seq_num, the_geom
			      FROM temp_new_link
			      where linkid in (
					 -- the link, have attribute and are split. 
					 select linkid
					 from (
					     select linkid, count(linkid) as cnt
					     from temp_new_link
					     group by linkid
					 ) as A
					 where cnt > 1
					 ) 
			) AS B
			LEFT JOIN road_link_4326
			ON linkid = objectid
			order by linkid, split_seq_num;
	cnt2 := 0;
	
	FETCH curs1 INTO curr_link;
	FETCH curs1 INTO temp_link;
	
	while curr_link.linkid is not NULL LOOP
		IF temp_link IS NULL OR curr_link.linkid <> temp_link.linkid then
			-- insert last link
			INSERT INTO temp_road_link(
					    objectid, from_node_id, to_node_id, roadclass_c, naviclass_c, 
	                    linkclass_c, roadmanager_c, roadwidth_c, widearea_f, smartic_c, 
	                    bypass_f, caronly_f, island_f, road_no, road_code, 
	                    nopassage_c, oneway_c, lane_count, 
	                    linkattr_c, split_seq_num, the_geom)
					values (
					    curr_link.linkid, curr_node_id, curr_link.to_node_id, curr_link.roadclass_c, curr_link.naviclass_c, 
					    curr_link.linkclass_c, curr_link.roadmanager_c, curr_link.roadwidth_c, curr_link.widearea_f, curr_link.smartic_c, 
					    curr_link.bypass_f, curr_link.caronly_f, curr_link.island_f, curr_link.road_no, curr_link.road_code, 
					    curr_link.nopassage_c, curr_link.oneway_c, curr_link.lane_count, 
					    curr_link.linkattr_c, curr_link.split_seq_num, curr_link.the_geom);
			cnt2      := cnt2 + 1;
			curr_link := temp_link;
		else 
			if curr_link.split_seq_num = 0 then
				curr_node_id := curr_node_id + 1;
				-- insert end node 
				INSERT INTO temp_road_node(
						objectid, signal_f, new_add, the_geom)
					values (curr_node_id, 0, true, ST_EndPoint(curr_link.the_geom));
				
				-- insert first link
				INSERT INTO temp_road_link(
						    objectid, from_node_id, to_node_id, roadclass_c, naviclass_c, 
		                    linkclass_c, roadmanager_c, roadwidth_c, widearea_f, smartic_c, 
		                    bypass_f, caronly_f, island_f, road_no, road_code, 
		                    nopassage_c, oneway_c, lane_count, 
		                    linkattr_c, split_seq_num, the_geom)
						values (
						    curr_link.linkid, curr_link.from_node_id, curr_node_id, curr_link.roadclass_c, curr_link.naviclass_c, 
						    curr_link.linkclass_c, curr_link.roadmanager_c, curr_link.roadwidth_c, curr_link.widearea_f, curr_link.smartic_c, 
						    curr_link.bypass_f, curr_link.caronly_f, curr_link.island_f, curr_link.road_no, curr_link.road_code, 
						    curr_link.nopassage_c, curr_link.oneway_c, curr_link.lane_count, 
						    curr_link.linkattr_c, curr_link.split_seq_num, curr_link.the_geom);
				cnt2 := cnt2 + 1;
			else 
				curr_node_id := curr_node_id + 1;
				-- insert end node 
				INSERT INTO temp_road_node(
						objectid, signal_f, new_add, the_geom)
					values (curr_node_id, 0, true, ST_EndPoint(curr_link.the_geom));
				
				-- insert link
				INSERT INTO temp_road_link(
						    objectid, from_node_id, to_node_id, roadclass_c, naviclass_c, 
		                    linkclass_c, roadmanager_c, roadwidth_c, widearea_f, smartic_c, 
		                    bypass_f, caronly_f, island_f, road_no, road_code, 
		                    nopassage_c, oneway_c, lane_count,  
		                    linkattr_c, split_seq_num, the_geom)
						values (
						    curr_link.linkid, curr_node_id - 1, curr_node_id, curr_link.roadclass_c, curr_link.naviclass_c, 
						    curr_link.linkclass_c, curr_link.roadmanager_c, curr_link.roadwidth_c, curr_link.widearea_f, curr_link.smartic_c, 
						    curr_link.bypass_f, curr_link.caronly_f, curr_link.island_f, curr_link.road_no, curr_link.road_code, 
						    curr_link.nopassage_c, curr_link.oneway_c, curr_link.lane_count, 
						    curr_link.linkattr_c, curr_link.split_seq_num, curr_link.the_geom);					    
				cnt2 := cnt2 + 1;
			end if;
			curr_link := temp_link;
		end if;
		
		FETCH curs1 INTO temp_link;
	END LOOP;
	
	close curs1;
	
	return cnt2;
END;
$$;

CREATE OR REPLACE FUNCTION mid_check_split_Link_by_attr()
	RETURNS integer
	LANGUAGE plpgsql
AS $$ 
DECLARE
	rec		record;
	index	integer;
	count	integer;
BEGIN
	-- 1.sublink/node is connective
	-- 2.sub geoms is connective
	for rec in
		select  objectid, 
				count(*) as sub_count,
				array_agg(split_seq_num) as seq_array,
				array_agg(from_node_id) as fnode_array,
				array_agg(to_node_id) as tnode_array, 
				array_agg(the_geom) as geom_array
		from
		(
			select objectid, split_seq_num, from_node_id, to_node_id, the_geom
			from temp_road_link
			where split_seq_num >= 0
			order by objectid, split_seq_num
		)as t
		group by objectid
	loop
		for index in 2..rec.sub_count loop
			if rec.fnode_array[index] != rec.tnode_array[index-1] then
				raise EXCEPTION 'split_link error 1 (sublink/node connectivity): objectid = %', rec.objectid;
			end if;
			if not st_equals(st_startpoint(rec.geom_array[index]), st_endpoint(rec.geom_array[index-1])) then
				raise EXCEPTION 'split_link error 2 (subgeom connectivity): objectid = %', rec.objectid;
			end if;
		end loop;
	end loop;
	
	-- 3.sublink's startnode/endnode geom is the start/end point of geom
	for rec in
		select  a.objectid, a.split_seq_num, 
				a.the_geom as link_geom,
				b.the_geom as from_geom,
				c.the_geom as to_geom
		from temp_road_link as a
		left join temp_road_node as b
		on a.from_node_id = b.objectid
		left join temp_road_node as c
		on a.to_node_id = c.objectid
		where a.split_seq_num >= 0
	loop
		if not st_equals(rec.from_geom, st_startpoint(rec.link_geom)) or not st_equals(rec.to_geom, st_endpoint(rec.link_geom)) then
			raise EXCEPTION 'split_link error 3 (subnode geom): objectid = %, split_seq_num = %', rec.objectid, rec.split_seq_num;
		end if;
	end loop;
	
	-- 4.first node and last node is old node, and middle node is new node
	for rec in
		select  objectid, 
				count(*) as sub_count, 
				array_agg(fnode) as fnode_array,
				array_agg(tnode) as tnode_array
		from
		(
			select a.objectid, a.split_seq_num, b.objectid as fnode, c.objectid as tnode
			from temp_road_link as a
			left join road_node_4326 as b
			on a.from_node_id = b.objectid
			left join road_node_4326 as c
			on a.to_node_id = c.objectid
			where a.split_seq_num >= 0
			order by a.objectid, a.split_seq_num
		)as t
		group by objectid
	loop
		for index in 1..rec.sub_count loop
			if 	   (index = 1 and rec.fnode_array[index] is null) 
				or (index > 1 and rec.fnode_array[index] is not null) 
				or (index = rec.sub_count and rec.tnode_array[index] is null) 
				or (index < rec.sub_count and rec.tnode_array[index] is not null) 
			then
				raise EXCEPTION 'split_link error 4 (subnode new/old): objectid = %', rec.objectid;
			end if;
		end loop;
	end loop;
	
	-- 5.sum of sub geoms equals old geom
	for rec in
		select	objectid
		from
		(
			select objectid, old_geom, st_linemerge(st_collect(sub_geom)) as merge_geom
			from
			(
				select a.objectid, a.the_geom as old_geom, b.the_geom as sub_geom
				from road_link_4326 as a
				inner join temp_road_link as b
				on a.objectid = b.objectid and b.split_seq_num >= 0
			)as t
			group by objectid, old_geom
		)as t2
		where not st_equals(old_geom, merge_geom)
	loop
		raise EXCEPTION 'split_link error 5 (subgeom): objectid = %', rec.objectid;
	end loop;
	
	-- 6.sublink's geom can be find in attr_table
	for rec in 
		select a.objectid, a.split_seq_num
		from temp_road_link as a
		left join segment_attr_4326 as b
		on a.objectid = b.link_id and a.linkattr_c = b.linkattr_c ---and st_equals(a.the_geom, b.the_geom)
		where a.linkattr_c > 0 and b.objectid is null
	loop
		raise EXCEPTION 'split_link error 6 (sublink attr): objectid = %, split_seq_num = %', rec.objectid, rec.split_seq_num;
	end loop;
	
	return 0;
END;
$$;


CREATE OR REPLACE FUNCTION getSplitLinkID(nCurrentLinkid integer, nConnectedLinkid integer, bConnectedFlag boolean)
  RETURNS bigint 
  LANGUAGE plpgsql
  AS $$ 
DECLARE
	curs1 refcursor;
	curs2 refcursor;
	rec1 record;
	rec2 record;
	nTargetLinkid bigint;
	nConnectedNodeid integer;
	bStartSubLink boolean;
BEGIN
	nTargetLinkid := null;
	
	open curs1 for execute
		'select * from middle_tile_link where old_link_id = ' || (@nCurrentLinkid) || 'order by split_seq_num asc';
	
	fetch curs1 into rec1;
	if rec1.tile_link_id is not null then
		if rec1.split_seq_num = -1 then
			nTargetLinkid := rec1.tile_link_id;
		else
			-- get connected nodeid
			nConnectedNodeid := null;
			open curs2 for execute
				'SELECT rdb_get_connected_nodeid_ipc(A.from_node_id, A.to_node_id, B.from_node_id, B.to_node_id) as nodeid
				  From (select * from road_link where objectid = ' || @nCurrentLinkid   || ') as A,
				       (select * from road_link where objectid = ' || @nConnectedLinkid || ') as B';
			
			fetch curs2 into rec2;
			if not rec2.nodeid is null then
				nConnectedNodeid := rec2.nodeid;
			end if;
			close curs2;
			
			-- get target link
			bStartSubLink := (nConnectedNodeid = any(rec1.old_s_e_node));
			if bStartSubLink = bConnectedFlag then
				nTargetLinkid := rec1.tile_link_id;
			else
				while not rec1.tile_link_id is null LOOP
					nTargetLinkid := rec1.tile_link_id;
					fetch curs1 into rec1;
				END LOOP;
			end if;
		end if;
	end if;
	close curs1;
	
	return nTargetLinkid;
END;
$$;

CREATE OR REPLACE FUNCTION getSplitLinkIDArray(nCurrentLinkid integer, nConnectedLinkid integer, bConnectedFlag boolean)
  RETURNS bigint[] 
  LANGUAGE plpgsql
  AS $$
DECLARE
	curs1 refcursor;
	curs2 refcursor;
	rec1 record;
	rec2 record;
	arrayTargetLinkid bigint[];
	nConnectedNodeid integer;
	bStartSubLink boolean;
	bReverseFlag boolean;
BEGIN
	open curs1 for execute
		'select * from middle_tile_link where old_link_id = ' || (@nCurrentLinkid) || 'order by split_seq_num asc';
	
	fetch curs1 into rec1;
	if not rec1.tile_link_id is null then
		if rec1.split_seq_num = -1 then
			arrayTargetLinkid[1] := rec1.tile_link_id;
		else
			-- get connected nodeid
			nConnectedNodeid := null;
			open curs2 for execute
				'SELECT rdb_get_connected_nodeid_ipc(A.from_node_id, A.to_node_id, B.from_node_id, B.to_node_id) as nodeid
				  From (select * from road_link where objectid = ' || @nCurrentLinkid   || ') as A,
				       (select * from road_link where objectid = ' || @nConnectedLinkid || ') as B';
			fetch curs2 into rec2;
			if not rec2.nodeid is null then
				nConnectedNodeid := rec2.nodeid;
			end if;
			close curs2;

			-- need to reverse?
			bStartSubLink := (nConnectedNodeid = any(rec1.old_s_e_node));
			if bStartSubLink = bConnectedFlag then
				bReverseFlag := True;
			else
				bReverseFlag := False;
			end if;

			-- get target links
			while not rec1.tile_link_id is null LOOP
				if bReverseFlag then
					-- array_prepend (rec1.tile_link_id, arrayTargetLinkid);
					arrayTargetLinkid = rec1.tile_link_id || arrayTargetLinkid;
				else
					-- array_append (arrayTargetLinkid, rec1.tile_link_id);
					arrayTargetLinkid = arrayTargetLinkid || rec1.tile_link_id;
				end if;
				fetch curs1 into rec1;
			END LOOP;
			
		end if;
	end if;
	close curs1;
	
	return arrayTargetLinkid;
END;
$$;


CREATE OR REPLACE FUNCTION rdb_make_tile_nodes_ipc()
  RETURNS integer 
  LANGUAGE plpgsql
  AS $$ 
DECLARE
	curs1 refcursor;
	tile_node_id_64 bigint;
	tile_node_id integer;
	old_node_id bigint;
	temp_tile_id bigint;
	length bigint;
	target record;
	i integer;
	cnt integer;
BEGIN
                                                    
	OPEN curs1 FOR 
	select * from (
		SELECT array_agg(objectid) as id_array, tile_id from (
			select distinct tile_id,objectid from (
				select rdb_geom2tileid(the_geom, 14) as tile_id,objectid
				FROM temp_road_node 
			) a  order by tile_id,objectid 
		) a group by tile_id 
	)a order by tile_id;
	
	tile_node_id := 0;
	cnt          := 0;
    
	-- Get the first record        
	FETCH curs1 INTO target;
	while target is not NULL LOOP
		length         := array_upper(target.id_array, 1);
		temp_tile_id   := target.tile_id;
        
		i := 1;
		while i <= length LOOP
			old_node_id  := cast(target.id_array[i] as bigint);
			tile_node_id := tile_node_id + 1;
			tile_node_id_64 := temp_tile_id;
			tile_node_id_64 := tile_node_id_64 << 32;
			tile_node_id_64 := tile_node_id_64 + (tile_node_id << 4);
            
			INSERT INTO middle_tile_node("tile_node_id", "old_node_id", "tile_id")
				VALUES (tile_node_id_64, old_node_id, temp_tile_id);
			
			i   := i + 1;
			cnt := cnt + 1;
		END LOOP;
		
		-- Reserver 200 nodeid for every tile
		--tile_node_id := tile_node_id + 200;
       	
		-- Get the next record
		FETCH curs1 INTO target;
	END LOOP;
	close curs1;
    
	return cnt;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_make_tile_links_ipc()
  RETURNS integer 
  LANGUAGE plpgsql  
  AS $$ 
DECLARE
	curs1 refcursor;
	seq_num integer;
	cnt integer;
	tile_link_id bigint;
	
	curr_link record;
	next_link record;
BEGIN

	OPEN curs1 FOR	select id, tile_id, start_node_id, end_node_id, old_S_E_node, split_seq_num
			from (
				select A.objectid as id, B.tile_node_id as start_node_id, T.tile_node_id as end_node_id,
					   B.tile_id as tile_id, 
				       array[from_node_id, to_node_id] as old_S_E_node, A.split_seq_num
				from temp_road_link as A 
				left join middle_tile_node as B 
				on cast ( A.from_node_id as bigint) = B.old_node_id
				left join middle_tile_node as T
				on cast ( A.to_node_id as bigint) = T.old_node_id
			) as C 
			order by start_node_id, id, split_seq_num;
	cnt     := 0;
	seq_num := 0;
	-- Get the first record	
	FETCH curs1 INTO curr_link;
	FETCH curs1 INTO next_link;
	
	while curr_link.id is not NULL LOOP
		tile_link_id := curr_link.start_node_id + seq_num;
		IF next_link.id is nulL THEN 
			INSERT INTO middle_tile_link(
					"tile_link_id", "old_link_id", "tile_id", "start_node_id", "end_node_id",
					"old_s_e_node", "split_seq_num", "seq_num")
			    VALUES (tile_link_id, curr_link.id, curr_link.tile_id, 
			    	curr_link.start_node_id, curr_link.end_node_id, 
					curr_link.old_S_E_node, curr_link.split_seq_num, seq_num);
			cnt       := cnt + 1;
			exit;
		END IF;
		
		IF next_link.start_node_id <> curr_link.start_node_id THEN 
			INSERT INTO middle_tile_link(
					"tile_link_id", "old_link_id", "tile_id", "start_node_id", "end_node_id",
					"old_s_e_node", "split_seq_num", "seq_num")
			    VALUES (tile_link_id, curr_link.id, curr_link.tile_id, 
			    	curr_link.start_node_id, curr_link.end_node_id,
					curr_link.old_S_E_node, curr_link.split_seq_num, seq_num);

			cnt       := cnt + 1;	
			seq_num   := 0;
			curr_link := next_link;
		ELSE
			INSERT INTO middle_tile_link(
					"tile_link_id", "old_link_id", "tile_id", "start_node_id",  "end_node_id",
					"old_s_e_node", "split_seq_num", "seq_num")
			    VALUES (tile_link_id, curr_link.id, curr_link.tile_id, 
			    	curr_link.start_node_id, curr_link.end_node_id,
					curr_link.old_S_E_node, curr_link.split_seq_num, seq_num);
			seq_num   := seq_num + 1;
			cnt       := cnt + 1;
			curr_link := next_link;
		END IF;
		 
		-- Get the next record
		FETCH curs1 INTO next_link;
	END LOOP;
	close curs1;

	return cnt;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_geom2tileid(point_geom geometry, z integer)
  RETURNS bigint 
  LANGUAGE plpgsql
AS $$ 
DECLARE
	 PI double precision;
     x double precision;
     y double precision;
     lon double precision;
     lat double precision;
     tile_x bigint;
     tile_y bigint;
     tile_id bigint;
BEGIN
	PI := 3.1415926535897931;
	
	--Convert LatLng to world coordinate
	--This is actually a Google Mercator projection
	if z > 32 then  
		return NULL; 
	end if;

	lon = ST_X(point_geom);
	lat = ST_Y(point_geom);
	x = lon / 360. + 0.5;
	y = ln(tan(PI / 4. + radians(lat) / 2.));
	y = 0.5 - y / 2. / PI;
                                    
	tile_x = cast(floor(x * power(2, z)) as integer);
	tile_y = cast(floor(y * power(2, z)) as integer);
                                            
	tile_id = tile_x << z | tile_y;
                                            
	RETURN tile_id;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_cnv_road_disp_class(roadcls integer) RETURNS smallint
    LANGUAGE plpgsql
    AS $$
    BEGIN
        RETURN CASE
            WHEN roadcls = 1 THEN 42
            WHEN roadcls = 103 THEN 42
            WHEN roadcls = 2 THEN 41
            WHEN roadcls = 104 THEN 41
            WHEN roadcls = 3 THEN 32
            WHEN roadcls = 105 THEN 32
            WHEN roadcls = 4 THEN 31
            WHEN roadcls = 106 THEN 31
            WHEN roadcls = 5 THEN 21
            WHEN roadcls = 107 THEN 21
            WHEN roadcls = 6 THEN 20
            WHEN roadcls = 7 THEN 20
            WHEN roadcls = 108 THEN 20
            WHEN roadcls = 8 THEN 12
            WHEN roadcls = 9 THEN 12
            WHEN roadcls = 302 THEN 11
            WHEN roadcls = 11 THEN 52
            WHEN roadcls = 101 THEN 52
            WHEN roadcls = 102 THEN 53
            WHEN roadcls = 303 THEN 54
            WHEN roadcls = 201 THEN 100
            WHEN roadcls = 202 THEN 100
            WHEN roadcls = 203 THEN 100
            WHEN roadcls = 204 THEN 100
            WHEN roadcls = 304 THEN 102
            ELSE 0
        END;
    END;
$$;


CREATE OR REPLACE FUNCTION rdb_cnv_road_type(roadcls integer, width_c integer, linkcls_c integer, navicls_c integer) RETURNS smallint
    LANGUAGE plpgsql
    AS $$
    BEGIN
	    RETURN CASE
	        WHEN roadcls = 11 THEN 0
	        WHEN roadcls = 101 THEN 0
	        WHEN roadcls = 102 THEN 1
	        WHEN roadcls = 1 THEN 2
	    	WHEN roadcls = 103 THEN 2
	    	WHEN roadcls = 2 THEN 3
	    	WHEN roadcls = 3 THEN 3
	    	WHEN roadcls = 104 THEN 3
	    	WHEN roadcls = 105 THEN 3
	    	WHEN roadcls = 4 THEN 4
	    	WHEN roadcls = 106 THEN 4
	    	WHEN roadcls = 5 THEN 4
	    	WHEN roadcls = 107 THEN 4
	    	WHEN roadcls = 6 and (width_c = 1 or (navicls_c > 0 and navicls_c < 6)) THEN 5
	    	WHEN roadcls = 6 and (width_c <> 1 and (navicls_c = 0 or navicls_c >= 6)) THEN 6
	    	WHEN roadcls = 108 THEN 6
	    	WHEN roadcls = 7 THEN 8
	    	WHEN roadcls = 8 THEN 8
	    	WHEN roadcls = 9 THEN 9
	    	WHEN roadcls = 302 THEN 9
	    	WHEN roadcls = 303 THEN 9
	    	WHEN roadcls = 304 THEN 9
			WHEN roadcls = 402 THEN 9
			WHEN roadcls = 401 THEN -1
	        WHEN roadcls = 201 THEN 10
	        WHEN roadcls = 202 THEN 10
	        WHEN roadcls = 203 THEN 10
	        WHEN roadcls = 204 THEN 10
	        WHEN roadcls = 305 THEN 15
	        WHEN linkcls_c = 9 THEN 9
	        ELSE 8
	    END;
    END;
$$;


CREATE OR REPLACE FUNCTION rdb_cnv_link_type(linkcls integer,roadcls integer) RETURNS smallint
    LANGUAGE plpgsql
    AS $$
    BEGIN
    	if roadcls = 401 then
    		return -1;
    	end if;
	    RETURN CASE
			WHEN linkcls = 0 THEN 1
	        WHEN linkcls = 1 THEN 1
	        WHEN linkcls = 2 THEN 2
	        WHEN linkcls = 3 THEN 3
	        WHEN linkcls = 4 THEN 4
	    	WHEN linkcls = 5 THEN 5
	    	WHEN linkcls = 6 THEN 6
	    	WHEN linkcls = 7 THEN 7
	    	WHEN linkcls = 9 THEN 3
	        ELSE 12
	    END;
    END;
$$;


CREATE OR REPLACE FUNCTION rdb_cnv_link_toll(roadcls integer, special_toll integer) RETURNS smallint
    LANGUAGE plpgsql
    AS $$
    BEGIN
		IF special_toll IS NOT NULL THEN
			RETURN 3;
		END IF;
		
	    RETURN CASE
	        WHEN roadcls = 101 THEN 1
	        WHEN roadcls = 102 THEN 1
	        WHEN roadcls = 103 THEN 1
	        WHEN roadcls = 104 THEN 1
	        WHEN roadcls = 105 THEN 1
	        WHEN roadcls = 106 THEN 1
	        WHEN roadcls = 107 THEN 1
	        WHEN roadcls = 108 THEN 1
	        ELSE 2
	    END;
    END;
$$;


CREATE OR REPLACE FUNCTION mid_linkdir(direction integer) RETURNS smallint
  LANGUAGE plpgsql
  AS $$
  BEGIN
  RETURN CASE
          WHEN Direction = 1 THEN 1
          WHEN Direction = 3 THEN 1
          WHEN Direction = 2 THEN 2
          WHEN Direction = 4 THEN 2
          WHEN Direction = 0 THEN 3
          WHEN Direction = 5 THEN 3
          ELSE 0
      END;
  END;
$$;

CREATE OR REPLACE FUNCTION get_lanecount_downflow(oneway integer, lanecount integer) RETURNS smallint
    LANGUAGE plpgsql
    AS $$
    BEGIN
	    RETURN CASE
	        WHEN oneway = 0 THEN lanecount
	        WHEN oneway = 1 THEN lanecount
	        WHEN oneway = 2 THEN 0
	        WHEN oneway = 3 THEN lanecount
	        WHEN oneway = 4 THEN 0
	        WHEN oneway = 5 THEN lanecount
	        ELSE 5
	    END;
    END;
$$;


CREATE OR REPLACE FUNCTION get_lanecount_upflow(oneway integer, lanecount integer) RETURNS smallint
    LANGUAGE plpgsql
    AS $$
    BEGIN
	    RETURN CASE
	        WHEN oneway = 0 THEN lanecount
	        WHEN oneway = 1 THEN 0
	        WHEN oneway = 2 THEN lanecount
	        WHEN oneway = 3 THEN 0
	        WHEN oneway = 4 THEN lanecount
	        WHEN oneway = 5 THEN lanecount
	        ELSE 5
	    END;
    END;
$$;


CREATE OR REPLACE FUNCTION get_width_downflow(oneway integer, width integer) RETURNS smallint
    LANGUAGE plpgsql
    AS $$
    BEGIN
		RETURN CASE
			WHEN oneway = 0 THEN width
			WHEN oneway = 1 THEN width
			WHEN oneway = 2 THEN 4
			WHEN oneway = 3 THEN width
			WHEN oneway = 4 THEN 4
			WHEN oneway = 5 THEN width
			ELSE 4
		END;
    END;
$$;


CREATE OR REPLACE FUNCTION get_width_upflow(oneway integer, width integer) RETURNS smallint
    LANGUAGE plpgsql
    AS $$
    BEGIN
		RETURN CASE
			WHEN oneway = 0 THEN width
			WHEN oneway = 1 THEN 4
			WHEN oneway = 2 THEN width
			WHEN oneway = 3 THEN 4
			WHEN oneway = 4 THEN width
			WHEN oneway = 5 THEN width
			ELSE 4
		END;
    END;
$$;


CREATE OR REPLACE FUNCTION rdb_linkwidth(width integer) RETURNS integer
  LANGUAGE plpgsql
  AS $$
  BEGIN
    RETURN CASE WHEN Width = 0 THEN 4
        WHEN Width = 1 THEN 3
        WHEN Width = 2 THEN 2
        WHEN Width = 3 THEN 1
        WHEN Width = 4 THEN 0
        ELSE 4
    END;
  END;
$$;


create or replace function rdb_cnv_function_class(navicls int, widearea_f int)
returns smallint
LANGUAGE plpgsql
AS $$
BEGIN

	RETURN CASE
	--8
	WHEN widearea_f = 1 THEN 1
	-- 6
	WHEN navicls = 101 THEN 2
	WHEN navicls = 102 THEN 2
	WHEN navicls = 201 THEN 2
	WHEN navicls = 1 THEN 2
	WHEN navicls = 103 THEN 2
	-- 4
	WHEN navicls = 403 THEN 3
	WHEN navicls = 2 THEN 3
	WHEN navicls = 104 THEN 3
	WHEN navicls = 3 THEN 3
	WHEN navicls = 105 THEN 3
	WHEN navicls = 4 THEN 3
	WHEN navicls = 107 THEN 3
	-- 2
	WHEN navicls = 106 THEN 4
	WHEN navicls = 5 THEN 4
	WHEN navicls = 6 THEN 4
	WHEN navicls = 108 THEN 4
	-- 0
	WHEN navicls = 0 THEN 5
	else 5
	END;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_cnv_name_suffix(_name character varying)
  RETURNS character varying
  LANGUAGE plpgsql 
  AS $$
  DECLARE
  subStr character varying;
begin
	subStr = substring(_name, '[^0-9|£°-£¹]*([0-9|£°-£¹]+)$');
	if subStr is not null then
		return RTRIM(_name,subStr);
	else 
		return _name;
	end if;
END;
$$;
				
CREATE OR REPLACE FUNCTION rdb_make_link_name()
  RETURNS integer 
  LANGUAGE plpgsql
  AS
$$
DECLARE
	curs1 refcursor;
	curr_name_id integer;
	max_name_id integer;
	rec record;
BEGIN

	max_name_id := 0;
	curr_name_id := 0;
	
	OPEN curs1 FOR	SELECT road_code, road_kanji, road_yomi from road_code_list;
					

	FETCH curs1 INTO rec;	  
	while rec.road_code is not NULL LOOP
		curr_name_id := curr_name_id + 1;
		
		if (rec.road_kanji is not null) then
			INSERT INTO temp_link_name_dictionary_tbl(
				    name_id, name, py, seq_nm, language, road_code)
			    VALUES (curr_name_id, rec.road_kanji, rec.road_yomi, 1, 5, rec.road_code);

		end if;
		
		FETCH curs1 INTO rec;
	END LOOP;
	close curs1;

	insert into name_dictionary_tbl (name_id, name, py, seq_nm, language)
		select name_id, name, py, seq_nm, language from temp_link_name_dictionary_tbl;

	return curr_name_id - max_name_id;
END;
$$;


CREATE OR REPLACE FUNCTION rdb_make_link_no()
  RETURNS integer 
  LANGUAGE plpgsql
  AS
$BODY$
DECLARE
	curs1 refcursor;
	curr_name_id integer;
	max_name_id integer;
	rec record;
	old_roadno integer;
BEGIN

	select case when max(id) is null then 0 else max(id) end into max_name_id from 
	(
		select max(name_id) as id from name_dictionary_tbl
	) as T;
	curr_name_id := max_name_id;
	
	OPEN curs1 FOR	SELECT objectid, roadno from temp_road_rlk where roadno != 0 order by roadno;
					
	old_roadno := 0;
	FETCH curs1 INTO rec;	  
	while rec.objectid is not NULL LOOP
		if rec.roadno != old_roadno then
			curr_name_id := curr_name_id + 1;
			old_roadno := rec.roadno;
		end if;

		INSERT INTO temp_link_no_dictionary_tbl(
			name_id, name, py, seq_nm, language, objectid)
		VALUES (curr_name_id, cast(rec.roadno as character varying), null, 1, 5, rec.objectid);
		
		FETCH curs1 INTO rec;
	END LOOP;
	close curs1;

	insert into name_dictionary_tbl (name_id, name, py, seq_nm, language)
		select distinct name_id, name, py, seq_nm, language from temp_link_no_dictionary_tbl;

	return curr_name_id - max_name_id;
END;
$BODY$;

CREATE OR REPLACE FUNCTION rdb_cnv_oneway_code(roadclass_c integer, naviclass_c integer, oneway_c integer, nopass_c integer)
  RETURNS smallint 
  LANGUAGE plpgsql
  AS
$$
BEGIN
		IF roadclass_c IN (202,203,204) AND naviclass_c = 0 THEN
			RETURN 4;
		END IF;
		
	    RETURN CASE
	    	when nopass_c in (1,3,4) then 4
	        WHEN oneway_c = 0 THEN 1
	        WHEN oneway_c = 1 THEN 2
	        WHEN oneway_c = 2 THEN 3
	        WHEN oneway_c = 3 THEN 1
	        WHEN oneway_c = 4 THEN 1
	        WHEN oneway_c = 5 THEN 1
	        ELSE 1
	    END;
END;
$$;


CREATE OR REPLACE FUNCTION rdb_cnv_oneway_condition(oneway_c integer)
  RETURNS smallint 
  LANGUAGE plpgsql
  AS
$BODY$

BEGIN
	    RETURN CASE
	        WHEN oneway_c = 0 THEN 0
	        WHEN oneway_c = 1 THEN 0
	        WHEN oneway_c = 2 THEN 0
	        WHEN oneway_c = 3 THEN 1
	        WHEN oneway_c = 4 THEN 1
	        WHEN oneway_c = 5 THEN 1
	        ELSE 0
	    END;

END;
$BODY$;


CREATE OR REPLACE FUNCTION rdb_cnv_pass_code(nopass_c integer)
  RETURNS smallint 
  LANGUAGE plpgsql
  AS
$BODY$

BEGIN
	    RETURN CASE
	        WHEN nopass_c = 0 THEN 0
	        WHEN nopass_c = 1 THEN 0
	        WHEN nopass_c = 2 THEN 0
	        WHEN nopass_c = 3 THEN 1
	        WHEN nopass_c = 4 THEN 2
	        WHEN nopass_c = 5 THEN 2
	        ELSE 1
	    END;

END;
$BODY$;


CREATE OR REPLACE FUNCTION rdb_cnv_pass_condition(nopass_c integer)
  RETURNS smallint 
  LANGUAGE plpgsql
  AS
$BODY$

BEGIN
	    RETURN CASE
	        WHEN nopass_c = 0 THEN 0
	        WHEN nopass_c = 1 THEN 0
	        WHEN nopass_c = 2 THEN 1
	        WHEN nopass_c = 3 THEN 0
	        WHEN nopass_c = 4 THEN 0
	        WHEN nopass_c = 5 THEN 1
	        ELSE 0
	    END;

END;
$BODY$;


CREATE OR REPLACE FUNCTION rdb_cnv_up_down_distinguish(oneway_c integer)
  RETURNS smallint 
  LANGUAGE plpgsql
  AS
$BODY$

BEGIN
	    RETURN CASE
	        WHEN oneway_c = 1 THEN 1
	        WHEN oneway_c = 2 THEN 1
	        WHEN oneway_c = 3 THEN 1
	        WHEN oneway_c = 4 THEN 1
	        ELSE 0
	    END;

END;
$BODY$;


CREATE OR REPLACE FUNCTION rdb_cnv_display_class(roadcls integer)
  RETURNS smallint AS
$BODY$
    BEGIN
        RETURN CASE
            WHEN roadcls = 1 THEN 9  --42
            WHEN roadcls = 103 THEN 10  --51
            WHEN roadcls = 2 THEN 8  --41
            WHEN roadcls = 104 THEN 10  --51
            WHEN roadcls = 3 THEN 8  --41
            WHEN roadcls = 105 THEN 10  --51
            WHEN roadcls = 4 THEN 7  --32
            WHEN roadcls = 106 THEN 10  --51
            WHEN roadcls = 5 THEN 6  --31
            WHEN roadcls = 107 THEN 10  --51
            WHEN roadcls = 6 THEN 5  --21
            WHEN roadcls = 7 THEN 5  --21
            WHEN roadcls = 108 THEN 10  --51
            WHEN roadcls = 8 THEN 3  --20
            WHEN roadcls = 9 THEN 18  --19
            WHEN roadcls = 302 THEN 1  --15
            WHEN roadcls = 11 THEN 12  --53
            WHEN roadcls = 101 THEN 12  --53
            WHEN roadcls = 102 THEN 11  --52
            WHEN roadcls = 303 THEN 1  --15
            WHEN roadcls = 201 THEN 14  --100
            WHEN roadcls = 202 THEN 14  --100
            WHEN roadcls = 203 THEN 14  --100
            WHEN roadcls = 204 THEN 14  --100
            WHEN roadcls = 304 THEN 1  --15
            WHEN roadcls = 305 THEN 1  --15
			WHEN roadcls = 402 THEN 1  --15
			WHEN roadcls = 401 THEN 20
            ELSE 0
        END;
    END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
ALTER FUNCTION rdb_cnv_display_class(roadcls integer) OWNER TO postgres;




CREATE OR REPLACE FUNCTION deal_type_of_sidefile(tablename varchar) RETURNS smallint
  LANGUAGE plpgsql
AS $$
DECLARE
	cur refcursor;
	rec record;
	i int;
	name varchar[];
	id int[];
	type_value varchar;
	gid_value int;
BEGIN 
	i := 0;
	
	open cur for execute

		'SELECT gid,type FROM ' || quote_ident(tablename) || ' where type is not null order by gid';
	fetch cur into rec;
	
	while rec.gid is not null loop

		raise info 'rec.gid:%',rec.gid;
		if i <> 0 then
		
			type_value := name[i - 1];
			gid_value := id[i - 1];

			execute 'update ' || quote_ident(tablename) || ' set "type" = ' || chr(39) ||  type_value || chr(39) || ' where gid < ' || rec.gid || ' and gid > ' || gid_value;

		end if;
		
		name[i] := rec.type;
		id[i] := rec.gid;		
		i := i + 1;
		
		fetch cur into rec;
	end loop;

	close cur;

	execute 'update ' || quote_ident(tablename) || ' set "type" = ' || chr(39) || name[i - 1] || chr(39) || ' where gid > ' || id[i - 1];	
	
    return 1;
END;
$$;


CREATE OR REPLACE FUNCTION create_geom_of_sidefile(meshcode int, x int, y int) RETURNS geometry
  LANGUAGE plpgsql
AS $$
DECLARE
	unitx float;
	unity float;
	Px smallint;
	Py smallint;
	Pnum smallint;
	col smallint;
	row smallint;
	lon float;
	lat float;
BEGIN 
        unitx := 1.0/8.0;
        unity := 40.0/60.0/8.0;

        Py := round(meshcode/10000);  
        Px := round((meshcode - py * 10000)/100);  
        Pnum := meshcode - Py * 10000 - Px * 100;  
        
        col := round(Pnum / 10); 
        row := Pnum - col * 10; 

	Py := Py * 8 + col;
	Px := Px * 8 + row;
	
        lon := unitx*Px + unitx/1000000.0 * x + 100; 
        lat := unity*Py + unity/1000000.0 * y; 

	return  st_transform(ST_SetSRID(ST_Point(lon, lat),4301),4326);
	
END;
$$;

----------------------------------------------------------------------------------------
-- Dictionary for Towand Name and SignPost
----------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION rdb_insert_name_dictionary_towardname_jdb()
    RETURNS integer
    LANGUAGE plpgsql
    AS $$ 
DECLARE
	curs1        refcursor;
	i            integer;    
	curr_name_id integer;
	gid_array    integer[];
	_name        character varying;
	yomi_name    character varying;
	cnt          integer;
BEGIN
	curr_name_id := 0;
	cnt          := 0;

	OPEN curs1 FOR  select gid_array2, name_kanji, name_yomi
					from (
						SELECT array_agg(gid) as gid_array2, name_kanji, name_yomi
						  from road_dir
						  where mid_isNotAllDigit(name_kanji) = true
						  group by name_kanji, name_yomi
					) as a
					order by name_kanji, name_yomi;
		
    
	-- Get the first record        
	FETCH curs1 INTO gid_array, _name, yomi_name;
	while gid_array is not null LOOP
		-- rdb_add_one_name(n_id, _name, yomi, seq_nm, lang_type)
		curr_name_id := rdb_add_one_name(curr_name_id, _name, yomi_name, 5);

		i := 1;
		while i <= array_upper(gid_array, 1) loop 
			insert into temp_toward_name values(gid_array[i], curr_name_id);
			i := i + 1;
		end loop;
		
		curr_name_id := curr_name_id + 1;
		cnt          := cnt + array_upper(gid_array, 1);
		-- Get the next record
		FETCH curs1 INTO gid_array, _name, yomi_name;
	END LOOP;
	close curs1;

    return cnt;
END;
$$;

-------------------------------------------------------------------------------------------------------------
-- add name to Dictionary
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION rdb_add_one_name(n_id integer, _name character varying, yomi character varying, lang_type integer)
  RETURNS integer 
  LANGUAGE plpgsql 
  AS $BODY$
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
		values(n_id, _name, yomi, lang_type);
            
	return n_id;
END;
$BODY$;

-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION rdb_insert_name_dictionary(from_tbl character varying, id_col character varying, name_col character varying, 
						     yomi_col character varying, temp_tbl character varying, condition character varying)
    RETURNS integer
    LANGUAGE plpgsql
    AS $$ 
DECLARE
    curs1 refcursor;
    curr_name_id integer;
    id integer;
    _name character varying;
    yomi_name character varying;
    cnt integer;
BEGIN
	curr_name_id := 0;
	cnt          := 0;

	IF yomi_col IS NOT NULL AND yomi_col <> '' THEN
		IF condition IS NOT NULL OR condition <> '' THEN
			OPEN curs1 FOR execute 'SELECT ' || id_col || ',' || name_col ||',' || yomi_col || ' from ' || ($1) ||
			         ' WHERE ' || condition || ' order by ' || name_col ||',' || yomi_col;
		ELSE
			OPEN curs1 FOR execute 'SELECT ' || id_col || ',' || name_col ||',' || yomi_col || ' from ' || ($1) || 
			         ' order by ' || name_col ||',' || yomi_col;
		END IF;
	ELSE
		IF condition IS NOT NULL OR condition <> '' THEN
			OPEN curs1 FOR execute 'SELECT ' || id_col || ', CAST(' || name_col ||' AS CHARACTER VARYING) from ' || ($1)||
			          ' WHERE ' || condition || ' order by ' || name_col;
		ELSE
			OPEN curs1 FOR execute 'SELECT ' || id_col || ', CAST(' || name_col ||' AS CHARACTER VARYING) from ' || ($1) || 
			          ' order by ' || name_col;
		END IF;
	END IF;
    
	-- Get the first record        
	FETCH curs1 INTO id, _name, yomi_name;
	while id is not null LOOP
		-- rdb_add_one_name(n_id, _name, yomi, seq_nm, lang_type)
		curr_name_id := rdb_add_one_name(curr_name_id, _name, yomi_name, 5);
		
		EXECUTE 'INSERT INTO ' || temp_tbl || ' VALUES (' || id || ', ' || curr_name_id || ')';
		curr_name_id := curr_name_id + 1;
		cnt          := cnt + 1;
		-- Get the next record
		FETCH curs1 INTO id, _name, yomi_name;
	END LOOP;
	close curs1;

    return cnt;
END;
$$;

-------------------------------------------------------------------------------------------------------------
-- Get connect link id
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION rdb_get_connected_linkid_ipc(tolinkid integer, midlinkid character varying)
  RETURNS integer 
  LANGUAGE plpgsql
  AS $$ 
DECLARE
	midlinkid_array character varying[];
BEGIN
	IF midlinkid is null or midlinkid = '' THEN
		return tolinkid;
	END IF;
	
	midlinkid_array = regexp_split_to_array(midlinkid, E'\\,+');

	return cast(midlinkid_array[1] as integer);
END;
$$;

-------------------------------------------------------------------------------------------------------------
-- Get connect from link id
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION rdb_get_connected_from_linkid_ipc(fromlinkid integer, midlinkid character varying)
  RETURNS integer 
  LANGUAGE plpgsql
  AS $$ 
DECLARE
	midlinkid_array character varying[];
BEGIN
	IF midlinkid is null or midlinkid = '' THEN
		return fromlinkid;
	END IF;
	
	midlinkid_array = regexp_split_to_array(midlinkid, E'\\,+');

	return cast(midlinkid_array[array_upper(midlinkid_array, 1)] as integer);
END;
$$;

-------------------------------------------------------------------------------------------------------------
-- Get connect node id
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION rdb_get_connected_nodeid_ipc(fromnodeid1 integer, tonodeid1 integer, fromnodeid2 integer, tonodeid2 integer)
  RETURNS integer 
  LANGUAGE plpgsql
  AS $$ 
DECLARE
	midlinkid_array character varying[];
BEGIN
	IF fromnodeid1 in (fromnodeid2,tonodeid2) THEN
		return fromnodeid1;
	END IF;
	
	IF tonodeid1 in (fromnodeid2,tonodeid2) THEN
		return tonodeid1;
	END IF;
	
	return NULL;
END;
$$;

-------------------------------------------------------------------------------------------------------------
-- Get Split Link ID / IDArray
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_getSplitLinkID(nCurrentLinkid integer, nConnectedLinkid integer, bConnectedFlag boolean)
  RETURNS bigint 
  LANGUAGE plpgsql
  AS $$ 
DECLARE
	curs1 refcursor;
	curs2 refcursor;
	rec1 record;
	rec2 record;
	nTargetLinkid bigint;
	nConnectedNodeid integer;
	bStartSubLink boolean;
BEGIN
	nTargetLinkid := null;
	
	open curs1 for execute
		'select * from middle_tile_link where old_link_id = ' || (@nCurrentLinkid) || 'order by split_seq_num asc';
	
	fetch curs1 into rec1;
	if rec1.tile_link_id is not null then
		if rec1.split_seq_num = -1 then
			nTargetLinkid := rec1.tile_link_id;
		else
			-- get connected nodeid
			nConnectedNodeid := null;
			open curs2 for execute
				'SELECT rdb_get_connected_nodeid_ipc(A.from_node_id, A.to_node_id, B.from_node_id, B.to_node_id) as nodeid
				  From (select * from road_link_4326 where objectid = ' || @nCurrentLinkid   || ') as A,
				       (select * from road_link_4326 where objectid = ' || @nConnectedLinkid || ') as B';
			
			fetch curs2 into rec2;
			if not rec2.nodeid is null then
				nConnectedNodeid := rec2.nodeid;
			end if;
			close curs2;
			
			-- get target link
			bStartSubLink := (nConnectedNodeid = any(rec1.old_s_e_node));
			if bStartSubLink = bConnectedFlag then
				nTargetLinkid := rec1.tile_link_id;
			else
				while not rec1.tile_link_id is null LOOP
					nTargetLinkid := rec1.tile_link_id;
					fetch curs1 into rec1;
				END LOOP;
			end if;
		end if;
	end if;
	close curs1;
	
	return nTargetLinkid;
END;
$$;


-----------------------------------------------------------------------------------------
-- get split links between tow links
-----------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_getSplitLinkIDArray(nCurrentLinkid integer, nConnectedLinkid integer, bConnectedFlag boolean)
    RETURNS bigint[] 
    LANGUAGE plpgsql
AS $$
DECLARE
	curs1 refcursor;
	curs2 refcursor;
	rec1 record;
	rec2 record;
	arrayTargetLinkid bigint[];
	nConnectedNodeid integer;
	bStartSubLink boolean;
	bReverseFlag boolean;
BEGIN
	open curs1 for execute
		'select * from middle_tile_link where old_link_id = ' || (@nCurrentLinkid) || 'order by split_seq_num asc';
	
	fetch curs1 into rec1;
	if not rec1.tile_link_id is null then
		if rec1.split_seq_num = -1 then
			arrayTargetLinkid[1] := rec1.tile_link_id;
		else
			-- get connected nodeid
			nConnectedNodeid := null;
			open curs2 for execute
				'SELECT rdb_get_connected_nodeid_ipc(A.from_node_id, A.to_node_id, B.from_node_id, B.to_node_id) as nodeid
				  From (select * from road_link_4326 where objectid = ' || @nCurrentLinkid   || ') as A,
				       (select * from road_link_4326 where objectid = ' || @nConnectedLinkid || ') as B';
			fetch curs2 into rec2;
			if not rec2.nodeid is null then
				nConnectedNodeid := rec2.nodeid;
			end if;
			close curs2;

			-- need to reverse?
			bStartSubLink := (nConnectedNodeid = any(rec1.old_s_e_node));
			if bStartSubLink = bConnectedFlag then
				bReverseFlag := True;
			else
				bReverseFlag := False;
			end if;

			-- get target links
			while not rec1.tile_link_id is null LOOP
				if bReverseFlag then
					-- array_prepend (rec1.tile_link_id, arrayTargetLinkid);
					arrayTargetLinkid = rec1.tile_link_id || arrayTargetLinkid;
				else
					-- array_append (arrayTargetLinkid, rec1.tile_link_id);
					arrayTargetLinkid = arrayTargetLinkid || rec1.tile_link_id;
				end if;
				fetch curs1 into rec1;
			END LOOP;
			
		end if;
	end if;
	close curs1;
	
	return arrayTargetLinkid;
END;
$$;

CREATE OR REPLACE FUNCTION mid_getConnectionNodeID(nCurrentLinkid bigint, nConnectedLinkid bigint, bConnectedFlag boolean)
-- input rdb link id, get rdb node id
    RETURNS bigint
    LANGUAGE plpgsql
AS $$
DECLARE
	cur_start_node_id	bigint;
	cur_end_node_id		bigint;
	con_start_node_id	bigint;
	con_end_node_id		bigint;
	target_node_id		bigint;
BEGIN
	target_node_id := null;
	
	execute
		'
		 select start_node_id, end_node_id
		 from middle_tile_link
		 where tile_link_id = ' || (@nCurrentLinkid) || '
		'
		into cur_start_node_id, cur_end_node_id;
	
	execute
		'
		 select start_node_id, end_node_id
		 from middle_tile_link
		 where tile_link_id = ' || (@nConnectedLinkid) || '
		'
		into con_start_node_id, con_end_node_id;
	
	if bConnectedFlag then
		target_node_id := case when cur_start_node_id in (con_start_node_id, con_end_node_id) then cur_start_node_id else cur_end_node_id end;
	else
		target_node_id := case when cur_start_node_id in (con_start_node_id, con_end_node_id) then cur_end_node_id else cur_start_node_id end;
	end if;
	
	return target_node_id;
END;
$$;

-------------------------------------------------------------------------------------------------------------
-- Get new link id for pass link
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_get_new_passlid(old_passlid character varying, out_linkid integer, split_char character varying)
	-- get new passlid string of rdb link id.
    RETURNS character varying
    LANGUAGE plpgsql
AS $$
DECLARE
	old_passlid_array character varying[];
	new_passlids character varying;
	old_num integer;
	new_num integer;
	i integer;
	j integer;
	new_link_ids bigint[];
BEGIN
	IF old_passlid IS NULL OR old_passlid = '' THEN 
		RETURN NULL;
	END IF;

	old_passlid_array := regexp_split_to_array(old_passlid, E'\\'|| split_char ||'+');
	old_num      := array_upper(old_passlid_array, 1);
	i            := 1;
	new_num      := 0;
	new_passlids := '';
	
	WHILE i < old_num LOOP
		new_link_ids = mid_getSplitLinkIDArray(old_passlid_array[i]::integer, old_passlid_array[i+1]::integer, true);
		j := 1;
		while j <= array_upper(new_link_ids, 1) loop
			if new_num = 0 then 
				new_passlids := cast(new_link_ids[j] as character varying);
			else
				new_passlids := new_passlids || '|' || cast(new_link_ids[j] as character varying);
			end if;
			new_num := new_num + 1;
			j := j + 1;
		END LOOP;

		i := i + 1;
	END LOOP;

	-- last pass link
	new_link_ids = mid_getSplitLinkIDArray(old_passlid_array[i]::integer, out_linkid, true);
	j := 1;
	while j <= array_upper(new_link_ids, 1) loop
		if new_num = 0 then 
			new_passlids := cast(new_link_ids[j] as character varying);
		else
			new_passlids := new_passlids || '|' || cast(new_link_ids[j] as character varying);
		end if;
		new_num := new_num + 1;
		j := j + 1;
	END LOOP;
	
	return new_passlids;
END;
$$;

-------------------------------------------------------------------------------------------------------------
-- Get full path for highway node [Out]
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_find_highway_path_out_jdb(nformnodeid bigint, facilclass_array integer[])
  RETURNS character varying[] 
  LANGUAGE plpgsql VOLATILE
  AS $$ 
DECLARE
	rstPathArray		varchar[];
	rstPathCount		integer;
	rstPath			varchar;
	
	tmpPathArray		varchar[];
	tmpLastNodeArray	bigint[];
	tmpPathCount		integer;
	tmpPathIndex		integer;
	tmplastLinkArray	bigint[];
	repeatCountArray        integer[];
	tmplastLinkTypeArray    smallint[];
	
	nStartNode		integer;
	nEndNode		integer;
	
	nNewFromLink		bigint;
	nNewToLink		bigint;
	
	rec			record;
	i			integer;
BEGIN
	if nFormNodeID is null then 
		return null;
	end if;
	
	-- init
	nStartNode           := nFormNodeID;
	rstPathCount         := 0;
	tmpPathIndex         := 1;
	tmpPathCount         := 0;

	-- Get Start Links
	tmpPathArray  := mid_get_in_link_jdb(nFormNodeID);
	
	if tmpPathArray is null or array_upper(tmpPathArray, 1) = 0 then 
		raise exception 'No In links of node: %', nStartNode;
		return null;
	else
		i := 1;
		while i <= array_upper(tmpPathArray, 1) loop
			if tmpPathCount = 0 then
				tmpLastNodeArray := array[nStartNode::bigint];
				tmplastLinkArray := array[tmpPathArray[i]::bigint];
				repeatCountArray := array[0];
				tmplastLinkTypeArray := array[-1];
			else
                                raise info 'In link number > 1, node: %', nStartNode;
				tmpLastNodeArray := tmpLastNodeArray || array[nStartNode::bigint];
				tmplastLinkArray := tmplastLinkArray || tmpPathArray[i]::bigint;
				repeatCountArray := repeatCountArray || 0;
				tmplastLinkTypeArray := tmplastLinkTypeArray || 0::smallint;
			end if;
			tmpPathCount := tmpPathCount + 1;
			i := i + 1;
		end loop;
	end if;

	-- search 
	WHILE tmpPathIndex <= tmpPathCount LOOP
                -- JCT
		if (3 = any (facilclass_array) and
                    array_upper(regexp_split_to_array(tmpPathArray[tmpPathIndex], E'\\|+'), 1) > 1) then
                        -- main road
                        if 2 = tmplastLinkTypeArray[tmpPathIndex] then
                                rstPathCount  := rstPathCount + 1;
                                rstPath	      := cast(tmpPathArray[tmpPathIndex] as varchar);
                                rstPath	      := replace(rstPath, '(2)', '');
                                rstPath       := replace(rstPath, '(3)', '');
                                rstPath	      := rstPath || '|' || tmpLastNodeArray[tmpPathIndex]::varchar;
                                if rstPathCount = 1 then
                                        rstPathArray	:= ARRAY[rstPath];
                                else
                                        rstPathArray	:= array_append(rstPathArray, rstPath);
                                end if;
                                tmpPathIndex := tmpPathIndex + 1;
                                continue;
                        end if;

                        -- Out the hwy
                        if mid_check_hwy_exit_node(tmpLastNodeArray[tmpPathIndex], tmplastLinkArray[tmpPathIndex]) then
                                tmpPathIndex := tmpPathIndex + 1;
                                continue;
                        end if;	
		end if;
		
		-- JCT  ==> JCT Stop  : facilclass_array[3]
		-- SAPA/IC ==> SAPA/IC Stop : facilclass_array[1, 2, 4, 5, 7]
		if (array_upper(regexp_split_to_array(tmpPathArray[tmpPathIndex], E'\\|+'), 1) > 1
			and mid_check_hwy_node_type_jdb(tmpLastNodeArray[tmpPathIndex], facilclass_array))  then
			--and mid_is_hwy_node_jdb(tmpLastNodeArray[tmpPathIndex])) then
			rstPathCount			:= rstPathCount + 1;
			rstPath				:= cast(tmpPathArray[tmpPathIndex] as varchar);
			rstPath				:= replace(rstPath, '(2)', '');
			rstPath				:= replace(rstPath, '(3)', '');
			rstPath				:= rstPath || '|' || tmpLastNodeArray[tmpPathIndex]::varchar;
			if rstPathCount = 1 then
				rstPathArray	:= ARRAY[rstPath];
			else
				rstPathArray	:= array_append(rstPathArray, rstPath);
			end if;
			
		elseif array_upper(regexp_split_to_array(tmpPathArray[tmpPathIndex], E'\\|+'), 1) >= 18 then  -- It is longer then 18.
			raise info 'It is longer then 18: %', nStartNode;
			tmpPathIndex := tmpPathIndex + 1;
			continue;
		else
			for rec in execute
				'
				 select  nextroad, a.dir, a.nextnode, link_type
				 from
				 (
					 select objectid as nextroad, ''(2)'' as dir, to_node_id as nextnode,
                                                rdb_cnv_link_type(linkclass_c,roadclass_c) as link_type
					 from road_link_4326
					 where	(from_node_id = ' || tmpLastNodeArray[tmpPathIndex]|| ') and 
							(rdb_cnv_oneway_code(roadclass_c, naviclass_c, oneway_c,nopassage_c) in (1,2)) and 
							(rdb_cnv_road_type(roadclass_c, roadwidth_c, linkclass_c, naviclass_c) in (0, 1) or 
							 rdb_cnv_link_type(linkclass_c,roadclass_c) in (3, 5, 7) or
							 rdb_cnv_link_toll(roadclass_c,null) = 1)
					 union
					 
					 select objectid as nextroad, ''(3)'' as dir, from_node_id as nextnode,
                                                rdb_cnv_link_type(linkclass_c,roadclass_c) as link_type
					 from road_link_4326
					 where	(to_node_id = ' || tmpLastNodeArray[tmpPathIndex]|| ') and
							(rdb_cnv_oneway_code(roadclass_c, naviclass_c, oneway_c,nopassage_c) in (1,3)) and 
							(rdb_cnv_road_type(roadclass_c, roadwidth_c, linkclass_c, naviclass_c) in (0, 1) or 
							 rdb_cnv_link_type(linkclass_c,roadclass_c) in (3, 5, 7) or
							 rdb_cnv_link_toll(roadclass_c,null) = 1)
				 )as a
				'
			loop
				
				if not ((rec.nextroad||rec.dir) = ANY(regexp_split_to_array(tmpPathArray[tmpPathIndex], E'\\|+'))) then
					tmpPathCount		:= tmpPathCount + 1;
					tmpPathArray		:= array_append(tmpPathArray, cast(tmpPathArray[tmpPathIndex]||'|'||rec.nextroad||rec.dir as varchar));
					tmpLastNodeArray	:= array_append(tmpLastNodeArray, cast(rec.nextnode as bigint));
					tmplastLinkArray        := array_append(tmplastLinkArray, cast(rec.nextroad as bigint));
					repeatCountArray        := array_append(repeatCountArray, repeatCountArray[tmpPathIndex]);
					tmplastLinkTypeArray    := array_append(tmplastLinkTypeArray, rec.link_type);
				elseif 0 = repeatCountArray[tmpPathIndex] then
					tmpPathCount		:= tmpPathCount + 1;
					tmpPathArray		:= array_append(tmpPathArray, cast(tmpPathArray[tmpPathIndex]||'|'||rec.nextroad||rec.dir as varchar));
					tmpLastNodeArray	:= array_append(tmpLastNodeArray, cast(rec.nextnode as bigint));
					tmplastLinkArray        := array_append(tmplastLinkArray, cast(rec.nextroad as bigint));
					repeatCountArray        := array_append(repeatCountArray, repeatCountArray[tmpPathIndex] + 1);
					tmplastLinkTypeArray    := array_append(tmplastLinkTypeArray, rec.link_type);
				else
					-- do nothing
					NULL;
				end if;
			end loop;
		end if;
		tmpPathIndex := tmpPathIndex + 1;
	END LOOP;

	return rstPathArray;
END;
$$;

-----------------------------------------------------------------------------------------
-- High Exit.
-----------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_check_hwy_exit_node(nnodeid bigint, nlink_id bigint)
  RETURNS boolean 
  LANGUAGE plpgsql AS
$$ 
DECLARE
BEGIN
	if nNodeID is null or nlink_id is null then
		return false;
	end if;
	
	PERFORM  objectid
	  from road_link_4326
	  where ((from_node_id = nNodeID and (rdb_cnv_oneway_code(roadclass_c, naviclass_c, oneway_c,nopassage_c) in (1,2)) ) 
	         or (to_node_id = nNodeID and (rdb_cnv_oneway_code(roadclass_c, naviclass_c, oneway_c,nopassage_c) in (1,3))))
	         and objectid <> nlink_id and 
	        (rdb_cnv_road_type(roadclass_c, roadwidth_c, linkclass_c, naviclass_c) not in (0, 1) and 
	         rdb_cnv_link_type(linkclass_c,roadclass_c) not in (3,5,7) and 
	         rdb_cnv_link_toll(roadclass_c,null) <> 1);

	if not found then
		return False;
	end if;

	return true;
END;
$$;
-------------------------------------------------------------------------------------------------------------
-- Get in link of a highway node [Out]
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_get_in_link_jdb(nnodeid bigint)
  RETURNS character varying[]
  LANGUAGE plpgsql
AS $$ 
DECLARE
	startLinkArray   varchar[];
	nlinkCount       integer;
	rec              record;
BEGIN
	nlinkCount     := 0;
	startLinkArray := NULL;
	for rec in execute ' 
			SELECT objectid as link_id 
			   from road_link_4326
				where ((from_node_id = ' || nNodeId || ' and rdb_cnv_oneway_code(roadclass_c, naviclass_c, oneway_c,nopassage_c) in (1, 3)) 
				 or (to_node_id = ' || nNodeId || ' and rdb_cnv_oneway_code(roadclass_c, naviclass_c, oneway_c,nopassage_c) in (1, 2))) 
				 and rdb_cnv_link_type(linkclass_c,roadclass_c) = 2
			'
	loop
		if nlinkCount = 0 then 
			startLinkArray   := ARRAY[rec.link_id::varchar];
		else
			startLinkArray   := startLinkArray || ARRAY[rec.link_id::varchar];
		end if;
		nlinkCount := nlinkCount + 1;
	end loop;
	
	return startLinkArray;
END;
$$;

-------------------------------------------------------------------------------------------------------------
-- check if a node is highway facility node (not a toll node)
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_is_hwy_node_jdb(nnodeid bigint)
  RETURNS boolean
  language plpgsql
AS
$$ 
DECLARE
	startLinkArray   varchar[];
	nlinkCount       integer;
	rec              record;
BEGIN
	PERFORM  node_id
	  FROM highway_node_add_nodeid
	  where  node_id  = nNodeId and tollclass_c in (0, 4);
	if found then
		return TRUE;
	end if;
	return FALSE;
END;
$$;

-------------------------------------------------------------------------------------------------------------
-- pass link exists SAPA link
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_get_sapa_link_index(full_pass_link character varying[])
  RETURNS integer
  language plpgsql
AS
$$ 
DECLARE
	startLinkArray   varchar[];
	nlinkCount       integer;
BEGIN
	nlinkCount := 1;

	while nlinkCount <= array_upper(full_pass_link, 1) loop
		PERFORM *
		  from road_link_4326
		  where objectid = full_pass_link[nlinkCount]::bigint and rdb_cnv_link_type(linkclass_c,roadclass_c) in (7);
		  
		if found then
			return nlinkCount;
		end if;
		nlinkCount := nlinkCount + 1;
	end loop;
	
	return 0;
END;
$$;

-------------------------------------------------------------------------------------------------------------
-- find path for IC/Ramp/SmartIC enter
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_findpath_hwy_enter_jdb(nEnterNodeId bigint)
  RETURNS character varying[] 
  LANGUAGE plpgsql AS
$$ 
DECLARE
	rstPathArray            varchar[];
	rstPathCount            integer;
	rstPath                 varchar;
	
	tmpPathArray            varchar[];
	tmpLastNodeArray        bigint[];
	tmpPathCount            integer;
	tmpPathIndex            integer;
	tmplastLinkArray        bigint[];
	tmpLastLinkTypeArray	integer[][3];
	tmpHwyEnterNodeArray    integer[];
	
	tmpNextLinkArray        bigint[];
	tmpNextNodeArray        bigint[];
	tmpNextLinkTypeArray    integer[][3];
	nNewFromLink            bigint;
	nNewToLink              bigint;
	tmpLength               integer;
	isEnterNode             boolean;
	isEnterLink             boolean;
	isPassedJCTRampLink     boolean[];
	isForSmartIC            boolean;
	i                       integer;
	j                       integer;
	tmpArray                integer[3];
	rec                     record;
BEGIN
	if nEnterNodeId is null then 
		return null;
	end if;
	
	--rstPathArray
	rstPathCount            := 0;
	tmpPathArray            := array['0'];
	tmpLastNodeArray        := ARRAY[nEnterNodeId];
	tmplastLinkArray        := array[0];
	tmpLastLinkTypeArray    := array[[-1, -1, -1]];
	tmpHwyEnterNodeArray    := array[NULL];
	isPassedJCTRampLink     := array[false];
	tmpPathCount            := 1;
	tmpPathIndex            := 1;
	tmpLength               := 0;
	isForSmartIC            := false;

	-- search
	WHILE tmpPathIndex <= tmpPathCount LOOP
		
		-- 4: Ramp; 5: IC; 7: SmartIC
		if (tmpPathIndex > 1 and mid_check_hwy_node_type_jdb(tmpLastNodeArray[tmpPathIndex], array[4, 5, 7])) then 
			rstPathCount			:= rstPathCount + 1;
			rstPath				:= cast(tmpPathArray[tmpPathIndex] as varchar);
			rstPath				:= replace(rstPath, '(2)', '');
			rstPath				:= replace(rstPath, '(3)', '');
			rstPath				:= rstPath || '|' || tmpLastNodeArray[tmpPathIndex]::varchar;
			if rstPathCount = 1 then
				-- append enter node id at the end
				rstPathArray := ARRAY[rstPath]; 
			else
				-- append enter node id at the end
				rstPathArray := array_append(rstPathArray, rstPath); 
			end if;
			
			tmpPathIndex := tmpPathIndex + 1;
			continue;
		end if;
		
		--raise INFO 'tmpLastNodeArray------- %', cast(tmpPathArray[tmpPathIndex] as varchar);	
		tmpLength  = array_upper(regexp_split_to_array(tmpPathArray[tmpPathIndex], E'\\|+'), 1);
		if tmpLength >= 18 then
			--raise INFO 'tmpPathArray------- %', cast(tmpPathArray[tmpPathIndex] as varchar); 
			tmpPathIndex := tmpPathIndex + 1;
			continue;
		end if;	
	
		for rec in execute
			'
			SELECT nextroad, dir, nextnode, array[link_type, road_type, toll] as attr
			from (
				select objectid as nextroad, ''(3)'' as dir, to_node_id as nextnode, 
				       rdb_cnv_link_type(linkclass_c,roadclass_c) as link_type,
				       rdb_cnv_road_type(roadclass_c, roadwidth_c, linkclass_c, naviclass_c) as road_type,
				       rdb_cnv_link_toll(roadclass_c,null) as toll
				from road_link_4326
				where	(from_node_id = ' || tmpLastNodeArray[tmpPathIndex] || ' ) 
					and (rdb_cnv_oneway_code(roadclass_c, naviclass_c, oneway_c,nopassage_c) in (1, 3))
					        and (rdb_cnv_link_type(linkclass_c,roadclass_c) in (3, 5, 7) 
					        or rdb_cnv_link_toll(roadclass_c,null) = 1 
					        or rdb_cnv_road_type(roadclass_c, roadwidth_c, linkclass_c, naviclass_c) in (0, 1))
					and objectid <> ' || tmplastLinkArray[tmpPathIndex] || ' 
					
				union

				select objectid as nextroad, ''(2)'' as dir, from_node_id as nextnode, 
				       rdb_cnv_link_type(linkclass_c,roadclass_c) as link_type,
				       rdb_cnv_road_type(roadclass_c, roadwidth_c, linkclass_c, naviclass_c) as road_type,
				       rdb_cnv_link_toll(roadclass_c,null) as toll
				from road_link_4326
				where	(to_node_id = ' || tmpLastNodeArray[tmpPathIndex] || ')
					and (rdb_cnv_oneway_code(roadclass_c, naviclass_c, oneway_c,nopassage_c) in (1,2))
					     and (rdb_cnv_link_type(linkclass_c,roadclass_c) in (3, 5, 7) 
					     or rdb_cnv_link_toll(roadclass_c,null) = 1 
					     or rdb_cnv_road_type(roadclass_c, roadwidth_c, linkclass_c, naviclass_c) in (0, 1))
					and objectid <> ' || tmplastLinkArray[tmpPathIndex] || ' 
					
			) as a
			'
		loop	
			if not ((rec.nextroad::varchar || rec.dir) = ANY(regexp_split_to_array(tmpPathArray[tmpPathIndex], E'\\|+'))) then
				if tmpPathIndex = 1 then
					tmpPathArray     := array_append(tmpPathArray, cast(rec.nextroad || rec.dir as varchar));
					tmpLastNodeArray := array_append(tmpLastNodeArray, cast(rec.nextnode as bigint));
					tmplastLinkArray := array_append(tmplastLinkArray, cast(rec.nextroad as bigint));
				else 
					tmpPathArray     := array_append(tmpPathArray, cast(rec.nextroad || rec.dir || '|' || tmpPathArray[tmpPathIndex] as varchar));
					tmpLastNodeArray := array_append(tmpLastNodeArray, cast(rec.nextnode as bigint));
					tmplastLinkArray := array_append(tmplastLinkArray, cast(rec.nextroad as bigint));
				end if;
				tmpPathCount		 := tmpPathCount + 1;
			end if;
		
		end loop;
		tmpPathIndex := tmpPathIndex + 1;
	end loop;

	return rstPathArray;
END;
$$;

-----------------------------------------------------------------------------------------
-- check Highway Node type
-- 1: SA; 2: PA; 3: JCT; 4: Ramp; 5: IC; 6: Toll; 7: SmartIC
-----------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_check_hwy_node_type_jdb(nNodeID bigint, facilcls integer[])
	RETURNS boolean
	LANGUAGE plpgsql
AS $$ 
DECLARE
BEGIN
	if nNodeID is null or facilcls is null then
		return false;
	end if;
	
	PERFORM  node_id
	  FROM highway_node_add_nodeid
	  where node_id = nNodeID and facilclass_c = Any(facilcls) and tollclass_c in (0, 4);  -- not toll node

	if not found then
		return False;
	end if;

	return true;
END;
$$;
-------------------------------------------------------------------------------------------------------------
-- find in link for IC Enter
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_findpath_hwy_enter_inlink_jdb(nEnterNodeId bigint, out_link_id bigint)
  RETURNS character varying[] 
  LANGUAGE plpgsql AS
$$ 
DECLARE
	rstPathArray            varchar[];
	rstPathCount            integer;
	rstPath                 varchar;
	
	tmpPathArray            varchar[];
	tmpNodeArray            varchar[];
	tmpLastNodeArray        bigint[];
	tmpPathCount            integer;
	tmpPathIndex            integer;
	tmplastLinkArray        bigint[];
	tmpLastLinkAttrArray	integer[][3];
	tmpHwyEnterNodeArray    integer[];
	
	tmpNextLinkArray        bigint[];
	tmpNextNodeArray        bigint[];
	tmpNextLinkTypeArray    integer[][3];
	nNewFromLink            bigint;
	nNewToLink              bigint;
	tmpLength               integer;
	isEnterNode             boolean;
	isEnterLink             boolean;
	isPassedJCTRampLink     boolean[];
	isForSmartIC            boolean;
	i                       integer;
	j                       integer;
	tmpArray                integer[3];
	rec                     record;
BEGIN
	if nEnterNodeId is null or out_link_id is null then 
		return null;
	end if;
	
	--rstPathArray
	rstPathCount            := 0;
	tmpPathArray            := array['0'];
	tmpNodeArray            := array[nEnterNodeId::varchar];
	tmpLastNodeArray        := ARRAY[nEnterNodeId];
	tmplastLinkArray        := array[0];
	tmpLastLinkAttrArray    := array[[-1, -1, -1]];
	tmpHwyEnterNodeArray    := array[NULL];
	isPassedJCTRampLink     := array[false];
	tmpPathCount            := 1;
	tmpPathIndex            := 1;
	tmpLength               := 0;
	isForSmartIC            := false;
	
	--raise INFO 'nEnterNodeId: %,  current_time: %.', nEnterNodeId, current_time;
	-- search
	WHILE tmpPathIndex <= tmpPathCount LOOP
		--raise INFO 'tmpPathIndex: %=====================================', tmpPathIndex;
		-- Not Inner Link
		if tmpLastLinkAttrArray[tmpPathIndex][1] not in(-1, 4) then 
                        --raise INFO 'tmpPathArray[tmpPathIndex]: %', tmpPathArray[tmpPathIndex];
			rstPathCount			:= rstPathCount + 1;
			rstPath				:= cast(tmpPathArray[tmpPathIndex] as varchar);
			if rstPathCount = 1 then
				-- append enter node id at the end
				rstPathArray := ARRAY[rstPath]; 
			else
				-- append enter node id at the end
				rstPathArray := array_append(rstPathArray, rstPath); 
			end if;
			
			tmpPathIndex := tmpPathIndex + 1;
			continue;
		end if;

		-- 4: Ramp; 5: IC; 7: SmartIC
		if (tmpPathIndex > 1 and 
                        (tmpLastLinkAttrArray[tmpPathIndex][1] not in(-1, 4)) and    -- not inner link
                        mid_check_hwy_node_type_jdb(tmpLastNodeArray[tmpPathIndex], array[4, 5, 7])) then 
			tmpPathIndex := tmpPathIndex + 1;
			continue;
		end if;

		for rec in execute
			'
			SELECT nextroad, dir, nextnode, array[link_type, road_type, toll] as attr
			from (
				select objectid as nextroad, ''(3)'' as dir, to_node_id as nextnode, 
				       rdb_cnv_link_type(linkclass_c,roadclass_c) as link_type,
				       rdb_cnv_road_type(roadclass_c, roadwidth_c, linkclass_c, naviclass_c) as road_type,
				       rdb_cnv_link_toll(roadclass_c,null) as toll
				from road_link_4326
				where	(from_node_id = ' || tmpLastNodeArray[tmpPathIndex] || ' ) 
					and (rdb_cnv_oneway_code(roadclass_c, naviclass_c, oneway_c,nopassage_c) in (1, 3))
					and objectid <> ' || tmplastLinkArray[tmpPathIndex] || ' 
					
				union

				select objectid as nextroad, ''(2)'' as dir, from_node_id as nextnode, 
				       rdb_cnv_link_type(linkclass_c,roadclass_c) as link_type,
				       rdb_cnv_road_type(roadclass_c, roadwidth_c, linkclass_c, naviclass_c) as road_type,
				       rdb_cnv_link_toll(roadclass_c,null) as toll
				from road_link_4326
				where	(to_node_id = ' || tmpLastNodeArray[tmpPathIndex] || ')
					and (rdb_cnv_oneway_code(roadclass_c, naviclass_c, oneway_c,nopassage_c) in (1,2))
					and objectid <> ' || tmplastLinkArray[tmpPathIndex] || ' 	
			) as a
			'
		loop	
			if not ((rec.nextroad::varchar) = ANY(regexp_split_to_array(tmpPathArray[tmpPathIndex], E'\\|+'))) 
			   and not ((rec.nextnode::varchar) = ANY(regexp_split_to_array(tmpNodeArray[tmpPathIndex], E'\\|+')))  then
				
				if tmpLastLinkAttrArray[tmpPathIndex][1] = 4 and rec.attr[1] = 5 then
					continue;
				end if;
				
				if 1 = tmpPathIndex then
					tmpPathArray     := array_append(tmpPathArray, cast(rec.nextroad as varchar));
					tmpLastNodeArray := array_append(tmpLastNodeArray, cast(rec.nextnode as bigint));
					tmplastLinkArray := array_append(tmplastLinkArray, cast(rec.nextroad as bigint));
				else 
					tmpPathArray     := array_append(tmpPathArray, cast(rec.nextroad || '|' || tmpPathArray[tmpPathIndex] as varchar));
					tmpLastNodeArray := array_append(tmpLastNodeArray, cast(rec.nextnode as bigint));
					tmplastLinkArray := array_append(tmplastLinkArray, cast(rec.nextroad as bigint));
				end if;
				tmpLastLinkAttrArray := tmpLastLinkAttrArray || rec.attr::integer[];
				tmpNodeArray         := array_append(tmpNodeArray, cast(rec.nextnode || '|' || tmpNodeArray[tmpPathIndex] as varchar));
				tmpPathCount     	 := tmpPathCount + 1;
			end if;
		
		end loop;
		tmpPathIndex := tmpPathIndex + 1;
	end loop;

	return rstPathArray;
END;
$$;

-----------------------------------------------------------------------------------------
-- short cut the full path
-----------------------------------------------------------------------------------------
CREATE or REPLACE function mid_get_fac_name_shot_passlid(id integer, inlinkid bigint, nodeid bigint,
							full_path character varying[], tonodeid bigint, 
							nfacil_class integer, nin_out integer, strName_sp character varying)
  RETURNS character varying
  LANGUAGE plpgsql 
  AS
$$
DECLARE
	link_num       integer;
	passlink_cnt   integer;
	connect_node   bigint;
	passlink_str   character varying;
	rec            record;
	i              integer;
	temp_nextlink  bigint;
BEGIN	
	if inlinkid is null or nodeid is null or tonodeid is null 
	   or full_path is null or array_upper(full_path, 1) < 1 then
		return NULL;
	end if;
	
	passlink_cnt := array_upper(full_path, 1);
	i            := 0;
	-- JCT 
	if nfacil_class = 3 then
		-- Not JCT IN (IC OUT: 2, JCT )
		--if not mid_check_hwy_node_type(tonodeid, array[7]) then 
		--	return full_path[1]::character varying;
		--end if;
		
		-- there are Ramp Link in the JCT IN Node
		if mid_has_ramp_link(tonodeid, full_path[passlink_cnt]::bigint) then
			for rec in execute
				'
				SELECT nextroad
				from (
					select objectid as nextroad
					  from road_link_4326
					  where	(from_node_id = ' || tonodeid || ' ) 
					       and (rdb_cnv_oneway_code(roadclass_c, naviclass_c, oneway_c,nopassage_c) in (1,2)) 
					       and rdb_cnv_link_type(linkclass_c,roadclass_c) <> 5 
					       and objectid <> ' || full_path[passlink_cnt] || '
					union
						
					select objectid as nextroad
					  from road_link_4326
					  where	(to_node_id = ' || tonodeid || ')
					       and (rdb_cnv_oneway_code(roadclass_c, naviclass_c, oneway_c,nopassage_c) in (1,3)) 
					       and rdb_cnv_link_type(linkclass_c,roadclass_c) <> 5 
					       and objectid <> ' || full_path[passlink_cnt] || '
				) as a
				'
			loop
				i             := i + 1;
				temp_nextlink := rec.nextroad;
			end loop;
				
			if i = 0 then
				raise info 'gid = %, There are no out link.',  id;
				return null;
			end if;
				
			if i > 1 then 
				raise EXCEPTION 'gid = %, There are more than two out links.',  id;
			end if;
			-- append the out link
			return array_to_string(full_path, '|') || '|' || temp_nextlink::varchar;
		else
			--raise info '%', tonodeid;
			null;
		end if;
	end if;

	while passlink_cnt > 1 LOOP
		-- highway Enter
		if nfacil_class in (4, 5, 7) and nin_out = 1 then
			-- Inner link
			PERFORM objectid from road_link_4326 
			    where objectid = full_path[passlink_cnt - 1]::bigint and rdb_cnv_link_type(linkclass_c,roadclass_c) = 4;
			if found then				
				return array_to_string(full_path[1:passlink_cnt], '|');
			end if;
		end if;
		
		connect_node = mid_get_intersect_node(full_path[passlink_cnt - 1]::bigint, full_path[passlink_cnt]::bigint);
		
		if connect_node is not null then 
			-- Get Other Out Link
			for rec in execute
				'
				SELECT nextroad, link_type
				from (
					select objectid as nextroad, rdb_cnv_link_type(linkclass_c,roadclass_c) as link_type
					  from road_link_4326
					  where	(from_node_id = ' || connect_node || ' ) 
					       and (rdb_cnv_oneway_code(roadclass_c, naviclass_c, oneway_c,nopassage_c) in (1,2)) 
					       and objectid <> ' || full_path[passlink_cnt - 1]|| '
					       and objectid <> ' || full_path[passlink_cnt] || '
					union
					
					select objectid as nextroad, rdb_cnv_link_type(linkclass_c,roadclass_c) as link_type
					  from road_link_4326
					  where	(to_node_id = ' || connect_node || ')
					       and (rdb_cnv_oneway_code(roadclass_c, naviclass_c, oneway_c,nopassage_c) in (1,3)) 
					       and objectid <> ' || full_path[passlink_cnt - 1]|| '
					       and objectid <> ' || full_path[passlink_cnt] || '
				) as a
				'
			loop
				passlink_str := array_to_string(full_path[1:(passlink_cnt - 1)], '|') || '|' || rec.nextroad::varchar;
				
				PERFORM gid
				  from mid_temp_facilit_matched_add_nameid
				  where in_link_id = inlinkid and node_id = nodeid and gid <> id
				       and array_to_string(full_pass_link, '|') like passlink_str || '%'
				       and (facilclass_c <> nfacil_class or sp_name <> strName_sp);
				-- Exist Other Facility Name or Other Type Facility.
				if found then
					return array_to_string(full_path[1:passlink_cnt], '|');
				else
					-- Not Smart IC Enter
					if not (nfacil_class = 7 and nin_out = 1) then
						PERFORM gid
						  from mid_temp_facilit_matched_add_nameid
						  where in_link_id = inlinkid and node_id = nodeid and gid <> id
						       and array_to_string(full_pass_link, '|') like passlink_str || '%';
						 -- Exist Other Direction, but dose not exist Facility Name.
						 if not found then
							if nfacil_class not in(1, 2) then -- not SA/PA
								return array_to_string(full_path[1:passlink_cnt], '|');
							else -- SA/PA
								if rec.link_type <> 7 then    -- Other links is not SA/PA link
									return array_to_string(full_path[1:passlink_cnt], '|');
								end if;
								raise info 'nodeid: %', nodeid;
							end if;
						 end if;
					end if;

					 -- Same Facility Name and Type
				end if;
				
			end loop;
		else
			raise EXCEPTION 'gid = %, link:% and link:% dose not intersect.',  id;
		end if;
		
		passlink_cnt := passlink_cnt -1;
	END LOOP;

	return array_to_string(full_path[1:passlink_cnt], '|');
END;
$$;

-----------------------------------------------------------------------------------------
-- Two Links' Intersect_node
-----------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_get_intersect_node(fromlink bigint, tolink bigint)
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
  
	select from_node_id, to_node_id into fromnodeid1, tonodeid1 
		from road_link_4326 where objectid = fromLink;
	if not found then
		return NULL;
	end if;
	
	select from_node_id, to_node_id into fromnodeid2, tonodeid2 
		from road_link_4326 where objectid = toLink;
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

-----------------------------------------------------------------------------------------
-- Ramp Link 
-----------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_has_ramp_link(nnodeid bigint, nlink_id bigint)
  RETURNS boolean 
  LANGUAGE plpgsql AS
$$ 
DECLARE
BEGIN
	if nNodeID is null or nlink_id is null then
		return false;
	end if;
	
	PERFORM  objectid
	  from road_link_4326
	  where ((from_node_id = nNodeID and (rdb_cnv_oneway_code(roadclass_c, naviclass_c, oneway_c,nopassage_c) in (1,2))) 
	          or (to_node_id = nNodeID and (rdb_cnv_oneway_code(roadclass_c, naviclass_c, oneway_c,nopassage_c) in (1,3)))) 
	         and objectid <> nlink_id  
	         and rdb_cnv_link_type(linkclass_c,roadclass_c) in (5, 7);

	if not found then
		return False;
	end if;

	return true;
END;
$$;

-----------------------------------------------------------------------------------------
-- facility Name Guide Attribute
-----------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_convert_fac_name_guideattr(fac_type integer)
  RETURNS smallint
  LANGUAGE plpgsql AS
$$ 
DECLARE
begin
	return 
	case 
	when fac_type = 1 then 4  -- SA
	when fac_type = 2 then 5  -- PA
	when fac_type = 3 then 3  -- JCT
	when fac_type = 4 then 2  -- Ramp Enter/Exit
	when fac_type = 5 then 1  -- IC Enter/Exit
	when fac_type = 7 then 1  -- SmartIC Enter/Exit
	else 0
	end;
end;
$$;


-------------------------------------------------------------------------------------------------------------
-- regulation
-------------------------------------------------------------------------------------------------------------


CREATE OR REPLACE FUNCTION mid_convert_weekflag(dayofweek integer)
    RETURNS integer[]
    LANGUAGE plpgsql
AS $$
DECLARE
	weekflag 		integer;
	weekflag_array	integer[];
BEGIN	
	-- notset
    if dayofweek = -1 then
        weekflag := 0;
        weekflag_array	:= ARRAY[weekflag];
    -- sunday or holiday
    elseif dayofweek = 1 then
        weekflag := (1<<0);
        weekflag_array	:= ARRAY[weekflag];
        weekflag := 127 | (1<<7) | (1<<9);
        weekflag_array	:= array_append(weekflag_array, weekflag);
    -- not sunday and not holiday
    elseif dayofweek = 2 then
        weekflag := (1<<1) | (1<<2) | (1<<3) | (1<<4) | (1<<5) | (1<<6) | (1<<7) | (1<<10);
        weekflag_array	:= ARRAY[weekflag];
    -- holiday
    elseif dayofweek = 3 then
        weekflag := 127 | (1<<7) | (1<<9);
        weekflag_array	:= ARRAY[weekflag];
    -- not holiday
    elseif dayofweek = 4 then
        weekflag := 127 | (1<<7) | (1<<10);
        weekflag_array	:= ARRAY[weekflag];
    -- workday
    elseif dayofweek = 5 then
        weekflag := (1<<1) | (1<<2) | (1<<3) | (1<<4) | (1<<5) | (1<<6);
        weekflag_array	:= ARRAY[weekflag];
    -- sunday
    elseif dayofweek = 6 then
        weekflag := (1<<0);
        weekflag_array	:= ARRAY[weekflag];
    -- monday
    elseif dayofweek = 7 then
        weekflag := (1<<1);
        weekflag_array	:= ARRAY[weekflag];
    -- tuesday
    elseif dayofweek = 8 then
        weekflag := (1<<2);
        weekflag_array	:= ARRAY[weekflag];
    -- wednesday
    elseif dayofweek = 9 then
        weekflag := (1<<3);
        weekflag_array	:= ARRAY[weekflag];
    -- thursday
    elseif dayofweek = 10 then
        weekflag := (1<<4);
        weekflag_array	:= ARRAY[weekflag];
    -- friday
    elseif dayofweek = 11 then
        weekflag := (1<<5);
        weekflag_array	:= ARRAY[weekflag];
    -- saturday
    elseif dayofweek = 12 then
        weekflag := (1<<6);
        weekflag_array	:= ARRAY[weekflag];
    -- saturday or sunday or holiday
    elseif dayofweek = 13 then
        weekflag := (1<<6) | (1<<0);
        weekflag_array	:= ARRAY[weekflag];
        weekflag := 127 | (1<<7) | (1<<9);
        weekflag_array	:= array_append(weekflag_array, weekflag);
    -- not saturday and not sunday and not holiday
    elseif dayofweek = 14 then
        weekflag := (1<<1) | (1<<2) | (1<<3) | (1<<4) | (1<<5) | (1<<7) | (1<<10);
        weekflag_array	:= ARRAY[weekflag];
    -- all day
    elseif dayofweek = 15 then
        weekflag := 0;
        weekflag_array	:= ARRAY[weekflag];
    -- not sunday
    elseif dayofweek = 16 then
        weekflag := (1<<1) | (1<<2) | (1<<3) | (1<<4) | (1<<5) | (1<<6);
        weekflag_array	:= ARRAY[weekflag];
    -- saturday or the day before holiday
    elseif dayofweek = 17 then
        weekflag := (1<<6);
        weekflag_array	:= ARRAY[weekflag];
        weekflag := 127 | (1<<7) | (1<<11);
        weekflag_array	:= array_append(weekflag_array, weekflag);
    else
		weekflag := 0;
        weekflag_array	:= ARRAY[weekflag];
    end if;
    
	return weekflag_array;
END;
$$;

CREATE OR REPLACE FUNCTION mid_convert_condition_regulation_table()
    RETURNS smallint
    LANGUAGE plpgsql
AS $$
DECLARE
	rec				record;
	cur_cond_id		integer;
	dayofweek_array	integer[];
	dayofweek_count integer;
	dayofweek_index integer;
BEGIN
	cur_cond_id := 0;
	
	for rec in 
        select *
        from
        (
        select start_month, start_day, start_hour, start_min, end_month, end_day, end_hour, end_min, dayofweek_c
        from time_nopassage
        union 
        select start_month, start_day, start_hour, start_min, end_month, end_day, end_hour, end_min, dayofweek_c
        from time_oneway
        union
        select start_month1, start_day1, start_hour1, start_min1, end_month1, end_day1, end_hour1, end_min1, dayofweek1_c
        from inf_turnreg
        union
        select start_month2, start_day2, start_hour2, start_min2, end_month2, end_day2, end_hour2, end_min2, dayofweek2_c
        from inf_turnreg
        union
        select start_month3, start_day3, start_hour3, start_min3, end_month3, end_day3, end_hour3, end_min3, dayofweek3_c
        from inf_turnreg
        union
        select start_month4, start_day4, start_hour4, start_min4, end_month4, end_day4, end_hour4, end_min4, dayofweek4_c
        from inf_turnreg
        )as t
        where t.start_month is not null
        group by start_month, start_day, start_hour, start_min, end_month, end_day, end_hour, end_min, dayofweek_c
        order by start_month, start_day, start_hour, start_min, end_month, end_day, end_hour, end_min, dayofweek_c
    LOOP
    	dayofweek_array	:= mid_convert_weekflag(rec.dayofweek_c);
    	dayofweek_count	:= array_upper(dayofweek_array, 1);
    	for dayofweek_index in 1..dayofweek_count loop
    		
	    	cur_cond_id := cur_cond_id + 1;
	    	
	    	insert into temp_condition_regulation_tbl
	    				(
	    				"cond_id", "dayofweek_c", 
						"start_month", "start_day", "start_hour", "start_min", 
						"end_month", "end_day", "end_hour", "end_min"
						)
	    		VALUES	(
	    				cur_cond_id, rec.dayofweek_c, 
	    				rec.start_month, rec.start_day, rec.start_hour, rec.start_min, 
	    				rec.end_month, rec.end_day, rec.end_hour, rec.end_min
	    				);
	    	
	    	insert into condition_regulation_tbl
	    				(
	    				"cond_id", "car_type", "day_of_week",
						"start_year", "start_month", "start_day", "start_hour", "start_minute",
						"end_year", "end_month", "end_day", "end_hour", "end_minute", "exclude_date"
						)
	    		VALUES	(
	    				cur_cond_id, -1, dayofweek_array[dayofweek_index],
	    				0, rec.start_month, rec.start_day, rec.start_hour, rec.start_min,
	    				0, rec.end_month, rec.end_day, rec.end_hour, rec.end_min, 0
	    				);
	    end loop;
    END LOOP;
    
    return 1;
END;
$$;


CREATE OR REPLACE FUNCTION mid_convert_regulation_from_trn()
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
    	select	inf_id, old_linkid_array, 
    			array_agg(cond_id) as array_condid
    	from
    	(
	    	select	a.inf_id, a.old_linkid_array, c.cond_id
	    	from
	    	(
		    	select inf_id, array_agg(link_id) as old_linkid_array
		    	from
		    	(
			    	select inf_id, sequence, link_id
			    	from lq_turnreg
			    	order by inf_id, sequence
		    	)as t
		    	group by inf_id
	    	)as a
	    	left join inf_turnreg as b
	    	on a.inf_id = b.objectid
	    	left join temp_condition_regulation_tbl c
	    	on (	
		    			(b.start_month1 = c.start_month)
		    		and (b.start_day1 = c.start_day)
		    		and (b.start_hour1 = c.start_hour)
		    		and (b.start_min1 = c.start_min)
		    		and (b.end_month1 = c.end_month)
		    		and (b.end_day1 = c.end_day)
		    		and (b.end_hour1 = c.end_hour)
		    		and (b.end_min1 = c.end_min)
		    		and (b.dayofweek1_c = c.dayofweek_c)
	    		)
	    		or
	    		(	
		    			(b.start_month2 = c.start_month)
		    		and (b.start_day2 = c.start_day)
		    		and (b.start_hour2 = c.start_hour)
		    		and (b.start_min2 = c.start_min)
		    		and (b.end_month2 = c.end_month)
		    		and (b.end_day2 = c.end_day)
		    		and (b.end_hour2 = c.end_hour)
		    		and (b.end_min2 = c.end_min)
		    		and (b.dayofweek2_c = c.dayofweek_c)
	    		)
	    		or
	    		(	
		    			(b.start_month3 = c.start_month)
		    		and (b.start_day3 = c.start_day)
		    		and (b.start_hour3 = c.start_hour)
		    		and (b.start_min3 = c.start_min)
		    		and (b.end_month3 = c.end_month)
		    		and (b.end_day3 = c.end_day)
		    		and (b.end_hour3 = c.end_hour)
		    		and (b.end_min3 = c.end_min)
		    		and (b.dayofweek3_c = c.dayofweek_c)
	    		)
	    		or
	    		(	
		    			(b.start_month4 = c.start_month)
		    		and (b.start_day4 = c.start_day)
		    		and (b.start_hour4 = c.start_hour)
		    		and (b.start_min4 = c.start_min)
		    		and (b.end_month4 = c.end_month)
		    		and (b.end_day4 = c.end_day)
		    		and (b.end_hour4 = c.end_hour)
		    		and (b.end_min4 = c.end_min)
		    		and (b.dayofweek4_c = c.dayofweek_c)
	    		)
	    	order by a.inf_id, a.old_linkid_array, c.cond_id
    	)as t
    	group by inf_id, old_linkid_array
    	order by inf_id, old_linkid_array
    LOOP
		-- new link id array
		nCount := array_upper(rec.old_linkid_array, 1);
		nIndex := 1;
		while nIndex <= nCount LOOP
			if nIndex = 1 then
				nLinkid				:= rec.old_linkid_array[nIndex];
				nConnectedLinkid	:= rec.old_linkid_array[nIndex + 1];
				nTargetLinkid		:= mid_getSplitLinkID(nLinkid, nConnectedLinkid, True);
				linkid_array		:= ARRAY[nTargetLinkid];
			elseif nIndex < nCount then
				nLinkid				:= rec.old_linkid_array[nIndex];
				nConnectedLinkid	:= rec.old_linkid_array[nIndex + 1];
				linkid_array		:= linkid_array || mid_getSplitLinkIDArray(nLinkid, nConnectedLinkid, True);
			else
				nLinkid				:= rec.old_linkid_array[nIndex];
				nConnectedLinkid	:= rec.old_linkid_array[nIndex - 1];
				nTargetLinkid		:= mid_getSplitLinkID(nLinkid, nConnectedLinkid, True);
				linkid_array		:= linkid_array || nTargetLinkid;
			end if;
			nIndex := nIndex + 1;
		END LOOP;
		
		-- new node id array
		nCount := array_upper(linkid_array, 1);
		nIndex := 1;
		while nIndex <= nCount LOOP
			if nIndex = 1 then
				nTileLinkid				:= linkid_array[nIndex];
				nTileConnectedLinkid	:= linkid_array[nIndex + 1];
				nTargetNodeid			:= mid_getConnectionNodeID(nTileLinkid, nTileConnectedLinkid, True);
				nodeid_array			:= ARRAY[nTargetNodeid];
			elseif nIndex < nCount then
				nTileLinkid				:= linkid_array[nIndex];
				nTileConnectedLinkid	:= linkid_array[nIndex + 1];
				nTargetNodeid			:= mid_getConnectionNodeID(nTileLinkid, nTileConnectedLinkid, True);
				nodeid_array			:= nodeid_array || nTargetNodeid;
			else
				nTileLinkid				:= linkid_array[nIndex];
				nTileConnectedLinkid	:= linkid_array[nIndex - 1];
				nTargetNodeid			:= mid_getConnectionNodeID(nTileLinkid, nTileConnectedLinkid, False);
				nodeid_array			:= nodeid_array || nTargetNodeid;
			end if;
			nIndex := nIndex + 1;
		END LOOP;
		
		-- current regulation id
    	cur_regulation_id := cur_regulation_id + 1;
		    	
    	-- insert into regulation_item_tbl
		nCount := array_upper(linkid_array, 1);
		nIndex := 1;
		while nIndex <= nCount LOOP
			if nIndex = 1 then
	    		insert into regulation_item_tbl
	    					("regulation_id", "linkid", "nodeid", "seq_num")
	    			VALUES	(cur_regulation_id, linkid_array[nIndex], null, nIndex);
	    		insert into regulation_item_tbl
	    					("regulation_id", "linkid", "nodeid", "seq_num")
	    			VALUES	(cur_regulation_id, null, nodeid_array[nIndex], nIndex+1);
	    	else
	    		insert into regulation_item_tbl
	    					("regulation_id", "linkid", "nodeid", "seq_num")
	    			VALUES	(cur_regulation_id, linkid_array[nIndex], null, nIndex+1);
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
	    						 nodeid_array[1], linkid_array[1], linkid_array[array_upper(linkid_array,1)], 
	    						 1, rec.array_condid[nIndex]);
	    	if rec.array_condid[nIndex] is null then
	    		exit;
	    	end if;
			nIndex := nIndex + 1;
		END LOOP;    	
    	
    END LOOP;
    return 1;
END;
$$;

CREATE OR REPLACE FUNCTION mid_convert_regulation_from_owc()
    RETURNS smallint
    LANGUAGE plpgsql
AS $$
DECLARE
	rec record;
	rec2 record;
	cur_regulation_id integer;
	nCount integer;
	nIndex integer;
BEGIN
	-- regulation_id
	select (case when max(regulation_id) is null then 0 else max(regulation_id) end)
	from regulation_relation_tbl
	into cur_regulation_id;
	

    FOR rec IN
    
    	select link_id, array_agg(linkdir_c) array_linkdir, array_agg(cond_id) as array_condid
    	from
    	(
	    	select link_id, linkdir_c, cn.cond_id
	    	from time_oneway as r
	    	left join temp_condition_regulation_tbl as cn
	    	on	r.start_month = cn.start_month and 
	    		r.start_day = cn.start_day and
	    		r.start_hour = cn.start_hour and
	    		r.start_min = cn.start_min and
	    		r.end_month = cn.end_month and
	    		r.end_day = cn.end_day and
	    		r.end_hour = cn.end_hour and
	    		r.end_min = cn.end_min and
	    		r.dayofweek_c = cn.dayofweek_c
	    	order by link_id, linkdir_c, cn.cond_id
    	)as t
    	group by link_id
    	order by link_id
    
    LOOP
		-- for every sublink
		FOR rec2 IN execute 
			'select tile_link_id from middle_tile_link where old_link_id = ' || (@rec.link_id) || ' order by split_seq_num asc'
		LOOP
			-- current regulation id
	    	cur_regulation_id := cur_regulation_id + 1;
	    	
	    	-- insert into regulation_item_tbl
	    	insert into regulation_item_tbl
	    				("regulation_id", "linkid", "nodeid", "seq_num")
	    		VALUES 	(cur_regulation_id, rec2.tile_link_id, null, 1);
			
			-- insert into regulation_relation_tbl
			nCount := array_upper(rec.array_condid, 1);
			nIndex := 1;
			while nIndex <= nCount LOOP
				if rec.array_linkdir[nIndex] = 1 then
		    		insert into regulation_relation_tbl
		    					("regulation_id", "nodeid", "inlinkid", "outlinkid", "condtype", "cond_id")
		    			VALUES 	(cur_regulation_id, null, rec2.tile_link_id, null, 43::smallint, rec.array_condid[nIndex]);
	    		else
		    		insert into regulation_relation_tbl
		    					("regulation_id", "nodeid", "inlinkid", "outlinkid", "condtype", "cond_id")
		    			VALUES 	(cur_regulation_id, null, rec2.tile_link_id, null, 42::smallint, rec.array_condid[nIndex]);
	    		end if;
	    		nIndex := nIndex + 1;
	    	END LOOP;
		END LOOP;
    END LOOP;
    return 1;
END;
$$;

CREATE OR REPLACE FUNCTION mid_convert_regulation_from_npc()
    RETURNS smallint
    LANGUAGE plpgsql
AS $$
DECLARE
	rec record;
	rec2 record;
	cur_regulation_id integer;
	nCount integer;
	nIndex integer;
BEGIN
	-- regulation_id
	select (case when max(regulation_id) is null then 0 else max(regulation_id) end)
	from regulation_relation_tbl
	into cur_regulation_id;
	
    FOR rec IN
    
    	select link_id, array_agg(cond_id) as array_condid
    	from
    	(
	    	select link_id, cn.cond_id
	    	from time_nopassage as r
	    	left join temp_condition_regulation_tbl as cn
	    	on	r.start_month = cn.start_month and 
	    		r.start_day = cn.start_day and
	    		r.start_hour = cn.start_hour and
	    		r.start_min = cn.start_min and
	    		r.end_month = cn.end_month and
	    		r.end_day = cn.end_day and
	    		r.end_hour = cn.end_hour and
	    		r.end_min = cn.end_min and
	    		r.dayofweek_c = cn.dayofweek_c
	    	order by link_id, cn.cond_id
    	)as t
    	group by link_id
    	order by link_id
    
    LOOP
		-- for every sublink
		FOR rec2 IN execute 
			'select tile_link_id from middle_tile_link where old_link_id = ' || (@rec.link_id) || ' order by split_seq_num asc'
		LOOP
			-- current regulation id
	    	cur_regulation_id := cur_regulation_id + 1;
	    	
	    	-- insert into regulation_item_tbl
	    	insert into regulation_item_tbl("regulation_id", "linkid", "nodeid", "seq_num")
	    							VALUES (cur_regulation_id, rec2.tile_link_id, null, 1);
			
	    	-- insert into regulation_relation_tbl
			nCount := array_upper(rec.array_condid, 1);
			nIndex := 1;
			while nIndex <= nCount LOOP
	    		insert into regulation_relation_tbl
	    					("regulation_id", "nodeid", "inlinkid", "outlinkid", "condtype", "cond_id")
	    			VALUES	(cur_regulation_id, null, rec2.tile_link_id, null, 4, rec.array_condid[nIndex]);
	    		nIndex := nIndex + 1;
	    	END LOOP;
		END LOOP;
    END LOOP;
    return 1;
END;
$$;


-----------------------------------------------------------------------------------------
-- make temp lane_strand tbl
-----------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION make_temp_lane_strand_tbl()
  RETURNS integer 
  LANGUAGE plpgsql 
  AS $$
DECLARE
	
	inf_id_list       integer[];
	inf_id_list_len   integer;
	inf_id_list_index integer;
	link_id_array     integer[];
	link_dir_array    integer[];
	linkids_len       integer;
	linkids_index     integer;
	temp_from_link    integer;
	temp_to_link      integer;
	temp_link_cnt     integer;
	temp_linklib      character varying;
	temp_lanedir_c    integer;
	temp_lanes        integer;
	temp_r_change     integer;
	temp_l_change     integer;
	temp_flow_lane    integer;
	temp_lane_passable character varying;
	temp_link_dir     integer;
BEGIN
	inf_id_list_len = 0;
	SELECT array_agg(distinct inf_id)
	into inf_id_list
	FROM lq_lane;

	inf_id_list_len = array_upper(inf_id_list, 1);
	if inf_id_list_len = 0 then
		return 0;
	end if;
	inf_id_list_index = 1;
	raise info '%', inf_id_list_len;
	while inf_id_list_index <= inf_id_list_len loop
		linkids_len = 0;
		linkids_index = 2;
		temp_linklib = '';
		select array_agg(link_id), array_agg(linkdir_c)
		into link_id_array, link_dir_array
		from (
			select *
			from lq_lane 
			where  inf_id = inf_id_list[inf_id_list_index]
			order by sequence
		) as a;
		
		linkids_len = array_upper(link_id_array, 1);
		if linkids_len < 1 then
			CONTINUE;
		end if;
		temp_from_link = link_id_array[1];
		temp_link_dir = link_dir_array[1];
		temp_to_link = link_id_array[linkids_len];
		temp_link_cnt = linkids_len - 2;
		while linkids_index < linkids_len loop
			if temp_linklib = '' then
				temp_linklib = temp_linklib || cast(link_id_array[linkids_index] as character varying);
			else
				temp_linklib = temp_linklib || ',' || cast(link_id_array[linkids_index] as character varying);
			end if;
			linkids_index = linkids_index + 1;
		end loop;
		select lanedir_c, approach_lane, right_change, left_change, null, lane_passable 
		into temp_lanedir_c, temp_lanes, temp_r_change, temp_l_change, temp_flow_lane, temp_lane_passable
		from inf_lane
		where objectid = inf_id_list[inf_id_list_index];
		insert into temp_lane_strand_tbl(objectid,lanedir_c,approach_lane,right_change,left_change,flow_lane,from_lane_passable,from_link_id,to_link_id,from_linkdir_c,passlink_cnt,passlid)
			values(inf_id_list[inf_id_list_index],temp_lanedir_c,temp_lanes,temp_r_change,temp_l_change,temp_flow_lane,temp_lane_passable,temp_from_link,temp_to_link,temp_link_dir,temp_link_cnt, temp_linklib);
		
		inf_id_list_index = inf_id_list_index + 1;
	end loop;


	
	return 1;
END;
$$;



-- GuideInfo Lane
CREATE OR REPLACE FUNCTION rdb_Lane_ArrowInfo_2Angle(arrow integer)
  RETURNS smallint 
  LANGUAGE plpgsql
  AS
$BODY$
BEGIN
	    RETURN CASE
	        WHEN arrow = 1 THEN 1
	        WHEN arrow = 2 THEN 2
	        WHEN arrow = 3 THEN 4
	        WHEN arrow = 4 THEN 8
			WHEN arrow = 5 THEN 2048
			WHEN arrow = 6 THEN 32
			WHEN arrow = 7 THEN 64
			WHEN arrow = 8 THEN 128
	        ELSE NULL
	    END;
END;
$BODY$;

CREATE OR REPLACE FUNCTION rdb_get_connected_linkid_ipc(tolinkid integer, midlinkid character varying)
  RETURNS integer 
  LANGUAGE plpgsql
  AS $$ 
DECLARE
	midlinkid_array character varying[];
BEGIN
	IF midlinkid is null or midlinkid = '' THEN
		return tolinkid;
	END IF;
	
	midlinkid_array = regexp_split_to_array(midlinkid, E'\\,+');

	return cast(midlinkid_array[1] as integer);
END;
$$;

CREATE OR REPLACE FUNCTION rdb_get_connected_from_linkid_ipc(fromlinkid integer, midlinkid character varying)
  RETURNS integer 
  LANGUAGE plpgsql
  AS $$ 
DECLARE
	midlinkid_array character varying[];
BEGIN
	IF midlinkid is null or midlinkid = '' THEN
		return fromlinkid;
	END IF;
	
	midlinkid_array = regexp_split_to_array(midlinkid, E'\\,+');

	return cast(midlinkid_array[array_upper(midlinkid_array, 1)] as integer);
END;
$$;

CREATE OR REPLACE FUNCTION rdb_get_connected_nodeid_ipc(fromnodeid1 integer, tonodeid1 integer, fromnodeid2 integer, tonodeid2 integer)
  RETURNS integer 
  LANGUAGE plpgsql
  AS $$ 
DECLARE
	midlinkid_array character varying[];
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
CREATE OR REPLACE FUNCTION mid_getSplitLinkIDarray_dir(nLinkid integer,nlink_dir integer, bConnectedFlag boolean)
    RETURNS bigint[] 
    LANGUAGE plpgsql
AS $$
DECLARE
	curs1 refcursor;
	curs2 refcursor;
	rec1 record;
	rec2 record;
	arrayTargetLinkid bigint[];
	nConnectedNodeid integer;
	bStartSubLink boolean;
	bReverseFlag boolean;
BEGIN
	open curs1 for execute
		'select * from middle_tile_link where old_link_id = ' || (@nLinkid) || 'order by split_seq_num asc';
	
	fetch curs1 into rec1;
	if not rec1.tile_link_id is null then
		if rec1.split_seq_num = -1 then
			arrayTargetLinkid[1] := rec1.tile_link_id;
		else
			-- get connected nodeid
			nConnectedNodeid := null;
			open curs2 for execute
				'SELECT (case when ' || @nlink_dir || ' =1 then to_node_id else from_node_id end) as nodeid
				  From road_link_4326 
				  where objectid = ' || @nLinkid || '' ;
			fetch curs2 into rec2;
			if not rec2.nodeid is null then
				nConnectedNodeid := rec2.nodeid;
			end if;
			close curs2;

			-- need to reverse?
			bStartSubLink := (nConnectedNodeid = any(rec1.old_s_e_node));
			if bStartSubLink = bConnectedFlag then
				bReverseFlag := True;
			else
				bReverseFlag := False;
			end if;

			-- get target links
			while not rec1.tile_link_id is null LOOP
				if bReverseFlag then
					-- array_prepend (rec1.tile_link_id, arrayTargetLinkid);
					arrayTargetLinkid = rec1.tile_link_id || arrayTargetLinkid;
				else
					-- array_append (arrayTargetLinkid, rec1.tile_link_id);
					arrayTargetLinkid = arrayTargetLinkid || rec1.tile_link_id;
				end if;
				fetch curs1 into rec1;
			END LOOP;
			
		end if;
	end if;
	close curs1;
	
	return arrayTargetLinkid;
END;
$$;
CREATE OR REPLACE FUNCTION mid_get_new_passlid_dir(old_passlid character varying, lind_dir integer[], split_char character varying)
	-- get new passlid string of rdb link id.
    RETURNS character varying
    LANGUAGE plpgsql
AS $$
DECLARE
	old_passlid_array character varying[];
	new_passlids character varying;
	old_num integer;
	new_num integer;
	i integer;
	j integer;
	new_link_ids bigint[];
BEGIN
	IF old_passlid IS NULL OR old_passlid = '' THEN 
		RETURN NULL;
	END IF;

	old_passlid_array := regexp_split_to_array(old_passlid, E'\\'|| split_char ||'+');
	old_num      := array_upper(old_passlid_array, 1);
	i            := 1;
	new_num      := 0;
	new_passlids := '';
	
	WHILE i <= old_num LOOP
		new_link_ids = mid_getSplitLinkIDarray_dir(old_passlid_array[i]::integer, lind_dir[i+1]::integer, true);
		j := 1;
		while j <= array_upper(new_link_ids, 1) loop
			if new_num = 0 then 
				new_passlids := cast(new_link_ids[j] as character varying);
			else
				new_passlids := new_passlids || '|' || cast(new_link_ids[j] as character varying);
			end if;
			new_num := new_num + 1;
			j := j + 1;
		END LOOP;

		i := i + 1;
	END LOOP;
	
	return new_passlids;
END;
$$;
CREATE OR REPLACE FUNCTION mid_getSplitLinkID_dir(nlinkid integer, nlink_dir integer, bConnectedFlag boolean)
  RETURNS bigint 
  LANGUAGE plpgsql
  AS $$ 
DECLARE
	curs1 refcursor;
	curs2 refcursor;
	rec1 record;
	rec2 record;
	nTargetLinkid bigint;
	nConnectedNodeid integer;
	bStartSubLink boolean;
BEGIN
	nTargetLinkid := null;
	
	open curs1 for execute
		'select * from middle_tile_link where old_link_id = ' || (@nlinkid) || 'order by split_seq_num asc';
	
	fetch curs1 into rec1;
	if rec1.tile_link_id is not null then
		if rec1.split_seq_num = -1 then
			nTargetLinkid := rec1.tile_link_id;
		else
			-- get connected nodeid
			nConnectedNodeid := null;
			open curs2 for execute
				'SELECT (case when ' || @nlink_dir || ' =1 then to_node_id else from_node_id end) as nodeid
				  From road_link_4326 
				  where objectid = ' || @nlinkid || '' ;
			
			fetch curs2 into rec2;
			if not rec2.nodeid is null then
				nConnectedNodeid := rec2.nodeid;
			end if;
			close curs2;
			
			-- get target link
			bStartSubLink := (nConnectedNodeid = any(rec1.old_s_e_node));
			if bStartSubLink = bConnectedFlag then
				nTargetLinkid := rec1.tile_link_id;
			else
				while not rec1.tile_link_id is null LOOP
					nTargetLinkid := rec1.tile_link_id;
					fetch curs1 into rec1;
				END LOOP;
			end if;
		end if;
	end if;
	close curs1;
	
	return nTargetLinkid;
END;
$$;

CREATE OR REPLACE FUNCTION mid_convert_signpost()
    RETURNS smallint
    LANGUAGE plpgsql
AS $$
DECLARE
	rec					record;
	midlinkcount		smallint;
	midlinkids			character varying;
	array_old_midlink	integer[];
	array_new_midlink	bigint[];
	array_dir_link integer[];
	nodeid				bigint;
	inlinkid			bigint;
	outlinkid			bigint;
BEGIN
	for rec in 
		select  d.gid,
				fromlinkid,
				tolinkid,
				midcount,
				midlinkid,
				link_dir_kib,
				guideclass,
				sp_name,
				boardptn_c,
				board_seq
		from (
			-- Name 
			SELECT gid, objectid, fromlinkid, tolinkid, midcount, midlinkid,link_dir_kib,
			 			 guideclass,name_kanji, sp_name, boardptn_c, board_seq
			  from temp_road_dir
			  where boardptn_c <> 0 and mid_isNotAllDigit(name_kanji) = true

			Union

			-- Route Number
			SELECT gid, objectid, fromlinkid, tolinkid, midcount, midlinkid,link_dir_kib,
			 			 guideclass,name_kanji, sp_name, boardptn_c, board_seq
			  from temp_road_dir
			  where boardptn_c <> 0 and mid_isNotAllDigit(name_kanji) = false

			Union

			-- NUll
			SELECT gid, objectid, fromlinkid, tolinkid, midcount, midlinkid,link_dir_kib,
			 			 guideclass,name_kanji, sp_name, boardptn_c, board_seq
			  from temp_road_dir
			  where boardptn_c <> 0 and name_kanji is null
		) as d
		order by d.gid
	LOOP
		-- link_dir
		array_dir_link := rec.link_dir_kib;
		-- mid links
		if rec.midcount > 0 then
			midlinkids			:= mid_get_new_passlid_dir(rec.midlinkid, array_dir_link, ',');
			array_new_midlink	:= cast(regexp_split_to_array(midlinkids, E'\\|+') as bigint[]);
			array_old_midlink	:= cast(regexp_split_to_array(rec.midlinkid, E'\\,+') as integer[]);
			midlinkcount		:= cast(array_upper(array_new_midlink, 1) as smallint);
		else
			midlinkids			:= null;
			midlinkcount		:= 0;
		end if;
		
		-- inlink, outlink, node
		inlinkid			:= mid_getSplitLinkID_dir(rec.fromlinkid, array_dir_link[1], true);
		outlinkid			:= mid_getSplitLinkID_dir(rec.tolinkid, array_dir_link[rec.midcount+2], false);
		if rec.midcount > 0 then
			nodeid				:= mid_getConnectionNodeID(inlinkid, array_new_midlink[1], true);
		else
			nodeid		:= mid_getConnectionNodeID(inlinkid, outlinkid, true);
		end if;
		
		INSERT INTO temp_signpost_tbl
			("id", 
			 "nodeid", "inlinkid", "outlinkid", 
			 "passlid", "passlink_cnt",
			 "direction", "guideattr", "namekind", "guideclass",
			 "sp_name", "patternno", "arrowno"
			 )
			VALUES
			(
			rec.gid,
			nodeid, inlinkid, outlinkid,
			midlinkids, midlinkcount,
			0, 0, 0, rec.guideclass,
			rec.sp_name, cast(rec.boardptn_c as varchar), cast(rec.board_seq as varchar)
			);
	END LOOP;
	return 1;
END;
$$;
-- make road_dir data
CREATE OR REPLACE FUNCTION make_road_dir()
  RETURNS integer 
  LANGUAGE plpgsql
  AS $$ 
DECLARE
	midlinkid_array character varying[];
BEGIN
	IF midlinkid is null or midlinkid = '' THEN
		return fromlinkid;
	END IF;
	
	midlinkid_array = regexp_split_to_array(midlinkid, E'\\,+');

	return cast(midlinkid_array[array_upper(midlinkid_array, 1)] as integer);
END;
$$;

CREATE OR REPLACE FUNCTION make_road_dir_t()
  RETURNS integer 
  LANGUAGE plpgsql
  AS $$ 
DECLARE
	rec            record;
	i              integer;
	t_name_kanji   character varying;
	t_name_yomi    character varying;
	t_guideclass_c integer;
	dirguide_array text[];
	lenth          integer;
	sub_str        text;
	---','ï¿½Ä¸ï¿½ï¿½ï¿½
	commas         integer;
	flag           integer;
BEGIN
	for rec in
		select 	link_lid[1] as from_link,
			link_lid[array_upper(link_lid, 1)] as to_link,
			(array_upper(link_lid, 1)-2) as pass_cnt,
			array_to_string(link_lid[2:(array_upper(link_lid, 1)-1)],',') as pass_link,
			substring(cast((array[c.*])[1] as text) from 2 for char_length(cast((array[c.*])[1] as text))-2) as temp_dirguide,
			objectid, guidepattern_c, sequence
		from (
			select inf_id, array_agg(link_id) link_lid
			from (
				SELECT objectid, link_id, linkdir_c, "sequence", inf_id
				FROM lq_dirguide
				order by inf_id, sequence
				) as a
			group by inf_id
		) as b
		left join inf_dirguide as c
		on b.inf_id = c.objectid
	LOOP	
		commas = 0;
		flag = 1;
		sub_str = rec.temp_dirguide;
		while commas <> 3 and flag <= char_length(sub_str) loop
			if substring(sub_str from flag for 1) = ',' then
				commas = commas + 1;
			end if;
			flag = flag + 1;
		end loop;
		i = 1;
		lenth = 0;
		dirguide_array = regexp_split_to_array(substring(sub_str from flag for char_length(sub_str)),E'\\,+');
		lenth = array_upper(dirguide_array, 1);
		if lenth < 4 then
			insert into road_dir_t(objectid,fromlinkid,tolinkid,midcount,midlinkid,guideclass,name_kanji,name_yomi,boardptn_c,board_seq)
						values(rec.objectid,rec.from_link,rec.to_link,rec.pass_cnt,rec.pass_link,-1,NULL,NULL,rec.guidepattern_c,rec.sequence);
		else
			while i <= lenth loop
				if dirguide_array[i] <> '' then
					insert into road_dir_t(objectid,fromlinkid,tolinkid,midcount,midlinkid,guideclass,name_kanji,name_yomi,boardptn_c,board_seq)
						values(rec.objectid,rec.from_link,rec.to_link,rec.pass_cnt,rec.pass_link,dirguide_array[i+2]::integer,dirguide_array[i],dirguide_array[i+1],rec.guidepattern_c,rec.sequence);
				end if;
				i = i + 3;
			end loop;
		end if;
	end loop;
	return 1;	
END;
$$;

CREATE OR REPLACE FUNCTION rdb_cnv_oneway_code_new(oneway_c integer)
  RETURNS smallint 
  LANGUAGE plpgsql
  AS
$BODY$

BEGIN
	    RETURN CASE
	        WHEN oneway_c = 0 THEN 1
	        WHEN oneway_c = 1 THEN 2
	        WHEN oneway_c = 2 THEN 3
	        WHEN oneway_c = 3 THEN 2
	        WHEN oneway_c = 4 THEN 3
	        WHEN oneway_c = 5 THEN 1
	        ELSE 1
	    END;

END;
$BODY$;


-----make spotguide data
CREATE OR REPLACE FUNCTION mid_convert_spotguide()
    RETURNS smallint
    LANGUAGE plpgsql
AS $$
DECLARE
	rec					record;
	
	array_temp			integer[];
	array_old_node		integer[];
	nIndex				integer;
	nCount				integer;
	oldlinkid			integer;
	
	oldmidcount			integer;
	oldmidlinkid		character varying;
	oldfromlinkid		integer;
	oldtolinkid			integer;
	
	midlinkcount		smallint;
	midlinkids			character varying;
	array_old_midlink	integer[];
	array_new_midlink	bigint[];
	nodeid				bigint;
	inlinkid			bigint;
	outlinkid			bigint;
BEGIN
	-- for branch/grade/inic
	for rec in 
		select gid, nodelist, picname, pictype from road_illust where not pictype in (9,10) order by gid
	LOOP
		-- get old node array (same node in nodelist should be deleted here)
		array_temp		:= cast(regexp_split_to_array(rec.nodelist, E'\\,+') as integer[]);
		nCount			:= array_upper(array_temp,1);
		nIndex			:= 1;
		while nIndex <= nCount loop
			if nIndex = 1 then
				array_old_node	:= ARRAY[array_temp[nIndex]];
			elseif array_temp[nIndex] != array_temp[nIndex-1] then
				array_old_node	:= array_old_node || array_temp[nIndex];
			end if;
			nIndex	:= nIndex + 1;
		end loop;
		nCount		:= array_upper(array_old_node,1);
		
		-- get link info
		oldmidcount		:= 0;
		oldmidlinkid	:= '';
		oldfromlinkid	:= null;
		oldtolinkid		:= null;
		
		-- oldfromlink (for branch, first node is target node, fromlink should be auto-search)
		if rec.pictype in (1,6,8) then
			select objectid
			from road_link_4326
			where (from_node_id = array_old_node[1] and rdb_cnv_oneway_code(roadclass_c, naviclass_c, oneway_c,nopassage_c) in (1,3))
			   or (to_node_id = array_old_node[1] and rdb_cnv_oneway_code(roadclass_c, naviclass_c, oneway_c,nopassage_c) in (1,2))
			into oldfromlinkid;
		else
			select objectid
			from road_link_4326
			where (from_node_id = array_old_node[1] or to_node_id = array_old_node[1])
			  and (from_node_id = array_old_node[2] or to_node_id = array_old_node[2])
			into oldfromlinkid;
		end if;
		
		-- oldtolink
		select objectid
		from road_link_4326
		where (from_node_id = array_old_node[nCount-1] or to_node_id = array_old_node[nCount-1])
		  and (from_node_id = array_old_node[nCount] or to_node_id = array_old_node[nCount])
		into oldtolinkid;
		
		if oldfromlinkid is null or oldtolinkid is null then
			raise EXCEPTION 'illust linkrow error, maybe nodelist is not continuous.';
		end if;
		
		-- oldmidlink
		if rec.pictype in (1,6,8) then
			nIndex	:= 1;
		else
			nIndex	:= 2;
		end if;
		while nIndex < nCount-1 LOOP
			select objectid
			from road_link_4326
			where (from_node_id = array_old_node[nIndex] or to_node_id = array_old_node[nIndex])
			  and (from_node_id = array_old_node[nIndex+1] or to_node_id = array_old_node[nIndex+1])
			into oldlinkid;
			
			if oldlinkid is null then
				raise EXCEPTION 'illust linkrow error, maybe nodelist is not continuous.';
			end if;
			
			oldmidcount		:= oldmidcount + 1;
			if oldmidcount = 1 then
				oldmidlinkid	:= cast(oldlinkid as character varying);
			else
				oldmidlinkid	:= oldmidlinkid || ',' || cast(oldlinkid as character varying);
			end if;
			
			nIndex := nIndex + 1;
		END LOOP;
				
		-- mid links
		if oldmidcount > 0 then
			midlinkids			:= mid_get_new_passlid(oldmidlinkid, oldtolinkid, ',');
			array_new_midlink	:= cast(regexp_split_to_array(midlinkids, E'\\|+') as bigint[]);
			array_old_midlink	:= cast(regexp_split_to_array(oldmidlinkid, E'\\,+') as integer[]);
			midlinkcount		:= cast(array_upper(array_new_midlink, 1) as smallint);
		else
			midlinkids			:= null;
			midlinkcount		:= 0;
		end if;
		
		-- inlink, outlink, node
		if oldmidcount > 0 then
			inlinkid			:= mid_getSplitLinkID(oldfromlinkid, array_old_midlink[1], true);
			outlinkid			:= mid_getSplitLinkID(oldtolinkid, array_old_midlink[oldmidcount], true);
			nodeid				:= mid_getConnectionNodeID(inlinkid, array_new_midlink[1], true);
		else
			inlinkid	:= mid_getSplitLinkID(oldfromlinkid, oldtolinkid, true);
			outlinkid	:= mid_getSplitLinkID(oldtolinkid, oldfromlinkid, true);
			nodeid		:= mid_getConnectionNodeID(inlinkid, outlinkid, true);
		end if;
		
		INSERT INTO spotguide_tbl
			("id", 
			 "nodeid", "inlinkid", "outlinkid", 
			 "passlid", "passlink_cnt",
			 "direction", "guideattr", "namekind", "guideclass",
			 "patternno", "arrowno", "type"
			 )
			VALUES
			(
			rec.gid,
			nodeid, inlinkid, outlinkid,
			midlinkids, midlinkcount,
			0, 0, 0, 0,
			rec.picname, null, rec.pictype
			);
	END LOOP;
	
	-- for etc
	INSERT INTO spotguide_tbl
	(
		 "id", 
		 "nodeid", "inlinkid", "outlinkid", 
		 "passlid", "passlink_cnt",
		 "direction", "guideattr", "namekind", "guideclass",
		 "patternno", "arrowno", "type"
	)
	(
		select 	a.gid, 
				f.tile_node_id, c.tile_link_id as inlinkid, e.tile_link_id as outlinkid,
				null, 0,
				0, 0, 0, 0,
				a.picname, null, a.pictype
		from
		(
			select gid, cast(nodelist as integer) as nodeid, picname, pictype 
			from road_illust 
			where pictype in (9,10)
		)as a
	    left join temp_road_link as b
	    on (b.from_node_id = a.nodeid and b.oneway_c in (2,4,5)) or (b.to_node_id = a.nodeid and b.oneway_c in (1,3,5))
	    left join middle_tile_link as c
	    on b.objectid = c.old_link_id and b.split_seq_num = c.split_seq_num
	    left join temp_road_link as d
	    on (d.from_node_id = a.nodeid and d.oneway_c in (1,3,5)) or (d.to_node_id = a.nodeid and d.oneway_c in (2,4,5))
	    left join middle_tile_link as e
	    on d.objectid = e.old_link_id and d.split_seq_num = e.split_seq_num
	    left join middle_tile_node as f
        on a.nodeid = f.old_node_id
        order by a.gid
	);
	
	return 1;
END;
$$;

-----------------------------------------------------------------------------------------
-- convert shield id
-----------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION rdb_cnv_shield_id(roadcls integer) 
    RETURNS smallint
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF roadcls = 1 or roadcls = 103 THEN  -- Country road's roadno
		RETURN 496;
	ELSE 
		RETURN 503;                       -- Prefectural road's roadno
	END IF;
END;
$$;
-------------------------------------------------------------------------------------------------
-- admin
-------------------------------------------------------------------------------------------------

---------------------------------------------------------------------
---mid_make_link_list_by_nodes
--------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_make_link_list_by_nodes(nodeid_list integer[])
  RETURNS int[]
  LANGUAGE plpgsql AS
$$ 
DECLARE
rtn_link_list int[];
valid_node_cnt int;
iterator int;
linkid int;
node_link_str varchar;
pos int;
BEGIN
	rtn_link_list := array[]::int[]; 
	
	if nodeid_list is null then
		return rtn_link_list;
	end if;

	iterator := 1;

	node_link_str := array_to_string(nodeid_list, ',');
	--raise info 'node_link_str = %', node_link_str;
	 
	valid_node_cnt := array_upper(string_to_array(node_link_str,','),1);

	while iterator < valid_node_cnt loop
		select objectid into linkid
		from road_link 
		where ((from_node_id = nodeid_list[iterator] and to_node_id = nodeid_list[iterator +1]) 
		OR (to_node_id = nodeid_list[iterator] and from_node_id = nodeid_list[iterator +1]));

		if found then
			rtn_link_list := array_append(rtn_link_list,linkid);
		else 
			raise info 'It does not found link with node1 = % and node2 =%', nodeid_list[iterator],nodeid_list[iterator+1];
		end if;
		
		iterator := iterator + 1;
	end loop;

	-- test whether found link cnt is correct
	if array_upper(rtn_link_list,1) <> (valid_node_cnt - 1) then
		raise info 'Found link error, may be node dis-connect';
		rtn_link_list := array[NULL];
	end if;

	return rtn_link_list;
END;
$$;

CREATE OR REPLACE FUNCTION mid_findpasslinkbybothnodes(startnode integer, endnode integer)
  RETURNS character varying
LANGUAGE plpgsql volatile
AS $$
DECLARE
	rstPathStr  		varchar;
	rstPathCount		integer;
	rstPath		        varchar;
	nFromLink               bigint;
	tmpPathArray		varchar[];
	tmpLastNodeArray	bigint[];
	tmpLastLinkArray	bigint[];
	tmpPathCount		integer;
	tmpPathIndex		integer;
	rec        		record;
	nStartNode              integer;
	nEndNode              integer;
BEGIN
	--rstPath
	nFromLink               := -1;
	rstPathCount		:= 0;
	tmpPathArray		:= ARRAY[cast(nFromLink as varchar)];
	tmpPathCount		:= 1;
	tmpPathIndex		:= 1;
	nStartNode          := startnode;
	nEndNode          := endnode;
	tmpLastNodeArray	:= ARRAY[nStartNode];
	tmpLastLinkArray	:= ARRAY[nFromLink];
	-- search
        WHILE tmpPathIndex <= tmpPathCount LOOP
                --raise INFO 'tmpPathArray = %', tmpPathArray[tmpPathIndex];
                -----stop if tmpPath has been more than layer given
                if array_upper(regexp_split_to_array(tmpPathArray[tmpPathIndex], E'\\,+'),1) < 5 then
                        if tmpLastNodeArray[tmpPathIndex] = nEndNode then
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
                                                        SELECT objectid as nextroad,'(2)' as dir,to_node_id as nextnode
                                                        FROM road_link
                                                        where from_node_id = tmpLastNodeArray[tmpPathIndex] and oneway_c in (0,1,3,5) 

                                                        union

                                                        SELECT objectid as nextroad,'(3)' as dir,from_node_id as nextnode
                                                        FROM road_link
                                                        where to_node_id = tmpLastNodeArray[tmpPathIndex] and oneway_c in (0,2,4,5)
                                                ) as e
                                loop
                                        
                                        if 		not (rec.nextroad in (nFromLink, tmpLastLinkArray[tmpPathIndex]))
                                                and	not ((rec.nextroad||rec.dir) = ANY(regexp_split_to_array(tmpPathArray[tmpPathIndex], E'\\,+')))
                                        then
                                                tmpPathCount		:= tmpPathCount + 1;
                                                tmpPathArray		:= array_append(tmpPathArray, cast(tmpPathArray[tmpPathIndex]||','||rec.nextroad||rec.dir as varchar));
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
        if array_upper(regexp_split_to_array(rstPathStr,E'\\,+'),1) > 1 then
                return substring(rstPathStr,4);
        else
                return null;
        end if;
END;
$$;

CREATE OR REPLACE FUNCTION mid_convert_forceguide()
    RETURNS smallint
    LANGUAGE plpgsql
AS $$
DECLARE
	rec					record;
	midlinkcount		smallint;
	midlinkids			character varying; 
	array_old_midlink	integer[];
	array_new_midlink	bigint[];
	nodeid				bigint;
	inlinkid			bigint;
	outlinkid			bigint;
	TYPE_MAPPING        smallint[];
	guide_type_value        smallint;
BEGIN
	---init TYPE_MAPPING
	TYPE_MAPPING = array[0,1,2,3,3,5,7,7,8,9,9,11,13,13];
	for rec in 
		select	a.gid,
				a.objectid,
				a.fromlinkid,
			    a.tolinkid,
			    a.midcount,
			    a.midlinkid,
			    (case when a.guidetype = 0 then 255 else a.guidetype end) as guidetype,
			    0 as position_type
		from
		(
			select	a.gid, a.objectid, fromlinkid, tolinkid, midcount, midlinkid, guidetype,
					rdb_get_connected_nodeid_ipc(b.from_node_id, b.to_node_id, c.from_node_id, c.to_node_id) as node_id
			from
			(
				select	gid, objectid, fromlinkid, tolinkid, midcount, midlinkid, guidetype,
						(
						case 
						when midcount = 0 then tolinkid 
						else cast((regexp_split_to_array(midlinkid, E','))[1] as integer)
						end
						)as nextlinkid
				from road_gid
			)as a
			left join road_link as b
			on a.fromlinkid = b.objectid
			left join road_link as c
			on a.nextlinkid = c.objectid
		)as a
		order by a.objectid
	LOOP
		-- mid links
		if rec.midcount > 0 then
			midlinkids			:= mid_get_new_passlid(rec.midlinkid, rec.tolinkid, ',');
			array_new_midlink	:= cast(regexp_split_to_array(midlinkids, E'\\|+') as bigint[]);
			array_old_midlink	:= cast(regexp_split_to_array(rec.midlinkid, E'\\,+') as integer[]);
			midlinkcount		:= cast(array_upper(array_new_midlink, 1) as smallint);
		else
			midlinkids			:= null;
			midlinkcount		:= 0;
		end if;
		
		-- inlink, outlink, node
		if rec.midcount > 0 then
			inlinkid			:= mid_getSplitLinkID(rec.fromlinkid, array_old_midlink[1], true);
			outlinkid			:= mid_getSplitLinkID(rec.tolinkid, array_old_midlink[rec.midcount], true);
			nodeid				:= mid_getConnectionNodeID(inlinkid, array_new_midlink[1], true);
		else
			inlinkid	:= mid_getSplitLinkID(rec.fromlinkid, rec.tolinkid, true);
			outlinkid	:= mid_getSplitLinkID(rec.tolinkid, rec.fromlinkid, true);
			nodeid		:= mid_getConnectionNodeID(inlinkid, outlinkid, true);
		end if;
		if rec.guidetype > 13 then
			guide_type_value = rec.guidetype;
		else
			guide_type_value = TYPE_MAPPING[rec.guidetype + 1];
		end if;
		INSERT INTO force_guide_tbl
			("force_guide_id", 
			 "nodeid", "inlinkid", "outlinkid", 
			 "passlid", "passlink_cnt", "guide_type", "position_type"
			 )
			VALUES
			(
			rec.objectid,
			nodeid, inlinkid, outlinkid,
			midlinkids, midlinkcount, guide_type_value, rec.position_type
			);
	END LOOP;
	return 1;
END;
$$;


CREATE OR REPLACE FUNCTION mid_get_inout_link(
		link_id bigint, s_node bigint, e_node bigint, one_way_code smallint,
		link_id2 bigint, s_node2 bigint, e_node2 bigint, one_way_code2 smallint) 
	RETURNS bigint[]
    LANGUAGE plpgsql
    AS $$
    DECLARE
    	a bigint[];
    BEGIN
	    
	    IF one_way_code not in (2,3) or one_way_code2 not in (2,3) THEN
	    	return NULL;
	    END IF;
	    
	    IF one_way_code = 2 and one_way_code2 = 2 THEN
	    	IF s_node = e_node2 THEN
	    		a = a || link_id2 || link_id || s_node2 || e_node;
	    	END IF;
	    	
	    	IF e_node = s_node2 THEN
	    		a = a || link_id || link_id2 || s_node || e_node2;
	    	END IF;
	    END IF;
	    
	    IF one_way_code = 2 and one_way_code2 = 3 THEN
	    	IF s_node = s_node2 THEN
	    		a = a || link_id2 || link_id || e_node2 || e_node;
	    	END IF;
	    	
	    	IF e_node = e_node2 THEN
	    		a = a || link_id || link_id2 || s_node || s_node2;
	    	END IF;
	    END IF;
	    
	    IF one_way_code = 3 and one_way_code2 = 2 THEN
	    	IF s_node = s_node2 THEN
	    		a = a || link_id || link_id2 || e_node || e_node2;
	    	END IF;
	    	
	    	IF e_node = e_node2 THEN
	    		a = a || link_id2 || link_id || s_node2 || s_node;
	    	END IF;
	    END IF;
	    
	    IF one_way_code = 3 and one_way_code2 = 3 THEN
	    	IF s_node = e_node2 THEN
	    		a = a || link_id || link_id2 || e_node || s_node2;
	    	END IF;
	    	
	    	IF e_node = s_node2 THEN
	    		a = a || link_id2 || link_id || e_node2 || s_node;
	    	END IF;
	    END IF;
	    
	    return a;
	    
    END;
$$;
-------------------------------------------------------------------------------------------------
-- comp_trans_dirguide_jdb
-------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_trans_getnamearray(name_old character varying[][])
	RETURNS character varying[]
	LANGUAGE plpgsql
AS $$
DECLARE
	name_array character varying[];
	new_idx smallint;
	is_exit_num_flage boolean;
BEGIN
	new_idx := 1;
	if array_upper(name_old,1) <> 10 then
		raise EXCEPTION 'inf_dirguide row is wrong,name is %s', name_old;
	end if;
	is_exit_num_flage := false;
	for old_idx in 1..array_upper(name_old,1) loop
		if mid_isNotAllDigit(name_old[old_idx][1]) = true then
			if new_idx = 2 and is_exit_num_flage then
				name_array[1] := name_old[old_idx][1] || ',' || name_old[old_idx][2] || ',' ||name_old[old_idx][3] || ',' || 1::character varying;
				is_exit_num_flage := false;
			else
				name_array[new_idx] := name_old[old_idx][1] || ',' || name_old[old_idx][2] || ',' ||name_old[old_idx][3] || ',' || new_idx::character varying;
				new_idx := new_idx + 1;
			end if;	
		elsif mid_isNotAllDigit(name_old[old_idx][1]) = false then
			if new_idx = 1 then
				name_array[new_idx] := name_old[old_idx][1] || ',' || name_old[old_idx][2] || ',' ||name_old[old_idx][3] || ',' || new_idx::character varying;
				new_idx := new_idx + 1;
				is_exit_num_flage = true;
			end if;
		end if;
	end loop;
	if name_array is null then
		name_array[1] := 0::character varying || ',' || 0::character varying || ',' || (-1)::character varying || ',' || 1::character varying;
	end if;
	return name_array;
END;
$$;

-----------------------------------------------------------------------------------------
-- High In/Out.
-----------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_check_hwy_in_out(nnodeid bigint)
  RETURNS boolean 
  LANGUAGE plpgsql AS
$$ 
DECLARE
BEGIN
	if nNodeID is null then
		return false;
	end if;
	
	PERFORM  link_id
	  from link_tbl
	  where ((s_node = nNodeID) or (e_node = nNodeID))
	         and (road_type not in (0, 1) and toll <> 1);

	if not found then
		return False;
	end if;

	return true;
END;
$$;
-------------------------------------------------------------------------------------------------
-- park link/node/polygon function
-------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_get_link_node_array(node_array integer[])
	RETURNS integer[]
	LANGUAGE PLPGSQL
AS $$
DECLARE
	rec record;
	icurrent integer;
	icount integer;
	link_array integer[];
BEGIN
	link_array := null;
	icurrent := 1;
	icount := array_upper(node_array,1);
	while icurrent <= icount loop
		for rec in select a.objectid,(case when from_node_id = node_array[icurrent] then to_node_id
																else from_node_id end) as t_node
							 from road_link_4326 as a
							 left join parking_link_attr as b
							 on a.objectid = b.objectid
							 where ((from_node_id = node_array[icurrent]) or (to_node_id = node_array[icurrent]))
							 			and b.objectid is not null
		loop
			if rec.objectid is not null then
			
				if link_array is null then
					link_array := array[rec.objectid];
				else
					if rec.objectid = any(link_array) then
						continue;
					else
						link_array := array_append(link_array,rec.objectid);
					end if;
				end if;
				
				if rec.t_node = any(node_array) then
					continue;
				else
					icount := icount + 1;
					node_array[icount] := rec.t_node;
				end if;
				
			end if;
		end loop;
		icurrent := icurrent + 1;
	end loop;
	
	return link_array;
END;
$$;

CREATE OR REPLACE FUNCTION mid_get_link_node_parking_id()
	RETURNS SMALLINT
	LANGUAGE PLPGSQL
AS $$
DECLARE
	rec record;
	link_array integer[];
	node_array integer[];
BEGIN
	for rec in select parking_id,array_agg(objectid) as node_array_temp
						 from parking_node_attr
						 group by parking_id
	loop
		if rec.parking_id	is not null then
			--link
			link_array := mid_get_link_node_array(rec.node_array_temp);
			insert into temp_park_link(link_id, parking_id)
			(
				select unnest(link_array),rec.parking_id
			);
		end if;
	end loop;
	
	insert into temp_park_link(link_id, parking_id)
	(
		select objectid,-1
		from parking_link_attr as a
		left join temp_park_link as b
		on a.objectid = b.link_id
		where b.link_id is null
	);
	
	
	insert into temp_park_node(node_id,parking_id,the_geom)
	(
		select from_node_id, parking_id,c.the_geom
		from temp_park_link as a
		left join road_link_4326 as b
		on a.link_id = b.objectid
		left join road_node_4326 as c
		on b.from_node_id = c.objectid
		
		union
		
		select to_node_id, parking_id,c.the_geom
		from temp_park_link as a
		left join road_link_4326 as b
		on a.link_id = b.objectid
		left join road_node_4326 as c
		on b.to_node_id = c.objectid
	);

	return 0;
END;
$$;
CREATE OR REPLACE FUNCTION mid_make_park_link_tbl()
	RETURNS SMALLINT
	LANGUAGE PLPGSQL
AS $$
DECLARE
BEGIN
	insert into park_link_tbl(park_link_id,park_link_oneway,park_s_node_id,park_e_node_id,park_link_length,
														park_link_type,park_link_connect_type,park_floor,park_link_lean,
														park_link_toll,park_link_add1,park_link_add2,park_region_id,the_geom)
	select a.link_id as park_link_id,
				 a.one_way_code as park_link_oneway,
				 a.s_node as park_s_node_id,
				 a.e_node as park_e_node_id,
				 a.length as park_link_length,
				(case when c.parkinglinkclass_c = 4 then 0 else 1 end)::smallint as park_link_type,
				 c.parkinglinkclass_c::smallint as park_link_connect_type,
				 c.floor_level + c.middlefloor_level as park_floor,
				 0::smallint as park_link_lean,
				 a.toll as park_link_toll,
				 null as park_link_add1,
				 null as park_link_add2,
				 d.parking_id as park_region_id,
				 (ST_Dump(a.the_geom)).geom as the_geom
	from link_tbl as a
	left join middle_tile_link as b
	on a.link_id = b.tile_link_id
	left join parking_link_attr as c
	on b.old_link_id = c.objectid
	left join temp_park_link as d
	on b.old_link_id = d.link_id
	where d.link_id is not null;
	
	return 0;
END;
$$;
CREATE OR REPLACE FUNCTION mid_make_park_polygon_tbl()
	RETURNS SMALLINT
	LANGUAGE PLPGSQL
AS $$
DECLARE
BEGIN
	insert into park_polygon_tbl(park_region_id,park_type,park_toll,park_map_ID,park_name,the_geom)
	select objectid as park_region_id,
				parkingclass_c as park_type,
				0::smallint as park_toll,
				null as park_map_ID,
				null as park_name,
				the_geom
	from parking_shape_4326
	order by objectid;
	return 0;
END;
$$;

CREATE OR REPLACE FUNCTION mid_make_park_node_tbl()
	RETURNS SMALLINT
	LANGUAGE PLPGSQL
AS $$
DECLARE
BEGIN
	insert into park_node_tbl(park_node_id,base_node_id,park_node_type,node_name,park_region_id,the_geom)
	select distinct b.tile_node_id as park_node_id,
				(case when c.node_id is null then null else d.tile_node_id end) as base_node_id,
				(case when c.node_id is null then 0 
							when link1.park_link_id is not null then 1 
							when link2.park_link_id is not null then 2 
							when link3.park_link_id is not null then 3
				 			else 3 end)::smallint as park_node_type,
				null as node_name,
				a.parking_id as park_region_id,
				a.the_geom as the_geom
	from temp_park_node as a
	left join middle_tile_node as b
	on a.node_id = b.old_node_id
	left join temp_park_node_connect as c
	on a.node_id = c.node_id
	left join middle_tile_node as d
	on c.node_id = d.old_node_id
	left join park_link_tbl as link1
	on ((link1.park_s_node_id = d.tile_node_id) or (link1.park_e_node_id = d.tile_node_id)) 
			and link1.park_link_connect_type = 1
		left join park_link_tbl as link2
	on ((link2.park_s_node_id = d.tile_node_id) or (link2.park_e_node_id = d.tile_node_id)) 
			and link2.park_link_connect_type = 2
	left join park_link_tbl as link3
	on ((link3.park_s_node_id = d.tile_node_id) or (link3.park_e_node_id = d.tile_node_id)) 
			and link1.park_link_connect_type = 3
	order by a.parking_id;
	return 0;
END;
$$;
CREATE OR REPLACE FUNCTION mid_node_onnection_type(link_array integer[])
	RETURNS boolean
	LANGUAGE PLPGSQL
AS $$
DECLARE
	link_array_num integer;
BEGIN
	link_array_num := array_upper(link_array,1);
	for link_array_num_idx in 1..link_array_num loop
		if link_array[link_array_num_idx] != 401 then
			return false;
		end if;
	end loop;
	
	return true;
END;
$$;


-----------------------------------------------------------------------------------------
-- Get Middle Link ID by ORG Link ID
-----------------------------------------------------------------------------------------
create or replace function mid_get_midle_linkid_by_org_linkid(org_linkid bigint)
  RETURNS bigint[]
  LANGUAGE plpgsql
AS $$
DECLARE
  mid_link_array    bigint[];
  rec               record;
BEGIN
	mid_link_array = array[]::bigint[];
	for rec in execute
		'
		 select middle_link_id, split_seq_num, sub_index, link_num, merge_index
		 from (
			SELECT  a.objectid as org_link_id, 
				(case 
				  when d.link_id is not null then d.link_id
				  when c.link_id is not null then c.link_id 
				  else b.tile_link_id 
				  end ) as middle_link_id,
				a.split_seq_num,
				sub_count,
				sub_index,
				link_num,
				merge_index
			  FROM (
				select objectid, from_node_id, to_node_id, split_seq_num
				from temp_road_link
				where objectid = '|| org_linkid ||'
			  ) as a
			  left join middle_tile_link as b
			  on a.objectid = b.old_link_id and a.split_seq_num = b.split_seq_num
			  left join temp_split_newlink as c
			  on b.tile_link_id = c.old_link_id
			  left join temp_merge_link_mapping as d
			  on (c.link_id = d.merge_link_id) or (b.tile_link_id = d.merge_link_id)
		  ) as middle
		  --on org.objectid = middle.org_link_id
		  order by org_link_id, split_seq_num, sub_index, merge_index
		'
	loop
		mid_link_array := mid_link_array || rec.middle_link_id;
	end loop;

	if array_upper(mid_link_array, 1) = 0 then
		raise exception 'Can not find middle link id for %.', org_linkid; 
		return NULL;
	end if;
	return mid_link_array;
END;
$$;
CREATE OR REPLACE FUNCTION mid_replace_adminstr(name character varying)
	returns character varying
	language plpgsql
AS $$
declare
	ipos integer;
begin
	ipos = position('_' in name);
	if ipos = 0 then
		return name;
	elsif ipos = 1 then
		return substring(name,2,length(name)-1);
	elsif ipos = length(name) then
		return substring(name,1,length(name)-1);
	end if;
	return substring(name,1,ipos-1)||' '||substring(name,ipos+1,length(name)-ipos);
end;
$$;

-----------------------------------------------------------------------------------------
-- convert road code
-----------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_convert_road_code(code integer)
	RETURNS INTEGER
	LANGUAGE plpgsql volatile
AS $$
BEGIN
        RETURN code / 1000;
END;
$$;

-----------------------------------------------------------------------------------------
-- convert road seq
-----------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_convert_road_seq(code integer)
	RETURNS SMALLINT
	LANGUAGE plpgsql volatile
AS $$
BEGIN
        RETURN code % 1000;
END;
$$;
------------------------------------------------------------------------------------------
--hwynode2towardname
------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_change_symbol(strlib varchar)
	RETURNS varchar
	LANGUAGE plpgsql
AS $$
DECLARE
	strlib_temp varchar;
	strlib_num integer;
BEGIN
	if strlib is null then
		return null;
	end if;
	
	strlib_num := length(strlib);
	--delete '{}'
	strlib_temp := substr(strlib,2,strlib_num - 2);
	--repalce ',' to '|'
	return replace(strlib_temp,',','|');
END;
$$;
--return last node,if inode_id is not in path,return inode_id
CREATE OR REPLACE FUNCTION mid_move_node(inode_id bigint, strnode_lid varchar)
 RETURNS bigint
 LANGUAGE plpgsql
AS $$
DECLARE
	node_array bigint[];
	node_array_num integer;
BEGIN
	node_array := regexp_split_to_array(strnode_lid,E'\\|+')::bigint[];
	node_array_num := array_upper(node_array,1);
	
	if inode_id = any(node_array) then
		return node_array[node_array_num];
	else
		return inode_id;
	end if;
END;
$$;
--for in path
CREATE OR REPLACE FUNCTION mid_move_node_array(inode_id bigint, strnode_lid varchar)
 RETURNS bigint[]
 LANGUAGE plpgsql
AS $$
DECLARE	
	curs1 refcursor;
	rec1 record;
	curs2 refcursor;
	rec2 record;
	
	node_array_one bigint[];
	node_array_one_num integer;
	inode_id_temp bigint;
	
	node_array varchar[];
	ilastnum integer;
	iallnum integer;
BEGIN
	node_array_one := regexp_split_to_array(strnode_lid,E'\\|+')::bigint[];
	node_array_one_num := array_upper(node_array_one,1);
	
	if inode_id = any(node_array_one) then
		return node_array_one;
	else
		iallnum := 1;
		node_array[iallnum] := strnode_lid;
		ilastnum := 1;
		while ilastnum <= iallnum loop
			node_array_one := regexp_split_to_array(node_array[ilastnum],E'\\|+')::bigint[];
			node_array_one_num := array_upper(node_array_one,1);
			
			open curs1 for select s_node, e_node
									from link_tbl
									where ((one_way_code in (1,3)) and (s_node = node_array_one[node_array_one_num])) 
									or ((one_way_code in (1,2)) and (e_node = node_array_one[node_array_one_num]));
			fetch curs1 into rec1;
			while rec1.s_node is not null loop
				if rec1.s_node = node_array_one[node_array_one_num] then
					inode_id_temp := rec1.e_node;
				else
					inode_id_temp := rec1.s_node;
				end if;
				
				if inode_id_temp = inode_id then
					close curs1;
					return node_array_one || array[inode_id];
				else
					iallnum := iallnum + 1;
					node_array[iallnum] := node_array[iallnum] || '|' || inode_id_temp::varchar;
				end if;
				
				fetch curs1 into rec1;
			end loop;
			close curs1;
			ilastnum := ilastnum + 1;
			
			if iallnum > 10 then
				exit;
			end if;
			
		end loop;
	end if;
	
	node_array_one := regexp_split_to_array(strnode_lid,E'\\|+')::bigint[];
	
	return node_array_one;
END;
$$;
--jude node in mid_road_hwynode_middle_nodeid
CREATE OR REPLACE FUNCTION mid_jude_nodename(inode_id bigint)
 RETURNS boolean
 LANGUAGE plpgsql
AS $$
DECLARE
	strname varchar;
	inum integer;
	
	curs1 refcursor;
	rec1 record;
BEGIN
	inum = 1;
	open curs1 for select name_kanji
						from mid_road_hwynode_middle_nodeid
						where inout_c = 1 and node_id = inode_id;
	fetch curs1 into rec1;
	while rec1.name_kanji is not null loop
		if inum = 1 then
			strname := rec1.name_kanji;
		else
			if strname <> rec1.name_kanji then
				close curs1;
				return false;
			end if;
		end if;
		
	inum := inum + 1;
	fetch curs1 into rec1;
	end loop;
	close curs1;
	
	return true;
END;
$$;
--get first link
CREATE OR REPLACE FUNCTION mid_get_first_in_link(inode_id bigint,strpath varchar)
 RETURNS varchar
 LANGUAGE plpgsql
AS $$
DECLARE
	node_array bigint[];
	node_array_num integer;
BEGIN
	node_array := regexp_split_to_array(strpath,E'\\|+')::bigint[];
	node_array_num := array_upper(node_array,1);
	
	for node_array_num_idx in 1..node_array_num-1 loop
		if inode_id = node_array[node_array_num_idx] then
			return array_to_string(node_array[1:node_array_num_idx+1],'|');
		end if;
	end loop;
	
	return null;
END;
$$;

--outlinks num in link_tbl
CREATE OR REPLACE FUNCTION mid_get_single_out_node_num(inode_id bigint)
 RETURNS integer
 LANGUAGE plpgsql
AS $$
DECLARE
	inum integer;
BEGIN
	select num into inum
	from temp_node_num_for_towardname
	where node_id = inode_id;
	
	return inum;
END;
$$;

--outlinks num in this path
CREATE OR REPLACE FUNCTION mid_get_node_inarray(inode_id bigint,node_array varchar[])
 RETURNS integer
 LANGUAGE plpgsql
AS $$
DECLARE
	inum integer;
	link_array bigint[];
BEGIN
	--get path all links
	link_array := mid_get_all_link(node_array);
	
	select count(*) into inum
	from
	(
		select link_id,s_node as node_id
		from link_tbl
		where link_id = any(link_array) and one_way_code in(1,2)
		
		union
		
		select link_id,e_node as node_id
		from link_tbl
		where link_id = any(link_array) and one_way_code in(1,3)
	)temp
	where node_id = inode_id
	group by node_id;
	
	return inum;
END;
$$;
CREATE OR REPLACE FUNCTION mid_jude_path_appear(strnode varchar,node_array varchar[])
 RETURNS boolean
 LANGUAGE plpgsql
AS $$
DECLARE
	node_array_num integer;
BEGIN
	if node_array is null then
		return true;
	end if;
	
	node_array_num := array_upper(node_array,1);
	for node_array_num_idx in 1..node_array_num loop
		if strpos(node_array[node_array_num_idx],strnode) > 0 then
			return false;
		end if;
	end loop;
	return true;

END;
$$;

--
CREATE OR REPLACE FUNCTION mid_no_merge_node(node_array varchar[])
 RETURNS bigint[]
 LANGUAGE plpgsql
AS $$
DECLARE
	node_array_num integer;
	node_array_one bigint[];
	node_array_one_num integer;
	
	node_connect_link_num integer;
	node_no_merge_array bigint[];
BEGIN
	--case path1:node1->node2->node3->node4
	--id node3 have outlink not in path1,node3 is no merge node
	--so,node1 and node2 are no merge node
	if node_array is null then
		return null;
	end if;

	node_array_num := array_upper(node_array,1);
	for node_array_num_idx in 1..node_array_num loop
		node_array_one := regexp_split_to_array(node_array[node_array_num_idx],E'\\|+')::bigint[];
		node_array_one_num := array_upper(node_array_one,1);
		--last second node
		node_array_one_num := node_array_one_num - 2;
		while node_array_one_num > 1 loop
			--outlinks num in link_tbl
			node_connect_link_num := mid_get_single_out_node_num(node_array_one[node_array_one_num]);
			if node_array_one_num > 1 then
				--outlinks num in this path
				if node_connect_link_num > mid_get_node_inarray(node_array_one[node_array_one_num],node_array) then
					if node_no_merge_array is null then
						node_no_merge_array := node_array_one[1:node_array_one_num];
					else
						node_no_merge_array := node_no_merge_array || node_array_one[1:node_array_one_num];
					end if;
					exit;
				end if;
			end if;
			node_array_one_num := node_array_one_num - 1;
		end loop;
	end loop;

	return node_no_merge_array;
END;
$$;

CREATE OR REPLACE FUNCTION mid_delete_long_path(node_array varchar[])
 RETURNS varchar[]
 LANGUAGE plpgsql
AS $$
DECLARE
	node_array_num integer;
	node_array_one varchar;
	node_array_new varchar[];
	node_array_new_num integer;
	
	node_array_num_temp integer;
	node_array_one_temp varchar;
	flage1 boolean;
BEGIN
	--case when 
	--path1:link1->link2->link3
	--path2:link1->link2->link3->link4
	--delete path2
	if node_array is null then
		return null;
	end if;
	
	node_array_new_num := 1;
	node_array_num := array_upper(node_array,1);
	node_array_num_temp := node_array_num;
	for node_array_num_idx in 1..node_array_num loop
		node_array_one := node_array[node_array_num_idx];
		flage1 := true;
		for node_array_num_temp_idx in 1..node_array_num_temp loop
			node_array_one_temp := node_array[node_array_num_temp_idx];
			if node_array_num_temp_idx <> node_array_num_idx
				and strpos(node_array_one,node_array_one_temp) = 1
				and strpos(node_array_one_temp,node_array_one) = 0 then
				flage1 := false;
				exit;
			end if;
		end loop;
		if flage1 then
			node_array_new[node_array_new_num] := node_array[node_array_num_idx];
			node_array_new_num := node_array_new_num + 1;
		end if;
	end loop;
	
	return node_array_new;

END;
$$;

--merge same path  
CREATE OR REPLACE FUNCTION mid_cut_path(node_array varchar[])
 RETURNS varchar[]
 LANGUAGE plpgsql
AS $$
DECLARE
	node_array_num integer;
	node_array_one bigint[];
	node_array_one_num integer;
	
	node_array_temp varchar[];
	node_connect_link_num integer;
	node_array_no_merge bigint[];
BEGIN
	if node_array is null then
		return null;
	end if;
	
	--delete long path
	node_array := mid_delete_long_path(node_array);
	--get no merge node
	node_array_no_merge := mid_no_merge_node(node_array);
	
	--cut path
	node_array_num := array_upper(node_array,1);
	for node_array_num_idx in 1..node_array_num loop
		node_array_one := regexp_split_to_array(node_array[node_array_num_idx],E'\\\|+')::bigint[];
		node_array_one_num := array_upper(node_array_one,1);
		--last second node
		node_array_one_num := node_array_one_num - 2;
		while node_array_one_num > 1 loop
			if node_array_one[node_array_one_num] = any(node_array_no_merge) then
				exit;
			end if;
			node_array_one_num := node_array_one_num - 1;
		end loop;
		node_array_temp[node_array_num_idx] := array_to_string(node_array_one[1:node_array_one_num+1],'|');
	end loop;

	return node_array_temp;
END;
$$;

--delete Cycling path
CREATE OR REPLACE FUNCTION mid_delete_cycle(node_array varchar[])
 RETURNS varchar[]
 LANGUAGE plpgsql
AS $$
DECLARE
	node_array_num integer;
	node_array_one bigint[];
	node_array_one_num integer;
	
	node_array_temp varchar[];
	node_array_temp_num integer;
	flage_temp boolean;
BEGIN
	if node_array is null then
		return null;
	end if;
	
	node_array_temp_num := 0;
	node_array_num := array_upper(node_array,1);
	for node_array_num_idx in 1..node_array_num loop
		flage_temp := true;
		node_array_one := regexp_split_to_array(node_array[node_array_num_idx],E'\\|+')::bigint[];
		node_array_one_num := array_upper(node_array_one,1);
		for node_array_one_num_idx in 1..node_array_one_num - 1 loop
			--jude node have appear in [node+1,last_node]
			if mid_onedata_in_array(node_array_one[node_array_one_num_idx],
						node_array_one[node_array_one_num_idx +1:node_array_one_num]) > 0 then
				flage_temp := false;
				exit;
			end if;
		end loop;
		--if not Cycling,save path
		if flage_temp then
			node_array_temp_num := node_array_temp_num + 1;
			node_array_temp[node_array_temp_num] := node_array[node_array_num_idx];
		end if;
	end loop;

	return node_array_temp;
END;
$$;

--multi path
CREATE OR REPLACE FUNCTION mid_get_outmulti_path(node_id_s bigint,road_code integer, road_seq integer, s_type integer,
																node_array varchar[],type_array integer[])
 RETURNS varchar[]
 LANGUAGE plpgsql
AS $$
DECLARE
	node_array_temp varchar[];
	node_array_temp_num integer;
	node_array_num integer;
	node_array_one bigint[];
	node_array_one_num integer;
	node_array_real_num integer;
BEGIN
	--jude martic in path
	if s_type = 7 and (7 = all(type_array)) = false then
		return null;
	end if;
	if node_array is null then
		return null;
	end if;
	--add path type
	--if flage_temp then
	node_array := mid_add_out_type(node_id_s,road_code,road_seq,s_type,node_array,type_array);
	--else
		--node_array := mid_path_add_type(road_code,road_seq,node_array,1);
	--end if;
	
	if node_array is null then
		return null;
	end if;
	
	--cut path
	node_array_temp_num := 0;
	node_array_num := array_upper(node_array,1);
	for node_array_num_idx in 1..node_array_num loop
		node_array_one := regexp_split_to_array(node_array[node_array_num_idx],E'\\|+')::bigint[];
		node_array_one_num := array_upper(node_array_one,1);
		if node_array_one[node_array_one_num] = 1 then
			--
			--for node_array_one_num_idx in 2..node_array_one_num loop
				--if mid_onenode_in_allpath(node_array_one[node_array_one_num_idx],node_array) then
					--node_array_real_num := node_array_one_num_idx;
					--exit;
				--end if;
				--node_array_real_num := node_array_one_num_idx;
			--end loop;
			node_array_temp_num := node_array_temp_num + 1;
			node_array_temp[node_array_temp_num] := array_to_string(node_array_one[1:node_array_one_num-1],'|');
		end if;
	end loop;

	
	--delete Cycling path
	node_array_temp := mid_delete_cycle(node_array_temp);
	
	return node_array_temp;
END;
$$;
--get access position in path 
CREATE OR REPLACE FUNCTION mid_access_in_path(node_array bigint[],access_array bigint[])
 RETURNS integer
 LANGUAGE plpgsql
AS $$
DECLARE
	node_array_num integer;
	node_get_num integer;
BEGIN
	node_get_num := 0;
	if node_array && access_array then
		node_array_num := array_upper(node_array,1);
		for node_array_num_idx in 1..node_array_num loop
			if mid_onedata_in_array(node_array[node_array_num_idx],access_array) > 0 then
				node_get_num := node_array_num_idx;
				exit;
			end if;
		end loop;
	end if;
	
	return node_get_num;
END;
$$;
--ADD out type
CREATE OR REPLACE FUNCTION mid_get_access_byrode(iroad_code integer, iroad_seq integer,in_out integer)
 RETURNS bigint[]
 LANGUAGE plpgsql
AS $$
DECLARE
	curs1 refcursor;
	rec1 record;
	
	access_array bigint[];
	access_array_num integer;
BEGIN
	access_array_num := 0;
	open curs1 for select node_id as node_id
							 from mid_access_point_middle_nodeid 
							 where road_code = iroad_code and road_seq = iroad_seq and inout_c in(0,in_out);
	fetch curs1 into rec1;
	while rec1.node_id is not null loop
		access_array_num := access_array_num + 1;
		access_array[access_array_num] := rec1.node_id;
		fetch curs1 into rec1;
	end loop;
	close curs1;
	
	if access_array_num = 0 then
		return null;
	end if;
	
	return access_array;
END;
$$;
--ADD out type
CREATE OR REPLACE FUNCTION mid_add_out_type(node_id_s bigint,iroad_code integer, iroad_seq integer, s_type integer,
																node_array varchar[],type_array integer[])
 RETURNS varchar[]
 LANGUAGE plpgsql
AS $$
DECLARE
	node_array_num integer;
	node_array_one bigint[];
	node_array_one_num integer;
	node_array_other varchar[];
	node_array_other_num integer;
	node_array_other_one bigint[];
	node_array_other_one_num integer;
	
	access_array bigint[];
	node_array_new varchar[];
	node_array_new_num integer;
	
BEGIN
	--get all ccess_point
	access_array := mid_get_access_byrode(iroad_code,iroad_seq,2);
	if access_array is null then
		return null;
	end if;
	
	--cut path
	node_array_new_num := 1;
	node_array_num := array_upper(node_array,1);
	for node_array_num_idx in 1..node_array_num loop
		node_array_one := regexp_split_to_array(node_array[node_array_num_idx],E'\\|+')::bigint[];
		if s_type in (1,2) then
			--path in this node,same path
			node_array_one_num := mid_access_in_path(node_array_one,access_array);
			if  node_array_one_num > 0 then
				node_array_new[node_array_new_num] := array_to_string(node_array_one[1:node_array_one_num],'|') || '|' || 1::varchar;
			else
				node_array_new[node_array_new_num] := node_array[node_array_num_idx] || '|' || 0::varchar;
			end if;
		else
			node_array_new[node_array_new_num] := node_array[node_array_num_idx] || '|' || 1::varchar;
		end if;
		node_array_new_num := node_array_new_num + 1;	
	end loop;
		
	
	select array_agg(mid_change_symbol(node_lid)) into node_array_other
	from mid_hwy_ic_path
	where s_node_id = node_id_s and (s_road_code <> iroad_code or s_road_point <> iroad_seq or s_facilcls <> s_type)
	group by s_node_id;
	--path in this node,no same path
	node_array_other_num := array_upper(node_array_other,1);
	for node_array_other_num_idx in 1..node_array_other_num loop
		node_array_other_one := regexp_split_to_array(node_array_other[node_array_other_num_idx],E'\\|+')::bigint[];
		if s_type in (1,2) then
			--cut path if SA/PA in path
			node_array_other_one_num := mid_access_in_path(node_array_other_one,access_array);
			if  node_array_other_one_num > 0 then
				node_array_new[node_array_new_num] := array_to_string(node_array_other_one[1:node_array_other_one_num],'|') || '|' || 1::varchar;
			else
				node_array_new[node_array_new_num] := node_array_other[node_array_other_num_idx] || '|' || 0::varchar;
			end if;
		else
			node_array_new[node_array_new_num] := node_array_other[node_array_other_num_idx] || '|' || 0::varchar;
		end if;
		node_array_new_num := node_array_new_num + 1;	
	end loop;
	
	--cut path for SA/PA
	if 1 = any(type_array) or 2 = any(type_array) then
		return mid_get_sapa_array(s_type,type_array,node_array_new);
	end if;
	
	return node_array_new;
END;
$$;
CREATE OR REPLACE FUNCTION mid_turn_array(node_array bigint[])
 RETURNS bigint[]
 LANGUAGE plpgsql
AS $$
DECLARE
	node_array_num integer;
	node_array_new bigint[];
	node_array_new_num integer;
BEGIN
	if node_array is null then
		return null;
	end if;
	
	node_array_new_num := 0;
	node_array_num := array_upper(node_array,1);
	while node_array_num > 0 loop
		node_array_new_num := node_array_new_num + 1;
		node_array_new[node_array_new_num] = node_array[node_array_num];
		node_array_num := node_array_num - 1;
	end loop;
	
	return node_array_new;
END;
$$;

--SAPA path have martic
CREATE OR REPLACE FUNCTION mid_sapa_smartic_multi(node_arraytype varchar[])
 RETURNS varchar[]
 LANGUAGE plpgsql
AS $$
DECLARE
	curs1 refcursor;
	rec1 record;
	
	access_array bigint[];
	access_array_num integer;
	access_array_one bigint[];
	access_array_one_num integer;
	
	node_arraytype_num integer;
	node_arraytype_one bigint[];
	node_arraytype_one_num integer;
	
	node_arraytype_new varchar[];
	
BEGIN
	
	node_arraytype_one := regexp_split_to_array(node_arraytype[1],E'\\|+')::bigint[];
	
	access_array_num := 0;
	--jude road_code,road_seq
	open curs1 for select node_id,road_code,road_seq
							 from mid_road_hwynode_middle_nodeid 
							 where node_id = node_arraytype_one[1] and facilclass_c = 7;
	fetch curs1 into rec1;
	while rec1.node_id is not null loop
		access_array_one := mid_get_access_byrode(rec1.road_code,rec1.road_seq,2);
		if access_array_one is not null then
			access_array_one_num := array_upper(access_array_one,1);
			for access_array_one_num_idx in 1..access_array_one_num loop
				access_array_num := access_array_num + 1;
				access_array[access_array_num] := access_array_one[access_array_one_num_idx];
			end loop;
		end if;
		fetch curs1 into rec1;
	end loop;
	close curs1;
	
	--
	node_arraytype_new := null;
	node_arraytype_num := array_upper(node_arraytype,1);
	for node_arraytype_num_idx in 1..node_arraytype_num loop
		node_arraytype_one := regexp_split_to_array(node_arraytype[node_arraytype_num_idx],E'\\|+')::bigint[];
		node_arraytype_one_num := array_upper(node_arraytype_one,1);
		if node_arraytype_one[node_arraytype_one_num] = 0 then
			if mid_access_in_path(node_arraytype_one[1:node_arraytype_one_num-1],access_array) > 0 then
				return node_arraytype;
				exit;
			else
				node_arraytype_new[node_arraytype_num_idx] := array_to_string(node_arraytype_one[1:node_arraytype_one_num-1],'|')|| '|' ||1::varchar;
			end if;
		else
			node_arraytype_new[node_arraytype_num_idx] := node_arraytype[node_arraytype_num_idx];
		end if;
	end loop;
	
	return node_arraytype_new;
END;
$$;
--jude link_type  = 7 in this path
CREATE OR REPLACE FUNCTION mid_onenode_in_allpath_for7type(node_one bigint,node_array varchar[])
 RETURNS BOOLEAN
 LANGUAGE plpgsql
AS $$
DECLARE
	node_array_num integer;
	
	node_array_one bigint[];
	node_array_one_num integer;
BEGIN
	if node_array is null then
		return 0;
	end if;
	node_array_num := array_upper(node_array,1);
	
	for node_array_num_idx in 1..node_array_num loop
		node_array_one := regexp_split_to_array(node_array[node_array_num_idx],E'\\|+')::bigint[];
		if mid_onedata_in_array(node_one,node_array_one) > 0 and node_array_one[array_upper(node_array_one,1)] = 1 then
			return true;
		end if;
	end loop;
	
	return false;
END;
$$;
--for SAPA path
CREATE OR REPLACE FUNCTION mid_get_sapa_array(s_type integer,type_array integer[],node_arraytype varchar[])
	RETURNS varchar[]
	LANGUAGE plpgsql
AS $$
DECLARE
	node_arraytype_num integer;
	node_arraytype_new varchar[];
	
	node_array_one bigint[];
	node_array_one_num integer;
	link_7type_position integer;
BEGIN
	if node_arraytype is null then
		return null;
	end if;
	
	--smartic path
	if 7 = any(type_array) then
		 return mid_sapa_smartic_multi(node_arraytype);
	end if;
	
	--if ink_type = 7 get path
	node_arraytype_num := array_upper(node_arraytype,1);
	for node_arraytype_num_idx in 1..node_arraytype_num loop
		node_array_one := regexp_split_to_array(node_arraytype[node_arraytype_num_idx],E'\\|+')::bigint[];
		node_array_one_num := array_upper(node_array_one,1);
		link_7type_position := mid_get_link_7type(node_array_one[1:node_array_one_num-1]);
		if link_7type_position > 0 then
			if (s_type = 1 or s_type = 2) and mid_onenode_in_allpath_for7type(node_array_one[link_7type_position-1],node_arraytype) then
				node_arraytype_new[node_arraytype_num_idx] := array_to_string(node_array_one[1:link_7type_position],'|')|| '|' ||1::varchar;
			else
				node_arraytype_new[node_arraytype_num_idx] := array_to_string(node_array_one[1:link_7type_position],'|')|| '|' ||0::varchar;
			end if;
		else
			node_arraytype_new[node_arraytype_num_idx] := node_arraytype[node_arraytype_num_idx];
		end if;
	end loop;
	
	return node_arraytype_new;
END;
$$;
CREATE OR REPLACE FUNCTION mid_judge_link_in_array(s_node bigint,e_node bigint,node_lib varchar[])
	RETURNS boolean
	LANGUAGE plpgsql
AS $$
DECLARE
	strs_e varchar;
	stre_s varchar;
	node_lib_num integer;
BEGIN
	if node_lib is null then
		return false;
	end if;
	
	strs_e := s_node::varchar || '|' || e_node::varchar;
	stre_s := e_node::varchar || '|' || s_node::varchar;
	
	node_lib_num := array_upper(node_lib,1);
	for node_lib_num_idx in 1..node_lib_num loop
		if strpos(node_lib[node_lib_num_idx],strs_e) = 1 or strpos(node_lib[node_lib_num_idx],stre_s) = 1 then
			return false;
		end if;
	end loop;

	return true;
END;
$$;
--link_type = 7
CREATE OR REPLACE FUNCTION mid_get_link_7type(node_array bigint[])
	RETURNS integer
	LANGUAGE plpgsql
AS $$
DECLARE
	type_array integer[];
	type_array_num integer;
BEGIN
	if node_array is null then
		return 0;
	end if;
	type_array := mid_get_link_array_type(node_array);
	if 7 = any(type_array) then
		type_array_num := array_upper(type_array,1);
		for type_array_num_idx in 1..type_array_num loop
			if type_array[type_array_num_idx] = 7 then
				return type_array_num_idx + 1;
			end if;
		end loop;
	else
		return 0;
	end if;
	
	return 0;
END;
$$;
CREATE OR REPLACE FUNCTION mid_onedata_in_array(nodeid bigint,node_array bigint[])
	RETURNS integer
	LANGUAGE plpgsql
AS $$
DECLARE
	node_array_num integer;
	data_num integer;
BEGIN
	--if node no in path, return 0
	if nodeid = any(node_array) then
		node_array_num := array_upper(node_array,1);
		for node_array_num_idx in 1..node_array_num loop
			if nodeid = node_array[node_array_num_idx] then
				data_num := node_array_num_idx;
				exit;
			end if;
		end loop;
	else
		data_num := 0;
	end if;
	return data_num;
END;
$$;
CREATE OR REPLACE FUNCTION mid_get_middle_intersect_node(fromlink bigint, tolink bigint)
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
		from link_tbl where link_id = fromLink;
	if not found then
		return NULL;
	end if;
	
	select s_node, e_node into fromnodeid2, tonodeid2 
		from link_tbl where link_id = toLink;
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
CREATE OR REPLACE FUNCTION mid_get_inlink_array(inode_id bigint,ilink_id bigint)
	RETURNS varchar[]
	LANGUAGE plpgsql
AS $$
DECLARE
	curs1 refcursor;
	rec1 record;
	curs2 refcursor;
	rec2 record;
	
	node_temp bigint;
	node_array bigint[];
	node_array_num integer;
	
	link_array varchar[];
	link_array_num integer;
	link_array_one bigint[];
	link_array_one_num integer;

	
	flage_temp boolean;
	ilast_num integer;
	icurrent_num integer;
	
BEGIN
	node_array_num := 1;
	node_array[node_array_num] := inode_id;
	link_array_num := 1;
	link_array[link_array_num] := inode_id::varchar || '|' ||ilink_id::varchar;

	
	flage_temp := true;
	ilast_num := 1;
	while flage_temp loop
		flage_temp := false;
		icurrent_num := link_array_num;
		while ilast_num <= icurrent_num loop
			link_array_one := (regexp_split_to_array(link_array[ilast_num],E'\\|+'))::bigint[];
			link_array_one_num := array_upper(link_array_one,1);
			
			open curs1 for select link_id,link_type,s_node,e_node
					 				from link_tbl
					where link_id = link_array_one[link_array_one_num];
			fetch curs1 into rec1;
			if rec1.link_id is null then
				close curs1;
				return null;
			end if;
			if rec1.link_type = 4 then
				if link_array_one[1] = rec1.s_node then
					node_temp := rec1.e_node;
				else
					node_temp := rec1.s_node;
				end if;
				
				if node_temp = any(node_array) then
					close curs1;
					ilast_num := ilast_num + 1;
					continue;
				else
					flage_temp := true;
					node_array_num := node_array_num + 1;
					node_array[node_array_num] := node_temp;
					open curs2 for select link_id
						 				from link_tbl
							where ((s_node = node_temp and one_way_code in (1,3)) or 
										(e_node = node_temp and one_way_code in (1,2))) and 
										link_id != link_array_one[link_array_one_num] and link_type <> 5;
					fetch curs2 into rec2;
					while rec2.link_id is not null loop
						link_array_num := link_array_num + 1;
						link_array[link_array_num] = node_temp::varchar || '|' || 
													array_to_string(link_array_one[1:link_array_one_num-1],'|') || '|' ||rec2.link_id::varchar;			
					fetch curs2 into rec2;
					end loop;
					close curs2;
				end if;
			end if;
			close curs1;
			ilast_num := ilast_num + 1;
		end loop;
	end loop;
	 				
	return link_array;
END;
$$;
--get link_type
CREATE OR REPLACE FUNCTION mid_get_link_array_type(node_array bigint[])
	RETURNS integer[]
	LANGUAGE plpgsql
AS $$
DECLARE
	curs1 refcursor;
	rec1 record;
	
	node_array_num integer;
	link_type_array integer[];
	
BEGIN
	node_array_num := array_upper(node_array,1);
	
	for node_array_num_idx in 1..node_array_num-1 loop

		open curs1 for select link_type
			 				from link_tbl 
			 where (s_node = node_array[node_array_num_idx] and e_node = node_array[node_array_num_idx+1]) or 
			  	(e_node = node_array[node_array_num_idx] and s_node = node_array[node_array_num_idx+1]);
		fetch curs1 into rec1;
		if rec1.link_type is null then
			close curs1;
			return null;
		end if;
		link_type_array[node_array_num_idx] := rec1.link_type;
		close curs1;
	end loop;
	return link_type_array;
END;
$$;
--node path Change to link path
CREATE OR REPLACE FUNCTION mid_get_all_link(node_array varchar[])
 RETURNS bigint[]
 LANGUAGE plpgsql
AS $$
DECLARE
	link_array bigint[];
	node_array_num integer;
	node_array_one bigint[];
	node_array_one_num integer;
BEGIN
	if node_array is null then
		return null;
	end if;
	node_array_num := array_upper(node_array,1);
	
	for node_array_num_idx in 1..node_array_num loop
		node_array_one := regexp_split_to_array(node_array[node_array_num_idx],E'\\|+')::bigint[];
		node_array_one_num := array_upper(node_array_one,1);
		--if node_array_one[node_array_one_num] = 1 then
			if link_array is null then
				link_array := mid_get_link_array(array_to_string(node_array_one[1:node_array_one_num-1],'|'));
			else
				link_array := link_array || mid_get_link_array(array_to_string(node_array_one[1:node_array_one_num-1],'|'));
			end if;
		--end if;
	end loop;
	
	return link_array;
END;
$$;
CREATE OR REPLACE FUNCTION mid_get_link_array(node_array varchar)
	RETURNS BIGINT[]
	LANGUAGE plpgsql
AS $$
DECLARE
	curs1 refcursor;
	rec1 record;
	
	node_array_temp bigint[];
	node_array_temp_num integer;
	
	link_array bigint[];
	
BEGIN
	--one node in path
	node_array_temp := (regexp_split_to_array(node_array,E'\\|+'))::bigint[];
	node_array_temp_num := array_upper(node_array_temp,1);
	if node_array_temp_num < 1 or node_array is null then
		return null;
	end if;
	
	for node_array_temp_num_idx in 1..node_array_temp_num-1 loop

		open curs1 for select link_id
			 from link_tbl 
			 where (s_node = node_array_temp[node_array_temp_num_idx] and e_node = node_array_temp[node_array_temp_num_idx+1] and one_way_code in (1,2)) or 
			  	(e_node = node_array_temp[node_array_temp_num_idx] and s_node = node_array_temp[node_array_temp_num_idx+1] and one_way_code in (1,3));
		fetch curs1 into rec1;
		if rec1.link_id is null then
			close curs1;
			return null;
		end if;
		link_array[node_array_temp_num_idx] := rec1.link_id;
		close curs1;
	end loop;
	
	return link_array;
END;
$$;
CREATE OR REPLACE FUNCTION make_find_node_mapping_building_link(kindcodes character varying[])
  RETURNS integer
  language plpgsql
as $$ 
DECLARE
	rec              record;
	building_geoms   geometry[];
	inlink_list      bigint[];
	building_num     integer;
	link_num         integer;
	i                integer;
	attr_codes       text[];
	kiwicode_v        text;
	name_str_v        text;
BEGIN
	for rec in
		SELECT node_id,the_geom,the_geom_circ_buffer
		FROM temp_building_mapping_node
		limit 10000
	loop	
		building_num = 0;
		link_num = 0;
		i = 0;
		if kindcodes is not null then
			select array_agg(the_geom),array_agg(attr_code)
			into building_geoms,attr_codes
			from(
				SELECT the_geom, attr_code,1 as groupid
				FROM t_logomark_4326
				where  ST_Intersects(rec.the_geom_circ_buffer,the_geom) and attr_code = any(kindcodes)
			) as a
			group by groupid;
		else
			select array_agg(the_geom),array_agg(attr_code)
			into building_geoms,attr_codes
			from(
				SELECT the_geom, attr_code,1 as groupid
				FROM t_logomark_4326
				where  ST_Intersects(rec.the_geom_circ_buffer,the_geom)
			) as a
			group by groupid;
		end if;
		
		building_num = array_upper(building_geoms,1);

		if building_num < 1 then
			continue;
		end if;
		
		select array_agg(link_id)
		into inlink_list
		from(
			SELECT link_id, 1 as groupid
			FROM link_tbl
			where (s_node = rec.node_id and one_way_code in (1,3)) or (e_node = rec.node_id and one_way_code in (1,2))
		) as a
		group by groupid;
		link_num = array_upper(inlink_list,1);

		while link_num >=1 loop
			i = 1;
			while i <= building_num loop
				SELECT kiwicode, name_str
				into kiwicode_v,name_str_v
				FROM org_logmark_code_mapping
				where attr_code = attr_codes[i];
				insert into building_structure(in_link_id,node_id,centroid_lontitude,centroid_lantitude,type_code,building_name)
				values(inlink_list[link_num],rec.node_id,(st_x(building_geoms[i]) * 3600 * 256)::integer,(st_y(building_geoms[i]) * 3600 * 256)::integer,
				hex_to_int(kiwicode_v),name_str_v
				);
				i = i + 1;
			end loop;
			link_num = link_num - 1;
		end loop;
		
		
	end loop;
	
	return 0;
END;
$$;
CREATE OR REPLACE FUNCTION hex_to_int(in_hex text)
  RETURNS integer 
  LANGUAGE PLPGSQL
AS $$ 
DECLARE
	result_int   integer;
	char_num     smallint;
BEGIN
	char_num = char_length(in_hex);
	if in_hex is null then
		return null;
	end if;
	if char_num = 1 then
		result_int = cast(cast(('x' || in_hex) as bit(4)) as int);
	elsif char_num = 2 then
		result_int = cast(cast(('x' || in_hex) as bit(8)) as int);
	elsif char_num = 3 then
		result_int = cast(cast(('x' || in_hex) as bit(12)) as int);
	elsif char_num = 4 then
		result_int = cast(cast(('x' || in_hex) as bit(16)) as int);
	elsif char_num = 5 then
		result_int = cast(cast(('x' || in_hex) as bit(20)) as int);
	else
		raise EXCEPTION 'typecode num too length';
	end if;
		
	return result_int;
END;
$$;
CREATE OR REPLACE FUNCTION mid_getpath_bynodes(startnode bigint, endnode bigint)
  RETURNS character varying 
  LANGUAGE PLPGSQL
AS $$
DECLARE
	rstPathStr  		varchar;
	rstPathCount		integer;
	rstPath		        varchar;
	nFromLink               bigint;
	tmpPathArray		varchar[];
	tmpLastNodeArray	bigint[];
	tmpLastLinkArray	bigint[];
	tmpPathCount		integer;
	tmpPathIndex		integer;
	rec        		record;
	nStartNode              bigint;
	nEndNode              bigint;
BEGIN
	--rstPath
	nFromLink               := -1;
	rstPathCount		:= 0;
	tmpPathArray		:= ARRAY[cast(nFromLink as varchar)];
	tmpPathCount		:= 1;
	tmpPathIndex		:= 1;
	nStartNode          := startnode;
	nEndNode          := endnode;
	tmpLastNodeArray	:= ARRAY[nStartNode];
	tmpLastLinkArray	:= ARRAY[nFromLink];
	-- search
        WHILE tmpPathIndex <= tmpPathCount LOOP
                -----stop if tmpPath has been more than layer given
                if array_upper(regexp_split_to_array(tmpPathArray[tmpPathIndex], E'\\,+'),1) < 15 then
                ---raise INFO 'tmpPathArray = %', tmpPathArray[tmpPathIndex];
                ---raise INFO 'tmpLastNodeArray[tmpPathIndex] = %', tmpLastNodeArray[tmpPathIndex];
                        if tmpLastNodeArray[tmpPathIndex] = nEndNode then
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
                                                        SELECT link_id as nextroad,'(2)' as dir,e_node as nextnode
                                                        FROM link_tbl
                                                        where s_node = tmpLastNodeArray[tmpPathIndex] and one_way_code in (1,2) 

                                                        union

                                                        SELECT link_id as nextroad,'(3)' as dir,s_node as nextnode
                                                        FROM link_tbl
                                                        where e_node = tmpLastNodeArray[tmpPathIndex] and one_way_code in (1,3)
                                                ) as e
                                loop
                                        
                                        if 		not (rec.nextroad in (nFromLink, tmpLastLinkArray[tmpPathIndex]))
                                                and	not ((rec.nextroad||rec.dir) = ANY(regexp_split_to_array(tmpPathArray[tmpPathIndex], E'\\,+')))
                                        then
                                                tmpPathCount		:= tmpPathCount + 1;
                                                tmpPathArray		:= array_append(tmpPathArray, cast(tmpPathArray[tmpPathIndex]||','||rec.nextroad||rec.dir as varchar));
                                                tmpLastNodeArray	:= array_append(tmpLastNodeArray, cast(rec.nextnode as bigint));
                                                tmpLastLinkArray	:= array_append(tmpLastLinkArray, cast(rec.nextroad as bigint));
                                                --raise INFO 'rec.nextroad = %', rec.nextroad;
                                        end if;
                                        
                                end loop;
                        end if;
                else
                        return null;
                end if;
                tmpPathIndex := tmpPathIndex + 1;
        END LOOP;
        --raise INFO 'tmpPathArray = %', tmpPathArray[tmpPathIndex];
        if array_upper(regexp_split_to_array(rstPathStr,E'\\,+'),1) > 1 then
                return substring(rstPathStr,4);
        else
                return null;
        end if;
END;
$$;
CREATE OR REPLACE FUNCTION make_find_node_mapping_building_no_link(maxsum integer, maxdistance integer, delete_distance integer, radis_len integer,fraction float)
  RETURNS integer
  language plpgsql
as $$ 
DECLARE
	rec                   record;
	building_geoms        geometry[];
	building_num          integer;
	right_num             integer;
	i                     integer;
	node_lonlat_record    record;
	mark_lonlat_record    record;
	temp_building_geom    geometry;
	relat_x               integer;
	relat_y               integer;
	comp_mark_distance    double precision;
	circl_geometry        geometry;
	country_geoms         geometry[];
	country_geoms_len     integer;
	road_numbers          character varying[];
	road_number_t           character varying;
	shield                character varying;
	road_no               character varying;
BEGIN
	for rec in
		select node_id,country_road_f,node_geom,array_agg(logmark_goem) as logmark_goems,array_agg(kiwicode) as kiwicodes,array_agg(name_str) as name_strs
		from(
			select a.node_id,a.priority,node_geom,logmark_goem,b.kiwicode,b.name_str,country_road_f
			from(
			  SELECT a.node_id,a.the_geom as node_geom,b.priority,b.the_geom as logmark_goem,b.attr_code,country_road_f
			  FROM temp_building_mapping_node as a
			  inner join t_logomark_4326 as b
			  on ST_Intersects(a.the_geom_circ_buffer,b.the_geom)=true
			) as a
			left join org_logmark_code_mapping as b
			on a.attr_code = b.attr_code
			order by node_id,priority
			) as a
		group by node_id,country_road_f,node_geom
	loop	
		if rec.logmark_goems is null then
			continue;
		end if;
		country_geoms_len = 0;
		building_num = array_upper(rec.logmark_goems,1);
		----delete nearly between node
		building_geoms = delete_logmark_by_node_distance(rec.logmark_goems,maxdistance,delete_distance,rec.node_geom);
		----delete nearly
		building_geoms = delete_nearly_logmark(building_geoms,delete_distance);
		right_num = 0;
		i = 1;
		if rec.country_road_f = 1 then
			----å¾—åˆ°çŽ?
			circl_geometry = ST_Boundary(ST_GeometryFromText(ST_AsText(ST_Buffer(ST_GeographyFromText(ST_AsEWKT(rec.node_geom)),radis_len)),4326));
			select array_agg(pos_geom),array_agg(road_number)
			into country_geoms,road_numbers
			from(
				select distinct link_id,road_number,ST_Line_Interpolate_Point(the_geom,fraction) as pos_geom,1 as groupid----ST_Intersection(circl_geometry,the_geom) as intersec_node,
				from link_tbl
				where ST_Intersects(circl_geometry,the_geom)=true and road_type = 2 and link_type in (1,2) 
			) as a
			group by groupid;
		end if;
		if country_geoms is not null then
			country_geoms_len = array_upper(country_geoms,1);
		end if;
		----å›½é“
		while country_geoms_len > 0 loop
			comp_mark_distance = ST_Distance(country_geoms[country_geoms_len],rec.node_geom,true);
			relat_x = round(comp_mark_distance * cos(get_two_point_angle(rec.node_geom,country_geoms[country_geoms_len])/180.0 * 3.1415926535897932));
			relat_y = round(comp_mark_distance * sin(get_two_point_angle(rec.node_geom,country_geoms[country_geoms_len])/180.0 * 3.1415926535897932));
			road_number_t = road_numbers[country_geoms_len];
			shield = substring((regexp_split_to_array(road_number_t,E'\\t'))[1] from '[0-9]+');
			road_no = substring((regexp_split_to_array(road_number_t,E'\\t'))[2] from '[0-9]+');
			insert into building_structure(nodeid,centroid_lontitude,centroid_lantitude,type_code,building_name)
			values(rec.node_id,relat_x,relat_y,shield::integer,road_no);
			country_geoms_len = country_geoms_len - 1;
		end loop;
		----ä¸€èˆ¬logmark
		while right_num < maxsum and i <= building_num loop
			temp_building_geom = building_geoms[i];
			if temp_building_geom is not null then
				comp_mark_distance = ST_Distance(temp_building_geom,rec.node_geom,true);
				relat_x = round(comp_mark_distance * cos(get_two_point_angle(rec.node_geom,temp_building_geom)/180.0 * 3.1415926535897932));
				relat_y = round(comp_mark_distance * sin(get_two_point_angle(rec.node_geom,temp_building_geom)/180.0 * 3.1415926535897932));
				
				insert into building_structure(nodeid,centroid_lontitude,centroid_lantitude,type_code,building_name)
				values(rec.node_id,relat_x,relat_y,hex_to_int(rec.kiwicodes[i]),rec.name_strs[i]);
				right_num = right_num + 1;
			end if;
			i = i + 1;
		end loop;

	end loop;
	
	return 0;
END;
$$;
CREATE OR REPLACE FUNCTION delete_nearly_logmark(logmark_geoms geometry[],delete_distance integer)
  RETURNS geometry[]
  language plpgsql
as $$ 
DECLARE
	array_num   integer;
	i           integer;
	j           integer;
	temp_geom   geometry;
	com_geom    geometry;
BEGIN
	array_num = array_upper(logmark_geoms,1);
	i = 1;
	while i <= array_num loop
		if logmark_geoms[i] is null then
			i = i + 1;
			continue;
		end if;
			temp_geom = logmark_geoms[i];
			j = i + 1;
		while j <= array_num loop
			com_geom = logmark_geoms[j];
			if com_geom is not null and ST_Distance(temp_geom,com_geom) < delete_distance then
				logmark_geoms[j] = null;
			end if;
			j = j + 1;
		end loop;
		i = i + 1;
	end loop;
	return logmark_geoms;
END;
$$;
CREATE OR REPLACE FUNCTION get_two_point_angle(org_lon_lat geometry, dst_lon_lat geometry) 
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
    
BEGIN 
    PI          := 3.1415926535897932;
    A_WGS84     := 6378137.0;
    E2_WGS84    := 6.69437999013e-3;
    MIN_DIFF_CRD := 31.0; -- 0x1f



    org_lon := ST_X(org_lon_lat) * 256 * 3600;
    org_lat := ST_Y(org_lon_lat) * 256 * 3600;
    

    dst_lon := ST_X(dst_lon_lat) * 256 * 3600;
    dst_lat := ST_Y(dst_lon_lat) * 256 * 3600;
    delta_lon := dst_lon - org_lon;
    delta_lat := dst_lat - org_lat;

    if delta_lon = 0 and delta_lat = 0 then
    	real_dir  := 0.0;
	else
	    ref_lat   := (org_lat + delta_lat / 2.0) / 256.0;
	    lat_radi  := ref_lat / 3600.0 * PI / 180.0;
	    sinlat    := sin(lat_radi);
	    c         := 1.0 - (E2_WGS84 * sinlat * sinlat);
	    M2SecLon  := 3600.0 / ((PI/180.0) * A_WGS84 * cos(lat_radi) / sqrt(c));
	    M2SecLat  := 1.0 / ((PI/648000.0) * A_WGS84 * (1 - E2_WGS84)) * sqrt(c*c*c);
	    real_delta_lat := delta_lat / M2SecLat / 256.0;
	    real_delta_lon := delta_lon / M2SecLon / 256.0;
        real_dir := (atan2(real_delta_lat, real_delta_lon) * 180.0 / PI);
    end if;
    if (real_dir > 0.0) then
        return real_dir;
    else 
        return 360.0 - abs(real_dir);
    end if;
   
END;
$$;
CREATE OR REPLACE FUNCTION delete_logmark_by_node_distance(logmark_geoms geometry[],maxdistance integer, mindistance integer,node_geom geometry)
  RETURNS geometry[]
  language plpgsql
as $$ 
DECLARE
	array_num      integer;
	i              integer;
	j              integer;
	temp_geom      geometry;
	com_geom       geometry;
	temp_distance  double precision;
BEGIN
	array_num = array_upper(logmark_geoms,1);
	i = 1;
	while i <= array_num loop
		if logmark_geoms[i] is null then
			i = i + 1;
			continue;
		end if;
		temp_distance = ST_Distance(node_geom,logmark_geoms[i],true);
		if temp_distance > maxdistance or maxdistance < mindistance then
			logmark_geoms[i] = null;
		end if;
		i = i + 1;
	end loop;
	return logmark_geoms;
END;
$$;
