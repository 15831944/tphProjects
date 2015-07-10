CREATE OR REPLACE FUNCTION rdb_cal_zm(shape_point geometry, dir integer) RETURNS smallint
  LANGUAGE plpgsql
AS $$
DECLARE
	ref_lat double precision;
	real_delta_lat double precision;
    real_delta_lon double precision;
    real_delta_d double precision; 
    real_dir double precision; 
    delta_lat bigint; 
    delta_lon bigint; 
    --ret_value smallint;
    
    A_WGS84  double precision;
    E2_WGS84 double precision;
    PI double precision;
    --DEG_TO_NAVI double precision;
    
    org_lon bigint;
    org_lat bigint;
    dst_lon bigint;
    dst_lat bigint;

    sinlat double precision;
    c double precision;
    lat_radi double precision;
    
    M2SecLon double precision;
    M2SecLat double precision;
    --max      SMALLINT;
    
    direction smallint;
    direction_32 integer;
    num     integer;
    i       integer;
BEGIN 
    PI          := 3.1415926535897932;
    A_WGS84     := 6378137.0;
    E2_WGS84    := 6.69437999013e-3;
    --DEG_TO_NAVI := (65536 / 360.0);  -- 0x10000/360.0

    num := ST_NPoints(shape_point);  
    IF (num < 2) THEN
        RETURN NULL;
    END IF;

    IF (dir = -1) THEN
	org_lon := round(ST_X(ST_PointN(shape_point, num)) * 256 * 3600);
	org_lat := round(ST_Y(ST_PointN(shape_point, num)) * 256 * 3600);
    else
	org_lon := round(ST_X(ST_PointN(shape_point, 1)) * 256 * 3600);
	org_lat := round(ST_Y(ST_PointN(shape_point, 1)) * 256 * 3600);
    end if;
    i := 1;
    while (num - 1) >= i LOOP
	IF (dir = -1) THEN
		dst_lon := round(ST_X(ST_PointN(shape_point, num - i)) * 256 * 3600);
		dst_lat := round(ST_Y(ST_PointN(shape_point, num - i)) * 256 * 3600);
	else
		dst_lon := round(ST_X(ST_PointN(shape_point, 1 + i)) * 256 * 3600);
		dst_lat := round(ST_Y(ST_PointN(shape_point, 1 + i)) * 256 * 3600);
	end if;

	delta_lon := dst_lon - org_lon;
	delta_lat := dst_lat - org_lat;
	-- Get the next shape point
	if delta_lon = 0 and delta_lat = 0 then
		if num - 1 = i then
			return null;
		end if;
		i := i + 1;
	else
		exit;
	end if;
    END LOOP;
    
    ref_lat   := (org_lat + delta_lat / 2.0) / 256.0;
    lat_radi  := ref_lat / 3600.0 * PI / 180.0;
    sinlat    := sin(lat_radi);
    c         := 1.0 - (E2_WGS84 * sinlat * sinlat);
    M2SecLon  := 3600.0 / ((PI/180.0) * A_WGS84 * cos(lat_radi) / sqrt(c));
    M2SecLat  := 1.0 / ((PI/648000.0) * A_WGS84 * (1 - E2_WGS84)) * sqrt(c*c*c);
    real_delta_lat := delta_lat / M2SecLat / 256.0;
    real_delta_lon := delta_lon / M2SecLon / 256.0;
  
    real_delta_d := sqrt((real_delta_lat*real_delta_lat) + (real_delta_lon*real_delta_lon));
    
    if (real_delta_d = 0.0) then
        real_dir  := 0.0;
    else 
        real_dir := (atan2(real_delta_lat, real_delta_lon) * 180.0 / PI);
    END IF;
    
    direction_32 := 0;
    if (real_dir > 0.0) then
        direction_32 := round(real_dir * 65536 / 360.0);
    else 
        --max := 65535;  --0xffff;
        direction_32 := round(65535 - abs(real_dir * 65536 / 360.0));
    end if;
    
    direction := direction_32 - 32768;
    
    return direction;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_cal_zm_path(shape_point geometry, dir integer) RETURNS smallint
  LANGUAGE plpgsql
AS $$
DECLARE
	ref_lat double precision;
	real_delta_lat double precision;
    real_delta_lon double precision;
    real_delta_d double precision; 
    real_dir double precision; 
    delta_lat bigint; 
    delta_lon bigint;
    delta_lat2 bigint; 
    delta_lon2 bigint; 
    --ret_value smallint;
    
    A_WGS84  double precision;
    E2_WGS84 double precision;
    PI double precision;
    --DEG_TO_NAVI double precision;
    
    org_lon bigint;
    org_lat bigint;
    dst_lon bigint;
    dst_lat bigint;
    dst_lon2 bigint;
    dst_lat2 bigint;

    sinlat double precision;
    c double precision;
    lat_radi double precision;
    
    M2SecLon double precision;
    M2SecLat double precision;
    
    direction smallint;
    direction_32 integer;
    num     integer;
    i       integer;
    
    MIN_DIFF_CRD INTEGER;
    lDiffCrdX INTEGER;
    lDiffCrdY INTEGER;
BEGIN 
    PI          := 3.1415926535897932;
    A_WGS84     := 6378137.0;
    E2_WGS84    := 6.69437999013e-3;
    MIN_DIFF_CRD := 31; -- 0x1f

    num := ST_NPoints(shape_point);  
    IF (num < 2) THEN
        RETURN NULL;
    END IF;

    IF (dir = -1) THEN
	org_lon := round(ST_X(ST_PointN(shape_point, num)) * 256 * 3600);
	org_lat := round(ST_Y(ST_PointN(shape_point, num)) * 256 * 3600);
    else
	org_lon := round(ST_X(ST_PointN(shape_point, 1)) * 256 * 3600);
	org_lat := round(ST_Y(ST_PointN(shape_point, 1)) * 256 * 3600);
    end if;
    
    i := 1;
    while (num - 1) >= i LOOP
	IF (dir = -1) THEN
		dst_lon := round(ST_X(ST_PointN(shape_point, num - i)) * 256 * 3600);
		dst_lat := round(ST_Y(ST_PointN(shape_point, num - i)) * 256 * 3600);
	else
		dst_lon := round(ST_X(ST_PointN(shape_point, 1 + i)) * 256 * 3600);
		dst_lat := round(ST_Y(ST_PointN(shape_point, 1 + i)) * 256 * 3600);
	end if;

	delta_lon := dst_lon - org_lon;
	delta_lat := dst_lat - org_lat;
	
	-- Get the next shape point
	if delta_lon = 0 and delta_lat = 0 then   -- this include conditon: 
		-- the last shape point
		if num - 1 = i then
			return null;
		end if;
		i := i + 1;
	else
		exit;
	end if;
    END LOOP;

    lDiffCrdX := abs(delta_lon);
    lDiffCrdY := abs(delta_lat);
    -- If start point and the first shape point is too close
    if (lDiffCrdX + lDiffCrdY) < MIN_DIFF_CRD and (num - 1) > i then
	i := i + 1;
	while (num - 1) >= i LOOP
		IF (dir = -1) THEN
			dst_lon2 := round(ST_X(ST_PointN(shape_point, num - i)) * 256 * 3600);
			dst_lat2 := round(ST_Y(ST_PointN(shape_point, num - i)) * 256 * 3600);
		else
			dst_lon2 := round(ST_X(ST_PointN(shape_point, 1 + i)) * 256 * 3600);
			dst_lat2 := round(ST_Y(ST_PointN(shape_point, 1 + i)) * 256 * 3600);
		end if;

		delta_lon2 := dst_lon2 - dst_lon;
		delta_lat2 := dst_lat2 - dst_lat;
		
		-- Get the next shape point
		if delta_lon2 = 0 and delta_lat2 = 0 then   -- this include conditon: 
			i := i + 1;
		else
			exit;
		end if;
	END LOOP;
	dst_lon   := dst_lon2;
	dst_lat   := dst_lat2;
	delta_lon := dst_lon - org_lon;
	delta_lat := dst_lat - org_lat;
    END IF;
    
    ref_lat   := (org_lat + delta_lat / 2.0) / 256.0;
    lat_radi  := ref_lat / 3600.0 * PI / 180.0;
    sinlat    := sin(lat_radi);
    c         := 1.0 - (E2_WGS84 * sinlat * sinlat);
    M2SecLon  := 3600.0 / ((PI/180.0) * A_WGS84 * cos(lat_radi) / sqrt(c));
    M2SecLat  := 1.0 / ((PI/648000.0) * A_WGS84 * (1 - E2_WGS84)) * sqrt(c*c*c);
    real_delta_lat := delta_lat / M2SecLat / 256.0;
    real_delta_lon := delta_lon / M2SecLon / 256.0;
  
    real_delta_d := sqrt((real_delta_lat*real_delta_lat) + (real_delta_lon*real_delta_lon));
    
    if (real_delta_d = 0.0) then
        real_dir  := 0.0;
    else 
        real_dir := (atan2(real_delta_lat, real_delta_lon) * 180.0 / PI);
    END IF;
    
    direction_32 := 0;
    if (real_dir > 0.0) then
        direction_32 := round(real_dir * 65536 / 360.0);
    else 
        --max := 65535;  --0xffff;
        direction_32 := round(65535 - abs(real_dir * 65536 / 360.0));
    end if;
    
    direction := direction_32 - 32768;
    
    return direction;
END;
$$;
-----------------------------------------------------------------------------
-- include z_level
-----------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION rdb_geom2tileid_z(point_geom geometry, z integer)
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
	if z > 14 then  
		return NULL; 
	end if;

	lon = ST_X(point_geom);
	lat = ST_Y(point_geom);
	x = lon / 360. + 0.5;
	y = ln(tan(PI / 4. + radians(lat) / 2.));
	y = 0.5 - y / 2. / PI;
                                    
	tile_x = cast(floor(x * power(2, z)) as integer);
	tile_y = cast(floor(y * power(2, z)) as integer);
                                            
	tile_id = (z << 28) | (tile_x << 14) | tile_y;
                                            
	RETURN tile_id;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_convert_node_extendflag(light_flag boolean, dr_flag boolean, br_flag boolean, 
                                                      dm_flag boolean, ln_flag boolean, sp_flag boolean, 
													  cross_name_flag boolean, toll boolean, bifurcation_flag boolean,
													  etc_only boolean, sp_uc_flag boolean,
													  caution_flag boolean, force_flag boolean, boundary_flag boolean,
													  hwy_flag boolean, stopsign_flag boolean, natural_flag boolean, park_flage boolean,
													  gbs_flag boolean, hook_flag boolean) 
  RETURNS integer
  LANGUAGE plpgsql
  AS $$ 
DECLARE
	ExtendFlag integer;
BEGIN
	ExtendFlag  := 0;
	-- hwy
	if hwy_flag = true then
		ExtendFlag := ExtendFlag | 1; 
	end if;
	--park
	if park_flage = true then
		ExtendFlag := ExtendFlag | (1<<1);
	end if;
	-- Cross name
	if cross_name_flag = true then
		ExtendFlag := ExtendFlag | (1 << 2); 
	end if;
	-- Sign Name
	if dr_flag = TRUE THEN   
		ExtendFlag := ExtendFlag | (1 << 3);  -- Sign Name
        END IF;
	-- Illust 
	if br_flag = TRUE THEN
		ExtendFlag := ExtendFlag | (1 << 4);  -- Sport Guide
	END IF;
	-- Normal Way 3D
	if dm_flag = TRUE THEN
		ExtendFlag := ExtendFlag | (1 << 4);  -- Sport Guide
	END IF;
                                                    
	-- Sign Post
	if sp_flag = TRUE THEN
		ExtendFlag := ExtendFlag | (1 << 5);  -- Sign Post
	END IF;
                                                        
	-- Lane 
	if ln_flag = TRUE THEN
		ExtendFlag := ExtendFlag | (1 << 6);  -- Direction Indicator
	END IF;  
	
	--
    	if gbs_flag = TRUE THEN
    		ExtendFlag := ExtendFlag | (1 << 8);  -- building.structure
    	END IF;
    
	-- Signal
	if light_flag = TRUE then
		ExtendFlag := ExtendFlag | (1 << 9);  -- Signal
	end if;    
    
	if toll = TRUE THEN
		ExtendFlag := ExtendFlag | (1 << 10);  -- TOLL
		if etc_only = TRUE THEN
			ExtendFlag := ExtendFlag | (1 << 14);  -- ETC_ONLY
		END IF;
	END IF;
	
	-- SignPost UC
	if sp_uc_flag = TRUE THEN
		ExtendFlag := ExtendFlag | (1 << 11);  -- SignPost UC
	END IF;
	
	if boundary_flag = TRUE THEN
		ExtendFlag := ExtendFlag | (1 << 12);  -- tile boundary flag
	END IF;
	
	if caution_flag = TRUE THEN
		ExtendFlag := ExtendFlag | (1 << 13);  -- Caution
	END IF;
	
	if force_flag = TRUE THEN
		ExtendFlag := ExtendFlag | (1 << 15);  -- Forceguide
	END IF;
	
	if stopsign_flag = TRUE THEN
		ExtendFlag := ExtendFlag | (1 << 16);  -- stop sign
	END IF;
	
	if natural_flag = TRUE THEN
		ExtendFlag := ExtendFlag | (1 << 17);  -- natural guidence
	END IF;
	
	if hook_flag = TRUE THEN
		ExtendFlag := ExtendFlag | (1 << 18);  -- hook guidence
	END IF;

	if bifurcation_flag = TRUE THEN
		ExtendFlag := ExtendFlag | (1 << 19);  -- bifurcation flag
	END IF;
	
	return ExtendFlag;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_linkdir(direction integer) RETURNS integer
  LANGUAGE plpgsql
  AS $$
  BEGIN
  RETURN CASE
          WHEN Direction = 1 THEN 3
          WHEN Direction = 2 THEN 1
          WHEN Direction = 3 THEN 2
          WHEN Direction = 4 THEN 3
          ELSE 1
      END;
  END;
$$;

CREATE OR REPLACE FUNCTION rdb_cnv_road_direction(oneway_c integer) RETURNS integer
  LANGUAGE plpgsql
  AS $$
  BEGIN
  RETURN CASE 
          WHEN oneway_c = 0 THEN 1
          WHEN oneway_c = 5 THEN 1
          WHEN oneway_c = 1 THEN 2
          WHEN oneway_c = 3 THEN 2
          WHEN oneway_c = 2 THEN 3
          WHEN oneway_c = 4 THEN 3
          ELSE 4
      END;
  END;
$$;

CREATE OR REPLACE FUNCTION rdb_linkwidth(width integer) RETURNS integer
    LANGUAGE plpgsql
    AS $$
DECLARE
  	width_value integer;
BEGIN
    if width <= 3 then
    	width_value := 0;
    elseif width <= 5.5 then
    	width_value := 1;
    elseif width <= 13 then
    	width_value := 2;
    else
    	width_value := 3;
    end if;
    return width_value;
END;
$$;


CREATE OR REPLACE FUNCTION rdb_conn_linkid(curr_linkid bigint, conne_linkids bigint[]) RETURNS bigint
    LANGUAGE plpgsql
    AS $$
	DECLARE
	len integer;
	i integer;
	BEGIN
	    if conne_linkids is null then
	        return null;
	    end if;
	    len := array_upper(conne_linkids, 1);
	    
	    IF len = 1 THEN  -- only one link
	        return null;
	    end if;
	                                
	    i := 1;
	    while i < len LOOP
	        IF curr_linkid = conne_linkids[i] THEN
	            return conne_linkids[i + 1];
	        END IF;
	        i := i + 1;
	    END LOOP;
	    
	    IF curr_linkid = conne_linkids[len]  THEN
	    	return conne_linkids[1];  -- first link id
	    END IF;            
	    
	    return NULL;
	END;
$$;


CREATE OR REPLACE FUNCTION rdb_numgeometries(the_geom geometry) RETURNS integer
    LANGUAGE plpgsql
    AS $$ 
	DECLARE
	BEGIN
	    if st_geometrytype(the_geom) = 'ST_LineString' then
	        return 1;
	    else
	        return ST_NumGeometries(the_geom);
	    end if;
	
	    return NULL;
	END;
$$;

CREATE OR REPLACE FUNCTION rdb_geometryn(the_geom geometry, n integer) RETURNS geometry
    LANGUAGE plpgsql
    AS $$ 
	DECLARE
	BEGIN
	    if st_geometrytype(the_geom) = 'ST_LineString' then
	        return the_geom;
	    else
	        return ST_GeometryN(the_geom, n);
	    end if;
	END;
$$;

CREATE OR REPLACE FUNCTION rdb_arry_geom_by_idx(sub_geom geometry[], i integer)
    RETURNS geometry
    LANGUAGE plpgsql
AS $$ 
DECLARE
                                               
BEGIN
    if i < 1 or i > array_upper(sub_geom, 1) then
        return null;
    else 
        return sub_geom[i];
    end if;
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

CREATE OR REPLACE FUNCTION rdb_line_substring_for_linename(the_geom geometry, cut_len double precision)
  RETURNS geometry[]
  LANGUAGE plpgsql
  AS $$ 
DECLARE
    cut_len_radian float;
    sub_string geometry[];
    i integer;
    start_fraction float;
    end_fraction float;
    geom_radian float;
    percent float;
BEGIN
    cut_len_radian := cut_len / 1852.0 / 60.0;  -- convert length (meter) to radian
    geom_radian    := length(the_geom);
    percent        := cut_len_radian / geom_radian;
    start_fraction := 0.0;
    end_fraction   := percent;
    i              := 1;
    
    WHILE start_fraction < 1.0 LOOP
        
        if end_fraction > 1.0 then
            end_fraction = 1.0;
        end if;
        
        sub_string[i] = ST_Line_Substring(the_geom, start_fraction, end_fraction);
        
        start_fraction  := percent * i;
        end_fraction    := percent * (i + 1);
        i := i + 1;
    END LOOP;

    return sub_string;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_cvt_line_name_high_level(the_geom geometry, high_level integer, coefficient double precision)
    RETURNS integer
    LANGUAGE plpgsql
AS $$ 
DECLARE
    geom_radian float;
    length_meter float;
BEGIN    
    geom_radian    := length(the_geom);
    length_meter   := geom_radian * 60 * 1852; -- convert radian to length (meter)

    if length_meter > (50.0 * coefficient) then
        return high_level;
    end if;
    
    if length_meter > 50.0 then  -- 14 2090  8m/p       50m
        if high_level < 14 then
            return 14;
        else 
            return high_level;
        end if;
    end if;

    if length_meter > 25.0 then  -- 15 1045  4m/p       25m
        if high_level < 15 then
            return 15;
        else 
            return high_level;
        end if;
    end if;

    if length_meter > 13.0 then  -- 16 522   2m/p       13m
        if high_level < 16 then
            return 16;
        else 
            return high_level;
        end if;
    end if;

    if length_meter > 7.0 then   -- 17 256   1m/p       7m
        if high_level < 17 then
            return 17;
        else 
            return high_level;
        end if;
    end if;

    if length_meter > 3.0 then   -- 18 128   0.5m/p     3m 
        if high_level < 18 then
            return 18;
        else 
            return high_level;
        end if;
    end if;
    
    return 19;                -- 19 64    0.25m/p    1.5m
END;
$$;

CREATE OR REPLACE FUNCTION rdb_merge_common_link_attribute(
													extend_flag smallint,
												    toll smallint,
												    function_code smallint,
												    road_type smallint, 
												    display_class smallint
												    )
    RETURNS SMALLINT
    LANGUAGE plpgsql VOLATILE
AS $$
DECLARE
	rtn_value smallint;
BEGIN
    rtn_value := 0;
    rtn_value := rtn_value | (extend_flag & 1);
    rtn_value := rtn_value | (toll << 1);
    rtn_value := rtn_value | (function_code << 3);
    rtn_value := rtn_value | (road_type << 6);
    rtn_value := rtn_value | (display_class << 10);
    return rtn_value;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_merge_multi_link_attribute(
												    one_way smallint,
												    regulation_flag boolean,
												    tilt_flag boolean,
												    speed_regulation_flag boolean,
												    link_add_info_flag boolean,
												    shield_flag boolean,
												    extend_flag smallint,
												    link_type integer
												    )
    RETURNS SMALLINT
    LANGUAGE plpgsql VOLATILE
AS $$
DECLARE
	rtn_value smallint;
BEGIN
    rtn_value := 0;
    rtn_value := rtn_value | (one_way % 4);
    
    if (regulation_flag = true) then
        rtn_value := rtn_value | (1 << 2);
    end if;

    if (tilt_flag = true) then
        rtn_value := rtn_value | (1 << 3);
    end if;

    if (speed_regulation_flag = true) then
        rtn_value := rtn_value | (1 << 4);
    end if;

    if (link_add_info_flag = true) then
        rtn_value := rtn_value | (1 << 5);
    end if;

    if (shield_flag = true) then
        rtn_value := rtn_value | (1 << 6);
    end if;

    rtn_value := rtn_value | (((extend_flag >> 1) & 1) << 7);

    rtn_value := rtn_value | (((extend_flag >> 2) & 7) << 8);
    
    rtn_value := rtn_value | (link_type << 11);

    return rtn_value;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_smallint_2_octal_bytea(val smallint)
  RETURNS bytea
  LANGUAGE plpgsql VOLATILE
AS $$
DECLARE
    val_8 smallint;
    octal_str character varying;
    val_octal bytea;
    pow_val smallint;
    i smallint;
BEGIN
    i         := 2;
    octal_str := E'\\';
    val_octal := ''::bytea;
    while i > 0 loop
        octal_str := E'\\';
        val_8     := (val >> ((i - 1) * 8)) & 255; --bit:11111111
        octal_str := octal_str || cast((val_8 / 64) as  character varying);
        val_8     := val_8 - (val_8 / 64 * 64);
        octal_str := octal_str || cast((val_8 / 8) as  character varying);
        val_8     := val_8 - (val_8 / 8 * 8);
        octal_str := octal_str || cast(val_8 as  character varying);

        val_octal := octal_str::bytea || val_octal;
        i := i - 1;
    END LOOP;

    return val_octal;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_integer_2_octal_bytea(val integer)
  RETURNS bytea
  LANGUAGE plpgsql VOLATILE
AS $$
DECLARE
    val_8 smallint;
    octal_str character varying;
    val_octal bytea;
    i smallint;
BEGIN
    i         := 4;
    octal_str := E'\\';
    val_octal := ''::bytea;
    while i > 0 loop
        octal_str := E'\\';
        val_8     := (val >> ((i - 1) * 8)) & 255;  -- 0xFF
        octal_str := octal_str || cast((val_8 / 64) as  character varying);
        val_8     := val_8 - (val_8 / 64 * 64);
        octal_str := octal_str || cast((val_8 / 8) as  character varying);
        val_8     := val_8 - (val_8 / 8 * 8);
        octal_str := octal_str || cast(val_8 as  character varying);

        val_octal := octal_str::bytea || val_octal;
        i := i - 1;
    END LOOP;
    
    return val_octal;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_makeshape2bytea(the_geom geometry)
    RETURNS bytea
    LANGUAGE plpgsql
AS $$
DECLARE
	geom_content bytea;
    cnt_geom integer;
    i integer;
    t integer;
    BASELON integer;
    BASELAT integer;
    CURRENTLON integer;
    CURRENTLAT integer;
    DELTALON integer;-- SMALLINT
    DELTALAT integer;-- SMALLINT
    add_point_flag BOOLEAN;
    bytea_intor integer;
    u integer;
    n integer;
    DIFF_LON_TEMP integer; -- SMALLINT
    DIFF_LAT_TEMP integer;-- SMALLINT
    SPLIT_CNT SMALLINT;
BEGIN
    BEGIN
        cnt_geom := st_npoints(the_geom);
        i := 0;
        BASELON := 0;
        BASELAT := 0;
        add_point_flag := FALSE;
        bytea_intor := 0;

        geom_content = '';
        WHILE i < cnt_geom LOOP
            IF (i = 0) THEN
                t := 0;
                BASELON := cast(round(st_x(st_pointn(the_geom,i+1)) * 256 * 3600) as int);
                BASELAT := cast(round(st_y(st_pointn(the_geom,i+1)) * 256 * 3600) as int);
                
                geom_content := rdb_integer_2_octal_bytea(BASELON) || rdb_integer_2_octal_bytea(BASELAT);
            ELSE 
                CURRENTLON := cast(round(st_x(st_pointn(the_geom,i+1)) * 256 * 3600) as int);
                CURRENTLAT := cast(round(st_y(st_pointn(the_geom,i+1)) * 256 * 3600) as int);    
                DELTALON := CURRENTLON - BASELON;
                DELTALAT := CURRENTLAT - BASELAT;

                IF ((DELTALON > 32767 or DELTALON < -32768) or (DELTALAT > 32767 or DELTALAT < -32768) ) THEN
                    add_point_flag := TRUE;
                END IF;

                IF ( add_point_flag != TRUE) THEN
                    geom_content := geom_content || rdb_smallint_2_octal_bytea(cast(DELTALON as smallint)) || rdb_smallint_2_octal_bytea(cast(DELTALAT as smallint));
                    
                    BASELON :=  CURRENTLON;
                    BASELAT :=  CURRENTLAT;
                ELSE 
                    u := 0;
                    WHILE u < 10 LOOP
                        DIFF_LON_TEMP = ROUND(DELTALON/POWER(2, u));
                        DIFF_LAT_TEMP = ROUND(DELTALAT/POWER(2, u));
                        IF ((DIFF_LON_TEMP <= 32767 and DIFF_LON_TEMP >= -32768) and (DIFF_LAT_TEMP <= 32767 and DIFF_LAT_TEMP >= -32768)) THEN
                            EXIT;
                        END IF;
                        u := u + 1;            
                    END LOOP;

                    SPLIT_CNT :=  POWER(2, u);
                    n := 0;
                    WHILE n < (SPLIT_CNT - 1) LOOP
                        geom_content = geom_content || rdb_smallint_2_octal_bytea(cast(DIFF_LON_TEMP as smallint)) || rdb_smallint_2_octal_bytea(cast(DIFF_LAT_TEMP as smallint));
                        n := n + 1;
                    END LOOP;
                    
                    -- deal with last split section
                    geom_content = geom_content || rdb_smallint_2_octal_bytea(cast((DELTALON - n*DIFF_LON_TEMP) as smallint)) || rdb_smallint_2_octal_bytea(cast((DELTALAT - n*DIFF_LAT_TEMP) as smallint));

                    add_point_flag := FALSE;
                    
                    BASELON :=  CURRENTLON;
                    BASELAT :=  CURRENTLAT;
                END IF;
            END IF;
            i = i + 1;
        END LOOP;
    END;
    return geom_content;
END;
$$;

              
CREATE OR REPLACE FUNCTION rdb_makeshape2pixelbytea(tz smallint,tx int,ty int,the_geom geometry)
    RETURNS bytea
    LANGUAGE plpgsql
AS $$
DECLARE
    geom_content bytea;
    cnt_geom integer;
    i integer;
    record_xy record;
    BASELON SMALLINT;
    BASELAT SMALLINT;
    CURRENTLON SMALLINT;
    CURRENTLAT SMALLINT;

BEGIN
    BEGIN
        cnt_geom := st_npoints(the_geom);
        i := 0;
        BASELON := 0;
        BASELAT := 0;

        geom_content = '';
        WHILE i < cnt_geom LOOP
            IF (i = 0) THEN
                record_xy := lonlat2pixel_tile(tz::smallint, tx::integer, ty::integer, st_x(ST_PointN(the_geom,i+1))::float, st_y(ST_PointN(the_geom,i+1))::float, 4096::smallint);
                BASELON := record_xy.x;
                BASELAT := record_xy.y;
				
                geom_content := rdb_smallint_2_octal_bytea(BASELON) || rdb_smallint_2_octal_bytea(BASELAT);
            ELSE 
                record_xy := lonlat2pixel_tile(tz::smallint, tx::integer, ty::integer, st_x(ST_PointN(the_geom,i+1))::float, st_y(ST_PointN(the_geom,i+1))::float, 4096::smallint);
                CURRENTLON := record_xy.x;
                CURRENTLAT := record_xy.y;
		
                geom_content = geom_content || rdb_smallint_2_octal_bytea(CURRENTLON) || rdb_smallint_2_octal_bytea(CURRENTLAT);

            END IF;
            i = i + 1;
        END LOOP;
    END;
    return geom_content;
END;
$$;

