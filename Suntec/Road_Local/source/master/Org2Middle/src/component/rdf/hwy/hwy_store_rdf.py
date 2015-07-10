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
        self._check_store_kind()

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
               u_code as store_kind, 0 as open_hour, 0 as open_min,
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
          LEFT JOIN hwy_chain_name as c
          ON a.store_cat_id = c.cat_id and
             a.sub_cat = c.sub_cat and
             a.store_chain_id = c.chain_id and
             a.chain_name = c.chain_name
          WHERE u_code IS NOT NULL
          ORDER BY ic_no, store_kind
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _make_store_picture(self):
        # 17cy不做(store_kind就做为图片id)
        return 0
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
        SELECT a.store_kind, chain_name, language_code
          FROM (
            SELECT DISTINCT store_kind
             FROM highway_store_info
          ) as a
          INNER JOIN hwy_chain_name as b
          ON a.store_kind = b.u_code
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

    def _check_store_kind(self):
        sqlcmd = """
        SELECT distinct a.store_chain_id, a.chain_name, chain_names
          FROM hwy_store as a
          LEFT JOIN (
            SELECT u_code, cat_id, sub_cat,
                   chain_id, array_agg(chain_name) as chain_names
              FROM (
                select u_code, cat_id, sub_cat, chain_id, chain_name
                  from hwy_chain_name
                  order by gid
              ) AS a
              group by u_code, cat_id, sub_cat, chain_id
          ) as b
          ON a.store_cat_id = b.cat_id and
             a.sub_cat = b.sub_cat and
             a.store_chain_id = b.chain_id and
             ((a.chain_name <> '' and a.chain_name = any(chain_names))
              or (a.chain_name = '' or a.chain_name is null))
          WHERE u_code IS NULL
          ORDER BY a.store_chain_id, a.chain_name
        """
        for row in self.pg.get_batch_data2(sqlcmd):
            store_chain_id = row[0]
            chain_name = row[1]
            chain_names = row[2]
            if chain_name:
                self.log.warning('No store kind/u_code for store=%s'
                                 % chain_name)
            elif chain_names:
                self.log.warning('No store kind/u_code for store=%s'
                                 % chain_names[0])
            elif store_chain_id:
                self.log.warning('No store kind/u_code for chain=%s'
                                 % chain_names[0])
            else:
                self.log.error('No store kind/u_code')
