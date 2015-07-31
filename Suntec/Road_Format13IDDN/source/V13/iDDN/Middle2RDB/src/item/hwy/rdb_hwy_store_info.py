# -*- coding: UTF8 -*-
'''
Created on 2013-10-10

@author: hongchenzai
'''
from base.rdb_ItemBase import ItemBase


class rdb_highway_store_info(ItemBase):
    '''店铺信息
    '''

    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self, 'Highway_Store')

    def Do(self):
        self.CreateTable2('rdb_highway_store_info')
        if self.pg.IsExistTable('highway_store_info') != True:
            return 0
        tile_index = 0  # tile内的序号
        prev_tile = None
        for store_info in self._get_point_info():
            tile_id = store_info[-1]
            if prev_tile != tile_id:
                tile_index = 0
                prev_tile = tile_id
            else:
                tile_index += 1
            self._insert_store_info(store_info + (tile_index, ))
        self.pg.commit2()
        # 更新IC表店铺数
        self._update_store_num()
        return 0

    def _insert_store_info(self, ic_store_info):
        sqlcmd = """
        INSERT INTO rdb_highway_store_info(
                    ic_no, bis_time_flag, bis_time_num, store_kind,
                    open_hour, open_min, close_hour, close_min,
                    holiday, goldenweek, newyear, yearend,
                    bonfestival, sunday, saturday, friday,
                    thursday, wednesday, tuesday, monday,
                    seq_nm, tile_id, tile_index)
            VALUES (%s, %s, %s, %s,
                    %s, %s, %s, %s,
                    %s, %s, %s, %s,
                    %s, %s, %s, %s,
                    %s, %s, %s, %s,
                    %s, %s, %s);
        """
        self.pg.execute2(sqlcmd, ic_store_info)

    def _get_point_info(self):
        sqlcmd = """
        SELECT store.ic_no, bis_time_flag, bis_time_num, store_kind,
               open_hour, open_min, close_hour, close_min,
               holiday, goldenweek, newyear, yearend,
               bonfestival, sunday, saturday, friday,
               thursday, wednesday, tuesday, monday,
               seq_nm, tile_id
          FROM highway_store_info as store
          LEFT JOIN highway_ic_info as ic
          ON store.ic_no = ic.ic_no
          ORDER BY tile_id, store.ic_no, store.gid
        """
        return self.pg.get_batch_data2(sqlcmd)

    def _update_store_num(self):
        '''更新IC表店铺数'''
        sqlcmd = """
        UPDATE rdb_highway_ic_info SET store_count = store_num
          FROM (
                SELECT ic_no, count(*) as store_num
                  FROM (
                        SELECT distinct ic_no, store_kind
                          FROM rdb_highway_store_info
                  ) as a
                  GROUP BY ic_no
           ) AS b
           where rdb_highway_ic_info.ic_no = b.ic_no
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return 0
