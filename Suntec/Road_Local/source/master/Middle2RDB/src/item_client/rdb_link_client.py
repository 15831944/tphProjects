# -*- coding: cp936 -*-
'''
Created on 2011-12-22

@author: liuxinxing
'''
from base.rdb_ItemBase import ItemBase
from common import rdb_log, rdb_database

class rdb_link_client(ItemBase):
    '''
    rdb_link_client类
    '''
    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self, 'Link_client'
                          , 'rdb_link'
                          , 'gid'
                          , 'rdb_link_client'
                          , 'link_id'
                          , False)
        
    def Do_CreateTable(self):
        return self.CreateTable2('rdb_link_client')
        return 0
    
    def Do_CreateFunction(self):    
        if self.CreateFunction2('rdb_merge_common_link_attribute') == -1:
            return -1
        if self.CreateFunction2('rdb_merge_multi_link_attribute') == -1:
            return -1
        if self.CreateFunction2('rdb_smallint_2_octal_bytea') == -1:
            return -1
        if self.CreateFunction2('rdb_integer_2_octal_bytea') == -1:
            return -1
        if self.CreateFunction2('rdb_makeshape2bytea') == -1:
            return -1
        if self.CreateFunction2('rdb_split_tileid') == -1:
            return -1

        if self.CreateFunction2('lonlat2pixel_tile') == -1:
            return -1
        if self.CreateFunction2('lonlat2pixel') == -1:
            return -1
        if self.CreateFunction2('tile_bbox') == -1:
            return -1
        if self.CreateFunction2('world2lonlat') == -1:
            return -1
        if self.CreateFunction2('lonlat2world') == -1:
            return -1
        if self.CreateFunction2('pixel2world') == -1:
            return -1

        if self.CreateFunction2('rdb_makeshape2pixelbytea') == -1:
            return -1
             
        return 0
    
    def Do(self):
        
        # 求最小和最大id
        sqlcmd = """
            SELECT min(gid), max(gid)
              FROM rdb_link;
            """
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchone2()
        min_id  = row[0]
        max_id  = row[1]
        
        sqlcmd = """
                insert into rdb_link_client (
                    link_id, link_id_t, start_node_id, start_node_id_t, end_node_id, end_node_id_t, link_type, link_length, 
                    road_name, lane_id, common_main_link_attri, multi_link_attri, fnlink, fnlink_t, tnlink, 
                    tnlink_t, geom_blob, extend_flag, bypass_flag, matching_flag, highcost_flag, 
                    fazm_path, tazm_path, shield_flag, the_geom)
                (
                select rdb_split_tileid(link_id), link_id_t, 
                       rdb_split_tileid(start_node_id), start_node_id_t, 
                       rdb_split_tileid(end_node_id), end_node_id_t, 
                       link_type, link_length, road_name, lane_id, 
                       rdb_merge_common_link_attribute(
                                                extend_flag,
                                                toll,
                                                function_code,
                                                road_type, 
                                                display_class
                                                ),
                       rdb_merge_multi_link_attribute(
                                                one_way,
                                                regulation_flag,
                                                tilt_flag,
                                                speed_regulation_flag,
                                                link_add_info_flag,
                                                shield_flag,
                                                extend_flag,
                                                link_type
                                                ),
                        rdb_split_tileid(fnlink), fnlink_t, rdb_split_tileid(tnlink), tnlink_t,
                        --rdb_makeshape2pixelbytea(14::smallint, (link_id_t >> 14) & 16383, (link_id_t & 16383), the_geom ) as geom_blob,
                        null as geom_blob,
                        extend_flag,
                        bypass_flag, 
                        matching_flag, 
                        highcost_flag,
                        fazm_path, 
                        tazm_path,
                        shield_flag,
                        the_geom 
                  from rdb_link 
				  where gid >= %s and gid <= %s
                );
                """
        rdb_log.log(self.ItemName, 'Inserting records into rdb_link_client...', 'info')
        self._call_child_thread(min_id, max_id, sqlcmd, 4, 1000000)
        
        return 0
    
        
    
    def Do_CreatIndex(self):
        '创建相关表索引.'
        self.CreateIndex2('rdb_link_client_start_node_id_idx')
        self.CreateIndex2('rdb_link_client_start_node_id_t_idx')
        self.CreateIndex2('rdb_link_client_end_node_id_idx')
        self.CreateIndex2('rdb_link_client_end_node_id_t_idx')
        self.CreateIndex2('rdb_link_client_link_id_idx')
        self.CreateIndex2('rdb_link_client_link_id_t_idx')
        self.CreateIndex2('rdb_link_client_link_type_idx')
        self.CreateIndex2('rdb_link_client_the_geom_idx')
        return 0
    
    def _DoContraints(self):
        '添加外键'
        sqlcmd = """
                ALTER TABLE rdb_link_client DROP CONSTRAINT if exists rdb_link_client_end_node_id_fkey;    
                ALTER TABLE rdb_link_client
                  ADD CONSTRAINT rdb_link_client_end_node_id_fkey FOREIGN KEY (end_node_id)
                      REFERENCES rdb_node_client (node_id) MATCH FULL
                      ON UPDATE NO ACTION ON DELETE NO ACTION;
                """
        #if self.pg.execute2(sqlcmd) == -1:
        #    return -1
        #else:
        #    self.pg.commit2()
        return 0
        
