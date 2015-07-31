-------------------------------------------------------------------------------------------------------------
-- common
-------------------------------------------------------------------------------------------------------------

CREATE OR REPLACE FUNCTION mid_findInnerPath(nMeshID integer, nFromLink integer, nToLink integer, nTargetCross integer)
	RETURNS varchar[]
	LANGUAGE plpgsql
AS $$ 
DECLARE
	rstPathArray		varchar[];
	rstPathCount		integer;
	rstPath				varchar;
	
	tmpPathArray		varchar[];
	tmpLastNodeArray	bigint[];
	tmpLastLinkArray	bigint[];
	tmpPathCount		integer;
	tmpPathIndex		integer;
	
	nStartNode			integer;
	nEndNode			integer;
	
	nNewFromLink		bigint;
	nNewToLink			bigint;
	
	rec        			record;
BEGIN
	-- init
	execute
	'
	 select (
		 case
		 when fnode in
			(
			 select node
			 from org_roadnode
			 where meshid = '||nMeshID||' and comp_node = '||nTargetCross||'
			)
		 then fnode
		 else tnode
		 end
		)as curnode
	 from org_roadsegment
	 where meshid = '||nMeshID||' and road = ' ||nFromLink|| '
	'
	into nStartNode
	;
	
	execute
	'
	 select (
		 case
		 when fnode in
			(
			 select node
			 from org_roadnode
			 where meshid = '||nMeshID||' and comp_node = '||nTargetCross||'
			)
		 then fnode
		 else tnode
		 end
		)as curnode
	 from org_roadsegment
	 where meshid = '||nMeshID||' and road = ' ||nToLink|| '
	'
	into nEndNode
	;
	
	execute
	'
	 select new_road
	 from temp_link_mapping
	 where meshid = '||nMeshID||' and road = ' ||nFromLink|| '
	'
	into nNewFromLink
	;
	
	execute
	'
	 select new_road
	 from temp_link_mapping
	 where meshid = '||nMeshID||' and road = ' ||nToLink|| '
	'
	into nNewToLink
	;
	
	--rstPathArray
	rstPathCount		:= 0;
	tmpPathArray		:= ARRAY[cast(nNewFromLink as varchar)];
	tmpLastNodeArray	:= ARRAY[nStartNode];
	tmpLastLinkArray	:= ARRAY[nNewFromLink];
	tmpPathCount		:= 1;
	tmpPathIndex		:= 1;

	-- search
	WHILE tmpPathIndex <= tmpPathCount LOOP
		--raise INFO '%', cast(tmpPathArray as varchar);
		if tmpLastNodeArray[tmpPathIndex] = nEndNode then
			rstPathCount		:= rstPathCount + 1;
			rstPath				:= cast(tmpPathArray[tmpPathIndex]||'|'||nNewToLink as varchar);
			rstPath				:= replace(rstPath, '(2)', '');
			rstPath				:= replace(rstPath, '(3)', '');
			if rstPathCount = 1 then
				rstPathArray	:= ARRAY[rstPath];
			else
				rstPathArray	:= array_append(rstPathArray, rstPath);
			end if;
		else
			for rec in execute
				'
				 select b.new_road as nextroad, a.dir, a.nextnode
				 from
				 (
					 select meshid, road as nextroad, ''(2)'' as dir, tnode as nextnode
					 from org_roadsegment
					 where	(meshid = '||nMeshID||') and
					 		(fnode = ' ||tmpLastNodeArray[tmpPathIndex]|| ') and 
							(direction in (1,2)) and
							(
							tnode in
								(
								 select node
								 from org_roadnode
								 where meshid = '||nMeshID||' and comp_node = '||nTargetCross||'
								)
							)
					 union
					 
					 select meshid, road as nextroad, ''(3)'' as dir, fnode as nextnode
					 from org_roadsegment
					 where	(meshid = '||nMeshID||') and
					 		(tnode = ' ||tmpLastNodeArray[tmpPathIndex]|| ') and
							(direction in (1,3)) and
							(
							fnode in
								(
								 select node
								 from org_roadnode
								 where meshid = '||nMeshID||' and comp_node = '||nTargetCross||'
								)
							)
				 )as a
				 left join temp_link_mapping as b
				 on a.meshid = b.meshid and a.nextroad = b.road
				'
			loop
				if 		not (rec.nextroad in (nNewFromLink, nNewToLink, tmpLastLinkArray[tmpPathIndex]))
					and	not ((rec.nextroad||rec.dir) = ANY(regexp_split_to_array(tmpPathArray[tmpPathIndex], E'\\|+')))
				then
					tmpPathCount		:= tmpPathCount + 1;
					tmpPathArray		:= array_append(tmpPathArray, cast(tmpPathArray[tmpPathIndex]||'|'||rec.nextroad||rec.dir as varchar));
					tmpLastNodeArray	:= array_append(tmpLastNodeArray, cast(rec.nextnode as bigint));
					tmpLastLinkArray	:= array_append(tmpLastLinkArray, cast(rec.nextroad as bigint));
				end if;
			end loop;
		end if;
		tmpPathIndex := tmpPathIndex + 1;
	END LOOP;

	return rstPathArray;
END;
$$;


CREATE OR REPLACE FUNCTION mid_findTargetNodeIndex(fnode_array bigint[], tnode_array bigint[], nTargetNode bigint)
    RETURNS smallint
    LANGUAGE plpgsql
AS $$
DECLARE
	nCount					integer;
	nIndex					integer;
BEGIN
	nCount	:= array_upper(fnode_array, 1);
	for nIndex in 1..(nCount-1) loop
		if (fnode_array[nIndex] = nTargetNode) or (tnode_array[nIndex] = nTargetNode) then
			return nIndex;
		end if;
	end loop;
	raise INFO 'signpost wrong: target node is not on the path';
	return 0;
END;
$$;

-------------------------------------------------------------------------------------------------------------
-- regulation
-------------------------------------------------------------------------------------------------------------

CREATE OR REPLACE FUNCTION mid_convert_condition_regulation_tbl()
    RETURNS smallint
    LANGUAGE plpgsql
AS $$
DECLARE
	rec				record;
	nTimeCount		integer;
	nTimeIndex		integer;
	nDate1Count		integer;
	nDate1Index		integer;
	nTempPos		integer;
	
	cur_cond_id		integer;
	car_type		smallint;
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
	
	strDateTime		varchar;
	strTime			varchar;
	strAndDate1		varchar;
	strAndDate2		varchar;
	strNotDate		varchar;
	
	time_array		varchar[];
	strSubTime		varchar;
	
	strStartTime	varchar;
	strLastTime		varchar;
	nHourPos		integer;
	nMinPos			integer;
	
	date1_array		varchar[];
	strSubDate		varchar;
	
	strStartDate	varchar;
	strLastDate		varchar;
	
	nStartWeekT		integer;
	nLastWeekD		integer;
	nWeekDay		integer;
