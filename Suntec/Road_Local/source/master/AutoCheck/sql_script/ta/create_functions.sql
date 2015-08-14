CREATE OR REPLACE FUNCTION org_check_linklist_continuity(linkid_array double precision[])
	RETURNS smallint
    LANGUAGE plpgsql
AS $$
DECLARE
	linkcnt integer;
	linkidx integer;
	jnctid_array double precision[];
	recordcnt integer;
BEGIN
	--raise INFO 'linkid_array = %', linkid_array;
	linkcnt := array_upper(linkid_array, 1);

	if linkcnt <= 1 then
		return -1;
	end if;

	for linkidx in 1..linkcnt-1 loop
		--raise INFO 'linkidx = %, linkid1 = %, linkid2 = %', linkidx, linkid_array[linkidx], linkid_array[linkidx+1];
		select array[f_jnctid, t_jnctid] into jnctid_array
		from org_nw
		where id = linkid_array[linkidx+1];

		select count(*) into recordcnt
		from org_nw
		where id = linkid_array[linkidx] and ((f_jnctid = any(jnctid_array)) or (t_jnctid = any(jnctid_array)));

		if recordcnt = 0 then
			return -1;
		end if;
	end loop;
	
	return 0;
END;
$$;