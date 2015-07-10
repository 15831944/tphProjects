# -*- coding: UTF-8 -*-
'''
Created on 2014-12-30

@author: hcz
'''
import copy
import networkx as nx
import re
from component.jdb.hwy.hwy_graph import MAX_CUT_OFF
from component.jdb.hwy.hwy_graph import ONE_WAY_BOTH
from component.jdb.hwy.hwy_graph import ONE_WAY_POSITIVE
from component.jdb.hwy.hwy_graph import ONE_WAY_RERVERSE
from component.jdb.hwy.hwy_graph import ONE_WAY_PROHIBITION
from component.jdb.hwy.hwy_graph import HwyGraph
from component.jdb.hwy.hwy_graph import dfs_edges_reverse
from component.rdf.hwy.hwy_def_rdf import SIMILAR_DEGREE_NUM
from component.rdf.hwy.hwy_def_rdf import SIMILAR_DEGREE_NAME
from component.rdf.hwy.hwy_def_rdf import SIMILAR_DEGREE_NO_NUM_NAME
from component.rdf.hwy.hwy_def_rdf import SIMILAR_DEGREE_ROAD_TYPE
from component.rdf.hwy.hwy_def_rdf import SIMILAR_DEGREE_LINK_TYPE
from component.rdf.hwy.hwy_def_rdf import SIMILAR_DEGREE_NONE
from component.rdf.hwy.hwy_def_rdf import HWY_ROAD_TYPE_HWY
from component.rdf.hwy.hwy_def_rdf import HWY_LINK_TYPE_MAIN1
from component.rdf.hwy.hwy_def_rdf import HWY_LINK_TYPE_MAIN2
from component.rdf.hwy.hwy_def_rdf import HWY_LINK_TYPE_MAIN
from component.rdf.hwy.hwy_def_rdf import HWY_LINK_TYPE_JCT
from component.rdf.hwy.hwy_def_rdf import HWY_LINK_TYPE_RAMP
from component.rdf.hwy.hwy_def_rdf import HWY_LINK_TYPE_SIDE
from component.rdf.hwy.hwy_def_rdf import HWY_LINK_TYPE_SAPA
from component.rdf.hwy.hwy_def_rdf import HWY_LINK_TYPE_SIDE
from component.rdf.hwy.hwy_def_rdf import HWY_LINK_TYPE_INNER
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_PA
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_IC
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_JCT
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_VIRTUAl_JCT
from component.rdf.hwy.hwy_def_rdf import ANGLE_45
from component.rdf.hwy.hwy_def_rdf import ANGLE_60
from component.rdf.hwy.hwy_def_rdf import ANGLE_80
from component.rdf.hwy.hwy_def_rdf import ANGLE_90
from component.rdf.hwy.hwy_def_rdf import ANGLE_180
from component.rdf.hwy.hwy_def_rdf import ANGLE_270
from component.rdf.hwy.hwy_def_rdf import ANGLE_280
from component.rdf.hwy.hwy_def_rdf import ANGLE_300
from component.rdf.hwy.hwy_def_rdf import ANGLE_315
from component.rdf.hwy.hwy_def_rdf import ANGLE_360
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_TOLL
from component.rdf.hwy.hwy_def_rdf import HWY_INOUT_TYPE_NONE
from component.rdf.hwy.hwy_def_rdf import HWY_INOUT_TYPE_IN
from component.rdf.hwy.hwy_def_rdf import HWY_INOUT_TYPE_OUT
HWY_EXIT_POI_NAME = 'exit_poi_name'
HWY_EXIT_NAME = "exit_name"  # 出口名称/出口番号
HWY_JUNCTION_NAME = "juntion_name"
HWY_TOLL_FLAG = "toll_flag"
HWY_NODE_NAME = "node_name"
HWY_ROAD_CODE = "road_code"
HWY_PATH_ID = "path_id"
HWY_LINK_TYPE = "link_type"
HWY_ROAD_TYPE = "road_type"
HWY_LINK_LENGTH = "length"
HWY_ROAD_CODE = "road_code"
HWY_FACIL_CLASS = "facil_cls"  # 设施类型/种别
HWY_DISP_CLASS = "display_class"
HWY_TILE_ID = "tile_id"
HWY_1ST_ROAD_NAME = 'first_road_name'
HWY_ORG_FACIL_ID = 'org_facil_id'
HWY_ROAD_NUMS = "numbers"
HWY_ROAD_NAMES = "names"


