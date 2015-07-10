-- 表1：求出所有highway-signpost点，并带出它们的inlink信息
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

-- 表2：求出所有车线交点所属的link信息。
drop table if exists org_ln_with_link_information;
select a.id, a.fromto, a.jnctid, b.seqnr, b.trpelid
into org_ln_with_link_information
from
org_ln as a
left join org_lp as b
on a.id=b.id
order by a.id, b.seqnr;

-- 表3：仅为了方便qgis查看：求出所有与highway-signpost相关的link。
drop table if exists org_nw_highway_only;
create table org_nw_highway_only as
(select * 
 from org_nw 
 where fow=1 or freeway=1)
union
(select * 
 from org_nw 
 where  id in (select distinct trpelid from org_ln_with_link_information));

-- 根据seqnrlist和tolinklist求出inlink id
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

-- 根据seqnrlist和tolinklist求出tolink id
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

-- 表4：将表2按jnctid分组。
drop table if exists org_ln_with_link_information_grouped;
select id, jnctid, get_inlink_id(array_agg(seqnr), array_agg(trpelid)) as inlinkid, 
	get_tolink_id(array_agg(seqnr), array_agg(trpelid)) as tolinkid,
	substr(fromto, 1, position('/' in fromto)-1) as from_lane, 
	substr(fromto, position('/' in fromto)+1, length(fromto)) as to_lane
into org_ln_with_link_information_grouped
from org_ln_with_link_information
group by id, fromto, jnctid;

-- 表5：表1连表4求出所有highway signpost的车线转向信息。
drop table if exists temp_sign_and_its_arrow_dir;
select a.id as sign_id, a.jnctid, a.the_geom, b.id as lane_conn_id, b.inlinkid, b.tolinkid, b.from_lane, b.to_lane,
	to_char(st_y(a.the_geom), '999D999999') || ' ' || to_char(st_x(a.the_geom), '999D999999') as lat_lon
into temp_sign_and_its_arrow_dir
from
org_sg_highway_only as a
left join org_ln_with_link_information_grouped as b
on a.jnctid=b.jnctid and a.inlink_id=b.inlinkid;

-- 表6：表4连org_ld表，根据org_ld.vilidity求出inlink的车线数。
drop table if exists temp_sign_and_its_arrow_dir_vilidity;
select distinct a.inlinkid, a.jnctid, a.tolinkid, a.from_lane, a.to_lane, a.lat_lon, length(b.validity)-1 as inlink_lane_count
into temp_sign_and_its_arrow_dir_vilidity
from
temp_sign_and_its_arrow_dir as a
left join org_ld as b
on a.inlinkid=b.id;

-- 表7：中间结果表1，未记有所有outlink信息，只记有流出lane信息和它所关联的流出link信息。
drop table if exists temp_target_table1;
select a.inlinkid, a.jnctid, a.inlink_lane_count, a.lat_lon, array_agg(tolinkid) as tolink_list, 
	array_agg(from_lane) as from_list, 
	array_agg(to_lane) as to_list
into temp_target_table1
from 
temp_sign_and_its_arrow_dir_vilidity as a
group by a.inlinkid, a.jnctid, a.inlink_lane_count, a.lat_lon;

-- 表8：表7连org_nw取得该highway-signpost点关联的所有outlink信息,
--      再连org_ld表，根据org_ld.vilidity求出inlink的车线数。
-- 目的：统计该highway-signpost点的所有outlink（注意与车线信息中的tolink区分，tolink应该是此处outlink的子集）
drop table if exists temp_highway_signpost_jnct_with_outlinkid;
select d.inlinkid, d.lat_lon, d.outlinkid, length(e.validity)-1 as outlink_lane_count
into temp_highway_signpost_jnct_with_outlinkid
from
(
select a.inlinkid, a.lat_lon, b.id as outlinkid
from temp_target_table1 as a
left join org_nw as b
on (a.jnctid=b.f_jnctid and (b.oneway='FT' or b.oneway is null)) 
where b.id is not null and a.inlinkid<>b.id
union 
select a.inlinkid, a.lat_lon, b.id as outlinkid
from temp_target_table1 as a
left join org_nw as b
on (a.jnctid=b.t_jnctid and (b.oneway='TF' or b.oneway is null))
where b.id is not null and a.inlinkid<>b.id
) as d
left join org_ld as e
on d.outlinkid=e.id;

-- 表9：将表8按照inlink分组，每条inlink对应到多条outlink。
drop table if exists temp_highway_signpost_jnct_with_outlinkid_grouped;
select a.inlinkid, a.lat_lon, array_agg(a.outlinkid) as outlink_list, array_agg(a.outlink_lane_count) as outlink_lanecount_list
into temp_highway_signpost_jnct_with_outlinkid_grouped
from 
temp_highway_signpost_jnct_with_outlinkid as a
group by a.inlinkid, a.lat_lon;

-- 表10：表7连表9，将所有信息组合到一起。
drop table if exists temp_target_table;
select a.*, b.outlink_list, b.outlink_lanecount_list
into temp_target_table
from temp_target_table1 as a
left join temp_highway_signpost_jnct_with_outlinkid_grouped as b
on a.inlinkid=b.inlinkid;














