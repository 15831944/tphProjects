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