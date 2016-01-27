# -*- coding: UTF8 -*-
'''
Created on 2015-10-14

@author: liushengqiang
'''



import platform.TestCase

class CCheckConnectivity(platform.TestCase.CTestCase):
    
    def _do(self):

        # 验证level 14道路连通性
        self._check_level14_road_network_connectivity()
        
        # 验证Region各层道路连通性
        region_layer_list = self.pg.GetRegionLayers()
        
        for layer_no in region_layer_list:
            self._check_region_layer_road_network_connectivity(layer_no)
        
        self._delete_no_use_tbl()
        
        return True
    
    def _make_temp_common_tbl(self, table_name = 'rdb_link'):
        
        # 表单temp_base_link_tbl记录指定层所有link的基本信息（主要与验证连通性相关的link属性）
        # 表单temp_links_to_search记录连通性验证过程中将要探索的link，已被探索过的link会从该表单中删除
        # 上述表单在连通性验证结束之后会被删除
        self.logger.info('Begin preparing data...')
        
        sqlcmd = """
                DROP TABLE IF EXISTS temp_base_link_tbl;
                CREATE TABLE temp_base_link_tbl
                AS (
                    SELECT link_id, start_node_id as s_node, end_node_id as e_node, one_way, road_type
                    FROM %s
                );
                
                DROP INDEX IF EXISTS temp_base_link_tbl_s_node_idx;
                CREATE INDEX temp_base_link_tbl_s_node_idx
                  ON temp_base_link_tbl
                  USING btree
                  (s_node);
                
                DROP INDEX IF EXISTS temp_base_link_tbl_e_node_idx;
                CREATE INDEX temp_base_link_tbl_e_node_idx
                  ON temp_base_link_tbl
                  USING btree
                  (e_node);
                
                analyze temp_base_link_tbl;
                
                DROP TABLE IF EXISTS temp_links_to_search;
                CREATE TABLE temp_links_to_search
                AS (
                    SELECT link_id, s_node, e_node, one_way
                    FROM temp_base_link_tbl
                    WHERE one_way !=0 and road_type not in (7, 8, 9, 14)
                );
                
                DROP INDEX IF EXISTS temp_links_to_search_link_id_idx;
                CREATE INDEX temp_links_to_search_link_id_idx
                  ON temp_links_to_search
                  USING btree
                  (link_id);
                
                DROP INDEX IF EXISTS temp_links_to_search_s_node_idx;
                CREATE INDEX temp_links_to_search_s_node_idx
                  ON temp_links_to_search
                  USING btree
                  (s_node);
                
                DROP INDEX IF EXISTS temp_links_to_search_e_node_idx;
                CREATE INDEX temp_links_to_search_e_node_idx
                  ON temp_links_to_search
                  USING btree
                  (e_node);
                  
                analyze temp_links_to_search;
            """ % (table_name)
        
        self.pg.execute(sqlcmd)
        self.pg.commit()
        
        self.logger.info('End preparing data.')
        return True
    
    def _check_level14_road_network_connectivity(self):
        
        # 验证level14道路连通性
        self.logger.info('Begin checking isolated road network from rdb_link...')
        
        self._make_temp_common_tbl()
        
        # 表单temp_rdb_region_walked_link_layer0_tbl记录level14 link与所属'岛'的对照关系
        
        sqlcmd = """
                DROP TABLE IF EXISTS temp_rdb_region_walked_link_layer0_tbl;
                CREATE TABLE temp_rdb_region_walked_link_layer0_tbl
                (
                    link_id bigint primary key,
                    island_id bigint,
                    the_geom geometry
                );
            """
        
        self.pg.execute(sqlcmd)
        self.pg.commit()
        
        # 作成level14 link与所属'岛'的对照关系
        self._fill_isolated_tbl()
        
        # 为方便在postgis上查看'岛'与link的对照关系，更新link形点
        self._update_the_geom_in_isolated_tbl()
        
        self.logger.info('End checking isolated road network from rdb_link.')  
        
        return True
    
    def _check_region_layer_road_network_connectivity(self, layer_no):
        
        # 验证Region层道路连通性
        region_table_name = """rdb_region_link_layer%s_tbl""" % (str(layer_no))
        self.logger.info('Begin checking isolated road network from ' + region_table_name + '...')
        
        self._make_temp_common_tbl(region_table_name)
        
        # 表单temp_rdb_region_walked_link_layerX_tbl记录Region layer X中的 link与所属'岛'的对照关系
        
        region_link_walked_tbl_name = """temp_rdb_region_walked_link_layer%s_tbl""" % (str(layer_no))
        sqlcmd = """
                DROP TABLE IF EXISTS %s;
                CREATE TABLE %s
                (
                    link_id bigint primary key,
                    island_id bigint,
                    the_geom geometry
                );
            """ % (region_link_walked_tbl_name, region_link_walked_tbl_name)
        
        self.pg.execute(sqlcmd)
        self.pg.commit()
        
        # 作成Region layer X中的 link与所属'岛'的对照关系
        
        self._fill_isolated_tbl(region_link_walked_tbl_name)
        
        # 为方便在postgis上查看'岛'与link的对照关系，更新link形点
        self._update_the_geom_in_isolated_tbl(region_link_walked_tbl_name, region_table_name)
        
        # 作成region层的'岛'与level14的'岛'的对照关系，以便验证道路分层是否导致孤岛
        self._make_region_island_to_level14_island_rel(layer_no)
        
        self.logger.info('End checking isolated road network from ' + region_table_name + '.')
        
        return True
    
    def _fill_isolated_tbl(self, table_name = 'temp_rdb_region_walked_link_layer0_tbl'):
        
        # 作成指定层的 link与所属'岛'的对照关系
        self.logger.info('Begin finding isolated road network ...')
        
        self.pg.CreateFunction_ByName('find_isolated_road_network_from_tbl')
        sqlcmd = """
                SELECT find_isolated_road_network_from_tbl('%s')
            """ % (str(table_name))
            
        self.pg.execute(sqlcmd)
        self.pg.commit()
        
        self.logger.info('End finding isolated road network .')
        return True
    
    def _update_the_geom_in_isolated_tbl(self, isolated_tbl = 'temp_rdb_region_walked_link_layer0_tbl', ref_tbl = 'rdb_link'):
        
        # 更新孤岛中link的形点信息，以便在postgis上查看
        self.logger.info('Begin updating geometry for isolated road network ...')
        
        sqlcmd = """
                UPDATE %s a
                SET the_geom = b.the_geom
                FROM %s b
                WHERE a.link_id = b.link_id
            """ % (isolated_tbl, ref_tbl)
        
        self.pg.execute(sqlcmd)
        self.pg.commit()
        
        self.logger.info('End updating geometry for isolated road network .')
        return True
    
    def _make_region_island_to_level14_island_rel(self, layer_no = '4', rdf_tbl = 'temp_rdb_region_walked_link_layer0_tbl'):
        
        # 作成region层的'岛'与level14的'岛'的对照关系，以便验证道路分层是否导致孤岛
        region_island_id_list = []
        ref_island_id = 1
        region_island_tbl = """temp_rdb_region_walked_link_layer%s_tbl""" % (str(layer_no))
        region_mapping_tbl = """rdb_region_layer%s_link_mapping""" % (str(layer_no))
        region_island_to_level14_island_rel_tbl = """region_layer%s_island_to_level14_island_rel_tbl""" % (str(layer_no))
        
        self.logger.info('Begin creating a relationship between region layer' + str(layer_no) + ' island and level14 island...')
        
        sqlcmd = """
                DROP TABLE IF EXISTS %s;
                CREATE TABLE %s
                (
                    region_island_id bigint,
                    island_id_14 bigint
                );
            """ % (str(region_island_to_level14_island_rel_tbl), str(region_island_to_level14_island_rel_tbl))
        
        self.pg.execute(sqlcmd)
        self.pg.commit()
        
        sqlcmd = """
                SELECT DISTINCT island_id
                FROM %s
                ORDER BY island_id
            """ % (region_island_tbl)
        
        rows = self.pg.get_batch_data(sqlcmd)
        for row in rows:
            region_island_id_list.append(row[0])
        
        for region_island_id in region_island_id_list:
            sqlcmd = """
                    SELECT *
                    FROM %s
                    WHERE region_link_id in (SELECT link_id FROM %s WHERE island_id = %s LIMIT 1)
                """ % (str(region_mapping_tbl), str(region_island_tbl), str(region_island_id))
                
            rows2 = self.pg.get_batch_data(sqlcmd)
            for row2 in rows2:
                link_id_14 = row2[1][0]
                                
            sqlcmd = """
                    SELECT island_id
                    FROM %s
                    WHERE link_id = %s
                """ % (str(rdf_tbl), str(link_id_14))
                    
            rows3 = self.pg.get_batch_data(sqlcmd)
            for row3 in rows3:
                ref_island_id = row3[0]
                                    
            sqlcmd = """
                    INSERT INTO %s(region_island_id, island_id_14)
                    VALUES(%s, %s)
                """ % (str(region_island_to_level14_island_rel_tbl), str(region_island_id), str(ref_island_id))
                        
            self.pg.execute(sqlcmd)
            self.pg.commit()          
        
        sqlcmd = """
                DROP TABLE IF EXISTS %s_bak;
                CREATE TABLE %s_bak AS (SELECT * FROM %s);
                DROP TABLE IF EXISTS %s;
                CREATE TABLE %s
                AS (
                    SELECT *
                    FROM (
                        SELECT island_id_14, array_agg(region_island_id) as region_island_list
                        FROM %s_bak
                        GROUP BY island_id_14
                    ) a
                    ORDER BY island_id_14
                );
                DROP TABLE IF EXISTS %s_bak;
            """ % (str(region_island_to_level14_island_rel_tbl), str(region_island_to_level14_island_rel_tbl), 
                   str(region_island_to_level14_island_rel_tbl), str(region_island_to_level14_island_rel_tbl), 
                   str(region_island_to_level14_island_rel_tbl), str(region_island_to_level14_island_rel_tbl), 
                   str(region_island_to_level14_island_rel_tbl))
        
        self.pg.execute(sqlcmd)
        self.pg.commit()
        
        self.logger.info('End creating a relationship between region layer' + str(layer_no) + ' island and level14 island.')
        return True
    
    def _delete_no_use_tbl(self):
        
        # 删除中间临时表单
        
        sqlcmd = """
                DROP TABLE IF EXISTS temp_base_link_tbl;
                DROP TABLE IF EXISTS temp_links_to_search;
            """
        
        self.pg.execute(sqlcmd)
        self.pg.commit()
        
        return True