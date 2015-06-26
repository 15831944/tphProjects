'''
Created on 2011-12-5

@author: hongchenzai
'''
import component.component_base

class comp_tile_jpn(component.component_base.comp_base):
    '''
    classdocs
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Tile')        
        
    def _DoCreateTable(self):
        
        if self.CreateTable2('middle_tile_node') == -1:
            return -1 
        if self.CreateTable2('middle_tile_link') == -1:
            return -1
        
        return 0
    
    def _DoCreateFunction(self):
        
        if self.CreateFunction2('rdb_make_tile_nodes_ipc') == -1:
            return -1
            
        if self.CreateFunction2('rdb_geom2tileid') == -1:
            return -1
        
        if self.CreateFunction2('rdb_make_tile_links_ipc') == -1:
            return -1
        
        return 0
        
        
    def _Do(self):
        
        if self.__MakeTileNode() == -1:
            return -1
        
        if self.__MakeTileLink() == -1:
            return -1
        
        if self.__CreateCommonFunction() == -1:
            return -1
        
        return 0
    
    def __MakeTileNode(self):
        self.log.info('Start make tile nodes.')
     
        self.CreateIndex2('road_rnd_objectid_idx')
        
        self.log.info('Now it is inserting record into middle_tile_node...')
        if self.pg.callproc('rdb_make_tile_nodes_ipc') == -1:
            return -1
        else:
            self.pg.commit2()
            self.CreateIndex2('middle_tile_node_tile_node_id_idx')
            self.CreateIndex2('middle_tile_node_old_node_id_idx')
            
        self.log.info('End make tile nodes.')
        return 0
    
    def __MakeTileLink(self):
        self.log.info('Start make tile links.')
       
        self.CreateIndex2('road_rlk_objectid_idx')
        self.CreateIndex2('road_rlk_fromnodeid_idx')
        self.CreateIndex2('road_rlk_tonodeid_idx')
        

        self.log.info('Now it is inserting record into middle_tile_link...')
        if self.pg.callproc('rdb_make_tile_links_ipc') == -1:
            return -1
        else:
            self.pg.commit2()
            self.CreateIndex2('middle_tile_link_tile_link_id_idx')
            self.CreateIndex2('middle_tile_link_old_link_id_idx')
            self.CreateIndex2('middle_tile_link_old_link_id_old_s_e_node_idx')
            self.CreateIndex2('middle_tile_link_old_link_id_split_seq_num_idx')
            
        self.log.info('End make tile links.')
        return 0
    
    def __CreateCommonFunction(self):
        
        if self.CreateFunction2('getSplitLinkID') == -1:
            return -1
        
        if self.CreateFunction2('getSplitLinkIDArray') == -1:
            return -1
        
        return 0
    