---------------------convert wgs84 to 4096---------start--------------------
CREATE OR REPLACE FUNCTION lonlat2pixel_tile(tz smallint, tx int, ty int, lon float,  lat float,  tile_pixel smallint, out x smallint, out y smallint) RETURNS record
  LANGUAGE plpgsql
AS $$
DECLARE
	world record;
BEGIN
    world = lonlat2world(lon,lat);
    x = round((world.x * (1 << tz) - tx) * tile_pixel);
    y = round((world.y * (1 << tz) - ty) * tile_pixel);
END;
$$;

CREATE OR REPLACE FUNCTION lonlat2pixel_tile_backup(tz smallint, tx int, ty int, lon float,  lat float,  tile_pixel smallint, out x smallint, out y smallint) RETURNS record
  LANGUAGE plpgsql
AS $$
DECLARE
	lleft float;
	bottom float;
	rright float;
	top float;
	box record;
	pixel record;

BEGIN 

    box := tile_bbox(tz, tx, ty);
    lleft := box.lleft;
    bottom := box.bottom;
    rright := box.rright;
    top := box.top;
    
    pixel := lonlat2pixel(lon, lat, lleft, top, rright, bottom, tile_pixel, tile_pixel);

    x := round(pixel.wx_out);
    y := round(pixel.wy_out);

END;
$$;


CREATE OR REPLACE FUNCTION lonlat2pixel(lon float,lat float, lleft float,top float, rright float,bottom float, width int, height int,out wx_out float,out wy_out float ) RETURNS record
  LANGUAGE plpgsql
AS $$
DECLARE
	world1 record;
	world2 record;
	world3 record;
	w float;
	h float;
	wx float;
	wy float;

BEGIN 

    world1 := lonlat2world(lleft,top);
    lleft := world1.x;
    top := world1.y;
    world2 := lonlat2world(rright,bottom);
    rright := world2.x;
    bottom := world2.y;
    
    w := abs(rright - lleft) / width;
    h := abs(bottom - top) / height;
    world3 := lonlat2world(lon,lat);
    wx := world3.x;
    wy := world3.y;
    
    wx_out := (wx - lleft) / w;
    wy_out := (wy - top) / h;
    
END;
$$;


CREATE OR REPLACE FUNCTION tile_bbox(z smallint,x int,y int,out lleft float, out bottom float, out rright float, out top float) RETURNS record
  LANGUAGE plpgsql
AS $$
DECLARE
	world1 record;
	x1 float;
	y1 float;
	lonlat1 record;
	world2 record;
	x2 float;
	y2 float;
	lonlat2 record;
BEGIN 

    world1 := pixel2world(z, x, y, 0, 1, 1);
    x1 := world1.wx;
    y1 := world1.wy;
    lonlat1 :=  world2lonlat(x1,y1);
    lleft := lonlat1.lon;
    bottom := lonlat1.lat;

    world2 := pixel2world(z, x, y, 1, 0, 1);
    x2 := world2.wx;
    y2 := world2.wy;
    lonlat2 :=  world2lonlat(x2,y2);
    rright := lonlat2.lon;
    top := lonlat2.lat;

    
END;
$$;


CREATE OR REPLACE FUNCTION world2lonlat(x float,y float, out lon float, out lat float) RETURNS record
  LANGUAGE plpgsql
AS $$
DECLARE
	PI double precision;
BEGIN 
    PI          := 3.1415926535897931;
    lon := (x - 0.5) * 360.;
    lat := degrees(2 * atan(exp((1. - 2. * y) * PI)) - PI / 2.);
    
END;
$$;


CREATE OR REPLACE FUNCTION lonlat2world(lon float, lat float, out x float, out y float) RETURNS record
  LANGUAGE plpgsql
AS $$
DECLARE
	PI double precision;
BEGIN 

    PI          := 3.1415926535897931;
    x := lon / 360. + 0.5;
    y := ln(tan(PI / 4. + radians(lat)/ 2.));
    y := 0.5 - y / 2. / PI;

END;
$$;


CREATE OR REPLACE FUNCTION pixel2world(tz smallint, tx int, ty int, px int, py int, size int ,out wx float, out wy float) RETURNS record
  LANGUAGE plpgsql
AS $$
DECLARE
	
BEGIN 

    wx := (tx * size + px) / (2 ^ tz * size);
    wy := (ty * size + py) / (2 ^ tz * size);
    
END;
$$; 

---------------------convert wgs84 to 4096---------end--------------------

             
CREATE OR REPLACE FUNCTION rdb_makenode2pixelbytea(tz smallint,tx int,ty int,the_geom geometry)
    RETURNS bytea
    LANGUAGE plpgsql
AS $$
DECLARE
    geom_content bytea;
    record_xy record;
    LON SMALLINT;
    LAT SMALLINT;

BEGIN
    BEGIN
    geom_content = '';
	record_xy := lonlat2pixel_tile(tz::smallint, tx::integer, ty::integer, st_x(the_geom)::float, st_y(the_geom)::float, 4096::smallint);
	LON := record_xy.x;
	LAT := record_xy.y;
	geom_content := rdb_smallint_2_octal_bytea(LON) || rdb_smallint_2_octal_bytea(LAT);

    END;
    return geom_content;
END;
$$;


CREATE OR REPLACE FUNCTION rdb_split_tileid(inputid bigint)
    RETURNS INTEGER
    LANGUAGE plpgsql
AS $$
    BEGIN
        RETURN cast((cast (((inputid << 32 ) >> 32) as int) & (-1)) as int);
    END;
$$;

CREATE OR REPLACE FUNCTION rdb_split_to_tileid(inputid bigint)
    RETURNS INTEGER
    LANGUAGE plpgsql
AS $$
    BEGIN
        RETURN cast((cast ((inputid >> 32) as int) & (-1)) as int);
    END;
$$;

CREATE OR REPLACE FUNCTION rdb_convert_keystring(keystring varchar)
    RETURNS varchar
    LANGUAGE plpgsql
AS $$
DECLARE
	new_keystring	varchar;
	link_array		bigint[];
	nIndex			integer;
	nCount			integer;
	linkid			integer;
	tileid			integer;
BEGIN
	link_array	:= cast(regexp_split_to_array(keystring, E',') as bigint[]);
	nCount		:= array_upper(link_array, 1);
	nIndex		:= 1;
	
	new_keystring	:= '[';
	while nIndex <= nCount loop
		if nIndex > 1 then
			new_keystring	:= new_keystring || ',';
		end if;
		
		tileid	:= rdb_split_to_tileid(link_array[nIndex]);
		linkid	:= rdb_split_tileid(link_array[nIndex]);
		new_keystring	:= new_keystring || '[' || tileid || ',' || linkid || ']';
		
		nIndex	:= nIndex + 1;
	end loop;
	new_keystring	:= new_keystring || ']';
	
    RETURN new_keystring;
END;
$$;


CREATE OR REPLACE FUNCTION rdb_cnv_add_info_client(struct_code smallint, shortcut_code smallint, parking_flag smallint, etc_lane_flag smallint)
  RETURNS smallint
  LANGUAGE plpgsql VOLATILE
AS $$
DECLARE
    multi_link_add_info smallint;
BEGIN
    multi_link_add_info := 0;
    
    IF struct_code IS NOT null THEN
        --IF struct_code > 3 THEN
        --END IF;
        multi_link_add_info := struct_code;
    END IF;

    IF shortcut_code IS NOT NULL THEN
        --IF parking_flag > 3 THEN
        --END IF;
        multi_link_add_info := multi_link_add_info | (shortcut_code << 3);
    END IF;

    IF parking_flag IS NOT NULL THEN
        --IF parking_flag > 3 THEN
        --END IF;
        multi_link_add_info := multi_link_add_info | (parking_flag << 5);
    END IF;

    IF etc_lane_flag IS NOT NULL THEN
        --IF parking_flag > 1 THEN
        --END IF;
        multi_link_add_info := multi_link_add_info | (etc_lane_flag << 7);
    END IF;

    return multi_link_add_info;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_cnv_linklane01_client(disobey_flag boolean, lane_dir smallint, lane_up_down_distingush boolean)
  RETURNS smallint
  LANGUAGE plpgsql VOLATILE
AS $$
DECLARE
    multi_lane_attribute_1 smallint;
    flag smallint;
BEGIN
    -- bit[0]:   disobey_flag
    -- bit[2:1]: lane_dir
    -- bit[3]:   up_down_distingush
    multi_lane_attribute_1 := 0;
    
    IF disobey_flag IS NOT null and disobey_flag = true THEN
        flag := 1;
        multi_lane_attribute_1 := flag;
    END IF;

    IF lane_dir IS NOT NULL THEN
        multi_lane_attribute_1 :=  multi_lane_attribute_1 | (lane_dir << 1);
    END IF;

    IF lane_up_down_distingush IS NOT NULL and lane_up_down_distingush = true THEN
        flag := 1;
        multi_lane_attribute_1 :=  multi_lane_attribute_1 | (flag << 3);
    END IF;

    return multi_lane_attribute_1;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_cnv_linklane02_client(ops_lane_number smallint, ops_width smallint, neg_lane_number smallint, neg_width smallint)
  RETURNS smallint
  LANGUAGE plpgsql VOLATILE
AS $$
DECLARE
    multi_lane_attribute_2 smallint;
BEGIN
    -- bit[3:0]:    ops_lane_number
    -- bit[7:4]:    ops_width
    -- bit[11:8]:   neg_lane_number
    -- bit[15:12]:  neg_width
    
    multi_lane_attribute_2 := 0;
    
    IF ops_lane_number IS NOT null THEN
        multi_lane_attribute_2 := ops_lane_number;
    END IF;

    IF ops_width IS NOT NULL THEN
        multi_lane_attribute_2 :=  multi_lane_attribute_2 | (ops_width << 4);
    END IF;

    IF neg_lane_number IS NOT NULL THEN
        multi_lane_attribute_2 :=  multi_lane_attribute_2 | (neg_lane_number << 8);
    END IF;

    IF neg_width IS NOT NULL THEN
        multi_lane_attribute_2 :=  multi_lane_attribute_2 | (neg_width << 12);
    END IF;

    return multi_lane_attribute_2;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_cnt_lane_num_client(lane_num smallint, lane_num_l smallint, lane_num_r smallint)
  RETURNS smallint
  LANGUAGE plpgsql VOLATILE
AS $$
DECLARE
    lane_num_client smallint;
    change_num_l smallint;
    change_num_r smallint;
BEGIN 
    if lane_num is null THEN
        return null;
    END IF;
    
    lane_num_client := lane_num;
    
    -- left change lane num
    IF lane_num_l is null THEN
        change_num_l := 0;
    ELSE
        --IF abs(lane_num_l) > 7 THEN
        --END IF;
        
        IF lane_num_l < 0 THEN
            change_num_l := abs(lane_num_l);
            change_num_l := change_num_l | (1 << 3);
        ELSE
            change_num_l := lane_num_l;    
        END IF;
    END IF;
    
    -- left change lane num
    IF lane_num_r is null THEN
        change_num_r := 0;
    ELSE
        --IF abs(lane_num_r) > 7 THEN
        --END IF;
        
        IF lane_num_r < 0 THEN
            change_num_r := abs(lane_num_r);
            change_num_r := change_num_r | (1 << 3);
        ELSE
            change_num_r := lane_num_r;
        END IF;
    END IF;
    
    lane_num_client := lane_num_client | (change_num_l << 12);
    lane_num_client := lane_num_client | (change_num_r << 8);

    return lane_num_client;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_cnt_lane_arrow_client(arrow smallint, pass_cnt smallint)
  RETURNS smallint
  LANGUAGE plpgsql VOLATILE
AS $$
DECLARE
    arrow_info smallint;
BEGIN 
    IF arrow is null THEN
        return null;
    END IF;
    
    --IF  arrow > 255 OR pass_cnt > 255 THEN
    --END IF;

    arrow_info := arrow | (pass_cnt << 8);
    
    return arrow_info;
END;
$$;
  
CREATE OR REPLACE FUNCTION rdb_make_tile_links(target_tile_id integer)
  RETURNS integer 
  LANGUAGE plpgsql VOLATILE
AS $$ 
DECLARE
	rec				temp_link_tbl;
	rec2				record;
	tmp_rec                         record;
	pos_rec				record;
	neg_rec				record;
	start_node			bigint;
	end_node			bigint;
	link_array			bigint[];
	nCount				integer;
	nIndex				integer;
	
BEGIN
	
	for rec in 
	(
		select link_id,s_node,e_node,tile_id,common_main_link_attri,display_class
		from temp_link_tbl 
		where tile_id = target_tile_id and display_class not in(30,31)
		order by tile_id,common_main_link_attri,link_id,display_class
	)

	LOOP
		
 		select link_id from temp_link_tbl_walked where link_id = rec.link_id into tmp_rec;
 		
 		if found then
 			continue;
 		else 
			insert into temp_link_tbl_walked ("link_id") values (rec.link_id);
 		end if;	
 		
		pos_rec			:= rdb_make_tile_links_in_one_direction(rec, rec.e_node, rec.tile_id, rec.common_main_link_attri);
		end_node		:= pos_rec.reach_node;
		
		neg_rec			:= rdb_make_tile_links_in_one_direction(rec, rec.s_node, rec.tile_id, rec.common_main_link_attri);
		start_node		:= neg_rec.reach_node;
		
		link_array		:= ARRAY[rec.link_id];
		
		nCount			:= array_upper(neg_rec.linkrow, 1);
		if nCount is not null then
			for nIndex in 1..nCount loop
				link_array	:= array_prepend(neg_rec.linkrow[nIndex], link_array);
			end loop;
		end if;
		
		nCount			:= array_upper(pos_rec.linkrow, 1);
		if nCount is not null then
			for nIndex in 1..nCount loop
				link_array	:= array_append(link_array, pos_rec.linkrow[nIndex]);
			end loop;
		end if;
		
		-- insert new linkrow
		nCount			:= array_upper(link_array, 1);
		
		nIndex := 1;
			
		while nIndex  <= nCount loop	
					
			insert into temp_link_tbl_newid ("link_id", "s_node", "e_node", "tile_id", "common_main_link_attri","display_class")
				values (link_array[nIndex], start_node, end_node, rec.tile_id,rec.common_main_link_attri,rec.display_class);
			nIndex := nIndex + 1;	
		end loop;

		
	END LOOP;

	return 1;
	
END;
$$;

CREATE OR REPLACE FUNCTION rdb_make_tile_links_park()
  RETURNS integer 
  LANGUAGE plpgsql VOLATILE
AS $$ 
DECLARE
	rec				temp_link_tbl;
	rec2				record;
	tmp_rec                         record;
	pos_rec				record;
	neg_rec				record;
	start_node			bigint;
	end_node			bigint;
	link_array			bigint[];
	nCount				integer;
	nIndex				integer;
	
BEGIN
	
	for rec in 
	(
		select link_id,s_node,e_node,tile_id,common_main_link_attri,display_class
		from temp_link_tbl 
		where display_class in(30,31)
		order by tile_id,common_main_link_attri,link_id,display_class
	)

	LOOP
		
 		select link_id from temp_link_tbl_walked where link_id = rec.link_id into tmp_rec;
 		
 		if found then
 			continue;
 		else 
			insert into temp_link_tbl_walked ("link_id") values (rec.link_id);
 		end if;	
 		
		pos_rec			:= rdb_make_tile_links_in_one_direction(rec, rec.e_node, rec.tile_id, rec.common_main_link_attri);
		end_node		:= pos_rec.reach_node;
		
		neg_rec			:= rdb_make_tile_links_in_one_direction(rec, rec.s_node, rec.tile_id, rec.common_main_link_attri);
		start_node		:= neg_rec.reach_node;
		
		link_array		:= ARRAY[rec.link_id];
		
		nCount			:= array_upper(neg_rec.linkrow, 1);
		if nCount is not null then
			for nIndex in 1..nCount loop
				link_array	:= array_prepend(neg_rec.linkrow[nIndex], link_array);
			end loop;
		end if;
		
		nCount			:= array_upper(pos_rec.linkrow, 1);
		if nCount is not null then
			for nIndex in 1..nCount loop
				link_array	:= array_append(link_array, pos_rec.linkrow[nIndex]);
			end loop;
		end if;
		
		-- insert new linkrow
		nCount			:= array_upper(link_array, 1);
		
		nIndex := 1;
			
		while nIndex  <= nCount loop	
					
			insert into temp_link_tbl_newid ("link_id", "s_node", "e_node", "tile_id", "common_main_link_attri","display_class")
				values (link_array[nIndex], start_node, end_node, rec.tile_id,rec.common_main_link_attri,rec.display_class);
			nIndex := nIndex + 1;	
		end loop;

		
	END LOOP;

	return 1;
	
END;
$$;

CREATE OR REPLACE FUNCTION rdb_make_tile_links_in_one_direction(	
								rec temp_link_tbl, search_node bigint, cur_tile_id int, cur_common_attri smallint, 
								OUT linkrow bigint[], OUT reach_node bigint)
	RETURNS record
	LANGUAGE plpgsql volatile
AS $$
DECLARE
	rec2				record;
	reach_link                      bigint;
BEGIN
	-- init
	reach_link	:= rec.link_id;
	reach_node	:= search_node;
	
	-- search
	while true loop
		
		-- search link
		select * from (
			select	a.*
			from
			(
				(
					select	link_id, e_node as nextnode, tile_id,common_main_link_attri 
					from temp_link_tbl
					where 	(reach_node = s_node) 
						and (link_id != reach_link)
						and (tile_id = cur_tile_id)
						and (common_main_link_attri = cur_common_attri)
				)
				union
				(
					select	link_id, s_node as nextnode, tile_id,common_main_link_attri
					from temp_link_tbl
					where 	(reach_node = e_node) 
						and (link_id != reach_link)
						and (tile_id = cur_tile_id)
						and (common_main_link_attri = cur_common_attri)
				)
			)as a
			left join temp_link_tbl_walked as b
			on a.link_id = b.link_id
			where b.link_id is null
		) a order by link_id limit 1
		into rec2;
		
		if FOUND then
			
			insert into temp_link_tbl_walked ("link_id") values (rec2.link_id);
			
			-- walk this link
			linkrow		:= array_append(linkrow, rec2.link_id);
			reach_link	:= rec2.link_id;
			reach_node	:= rec2.nextnode;
		else
			EXIT;
		end if;
	end loop;
END;
$$;


CREATE OR REPLACE FUNCTION rdb_cnv_road_struct_code(elevated integer, structure integer, tunnel integer, rail_cross integer) RETURNS integer 
  LANGUAGE plpgsql VOLATILE
  AS $$ 
DECLARE
BEGIN
	IF rail_cross IS NOT NULL and rail_cross = 1 THEN
		RETURN 3;
	END IF;
	
	IF rail_cross IS NOT NULL and rail_cross = 2 THEN
		RETURN 6;
	ELSIF rail_cross IS NOT NULL and rail_cross = 3 THEN
		RETURN 7;
	END IF;

	IF tunnel IS NOT NULL and tunnel = 1 THEN
		RETURN 1;
	END IF;

	IF elevated IS NOT NULL and elevated = 1 THEN
		RETURN 5;
	END IF;
	
	IF structure IS NOT NULL and structure = 1 THEN
		RETURN 8;
	END IF;

	IF structure IS NOT NULL and structure = 2 THEN
		RETURN 4;
	END IF;

	IF structure IS NOT NULL and structure = 3 THEN
		RETURN 2;
	END IF;
    
	RETURN 0;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_cnv_path_extra_info(paved smallint, uturn smallint, ipd smallint, urban smallint, four_wheel boolean, stopsign integer) 
  RETURNS smallint 
  LANGUAGE plpgsql VOLATILE
  AS $$ 
DECLARE
	rtnValue smallint;
BEGIN
	rtnValue = 0;
	
	IF paved = 0 THEN
		rtnValue = 1;
	END IF;

	IF uturn = 1 THEN
		rtnValue = rtnValue | (1 << 1);
	END IF;

	IF ipd = 1 THEN
		rtnValue = rtnValue | (1 << 2);
	END IF;

	IF urban = 1 THEN
		rtnValue = rtnValue | (1 << 3);
	END IF;

	IF four_wheel = 't' THEN
		rtnValue = rtnValue | (1 << 4);
	END IF;

	IF stopsign is not null THEN
		rtnValue = rtnValue | (stopsign << 6);
	END IF;
	
	RETURN rtnValue;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_cnv_link_add_info2(
	erp smallint, rodizio smallint, safety_zone boolean, hook_turn smallint, safety_alert boolean) 
  RETURNS smallint 
  LANGUAGE plpgsql VOLATILE
  AS $$ 
DECLARE
	rtnValue smallint;
BEGIN
	rtnValue = 0;
	
	IF rodizio = 1 THEN
		rtnValue = 1;
	END IF;

	IF erp != 0 THEN
		rtnValue = rtnValue | (erp << 1);
	END IF;

	IF safety_zone = 't' THEN
		rtnValue = rtnValue | (1 << 3);
	END IF;

	IF hook_turn != 0 THEN
		rtnValue = rtnValue | (1 << 4);
	END IF;

	IF safety_alert = 't' THEN
		rtnValue = rtnValue | (1 << 5);
	END IF;
	
	RETURN rtnValue;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_array2string(link_array bigint[])
  RETURNS character varying
  LANGUAGE plpgsql VOLATILE
  AS $$
DECLARE
	link_str	character varying;
	nIndex		integer;
	nCount		integer;
BEGIN
	link_str	:= '';
	nIndex		:= 1;
	nCount		:= array_upper(link_array, 1);
	while nIndex <= nCount loop
		if nIndex = 1 then
			link_str := cast(link_array[nIndex] as character varying);
		else
			link_str := link_str || ',' || cast(link_array[nIndex] as character varying);
		end if;
		nIndex := nIndex + 1;
	end loop;
	return link_str;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_getregulationspeed(speedclass integer)
  RETURNS smallint
  LANGUAGE plpgsql VOLATILE
  AS $$
DECLARE
	rtnvalue INT;
BEGIN
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
END;
$$;

CREATE OR REPLACE FUNCTION rdb_convert_angle(x smallint)
  RETURNS smallint 
  LANGUAGE plpgsql VOLATILE
  AS
$BODY$
BEGIN 
	return case when x >= 0 and x <=15 then 1
		when x >15 and x <= 45 then 2
		when x >45 and x <= 75 then 3
		when x >75 and x <= 105 then 4
		when x >105 and x <= 135 then 5
		when x >135 and x <= 165 then 6
		when x >165 and x <= 195 then 7
		when x >195 and x <= 225 then 8
		when x >225 and x <= 255 then 9
		when x >255 and x <= 285 then 10
		when x >285 and x <= 315 then 11
		when x >315 and x <= 345 then 12
		when x > 345 and x <360 then 1
	end;
END;
$BODY$;

CREATE OR REPLACE FUNCTION rdb_cnv_road_disp_class(link_type integer, road_type integer, private smallint, toll smallint, elevated smallint)
  RETURNS smallint
  LANGUAGE plpgsql VOLATILE
  AS $$
BEGIN
	IF elevated = 1 THEN 
		return 54; 
	END IF;
	
	IF private = 1 THEN 
		RETURN 11; 
	END IF;
	
	IF road_type = 0 THEN    -- Freeway class road1
		RETURN 53;
	END IF;
	
	if road_type = 1 then     -- Freeway class road2
		return 53;
	end if;
       
	if road_type = 2 then     -- national road
		return 42;
	end if;
	if road_type = 3 then     -- Throughway class road  
		return 41;
	end if;  
	if road_type = 4 then     -- Local road class road
		return 32;
	end if;
	--if road_type = 5 then   -- General road1 / Frontage road
	--	return ;
	--end if;
	if road_type = 6 then     -- General road2 / Very low speed road(<30KPH)
		return 21;
	end if;
	
	--if road_type = 7 then   -- Introductory Road / Private road
	--	return 11;
	--end if;
	
	if road_type = 8 then     -- Narrow Street1 / Walkway
		return 11;
	end if;
	
	if road_type = 9 then     -- Narrow Street2 / Non-navigable road 
		return 104;
        end if;
	if road_type = 10 then    -- Ferry Route
		return 100;
	end if;

        return 11;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_insert_temp_name_dictionary()
  RETURNS integer 
  LANGUAGE plpgsql VOLATILE
  AS
$$ 
DECLARE
  new_name_id integer;
  curs1 refcursor; 
  old_name_id integer;
  temp_seq_nm integer;                           
BEGIN
	new_name_id := 1;
	OPEN curs1 FOR SELECT distinct(name_id)
			FROM name_dictionary_tbl
			order by name_id;
	
	-- Get the first record    
	FETCH curs1 INTO old_name_id, temp_seq_nm;
	while old_name_id is not NULL LOOP
		INSERT INTO temp_name_dictionary(
			    "new_name_id", "old_name_id")
		    VALUES (new_name_id, old_name_id);
		new_name_id := new_name_id + 1;
		FETCH curs1 INTO old_name_id;
	END LOOP;
	close curs1;

	return new_name_id - 1;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_getpasslinkcnt(passlid character varying, passlid2 character varying)
RETURNS integer 
  LANGUAGE plpgsql VOLATILE
  AS
$$ 
DECLARE rtnvalue INT;
BEGIN
	IF ( passlid is null or passlid = '' ) THEN
		RETURN 0;
	ELSE
		rtnvalue := LENGTH(passlid) - LENGTH(REPLACE(passlid,'|','')) + 1;
                                            
		IF ( passlid2 is null ) THEN
			RETURN rtnvalue;
		ELSE
			RETURN rtnvalue + (LENGTH(passlid2) - LENGTH(REPLACE(passlid2,'|','')));
		END IF;
	END IF;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_cnv_lane_passable(lane_num integer, lane_info character varying)
  RETURNS smallint 
  LANGUAGE plpgsql
  AS $$
DECLARE
	lane smallint;
	i integer;
BEGIN
	--return (lane_info::bit(16) >> (16-lane_num))::integer::smallint 
	if lane_num is null or lane_info is null THEN
		return 0;
	end if;
	
	if lane_num > 16 THEN
		return 0;
	end if;
		
	lane := 0;
	i    := 1;
	while i <= lane_num LOOP
		if substring(lane_info, i, 1) = '1' THEN
			lane := lane | (1::smallint << (lane_num - i));
		END IF;
		i := i + 1;
	END LOOP;
            
	return lane;
END;
$$;


CREATE OR REPLACE FUNCTION rdb_cond_speed_merge_attribute
(
cond_speed smallint, 
cond_speed_source smallint,
unit smallint
)
  RETURNS SMALLINT 
  LANGUAGE plpgsql
  AS $$ 
DECLARE
rtn_value smallint;
BEGIN
    rtn_value := 0;
    --rtn_value := rtn_value | cond_speed ;
    rtn_value   := cond_speed | (cond_speed_source << 11) ;
    rtn_value   := rtn_value | (unit << 14);
    return rtn_value;
END;    
$$;


CREATE OR REPLACE FUNCTION rdb_cnt_towardname_client(name_attr smallint, name_kind smallint, passlink_count smallint)
  RETURNS smallint 
  LANGUAGE plpgsql
  AS $$
DECLARE
    multi_towardname_attr smallint;
BEGIN 
    multi_towardname_attr := 0;
    
    IF name_attr is null  and name_kind is NUll and passlink_count is NULL THEN
        return null;
    END IF;
    
    IF name_attr is not null THEN
        --IF name_attr > 7 THEN
        --END IF;
        multi_towardname_attr := name_attr;
    END IF;

    IF name_kind is not null THEN
        --IF name_kind > 3 THEN
        --END IF;
        multi_towardname_attr := multi_towardname_attr | (name_kind << 4);
    END IF;

    IF passlink_count is not null THEN
        --IF name_kind > 1023 THEN
        --END IF;
        multi_towardname_attr := multi_towardname_attr | (passlink_count << 6);
    END IF;
    
    return multi_towardname_attr;
END;
$$; 

CREATE OR REPLACE FUNCTION rdb_cvt_line_name_high_level2(intersecion_flag boolean, func_classes smallint[], high_level integer)
    RETURNS integer
    LANGUAGE plpgsql
AS $$ 
DECLARE
    sorted_func_classes character varying[];
BEGIN    
    if func_classes is null then
        return high_level;
    end if;
    
    if not intersecion_flag then  -- not local road
        return high_level;
    end if;

    -- sort the func_classes
    sorted_func_classes = array(SELECT unnest(func_classes) AS x ORDER BY x);

    if sorted_func_classes[1] = 2 then 
        return 13;
    end if;
    
    if sorted_func_classes[1] = 3 then 
        return 14;
    end if;
    
    if sorted_func_classes[1] = 4 then 
        return 15;
    end if;
    
    if sorted_func_classes[1] = 5 then 
        return 16;
    end if;
    
    return high_level;  
