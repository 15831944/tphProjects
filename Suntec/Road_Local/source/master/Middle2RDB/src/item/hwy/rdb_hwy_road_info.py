# -*- coding: UTF8 -*-
'''
Created on 2013-10-10

@author: hongchenzai
'''
from base.rdb_ItemBase import ItemBase


class rdb_highway_road_info(ItemBase):
    '''高速道路信息
    '''

    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self, 'Highway_Road')

    def Do(self):
        # 当前不做特别处理，只是简单的
        self.CreateTable2('rdb_highway_road_info')
        if self.pg.IsExistTable('highway_road_info') != True:
            return 0
        tile_index = 0  # tile内的序号
        prev_tile = None
        for road_info in self._get_road_info():
            tile_id = road_info[-1]
            if prev_tile != tile_id:
                tile_index = 0
                prev_tile = tile_id
            else:
                tile_index += 1
            self._insert_road_info(road_info + (tile_index, ))
        self.pg.commit2()
        # 更新IC表,道路情报的index
        self._update_road_index()
        self.CreateIndex2('rdb_highway_road_info_tile_id_idx')
        return 0

    def _insert_road_info(self, road_info):
        sqlcmd = """
        INSERT INTO rdb_highway_road_info(
                    road_no, iddn_road_kind, road_kind,
                    road_attr, loop, "new",
                    un_open, start_ic_no, ic_count,
                    "name", up_down, tile_id,
                    tile_index)
            values(%s, %s, %s, %s, %s, %s,
                   %s, %s, %s, %s, %s, %s,
                   %s)
        """
        self.pg.execute2(sqlcmd, road_info)

    def _get_road_info(self):
        sqlcmd = """
        SELECT a.road_no, iddn_road_kind, road_kind,
               road_attr, loop, "new",
               un_open, start_ic_no, ic_count,
               "name", c.up_down, a.tile_id
          from (
                -- Get Start IC No
                SELECT tile_id, road_no, up_down,
                       MIN(ic_no) as start_ic_no
                  FROM rdb_highway_ic_info
                  group by tile_id, road_no, up_down
          ) as a
          LEFT JOIN (
                -- Get IC Count
                SELECT tile_id, road_no, up_down,
                       count(ic_no) as ic_count
                  FROM rdb_highway_ic_info
                  group by tile_id, road_no, up_down
          ) as b
          ON a.tile_id = b.tile_id and
             a.road_no = b.road_no and
             a.up_down = b.up_down
          LEFT JOIN highway_road_info as c
          ON a.road_no = c.road_no and a.up_down = c.up_down
          ORDER BY tile_id, a.road_no, c.up_down;
        """
        return self.pg.get_batch_data2(sqlcmd)

    def _update_road_index(self):
        '''更新IC表道路情报的index'''
        sqlcmd = """
        UPDATE rdb_highway_ic_info as ic SET road_index = road.tile_index
          FROM rdb_highway_road_info as road
          WHERE ic.tile_id = road.tile_id
               and ic.road_no = road.road_no
               and ic.up_down = road.up_down
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _DoContraints(self):
        return 0
