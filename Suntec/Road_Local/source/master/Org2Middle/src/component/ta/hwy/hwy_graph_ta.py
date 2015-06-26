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
        if angle < ANGLE_90 or angle > ANGLE_270:
            return False
        return True

    def bigger_hwy_main_min_angle(self, angle):
        if angle < ANGLE_90 or angle > ANGLE_270:
            return False
        return True