END;
$$;



-------------------------------------------------------------------------------------------------------------
-- check logic
-------------------------------------------------------------------------------------------------------------

CREATE OR REPLACE FUNCTION rdb_check_regulation_item()
	RETURNS integer
	LANGUAGE plpgsql
	AS $$ 
DECLARE
	rec				record;
	nIndex			integer;
	nCount			integer;
BEGIN
	for rec in
		select *
		from
		(
			select	record_no, 
					array_agg(link_id) as link_array, 
					array_agg(seq_num) as seq_array,
					array_agg(one_way) as oneway_array,
					array_agg(start_node_id) as snode_array,
					array_agg(end_node_id) as enode_array
			from
			(
				select record_no, m.link_id, seq_num, n.one_way, n.start_node_id, n.end_node_id
				from
				(
					select record_no, cast(link_array[seq_num] as bigint) as link_id, seq_num
					from
					(
						select record_no, link_array, generate_series(1,array_upper(link_array,1)) as seq_num
						from
						(
							select record_no, regexp_split_to_array(key_string, E'\\,+') as link_array
							from rdb_link_regulation
						)as a
						where (array_upper(link_array,1) > 2) or 
							  (array_upper(link_array,1) = 2 and link_array[1] != link_array[2])
					)as b
				)as m
				left join rdb_link as n
				on m.link_id = n.link_id
				order by record_no, seq_num
			)as a
			group by record_no
		)as r
	loop
		-- check linkrow continuable
		nCount		:= array_upper(rec.seq_array, 1);
		nIndex		:= 1;
		while nIndex < nCount loop
			if rec.snode_array[nIndex] in (rec.snode_array[nIndex+1], rec.enode_array[nIndex+1]) then
				if rec.oneway_array[nIndex] in (2,4) then
					raise WARNING 'regulation linkrow error: traffic flow error, record_no = %', rec.record_no;
				end if;
			elseif rec.enode_array[nIndex] in (rec.snode_array[nIndex+1], rec.enode_array[nIndex+1]) then
				if rec.oneway_array[nIndex] in (3,4) then
					raise WARNING 'regulation linkrow error: traffic flow error, record_no = %', rec.record_no;
				end if;
			else
				raise EXCEPTION 'regulation linkrow error: links have no intersection, record_no = %', rec.record_no;
			end if;
			
			nIndex		:= nIndex + 1;
		end loop;
	end loop;
	
	return 1;
END;
$$;


CREATE OR REPLACE FUNCTION rdb_check_regulation_condition()
	RETURNS integer
	LANGUAGE plpgsql
	AS $$ 
DECLARE
	rec				record;
	month			integer;
	day				integer;
	hour			integer;
	minute			integer;
BEGIN
	for rec in
		select * from rdb_link_cond_regulation
	loop
		-- check start_date
		month	:= rec.start_date / 100;
		day		:= rec.start_date % 100;
		if month = 2 then
			if not (day >= 0 and day <= 29) then
				raise EXCEPTION 'regulation condition error: start_date format is uncorrect, regulation_id = %', rec.regulation_id;
			end if;
		elseif month in (0,1,3,5,7,8,10,12) then
			if not (day >= 0 and day <= 31) then
				raise EXCEPTION 'regulation condition error: start_date format is uncorrect, regulation_id = %', rec.regulation_id;
			end if;
		elseif month in (4,6,9,11) then
			if not (day >= 0 and day <= 30) then
				raise EXCEPTION 'regulation condition error: start_date format is uncorrect, regulation_id = %', rec.regulation_id;
			end if;
		end if;
		
		-- check end_date
		month	:= rec.end_date / 100;
		day		:= rec.end_date % 100;
		if month = 2 then
			if not (day >= 0 and day <= 29) then
				raise EXCEPTION 'regulation condition error: end_date format is uncorrect, regulation_id = %', rec.regulation_id;
			end if;
		elseif month in (0,1,3,5,7,8,10,12) then
			if not (day >= 0 and day <= 31) then
				raise EXCEPTION 'regulation condition error: end_date format is uncorrect, regulation_id = %', rec.regulation_id;
			end if;
		elseif month in (4,6,9,11) then
			if not (day >= 0 and day <= 30) then
				raise EXCEPTION 'regulation condition error: end_date format is uncorrect, regulation_id = %', rec.regulation_id;
			end if;
		end if;
		
		-- check start_time
		if rec.start_time != 2400 then
			hour	:= rec.start_time / 100;
			minute	:= rec.start_time % 100;
			if (hour < 0 or hour >= 24) or (minute < 0 or minute >= 60) then
				raise EXCEPTION 'regulation condition error: start_time format is uncorrect, regulation_id = %', rec.regulation_id;
			end if;
		end if;
		
		-- check end_time
		if rec.end_time != 2400 then
			hour	:= rec.end_time / 100;
			minute	:= rec.end_time % 100;
			if (hour < 0 or hour >= 24) or (minute < 0 or minute >= 60) then
				raise EXCEPTION 'regulation condition error: end_time format is uncorrect, regulation_id = %', rec.regulation_id;
			end if;
		end if;
	
	end loop;
	
	return 1;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_check_regulation_flag()
	RETURNS integer
	LANGUAGE plpgsql
	AS $$ 
DECLARE
	rec				record;
BEGIN
	for rec in
		select *
		from
		(
			select a.link_id, a.regulation_flag, b.reg_link_id
			from rdb_link as a
			left join
			(
				select distinct unnest(string_to_array(key_string, ',')::bigint[]) as reg_link_id 
				from rdb_link_regulation
			)as b
			on a.link_id = b.reg_link_id
		)as c
		where (regulation_flag is true and reg_link_id is null) or
			  (regulation_flag is false and reg_link_id is not null)
	loop
		raise EXCEPTION 'regulation_flag error: link_id = %', rec.link_id;
	end loop;
	
	return 1;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_check_admin_zone()
	RETURNS integer
	LANGUAGE plpgsql
	AS $$ 
DECLARE
	gid_exist			integer;
BEGIN
	select a.gid
	from (select * from rdb_tile_admin_zone where ad_code != -1)as a
	left join rdb_admin_zone as b
	on a.ad_code = b.ad_code
	where b.ad_code is null
	into gid_exist;
	
	if FOUND then
		raise EXCEPTION 'rdb_tile_admin_zone ad_code error: gid = %', gid_exist;
	end if;
	
	return 1;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_auto_simplify_geom(the_geom geometry)
    RETURNS geometry 
    LANGUAGE plpgsql
AS $$ 
DECLARE 
	temp_auto_tolerance float;
	init_auto_tolerance float;
	init_repleat_cnt integer;
	i integer;
	old_shape_cnt integer;
	new_shape_cnt integer;
	rtn_geometry_value geometry;
BEGIN
	init_repleat_cnt := 10;
	i := 0;
	old_shape_cnt := st_npoints(the_geom);

	-- set initial tolerance value 
	if old_shape_cnt <= 1500 then
		init_auto_tolerance := 0.000005;
	elsif 1500 < old_shape_cnt and old_shape_cnt <= 2000 then
		init_auto_tolerance := 0.00001;
	elsif 2000 < old_shape_cnt and old_shape_cnt <= 2500 then
		init_auto_tolerance := 0.000015;
	elsif 2500 < old_shape_cnt and old_shape_cnt <= 3000 then
		init_auto_tolerance := 0.00002;
	elsif 3000 < old_shape_cnt then
		init_auto_tolerance := 0.000025;
	end if;

	temp_auto_tolerance := init_auto_tolerance;
	
	while i < init_repleat_cnt loop
		new_shape_cnt := st_npoints(ST_SimplifyPreserveTopology(the_geom,temp_auto_tolerance));
		rtn_geometry_value = ST_SimplifyPreserveTopology(the_geom,temp_auto_tolerance);

		if new_shape_cnt <= 1023 then
			exit;
		end if;

		i := i + 1;

		temp_auto_tolerance :=  init_auto_tolerance + i * 0.000005;
	end loop;

	return rtn_geometry_value;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_auto_simplify_geom_900913(the_geom geometry)
    RETURNS geometry 
    LANGUAGE plpgsql
AS $$ 
DECLARE 
	temp_tolerance float;
BEGIN
	temp_tolerance = 1.0;
	while st_npoints(the_geom) >= 1023 loop
		the_geom = ST_SimplifyPreserveTopology(the_geom,temp_tolerance);
		temp_tolerance = temp_tolerance + 0.5;
	end loop;
	return the_geom;
END;
$$;


CREATE OR REPLACE FUNCTION rdb_cal_lon(tile_x integer, tile_z integer)
  RETURNS float
  LANGUAGE plpgsql
AS $$
DECLARE
	world_x	float;
	lon	float;
BEGIN
	world_x	:= cast(tile_x as float) / power(2, tile_z);
	lon	:= (world_x - 0.5) * 360;
	return lon;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_cal_lat(tile_y integer, tile_z integer)
  RETURNS float
  LANGUAGE plpgsql
AS $$
DECLARE
	world_y	float;
	lat	float;
	PI	float;
BEGIN
	PI	:= 3.1415926535897931;
	world_y	:= cast(tile_y as float) / power(2, tile_z);
	lat	:= (2 * atan(exp((1 - 2 * world_y) * PI)) - PI / 2) * 180 / PI;
	return lat;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_cal_tilex(point_geom geometry, tile_z integer)
  RETURNS integer
  LANGUAGE plpgsql
AS $$
DECLARE
     x double precision;
     lon double precision;
     tile_x integer;
BEGIN
	lon = ST_X(point_geom);
	x = lon / 360. + 0.5;        
	tile_x = cast(floor(x * power(2, tile_z)) as integer);
	
	return tile_x;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_cal_tiley(point_geom geometry, tile_z integer)
  RETURNS integer
  LANGUAGE plpgsql
AS $$
DECLARE
     y double precision;
     lat double precision;
     tile_y integer;
	 PI	float;
BEGIN
	PI	:= 3.1415926535897931;
	lat = ST_Y(point_geom);
	y = ln(tan(PI / 4. + radians(lat) / 2.));
	y = 0.5 - y / 2. / PI;
                                    
	tile_y = cast(floor(y * power(2, tile_z)) as integer);
	
	return tile_y;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_get_tile_area_byid(tile_id integer, tile_z integer)
  RETURNS geometry
  LANGUAGE plpgsql
AS $$
DECLARE
	tile_x			integer;
	tile_y			integer;
	lon_left_top		float;
	lat_left_top		float;
	lon_right_bottom	float;
	lat_right_bottom	float;
	tile_area		geometry;
	pts			geometry[];
BEGIN
	tile_x			:= (tile_id >> 14) & ((1 << 14) - 1);
	tile_y			:= tile_id & ((1 << 14) - 1);
	
	lon_left_top		:= rdb_cal_lon(tile_x, tile_z);
	lat_left_top		:= rdb_cal_lat(tile_y, tile_z);
	
	lon_right_bottom	:= rdb_cal_lon(tile_x + 1, tile_z);
	lat_right_bottom	:= rdb_cal_lat(tile_y + 1, tile_z);

	pts			:= ARRAY
				[
					st_point(lon_left_top, lat_left_top),
					st_point(lon_right_bottom, lat_left_top),
					st_point(lon_right_bottom, lat_right_bottom),
					st_point(lon_left_top, lat_right_bottom),
					st_point(lon_left_top, lat_left_top)
				];

	tile_area		:= st_polygon(ST_LineFromMultiPoint(st_collect(pts)), 4326);

	return tile_area;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_get_tile_area_byxy(tile_x integer, tile_y integer, tile_z integer)
  RETURNS geometry
  LANGUAGE plpgsql
AS $$
DECLARE
	lon_left_top		float;
	lat_left_top		float;
	lon_right_bottom	float;
	lat_right_bottom	float;
	tile_area		geometry;
	pts			geometry[];
BEGIN	
	lon_left_top		:= rdb_cal_lon(tile_x, tile_z);
	lat_left_top		:= rdb_cal_lat(tile_y, tile_z);
	
	lon_right_bottom	:= rdb_cal_lon(tile_x + 1, tile_z);
	lat_right_bottom	:= rdb_cal_lat(tile_y + 1, tile_z);

	pts			:= ARRAY
				[
					st_point(lon_left_top, lat_left_top),
					st_point(lon_right_bottom, lat_left_top),
					st_point(lon_right_bottom, lat_right_bottom),
					st_point(lon_left_top, lat_right_bottom),
					st_point(lon_left_top, lat_left_top)
				];

	tile_area		:= st_polygon(ST_LineFromMultiPoint(st_collect(pts)), 4326);

	return tile_area;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_multiline2multipolygon(multiline geometry)
    RETURNS geometry
    LANGUAGE plpgsql
AS $$
DECLARE
    multipolygon		  geometry;
BEGIN
    select st_multi(st_collect(st_polygon(the_geom, 4326))) as the_geom
    from
    (
	    select st_geometryn(the_geom, polyline_index) as the_geom
	    from
	    (
	    	select the_geom, generate_series(1,polyline_count) as polyline_index
	    	from
	    	(
	    		select multiline as the_geom, ST_NumGeometries(multiline) as polyline_count
	    	)as a
	    )as b
    )as c
    into multipolygon;
    
    return multipolygon;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_makepolygon2bytea(the_geom geometry)
    RETURNS bytea
    LANGUAGE plpgsql
AS $$
DECLARE
    geom_content 	bytea;
    
    boundary_geom 	geometry;
    polyline_cnt  	integer;
    polyline_idx  	integer;
    
    polyline_geom 	geometry;
    point_cnt  		integer;
    point_idx	  	integer;
    
    point_geom		geometry;
    LON 			integer;
    LAT 			integer;
BEGIN
        geom_content	:= '';
    	boundary_geom	:= st_multi(st_boundary(the_geom));
    	polyline_cnt	:= st_numgeometries(boundary_geom);
    	for polyline_idx in 1..polyline_cnt loop
	        polyline_geom	:= st_geometryn(boundary_geom, polyline_idx);
	        point_cnt		:= st_npoints(polyline_geom);
	        for point_idx in 1..point_cnt loop
	        	point_geom	:= st_pointn(polyline_geom, point_idx);
	            LON := cast(round(st_x(point_geom) * 256 * 3600) as int);
	            LAT := cast(round(st_y(point_geom) * 256 * 3600) as int);
	            geom_content := geom_content || rdb_integer_2_octal_bytea(LON) || rdb_integer_2_octal_bytea(LAT);
	        end loop;
        end loop;
        return geom_content;
END;
$$;


                        
CREATE OR REPLACE FUNCTION rdb_makepolygon2pixelbytea(tz smallint,tx int,ty int,the_geom geometry)
    RETURNS bytea
    LANGUAGE plpgsql
AS $$
DECLARE
    geom_content 	bytea;
    
    boundary_geom 	geometry;
    polyline_cnt  	integer;
    polyline_idx  	integer;
    
    polyline_geom 	geometry;
    point_cnt  		integer;
    point_idx	  	integer;
    record_xy		record;
    
    point_geom		geometry;
    LON 			integer;
    LAT 			integer;
BEGIN
        geom_content	:= '';
    	boundary_geom	:= st_multi(st_boundary(the_geom));
    	polyline_cnt	:= st_numgeometries(boundary_geom);
    	for polyline_idx in 1..polyline_cnt loop
	        polyline_geom	:= st_geometryn(boundary_geom, polyline_idx);
	        point_cnt		:= st_npoints(polyline_geom);
	        for point_idx in 1..point_cnt loop
	            point_geom	:= st_pointn(polyline_geom, point_idx);
				record_xy := lonlat2pixel_tile(tz::smallint, tx::integer, ty::integer, st_x(point_geom)::float, st_y(point_geom)::float, 4096::smallint);
	            LON := record_xy.x;
	            LAT := record_xy.y;
	            geom_content := geom_content || rdb_smallint_2_octal_bytea(LON::smallint) || rdb_smallint_2_octal_bytea(LAT::smallint);
	        end loop;
        end loop;
        return geom_content;
END;
$$;           

---------------------------------------------------------------------------------------
-- Convert branch of node to json format
---------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION rdb_cvt_branch_to_json_format(branches bigint[])
  RETURNS character varying
  LANGUAGE plpgsql 
  AS $$
DECLARE
	i integer;
	json_str character varying;
	tile_id_str character varying;
	link_id_str character varying;
BEGIN
	i        := 1;
	json_str := '[';
	
	while i <= array_upper(branches, 1) LOOP
		tile_id_str := (branches[i] >> 32)::character varying;
		link_id_str := (rdb_split_tileid(branches[i]))::character varying;

		-- "[[tile_id1,link_id1],[tile_id2,link_id2],[tile_id3,link_id3]]"
		if i > 1 then 
			json_str := json_str || ',';
		end if;
		
		json_str    := json_str || '[' || tile_id_str || ',' || link_id_str || ']';
		i := i + 1;
	END LOOP;

	json_str := json_str || ']';
	return json_str;
END;
$$;



CREATE OR REPLACE FUNCTION rdb_get_friendly_guidetype(laneinfo varchar, guidetype smallint,arrowinfo smallint)
    RETURNS smallint
    LANGUAGE plpgsql
AS $$
DECLARE
	friendly_guidetype	smallint;
	dir					smallint;
	lanenum				smallint;
	lanenum_walkable	smallint;
	laneidx_left		smallint;
	laneidx_right		smallint;
BEGIN
	if guidetype >= 32 then 
		return guidetype; -- return friendly type directly
	end if ; 

	-- analyze linkrow dir
	if guidetype = 2 then
		dir := 0;	-- dir is straight
	elseif guidetype in (3,4,5,6,7,8) then
		dir := 1;	-- dir is right
	elseif guidetype in (9,10,11,12,13) then
		dir := 2;	-- dir is left
	elseif guidetype = 1 and (position('0' in laneinfo) > 0 or position('2' in laneinfo) > 0) then
		if arrowinfo = 1 then 
			dir := 0;	-- dir is straight
		elseif arrowinfo in (2,4,8,256,2048,4096) then
			dir := 1;	-- dir is right
		elseif arrowinfo in (16,32,64,128,512,8192) then
			dir := 2;	-- dir is left
		else
			dir := 0;	-- dir is straight
		end if;
	else
		return NULL;
	end if;
	
	-- analyze continues walkable lanes's index
	lanenum	:= length(laneinfo);
	if dir = 1 then
		laneidx_right	:= lanenum;
		while True loop
			if laneidx_right = 0 then
				return NULL;
			end if;
			if substr(laneinfo, laneidx_right, 1) = '1' then
				exit;
			end if;
			laneidx_right	:= laneidx_right - 1;
		end loop;
		
		laneidx_left 	:= laneidx_right;
		while laneidx_left >= 2 loop
			if substr(laneinfo, laneidx_left-1, 1) <> '1' then
				exit;
			end if;
			laneidx_left	:= laneidx_left - 1;
		end loop;
	else
		laneidx_left	:= 1;
		while True loop
			if laneidx_left > lanenum then
				return NULL;
			end if;
			if substr(laneinfo, laneidx_left, 1) = '1' then
				exit;
			end if;
			laneidx_left	:= laneidx_left + 1;
		end loop;
		
		laneidx_right 	:= laneidx_left;
		while laneidx_right <= (lanenum - 1) loop
			if substr(laneinfo, laneidx_right+1, 1) <> '1' then
				exit;
			end if;
			laneidx_right	:= laneidx_right + 1;
		end loop;
	end if;
	
	--raise INFO 'laneidx_left = %, laneidx_right = %', laneidx_left, laneidx_right;
	-- the number of walkable lanes must be legal
	lanenum_walkable	:= (laneidx_right-laneidx_left+1);
	if lanenum_walkable = lanenum or lanenum_walkable > 4 then
		return NULL;
	end if;
	
	-- set the friendly guidtype
	if laneidx_left = 1 then
		-- left side lane
		friendly_guidetype := 64 + (lanenum_walkable-1) * 3 + dir;
	elseif laneidx_right = lanenum then
		-- right side lane
		friendly_guidetype := 32 + (lanenum_walkable-1) * 3 + dir;
	else
		-- middle side lane, the number of left unwalkable lanes and left unwalkable lanes must be the same
		if (laneidx_left-1) != (lanenum-laneidx_right) then
			return NULL;
		else
			friendly_guidetype := 48 + (lanenum_walkable-1) * 3 + dir;
		end if;
	end if;
	
	return friendly_guidetype;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_get_friendly_guidetype_common_road(laneinfo varchar, guidetype smallint)
    RETURNS smallint
    LANGUAGE plpgsql
AS $$
DECLARE
	friendly_guidetype	smallint;
	dir					smallint;
	lanenum				smallint;
	lanenum_walkable	smallint;
	laneidx_left		smallint;
	laneidx_right		smallint;
BEGIN
	if guidetype >= 32 then 
		return guidetype; -- return friendly type directly
	end if ; 

	-- analyze linkrow dir
	if guidetype = 2 then
		dir := 0;	-- dir is straight
	elseif guidetype = 3 then
		dir := 1;	-- dir is right
	elseif guidetype in (4,5,6) then
		dir := 2;	-- dir is right
	elseif guidetype in (7,8) then
		dir := 3;	-- dir is right
	elseif guidetype = 9 then
		dir := 4;	-- dir is right
	elseif guidetype in (10,11,12) then
		dir := 5;	-- dir is right
	elseif guidetype = 13 then
		dir := 6;	-- dir is right
	else
		return NULL;
	end if;
	
	-- analyze continues walkable lanes's index
	lanenum	:= length(laneinfo);
	if dir in (1,2,3) then
		laneidx_right	:= lanenum;
		while True loop
			if laneidx_right = 0 then
				return NULL;
			end if;
			if substr(laneinfo, laneidx_right, 1) = '1' then
				exit;
			end if;
			laneidx_right	:= laneidx_right - 1;
		end loop;
		
		laneidx_left 	:= laneidx_right;
		while laneidx_left >= 2 loop
			if substr(laneinfo, laneidx_left-1, 1) <> '1' then
				exit;
			end if;
			laneidx_left	:= laneidx_left - 1;
		end loop;
	else
		laneidx_left	:= 1;
		while True loop
			if laneidx_left > lanenum then
				return NULL;
			end if;
			if substr(laneinfo, laneidx_left, 1) = '1' then
				exit;
			end if;
			laneidx_left	:= laneidx_left + 1;
		end loop;
		
		laneidx_right 	:= laneidx_left;
		while laneidx_right <= (lanenum - 1) loop
			if substr(laneinfo, laneidx_right+1, 1) <> '1' then
				exit;
			end if;
			laneidx_right	:= laneidx_right + 1;
		end loop;
	end if;
	
	--raise INFO 'laneidx_left = %, laneidx_right = %', laneidx_left, laneidx_right;
	-- the number of walkable lanes must be legal
	lanenum_walkable	:= (laneidx_right-laneidx_left+1);
	if lanenum_walkable = lanenum or lanenum_walkable > 8 then
		return NULL;
	end if;
	
	-- set the friendly guidtype
	if laneidx_left = 1 then
		-- left side lane
		friendly_guidetype := 192 + (lanenum_walkable-1) * 7 + dir;
	elseif laneidx_right = lanenum then
		-- right side lane
		friendly_guidetype := 80 + (lanenum_walkable-1) * 7 + dir;
	else
		-- middle side lane, the number of left unwalkable lanes and left unwalkable lanes must be the same
		if (laneidx_left-1) != (lanenum-laneidx_right) then
			return NULL;
		else
			friendly_guidetype := 136 + (lanenum_walkable-1) * 7 + dir;
		end if;
	end if;
	
	return friendly_guidetype;
END;
$$;



CREATE OR REPLACE FUNCTION rdb_search_region_linkrow()
	RETURNS smallint
	LANGUAGE plpgsql volatile
AS $$
DECLARE
	rec					temp_region_merge_link_suspect;
	rec2				record;
	pos_rec				record;
	neg_rec				record;
	dir_search_e		integer;
	dir_search_s		integer;
	start_node			bigint;
	end_node			bigint;
	link_array			bigint[];
	linkdir_array		boolean[];
	nCount				integer;
	nIndex				integer;
BEGIN
	for rec in
		select	link_id, region_tile_id, max_level, abs_link_id, start_node_id, end_node_id, one_way, 
				toll, road_name, road_number, road_type, pdm_flag, bypass_flag, highcost_flag, path_extra_info
--				, link_type, etc_lane_flag
--				, speed_limit_s2e, speed_limit_e2s
--				, disobey_flag, lane_dir, lane_up_down_distingush
--				, ops_lane_number, ops_width, neg_lane_number, neg_width
		from temp_region_merge_link_suspect
		order by link_id
	loop
		-- mark walked_flag of current link
		select * 
		from temp_region_merge_link_walked
		where link_id = rec.link_id
		into rec2;
		if found then
			continue;
		else
			insert into temp_region_merge_link_walked ("link_id") values (rec.link_id);
		end if;
		
		-- search in positive and negative digitized direction
		--raise INFO 'link_id = %', rec.link_id;
		
		pos_rec			:= rdb_search_region_linkrow_in_one_direction(rec, rec.start_node_id, rec.end_node_id, 2);
		end_node		:= pos_rec.reach_node;
		
		neg_rec			:= rdb_search_region_linkrow_in_one_direction(rec, end_node, rec.start_node_id, 3);
		start_node		:= neg_rec.reach_node;
		
		link_array		:= neg_rec.linkrow || ARRAY[rec.link_id] || pos_rec.linkrow;
		linkdir_array	:= neg_rec.linkdirrow || ARRAY[True] || pos_rec.linkdirrow;
		
		-- insert new linkrow
		nCount			:= array_upper(link_array, 1);
		if nCount > 1 then
			insert into temp_region_merge_linkrow ("link_id", "start_node_id", "end_node_id", "one_way", "link_num", "link_array", "linkdir_array")
				values (rec.link_id, start_node, end_node, rec.one_way, nCount, link_array, linkdir_array);
		end if;
	end loop;
	
	RETURN 1;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_search_region_linkrow_in_one_direction(	
								rec temp_region_merge_link_suspect, static_node bigint, search_node bigint, dir integer, 
								OUT linkrow bigint[], OUT linkdirrow boolean[], OUT reach_node bigint, OUT reach_link bigint)
	RETURNS record
	LANGUAGE plpgsql volatile
AS $$
DECLARE
	rec2				record;
BEGIN
	-- init
	reach_link	:= rec.link_id;
	reach_node	:= search_node;
	
	-- search
	while true loop
		-- check node is not end node
		select *
		from temp_region_merge_node_suspect
		where node_id = reach_node
		into rec2;
		if not found then
			exit;
		end if;
		
		-- search link
		select	a.*
		from
		(
			(
				select	link_id, region_tile_id, max_level, 
						True as linkdir, 
						(case when (reach_node = start_node_id) then end_node_id else start_node_id end) as nextnode, 
						one_way, 
						toll, bypass_flag, highcost_flag, road_name, road_number, road_type, pdm_flag, path_extra_info
--						, link_type, etc_lane_flag
--						, disobey_flag, lane_dir, lane_up_down_distingush
--						, ops_lane_number, ops_width
--						, neg_lane_number, neg_width
--						, speed_limit_s2e, speed_limit_e2s
				from temp_region_merge_link_suspect
				where 	(
							(reach_node = start_node_id) and (dir = 2)
							or
							(reach_node = end_node_id) and (dir = 3)
						)
						and 
						(link_id != reach_link)
			)
			union
			(
				select	link_id, region_tile_id, max_level, 
						False as linkdir, 
						(case when (reach_node = start_node_id) then end_node_id else start_node_id end) as nextnode, 
						(case when one_way = 2 then 3 when one_way = 3 then 2 else one_way end) as one_way, 
						toll, bypass_flag, highcost_flag, road_name, road_number, road_type, pdm_flag, path_extra_info
--						, link_type, etc_lane_flag
--						, disobey_flag, lane_dir, lane_up_down_distingush
--						, neg_lane_number as ops_lane_number, neg_width as ops_width
--						, ops_lane_number as neg_lane_number, ops_width as neg_width
--						, speed_limit_e2s as speed_limit_s2e, speed_limit_s2e as speed_limit_e2s
				from temp_region_merge_link_suspect
				where 	(
							(reach_node = start_node_id) and (dir = 3)
							or
							(reach_node = end_node_id) and (dir = 2)
						)
						and 
						(link_id != reach_link)
			)
		)as a
		left join temp_region_merge_link_walked as b
		on a.link_id = b.link_id
		where b.link_id is null
		into rec2;
		
		if	FOUND
			and	not (rec.region_tile_id is distinct from rec2.region_tile_id)
			and	not (rec.max_level is distinct from rec2.max_level)
			and	not (rec.one_way is distinct from rec2.one_way)
			and	not (rec.toll is distinct from rec2.toll)
			and	not (rec.bypass_flag is distinct from rec2.bypass_flag)
			and	not (rec.highcost_flag is distinct from rec2.highcost_flag)
			and	not (rec.road_type is distinct from rec2.road_type)
			and	not (rec.pdm_flag is distinct from rec2.pdm_flag)
			and	not (rec.path_extra_info is distinct from rec2.path_extra_info)
			and	(
					not (rec.road_name is distinct from rec2.road_name)
					or 
					(rec.road_number is not null and not (rec.road_number is distinct from rec2.road_number))
				)
