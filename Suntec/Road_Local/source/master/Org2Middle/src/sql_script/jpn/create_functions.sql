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
				FROM temp_road_rnd 
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
				       array[fromnodeid, tonodeid] as old_S_E_node, A.split_seq_num
				from temp_road_rlk as A 
				left join middle_tile_node as B 
				on cast ( A.fromnodeid as bigint) = B.old_node_id
				left join middle_tile_node as T
				on cast ( A.tonodeid as bigint) = T.old_node_id
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

CREATE OR REPLACE FUNCTION rdb_cnv_link_addinfo_structcode(linkattr integer) 
    RETURNS smallint
    LANGUAGE plpgsql
    AS $$
BEGIN
	RETURN CASE
		WHEN linkattr = 2 THEN 1
		WHEN linkattr = 4 THEN 3
		ELSE 0
	END;
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


CREATE OR REPLACE FUNCTION rdb_cnv_link_type(linkcls integer) RETURNS smallint
    LANGUAGE plpgsql
    AS $$
    BEGIN
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

CREATE OR REPLACE FUNCTION rdb_cnv_link_toll(roadcls integer) RETURNS smallint
    LANGUAGE plpgsql
    AS $$
    BEGIN
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

CREATE OR REPLACE FUNCTION rdb_cnv_road_function_code(navicls integer) RETURNS smallint
    LANGUAGE plpgsql
    AS $$
    BEGIN
	    RETURN CASE
	        WHEN navicls = 1 THEN 1
	        WHEN navicls = 101 THEN 1
	        WHEN navicls = 102 THEN 1
	        WHEN navicls = 103 THEN 1
	        WHEN navicls = 2 THEN 2
	        WHEN navicls = 3 THEN 2
	        WHEN navicls = 104 THEN 2
	        WHEN navicls = 105 THEN 2
	        WHEN navicls = 403 THEN 2
	        WHEN navicls = 4 THEN 3
	        WHEN navicls = 106 THEN 3
	        WHEN navicls = 5 THEN 4
	        WHEN navicls = 107 THEN 4
	        WHEN navicls = 0 THEN 5
	        WHEN navicls = 6 THEN 5
	        WHEN navicls = 108 THEN 5
	        WHEN navicls = 201 THEN 5
	        ELSE 5
	    END;
    END;
$$;

----------------------------------------------------------------------------
-- temp function type -- zhangliang
----------------------------------------------------------------------------
create or replace function zl_test_function_type(navicls int, widearea_f int)
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
	        ELSE 0
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
	        ELSE 0
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
	        WHEN oneway = 2 THEN 0
	        WHEN oneway = 3 THEN width
	        WHEN oneway = 4 THEN 0
	        WHEN oneway = 5 THEN width
	        ELSE 5
	    END;
    END;
$$;

CREATE OR REPLACE FUNCTION get_width_upflow(oneway integer, width integer) RETURNS smallint
    LANGUAGE plpgsql
    AS $$
    BEGIN
	    RETURN CASE
	        WHEN oneway = 0 THEN width
	        WHEN oneway = 1 THEN 0
	        WHEN oneway = 2 THEN width
	        WHEN oneway = 3 THEN 0
	        WHEN oneway = 4 THEN width
	        WHEN oneway = 5 THEN width
	        ELSE 5
	    END;
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
				'SELECT rdb_get_connected_nodeid_ipc(A.fromnodeid, A.tonodeid, B.fromnodeid, B.tonodeid) as nodeid
				  From (select * from road_rlk where objectid = ' || @nCurrentLinkid   || ') as A,
				       (select * from road_rlk where objectid = ' || @nConnectedLinkid || ') as B';
			
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
				'SELECT rdb_get_connected_nodeid_ipc(A.fromnodeid, A.tonodeid, B.fromnodeid, B.tonodeid) as nodeid
				  From (select * from road_rlk where objectid = ' || @nCurrentLinkid   || ') as A,
				       (select * from road_rlk where objectid = ' || @nConnectedLinkid || ') as B';
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

CREATE OR REPLACE FUNCTION rdb_make_new_link_by_attr()
  RETURNS integer
  LANGUAGE plpgsql
  AS $$ 
DECLARE
	curs1 refcursor;
	curr_objectid integer;
	curr_linkid   integer;
	curr_linkattr_c integer;
	curr_delt_fraction double precision;
	curr_geom geometry;

	next_objectid integer;
	next_linkid   integer;
	next_linkattr_c integer;
	next_delt_fraction double precision;
	next_geom geometry;
	
	seq_num integer;
	insert_flg boolean;
	merger_flg boolean;
	cnt integer;
BEGIN
	OPEN curs1 FOR	SELECT objectid, linkid, linkattr_c, (end_fraction - start_fraction) as delt_fraction, 
			       the_geom
			  FROM temp_split_link
			  order by linkid, start_fraction;

	cnt := 0;
	-- Get the first record	
	FETCH curs1 INTO curr_objectid, curr_linkid, curr_linkattr_c, curr_delt_fraction, curr_geom;
	FETCH curs1 INTO next_objectid, next_linkid, next_linkattr_c, next_delt_fraction, next_geom;
	seq_num     := 0;
	insert_flg  := TRUE;
	while curr_linkid is not NULL LOOP
		IF next_linkid is null or curr_linkid <> next_linkid THEN
			INSERT INTO temp_new_link(
					objectid, linkid, linkattr_c, "split_seq_num", the_geom)
				VALUES (curr_objectid, curr_linkid, curr_linkattr_c, seq_num, curr_geom);
			cnt                 := cnt + 1;
			seq_num             := 0;
			curr_objectid       := next_objectid;
			curr_linkid         := next_linkid;
			curr_linkattr_c     := next_linkattr_c;
			curr_delt_fraction  := next_delt_fraction;
			curr_geom           := next_geom;
		else 	
			insert_flg := true;
			merger_flg := false;
			if next_linkattr_c = curr_linkattr_c then 
				-- error
			end if;
			
			-- normal link ==> railway crossing
			--if next_linkattr_c = 4 and curr_linkattr_c = 0 and next_delt_fraction > (curr_delt_fraction * 2) then
			--	curr_geom  := ST_LineMerge(ST_union(curr_geom, next_geom));
			--	insert_flg := FALSE;
			--	merger_flg := true;
			--end if;
			
			-- railway crossing ==> normal link
			--if next_linkattr_c = 0 and curr_linkattr_c = 4 and next_delt_fraction < (curr_delt_fraction / 2) then
			--	curr_geom  := ST_LineMerge(ST_union(curr_geom, next_geom));
			--	merger_flg := true;
			--end if;
			
			-- normal link ==> tunnel
			--if next_linkattr_c = 2 and curr_linkattr_c = 0 and next_delt_fraction > (curr_delt_fraction * 20) then
			--	curr_geom  := ST_LineMerge(ST_union(curr_geom, next_geom));
			--	insert_flg := FALSE;
			--	merger_flg := true;
			--end if;
			
			-- tunnel ==> normal link
			--if next_linkattr_c = 0 and curr_linkattr_c = 2 and next_delt_fraction < (curr_delt_fraction / 20) then
			--	curr_geom  := ST_LineMerge(ST_union(curr_geom, next_geom));
			--	merger_flg := true;
			--end if;
			
			if insert_flg = TRUE then 
				INSERT INTO temp_new_link(
						    objectid, linkid, linkattr_c, "split_seq_num", the_geom)
					    VALUES (curr_objectid, curr_linkid, curr_linkattr_c, seq_num, curr_geom);
					
				cnt      := cnt + 1;
				seq_num  := seq_num + 1;
				if merger_flg = FALSE THEN 
					curr_objectid       := next_objectid;
					curr_linkattr_c     := next_linkattr_c;
					curr_delt_fraction  := next_delt_fraction;
					curr_geom           := next_geom; 
				ELSE
					FETCH curs1 INTO curr_objectid, curr_linkid, curr_linkattr_c, curr_delt_fraction, curr_geom;
					if curr_linkid <> next_linkid then 
						seq_num  := 0;						
					end if;
				END IF;
			else
				curr_objectid       := next_objectid;
				curr_linkattr_c     := next_linkattr_c;
				curr_delt_fraction  := next_delt_fraction;
			end if;	
		END IF;
		
		
		FETCH curs1 INTO next_objectid, next_linkid, next_linkattr_c, next_delt_fraction, next_geom;
	END LOOP;			           
	close curs1;
	
	return cnt;
END;
$$;

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
	OPEN curs1 FOR	SELECT objectid, linkid, linkattr_c, 
                               rdb_line_reverse(the_geom, start_fraction, end_fraction) as attr_geom,
                               rdb_min(start_fraction, end_fraction) as start_fraction,
                               rdb_Max(start_fraction, end_fraction) as end_fraction,
                               link_geom
                          FROM (
                               SELECT A.objectid, linkid, linkattr_c, A.the_geom, 
                                      mid_get_fraction(ST_LineMerge(road_rlk.the_geom), ST_StartPoint(A.the_geom)) as start_fraction,
                                      mid_get_fraction(ST_LineMerge(road_rlk.the_geom), ST_EndPoint(A.the_geom)) as end_fraction,
                                      ST_LineMerge(road_rlk.the_geom) as link_geom
                                FROM (
                                      SELECT objectid, linkid, linkattr_c, ST_LineMerge(the_geom) as the_geom
                                        FROM road_rsa
                                        -- Tunnel, Railway Crossing
                                        WHERE linkattr_c IN (2, 4) 
                                ) as A
                                LEFT JOIN road_rlk
                                ON linkid = road_rlk.objectid
                          ) AS B
                          ORDER BY linkid, start_fraction, linkattr_c;

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
		    FROM road_rnd;
	FETCH curs2 INTO curr_node_id;
	if curr_node_id is null then
		curr_node_id := 0;
	end if;
	close curs2;
	
	OPEN curs1 FOR select linkid, fromnodeid, tonodeid, roadcls_c, navicls_c, linkcls_c, 
			       manager_c, width_c, widearea_f, island_f, bypass_f, caronly_f, 
			       roadno, roadcode, lanecount, nopass_c, oneway_c, linkattr_c, 
			       split_seq_num, the_geom
			from (
			SELECT linkid, fromnodeid, tonodeid, roadcls_c, navicls_c, linkcls_c, 
			       manager_c, width_c, widearea_f, island_f, bypass_f, caronly_f, 
			       roadno, roadcode, lanecount, nopass_c, oneway_c, linkattr_c, 
			       split_seq_num, ST_SETSRID(ST_linemerge(B.the_geom), 4326) as the_geom
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
			  LEFT JOIN road_rlk
			  ON linkid = objectid
			) as C
			order by linkid, split_seq_num;
	cnt2 := 0;
	
	FETCH curs1 INTO curr_link;
	FETCH curs1 INTO temp_link;
	
	while curr_link.linkid is not NULL LOOP
		IF temp_link IS NULL OR curr_link.linkid <> temp_link.linkid then
			-- insert last link
			INSERT INTO temp_road_rlk(
					    objectid, fromnodeid, tonodeid, roadcls_c, 
					    navicls_c, linkcls_c, manager_c, width_c, 
					    widearea_f, island_f, bypass_f, caronly_f, 
					    roadno, roadcode, lanecount, nopass_c,
					    oneway_c, linkattr_c, split_seq_num, the_geom)
					values (
					    curr_link.linkid, curr_node_id, curr_link.tonodeid, curr_link.roadcls_c, 
					    curr_link.navicls_c, curr_link.linkcls_c, curr_link.manager_c, curr_link.width_c, 
					    curr_link.widearea_f, curr_link.island_f, curr_link.bypass_f, curr_link.caronly_f, 
					    curr_link.roadno, curr_link.roadcode, curr_link.lanecount, curr_link.nopass_c,
					    curr_link.oneway_c, curr_link.linkattr_c, curr_link.split_seq_num, curr_link.the_geom);
			cnt2      := cnt2 + 1;
			curr_link := temp_link;
		else 
			if curr_link.split_seq_num = 0 then
				curr_node_id := curr_node_id + 1;
				-- insert end node 
				INSERT INTO temp_road_rnd(
						objectid, signal_f, new_add, the_geom)
					values (curr_node_id, 0, true, ST_EndPoint(curr_link.the_geom));
				
				-- insert first link
				INSERT INTO temp_road_rlk(
					    objectid, fromnodeid, tonodeid, roadcls_c, 
					    navicls_c, linkcls_c, manager_c, width_c, 
					    widearea_f, island_f, bypass_f, caronly_f, 
					    roadno, roadcode, lanecount, nopass_c,
					    oneway_c, linkattr_c, split_seq_num, the_geom)
					values (
					    curr_link.linkid, curr_link.fromnodeid, curr_node_id, curr_link.roadcls_c, 
					    curr_link.navicls_c, curr_link.linkcls_c, curr_link.manager_c, curr_link.width_c, 
					    curr_link.widearea_f, curr_link.island_f, curr_link.bypass_f, curr_link.caronly_f, 
					    curr_link.roadno, curr_link.roadcode, curr_link.lanecount, curr_link.nopass_c,
					    curr_link.oneway_c, curr_link.linkattr_c, curr_link.split_seq_num, curr_link.the_geom);
				cnt2 := cnt2 + 1;
			else 
				curr_node_id := curr_node_id + 1;
				-- insert end node 
				INSERT INTO temp_road_rnd(
						objectid, signal_f, new_add, the_geom)
					values (curr_node_id, 0, true, ST_EndPoint(curr_link.the_geom));
				
				-- insert link
				INSERT INTO temp_road_rlk(
					    objectid, fromnodeid, tonodeid, roadcls_c, 
					    navicls_c, linkcls_c, manager_c, width_c, 
					    widearea_f, island_f, bypass_f, caronly_f, 
					    roadno, roadcode, lanecount, nopass_c,
					    oneway_c, linkattr_c, split_seq_num, the_geom)
					values (
					    curr_link.linkid, curr_node_id - 1, curr_node_id, curr_link.roadcls_c, 
					    curr_link.navicls_c, curr_link.linkcls_c, curr_link.manager_c, curr_link.width_c, 
					    curr_link.widearea_f, curr_link.island_f, curr_link.bypass_f, curr_link.caronly_f, 
					    curr_link.roadno, curr_link.roadcode, curr_link.lanecount, curr_link.nopass_c,
					    curr_link.oneway_c, curr_link.linkattr_c, curr_link.split_seq_num, curr_link.the_geom);
					    
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

CREATE OR REPLACE FUNCTION rdb_insert_line_name_dictionary()
    RETURNS integer
    LANGUAGE plpgsql
    AS $$
DECLARE
    curs1 refcursor;
    curr_name_id integer;
    ch_name_record RECORD;
    py_name_record RECORD;
    en_name_record RECORD;
    target RECORD;
    curr_route_id bigint;
    start_name_id integer;
