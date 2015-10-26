# -*- coding: UTF-8 -*-
'''
Created on 2015-3-23

@author: hcz
'''
from component.rdf.hwy.hwy_graph_rdf import HwyGraphRDF
from component.rdf.hwy.hwy_def_rdf import ANGLE_45
from component.rdf.hwy.hwy_def_rdf import ANGLE_90
from component.rdf.hwy.hwy_def_rdf import ANGLE_270
from component.rdf.hwy.hwy_def_rdf import ANGLE_315
from component.rdf.hwy.hwy_def_rdf import HWY_ROAD_TYPE_HWY
from component.rdf.hwy.hwy_def_rdf import HWY_LINK_TYPE_INNER
from component.rdf.hwy.hwy_graph_rdf import HWY_ROAD_TYPE
from component.rdf.hwy.hwy_graph_rdf import HWY_LINK_TYPE
from component.rdf.hwy.hwy_graph_rdf import HWY_ROAD_CODE
from component.jdb.hwy.hwy_graph import MAX_CUT_OFF
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_PA
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_IC
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_JCT
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_UTURN
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_VIRTUAl_JCT
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_SERVICE_ROAD
from component.rdf.hwy.hwy_def_rdf import HWY_LINK_TYPE_RAMP
from component.rdf.hwy.hwy_def_rdf import HWY_LINK_TYPE_JCT