--			and	not (rec.extend_flag is distinct from rec2.extend_flag)
--			and	not (rec.struct_code is distinct from rec2.struct_code)
--			and	not (rec.etc_lane_flag is distinct from rec2.etc_lane_flag)
--			and	not (rec.link_type is distinct from rec2.link_type)
--			and	not (rec.speed_limit_s2e is distinct from rec2.speed_limit_s2e)
--			and	not (rec.speed_limit_e2s is distinct from rec2.speed_limit_e2s)
		then
			-- mark walked_flag of current link
			insert into temp_region_merge_link_walked ("link_id") values (rec2.link_id);
			
			-- walk this link
			if (dir = 2) then
				linkrow		:= array_append(linkrow, rec2.link_id);
				linkdirrow	:= array_append(linkdirrow, rec2.linkdir);
			else
				linkrow		:= array_prepend(rec2.link_id, linkrow);
				linkdirrow	:= array_prepend(rec2.linkdir, linkdirrow);
			end if;
			reach_link	:= rec2.link_id;
			reach_node	:= rec2.nextnode;
		else
			EXIT;
		end if;
	end loop;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_split_region_linkrow()
  RETURNS smallint 
  LANGUAGE plpgsql
  AS $$ 
DECLARE
	rec						record;
	nIndex					integer;
	
	nLength					integer;
	proxy_linkid			bigint;
	proxy_oneway			smallint;
	proxy_linkdir			boolean;
	current_linkdir			boolean;
	linkrow_snode			bigint;
	linkrow_enode			bigint;
	linkrow_link_num		integer;
	linkrow_link_array		bigint[];
	linkrow_dir_array		boolean[];
BEGIN
	-- backup
	drop table if exists temp_region_merge_linkrow_bak_split;
	create table temp_region_merge_linkrow_bak_split 
	as 
	select * from temp_region_merge_linkrow;
	
	-- split linkrow which is longer than 50km
	for rec in
		select 	link_id, sub_count, 
				array_agg(sub_link) as link_array, 
				array_agg(sub_dir) as dir_array,
				array_agg(one_way) as oneway_array,
				array_agg(link_length) as length_array,
				array_agg(start_node_id) as snode_array, 
				array_agg(end_node_id) as enode_array
		from
		(
			select a.link_id, a.sub_count, a.sub_index, a.sub_link, a.sub_dir, b.start_node_id, b.end_node_id, b.one_way, b.link_length
			from
			(
				select link_id, sub_count, sub_index, link_array[sub_index] as sub_link, linkdir_array[sub_index] as sub_dir
				from
				(
					select link_id, link_num as sub_count, link_array, linkdir_array, generate_series(1,link_num) as sub_index
					from temp_region_merge_linkrow
				)as tl
			)as a
			left join rdb_link as b
			on a.sub_link = b.link_id
			where a.sub_count > 1 and b.road_type != 10
			order by a.link_id, a.sub_count, a.sub_index
		)as t
		group by link_id, sub_count having sum(link_length) > 50000
	loop
		-- delete old record
		delete from temp_region_merge_linkrow where link_id = rec.link_id;
	
		-- split linkrow
		nIndex	:= 1;
		while nIndex <= rec.sub_count loop
			proxy_linkid			:= rec.link_array[nIndex];
			proxy_oneway			:= rec.oneway_array[nIndex];
			proxy_linkdir			:= rec.dir_array[nIndex];
			
			linkrow_link_num		:= 1;
			nLength					:= rec.length_array[nIndex];
			linkrow_link_array		:= ARRAY[rec.link_array[nIndex]];
			linkrow_dir_array		:= ARRAY[True];
			linkrow_snode			:= rec.snode_array[nIndex];
			linkrow_enode			:= rec.enode_array[nIndex];
			
			nIndex := nIndex + 1;
			while True loop
				if (nIndex > rec.sub_count) or (nLength + rec.length_array[nIndex] > 50000) then
					insert into temp_region_merge_linkrow 
								("link_id", "start_node_id", "end_node_id", "one_way", "link_num", "link_array", "linkdir_array")
						values 	(proxy_linkid, linkrow_snode, linkrow_enode, proxy_oneway, linkrow_link_num, linkrow_link_array, linkrow_dir_array);
					exit;
				else
					linkrow_link_num 	:= linkrow_link_num + 1;
					nLength				:= nLength + rec.length_array[nIndex];
					current_linkdir		:= (rec.dir_array[nIndex] = proxy_linkdir);
					
					if proxy_linkdir then
						linkrow_link_array	:= array_append(linkrow_link_array, rec.link_array[nIndex]);
						linkrow_dir_array	:= array_append(linkrow_dir_array, current_linkdir);
						if current_linkdir then
							linkrow_enode	:= rec.enode_array[nIndex];
						else
							linkrow_enode	:= rec.snode_array[nIndex];
						end if;
					else
						linkrow_link_array	:= array_prepend(rec.link_array[nIndex], linkrow_link_array);
						linkrow_dir_array	:= array_prepend(current_linkdir, linkrow_dir_array);
						if current_linkdir then
							linkrow_snode	:= rec.snode_array[nIndex];
						else
							linkrow_snode	:= rec.enode_array[nIndex];
						end if;
					end if;
					
					nIndex := nIndex + 1;
				end if;
			end loop;
		end loop;
	end loop;
	
	return 0;
END;
$$;


CREATE OR REPLACE FUNCTION rdb_delete_isolated_ic_from_region()
  RETURNS smallint 
  LANGUAGE plpgsql
  AS $$ 
DECLARE
	ic_num_old	integer;
	ic_num_new	integer;
BEGIN
	select count(link_id)
	from temp_region_delete_ic
	into ic_num_old;
	
	while true loop
--		-- delete link which has isolated node
--		insert into temp_region_delete_ic
--		select a.link_id
--		from temp_region_links as a
--		left join temp_region_delete_ic as b
--		on a.link_id = b.link_id
--		left join
--		(
--			select distinct a.link_id
--			from temp_region_links as a
--			left join temp_region_delete_ic as b
--			on a.link_id = b.link_id
--			left join temp_region_links as c
--			on (a.link_id != c.link_id) and a.start_node_id in (c.start_node_id, c.end_node_id)
--			left join temp_region_delete_ic as d
--			on c.link_id = d.link_id
--			left join temp_region_links as e
--			on (a.link_id != e.link_id) and a.end_node_id in (e.start_node_id, e.end_node_id)
--			left join temp_region_delete_ic as f
--			on e.link_id = f.link_id
--			where (a.link_type in (3,5) and b.link_id is null)
--				  and 
--				  (c.link_id is not null and d.link_id is null) 
--				  and 
--				  (e.link_id is not null and f.link_id is null)
--		)as c
--		on a.link_id = c.link_id
--		where (a.link_type in (3,5) and b.link_id is null) and c.link_id is null
--		;
		
		-- delete link which has isolated traffic node
		drop table if exists temp_region_traffic_through_node;
		create temp table temp_region_traffic_through_node
		as
		(
			select distinct a.node_id
			from
			(
				select distinct node_id
				from
				(
					select start_node_id as node_id
					from temp_region_links
					where link_type in (3,5,7)
					union
					select end_node_id as node_id
					from temp_region_links
					where link_type in (3,5,7)
				)as t
			)as a
			left join temp_region_links as b
			on (a.node_id = b.start_node_id and b.one_way in (1,2)) or (a.node_id = b.end_node_id and b.one_way in (1,3))
			left join temp_region_delete_ic as c
			on b.link_id = c.link_id
			left join temp_region_links as d
			on (a.node_id = d.start_node_id and d.one_way in (1,3)) or (a.node_id = d.end_node_id and d.one_way in (1,2))
			left join temp_region_delete_ic as e
			on d.link_id = e.link_id
			where b.link_id is not null and c.link_id is null and d.link_id is not null and e.link_id is null
		);
		
		insert into temp_region_delete_ic
		select distinct a.link_id
		from temp_region_links as a
		left join temp_region_delete_ic as b
		on a.link_id = b.link_id
		left join temp_region_traffic_through_node as c
		on a.start_node_id = c.node_id
		left join temp_region_traffic_through_node as d
		on a.end_node_id = d.node_id
		where (a.link_type in (3,5,7) and b.link_id is null) and (c.node_id is null or d.node_id is null)
		;
		
		--
		select count(link_id)
		from temp_region_delete_ic
		into ic_num_new;
		
		if (ic_num_old is distinct from ic_num_new) then
			ic_num_old 	:= ic_num_new;
		else
			exit;
		end if;
	end loop;
	
	return 0;
END;
$$;


CREATE OR REPLACE FUNCTION rdb_find_isolated_link_from_region()
  RETURNS bigint 
  LANGUAGE plpgsql
  AS $$ 
DECLARE
	island_id	 bigint;
	
    nWalkedLink  bigint;
    curs1        refcursor;
    curs2        refcursor;
    rec1         record;
    rec2         record;

    listNode     bigint[];
    nIndex       bigint;
    nCurNodeID   bigint;

    nNextLink    bigint;
    nNextNode    bigint;
BEGIN
	island_id	:= 0;
	
	While True LOOP
		-- find a node as the start point of a new island
		select start_node_id
		from temp_region_links as a
		left join temp_region_walked_link as b
		on a.link_id = b.link_id
		where b.link_id is null
		limit 1
		into nCurNodeID;
		
		if not FOUND then 
			exit;
		else
			island_id	:= island_id + 1;
		end if;
	
		-- walk around current island
	    nWalkedLink   := 0;
	    nIndex        := 1;
	    listNode[nIndex]:= nCurNodeID;
	    WHILE nIndex > 0 LOOP
	        nCurNodeID    := listNode[nIndex];
	        nIndex        := nIndex - 1;
	
	        open curs1 for 
	            select *
	                     from
	                     (
		                    select link_id as nextlink,
		                           end_node_id as nextnode
		                    from temp_region_links
		                    where (start_node_id = nCurNodeID and one_way in (1,2,3))
	                     ) as a
	                     union
	                     (
		                    select link_id as nextlink,
		                           start_node_id as nextnode
		                    from temp_region_links
		                    where (end_node_id = nCurNodeID and one_way in (1,2,3))
	                     )
	        ;
	
	        fetch curs1 into rec1;
	        while rec1 is not null loop
	            nNextLink    := rec1.nextlink;
	            nNextNode    := rec1.nextnode;
	
	            open curs2 for
	                select link_id 
	                from temp_region_walked_link 
	                where link_id = nNextLink
	            ;
	            fetch curs2 into rec2;
	            if rec2 is null then
	                nWalkedLink    := nWalkedLink + 1;
	
	                nIndex        := nIndex + 1;
	                listNode[nIndex]:= nNextNode;
	
	                INSERT INTO temp_region_walked_link(link_id, island_id) VALUES (nNextLink, island_id);
	            end if;
	            close curs2;
	
	            fetch curs1 into rec1;
	        end loop;
	
	        close curs1;
	    END LOOP;
    END LOOP;
    
    return island_id;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_find_sa_ramp_from_region()
  RETURNS bigint 
  LANGUAGE plpgsql
  AS $$ 
DECLARE
	rec					record;
	rec2				record;
	exp_count			integer;
	exp_index			integer;
	exp_node_array		bigint[];
	exp_dir_array		boolean[];
	exp_link_array		bigint[];
	exp_path_array		varchar[];
BEGIN
	for rec in
		select node_id
		from
		(
			select distinct a.node_id, b.link_type
			from temp_region_nodes as a
			left join temp_region_links as b
			on a.node_id in (b.start_node_id, b.end_node_id)
			where b.link_type in (1,2,7)
		)as t
		group by node_id having count(*) > 1
	loop
		exp_node_array	:= ARRAY[rec.node_id, rec.node_id];		-- current position
		exp_dir_array	:= ARRAY[true, false];					-- explore direction, True:forward, False:backward
		exp_link_array	:= ARRAY[0::bigint, 0::bigint];			-- last link
		exp_path_array	:= ARRAY['0', '0'];						-- explore roadmap
		
		exp_count		:= 2;
		exp_index		:= 1;
		while exp_index <= exp_count loop
			for rec2 in
				select end_node_id as nextnode, link_id, (link_id || '+' || 1) as link_dir, link_type
				from temp_region_links
				where 	(exp_node_array[exp_index] = start_node_id) 
					and (
						(exp_dir_array[exp_index] and one_way in (1,2))
						or
						(not exp_dir_array[exp_index] and one_way in (1,3))
						)
				union
				select start_node_id as nextnode, link_id, (link_id || '+' || 2) as link_dir, link_type
				from temp_region_links
				where 	(exp_node_array[exp_index] = end_node_id) 
					and (
						(exp_dir_array[exp_index] and one_way in (1,3))
						or
						(not exp_dir_array[exp_index] and one_way in (1,2))
						)
			loop
				if 		(rec2.link_type = 7) 
					and (rec2.link_id != exp_link_array[exp_index] )
					and not (rec2.link_dir = ANY(string_to_array(exp_path_array[exp_index], ',')))
				then
					-- go on exploring
					exp_count					:= exp_count + 1;
					exp_node_array[exp_count]	:= rec2.nextnode;
					exp_dir_array[exp_count]	:= exp_dir_array[exp_index];
					exp_link_array[exp_count]	:= rec2.link_id;
					exp_path_array[exp_count]	:= exp_path_array[exp_index] || ',' || rec2.link_dir;
				elseif rec2.link_type in (3,5) then
					-- end current path, mark walked link as sa_ramp
					insert into temp_region_links_sa_ramp
					select (string_to_array(link_dir, '+'))[1]::bigint
					from
					(
						select unnest(string_to_array(exp_path_array[exp_index], ',')) as link_dir
					)as t
					where link_dir != '0'
					;
				else
					-- end current path
				end if;
			end loop;
			
			exp_index	:= exp_index + 1;
		end loop;
	end loop;
	return 0;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_region_find_sa_path()
  RETURNS bigint 
  LANGUAGE plpgsql
  AS $$ 
DECLARE
	rec						record;
	rec2					record;
	exp_count				integer;
	exp_index				integer;
	exp_curnode_array		bigint[];
	exp_prelink_array		bigint[];
	exp_cost_array			float[];
	exp_path_array			varchar[];
BEGIN
	for rec in
		-- for all entry point of sapa area
		select distinct a.node_id
		from
		(
			select start_node_id as node_id
			from temp_region_links
			where link_type = 7 and one_way in (1,2)
			union
			select end_node_id as node_id
			from temp_region_links
			where link_type = 7 and one_way in (1,3)
		)as a
		inner join temp_region_links as b
		on 	b.link_type in (1,2,3,5)
			and 
			(
			(a.node_id = b.start_node_id and b.one_way in (1,3))
			or 
			(a.node_id = b.end_node_id and b.one_way in (1,2))
			)
	loop
		exp_curnode_array	:= ARRAY[rec.node_id];			-- current position
		exp_prelink_array	:= ARRAY[0::bigint];			-- last link
		exp_cost_array		:= ARRAY[0::float];				-- cost
		exp_path_array		:= ARRAY['0'];					-- explore roadmap
		
		exp_count		:= 1;
		exp_index		:= 1;
		while exp_index <= exp_count loop
			for rec2 in
				select end_node_id as nextnode, link_id, (link_id || '+' || 1) as link_dir, link_type, link_length
				from temp_region_links
				where (exp_curnode_array[exp_index] = start_node_id) and one_way in (1,2)
				union
				select start_node_id as nextnode, link_id, (link_id || '+' || 2) as link_dir, link_type, link_length
				from temp_region_links
				where (exp_curnode_array[exp_index] = end_node_id) and one_way in (1,3)
			loop
				if 		(rec2.link_type = 7)
					and (rec2.link_id != exp_prelink_array[exp_index])
					and not (rec2.link_dir = ANY(string_to_array(exp_path_array[exp_index], ',')))
				then
					-- go on exploring
					exp_count						:= exp_count + 1;
					exp_curnode_array[exp_count]	:= rec2.nextnode;
					exp_prelink_array[exp_count]	:= rec2.link_id;
					exp_cost_array[exp_count]		:= exp_cost_array[exp_index] + rec2.link_length;
					exp_path_array[exp_count]		:= exp_path_array[exp_index] || ',' || rec2.link_dir;
				elseif rec2.link_type in (1,2,3,5) then
					-- end current path, mark walked link as sa_ramp
					insert into temp_region_links_sa_path(fnode, tnode, cost, link_array)
					select	fnode, tnode, cost, array_agg(link_id) as link_array
					from
					(
						select	fnode, tnode, cost, 
								(string_to_array(link_and_dir, '+'))[1]::bigint as link_id
						from
						(
							select	rec.node_id as fnode, 
									exp_curnode_array[exp_index] as tnode, 
									exp_cost_array[exp_index] as cost,
									unnest(string_to_array(exp_path_array[exp_index], ',')) as link_and_dir
						)as t
						where link_and_dir != '0' and fnode != tnode
					)as t2
					group by fnode, tnode, cost
					;
				else
					-- end current path
				end if;
			end loop;
			
			exp_index	:= exp_index + 1;
		end loop;
	end loop;
	return 0;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_region_check_sa_path_necessary(fnode bigint, tnode bigint, cost float)
  RETURNS boolean
  LANGUAGE plpgsql
  AS $$ 
DECLARE
	rec						record;
	rec2					record;
	max_cost				float;
	next_cost				float;
	exp_count				integer;
	exp_index				integer;
	exp_curnode_array		bigint[];
	exp_prelink_array		bigint[];
	exp_cost_array			float[];
	exp_path_array			varchar[];
BEGIN
	-- explore max cost
	max_cost			:= cost * 1.1;
	
	--
	for rec in
		select ARRAY[1,2] as exp_linktype_array
		union
		select ARRAY[3,5] as exp_linktype_array
	loop
		exp_curnode_array	:= ARRAY[fnode];			-- current position
		exp_prelink_array	:= ARRAY[0::bigint];		-- last link
		exp_cost_array		:= ARRAY[0::float];			-- cost
		exp_path_array		:= ARRAY['0'];				-- explore roadmap
		
		exp_count		:= 1;
		exp_index		:= 1;
		while exp_index <= exp_count loop
			for rec2 in
				select *
				from
				(
					select end_node_id as nextnode, link_id, (link_id || '+' || 1) as link_dir, link_type, link_length
					from temp_region_links
					where (exp_curnode_array[exp_index] = start_node_id) and one_way in (1,2)
					union
					select start_node_id as nextnode, link_id, (link_id || '+' || 2) as link_dir, link_type, link_length
					from temp_region_links
					where (exp_curnode_array[exp_index] = end_node_id) and one_way in (1,3)
				)as t
				where 		link_type = ANY(rec.exp_linktype_array)
						and (link_id != exp_prelink_array[exp_index])
						and not (link_dir = ANY(string_to_array(exp_path_array[exp_index], ',')))
			loop
				next_cost	:= exp_cost_array[exp_index] + rec2.link_length;
				if next_cost > max_cost then 
					continue;
				elseif rec2.nextnode = tnode then
					return false;
				else
					-- go on exploring
					exp_count						:= exp_count + 1;
					exp_curnode_array[exp_count]	:= rec2.nextnode;
					exp_prelink_array[exp_count]	:= rec2.link_id;
					exp_cost_array[exp_count]		:= exp_cost_array[exp_index] + rec2.link_length;
					exp_path_array[exp_count]		:= exp_path_array[exp_index] || ',' || rec2.link_dir;
				end if;
			end loop;
			exp_index	:= exp_index + 1;
		end loop;
	end loop;
	return true;
END;
$$;

create or replace function get_new_tile_id_by_zlevel(base14_tile_id int, target_z_level int)
	returns int
	LANGUAGE plpgsql
AS $$ 
DECLARE
	tile14_x int;
	tile14_y int;
	target_tile_x int;
	target_tile_y int;
	target_tile_id int;
BEGIN
	tile14_y := base14_tile_id & 16383;
	tile14_x := (base14_tile_id >> 14) & 16383;
	
	target_tile_x := cast (floor(cast(tile14_x as numeric) / ( power(2, 14 - target_z_level))) as int);
	target_tile_y := cast (floor(cast(tile14_y as numeric) / ( power(2, 14 - target_z_level))) as int);
	
	target_tile_id := (target_tile_x << 14) | target_tile_y;
	target_tile_id := (target_z_level << 28) | target_tile_id;
	
	return target_tile_id;
END;
$$;

create or replace function get_new_tile_id_in_china_level6(base14_tile_id int, target_z_level int)
	returns int
	LANGUAGE plpgsql
AS $$ 
DECLARE
	tile14_x int;
	tile14_y int;
	target_tile_x int;
	target_tile_y int;
	target_tile_id int;
BEGIN
	tile14_y := base14_tile_id & 16383;
	
	if tile14_y < 6400 then
		target_tile_id := 1;
	elseif tile14_y < 6912 then
		target_tile_id := 2;
	else
		target_tile_id := 3;
	end if;
	
	target_tile_id := (target_z_level << 28) | target_tile_id;
	
	return target_tile_id;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_get_average_attr(attr_array smallint[])
  RETURNS smallint 
  LANGUAGE plpgsql
  AS $$ 
DECLARE
	avg_value	smallint;
BEGIN
	select cast(attr_value as smallint)
	from
	(
		select avg(attr_value) as attr_value
		from
		(
			select unnest(attr_array) as attr_value
		)as a
		where attr_value is not null
	)as b
	into avg_value;
	return avg_value;
END;
$$;


CREATE OR REPLACE FUNCTION rdb_make_arrowinfo_from_link_ipc(inlinkid bigint,nodeid bigint,passlid varchar,outlinkid bigint)
  RETURNS smallint 
  LANGUAGE plpgsql
  AS $$ 
DECLARE
	curs refcursor;
	rec  record;
	rec2 record;
	s_link bigint;
	e_link bigint;
	s_angle float;
	e_angle float;
	angle float;
	array_midlink bigint[];
	array_len smallint;
	i smallint;
	tmp_nodeid bigint;
	new_nodeid bigint;
	num int;
	p0 geometry;
	p1 geometry;
	p2 geometry;
	
BEGIN

	s_link := inlinkid;
	e_link := outlinkid;
	if passlid = '' or passlid is null then
		new_nodeid := nodeid;
	else
		array_midlink := cast(regexp_split_to_array(passlid, E'\\|+') as bigint[]);
		array_len := array_upper(array_midlink,1);

		open curs for execute 
			'select a.s_node as a_s_node,a.e_node as a_e_node,b.s_node as b_s_node,b.e_node as b_e_node from ( 
				select 1 as id,s_node,e_node from link_tbl where link_id = ' ||  @array_midlink[array_len] || ' ) as a 
			left join (
				select 1 as id,s_node,e_node from link_tbl where link_id = ' ||  @outlinkid || ' ) as b
			on a.id = b.id' ;
				
		fetch curs into rec2;

		close curs;

		if rec2.a_s_node = rec2.b_s_node or rec2.a_s_node = rec2.b_e_node then
			new_nodeid := rec2.a_s_node;			
		elseif rec2.a_e_node = rec2.b_s_node or rec2.a_e_node = rec2.b_e_node  then
			new_nodeid := rec2.a_e_node;

		end if;
			
	end if;

	
	for rec in  
		select link_id,s_node,e_node,the_geom from link_tbl where link_id = s_link or link_id = e_link

	loop

		if rec.link_id = s_link then
		
			if rec.s_node = nodeid then
				s_angle := rdb_cal_zm_lane(rec.the_geom, 1);

				p1 := st_startpoint(rec.the_geom);
				p0 := st_pointn(rec.the_geom,2);				
				
			else 
				if rec.e_node = nodeid then
					s_angle := rdb_cal_zm_lane(rec.the_geom, -1);
					
					num := st_npoints(rec.the_geom);
					p1 := st_endpoint(rec.the_geom);
					p0 := st_pointn(rec.the_geom,num - 1);
					
				end if;
			end if;
		else

			if rec.s_node = new_nodeid then
				e_angle := rdb_cal_zm_lane(rec.the_geom, 1);

				num := st_npoints(rec.the_geom);
				if nodeid = new_nodeid then 
					p2 := st_pointn(rec.the_geom,2);
				else
					p2 := st_startpoint(rec.the_geom);
				end if;
				
			else 
				if rec.e_node = new_nodeid then
					e_angle := rdb_cal_zm_lane(rec.the_geom, -1);
										
					num := st_npoints(rec.the_geom);

					if nodeid = new_nodeid then
						p2 := st_pointn(rec.the_geom,num - 1);
					else 
						p2 := st_endpoint(rec.the_geom);
					end if;
					
				end if;
			end if;
		end if;

	end loop;

	angle := s_angle - e_angle + 180;
	if angle >= 360 then
		angle := angle - 360;
	elseif angle < 0 then
		angle := angle + 360;
	end if;
  	
	return case when (angle > 332 and angle <= 360) or (angle >= 0 and angle < 25) then  1
		when angle >= 25 and angle < 51 then  2
		when angle >= 51 and angle < 133 then 4
		when angle >= 133 and angle < 157 then 8
		when angle >= 157 and angle < 200 and rdb_u_turn_dir(p0,p1,p2) = 2 then 16 
		when angle >= 157 and angle < 200 and rdb_u_turn_dir(p0,p1,p2) = 1 then 2048
		when angle >= 200 and angle < 227 then 32
		when angle >= 227 and angle < 310 then 64 
		when angle >= 310 and angle < 332 then 128 
		else   null
	end;

END;
$$;

CREATE OR REPLACE FUNCTION rdb_u_turn_dir(p0 geometry, p1 geometry, p2 geometry) 
RETURNS integer
LANGUAGE plpgsql
AS $$
DECLARE
    x2x0    float;
    y2y0    float;
    x1x0    float;
    y1y0    float;
    m    float;
BEGIN 
    x2x0    := st_x(p2) - st_x(p0);
    y2y0    := st_y(p2) - st_y(p0);
    x1x0    := st_x(p1) - st_x(p0);
    y1y0    := st_y(p1) - st_y(p0);
 
    m    := (x2x0 * y1y0 - x1x0 * y2y0);
    
    if m > 0 then
        return 1;    -- right
    elsif m < 0 then
        return 2;    -- left
    else
        return 0;    -- three point is a line
    end if;
END;
$$;


CREATE OR REPLACE FUNCTION rdb_cal_zm_lane(geom geometry, dir integer) RETURNS float
  LANGUAGE plpgsql
AS $$
DECLARE
	angle_int int;

	angle_f float;

BEGIN 
    angle_int := rdb_cal_zm(geom,dir);
    
    if angle_int > 0 then
		angle_int := angle_int + 32768;
		angle_int := angle_int - 65535;
		angle_f := angle_int * 360.0 / 65535.0;
	
    else
		angle_int := angle_int + 32768;
		angle_f := angle_int * 360.0 / 65535.0;
	
    end if;
    return angle_f;
END;
$$;



CREATE OR REPLACE FUNCTION rdb_convert_guidetype(guidetype smallint,flag smallint)
  RETURNS smallint 
  LANGUAGE plpgsql VOLATILE
  AS
$$
BEGIN 
	return case 
			when guidetype = 2 then 1
			when guidetype in (3,4) then 2
			when guidetype = 5 then 4
			when guidetype in (6,7) then 8
			when guidetype = 8 and flag = 0 then 16
			when guidetype = 8 and flag = 1 then 2048
			when guidetype in (9,10) then 32
			when guidetype = 11 then 64
			when guidetype in (12,13) then 128
			end;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_merge_link_common_attri(
							pass_side smallint,
							toll smallint,
							function_code smallint,
							road_type smallint,
							display_class smallint
							)
    RETURNS SMALLINT
    LANGUAGE plpgsql VOLATILE
