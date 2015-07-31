# -*- coding: UTF8 -*-
'''
Created on 2014-1-14

@author: hongchenzai
'''
import networkx as nx
from base.component_base import comp_base
from common.database import pg_client
ONE_WAY_BOTH = 1  # 双方
ONE_WAY_POSITIVE = 2  # 正方向
ONE_WAY_RERVERSE = 3  # 逆方向
ONE_WAY_PROHIBITION = 4  # 禁止
UNITS_TO_EXPAND = 0.003  # 大约300米
MAX_EXPAND_COUNT = 2


def load_link_by_box(graph_obj, ewkt_box_polygon):
    if not ewkt_box_polygon:
        return False
    sqlcmd = """
            select link_id, s_node, e_node, one_way_code,
                   length, road_type, link_type, toll
              from link_tbl
              where ST_Intersects(%s, the_geom);
            """
    pg = pg_client()
    pg.connect2()
    links = pg.get_batch_data2(sqlcmd, (ewkt_box_polygon,))
    for link_info in links:
        linkid = link_info[0]
        start_node = link_info[1]
        end_node = link_info[2]
        one_way_code = link_info[3]
        length = link_info[4]
        road_type = link_info[5]
        link_type = link_info[6]
        toll = link_info[7]
        attr = {'link_id': linkid,
                'length': length,
                'road_type': road_type,
                'link_type': link_type,
                'toll': toll
                }
        if one_way_code == ONE_WAY_POSITIVE:
            graph_obj.add_edge(start_node, end_node, attr)
        elif one_way_code == ONE_WAY_RERVERSE:
            graph_obj.add_edge(end_node, start_node, attr)
        elif one_way_code == ONE_WAY_BOTH:
            graph_obj.add_edge(start_node, end_node, attr)
            graph_obj.add_edge(end_node, start_node, attr)
        else:  # 禁止
            pass
    pg.close2()
    return True


#==============================================================================
# all_shortest_paths_in_expand_box
#==============================================================================
def all_shortest_paths_in_expand_box(graph_obj,
                                     ewkt_box_polygon,
                                     start_edge,
                                     end_edge,
                                     units_to_expand=UNITS_TO_EXPAND,
                                     max_expand=MAX_EXPAND_COUNT,
                                     direction=False,
                                     weight='length',
                                     in_oneway_code=None,
                                     out_oneway_code=None
                                     ):
    paths = list()
    inter_node = get_intersection_node(start_edge,
                                       end_edge,
                                       in_oneway_code,
                                       out_oneway_code
                                       )
    if inter_node:
        return [[inter_node]]  # 两条link相交，直接返回空list。注：不判断是否可通行

    expand_cnt = 0
    while expand_cnt < max_expand:
        if not load_link_by_box(graph_obj, ewkt_box_polygon):
            break
        try:
            paths = graph_obj.all_shortest_paths(start_edge,
                                                 end_edge,
                                                 direction,
                                                 weight
                                                 )
            break
        except nx.NetworkXNoPath:  # 找不到路
            expand_cnt += 1
            ewkt_box_polygon = expand_box(ewkt_box_polygon,
                                          units_to_expand * expand_cnt
                                          )
    return paths


#==============================================================================
# is_intersection
#==============================================================================
def get_intersection_node(start_edge,
                          end_edge,
                          in_oneway_code=None,
                          out_oneway_code=None
                          ):
    '''InLink和OutLink相交'''
    start_nodes = list(start_edge)
    end_nodes = list(end_edge)
    if in_oneway_code:
        if in_oneway_code == ONE_WAY_BOTH:
            start_nodes = list(start_edge)
        elif in_oneway_code == ONE_WAY_POSITIVE:
            start_nodes = [start_edge[1]]
        elif in_oneway_code == ONE_WAY_RERVERSE:
            start_nodes = [start_edge[0]]
        else:  # 禁止
            return None
    if out_oneway_code:
        if out_oneway_code == ONE_WAY_BOTH:
            end_nodes = end_edge
        elif out_oneway_code == ONE_WAY_POSITIVE:
            end_nodes = [end_edge[0]]
        elif out_oneway_code == ONE_WAY_RERVERSE:
            end_nodes = [end_edge[1]]
        else:  # 禁止
            return None

    for s_node in start_nodes:
        for e_node in end_nodes:
            if s_node == e_node:
                return s_node
    return None


#==============================================================================
# expand_box
#==============================================================================
def expand_box(ewkt_box_polygon,
               units_to_expand=UNITS_TO_EXPAND):
    pg = pg_client()
    pg.connect2()
    sqlcmd = """
    SELECT ST_ASEWKT(ST_Expand(ST_GeomFromEWKT(%s), %s))
    """
    pg.execute2(sqlcmd, (ewkt_box_polygon, units_to_expand))
    row = pg.fetchone2()
    if row:
        return row[0]
    return None