BEGIN
    -- get the last name id
    curr_name_id := 0;
    
    SELECT INTO ch_name_record name_id  FROM rdb_name_dictionary_jp order by name_id desc limit 1;
    IF FOUND then
        curr_name_id := ch_name_record.name_id;
    END IF;    
    SELECT INTO py_name_record name_id  FROM rdb_name_dictionary_yomi order by name_id desc limit 1;
    IF FOUND THEN
        if curr_name_id < py_name_record.name_id THEN
            curr_name_id := py_name_record.name_id;
        END IF;
    END IF;
    
    start_name_id := curr_name_id;
        
    OPEN curs1 FOR  SELECT roadcode, roadname, roadyomi FROM road_roadcode;

    -- Get the first record        
    FETCH curs1 INTO target;
    while target.roadcode is not NULL LOOP
        curr_route_id := target.roadcode;
        curr_name_id  := curr_name_id + 1;
        
        -- insert jp name
        IF target.roadname is not null and target.roadname <> '' THEN
            INSERT INTO rdb_name_dictionary_jp(name_id, "name")
            VALUES (curr_name_id, target.roadname);
        END IF;
        
        -- insert yomi name
        IF target.roadyomi is not null and target.roadyomi <> '' THEN
            INSERT INTO rdb_name_dictionary_yomi(name_id, "name")
            VALUES (curr_name_id, target.roadyomi);
        END IF;

        INSERT INTO temp_routeid_nameid(route_id, name_id)
                   VALUES (curr_route_id, curr_name_id);
        
        -- Get the next record
        FETCH curs1 INTO target;
    END LOOP;
    close curs1;

    return curr_name_id - start_name_id + 1;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_cnv_lane_passable_ipc(applane integer, passable character varying)
  RETURNS smallint 
  LANGUAGE plpgsql
  AS $$
DECLARE
	lane smallint;
	i integer;
BEGIN
	--return (passable::bit(16) >> (16-applane))::integer::smallint 
	if applane is null or passable is null THEN
		return 0;
	end if;
	
	if applane > 16 THEN
		return 0;
	end if;
		
	lane := 0;
	i    := 1;
	while i <= applane LOOP
		if substring(passable, i, 1) = '1' THEN
			lane := lane | (1 << (applane - i));
		END IF;
		i := i + 1;
	END LOOP;
            
	return lane;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_cnv_lane_arrow_ipc(lanedir integer)
  RETURNS smallint 
  LANGUAGE plpgsql
  AS $$
BEGIN
        RETURN CASE
            WHEN lanedir = 0 THEN NULL   -- doesn't investigate
            WHEN lanedir = 1 THEN 1   -- 0 degree
            WHEN lanedir = 2 THEN 2   -- 45 (3)
            WHEN lanedir = 3 THEN 4   -- 90
            WHEN lanedir = 4 THEN 5   -- 135(6)
            WHEN lanedir = 5 THEN 7   -- 180
            WHEN lanedir = 6 THEN 8   -- 225(9)
            WHEN lanedir = 7 THEN 10  -- 270
            WHEN lanedir = 8 THEN 11  -- 315(12)
            ELSE NULL
        END;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_get_max_name_id_ipc()
  RETURNS integer
  LANGUAGE plpgsql
  AS $$
DECLARE
	max_name_id integer;
	curr_name_id integer;
	temp_table_name character varying;
BEGIN
	perform tablename FROM pg_tables  where tablename = 'rdb_name_dictionary_jp';
        IF not FOUND then  
		RETURN NULL;
        END IF;
	
	perform tablename FROM pg_tables where tablename = 'rdb_name_dictionary_yomi';
        IF NOT FOUND then  
		RETURN NULL;
        END IF;
	
	-- get the last name id
	curr_name_id := 0;
	max_name_id  := 0;                            
	SELECT max(name_id) into curr_name_id FROM rdb_name_dictionary_jp;
	SELECT max(name_id) into max_name_id FROM rdb_name_dictionary_yomi;

	IF max_name_id < curr_name_id THEN
		max_name_id := curr_name_id;
	end if;

	if max_name_id is null then
		max_name_id := 0;
	end if;

	return max_name_id;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_make_cross_name_ipc()
  RETURNS integer 
  LANGUAGE plpgsql
  AS $$
DECLARE
	curs1 refcursor;
	curr_name_id integer;
	max_name_id integer;
	node_record record;
BEGIN
	-- get the last name id
	max_name_id := rdb_get_max_name_id_ipc();
	if max_name_id is null then
		return null;
	end if;

	curr_name_id := max_name_id;
	
	OPEN curs1 FOR	SELECT tile_node_id, tile_id, name_kanji, name_yomi
					FROM(
					 SELECT objectid, name_kanji, name_yomi
					   FROM road_rnd
					   where name_kanji <> '' or name_yomi <> ''
					 ) AS A
					 LEFT JOIN middle_tile_node
					 ON objectid = middle_tile_node.old_node_id;

	FETCH curs1 INTO node_record;	  
	while node_record.tile_node_id is not NULL LOOP
		if (node_record.name_kanji is not null) or  (node_record.name_yomi is not null) then
			curr_name_id := curr_name_id + 1;
			INSERT INTO rdb_guideinfo_crossname(
				    node_id, node_id_t, name_id)
			    VALUES (node_record.tile_node_id, node_record.tile_id, curr_name_id);

		end if;
		
		if node_record.name_kanji is not null then
			INSERT INTO rdb_name_dictionary_jp(
				    "name_id", "name")
			    VALUES (curr_name_id, node_record.name_kanji);
		end if;
		
		if node_record.name_yomi is not null then
			INSERT INTO rdb_name_dictionary_yomi(
				    "name_id", "name")
			    VALUES (curr_name_id, node_record.name_yomi);
		end if;
		
		FETCH curs1 INTO node_record;
	END LOOP;
	close curs1;

	return curr_name_id - max_name_id;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_convert_temp_regulation_trn()
  RETURNS integer
  LANGUAGE plpgsql
  AS $$ 
DECLARE
	curs1 refcursor;
	rec1 record;
	nFromlinkid integer;
	nTolinkid integer;
	nMidcount integer;
	midlinkids character varying;
	linkid_array integer[];
BEGIN

	OPEN curs1 FOR select * from road_trn;
	
	-- Get the first record	
	FETCH curs1 INTO rec1;
	while rec1.gid is not NULL LOOP
		nFromlinkid 	:= rec1.fromlinkid;
		nTolinkid 		:= rec1.tolinkid;
		nMidcount		:= rec1.midcount;
		midlinkids		:= rec1.midlinkid;
		
		-- old link id array
		linkid_array := ARRAY[nFromlinkid];
		IF nMidcount > 0 THEN
			linkid_array := linkid_array || cast(regexp_split_to_array(midlinkids, E'\\,+') as integer[]);
		END IF;
		linkid_array[nMidcount + 2] := nTolinkid;
		
		-- insert record into temp_regulation
		INSERT INTO temp_regulation
			(
			 "link_array",
			 "from_month",
			 "from_day",
			 "to_month",
			 "to_day",
			 "from_hour",
			 "from_min",
			 "to_hour",
			 "to_min",
			 "dayofweek"
			)
		VALUES
			(
			 linkid_array,
			 rec1.from_month,
			 rec1.from_day,
			 rec1.to_month,
			 rec1.to_day,
			 rec1.from_hour,
			 rec1.from_min,
			 rec1.to_hour,
			 rec1.to_min,
			 rec1.dayofweek
			);
		
		-- Get the next record
		FETCH curs1 INTO rec1;
	END LOOP;
	
	close curs1;
	return 1;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_convert_temp_regulation_npc()
  RETURNS integer
  LANGUAGE plpgsql
  AS $$ 
DECLARE
	curs1 refcursor;
	rec1 record;
	nCurrentlinkid integer;
	linkid_array integer[];
BEGIN

	OPEN curs1 FOR select * from road_npc;
	
	-- Get the first record	
	FETCH curs1 INTO rec1;
	while rec1.gid is not NULL LOOP
		nCurrentlinkid 	:= rec1.linkid;

		-- old link id array
		linkid_array := ARRAY[nCurrentlinkid];
		
		-- insert record into temp_regulation
		INSERT INTO temp_regulation
			(
			 "link_array",
			 "from_month",
			 "from_day",
			 "to_month",
			 "to_day",
			 "from_hour",
			 "from_min",
			 "to_hour",
			 "to_min",
			 "dayofweek"
			)
		VALUES
			(
			 linkid_array,
			 rec1.from_month,
			 rec1.from_day,
			 rec1.to_month,
			 rec1.to_day,
			 rec1.from_hour,
			 rec1.from_min,
			 rec1.to_hour,
			 rec1.to_min,
			 rec1.dayofweek
			);
		
		-- Get the next record
		FETCH curs1 INTO rec1;
	END LOOP;
	
	close curs1;
	return 1;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_convert_temp_regulation_owc()
  RETURNS integer
  LANGUAGE plpgsql
  AS $$ 
DECLARE
	curs1			refcursor;
	rec1			record;
	nCurrentlinkid	integer;
	nLinkdir		integer;
	linkrow_array	integer[];
	linkid_array	integer[];
	nCount			integer;
	nIndex			integer;
BEGIN

	OPEN curs1 FOR select * from road_owc;
	
	-- Get the first record	
	FETCH curs1 INTO rec1;
	while rec1.gid is not NULL LOOP
		nCurrentlinkid 	:= rec1.linkid;
		nLinkdir		:= rec1.linkdir;
		
		-- for mult linkrow
		linkrow_array	:= getLinkrowArrayForOwcLink(nCurrentlinkid, nLinkdir);
		nCount			:= array_upper(linkrow_array,1);
		nIndex			:= 1;
		
		while nIndex <= nCount LOOP
			linkid_array	:= linkrow_array[nIndex : nIndex + 1];
			
			-- insert record into temp_regulation
			INSERT INTO temp_regulation
				(
				 "link_array",
				 "from_month",
				 "from_day",
				 "to_month",
				 "to_day",
				 "from_hour",
				 "from_min",
				 "to_hour",
				 "to_min",
				 "dayofweek"
				)
			VALUES
				(
				 linkid_array,
				 rec1.from_month,
				 rec1.from_day,
				 rec1.to_month,
				 rec1.to_day,
				 rec1.from_hour,
				 rec1.from_min,
				 rec1.to_hour,
				 rec1.to_min,
				 rec1.dayofweek
				);
			
			nIndex := nIndex + 2;
		END LOOP;
		
		-- Get the next record
		FETCH curs1 INTO rec1;
	END LOOP;
	
	close curs1;
	return 1;
END;
$$;

CREATE OR REPLACE FUNCTION rdb_convert_temp_link_regulation()
  RETURNS integer
  LANGUAGE plpgsql
  AS $$ 
DECLARE
	curs1				refcursor;
	rec1				record;
	curs2				refcursor;
	rec2				record;
	nTempRegulationid	integer;
	nRegulationid		integer;
	linkid_array		integer[];		-- old link array
	arrayTargetLinkid	bigint[];		-- new link array
	
	nIndex integer;
	nCount integer;
	nLinkid integer;
	nConnectedLinkid integer;
	nTargetLinkid bigint;
BEGIN
		
	OPEN curs1 FOR execute
		'
		 select (array_agg(temp_regulation_id))[1] as temp_regulation_id,
		 		link_array,
				from_month, from_day, to_month, to_day, from_hour, from_min, to_hour, to_min, dayofweek
		 from temp_regulation
		 group by link_array, from_month, from_day, to_month, to_day, from_hour, from_min, to_hour, to_min, dayofweek
		 order by temp_regulation_id
		'
		;
	
	nRegulationid := 0;
	
	-- Get the first record
	FETCH curs1 INTO rec1;
	while rec1.temp_regulation_id is not NULL LOOP
		nTempRegulationid 	:= rec1.temp_regulation_id;
		linkid_array 		:= rec1.link_array;
		
		-- convert old links to new links
		nCount := array_upper(linkid_array, 1);
		IF nCount = 1 THEN
			-- for npc
			open curs2 FOR execute
				'
				 select tile_link_id
				 from middle_tile_link
				 where old_link_id = ' || @linkid_array[1] || '
				 order by split_seq_num
				'
				;
			
			fetch curs2 into rec2;
			while rec2.tile_link_id is not null LOOP
				nRegulationid := nRegulationid + 1;
				INSERT INTO temp_link_regulation("regulation_id", "temp_regulation_id", "link_id")
						VALUES (nRegulationid, nTempRegulationid, rec2.tile_link_id);
				fetch curs2 into rec2;
			END LOOP;
			
			close curs2;
			
		ELSE
			-- for trn and extend_owc
			nRegulationid := nRegulationid + 1;
			
			nIndex := 1;
			while nIndex <= nCount LOOP
				if nIndex = 1 then
					nLinkid				:= linkid_array[nIndex];
					nConnectedLinkid	:= linkid_array[nIndex + 1];
					nTargetLinkid		:= getSplitLinkID(nLinkid, nConnectedLinkid, True);
					arrayTargetLinkid	:= ARRAY[nTargetLinkid];
				elseif nIndex < nCount then
					nLinkid				:= linkid_array[nIndex];
					nConnectedLinkid	:= linkid_array[nIndex + 1];
					arrayTargetLinkid	:= arrayTargetLinkid || getSplitLinkIDArray(nLinkid, nConnectedLinkid, True);
				else
					nLinkid				:= linkid_array[nIndex];
					nConnectedLinkid	:= linkid_array[nIndex - 1];
					nTargetLinkid		:= getSplitLinkID(nLinkid, nConnectedLinkid, True);
					arrayTargetLinkid	:= arrayTargetLinkid || nTargetLinkid;
				end if;
				nIndex := nIndex + 1;
			END LOOP;
		
			-- insert all links
			nIndex := 1;
			nCount := array_upper(arrayTargetLinkid,1);
			while nIndex <= nCount LOOP
				nTargetLinkid := arrayTargetLinkid[nIndex];
				INSERT INTO temp_link_regulation("regulation_id", "temp_regulation_id", "link_id")
						VALUES (nRegulationid, nTempRegulationid, nTargetLinkid);
				nIndex := nIndex + 1;
			END LOOP;
		END IF;
		
		-- Get the next record
		FETCH curs1 INTO rec1;
	END LOOP;
	
	close curs1;
	return 1;
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
	
	OPEN curs1 FOR	SELECT roadcode, roadname, roadyomi from road_roadcode;
					

	FETCH curs1 INTO rec;	  
	while rec.roadcode is not NULL LOOP
		curr_name_id := curr_name_id + 1;
		
		if (rec.roadname is not null) then
			INSERT INTO temp_link_name_dictionary_tbl(
				    name_id, name, py, seq_nm, language, roadcode)
			    VALUES (curr_name_id, rec.roadname, rec.roadyomi, 1, 5, rec.roadcode);

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

CREATE OR REPLACE FUNCTION rdb_make_node_name()
  RETURNS integer 
  LANGUAGE plpgsql  
  AS