AS $$
DECLARE
	rtn_value smallint;
BEGIN
    rtn_value := 0;
    rtn_value := rtn_value | pass_side;
    rtn_value := rtn_value | (toll << 1);
    rtn_value := rtn_value | (function_code << 3);
    rtn_value := rtn_value | (road_type << 6);
    rtn_value := rtn_value | (display_class << 10);

    return rtn_value;
END;
$$;


CREATE OR REPLACE FUNCTION rdb_cnv_length(length double precision) RETURNS integer
    LANGUAGE plpgsql
    AS $$
    DECLARE
    	int_length	integer;
    BEGIN
    	int_length	:= cast(length as integer);
	    RETURN CASE
	        WHEN int_length = 0 THEN 1
	        ELSE int_length
	    END;
    END;
$$;

create or replace function rdb_calc_lane_dir(one_way smallint)
returns int
LANGUAGE plpgsql
AS $$
BEGIN
RETURN CASE
          WHEN one_way = 1 THEN 3
          WHEN one_way = 2 THEN 1
          WHEN one_way = 3 THEN 2
          WHEN one_way = 0 THEN 3
          ELSE 1
      END;
end;
$$;

CREATE OR REPLACE FUNCTION rdb_calc_length_by_unit(length integer)
  RETURNS integer[] 
 LANGUAGE plpgsql volatile
AS $$
DECLARE
unit int;
value int;
BEGIN
	if length <= 2097151 then
		unit := 0;
		value := length;
	elsif length <= 8388604 then
		unit := 1;
		value := length / 4;
	elsif length <= 33554416 then
		unit := 2;
		value := length /16;
	elsif length <= 134217664 then
		unit := 3;
		value := length /64;
	elsif length <= 536870656 then
		unit := 4;
		value := length /256;
	elsif length <= 2147482624 then
		unit := 5;
		value := length/1024;
	elsif length <= 8589930496 then
		unit := 6;
		value := length /4096;
	else 
		unit := 7;
		value := 0;
	end if;

	return array[unit, value];
END;
$$;


CREATE OR REPLACE FUNCTION rdb_get_specify_link_idx(find_linkid bigint, link_ids bigint[])
  RETURNS integer 
LANGUAGE plpgsql volatile
AS $$
DECLARE
itor int;
branches_cnt int;
BEGIN
	branches_cnt := array_upper(link_ids, 1);
	itor := 0;
	while itor < branches_cnt loop
		if find_linkid = link_ids[itor+1] then
			exit;
		end if;
		itor := itor + 1;
	end loop;

	if itor = branches_cnt then
		itor := -1;
	end if;

	return itor;
END;
$$; 


--------------------------------------------------------------------------------
-- in : angle1,angle2 in [0,360]
-- out: 0: not parallel, 1: same angle, 2: opposite angle
--------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION rdb_detect_parallel_angle(angle1 float, angle2 float)
	RETURNS integer 
	LANGUAGE plpgsql volatile
AS $$
DECLARE
	angle float;
BEGIN
	angle = abs(angle1 - angle2);
	
	if (angle <= 15 or angle >= 345) then
		return 1;
	elseif (angle <= 195 and angle >= 165) then
		return 2;
	else
		return 0;
	end if;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_is_uturn_link(target_link_id bigint)
	RETURNS boolean 
	LANGUAGE plpgsql volatile
AS $$
DECLARE
	uturn_link_array			bigint[];
	inner_linktypes				smallint[];
	rec							record;
	search_node					bigint;
	s_conn_link_angle_array		float[];
	e_conn_link_angle_array		float[];
BEGIN
	-- init
	inner_linktypes		:= ARRAY[4,8,13];
	uturn_link_array	:= ARRAY[target_link_id];
	
	-- search from start_node
	select start_node_id from rdb_link where link_id = target_link_id into search_node;
	while true loop
		select 	array_agg(nextlink) as nextlink_array,
				array_agg(nextnode) as nextnode_array,
				array_agg(angle) as angle_array,
				array_agg(link_type) as linktype_array,
				array_agg(uturn) as uturn_array,
				count(*) as conn_link_num
		from
		(
			select	nextlink, nextnode, link_type, uturn, ((angle + 32768) * 360.0 / 65536.0) as angle
			from
			(
				select 	link_id as nextlink, 
						(case when start_node_id = search_node then end_node_id else start_node_id end) as nextnode,
						(case when start_node_id = search_node then fazm_path else tazm_path end) as angle,
						link_type, uturn
				from temp_region_links
				where not (link_id = ANY(uturn_link_array)) and search_node in (start_node_id, end_node_id)
			)as t
			order by angle
		)as t
		into rec;
		
		if rec.conn_link_num = 1 and (rec.linktype_array[1] = ANY(inner_linktypes) or rec.uturn_array[1] = 1) then 
			search_node				:= rec.nextnode_array[1];
			uturn_link_array		:= array_append(uturn_link_array, rec.nextlink_array[1]);
		elseif rec.conn_link_num = 2 then
			s_conn_link_angle_array	:= rec.angle_array;
			exit;
		else
			return false;
		end if;
	end loop;
	
	-- search from end_node
	select end_node_id from rdb_link where link_id = target_link_id into search_node;
	while true loop
		select 	array_agg(nextlink) as nextlink_array,
				array_agg(nextnode) as nextnode_array,
				array_agg(angle) as angle_array,
				array_agg(link_type) as linktype_array,
				array_agg(uturn) as uturn_array,
				count(*) as conn_link_num
		from
		(
			select	nextlink, nextnode, link_type, uturn, ((angle + 32768) * 360.0 / 65536.0) as angle
			from
			(
				select 	link_id as nextlink, 
						(case when start_node_id = search_node then end_node_id else start_node_id end) as nextnode,
						(case when start_node_id = search_node then fazm_path else tazm_path end) as angle,
						link_type, uturn
				from temp_region_links
				where not (link_id = ANY(uturn_link_array)) and search_node in (start_node_id, end_node_id)
			)as t
			order by angle
		)as t
		into rec;
		
		if rec.conn_link_num = 1 and (rec.linktype_array[1] = ANY(inner_linktypes) or rec.uturn_array[1] = 1) then 
			search_node				:= rec.nextnode_array[1];
			uturn_link_array		:= array_append(uturn_link_array, rec.nextlink_array[1]);
		elseif rec.conn_link_num = 2 then
			e_conn_link_angle_array	:= rec.angle_array;
			exit;
		else
			return false;
		end if;
	end loop;
	
	-- judge
	return 
	(
		(rdb_detect_parallel_angle(s_conn_link_angle_array[1], s_conn_link_angle_array[2]) = 2)
		and
		(rdb_detect_parallel_angle(e_conn_link_angle_array[1], e_conn_link_angle_array[2]) = 2)
		and
		(
			(
				(rdb_detect_parallel_angle(s_conn_link_angle_array[1], e_conn_link_angle_array[1]) = 1)
		        and
				(rdb_detect_parallel_angle(s_conn_link_angle_array[2], e_conn_link_angle_array[2]) = 1)
			)
			or
			(
				(rdb_detect_parallel_angle(s_conn_link_angle_array[1], e_conn_link_angle_array[2]) = 1)
		        and
				(rdb_detect_parallel_angle(s_conn_link_angle_array[2], e_conn_link_angle_array[1]) = 1)
			)
		)
	);
END;
$$;

CREATE OR REPLACE FUNCTION rdb_is_uturn_link_for_thailand(target_link_id bigint, target_link_length integer)
	RETURNS boolean 
	LANGUAGE plpgsql volatile
AS $$
DECLARE
	uturn_link_array			bigint[];
	inner_linktypes				smallint[];
	sum_link_length				integer;
	rec							record;
	search_node					bigint;
	s_conn_link_angle_array		float[];
	e_conn_link_angle_array		float[];
BEGIN
	-- init
	inner_linktypes		:= ARRAY[4,8,13];
	uturn_link_array	:= ARRAY[target_link_id];
	sum_link_length		:= target_link_length;
	
	-- search from start_node
	select start_node_id from rdb_link where link_id = target_link_id into search_node;
	while true loop
		select 	array_agg(nextlink) as nextlink_array,
				array_agg(nextnode) as nextnode_array,
				array_agg(angle) as angle_array,
				array_agg(link_type) as linktype_array,
				array_agg(uturn) as uturn_array,
				array_agg(link_length) as link_length_array,
				count(*) as conn_link_num
		from
		(
			select	nextlink, nextnode, link_type, uturn, link_length, ((angle + 32768) * 360.0 / 65536.0) as angle
			from
			(
				select 	link_id as nextlink, 
						(case when start_node_id = search_node then end_node_id else start_node_id end) as nextnode,
						(case when start_node_id = search_node then fazm_path else tazm_path end) as angle,
						link_type, uturn, link_length
				from temp_region_links
				where not (link_id = ANY(uturn_link_array)) and search_node in (start_node_id, end_node_id)
			)as t
			order by angle
		)as t
		into rec;
		
		if rec.conn_link_num = 1 and (rec.linktype_array[1] = ANY(inner_linktypes) or rec.uturn_array[1] = 1) then 
			search_node				:= rec.nextnode_array[1];
			uturn_link_array		:= array_append(uturn_link_array, rec.nextlink_array[1]);
			sum_link_length			:= sum_link_length + rec.link_length_array[1];
		elseif rec.conn_link_num = 2 then
			s_conn_link_angle_array	:= rec.angle_array;
			exit;
		else
			return false;
		end if;
	end loop;
	
	-- search from end_node
	select end_node_id from rdb_link where link_id = target_link_id into search_node;
	while true loop
		select 	array_agg(nextlink) as nextlink_array,
				array_agg(nextnode) as nextnode_array,
				array_agg(angle) as angle_array,
				array_agg(link_type) as linktype_array,
				array_agg(uturn) as uturn_array,
				array_agg(link_length) as link_length_array,
				count(*) as conn_link_num
		from
		(
			select	nextlink, nextnode, link_type, uturn, link_length, ((angle + 32768) * 360.0 / 65536.0) as angle
			from
			(
				select 	link_id as nextlink, 
						(case when start_node_id = search_node then end_node_id else start_node_id end) as nextnode,
						(case when start_node_id = search_node then fazm_path else tazm_path end) as angle,
						link_type, uturn, link_length
				from temp_region_links
				where not (link_id = ANY(uturn_link_array)) and search_node in (start_node_id, end_node_id)
			)as t
			order by angle
		)as t
		into rec;
		
		if rec.conn_link_num = 1 and (rec.linktype_array[1] = ANY(inner_linktypes) or rec.uturn_array[1] = 1) then 
			search_node				:= rec.nextnode_array[1];
			uturn_link_array		:= array_append(uturn_link_array, rec.nextlink_array[1]);
			sum_link_length			:= sum_link_length + rec.link_length_array[1];
		elseif rec.conn_link_num = 2 then
			e_conn_link_angle_array	:= rec.angle_array;
			exit;
		else
			return false;
		end if;
	end loop;
	
	-- judge
	return 
	(sum_link_length <= 100)
	and
	(
		(rdb_detect_parallel_angle(s_conn_link_angle_array[1], s_conn_link_angle_array[2]) = 2)
		and
		(rdb_detect_parallel_angle(e_conn_link_angle_array[1], e_conn_link_angle_array[2]) = 2)
		and
		(
			(
				(rdb_detect_parallel_angle(s_conn_link_angle_array[1], e_conn_link_angle_array[1]) = 1)
		        and
				(rdb_detect_parallel_angle(s_conn_link_angle_array[2], e_conn_link_angle_array[2]) = 1)
			)
			or
			(
				(rdb_detect_parallel_angle(s_conn_link_angle_array[1], e_conn_link_angle_array[2]) = 1)
		        and
				(rdb_detect_parallel_angle(s_conn_link_angle_array[2], e_conn_link_angle_array[1]) = 1)
			)
		)
	);
END;
$$;


CREATE OR REPLACE FUNCTION rdb_promote_link_fc_by_same_name(new_fc integer)
	RETURNS integer 
	LANGUAGE plpgsql volatile
AS $$
DECLARE
	root_rec			record;
	search_rec			record;
	root_nodes			bigint[];
	nIndex				integer;
	nLinkCount			integer;
BEGIN
	for root_rec in 
		select a.* 
		from rdb_link as a
		left join temp_region_promote_link_fc as b
		on a.link_id = b.link_id
		where b.link_id is null and a.function_code = new_fc and a.road_name is not null
	loop
		root_nodes	:= ARRAY[root_rec.start_node_id, root_rec.end_node_id];
		nIndex		:= 1;
		while nIndex <= array_upper(root_nodes, 1) loop
			--select count(*)
			--from rdb_link
			--where root_nodes[nIndex] in (start_node_id, end_node_id) and road_name = root_rec.road_name
			--into nLinkCount;
			--if nLinkCount = 2 then
				for search_rec in
					select a.*, a.end_node_id as next_node_id
					from rdb_link as a
					left join temp_region_promote_link_fc as b
					on a.link_id = b.link_id
					where 	b.link_id is null 
						and a.function_code > new_fc 
						and a.start_node_id = root_nodes[nIndex]
						and a.road_name = root_rec.road_name
					union
					select a.*, a.start_node_id as next_node_id
					from rdb_link as a
					left join temp_region_promote_link_fc as b
					on a.link_id = b.link_id
					where 	b.link_id is null 
						and a.function_code > new_fc 
						and a.end_node_id = root_nodes[nIndex]
						and a.road_name = root_rec.road_name
				loop
					root_nodes	:= array_append(root_nodes, search_rec.next_node_id);
					insert into temp_region_promote_link_fc(link_id, new_function_code)
								values(search_rec.link_id, new_fc);
				end loop;
			--end if;
			nIndex	:= nIndex + 1;
		end loop;
	end loop;
	return 0;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_remove_polygon_from_collection(i_geom geometry)
	RETURNS geometry 
	LANGUAGE plpgsql volatile
AS $$
DECLARE
BEGIN
	if st_isempty(i_geom) or st_geometrytype(i_geom) in ('ST_MultiLineString', 'ST_LineString') then
		return i_geom;
	else
		select st_collect(sub_geom) as new_geom
		from
		(
			select sub_geom
			from
			(
				select st_geometryn(i_geom, index) as sub_geom, index
				from
				(
					select generate_series(1,st_numgeometries(i_geom)) as index
				)as a
			)as b
			where st_geometrytype(sub_geom) in ('ST_MultiLineString', 'ST_LineString')
			order by index
		)as c
		into i_geom;
		return i_geom;
	end if;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_collection_to_multi_line(i_geom geometry)
	RETURNS geometry 
	LANGUAGE plpgsql volatile
AS $$
DECLARE
BEGIN
	if st_isempty(i_geom) then
		return i_geom;
	end if;
	
	if st_geometrytype(i_geom) = 'ST_MultiLineString' then
		select st_linemerge(i_geom) into i_geom;
	end if;
	
	if st_geometrytype(i_geom) = 'ST_LineString' then
		return st_multi(i_geom);
	end if;

	select st_multi(st_linemerge(st_union(sub_geom))) as new_geom
	from
	(
		select sub_geom
		from
		(
			select st_geometryn(i_geom, index) as sub_geom, index
			from
			(
				select generate_series(1,st_numgeometries(i_geom)) as index
			)as a
		)as b
		where st_geometrytype(sub_geom) in ('ST_MultiLineString', 'ST_LineString')
	)as c
	into i_geom;
	return i_geom;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_linemerge(multi_line_geom geometry)
	RETURNS geometry 
	LANGUAGE plpgsql volatile
AS $$
DECLARE
BEGIN
	if st_isvalid(multi_line_geom) then
		return st_linemerge(multi_line_geom);
	else
		return multi_line_geom;
	end if;
END;
$$;

CREATE OR REPLACE FUNCTION vics_get_shapepoints(linkid bigint,geom geometry,fraction float)
  RETURNS int
  LANGUAGE plpgsql
AS $$
DECLARE
	Pnum		int;
	Nnum		int;
	i		int;
	Fpoint		float;
	Npoint		int;
	delta		float[];
	len_delta 	int;
	min 		float;
	flag 		int;
BEGIN

	Nnum := ST_NumPoints(geom);
--  	raise info 'number of points : %',Nnum;

	Pnum := -1;
	min := 1;
	flag := -1;

-- 	raise info '------------linkid:%',linkid;
	
	if fraction = 0 then
	
		Pnum :=  0;
--  		raise info '  ---0---Pnum :%',Pnum;
		return Pnum;
	elseif fraction = 1 then
	
		Pnum := Nnum - 1;
-- 		raise info '  ---1---Pnum :%',Pnum;
		return Pnum;
	else

		for i in 0..(Nnum - 1) loop
			
			Fpoint := mid_get_fraction(geom, ST_pointN(geom,(i + 1)));
			
			if Fpoint = fraction then
				Pnum := i ;
--  				raise info '  ---else---Pnum :%',Pnum;
				return Pnum;
			else
				if Fpoint < mid_get_fraction(geom, ST_pointN(geom,i)) then
					Fpoint := 1 - mid_get_fraction(st_reverse(geom), ST_pointN(geom,(i + 1)));
 					raise info 'reverse!';
				end if;
				
				if min > abs(Fpoint - fraction) then
					
					min := abs(Fpoint - fraction);
					flag := i;
				end if;
				
			end if;
			
		end loop;
		
-- 		raise info '  ---delta : %',min;	
		return flag;			
	end if;
	
	
END;
$$;

CREATE OR REPLACE FUNCTION rdb_make_vics_merge_links_axf()
  RETURNS integer 
  LANGUAGE plpgsql VOLATILE
AS $$ 
DECLARE
	rec				record;
	i				integer;
	
	pre_loc_code_mesh 		integer;
	pre_loc_code			integer;
	pre_dir 			smallint;
	pre_link_id 			bigint;
	pre_s_fraction			double precision;
	pre_e_fraction			double precision;
	pre_s_point			smallint;
	pre_e_point			smallint;
	pre_type			smallint;
	pre_service_id			integer;
	pre_link_dir			smallint;
	pre_seq				smallint;
	
	cur_loc_code_mesh 		integer;
	cur_loc_code			integer;
	cur_dir 			smallint;
	cur_link_id 			bigint;	
	cur_s_fraction			double precision;
	cur_e_fraction			double precision;
	cur_s_point			smallint;
	cur_e_point			smallint;
	cur_type			smallint;
	cur_service_id			integer;
	cur_link_dir			smallint;
	cur_seq				smallint;
			
BEGIN
	i := 1;

	for rec in select * from temp_vics_org2rdb_seq_axf 
		order by loc_code_mesh,loc_code,dir,target_link_id,seq
	LOOP
		if i = 1 then 
			pre_loc_code_mesh := rec.loc_code_mesh;
			pre_loc_code := rec.loc_code;
			pre_dir := rec.dir;
			pre_link_id := rec.target_link_id;
			pre_type := rec.type;
			pre_service_id := rec.service_id;
			pre_link_dir := rec.link_dir;
			pre_seq := rec.seq;
			pre_s_fraction := rec.s_fraction;
			pre_e_fraction := rec.e_fraction;
			pre_s_point := rec.s_point;
			pre_e_point := rec.e_point;
		else
			cur_loc_code_mesh := rec.loc_code_mesh;
			cur_loc_code := rec.loc_code;
			cur_dir := rec.dir;
			cur_link_id := rec.target_link_id;
			cur_type := rec.type;
			cur_service_id := rec.service_id;
			cur_link_dir := rec.link_dir;
			cur_seq := rec.seq;
			cur_s_fraction := rec.s_fraction;
			cur_e_fraction := rec.e_fraction;
			cur_s_point := rec.s_point;
			cur_e_point := rec.e_point;
			
			if cur_loc_code_mesh = pre_loc_code_mesh and cur_loc_code = pre_loc_code and cur_dir = pre_dir
				and cur_link_id = pre_link_id  then
				
				if cur_s_fraction = pre_e_fraction then 
					cur_s_fraction = pre_s_fraction;
					cur_s_point = pre_s_point;
					
					pre_service_id := cur_service_id;
					pre_link_dir := cur_link_dir;
					pre_seq := cur_seq;				
					pre_s_fraction := cur_s_fraction;
					pre_e_fraction := cur_e_fraction;
					pre_s_point := cur_s_point;
					pre_e_point := cur_e_point;
					
				elseif cur_e_fraction = pre_s_fraction then
					cur_e_fraction = pre_e_fraction;
					cur_e_point = pre_e_point;
					
					pre_service_id := cur_service_id;
					pre_link_dir := cur_link_dir;
					pre_seq := cur_seq;				
					pre_s_fraction := cur_s_fraction;
					pre_e_fraction := cur_e_fraction;
					pre_s_point := cur_s_point;
					pre_e_point := cur_e_point;					
				else					
					insert into rdb_vics_org2rdb_axf(loc_code,dir,"type",loc_code_mesh,service_id,
						s_fraction,e_fraction,s_point,e_point,target_link_id,link_dir) 
					values(rec.loc_code,rec.dir,rec.type,rec.loc_code_mesh,pre_service_id,
						pre_s_fraction,pre_e_fraction,pre_s_point,pre_e_point,rec.target_link_id,pre_link_dir);

					pre_service_id := cur_service_id;
					pre_link_dir := cur_link_dir;
					pre_seq := cur_seq;				
					pre_s_fraction := cur_s_fraction;
					pre_e_fraction := cur_e_fraction;
					pre_s_point := cur_s_point;
					pre_e_point := cur_e_point;
				end if;
				
			else
				insert into rdb_vics_org2rdb_axf(loc_code,dir,"type",loc_code_mesh,service_id,
					s_fraction,e_fraction,s_point,e_point,target_link_id,link_dir) 
				values(pre_loc_code,pre_dir,pre_type,pre_loc_code_mesh,pre_service_id,
					pre_s_fraction,pre_e_fraction,pre_s_point,pre_e_point,pre_link_id,pre_link_dir);			
				
				pre_loc_code_mesh := cur_loc_code_mesh;
				pre_loc_code := cur_loc_code;
				pre_dir := cur_dir;
				pre_link_id := cur_link_id;
				pre_type := cur_type;
				pre_service_id := cur_service_id;
				pre_link_dir := cur_link_dir;	
				pre_seq := cur_seq;			
				pre_s_fraction := cur_s_fraction;
				pre_e_fraction := cur_e_fraction;
				pre_s_point := cur_s_point;
				pre_e_point := cur_e_point;
			end if;
			
		end if;

		i := i + 1;
	END LOOP;

	insert into rdb_vics_org2rdb_axf(loc_code,dir,"type",loc_code_mesh,service_id,
		s_fraction,e_fraction,s_point,e_point,target_link_id,link_dir) 
	values(pre_loc_code,pre_dir,pre_type,pre_loc_code_mesh,pre_service_id,
		pre_s_fraction,pre_e_fraction,pre_s_point,pre_e_point,pre_link_id,pre_link_dir);
			
	return 1;	
END;
$$;

CREATE OR REPLACE FUNCTION rdb_make_vics_merge_links_jpn(temp_table character varying,target_table character varying)
  RETURNS integer 
  LANGUAGE plpgsql VOLATILE
AS $$ 
DECLARE
	rec					record;
	i					integer;
	curs				refcursor;
	
	pre_meshcode 		integer;
	pre_vicsid			integer;
	pre_vicsclass 		smallint;
	pre_link_id 		bigint;
	pre_s_fraction		double precision;
	pre_e_fraction		double precision;
	pre_s_point			smallint;
	pre_e_point			smallint;
	pre_linkdir			smallint;
	pre_seq				smallint;
	
	cur_meshcode 		integer;
	cur_vicsid			integer;
	cur_vicsclass 		smallint;
	cur_link_id 		bigint;	
	cur_s_fraction		double precision;
	cur_e_fraction		double precision;
	cur_s_point			smallint;
	cur_e_point			smallint;
	cur_linkdir			smallint;
	cur_seq				smallint;
			
BEGIN
	i := 1;
	open curs for execute 
		'select * from ' || quote_ident(temp_table) || ' order by meshcode,vicsclass,vicsid,target_link_id,seq';
	fetch curs into rec;
	
	WHILE rec.target_link_id IS NOT NULL
	LOOP
		if i = 1 then 
			pre_meshcode := rec.meshcode;
			pre_vicsid := rec.vicsid;
			pre_vicsclass := rec.vicsclass;
			pre_link_id := rec.target_link_id;
			pre_linkdir := rec.linkdir;
			pre_seq := rec.seq;
			pre_s_fraction := rec.s_fraction;
			pre_e_fraction := rec.e_fraction;
			pre_s_point := rec.s_point;
			pre_e_point := rec.e_point;
		else
			cur_meshcode := rec.meshcode;
			cur_vicsid := rec.vicsid;
			cur_vicsclass := rec.vicsclass;
			cur_link_id := rec.target_link_id;
			cur_linkdir := rec.linkdir;
			cur_seq := rec.seq;
			cur_s_fraction := rec.s_fraction;
			cur_e_fraction := rec.e_fraction;
			cur_s_point := rec.s_point;
			cur_e_point := rec.e_point;
			
			if cur_meshcode = pre_meshcode and cur_vicsid = pre_vicsid and cur_vicsclass = pre_vicsclass
				and cur_link_id = pre_link_id  then
				
				if cur_s_fraction = pre_e_fraction then 
-- 					raise info 'connect link---- pre_link_id:%,cur_link_id:%,pre_e_fraction:%,cur_s_fraction:%',pre_link_id,cur_link_id,pre_e_fraction,cur_s_fraction;				
					cur_s_fraction = pre_s_fraction;
					cur_s_point = pre_s_point;
					
					pre_linkdir := cur_linkdir;
					pre_seq := cur_seq;				
					pre_s_fraction := cur_s_fraction;
					pre_e_fraction := cur_e_fraction;
					pre_s_point := cur_s_point;
					pre_e_point := cur_e_point;

-- 					raise info '                 ---- pre_link_id:%,cur_link_id:%,pre_s_fraction:%,pre_e_fraction:%',pre_link_id,cur_link_id,pre_s_fraction,pre_e_fraction;
					
				elseif cur_e_fraction = pre_s_fraction then
-- 					raise info 'connect link----pre_link_id:%,cur_link_id:%,cur_e_fraction:%,pre_s_fraction:%',pre_link_id,cur_link_id,cur_e_fraction,pre_s_fraction;	
					cur_e_fraction = pre_e_fraction;
					cur_e_point = pre_e_point;
					
					pre_linkdir := cur_linkdir;
					pre_seq := cur_seq;				
					pre_s_fraction := cur_s_fraction;
					pre_e_fraction := cur_e_fraction;
					pre_s_point := cur_s_point;
					pre_e_point := cur_e_point;					
				else					
-- 					raise info 'last part of link----pre_link_id:%,cur_link_id:%',pre_link_id,cur_link_id;
					execute 'insert into ' || quote_ident(target_table) || '(meshcode,vicsclass,vicsid,linkdir,target_link_id,s_fraction,e_fraction,s_point,e_point) 	
					values('||rec.meshcode||','||rec.vicsclass||','||rec.vicsid||','||pre_linkdir||','||rec.target_link_id||','||pre_s_fraction||','||pre_e_fraction||','||pre_s_point||','||pre_e_point||')';
					pre_linkdir := cur_linkdir;
					pre_seq := cur_seq;				
					pre_s_fraction := cur_s_fraction;
					pre_e_fraction := cur_e_fraction;
					pre_s_point := cur_s_point;
					pre_e_point := cur_e_point;
				end if;
				
			else
-- 				raise info 'new link false----pre_link_id:%,cur_link_id:%,pre_s_fraction:%,pre_e_fraction:%',pre_link_id,cur_link_id,pre_s_fraction,pre_e_fraction;		
				execute 'insert into ' || quote_ident(target_table) || '(meshcode,vicsclass,vicsid,linkdir,target_link_id,s_fraction,e_fraction,s_point,e_point) 	
				values('||pre_meshcode||','||pre_vicsclass||','||pre_vicsid||','||pre_linkdir||','||pre_link_id||','||pre_s_fraction||','||pre_e_fraction||','||pre_s_point||','||pre_e_point||')';				
				pre_meshcode := cur_meshcode;
				pre_vicsid := cur_vicsid;
				pre_vicsclass := cur_vicsclass;
				pre_link_id := cur_link_id;
				pre_linkdir := cur_linkdir;	
				pre_seq := cur_seq;			
				pre_s_fraction := cur_s_fraction;
				pre_e_fraction := cur_e_fraction;
				pre_s_point := cur_s_point;
				pre_e_point := cur_e_point;
			end if;
			
		end if;

		i := i + 1;
		fetch curs into rec;
	END LOOP;

	execute 'insert into ' || quote_ident(target_table) || '(meshcode,vicsclass,vicsid,linkdir,target_link_id,s_fraction,e_fraction,s_point,e_point) 	
	values('||pre_meshcode||','||pre_vicsclass||','||pre_vicsid||','||pre_linkdir||','||pre_link_id||','||pre_s_fraction||','||pre_e_fraction||','||pre_s_point||','||pre_e_point||')';	

	close curs;
	return 1;	
