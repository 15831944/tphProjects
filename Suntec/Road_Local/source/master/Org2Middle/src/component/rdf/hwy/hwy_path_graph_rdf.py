# -*- coding: UTF-8 -*-
'''
Created on 2015年1月29日

@author: hcz
'''
from component.rdf.hwy.hwy_graph_rdf import HwyGraphRDF
from component.rdf.hwy.hwy_def_rdf import HWY_PATH_CONN_TYPE_NONE
from component.rdf.hwy.hwy_def_rdf import HWY_PATH_CONN_TYPE_S
from component.rdf.hwy.hwy_def_rdf import HWY_PATH_CONN_TYPE_E
from component.rdf.hwy.hwy_def_rdf import HWY_PATH_CONN_TYPE_SE


# ==============================================================================
# HwyPathGraphRDF--路径的图
# ==============================================================================
class HwyPathGraphRDF(HwyGraphRDF):
    '''
    Highway Path Graph
    '''

    def __init__(self, hwy_graph, data=None, **attr):
        HwyGraphRDF.__init__(self, data, **attr)
        self.G = hwy_graph  # 高速路网

    def add_path(self, path_id, length, node_list):
        u = node_list[0]
        v = node_list[-1]
        self.add_edge(u, v, {'path_id': path_id, 'length': length,
                             'node_list': node_list}
                      )

    def get_path_id(self, u, v):
        return self[u][v].get('path_id')

    def get_node_list(self, u, v):
        return self[u][v].get('node_list')

    def merge_edges(self, path, new_path_id):
        new_node_list = []
        length = 0.0
        for u, v in zip(path[0:-1], path[1:]):
            old_node_list = self.get_node_list(u, v)
            length += self[u][v].get('length')
            if not new_node_list:
                new_node_list = old_node_list
            else:
                new_node_list += old_node_list[1:]
            # delete old edge(path)
            self.remove_edge(u, v)
        self.add_path(new_path_id, length, new_node_list)
        u = new_node_list[0]
        v = new_node_list[-1]
        return u, v

    def get_angle(self, in_edge, out_edge, reverse=False):
        in_node_list = self.get_node_list(in_edge[0], in_edge[1])
        out_node_list = self.get_node_list(out_edge[0], out_edge[1])
        in_link = (in_node_list[-2], in_node_list[-1])
        out_link = (out_node_list[0], out_node_list[1])
        # 得取link的夹角
        angle = self.G.get_angle(in_link, out_link, reverse)
        return angle

    def merge_path_by_similir(self, u, v):
        '''(去掉侧道之后)通过相度再连接'''
        path = [u, v]
        path = self.get_most_similar_in_path(u, v) + path
        path += self.get_most_similar_out_path(u, v)
        return path

    def get_most_similar_in_path(self, u, v):
        node_list = self.get_node_list(u, v)
        start_v = node_list[1]
        predecessors = self.G.get_most_similar_in_main_link(u, start_v)
        if predecessors and len(predecessors) == 1:
            for p, u, data in self.in_edges_iter(u, True):
                # in_node_list = self.get_node_list(p, u)
                in_node_list = data.get('node_list')
                if predecessors[0] == in_node_list[-2]:
                    return [p]
        return []

    def get_most_similar_out_path(self, u, v):
        node_list = self.get_node_list(u, v)
        last_u = node_list[-2]
        successors = self.G.get_most_similar_out_main_link(last_u, v)
        if successors and len(successors) == 1:
            for v, child, data in self.out_edges_iter(v, True):
                # in_node_list = self.get_node_list(p, u)
                out_node_list = data.get('node_list')
                if successors[0] == out_node_list[1]:
                    return [child]
        return []

    def is_jct_path(self, edge):
        '''该路径两头都相接其他路径'''
        u, v = edge
        in_pathes = self.in_edges(u)
        out_pathes = self.out_degree(v)
        if in_pathes and out_pathes:
            return True
        return False

    def is_ic_path(self, edge):
        '''该路径只有一头相接其他路径'''
        return False

    def get_path_conn_type(self, edge):
        '''路径相接种别'''
        u, v = edge
        curr_node_list = self.get_node_list(u, v)
        second_node = curr_node_list[1]
        in_pathes, out_pathes = [], []
        for in_u, in_v in self.in_edges_iter(u, False):
            in_node_list = self.get_node_list(in_u, in_v)
            temp_path = in_node_list + [second_node]
            if self.G.check_regulation(temp_path):
                in_pathes.append((in_u, in_v))
            else:
                # print 'regulation: ', u
                pass
        for out_u, out_v in self.out_edges_iter(v, False):
            out_node_list = self.get_node_list(out_u, out_v)
            second_node = out_node_list[1]
            temp_path = curr_node_list + [second_node]
            if self.G.check_regulation(temp_path):
                out_pathes.append((out_u, out_v))
            else:
                # print 'regulation: ', v
                pass
        if in_pathes and out_pathes:  # 两头相接
            return HWY_PATH_CONN_TYPE_SE
        if in_pathes and not out_pathes:  # 起点相接
            return HWY_PATH_CONN_TYPE_S
        if not in_pathes and out_pathes:  # 终点相接
            return HWY_PATH_CONN_TYPE_E
        return HWY_PATH_CONN_TYPE_NONE  # 无相接
