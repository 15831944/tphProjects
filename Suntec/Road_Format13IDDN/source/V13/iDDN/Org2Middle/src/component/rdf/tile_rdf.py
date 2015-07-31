# -*- coding: cp936 -*-
'''
Created on 2011-12-5

@author: hongchenzai
'''
from component import tile

class comp_tile_rdf(tile.comp_tile):
    '''北美版Tile
    '''

    def __init__(self):
        '''
        Constructor
        '''
        tile.comp_tile.__init__(self, 'Tile')        
        
    
    def _MakeTileNode(self):
        self.log.info('Start make tile nodes.')
        
        self.log.info('Now it is inserting record into middle_tile_node...')
        if self.pg.callproc('mid_make_tile_nodes') == -1:
            return -1
        else:
            self.pg.commit2()
            self.CreateIndex2('middle_tile_node_tile_node_id_idx')
            self.CreateIndex2('middle_tile_node_old_node_id_idx')
            
        self.log.info('End make tile nodes.')
        return 0
    
    def _MakeTileLink(self):
        self.log.info('Start make tile links.')        

        self.log.info('Now it is inserting record into middle_tile_link...')
        if self.pg.callproc('mid_make_tile_links') == -1:
            return -1
        else:
            self.pg.commit2()
            
        self.log.info('End make tile links.')
        return 0
    
    