END;
$$;

CREATE OR REPLACE FUNCTION rdb_make_vics_link_seq()
  RETURNS integer 
  LANGUAGE plpgsql VOLATILE
AS $$ 
DECLARE
	rec				temp_vics_org2rdb_with_node;
	rec2				record;
	cur1				refcursor;
	rec3				record;
	tmp_rec                         record;
	pos_rec				record;
	neg_rec				record;
	start_node			bigint;
	end_node			bigint;
	link_array			bigint[];
	linkdir_array			integer[];
	class3_array			smallint[];
	class4_array			character varying[];
	nCount				integer;
	nIndex				integer;
	pre_class0 int;
	pre_class1 int;
	pre_class2 int;
	pre_class3 int;
	i int;
	new_group_id int;
BEGIN
	i = 0;
	new_group_id = 0;
	for rec in select * from temp_vics_org2rdb_with_node 
		order by class0, class1, class2, class3 desc, first_flag
	LOOP
		
		i = i + 1;
		if i = 1 then
			pre_class0 = rec.class0;
			pre_class1 = rec.class1;
			pre_class2 = rec.class2;
			pre_class3 = rec.class3;
		end if;	

		select link_id from temp_vics_link_walked where class0 = rec.class0 
			and class1 = rec.class1 and class2 = rec.class2
			and class3 = rec.class3 and link_id = rec.link_id into tmp_rec;
		
		if found then
			continue;
		else 
			insert into temp_vics_link_walked ("class0","class1","class2","class3","class4","link_id") values (rec.class0,rec.class1,rec.class2,rec.class3,rec.class4,rec.link_id);
		end if;	

		if rec.linkdir = 0 then 
			pos_rec			:= rdb_make_vics_link_seq_in_one_direction(rec, rec.e_node, 1);
			neg_rec			:= rdb_make_vics_link_seq_in_one_direction(rec, rec.s_node, 2);
		else
			pos_rec			:= rdb_make_vics_link_seq_in_one_direction(rec, rec.s_node, 1);
			neg_rec			:= rdb_make_vics_link_seq_in_one_direction(rec, rec.e_node, 2);
		end if;

		--- initialize link sequence.
		link_array		:= ARRAY[rec.link_id];
		linkdir_array		:= ARRAY[rec.linkdir];
		class3_array		:= ARRAY[rec.class3];
		class4_array		:= ARRAY[rec.class4];
		
		nCount			:= array_upper(neg_rec.linkrow, 1);
		if nCount is not null then
			for nIndex in 1..nCount loop
				link_array	:= array_prepend(neg_rec.linkrow[nIndex], link_array);
				linkdir_array	:= array_prepend(neg_rec.linkdirrow[nIndex], linkdir_array);
				class3_array	:= array_prepend(neg_rec.class3row[nIndex], class3_array);
				class4_array	:= array_prepend(neg_rec.class4row[nIndex], class4_array);
			end loop;
		end if;
		
		nCount			:= array_upper(pos_rec.linkrow, 1);
		if nCount is not null then
			for nIndex in 1..nCount loop
				link_array	:= array_append(link_array, pos_rec.linkrow[nIndex]);
				linkdir_array	:= array_append(linkdir_array, pos_rec.linkdirrow[nIndex]);
				class3_array	:= array_append(class3_array,pos_rec.class3row[nIndex]);
				class4_array	:= array_append(class4_array,pos_rec.class4row[nIndex]);
			end loop;
		end if;

		if i <> 1 and rec.class0 = pre_class0 and rec.class1 = pre_class1 
			and rec.class2 = pre_class2 and rec.class3 = pre_class3 
		then
			new_group_id = new_group_id + 1;
		else
			new_group_id = 0;
		end if;
		
		nCount			:= array_upper(link_array, 1);		
		nIndex := 1;
	 
		while nIndex  <= nCount loop				
			insert into temp_vics_link_seq (link_id, seq, class0, class1, class2, class3, class4, linkdir, group_id)
				values (link_array[nIndex], nIndex, rec.class0, rec.class1, rec.class2, class3_array[nIndex], class4_array[nIndex], linkdir_array[nIndex], new_group_id);
			nIndex := nIndex + 1;	
		end loop;
		
		pre_class0 = rec.class0;
		pre_class1 = rec.class1;
		pre_class2 = rec.class2;
		pre_class3 = rec.class3;	
	END LOOP;

	return 1;	
END;
$$;

CREATE OR REPLACE FUNCTION rdb_make_vics_link_seq_in_one_direction(
		rec temp_vics_org2rdb_with_node, search_node bigint, search_dir int, OUT linkrow bigint[], OUT linkdirrow int[], OUT class3row smallint[], OUT class4row character varying[], OUT reach_node bigint)
	RETURNS record
	LANGUAGE plpgsql volatile
AS $$
DECLARE
	rec2				record;
	rec3				record;
	
	reach_link                      bigint;
BEGIN
	-- init
	reach_link	:= rec.link_id;
	reach_node	:= search_node;
	-- search
	while true loop
		
		select * from (
			select	a.*
			from
			(
				select	link_id
					, case when reach_node = s_node then e_node
						when reach_node = e_node then s_node
					  end as nextnode
					, class0, class1, class2, class3, class4, linkdir
				from temp_vics_org2rdb_with_node
				where 	(
						
						(search_dir = 1 and linkdir = 0 and reach_node = s_node)
						or (search_dir = 1 and linkdir = 1 and reach_node = e_node)
						or (search_dir = 2 and linkdir = 1 and reach_node = s_node)
						or (search_dir = 2 and linkdir = 0 and reach_node = e_node)
					)
					and 
					(
						(link_id != reach_link)
						and (class0 = rec.class0)
						and (class1 = rec.class1)
						and (class2 = rec.class2)
						and (class3 = rec.class3)
					)
			)as a
			left join temp_vics_link_walked as b
			on a.class0 = b.class0 
			and a.class1 = b.class1 and a.class2 = b.class2
			and a.class3 = b.class3 and a.link_id = b.link_id
			where b.link_id is null 
		) a order by link_id limit 1
		into rec2;
		
		if FOUND and (linkrow is null or not (rec2.link_id = any(linkrow))) then
			insert into temp_vics_link_walked ("class0","class1","class2","class3","class4","linkdir","link_id") values (rec2.class0,rec2.class1,rec2.class2,rec2.class3,rec2.class4,rec2.linkdir,rec2.link_id);	    
			
			linkrow		:= array_append(linkrow, rec2.link_id);
			linkdirrow	:= array_append(linkdirrow, rec2.linkdir);
			class3row	:= array_append(class3row, rec2.class3);
			class4row	:= array_append(class4row, rec2.class4);
			reach_link	:= rec2.link_id;
			reach_node	:= rec2.nextnode;
		else
			EXIT;
		end if;
	end loop;
END;
$$;


CREATE OR REPLACE FUNCTION rdb_make_trf_link_seq()
  RETURNS integer 
  LANGUAGE plpgsql VOLATILE
AS $$ 
DECLARE
	rec				temp_rdf_link_tmc_node;
	rec2				record;
	tmp_rec                         record;
	pos_rec				record;
	neg_rec				record;
	start_node			bigint;
	end_node			bigint;
	link_array			bigint[];
	linkdir_array			char[];
	type_array			integer[];
	nCount				integer;
	nIndex				integer;
	pre_country_code char;
	pre_table_no   int;
	pre_location_code int;
	pre_dir smallint;
	pre_type smallint;	
	i int;
	new_group_id int;
BEGIN
	i = 0;
	new_group_id = 0;
	for rec in select * from temp_rdf_link_tmc_node 
		order by ebu_country_code, location_table_nr, location_code, dir, type desc, link_id
	LOOP
		i = i + 1;
		if i = 1 then
			pre_country_code = rec.ebu_country_code;
			pre_table_no = rec.location_table_nr;
			pre_location_code = rec.location_code;
			pre_dir = rec.dir;
			pre_type = rec.type;
		end if;	
				
		select link_id from temp_rdf_link_tmc_walked where ebu_country_code = rec.ebu_country_code 
			and location_table_nr = rec.location_table_nr and location_code = rec.location_code
			and type = rec.type and dir = rec.dir and link_id = rec.link_id into tmp_rec;
		
		if found then
			continue;
		else 
			insert into temp_rdf_link_tmc_walked ("ebu_country_code","location_table_nr","location_code","dir","type","link_id") values (rec.ebu_country_code,rec.location_table_nr,rec.location_code,rec.dir, rec.type, rec.link_id);
		end if;	
		
		--- search in positive direction.
		pos_rec			:= rdb_make_trf_link_seq_in_one_direction(rec, rec.nonref_node_id, 1);
		end_node		:= pos_rec.reach_node;
		
		--- search in negative direction.
		neg_rec			:= rdb_make_trf_link_seq_in_one_direction(rec, rec.ref_node_id, 2);
		start_node		:= neg_rec.reach_node;
		
		--- initialize link sequence.
		link_array		:= ARRAY[rec.link_id];
		linkdir_array		:= ARRAY[rec.road_direction];
		type_array		:= ARRAY[rec.type];
		
		if rec.road_direction = 'F' then 
			nCount			:= array_upper(neg_rec.linkrow, 1);
			if nCount is not null then
				for nIndex in 1..nCount loop
					link_array	:= array_prepend(neg_rec.linkrow[nIndex], link_array);
					linkdir_array	:= array_prepend(neg_rec.linkdirrow[nIndex], linkdir_array);
					type_array	:= array_prepend(neg_rec.typerow[nIndex], type_array);
				end loop;
			end if;
			
			nCount			:= array_upper(pos_rec.linkrow, 1);
			if nCount is not null then
				for nIndex in 1..nCount loop
					link_array	:= array_append(link_array, pos_rec.linkrow[nIndex]);
					linkdir_array	:= array_append(linkdir_array, pos_rec.linkdirrow[nIndex]);
					type_array	:= array_append(type_array,pos_rec.typerow[nIndex]);
				end loop;
			end if;
		else
			nCount			:= array_upper(pos_rec.linkrow, 1);
			if nCount is not null then
				for nIndex in 1..nCount loop
					link_array	:= array_prepend(pos_rec.linkrow[nIndex], link_array);
					linkdir_array	:= array_prepend(pos_rec.linkdirrow[nIndex], linkdir_array);
					type_array	:= array_prepend(pos_rec.typerow[nIndex], type_array);
				end loop;
			end if;
			
			nCount			:= array_upper(neg_rec.linkrow, 1);
			if nCount is not null then
				for nIndex in 1..nCount loop
					link_array	:= array_append(link_array, neg_rec.linkrow[nIndex]);
					linkdir_array	:= array_append(linkdir_array, neg_rec.linkdirrow[nIndex]);
					type_array	:= array_append(type_array,neg_rec.typerow[nIndex]);
				end loop;
			end if;
		end if;

		--- if current record is not the first, and current location code is same with previous one,
		--- give it a new group id. cause they should be continous, but actually not.
		--- if current record is the first or current location code is different with previous one,
		--- give it a group id 0. cause it's a new start.
		if i <> 1 and rec.ebu_country_code = pre_country_code and rec.location_table_nr = pre_table_no 
			and rec.location_code = pre_location_code and rec.dir = pre_dir 
		then
			new_group_id = new_group_id + 1;
		else 
			new_group_id = 0;
		end if;
		
-- 		raise info '   --link_array:%',link_array;
		nCount			:= array_upper(link_array, 1);		
		nIndex := 1;
			
		while nIndex  <= nCount loop				
			insert into temp_rdf_link_tmc_seq (link_id, seq, ebu_country_code, location_table_nr, location_code, dir, type, road_direction, group_id)
				values (link_array[nIndex], nIndex, rec.ebu_country_code, rec.location_table_nr, rec.location_code, rec.dir, type_array[nIndex], linkdir_array[nIndex], new_group_id);
			nIndex := nIndex + 1;	
		end loop;
		
		pre_country_code = rec.ebu_country_code;
		pre_table_no = rec.location_table_nr;
		pre_location_code = rec.location_code;
		pre_dir = rec.dir;	
		pre_type = rec.type;
		
	END LOOP;

	return 1;
	
END;
$$;


CREATE OR REPLACE FUNCTION rdb_make_trf_link_seq_in_one_direction(
		rec temp_rdf_link_tmc_node, search_node bigint, search_dir integer, OUT linkrow bigint[], OUT linkdirrow char[], OUT typerow int[], OUT reach_node bigint)
	RETURNS record
	LANGUAGE plpgsql volatile
AS $$
DECLARE
	rec2				record;
	rec3				record;
	reach_link                      bigint;
BEGIN
	-- init
	reach_link	:= rec.link_id;
	reach_node	:= search_node;
	
	-- search
	while true loop
		
		-- search link
		if rec.road_direction = 'F' then
			select * from (
				select	a.*
				from
				(
					(
						select	link_id, nonref_node_id as nextnode, ebu_country_code, 
							location_table_nr, location_code, dir, type, road_direction
						from temp_rdf_link_tmc_node
						where 	(
								(search_dir = 1 and road_direction = 'F')
								or 
								(search_dir = 2 and road_direction = 'T')
							)
							and 
							(
								(reach_node = ref_node_id) 
								and (link_id != reach_link)
								and (ebu_country_code = rec.ebu_country_code)
								and (location_table_nr = rec.location_table_nr)
								and (location_code = rec.location_code)
								and (dir = rec.dir)
							)
					)
					union
					(
						select	link_id, ref_node_id as nextnode, ebu_country_code, 
							location_table_nr, location_code, dir, type, road_direction
						from temp_rdf_link_tmc_node
						where 	(
								(search_dir = 1 and road_direction = 'T')
								or 
								(search_dir = 2 and road_direction = 'F')
							)
							and 
							(
								(reach_node = nonref_node_id) 
								and (link_id != reach_link)
								and (ebu_country_code = rec.ebu_country_code)
								and (location_table_nr = rec.location_table_nr)
								and (location_code = rec.location_code)
								and (dir = rec.dir)
							)
					)
				)as a
				left join temp_rdf_link_tmc_walked as b
				on a.ebu_country_code = b.ebu_country_code 
				and a.location_table_nr = b.location_table_nr and a.location_code = b.location_code
				and a.dir = b.dir and a.type = b.type and a.link_id = b.link_id
				where b.link_id is null
			) a order by link_id limit 1
			into rec2;
		else
			select * from (
				select	a.*
				from
				(
					(
						select	link_id, nonref_node_id as nextnode, ebu_country_code, 
							location_table_nr, location_code, dir, type, road_direction
						from temp_rdf_link_tmc_node
						where 	(
								(search_dir = 1 and road_direction = 'T')
								or 
								(search_dir = 2 and road_direction = 'F')
							)
							and 
							(
								(reach_node = ref_node_id) 
								and (link_id != reach_link)
								and (ebu_country_code = rec.ebu_country_code)
								and (location_table_nr = rec.location_table_nr)
								and (location_code = rec.location_code)
								and (dir = rec.dir)
							)
					)
					union
					(
						select	link_id, ref_node_id as nextnode, ebu_country_code, 
							location_table_nr, location_code, dir, type, road_direction
						from temp_rdf_link_tmc_node
						where 	(
								(search_dir = 1 and road_direction = 'F')
								or 
								(search_dir = 2 and road_direction = 'T')
							)
							and 
							(
								(reach_node = nonref_node_id) 
								and (link_id != reach_link)
								and (ebu_country_code = rec.ebu_country_code)
								and (location_table_nr = rec.location_table_nr)
								and (location_code = rec.location_code)
								and (dir = rec.dir)
							)
					)
				)as a
				left join temp_rdf_link_tmc_walked as b
				on a.ebu_country_code = b.ebu_country_code 
				and a.location_table_nr = b.location_table_nr and a.location_code = b.location_code
				and a.dir = b.dir and a.type = b.type and a.link_id = b.link_id
				where b.link_id is null
			) a order by link_id limit 1
			into rec2;
		end if;
		
		if FOUND then
			
			insert into temp_rdf_link_tmc_walked ("ebu_country_code","location_table_nr","location_code","dir","type","link_id") values (rec2.ebu_country_code,rec2.location_table_nr,rec2.location_code,rec2.dir, rec2.type, rec2.link_id);			
			-- walk this link
			linkrow		:= array_append(linkrow, rec2.link_id);
			linkdirrow	:= array_append(linkdirrow, rec2.road_direction);
			typerow		:= array_append(typerow, rec2.type);
			reach_link	:= rec2.link_id;
			reach_node	:= rec2.nextnode;
		else
			EXIT;
		end if;
	end loop;
END;
$$;


CREATE OR REPLACE FUNCTION rdb_makeTrfshape2pixel(tz smallint,tx int,ty int,the_geom geometry)
    RETURNS character varying
    LANGUAGE plpgsql
AS $$
DECLARE
    geom_content character varying;
    cnt_geom integer;
    i integer;
    record_xy record;
    BASELON SMALLINT;
    BASELAT SMALLINT;
    CURRENTLON SMALLINT;
    CURRENTLAT SMALLINT;

BEGIN
    BEGIN
        cnt_geom := st_npoints(the_geom);
        i := 0;
        BASELON := 0;
        BASELAT := 0;

        geom_content = '';
        WHILE i < cnt_geom LOOP
            IF (i = 0) THEN
                record_xy := lonlat2pixel_tile(tz::smallint, tx::integer, ty::integer, st_x(ST_PointN(the_geom,i+1))::float, st_y(ST_PointN(the_geom,i+1))::float, 4096::smallint);
                BASELON := record_xy.x;
                BASELAT := record_xy.y;
				
                geom_content := BASELON::character varying || '|' || BASELAT::character varying;
--                 raise info '%', geom_content;
            ELSE 
                record_xy := lonlat2pixel_tile(tz::smallint, tx::integer, ty::integer, st_x(ST_PointN(the_geom,i+1))::float, st_y(ST_PointN(the_geom,i+1))::float, 4096::smallint);
                CURRENTLON := record_xy.x;
                CURRENTLAT := record_xy.y;
		
                geom_content = geom_content || ',' || CURRENTLON::character varying || '|' || CURRENTLAT::character varying;
-- 		raise info '%', geom_content;
            END IF;
            i = i + 1;
        END LOOP;
    END;
    return geom_content;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_make_trf_merge_links(temp_table character varying,target_table character varying)
  RETURNS integer 
  LANGUAGE plpgsql VOLATILE
AS $$ 
DECLARE
	rec				record;
	i				integer;
	seq				integer;
	curs 			refcursor;	

	pre_type_flag			smallint;
	pre_area_code 			integer;
	pre_infra_id			integer;
	pre_dir 				smallint;
	pre_link_id 			bigint;
	pre_s_fraction			double precision;
	pre_e_fraction			double precision;
	pre_pos_type			smallint;
	pre_extra_flag			integer;
	pre_link_dir			smallint;
	pre_group_id			smallint;
	pre_length				integer;
	pre_geom_string			character varying;
	
	cur_type_flag 			smallint;
	cur_area_code 			integer;
	cur_infra_id			integer;
	cur_dir 				smallint;
	cur_link_id 			bigint;	
	cur_s_fraction			double precision;
	cur_e_fraction			double precision;
	cur_pos_type			smallint;
	cur_extra_flag			integer;
	cur_link_dir			smallint;
	cur_group_id			smallint;
	cur_length				integer;
	cur_geom_string			character varying;
		
BEGIN
	i := 1;
	seq := 1;

	open curs for execute 
	'select * from ' || quote_ident(temp_table) || ' order by type_flag,area_code,extra_flag,infra_id,dir,group_id,seq';
	fetch curs into rec;
		
	while rec.rdb_link_id is not null 
	LOOP	
		if i = 1 then
			pre_type_flag := rec.type_flag;
			pre_area_code := rec.area_code;
			pre_infra_id := rec.infra_id;
			pre_dir := rec.dir;
			pre_link_id := rec.rdb_link_id;
			pre_group_id := rec.group_id;
			pre_pos_type := rec.pos_type;
			pre_extra_flag := rec.extra_flag;
			pre_link_dir := rec.link_dir;
			pre_s_fraction := rec.s_fraction;
			pre_e_fraction := rec.e_fraction;
			pre_length := rec.length;
			pre_geom_string := rec.geom_string;			
		else
			cur_type_flag := rec.type_flag;
			cur_area_code := rec.area_code;
			cur_infra_id := rec.infra_id;
			cur_dir := rec.dir;
			cur_link_id := rec.rdb_link_id;
			cur_group_id := rec.group_id;
			cur_pos_type := rec.pos_type;
			cur_extra_flag := rec.extra_flag;
			cur_link_dir := rec.link_dir;
			cur_s_fraction := rec.s_fraction;
			cur_e_fraction := rec.e_fraction;
			cur_length := rec.length;
			cur_geom_string := rec.geom_string;			
			
			if cur_area_code = pre_area_code and cur_extra_flag = pre_extra_flag 
				and cur_infra_id = pre_infra_id and cur_dir = pre_dir 
				and cur_link_id = pre_link_id and cur_group_id = pre_group_id then
				--- same location code, same group, same linkid.
				if cur_s_fraction = pre_e_fraction and cur_pos_type = pre_pos_type then 
					--- link dir = 0
					--- fraction should be merged.
					cur_s_fraction = pre_s_fraction;

					pre_link_dir := cur_link_dir;			
					pre_s_fraction := cur_s_fraction;
					pre_e_fraction := cur_e_fraction;
					
				elseif cur_e_fraction = pre_s_fraction and cur_pos_type = pre_pos_type then
					--- link dir = 1
					--- fraction should be merged.
					cur_e_fraction = pre_e_fraction;

					pre_link_dir := cur_link_dir;			
					pre_s_fraction := cur_s_fraction;
					pre_e_fraction := cur_e_fraction;				
				else
					--- fraction cannot be merged, maybe a border of external/internal.
					execute 'insert into ' || quote_ident(target_table) || '(type_flag,infra_id,dir,pos_type,area_code,extra_flag,s_fraction,e_fraction,seq,group_id,rdb_link_id,rdb_link_t,link_dir,length,geom_string) 	
					values('||rec.type_flag||','||rec.infra_id||','||rec.dir||','||pre_pos_type||','||rec.area_code||','||rec.extra_flag||','||pre_s_fraction||','||pre_e_fraction||','||seq||','||pre_group_id||','||rec.rdb_link_id||','||(rec.rdb_link_id >> 32)||','||pre_link_dir||','||pre_length||','||quote_literal(pre_geom_string)||')';

					seq := seq + 1;
					pre_pos_type := cur_pos_type;
					pre_group_id := cur_group_id;
					pre_link_dir := cur_link_dir;			
					pre_s_fraction := cur_s_fraction;
					pre_e_fraction := cur_e_fraction;
				end if;
				
			elsif cur_area_code = pre_area_code and cur_extra_flag = pre_extra_flag 
				and cur_infra_id = pre_infra_id and cur_dir = pre_dir and cur_group_id = pre_group_id
				and cur_link_id <> pre_link_id then
				--- same location code, same group, but different linkid.
				execute 'insert into ' || quote_ident(target_table) || '(type_flag,infra_id,dir,pos_type,area_code,extra_flag,s_fraction,e_fraction,seq,group_id,rdb_link_id,rdb_link_t,link_dir,length,geom_string) 
				values('||pre_type_flag||','||pre_infra_id||','||pre_dir||','||pre_pos_type||','||pre_area_code||','||pre_extra_flag||','||pre_s_fraction||','||pre_e_fraction||','||seq||','||pre_group_id||','||pre_link_id||','||(pre_link_id >> 32)||','||pre_link_dir||','||pre_length||','||quote_literal(pre_geom_string)||')';		
				
				seq := seq + 1;
				pre_area_code := cur_area_code;
				pre_infra_id := cur_infra_id;
				pre_extra_flag := cur_extra_flag;				
				pre_dir := cur_dir;
				pre_link_id := cur_link_id;
				pre_pos_type := cur_pos_type;
				pre_group_id := cur_group_id;
				pre_link_dir := cur_link_dir;				
				pre_s_fraction := cur_s_fraction;
				pre_e_fraction := cur_e_fraction;
				pre_length := cur_length;
				pre_geom_string := cur_geom_string;
			else					
				execute 'insert into ' || quote_ident(target_table) || '(type_flag,infra_id,dir,pos_type,area_code,extra_flag,s_fraction,e_fraction,seq,group_id,rdb_link_id,rdb_link_t,link_dir,length,geom_string) 
				values('||pre_type_flag||','||pre_infra_id||','||pre_dir||','||pre_pos_type||','||pre_area_code||','||pre_extra_flag||','||pre_s_fraction||','||pre_e_fraction||','||seq||','||pre_group_id||','||pre_link_id||','||(pre_link_id >> 32)||','||pre_link_dir||','||pre_length||','||quote_literal(pre_geom_string)||')';		
				--- new location code, so, start a new sequence.
				seq := 1;
				pre_area_code := cur_area_code;
				pre_infra_id := cur_infra_id;
				pre_extra_flag := cur_extra_flag;				
				pre_dir := cur_dir;
				pre_link_id := cur_link_id;
				pre_group_id := cur_group_id;
				pre_pos_type := cur_pos_type;
				pre_link_dir := cur_link_dir;				
				pre_s_fraction := cur_s_fraction;
				pre_e_fraction := cur_e_fraction;
				pre_length := cur_length;
				pre_geom_string := cur_geom_string;			
			end if;
			
		end if;

		i := i + 1;

		fetch curs into rec;
	END LOOP;
	--- the last record.
	execute 'insert into ' || quote_ident(target_table) || '(type_flag,infra_id,dir,pos_type,area_code,extra_flag,s_fraction,e_fraction,seq,group_id,rdb_link_id,rdb_link_t,link_dir,length,geom_string) 
	values('||pre_type_flag||','||pre_infra_id||','||pre_dir||','||pre_pos_type||','||pre_area_code||','||pre_extra_flag||','||pre_s_fraction||','||pre_e_fraction||','||seq||','||pre_group_id||','||pre_link_id||','||(pre_link_id >> 32)||','||pre_link_dir||','||pre_length||','||quote_literal(pre_geom_string)||')';
	
	close curs;
	return 1;	
END;
$$;

create or replace function rdb_get_json_string_for_traffic(lang_code_eng varchar, name_eng varchar)
	returns varchar
	language plpgsql
as $$
declare
	string_th		varchar;
	string_name		varchar;
begin
	string_th		:= '{"lang": "'||lang_code_eng||'", "val": "'||name_eng||'"}';
	string_name		:= '['||array_to_string(ARRAY[string_th],', ')||']';
	return string_name;
end;
$$;

create or replace function rdb_get_json_string_for_trf_names(lang_array varchar[], name_array varchar[])
	returns varchar
	language plpgsql
as $$
declare
	array_len smallint;
	i smallint;
	name varchar[];
	string_name varchar;
begin
	array_len := array_upper(lang_array,1);
	
	if array_len is null then
		return null;
	else
		for i in 1..(array_len) loop
			if name_array[i] is null then
				name_array[i] := '';
			end if;
			name := array_append(name,('{"lang": "'||lang_array[i]||'", "val": "'||name_array[i]||'"}')::varchar);
		end loop;
		string_name := '['||array_to_string(name,', ')||']';
	end if;
	
	return string_name;
end;
$$;

CREATE OR REPLACE FUNCTION rdb_cnv_country_code_common(ecc char, cc char)
  RETURNS integer
  LANGUAGE plpgsql
AS $$
DECLARE
	cc_val integer;
	ecc_val_left integer;
	ecc_val_right integer; 
	ecc_left char;
	ecc_right char;