$BODY$
DECLARE
	curs1 refcursor;
	curr_name_id integer;
	max_name_id integer;
	rec record;
	old_name character varying;
	old_yomi character varying;
BEGIN

	select case when max(id) is null then 0 else max(id) end into max_name_id from 
	(
	select max(name_id) as id from name_dictionary_tbl
	) as T;
	curr_name_id := max_name_id;
	
	OPEN curs1 FOR	SELECT objectid, name_kanji, name_yomi from temp_road_rnd
		where not (name_kanji is null and name_yomi is null)
		order by name_kanji, name_yomi;
					

	old_name := '';
	old_yomi := '';
	FETCH curs1 INTO rec;	  
	while rec.objectid is not NULL LOOP
		if rec.name_kanji != old_name or rec.name_yomi != old_yomi then
			curr_name_id := curr_name_id + 1;
			old_name := rec.name_kanji;
			old_yomi := rec.name_yomi;
		end if;
		

		INSERT INTO temp_node_name_dictionary_tbl(
			    name_id, name, py, seq_nm, language, objectid)
		    VALUES (curr_name_id, rec.name_kanji, rec.name_yomi, 1, 5, rec.objectid);

		
		FETCH curs1 INTO rec;
	END LOOP;
	close curs1;

	insert into name_dictionary_tbl (name_id, name, py, seq_nm, language)
		select distinct name_id, name, py, seq_nm, language from temp_node_name_dictionary_tbl;

	return curr_name_id - max_name_id;
END;
$BODY$;

CREATE OR REPLACE FUNCTION rdb_cnv_oneway_code(oneway_c integer)
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
------------------------------------------------------------------------
-- convert One Way code---hcz
------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION rdb_cnv_oneway_code_hcz(oneway_c integer, nopass_c integer)
  RETURNS smallint 
  LANGUAGE plpgsql
  AS
$$
BEGIN
	    RETURN CASE
	    	when nopass_c = 1 then 4
	    	when nopass_c = 3 then 4
	        WHEN oneway_c = 0 THEN 1
	        WHEN oneway_c = 1 THEN 2
	        WHEN oneway_c = 2 THEN 3
	        WHEN oneway_c = 3 THEN 2
	        WHEN oneway_c = 4 THEN 3
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
            WHEN roadcls = 6 THEN 0  --21
            WHEN roadcls = 7 THEN 0  --21
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
            ELSE 0
        END;
    END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
ALTER FUNCTION rdb_cnv_display_class(roadcls integer) OWNER TO postgres;




-------------------------------------------------------------------------------------------------------------
-- Dictionary
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
$$;

CREATE OR REPLACE FUNCTION rdb_insert_poi_name_dictionary(layer_name character varying)
    RETURNS integer
    LANGUAGE plpgsql
    AS $$ 
DECLARE
    curs1 refcursor;
    curr_name_id integer;
    target RECORD;
    inner_name_id integer;
    curr_objectid integer;
BEGIN
	curr_name_id := 0;

	OPEN curs1 FOR execute 'SELECT * from ' || ($1) || '_ant';
    
	-- Get the first record        
	FETCH curs1 INTO target;
	while target.objectid is not NULL LOOP
		curr_objectid     := target.objectid;
		
		-- insert namestr1
		IF target.namestr1 is not null and target.namestr1 <> '' THEN
			-- rdb_add_one_name(n_id, _name, yomi, seq_nm, lang_type)
			curr_name_id := rdb_add_one_name(curr_name_id, target.namestr1, NULL, 5);
			
			INSERT INTO temp_poi_name("layer_name", "objectid", "seq_nm", "name_id")
				   VALUES ($1, curr_objectid, 1, curr_name_id);
				   
			curr_name_id    := curr_name_id + 1;
		END IF;
		
		-- insert namestr2
		IF target.namestr2 is not null and target.namestr2 <> '' THEN
			curr_name_id := rdb_add_one_name(curr_name_id, target.namestr2, NULL, 5);
			
			INSERT INTO temp_poi_name("layer_name", "objectid", "seq_nm", "name_id")
				   VALUES ($1, curr_objectid, 2, curr_name_id);
			curr_name_id    := curr_name_id + 1;
		END IF;
		
		-- insert namestr3
		IF layer_name != 'citymap' and target.namestr3 is not null and target.namestr3 <> '' THEN
			curr_name_id := rdb_add_one_name(curr_name_id, target.namestr3, NULL, 5);
			
			INSERT INTO temp_poi_name("layer_name", "objectid", "seq_nm", "name_id")
				   VALUES ($1, curr_objectid, 3, curr_name_id);
			curr_name_id    := curr_name_id + 1;
		END IF;
		
		-- Get the next record
		FETCH curs1 INTO target;
	END LOOP;
	close curs1;

    return 1;
END;
$$;

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
        select from_month, from_day, from_hour, from_min, to_month, to_day, to_hour, to_min, dayofweek
        from road_trn
        union 
        select from_month, from_day, from_hour, from_min, to_month, to_day, to_hour, to_min, dayofweek
        from road_owc
        union
        select from_month, from_day, from_hour, from_min, to_month, to_day, to_hour, to_min, dayofweek
        from road_npc
        )as t
        where t.from_month != -1
        group by from_month, from_day, from_hour, from_min, to_month, to_day, to_hour, to_min, dayofweek
        order by from_month, from_day, from_hour, from_min, to_month, to_day, to_hour, to_min, dayofweek
    LOOP
    	dayofweek_array	:= mid_convert_weekflag(rec.dayofweek);
    	dayofweek_count	:= array_upper(dayofweek_array, 1);
    	for dayofweek_index in 1..dayofweek_count loop
    		
	    	cur_cond_id := cur_cond_id + 1;
	    	
	    	insert into temp_condition_regulation_tbl
	    				(
	    				"cond_id", "dayofweek", 
						"from_month", "from_day", "from_hour", "from_min", 
						"to_month", "to_day", "to_hour", "to_min"
						)
	    		VALUES	(
	    				cur_cond_id, rec.dayofweek, 
	    				rec.from_month, rec.from_day, rec.from_hour, rec.from_min, 
	    				rec.to_month, rec.to_day, rec.to_hour, rec.to_min
	    				);
	    	
	    	insert into condition_regulation_tbl
	    				(
	    				"cond_id", "car_type", "day_of_week",
						"start_year", "start_month", "start_day", "start_hour", "start_minute",
						"end_year", "end_month", "end_day", "end_hour", "end_minute", "exclude_date"
						)
	    		VALUES	(
	    				cur_cond_id, -1, dayofweek_array[dayofweek_index],
	    				0, rec.from_month, rec.from_day, rec.from_hour, rec.from_min,
	    				0, rec.to_month, rec.to_day, rec.to_hour, rec.to_min, 0
	    				);
	    end loop;
    END LOOP;
    return 1;
END;
$$;

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


CREATE OR REPLACE FUNCTION mid_convert_regulation_from_trn()
    RETURNS smallint
    LANGUAGE plpgsql
AS $$
DECLARE
	rec record;
	cur_regulation_id integer;
	
	old_linkid_array integer[];
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
    
    	select fromlinkid, tolinkid, midcount, midlinkid, array_agg(cond_id) as array_condid
    	from
    	(
	    	select fromlinkid, tolinkid, midcount, midlinkid, cn.cond_id
	    	from road_trn as r
	    	left join temp_condition_regulation_tbl as cn
	    	on	r.from_month = cn.from_month and 
	    		r.from_day = cn.from_day and
	    		r.from_hour = cn.from_hour and
	    		r.from_min = cn.from_min and
	    		r.to_month = cn.to_month and
	    		r.to_day = cn.to_day and
	    		r.to_hour = cn.to_hour and
	    		r.to_min = cn.to_min and
	    		r.dayofweek = cn.dayofweek
	    	order by fromlinkid, tolinkid, midcount, midlinkid, cn.cond_id
    	)as t
    	group by fromlinkid, tolinkid, midcount, midlinkid
    	order by fromlinkid, tolinkid, midcount, midlinkid
    	
    LOOP
		-- old link id array
		old_linkid_array := ARRAY[rec.fromlinkid];
		IF rec.midcount > 0 THEN
			old_linkid_array := old_linkid_array || cast(regexp_split_to_array(rec.midlinkid, E'\\,+') as integer[]);
		END IF;
		old_linkid_array[rec.midcount + 2] := rec.tolinkid;
		
		-- new link id array
		nCount := array_upper(old_linkid_array, 1);
		nIndex := 1;
		while nIndex <= nCount LOOP
			if nIndex = 1 then
				nLinkid				:= old_linkid_array[nIndex];
				nConnectedLinkid	:= old_linkid_array[nIndex + 1];
				nTargetLinkid		:= mid_getSplitLinkID(nLinkid, nConnectedLinkid, True);
				linkid_array		:= ARRAY[nTargetLinkid];
			elseif nIndex < nCount then
				nLinkid				:= old_linkid_array[nIndex];
				nConnectedLinkid	:= old_linkid_array[nIndex + 1];
				linkid_array		:= linkid_array || mid_getSplitLinkIDArray(nLinkid, nConnectedLinkid, True);
			else
				nLinkid				:= old_linkid_array[nIndex];
				nConnectedLinkid	:= old_linkid_array[nIndex - 1];
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
			nIndex := nIndex + 1;
		END LOOP;    	
    	
    END LOOP;
    return 1;
END;
$$;

CREATE OR REPLACE FUNCTION mid_getSplitLinkID(nCurrentLinkid integer, nConnectedLinkid integer, bConnectedFlag boolean)
-- input org link id, get rdb link id
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
		'select * from middle_tile_link where old_link_id = ' || (@nCurrentLinkid) || ' order by split_seq_num asc';
	
	fetch curs1 into rec1;
	if rec1.tile_link_id is not null then
		if rec1.split_seq_num = -1 then
			nTargetLinkid := rec1.tile_link_id;
		else
			-- get connected nodeid
			nConnectedNodeid := null;
			open curs2 for execute
				'SELECT rdb_get_connected_nodeid_ipc(A.fromnodeid, A.tonodeid, B.fromnodeid, B.tonodeid) as nodeid
				  From (select * from road_rlk where objectid = ' || @nCurrentLinkid   || ') as A,
				       (select * from road_rlk where objectid = ' || @nConnectedLinkid || ') as B';
			
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
				'SELECT rdb_get_connected_nodeid_ipc(A.fromnodeid, A.tonodeid, B.fromnodeid, B.tonodeid) as nodeid
				  From (select * from road_rlk where objectid = ' || @nCurrentLinkid   || ') as A,
				       (select * from road_rlk where objectid = ' || @nConnectedLinkid || ') as B';
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
    
    	select linkid, array_agg(linkdir) array_linkdir, array_agg(cond_id) as array_condid
    	from
    	(
	    	select linkid, linkdir, cn.cond_id
	    	from road_owc as r
	    	left join temp_condition_regulation_tbl as cn
	    	on	r.from_month = cn.from_month and 
	    		r.from_day = cn.from_day and
	    		r.from_hour = cn.from_hour and
	    		r.from_min = cn.from_min and
	    		r.to_month = cn.to_month and
	    		r.to_day = cn.to_day and
	    		r.to_hour = cn.to_hour and
	    		r.to_min = cn.to_min and
	    		r.dayofweek = cn.dayofweek
	    	order by linkid, linkdir, cn.cond_id
    	)as t
    	group by linkid
    	order by linkid
    
    LOOP
		-- for every sublink
		FOR rec2 IN execute 
			'select tile_link_id from middle_tile_link where old_link_id = ' || (@rec.linkid) || ' order by split_seq_num asc'
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
	    		insert into regulation_relation_tbl
	    					("regulation_id", "nodeid", "inlinkid", "outlinkid", "condtype", "cond_id")
	    			VALUES 	(cur_regulation_id, null, rec2.tile_link_id, null, cast(rec.array_linkdir[nIndex]+1 as smallint), rec.array_condid[nIndex]);
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
    
    	select linkid, array_agg(cond_id) as array_condid
    	from
    	(
	    	select linkid, cn.cond_id
	    	from road_npc as r
	    	left join temp_condition_regulation_tbl as cn
	    	on	r.from_month = cn.from_month and 
	    		r.from_day = cn.from_day and
	    		r.from_hour = cn.from_hour and
	    		r.from_min = cn.from_min and
	    		r.to_month = cn.to_month and
	    		r.to_day = cn.to_day and
	    		r.to_hour = cn.to_hour and
	    		r.to_min = cn.to_min and
	    		r.dayofweek = cn.dayofweek
	    	order by linkid, cn.cond_id
    	)as t
    	group by linkid
    	order by linkid
    
    LOOP
		-- for every sublink
		FOR rec2 IN execute 
			'select tile_link_id from middle_tile_link where old_link_id = ' || (@rec.linkid) || ' order by split_seq_num asc'
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
				guideclass,
				n.name_id,
				boardptn_c,
				board_seq
		from (
			-- Name 
			SELECT gid, objectid, fromlinkid, tolinkid, midcount, midlinkid, guideclass, 
			       name_kanji, name_yomi, boardptn_c, board_seq
			  from road_dir
			  where boardptn_c <> 0 and mid_isNotAllDigit(name_kanji) = true

			Union

			-- Route Number
			SELECT gid, objectid, fromlinkid, tolinkid, midcount, midlinkid, guideclass, 
			       name_kanji, name_yomi, boardptn_c, board_seq
			  from road_dir
			  where boardptn_c <> 0 and mid_isNotAllDigit(name_kanji) = false

			Union

			-- NUll
			SELECT gid, objectid, fromlinkid, tolinkid, midcount, midlinkid, guideclass, 
			       name_kanji, name_yomi, boardptn_c, board_seq
			  from road_dir
			  where boardptn_c <> 0 and mid_isNotAllDigit(name_kanji) is null
		) as d
		left join temp_toward_name as n
		on d.gid = n.gid
		order by d.gid
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
		
		INSERT INTO temp_signpost_tbl
			("id", 
			 "nodeid", "inlinkid", "outlinkid", 
			 "passlid", "passlink_cnt",
			 "direction", "guideattr", "namekind", "guideclass",
			 "name_id", "patternno", "arrowno"
			 )
			VALUES
			(
			rec.gid,
			nodeid, inlinkid, outlinkid,
			midlinkids, midlinkcount,
			0, 0, 0, rec.guideclass,
			rec.name_id, cast(rec.boardptn_c as varchar), cast(rec.board_seq as varchar)
			);
	END LOOP;
	return 1;
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
BEGIN
	for rec in 
		select	a.gid,
				a.objectid,
				a.fromlinkid,
			    a.tolinkid,
			    a.midcount,
			    a.midlinkid,
			    (case when a.guidetype = 0 then 255 else a.guidetype end) as guidetype,
			    (case when b.position_type is null then 0 else b.position_type end) as position_type
		from
		(
			select	a.gid, a.objectid, fromlinkid, tolinkid, midcount, midlinkid, guidetype,
					rdb_get_connected_nodeid_ipc(b.fromnodeid, b.tonodeid, c.fromnodeid, c.tonodeid) as node_id
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
			left join road_rlk as b
			on a.fromlinkid = b.objectid
			left join road_rlk as c
			on a.nextlinkid = c.objectid
		)as a
		left join 
		(
			select	node_id, (array_agg(position_type))[1] as position_type
			from
			(
				select	distinct node_id, position_type
				from
				(
					select 	node_id, 
							(
							case 
							when pos_code in (4,6) then 1		-- sa/pa entry
							when pos_code = 2 then 5			-- ic exit
							when pos_code = 8 then 6			-- jct exit
							when pos_code in (1,7) then 7		-- ic/jct entry
							end
							)as position_type
					from road_hwynode
					where pos_code in (4,6,2,8,1,7)
				)as a
			)as b
			group by node_id having count(*) = 1
		)as b
		on a.node_id = b.node_id
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
		
		INSERT INTO force_guide_tbl
			("force_guide_id", 
			 "nodeid", "inlinkid", "outlinkid", 
			 "passlid", "passlink_cnt", "guide_type", "position_type"
			 )
			VALUES
			(
			rec.objectid,
			nodeid, inlinkid, outlinkid,
			midlinkids, midlinkcount, rec.guidetype, rec.position_type
			);
	END LOOP;
	return 1;
