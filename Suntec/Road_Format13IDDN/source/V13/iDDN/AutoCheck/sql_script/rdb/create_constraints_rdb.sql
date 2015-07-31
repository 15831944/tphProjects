-- create constraints for rdb
--------------------------------------------------------------------------------------- 
-- create index 
--------------------------------------------------------------------------------------- 
CREATE INDEX rdb_tile_node_tile_node_id_idx
  ON rdb_tile_node
  USING btree
  (tile_node_id);
