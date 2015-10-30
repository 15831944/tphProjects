# -*- coding: UTF8 -*-
'''
Created on 2014-6-24

@author: hongchenzai
'''
from base.rdb_ItemBase import ItemBase


class rdb_hwy_node_add_info(ItemBase):
    '''
    node附加情报类
    '''

    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self, 'Highway_Add_Info')

    def Do(self):
        # 注： 距离四舍五入处理
        self.CreateFunction2('rdb_get_start_end_flag')  # 起终点标志
        self.CreateTable2('rdb_highway_node_add_info')
        self._add_geom_column()
        if self.pg.IsExistTable('highway_node_add_info') != True:
            return 0
        sqlcmd = """
        INSERT INTO rdb_highway_node_add_info(
                    link_id, node_id, start_end_flag,
                    toll_flag, no_toll_money, facility_num,
                    up_down, facility_id, seq_num,
                    etc_antenna, enter, exit,
                    jct, sapa, gate,
                    un_open, dummy, toll_type_num,
                    non_ticket_gate, check_gate, single_gate,
                    cal_gate, ticket_gate, nest,
                    uturn, not_guide, normal_toll,
                    etc_toll, etc_section, "name",
                    tile_id, node_geom, link_geom,
                    dir, dir_s_node, dir_e_node)
        (
        SELECT tile_link_id, c.tile_node_id,
               rdb_get_start_end_flag(a.link_id, node_id),
               toll_flag, no_toll_money, facility_num,
               up_down, facility_id, a.seq_num,
               etc_antenna, enter, exit,
               jct, sapa, gate,
               un_open, dummy, toll_type_num,
               non_ticket_gate, check_gate, single_gate,
               cal_gate, ticket_gate, nest,
               a.uturn, not_guide, normal_toll,
               etc_toll, etc_section, "name",
               a.tile_id, node_geom, link_geom,
               (case
               when dir_s_node = d.s_node and dir_e_node = d.e_node then 0 --正
               when dir_s_node = d.e_node and dir_e_node = d.s_node then 1 --逆
               else NULL end) as dir,
               e.tile_node_id as dir_s_node,
               f.tile_node_id as dir_e_node
          FROM highway_node_add_info as a
          LEFT JOIN rdb_tile_link as b
          ON a.link_id = b.old_link_id
          left join rdb_tile_node as c
          ON a.node_id = c.old_node_id
          LEFT JOIN link_tbl as d
          ON a.link_id = d.link_id
          left join rdb_tile_node as e
          ON a.dir_s_node = e.old_node_id
          left join rdb_tile_node as f
          ON a.dir_e_node = f.old_node_id
          order by a.tile_id, tile_link_id, a.seq_num
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('rdb_highway_node_add_info_link_id_idx')
        self.CreateIndex2('rdb_highway_node_add_info_link_geom_idx')
        self.CreateIndex2('rdb_highway_node_add_info_node_geom_idx')
        # 更新link表的高速附加情报Flag
        self._update_hwy_flag()

    def _add_geom_column(self):
        sqlcmd = """
        SELECT AddGeometryColumn('','rdb_highway_node_add_info',
                                 'node_geom','4326','POINT',2);
        SELECT AddGeometryColumn('','rdb_highway_node_add_info',
                                 'link_geom','4326','LINESTRING',2);
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _update_hwy_flag(self):
        '''更新link表的高速附加情报Flag'''
        sqlcmd = """
        UPDATE rdb_link set hwy_flag = 1
          FROM rdb_highway_node_add_info as hwy
          WHERE rdb_link.link_id = hwy.link_id;

        UPDATE rdb_link_with_all_attri_view set hwy_flag = 1
          FROM rdb_highway_node_add_info as hwy
          WHERE rdb_link_with_all_attri_view.link_id = hwy.link_id;
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