# ==============================================================================
# HwyGraphRDF
# ==============================================================================
class HwyGraphRDF(HwyGraph):
    '''
    Highway Graph
    '''

    def __init__(self, data=None, **attr):
        '''
        Constructor
        '''
        HwyGraph.__init__(self, data, **attr)

    def set_ref(self, u, v):
        self.add_edge(u, v, reference=True)

    def add_link(self, u, v, one_way, link_id, s_angle, e_angle, **attr):
        s_angle += ANGLE_180
        e_angle += ANGLE_180
        if one_way == ONE_WAY_BOTH:
            self.add_edge(u, v,
                          {'link_id': link_id, 'one_way': one_way,
                           's_angle': s_angle, 'e_angle': e_angle},
                          **attr)
            self.add_edge(v, u,
                          {'link_id': link_id, 'one_way': one_way,
                           's_angle': e_angle, 'e_angle': s_angle},
                          **attr)
        elif one_way == ONE_WAY_POSITIVE:
            self.add_edge(u, v,
                          {'link_id': link_id, 'one_way': one_way,
                           's_angle': s_angle, 'e_angle': e_angle},
                          **attr)
        elif one_way == ONE_WAY_RERVERSE:
            self.add_edge(v, u,
                          {'link_id': link_id, 'one_way': one_way,
                           's_angle': e_angle, 'e_angle': s_angle},
                          **attr)
        else:
            pass

    def get_ref(self, u, v):
        return self[u][v]["reference"]

    def is_referenceed(self, data):
        if data.get("reference"):
            return True
        return False

    def get_length(self, u, v, field=HWY_LINK_LENGTH):
        if not self.has_edge(u, v):
            return None
        return self[u][v].get(field)

    def get_main_path_by_similar(self, u, v):
        '''取得本线道路片段路径从开关、分歧、合流到结尾，分歧、合流之间的段路径。(本线)'''
        if not self.has_edge(u, v):
            return []
        # 正方向(顺车流)的路径
        pathes = []
        for path in self.dfs_main_path_by_similar(u, v):
            if path:
                if is_cycle_path(path):  # 环
                    return copy.copy(path)
                else:
                    pathes.append(copy.copy(path))
            else:
                print '' % (u, v, pathes)
        if len(pathes) > 1:
            print 'More Path. u=%s, v=%s' % (u, v)
            for path in pathes:
                print path
            # return []
        # 反方向(逆车流)的路径
        reverse_pathes = []
        for path in self.dfs_main_path_reverse_by_similar(u, v):
            if path:
                path = copy.copy(path)
                path.reverse()
                if is_cycle_path(path):  # 环
                    return path
                else:
                    reverse_pathes.append(path)
        if len(reverse_pathes) > 1:
            print 'More Reverse Path. u=%s, v=%s' % (u, v)
            for path in reverse_pathes:
                print path
            # return []
        if reverse_pathes and reverse_pathes[0] and pathes and pathes[0]:
            return reverse_pathes[0] + pathes[0]
        return []

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
#             elif self._is_conn_hwy_both_link(child):  # 到达双向通行高速
#                 stack.pop()
#                 visited.pop()
            elif (self._outlink_is_hov(child) and
                  self._inlink_exist_hov(child)):
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
#             elif self._is_conn_hwy_both_link(parent):  # 到达双向通行高速
#                 stack.pop()
#                 visited.pop()
            elif (self._outlink_exist_hov(parent) and
                  self._inlink_is_hov(parent)):
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

    def get_most_similar_out_main_link(self, u, v):
        '''最相似的脱出本线.'''
        successors = []
        data = self[u][v]
        out_main_link = self._get_out_main_link_degree(v, data)
        max_out_links = self._get_max_degree_link(out_main_link)
        for (child, child_data, degree) in max_out_links:
            if degree <= SIMILAR_DEGREE_NONE:  # 相似度为0
                continue
            # ## 子link的所有in link情报
            in_main_link = self._get_in_main_link_degree(v, child_data)
            max_in_links = self._get_max_degree_link(in_main_link)
            if len(max_in_links) > 1:  # 多条相似度一样的link.
                # print 'Similar In Link more than 1. nodeid=%s' % u
                return []
            (parent, parent_data, p_degree) = max_in_links[0]
            if u == parent and p_degree > SIMILAR_DEGREE_NONE:
                successors.append(child)
        if len(successors) > 1:
            return []
        return successors

    def get_most_similar_in_main_link(self, u, v):
        '''最相似的脱出本线. 如果有多条返回空'''
        predecessors = []
        data = self[u][v]
        in_main_link = self._get_in_main_link_degree(u, data)
        max_in_link = self._get_max_degree_link(in_main_link)
        for (parent, parent_data, degree) in max_in_link:
            if degree <= SIMILAR_DEGREE_NONE:  # 相似度为0
                continue
            # 父link的所有out link情报
            out_main_link = self._get_out_main_link_degree(u, parent_data)
            max_out_link = self._get_max_degree_link(out_main_link)
            if len(max_out_link) > 1:  # 多条相似度一样的link.
                # print 'Similar Out Link more than 1. nodeid=%s' % u
                return []
            (child, child_data, c_degree) = out_main_link[0]
            if v == child and c_degree > SIMILAR_DEGREE_NONE:
                predecessors.append(parent)
        if len(predecessors) > 1:
            return []
        return predecessors

    def _get_out_main_link_degree(self, node, data):
        out_main_link = []
        for node, child, child_data in self.out_edges_iter(node, True):
            if(not self.is_hwy_main_link(child_data) and
               not self.is_hwy_inner_link(child_data) and
               not self.has_edge(child, node)):  # 双向通行
                continue
            degree = self._get_similar_degree(data, child_data)
            out_main_link.insert(degree, (child, child_data, degree))
        # 相似度高的排前面
        out_main_link.sort(cmp=lambda x, y: cmp(x[-1], y[-1]), reverse=True)
        return out_main_link

    def _get_max_degree_link(self, link_degree):
        if not link_degree:
            return []
        max_degree_info = []
        max_degree = SIMILAR_DEGREE_NONE - 1
        for (node, data, degree) in link_degree:
            if degree >= max_degree:
                max_degree_info.append((node, data, degree))
                max_degree = degree
            else:
                break
        return max_degree_info

    def _get_in_main_link_degree(self, node, data):
        in_main_link = []
        for parent, node, parent_data in self.in_edges_iter(node, True):
            if(not self.is_hwy_main_link(parent_data) and
               not self.is_hwy_inner_link(parent_data) and
               not self.has_edge(node, parent)):  # 双向通行:
                continue
            degree = self._get_similar_degree(data, parent_data)
            in_main_link.append((parent, parent_data, degree))
        # 相似度高的排前面
        in_main_link.sort(cmp=lambda x, y: cmp(x[-1], y[-1]), reverse=True)
        return in_main_link

    def _get_similar_degree(self, data1, data2):
        degree = SIMILAR_DEGREE_NONE
        number1 = data1.get('numbers')
        number2 = data2.get('numbers')
        name1 = data1.get('names')
        name2 = data2.get('names')
        if not number1 or not number2 and not name1 and not name2:
            degree |= SIMILAR_DEGREE_NO_NUM_NAME
        else:
            if self._is_similer_number(number1, number2):
                degree |= SIMILAR_DEGREE_NUM
            if self._is_similer_number(name1, name2):
                degree |= SIMILAR_DEGREE_NAME
        if degree > SIMILAR_DEGREE_NONE:
            link_type1 = data1.get(HWY_LINK_TYPE)
            link_type2 = data2.get(HWY_LINK_TYPE)
            if link_type1 == link_type2:
                degree |= SIMILAR_DEGREE_LINK_TYPE
        return degree

    def is_hwy_main_link(self, data):
        if(data.get(HWY_ROAD_TYPE) in (0, 1) and
           data.get(HWY_LINK_TYPE) in (1, 2)):
            return True
        else:
            return False

    def is_hwy_inner_link(self, data):
        if(data.get(HWY_ROAD_TYPE) in HWY_ROAD_TYPE_HWY and
           data.get(HWY_LINK_TYPE) == HWY_LINK_TYPE_INNER):
            return True
        else:
            return False

    def _is_conn_hwy_both_link(self, node):
        '''连接高速双向link.'''
        for node, child, child_data in self.out_edges_iter(node, True):
            if(self.has_edge(child, node) and
               self.is_hwy_main_link(child_data)):
                return True
        for parent, node, parent_data in self.in_edges_iter(node, True):
            if(self.has_edge(node, parent) and
               self.is_hwy_main_link(parent_data)):
                return True
        return False

    def _is_conn_hov_link(self, node):
        '''连接HOV道路'''
        for node, child, child_data in self.out_edges_iter(node, True):
            if(self.is_hwy_main_link(child_data) and
               self._is_hov(child_data)):
                return True
        for parent, node, parent_data in self.in_edges_iter(node):
            if(self.is_hwy_main_link(parent_data) and
               self._is_hov(parent_data)):
                return True
        return False

    def _outlink_exist_hov(self, node):
        for node, child, child_data in self.out_edges_iter(node, True):
            if(self.is_hwy_main_link(child_data) and
               self._is_hov(child_data)):
                return True
        return False

    def _inlink_exist_hov(self, node):
        for parent, node, parent_data in self.in_edges_iter(node, True):
            if(self.is_hwy_main_link(parent_data) and
               self._is_hov(parent_data)):
                return True
        return False

    def _outlink_is_hov(self, node):
        '''所有的高速本线out link都是HOV'''
        hov_flag = True
        exist_hov = False
        for node, child, child_data in self.out_edges_iter(node, True):
            if self.is_hwy_main_link(child_data):
                if self._is_hov(child_data):
                    exist_hov = True
                else:
                    hov_flag = False
        if exist_hov:
            return hov_flag
        return False

    def _inlink_is_hov(self, node):
        '''所有的高速本线in link都是HOV'''
        hov_flag = True
        exist_hov = False
        for parent, node, parent_data in self.in_edges_iter(node, True):
            if self.is_hwy_main_link(parent_data):
                if self._is_hov(parent_data):
                    exist_hov = True
                else:
                    hov_flag = False
        if exist_hov:
            return hov_flag
        return False

    def _outlink_is_both(self, node):
        '''连接高速双向link.'''
        both_flag = True
        exist_both = False
        for node, child, child_data in self.out_edges_iter(node, True):
            if self.is_hwy_main_link(child_data):
                if self.has_edge(child, node):
                    exist_both = True
                else:
                    both_flag = False
        if exist_both:
            return both_flag
        return False

    def _inlink_is_both(self, u, v):
        '''连接高速双向link.'''
        both_flag = True
        exist_both = False
        for parent, node, parent_data in self.in_edges_iter(node, True):
            if self._is_hwy_main_link(parent_data):
                if self.has_edge(node, parent):
                    exist_both = True
                else:
                    both_flag = False
        if exist_both:
            return both_flag
        return False

    def is_hov(self, u, v, data=None):
        if data:
            return self._is_hov(data)
        else:
            return self._is_hov(self[u][v])

    def _is_hov(self, data):
        '''HOV道路'''
        hov = r'\WHOV\W'  # HOV
        p = re.compile(hov, re.I)  # 忽略大小写
        names = data.get(HWY_ROAD_NAMES)
        for name in names:
            s = name.get('val')
            # 名称有 HOV
            temp = p.findall(s)
            if temp:
                return True
        return False

    def _is_multi_edges(self, node):
        '''边大于2'''
        if len(self.out_edges(node)) + len(self.in_edges(node)) > 3:
            return True
        return False

    def _is_similer_number(self, num1, num2):
        if (num1 and not num2) or (not num1 and num2):
            return False
        if not num1 and not num2:
            return True
        if self._get_intersection(num1, num2):
            return True
        return False

    def _is_similer_name(self, name1, name2):
        if (name1 and not name2) or (not name1 and name2):
            return False
        if self._get_intersection(name1, name2):
            return True
        return False

    def _get_intersection(self, name1, name2):
        '''名称求交集'''
        for n in name1:
            if n in name2:
                return True
        return False

    def get_angle(self, in_edge, out_edge, reverse=False):
        '''取得两条边的夹角'''
        in_u, in_v = in_edge
        out_u, out_v = out_edge
        if not reverse:  # in_edge, out_edge是顺车方向
            in_angle = self.get_zm_angle(in_edge, in_v)
            out_angle = self.get_zm_angle(out_edge, out_u)
        else:  # in_edge, out_edge是逆车方向
            in_angle = self.get_zm_angle(in_edge, in_u)
            out_angle = self.get_zm_angle(out_edge, out_v)
        if in_angle is None:
            raise nx.NetworkXError('No angle. edge=%s' % in_edge)
            return None
        if out_angle is None:
            raise nx.NetworkXError('No angle. edge=%s' % out_edge)
        angle = abs(out_angle - in_angle) % ANGLE_360
        return angle

    def get_zm_angle(self, edge, node):
        '''取得 笛卡尔坐标的绝对角度'''
        u, v = edge
        data = self[u][v]
        if not data:
            return None
        if node == u:  # 起点的角度
            return data.get('s_angle')
        elif node == v:
            return data.get('e_angle')
        return None

    def is_hwy_exit_poi(self, node_id):
        '''有出口POI'''
        return False

    def get_org_facil_id(self, node_id):
        data = self.node[node_id]
        return data.get(HWY_ORG_FACIL_ID)

    def is_exit_name(self, node_id):
        '''有出口名称'''
        data = self.node[node_id]
        if data.get(HWY_EXIT_NAME):
            return True
        return False

    def get_exit_poi_name(self, node_id):
        data = self.node[node_id]
        return data.get(HWY_EXIT_POI_NAME)

    def get_exit_name(self, node_id):
        data = self.node[node_id]
        return data.get(HWY_EXIT_NAME)

    def get_junction_name(self, node_id):
        data = self.node[node_id]
        return data.get(HWY_JUNCTION_NAME)

    def is_tollgate(self, node_id):
        '''收费站'''
        data = self.node[node_id]
        if data.get(HWY_TOLL_FLAG):
            return True
        return False

    def get_facil_types(self, node, road_code, code_field=HWY_ROAD_CODE):
        '''
           road_code: code_field='road_code', 是road_code值。
               otherwise, 是pathid值。
           road_code: 指定线路号，用的是哪个字段('road_code'、 'pathid')
        '''
        types = set()
        if self.is_tollgate(node):
            types.add((HWY_IC_TYPE_TOLL, HWY_INOUT_TYPE_NONE))
        # 入口/合流
        out_nodes = self.get_main_link(node, road_code, code_field,
                                       same_code=True, reverse=False)
        if out_nodes and len(out_nodes) == 1:
            out_node = out_nodes[0]
            for path, facilcls in self.__all_facil_path2(node, out_node,
                                                         road_code, code_field,
                                                         reverse=True):
                types.add((facilcls, HWY_INOUT_TYPE_IN))
        if len(out_nodes) > 1:
            raise nx.NetworkXError('Multi Out Main Links. node=%s ' % node)
        # 出口/分歧
        in_nodes = self.get_main_link(node, road_code, code_field,
                                      same_code=True, reverse=True)
        if in_nodes and len(in_nodes) == 1:
            in_node = in_nodes[0]
            for path, facilcls in self.__all_facil_path2(in_node, node,
                                                         road_code, code_field,
                                                         reverse=False):
                types.add((facilcls, HWY_INOUT_TYPE_OUT))
        if len(in_nodes) > 1:
            raise nx.NetworkXError('Multi In Main Links. node=%s ' % node)
        return types
        # ########################
        for path, facilcls in self.__all_facil_path(node, road_code,
                                                    code_field, reverse=True):
            types.add((facilcls, HWY_INOUT_TYPE_IN))
        # 出口/分歧
        for path, facilcls in self.__all_facil_path(node, road_code,
                                                    code_field, reverse=False):
            types.add((facilcls, HWY_INOUT_TYPE_OUT))
        return types

    def get_all_facil(self, node, road_code, code_field=HWY_ROAD_CODE):
        '''
           road_num: road_code指定字段对应的值
                while road_code='road_code', 是road_code值。
                otherwise, 是pathid值。
           road_code: 指定线路号，用的是哪个字段('road_code'、 'pathid')
        '''
        all_facils = []
        if self.is_tollgate(node):
            all_facils.append((HWY_IC_TYPE_TOLL, HWY_INOUT_TYPE_NONE, [node]))
        # 入口/合流
        out_nodes = self.get_main_link(node, road_code, code_field,
                                       same_code=True, reverse=False)
        if out_nodes and len(out_nodes) == 1:
            out_node = out_nodes[0]
            for path, facilcls in self.__all_facil_path2(node, out_node,
                                                         road_code, code_field,
                                                         reverse=True):
                all_facils.append((facilcls, HWY_INOUT_TYPE_IN, path))
        if len(out_nodes) > 1:
            raise nx.NetworkXError('Multi Out Main Links. node=%s ' % node)
        # 出口/分歧
        in_nodes = self.get_main_link(node, road_code, code_field,
                                      same_code=True, reverse=True)
        if in_nodes and len(in_nodes) == 1:
            in_node = in_nodes[0]
            for path, facilcls in self.__all_facil_path2(in_node, node,
                                                         road_code, code_field,
                                                         reverse=False):
                all_facils.append((facilcls, HWY_INOUT_TYPE_OUT, path))
        if len(in_nodes) > 1:
            raise nx.NetworkXError('Multi In Main Links. node=%s ' % node)
        return all_facils

    def get_in_facil(self, node_id, road_code, code_field=HWY_ROAD_CODE):
        '''入口/合流设施'''
        for path, type in self.__all_facil(node_id, road_code,
                                           code_field, reverse=True):
            pass
        pass

    def get_out_facil(self, node_id, road_code, code_field=HWY_ROAD_CODE):
        '''出口/分歧设施'''
        pass

    def is_hwy_inout(self, u, v, reverse=False):
        '''高速(Ramp)和一般道、高速双向路的交汇点。
           reverse: False,顺车流；True,逆车流
        '''
        if reverse:  # 逆
            edges_iter = self.in_edges_iter(u, True)
        else:  # 顺
            edges_iter = self.out_edges_iter(v, True)
        in_edge = (u, v)
        for temp_u, temp_v, data in edges_iter:
            out_edge = temp_u, temp_v
            # 夹角小于90度
            angle = self.get_angle(in_edge, out_edge, reverse)
            if not self._bigger_inout_min_angle(angle):
                continue
            # 注：这里没有考虑角度
            if self.has_edge(temp_v, temp_u):  # 双向道路
                return True
            link_type = data.get(HWY_LINK_TYPE)
            road_type = data.get(HWY_ROAD_TYPE)
            if(link_type != HWY_LINK_TYPE_SAPA and
               road_type not in HWY_ROAD_TYPE_HWY):
                return True
        return False

    def _bigger_inout_min_angle(self, angle):
        if angle < ANGLE_80 or angle > ANGLE_280:
            return False
        return True

    def bigger_hwy_main_min_angle(self, angle):
        if angle < ANGLE_60 or angle > ANGLE_300:
            return False
        return True

    def is_jct(self, node, road_code, code_field=HWY_ROAD_CODE, reverse=False):
        '''JCT分歧、合流'''
        if reverse:
            edges_iter = self.in_edges_iter(node, True)
        else:
            edges_iter = self.out_edges_iter(node, True)
        for u, v, data in edges_iter:
            temp_route_code = data.get(code_field)
            if temp_route_code and temp_route_code != road_code:
                return True
        return False

    def is_virtual_jct(self, node, road_code,
                       code_field=HWY_ROAD_CODE, reverse=False):
        ''''''
        if reverse:  # 逆
            # 取得进入本线
            in_mains = self.get_main_link(node, road_code, code_field,
                                          same_code=False, reverse=reverse)
            if in_mains:
                return False
            # 取得脱出本线
            out_mains = self.get_main_link(node, road_code, code_field,
                                           same_code=False, reverse=False)
            if out_mains:
                return True
        else:  # 顺
            # 取得脱出本线
            out_mains = self.get_main_link(node, road_code, code_field,
                                           same_code=False, reverse=reverse)
            if out_mains:
                return False
            # 取得进入本线
            in_mains = self.get_main_link(node, road_code, code_field,
                                          same_code=False, reverse=True)
            if in_mains:
                return True
        return False

    def is_same_road_code(self, node, road_code,
                          code_field=HWY_ROAD_CODE, reverse=False):
        nodes = self.get_main_link(node, road_code, code_field,
                                   same_code=True, reverse=reverse)
        if nodes:
            return True
        return False

    def exist_sapa_link(self, node, reverse):
        if reverse:
            edges_iter = self.in_edges_iter(node, True)
        else:
            edges_iter = self.out_edges_iter(node, True)
        for u, v, data in edges_iter:
            if data.get(HWY_LINK_TYPE) == HWY_LINK_TYPE_SAPA:
                return True
        return False

    def is_sapa_link(self, u, v):
        data = self[u][v]
        if data.get(HWY_LINK_TYPE) == HWY_LINK_TYPE_SAPA:
            return True
        return False

    def _is_sapa_path(self, path, road_code, code_field, reverse):
        # 第一条link和最后一条都是SAPA Link
        if reverse:  # 逆
            # if self._get_main_link(path, road_code, code_field, reverse):
            f_sapa = self.is_sapa_link(path[1], path[0])
            l_sapa = self.is_sapa_link(path[-1], path[-2])
        else:  # 顺
            # if self._get_main_link(path, road_code, code_field, reverse):
            f_sapa = self.is_sapa_link(path[0], path[1])
            l_sapa = self.is_sapa_link(path[-2], path[-1])
        if f_sapa and l_sapa:
            return True
        return False

    def __all_facil_path(self, source, road_code, code_field=HWY_ROAD_CODE,
                         reverse=False, cutoff=MAX_CUT_OFF):
        ''''''
        if cutoff < 1:
            return
        if source not in self:
            return
        if self.is_hwy_inout(source, reverse):  # 本线直接和一般道直接相连
            yield [source], HWY_IC_TYPE_IC
        visited = [source]
        # stack = [iter(self[source])]
        nodes = self._get_not_main_link(source, code_field, reverse)
        if not nodes:  # 没有分歧/合流
            # 本线和本线直接相连
            if self.is_jct(source, road_code, code_field, reverse):
                yield [source], HWY_IC_TYPE_JCT
            else:
                return
        stack = [iter(nodes)]
        while stack:
            children = stack[-1]
            child = next(children, None)
            if child is None:
                stack.pop()
                visited.pop()
            elif len(visited) <= cutoff:
                if self.is_hwy_inout(child, reverse):  # 出入口
                    yield visited + [child], HWY_IC_TYPE_IC
                    if self.is_virtual_jct(child, road_code,
                                           code_field, reverse):
                        yield visited + [child], HWY_IC_TYPE_VIRTUAl_JCT
                    # 取得非本线
                    nodes = self._get_not_main_link(child, code_field, reverse)
                    if nodes:
                        visited.append(child)
                        stack.append(iter(nodes))
                elif self.is_jct(child, road_code, code_field, reverse):
                    path = visited + [child]
                    yield path, HWY_IC_TYPE_JCT
                    if self._is_sapa_path(path, road_code,
                                          code_field, reverse):
                        yield path, HWY_IC_TYPE_PA
                elif self.is_same_road_code(child, road_code,  # 回到当前线路
                                            code_field, reverse):
                    path = visited + [child]
                    if self._is_sapa_path(path, road_code,
                                          code_field, reverse):
                        yield path, HWY_IC_TYPE_PA
                elif len(visited) < cutoff:
                    if self.is_virtual_jct(child, road_code,
                                           code_field, reverse):
                        yield visited + [child], HWY_IC_TYPE_VIRTUAl_JCT
                    # 取得非本线
                    nodes = self._get_not_main_link(child, code_field, reverse)
                    if nodes:
                        visited.append(child)
                        stack.append(iter(nodes))
                    else:
                        yield visited + [child], HWY_IC_TYPE_IC
                        stack.pop()
                        visited.pop()
                elif len(visited) == cutoff:
                    stack.pop()
                    visited.pop()

    def __all_facil_path2(self, u, v, road_code,
                          code_field=HWY_ROAD_CODE,
                          reverse=False, cutoff=MAX_CUT_OFF):
        ''''''
        if cutoff < 1:
            return
        if reverse:  # 逆
            source = u
        else:  # 顺
            source = v
        if source not in self:
            return
        if self.is_hwy_inout(u, v, reverse):  # 本线直接和一般道直接相连
            yield [source], HWY_IC_TYPE_IC
        visited = [source]
        # stack = [iter(self[source])]
        nodes = self._get_not_main_link(source, code_field, reverse)
        if not nodes:  # 没有分歧/合流
            # 本线和本线直接相连
            if self.is_jct(source, road_code, code_field, reverse):
                yield [source], HWY_IC_TYPE_JCT
            else:
                return
        stack = [iter(nodes)]
        while stack:
            children = stack[-1]
            child = next(children, None)
            if child is None:
                stack.pop()
                visited.pop()
            elif len(visited) <= cutoff:
                # ## 判断两条link的夹角
                # 取得link
                if len(visited) > 1:
                    if reverse:  # 逆
                        in_edge = (visited[-1], visited[-2])
                        out_edge = (child, visited[-1])
                    else:  # 顺
                        in_edge = (visited[-2], visited[-1])
                        out_edge = (visited[-1], child)
                else:
                    in_edge = u, v
                    # 取得本线
                    if reverse:  # 逆
                        out_edge = (child, visited[-1])
                    else:  # 顺
                        out_edge = (visited[-1], child)
                # 夹角小于90
                angle = self.get_angle(in_edge, out_edge, reverse)
                if angle < ANGLE_80 or angle > ANGLE_280:
                    continue
                if self.is_jct(child, road_code, code_field, reverse):
                    path = visited + [child]
                    yield path, HWY_IC_TYPE_JCT
                    if self._is_sapa_path(path, road_code,
                                          code_field, reverse):
                        yield path, HWY_IC_TYPE_PA
                elif self.is_same_road_code(child, road_code,  # 回到当前线路
                                            code_field, reverse):
                    path = visited + [child]
                    if self._is_sapa_path(path, road_code,
                                          code_field, reverse):
                        yield path, HWY_IC_TYPE_PA
                    continue
                elif self.is_hwy_inout(out_edge[0], out_edge[1], reverse):
                    yield visited + [child], HWY_IC_TYPE_IC
                if len(visited) < cutoff:
                    if self.is_virtual_jct(child, road_code,
                                           code_field, reverse):
                        yield visited + [child], HWY_IC_TYPE_VIRTUAl_JCT
                    # 取得非本线
                    nodes = self._get_not_main_link(child, code_field, reverse)
                    if nodes:
                        visited.append(child)
                        stack.append(iter(nodes))
