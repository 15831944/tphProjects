# -*- coding: UTF-8 -*-
'''
Created on 2014-3-26

@author: hongchenzai
'''
import base
from component.jdb.hwy.hwy_def import INOUT_TYPE_IN  # 入口・合流
from component.jdb.hwy.hwy_data_mng import  HwyDataMng


class HwyMainPath(base.component_base.comp_base):
    '''Path of main link
    '''

    def __init__(self):
        '''
        Constructor
        '''
        base.component_base.comp_base.__init__(self, 'Highway_MainPath')

    def _Do(self):
        self.CreateTable2('mid_hwy_main_link_path')
        data = self.__get_data()
        hwy_data = HwyDataMng.instance()
        G = hwy_data.get_graph()
        self.pg.connect1()
        import networkx as nx
        for hwy_info in data:
            road_code = hwy_info[0]
            updown = hwy_info[1]
            nodes = hwy_info[2]
            linecls = hwy_info[3]
            max_roadpoint = hwy_info[4][-1]
            # 通过当前road_code任取一点，求整条Highway路径
            path = []
            for node in nodes:
                try:
                    path = G.dfs_path(node, road_code)
                except nx.NetworkXError:
                    continue
                if path:
                    break
            for node in nodes:
                if node not in G:
                    continue
                if node not in path:
                    self.log.warning('Node=%s not in path.'
                                     'roadcode=%s,updown=%s'
                                     % (node, road_code, updown)
                                     )
            cycle_flag = self._is_cycle(path)  # 环线
            if cycle_flag:
                if not self.adjust_path_for_cycle(road_code, updown, path):
                    self.log.error('Move start Facility to first Failed.'
                                   'roadcode=%s, updown=%s'
                                   % (road_code, updown))
            for node_cnt in xrange(0, len(path)):
                if node_cnt > 0:
                    linkid = G.get_linkid(path[node_cnt - 1], path[node_cnt])
                else:
                    linkid = None
                self.__insert_path((road_code, updown, path[node_cnt],
                                    linkid, node_cnt, int(cycle_flag),
                                    linecls))
            self.pg.commit1()
        self.CreateIndex2('mid_hwy_main_link_path_link_id_idx')
        self.CreateIndex2('mid_hwy_main_link_path_node_id_idx')
        return 0

    def _is_cycle(self, path):
        '''环线'''
        if path:
            return path[0] == path[-1]
        return False

    def adjust_path_for_cycle(self, road_code, updown, path):
        '''调整环状的开始位置。'''
        if not self._is_cycle(path):
            return path
        path.pop()
        result = self._move_start_facil_to_first(road_code, updown, path)
        path.append(path[0])
        return result

    def _move_start_facil_to_first(self, roadcode, updown, path):
        hwy_data = HwyDataMng.instance()
        facil_infos = hwy_data.get_first_facil(roadcode, updown)
        index_list = []
        for facil in facil_infos:
            if facil.inout == INOUT_TYPE_IN:
                if facil.node_id in path:
                    index_list.append(path.index(facil.node_id))
        if index_list:
            min_idx = min(index_list)
            max_idx = max(index_list)
            if max_idx - min_idx <= min_idx + len(path) - max_idx - 1:
                i = 0
                while i < min_idx:
                    path.append(path.pop(0))
                    i += 1
            else:
                i = 0
                num = len(path) - max_idx
                while i < num:
                    path.insert(0, path.pop())
                    i += 1
            return True
        return False

    def __get_data(self):
        sqlcmd = '''
        SELECT a.road_code, updown_flag, nodes,
               lineclass_c, road_seq
          FROM (
                SELECT road_code, updown_flag,
                       array_agg(node_id) as nodes,
                       array_agg(road_seq) road_seq
                  from (
                        SELECT node_id, road_code, road_seq,
                               facilclass_c, direction_c as updown_flag,inout_c
                          FROM mid_road_hwynode_middle_nodeid
                          WHERE (tollclass_c = 0 and tollfunc_c = 0)
                          and direction_c = 1
                          ORDER BY road_code, road_seq
                  ) as up
                  GROUP BY road_code, updown_flag

                UNION

                SELECT road_code, updown_flag,
                       array_agg(node_id) as nodes,
                       array_agg(road_seq) road_seq
                  from (
                        SELECT node_id, road_code, road_seq,
                               facilclass_c, direction_c as updown_flag,inout_c
                          FROM mid_road_hwynode_middle_nodeid
                          WHERE (tollclass_c = 0 and tollfunc_c = 0)
                                and direction_c = 2
                          ORDER BY road_code, road_seq
                   ) as down
                   GROUP BY road_code, updown_flag
            ) AS a
            LEFT JOIN road_code_list as b
            ON a.road_code = b.road_code
            order by lineclass_c, road_code, updown_flag;
        '''
        return self.pg.get_batch_data2(sqlcmd)

    def __insert_path(self, parames):
        sqlcmd = '''
        INSERT INTO mid_hwy_main_link_path(
                    road_code, updown_flag, node_id,
                    link_id, seqnum, cycle_flag,
                    lineclass_c)
            VALUES (%s, %s, %s,
                    %s, %s, %s,
                    %s);
        '''
        self.pg.execute1(sqlcmd, parames)
