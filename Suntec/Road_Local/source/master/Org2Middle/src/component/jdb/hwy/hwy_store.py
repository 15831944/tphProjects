# -*- coding: UTF8 -*-
'''
Created on 2014-6-25

@author: hongchenzai
'''
import component.component_base
from component.jdb.hwy.hwy_def import IC_TYPE_TRUE
from component.jdb.hwy.hwy_def import IC_DEFAULT_VAL


class HwyStore(component.component_base.comp_base):
    ''' highway store (For SA/PA)
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Highway_Store')

    def _Do(self):
        self.CreateTable2('highway_store_info')
        # Check店铺code对应表是否完整
        if self._CheckStoreCodeMapping() == False:
            self.log.error('Missing some store code in '
                           'table StoreCodeMapping.')
            return 0
        data = self._get_store_info()
        for info in data:
            ic_no = info[0]
            store_code_list = info[1]
            prev_store_code = None
            seq_nm = 1
            for store_info in \
                 zip(info[1], info[2], info[3], info[4], info[5],
                     info[6], info[7], info[8], info[9], info[10],
                     info[11], info[12], info[13], info[14], info[15],
                     info[16], info[17]):
                store_code = store_info[0]
                if not store_code:  # code为空
                    prev_store_code = None
                    continue
                if store_code != prev_store_code:
                    prev_store_code = store_code
                    seq_nm = 1
                else:
                    seq_nm += 1
                time_info = store_info[1:]
                # 営業時間有無
                if set(time_info) == set([0]):
                    time_flag = IC_DEFAULT_VAL
                    self.log.warning('営業時間無。ic_no=%s' % ic_no)
                else:
                    time_flag = IC_TYPE_TRUE
                # 营业时间情报数
                time_num = store_code_list.count(store_code)
                ic_store_info = (ic_no, time_flag, time_num,
                                 store_code) + time_info + (seq_nm,)
                self._insert_store_info(ic_store_info)
        self.pg.commit2()
        self._make_store_picture()
        return 0

    def _insert_store_info(self, ic_store_info):
        sqlcmd = """
        INSERT INTO highway_store_info(
                    ic_no, bis_time_flag, bis_time_num, store_kind,
                    open_hour, open_min, close_hour, close_min,
                    sunday, monday, tuesday, wednesday,
                    thursday, friday, saturday, bonfestival,
                    yearend, newyear, goldenweek, holiday,
                    seq_nm)
            VALUES (%s, %s, %s, %s,
                    %s, %s, %s, %s,
                    %s, %s, %s, %s,
                    %s, %s, %s, %s,
                    %s, %s, %s, %s,
                    %s);
        """
        self.pg.execute2(sqlcmd, ic_store_info)

    def _get_store_info(self):
        sqlcmd = """
        SELECT ic_no, array_agg(ddn_store_code) as store_code,
              array_agg(start_hour) start_hour, array_agg(start_min) start_min,
              array_agg(end_hour) end_hour, array_agg(end_min) end_min,
              array_agg(sunday_f) sunday, array_agg(monday_f) monday,
              array_agg(tuesday_f) tuesday, array_agg(wednesday_f) wednesday,
              array_agg(thursday_f) thursday, array_agg(friday_f) friday,
              array_agg(saturday_f) saturday, array_agg(bon_f) bon,
              array_agg(yearend_f) yearend, array_agg(newyear_f) newyear,
              array_agg(goldenweek_f) goldenweek, array_agg(holiday_f) holiday
          FROM (
                SELECT ic_no, ddn_store_code,
                       start_hour, start_min,
                       end_hour, end_min,
                       sunday_f, monday_f,
                       tuesday_f, wednesday_f,
                       thursday_f, friday_f,
                       saturday_f, bon_f,
                       yearend_f, newyear_f,
                       goldenweek_f, holiday_f,
                       "name"
                  FROM access_store
                  LEFT JOIN store_code
                  ON multiplestore_c = mastercode
                  LEFT JOIN store_code_mapping
                  ON mastercode = org_store_code
                  LEFT JOIN access_point_4326 as ap
                  ON accesspoint_id = ap.objectid
                  LEFT JOIN (
                        SELECT distinct ic_no, road_code, road_point, updown
                          FROM mid_hwy_ic_no
                  ) as ic
                  ON ap.road_code = ic.road_code
                     and ap.road_seq = ic.road_point
                     and ap.direction_c = ic.updown
                  order by ic_no, multiplestore_c DESC, access_store.objectid
          ) AS b
          group by ic_no;
        """
        return self.get_batch_data(sqlcmd)

    def _make_store_picture(self):
        '''店铺的品牌商标'''
        self.log.info('Making Store Picture.')
        self.CreateTable2('highway_store_picture')
        sqlcmd = """
        INSERT INTO highway_store_picture(store_kind, picture_name)
        (
        SELECT ddn_store_code AS store_kind, picture_name
          FROM store_code_mapping
          WHERE picture_name is not null
          ORDER BY store_kind
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _CheckStoreCodeMapping(self):
        'Check店铺code对应表是否完整'
        sqlcmd = """
        SELECT count(*)
          FROM store_code
          where mastercode not in (
                SELECT org_store_code
                 FROM store_code_mapping
               );
        """
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchone2()
        if row:
            if row[0] == 0:
                return True
        return False
