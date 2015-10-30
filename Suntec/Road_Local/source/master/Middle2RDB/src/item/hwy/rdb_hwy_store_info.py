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
        self.CreateTable2('rdb_highway_store_picture')
        self.CreateTable2('rdb_highway_store_name')
        if not self.pg.IsExistTable('rdb_highway_service_category_mapping'):
            self.log.error('no rdb_highway_service_category_mapping')
            return 0
        if not self.pg.IsExistTable('highway_store_info'):
            return 0
        tile_index = 0  # tile内的序号
        first_store_list = list()
        other_store_list = list()
        temp_store_info = None
        prev_tile = None
        prev_ic_no = None
        prev_service_id = None
        for store_info in self._get_store_info():
            ic_no = store_info[0]
            service_id = store_info[22]
            tile_id = store_info[23]
            if prev_tile != tile_id:
                tile_index = 0
                prev_tile = tile_id
            else:
                tile_index += 1
            temp_store_info = store_info + (tile_index,)
            if prev_ic_no != ic_no:
                self._insert_store_list(first_store_list, other_store_list)
                first_store_list = list()
                other_store_list = list()
                first_store_list.append(temp_store_info)
                prev_ic_no = ic_no
                prev_service_id = service_id
            else:
                if prev_service_id != service_id:
                    first_store_list.append(temp_store_info)
                    prev_service_id = service_id
                else:
                    other_store_list.append(temp_store_info)
        # Last IC_NO
        self._insert_store_list(first_store_list, other_store_list)
        self.pg.commit2()
        self._make_store_picture()
        self._make_store_name()
        # 更新IC表店铺数
        self._update_store_num()
        return 0

    def _insert_store_list(self, first_store_list, other_store_list):
        for first_store in first_store_list:
            self._insert_store_info(first_store)
        for other_store in other_store_list:
            self._insert_store_info(other_store)

    def _make_store_picture(self):
        '''店铺的品牌商标'''
        self.log.info('Making Store Picture.')
        if not self.pg.IsExistTable('highway_store_picture'):
            return 0
        sqlcmd = """
        INSERT INTO rdb_highway_store_picture(store_kind, picture_name)
        (
        SELECT store_kind, picture_name
          FROM highway_store_picture
          ORDER BY store_kind
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _make_store_name(self):
        if not self.pg.IsExistTable('highway_store_name'):
            return 0
        sqlcmd = """
        INSERT INTO rdb_highway_store_name(store_kind, name, language_code)
        (
        SELECT store_kind, name, language_code
          FROM highway_store_name
          ORDER BY store_kind, gid
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _insert_store_info(self, ic_store_info):
        sqlcmd = """
        INSERT INTO rdb_highway_store_info(
                    ic_no, bis_time_flag, bis_time_num, store_kind,
                    open_hour, open_min, close_hour, close_min,
                    holiday, goldenweek, newyear, yearend,
                    bonfestival, sunday, saturday, friday,
                    thursday, wednesday, tuesday, monday,
                    seq_nm, store_name,  service_id, tile_id,
                    tile_index)
            VALUES (%s, %s, %s, %s,
                    %s, %s, %s, %s,
                    %s, %s, %s, %s,
                    %s, %s, %s, %s,
                    %s, %s, %s, %s,
                    %s, %s, %s, %s,
                    %s);
        """
        self.pg.execute2(sqlcmd, ic_store_info)

    def _get_store_info(self):
        sqlcmd = """
        SELECT store.ic_no, bis_time_flag, bis_time_num, store_kind,
               open_hour, open_min, close_hour, close_min,
               holiday, goldenweek, newyear, yearend,
               bonfestival, sunday, saturday, friday,
               thursday, wednesday, tuesday, monday,
               seq_nm, store_name,  m.service_id,
               tile_id
          FROM highway_store_info as store
          LEFT JOIN highway_ic_info as ic
          ON store.ic_no = ic.ic_no
          LEFT JOIN rdb_highway_service_category_mapping as m
          ON store.service_kind = m.service
          order by store.ic_no, m.service_id, store.priority, store_kind
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
