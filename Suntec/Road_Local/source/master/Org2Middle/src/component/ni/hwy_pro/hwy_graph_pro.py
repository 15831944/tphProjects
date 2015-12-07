# -*- coding: UTF-8 -*-
'''
Created on 2015-10-23

@author: hcz
'''
from component.rdf.hwy.hwy_graph_rdf import HWY_PATH_ID
import networkx as nx
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_PA
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_IC
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_JCT
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_URBAN_JCT
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_UTURN
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_VIRTUAl_JCT
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_SERVICE_ROAD
from component.rdf.hwy.hwy_graph_rdf import HWY_ROAD_CODE
from component.jdb.hwy.hwy_graph import MAX_CUT_OFF_CHN
from component.jdb.hwy.hwy_graph import MAX_CUT_OFF
from component.jdb.hwy.hwy_graph import MIN_CUT_OFF_CHN
from component.rdf.hwy.hwy_def_rdf import ANGLE_35
from component.rdf.hwy.hwy_def_rdf import ANGLE_360
from component.rdf.hwy.hwy_graph_rdf import HWY_ROAD_TYPE
from component.rdf.hwy.hwy_graph_rdf import HWY_LINK_TYPE
from component.rdf.hwy.hwy_def_rdf import HWY_ROAD_TYPE_HWY
from component.rdf.hwy.hwy_def_rdf import HWY_ROAD_TYPE_HWY0
from component.rdf.hwy.hwy_def_rdf import HWY_ROAD_TYPE_HWY1
from component.rdf.hwy.hwy_def_rdf import HWY_LINK_TYPE_MAIN
from component.rdf.hwy.hwy_def_rdf import HWY_LINK_TYPE_SAPA
from component.rdf.hwy.hwy_graph_rdf import HwyGraphRDF
HWY_FIRST_ICS_LINK = 'first_ics_link'  # 设施入/出第一条link


class HwyGraphNiPro(HwyGraphRDF):
    '''
    classdocs
    '''

    def __init__(self, data=None, **attr):
        '''
        Constructor
        '''
        HwyGraphRDF.__init__(self, data, **attr)

    def get_main_path(self, nodes, path_id, code_field=HWY_PATH_ID):
        '''通过间断的点求出整条本线。'''
        path = nodes[0:1]
        for u, v in zip(nodes[:-1], nodes[1:]):
            if u == v:
                continue
            # u_out与v_in的路径
            pathes = list(self._all_main_pathes(u, v, path_id,
                                                code_field))
            if pathes:
                temp_pathes = []
                if len(pathes) > 1:
                    for temp_path in pathes:
                        u, v = temp_path[0], temp_path[1]
                        data = self[u][v]
                        temp_path_id = data.get(code_field)
                        if path_id == temp_path_id:
                            temp_pathes.append(temp_path)
                else:
                    temp_pathes = pathes
                if not temp_pathes:
                    self.log.error('No Path. path_id=%s u=%s, v=%s'
                                   % (path_id, u, v))
                    return []
                elif len(temp_pathes) == 1:
                    path += temp_pathes[0][1:]
                else:
                    self.log.error('Path > 1. path_id=%s, u=%s, v=%s'
                                   % (path_id, u, v))
            else:
                self.log.error('No Path. path_id=%s u=%s, v=%s'
                               % (path_id, u, v))
                return []
                raise nx.NetworkXError('No Path. path_id=%s, u=%s, v=%s'
                                       % (path_id, u, v))
        return path

    def _all_main_pathes(self, source, target,
                         path_id, code_field=HWY_PATH_ID):
        visited = [source]
        stack = [iter(self[source])]
        while stack:
            children = stack[-1]
            child = next(children, None)
            if not child:
                visited.pop()
                stack.pop()
                continue
            if child in visited[1:]:
                continue
            if self.has_edge(child, visited[-1]):  # 双向
                continue
            # Path Id不同
            data = self[visited[-1]][child]
            temp_path_id = data.get(code_field)
            if temp_path_id:
                if temp_path_id != path_id:
                    continue
            else:
                if self._is_first_ics_link(visited[-1], child):
                    continue
            if child == target:
                yield visited + [child]
            else:
                visited.append(child)
                stack.append(iter(self[child]))

    def _is_first_ics_link(self, u, v, path_id=None):
        # print u, v
        data = self[u][v]
        if data.get(HWY_FIRST_ICS_LINK):
            return True
        return False

    def is_urban_jct(self, path, reverse):
        '''高速和城市高速的交汇点。
           reverse: False,顺车流；True,逆车流
        '''
        if reverse:  # 逆
            edges_iter = self.in_edges_iter(path[-1], True)
        else:  # 顺
            edges_iter = self.out_edges_iter(path[-1], True)
        for temp_u, temp_v, data in edges_iter:
            if reverse:  # 逆
                temp_path = path + [temp_u]
            else:  # 顺
                temp_path = path + [temp_v]
            # 规制
            if not self.check_regulation(temp_path, reverse):
                continue
            road_type = data.get(HWY_ROAD_TYPE)