#                     else:
#                         yield visited + [child], HWY_IC_TYPE_IC
#                         stack.pop()
#                         visited.pop()
                elif len(visited) == cutoff:
                    stack.pop()
                    visited.pop()

    def _get_not_main_link(self, node, code_field, reverse=False):
        '''非本线高速link'''
        nodes = []
        if reverse:  # 逆
            edges_iter = self.in_edges_iter(node, True)
        else:  # 顺
            edges_iter = self.out_edges_iter(node, True)
        for u, v, data in edges_iter:
            if data.get(code_field):  # 本线
                continue
            if self.has_edge(v, u):  # 双向
                continue
            road_type = data.get(HWY_ROAD_TYPE)
            if road_type in HWY_ROAD_TYPE_HWY:
                if reverse:  # 逆
                    nodes.append(u)
                else:  # 顺
                    nodes.append(v)
            else:
                link_type = data.get(HWY_LINK_TYPE)
                if link_type in (HWY_LINK_TYPE_JCT,
                                 HWY_LINK_TYPE_RAMP,
                                 HWY_LINK_TYPE_SAPA):
                    if reverse:  # 逆
                        nodes.append(u)
                    else:  # 顺
                        nodes.append(v)
        return nodes

    def get_main_link(self, node, road_code, code_field,
                      same_code=False, reverse=False):
        '''本线高速link'''
        nodes = []
        if reverse:  # 逆
            edges_iter = self.in_edges_iter(node, True)
        else:  # 顺
            edges_iter = self.out_edges_iter(node, True)
        for u, v, data in edges_iter:
            temp_road_code = data.get(code_field)
            if not temp_road_code:  # 非本线
                continue
            # 线路号要相同
            if same_code:
                if temp_road_code == road_code:
                    if reverse:  # 逆
                        nodes.append(u)
                    else:  # 顺
                        nodes.append(v)
            # 线路号不同
            else:
                if temp_road_code != road_code:
                    if reverse:  # 逆
                        nodes.append(u)
                    else:  # 顺
                        nodes.append(v)
        return nodes

    def get_normal_link(self, u, v, reverse=False):
        '''高速(Ramp)相连的一般道、。
           reverse: False,顺车流；True,逆车流
        '''
        nodes = []
        if reverse:  # 逆
            edges_iter = self.in_edges_iter(u, True)
        else:  # 顺
            edges_iter = self.out_edges_iter(v, True)
        in_edge = (u, v)
        for temp_u, temp_v, data in edges_iter:
            out_edge = temp_u, temp_v
            # 夹角小于90度
            angle = self.get_angle(in_edge, out_edge, reverse)
            if angle < ANGLE_80 or angle > ANGLE_280:
                continue
            # 注：这里没有考虑角度
            if self.has_edge(temp_v, temp_u):  # 双向道路
                if reverse:  # 逆
                    nodes.append(temp_u)
                else:  # 顺
                    nodes.append(temp_v)
                continue
            link_type = data.get(HWY_LINK_TYPE)
            road_type = data.get(HWY_ROAD_TYPE)
            if(link_type != HWY_LINK_TYPE_SAPA and
               road_type not in HWY_ROAD_TYPE_HWY):
                if reverse:  # 逆
                    nodes.append(temp_u)
                else:  # 顺
                    nodes.append(temp_v)
        return nodes

    def is_hwy_node(self, nodeid):
        ic_types = self.node[nodeid].get(HWY_FACIL_CLASS)
        if ic_types:
            return True
        else:
            return False

    def get_disp_class(self, u, v):
        return self[u][v][HWY_DISP_CLASS]

    def get_tile_id(self, u, v):
        return self[u][v][HWY_TILE_ID]

    def get_first_name(self, u, v):
        return self[u][v][HWY_1ST_ROAD_NAME]


# ==============================================================================
#
# ==============================================================================
def is_cycle_path(path):
    '''线路是环型的'''
    if len(path) > 2 and path[0] == path[-1]:
        return True
    else:
        return False
