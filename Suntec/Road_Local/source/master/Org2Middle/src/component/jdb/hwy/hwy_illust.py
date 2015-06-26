# -*- coding: UTF8 -*-
'''
Created on 2014-5-14

@author: hongchenzai
'''
import component.component_base


class HwyIllust(component.component_base.comp_base):
    '''
    High Illust(for SAPA).
    '''
    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'HighwayIllust')

    def _Do(self):
        # 插图
        self._make_illust()
        # 更新IC情报里的Illust count
        self._update_illust_count()
        return 0

    def _make_illust(self):
        self.CreateTable2('highway_illust_info')
        sqlcmd = """
        INSERT INTO highway_illust_info(ic_no, image_id)
        (
        SELECT ic_no, illust_name
          FROM road_sapa_illust
          LEFT JOIN middle_tile_node
          on road_sapa_illust.node_id = old_node_id
          LEFT JOIN mid_hwy_ic_no as ic
          on tile_node_id = ic.node_id
          WHERE facilclass_c in (1, 2)
          order by ic_no, illust_name
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return 0

    def _update_illust_count(self):
        '''更新IC情报里的Illust count'''
        sqlcmd = """
        UPDATE highway_ic_info
           SET illust_count = illust.illust_count
           from (
                SELECT ic_no, count(*) as illust_count
                  FROM highway_illust_info
                  GROUP BY ic_no
          ) as illust
         WHERE highway_ic_info.ic_no = illust.ic_no;
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
