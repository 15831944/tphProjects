CREATE OR REPLACE FUNCTION test_link_shape_turn_number(shape_point geometry)
RETURNS integer
LANGUAGE plpgsql
AS $$
DECLARE
    PI                double precision;
    turn_number        integer;
    point_count        integer;
    point_index        integer;
    angle_array        double precision[];
    angle_diff        double precision;
   
    org_lon            double precision;
    org_lat            double precision;
    dst_lon            double precision;
    dst_lat            double precision;
    delta_lat        double precision;
    delta_lon        double precision;
BEGIN
    --
    PI          := 3.1415926535897932;
   
    --
    point_count := ST_NPoints(shape_point);
    if (point_count <= 2) then
        RETURN 0;
    end if;
   
    --
    for point_index in 2..point_count loop
        org_lon := ST_X(ST_PointN(shape_point, point_index-1));
        org_lat := ST_Y(ST_PointN(shape_point, point_index-1));
       
        dst_lon := ST_X(ST_PointN(shape_point, point_index));
        dst_lat := ST_Y(ST_PointN(shape_point, point_index));
       
        delta_lon := dst_lon - org_lon;
        delta_lat := dst_lat - org_lat;   
       
        if delta_lon = 0 and delta_lat = 0 then
            angle_array[point_index]  := 0.0;
        else
            angle_array[point_index] := (atan2(delta_lat, delta_lon) * 180.0 / PI);
        end if;
    end loop;
   
    --
    turn_number    := 0;
    for point_index in 3..point_count loop
        angle_diff    := abs(angle_array[point_index] - angle_array[point_index-1]);
        if angle_diff >= 175 and angle_diff <= 185 then
            turn_number    := turn_number + 1;
            raise INFO 'point = %', st_astext(ST_PointN(shape_point, point_index-1));
        end if;
    end loop;
   
    return turn_number;
END;
$$;

CREATE OR REPLACE FUNCTION org_check_jct_number(jct_number varchar)
	RETURNS smallint
    LANGUAGE plpgsql
AS $$
DECLARE
	index      integer;
BEGIN
	if jct_number is null then
		raise EXCEPTION 'jct_number is null';
	end if;

	if char_length(jct_number) != 10 then
		return -1;
	end if;

	if substr(jct_number, 7, 1) != '_' then
		return -1;
	end if;

	for index in 1..10 loop
		if (index != 7) and (substr(jct_number, index, 1) not in ('0', '1', '2', '3', '4', '5', '6', '7', '8', '9')) then
			return -1;
		end if;
		index := index + 1;
	end loop;
	
	return 0;
END;
$$;

CREATE OR REPLACE FUNCTION org_get_meshcode(jct_number varchar)
	RETURNS varchar
    LANGUAGE plpgsql
AS $$
DECLARE
	meshcode            character varying(7);
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

CREATE OR REPLACE FUNCTION org_check_seq()
	RETURNS smallint
    LANGUAGE plpgsql
AS $$
DECLARE
	rec record;
	linknum smallint;
BEGIN
	for rec in
		select seq, link1, link2, link3, link4, link5, link6
		from org_eci_jctv
	loop
		linknum := 0;
		if rec.link1 is not null then 
			linknum := linknum + 1;
		end if;
		
		if rec.link2 is not null then 
			linknum := linknum + 1;
		end if;
		
		if rec.link3 is not null then 
			linknum := linknum + 1;
		end if;
		
		if rec.link4 is not null then 
			linknum := linknum + 1;
		end if;
		
		if rec.link5 is not null then 
			linknum := linknum + 1;
		end if;
		
		if rec.link6 is not null then 
			linknum := linknum + 1;
		end if;
		
		if rec.seq != linknum then
			return -1;
		end if;
	end loop;
	
	return 0;
END;
$$;