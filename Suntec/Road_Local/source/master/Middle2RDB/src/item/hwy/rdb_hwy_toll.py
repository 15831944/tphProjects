# -*- coding: UTF8 -*-
'''
Created on 2013-12-27

@author: hongchenzai
'''

from base.rdb_ItemBase import ItemBase
from common import rdb_log
UP_DOWN_COMMON = 3  # 上下行共用


class rdb_hwy_toll(ItemBase):
    '''店铺信息
    '''

    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self, 'Highway_Toll')

    def Do(self):
        self._make_toll_info()
        self._update_ic_toll_count()
        self._update_jct_toll_count()

    def _make_toll_info(self):
        self.CreateTable2('rdb_highway_toll_info')
        if self.pg.IsExistTable('highway_toll_info') != True:
            return 0
        tile_index = 0  # tile内的序号
        prev_tile = None
        for info in self._get_toll_info():
            ic_no, conn_ic_no = info[0:2]
            toll_class, class_name = info[2:4]
            tile_id = info[-1]
            if prev_tile != tile_id:
                tile_index = 0
                prev_tile = tile_id
            if ic_no == 4287:
                pass
            core_info_list = zip(*info[4:-1])  # up_down ~~ toll_no
            # 按toll_no排序
            core_info_list = sorted(core_info_list,
                                    cmp=lambda x, y: cmp(x[-1], y[-1]))
            temp_info_list = [core_info_list[0]]
            for core_info in core_info_list[1:]:
                add_flag = True
                for info_idx in range(0, len(temp_info_list)):
                    temp_info = temp_info_list[info_idx]
                    # 料金情报相同(Updown和toll_no，不参加比较)
                    if core_info[1:-1] == temp_info[1:-1]:
                        temp_updown = temp_info[0]
                        curr_updwon = core_info[0]
                        if temp_updown == curr_updwon:
                            add_flag = False
                            break
                        elif temp_updown == UP_DOWN_COMMON:
                            # 替换
                            temp_info_list[info_idx] = core_info
                            add_flag = False
                            break
                        elif curr_updwon == UP_DOWN_COMMON:
                            add_flag = False
                            break
                        else:  # temp_updown != curr_updwon，且不等上下共用:
                            pass
                if add_flag:
                    temp_info_list.append(core_info)
            if len(temp_info_list) > 1:
                facil_name = temp_info_list[0][-2]
                rdb_log.log(self.ItemName,
                            'Toll Number > 1. name=' + facil_name, 'info')
            toll_info_list = []
            for temp_core_info in temp_info_list:
                toll_info = [ic_no, conn_ic_no, toll_class, class_name]
                toll_info += list(temp_core_info)
                toll_info += [tile_id, tile_index]
                tile_index += 1
                toll_info_list.append(toll_info)
            self.__insert_toll_info(toll_info_list)
        self.pg.commit2()
        return 0

    def _update_ic_toll_count(self):
        '''更新IC情报的toll_count'''
        sqlcmd = """
        update rdb_highway_ic_info
          SET toll_count = toll.toll_count, toll_index = toll.toll_index
          FROM (
                SELECT ic_no, count(ic_no) AS toll_count,
                       min(tile_index) AS toll_index
                  FROM rdb_highway_toll_info
                  WHERE conn_ic_no is null
                  group by ic_no
          ) AS toll
          where rdb_highway_ic_info.ic_no = toll.ic_no
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _update_jct_toll_count(self):
        '''更新JCT情报的toll_count (注：料金只做在正向JCT上，即conn_direction = 0)'''
        sqlcmd = """
        update rdb_highway_conn_info
          SET toll_count = toll.toll_count,
              toll_index = toll.toll_index
          FROM (
                SELECT ic_no, conn_ic_no,
                       count(ic_no) AS toll_count,
                       min(tile_index) AS toll_index
                  FROM rdb_highway_toll_info
                  WHERE conn_ic_no is not null
                  group by ic_no, conn_ic_no
          ) AS toll
          where rdb_highway_conn_info.ic_no = toll.ic_no
                and rdb_highway_conn_info.conn_ic_no = toll.conn_ic_no
                and conn_direction = 0;
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _get_toll_info(self):
        sqlcmd = """
        SELECT ic_no, conn_ic_no,
               toll_class, class_name,
               array_agg(up_down), array_agg(facility_id),
               array_agg(tollgate_count), array_agg(etc_antenna),
               array_agg(enter), array_agg(exit),
               array_agg(jct), array_agg(sa_pa),
               array_agg(gate), array_agg(unopen),
               array_agg(dummy), array_agg(non_ticket_gate),
               array_agg(check_gate), array_agg(single_gate),
               array_agg(cal_gate), array_agg(ticket_gate),
               array_agg(nest), array_agg(uturn),
               array_agg(not_guide), array_agg(normal_toll),
               array_agg(etc_toll), array_agg(etc_section),
               array_agg(name), array_agg(toll_no),
               tile_id
          FROM (
                SELECT toll.ic_no, conn_ic_no, toll_class, class_name,
                       toll.up_down, toll.facility_id,
                       tollgate_count, etc_antenna,
                       toll.enter, toll.exit, toll.jct, toll.sa_pa,
                       gate, toll.unopen, toll.dummy, non_ticket_gate,
                       check_gate, single_gate, cal_gate, ticket_gate,
                       nest, uturn, not_guide, normal_toll,
                       etc_toll, etc_section, toll.name, toll_no,
                       tile_id
                  FROM highway_toll_info as toll
                  LEFT JOIN highway_ic_info as ic
                  ON toll.ic_no = ic.ic_no
                  ORDER BY tile_id, toll.ic_no, toll.toll_class,
                           conn_ic_no, toll_no
          ) as a
          group by tile_id, ic_no, toll_class, class_name, conn_ic_no
          order by tile_id, ic_no, toll_class, class_name, conn_ic_no
        """
        return self.pg.get_batch_data2(sqlcmd)

    def __insert_toll_info(self, toll_info_list):
        sqlcmd = """
        INSERT INTO rdb_highway_toll_info(
                    ic_no, conn_ic_no, toll_class,
                    class_name, up_down, facility_id,
                    tollgate_count, etc_antenna, enter,
                    exit, jct, sa_pa,
                    gate, unopen, dummy,
                    non_ticket_gate, check_gate, single_gate,
                    cal_gate, ticket_gate, nest,
                    uturn, not_guide, normal_toll,
                    etc_toll, etc_section, "name",
                    old_toll_no, tile_id, tile_index)
            VALUES (%s, %s, %s, %s, %s, %s,
                    %s, %s, %s, %s, %s, %s,
                    %s, %s, %s, %s, %s, %s,
                    %s, %s, %s, %s, %s, %s,
                    %s, %s, %s, %s, %s, %s);
        """
        for toll_info in toll_info_list:
            self.pg.execute2(sqlcmd, toll_info)