END;
$$;

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
			from road_rlk
			where (fromnodeid = array_old_node[1] and rdb_cnv_oneway_code(oneway_c) in (1,3))
			   or (tonodeid = array_old_node[1] and rdb_cnv_oneway_code(oneway_c) in (1,2))
			into oldfromlinkid;
		else
			select objectid
			from road_rlk
			where (fromnodeid = array_old_node[1] or tonodeid = array_old_node[1])
			  and (fromnodeid = array_old_node[2] or tonodeid = array_old_node[2])
			into oldfromlinkid;
		end if;
		
		-- oldtolink
		select objectid
		from road_rlk
		where (fromnodeid = array_old_node[nCount-1] or tonodeid = array_old_node[nCount-1])
		  and (fromnodeid = array_old_node[nCount] or tonodeid = array_old_node[nCount])
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
			from road_rlk
			where (fromnodeid = array_old_node[nIndex] or tonodeid = array_old_node[nIndex])
			  and (fromnodeid = array_old_node[nIndex+1] or tonodeid = array_old_node[nIndex+1])
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
	    left join temp_road_rlk as b
	    on (b.fromnodeid = a.nodeid and b.oneway_c in (2,4,5)) or (b.tonodeid = a.nodeid and b.oneway_c in (1,3,5))
	    left join middle_tile_link as c
	    on b.objectid = c.old_link_id and b.split_seq_num = c.split_seq_num
	    left join temp_road_rlk as d
	    on (d.fromnodeid = a.nodeid and d.oneway_c in (1,3,5)) or (d.tonodeid = a.nodeid and d.oneway_c in (2,4,5))
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

-----------------------------------------------------------------------------------------
-- check Highway Node type
-----------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_check_hwy_node_type(nNodeID bigint, nCheckType integer[])
	RETURNS boolean
	LANGUAGE plpgsql
AS $$ 
DECLARE
BEGIN
	if nNodeID is null or nCheckType is null then
		return false;
	end if;
	
	PERFORM  node_id, pos_code
	  FROM road_hwynode
	  where node_id = nNodeID and pos_code = Any(nCheckType);

	if not found then
		return False;
	end if;

	return true;
END;
$$;

-----------------------------------------------------------------------------------------
-- find path for JCT or IC
-----------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_findPath_HWY(nFormNodeID bigint, nToNodeType integer)
	RETURNS varchar[]
	LANGUAGE plpgsql
AS $$ 
DECLARE
	rstPathArray		varchar[];
	rstPathCount		integer;
	rstPath				varchar;
	
	tmpPathArray		varchar[];
	tmpLastNodeArray	bigint[];
	tmpPathCount		integer;
	tmpPathIndex		integer;
	tmplastLinkArray	bigint[];
	
	nStartNode		integer;
	nEndNode		integer;
	
	nNewFromLink	bigint;
	nNewToLink		bigint;
	
	rec				record;
BEGIN
	if nToNodeType is null then 
		return null;
	end if;
	
	-- init
	nStartNode = nFormNodeID;
	 
	SELECT objectid as link_id into nNewFromLink
	   from road_rlk
		where (fromnodeid = nFormNodeID and rdb_cnv_oneway_code(oneway_c) in (1, 3)) 
		 or (tonodeid = nFormNodeID and rdb_cnv_oneway_code(oneway_c) in (1, 2))
	;
	
	rstPathCount		:= 0;
	tmpPathArray		:= ARRAY[cast(nNewFromLink as varchar)];
	tmpLastNodeArray	:= ARRAY[nStartNode];
	tmplastLinkArray    := ARRAY[cast(nNewFromLink as varchar)];
	tmpPathCount		:= 1;
	tmpPathIndex		:= 1;

	-- search
	WHILE tmpPathIndex <= tmpPathCount LOOP
		--raise INFO '%', cast(tmpPathArray as varchar);
		if (tmpPathIndex > 1 
		   and 2 = nToNodeType
		   and mid_check_hwy_node_type(tmpLastNodeArray[tmpPathIndex], array[2])) then 
			tmpPathIndex := tmpPathIndex + 1;
			continue;
		end if;
		
		if (2 = nToNodeType and mid_check_hwy_node_type(tmpLastNodeArray[tmpPathIndex], array[7])) then
			-- there are Ramp Link in the JCT Node
			if mid_has_ramp_link(tmpLastNodeArray[tmpPathIndex], tmplastLinkArray[tmpPathIndex]) then
				for rec in execute
					'
					SELECT nextroad, dir, nextnode 
					from (
						select objectid as nextroad, ''(2)'' as dir, tonodeid as nextnode
						  from road_rlk
						  where	(fromnodeid = ' || tmpLastNodeArray[tmpPathIndex]|| ' ) 
						       and (rdb_cnv_oneway_code(oneway_c) in (1,2)) 
						       and rdb_cnv_link_type(linkcls_c) = 5 
						       and objectid <> ' || tmplastLinkArray[tmpPathIndex]|| '
						union
						
						select objectid as nextroad, ''(3)'' as dir, fromnodeid as nextnode
						  from road_rlk
						  where	(tonodeid = ' || tmpLastNodeArray[tmpPathIndex]|| ')
						       and (rdb_cnv_oneway_code(oneway_c) in (1,3)) 
						       and rdb_cnv_link_type(linkcls_c) = 5 
						       and objectid <> ' || tmplastLinkArray[tmpPathIndex]|| '
					) as a
					'
				loop
					if not ((rec.nextroad||rec.dir) = ANY(regexp_split_to_array(tmpPathArray[tmpPathIndex], E'\\|+'))) then
						tmpPathCount		:= tmpPathCount + 1;
						tmpPathArray		:= array_append(tmpPathArray, cast(tmpPathArray[tmpPathIndex]||'|'||rec.nextroad||rec.dir as varchar));
						tmpLastNodeArray	:= array_append(tmpLastNodeArray, cast(rec.nextnode as bigint));
						tmplastLinkArray        := array_append(tmplastLinkArray, cast(rec.nextroad as bigint));
					end if;
				end loop;
			end if;

			tmpPathIndex := tmpPathIndex + 1;
			continue;
		elseif (2 <> nToNodeType 
		        and array_upper(regexp_split_to_array(tmpPathArray[tmpPathIndex], E'\\|+'), 1) > 1
		        and mid_check_hwy_node_type(tmpLastNodeArray[tmpPathIndex], array[nToNodeType]))
		             or
		       (2 = nToNodeType 
		        and array_upper(regexp_split_to_array(tmpPathArray[tmpPathIndex], E'\\|+'), 1) > 1 
		        and mid_check_hwy_exit_node(tmpLastNodeArray[tmpPathIndex], tmplastLinkArray[tmpPathIndex])
		        ) then             -- hwy exit

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
		
		elseif array_upper(regexp_split_to_array(tmpPathArray[tmpPathIndex], E'\\|+'), 1) >= 15 then  -- It is longer then 15.
			tmpPathIndex := tmpPathIndex + 1;
			continue;
		else
			for rec in execute
				'
				 select  nextroad, a.dir, a.nextnode
				 from
				 (
					 select objectid as nextroad, ''(2)'' as dir, tonodeid as nextnode
					 from road_rlk
					 where	(fromnodeid = ' || tmpLastNodeArray[tmpPathIndex]|| ') and 
							(rdb_cnv_oneway_code(oneway_c) in (1,2)) and 
							(rdb_cnv_road_type(roadcls_c, width_c, linkcls_c, navicls_c) in (0, 1) or 
							 rdb_cnv_link_type(linkcls_c) in (3,5) or
							 rdb_cnv_link_toll(roadcls_c) = 1)
					 union
					 
					 select objectid as nextroad, ''(3)'' as dir, fromnodeid as nextnode
					 from road_rlk
					 where	(tonodeid = ' || tmpLastNodeArray[tmpPathIndex]|| ') and
							(rdb_cnv_oneway_code(oneway_c) in (1,3)) and 
							(rdb_cnv_road_type(roadcls_c, width_c, linkcls_c, navicls_c) in (0, 1) or 
							 rdb_cnv_link_type(linkcls_c) in (3,5) or
							 rdb_cnv_link_toll(roadcls_c) = 1)
				 )as a
				'
			loop
				if not ((rec.nextroad||rec.dir) = ANY(regexp_split_to_array(tmpPathArray[tmpPathIndex], E'\\|+'))) then
					tmpPathCount		:= tmpPathCount + 1;
					tmpPathArray		:= array_append(tmpPathArray, cast(tmpPathArray[tmpPathIndex]||'|'||rec.nextroad||rec.dir as varchar));
					tmpLastNodeArray	:= array_append(tmpLastNodeArray, cast(rec.nextnode as bigint));
					tmplastLinkArray    := array_append(tmplastLinkArray, cast(rec.nextroad as bigint));
				end if;
			end loop;
		end if;
		tmpPathIndex := tmpPathIndex + 1;
	END LOOP;

	return rstPathArray;
END;
$$;

-----------------------------------------------------------------------------------------
-- find pathes for Highway JCT.
-----------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_findPath_JCT(nFormNodeID bigint)
	RETURNS varchar[]
	LANGUAGE plpgsql
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
	nStartNode    := nFormNodeID;
	rstPathCount  := 0;
	tmpPathIndex  := 1;
	tmpPathCount  := 0;

	-- Get Start Links
	tmpPathArray  := mid_get_start_link(nFormNodeID);
	
	if tmpPathArray is null or array_upper(tmpPathArray, 1) = 0 then 
		raise INFO 'No start links of node: %', nStartNode;
		return null;
	else
		i := 1;
		while i <= array_upper(tmpPathArray, 1) loop
			if tmpPathCount = 0 then
				tmpLastNodeArray := array[nStartNode::bigint];
				tmplastLinkArray := array[tmpPathArray[i]::bigint];
			else
				tmpLastNodeArray := tmpLastNodeArray || array[nStartNode::bigint];
				tmplastLinkArray := tmplastLinkArray || tmpPathArray[i]::bigint;
			end if;
			tmpPathCount := tmpPathCount + 1;
			i := i + 1;
		end loop;
	end if;

	-- search 
	WHILE tmpPathIndex <= tmpPathCount LOOP
		--raise INFO '%', tmpPathArray[tmpPathIndex];
		-- SA/PA
		--if mid_check_hwy_node_type(tmpLastNodeArray[tmpPathIndex], array[4, 6]) then  -- SA: 4, PA: 6
		--	raise INFO 'Pass the SA/PA, from_node: %, sa_pa_node: %', nFormNodeID, tmpLastNodeArray[tmpPathIndex];
		--end if;
		
		-- Out the Hwy
		if mid_check_hwy_exit_node(tmpLastNodeArray[tmpPathIndex], tmplastLinkArray[tmpPathIndex]) then
			tmpPathIndex := tmpPathIndex + 1;
			continue;
		elseif (array_upper(regexp_split_to_array(tmpPathArray[tmpPathIndex], E'\\|+'), 1) > 1
		     and mid_check_hwy_node_type(tmpLastNodeArray[tmpPathIndex], array[7, 2, 8])) then  -- JCT IN:7, IC Out:2, JCT OUT: 8
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
			tmpPathIndex := tmpPathIndex + 1;
			continue;
		else
			for rec in execute
				'
				 select  nextroad, a.dir, a.nextnode
				 from
				 (
					 select objectid as nextroad, ''(2)'' as dir, tonodeid as nextnode
					 from road_rlk
					 where	(fromnodeid = ' || tmpLastNodeArray[tmpPathIndex]|| ') and 
							(rdb_cnv_oneway_code(oneway_c) in (1,2)) and 
							rdb_cnv_link_type(linkcls_c) <> 7 and          -- not SA/PA Link
							(rdb_cnv_road_type(roadcls_c, width_c, linkcls_c, navicls_c) in (0, 1) or 
							 rdb_cnv_link_type(linkcls_c) in (3,5) or
							 rdb_cnv_link_toll(roadcls_c) = 1)
					 union
					 
					 select objectid as nextroad, ''(3)'' as dir, fromnodeid as nextnode
					 from road_rlk
					 where	(tonodeid = ' || tmpLastNodeArray[tmpPathIndex]|| ') and
							(rdb_cnv_oneway_code(oneway_c) in (1,3)) and 
							rdb_cnv_link_type(linkcls_c) <> 7 and           -- not SA/PA Link
							(rdb_cnv_road_type(roadcls_c, width_c, linkcls_c, navicls_c) in (0, 1) or 
							 rdb_cnv_link_type(linkcls_c) in (3,5) or
							 rdb_cnv_link_toll(roadcls_c) = 1)
				 )as a
				'
			loop
				if not ((rec.nextroad||rec.dir) = ANY(regexp_split_to_array(tmpPathArray[tmpPathIndex], E'\\|+'))) then
					tmpPathCount		:= tmpPathCount + 1;
					tmpPathArray		:= array_append(tmpPathArray, cast(tmpPathArray[tmpPathIndex]||'|'||rec.nextroad||rec.dir as varchar));
					tmpLastNodeArray	:= array_append(tmpLastNodeArray, cast(rec.nextnode as bigint));
					tmplastLinkArray    := array_append(tmplastLinkArray, cast(rec.nextroad as bigint));
				end if;
			end loop;
		end if;
		tmpPathIndex := tmpPathIndex + 1;
	END LOOP;

	return rstPathArray;
END;
$$;


-----------------------------------------------------------------------------------------
-- Get JCT name.
-----------------------------------------------------------------------------------------
create or replace function mid_get_JCT_name(nFromNode bigint, nToNode bigint)
	RETURNS varchar
	LANGUAGE plpgsql
AS $$ 
DECLARE
	rstFacName       varchar;
	nCount           integer;
	rec              record;