class HwyGraphTa(HwyGraphRDF):
    '''
    Graph for Tomtom
    '''

    def __init__(self, data=None, **attr):
        '''
        Constructor
        '''
        HwyGraphRDF.__init__(self, data, **attr)

    def dfs_main_path_by_similar(self, u, v):
        """Produce edges in a depth-first-search starting at source.
           (按名称、番号向前搜索本线link)
        """
        visited = [v]
        successors = self.get_most_similar_out_main_link(u, v)
        if not successors:
            yield visited
        stack = [iter(successors)]
        while stack:
            children = stack[-1]
            child = next(children, None)
            if child is None:
                stack.pop()
                visited.pop()
            elif child == u:  # 环
                yield [u] + visited + [u]
                stack.pop()
                visited.pop()
            elif self._is_conn_hwy_both_link(child):  # 到达双向通行高速
                yield visited + [child]
                stack.pop()
                visited.pop()
            elif (self._outlink_is_hov(child) and
                  self._inlink_exist_hov(child)):
                yield visited + [child]
                stack.pop()
                visited.pop()
            elif self._outlink_is_both(child):
                yield visited
                stack.pop()
                visited.pop()
            else:
                successors = self.get_most_similar_out_main_link(visited[-1],
                                                                 child)
                if not successors:
                    yield visited + [child]
                    stack.pop()
                    visited.pop()
                else:
                    if len(visited) == 1:
                        in_edge = (u, v)
                    else:
                        in_edge = (visited[-2], visited[-1])
                    out_edge = (visited[-1], child)
                    # 角度很小上下行本线直接相连
                    angle = self.get_angle(in_edge, out_edge)
                    if not self.bigger_hwy_main_min_angle(angle):
                        yield visited
                        stack.pop()
                        visited.pop()
                        continue
                    visited.append(child)
                    stack.append(iter(successors))

    def dfs_main_path_reverse_by_similar(self, u, v):
        """Produce reverse edges in a depth-first-search starting at source.
           (按名称、番号向后搜索本线link)
        """
        if not self.has_edge(u, v):
            return
        visited = [u]
        predecessors = self.get_most_similar_in_main_link(u, v)
        if not predecessors:
            yield visited
        stack = [iter(predecessors)]
        while stack:
            parents = stack[-1]
            parent = next(parents, None)
            if parent is None:
                stack.pop()
                visited.pop()
            elif parent == v:  # 环
                yield [v] + visited + [v]
                stack.pop()
                visited.pop()
            elif self._is_conn_hwy_both_link(parent):  # 到达双向通行高速
                yield visited + [parent]
                stack.pop()
                visited.pop()
            elif (self._outlink_exist_hov(parent) and
                  self._inlink_is_hov(parent)):
                yield visited + [parent]
                stack.pop()
                visited.pop()
            else:
                predecessors = self.get_most_similar_in_main_link(parent,
                                                                  visited[-1]
                                                                  )
                if not predecessors:
                    yield visited + [parent]
                    stack.pop()
                    visited.pop()
                else:
                    if len(visited) == 1:
                        in_edge = (u, v)
                    else:
                        in_edge = (visited[-1], visited[-2])
                    out_edge = (parent, visited[-1])
                    # 角度很小上下行本线直接相连
                    angle = self.get_angle(in_edge, out_edge, True)
                    if not self.bigger_hwy_main_min_angle(angle):
                        yield visited
                        stack.pop()
                        visited.pop()
                        continue
                    visited.append(parent)
                    stack.append(iter(predecessors))

    def _get_out_main_link_degree(self, node, data):
        out_main_link = []
        for node, child, child_data in self.out_edges_iter(node, True):
            if(not self.is_hwy_main_link(child_data) and
               not self.is_hwy_inner_link(child_data) and
               not self.has_edge(child, node)):  # 双向
                continue
            degree = self._get_similar_degree(data, child_data)
            out_main_link.insert(degree, (child, child_data, degree))
        # 相似度高的排前面
        out_main_link.sort(cmp=lambda x, y: cmp(x[-1], y[-1]), reverse=True)
        return out_main_link

    def _get_in_main_link_degree(self, node, data):
        in_main_link = []
        for parent, node, parent_data in self.in_edges_iter(node, True):
            if(not self.is_hwy_main_link(parent_data) and
               not self.is_hwy_inner_link(parent_data) and
               not self.has_edge(node, parent)):  # 双向:
                continue
            degree = self._get_similar_degree(data, parent_data)
            in_main_link.append((parent, parent_data, degree))
        # 相似度高的排前面
        in_main_link.sort(cmp=lambda x, y: cmp(x[-1], y[-1]), reverse=True)
        return in_main_link

    def is_hwy_inner_link(self, data):
        if(data.get(HWY_ROAD_TYPE) in HWY_ROAD_TYPE_HWY and
           data.get(HWY_LINK_TYPE) == HWY_LINK_TYPE_INNER):
            return True
        else:
            return False

    def _bigger_inout_min_angle(self, angle):
        if angle < ANGLE_45 or angle > ANGLE_315:
            return False
        return True

    def bigger_hwy_main_min_angle(self, angle):
        if angle < ANGLE_90 or angle > ANGLE_270:
            return False
        return True

    def _all_facil_path(self, u, v, road_code,
                        code_field=HWY_ROAD_CODE,
                        reverse=False, cutoff=MAX_CUT_OFF):
        ''''''
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
        # if source in (20360201933504,):
        #     print road_code
        if self.is_hwy_inout(visited, reverse):  # 本线直接和一般道直接相连
            yield visited[1:], HWY_IC_TYPE_IC
        if self.is_jct(visited, road_code, code_field, reverse):  # 本线和本线直接相连
            yield visited[1:], HWY_IC_TYPE_JCT
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
            else:
                cutoff2 = cutoff
            ignore_inner = False
            children = stack[-1]
            child = next(children, None)
            temp_path = visited + [child]
            if child is None:
                if(len(visited) > 1 and
                   self._is_both_dir_sapa(visited[-2], visited[-1],
                                          reverse)):
                    both_sapa_cnt -= 1
                stack.pop()
                visited.pop()
            elif(len(visited) >= 2 and child == visited[-2] and  # 折返
                 not self._is_cuted_road_end(visited[-1])):  # 不是断头路的最后一个点
                # 即 不是断头路的最后一个点, 不能折返
                continue
            elif not self.check_regulation(temp_path, reverse):
                continue
            elif child == visited[1]:
                path = visited + [child]
                if self._is_sapa_path(temp_path, road_code,
                                      code_field, reverse):
                    yield path[1:], HWY_IC_TYPE_PA
                    exist_sapa_facil = True
                continue
            elif len(visited) <= cutoff2:
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
                    if self._is_sapa_path(temp_path, road_code,
                                          code_field, reverse):
                        yield temp_path[1:], HWY_IC_TYPE_PA
                        exist_sapa_facil = True
                elif self.is_same_road_code(temp_path, road_code,  # 回到当前线路
                                            code_field, reverse):
                    if self._is_sapa_path(temp_path, road_code,
                                          code_field, reverse):
                        yield temp_path[1:], HWY_IC_TYPE_PA
                        exist_sapa_facil = True
                    else:
                        # 辅路、类辅路设施
                        yield temp_path[1:], HWY_IC_TYPE_SERVICE_ROAD
                # 和一般道交汇
                if self.is_hwy_inout(temp_path, reverse):
                    yield temp_path[1:], HWY_IC_TYPE_IC
                    out_data = self[out_edge[0]][out_edge[1]]
                    ignore_inner = True
                    # 一般道Inner Link
                    if self.is_normal_inner_link(out_data):
                        continue
                if len(visited) < cutoff2:
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
                                            code_field, reverse, cutoff/2):
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
                                 'u=%s,v=%s' % (u, v))

    def exist_ic_link(self, path, reverse=False):
        '''Ramp/JCT/SAPA Link'''
        if len(path) < 2:
            return False
        if reverse:
            path = path[::-1]
        for u, v in zip(path[:-1], path[1:]):
            if self.has_edge(v, u):  # 双向
                continue
            data = self[u][v]
            link_type = data.get(HWY_LINK_TYPE)
            if link_type in (HWY_LINK_TYPE_RAMP,
                             HWY_LINK_TYPE_JCT,
                             # HWY_LINK_TYPE_SAPA,
                             ):
                return True
        return False