BEGIN

	if cc = 'A' then cc_val = 10;
	elseif cc = 'B' then cc_val = 11;
	elseif cc = 'C' then cc_val = 12;
	elseif cc = 'D' then cc_val = 13;
	elseif cc = 'E' then cc_val = 14;
	elseif cc = 'F' then cc_val = 15;
	else cc_val = cast(cc as integer);
	end if; 

	ecc_left = substring(ecc,1,1);
	ecc_right = substring(ecc,2,1);
	
	if ecc_left = 'A' then ecc_val_left = 10;
	elseif ecc_left = 'B' then ecc_val_left = 11;
	elseif ecc_left = 'C' then ecc_val_left = 12;
	elseif ecc_left = 'D' then ecc_val_left = 13;
	elseif ecc_left = 'E' then ecc_val_left = 14;
	elseif ecc_left = 'F' then ecc_val_left = 15;
	else ecc_val_left = cast(ecc_left as integer);
	end if; 

	ecc_val_right = cast(ecc_right as integer);

	return     ((ecc_val_left << 20) | (ecc_val_right << 16)) |  cc_val;
END;
$$; 
		
CREATE OR REPLACE FUNCTION rdb_make_trf_region_link_seqs(temp_table character varying,target_table character varying)
  RETURNS integer 
  LANGUAGE plpgsql VOLATILE
AS $$ 
DECLARE
	rec				record;
	i				integer;
	seq				integer;
	new_group_id			smallint;
	curs 					refcursor;	

	pre_type_flag			smallint;
	pre_area_code 			integer;
	pre_infra_id			integer;
	pre_dir 				smallint;
	pre_link_id 			bigint;
	pre_s_fraction			double precision;
	pre_e_fraction			double precision;
	pre_pos_type			smallint;
	pre_extra_flag			integer;
	pre_link_dir			smallint;
	pre_seq					integer;
	pre_group_id			smallint;
	pre_length				integer;
	pre_geom_string			character varying;

	cur_type_flag			smallint;
	cur_area_code 			integer;
	cur_infra_id			integer;
	cur_dir 				smallint;
	cur_link_id 			bigint;	
	cur_s_fraction			double precision;
	cur_e_fraction			double precision;
	cur_pos_type			smallint;
	cur_extra_flag			integer;
	cur_link_dir			smallint;
	cur_seq					integer;
	cur_group_id			smallint;
	cur_length				integer;
	cur_geom_string			character varying;
		
BEGIN
	i := 1;
	seq := 1;
	new_group_id := 0;
	
	open curs for execute 
	'select * from ' || quote_ident(temp_table) || ' order by type_flag,area_code,extra_flag,infra_id,dir,group_id,seq';
	fetch curs into rec;
		
	while rec.rdb_link_id is not null 
	LOOP	
		if i = 1 then 
			pre_type_flag := rec.type_flag;
			pre_area_code := rec.area_code;
			pre_infra_id := rec.infra_id;
			pre_dir := rec.dir;
			pre_link_id := rec.rdb_link_id;
			pre_group_id := rec.group_id;
			pre_pos_type := rec.pos_type;
			pre_extra_flag := rec.extra_flag;
			pre_link_dir := rec.link_dir;
			pre_seq := rec.seq;
			pre_s_fraction := rec.s_fraction;
			pre_e_fraction := rec.e_fraction;
			pre_length := rec.length;
			pre_geom_string := rec.geom_string;			
		else
			cur_type_flag := rec.type_flag;
			cur_area_code := rec.area_code;
			cur_infra_id := rec.infra_id;
			cur_dir := rec.dir;
			cur_link_id := rec.rdb_link_id;
			cur_group_id := rec.group_id;
			cur_pos_type := rec.pos_type;
			cur_extra_flag := rec.extra_flag;
			cur_link_dir := rec.link_dir;
			cur_seq := rec.seq;
			cur_s_fraction := rec.s_fraction;
			cur_e_fraction := rec.e_fraction;
			cur_length := rec.length;
			cur_geom_string := rec.geom_string;			
			
			if cur_area_code = pre_area_code and cur_extra_flag = pre_extra_flag 
				and cur_infra_id = pre_infra_id and cur_dir = pre_dir 
				and cur_group_id = pre_group_id then
				
				--- same location code, same group.
				if cur_seq = pre_seq + 1 then 
					--- normal. links are continuous.
					execute 'insert into ' || quote_ident(target_table) || '(type_flag,infra_id,dir,pos_type,area_code,extra_flag,s_fraction,e_fraction,seq,group_id,rdb_link_id,rdb_link_t,link_dir,length,geom_string) 	
					values('||pre_type_flag||','||pre_infra_id||','||pre_dir||','||pre_pos_type||','||pre_area_code||','||pre_extra_flag||','||pre_s_fraction||','||pre_e_fraction||','||seq||','||new_group_id||','||pre_link_id||','||(pre_link_id >> 32)||','||pre_link_dir||','||pre_length||','||quote_literal(pre_geom_string)||')';			
					seq := seq + 1;
					pre_pos_type := cur_pos_type;
					pre_link_dir := cur_link_dir;
					pre_link_id := cur_link_id;
					pre_seq := cur_seq;
					pre_s_fraction := cur_s_fraction;
					pre_e_fraction := cur_e_fraction;
					pre_length := cur_length;
					pre_geom_string := cur_geom_string;					
				else
					--- unnormal. links are not continuous.
					execute 'insert into ' || quote_ident(target_table) || '(type_flag,infra_id,dir,pos_type,area_code,extra_flag,s_fraction,e_fraction,seq,group_id,rdb_link_id,rdb_link_t,link_dir,length,geom_string) 	
					values('||pre_type_flag||','||pre_infra_id||','||pre_dir||','||pre_pos_type||','||pre_area_code||','||pre_extra_flag||','||pre_s_fraction||','||pre_e_fraction||','||seq||','||new_group_id||','||pre_link_id||','||(pre_link_id >> 32)||','||pre_link_dir||','||pre_length||','||quote_literal(pre_geom_string)||')';
					
					--- start a new group, and start a new sequence.
					seq = 1;
					new_group_id = new_group_id + 1;
					
					pre_pos_type := cur_pos_type;
					pre_group_id := cur_group_id;
					pre_link_dir := cur_link_dir;
					pre_link_id := cur_link_id;
					pre_seq := cur_seq;
					pre_s_fraction := cur_s_fraction;
					pre_e_fraction := cur_e_fraction;
					pre_length := cur_length;
					pre_geom_string := cur_geom_string;			
				end if;
			elsif cur_area_code = pre_area_code and cur_extra_flag = pre_extra_flag 
				and cur_infra_id = pre_infra_id and cur_dir = pre_dir 
				and cur_group_id <> pre_group_id then
					
				execute 'insert into ' || quote_ident(target_table) || '(type_flag,infra_id,dir,pos_type,area_code,extra_flag,s_fraction,e_fraction,seq,group_id,rdb_link_id,rdb_link_t,link_dir,length,geom_string) 
				values('||pre_type_flag||','||pre_infra_id||','||pre_dir||','||pre_pos_type||','||pre_area_code||','||pre_extra_flag||','||pre_s_fraction||','||pre_e_fraction||','||seq||','||new_group_id||','||pre_link_id||','||(pre_link_id >> 32)||','||pre_link_dir||','||pre_length||','||quote_literal(pre_geom_string)||')';		
				--- same location code, but a new group.
				seq := 1;
				new_group_id = new_group_id + 1;
				
				pre_pos_type := cur_pos_type;
				pre_group_id := cur_group_id;
				pre_link_dir := cur_link_dir;
				pre_link_id := cur_link_id;
				pre_seq := cur_seq;
				pre_s_fraction := cur_s_fraction;
				pre_e_fraction := cur_e_fraction;
				pre_length := cur_length;
				pre_geom_string := cur_geom_string;
			else
				--- different location code.
				execute 'insert into ' || quote_ident(target_table) || '(type_flag,infra_id,dir,pos_type,area_code,extra_flag,s_fraction,e_fraction,seq,group_id,rdb_link_id,rdb_link_t,link_dir,length,geom_string) 
				values('||pre_type_flag||','||pre_infra_id||','||pre_dir||','||pre_pos_type||','||pre_area_code||','||pre_extra_flag||','||pre_s_fraction||','||pre_e_fraction||','||seq||','||new_group_id||','||pre_link_id||','||(pre_link_id >> 32)||','||pre_link_dir||','||pre_length||','||quote_literal(pre_geom_string)||')';
				
				--- start a new group, and start a new sequence.
				seq := 1;
				new_group_id = 0;
				pre_area_code := cur_area_code;
				pre_infra_id := cur_infra_id;
				pre_extra_flag := cur_extra_flag;				
				pre_dir := cur_dir;
				pre_link_id := cur_link_id;
				pre_group_id := cur_group_id;
				pre_pos_type := cur_pos_type;
				pre_link_dir := cur_link_dir;	
				pre_seq := cur_seq;			
				pre_s_fraction := cur_s_fraction;
				pre_e_fraction := cur_e_fraction;
				pre_length := cur_length;
				pre_geom_string := cur_geom_string;			
			end if;
			
		end if;

		i := i + 1;

		fetch curs into rec;
	END LOOP;
	--- the last record.
	execute 'insert into ' || quote_ident(target_table) || '(type_flag,infra_id,dir,pos_type,area_code,extra_flag,s_fraction,e_fraction,seq,group_id,rdb_link_id,rdb_link_t,link_dir,length,geom_string) 
	values('||pre_type_flag||','||pre_infra_id||','||pre_dir||','||pre_pos_type||','||pre_area_code||','||pre_extra_flag||','||pre_s_fraction||','||pre_e_fraction||','||seq||','||new_group_id||','||pre_link_id||','||(pre_link_id >> 32)||','||pre_link_dir||','||pre_length||','||quote_literal(pre_geom_string)||')';
	
	close curs;
	return 1;	
END;
$$;

CREATE OR REPLACE FUNCTION rdb_make_trf_rtic_link_seq_thai()
  RETURNS integer 
  LANGUAGE plpgsql VOLATILE
AS $$ 
DECLARE
	rec				temp_rtic_link_temp;
	rec2				record;
	tmp_rec                         record;
	pos_rec				record;
	neg_rec				record;
	start_node			bigint;
	end_node			bigint;
	link_array			character varying[];
	linkdir_array			character varying[];
	sourceid_array			character varying[];
	sfraction_array			double precision[];
	efraction_array			double precision[];
	nCount				integer;
	nIndex				integer;
	pre_meshcode character varying;
	pre_kind   character varying;
	pre_rticid character varying;	
	i int;
	new_group_id int;
BEGIN
	i = 0;
	new_group_id = 0;
	for rec in select * from temp_rtic_link_temp 
		order by meshcode,kind,rticid,linkid
	LOOP
		i = i + 1;
		if i = 1 then
			pre_meshcode = rec.meshcode;
			pre_kind = rec.kind;
			pre_rticid = rec.rticid;
		end if;	
		
		select linkid from temp_rtic_link_walked where meshcode = rec.meshcode 
			and kind = rec.kind and rticid = rec.rticid
			and linkid = rec.linkid into tmp_rec;
		
		if found then
			continue;
		else 
			insert into temp_rtic_link_walked ("meshcode","kind","rticid","source_id","linkid") values (rec.meshcode,rec.kind,rec.rticid,rec.source_id,rec.linkid);
		end if;	
		
		--- search in positive direction.
		pos_rec			:= rdb_make_trf_rtic_link_seq_thai_in_one_direction(rec, rec.e_node, 1);
		end_node		:= pos_rec.reach_node;
		
		--- search in negative direction.
		neg_rec			:= rdb_make_trf_rtic_link_seq_thai_in_one_direction(rec, rec.s_node, 2);
		start_node		:= neg_rec.reach_node;
		
		--- initialize link sequence.
		link_array		:= ARRAY[rec.linkid];
		linkdir_array		:= ARRAY[rec.linkdir];
		sourceid_array		:= ARRAY[rec.source_id];
		sfraction_array 	:= ARRAY[rec.s_fraction];
		efraction_array 	:= ARRAY[rec.e_fraction];
		
		if rec.linkdir = '1' then 
			nCount			:= array_upper(neg_rec.linkrow, 1);
			if nCount is not null then
				for nIndex in 1..nCount loop
					link_array	:= array_prepend(neg_rec.linkrow[nIndex], link_array);
					linkdir_array	:= array_prepend(neg_rec.linkdirrow[nIndex], linkdir_array);
					sourceid_array	:= array_prepend(neg_rec.sourceidrow[nIndex], sourceid_array);
					sfraction_array	:= array_prepend(neg_rec.sfractionrow[nIndex], sfraction_array);
					efraction_array	:= array_prepend(neg_rec.efractionrow[nIndex], efraction_array);
				end loop;
			end if;
			
			nCount			:= array_upper(pos_rec.linkrow, 1);
			if nCount is not null then
				for nIndex in 1..nCount loop
					link_array	:= array_append(link_array, pos_rec.linkrow[nIndex]);
					linkdir_array	:= array_append(linkdir_array, pos_rec.linkdirrow[nIndex]);
					sourceid_array	:= array_append(sourceid_array,pos_rec.sourceidrow[nIndex]);
					sfraction_array	:= array_append(sfraction_array,pos_rec.sfractionrow[nIndex]);
					efraction_array	:= array_append(efraction_array,pos_rec.efractionrow[nIndex]);					
				end loop;
			end if;
		else
			nCount			:= array_upper(pos_rec.linkrow, 1);
			if nCount is not null then
				for nIndex in 1..nCount loop
					link_array	:= array_prepend(pos_rec.linkrow[nIndex], link_array);
					linkdir_array	:= array_prepend(pos_rec.linkdirrow[nIndex], linkdir_array);
					sourceid_array	:= array_prepend(pos_rec.sourceidrow[nIndex], sourceid_array);
					sfraction_array	:= array_prepend(pos_rec.sfractionrow[nIndex], sfraction_array);
					efraction_array	:= array_prepend(pos_rec.efractionrow[nIndex], efraction_array);				end loop;
			end if;
			
			nCount			:= array_upper(neg_rec.linkrow, 1);
			if nCount is not null then
				for nIndex in 1..nCount loop
					link_array	:= array_append(link_array, neg_rec.linkrow[nIndex]);
					linkdir_array	:= array_append(linkdir_array, neg_rec.linkdirrow[nIndex]);
					sourceid_array	:= array_append(sourceid_array,neg_rec.sourceidrow[nIndex]);
					sfraction_array	:= array_append(sfraction_array,neg_rec.sfractionrow[nIndex]);
					efraction_array	:= array_append(efraction_array,neg_rec.efractionrow[nIndex]);				
				end loop;
			end if;
		end if;

		--- if current record is not the first, and current location code is same with previous one,
		--- give it a new group id. cause they should be continous, but actually not.
		--- if current record is the first or current location code is different with previous one,
		--- give it a group id 0. cause it's a new start.
		if i <> 1 and rec.meshcode = pre_meshcode and rec.kind = pre_kind 
			and rec.rticid = pre_rticid
		then
			new_group_id = new_group_id + 1;
		else 
			new_group_id = 0;
		end if;
		
-- 		raise info '   --link_array:%',link_array;
		nCount			:= array_upper(link_array, 1);		
		nIndex := 1;
			
		while nIndex  <= nCount loop				
			insert into temp_rtic_link (linkid, seq, meshcode, kind, rticid, source_id, linkdir, s_fraction, e_fraction, group_id)
				values (link_array[nIndex], nIndex, rec.meshcode, rec.kind, rec.rticid, sourceid_array[nIndex], linkdir_array[nIndex], sfraction_array[nIndex], efraction_array[nIndex], new_group_id);
			nIndex := nIndex + 1;	
		end loop;
		
		pre_meshcode = rec.meshcode;
		pre_kind = rec.kind;
		pre_rticid = rec.rticid;
		
	END LOOP;

	return 1;
	
END;
$$;


CREATE OR REPLACE FUNCTION rdb_make_trf_rtic_link_seq_thai_in_one_direction(
		rec temp_rtic_link_temp, search_node bigint, search_dir integer, OUT linkrow character varying[], OUT linkdirrow character varying[], OUT sourceidrow character varying[], OUT sfractionrow double precision[], OUT efractionrow double precision[],  OUT reach_node bigint)
	RETURNS record
	LANGUAGE plpgsql volatile
AS $$
DECLARE
	rec2				record;
	rec3				record;
	reach_link                      character varying;
BEGIN
	-- init
	reach_link	:= rec.linkid;
	reach_node	:= search_node;
	
	-- search
	while true loop
		
		-- search link
		if rec.linkdir = '1' then
			select * from (
				select	a.*
				from
				(
					(
						select	linkid, e_node as nextnode, meshcode, 
							kind, rticid, linkdir, source_id, s_fraction, e_fraction
						from temp_rtic_link_temp
						where 	(
								(search_dir = 1 and linkdir = '1')
								or 
								(search_dir = 2 and linkdir = '2')
							)
							and 
							(
								(reach_node = s_node) 
								and (linkid != reach_link)
								and (meshcode = rec.meshcode)
								and (kind = rec.kind)
								and (rticid = rec.rticid)
							)
					)
					union
					(
						select	linkid, s_node as nextnode, meshcode, 
							kind, rticid, linkdir, source_id, s_fraction, e_fraction
						from temp_rtic_link_temp
						where 	(
								(search_dir = 1 and linkdir = '2')
								or 
								(search_dir = 2 and linkdir = '1')
							)
							and 
							(
								(reach_node = e_node) 
								and (linkid != reach_link)
								and (meshcode = rec.meshcode)
								and (kind = rec.kind)
								and (rticid = rec.rticid)
							)
					)
				)as a
				left join temp_rtic_link_walked as b
				on a.meshcode = b.meshcode 
				and a.kind = b.kind and a.rticid = b.rticid
				and a.source_id = b.source_id and a.linkid = b.linkid
				where b.linkid is null
			) a order by linkid limit 1
			into rec2;
		else
			select * from (
				select	a.*
				from
				(
					(
						select	linkid, e_node as nextnode, meshcode, 
							kind, rticid, linkdir, source_id, s_fraction, e_fraction
						from temp_rtic_link_temp
						where 	(
								(search_dir = 1 and linkdir = '2')
								or 
								(search_dir = 2 and linkdir = '1')
							)
							and 
							(
								(reach_node = s_node) 
								and (linkid != reach_link)
								and (meshcode = rec.meshcode)
								and (kind = rec.kind)
								and (rticid = rec.rticid)
							)
					)
					union
					(
						select	linkid, s_node as nextnode, meshcode, 
							kind, rticid, linkdir, source_id, s_fraction, e_fraction
						from temp_rtic_link_temp
						where 	(
								(search_dir = 1 and linkdir = '1')
								or 
								(search_dir = 2 and linkdir = '2')
							)
							and 
							(
								(reach_node = e_node) 
								and (linkid != reach_link)
								and (meshcode = rec.meshcode)
								and (kind = rec.kind)
								and (rticid = rec.rticid)
							)
					)
				)as a
				left join temp_rtic_link_walked as b
				on a.kind = b.kind 
				and a.kind = b.kind and a.rticid = b.rticid
				and a.source_id = b.source_id and a.linkid = b.linkid
				where b.linkid is null
			) a order by linkid limit 1
			into rec2;
		end if;
		
		if FOUND then
			
			insert into temp_rtic_link_walked ("meshcode","kind","rticid","source_id","linkid") values (rec2.meshcode,rec2.kind,rec2.rticid,rec2.source_id, rec2.linkid);			
			-- walk this link
			linkrow		:= array_append(linkrow, rec2.linkid);
			linkdirrow	:= array_append(linkdirrow, rec2.linkdir);
			sourceidrow	:= array_append(sourceidrow, rec2.source_id);
			sfractionrow	:= array_append(sfractionrow, rec2.s_fraction);
			efractionrow	:= array_append(efractionrow, rec2.e_fraction);
			reach_link	:= rec2.linkid;
			reach_node	:= rec2.nextnode;
		else
			EXIT;
		end if;
	end loop;
END;
$$;

-----------------------------------------------------------------------------
-- 
CREATE OR REPLACE FUNCTION rdb_geom_2_lonlat(the_geom geometry)
  RETURNS bigint 
  LANGUAGE plpgsql
AS $$ 
DECLARE
        x   bigint;
        y   bigint;
BEGIN
	x = round(st_x(the_geom) * 3600 * 256);
        y = round(st_y(the_geom) * 3600 * 256);
	RETURN x << 32 | y;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_get_outlink_park(node_id bigint,node_type smallint,link_arrry bigint[],
																								link_s_node_array bigint[],link_e_node_array bigint[],
																								one_way_array smallint[])
	RETURNS BIGINT[]
	LANGUAGE PLPGSQL
AS $$
DECLARE
	link_num integer;
	link_num_idx integer;
	out_link_array bigint[];
BEGIN
	link_num := 0;
	link_num := array_upper(link_arrry,1);
	out_link_array = array[]::bigint[];
	
	for link_num_idx in 1..link_num loop
		if node_type = 0 then
			exit;
		elsif node_type = 1 then
			if (link_arrry[link_num_idx]::bit(32))::integer > 1<<23 then
				if (link_s_node_array[link_num_idx] = node_id and one_way_array[link_num_idx] in (1,2)) or 
					 (link_e_node_array[link_num_idx] = node_id and one_way_array[link_num_idx] in (1,3)) then
						out_link_array := out_link_array || link_arrry[link_num_idx];
				end if;
			end if;
		elsif node_type = 2 then
			if (link_arrry[link_num_idx]::bit(32))::integer < 1<<23 then
				if (link_s_node_array[link_num_idx] = node_id and one_way_array[link_num_idx] in (1,2)) or 
					 (link_e_node_array[link_num_idx] = node_id and one_way_array[link_num_idx] in (1,3)) then
						out_link_array := out_link_array || link_arrry[link_num_idx];
				end if;
			end if;
		elsif node_type = 3 then
			if (link_s_node_array[link_num_idx] = node_id and one_way_array[link_num_idx] in (1,2)) or 
				 (link_e_node_array[link_num_idx] = node_id and one_way_array[link_num_idx] in (1,3)) then
					out_link_array := out_link_array || link_arrry[link_num_idx];
			end if;
		else
			raise EXCEPTION 'park_node_type is %d,not in (0,1,2,3)!', node_type;
		end if;
	end loop;
	return out_link_array;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_cnv_slope_value(slope_value integer, swap_flag boolean ) 
  RETURNS integer
  LANGUAGE plpgsql
AS $$
DECLARE
	value_t integer;
BEGIN
	value_t := slope_value;
	
	IF swap_flag = TRUE THEN
		value_t = -value_t;
	END IF;
	
	value_t = CASE
		WHEN value_t =   0 THEN 0
		WHEN value_t =  10 THEN 1
		WHEN value_t =  20 THEN 2
		WHEN value_t =  30 THEN 3
		WHEN value_t =  40 THEN 4
		WHEN value_t = -10 THEN 5
		WHEN value_t = -20 THEN 6
		WHEN value_t = -30 THEN 7
		WHEN value_t = -40 THEN 8
		ELSE 0
	END;
	
	RETURN value_t;
	
END;
$$;


CREATE OR REPLACE FUNCTION rdb_check_slopes_ipc() 
  RETURNS integer
  LANGUAGE plpgsql
AS $$
DECLARE
	nRec_count	integer;
BEGIN
    
    SELECT COUNT(*)  
    FROM(
	    SELECT 
			abs(ST_X(slope_pt_s)-ST_X(link_pt_s)) AS dx_s,
			abs(ST_Y(slope_pt_s)-ST_Y(link_pt_s)) AS dy_s,
			abs(ST_X(slope_pt_e)-ST_X(link_pt_e)) AS dx_e,
			abs(ST_Y(slope_pt_e)-ST_Y(link_pt_e)) AS dy_e
	    FROM(
			SELECT 
				link_pt_s AS link_pt_s,
				link_pt_e AS link_pt_e,
				ST_Line_Interpolate_Point(slope_shape, slope_pos_s) AS slope_pt_s,
				ST_Line_Interpolate_Point(slope_shape, slope_pos_e) AS slope_pt_e
			FROM(	
				SELECT 
					link_pt_s AS link_pt_s,
					link_pt_e AS link_pt_e,
					slope_shape AS slope_shape,
					ST_Line_Locate_Point(slope_shape, link_pt_s) AS slope_pos_s,
					ST_Line_Locate_Point(slope_shape, link_pt_e) AS slope_pos_e
				FROM(		    
					SELECT  
						b.slope_shape AS slope_shape,
						ST_Line_Interpolate_Point(a.the_geom, b.link_pos_s) AS link_pt_s,
						ST_Line_Interpolate_Point(a.the_geom, b.link_pos_e) AS link_pt_e
					FROM rdb_link AS a
					INNER JOIN temp_rdb_slope AS b
					ON a.link_id = b.rdb_link_id
				) AS c
			) AS d
	    ) AS e
    ) AS f	
    WHERE dx_s > CAST('0.0001' AS float) OR
		  dy_s > CAST('0.0001' AS float) OR
		  dx_e > CAST('0.0001' AS float) OR
		  dY_e > CAST('0.0001' AS float)
	INTO nRec_count;
	
	IF nRec_count > 0 THEN
    	return -1;
    END IF;
    
    return 1;
END;
$$;

-------------------------------------------------------------------------
---
-------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION rdb_get_start_end_flag(nLinkID bigint, nNodeID bigint)
	RETURNS integer
	LANGUAGE plpgsql volatile
AS $$
DECLARE
        s_node_id  bigint;
        e_node_id  bigint;
BEGIN
        SELECT s_node, e_node into s_node_id, e_node_id
          from link_tbl
          where link_id = nLinkID;

        if s_node_id = nNodeID then
                return 0;
        end if;
        if e_node_id = nNodeID then
                return 1;
        end if;
        return NULL;
END;
$$;
-------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION rdb_get_park_nodeid()
	RETURNS integer
	LANGUAGE plpgsql volatile
AS $$
DECLARE
	rec1			record;
	cur1 			refcursor;
	rec2			record;
	cur2 			refcursor;
	imaxid 		bigint;
	
BEGIN
	open cur1 for select park_node_id_t
								from rdb_park_node
								group by park_node_id_t
								order by park_node_id_t;
	fetch cur1 into rec1;
	while rec1.park_node_id_t is not null loop
		
		imaxid := 0;
		select max(park_node_id) into imaxid
					from rdb_park_node 
					where park_node_id_t = rec1.park_node_id_t and park_base_id is null;
		
		if imaxid = 0 then
			imaxid := 1 + 1<<23;
		else
			imaxid := imaxid + 1;
		end if;
					
		open cur2 for select park_base_id
				from rdb_park_node
				where park_node_id_t = rec1.park_node_id_t and park_base_id is not null
				order by park_base_id;
		fetch cur2 into rec2;
		while rec2.park_base_id is not null loop
			insert into temp_park_node_id(base_node_id,new_park_node_id)
			values(rec2.park_base_id,imaxid);
			imaxid := imaxid + 1;
			fetch cur2 into rec2;
		end loop;
		close cur2;
		fetch cur1 into rec1;
	end loop;
	close cur1;
	return 0;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_delete_base_link(link_lib bigint[])
	RETURNS bigint[]
	LANGUAGE plpgsql volatile
AS $$
DECLARE
	new_link bigint[];
	new_link_num integer;
	link_lib_num integer;
BEGIN
	link_lib_num := array_upper(link_lib,1);
	new_link_num := 1;
	for link_lib_num_idx in 1..link_lib_num loop
		if (link_lib[link_lib_num_idx] & (1<<23)) > 0 then
			new_link[new_link_num] := link_lib[link_lib_num_idx];
			new_link_num := new_link_num + 1;
		end if;
	end loop;
	return new_link;
END;
$$;
-----------------------------------------------------------------------
--delete region for malsing
-------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION rdb_delete_shortlink_for_region()
	RETURNS integer
	LANGUAGE PLPGSQL
AS $$
DECLARE
	cur1	refcursor;
	rec1	record;
	iNum 	integer;
BEGIN
	
	while true loop
	
		drop table if exists temp_rout_add_delete;
		create table temp_rout_add_delete
		as
		(
			select link_id,unnest(node_array) as node_id
			from
			(
				select link_id,array[a.start_node_id,a.end_node_id] as node_array
				from temp_org_add_region as a
				join temp_node_connect_link_num as b
				on a.start_node_id = b.node_id or a.end_node_id = b.node_id
				where b.num = 1
			)temp
		);
		
		select count(link_id) into iNum
		from temp_rout_add_delete;
		
		if iNum = 0 then
			exit;
		end if;
		
		update temp_node_connect_link_num
		set num = num - 1
		where node_id in (
			select distinct node_id
			from temp_rout_add_delete
		);
		
		delete from temp_org_add_region
		where link_id in (
			select distinct link_id from temp_rout_add_delete
		);
	end loop;
	
	return 0;
