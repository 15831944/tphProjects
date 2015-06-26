# -*- coding: UTF8 -*-
'''
Created on 2013-10-11

@author: hongchenzai
'''
from base.rdb_ItemBase import ItemBase
from common import rdb_log
from item.hwy import rdb_hwy_ic_info
from item.hwy import rdb_hwy_path_point
from item.hwy import rdb_hwy_road_info
from item.hwy import rdb_hwy_sapa_info
from item.hwy import rdb_hwy_store_info
from item.hwy import rdb_hwy_mapping
from item.hwy import rdb_hwy_toll
from item.hwy import rdb_hwy_conn_info
from item.hwy import rdb_hwy_node_add_info
from item.hwy import rdb_hwy_fee


class rdb_highway(ItemBase):
    '''Highway
    '''

    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self, 'Highway')
        self.hwy_comp_list = [
                              rdb_hwy_mapping.rdb_hwy_mapping(),
                              rdb_hwy_path_point.rdb_hwy_path_point(),
                              rdb_hwy_conn_info.rdb_hwy_conn_info(),
                              rdb_hwy_ic_info.rdb_hwy_ic_info(),
                              rdb_hwy_toll.rdb_hwy_toll(),
                              rdb_hwy_road_info.rdb_highway_road_info(),
                              rdb_hwy_sapa_info.rdb_highway_sapa_info(),
                              rdb_hwy_store_info.rdb_highway_store_info(),
                              rdb_hwy_node_add_info.rdb_hwy_node_add_info(),
                              rdb_hwy_fee.rdb_hwy_fee()
                              ]

    def Do(self):
        for comp in self.hwy_comp_list:
            comp.Make()
