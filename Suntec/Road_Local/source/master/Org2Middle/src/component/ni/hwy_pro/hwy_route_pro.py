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
        # 连接环，原数据把环拆成两段路, 所有要把它他串成一条路
        self._connect_cycle()
        self._update_hwy_link_geom()
        self._make_road_code()
        if not self._check_overlap_node():
            self.log.error('Exist Overlap node.')
        if not self._check_leak_node():
            self.log.error('Leak highway Node for cycle.')

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
        self.CreateIndex2('hwy_link_road_code_info_link_id_idx')
        self.CreateIndex2('hwy_link_road_code_info_the_geom_idx')

    def _connect_cycle(self):
        # 连接环，原数据把环拆成两段路, 所以要把它他串成一条路
        self.log.info('Start Connect Cycle Path.')
        self.CreateTable2('mid_hwy_del_path_for_cycle')
        cycle_dict = {}
        for (road_code1, updown1, path1,
             road_code2, updown2, path2) in self._get_overlap_path():
            key = (road_code1, updown1)
            if key in cycle_dict:
                val = cycle_dict.get(key)
                if (road_code2, updown2) != (val[0], val[1]):
                    self.log.warning('Multi Cycle. road_code=%s, updown=%s'
                                     % key)
                continue
            key = (road_code2, updown2)
            if (road_code2, updown2) in cycle_dict:
                val = cycle_dict.get(key)
                if (road_code1, updown1) != (val[0], val[1]):
                    self.log.warning('Multi Cycle. road_code=%s, updown=%s'
                                     % key)
                continue
            if path1[0] == path1[-1] or path2[0] == path2[-1]:
                continue
            temp_path = path1 + path2
            if self.G.is_cycle(temp_path):
                # 保存环, 把短的线路 接到 长的线路, 然后删除短线
                if len(path1) >= len(path2):
                    start_seq_nm = len(path1)
                    cycle_dict[(road_code1, updown1)] = (road_code2, updown2,
                                                         start_seq_nm, path2)
                else:
                    start_seq_nm = len(path2)
                    cycle_dict[(road_code2, updown2)] = (road_code1, updown1,
                                                         start_seq_nm, path1)
        for key, val in cycle_dict.iteritems():
            road_code, updown = key
            d_road_code, d_updown, start_seq_nm, path = val
            self._store_cycle(road_code, updown, start_seq_nm, path)
            self._del_path(d_road_code, d_updown)

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

    def _get_overlap_path(self):
        '''取得头尾有相交的线路'''
        sqlcmd = """
        SELECT road_code1, updown1, path1,
               road_code2, updown2, path2
          FROM (
            SELECT road_code as road_code1, updown as updown1,
                   array_agg(node_id) AS path1
              FROM (
                SELECT road_code, updown, node_id
                  FROM hwy_link_road_code_info
                  ORDER BY road_code, updown, seq_nm
              ) AS a
              GROUP BY road_code, updown
          ) AS b
          INNER JOIN (
            SELECT road_code as road_code2, updown as updown2,
                   array_agg(node_id) AS path2
              FROM (
                SELECT road_code, updown, node_id
                  FROM hwy_link_road_code_info
                  ORDER BY road_code, updown, seq_nm
              ) AS a
              GROUP BY road_code, updown
          ) AS c
          ON path1[1] = path2[array_length(path2, 1)] and
             path1[array_length(path1, 1)] = path2[1] and
             road_code1 <> road_code2
          ORDER BY road_code1, updown1, road_code2, updown2
        """
        return self.get_batch_data(sqlcmd)

    def _store_cycle(self, road_code, updown, seq_nm, path):
        linkids = self.G.get_linkids(path)
        path_len = len(path)
        for node_idx in range(1, path_len):
            node_id = path[node_idx]
            link_id = linkids[node_idx - 1]
            self._insert_hwy_link_road_code((road_code, updown,
                                             node_id, link_id, seq_nm))
            seq_nm += 1
        self.pg.commit2()

    def _del_path(self, road_code, updown):
        # ## 先存保要删除的路径(因为接到环路去了)
        sqlcmd = """
        INSERT INTO mid_hwy_del_path_for_cycle(road_code, updown, node_id,
                                               link_id, seq_nm, the_geom)
        (
        SELECT road_code, updown, node_id, link_id, seq_nm, the_geom
          FROM hwy_link_road_code_info
          where road_code = %s and updown = %s
          ORDER BY road_code, updown, seq_nm
        );
        """
        self.pg.execute2(sqlcmd, (road_code, updown))
        self.pg.commit2()
        # ## 再删除
        sqlcmd = """
        DELETE from hwy_link_road_code_info
          where road_code= %s and updown = %s
        """
        self.pg.execute2(sqlcmd, (road_code, updown))
        self.pg.commit2()

    def _check_overlap_node(self):
        '''线路去掉头尾点之后，其他点不能重复。'''
        sqlcmd = """
        SELECT count(*)
        from (
            SELECT node_id
              FROM (
                SELECT a.road_code, a.updown, node_id, a.seq_nm
                FROM hwy_link_road_code_info AS a
                LEFT JOIN (
                SELECT road_code, updown, max(seq_nm) as max_seq_nm
                  FROM hwy_link_road_code_info
                  GROUP BY road_code, updown
                ) as b
                ON a.road_code = b.road_code and a.updown = b.updown
                WHERE a.seq_nm <> 0 and a.seq_nm <> b.max_seq_nm
                ORDER BY a.road_code, a.updown, a.seq_nm
              ) AS c
              GROUP BY node_id
              having count(*) > 1
        ) as d
        """
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchone2()
        if row:
            if not row[0]:
                return True
        return False

    def _check_leak_node(self):
        sqlcmd = """
        select count(a.node_id)
          from mid_hwy_del_path_for_cycle as a
          left join hwy_link_road_code_info AS b
          ON a.node_id = b.node_id
          where b.node_id is null
        """
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchone2()
        if row:
            if not row[0]:
                return True
        return False