BEGIN
	nCount     = 0;
	rstFacName = NULL;
	-- JCT IN
	if mid_check_hwy_node_type(nToNode, array[7]) then
		for rec in execute
		'
			SELECT distinct fac_name
			  from  (
				 select *
				  from road_hwynode
				  where pos_code = 7 and  node_id = ' ||  nToNode || '
			  ) as a
			  left join mid_road_roadpoint_bunnki_node as b
			  on a.road_code = b.road_code and a.seq_num = b.road_point
			  where  b.node_id = ' || nFromNode || 'and fac_type = 4
		'
		loop
			if nCount = 0 then
				rstFacName = rec.fac_name;
			end if;
			nCount = nCount + 1;
		end loop;
	end if;

	-- JCT Out: 8, IC Out: 2 or not find fac_name for JCT IN
	if nCount = 0 then
		for rec in execute
		'
			SELECT distinct fac_name
			  from  (
				 select *
				  from road_hwynode
				  where pos_code = 8 and  node_id = ' ||  nFromNode || '
			  ) as a
			  left join mid_road_roadpoint_bunnki_node as b
			  on a.road_code = b.road_code 
			  where b.node_id = ' || nFromNode  || ' and fac_type = 4  
		'
		loop
			if nCount = 0 then
				rstFacName = rec.fac_name;
			end if;
			nCount = nCount + 1;
		end loop;
	end if;

	if nCount > 1 or  nCount = 0 then
		raise info 'the number of fac name is %, nFromNode: %, nToNode: %', nCount, nFromNode, nToNode;
	end if;
	return rstFacName;
END;
$$;

-----------------------------------------------------------------------------------------
-- Get Start Links of a node.
-----------------------------------------------------------------------------------------
create or replace function mid_get_start_link(nNodeId bigint)
	RETURNS varchar[]
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
			   from road_rlk
				where (fromnodeid = ' || nNodeId || ' and rdb_cnv_oneway_code(oneway_c) in (1, 3)) 
				 or (tonodeid = ' || nNodeId || ' and rdb_cnv_oneway_code(oneway_c) in (1, 2))
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

-----------------------------------------------------------------------------------------
-- Is there ramp link in Highway node.
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
	  from road_rlk
	  where ((fromnodeid = nNodeID and (rdb_cnv_oneway_code(oneway_c) in (1,2))) 
	          or (tonodeid = nNodeID and (rdb_cnv_oneway_code(oneway_c) in (1,3)))) 
	         and objectid <> nlink_id  
	         and rdb_cnv_link_type(linkcls_c) in (5, 7);

	if not found then
		return False;
	end if;

	return true;
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
	  from road_rlk
	  where ((fromnodeid = nNodeID and (rdb_cnv_oneway_code(oneway_c) in (1,2)) ) 
	         or (tonodeid = nNodeID and (rdb_cnv_oneway_code(oneway_c) in (1,3))))
	         and objectid <> nlink_id and 
	        (rdb_cnv_road_type(roadcls_c, width_c, linkcls_c, navicls_c) not in (0, 1) and 
	         rdb_cnv_link_type(linkcls_c) not in (3,5,7) and 
	         rdb_cnv_link_toll(roadcls_c) <> 1);

	if not found then
		return False;
	end if;

	return true;
END;
$$;

-----------------------------------------------------------------------------------------
-- find path for Highway Enter
-----------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_findpath_hwy_enter(nformnodeid bigint, ntonodeid bigint)
  RETURNS character varying[] 
  LANGUAGE plpgsql AS
$$ 
DECLARE
	rstPathArray		varchar[];
	rstPathCount		integer;
	rstPath				varchar;
	
	tmpPathArray		varchar[];
	tmpLastNodeArray	bigint[];
	tmpPathCount		integer;
	tmpPathIndex		integer;
	tmplastLinkArray        bigint[];
	tmpLastLinkTypeArray	integer[][3];
	tmpPassedTonodeArray    integer[];
	
	nStartNode		integer;
	
	nNewFromLink	bigint;
	nNewToLink		bigint;
	tmpLength		integer;
	
	rec			record;
BEGIN
	if nformnodeid is null then 
		return null;
	end if;
	
	--rstPathArray
	rstPathCount		:= 0;
	tmpPathArray		:= array['0'];
	tmpLastNodeArray	:= ARRAY[ntonodeid];
	tmplastLinkArray        := array[0];
	tmpLastLinkTypeArray    := array[[-1, -1, -1]];
	tmpPathCount		:= 1;
	tmpPathIndex		:= 1;
	tmpPassedTonodeArray    := array[0];
	tmpLength		:= 0;

	-- search
	WHILE tmpPathIndex <= tmpPathCount LOOP
		--raise INFO 'tmpPathIndex: %', tmpPathIndex;
		--raise INFO 'tmpLastNodeArray------- %', cast(tmpLastNodeArray[tmpPathIndex] as varchar);
		if tmpPassedTonodeArray[tmpPathIndex] = 0 and tmpLastNodeArray[tmpPathIndex] = nformnodeid then 
			tmpPassedTonodeArray[tmpPathIndex] = 1;
		end if;

		if (tmpPathIndex > 1 and mid_check_hwy_node_type(tmpLastNodeArray[tmpPathIndex], array[2, 1])) then 
			tmpPathIndex := tmpPathIndex + 1;
			continue;
		end if;
		
		--raise INFO 'tmpLastNodeArray------- %', cast(tmpPathArray[tmpPathIndex] as varchar);	
		tmpLength  = array_upper(regexp_split_to_array(tmpPathArray[tmpPathIndex], E'\\|+'), 1);
		if  tmpPassedTonodeArray[tmpPathIndex] = 1 then  -- It is longer then 15.
			if tmpLength >= 18 then 
				tmpPathIndex := tmpPathIndex + 1;
				continue;
			end if;
		else 
			if tmpLength >= 13 then 
				tmpPathIndex := tmpPathIndex + 1;
				continue;
			end if;
		end if;	
		--raise INFO 'ATT: ---%, %, %', tmpLastLinkTypeArray[tmpPathIndex][1], tmpLastLinkTypeArray[tmpPathIndex][2], tmpLastLinkTypeArray[tmpPathIndex][3];
		if tmpPathIndex > 1 and tmpLastLinkTypeArray[tmpPathIndex][1] not in (4,5) 
		   and tmpLastLinkTypeArray[tmpPathIndex][2] not in (0,1)
		   and tmpLastLinkTypeArray[tmpPathIndex][3] <> 1
		  then
		   --and (nformnodeid = ntonodeid or tmpPassedTonodeArray[tmpPathIndex] = 1) then 
			--raise INFO 'Done:------- %', cast(tmpPathArray as varchar);
			rstPathCount			:= rstPathCount + 1;
			rstPath				:= cast(tmpPathArray[tmpPathIndex] as varchar);
			rstPath				:= replace(rstPath, '(2)', '');
			rstPath				:= replace(rstPath, '(3)', '');
			rstPath				:= rstPath;
			if rstPathCount = 1 then
				rstPathArray	:= ARRAY[rstPath];
			else
				rstPathArray	:= array_append(rstPathArray, rstPath);
			end if;
			-- Not Inner Link
			--if tmpLastLinkTypeArray[tmpPathIndex] <> 4 then 
			---	-- next path
			--	tmpPathIndex := tmpPathIndex + 1;
			--	continue;
			--end if;
			-- next path
			tmpPathIndex := tmpPathIndex + 1;
			continue;
		end if;	

	
		for rec in execute
			'
			SELECT nextroad, dir, nextnode, array[link_type, road_type, toll] as attr
			from (
				select objectid as nextroad, ''(3)'' as dir, tonodeid as nextnode, 
				       linkcls_c as link_type,
				       rdb_cnv_road_type(roadcls_c, width_c, linkcls_c, navicls_c) as road_type,
				       rdb_cnv_link_toll(roadcls_c) as toll
				from road_rlk
				where	(fromnodeid = ' || tmpLastNodeArray[tmpPathIndex] || ' ) 
					and (rdb_cnv_oneway_code(oneway_c) in (1, 3))
					and not (' || tmpLastLinkTypeArray[tmpPathIndex][1] ||' = 4 and linkcls_c = 5)
					and objectid <> ' || tmplastLinkArray[tmpPathIndex] || ' 
			
				union

				select objectid as nextroad, ''(2)'' as dir, fromnodeid as nextnode, 
				       linkcls_c as link_type,
				       rdb_cnv_road_type(roadcls_c, width_c, linkcls_c, navicls_c) as road_type,
				       rdb_cnv_link_toll(roadcls_c) as toll
				from road_rlk
				where	(tonodeid = ' || tmpLastNodeArray[tmpPathIndex] || ')
					and (rdb_cnv_oneway_code(oneway_c) in (1,2))
					and not (' || tmpLastLinkTypeArray[tmpPathIndex][1] || ' = 4 and linkcls_c = 5)
					and objectid <> ' || tmplastLinkArray[tmpPathIndex] || ' 
					and not ( ' || tmpLength || ' > 5 and (rdb_cnv_link_toll(roadcls_c) = 1 or 
					     rdb_cnv_road_type(roadcls_c, width_c, linkcls_c, navicls_c) in (0, 1)) 
					     and linkcls_c in (1, 2))
			) as a
			'
		loop	
			if not ((rec.nextroad||'(2)') = ANY(regexp_split_to_array(tmpPathArray[tmpPathIndex], E'\\|+'))
				or (rec.nextroad||'(3)') = ANY(regexp_split_to_array(tmpPathArray[tmpPathIndex], E'\\|+'))
			    ) 
			then
				tmpPathCount		:= tmpPathCount + 1;
				if tmpPathIndex = 1 then
					tmpPathArray := array_append(tmpPathArray, cast(rec.nextroad||rec.dir as varchar));
				else 
					tmpPathArray := array_append(tmpPathArray, (rec.nextroad||rec.dir ||'|'|| tmpPathArray[tmpPathIndex])::varchar);
				end if;
				tmpLastNodeArray	:= array_append(tmpLastNodeArray, cast(rec.nextnode as bigint));
				tmplastLinkArray        := array_append(tmplastLinkArray, cast(rec.nextroad as bigint));
				tmpPassedTonodeArray    := array_append(tmpPassedTonodeArray, tmpPassedTonodeArray[tmpPathIndex]);
				tmpLastLinkTypeArray	:= tmpLastLinkTypeArray || rec.attr;
			end if;
		
		end loop;

		
		tmpPathIndex := tmpPathIndex + 1;
	end loop;

	return rstPathArray;
END;
$$;

-----------------------------------------------------------------------------------------
-- find path for two Highway Points
-----------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_findpath_hwy_two_node(nformnodeid bigint, ntonodeid bigint, ntonodetype integer)
  RETURNS character varying[] 
  LANGUAGE plpgsql 
  AS
$$ 
DECLARE
	rstPathArray		varchar[];
	rstPathCount		integer;
	rstPath				varchar;
	
	tmpPathArray		varchar[];
	tmpLastNodeArray	bigint[];
	tmpPathCount		integer;
	tmpPathIndex		integer;
	tmplastLinkArray	bigint[];
	
	nStartNode			integer;
	
	nNewFromLink		bigint;
	nNewToLink			bigint;
	
	rec			record;
BEGIN
	if nformnodeid is null or ntonodeid is null then 
		return null;
	end if;
	
	-- init
	nStartNode = nFormNodeID;
	 
	SELECT objectid as link_id into nNewFromLink
	   from road_rlk
		where (fromnodeid = nFormNodeID and rdb_cnv_oneway_code(oneway_c) in (1, 3)) 
		 or (tonodeid = nFormNodeID and rdb_cnv_oneway_code(oneway_c) in (1, 2))
	;
	
	--rstPathArray
	rstPathCount		:= 0;
	tmpPathArray		:= ARRAY[cast(nNewFromLink as varchar)];
	tmpLastNodeArray	:= ARRAY[nStartNode];
	tmplastLinkArray        := ARRAY[cast(nNewFromLink as varchar)];
	tmpPathCount		:= 1;
	tmpPathIndex		:= 1;
	
	--raise INFO 'ntonodeid: %', ntonodeid; 
	-- search
	WHILE tmpPathIndex <= tmpPathCount LOOP
		--raise INFO '%', tmpPathIndex;
		--raise INFO '%', cast(tmpPathArray[tmpPathIndex] as varchar);
		--raise INFO 'tmpLastNodeArray[tmpPathIndex]: %', tmpLastNodeArray[tmpPathIndex];
		
		if tmpLastNodeArray[tmpPathIndex] = ntonodeid then
			--raise INFO 'Done:------- %', cast(tmpPathArray as varchar);
			rstPathCount			:= rstPathCount + 1;
			rstPath				:= cast(tmpPathArray[tmpPathIndex] as varchar);
			rstPath				:= replace(rstPath, '(2)', '');
			rstPath				:= replace(rstPath, '(3)', '');
			rstPath				:= rstPath;
			if rstPathCount = 1 then
				rstPathArray	:= ARRAY[rstPath];
			else
				rstPathArray	:= array_append(rstPathArray, rstPath);
			end if;
			-- next path
			tmpPathIndex := tmpPathIndex + 1;
			continue;
		end if;
		
		if array_upper(regexp_split_to_array(tmpPathArray[tmpPathIndex], E'\\|+'), 1) >= 15 then  -- It is longer then 15.
			raise INFO '%', 15;
			raise INFO '%', replace(replace(cast(tmpPathArray[tmpPathIndex] as varchar), '(2)', ''), '(3)', '');
			tmpPathIndex := tmpPathIndex + 1;
			continue;
		end if;	

		-- search next links
		for rec in execute
				'
				 select  nextroad, a.dir, a.nextnode
				 from
				 (
					 select objectid as nextroad, ''(2)'' as dir, tonodeid as nextnode
					 from road_rlk
					 where	(fromnodeid = ' || tmpLastNodeArray[tmpPathIndex]|| ') and 
							(rdb_cnv_oneway_code(oneway_c) in (1,2)) and 
							(rdb_cnv_road_type(roadcls_c, width_c, linkcls_c, navicls_c) in (0, 1) or 
							 rdb_cnv_link_type(linkcls_c) in (3,5,7) or
							 rdb_cnv_link_toll(roadcls_c) = 1)
					 union
					 
					 select objectid as nextroad, ''(3)'' as dir, fromnodeid as nextnode
					 from road_rlk
					 where	(tonodeid = ' || tmpLastNodeArray[tmpPathIndex]|| ') and
							(rdb_cnv_oneway_code(oneway_c) in (1,3)) and 
							(rdb_cnv_road_type(roadcls_c, width_c, linkcls_c, navicls_c) in (0, 1) or 
							 rdb_cnv_link_type(linkcls_c) in (3,5,7) or
							 rdb_cnv_link_toll(roadcls_c) = 1)
				 )as a
				'
		loop
			if not ((rec.nextroad||rec.dir) = ANY(regexp_split_to_array(tmpPathArray[tmpPathIndex], E'\\|+'))) then
				tmpPathCount		:= tmpPathCount + 1;
				tmpPathArray		:= array_append(tmpPathArray, cast(tmpPathArray[tmpPathIndex]||'|'||rec.nextroad||rec.dir as varchar));
				tmpLastNodeArray	:= array_append(tmpLastNodeArray, cast(rec.nextnode as bigint));
				tmplastLinkArray    := array_append(tmplastLinkArray, cast(rec.nextroad as bigint));
			end if;
		end loop;
		-- next path
		tmpPathIndex := tmpPathIndex + 1;
	END LOOP;

	return rstPathArray;
