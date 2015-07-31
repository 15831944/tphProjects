'''
Created on 2011-12-5

@author: hongchenzai
'''
import common
import base

class comp_tile(base.component_base.comp_base):
    '''
    Tile
    '''

    def __init__(self):
        '''
        Constructor
        '''
        base.component_base.comp_base.__init__(self, 'Tile')        
        
    def _DoCreateTable(self):
        
        if self.CreateTable2('middle_tile_node') == -1:
            return -1 
        if self.CreateTable2('middle_tile_link') == -1:
            return -1
        
        return 0
    
    def _DoCreateFunction(self):
        
        if self.CreateFunction2('rdb_make_tile_nodes') == -1:
            return -1
            
        if self.CreateFunction2('rdb_geom2tileid') == -1:
            return -1
        
        if self.CreateFunction2('rdb_make_tile_links') == -1:
            return -1
        
        return 0
        
    def _DoCreateIndex(self): 
        self.CreateIndex2('middle_tile_node_tile_node_id_idx')
        self.CreateIndex2('middle_tile_node_old_node_id_idx')
        self.CreateIndex2('middle_tile_link_tile_link_id_idx')
        self.CreateIndex2('middle_tile_link_old_link_id_idx')
        self.CreateIndex2('middle_tile_link_old_link_id_old_s_e_node_idx')
        self.CreateIndex2('middle_tile_link_old_link_id_split_seq_num_idx')
        return 0
    
    def _Do(self):
        
        if self._MakeTileNode() == -1:
            return -1
        
        if self._MakeTileLink() == -1:
            return -1
        
        return 0
    
    def _MakeTileNode(self):
        return 0
    
    def _MakeTileLink(self):
        return 0
    