def get_daufalt_expand_box(s_link_id, t_link_id,
                           units_to_expand=UNITS_TO_EXPAND):
    pg = pg_client()
    pg.connect2()
    pg.CreateFunction2_ByName('mid_get_expand_box')
    sqlcmd = """
    SELECT ST_ASEWKT(mid_get_expand_box(%s, %s, %s))
    """
    pg.execute2(sqlcmd, (s_link_id, t_link_id, units_to_expand))
    row = pg.fetchone2()
    if row:
        return row[0]
    return None


def get_node_expand_box(s_node_id, t_node_id,
                        units_to_expand=UNITS_TO_EXPAND):
    pg = pg_client()
    pg.connect2()
    pg.CreateFunction2_ByName('mid_get_node_expand_box')
    sqlcmd = """
    SELECT ST_ASEWKT(mid_get_node_expand_box(%s, %s, %s))
    """
    pg.execute2(sqlcmd, (s_node_id, t_node_id, units_to_expand))
    row = pg.fetchone2()
    if row:
        return row[0]
    return None


#==============================================================================
# LinkGraph
#==============================================================================
class LinkGraph(nx.DiGraph):
    '''
    Link图
    '''
    def __init__(self, data=None, **attr):
        '''
        Constructor
        '''
        nx.DiGraph.__init__(self, data, **attr)

    def all_shortest_paths(self, start_edge, end_edge,
                           direction=False, weight=None):
        """求两条的所有最短路径
        Parameters
        ----------
        start_edge : (node_id1, node_id2)
           Starting edge for path.

        end_edge : (node_id3, node_id4)
           Ending edge for path.

        direction : False or True, optional (default = False)
           If False, 双向搜索.
           If True, 单向搜索，即只搜索 node2 --> node3.
           If direction = 1,从inlink某个端点单向搜索到outlink两端，取最短路径
           If direction = 2,从inlink某个端点单向搜索到outlink一端，取最短路径
        Returns
        -------
        paths: lists
           all paths between start_edge and end_edge.
        """
        # diG = nx.DiGraph()
        if not direction:  # 双向
            start_dir = self._get_edge_direction(start_edge[0], start_edge[1])
            if start_dir == ONE_WAY_PROHIBITION:  # 双向禁止
                return []
            end_dir = self._get_edge_direction(end_edge[0], end_edge[1])
            if end_dir == ONE_WAY_PROHIBITION:  # 双向禁止
                return []

            if start_dir == ONE_WAY_POSITIVE:
                start_node_list = [start_edge[1]]
            elif start_dir == ONE_WAY_RERVERSE:
                start_node_list = [start_edge[0]]
            else:
                start_node_list = list(start_edge)

            if end_dir == ONE_WAY_POSITIVE:
                end_node_list = [end_edge[0]]
            elif end_dir == ONE_WAY_RERVERSE:
                end_node_list = [end_edge[1]]
            else:
                end_node_list = list(end_edge)
        elif direction == 1:  # 指定开始点，不指定结束点
            start_node_list = [start_edge[0]]
            end_node_list = list(end_edge)
        elif direction == 2:  # 指定开始点和结束点
            start_node_list = [start_edge[0]]
            end_node_list = [end_edge[0]]
        else:
            start_node_list = [start_edge[1]]
            end_node_list = [end_edge[0]]

        shortest_paths = list()
        shortest_length = -1
        for s_node in start_node_list:
            for e_node in end_node_list:
                paths = list(nx.all_shortest_paths(self, s_node,
                                                   e_node, weight))
                if paths:
                    length = self.get_path_weight(paths[0])
                    # length=0: 两个node点相同，即两条link相交
                    if shortest_length < 0:
                        shortest_length = length
                        shortest_paths = paths
                    else:
                        if shortest_length > length:
                            shortest_length = length
                            shortest_paths = paths
                        elif shortest_length == length:
                            shortest_paths += paths
                        else:
                            pass
        # ## nodeid转成linkid
#         path_link = list()
#         for path in shortest_paths:
#             links = self.get_linkid_of_path(path)
#             if links:
#                 path_link.append(links)
        return shortest_paths

    def get_linkid_of_path(self, path, link_id='link_id'):
        'nodeid转成linkid'
        linkid_list = list()
        for u, v in zip(path[0:-1], path[1:]):
            linkid = self.get_edge_data(u, v).get(link_id)
            linkid_list.append(str(linkid))
        return linkid_list

    def get_path_weight(self, path, weight='length'):
        w = 0
        if not weight:
            return len(path)
        for u, v in zip(path[0:-1], path[1:]):
            w += self.get_edge_data(u, v).get(weight)
        return w

    def _get_edge_direction(self, u, v):
        direction = ONE_WAY_PROHIBITION
        # 正向存在
        if self.has_edge(u, v):
            direction = ONE_WAY_POSITIVE
        # 逆向存在
        if self.has_edge(v, u):
            if direction == ONE_WAY_POSITIVE:
                direction = ONE_WAY_BOTH
            else:
                direction = ONE_WAY_RERVERSE
        return direction
