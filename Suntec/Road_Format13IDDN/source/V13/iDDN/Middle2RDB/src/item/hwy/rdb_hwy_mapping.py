# -*- coding: UTF8 -*-
'''
Created on 2013-10-22

@author: hongchenzai
'''

from base.rdb_ItemBase import ItemBase


class rdb_hwy_mapping(ItemBase):
    '''高速施设
    '''

    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self, 'Highway_Mapping')

    def Do(self):
        self.CreateTable2('rdb_highway_mapping')
        if self.pg.IsExistTable('highway_mapping') != True:
            return 0
        sqlcmd = """
        INSERT INTO rdb_highway_mapping(road_kind, ic_count, road_no,
                            display_class, link_id, forward_tile_id,
                            forward_ic_no, backward_tile_id, backward_ic_no,
                            path_type, tile_id, the_geom)
        (
        SELECT road_kind, ic_count, mapping.road_no,
               mapping.display_class, tile_link_id, fwd.tile_id as fwd_tile_id,
               forward_ic_no, bwd.tile_id as bwd_tile_id, backward_ic_no,
               path_type, mapping.tile_id, the_geom
          FROM highway_mapping as mapping
          LEFT JOIN rdb_tile_link
          on link_id = old_link_id
          LEFT JOIN highway_ic_info as fwd
          ON forward_ic_no = fwd.ic_no
             and (fwd.boundary <> 1 or
               (fwd.boundary = 1 and mapping.tile_id = fwd.tile_id))
          LEFT JOIN highway_ic_info as bwd
          ON backward_ic_no = bwd.ic_no
             and (bwd.boundary <> 1 or
             (bwd.boundary = 1 and mapping.tile_id = bwd.tile_id))
          LEFT JOIN link_tbl
          on mapping.link_id = link_tbl.link_id
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('rdb_highway_mapping_link_id_idx')
        self.CreateIndex2('rdb_highway_mapping_tile_id_idx')
        self.CreateIndex2('rdb_highway_mapping_the_geom_idx')
        self.CreateIndex2('rdb_highway_mapping_link_id_forward_ic_no_backward_ic_no_idx')
        return 0
