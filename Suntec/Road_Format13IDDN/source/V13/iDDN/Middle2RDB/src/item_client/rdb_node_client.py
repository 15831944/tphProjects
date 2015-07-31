# -*- coding: cp936 -*-
'''
Created on 2011-12-22

@author: liuxinxing
'''
from base.rdb_ItemBase import ItemBase
from common import rdb_log

class rdb_node_client(ItemBase):
    '''
    rdb_node_client类
    '''
    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self, 'Node_client'
                          , 'rdb_node'
                          , 'gid'
                          , 'rdb_node_client'
                          , 'node_id'
                          , False)
        
    def Do_CreateTable(self):
        return self.CreateTable2('rdb_node_client')
    
    def Do_CreateFunction(self): 
        if self.CreateFunction2('rdb_integer_2_octal_bytea') == -1:
            return -1
        if self.CreateFunction2('rdb_smallint_2_octal_bytea') == -1:
            return -1
        if self.CreateFunction2('rdb_split_tileid') == -1:
            return -1
        if self.CreateFunction2('rdb_cvt_branch_to_json_format') == -1:
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
        if self.CreateFunction2('rdb_makenode2pixelbytea') == -1:
            return -1
        return 0
        
    def Do(self):
        # 求最小和最大id
        sqlcmd = """
            SELECT min(gid), max(gid)
              FROM rdb_node;
            """
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchone2()
        min_id  = row[0]
        max_id  = row[1]
        
        sqlcmd = """
                INSERT INTO rdb_node_client(node_id, node_id_t, extend_flag, branches, geom_blob, the_geom)
                (
                SELECT rdb_split_tileid(node_id), node_id_t, extend_flag, rdb_cvt_branch_to_json_format(branches),
                       rdb_makenode2pixelbytea(14::smallint, ((node_id_t >> 14) & 16383), (node_id_t & 16383), the_geom_4096 ),
                       the_geom
                  FROM rdb_node
                  where gid >= %s and gid <= %s
                );
                """
        rdb_log.log(self.ItemName, 'Inserting records into rdb_node_client...(30minutes)', 'info')
        self._call_child_thread(min_id, max_id, sqlcmd, 4, 1000000)
        
        return 0
    
    def Do_CreatIndex(self):
        '创建相关表索引.'
        self.CreateIndex2('rdb_node_client_node_id_t_idx')
        self.CreateIndex2('rdb_node_client_the_geom_idx')
        return 0
        
        
        
        