END;
$$;

CREATE OR REPLACE FUNCTION mid_findpath_two_node(nformnodeid bigint, ntonodeid bigint)
  RETURNS character varying[] 
  LANGUAGE plpgsql 
  AS
$$ 
DECLARE
	rstPathArray		varchar[];
	rstPathCount		integer;
	rstPath				varchar;
	
	tmpPathArray		varchar[];
	tmpLastNodeArray	bigint[];
	tmpPathCount		integer;
	tmpPathIndex		integer;
	tmplastLinkArray	bigint[];
	
	nStartNode			integer;
	
	nNewFromLink		bigint;
	nNewToLink			bigint;
	
	rec			record;
BEGIN
	if nformnodeid is null or ntonodeid is null then 
		return null;
	end if;
	
	-- init
	nStartNode = nFormNodeID;
	 
	SELECT objectid as link_id into nNewFromLink
	   from road_rlk
		where (fromnodeid = nFormNodeID and rdb_cnv_oneway_code(oneway_c) in (1, 3)) 
		 or (tonodeid = nFormNodeID and rdb_cnv_oneway_code(oneway_c) in (1, 2))
	;
	
	--rstPathArray
	rstPathCount		:= 0;
	tmpPathArray		:= ARRAY[cast(nNewFromLink as varchar)];
	tmpLastNodeArray	:= ARRAY[nStartNode];
	tmplastLinkArray        := ARRAY[cast(nNewFromLink as varchar)];
	tmpPathCount		:= 1;
	tmpPathIndex		:= 1;
	
	--raise INFO 'ntonodeid: %', ntonodeid; 
	-- search
	WHILE tmpPathIndex <= tmpPathCount LOOP
		--raise INFO '%', tmpPathIndex;
		--raise INFO '%', cast(tmpPathArray[tmpPathIndex] as varchar);
		--raise INFO 'tmpLastNodeArray[tmpPathIndex]: %', tmpLastNodeArray[tmpPathIndex];
		
		if tmpLastNodeArray[tmpPathIndex] = ntonodeid then
			--raise INFO 'Done:------- %', cast(tmpPathArray as varchar);
			rstPathCount			:= rstPathCount + 1;
			rstPath				:= cast(tmpPathArray[tmpPathIndex] as varchar);
			rstPath				:= replace(rstPath, '(2)', '');
			rstPath				:= replace(rstPath, '(3)', '');
			rstPath				:= rstPath;
			if rstPathCount = 1 then
				rstPathArray	:= ARRAY[rstPath];
			else
				rstPathArray	:= array_append(rstPathArray, rstPath);
			end if;
			-- next path
			tmpPathIndex := tmpPathIndex + 1;
			continue;
		end if;
		
		if array_upper(regexp_split_to_array(tmpPathArray[tmpPathIndex], E'\\|+'), 1) >= 15 then  -- It is longer then 15.
			--raise INFO '%', 15;
			--raise INFO '%', replace(replace(cast(tmpPathArray[tmpPathIndex] as varchar), '(2)', ''), '(3)', '');
			tmpPathIndex := tmpPathIndex + 1;
			continue;
		end if;	

		-- search next links
		for rec in execute
				'
				 select  nextroad, a.dir, a.nextnode
				 from
				 (
					 select objectid as nextroad, ''(2)'' as dir, tonodeid as nextnode
					 from road_rlk
					 where	(fromnodeid = ' || tmpLastNodeArray[tmpPathIndex]|| ') and 
							(rdb_cnv_oneway_code(oneway_c) in (1,2))
					 union
					 
					 select objectid as nextroad, ''(3)'' as dir, fromnodeid as nextnode
					 from road_rlk
					 where	(tonodeid = ' || tmpLastNodeArray[tmpPathIndex]|| ') and
							(rdb_cnv_oneway_code(oneway_c) in (1,3)) 
				 )as a
				'
		loop
			if not ((rec.nextroad||rec.dir) = ANY(regexp_split_to_array(tmpPathArray[tmpPathIndex], E'\\|+'))) then
				tmpPathCount		:= tmpPathCount + 1;
				tmpPathArray		:= array_append(tmpPathArray, cast(tmpPathArray[tmpPathIndex]||'|'||rec.nextroad||rec.dir as varchar));
				tmpLastNodeArray	:= array_append(tmpLastNodeArray, cast(rec.nextnode as bigint));
				tmplastLinkArray    := array_append(tmplastLinkArray, cast(rec.nextroad as bigint));
			end if;
		end loop;
		-- next path
		tmpPathIndex := tmpPathIndex + 1;
	END LOOP;

	return rstPathArray;
END;
$$;

-----------------------------------------------------------------------------------------
-- short cut the full path
-----------------------------------------------------------------------------------------
CREATE or REPLACE function mid_get_fac_name_passlid(id integer, inlinkid bigint, nodeid bigint, full_path character varying[],
						  tonodeid bigint, PosCode smallint, FacName character varying)
  RETURNS character varying
  LANGUAGE plpgsql 
  AS
$$
DECLARE
	link_num     integer;
	passlink_cnt integer;
	connect_node bigint;
	passlink_str character varying;
	rec          record;
	i            integer;
BEGIN	
	if inlinkid is null or nodeid is null or tonodeid is null 
	   or full_path is null or array_upper(full_path, 1) < 1 then
		return NULL;
	end if;
	
	passlink_cnt := array_upper(full_path, 1);
	i            := 0;
	-- JCT 
	if PosCode = 8 then
		-- Not JCT IN (IC OUT: 2, JCT )
		if not mid_check_hwy_node_type(tonodeid, array[7]) then 
			return full_path[1]::character varying;
		end if;
		
		-- there are Ramp Link in the JCT Node
		if mid_has_ramp_link(tonodeid, full_path[passlink_cnt]::bigint) then
			for rec in execute
				'
				SELECT nextroad
				from (
					select objectid as nextroad
					  from road_rlk
					  where	(fromnodeid = ' || tonodeid || ' ) 
					       and (rdb_cnv_oneway_code(oneway_c) in (1,2)) 
					       and rdb_cnv_link_type(linkcls_c) <> 5 
					       and objectid <> ' || full_path[passlink_cnt] || '
					union
					
					select objectid as nextroad
					  from road_rlk
					  where	(tonodeid = ' || tonodeid || ')
					       and (rdb_cnv_oneway_code(oneway_c) in (1,3)) 
					       and rdb_cnv_link_type(linkcls_c) <> 5 
					       and objectid <> ' || full_path[passlink_cnt] || '
				) as a
				'
			loop
				i := i + 1;
			end loop;
			
			if i > 1 then 
				raise EXCEPTION 'gid = %, There are more than two out links.',  id;
			end if;
			
			return array_to_string(full_path, '|') || '|' || rec.nextroad::varchar;
		end if;
	end if;

	while passlink_cnt > 1 LOOP		
		-- highway Enter
		if PosCode = 1 or PosCode = 13 then
			-- Inner link
			PERFORM objectid from road_rlk 
			    where objectid = full_path[passlink_cnt - 1]::bigint and rdb_cnv_link_type(linkcls_c) = 4;
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
					select objectid as nextroad, rdb_cnv_link_type(linkcls_c) as link_type
					  from road_rlk
					  where	(fromnodeid = ' || connect_node || ' ) 
					       and (rdb_cnv_oneway_code(oneway_c) in (1,2)) 
					       and objectid <> ' || full_path[passlink_cnt - 1]|| '
					       and objectid <> ' || full_path[passlink_cnt] || '
					union
					
					select objectid as nextroad, rdb_cnv_link_type(linkcls_c) as link_type
					  from road_rlk
					  where	(tonodeid = ' || connect_node || ')
					       and (rdb_cnv_oneway_code(oneway_c) in (1,3)) 
					       and objectid <> ' || full_path[passlink_cnt - 1]|| '
					       and objectid <> ' || full_path[passlink_cnt] || '
				) as a
				'
			loop
				passlink_str := array_to_string(full_path[1:(passlink_cnt - 1)], '|') || '|' || rec.nextroad::varchar;

				
				PERFORM gid
				  from mid_fac_name_full_path
				  where in_link_id = inlinkid and node_id = nodeid and gid <> id
				       and array_to_string(path, '|') like passlink_str || '%'
				       and (pos_code <> PosCode or fac_name <> FacName);
				-- Exist Other Facility Name or Type.
				if found then
					return array_to_string(full_path[1:passlink_cnt], '|');
				else
					-- Not Smart IC Enter
					if PosCode <> 13 then
						PERFORM gid
						  from mid_fac_name_full_path
						  where in_link_id = inlinkid and node_id = nodeid and gid <> id
						       and array_to_string(path, '|') like passlink_str || '%';
						 -- Exist Other Direction, but dose not exist Facility Name.
						 if not found then
							if PosCode not in(4, 6) then -- not SA/PA
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
-- facility name dictionary
-----------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_insert_name_dictionary_fac_name()
    RETURNS integer
    LANGUAGE plpgsql
    AS $$ 
DECLARE
    curs1        refcursor;
    curr_name_id integer;
    id           integer;
    tmp_name     character varying;
    cnt          integer;
    temp_gid     integer[];
    i            integer;
BEGIN
	curr_name_id := 0;
	cnt          := 0;

	OPEN curs1 FOR SELECT gid_array, fac_name
					  from (
						SELECT array_agg(gid) as gid_array, fac_name
						  FROM mid_fac_name
						  group by fac_name
					  ) as a
					  order by fac_name;
			  
	-- Get the first record        
	FETCH curs1 INTO temp_gid, tmp_name;
	while temp_gid is not null LOOP
		-- rdb_add_one_name(n_id, _name, yomi, seq_nm, lang_type)
		curr_name_id := rdb_add_one_name(curr_name_id, tmp_name, NULL, 5);
		i := 1;
		while i <= array_upper(temp_gid, 1) LOOP
			INSERT INTO mid_temp_fac_name_id("gid", "new_name_id")
				values(temp_gid[i], curr_name_id);
			i := i + 1;
		end loop;
		
		curr_name_id := curr_name_id + 1;
		cnt          := cnt + array_upper(temp_gid, 1);
		-- Get the next record
		FETCH curs1 INTO temp_gid, tmp_name;
	END LOOP;
	close curs1;

    return cnt;
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
  
	select fromnodeid, tonodeid into fromnodeid1, tonodeid1 from road_rlk where objectid = fromLink;
	if not found then
		return NULL;
	end if;
	
	select fromnodeid, tonodeid into fromnodeid2, tonodeid2 from road_rlk where objectid = toLink;
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
	when fac_type = 2 then 4  -- SA
	when fac_type = 3 then 5  -- PA
	when fac_type = 4 then 3  -- JCT
	when fac_type = 5 then 2  -- Ramp Enter
	when fac_type = 6 then 2  -- Ramp Exit
	when fac_type = 7 then 1  -- IC Enter
	when fac_type = 8 then 1  -- IC Exit
	when fac_type = 10 then 1 -- Smart IC Enter
	when fac_type = 11 then 1 -- Smart IC Exit
	else 0
	end;
end;
$$;

----------------------------------------------------------------------------------------
-- Dictionary for Towand Name and SignPost
----------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION rdb_insert_name_dictionary_towardname()
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
-- find path of hwy enter (new)
----------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_findpath_hwy_enter_2(ntonodeid bigint, TypeArray integer[])
  RETURNS character varying[] 
  LANGUAGE plpgsql AS
$$ 
DECLARE
	rstPathArray		varchar[];
	rstPathCount		integer;
	rstPath			varchar;
	
	tmpPathArray		varchar[];
	tmpLastNodeArray	bigint[];
	tmpPathCount		integer;
	tmpPathIndex		integer;
	tmplastLinkArray        bigint[];
	tmpLastLinkTypeArray	integer[][3];
	tmpHwyEnterNodeArray    integer[];
	
	tmpNextLinkArray        bigint[];
	tmpNextNodeArray        bigint[];
	tmpNextLinkTypeArray    integer[][3];
	--nStartNode		integer;
	nNewFromLink		bigint;
	nNewToLink		bigint;
	tmpLength		integer;
	isEnterNode             boolean;
	isEnterLink             boolean;
	isPassedJCTRampLink     boolean[];
	isForSmartIC            boolean;
	i                       integer;
	j                       integer;
	tmpArray                integer[3];
	rec			record;
