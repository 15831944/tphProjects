# -*- coding: UTF8 -*-
'''
Created on 2013-10-10

@author: hongchenzai
'''

from base.rdb_ItemBase import ItemBase
from common import rdb_log


class rdb_hwy_ic_info(ItemBase):
    '''高速施设
    '''

    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self, 'Highway_IC')

    def Do(self):
        # 注： 距离四舍五入处理
        self.CreateTable2('rdb_highway_ic_info')
        self.CreateTable2('rdb_highway_ic_mapping')
        if not self.pg.IsExistTable('highway_ic_info'):
            return 0
        sqlcmd = """
        INSERT INTO rdb_highway_ic_info(
                    ic_no, up_down, facility_id,
                    between_distance, inside_distance, enter,
                    exit, tollgate, jct,
                    pa, sa, ic,
                    ramp, smart_ic, barrier,
                    dummy, boundary, "new",
                    unopen, forward_flag, reverse_flag,
                    toll_count, enter_direction, path_count,
                    vics_count, conn_count, illust_count,
                    store_count, servise_flag, toll_index,
                    vics_index, road_no, "name",
                    conn_tile_id, tile_id, "index")
            VALUES (%s, %s, %s,
                    %s, %s, %s,
                    %s, %s, %s,
                    %s, %s, %s,
                    %s, %s, %s,
                    %s, %s, %s,
                    %s, %s, %s,
                    %s, %s, %s,
                    %s, %s, %s,
                    %s, %s, %s,
                    %s, %s, %s,
                    %s, %s, %s);
        """
        tile_index = 0  # tile内的序号
        prev_tile = None
        for info in self._get_point_info():
            ic_no, up_down, facility_id = info[0:3]
            between_distance, inside_distance, enter = info[3:6]
            exit_flag, tollgate, jct = info[6:9]
            pa, sa, ic = info[9:12]
            ramp, smart_ic, barrier = info[12:15]
            dummy, boundary, new = info[15:18]
            unopen, forward_flag, reverse_flag = info[18:21]
            toll_count, enter_direction, path_count = info[21:24]
            vics_count, conn_count, illust_count = info[24:27]
            store_count, servise_flag, road_no = info[27:30]
            name, conn_tile_id, tile_id = info[30:33]
            toll_index = 0
            vics_index = 0
            if prev_tile != tile_id:
                tile_index = 0
                prev_tile = tile_id
            self.pg.execute2(sqlcmd, (ic_no, up_down, facility_id,
                                      between_distance, inside_distance, enter,
                                      exit_flag, tollgate, jct,
                                      pa, sa, ic,
                                      ramp, smart_ic, barrier,
                                      dummy, boundary, new,
                                      unopen, forward_flag, reverse_flag,
                                      toll_count, enter_direction, path_count,
                                      vics_count, conn_count, illust_count,
                                      store_count, servise_flag, toll_index,
                                      vics_index, road_no, name,
                                      conn_tile_id, tile_id, tile_index
                                      )
                             )
            tile_index += 1
        self.pg.commit2()
        self.CreateIndex2('rdb_highway_ic_info_ic_no_idx')
        self.CreateIndex2('rdb_highway_ic_info_tile_id_idx')
        self.CreateIndex2('rdb_highway_ic_info_up_down_facility_id_idx')
        # 生成设施番号到IC_NO的Mapping
        self._make_highway_ic_mapping()
        self._create_ic_info_view()
        return 0

    def _get_point_info(self):
        sqlcmd = """
        SELECT ic_no, up_down, facility_id,
               round(between_distance), round(inside_distance), enter,
               exit, tollgate, jct,
               pa, sa, ic,
               ramp, smart_ic, barrier,
               dummy, boundary, "new",
               unopen, forward_flag, reverse_flag,
               toll_count, enter_direction, path_count,
               vics_count, conn_count, illust_count,
               store_count, servise_flag, road_no,
               "name", conn_tile_id, tile_id
          FROM highway_ic_info as a
          ORDER BY tile_id, ic_no
        """
        return self.pg.get_batch_data2(sqlcmd)

    def _make_highway_ic_mapping(self):
        sqlcmd = """
        INSERT INTO rdb_highway_ic_mapping(up_down, facility_id,
                                           ic_no, tile_id, ic_index)
        (
        SELECT up_down, facility_id, ic_no, tile_id, "index"
          FROM rdb_highway_ic_info
          where boundary <>  1
          order by tile_id, up_down, facility_id, ic_no
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('rdb_highway_ic_mapping_up_down_facility_id_idx')

    def _create_ic_info_view(self):
        sqlcmd = '''
        CREATE OR REPLACE VIEW rdb_highway_ic_info_view
        AS
        (
            select ic_no ,up_down,facility_id,'JCT' as ic_type
            from rdb_highway_ic_info
            where jct = 1
            union
            select ic_no ,up_down,facility_id,'PA' as ic_type
            from rdb_highway_ic_info
            where pa = 1
            union
            select ic_no ,up_down,facility_id,'SA' as ic_type
            from rdb_highway_ic_info
            where sa = 1
            union
            select ic_no ,up_down,facility_id,'Tollgate' as ic_type
            from rdb_highway_ic_info
            where tollgate = 1
            union
            select ic_no ,up_down,facility_id,'IC' as ic_type
            from rdb_highway_ic_info
            where ic = 1
            union
            select ic_no ,up_down,facility_id,'IC Enter' as ic_type
            from rdb_highway_ic_info
            where enter = 1
            union
            select ic_no, up_down, facility_id, 'IC Exit' as ic_type
            from rdb_highway_ic_info
            where exit = 1
            order by ic_type
        )
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
