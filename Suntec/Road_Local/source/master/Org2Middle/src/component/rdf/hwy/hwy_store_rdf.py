# -*- coding: UTF-8 -*-
'''
Created on 2015-3-14

@author: hcz
'''
from component.jdb.hwy.hwy_store import HwyStore


class HwyStoreRDF(HwyStore):
    ''' highway store (For SA/PA)
    '''

    def __init__(self):
        '''
        Constructor
        '''
        HwyStore.__init__(self)

    def _Do(self):
        self.CreateTable2('highway_store_info')
        self.CreateTable2('highway_store_picture')
        self.CreateTable2('highway_store_name')
        self._make_store_info()
        self._make_store_picture()
        self._make_store_name()

    def _make_store_info(self):
        sqlcmd = """
        INSERT INTO highway_store_info(ic_no, bis_time_flag, bis_time_num,
                                       store_kind, open_hour, open_min,
                                       close_hour, close_min, holiday,
                                       goldenweek, newyear, yearend,
                                       bonfestival, sunday, saturday,
                                       friday, thursday, wednesday,
                                       tuesday, monday, seq_nm
                                       )
        (
        SELECT distinct ic_no, 0 as bis_time_flag, 0 as bis_time_num,
               store_chain_id as store_kind, 0 as open_hour, 0 as open_min,
               0 as close_hour, 0 as close_min, 0 as holiday,
               0 as goldenweek, 0 as newyear, 0 as yearend,
               0 as bonfestival,0 as sunday, 0 as saturday,
               0 as friday, 0 as thursday, 0 as wednesday,
               0 as tuesday, 0 as monday, 1 as seq_nm
          FROM hwy_store as a
          LEFT JOIN mid_hwy_ic_no as b
          ON a.road_code = b.road_code and
             a.road_seq = b.road_seq and
             a.updown_c = b.updown
          WHERE store_chain_id IS NOT NULL
          ORDER BY ic_no, store_kind
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _make_store_picture(self):
        sqlcmd = """
        INSERT INTO highway_store_picture(store_kind, picture_name)
        (
        SELECT distinct store_cat_id,
                        store_cat_id::character varying as picture_name
          FROM hwy_store
          WHERE store_cat_id IS NOT NULL
          ORDER BY store_cat_id
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _make_store_name(self):
        sqlcmd = """
        INSERT INTO highway_store_name(store_kind, name, language_code)
        (
        SELECT a.store_kind, name, language_code
          FROM (
            SELECT DISTINCT store_kind
             FROM highway_store_info
          ) as a
          INNER JOIN hwy_chain_name as b
          ON a.store_kind = b.store_chain_id
          ORDER BY a.store_kind, b.gid
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        # ## UPDATE STORE NAME
        sqlcmd = """
        UPDATE highway_store_info SET store_name = b.store_name
           FROM (
            SELECT store_kind,
                   array_to_string(array_agg(name), ',') as store_name
              FROM (
                SELECT store_kind, name
                  FROM highway_store_name
                  ORDER BY store_kind, gid
              ) as a
              GROUP BY store_kind
           ) AS b
           where highway_store_info.store_kind = b.store_kind
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
