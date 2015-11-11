# -*- coding: UTF-8 -*-
'''
Created on 2015��10��26��

@author: hcz
'''
from component.rdf.hwy.hwy_route_rdf import HwyRouteRDF_HKG
from component.rdf.hwy.hwy_def_rdf import HWY_UPDOWN_TYPE_UP
from component.rdf.hwy.hwy_def_rdf import HWY_ROAD_TYPE_HWY1


class HwyRouteNiPro(HwyRouteRDF_HKG):
    '''
    classdocs
    '''

    def __init__(self, data_mng, ItemName='HwyRouteNiPro'):
        HwyRouteRDF_HKG.__init__(self, data_mng, ItemName=ItemName)

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
        # 把高速本线link串起来
        self._make_main_path()
        self._make_road_code()

    def _make_main_path(self):
        '''把高速本线link串起来'''
        self.log.info('Start Make Highway Main Link Path.')
        self.CreateTable2('hwy_link_road_code_info')
        for path_id, nodes, road_types in self._get_org_main_path():
            temp_nodes = []
            for node in nodes:
                if self.G.has_node(node):
                    temp_nodes.append(node)
            if not temp_nodes or len(temp_nodes) <= 1:
                continue
            if set(road_types) == set([HWY_ROAD_TYPE_HWY1]):
                continue
            path = self.G.get_main_path(temp_nodes, path_id)
            if not path or len(path) <= 1:
                print 'path_id:', path_id
                continue
            self._store_hwy_link(path_id, path)
        self.pg.commit2()
        self._update_hwy_link_geom()
        self.CreateIndex2('hwy_link_road_code_info_link_id_idx')
        self.CreateIndex2('hwy_link_road_code_info_the_geom_idx')

    def _store_hwy_link(self, path_id, path):
        if not path or len(path) <= 1:
            return
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

    def _make_road_code(self):
        '''make road_code_info '''
        self.log.info('start make road code info')
        self.CreateTable2('road_code_info')
        # road_code和path_id一致，即都是使用原数据是线路编号
        sqlcmd = '''
        INSERT INTO road_code_info(road_code, road_name, path_id)
        (
        SELECT path_id as road_code, name, path_id
          FROM mid_temp_hwy_path_name_ni AS a
          INNER JOIN (
            SELECT distinct road_code
              FROM hwy_link_road_code_info
          ) as b
          ON a.path_id = b.road_code
          ORDER BY path_id
        );
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.log.info('end make road code info')
        return 0

    def _get_org_main_path(self):
        '''取得原数据间断的本线路径'''
        sqlcmd = """
        SELECT hw_pid AS path_id, array_agg(nodeid), array_agg(road_type)
          FROM (
            SELECT hw_pid, nodeid,
                   min(seq_nm) as seq_nm, min(road_type) as road_type
            FROM (
                SELECT distinct hw_pid, nodeid, seq_nm,
                       (case
                    when accesstype = 2 then b.road_type
                    when accesstype = 1 then c.road_type
                    when b.road_type is not null then b.road_type
                    else c.road_type end) as road_type
                FROM mid_hwy_org_hw_junction_mid_linkid as a
                LEFT JOIN link_tbl as b
                ON inlinkid = b.link_id and accesstype = 2  -- out
                LEFT JOIN link_tbl as c
                ON outlinkid = c.link_id and accesstype = 1 -- in
            ) AS d
            GROUP BY hw_pid, nodeid
            order by hw_pid, seq_nm
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
        self.pg.commit2()