END;
$$;
--------------------------------------------------------------------------------------
--get link sequence
--------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION rdb_get_rdb_link_sequence(node_id bigint,link_array bigint[],s_node_array bigint[],e_node_array bigint[],
																one_way_array smallint[],fazm_array smallint[],tazm_array smallint[],toll_array smallint[],
																road_type_array smallint[],link_type_array integer[],name_array varchar[],name_num_array varchar[])
	RETURNS varchar[]
	LANGUAGE PLPGSQL
AS $$
DECLARE
	inlink_array bigint[];
	inlink_array_num integer;
	outlink_array bigint[];
	outlink_array_num integer;
	link_array_num integer;	
	sequence_array varchar[];
	sequence_array_num integer;
	dir_array integer[];
	all_azm_array smallint[];
	istart_position integer;
	iend_position integer;
BEGIN
	if link_array is null then
		return null;
	end if;
	
	dir_array := rdb_get_dir_sequence(node_id,link_array,s_node_array,e_node_array);
	inlink_array := rdb_get_inlink_sequence(link_array,dir_array,one_way_array);
	outlink_array := rdb_get_outlink_sequence(link_array,dir_array,one_way_array);
	all_azm_array := rdb_get_azm_sequence(dir_array,fazm_array,tazm_array);
	
	
	if outlink_array is null or inlink_array is null then
		return null;
	end if;
	
	sequence_array_num := 0;
	inlink_array_num := array_upper(inlink_array,1);
	outlink_array_num := array_upper(outlink_array,1);
	for inlink_array_num_idx in 1..inlink_array_num loop
	
		istart_position := rdb_get_position_array_sequence(inlink_array[inlink_array_num_idx],link_array) + 1;
		iend_position := rdb_get_outlink_position_sequence(istart_position,link_array,outlink_array,toll_array,
																all_azm_array,road_type_array,link_type_array,name_array,name_num_array);
		if iend_position >= 0 then
			sequence_array_num := sequence_array_num + 1;
			sequence_array[sequence_array_num] := inlink_array[inlink_array_num_idx]::varchar || '|' || dir_array[istart_position]::varchar 
															|| '|' || iend_position::varchar || '|' || link_array[iend_position + 1]::varchar;
		end if;
	end loop;
	
	return sequence_array;
END;
$$;
CREATE OR REPLACE FUNCTION rdb_get_outlink_position_sequence(istart_position integer,link_array bigint[],outlink_array bigint[],toll_array smallint[],
																azm_array smallint[],road_type_array smallint[],link_type_array integer[],name_array varchar[],name_num_array varchar[])
	RETURNS integer
	LANGUAGE PLPGSQL
AS $$
DECLARE
	outlink_position integer;
	link_array_num integer;
	outlink_array_num integer;
	iposition integer;
	iNum integer;
	iazm smallint;
	iazm_temp smallint;
	name_flage boolean;
	toll_flage boolean;
	highway_flage boolean;
BEGIN
	outlink_array_num := array_upper(outlink_array,1);

	iNum = 0;
	iazm = 360;
	name_flage := false;
	toll_flage := false;
	highway_flage := false;
	for outlink_array_num_idx in 1..outlink_array_num loop
		iposition := rdb_get_position_array_sequence(outlink_array[outlink_array_num_idx],link_array) + 1;
		iazm_temp := rdb_get_angle_sequence(istart_position,iposition,azm_array);
		
		--angle is big		
		if istart_position = iposition or iazm_temp > 90 or 
			rdb_jude_two_link_type_sequence(link_type_array[istart_position],link_type_array[iposition]) = false then
			continue;
		end if;
		
		--name
		if (((name_array[istart_position] = name_array[iposition]) = true) or ((name_num_array[istart_position] = name_num_array[iposition]) = true))
			  and (link_type_array[istart_position] <> 4 and link_type_array[iposition] <> 4) then
			  
		  if name_flage and iazm < iazm_temp then
				continue;
			else
				iNum := iposition;
				iazm := iazm_temp;
				name_flage := true;
				continue;
			end if;
				
		end if;
		
		--toll
		if link_type_array[istart_position] in (1,2) and link_type_array[iposition] in (1,2) and 
				toll_array[istart_position] in (1,3) and toll_array[iposition] in (1,3) and name_flage = false then
			if toll_flage and iazm < iazm_temp then
				continue;
			else
				iNum := iposition;
				iazm := iazm_temp;
				toll_flage := true;
				continue;
			end if;
		end if;
		
		if name_flage = false and toll_flage = false then
			--highway
			if link_type_array[istart_position] in (1,2) then
				
				if link_type_array[iposition] in (3,5,6,7) then
					continue;
				end if;

				if road_type_array[istart_position] in (0,1) and road_type_array[iposition] in (0,1) then
					if highway_flage and iazm < iazm_temp then
						continue;
					else
						iNum := iposition;
						iazm := iazm_temp;
						highway_flage := true;
						continue;
					end if;
				end if;
				
				if highway_flage = false and iazm_temp <= 30 and iazm > iazm_temp then
					iNum := iposition;
					iazm := iazm_temp;
				end if;
			
			elsif link_type_array[istart_position] in (3,5,6,7) then
				if link_type_array[iposition] in (3,5,6,7) and iazm > iazm_temp then
					iNum := iposition;
					iazm := iazm_temp;
				end if;
			elsif link_type_array[istart_position] = 4 then
				if link_type_array[iposition] in (1,2,4) and iazm_temp <= 30 and iazm > iazm_temp then
					iNum := iposition;
					iazm := iazm_temp;
				end if;
			else
				raise EXCEPTION 'link_type %s is error!', link_type_array[istart_position];
			end if;		
		end if;

	end loop;
	
	return iNum - 1;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_get_outlinkid(link_array bigint[], toll_array smallint[],azm_array smallint[],
							road_type_array smallint[],link_type_array integer[],name_array varchar[],name_num_array varchar[])
	RETURNS bigint
	LANGUAGE PLPGSQL
AS $$
DECLARE
	outlink_position integer;
	link_array_num integer;
	outlink_array_num integer;
	iposition integer;
	iNum integer;
	iazm smallint;
	iazm_temp smallint;
	name_flage boolean;
	toll_flage boolean;
	highway_flage boolean;
BEGIN
	if link_array is null then
		return null;
	end if; 
	link_array_num := array_upper(link_array,1);
	if link_array_num = 1 then
		return null;
	end if;

	iNum = 0;
	iazm = 360;
	name_flage := false;
	toll_flage := false;
	highway_flage := false;
	for link_array_num_idx in 2..link_array_num loop
		iazm_temp := rdb_get_angle_sequence_temp(azm_array[1], azm_array[link_array_num_idx]);
		
		--angle is big		
		if iazm_temp > 90 or rdb_jude_two_link_type_sequence(link_type_array[1],link_type_array[link_array_num_idx]) = false then
			continue;
		end if;
		
		--name
		if (((name_array[1] = name_array[link_array_num_idx]) = true) or ((name_num_array[1] = name_num_array[link_array_num_idx]) = true))
			  and (link_type_array[1] <> 4 and link_type_array[link_array_num_idx] <> 4) then
			  
		  if name_flage and iazm < iazm_temp then
				continue;
			else
				iNum := link_array_num_idx;
				iazm := iazm_temp;
				name_flage := true;
				continue;
			end if;
				
		end if;
		
		--toll
		if link_type_array[1] in (1,2) and link_type_array[link_array_num_idx] in (1,2) and 
				toll_array[1] in (1,3) and toll_array[link_array_num_idx] in (1,3) and name_flage = false then
			if toll_flage and iazm < iazm_temp then
				continue;
			else
				iNum := link_array_num_idx;
				iazm := iazm_temp;
				toll_flage := true;
				continue;
			end if;
		end if;
		
		if name_flage = false and toll_flage = false then
			--highway
			if link_type_array[1] in (1,2) then
				
				if link_type_array[link_array_num_idx] in (3,5,6,7) then
					continue;
				end if;

				if road_type_array[1] in (0,1) and road_type_array[link_array_num_idx] in (0,1) then
					if highway_flage and iazm < iazm_temp then
						continue;
					else
						iNum := link_array_num_idx;
						iazm := iazm_temp;
						highway_flage := true;
						continue;
					end if;
				end if;
				
				if highway_flage = false and iazm_temp <= 30 and iazm > iazm_temp then
					iNum := link_array_num_idx;
					iazm := iazm_temp;
				end if;
			
			elsif link_type_array[1] in (3,5,6,7) then
				if link_type_array[link_array_num_idx] in (3,5,6,7) and iazm > iazm_temp then
					iNum := link_array_num_idx;
					iazm := iazm_temp;
				end if;
			elsif link_type_array[1] = 4 then
				if link_type_array[link_array_num_idx] in (1,2,4) and iazm_temp <= 30 and iazm > iazm_temp then
					iNum := link_array_num_idx;
					iazm := iazm_temp;
				end if;
			else
				raise EXCEPTION 'link_type %s is error!', link_type_array[1];
			end if;		
		end if;

	end loop;
	
	if iNum = 0 then
		return null;
	else
		return link_array[iNum];
	end if;
END;
$$;


CREATE OR REPLACE FUNCTION rdb_jude_two_link_type_sequence(start_link_type integer,end_link_type integer)
	RETURNS boolean
	LANGUAGE PLPGSQL
AS $$
DECLARE
BEGIN
	if (start_link_type in (3,5,6,7) and end_link_type in (3,5,6,7)) or
	   (start_link_type in (1,2,4) and end_link_type in (1,2,4)) then
		return true;
	else
		return false;
	end if;
	
	return false;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_get_outlink_position_sequence_old(istart_position integer,link_array bigint[],outlink_array bigint[],toll_array smallint[],
																azm_array smallint[],road_type_array smallint[],link_type_array integer[],name_array varchar[],name_num_array varchar[])
	RETURNS integer
	LANGUAGE PLPGSQL
AS $$
DECLARE
	outlink_position integer;
	link_array_num integer;
	outlink_array_num integer;
	iposition integer;
	iNum integer;
	iazm smallint;
	iazm_temp smallint;
	name_flage boolean;
BEGIN
	outlink_array_num := array_upper(outlink_array,1);
	--if outlink_array_num = 1 then
		--if link_array[istart_position] = outlink_array[outlink_array_num] then
			--return -1;
		--else
			--return rdb_get_position_array_sequence(outlink_array[outlink_array_num],link_array);
		--end if;
	--else
		iNum = 0;
		iazm = 360;
		name_flage := false;
		for outlink_array_num_idx in 1..outlink_array_num loop
			iposition := rdb_get_position_array_sequence(outlink_array[outlink_array_num_idx],link_array) + 1;
			iazm_temp := rdb_get_angle_sequence(istart_position,iposition,azm_array);
			
			--angle is big		
			if istart_position = iposition or iazm_temp > 90 then
				continue;
			end if;
			
			--judge name
			if (((name_array[istart_position] = name_array[iposition]) = true) or 
					((name_num_array[istart_position] = name_num_array[iposition]) = true)) 
					and
						((link_type_array[istart_position] in (3,5,6,7) and link_type_array[iposition] in (3,5,6,7)) 
						or
						(link_type_array[istart_position] not in (3,5,6,7) and link_type_array[iposition] not in (3,5,6,7)))
					and 
					(link_type_array[iposition] <> 4) then
					
				if name_flage and iazm < iazm_temp then
					continue;
				else
					iNum := iposition;
					iazm := iazm_temp;
					name_flage := true;
					continue;
				end if;
			end if;
			
			--jude type and angle
			if name_flage = false then
				if road_type_array[istart_position] in (0,1) and link_type_array[istart_position] in (1,2) then
					if road_type_array[iposition] in (0,1) and link_type_array[iposition] in (1,2) then
						if iazm > iazm_temp then
							iNum := iposition;
							iazm := iazm_temp;
						end if;
					end if;
					
				elsif toll_array[istart_position] in (1,3) then
					if toll_array[iposition] in (1,3)then
						if iazm > iazm_temp then
							iNum := iposition;
							iazm := iazm_temp;
						end if;
					else
						if iazm > iazm_temp and iazm_temp <= 30 then
							iNum := iposition;
							iazm := iazm_temp;
						end if;
					end if;
					
				elsif link_type_array[istart_position] in (3,5,6,7) then
					if link_type_array[iposition] in (3,5,6,7) then
						if iazm > iazm_temp then
							iNum := iposition;
							iazm := iazm_temp;
						end if;
					end if;
					
				else
					if (link_type_array[iposition] in (1,2) and road_type_array[iposition] not in (0,1) and toll_array[iposition] in (0,2)) or
							 link_type_array[iposition] = 4 then
						if iazm > iazm_temp and iazm_temp <= 30 then
							iNum := iposition;
							iazm := iazm_temp;
						end if;
					end if;
					
				end if;
			end if;
		end loop;
	--end if;
	
	return iNum - 1;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_get_angle_sequence_temp(fstart_azm smallint, fend_azm smallint)
	RETURNS smallint
	LANGUAGE PLPGSQL
AS $$
DECLARE
	fstart double precision;
	fend double precision;
BEGIN
	 fstart := (fstart_azm + 32768) * 360.0 / 65536.0;
	 fend := (fend_azm + 32768) * 360.0 / 65536.0;
	 
	 return abs(abs(fstart - fend) - 180.0)::smallint;
END;
$$;
CREATE OR REPLACE FUNCTION rdb_get_angle_sequence(istart_position integer,iend_position integer,azm_array smallint[])
	RETURNS smallint
	LANGUAGE PLPGSQL
AS $$
DECLARE
	fstart double precision;
	fend double precision;
BEGIN
	 fstart := (azm_array[istart_position] + 32768) * 360.0 / 65536.0;
	 fend := (azm_array[iend_position] + 32768) * 360.0 / 65536.0;
	 
	 return abs(abs(fstart - fend) - 180.0)::smallint;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_get_dir_sequence(node_id bigint,link_array bigint[],s_node_array bigint[],e_node_array bigint[])
	RETURNS integer[]
	LANGUAGE PLPGSQL
AS $$
DECLARE
	dir_array integer[];
	link_array_num integer;
BEGIN
	if link_array is null then
		return null;
	end if;
	
	--get dir
	link_array_num := array_upper(link_array,1);
	for link_array_num_idx in 1..link_array_num loop
		if s_node_array[link_array_num_idx] = node_id then
			dir_array[link_array_num_idx] := 0;
		else
			dir_array[link_array_num_idx] := 1;
		end if;
	end loop;
	return dir_array;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_get_outlink_sequence(link_array bigint[],dir_array integer[],one_way_array smallint[])
	RETURNS bigint[]
	LANGUAGE PLPGSQL
AS $$
DECLARE
	outlink_array bigint[];
	outlink_array_num integer;
	link_array_num integer;
BEGIN
	if link_array is null then
		return null;
	end if;
	
	--get outlink
	outlink_array_num := 0;
	link_array_num := array_upper(link_array,1);
	for link_array_num_idx in 1..link_array_num loop
		if (dir_array[link_array_num_idx] = 0 and one_way_array[link_array_num_idx] in (1,2)) or
			(dir_array[link_array_num_idx] = 1 and one_way_array[link_array_num_idx] in (1,3))	then
			outlink_array_num := outlink_array_num + 1;
			outlink_array[outlink_array_num] := link_array[link_array_num_idx];
		end if;
	end loop;
	return outlink_array;
END;
$$;
CREATE OR REPLACE FUNCTION rdb_get_inlink_sequence(link_array bigint[],dir_array integer[],one_way_array smallint[])
	RETURNS bigint[]
	LANGUAGE PLPGSQL
AS $$
DECLARE
	inlink_array bigint[];
	inlink_array_num integer;
	link_array_num integer;
BEGIN
	if link_array is null then
		return null;
	end if;
	
	--inlink
	inlink_array_num := 0;
	link_array_num := array_upper(link_array,1);
	for link_array_num_idx in 1..link_array_num loop
		if (dir_array[link_array_num_idx] = 0 and one_way_array[link_array_num_idx] in (1,3)) or
			(dir_array[link_array_num_idx] = 1 and one_way_array[link_array_num_idx] in (1,2))	then
			inlink_array_num := inlink_array_num + 1;
			inlink_array[inlink_array_num] := link_array[link_array_num_idx];
		end if;
	end loop;
	return inlink_array;
END;
$$;
CREATE OR REPLACE FUNCTION rdb_get_azm_sequence(dir_array integer[],fazm_array smallint[],tazm_array smallint[])
	RETURNS smallint[]
	LANGUAGE PLPGSQL
AS $$
DECLARE
	all_azm_array smallint[];
	dir_array_num integer;
BEGIN
	if dir_array is null then
		return null;
	end if;
	
	--get azm
	dir_array_num := array_upper(dir_array,1);
	for dir_array_num_idx in 1..dir_array_num loop
		if dir_array[dir_array_num_idx] = 0 then
			all_azm_array[dir_array_num_idx] := fazm_array[dir_array_num_idx];
		else
			all_azm_array[dir_array_num_idx] := tazm_array[dir_array_num_idx];
		end if;
	end loop;
	
	return all_azm_array;
END;
$$;
--from 0..max
CREATE OR REPLACE FUNCTION rdb_get_position_array_sequence(link_id bigint,link_array bigint[])
	RETURNS integer
	LANGUAGE PLPGSQL
AS $$
DECLARE
	link_array_num integer;
BEGIN
	if link_array is null then
		return -1;
	end if;
	
	link_array_num := array_upper(link_array,1);
	for link_array_num_idx in 1..link_array_num loop
		if link_id = link_array[link_array_num_idx] then
			return link_array_num_idx - 1;
		end if;
	end loop;
	
	return -1;
END;
$$;
CREATE OR REPLACE FUNCTION rdb_sequence_combine(dir_array smallint[],sequence_array smallint[])
	RETURNS smallint
	LANGUAGE PLPGSQL
AS $$
DECLARE
	dir_array_num integer;
	sequence_start smallint;
	sequence_end smallint;
BEGIN
	if dir_array is null then
		return null;
	end if;
	
	sequence_start := ((1<<4) - 1)::smallint;
	sequence_end := (((1<<4) - 1)<<4)::smallint;
	
	dir_array_num := array_upper(dir_array,1);
	for dir_array_num_idx in 1..dir_array_num loop
		if dir_array[dir_array_num_idx] = 0 then
			sequence_start := sequence_array[dir_array_num_idx];
		elsif dir_array[dir_array_num_idx] = 1 then
			sequence_end := (sequence_array[dir_array_num_idx] << 4)::smallint;
		else
			raise EXCEPTION 'link_sequence dir type is %s!', dir_array[dir_array_num_idx];
		end if;
	end loop;
	
	return sequence_start + sequence_end;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_sequence_get(dir_array smallint[],link_array bigint[])
	RETURNS bigint[]
	LANGUAGE PLPGSQL
AS $$
DECLARE
	dir_array_num integer;
	sequence_start bigint;
	sequence_end bigint;
BEGIN
	if dir_array is null then
		return null;
	end if;
	
	sequence_start := -1;
	sequence_end := -1;
	
	dir_array_num := array_upper(dir_array,1);
	for dir_array_num_idx in 1..dir_array_num loop
		if dir_array[dir_array_num_idx] = 0 then
			sequence_start := link_array[dir_array_num_idx];
		elsif dir_array[dir_array_num_idx] = 1 then
			sequence_end := link_array[dir_array_num_idx];
		else
			raise EXCEPTION 'link_sequence dir type is %s!', dir_array[dir_array_num_idx];
		end if;
	end loop;
	
	return array[sequence_start,sequence_end];
END;
$$;


CREATE OR REPLACE FUNCTION mid_get_fraction(link_geom geometry, node_geom geometry)
  RETURNS float
  LANGUAGE plpgsql
AS $$
DECLARE
	sub_string		geometry;
	sum_len			float;
	sub_len			float;
	fraction		float;
BEGIN
	fraction	:= st_line_locate_point(link_geom, node_geom);

	sub_string	:= ST_Line_Substring(link_geom,0,fraction);

	sum_len		:= ST_Length_Spheroid(link_geom,'SPHEROID("WGS_84", 6378137, 298.257223563)');
	sub_len		:= ST_Length_Spheroid(sub_string,'SPHEROID("WGS_84", 6378137, 298.257223563)');
	
	return sub_len/sum_len;
END;
$$;

----------------------------
CREATE OR REPLACE FUNCTION rdb_make_trf_rtic_link_seq()
  RETURNS integer 
  LANGUAGE plpgsql VOLATILE
AS $$ 
DECLARE
	rec				temp_rtic_link_temp;
	rec2				record;
	tmp_rec                         record;
	pos_rec				record;
	neg_rec				record;
	start_node			bigint;
	end_node			bigint;
	link_array			character varying[];
	linkdir_array			character varying[];
	nCount				integer;
	nIndex				integer;
	pre_meshcode character varying;
	pre_kind   character varying;
	pre_rticid character varying;	
	i int;
	new_group_id int;
BEGIN
	i = 0;
	new_group_id = 0;
	for rec in select * from temp_rtic_link_temp 
		order by meshcode,kind,rticid,linkid
	LOOP
		i = i + 1;
		if i = 1 then
			pre_meshcode = rec.meshcode;
			pre_kind = rec.kind;
			pre_rticid = rec.rticid;
		end if;	
		
		select linkid from temp_rtic_link_walked where meshcode = rec.meshcode 
			and kind = rec.kind and rticid = rec.rticid
			and linkid = rec.linkid into tmp_rec;
		
		if found then
			continue;
		else 
			insert into temp_rtic_link_walked ("meshcode","kind","rticid","linkid") values (rec.meshcode,rec.kind,rec.rticid,rec.linkid);
		end if;	
		
		--- search in positive direction.
		pos_rec			:= rdb_make_trf_rtic_link_seq_in_one_direction(rec, rec.e_node, 1);
		end_node		:= pos_rec.reach_node;
		
		--- search in negative direction.
		neg_rec			:= rdb_make_trf_rtic_link_seq_in_one_direction(rec, rec.s_node, 2);
		start_node		:= neg_rec.reach_node;
		
		--- initialize link sequence.
		link_array		:= ARRAY[rec.linkid];
		linkdir_array		:= ARRAY[rec.linkdir];
		
		if rec.linkdir = '1' then 
			nCount			:= array_upper(neg_rec.linkrow, 1);
			if nCount is not null then
				for nIndex in 1..nCount loop
					link_array	:= array_prepend(neg_rec.linkrow[nIndex], link_array);
					linkdir_array	:= array_prepend(neg_rec.linkdirrow[nIndex], linkdir_array);
				end loop;
			end if;
			
			nCount			:= array_upper(pos_rec.linkrow, 1);
			if nCount is not null then
				for nIndex in 1..nCount loop
					link_array	:= array_append(link_array, pos_rec.linkrow[nIndex]);
					linkdir_array	:= array_append(linkdir_array, pos_rec.linkdirrow[nIndex]);				
				end loop;
			end if;
		else
			nCount			:= array_upper(pos_rec.linkrow, 1);
			if nCount is not null then
				for nIndex in 1..nCount loop
					link_array	:= array_prepend(pos_rec.linkrow[nIndex], link_array);
					linkdir_array	:= array_prepend(pos_rec.linkdirrow[nIndex], linkdir_array);				
				end loop;
			end if;
			
			nCount			:= array_upper(neg_rec.linkrow, 1);
			if nCount is not null then
				for nIndex in 1..nCount loop
					link_array	:= array_append(link_array, neg_rec.linkrow[nIndex]);
					linkdir_array	:= array_append(linkdir_array, neg_rec.linkdirrow[nIndex]);				
				end loop;
			end if;
		end if;

		--- if current record is not the first, and current location code is same with previous one,
		--- give it a new group id. cause they should be continous, but actually not.
		--- if current record is the first or current location code is different with previous one,
		--- give it a group id 0. cause it's a new start.
		if i <> 1 and rec.meshcode = pre_meshcode and rec.kind = pre_kind 
			and rec.rticid = pre_rticid
		then
			new_group_id = new_group_id + 1;
		else 
			new_group_id = 0;
		end if;
		
-- 		raise info '   --link_array:%',link_array;
		nCount			:= array_upper(link_array, 1);		
		nIndex := 1;
			
		while nIndex  <= nCount loop				
			insert into temp_rtic_link (linkid, seq, meshcode, kind, rticid, linkdir, group_id)
				values (link_array[nIndex], nIndex, rec.meshcode, rec.kind, rec.rticid, linkdir_array[nIndex], new_group_id);
			nIndex := nIndex + 1;	
		end loop;
		
		pre_meshcode = rec.meshcode;
		pre_kind = rec.kind;
		pre_rticid = rec.rticid;
		
	END LOOP;

	return 1;
	
END;
$$;


CREATE OR REPLACE FUNCTION rdb_make_trf_rtic_link_seq_in_one_direction(
		rec temp_rtic_link_temp, search_node bigint, search_dir integer, OUT linkrow character varying[], OUT linkdirrow character varying[], OUT reach_node bigint)
	RETURNS record
	LANGUAGE plpgsql volatile
AS $$
DECLARE
	rec2				record;
	rec3				record;
	reach_link                      character varying;
BEGIN
	-- init
	reach_link	:= rec.linkid;
	reach_node	:= search_node;
	
	-- search
	while true loop
		
		-- search link
		if rec.linkdir = '1' then
			select * from (
				select	a.*
				from
				(
					(
						select	linkid, e_node as nextnode, meshcode, 
							kind, rticid, linkdir
						from temp_rtic_link_temp
						where 	(
								(search_dir = 1 and linkdir = '1')
								or 
								(search_dir = 2 and linkdir = '2')
							)
							and 
							(
								(reach_node = s_node) 
								and (linkid != reach_link)
								and (meshcode = rec.meshcode)
								and (kind = rec.kind)
								and (rticid = rec.rticid)
							)
					)
					union
					(
						select	linkid, s_node as nextnode, meshcode, 
							kind, rticid, linkdir
						from temp_rtic_link_temp
						where 	(
								(search_dir = 1 and linkdir = '2')
								or 
								(search_dir = 2 and linkdir = '1')
							)
							and 
							(
								(reach_node = e_node) 
								and (linkid != reach_link)
								and (meshcode = rec.meshcode)
								and (kind = rec.kind)
								and (rticid = rec.rticid)
							)
					)
				)as a
				left join temp_rtic_link_walked as b
				on a.meshcode = b.meshcode 
				and a.kind = b.kind and a.rticid = b.rticid
				and a.linkid = b.linkid
				where b.linkid is null
			) a order by linkid limit 1
			into rec2;
		else
			select * from (
				select	a.*
				from
				(
					(
						select	linkid, e_node as nextnode, meshcode, 
							kind, rticid, linkdir
						from temp_rtic_link_temp
						where 	(
								(search_dir = 1 and linkdir = '2')
								or 
								(search_dir = 2 and linkdir = '1')
							)
							and 
							(
								(reach_node = s_node) 
								and (linkid != reach_link)
								and (meshcode = rec.meshcode)
								and (kind = rec.kind)
								and (rticid = rec.rticid)
							)
					)
					union
					(
						select	linkid, s_node as nextnode, meshcode, 
							kind, rticid, linkdir
						from temp_rtic_link_temp
						where 	(
								(search_dir = 1 and linkdir = '1')
								or 
								(search_dir = 2 and linkdir = '2')
							)
							and 
							(
								(reach_node = e_node) 
								and (linkid != reach_link)
								and (meshcode = rec.meshcode)
								and (kind = rec.kind)
								and (rticid = rec.rticid)
							)
					)
				)as a
				left join temp_rtic_link_walked as b
				on a.kind = b.kind 
				and a.kind = b.kind and a.rticid = b.rticid
				and a.linkid = b.linkid
				where b.linkid is null
			) a order by linkid limit 1
			into rec2;
		end if;
		
		if FOUND then
			
			insert into temp_rtic_link_walked ("meshcode","kind","rticid","linkid") values (rec2.meshcode,rec2.kind,rec2.rticid,rec2.linkid);			
			-- walk this link
			linkrow		:= array_append(linkrow, rec2.linkid);
			linkdirrow	:= array_append(linkdirrow, rec2.linkdir);
			reach_link	:= rec2.linkid;
			reach_node	:= rec2.nextnode;
		else
			EXIT;
		end if;
	end loop;
END;
$$;