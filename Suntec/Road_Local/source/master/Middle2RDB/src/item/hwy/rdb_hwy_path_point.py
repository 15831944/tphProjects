# -*- coding: UTF8 -*-
'''
Created on 2013-10-10

@author: hongchenzai
'''
from base.rdb_ItemBase import ItemBase


class rdb_hwy_path_point(ItemBase):
    '''高速探索地点情報
    '''

    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self, 'Highway_PathPoint')

    def Do(self):
        self.CreateTable2('rdb_highway_path_point')
        # self.CreateFunction2('rdb_geom_2_lonlat')
        if self.pg.IsExistTable('highway_path_point') != True:
            return 0
        sqlcmd = """
        INSERT INTO rdb_highway_path_point(
                    ic_no, enter_flag, exit_flag,
                    main_flag, center_flag, new_flag,
                    unopen_flag, link_id, lon,
                    lat, tile_id, "index",
                    node_id, the_geom, link_geom)
            VALUES (%s, %s, %s,
                    %s, %s, %s,
                    %s, %s, %s,
                    %s, %s, %s,
                    %s, ST_GeomFromEWKT(%s), ST_GeomFromEWKT(%s));
        """
        tile_index = 0  # tile内的序号
        prev_tile = None
        for info in self._get_point_info():
            ic_no, enter_flag, exit_flag = info[0:3]
            main_flag, center_flag, new_flag = info[3:6]
            unopen_flag, link_id = info[6:8]
            lon, lat = info[8:10]
            tile_id, node_id, the_geom = info[10:13]
            link_geom = info[13]
            if prev_tile != tile_id:
                tile_index = 0
                prev_tile = tile_id
            self.pg.execute2(sqlcmd, (ic_no, enter_flag, exit_flag,
                                      main_flag, center_flag, new_flag,
                                      unopen_flag, link_id, lon,
                                      lat, tile_id, tile_index,
                                      node_id, the_geom, link_geom)
                             )
            tile_index += 1
        self.pg.commit2()
        self.CreateIndex2('rdb_highway_path_point_ic_no_idx')
        self.CreateTable2('rdb_highway_path_point_link_geom_idx')
        self.CreateIndex2('rdb_highway_path_point_the_geom_idx')
        return 0

    def _get_point_info(self):
        sqlcmd = """
        SELECT ic_no, enter_flag, exit_flag,
               main_flag, center_flag, new_flag,
               unopen_flag, tile_link_id,
               round(st_x(n.the_geom) * 3600 * 256)::INTEGER as lon,
               round(st_y(n.the_geom) * 3600 * 256)::INTEGER as lat,
               p.tile_id, tile_node_id, ST_ASEWKT(n.the_geom),
               ST_ASEWKT(link_tbl.the_geom)
          FROM highway_path_point AS p
          LEFT JOIN rdb_tile_link
          ON link_id = old_link_id
          LEFT JOIN rdb_tile_node
          ON node_id = old_node_id
          LEFT JOIN node_tbl as n
          ON p.node_id = n.node_id
          LEFT JOIN link_tbl
          ON p.link_id = link_tbl.link_id
          where pos_flag = 0
          order by tile_id, ic_no, p.gid
        """
        return self.pg.get_batch_data2(sqlcmd)
