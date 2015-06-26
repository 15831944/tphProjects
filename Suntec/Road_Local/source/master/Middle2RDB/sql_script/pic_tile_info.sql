CREATE TABLE rdb_pic_tile_info
(
  pic_id integer,
  tile_z integer,
  tile_x integer,
  tile_y integer,
  "type" smallint
);

insert into rdb_pic_tile_info(pic_id, tile_z, tile_x, tile_y, type)
(
	select pic_id, ((pic_tile_id >> 28) & 15) as tile_z, ((pic_tile_id >> 14) & 16383) as tile_x, (pic_tile_id & 16383) as tile_y, type from
	(
		select distinct(pic_id), pic_tile_id, type from
		(
			select pattern_id as pic_id, node_id_t as pic_tile_id, type from rdb_guideinfo_spotguidepoint
			union
			select arrow_id as pic_id, node_id_t as pic_tile_id, type from rdb_guideinfo_spotguidepoint
		) as a
	) as b
	where b.pic_id <> 0
);