BEGIN
	if ntonodeid is null then 
		return null;
	end if;
	
	--rstPathArray
	rstPathCount		:= 0;
	tmpPathArray		:= array['0'];
	tmpLastNodeArray	:= ARRAY[ntonodeid];
	tmplastLinkArray        := array[0];
	tmpLastLinkTypeArray    := array[[-1, -1, -1]];
	tmpHwyEnterNodeArray    := array[NULL];
	isPassedJCTRampLink     := array[false];
	tmpPathCount		:= 1;
	tmpPathIndex		:= 1;
	tmpLength		:= 0;
	isForSmartIC            := false;
	
	if array[10] = TypeArray then
		isForSmartIC = true;
	end if;
	
	--raise INFO 'ntonodeid: %,  current_time: %.', ntonodeid, current_time;
	-- search
	WHILE tmpPathIndex <= tmpPathCount LOOP
		--raise INFO 'tmpPathIndex: %', tmpPathIndex;
		--raise info 'tmpPathArray:%', cast(tmpPathArray[tmpPathIndex] as varchar);
		--raise info '----------------------------';
		isEnterNode          := False;
		tmpNextLinkArray     := NULL;
		tmpNextNodeArray     := NULL;
		tmpNextLinkTypeArray := NULL;
		
		-- 1: IC Enter; 2: IC Exit, 13: Smart IC Enter
		if (tmpPathIndex > 1 and mid_check_hwy_node_type(tmpLastNodeArray[tmpPathIndex], array[2, 1, 13])) then 
			tmpPathIndex := tmpPathIndex + 1;
			continue;
		end if;
		
		-- Facility------6: Ramp Enter 8: IC Enter or 11: smart IC
		--if tmpPathIndex > 1 
		--   and tmpNextLinkTypeArray[1] <> 4      -- Inner Link
		--   and mid_check_fac_point_type(tmpLastNodeArray[tmpPathIndex], array[6, 8, 11]) then
		--	tmpPathIndex := tmpPathIndex + 1;
		--	continue;
		--end if;
		
		-- Facility----5: Ramp Enter 7: IC Enter or 10: smart IC
		if  mid_check_fac_point_type(tmpLastNodeArray[tmpPathIndex], TypeArray) then
			--raise info '%', tmpLastNodeArray[tmpPathIndex];
			if tmpHwyEnterNodeArray[tmpPathIndex] is not NULL then
				if tmpHwyEnterNodeArray[tmpPathIndex] = tmpLastNodeArray[tmpPathIndex] then
					tmpPathIndex := tmpPathIndex + 1;
					continue;
				else
					if tmpHwyEnterNodeArray[tmpPathIndex] = ntonodeid then
						tmpHwyEnterNodeArray[tmpPathIndex] = tmpLastNodeArray[tmpPathIndex];
					else
						-- EXCEPTION
						raise info 'Passed two enter node: first %, second %, path %', tmpHwyEnterNodeArray[tmpPathIndex], tmpLastNodeArray[tmpPathIndex], tmpPathArray[tmpPathIndex];
					end if;
				end if;
			else 
				tmpHwyEnterNodeArray[tmpPathIndex] = tmpLastNodeArray[tmpPathIndex];
			end if;
			--raise info '%', tmpHwyEnterNodeArray[tmpPathIndex];
		end if;
		
		--raise INFO 'tmpLastNodeArray------- %', cast(tmpPathArray[tmpPathIndex] as varchar);	
		tmpLength  = array_upper(regexp_split_to_array(tmpPathArray[tmpPathIndex], E'\\|+'), 1);
		
		if (tmpLength > 4
		and isForSmartIC = false
		and isPassedJCTRampLink[tmpPathIndex] = false) then 
			tmpPathIndex := tmpPathIndex + 1;
			continue;
		end if;
		
		if  tmpHwyEnterNodeArray[tmpPathIndex] is not NULL then  -- It is longer then 15.
			if tmpLength >= 15 then 
				--raise INFO 'tmpPathArray------- %', cast(tmpPathArray[tmpPathIndex] as varchar); 
				tmpPathIndex := tmpPathIndex + 1;
				continue;
			end if;
		else 
			if tmpLength >= 15 then
				--raise INFO 'tmpPathArray------- %', cast(tmpPathArray[tmpPathIndex] as varchar); 
				tmpPathIndex := tmpPathIndex + 1;
				continue;
			end if;
		end if;	
	
		for rec in execute
			'
			SELECT nextroad, dir, nextnode, array[link_type, road_type, toll] as attr
			from (
				select objectid as nextroad, ''(3)'' as dir, tonodeid as nextnode, 
				       rdb_cnv_link_type(linkcls_c) as link_type,
				       rdb_cnv_road_type(roadcls_c, width_c, linkcls_c, navicls_c) as road_type,
				       rdb_cnv_link_toll(roadcls_c) as toll
				from road_rlk
				where	(fromnodeid = ' || tmpLastNodeArray[tmpPathIndex] || ' ) 
					and (rdb_cnv_oneway_code(oneway_c) in (1, 3))
					--and not (' || tmpLastLinkTypeArray[tmpPathIndex][1] ||' = 4 and 
					--        (rdb_cnv_link_type(linkcls_c) in(3, 5) or rdb_cnv_link_toll(roadcls_c) = 1 
					--        or rdb_cnv_road_type(roadcls_c, width_c, linkcls_c, navicls_c) in (0, 1)))
					and objectid <> ' || tmplastLinkArray[tmpPathIndex] || ' 
					and rdb_cnv_road_type(roadcls_c, width_c, linkcls_c, navicls_c) <> 9            -- Narrow Street 2
				union

				select objectid as nextroad, ''(2)'' as dir, fromnodeid as nextnode, 
				       rdb_cnv_link_type(linkcls_c) as link_type,
				       rdb_cnv_road_type(roadcls_c, width_c, linkcls_c, navicls_c) as road_type,
				       rdb_cnv_link_toll(roadcls_c) as toll
				from road_rlk
				where	(tonodeid = ' || tmpLastNodeArray[tmpPathIndex] || ')
					and (rdb_cnv_oneway_code(oneway_c) in (1,2))
					--and not (' || tmpLastLinkTypeArray[tmpPathIndex][1] || ' = 4 and 
					--     (rdb_cnv_link_type(linkcls_c) in(3, 5) or rdb_cnv_link_toll(roadcls_c) = 1 
					--     or rdb_cnv_road_type(roadcls_c, width_c, linkcls_c, navicls_c) in (0, 1)))
					and objectid <> ' || tmplastLinkArray[tmpPathIndex] || ' 
					and rdb_cnv_road_type(roadcls_c, width_c, linkcls_c, navicls_c) <> 9            -- Narrow Street 2
			) as a
			'
		loop	
			if not ((rec.nextroad::varchar) = ANY(regexp_split_to_array(tmpPathArray[tmpPathIndex], E'\\|+'))) then
				-- Not Smart IC and SA/PA Link
				if isForSmartIC =false and rec.attr[1] = 7 then
					continue;
				end if;
				
				isEnterLink := false;
				if (tmpPathIndex > 1 and tmpHwyEnterNodeArray[tmpPathIndex] is not null) then
					j := 1;
					while j <= 3 loop
						tmpArray[j] = tmpLastLinkTypeArray[tmpPathIndex][j];
						j := j + 1;
					end loop;
					--raise info 'tmpArray:%', tmpArray;
					if mid_is_hwy_enter(rec.nextroad::bigint, rec.attr, tmplastLinkArray[tmpPathIndex], tmpArray) then
						isEnterNode := TRUE;
						isEnterLink := true;
						--raise INFO '======Done==============';
						--raise INFO 'tmpPathArray------- %', cast(tmpPathArray[tmpPathIndex] as varchar);
						--raise INFO 'tmplastLink------- %', rec.nextroad;
						--raise INFO 'tmplastNode------- %', tmpLastNodeArray[tmpPathIndex];
						--raise INFO '========================';
						rstPathCount := rstPathCount + 1;
						rstPath      := rec.nextroad::varchar || '|' || cast(tmpPathArray[tmpPathIndex] as varchar);
						rstPath      := rstPath  || '|' || tmpHwyEnterNodeArray[tmpPathIndex];
						--raise INFO 'rstPath------- %', rstPath;
						if rstPathCount = 1 then
							-- append enter node id at the end
							rstPathArray := ARRAY[rstPath]; 
						else
							-- append enter node id at the end
							rstPathArray := array_append(rstPathArray, rstPath); 
						end if;
						--raise INFO '------------------------------------';
					end if;
				end if;	
				
				if isEnterLink = false then 
					if tmpNextLinkArray is null then
						tmpNextLinkArray        := array[rec.nextroad::bigint];
						tmpNextNodeArray        := array[rec.nextnode::bigint];
						tmpNextLinkTypeArray    := array[rec.attr];
					else 
						tmpNextLinkArray     := tmpNextLinkArray || array[rec.nextroad::bigint];
						tmpNextNodeArray     := tmpNextNodeArray || array[rec.nextnode::bigint];
						tmpNextLinkTypeArray := tmpNextLinkTypeArray || rec.attr::int[];
					end if;
				end if;
			end if;
		
		end loop;
		
		--raise info 'tmpNextLinkArray:%', tmpNextLinkArray;
		--raise info 'tmpNextLinkTypeArray: %', tmpNextLinkTypeArray;
		--raise info 'tmpLastLinkTypeArray: %', tmpLastLinkTypeArray;
		
		i := 1;
		while i <= array_upper(tmpNextLinkArray, 1) loop
			--if isEnterNode = true and tmpNextLinkTypeArray[i][1] = 5 then
			--	raise info 'Next Ramp Link: %', tmpNextLinkArray[i];
			--end if;
			IF not (isEnterNode = true and tmpNextLinkTypeArray[i][1] = 5) then -- ramp link
				tmpPathCount		:= tmpPathCount + 1;
				if tmpPathIndex = 1 then
					tmpPathArray := array_append(tmpPathArray, cast(tmpNextLinkArray[i] as varchar));
				else 
					tmpPathArray := array_append(tmpPathArray, (tmpNextLinkArray[i] ||'|'|| tmpPathArray[tmpPathIndex])::varchar);
				end if;
				tmpLastNodeArray	:= array_append(tmpLastNodeArray, cast(tmpNextNodeArray[i] as bigint));
				tmplastLinkArray        := array_append(tmplastLinkArray, cast(tmpNextLinkArray[i] as bigint));
				tmpHwyEnterNodeArray    := array_append(tmpHwyEnterNodeArray, tmpHwyEnterNodeArray[tmpPathIndex]);
				j := 1;
				while j <= 3 loop
					tmpArray[j] = tmpNextLinkTypeArray[i][j];
					j := j + 1;
				end loop;
				tmpLastLinkTypeArray := tmpLastLinkTypeArray || tmpArray;
				
				if tmpNextLinkTypeArray[i][1] in(3, 5) then -- Ramp Link
					isPassedJCTRampLink[tmpPathCount] = true;
				else
					isPassedJCTRampLink[tmpPathCount] = isPassedJCTRampLink[tmpPathIndex];
				end if;
			end if;
			i := i + 1;
		end loop;
			
		--raise info 'tmpLastLinkTypeArray: %', tmpLastLinkTypeArray;
		--raise info 'tmpPathCount:%', tmpPathCount;
		--raise info '---------------------------';
		tmpPathIndex := tmpPathIndex + 1;
	end loop;

	return rstPathArray;
END;
$$;

---------------------------------------------------------------------------------------------------
-- Get Enter name.
---------------------------------------------------------------------------------------------------
create or replace function mid_get_enter_name(nEnterNode bigint, nToNode bigint)
	RETURNS varchar[]
	LANGUAGE plpgsql
AS $$ 
DECLARE
	--rstFacName       varchar;
	nCount           integer;
	rec              record;
	mesh_code        varchar;
	road_code        varchar;
	seq_num          varchar;
	pos_code         varchar;
	updown_flag      varchar;
	fac_type         varchar;
	fac_name         varchar;
BEGIN
	nCount     = 0;
	--rstFacName = NULL;

	if (nEnterNode is null ) or nToNode is null then
		raise EXCEPTION 'nEnterNode or nToNode is null, nEnterNode: %, nToNode: %', nEnterNode, nToNode;
		--return NULL;
	end if;
	
	for rec in execute
		'
			select a.mesh_code, a.road_code, a.seq_num, a.pos_code, a.updown_flag, fac_type, fac_name
			  from (
				select *
				  from road_hwynode
				  where node_id = ' || nToNode || ' and pos_code = 1
			  ) as a
			  left join (
				SELECT gid, node_id, point_mesh_code, point_x, point_y, mesh_id, fac_mesh_code, 
				       fac_x, fac_y, addr_id, road_point, fac_name, fac_yomi, road_code, 
				       fac_type, point_pos
				  FROM mid_road_roadpoint_node
				  where node_id = ' || nEnterNode || ' and fac_type in (5, 7)
			  ) as b
			  on a.road_code = b.road_code and a.seq_num = b.road_point
			  where b.gid is not null
		'
	loop
		if nCount = 0 then
			--rstFacName = rec.fac_name;
			mesh_code   = rec.mesh_code::varchar;
			road_code   = rec.road_code::varchar;
			seq_num     = rec.seq_num::varchar;
			pos_code    = rec.pos_code::varchar;
			updown_flag = rec.updown_flag::varchar;
			fac_type    = rec.fac_type::varchar;
			fac_name    = rec.fac_name;
		end if;
		
		nCount = nCount + 1;
	end loop;

	if nCount > 1 or  nCount = 0 then
		raise info 'the number of fac name is %, nEnterNode: %, nToNode: %', nCount, nEnterNode, nToNode;
	end if;

	return array[mesh_code, road_code, seq_num, pos_code, updown_flag, fac_type, fac_name];
END;
$$;

-------------------------------------------------------------------------
-- LinkAttr[1]: link type, LinkAttr[2]: road type, LinkAttr[3]: toll
----------------
CREATE OR REPLACE FUNCTION mid_is_hwy_enter(nInLink bigint, aInLinkAttr integer[], nOutLink bigint, aOutLinkAttr integer[])
  RETURNS boolean
  LANGUAGE plpgsql AS
$$ 
DECLARE
	in_link_type   integer;
	out_link_type  integer;
	in_road_type   integer;
	out_road_type  integer;
	in_link_toll   integer;
	out_link_toll  integer;
BEGIN
	--if nInLink is null or  nOutLink is null then 
	--	-- raise EXCEPTION 'nInLink or nOutLink is null';
	--	return false;
	--end if;

	--select rdb_cnv_link_type(linkcls_c), rdb_cnv_road_type(roadcls_c, width_c, linkcls_c, navicls_c), 
	--      rdb_cnv_link_toll(roadcls_c) 
	--  into out_link_type, out_road_type, out_link_toll
	--  FROM road_rlk
	--  where objectid = nOutLink;
	  
	--if not found then
	--	raise EXCEPTION 'not exist link: %', nOutLink;
	--	return false;
	--end if;

	--select rdb_cnv_link_type(linkcls_c), rdb_cnv_road_type(roadcls_c, width_c, linkcls_c, navicls_c), 
	--       rdb_cnv_link_toll(roadcls_c) 
	--  into in_link_type, in_road_type, in_link_toll
	--  FROM road_rlk
	--  where objectid = nInLink;

	--if not found then
	--	raise EXCEPTION 'not exist link: %', nInLink;
	--	return false;
	--end if;
	in_link_type   = aInLinkAttr[1];
	out_link_type  = aOutLinkAttr[1];
	in_road_type   = aInLinkAttr[2];
	out_road_type  = aOutLinkAttr[2];
	in_link_toll   = aInLinkAttr[3];
	out_link_toll  = aOutLinkAttr[3];
	
	if in_link_type <> 4 then
		if out_road_type in (0, 1) then
			if in_road_type not in (0,1) and in_link_toll <> 1 then
				return true;
			end if;
		end if;
		if (out_link_toll = 1) then
			if in_link_toll <> 1 and in_road_type not in (0, 1) then
				return true;
			end if;
		end if;

		if (out_link_type = 5) and in_link_type not in (3, 5, 7) then
			return true;
		end if;

		if out_link_type = 4 then
			return true;
		end if;
		--if (out_link_toll <> 1 and out_road_type not in (0, 1) and in_road_type not in (0, 1) 
		--   and out_road_type <> in_road_type and in_link_toll <> 1) then
		--	return true;
		--end if;
	end if;

	return false;
END;
$$;

-------------------------------------------------------------------------
-- Path for hwy exit
-------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_findPath_hwy_exit(nFormNodeID bigint, TypeArray integer[])
	RETURNS varchar[]
	LANGUAGE plpgsql
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
	tmpHwyExitNodeArray     bigint[];
	isPassedJCTRampLink     boolean[];
	
	nStartNode		integer;
	nEndNode		integer;
	StartLinkAttr           integer[];
	nStartLinkType          integer;
	nStartLinkToll          integer;
	nStartRoadType          integer;
	
	nNewFromLink	        bigint;
	nNewToLink		bigint;
	tmpLength               integer;
	rec			record;