#             link_type = data.get(HWY_LINK_TYPE)
            path_id = data.get(HWY_PATH_ID)
            # 城市高速且为本线
            if(road_type == HWY_ROAD_TYPE_HWY1 and path_id):
                return True
        return False

    def is_hwy_inout(self, path, reverse=False):
        '''高速(Ramp)和一般道、高速双向路的交汇点。
           reverse: False,顺车流；True,逆车流
        '''
        if reverse:  # 逆
            edges_iter = self.in_edges_iter(path[-1], True)
            # in_edge = (path[-1], path[-2])
        else:  # 顺
            edges_iter = self.out_edges_iter(path[-1], True)
            # in_edge = (path[-2], path[-1])
        for temp_u, temp_v, data in edges_iter:
            # out_edge = temp_u, temp_v
            # 夹角小于90度
            # angle = self.get_angle(in_edge, out_edge, reverse)
            # if not self._bigger_inout_min_angle(angle):
            #    continue
            if reverse:  # 逆
                temp_path = path + [temp_u]
            else:  # 顺
                temp_path = path + [temp_v]
            # 规制
            if not self.check_regulation(temp_path, reverse):
                continue
            road_type = data.get(HWY_ROAD_TYPE)
            link_type = data.get(HWY_LINK_TYPE)
            # 非高速，非SAPA
            if(road_type not in (HWY_ROAD_TYPE_HWY0, HWY_ROAD_TYPE_HWY1) and
               link_type != HWY_LINK_TYPE_SAPA):
                return True
            # SAPA内部link_type=1,one_way=1的link, 所以不判断高速本线双向
            # else:  # HWY
            #    if self.has_edge(temp_v, temp_u):  # 双向道路
            #        # Main Link
            #        if link_type in (HWY_LINK_TYPE_MAIN1,
            #                         HWY_LINK_TYPE_MAIN2):
            #            return True
        return False

    def _get_not_main_link(self, node, code_field,
                           reverse=False, ignore_inner=False):
        '''非本线高速link'''
        nodes = []
        if reverse:  # 逆
            edges_iter = self.in_edges_iter(node, True)
        else:  # 顺
            edges_iter = self.out_edges_iter(node, True)
        for u, v, data in edges_iter:
            if data.get(code_field):  # 本线
                continue
            road_type = data.get(HWY_ROAD_TYPE)
            link_type = data.get(HWY_LINK_TYPE)
            if (data.get(HWY_PATH_ID) and
                road_type == HWY_ROAD_TYPE_HWY1):  # 城市高速本线
                continue
            if(road_type in (HWY_ROAD_TYPE_HWY0, HWY_ROAD_TYPE_HWY1)or  # 高速
               link_type == HWY_LINK_TYPE_SAPA):  # SAPA Link
                if reverse:  # 逆
                    nodes.append(u)
                else:  # 顺
                    nodes.append(v)
        return nodes

    def _is_both_dir_sapa(self, u, v, reverse=False):
        if reverse:  # 逆
            if self.has_edge(u, v):
                return True
        else:  # 顺
            if self.has_edge(v, u):
                return True
        return False

    def _all_facil_path(self, u, v, road_code,
                        code_field=HWY_ROAD_CODE,
                        reverse=False, cutoff=MAX_CUT_OFF_CHN):
        MAX_COUNT = 2
        MAX_TOLLGATE_NUM = 2
        sapa_link = False
        exist_sapa_facil = False
        both_sapa_cnt = 0
        if cutoff < 1:
            return
        if reverse:  # 逆
            source = u
            visited = [v, u]
        else:  # 顺
            source = v
            visited = [u, v]
        if source not in self:
            return
        if self.is_hwy_inout(visited, reverse):  # 本线直接和一般道直接相连
            yield visited[1:], HWY_IC_TYPE_IC
        if self.is_jct(visited, road_code, code_field, reverse):  # 本线和本线直接相连
            yield visited[1:], HWY_IC_TYPE_JCT
        if self.is_urban_jct(visited, reverse):
            yield visited[1:], HWY_IC_TYPE_URBAN_JCT
        # visited = [source]
        # stack = [iter(self[source])]
        nodes = self._get_not_main_link(visited[-1], code_field, reverse)
        if not nodes:  # 没有分歧/合流
            return
        stack = [iter(nodes)]
        while stack:
            if both_sapa_cnt > 0:
                # 双向的SAPA link，一条当两条权值算
                cutoff2 = cutoff - both_sapa_cnt
                if cutoff2 < MIN_CUT_OFF_CHN:
                    cutoff2 = MIN_CUT_OFF_CHN
            else:
                cutoff2 = cutoff
            curr_inout_flag = False
            ignore_inner = False
            children = stack[-1]
            child = next(children, None)
            temp_path = visited + [child]
            node_cnt = temp_path.count(child)
            if child is None:
                if(len(visited) > 1 and
                   self._is_both_dir_sapa(visited[-2], visited[-1],
                                          reverse)):
                    both_sapa_cnt -= 1
                stack.pop()
                visited.pop()
            elif len(visited) <= cutoff2:
                if(len(visited) >= 2 and child == visited[-2] and  # 折返
                   not self._is_cuted_road_end(visited[-1])):  # 不是断头路的最后一个点
                    # 即不是断头路的最后一个点, 不能折返
                    continue
                elif node_cnt > MAX_COUNT:
                    continue
                elif(node_cnt > 1 and
                     self._is_link_repeat(temp_path[:-2], u, v) and
                     set._has_other_path(u, v, reverse)):
                    continue
                elif not self.check_regulation(temp_path, reverse):
                    continue
                elif self.get_tollgate_num(temp_path) > MAX_TOLLGATE_NUM:
                    continue
                if child == visited[1]:
                    if self.is_sapa_path(temp_path, road_code,
                                         code_field, reverse):
                        yield temp_path[1:], HWY_IC_TYPE_PA
                        exist_sapa_facil = True
                    continue
                if len(visited) < cutoff2:
                    # 取得link
                    if reverse:  # 逆
                        out_edge = (child, visited[-1])
                    else:  # 顺
                        out_edge = (visited[-1], child)
                    # 本线和SAPA link直接相连
                    if(len(visited) == 2 and
                       self.is_sapa_link(out_edge[0], out_edge[1])):
                        # not self.has_edge(out_edge[1], out_edge[0])):
                        sapa_link = True
                    if self.is_jct(temp_path, road_code, code_field, reverse):
                        for uturn_info in self.get_uturns(temp_path, road_code,
                                                          code_field, reverse):
                            uturn_flg = uturn_info[-1]
                            if uturn_flg:
                                yield temp_path[1:], HWY_IC_TYPE_UTURN
                            else:
                                yield temp_path[1:], HWY_IC_TYPE_JCT
                            if self.is_sapa_path(temp_path, road_code,
                                                  code_field, reverse):
                                yield temp_path[1:], HWY_IC_TYPE_PA
                                exist_sapa_facil = True
                    elif self.is_same_road_code(temp_path, road_code,  # 回到当前线路
                                                code_field, reverse):
                        if self.is_sapa_path(temp_path, road_code,
                                             code_field, reverse):
                            yield temp_path[1:], HWY_IC_TYPE_PA
                            exist_sapa_facil = True
                        else:
                            # print temp_path[1:]
                            # 辅路、类辅路设施
                            yield temp_path[1:], HWY_IC_TYPE_SERVICE_ROAD
                            continue
                    # 与城市高速交汇
                    if self.is_urban_jct(temp_path, reverse):
                        yield temp_path[1:], HWY_IC_TYPE_URBAN_JCT
                    # 和一般道交汇
                    if self.is_hwy_inout(temp_path, reverse):
                        yield temp_path[1:], HWY_IC_TYPE_IC
                    curr_inout_flag = True

                    if self.is_virtual_jct(child, road_code,
                                           code_field, reverse):
                        yield temp_path[1:], HWY_IC_TYPE_VIRTUAl_JCT
                    # 取得非本线
                    nodes = self._get_not_main_link(child, code_field,
                                                    reverse, ignore_inner)
                    if nodes:
                        visited.append(child)
                        stack.append(iter(nodes))
                        if self._is_both_dir_sapa(visited[-2], visited[-1],
                                                  reverse):
                            both_sapa_cnt += 1
                    else:
                        if not curr_inout_flag:  # 当前点已经是出入口，不再判断
                            # 和一般道相连
                            if reverse:  # 逆
                                u = temp_path[-1]
                                v = temp_path[-2]
                            else:  # 顺
                                u = temp_path[-2]
                                v = temp_path[-1]
                            if self.get_normal_link(u, v, reverse):
                                yield temp_path[1:], HWY_IC_TYPE_IC
                elif len(visited) == cutoff2:
                    if(len(visited) > 1 and
                       self._is_both_dir_sapa(visited[-2], visited[-1],
                                              reverse)):
                        both_sapa_cnt -= 1
                    stack.pop()
                    visited.pop()
        # 存在SAPA link，但是没有生成SAPA设施
        if sapa_link and not exist_sapa_facil:
            for path in self._all_sapa_path(u, v, road_code,
                                            code_field, reverse,
                                            cutoff=MAX_CUT_OFF / 2):
                # 保证另头是SAPA link
                if reverse:
                    u, v = path[-1], path[-2]
                else:
                    u, v = path[-2], path[-1]
                if self.is_sapa_link(u, v):
                    exist_sapa_facil = True
                    yield path, HWY_IC_TYPE_PA
            if not exist_sapa_facil:
                self.log.warning('Exist SAPA Link, but no SAPA Facility.'
                                 ' u=%s,v=%s' % (u, v))

    def _is_cuted_road_end(self, node):
        '''断头路的最后一个点'''
        in_deges = self.in_edges(node, False)
        out_edges = self.out_edges(node, False)
        if len(in_deges) + len(out_edges) <= 1:
            return True
        if len(in_deges) == 1 and len(out_edges) == 1:
            in_node = in_deges[0][0]
            out_node = out_edges[0][1]
            if in_node == out_node:
                return True
        return False

    def get_normal_link(self, u, v, reverse=False):
        '''高速(Ramp)相连的一般道、。
           reverse: False,顺车流；True,逆车流
        '''
        nodes = []
        if reverse:  # 逆
            edges_iter = self.in_edges_iter(u, True)
        else:  # 顺
            edges_iter = self.out_edges_iter(v, True)
        for temp_u, temp_v, data in edges_iter:
            road_type = data.get(HWY_ROAD_TYPE)
            if road_type not in (HWY_ROAD_TYPE_HWY0, HWY_ROAD_TYPE_HWY1):
                if reverse:  # 逆
                    nodes.append(temp_u)
                else:  # 顺
                    nodes.append(temp_v)
        return nodes

    def _is_link_repeat(self, path, u, v):
        '''双向link,重复往返探索'''
        # 双向
        if not self.has_edge(u, v) or not self.has_edge(v, u):
            return False
        # 正逆都探过一次
        for edge in zip(path[0:-1], path[1:]):
            if edge == (v, u):
                return True

    def _has_other_path(self, u, v, reverse=False):
        '''同一条边的两点存在其他路径'''
        orther_path_flg = False
        if reverse:
            u, v = v, u
        if not self.has_edge(u, v):
            return orther_path_flg
        data = self[u][v]
        self.remove_edge(u, v)
        if nx.has_path(self, u, v):
            orther_path_flg = True
        self.add_edge(u, v, data)
        return orther_path_flg

    def is_uturn_angle(self, angle):
        # 小于30度/大于330度
        if angle < ANGLE_35 or angle > ANGLE_360 - ANGLE_35:
            return True
        return False
