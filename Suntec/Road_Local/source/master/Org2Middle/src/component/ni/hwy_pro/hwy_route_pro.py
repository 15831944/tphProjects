# -*- coding: UTF-8 -*-
'''
Created on 2015��10��26��

@author: hcz
'''
from component.rdf.hwy.hwy_def_rdf import HWY_UPDOWN_TYPE_UP
from component.ni.hwy.hwy_route_ni import HwyRouteNi


class HwyRouteNiPro(HwyRouteNi):
    '''
    classdocs
    '''

    def __init__(self, data_mng, ItemName='HwyRoutePro'):
        HwyRouteNi.__init__(self, data_mng, ItemName=ItemName)

    def _Do(self):
        if not self.data_mng:
            self.log.error('HWY data manager is None.')
            return
        if not self.G:
            if self.G is not None:
                self.log.warning('Graph is empty(No Hwy Link).')  # 没有高速道路
                # 创建一张空表
                self.CreateTable2('hwy_link_road_code_info')
            else:
                self.log.error('Graph is none.')
            return
        self._make_main_path()

    def _make_main_path(self):
        self.log.info('Start Make Highway Main Link Path.')
        for path_id, nodes in self._get_org_main_path():
            path = self.G.get_main_path(nodes, path_id)
            self._store_hwy_link(path_id, path)
        self.pg.commit2()
        self._update_hwy_link_geom()
        self.CreateIndex2('hwy_link_road_code_info_link_id_idx')
        self.CreateIndex2('hwy_link_road_code_info_the_geom_idx')

    def _store_hwy_link(self, path_id, path):
        linkids = self.G.get_linkids(path)
        seq_nm = 0
        path_len = len(path)
        road_code = path_id  # road_code直接使用原始的path id.
        updown = HWY_UPDOWN_TYPE_UP
        for seq_nm in range(0, path_len):
            node_id = path[seq_nm]
            if seq_nm == 0:
                link_id = None
            else:
                link_id = linkids[seq_nm - 1]
            self._insert_hwy_link_road_code((road_code, updown,
                                             node_id, link_id, seq_nm))

    def _get_org_main_path(self):
        '''取得原数据间断的本线路径'''
        sqlcmd = """
        SELECT hw_pid AS path_id, array_agg(nodeid)
          FROM (
            SELECT mapid::int, hw_pid::int,
                   nodeid::int, min(seq_nm::int) as seq_nm
              FROM org_hw_junction
              GROUP BY mapid, hw_pid, nodeid
              order by hw_pid::int, seq_nm
          ) AS a
          GROUP BY hw_pid
          ORDER BY hw_pid;
        """
        return self.get_batch_data(sqlcmd)

    def _insert_hwy_link_road_code(self, params):
        sqlcmd = """
        INSERT INTO hwy_link_road_code_info(
                    road_code, updown, node_id, link_id, seq_nm)
            VALUES (%s, %s, %s, %s, %s);
        """
        self.pg.execute2(sqlcmd, params)

    def _update_hwy_link_geom(self):
        sqlcmd = """
        update hwy_link_road_code_info as a
          set the_geom = link_tbl.the_geom
          from link_tbl
          where a.link_id  = link_tbl.link_id;
        """
        self.pg.execute2(sqlcmd)