BEGIN
	if nFormNodeID is null or TypeArray is null then 
		return null;
	end if;
	
	-- init
	nStartNode = nFormNodeID;
	 
	SELECT objectid, array[rdb_cnv_link_type(linkcls_c), 
	                       rdb_cnv_road_type(roadcls_c, width_c, linkcls_c, navicls_c), 
	                       rdb_cnv_link_toll(roadcls_c)] as attr
	   into nNewFromLink, StartLinkAttr
	   from road_rlk
		where (fromnodeid = nFormNodeID and rdb_cnv_oneway_code(oneway_c) in (1, 3)) 
		 or (tonodeid = nFormNodeID and rdb_cnv_oneway_code(oneway_c) in (1, 2))
	;
	
	rstPathCount		:= 0;
	tmpPathArray		:= ARRAY[cast(nNewFromLink as varchar)];
	tmpLastNodeArray	:= ARRAY[nStartNode];
	tmplastLinkArray        := ARRAY[cast(nNewFromLink as varchar)];
	tmpHwyExitNodeArray     := ARRay[NULL];
	isPassedJCTRampLink     := array[false];
	tmpPathCount		:= 1;
	tmpPathIndex		:= 1;
	raise INFO 'nFormNodeID: %,  current_time: %.', nFormNodeID, current_time;
	-- search
	WHILE tmpPathIndex <= tmpPathCount LOOP
		raise info 'tmpPathIndex: %', tmpPathIndex;
		raise info 'isPassedJCTRampLink: %', isPassedJCTRampLink[tmpPathIndex];
		raise info 'tmpPathArray[tmpPathIndex]: %', tmpPathArray[tmpPathIndex];
		raise info 'tmpHwyExitNodeArray: %', tmpHwyExitNodeArray[tmpPathIndex];
		
		-- It is IC Exist Point or Smart IC Point
		if (tmpPathIndex > 1 and mid_check_hwy_node_type(tmpLastNodeArray[tmpPathIndex], array[2, 14])) then 
			tmpPathIndex := tmpPathIndex + 1;
			continue;
		end if;
		
		-- It is Facility point----6: Ramp Enter 8: IC Enter or 11: smart IC
		if  mid_check_fac_point_type(tmpLastNodeArray[tmpPathIndex], TypeArray) then
			--raise info '%', tmpLastNodeArray[tmpPathIndex];
			--if tmpLastNodeArray[tmpPathIndex] <> nFormNodeID then 
			--	rstPathCount   := rstPathCount + 1;
			--	rstPath        := cast(tmpPathArray[tmpPathIndex] as varchar);
			--	rstPath	       := rstPath || '|' || tmpLastNodeArray[tmpPathIndex]::varchar;
			--	if rstPathCount = 1 then
			--		rstPathArray	:= ARRAY[rstPath];
			--	else
			--		rstPathArray	:= array_append(rstPathArray, rstPath);
			--	end if;
				
			--	tmpPathIndex := tmpPathIndex + 1;
			--	continue;
			--end if;
			
			if tmpHwyExitNodeArray[tmpPathIndex] is not NULL then
				if tmpHwyExitNodeArray[tmpPathIndex] = tmpLastNodeArray[tmpPathIndex] then
					tmpPathIndex := tmpPathIndex + 1;
					continue;
				else
					if tmpHwyExitNodeArray[tmpPathIndex] = nFormNodeID then
						tmpHwyExitNodeArray[tmpPathIndex] = tmpLastNodeArray[tmpPathIndex];
					else
						-- EXCEPTION
						raise info 'Passed two exist node: nFormNodeID %, first %, second %, path %', 
						     nFormNodeID, tmpHwyExitNodeArray[tmpPathIndex], tmpLastNodeArray[tmpPathIndex], tmpPathArray[tmpPathIndex];

						--tmpHwyExitNodeArray[tmpPathIndex] = tmpLastNodeArray[tmpPathIndex];
					end if;
				end if;
			else 
				tmpHwyExitNodeArray[tmpPathIndex] = tmpLastNodeArray[tmpPathIndex];
			end if;
			
			--raise info 'ExitNode: %', tmpHwyExitNodeArray[tmpPathIndex];
		end if;
		
		-- It is longer then 15.
		tmpLength = array_upper(regexp_split_to_array(tmpPathArray[tmpPathIndex], E'\\|+'), 1);
		if tmpLength >= 16 then  
			tmpPathIndex := tmpPathIndex + 1;
			continue;
		end if;		

		if tmpLength > 5 and isPassedJCTRampLink[tmpPathIndex] = false then 
			tmpPathIndex := tmpPathIndex + 1;
			continue;
		end if;
		
		-- hwy exit
		if (tmpPathIndex > 1 
		    and tmpHwyExitNodeArray[tmpPathIndex] is not null
		    and mid_check_hwy_exit_node_2(StartLinkAttr, tmpLastNodeArray[tmpPathIndex], tmplastLinkArray[tmpPathIndex])
		    ) then
			rstPathCount   := rstPathCount + 1;
			rstPath        := cast(tmpPathArray[tmpPathIndex] as varchar);
			rstPath	       := rstPath || '|' || tmpHwyExitNodeArray[tmpPathIndex]::varchar;
			if rstPathCount = 1 then
				rstPathArray	:= ARRAY[rstPath];
			else
				rstPathArray	:= array_append(rstPathArray, rstPath);
			end if;
			
			tmpPathIndex := tmpPathIndex + 1;
			continue;
		end if;
		
		for rec in execute
			'
			 select  nextroad, a.dir, a.nextnode, array[link_type, road_type, toll] as attr
			 from
			 (
				 select objectid as nextroad, ''(2)'' as dir, tonodeid as nextnode,
				        rdb_cnv_link_type(linkcls_c) as link_type,
				        rdb_cnv_road_type(roadcls_c, width_c, linkcls_c, navicls_c) as road_type,
				        rdb_cnv_link_toll(roadcls_c) as toll
				 from road_rlk
				 where	(fromnodeid = ' || tmpLastNodeArray[tmpPathIndex]|| ') and 
						(rdb_cnv_oneway_code(oneway_c) in (1,2)) and
						rdb_cnv_link_type(linkcls_c) <> 7
						--(rdb_cnv_road_type(roadcls_c, width_c, linkcls_c, navicls_c) in (0, 1) or 
						-- rdb_cnv_link_type(linkcls_c) in (3,5) or
						-- rdb_cnv_link_toll(roadcls_c) = 1)
				 union
				
				 select objectid as nextroad, ''(3)'' as dir, fromnodeid as nextnode,
				        rdb_cnv_link_type(linkcls_c) as link_type,
				        rdb_cnv_road_type(roadcls_c, width_c, linkcls_c, navicls_c) as road_type,
				        rdb_cnv_link_toll(roadcls_c) as toll
				 from road_rlk
				 where	(tonodeid = ' || tmpLastNodeArray[tmpPathIndex]|| ') and
						(rdb_cnv_oneway_code(oneway_c) in (1,3)) and
						rdb_cnv_link_type(linkcls_c) <> 7
						--(rdb_cnv_road_type(roadcls_c, width_c, linkcls_c, navicls_c) in (0, 1) or 
						-- rdb_cnv_link_type(linkcls_c) in (3,5) or
						-- rdb_cnv_link_toll(roadcls_c) = 1)
			 )as a
			'
		loop
			if not (rec.nextroad::varchar = ANY(regexp_split_to_array(tmpPathArray[tmpPathIndex], E'\\|+'))) then
				tmpPathCount       := tmpPathCount + 1;
				tmpPathArray       := array_append(tmpPathArray, cast(tmpPathArray[tmpPathIndex]||'|'||rec.nextroad as varchar));
				tmpLastNodeArray   := array_append(tmpLastNodeArray, cast(rec.nextnode as bigint));
				tmplastLinkArray   := array_append(tmplastLinkArray, cast(rec.nextroad as bigint));
				tmpHwyExitNodeArray := array_append(tmpHwyExitNodeArray, tmpHwyExitNodeArray[tmpPathIndex]);
				
				if rec.attr[1] in (3, 5) then
					isPassedJCTRampLink := isPassedJCTRampLink || array[true];
				else
					isPassedJCTRampLink := isPassedJCTRampLink || isPassedJCTRampLink[tmpPathIndex];
				end if;
			end if;
		end loop;
		--raise info 'tmpHwyExitNodeArray: %', tmpHwyExitNodeArray;
		tmpPathIndex := tmpPathIndex + 1;
	END LOOP;

	return rstPathArray;
END;
$$;

-------------------------------------------------------------------------
-- check Facility Point type
-------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_check_fac_point_type(nNodeid bigint, TypeArray integer[])
  RETURNS boolean
  LANGUAGE plpgsql AS
$$ 
DECLARE
BEGIN
	if nNodeid is null or TypeArray is null then
		return false;
	end if;
	
	PERFORM  node_id
	  from mid_road_roadpoint_node
	  where node_id = nNodeid and fac_type = ANY(TypeArray);
	  
	if found then
		return true;
	end if;

	return false;
END;
$$;

-----------------------------------------------------------------------------------------
-- High Exit.
-- StartLinkAttr[1]: link type, StartLinkAttr[2]: road type, StartLinkAttr[3]: toll
-----------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_check_hwy_exit_node_2(StartLinkAttr integer[], nnodeid bigint, nlink_id bigint)
  RETURNS boolean
  LANGUAGE plpgsql AS
$$ 
DECLARE
	in_link_type   integer;
	in_road_type   integer;
	in_link_toll   integer;
	rec            record;
BEGIN
	--if nInLink is null or  nOutLink is null then 
	--	-- raise EXCEPTION 'nInLink or nOutLink is null';
	--	return false;
	--end if;

	select rdb_cnv_link_type(linkcls_c), rdb_cnv_road_type(roadcls_c, width_c, linkcls_c, navicls_c), 
	      rdb_cnv_link_toll(roadcls_c) 
	  into in_link_type, in_road_type, in_link_toll
	  FROM road_rlk
	  where objectid = nlink_id;

	--raise info 'nlink_id: %, in_link_type: %, in_road_type: %, in_link_toll: %', nlink_id, in_link_type, in_road_type, in_link_toll;
	
	if not found then
		raise EXCEPTION 'not exist link: %', nOutLink;
		return false;
	end if;

	for rec in execute
		'select objectid, rdb_cnv_link_type(linkcls_c) as link_type, 
			rdb_cnv_road_type(roadcls_c, width_c, linkcls_c, navicls_c) as road_type,
			rdb_cnv_link_toll(roadcls_c) as toll
		  from road_rlk
		  where objectid <> ' || nlink_id  || ' and
		         ((fromnodeid = ' || nnodeid || ' and (rdb_cnv_oneway_code(oneway_c) in (1,2)) ) 
			 or (tonodeid = ' || nnodeid || ' and (rdb_cnv_oneway_code(oneway_c) in (1,3))))
			--(rdb_cnv_road_type(roadcls_c, width_c, linkcls_c, navicls_c) not in (0, 1) and 
			-- rdb_cnv_link_type(linkcls_c) not in (3,5,7) and 
			-- rdb_cnv_link_toll(roadcls_c) <> 1);
		'
	loop
		--raise info 'objectid: %, link_type: %, road_type: %, toll: %',  rec.objectid, rec.link_type, rec.road_type, rec.toll;
		if in_link_type = 5 and rec.link_type not in (3, 5, 7) then
			return true;
		end if;
		-- from HWY to Nwy
		if StartLinkAttr[1] in (0, 1) then
			if rec.link_type not in (0,1) and rec.toll <> 1 then
				return true;
			end if;
		end if;
		-- from Toll to non toll road
		if (StartLinkAttr[3] = 1) then
			if rec.link_type not in (0,1) and rec.toll <> 1 then
				return true;
			end if;
		end if;
		
		if StartLinkAttr[1] not in (0, 1) and StartLinkAttr[3] <> 1 then
			if StartLinkAttr[2] < rec.road_type then
				return true;
			end if;
		end if;
	end loop;
	
	return false;
END;
$$;

---------------------------------------------------------------------------------------------------
-- Get facility name.
---------------------------------------------------------------------------------------------------
create or replace function mid_get_facility_name(nHwyNode bigint, nPosCode integer, nFacNode bigint, FacTypeArray integer[])
	RETURNS varchar[]
	LANGUAGE plpgsql
AS $$ 
DECLARE
	--rstFacName       varchar;
	nCount           integer;
	rec              record;
	mesh_code        varchar;
	road_code        varchar;
	seq_num          varchar;
	pos_code         varchar;
	updown_flag      varchar;
	fac_type         varchar;
	fac_name         varchar;
BEGIN
	nCount     = 0;
	--rstFacName = NULL;
	if (nHwyNode is null ) or nFacNode is null then
		raise EXCEPTION 'nHwyNode or nFacNode is null, nHwyNode: %, nFacNode: %', nHwyNode, nFacNode;
		--return NULL;
	end if;
	
	for rec in execute
		'
			select a.mesh_code, a.road_code, a.seq_num, a.pos_code, a.updown_flag, fac_type, fac_name
			  from (
				select *
				  from road_hwynode
				  where node_id = ' || nHwyNode || ' and pos_code = ' || nPosCode || '
			  ) as a
			  left join (
				SELECT gid, node_id, point_mesh_code, point_x, point_y, mesh_id, fac_mesh_code, 
				       fac_x, fac_y, addr_id, road_point, fac_name, fac_yomi, road_code, 
				       fac_type, point_pos
				  FROM mid_road_roadpoint_node
				  where node_id = ' || nFacNode || ' and fac_type in (' || array_to_string(FacTypeArray, ',') || ')
			  ) as b
			  on a.road_code = b.road_code and a.seq_num = b.road_point
			  where b.gid is not null
		'
	loop
		if nCount = 0 then
			--rstFacName = rec.fac_name;
			mesh_code   = rec.mesh_code::varchar;
			road_code   = rec.road_code::varchar;
			seq_num     = rec.seq_num::varchar;
			pos_code    = rec.pos_code::varchar;
			updown_flag = rec.updown_flag::varchar;
			fac_type    = rec.fac_type::varchar;
			fac_name    = rec.fac_name;
		end if;
		
		nCount = nCount + 1;
	end loop;

	if nCount > 1 or  nCount = 0 then
		raise info 'the number of fac name is %, nHwyNode: %, nFacNode: %', nCount, nHwyNode, nFacNode;
	end if;

	return array[mesh_code, road_code, seq_num, pos_code, updown_flag, fac_type, fac_name];
END;
$$;

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
		from road_rlk 
		where ((fromnodeid = nodeid_list[iterator] and tonodeid = nodeid_list[iterator +1]) 
		OR (tonodeid = nodeid_list[iterator] and fromnodeid = nodeid_list[iterator +1]));

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