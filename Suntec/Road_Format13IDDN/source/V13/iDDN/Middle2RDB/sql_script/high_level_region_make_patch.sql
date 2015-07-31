-- create first/last link regulaton info
------------------------------------------------------------------------------------
drop table if exists temp_link_regulation_add_first_link CASCADE;

select record_no, first_link_id, rdb_split_to_tileid(first_link_id) as tile_id into temp_link_regulation_add_first_link from
(
select record_no, cast((string_to_array(key_string, ','))[1] as bigint) as first_link_id from rdb_link_regulation
) as a;

alter table rdb_link_regulation
add column first_link_id bigint;

alter table rdb_link_regulation
add column first_link_id_t integer;

update rdb_link_regulation
set first_link_id = temp_link_regulation_add_first_link.first_link_id, first_link_id_t = temp_link_regulation_add_first_link.tile_id
from temp_link_regulation_add_first_link
where temp_link_regulation_add_first_link.record_no = rdb_link_regulation.record_no;

alter table rdb_link_regulation_client
add column first_link_id int;

alter table rdb_link_regulation_client
add column first_link_id_t integer;

drop table if exists temp_link_regulation_client_add_first_link CASCADE;

select record_no, cast(first_link_info[1] as int) as tile_id, cast(first_link_info[2] as int) as first_link_id
into temp_link_regulation_client_add_first_link from 
(
	select record_no, string_to_array(substr(string_array[1],1,length(string_array[1])-1), ',') as first_link_info  from
	(
		select record_no, string_to_array(substr(key_string,3, length(key_string)-3), ',[') as string_array from rdb_link_regulation_client
	) as a
) as b;


update rdb_link_regulation_client
set first_link_id = temp_link_regulation_client_add_first_link.first_link_id, first_link_id_t = temp_link_regulation_client_add_first_link.tile_id
from temp_link_regulation_client_add_first_link
where temp_link_regulation_client_add_first_link.record_no = rdb_link_regulation_client.record_no;

-- add section for regulaion flag in rdb_link
-- 0: without regulation
-- 1: with regulation and overide in first link
-- 2: with regualtion and override in last link
-- 3: with regulation and both in first/last link
drop table if exists rdb_link_regulation_exist_state CASCADE;

select link_id, rdb_split_tileid(link_id) as link_id_32, rdb_split_to_tileid(link_id) as link_id_32_t, case when array_length(regulation_exist_state_array,1) = 2 then 3 else regulation_exist_state_array[1] end as regulation_exist_state
into rdb_link_regulation_exist_state from 
(
	select link_id, array_agg(regulation_exist_state) as regulation_exist_state_array from
	(
		select link_id, regulation_exist_state from 
		(
			select first_link_id as link_id, 1 as regulation_exist_state from rdb_link_regulation
			union 
			select last_link_id as link_id, 2 as regulation_exist_state from rdb_link_regulation
		) as a
		order by link_id, regulation_exist_state
	) as b
	group by link_id
) as c;

create index rdb_link_regulation_exist_state_link_id_idx
on rdb_link_regulation_exist_state
using btree
(link_id);

CREATE INDEX rdb_link_regulation_first_link_id_t_idx
  ON rdb_link_regulation
  USING btree
  (first_link_id_t);

------------------------------------------------------------------------------------

---------------------------------
-- create view for binary data creation

-- create view
drop table if exists rdb_link_bigint_2_int_mapping CASCADE;

select link_id ,link_id_t, rdb_split_tileid(link_id) as link_id_32 into rdb_link_bigint_2_int_mapping from rdb_link;

create index rdb_link_bigint_2_int_mapping_linkid_idx
on rdb_link_bigint_2_int_mapping
using btree
(link_id);

create index rdb_link_bigint_2_int_mapping_linkid_32_idx
on rdb_link_bigint_2_int_mapping
using btree
(link_id_32);

create index rdb_link_bigint_2_int_mapping_linkid_t_idx
on rdb_link_bigint_2_int_mapping
using btree
(link_id_t);

drop table if exists rdb_node_bigint_2_int_mapping cascade;

select node_id, node_id_t, rdb_split_tileid(node_id) as node_id_32 into rdb_node_bigint_2_int_mapping from rdb_node;

create index rdb_node_bigint_2_int_mapping_nodeid_idx
on rdb_node_bigint_2_int_mapping
using btree
(node_id);

create index rdb_node_bigint_2_int_mapping_nodeid_32_idx
on rdb_node_bigint_2_int_mapping
using btree
(node_id_32);

create index rdb_node_bigint_2_int_mapping_nodeid_t_idx
on rdb_node_bigint_2_int_mapping
using btree
(node_id_t);
---------------------------------
