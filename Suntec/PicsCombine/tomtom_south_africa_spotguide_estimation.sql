-- ��1���������highway-signpost�㣬���������ǵ�inlink��Ϣ
drop table if exists org_sg_highway_only;
select a.*, b.trpelid as inlink_id
into org_sg_highway_only
from 
org_sg as a
left join org_sp as b
on a.id=b.id
left join org_nw as c
on b.trpelid = c.id
where b.seqnr=1 and (c.fow=1 or c.freeway=1);

-- ��2��������г��߽���������link��Ϣ��
drop table if exists org_ln_with_link_information;
select a.id, a.fromto, a.jnctid, b.seqnr, b.trpelid
into org_ln_with_link_information
from
org_ln as a
left join org_lp as b
on a.id=b.id
order by a.id, b.seqnr;

-- ��3����Ϊ�˷���qgis�鿴�����������highway-signpost��ص�link��
drop table if exists org_nw_highway_only;
create table org_nw_highway_only as
(select * 
 from org_nw 
 where fow=1 or freeway=1)
union
(select * 
 from org_nw 
 where  id in (select distinct trpelid from org_ln_with_link_information));

-- ����seqnrlist��tolinklist���inlink id
CREATE OR REPLACE FUNCTION get_inlink_id(seqnr_list integer[], tolink_list double precision[])
	RETURNS double precision
    LANGUAGE plpgsql
AS $$
BEGIN
	if seqnr_list is null then
		return -1;
	end if;
	for i in 1..(array_length(seqnr_list,1)) loop
		if seqnr_list[i] = 1 then
			return tolink_list[i];
		end if;
	end loop;
	return -1;
END;
$$;

-- ����seqnrlist��tolinklist���tolink id
CREATE OR REPLACE FUNCTION get_tolink_id(seqnr_list integer[], tolink_list double precision[])
	RETURNS double precision
    LANGUAGE plpgsql
AS $$
BEGIN
	if seqnr_list is null then
		return -1;
	end if;
	for i in 1..(array_length(seqnr_list,1)) loop
		if seqnr_list[i] = 2 then
			return tolink_list[i];
		end if;
	end loop;
	return -1;
END;
$$;

-- ��4������2���������ӵ�id���顣
drop table if exists org_ln_with_link_information_grouped;
select id, fromto, jnctid, get_inlink_id(array_agg(seqnr), array_agg(trpelid)) as inlinkid, 
	get_tolink_id(array_agg(seqnr), array_agg(trpelid)) as tolinkid
into org_ln_with_link_information_grouped
from org_ln_with_link_information
group by id, fromto, jnctid;

-- ��5�����ݱ�1�ͱ�4�������highway signpost�ĳ���ת����Ϣ��
drop table if exists temp_sign_and_its_arrow_dir;
select a.id as sign_id, a.jnctid as sign_jnctid, a.the_geom, b.id as lane_conn_id, b.fromto, b.jnctid as lane_conn_jnctid,
	b.inlinkid, b.tolinkid, to_char(st_y(a.the_geom), '999D999999') || ' ' || to_char(st_x(a.the_geom), '999D999999') as lat_lon
into temp_sign_and_its_arrow_dir
from
org_sg_highway_only as a
left join org_ln_with_link_information_grouped as b
on a.jnctid=b.jnctid and a.inlink_id=b.inlinkid;

-- ��6��ʹ�ñ�4����org_ld�����vilidity�Լ���inlink��lane��
drop table if exists temp_sign_and_its_arrow_dir_vilidity;
select distinct a.inlinkid, a.tolinkid, a.fromto, a.lat_lon, b.validity as inlink_validity
into temp_sign_and_its_arrow_dir_vilidity
from
temp_sign_and_its_arrow_dir as a
left join org_ld as b
on a.inlinkid=b.id;

-- ��7��target table
drop table if exists temp_target_table;
select inlinkid, lat_lon, length(inlink_validity) as inlink_lane_count, array_agg(tolinkid) as tolink_list, 
	array_agg(substr(fromto, 1, 1)) as from_list, array_agg(substr(fromto, 3, 1)) as to_list
into temp_target_table
from temp_sign_and_its_arrow_dir_vilidity 
group by inlinkid, length(inlink_validity), lat_lon
