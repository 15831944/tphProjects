# -*- coding: UTF8 -*-
'''
Created on 2014-4-29

@author: hongchenzai
'''
from base.rdb_ItemBase import ItemBase


class rdb_hwy_conn_info(ItemBase):
    '''接续情报
    '''

    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self, 'Highway_Conn')

    def Do(self):
        self.CreateTable2('rdb_highway_conn_info')
        if self.pg.IsExistTable('highway_conn_info') != True:
            return 0
        sqlcmd = """
        INSERT INTO rdb_highway_conn_info(
                    ic_no, road_attr, conn_direction,
                    same_road_flag, tile_change_flag, uturn_flag,
                    new_flag, unopen_flag, vics_flag,
                    toll_flag, conn_road_no, conn_ic_no,
                    conn_link_length, conn_tile_id, toll_index,
                    toll_count, vics_index, vics_count,
                    tile_id, "index")
            VALUES (%s, %s, %s,
                    %s, %s, %s,
                    %s, %s, %s,
                    %s, %s, %s,
                    %s, %s, %s,
                    %s, %s, %s,
                    %s, %s);
        """
        tile_index = 0  # tile内的序号
        prev_tile = None
        for info in self._get_point_info():
            ic_no, road_attr, conn_direction = info[0:3]
            same_road_flag, tile_change_flag, uturn = info[3:6]
            new_flag, unopen_flag, vics_flag = info[6:9]
            toll_flag, conn_road_no, conn_ic_no = info[9:12]
            link_length, conn_tile_id, toll_count = info[12:15]
            vics_count, tile_id = info[15:17]
            toll_index = 0
            vics_index = 0
            if prev_tile != tile_id:
                tile_index = 0
                prev_tile = tile_id
            self.pg.execute2(sqlcmd, (ic_no, road_attr, conn_direction,
                                      same_road_flag, tile_change_flag, uturn,
                                      new_flag, unopen_flag, vics_flag,
                                      toll_flag, conn_road_no, conn_ic_no,
                                      link_length, conn_tile_id, toll_index,
                                      toll_count, vics_index, vics_count,
                                      tile_id, tile_index
                                      )
                             )
            tile_index += 1
        self.pg.commit2()
        self.CreateIndex2('rdb_highway_conn_info_ic_no_idx')
        self.CreateIndex2('rdb_highway_conn_info_tile_id_idx')
        return 0

    def _get_point_info(self):
        sqlcmd = """
        SELECT ic_no, road_attr, conn_direction,
               same_road_flag, tile_change_flag, uturn_flag,
               new_flag, unopen_flag, vics_flag,
               toll_flag, conn_road_no, conn_ic_no,
               round(conn_link_length), conn_tile_id, toll_count,
               vics_count, tile_id
          FROM highway_conn_info
          ORDER BY tile_id, ic_no, gid
        """
        return self.pg.get_batch_data2(sqlcmd)