BEGIN
	cur_cond_id := 0;
	
	for rec in 
		select rule_type, vehicle, rule_time
		from
		(
	        select rule_type, vehicle, rule_time
	        from org_roadnoderule
	        union
	        select rule_type, vehicle, rule_time
	        from org_roadcrossrule
        )as t
        group by rule_type, vehicle, rule_time
        order by rule_type, vehicle, rule_time
    LOOP
    	-- car_type
    	car_type	:= mid_get_access(rec.vehicle, rec.rule_type);
    	
    	-- special time
    	if strpos(rec.rule_time, 'h') = 0 and strpos(rec.rule_time, 'm') = 0 then
    		strDateTime		:= '[(h0){h24}]*' || rec.rule_time;
    	else
    		strDateTime		:= rec.rule_time;
    	end if;
    	strDateTime			:= replace(strDateTime, '+[(z13){z63}]+[(z14){z64}]', '');
    	strDateTime			:= replace(strDateTime, '[(z13){z63}]+[(z14){z64}]+', '');
    	
    	-- split time 
    	-- for example1: "[[[[[(h7m40){m20}]+[(h12){m15}]+[(h13m10){m20}]]*[[(M1d1){M4}]+[(M10d1){M2d31}]]]*[(z8){z58}]]-[(z4){z54}]]"
		---------------> strTime			:= [[[[[(h7m40){m20}]+[(h12){m15}]+[(h13m10){m20}]];
		---------------> strAndDate1		:= *[[(M1d1){M4}]+[(M10d1){M2d31}]]];
		---------------> strAndDate2		:= *[(z8){z58}]];
		---------------> strNotDate			:= -[(z4){z54}]];
		
		nTempPos		:= strpos(strDateTime, '-');
		if nTempPos = 0 then
			strNotDate	:= null;
		else
			strNotDate	:= substring(strDateTime, nTempPos);
			strDateTime	:= substring(strDateTime, 1, nTempPos-1);
		end if;
    	
    	nTempPos		:= strpos(strDateTime, '*');
    	if nTempPos = 0 then
    		strTime		:= strDateTime;
    		strDateTime	:= null;
    	else
    		strTime		:= substring(strDateTime, 1, nTempPos-1);
			strDateTime	:= substring(strDateTime, nTempPos+1);
    	end if;
    	
    	if strDateTime is null then
    		strAndDate1	:= null;
    		strAndDate2	:= null;
    	elseif strDateTime = '[[(z5){z55}]+[(z6){z56}]+[(z7){z57}]+[(z8){z58}]]]' then
    		strAndDate1	:= null;
    		strAndDate2	:= null;
    	else
	    	nTempPos		:= strpos(strDateTime, '*');
	    	if nTempPos = 0 then
	    		strAndDate1	:= '*' || strDateTime;
	    		strAndDate2	:= null;
	    	else
	    		strAndDate1	:= '*' || substring(strDateTime, 1, nTempPos-1);
	    		strAndDate2	:= substring(strDateTime, nTempPos);
	    	end if;
    	end if;
    	
    	-- for every datetime
    	time_array	:= regexp_split_to_array(strTime, E'\\++');
    	if strAndDate1 is null then
    		date1_array	:= ARRAY[null];
    	else
    		date1_array	:= regexp_split_to_array(strAndDate1, E'\\++');
    	end if;
    	
    	nTimeCount	:= array_upper(time_array, 1);
    	nDate1Count	:= array_upper(date1_array, 1);
    	
    	for nTimeIndex in 1..nTimeCount loop
	    	strSubTime			:= time_array[nTimeIndex];
			--raise INFO 'strSubTime = %', strSubTime;
	    	
	    	-- time init
			start_hour		:= 0;
			start_minute	:= 0;
			end_hour		:= 24;
			end_minute		:= 0;
			
	    	-- time set
	    	strStartTime	:= substring(strSubTime, strpos(strSubTime, '(')+1, strpos(strSubTime, ')')-strpos(strSubTime, '(')-1);
	    	strLastTime		:= substring(strSubTime, strpos(strSubTime, '{')+1, strpos(strSubTime, '}')-strpos(strSubTime, '{')-1);
	    	if substring(strStartTime, 1, 1) not in ('h', 'm') or substring(strLastTime, 1, 1) not in ('h', 'm') then
	    		raise EXCEPTION 'unhandled ruletime type1, rule_time = %', rec.rule_time;
	    	end if;
	    	
	    	nMinPos			:= strpos(strStartTime, 'm');
	    	if nMinPos = 0 then
	    		start_hour		:= cast(substring(strStartTime, 2) as integer);
	    		start_minute	:= 0;
	    	elseif nMinPos = 1 then
	    		start_hour		:= 0;
	    		start_minute	:= cast(substring(strStartTime, nMinPos+1) as integer);
	    	else
	    		start_hour		:= cast(substring(strStartTime, 2, nMinPos-2) as integer);
	    		start_minute	:= cast(substring(strStartTime, nMinPos+1) as integer);
	    	end if;
	    	
	    	nMinPos			:= strpos(strLastTime, 'm');
	    	if nMinPos = 0 then
	    		end_hour		:= start_hour + cast(substring(strLastTime, 2) as integer);
	    		end_minute		:= start_minute;
	    	elseif nMinPos = 1 then
	    		end_hour		:= start_hour;
	    		end_minute		:= start_minute + cast(substring(strLastTime, nMinPos+1) as integer);
	    	else
	    		end_hour		:= start_hour + cast(substring(strLastTime, 2, nMinPos-2) as integer);
	    		end_minute		:= start_minute + cast(substring(strLastTime, nMinPos+1) as integer);
	    	end if;
	    	
	    	-- update time format
	    	if end_minute >= 60 then
	    		end_minute		:= end_minute - 60;
	    		end_hour		:= end_hour + 1;
	    	end if;
	    	if end_hour >= 24 then
	    		end_hour		:= end_hour - 24;
	    	end if;
	    	if end_hour = 0 and end_minute = 0 then
	    		end_hour		:= 23;
	    		end_minute		:= 59;
	    	end if;
			
    		for nDate1Index in 1..nDate1Count loop
	    		strSubDate		:= date1_array[nDate1Index];
				--raise INFO 'strSubDate = %', strSubDate;
	    		
				-- date init
				start_year		:= 0;
				start_month		:= 0;
				start_day		:= 0;
				end_year		:= 0;
				end_month		:= 0;
				end_day			:= 0;
				day_of_week		:= 0;
				exclude_date	:= 0;
				
				-- subdate1
				if strSubDate is not null then
					strStartDate	:= substring(strSubDate, strpos(strSubDate, '(')+1, strpos(strSubDate, ')')-strpos(strSubDate, '(')-1);
					strLastDate		:= substring(strSubDate, strpos(strSubDate, '{')+1, strpos(strSubDate, '}')-strpos(strSubDate, '{')-1);
					if substring(strStartDate,1,1) = 't' and substring(strLastDate,1,1) = 'd' then
						nStartWeekT		:= cast(substring(strStartDate, 2) as integer);
						nLastWeekD		:= cast(substring(strLastDate, 2) as integer);
						for nWeekDay in (nStartWeekT-1)..(nStartWeekT+nLastWeekD-2) loop
							if nWeekDay >= 7 then
								day_of_week	:= day_of_week | (1<<(nWeekDay-7));
							else 
								day_of_week	:= day_of_week | (1<<nWeekDay);
							end if;
						end loop;
					elseif substring(strStartDate,1,1) = 'M' then
						nTempPos		:= strpos(strStartDate, 'd');
						start_month		:= cast(substring(strStartDate, 2, nTempPos-2) as integer);
						start_day		:= cast(substring(strStartDate, nTempPos+1) as integer);
						nTempPos		:= strpos(strLastDate, 'd');
						if substring(strLastDate, 1, 1) = 'M' and nTempPos != 0 then
							end_month		:= start_month + cast(substring(strLastDate, 2, nTempPos-2) as integer);
							end_day			:= start_day + cast(substring(strLastDate, nTempPos+1) as integer) - 1;
						elseif substring(strLastDate, 1, 1) = 'M' and nTempPos = 0 then
							end_month		:= start_month + cast(substring(strLastDate, 2) as integer);
							end_day			:= start_day;
						else
							end_month		:= start_month;
							end_day			:= start_day;
						end if;
						if end_day > 31 then
							end_day		:= end_day - 31;
							end_month 	:= end_month + 1;
						end if;
						if end_month > 12 then
							end_month 	:= end_month - 12;
						end if;
					elseif substring(strStartDate,1,2) = 'z4' and substring(strLastDate,1,3) = 'z54' then
						day_of_week		:= 127;
						day_of_week		:= day_of_week | (1<<7);
						day_of_week		:= day_of_week | (1<<9);
					elseif substring(strStartDate,1,2) = 'z5' and substring(strLastDate,1,3) = 'z55' then
						start_month		:= 2;
						start_day		:= 1;
						end_month		:= 4;
						end_day			:= 30;
					elseif substring(strStartDate,1,2) = 'z6' and substring(strLastDate,1,3) = 'z56' then
						start_month		:= 5;
						start_day		:= 1;
						end_month		:= 7;
						end_day			:= 31;
					elseif substring(strStartDate,1,2) = 'z7' and substring(strLastDate,1,3) = 'z57' then
						start_month		:= 8;
						start_day		:= 1;
						end_month		:= 10;
						end_day			:= 31;
					elseif substring(strStartDate,1,2) = 'z8' and substring(strLastDate,1,3) = 'z58' then
						start_month		:= 11;
						start_day		:= 1;
						end_month		:= 1;
						end_day			:= 31;
					else
						raise EXCEPTION 'unhandled ruletime type2, rule_time = %', rec.rule_time;
					end if;
				end if;
				
				-- and date2
				if strAndDate2 is not null then
					if end_month < start_month then
						end_month 	:= end_month + 12;
					end if;
					strStartDate	:= substring(strAndDate2, strpos(strAndDate2, '(')+1, strpos(strAndDate2, ')')-strpos(strAndDate2, '(')-1);
					strLastDate		:= substring(strAndDate2, strpos(strAndDate2, '{')+1, strpos(strAndDate2, '}')-strpos(strAndDate2, '{')-1);
					if substring(strStartDate,1,2) = 'z4' and substring(strLastDate,1,3) = 'z54' then
						if day_of_week = 0 then
							day_of_week	:= 127;
						end if;
						day_of_week		:= day_of_week | (1<<7);
						day_of_week		:= day_of_week | (1<<9);
					elseif substring(strStartDate,1,2) = 'z5' and substring(strLastDate,1,3) = 'z55' then
						if start_month = 0 or start_month < 2 then
							start_month		:= 2;
							start_day		:= 1;
						end if;
						if end_month = 0 or end_month > 4 then
							end_month		:= 4;
							end_day			:= 30;
						end if;
					elseif substring(strStartDate,1,2) = 'z6' and substring(strLastDate,1,3) = 'z56' then
						if start_month = 0 or start_month < 5 then
							start_month		:= 5;
							start_day		:= 1;
						end if;
						if end_month = 0 or end_month > 7 then
							end_month		:= 7;
							end_day			:= 31;
						end if;
					elseif substring(strStartDate,1,2) = 'z7' and substring(strLastDate,1,3) = 'z57' then
						if start_month = 0 or start_month < 8 then
							start_month		:= 8;
							start_day		:= 1;
						end if;
						if end_month = 0 or end_month > 10 then
							end_month		:= 10;
							end_day			:= 31;
						end if;
					elseif substring(strStartDate,1,2) = 'z8' and substring(strLastDate,1,3) = 'z58' then
						if start_month = 0 or start_month < 11 then
							start_month		:= 11;
							start_day		:= 1;
						end if;
						if end_month = 0 or end_month > 13 then
							end_month		:= 13;
							end_day			:= 31;
						end if;
					else
						raise EXCEPTION 'unhandled ruletime type3, rule_time = %', rec.rule_time;
					end if;
				end if;
				
				-- not date
				if strNotDate is not null then
					strStartDate	:= substring(strNotDate, strpos(strNotDate, '(')+1, strpos(strNotDate, ')')-strpos(strNotDate, '(')-1);
					strLastDate		:= substring(strNotDate, strpos(strNotDate, '{')+1, strpos(strNotDate, '}')-strpos(strNotDate, '{')-1);
					if substring(strStartDate,1,2) = 'z4' and substring(strLastDate,1,3) = 'z54' then
						if day_of_week = 0 then
							day_of_week	:= 127;
						end if;
						day_of_week	:= day_of_week | (1<<7);
						day_of_week	:= day_of_week | (1<<10);
					else
						raise EXCEPTION 'unhandled ruletime type2, rule_time = %', rec.rule_time;
					end if;
		    	end if;
		    	
		    	-- update date format
		    	if end_month in (1,3,5,7,8,10,12) and end_day > 31 then
	    			end_day	:= end_day - 31;
	    			end_month	:= end_month + 1;
		    	end if;
		    	
		    	if end_month in (4,6,9,11) and end_day > 30 then
	    			end_day	:= end_day - 30;
	    			end_month	:= end_month + 1;
		    	end if;
		    	
		    	if end_month = 2 and end_day > 28 then
	    			end_day	:= end_day - 28;
	    			end_month	:= end_month + 1;
		    	end if;
		    	
		    	if end_month > 12 then
		    		end_month	:= end_month - 12;
		    		if end_year != 0 then
		    			end_year	:= end_year + 1;
		    		end if;
		    	end if;
		    	
		    	-- set cond_id -1 if no car restriction
		    	if car_type = 0 then
			    	insert into temp_condition_regulation_tbl
			    				(
			    				"cond_id", "rule_type", "vehicle", "rule_time"
								)
			    		VALUES	(
			    				-1, rec.rule_type, rec.vehicle, rec.rule_time
			    				);
	    		-- insert new cond if condition is useful
	    		elseif not
	    			(
		    			(car_type = -2) and
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
			    	cur_cond_id := cur_cond_id + 1;
			    	
			    	insert into temp_condition_regulation_tbl
			    				(
			    				"cond_id", "rule_type", "vehicle", "rule_time"
								)
			    		VALUES	(
			    				cur_cond_id, rec.rule_type, rec.vehicle, rec.rule_time
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
    	end loop;
    END LOOP;
    return 1;
END;
$$;


CREATE OR REPLACE FUNCTION mid_get_access(vehicle varchar, rule_type smallint)
	RETURNS smallint
    LANGUAGE plpgsql
AS $$
DECLARE
	x			smallint;
	nVehicle	integer;
BEGIN
	x = 0;
	execute 'select cast(X''' || vehicle || ''' as integer)' into nVehicle;
	-- not local
	if rule_type in (2,3,6,7) then
		--modified on 20120817 for deleting ecdemic car restriction
		--x	:= x | 1;
		return 0;
	end if;
	-- all car type
	if (nVehicle&(1<<31)) != 0 then
		x	:= x | -2;
	end if;
	-- car
	if (nVehicle&(1<<30)) != 0 then
		x	:= x | (1<<13);
	end if;
	-- small car
	if (nVehicle&(1<<29)) != 0 then
		x	:= x | (1<<12);
	end if;
	-- small truck
	if (nVehicle&(1<<28)) != 0 then
		x	:= x | (1<<6);
	end if;
	-- large truck
	if (nVehicle&(1<<27)) != 0 then
		x	:= x | (1<<14);
	end if;
	-- tow truck
	if (nVehicle&(1<<26)) != 0 then
		x	:= x | ((cast(1 as smallint))<<15);
	end if;
	-- small passenger car
	if (nVehicle&(1<<25)) != 0 then
		x	:= x | (1<<10);
	end if;
	-- large passenger car
	if (nVehicle&(1<<24)) != 0 then
		x	:= x | (1<<7);
	end if;
	-- passerby
	if (nVehicle&(1<<0)) != 0 then
		x	:= x | (1<<4);
	end if;
	return x;
END;
$$;



CREATE OR REPLACE FUNCTION mid_convert_regulation_on_node()
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
    	select	x.new_node as node,
    			y.new_road as from_road,
    			z.new_road as to_road,
    			array_condid
    	from
    	(
			select	meshid,
					cast(node as integer) as node,
					cast(from_road as integer) as froad,
					cast(to_road as integer) as troad,
					array_agg(cond_id) as array_condid
			from
			(
				select  a.meshid, a.from_road, a.to_road, a.node, c.cond_id
				from
				(
					select	meshid, node, from_road, to_road, 
						generate_series(cast(rule as bigint), cast(rule+rule_cnt-1 as bigint)) as rule
					from
					(
						select meshid, node, from_road, to_road, rule, rule_cnt
						from org_roadnodemaat
						where rule != 0
					)as r
				)as a
				left join org_roadnoderule as b
				on a.meshid = b.meshid and a.rule = b.rule
				left join temp_condition_regulation_tbl as c
				on b.rule_type = c.rule_type and b.vehicle = c.vehicle and b.rule_time = c.rule_time
				where c.cond_id is null or c.cond_id != -1
				order by a.meshid, a.node, a.from_road, a.to_road, c.cond_id
			)as cc
			group by meshid, node, from_road, to_road
		)as org
		left join temp_node_mapping as x
		on org.meshid = x.meshid and org.node = x.node
		left join temp_link_mapping as y
		on org.meshid = y.meshid and org.froad = y.road
		left join temp_link_mapping as z
		on org.meshid = z.meshid and org.troad = z.road
		order by node, from_road, to_road
    LOOP
		-- current regulation id
    	cur_regulation_id := cur_regulation_id + 1;
		
    	-- insert into regulation_item_tbl
		insert into regulation_item_tbl
					("regulation_id", "linkid", "nodeid", "seq_num")
			VALUES	(cur_regulation_id, rec.from_road, null, 1);
		insert into regulation_item_tbl
					("regulation_id", "linkid", "nodeid", "seq_num")
			VALUES	(cur_regulation_id, null, rec.node, 2);
		insert into regulation_item_tbl
					("regulation_id", "linkid", "nodeid", "seq_num")
			VALUES	(cur_regulation_id, rec.to_road, null, 3);
		
	   	-- insert into regulation_relation_tbl
		nCount := array_upper(rec.array_condid, 1);
		nIndex := 1;
		while nIndex <= nCount LOOP
	    	insert into regulation_relation_tbl
	    						("regulation_id", "nodeid", "inlinkid", "outlinkid", "condtype", "cond_id")
	    				VALUES	(cur_regulation_id, rec.node, rec.from_road, rec.to_road, 1, rec.array_condid[nIndex]);
			nIndex := nIndex + 1;
		END LOOP;
    
    END LOOP;
    return 1;
END;
$$;


CREATE OR REPLACE FUNCTION mid_convert_regulation_on_cross()
    RETURNS smallint
    LANGUAGE plpgsql
AS $$
DECLARE
	rec					record;
	cur_regulation_id	integer;
	path_array			varchar[];
	link_array			bigint[];
	nPathIndex			integer;
	nPathCount			integer;
	nIndex				integer;
	nCount				integer;
BEGIN
	-- regulation_id
	select (case when max(regulation_id) is null then 0 else max(regulation_id) end)
	from regulation_relation_tbl
	into cur_regulation_id;
	
    FOR rec IN
    	select		org.meshid,
    				comp_node,
    				x.new_node,
					from_road,
					to_road,
					array_condid
		from
		(
	    	select		t.meshid,
	    				comp_node,
	    				(
						 case
						 when fr.fnode in
							(
							 select node
							 from org_roadnode
							 where meshid = t.meshid and comp_node = t.comp_node
							)
						 then fr.fnode
						 else fr.tnode
						 end
						)as node,
						from_road,
						to_road,
						array_condid
	    	from
	    	(
				select	meshid,
						cast(node as integer) as comp_node,
						cast(from_road as integer) as from_road,
						cast(to_road as integer) as to_road,
						array_agg(cond_id) as array_condid
				from
				(
					select  a.meshid, a.from_road, a.to_road, a.node, c.cond_id
					from
					(
						select	meshid, node, from_road, to_road, 
								generate_series(cast(rule as bigint), cast(rule+rule_cnt-1 as bigint)) as rule
						from
						(
							select meshid, node, from_road, to_road, rule, rule_cnt
							from org_roadcrossmaat
							where rule != 0
						)as r
					)as a
					left join org_roadcrossrule as b
					on a.meshid = b.meshid and a.rule = b.rule
					left join temp_condition_regulation_tbl as c
					on b.rule_type = c.rule_type and b.vehicle = c.vehicle and b.rule_time = c.rule_time
					where c.cond_id is null or c.cond_id != -1
					order by a.meshid, a.node, a.from_road, a.to_road, c.cond_id
				)as cc
				group by meshid, node, from_road, to_road
			)as t
			left join org_roadsegment as fr
			on t.meshid = fr.meshid and t.from_road = fr.road
		)as org
		left join temp_node_mapping as x
		on org.meshid = x.meshid and org.node = x.node
		order by new_node, from_road, to_road
    LOOP
		-- paths
		path_array	:= mid_findInnerPath(rec.meshid, rec.from_road, rec.to_road, rec.comp_node);
		
		-- for every path
		if path_array is null then
			nPathCount	:= 0;
		else
			nPathCount	:= array_upper(path_array, 1);
		end if;
		for nPathIndex in 1..nPathCount loop
			-- current regulation id
	    	cur_regulation_id := cur_regulation_id + 1;
	    	
	    	-- insert into regulation_item_tbl
	    	link_array	:= cast(regexp_split_to_array(path_array[nPathIndex], E'\\|+') as bigint[]);
	    	nCount		:= array_upper(link_array, 1);
	    	for nIndex in 1..nCount loop
	    		if nIndex = 1 then
					insert into regulation_item_tbl
								("regulation_id", "linkid", "nodeid", "seq_num")
						VALUES	(cur_regulation_id, link_array[nIndex], null, nIndex);
					insert into regulation_item_tbl
								("regulation_id", "linkid", "nodeid", "seq_num")
						VALUES	(cur_regulation_id, null, rec.new_node, nIndex+1);
				else
					insert into regulation_item_tbl
								("regulation_id", "linkid", "nodeid", "seq_num")
						VALUES	(cur_regulation_id, link_array[nIndex], null, nIndex+1);
				end if;
			end loop;
			
		   	-- insert into regulation_relation_tbl
			nCount := array_upper(rec.array_condid, 1);
	    	for nIndex in 1..nCount loop
		    	insert into regulation_relation_tbl
		    						("regulation_id", "nodeid", "inlinkid", "outlinkid", "condtype", "cond_id")
		    				VALUES	(cur_regulation_id, rec.new_node, link_array[1], link_array[nCount], 1, rec.array_condid[nIndex]);
			end loop;
    	end loop;
    
    END LOOP;
    return 1;
END;
$$;


CREATE OR REPLACE FUNCTION mid_get_connected_nodeid(fromnodeid1 integer, tonodeid1 integer, fromnodeid2 integer, tonodeid2 integer)
  RETURNS integer 
  LANGUAGE plpgsql
  AS
$$ 
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

-------------------------------------------------------------------------------------------------------------
-- Full-width ==> half-width
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_translate(_name character varying)
  RETURNS character varying 
  LANGUAGE plpgsql 
  AS $$
begin
	return translate(_name, '���£ãģţƣǣȣɣʣˣ̣ͣΣϣУѣңӣԣأ٣ڣգ֣ף�������������������', 'ABCDEFGHIJKLMNOPQRSTXYZUVW1234567890');
END;
$$;
-------------------------------------------------------------------------------------------------------------
-- Dictionary
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_add_one_name(n_id integer, _name character varying, py character varying, lang_type integer)
  RETURNS integer 
  LANGUAGE plpgsql 
  AS $$
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
		values(n_id, _name, py, lang_type);
    
	return n_id;
END;
$$;

-------------------------------------------------------------------------------------------------------------
-- road name, road alias name, node signpost, cross signpost
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_make_rass_name_dictionary()
 RETURNS integer
    LANGUAGE plpgsql
    AS  
$$ 
DECLARE
    curs1 refcursor;
    gid_lid integer[];
    gid_id integer;
    _name_chn character varying; 
    _name_trd character varying; 
    _name_py character varying;
    _name_eng character varying;
    _name_ctn character varying;
    cnt integer;
    curr_name_id integer;
    i integer;
BEGIN
	curr_name_id := 0;
	cnt          := 0;
	OPEN curs1 FOR SELECT gid_array, name_chn, name_trd, name_py, name_eng, name_ctn
					FROM (
						SELECT array_agg(gid) as gid_array, name_chn, 
							   name_trd, name_py, name_eng, name_ctn
						  FROM temp_org_rass_name
						  group by name_chn, name_trd, name_py, name_eng, name_ctn
					) AS A
					order by name_chn, name_trd, name_py, name_eng, name_ctn;
    
	-- Get the first record        
	FETCH curs1 INTO gid_lid, _name_chn, _name_trd, _name_py, _name_eng, _name_ctn;
	while gid_lid is not null LOOP
		-- Simplified Chinese and PinYin
		IF _name_chn IS NOT NULL THEN
			curr_name_id := mid_add_one_name(curr_name_id, mid_translate(_name_chn), _name_py, 1);
		END IF;
		-- Traditional Chinese and PinYin
		IF _name_trd IS NOT NULL THEN
			curr_name_id := mid_add_one_name(curr_name_id, mid_translate(_name_trd), _name_py, 2);
		END IF;
		-- English
		IF _name_eng IS NOT NULL THEN
			curr_name_id := mid_add_one_name(curr_name_id, _name_eng, null, 3);
		END IF;
		-- GuangDong
		IF _name_ctn IS NOT NULL THEN
			curr_name_id := mid_add_one_name(curr_name_id, _name_ctn, null, 54);
		END IF;
		-- They use the same name_id

		i := 1;
		while i <= array_upper(gid_lid, 1) LOOP
			gid_id = gid_lid[i];
			insert into temp_rass_name("gid", "new_name_id")
				values (gid_id, curr_name_id);
			i := i + 1;
		END LOOP;
		
		curr_name_id := curr_name_id + 1;
		cnt          := cnt + 1;
		-- Get the next record
		FETCH curs1 INTO gid_lid, _name_chn, _name_trd, _name_py, _name_eng, _name_ctn;
	END LOOP;
	close curs1;

    return cnt;
END;
$$;

-------------------------------------------------------------------------------------------------------------
-- road number
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_make_road_number_dictionary()
 RETURNS integer
    LANGUAGE plpgsql
    AS  
$$ 
DECLARE
    curs1 refcursor;
    gid_lid integer[];
    gid_id integer;
    _route_no character varying; 
    cnt integer;
    curr_name_id integer;
    i integer;
    temp_road_id integer;
    temp_meshid integer;
    temp_seq_nm integer;
    tr_route_no character varying;
BEGIN
	curr_name_id := 0;
	cnt          := 0;
	OPEN curs1 FOR select gid_array, merged_route_no
				  from (
					select array_agg(gid) as gid_array, (shield_id || E'\t' || route_no) as merged_route_no
					  from (
						select gid, mid_get_shield_id(mid_translate(route_no), road_class)::varchar as shield_id, 
						       mid_translate(route_no) as route_no
						  from temp_road_number_old
					  ) as a
					  group by shield_id || E'\t' || route_no
				  ) as b
				  order by merged_route_no;

	-- Get the first record        
	FETCH curs1 INTO gid_lid, _route_no;
	while gid_lid is not null LOOP

		IF _route_no IS NOT NULL THEN
			--tr_route_no  := mid_translate(_route_no);
			curr_name_id := mid_add_one_name(curr_name_id, _route_no, null, 1);  -- Simplified Chinese and PinYin
			curr_name_id := mid_add_one_name(curr_name_id, _route_no, null, 2);  -- Traditional Chinese and PinYin
			curr_name_id := mid_add_one_name(curr_name_id, _route_no, null, 3);  -- English
		END IF;
		
		-- They use the same name_id

		i := 1;
		while i <= array_upper(gid_lid, 1) LOOP
			gid_id = gid_lid[i];
			SELECT road, meshid, seq_nm INTO temp_road_id, temp_meshid, temp_seq_nm 
			  from temp_road_number_old where gid = gid_id;
			insert into temp_road_number("road", "meshid", "new_name_id", seq_nm)
				values (temp_road_id, temp_meshid, curr_name_id, temp_seq_nm);
			i   := i + 1;
			cnt := cnt + 1;
		END LOOP;
		
		curr_name_id := curr_name_id + 1;
		
		-- Get the next record
		FETCH curs1 INTO  gid_lid, _route_no;
	END LOOP;
	close curs1;

    return cnt;
END;
$$;

-------------------------------------------------------------------------------------------------------------
-- node name
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_make_node_name_dictionary()
 RETURNS integer
    LANGUAGE plpgsql
    AS  
$$ 
DECLARE
    curs1 refcursor;
    road_lid bigint[];
    node_id bigint;
    _name_chn character varying; 
    _name_trd character varying; 
    _name_py character varying;
    _name_eng character varying;
    _name_ctn character varying;
    cnt integer;
    curr_name_id integer;
    i integer;
BEGIN
	curr_name_id := 0;
	cnt          := 0;
	OPEN curs1 FOR SELECT node_array, name_chn, name_trd, name_py, name_eng, name_ctn
					  FROM (
						SELECT array_agg(new_node) as node_array, name_chn, 
							   name_trd, name_py, name_eng, name_ctn
						  FROM temp_org_node_name
						  group by name_chn, name_trd, name_py, name_eng, name_ctn
					  ) AS A
					  ORDER BY name_chn, name_trd, name_py, name_eng, name_ctn;
    
	-- Get the first record        
	FETCH curs1 INTO road_lid, _name_chn, _name_trd, _name_py, _name_eng, _name_ctn;
	while road_lid is not null LOOP
		-- Simplified Chinese and PinYin
		IF _name_chn IS NOT NULL THEN
			curr_name_id := mid_add_one_name(curr_name_id, mid_translate(_name_chn), _name_py, 1);
		END IF;
		-- Traditional Chinese and PinYin
		IF _name_trd IS NOT NULL THEN
			curr_name_id := mid_add_one_name(curr_name_id, mid_translate(_name_trd), _name_py, 2);
		END IF;
		-- English
		IF _name_eng IS NOT NULL THEN
			curr_name_id := mid_add_one_name(curr_name_id, _name_eng, null, 3);
		END IF;
		-- GuangDong
		IF _name_ctn IS NOT NULL THEN
			curr_name_id := mid_add_one_name(curr_name_id, _name_ctn, null, 54);
		END IF;
		-- They use the same name_id

		i := 1;
		while i <= array_upper(road_lid, 1) LOOP
			node_id = road_lid[i];
			insert into temp_node_name("new_node", "new_name_id")
				values (node_id, curr_name_id);
			i := i + 1;
		END LOOP;
		
		curr_name_id := curr_name_id + 1;
		cnt          := cnt + 1;
		-- Get the next record
		FETCH curs1 INTO road_lid, _name_chn, _name_trd, _name_py, _name_eng, _name_ctn;
	END LOOP;
	close curs1;

    return cnt;
END;
$$;

-------------------------------------------------------------------------------------------------------------
-- node alias name
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_make_org_road_alias_name()
 RETURNS integer
    LANGUAGE plpgsql
    AS  
$$ 
DECLARE
BEGIN
	INSERT INTO temp_org_rass_name(
                            id, meshid, name_chn, name_trd, name_py, name_eng, name_ctn, seq_nm, name_type)
        (
	SELECT  road, meshid,
		regexp_split_to_table(alias_chn, E'\\|+'),
		regexp_split_to_table(alias_trd, E'\\|+'),
		regexp_split_to_table(alias_py, E'\\|+'),
		regexp_split_to_table(alias_eng, E'\\|+'),
		regexp_split_to_table(alias_ctn, E'\\|+'),
		generate_series(2, array_upper(regexp_split_to_array(alias_chn, E'\\|+'), 1) + 1) as seq_num,
		2
	  from  temp_org_road_alias_name
	);

	return 0;
END;
$$;

-------------------------------------------------------------------------------------------------------------
-- signpost_exit_no(split)
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_split_org_signpost_exit_no()
 RETURNS integer
    LANGUAGE plpgsql
    AS  
$$ 
DECLARE
BEGIN
	INSERT INTO temp_org_signpost_exit_no_split(
                            sp, meshid, exitno_chn, exitno_trd, exitno_py, exitno_eng, exitno_ctn, seq_nm, exit_type)
        (
	SELECT  sp, meshid, 
		regexp_split_to_table(exitno_chn, E'\\|+'),
		regexp_split_to_table(exitno_trd, E'\\|+'),
		regexp_split_to_table(exitno_py, E'\\|+'),
		regexp_split_to_table(exitno_eng, E'\\|+'),
		regexp_split_to_table(exitno_ctn, E'\\|+'),
		generate_series(1, array_upper(regexp_split_to_array(exitno_chn, E'\\|+'), 1)) as seq_num,
		exit_type
	  from  temp_org_signpost_exit_no
	);

	return 0;
END;
$$;

-------------------------------------------------------------------------------------------------------------
-- signpost_exitno
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_make_signpost_exitno_dictionary()
 RETURNS integer
    LANGUAGE plpgsql
    AS  
$$ 
DECLARE
    curs1 refcursor;
    gid_lid integer[];
    gid_id integer;
    _exitno_chn character varying; 
    _exitno_trd character varying; 
    _exitno_py character varying;
    _exitno_eng character varying;
    _exitno_ctn character varying;
    cnt integer;
    curr_name_id integer;
    temp_sp integer;
    temp_meshid integer;
    temp_seq_nm smallint;
    temp_exit_type smallint;
    i integer;
BEGIN
	curr_name_id := 0;
	cnt          := 0;
	OPEN curs1 FOR SELECT gid_array, exitno_chn, exitno_trd, exitno_py, exitno_eng, exitno_ctn
					  from (
						SELECT array_agg(gid) gid_array, exitno_chn, 
							   exitno_trd, exitno_py, exitno_eng, exitno_ctn
						  FROM temp_org_signpost_exit_no_split
						  group by exitno_chn, exitno_trd, exitno_py, exitno_eng, exitno_ctn
					  ) as a
					  order by exitno_chn, exitno_trd, exitno_py, exitno_eng, exitno_ctn;
    
	-- Get the first record        
	FETCH curs1 INTO gid_lid, _exitno_chn, _exitno_trd, _exitno_py, _exitno_eng, _exitno_ctn;
	while gid_lid is not null LOOP
		-- Simplified Chinese and PinYin
		IF _exitno_chn IS NOT NULL THEN
			curr_name_id := mid_add_one_name(curr_name_id, mid_translate(_exitno_chn), _exitno_py, 1);
		END IF;
		-- Traditional Chinese and PinYin
		IF _exitno_trd IS NOT NULL THEN
			curr_name_id := mid_add_one_name(curr_name_id, mid_translate(_exitno_trd), _exitno_py, 2);
		END IF;
		-- English
		IF _exitno_eng IS NOT NULL THEN
			curr_name_id := mid_add_one_name(curr_name_id, _exitno_eng, null, 3);
		END IF;
		-- GuangDong
		IF _exitno_ctn IS NOT NULL THEN
			curr_name_id := mid_add_one_name(curr_name_id, _exitno_ctn, null, 54);
		END IF;
		-- They use the same exitno_id

		i := 1;
		while i <= array_upper(gid_lid, 1) LOOP
			gid_id = gid_lid[i];
			SELECT sp, meshid, seq_nm, exit_type INTO temp_sp, temp_meshid, temp_seq_nm, temp_exit_type from temp_org_signpost_exit_no_split where gid = gid_id;

			if temp_exit_type = 3 then -- node 
				insert into temp_node_signpost_exitno_dict("sp", "meshid", "new_name_id", "seq_nm")
					values (temp_sp, temp_meshid, curr_name_id, temp_seq_nm);
			else                       -- cross
				insert into temp_cross_signpost_exitno_dict("sp", "meshid", "new_name_id", "seq_nm")
					values (temp_sp, temp_meshid, curr_name_id, temp_seq_nm);
			end if;
			
			i := i + 1;
		END LOOP;
		
		curr_name_id := curr_name_id + 1;
		cnt          := cnt + 1;
		-- Get the next record
		FETCH curs1 INTO gid_lid, _exitno_chn, _exitno_trd, _exitno_py, _exitno_eng, _exitno_ctn;
	END LOOP;
	close curs1;

    return cnt;
END;
$$;

-------------------------------------------------------------------------------------------------------------
-- signpost_entrno
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_make_signpost_entrno_dictionary()
 RETURNS integer
    LANGUAGE plpgsql
    AS  
$$ 
DECLARE
    curs1 refcursor;
    gid_lid integer[];
    gid_id integer;
    _entr_no character varying; 
    tr_entr_no character varying; 
    cnt integer;
    curr_name_id integer;
    temp_sp integer;
    temp_meshid integer;
    temp_entr_type smallint;
    i integer;
BEGIN
	curr_name_id := 0;
	cnt          := 0;
	OPEN curs1 FOR select gid_array, entr_no
					  from (
						select array_agg(gid) as gid_array, entr_no
						  from temp_org_signpost_entr_no
						  group by entr_no
					  ) as a
					  order by entr_no;
    
	-- Get the first record        
	FETCH curs1 INTO gid_lid, _entr_no;
	while gid_lid is not null LOOP
		-- Simplified Chinese and PinYin
		IF _entr_no IS NOT NULL THEN
			tr_entr_no   := mid_translate(_route_no);
			curr_name_id := mid_add_one_name(curr_name_id, tr_entr_no, null, 1);  -- Simplified Chinese and PinYin
			curr_name_id := mid_add_one_name(curr_name_id, tr_entr_no, null, 2);  -- Traditional Chinese and PinYin
			curr_name_id := mid_add_one_name(curr_name_id, tr_entr_no, null, 3);  -- English
		END IF;
		-- They use the same name_id

		i := 1;
		while i <= array_upper(gid_lid, 1) LOOP
			gid_id = gid_lid[i];
			SELECT sp, meshid, entr_type INTO temp_sp, temp_meshid, temp_entr_type from temp_org_signpost_entr_no where gid = gid_id;

			if temp_entr_type = 3 then -- node 
				insert into temp_node_signpost_exitno_dict("sp", "meshid",  "new_name_id")
					values (temp_sp, temp_meshid, curr_name_id);
			else                       -- cross
				insert into temp_cross_signpost_exitno_dict("sp", "meshid", "new_name_id")
					values (temp_sp, temp_meshid, curr_name_id);
			end if;
			
			i := i + 1;
		END LOOP;
		
		curr_name_id := curr_name_id + 1;
		cnt          := cnt + 1;
		-- Get the next record
		FETCH curs1 INTO gid_lid, _entr_no;
	END LOOP;
	close curs1;

    return cnt;
END;
$$;

CREATE OR REPLACE FUNCTION mid_cnv_road_type(
road_class integer,
form_way smallint,
link_type smallint,
status smallint,
ownership smallint,
name_chn character varying,
direction smallint
)
RETURNS smallint
LANGUAGE plpgsql
AS
$$

BEGIN

    IF link_type = 1 THEN 
    	return 10;
	ELSEIF status = 1 or status = 2 THEN
		return 9;
    ELSEIF ownership = 3 THEN 
    	return 7;
    ELSEIF direction = 4 THEN
    	return 8;
	ELSEIF road_class = 41000 THEN
		return 0;
	ELSEIF road_class = 43000 THEN
		return 1;
    ELSEIF road_class = 42000 THEN 
    	return 2;
    ELSEIF road_class = 51000 THEN
    	return 3;
    ELSEIF road_class = 52000 THEN
    	return 4;
    ELSEIF road_class = 44000 THEN
    	return 5;
    ELSEIF road_class = 49 THEN
    	return 8;
    ELSEIF road_class in (45000, 47000) and ownership = 1 THEN
    	return 14;
    ELSEIF road_class in (45000, 47000, 53000, 54000) THEN
    	return 6;
    ELSE
    	return 9;
    END if;

END;
$$;


CREATE OR REPLACE FUNCTION mid_cnv_display_class(road_class integer,fc smallint,link_type smallint)
  RETURNS smallint AS
$BODY$
BEGIN
    IF (link_type = 1) THEN 
        return 14;  --100; 
    END IF;
    
    IF (fc = 6) THEN
        return 3;  --6;
    END IF;
 
    IF (road_class = 41000) THEN 
        return 12;  --1; 
    END IF;
        
    IF (road_class = 43000) THEN 
        return 13;  --11; 
    END IF;
 
    IF (road_class = 42000) THEN 
        return 9;  --2; 
    END IF;
    
    IF (road_class = 51000) THEN 
        return 8;  --3; 
    END IF;
 
    IF (road_class = 44000) THEN 
        return 7;  --31; 
    END IF;
 
    IF (road_class = 52000) THEN 
        return 6;  --4; 
    END IF;
 
    IF (road_class = 53000) THEN 
        return 6;  --4; 
    END IF;
 
    IF (road_class = 54000) THEN 
        return 5;  --5; 
    END IF;
 
    IF (road_class = 45000) THEN 
        return 5;  --5; 
    END IF;
 
    IF (road_class = 47000) THEN 
        return 5;  --5; 
    END IF;
 
    RETURN 3;  --6;
 
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
ALTER FUNCTION mid_cnv_display_class(road_class integer,fc smallint,link_type smallint) OWNER TO postgres;


CREATE OR REPLACE FUNCTION mid_cnv_link_type( 
form_way smallint
)
  RETURNS smallint 
  LANGUAGE plpgsql
  AS
$$
BEGIN

    IF form_way = 4 THEN
    	return 0;
    ELSEIF form_way = 1 THEN
    	return 2;
    ELSEIF form_way in (3, 8, 53, 58) THEN
    	return 3;
    ELSEIF form_way = 2 THEN
    	return 4;
    ELSEIF form_way in (6, 9, 10) THEN
    	return 5;
    ELSEIF form_way = 7 THEN
    	return 6;
    ELSEIF form_way in (5, 56) THEN
    	return 7;
    ELSEIF form_way = 11 or form_way = 12 THEN
    	return 8;
    ELSEIF form_way = 13 or form_way = 14 THEN
    	return 9;
    ELSE
    	return 1;
    END IF;
    
END;
$$;


CREATE OR REPLACE FUNCTION mid_cnv_function_class( 
fc smallint
)
  RETURNS smallint 
  LANGUAGE plpgsql
  AS
$$
BEGIN

    IF fc = 1 THEN
    	return 1;
    ELSEIF fc = 2 THEN
    	return 2;
    ELSEIF fc = 3 THEN
    	return 3;
    ELSEIF fc = 4 THEN
    	return 4;
    ELSEIF fc = 5 or fc = 6 THEN
    	return 5;
    ELSE
    	return 5;
    END IF;
    
END;
$$;

CREATE OR REPLACE FUNCTION mid_get_lanecount_downflow(direction smallint, max_lanes smallint)
RETURNS smallint 
LANGUAGE plpgsql
AS
$$
BEGIN
	IF max_lanes is null THEN
		return 0;
	END IF;
	RETURN CASE
		WHEN direction in (1,2,4) THEN max_lanes
		ELSE 0
	END;
END;
$$;

CREATE OR REPLACE FUNCTION mid_get_lanecount_upflow(direction smallint, max_lanes smallint)
RETURNS smallint 
LANGUAGE plpgsql
AS
$$
BEGIN
	IF max_lanes is null THEN
		return 0;
	END IF;
	RETURN CASE
		WHEN direction in (1,3,4) THEN max_lanes
		ELSE 0
	END;
END;
$$;

CREATE OR REPLACE FUNCTION mid_getspeedclass
(
road_class integer
)
  RETURNS smallint
  LANGUAGE plpgsql VOLATILE
  AS $$
DECLARE
	rtnvalue INT;
BEGIN
    RETURN CASE 
        WHEN road_class = 41000 THEN 3
        WHEN road_class in (42000, 43000) THEN 4
        WHEN road_class in (44000, 51000) THEN 5
        WHEN road_class in (45000, 47000, 52000) THEN 6
        WHEN road_class = 53000 THEN 7
        WHEN road_class in (54000, 49) THEN 8
        ELSE 0 
    END;
END;
$$;

CREATE OR REPLACE FUNCTION mid_getregulationspeed
(
oneway smallint,
speed_limit_posted integer[],
speed_limit integer,
speedclass integer,
dir boolean
)
  RETURNS double precision
  LANGUAGE plpgsql VOLATILE
  AS $$
DECLARE
	rtnvalue INT;
BEGIN

	IF      (oneway = 2 and dir = false) 
		or (oneway = 3 and dir = true)
		or oneway = 4 THEN
			return 0;
	ELSE

		IF oneway in (2,3) and speed_limit_posted is not null THEN
			RETURN speed_limit_posted[1];
		ELSEIF oneway = 1 and dir is true and speed_limit_posted[1] is not null THEN
			RETURN speed_limit_posted[1];
		ELSEIF oneway = 1 and dir is false and speed_limit_posted[2] is not null THEN
			RETURN speed_limit_posted[2];
		ELSE
			IF speed_limit is not null and speed_limit > 0 THEN
				IF speed_limit > 200 THEN
					RETURN -1;
				ELSE
					return speed_limit;
				END IF;
			ELSE
				RETURN CASE 	WHEN speedclass=1 THEN 160
						WHEN speedclass=2 THEN 130
						WHEN speedclass=3 THEN 100
						WHEN speedclass=4 THEN 90
						WHEN speedclass=5 THEN 70
						WHEN speedclass=6 THEN 50
						WHEN speedclass=7 THEN 30
						WHEN speedclass=8 THEN 11
						ELSE 0
				END;
			END IF;  
		END IF;
	END IF;
END;
$$;
			    
CREATE OR REPLACE FUNCTION mid_getregulationspeed_source
(
oneway smallint,
speed_limit_posted int[],
speed_limit integer,
speedclass integer,
dir boolean
)
  RETURNS double precision
  LANGUAGE plpgsql VOLATILE
  AS $$
DECLARE
	rtnvalue INT;
BEGIN
	
	IF      (oneway = 2 and dir = false) 
		or (oneway = 3 and dir = true)
		or oneway = 4 THEN
			return 0;
	ELSE

		IF speed_limit_posted is not null THEN
			RETURN 1;
		ELSE
			IF speed_limit is not null and speed_limit > 0 THEN
				return 2;
			ELSE
				RETURN 3;
			END IF;  
		END IF;
	END IF;
END;
$$;

CREATE OR REPLACE FUNCTION mid_get_width
(
width double precision
)
RETURNS smallint
LANGUAGE plpgsql VOLATILE
AS 
$$
BEGIN
	
	IF width is null THEN
		return 4;
	END IF;
	
	IF width > 0 and width <= 3.0 THEN
		return 0;
	ELSEIF width <= 5.5 THEN
		return 1;
	ELSEIF width <= 13 THEN
		return 2;
	ELSEIF width > 13 THEN
		return 3;
	ELSE
		return 4;
	END IF;

END;
$$;

CREATE OR REPLACE FUNCTION mid_get_width_downflow
(
direction smallint,
width double precision
)
RETURNS smallint
LANGUAGE plpgsql VOLATILE
AS 
$$
BEGIN
	IF direction is null OR width = 0 THEN
		return 4;
	END IF;
	
    IF direction in (1, 2, 4) THEN
		return mid_get_width(width);
    ELSE 
    	return 4; 
    END IF;
END;
$$;

CREATE OR REPLACE FUNCTION mid_get_width_upflow
(
direction smallint,
width double precision
)
RETURNS smallint
LANGUAGE plpgsql VOLATILE
AS 
$$
BEGIN
	IF direction is null OR width = 0 THEN
		return 4;
	END IF;
	
    IF direction in (1, 3, 4) THEN
		return mid_get_width(width);
    ELSE 
    	return 4; 
    END IF;
END;
$$;

CREATE OR REPLACE FUNCTION mid_cnv_node_kind
(
nodetype smallint
)
  RETURNS character varying
  LANGUAGE plpgsql VOLATILE
AS $$

BEGIN
	IF nodetype is null THEN
		return null;
	END IF;
	
	IF nodetype = 2 or nodetype = 4 or nodetype = 5 THEN
		return '1301';
	ELSE
		return null;
	END IF;
	
END;
$$;

CREATE OR REPLACE FUNCTION mid_get_connect_node(nFromLink integer, nTargetCross integer, _meshid integer)
	RETURNS integer
	LANGUAGE plpgsql
AS $$ 
DECLARE
	connect_node		integer;
	--new_connect_node        bigint;
BEGIN
	select (
		 case
		 when fnode in
			(
			 select node
			 from org_roadnode
			 where comp_node = nTargetCross and meshid = _meshid
			) and 
			tnode in 
		 	(
			 select node
			 from org_roadnode
			 where comp_node = nTargetCross and meshid = _meshid
			)
		 then null
		 when fnode in
			(
			 select node
			 from org_roadnode
			 where comp_node = nTargetCross and meshid = _meshid
			)
		 then fnode
		 when tnode in 
		 	(
			 select node
			 from org_roadnode
			 where comp_node = nTargetCross and meshid = _meshid
			)
		 then tnode
		 else NULL
		 end
		)as curnode
	 into connect_node
	 from org_roadsegment
	 where road = nFromLink and meshid = _meshid;

	---- get new node id
	--SELECT new_node into new_connect_node from temp_node_mapping where node = connect_node and meshid = _meshid;
	
	--return new_connect_node;
	
	return connect_node;
END;
$$;

CREATE OR REPLACE FUNCTION mid_get_pass_lid(link_lid character varying, split_char character)
	RETURNS character varying
	LANGUAGE plpgsql
AS $$ 
DECLARE
	first_pos integer;
	last_pos integer;
	i integer;
BEGIN
	first_pos := 0;
	last_pos  := 0;
	IF link_lid IS NULL or split_char is null THEN 
		RETURN NULL;
	END IF;

	first_pos = strpos(link_lid, split_char);
	last_pos  = first_pos;
	i = first_pos + 1;
	while i <= length(link_lid) loop
		if substr(link_lid, i, 1) = split_char then
			last_pos = i;
		end if;
		i := i + 1;
	end loop;

	IF first_pos = last_pos THEN
		RETURN NULL;
	END IF;
	
	return substr(link_lid, first_pos + 1, last_pos - first_pos -1);
END;
$$;

CREATE OR REPLACE FUNCTION mid_string_count(link_lid character varying, split_char character)
	RETURNS integer
	LANGUAGE plpgsql
AS $$ 
DECLARE
	cnt integer;
	i integer;
BEGIN
	IF link_lid IS NULL or split_char is null THEN 
		RETURN 0;
	END IF;
	
	cnt = 0;
	i = 1;
	while i <= length(link_lid) loop
		if substr(link_lid, i, 1) = split_char then
			cnt := cnt + 1;
		end if;
		i := i + 1;
	end loop;

	return cnt;
END;
$$;

---------------------------------------------------------------------------------------------
-- Toward Name guideattr
---------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_cvt_tw_guideattr(sp_type smallint, cont_type smallint)
	RETURNS integer
	LANGUAGE plpgsql
AS $$ 
DECLARE
	
BEGIN
	if cont_type = 7 then  -- SA
		return 3;
	end if;

	if cont_type = 8 then  -- PA
		return 4;
	end if;
	
	return 
	case 
	when sp_type = 1 then 5 -- direction
	when sp_type = 2 then 1 -- Enter
	when sp_type = 3 then 2 -- Exit
	when sp_type = 4 then 0 -- default
	else 0
	end;
END;
$$;
---------------------------------------------------------------------------------------------
-- Toward Name Namekind
---------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_cvt_tw_namekind(sp_type smallint, cont_type smallint)
	RETURNS integer
	LANGUAGE plpgsql
AS $$ 
DECLARE
	
BEGIN
	IF sp_type = 2 then return 5; end if;  -- Enter
	IF sp_type = 3 then return 6; end if;  -- Exit

	return 
	case
	when cont_type = 1 then 4 -- Location directions
	when cont_type = 2 then 4 -- Route Name
	when cont_type = 3 then 4 -- Street Name
	when cont_type = 4 then 0 -- Tourist attractions Name
	when cont_type = 5 then 0 -- Service Facility Name
	when cont_type = 6 then 0 -- Service Facility Icon
	when cont_type = 7 then 2 -- SA
	when cont_type = 8 then 3 -- PA
	else 0
	end;
END;
$$;

CREATE OR REPLACE FUNCTION mid_has_additional_lane(all_lane_info varchar) -- return 0: not, 1: right-side has, 2:left-side has, 3: both-has
RETURNS smallint
LANGUAGE plpgsql
AS
$$ 
DECLARE 
	lanecount smallint;
	itor	smallint;
	current_char char;
	left_increase boolean;
	right_increase boolean;

BEGIN
	left_increase := false;
	right_increase := false;
	
	itor := 1;
	lanecount := length(all_lane_info);
	
	WHILE itor <= lanecount LOOP
		current_char := substring(all_lane_info, itor, 1);
		if current_char >= 'G' and current_char <= 'O' then
			left_increase := true;
		end if;

		if current_char >= 'R' and current_char <= 'Z' then
			right_increase := true;
		end if;

		itor := itor + 1;
	END LOOP;

	if (left_increase and right_increase) then
		return 3;
	elsif left_increase then
		return 2;
	elsif right_increase then
		return 1;
	else 
		return 0;
	end if;
END;
$$;

create or replace function mid_get_arrow_info(lane_info_target_direction varchar, lane_no smallint[])
returns smallint
LANGUAGE plpgsql
AS
$$ 
DECLARE
	ipos integer;
	current_char char;
BEGIN
	ipos := 1;
	if lane_no is null then
		lane_no = -1;
	end if;

	while ipos <= char_length(lane_info_target_direction) loop
--		if ipos = lane_no then
--			ipos := ipos + 1;
--			continue;
--		end if;

		if ipos = any(lane_no) then
			ipos := ipos + 1;
			continue;
		end if;

		current_char := substring(lane_info_target_direction, ipos, 1);
		if current_char = 'D' or current_char = 'Q' then
			ipos := ipos + 1;
			continue;
		end if;
		
		if current_char = '0' or current_char = 'G' or current_char = 'R' then
			return 1;
		elsif current_char = '1' or current_char = 'H' or current_char = 'Z' then 
			return 64;
		elsif current_char = '3' or current_char = 'O' or current_char = 'S' then 
			return 4;
		elsif current_char = '5' or current_char = 'I' then
			return 16;
		elsif current_char = '8' or current_char = 'T' then
			return 2048;
		else
			ipos := ipos + 1;
		end if;
	end loop;

	return -1;
END;
$$;

create or replace function mid_get_access_lane_info(all_lane_info varchar, target_arrow_info smallint, lane_array char[], lane_info_target_direction varchar)
returns varchar
LANGUAGE plpgsql
AS
$$ 
DECLARE
	current_char char;
	ipos integer;
	rtn_value smallint;
	total_lane_no integer;
	itor_cnt integer;
BEGIN
	ipos := 1;
	rtn_value := 0;
	itor_cnt := 0;
	total_lane_no := char_length(all_lane_info);
	
	if target_arrow_info = -1 then
		while ipos <= total_lane_no  loop
			current_char := substring(all_lane_info, ipos, 1);
			if current_char = 'D' then
				rtn_value := rtn_value | ((1::smallint) << (total_lane_no - ipos));
			end if;

			ipos := ipos + 1;
		end loop;
	else
		while ipos <= total_lane_no loop
			current_char := substring(all_lane_info, ipos, 1);
			if current_char = any(lane_array) then
				rtn_value := rtn_value | ((1::smallint) << (total_lane_no - ipos));
			end if;

			ipos := ipos + 1;
		end loop;
	end if;

	-- feedback to adjust lane info
--	if mid_get_specific_char_num(cast(rtn_value::int::bit(16) as varchar), '1') > char_length(lane_info_target_direction) then
--		-- match real lane postion
--		ipos := position(lane_info_target_direction in all_lane_info);
--
--		while itor_cnt <= char_length(lane_info_target_direction) loop
--			rtn_value := rtn_value | ((1::smallint) << (total_lane_no - ipos - itor_cnt));
--			itor_cnt := itor_cnt + 1;
--		end loop;
--	end if;

	return substring(cast(rtn_value::int::bit(16) as varchar), 17 - total_lane_no, total_lane_no);
END;
$$;

create or replace function mid_get_lane_xlpath_info(start_xlpath bigint, path_cnt bigint,meshid integer)
returns bigint[]
LANGUAGE plpgsql
AS
$$ 
DECLARE
	curs1 refcursor;
	temp_path_links bigint[];
BEGIN
	OPEN curs1 for execute 'select array_agg(new_road) from (
	select gid, xlpath, xlpath_id, globl_mesh_id, new_road, area_flag from temp_org_xlpath  where xlpath between ' 
	|| cast(start_xlpath as varchar) || ' and '
	|| cast(start_xlpath + path_cnt -1 as varchar) || 'and meshid = ' || cast(meshid as varchar) || ' order by xlpath ) as A' ;
	
	FETCH curs1 INTO temp_path_links;

	if temp_path_links is not null then
		return temp_path_links;
	else
		return null;
	end if;

	close curs1;
END;
$$;

CREATE OR REPLACE FUNCTION mid_get_cross_xlpath_access_lane(lane_no smallint, lane_info_target_direction varchar, all_lane_info varchar, lane_array char[])
returns varchar
LANGUAGE plpgsql
AS
$$ 
DECLARE
	rtn_value smallint;
	search_char char;
	ipos integer;
	total_lane_num integer;
BEGIN
	-- delete this condition
	-- search_char :=  substring(lane_info_target_direction, lane_no, 1);

	--ipos := 1;
	ipos := lane_no;

	total_lane_num := length(all_lane_info);
	
	while ipos <= total_lane_num loop
		if substring(all_lane_info, ipos, 1) = any(lane_array) then
			exit;
		end if;

		ipos := ipos + 1;
	end loop;

	rtn_value := (1::smallint) << (total_lane_num - ipos);

	return substring(cast(rtn_value::int::bit(16) as varchar), 17 - total_lane_num, total_lane_num );
	
END;
$$;


CREATE OR REPLACE FUNCTION mid_get_specific_char_num(org_str varchar, find_char char)
returns int
LANGUAGE plpgsql
AS
$$ 
DECLARE
	rtn_cnt int;
	str_length int;
	ipos int;
BEGIN
	ipos := 1;
	rtn_cnt := 0;
	str_length := char_length(org_str);
	while ipos <= str_length loop
		if substring(org_str,ipos, 1) = find_char then
			rtn_cnt := rtn_cnt + 1;
		end if;
		ipos := ipos +1;
	end loop;
	
	return rtn_cnt;
END;
$$;

-------------------------------------------------------------------------------------------------------------
-- poi name
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_make_poi_dictionary()
 RETURNS integer
    LANGUAGE plpgsql
    AS  
$$ 
DECLARE
    curs1 refcursor;
    old_name_id integer;
    _meshid integer;
    _exitno_chn character varying; 
    _exitno_trd character varying; 
    _exitno_py character varying;
    _exitno_eng character varying;
    _exitno_ctn character varying;
    cnt integer;
    curr_name_id integer;
    temp_sp integer;
    temp_meshid integer;
    temp_seq_nm smallint;
    temp_exit_type smallint;
    i integer;
BEGIN
	curr_name_id := 0;
	cnt          := 0;
	OPEN curs1 FOR SELECT "name", meshid, name_chn, name_trd, name_py, name_eng, name_ctn
			FROM org_poiname;

	-- Get the first record        
	FETCH curs1 INTO old_name_id, _meshid, _exitno_chn, _exitno_trd, _exitno_py, _exitno_eng, _exitno_ctn;
	while old_name_id is not null LOOP
		-- Simplified Chinese and PinYin
		IF _exitno_chn IS NOT NULL THEN
			curr_name_id := mid_add_one_name(curr_name_id, mid_translate(_exitno_chn), _exitno_py, 1);
		END IF;
		-- Traditional Chinese and PinYin
		IF _exitno_trd IS NOT NULL THEN
			curr_name_id := mid_add_one_name(curr_name_id, mid_translate(_exitno_trd), _exitno_py, 2);
		END IF;
		-- English
		IF _exitno_eng IS NOT NULL THEN
			curr_name_id := mid_add_one_name(curr_name_id, _exitno_eng, null, 3);
		END IF;
		-- GuangDong
		IF _exitno_ctn IS NOT NULL THEN
			curr_name_id := mid_add_one_name(curr_name_id, _exitno_ctn, null, 54);
		END IF;
		
		-- They use the same poi name id
		insert into temp_poi_name("old_name_id", "meshid", "new_name_id")
				values (old_name_id, _meshid, curr_name_id);
		
		curr_name_id := curr_name_id + 1;
		cnt          := cnt + 1;
		-- Get the next record
		FETCH curs1 INTO old_name_id, _meshid, _exitno_chn, _exitno_trd, _exitno_py, _exitno_eng, _exitno_ctn;
	END LOOP;
	close curs1;

    return cnt;
END;
$$;

-------------------------------------------------------------------------------------------------------------
-- poi name --- temp
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_make_poi_dictionary_temp()
 RETURNS integer
    LANGUAGE plpgsql
    AS  
$$ 
DECLARE
    curs1 refcursor;
    _pk_poi integer;
    _poi_type integer;
    _exitno_chn character varying; 
    _exitno_py character varying;
    _exitno_eng character varying;
    _the_geom geometry;
    cnt integer;
    curr_name_id integer;
BEGIN
	curr_name_id := 0;
	cnt          := 1;
	OPEN curs1 FOR SELECT pk_poi, poi_type, poi_name, poi_py, poi_en, ST_SetSRID(ST_Point(lon, lat),4326) as the_geom
					FROM temp_search_poi;

	-- Get the first record        
	FETCH curs1 INTO _pk_poi, _poi_type, _exitno_chn, _exitno_py, _exitno_eng, _the_geom;
	while _pk_poi is not null LOOP
		-- Simplified Chinese and PinYin
		IF _exitno_chn IS NOT NULL THEN
			curr_name_id := mid_add_one_name(curr_name_id, mid_translate(_exitno_chn), _exitno_py, 1);
		END IF;
		
		-- English
		IF _exitno_eng IS NOT NULL THEN
			curr_name_id := mid_add_one_name(curr_name_id, _exitno_eng, null, 3);
		END IF;
		
		-- They use the same poi name id
		INSERT INTO poi_tbl("poi_id", "name_id", "poi_type", "the_geom")
				values (cnt, curr_name_id, _poi_type, _the_geom);
		
		curr_name_id := curr_name_id + 1;
		cnt          := cnt + 1;
		-- Get the next record
		FETCH curs1 INTO _pk_poi, _poi_type, _exitno_chn, _exitno_py, _exitno_eng, _the_geom;
	END LOOP;
	close curs1;

    return cnt - 1;
END;
$$;
------------------------------------------------------------------------------------------------
-- shield id
------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_get_shield_id(route_no character varying, road_class integer)
 RETURNS smallint
    LANGUAGE plpgsql
    AS  
$$
BEGIN
    IF (position('G' in route_no) > 0)AND(road_class IN (41000,43000)) THEN
        return 494;
    END IF;
   
    IF (position('S' in route_no) > 0)AND(road_class IN (41000,43000)) THEN
        return 495;
    END IF;
   
    IF (position('G' in route_no) > 0) THEN
        RETURN 496;
    END IF;
   
    IF (position('S' in route_no) > 0) THEN
        RETURN 497;
    END IF;
 
    IF (position('X' in route_no) > 0) THEN
        RETURN 500;
    END IF;
   
    RETURN 506;
END;
$$;

------------------------------------------------------------------------------------------------
-- Inter Change Name
------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_make_interchange_name_dictionary()
 RETURNS integer
    LANGUAGE plpgsql
    AS  
$$ 
DECLARE
    curs1        refcursor;
    gid_lid      bigint[];
    ic_gid       bigint;
    _name_chn    character varying; 
    _name_trd    character varying; 
    _name_py     character varying;
    _name_eng    character varying;
    _name_ctn    character varying;
    cnt          integer;
    curr_name_id integer;
    i            integer;
BEGIN
	curr_name_id := 0;
	cnt          := 0;
	OPEN curs1 FOR SELECT gids, name_chn, name_trd, name_py, name_eng, name_ctn
					FROM (
						SELECT array_agg(gid) as gids, 
							mid_delete_first_one_digit(name_chn) as name_chn, 
							mid_delete_first_one_digit(name_trd) as name_trd, 
							mid_delete_first_one_digit(name_py) as name_py, 
							mid_delete_first_one_digit(name_eng) as name_eng, 
							mid_delete_first_one_digit(name_ctn) as name_ctn
						  FROM org_interchange
						  group by name_chn, name_trd, name_py, name_eng, name_ctn
					) as a
					order by name_chn, name_trd, name_py, name_eng, name_ctn
					;
    
	-- Get the first record        
	FETCH curs1 INTO gid_lid, _name_chn, _name_trd, _name_py, _name_eng, _name_ctn;
	while gid_lid is not null LOOP
		-- Simplified Chinese and PinYin
		IF _name_chn IS NOT NULL THEN
			curr_name_id := mid_add_one_name(curr_name_id, mid_translate(_name_chn), _name_py, 1);
		END IF;
		-- Traditional Chinese and PinYin
		IF _name_trd IS NOT NULL THEN
			curr_name_id := mid_add_one_name(curr_name_id, mid_translate(_name_trd), _name_py, 2);
		END IF;
		-- English
		IF _name_eng IS NOT NULL THEN
			curr_name_id := mid_add_one_name(curr_name_id, _name_eng, null, 3);
		END IF;
		-- GuangDong
		IF _name_ctn IS NOT NULL THEN
			curr_name_id := mid_add_one_name(curr_name_id, _name_ctn, null, 54);
		END IF;
		-- They use the same name_id

		i := 1;
		while i <= array_upper(gid_lid, 1) LOOP
			ic_gid = gid_lid[i];
			insert into temp_interchange_name("gid", "new_name_id")
				values (ic_gid, curr_name_id);
			i := i + 1;
		END LOOP;
		
		curr_name_id := curr_name_id + 1;
		cnt          := cnt + 1;
		-- Get the next record
		FETCH curs1 INTO gid_lid, _name_chn, _name_trd, _name_py, _name_eng, _name_ctn;
	END LOOP;
	close curs1;

    return cnt;
END;
$$;

------------------------------------------------------------------------------------------------
-- SA/PA Name
------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_make_sa_pa_name_dictionary()
 RETURNS integer
    LANGUAGE plpgsql
    AS  
$$ 
DECLARE
    curs1        refcursor;
    name_lid     integer[];
    old_name_id  integer;
    _name_chn    character varying; 
    _name_trd    character varying; 
    _name_py     character varying;
    _name_eng    character varying;
    _name_ctn    character varying;
    cnt          integer;
    curr_name_id integer;
    i            integer;
BEGIN
	curr_name_id := 0;
	cnt          := 0;
	OPEN curs1 FOR select name_array, name_chn, name_trd, name_py, name_eng, name_ctn
					  from (
						SELECT array_agg(name) as name_array, name_chn, name_trd, 
							name_py, name_eng, name_ctn
						  FROM org_sapaname
						  group by name_chn, name_trd, name_py, name_eng, name_ctn
					  ) as a
					  order by name_chn, name_trd, name_py, name_eng, name_ctn
					  ;
    
	-- Get the first record        
	FETCH curs1 INTO name_lid, _name_chn, _name_trd, _name_py, _name_eng, _name_ctn;
	while name_lid is not null LOOP
		-- Simplified Chinese and PinYin
		IF _name_chn IS NOT NULL THEN
			curr_name_id := mid_add_one_name(curr_name_id, mid_translate(_name_chn), _name_py, 1);
		END IF;
		-- Traditional Chinese and PinYin
		IF _name_trd IS NOT NULL THEN
			curr_name_id := mid_add_one_name(curr_name_id, mid_translate(_name_trd), _name_py, 2);
		END IF;
		-- English
		IF _name_eng IS NOT NULL THEN
			curr_name_id := mid_add_one_name(curr_name_id, _name_eng, null, 3);
		END IF;
		-- GuangDong
		IF _name_ctn IS NOT NULL THEN
			curr_name_id := mid_add_one_name(curr_name_id, _name_ctn, null, 54);
		END IF;
		-- They use the same name_id

		i := 1;
		while i <= array_upper(name_lid, 1) LOOP
			old_name_id = name_lid[i];
			insert into temp_sapa_name("name", "new_name_id")
				values (old_name_id, curr_name_id);
			i := i + 1;
		END LOOP;
		
		curr_name_id := curr_name_id + 1;
		cnt          := cnt + 1;
		-- Get the next record
		FETCH curs1 INTO name_lid, _name_chn, _name_trd, _name_py, _name_eng, _name_ctn;
	END LOOP;
	close curs1;

    return cnt;
END;
$$;
CREATE OR REPLACE FUNCTION mid_get_toll_type(gate_info character varying)
 RETURNS  smallint[]
    LANGUAGE plpgsql
    AS  
$$ 
DECLARE
	temp         smallint[];
	cnt          integer;
	sp           integer;
	tollgatetype integer;
	mark         integer;            
BEGIN
	mark := 1;
	if gate_info is null then
		temp[1] := 0;
		temp[2] := 0;
		return temp;
	end if;
	tollgatetype := 0;
	sp = 1;
	-- Get the first record        
	--FETCH curs1 INTO ic, new_node, new_name_id, junction, pa, sa, ic_t, tollgate, gate_info, service_info_flag;
	while sp <= length(gate_info)  LOOP
		IF substring(gate_info from sp for 1) = '6' THEN 
			tollgatetype := tollgatetype | 7;
			mark := 0;
		ELSEIF substring(gate_info , sp, 1) = '5' THEN
			tollgatetype := tollgatetype | 5;
			mark := 0;
		ELSEIF substring(gate_info, sp, 1) = '4' THEN
			tollgatetype := tollgatetype | 3;
			mark := 0;
		ELSEIF substring(gate_info, sp, 1) = '3' THEN
			tollgatetype := tollgatetype | 6;
			mark := 0;
		ELSEIF substring(gate_info, sp, 1) = '2' THEN
			tollgatetype := tollgatetype | 4;
			mark := 0;
		ELSEIF substring(gate_info, sp, 1) = '1' THEN
			tollgatetype := tollgatetype | 2;
		ELSEIF substring(gate_info, sp, 1) = '0' THEN
			tollgatetype := tollgatetype | 1;
			mark := 0;
		ELSE tollgatetype := tollgatetype | 1;
		    mark := 0;
		END IF;
		sp          := sp + 1;
        
	END LOOP;
	temp[1] := tollgatetype ;
	temp[2] := mark;
    return temp;
END;
$$;

---get ic type
CREATE OR REPLACE FUNCTION mid_get_ic_tpye(ic_type integer,interchange_no integer)
 RETURNS  smallint[]
    LANGUAGE plpgsql
    AS  
$$ 
DECLARE
	curs1           	refcursor;
	temp_array   		smallint[];
	temp_typeflag 		integer[];
	sp           		integer;
	pa_flag      		smallint;
	i           	 	integer;
BEGIN
	pa_flag 	:= 0;
	if ic_type is null then
		return null;
	end if;
     
	IF ic_type = 2 THEN
		OPEN curs1 FOR SELECT  array_agg(type_flag) as tflags from org_sapa where ic_no=interchange_no;
	-- Get the first record        
		FETCH curs1 INTO temp_typeflag;
	END IF;
	
	IF ic_type = 4 or ic_type = 32 THEN 
		temp_array[1] := 1;
	ELSE temp_array[1] := 0;
	END IF;
	
	IF ic_type = 2 THEN 
	        i := 1;
		while i <= array_upper(temp_typeflag, 1)  LOOP
			if temp_typeflag[i] = 2 then
				pa_flag = 1;
			END IF;
			i = i + 1;
		END LOOP; 
		if pa_flag=1 then
			temp_array[2] := 1;
		ELSE temp_array[2] := 0;
		END IF;
	ELSE temp_array[2] := 0;
	END IF;
	IF ic_type = 2 THEN 
		if pa_flag != 1 then
			temp_array[3] := 1;
		ELSE temp_array[3] := 0;
		END IF;
	ELSE temp_array[3] := 0;
	END IF;
	IF ic_type = 8 THEN 
		temp_array[4] := 1;
	ELSE temp_array[4] := 0;
	END IF;
	IF ic_type = 16 THEN 
		temp_array[5] := 1;
	ELSE temp_array[5] := 0;
	END IF;
	IF temp_array[2] = 1 or temp_array[3] = 1 THEN 
		temp_array[6] := 1;
	ELSE temp_array[6] := 0;
	END IF;
	-- Get the first record        
	--FETCH curs1 INTO ic, new_node, new_name_id, junction, pa, sa, ic_t, tollgate, gate_info, service_info_flag;
	
    return temp_array;
END;
$$;

--------------------------------------------------------------------------
--Get ic data and insert into highway_facility_tbl
--------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_get_highway_ic_node()
	RETURNS smallint
	LANGUAGE plpgsql volatile
AS $$
DECLARE
	curs1            refcursor;
	curs2            refcursor;
	
	temprs1          bigint;
	temprs2          bigint;
	
	temp_ic          bigint;
	temp_newnode     bigint;
	temp_name_id     bigint;
	temp_gateinfo    character varying;
	gateinfoArray    smallint[];
	mark_ic          smallint;
	toga_type        smallint;
	temp_count       bigint;
	i                integer;
	j                integer;
 
BEGIN	
	i        := 1;
	j        := 1;	
	OPEN curs1 FOR select ic, new_node   
		from org_interchange as a
		left join mesh_mapping_tbl as b 
		on a.mesh = b.meshid_str 
		left join temp_node_mapping as c
		on globl_mesh_id = c.meshid and a.node = c.node
		left join temp_interchange_name as d
		on a.gid=d.gid 
		where  a.ic_type = 8;
	-- Get the first record        
	FETCH curs1 INTO temp_ic, temp_newnode;

	while temp_ic is not null LOOP
		temprs1 = mid_research_highway_ic_node(temp_ic, temp_newnode, 1);
		temprs2 = mid_research_highway_ic_node(temp_ic, temp_newnode, 2);
		FETCH curs1 INTO temp_ic, temp_newnode;
	END LOOP;
	close curs1;
	
	return 1;
END;
$$;

--------------------------------------------------------------------------
-------Make rdb ic data mapping to mainline node
--@dir, 1: forward, 2: backward
--@org_ic_id:ic_id
--------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_research_highway_ic_node(org_ic_id bigint, nhwynode bigint, dir integer)
  RETURNS bigint
LANGUAGE plpgsql volatile
AS $$
DECLARE
	curs1               refcursor;
	rec                 record;
	bRoadEnd            boolean;
	nRoadType           smallint;
	nFunctionClass      smallint;
	rstPath             varchar;
	tmpPath             varchar;
	
	tmpPathArray        varchar[];
	tmpLastNodeArray    bigint[];
	tmpPathCount        integer;
	tmpPathIndex        integer;
	
	nIndex              integer;
	mgnode_type         smallint;
	road_type_array     bigint[];
	link_type_array     bigint[];
	
	temp_link_type_array bigint[];
	temp_road_type_array bigint[];
	i                   integer;
	j                   integer;
	t                   integer;
	p                   integer;
	temp                integer;
	temp_ic_id          bigint;
	temp_link_type      integer;
	condition           integer;
	highway_link_no     smallint;
	start_condition     smallint[];
	end_condition       smallint[];
BEGIN	
	--rstPathArray
	tmpPathArray	    := ARRAY[null];
	tmpLastNodeArray    := ARRAY[nHwyNode];
	tmpPathCount	    := 1;
	tmpPathIndex	    := 1;
	i                   := 1;
	j                   := 1;
	t                   := 1;
	p                   := 1;
	temp                := 1;	
	highway_link_no     := 0;
	temp_link_type      := 0;
	condition           := 0;
	if dir=1 then
		mgnode_type=0;
		start_condition := array[1, 2]; 
		end_condition := array[1, 3];
	else 
		mgnode_type=1;
		start_condition := array[1, 3]; 
		end_condition := array[1, 2];
	end if;	
	-- search
	select	array_agg(link_type), array_agg(road_type)
	into link_type_array, road_type_array
	from link_tbl
	where  (
			(s_node = tmpLastNodeArray[tmpPathIndex] and one_way_code = any(start_condition))  --one_way_code in (1,2))
			or
			(e_node = tmpLastNodeArray[tmpPathIndex] and one_way_code = any(end_condition))
	);
	---Get highway mainline link number of this ic node: exclude inner link
	highway_link_no = mid_judge_highway_link(tmpLastNodeArray[tmpPathIndex], null);
	
	----IC node has many highwaylinks
	if highway_link_no >= 2  then
		while j <= array_upper(road_type_array, 1) loop
			-- normal link / highway link but not: roundabout link, main link, inner link
			if  road_type_array[j] not in (0, 1) or (road_type_array[j] in (0, 1) and link_type_array[j] not in (0, 1, 2, 4)) then
				temp_link_type = temp_link_type | 1;
			-- Highway link and inner link
			elseif road_type_array[j] in (0, 1) and link_type_array[j] = 4 then
				temp_link_type = temp_link_type | 2;
			-- Highway link and roundabout link
			elseif  road_type_array[j] in (0, 1) AND link_type_array[j] = 0 THEN 
				temp_link_type = temp_link_type | 4;
			end if;
			j = j + 1;
		end loop;
		
		-- Only Highway Roundabout Link
		if temp_link_type = 4  then 
			-- IN_OUT_TYPE based on hwy link (not Roundabout)
			while t <= array_upper(road_type_array, 1) loop
				-- Hwy and Not Roundbout Link
				if road_type_array[t] in (0, 1) and link_type_array[t] <> 0 then
					insert into temp_from_ic_to_highwaymainline_path(icnode_id, path, mainnode, direct)
						values(nHwyNode, '0', tmpLastNodeArray[tmpPathIndex], dir);
					insert into mid_mainline_ic_node_info(ic_id, ic_node, mainline_node, ic_type ) 
						values(org_ic_id, nHwyNode, tmpLastNodeArray[tmpPathIndex], cast(abs(mgnode_type - 1) as bigint));
					EXIT;
				end if;
				t = t + 1;
			end loop;
		-- There are normal link, Hwy inner link, Hwy Roundabout link
		elseif temp_link_type > 0 then
			-- IN_OUT_TYPE based on [dir]
			insert into temp_from_ic_to_highwaymainline_path(icnode_id, path, mainnode, direct)
				values(nHwyNode, '0', tmpLastNodeArray[tmpPathIndex], dir);
			insert into mid_mainline_ic_node_info(ic_id, ic_node, mainline_node, ic_type ) 
				values(org_ic_id, nHwyNode, tmpLastNodeArray[tmpPathIndex], cast(abs(mgnode_type - 1) as bigint));
		else
			-- 
		end if;
		
	----IC node has only one highway link
	elseif highway_link_no = 1 then
		-- find out this Main_link, and IN_OUT_TYPE base on [dir]
		while p <= array_upper(road_type_array, 1) loop
			-- Hwy link and link type is Roundabout or main_link.
			if road_type_array[p] in (0, 1) and link_type_array[p] in (0, 1, 2) then
				insert into temp_from_ic_to_highwaymainline_path(icnode_id, path, mainnode, direct)
					values(nHwyNode, '0', tmpLastNodeArray[tmpPathIndex], dir);
				insert into mid_mainline_ic_node_info(ic_id, ic_node, mainline_node, ic_type ) 
					values(org_ic_id, nHwyNode, tmpLastNodeArray[tmpPathIndex], cast(mgnode_type as bigint));
				exit;
			end if;
			p = p + 1;
		end loop;
		-- can not find out this Main_link, do nothing.
		
	-- IC node has not highwaylink
	else  -- highway_link_no = 0
		if highway_link_no = 0 then 
	
			WHILE tmpPathIndex <= tmpPathCount LOOP
				----raise INFO 'tmpPathArray = %', cast(tmpPathArray as varchar);
				-- Get hwy link number by [dir] of Last_Node: include Roundabout, main link, but exclude hwy inner link
				condition = mid_judge_highway_link(tmpLastNodeArray[tmpPathIndex], dir);
				for rec in
					select	(case when s_node = tmpLastNodeArray[tmpPathIndex] then e_node else s_node end) as nextnode, 
						link_id as nextlink, 
						link_type, road_type, function_class, toll
					from link_tbl
					where   (
							(s_node = tmpLastNodeArray[tmpPathIndex] and one_way_code = any(start_condition)) 
							or
							(e_node = tmpLastNodeArray[tmpPathIndex] and one_way_code = any(end_condition))
						)
						and 
						(
							tmpPathArray[tmpPathIndex] is null 
							or 
							not (cast(link_id as varchar) = ANY(regexp_split_to_array(tmpPathArray[tmpPathIndex], E'\\|+')))
						)
					order by road_type,  link_type, abs(toll - 1)
				loop	
					----raise INFO 'nextnode = %', rec.nextnode;
					tmpPath	:= tmpPathArray[tmpPathIndex];
					-- Hwy Roadabout or Hwy Main Link
					if rec.link_type in (0, 1, 2) and rec.road_type in (0, 1) then
						if tmpPath is not null then
							rstPath	:= tmpPath;	
							insert into temp_from_ic_to_highwaymainline_path(icnode_id, path, mainnode, direct)
								values(nHwyNode, rstPath, tmpLastNodeArray[tmpPathIndex], dir);
							insert into mid_mainline_ic_node_info(ic_id, ic_node, mainline_node, ic_type ) 
								values(org_ic_id, nHwyNode, tmpLastNodeArray[tmpPathIndex], cast(mgnode_type as bigint));	
						else 
							insert into temp_from_ic_to_highwaymainline_path(icnode_id, path, mainnode, direct)
								values(nHwyNode, tmpPath, tmpLastNodeArray[tmpPathIndex], dir);
							insert into mid_mainline_ic_node_info(ic_id, ic_node, mainline_node, ic_type ) 
								values(org_ic_id, nHwyNode, tmpLastNodeArray[tmpPathIndex], cast(mgnode_type as bigint));
						end if;

						EXIT;
					-- Roundabout, JCT, Ramp, SA or Normal Way inner link
					elseif (rec.link_type in (0, 3, 5, 7) or (rec.link_type = 4 and rec.road_type in (0, 2, 3, 5, 6) )) and condition = 0 THEN
						--raise INFO ' gghdgfsghshtryrtytrht----node_id is %', rec.nextlink;
						-- It is IC Node
						OPEN curs1 FOR select  a.ic_id  
							from org_interchange as a
							left join mesh_mapping_tbl as b 
							on a.mesh = b.meshid_str 
							left join temp_node_mapping as c
							on globl_mesh_id = c.meshid and a.node = c.node
							where  a.ic_type = 8 and c.new_node = rec.nextnode;
						FETCH curs1 INTO temp_ic_id;
						close curs1;
						---raise INFO ' yuansjhidian----node_id is %', temp_ic_id;
						if temp_ic_id is not null then
							continue;
						else
							tmpPathCount		:= tmpPathCount + 1;
							tmpLastNodeArray	:= array_append(tmpLastNodeArray, cast(rec.nextnode as bigint));
							if tmpPath is null then
								tmpPathArray	:= array_append(tmpPathArray, cast(rec.nextlink as varchar));
							else
								tmpPathArray	:= array_append(tmpPathArray, cast(tmpPath||'|'||rec.nextlink as varchar));
							end if;
						end if;
					end if;
				end loop;
				tmpPathIndex := tmpPathIndex + 1;
			END LOOP;
		else
		 
		--raise INFO ' ----node_id is %', nhwynode;
		
		end if;
	
	end if;
	
	return 0;
END;
$$;

  


--------------------------------------------------------------------------
-- find sapa user id, returns [sapa_user_id, in_out_type]
-- if not, return NUll
--------------------------------------------------------------------------
create or replace function mid_find_sapa_by_path(nIC integer)
	RETURNS integer[]
	LANGUAGE plpgsql
AS $$ 
DECLARE	
	tmpPathArray		varchar[];
	tmpLastNodeArray	bigint[];
	tmpPathCount		integer;
	tmpPathIndex		integer;
	tmplastLinkArray	bigint[];
	
	nStartNode              bigint;
	in_out_type             smallint;
	sapa_user_id            integer;
	start_oneway_cond       character varying;
	end_oneway_cond         character varying;
	rec			record;
BEGIN
	
	-- get new node id
	select new_node into nStartNode
	  from (
		select *
		from org_interchange
		where ic = nIC
	  ) as a
	  left join mesh_mapping_tbl as b
	  on a.mesh = b.meshid_str 
	  left join temp_node_mapping as c
	  on globl_mesh_id = c.meshid and a.node = c.node;
	if not found then 
		raise info 'IC: %', nIC;
		return null;
	end if;  
	
	-- get in/out type: 
	in_out_type := mid_get_hwy_in_out_type(nStartNode, 0::smallint, 1::smallint);

	-- out: forward
	if in_out_type = 1 then 
		start_oneway_cond := '(1, 2)';
		end_oneway_cond   := '(1, 3)';
	else -- IN: backward
		start_oneway_cond := '(1, 3)';
		end_oneway_cond   := '(1, 2)';
	end if;

	--rstPathArray
	tmpPathArray		:= ARRAY[''];
	tmpLastNodeArray	:= ARRAY[nStartNode];
	tmpPathCount		:= 1;
	tmpPathIndex		:= 1;
	sapa_user_id            := 0;
	
	--raise INFO 'ic: %', nIC;
	WHILE tmpPathIndex <= tmpPathCount LOOP
		--raise INFO '%', tmpPathIndex;
		--raise INFO '%', cast(tmpPathArray[tmpPathIndex] as varchar);
		--raise INFO 'tmpLastNodeArray[tmpPathIndex]: %', tmpLastNodeArray[tmpPathIndex];
		sapa_user_id = mid_get_sapa_user_id(tmpLastNodeArray[tmpPathIndex]);
		-- found the sapa node
		if sapa_user_id > 0 then
			exit;
		end if;
		
		if array_upper(regexp_split_to_array(tmpPathArray[tmpPathIndex], E'\\|+'), 1) >= 12 then  -- It is longer then 15.
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
					 select link_id as nextroad, ''(2)'' as dir, e_node as nextnode
					 from link_tbl
					 where	(s_node = ' || tmpLastNodeArray[tmpPathIndex]|| ') and 
							(one_way_code in ' || start_oneway_cond ||') and 
							--(road_type in (0, 1) or 
							 link_type in (3,4, 5,7)
							-- toll = 1)
					 union
					 
					 select link_id as nextroad, ''(3)'' as dir, s_node as nextnode
					 from link_tbl
					 where	(e_node = ' || tmpLastNodeArray[tmpPathIndex]|| ') and
							(one_way_code in ' || end_oneway_cond ||') and 
							--(road_type in (0, 1) or 
							 link_type in (3,4, 5,7)
							-- toll = 1)
				 )as a
				'
		loop
			if not ((rec.nextroad::varchar) = ANY(regexp_split_to_array(tmpPathArray[tmpPathIndex], E'\\|+'))) then
				tmpPathCount        := tmpPathCount + 1;
				tmpPathArray        := array_append(tmpPathArray, cast(tmpPathArray[tmpPathIndex]||'|'||rec.nextroad as varchar));
				tmpLastNodeArray    := array_append(tmpLastNodeArray, cast(rec.nextnode as bigint));
				tmplastLinkArray    := array_append(tmplastLinkArray, cast(rec.nextroad as bigint));
			end if;
		end loop;
		
		-- next path
		tmpPathIndex := tmpPathIndex + 1;
	END LOOP;

	-- does not find.
	if sapa_user_id = 0 then 
		return array[NULL, in_out_type];
	else
		return array[sapa_user_id, in_out_type];
	end if;
END;
$$;


--------------------------------------------------------------------------
-- if sapa node or sapa node enter 
-- and return the [sapa user id]
-- if not , returns [0]
--------------------------------------------------------------------------
create or replace function mid_get_sapa_user_id(nNewNodeID bigint)
	RETURNS integer
	LANGUAGE plpgsql
AS $$ 
DECLARE
	sapa_user_id     integer;
BEGIN
	sapa_user_id := 0;
	SELECT sapa into sapa_user_id
		  from (
			select *
			from temp_node_mapping
			where new_node = nNewNodeID
		  ) as a
		  left join mesh_mapping_tbl as b
		  on a.meshid = globl_mesh_id
		  left join org_sapa as c
		  on meshid_str = c.mesh and (a.node = c.node or a.node = c.node_entr);

	if found and sapa_user_id is not null then 
		return sapa_user_id;
	else 
		return 0;
	end if;
END;
$$;

--------------------------------------------------------------------------
-- find the SAPA by distance, returns sapa_user_id
-- if not , returns [0]
--------------------------------------------------------------------------
create or replace function mid_find_sapa_by_distance(nIC integer, nDistance integer)
	RETURNS integer
	LANGUAGE plpgsql
AS $$ 
DECLARE
	sapa_user_id     integer;
	ic_node          bigint;
	ic_geometry    geometry;
BEGIN
	-- Get the new node id of ic
	select new_node into ic_node
	  from (
		select mesh, node 
		  from org_interchange
		  where ic = nIC
	  ) as a
	  left join mesh_mapping_tbl as b
	  on a.mesh = b.meshid_str
	  left join temp_node_mapping as c
	  on globl_mesh_id = meshid and a.node = c.node;
	  
	if not found or ic_node is null then 
		return 0;
	end if;

	-- get the geometry of IC Node
	select the_geom into ic_geometry from node_tbl where node_id = ic_node;
	if not found or ic_geometry is null then 
		return 0;
	end if;

	--raise info '%', st_astext(ic_geometry);
	-- search SAPA in nDistance 
	SELECT sapa into sapa_user_id
		  from (
			select *
			from temp_node_mapping
			where new_node in (
				select node_id 
				  from node_tbl
				  where ST_DWithin(the_geom, st_setsrid(ic_geometry, 4326), 0.000002694945852358566745 * (nDistance / 0.25)) 
				  --and node_id <> ic_node
			 )
		  ) as a
		  left join mesh_mapping_tbl as b
		  on a.meshid = globl_mesh_id
		  inner join org_sapa as c
		  on meshid_str = c.mesh and (a.node = c.node);

	if sapa_user_id is not null then 
		return sapa_user_id;
	else 
		raise info '3';
		return 0;
	end if;
end;
$$;

--------------------------------------------------------------------------
-- Get In/out type for JCT or SAPA
-- 1: out, 0: IN
--------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_get_hwy_in_out_type(nNodeID bigint, junction smallint, sapa smallint)
	RETURNS smallint
	LANGUAGE plpgsql volatile
AS $$
DECLARE
	s_node_array         bigint[];
	e_node_array         bigint[];
	one_way_code_array   bigint[];
	road_type_array      bigint[];
	link_type_array      bigint[];
	i                    integer;
	j                    integer;
BEGIN
	 
	SELECT array_agg(s_node), array_agg(e_node), array_agg(one_way_code), 
	       array_agg(road_type), array_agg(link_type)
	  into s_node_array, e_node_array, one_way_code_array, road_type_array, link_type_array
	  FROM (
		SELECT 1 as gid, s_node, e_node, one_way_code, road_type, link_type
		FROM link_tbl
		where s_node = nNodeID or e_node = nNodeID
	  ) AS A
	  group by gid;
	  
	i            := 1;
	--raise info 's_node_array: %', s_node_array;
	while i <= array_upper(s_node_array, 1) loop
		-- JCT
		if junction = 1 and link_type_array[i] in (3) then
			if one_way_code_array[i] in (1, 4) then
				raise info 'sa node_id: %', nNodeID;
				return NULL;
			end if;
			--raise info 'nNodeID: %, s_node: %, e_node: %, one_way_code: %', nNodeID, s_node_array[i], e_node_array[i], one_way_code_array[i];
			-- In
			if (nNodeID = s_node_array[i] and 3 = one_way_code_array[i]) 
			  or (nNodeID = e_node_array[i] and 2 = one_way_code_array[i]) then
				return 0;
			else -- Out
				return 1;
			end if;
		end if;
		
		-- SAPA
		if sapa = 1 and link_type_array[i] in (7, 3) then
			if one_way_code_array[i] in (4) then
				raise info 'sa node_id: %', nNodeID;
				return NULL;
			end if;
			-- In
			if (nNodeID = s_node_array[i] and 3 = one_way_code_array[i]) 
			  or (nNodeID = e_node_array[i] and 2 = one_way_code_array[i]) then
				return 0;
			else -- Out
				return 1;
			end if;
		end if;
		i := i + 1;
	end loop;

	if junction = 1 then
		-- NODE JCT
		--i    := 1;
		--while i <= array_upper(s_node_array, 1) loop
			-- JCT
		--	if link_type_array[i] not in (2) then
		--		raise info 'Do not exist JCT LINK, node_id: %', nNodeID;
		--		return NULL;
		--	end if;
		--	i    := i + 1;
		--end loop;
		-- It is a NODE JCT
		--return 1;
		raise info 'Do not exist JCT LINK, node_id: %', nNodeID;
		return NULL;
	end if;

	if sapa = 1 then
		raise info 'Do not exist SAPA LINK, node_id: %', nNodeID;
	end if;
	
	return NULL;	
END;
$$;

--------------------------------------------------------------------------
-- Make the SAPA, which node_entr is same as ic node.
--------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_make_sapa_same()
	RETURNS smallint
	LANGUAGE plpgsql volatile
AS $$
DECLARE
	curs1                 refcursor;
	temp_ic_no            integer;
	ic_array              integer[];
	sapa_out_flag_array   integer[];
	sapa_array            integer[];
	sapa_idx              integer;
	i                     integer;
	prev_ic_idx         integer;
BEGIN
	OPEN curs1 FOR SELECT ic_no, ics, flags, sapas
			from (
				SELECT ic_no, array_agg(ic) as ics, array_agg(sapa_out_flag) as flags, array_agg(sapa) as sapas
				  from (
					SELECT a.ic_no, ic, ic_type, a.mesh, a.node, 
					       (sapa is not null)::integer as sapa_out_flag, sapa
					  FROM org_interchange as a
					  left join org_sapa as b
					  on a.mesh = b.mesh and a.node = b.node_entr
					  where ic_type = 2
					  order by ic_no, ic
				  ) as b
				  group by ic_no
			  ) as c
			  where 1 = any(flags);-- and ic_no = 4063; 
			  
	-- Get the first record        
	FETCH curs1 INTO temp_ic_no, ic_array, sapa_out_flag_array, sapa_array;
	while temp_ic_no is not null LOOP
		sapa_idx := 0;
		raise info 'ic_no: %', temp_ic_no;
		sapa_idx := mid_get_next_sapa_idx(sapa_array, sapa_idx);
		-- SAPA OUT
		if sapa_out_flag_array[1] = 1 then
			-- insert into;
			insert into mid_temp_sapa_ic_same("ic_no", "ic", "in_out_type", "sapa")
				values(temp_ic_no, ic_array[1], sapa_out_flag_array[1], sapa_array[sapa_idx]);
		else  -- SAPA IN
			if array_upper(ic_array, 1) > 1 then
				raise info 'ic_no: %, ic: %',  temp_ic_no, ic_array[1];
			end if;
			-- insert into;
			insert into  mid_temp_sapa_ic_same("ic_no", "ic", "in_out_type", "sapa")
				values(temp_ic_no, ic_array[1], sapa_out_flag_array[1], sapa_array[sapa_idx]);
		end if;
		
		i           := 2;
		prev_ic_idx := 0;
		while i <= array_upper(ic_array, 1) loop
			-- SAPA IN
			if sapa_out_flag_array[i] = 0 then
				-- the Last 
				if i = array_upper(ic_array, 1) then 
					-- insert into [the last]
					insert into  mid_temp_sapa_ic_same("ic_no", "ic", "in_out_type", "sapa")
						values(temp_ic_no, ic_array[i], sapa_out_flag_array[i], sapa_array[sapa_idx]);
					prev_ic_idx := 0;
				else 
					prev_ic_idx := i;
				end if;
			else   -- SAPA OUT
				if prev_ic_idx <> 0 then
					-- insert into:  ic_array[prev_ic_idx]
					insert into  mid_temp_sapa_ic_same("ic_no", "ic", "in_out_type", "sapa")
						values(temp_ic_no, ic_array[prev_ic_idx], sapa_out_flag_array[prev_ic_idx], sapa_array[sapa_idx]);
						
					prev_ic_idx := 0;
				end if;

				sapa_idx := mid_get_next_sapa_idx(sapa_array, sapa_idx);
				-- insert into: ic_array[i]
				insert into  mid_temp_sapa_ic_same("ic_no", "ic", "in_out_type", "sapa")
					values(temp_ic_no, ic_array[i], sapa_out_flag_array[i], sapa_array[sapa_idx]);
			end if;
			i       := i + 1;
		END loop;

		FETCH curs1 INTO temp_ic_no, ic_array, sapa_out_flag_array, sapa_array;
	END LOOP;

	return 0;
END;
$$;

----------------------------------------------------------------------------------------------------------------
-- The Get next sapa idx
----------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_get_next_sapa_idx(sapa_array integer[], curr_idx integer)
	RETURNS integer
	LANGUAGE plpgsql volatile
AS $$
DECLARE
	i   integer;
BEGIN
	if curr_idx > 0 then
		i := curr_idx;
	else
		i := 1;
	end if;

	while i <= array_upper(sapa_array, 1) loop
		if sapa_array[i] is not null and sapa_array[i] > 0 then
			return i;
		end if;
		i := i + 1;
	end loop;
	return NUll;
END;
$$;

--------------------------------------------------------------------------
-- Make the SAPA, which node_entr is not same as ic node.
--------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_make_sapa_not_same()
	RETURNS smallint
	LANGUAGE plpgsql volatile
AS $$
DECLARE
	curs1                 refcursor;
	temp_ic_no            integer;
	ic_array              integer[];
	sapa_out_flag_array   integer[];
	temp_sapa             integer;
	--sapa_idx              integer;
	i                     integer;
	prev_ic_idx           integer;
BEGIN
	OPEN curs1 FOR select ic_no, array_agg(ic) as ics, array_agg(in_out_type) as types, sapa
			  from (
				SELECT ic, ic_no, in_out_type, sapa
				  FROM mid_temp_sapa_ic_not_same_all
				  where sapa > 0
				  order by ic_no, ic
			  ) as a
			  group by ic_no, sapa; 
			  
	-- Get the first record        
	FETCH curs1 INTO temp_ic_no, ic_array, sapa_out_flag_array, temp_sapa;
	while temp_ic_no is not null LOOP
		if (array_upper(ic_array, 1) > 1)
		   and (sapa_out_flag_array[1] = sapa_out_flag_array[array_upper(ic_array, 1)]) then
			RAISE EXCEPTION 'SAPA, ic_no = %', temp_ic_no;
		end if;

		-- insert into the [first];
		insert into mid_temp_sapa_ic_not_same("ic_no", "ic", "in_out_type", "sapa")
			values(temp_ic_no, ic_array[1], sapa_out_flag_array[1], temp_sapa);

		if array_upper(ic_array, 1) > 1 then
			-- insert into the [last];
			insert into  mid_temp_sapa_ic_not_same("ic_no", "ic", "in_out_type", "sapa")
				values(temp_ic_no, ic_array[array_upper(ic_array, 1)], 
					sapa_out_flag_array[array_upper(ic_array, 1)], temp_sapa);
		end if;
		
		FETCH curs1 INTO temp_ic_no, ic_array, sapa_out_flag_array, temp_sapa;
	END LOOP;

	return 0;
END;
$$;

--------------------------------------------------------------------------
-- Delete the first One Digital.
--------------------------------------------------------------------------
create or replace function mid_delete_first_one_digit(org_str character varying)
 RETURNS character varying
    LANGUAGE plpgsql
    AS  
$$ 
DECLARE
	i          integer;
	ch         character;
	len        integer;
	digit_num  integer;
BEGIN
	i         := 1;
	digit_num := 0;
	if org_str is null then 
		return NULL;
	end if;

	len := char_length(org_str);
	while i < len loop 
		ch := mid_translate(substr(org_str, i, 1));
		if ch >= '0' and ch <= '9' then
			digit_num := digit_num + 1;
			i         := i + 1;
			continue;
		else
			exit;
		end if;
	end loop;
	-- delete the first one and only one.
	if digit_num = 1 then 
		return ltrim(substr(org_str, digit_num + 1));
	else
		return org_str;
	end if;
END;
$$;

--------------------------------------------------------------------------
-- judge ic_node on highway mainline.
--------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_judge_highway_link(ic_node bigint, dir integer)
  RETURNS integer 
LANGUAGE plpgsql volatile
AS $$
DECLARE
	result                   integer;	
	temp_road_type_array     bigint[];
	temp_link_type_array     bigint[];
	j                        integer;
	
BEGIN	
	result = 0;
	j = 1;
	if dir is null then
		select	array_agg(link_type), array_agg(road_type)
		into temp_link_type_array, temp_road_type_array
		from link_tbl
		where s_node = ic_node
		      or
		      e_node = ic_node;
		      
	else
		select	array_agg(link_type), array_agg(road_type)
		into temp_link_type_array, temp_road_type_array
		from link_tbl
		where(
				(
					(dir = 1)
					and
					(
						(s_node = ic_node and one_way_code in (1,2))
						or
						(e_node = ic_node and one_way_code in (1,3))
					)
				)
				or
				(
					(dir = 2)
					and
					(
						(s_node = ic_node and one_way_code in (1,3))
						or
						(e_node = ic_node and one_way_code in (1,2))
					)
				)
			);
        end if;
	
	while j <= array_upper(temp_link_type_array, 1) loop
		if temp_link_type_array[j] in (0, 1, 2) and temp_road_type_array[j] in (0, 1) then
			result = result+1;
		end if;
		j = j + 1;
	end loop;
	
	return result;
END;
$$;
  