CREATE TABLE rdb_test
(
  gid serial NOT NULL PRIMARY KEY, 
  link_id bigint NOT NULL,
  link_id_t bigint NOT NULL,
  pos_cond_speed smallint NOT NULL,
  neg_cond_speed smallint NOT NULL,
  unit smallint NOT NULL
);
