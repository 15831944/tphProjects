# -*- coding: UTF-8 -*-
'''
Created on 2014-12-30

@author: hcz
'''
import copy
import networkx as nx
import re
import common
from component.jdb.hwy.hwy_graph import MAX_CUT_OFF
from component.jdb.hwy.hwy_graph import ONE_WAY_BOTH
from component.jdb.hwy.hwy_graph import ONE_WAY_POSITIVE
from component.jdb.hwy.hwy_graph import ONE_WAY_RERVERSE
from component.jdb.hwy.hwy_graph import HwyGraph
from component.rdf.hwy.hwy_def_rdf import SIMILAR_DEGREE_NUM, HWY_IC_TYPE_SAPA
from component.rdf.hwy.hwy_def_rdf import HWY_ROAD_TYPE_HWY0
from component.rdf.hwy.hwy_def_rdf import HWY_ROAD_TYPE_HWY1
from component.rdf.hwy.hwy_def_rdf import SIMILAR_DEGREE_NAME
from component.rdf.hwy.hwy_def_rdf import SIMILAR_DEGREE_NO_NUM_NAME
from component.rdf.hwy.hwy_def_rdf import SIMILAR_DEGREE_LINK_TYPE
from component.rdf.hwy.hwy_def_rdf import SIMILAR_DEGREE_NONE
from component.rdf.hwy.hwy_def_rdf import HWY_ROAD_TYPE_HWY
from component.rdf.hwy.hwy_def_rdf import HWY_LINK_TYPE_JCT
from component.rdf.hwy.hwy_def_rdf import HWY_LINK_TYPE_RAMP
from component.rdf.hwy.hwy_def_rdf import HWY_LINK_TYPE_SAPA
from component.rdf.hwy.hwy_def_rdf import HWY_LINK_TYPE_INNER
from component.rdf.hwy.hwy_def_rdf import HWY_LINK_TYPE_RTURN
from component.rdf.hwy.hwy_def_rdf import HWY_LINK_TYPE_LTURN
from component.rdf.hwy.hwy_def_rdf import HWY_LINK_TYPE_MAIN
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_PA
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_IC
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_JCT
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_UTURN
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_VIRTUAl_JCT
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_SERVICE_ROAD
from component.rdf.hwy.hwy_def_rdf import ANGLE_10
from component.rdf.hwy.hwy_def_rdf import ANGLE_30
from component.rdf.hwy.hwy_def_rdf import ANGLE_60
from component.rdf.hwy.hwy_def_rdf import ANGLE_80
from component.rdf.hwy.hwy_def_rdf import ANGLE_180
from component.rdf.hwy.hwy_def_rdf import ANGLE_280
from component.rdf.hwy.hwy_def_rdf import ANGLE_300
from component.rdf.hwy.hwy_def_rdf import ANGLE_360
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_TOLL
from component.rdf.hwy.hwy_def_rdf import HWY_INOUT_TYPE_NONE
from component.rdf.hwy.hwy_def_rdf import HWY_INOUT_TYPE_IN
from component.rdf.hwy.hwy_def_rdf import HWY_INOUT_TYPE_OUT
HWY_EXIT_POI_NAME = 'exit_poi_name'
HWY_ENTER_POI_NAME = 'enter_poi_name'
HWY_EXIT_NAME = "exit_name"  # 出口名称/出口番号
HWY_JUNCTION_NAME = "juntion_name"
HWY_TOLL_FLAG = "toll_flag"
HWY_NODE_NAME = "node_name"
HWY_ROAD_CODE = "road_code"
HWY_UPDOWN_CODE = "up_down"
HWY_PATH_ID = "path_id"
HWY_LINK_TYPE = "link_type"
HWY_ROAD_TYPE = "road_type"
HWY_LINK_LENGTH = "length"
HWY_FACIL_CLASS = "facil_cls"  # 设施类型/种别
HWY_DISP_CLASS = "display_class"
HWY_TILE_ID = "tile_id"
HWY_1ST_ROAD_NAME = 'first_road_name'
HWY_ORG_FACIL_ID = 'org_facil_id'
HWY_ORG_FACIL_INFO = 'org_facil_info'
HWY_ROAD_NUMS = "numbers"
HWY_ROAD_NAMES = "names"
HWY_S_NODE = "start_node"
HWY_E_NODE = "end_node"
HWY_REGULATION = "regulation"
HWY_EXTEND_FLAG = "extend_flag"


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
        self.log = common.log.common_log.instance().logger('HwyGraph')

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

    def _get_road_code(self, u, v):
        return self[u][v][HWY_ROAD_CODE]

    def _get_road_updown(self, u, v):
        return self[u][v].get(HWY_UPDOWN_CODE)

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

    def get_node_name(self, node):
        data = self.node[node]
        return data.get(HWY_NODE_NAME)

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
            self.log.error('More Path. u=%s, v=%s' % (u, v))
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
            self.log.error('More Reverse Path. u=%s, v=%s' % (u, v))
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
            elif not self.check_regulation([u] + visited + [child]):
                # print visited[-1]
                yield visited
                stack.pop()
                visited.pop()
            elif (self._outlink_is_hov(child) and
                  self._inlink_exist_hov(child)):
                stack.pop()
                visited.pop()
            elif self._outlink_is_both(child):
                yield visited + [child]
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
            elif not self.check_regulation([v] + visited + [parent],
                                           reverse=True):
                # print visited[-1]
                yield visited
                stack.pop()
                visited.pop()
            elif (self._outlink_exist_hov(parent) and
                  self._inlink_is_hov(parent)):
                stack.pop()
                visited.pop()
            elif self._inlink_is_both(parent):
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
        if not number1 and not number2 and not name1 and not name2:
            degree |= SIMILAR_DEGREE_NO_NUM_NAME
        else:
            if self._is_similar_number(number1, number2):
                degree |= SIMILAR_DEGREE_NUM
            if self.is_similar_name(name1, name2):
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

    def is_normal_inner_link(self, data):
        lint_type = data.get(HWY_LINK_TYPE)
        if(data.get(HWY_ROAD_TYPE) not in HWY_ROAD_TYPE_HWY and
           lint_type in (HWY_LINK_TYPE_INNER,
                         HWY_LINK_TYPE_RTURN,
                         HWY_LINK_TYPE_LTURN)):
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

    def _inlink_is_both(self, node):
        '''连接高速双向link.'''
        both_flag = True
        exist_both = False
        for parent, node, parent_data in self.in_edges_iter(node, True):
            if self.is_hwy_main_link(parent_data):
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

    def _is_similar_number(self, num1, num2):
        if (num1 and not num2) or (not num1 and num2):
            return False
        if not num1 and not num2:
            return False
        if self._get_intersection(num1, num2):
            return True
        return False

    def is_similar_name(self, name1, name2):
        if (name1 and not name2) or (not name1 and name2):
            return False
        if not name1 and not name2:
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
            raise nx.NetworkXError('No angle. edge=(%s, %s)' % in_edge)
            return None
        if out_angle is None:
            raise nx.NetworkXError('No angle. edge=(%s, %s)'
                                   % out_edge)
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

    def get_enter_poi_name(self, node_id):
        data = self.node[node_id]
        return data.get(HWY_ENTER_POI_NAME)

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
            for path, facilcls in self._all_facil_path(node, out_node,
                                                       road_code, code_field,
                                                       reverse=True):
                if(facilcls in (HWY_IC_TYPE_JCT,
                                HWY_IC_TYPE_UTURN,
                                HWY_IC_TYPE_SERVICE_ROAD
                                ) and
                   (self.all_is_inner_link(path, reverse=True) or
                    self.exit_nwy_inner_link(path, reverse=True))
                   ):
                    continue
                if(facilcls in HWY_IC_TYPE_SAPA and
                   self.sapa_inout_on_one_line(path)):
                    continue
                types.add((facilcls, HWY_INOUT_TYPE_IN))
        if len(out_nodes) > 1:
            raise nx.NetworkXError('Multi Out Main Links. node=%s ' % node)
        # 出口/分歧
        in_nodes = self.get_main_link(node, road_code, code_field,
                                      same_code=True, reverse=True)
        if in_nodes and len(in_nodes) == 1:
            in_node = in_nodes[0]
            for path, facilcls in self._all_facil_path(in_node, node,
                                                       road_code, code_field,
                                                       reverse=False):
                if(facilcls in (HWY_IC_TYPE_JCT,
                                HWY_IC_TYPE_UTURN,
                                HWY_IC_TYPE_SERVICE_ROAD
                                ) and
                   (self.all_is_inner_link(path, reverse=False) or
                    self.exit_nwy_inner_link(path, reverse=False))
                   ):
                    continue
                if(facilcls in HWY_IC_TYPE_SAPA and
                   self.sapa_inout_on_one_line(path)):
                    continue
                types.add((facilcls, HWY_INOUT_TYPE_OUT))
        if len(in_nodes) > 1:
            raise nx.NetworkXError('Multi In Main Links. node=%s ' % node)
        return types

    def sapa_inout_on_one_line(self, path):
        '''SAPA出入口同条线上。'''
        first_link = path[0:2]
        last_link = path[-2:]
        last_link.reverse()
        if first_link == last_link:
            return True
        first_link.reverse()
        first_link = tuple(first_link)
        last_link = tuple(last_link)
        all_links = zip(path[0:], path[1:])
        if first_link in all_links[1:]:
            return True
        if last_link in all_links[:-1]:
            return True
        return False

    def get_all_facil(self, node, road_code, code_field=HWY_ROAD_CODE,
                      cutoff=MAX_CUT_OFF):
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
            for path, facilcls in self._all_facil_path(node, out_node,
                                                       road_code, code_field,
                                                       reverse=True,
                                                       cutoff=cutoff):
                if facilcls != HWY_IC_TYPE_VIRTUAl_JCT:
                    all_facils.append((facilcls, HWY_INOUT_TYPE_IN, path))
        if len(out_nodes) > 1:
            raise nx.NetworkXError('Multi Out Main Links. node=%s ' % node)
        # 出口/分歧
        in_nodes = self.get_main_link(node, road_code, code_field,
                                      same_code=True, reverse=True)
        if in_nodes and len(in_nodes) == 1:
            in_node = in_nodes[0]
            for path, facilcls in self._all_facil_path(in_node, node,
                                                       road_code, code_field,
                                                       reverse=False,
                                                       cutoff=cutoff):
                if facilcls != HWY_IC_TYPE_VIRTUAl_JCT:
                    all_facils.append((facilcls, HWY_INOUT_TYPE_OUT, path))
        if len(in_nodes) > 1:
            raise nx.NetworkXError('Multi In Main Links. node=%s ' % node)
        return all_facils

    def get_out_facil(self, node_id, road_code, code_field=HWY_ROAD_CODE):
        '''出口/分歧设施'''
        pass

    def is_hwy_inout(self, path, reverse=False, code_field=HWY_ROAD_CODE):
        '''高速(Ramp)和一般道、高速双向路的交汇点。
           reverse: False,顺车流；True,逆车流
        '''
        inner_num = 0
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
            link_type = data.get(HWY_LINK_TYPE)
            road_type = data.get(HWY_ROAD_TYPE)
            if(link_type != HWY_LINK_TYPE_SAPA and
               road_type not in HWY_ROAD_TYPE_HWY and
               not self.is_normal_inner_link(data)):
                return True
            if self.is_normal_inner_link(data):
                # 折返
                if reverse:
                    if temp_u == path[-2]:
                        continue
                else:
                    if temp_v == path[-2]:
                        continue
                # 非一进一出
                if(len(path) > 2 and
                   not self._one_in_one_out(path[-1], code_field) and
                   not self.get_main_link(path[-1], None, code_field,
                                          reverse=reverse)  # 该点不在本线上
                   ):
                    return True
            if(road_type in HWY_ROAD_TYPE_HWY and  # 高速
               link_type not in (HWY_LINK_TYPE_INNER,
                                 HWY_LINK_TYPE_RTURN,
                                 HWY_LINK_TYPE_LTURN,
                                 HWY_LINK_TYPE_SAPA,
                                 HWY_LINK_TYPE_RAMP,
                                 HWY_LINK_TYPE_JCT) and
               self.has_edge(temp_v, temp_u)):  # 双向道路
                return True
        if inner_num >= 2:
            return True
        return False

    def _bigger_inout_min_angle(self, angle):
        if angle < ANGLE_80 or angle > ANGLE_280:
            return False
        return True

    def bigger_hwy_main_min_angle(self, angle):
        if angle < ANGLE_60 or angle > (ANGLE_360 - ANGLE_60):
            return False
        return True

    def check_angle(self, angle, margin_angle=0):
        '''判定角度范围'''
        if not margin_angle:
            return False
        if angle < margin_angle or angle > ANGLE_360 - margin_angle:
            return True
        return False

    def is_straight(self, angle):
        if angle < ANGLE_10 or angle > ANGLE_360 - ANGLE_10:
            return True
        return False

    def is_jct(self, path, road_code, code_field=HWY_ROAD_CODE, reverse=False):
        '''JCT分歧、合流'''
        if reverse:
            edges_iter = self.in_edges_iter(path[-1], True)
        else:
            edges_iter = self.out_edges_iter(path[-1], True)
        for u, v, data in edges_iter:
            temp_route_code = data.get(code_field)
            if reverse:  # 逆
                f_link = (path[1], path[0])
                t_link = (u, path[-1])
            else:
                f_link = (path[0], path[1])
                t_link = (path[-1], v)
            if(temp_route_code and
               (temp_route_code != road_code or
                not self.is_same_updown(f_link, t_link))
               ):
                if reverse:  # 逆
                    temp_path = path + [u]
                else:  # 顺
                    temp_path = path + [v]
                if self.check_regulation(temp_path, reverse):
                    return True
                else:  # 路被规制
                    continue
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

    def get_uturns(self, path, road_code,
                   code_field=HWY_ROAD_CODE, reverse=False):
        if reverse:
            in_edge = (path[1], path[0])
        else:
            in_edge = (path[0], path[1])
        node = path[-1]
        main_nodes = self.get_main_link(node, road_code, code_field,
                                        False, reverse)
        if main_nodes:
            for e_v in main_nodes:
                if not reverse:
                    out_edge = (node, e_v)
                else:
                    out_edge = (e_v, node)
                if self._is_uturn(in_edge, out_edge, reverse):
                    yield e_v, True  # U-turn
                else:
                    yield e_v, False  # JCT

    def _is_uturn(self, in_edge, out_edge, reverse):
        # ## 名称相似，且进入本线和退出本线夹角接近360左右
        if set(in_edge) & set(out_edge):  # 相交
            return False
        data1 = self[in_edge[0]][in_edge[1]]
        data2 = self[out_edge[0]][out_edge[1]]
        similar_degree = self._get_similar_degree(data1, data2)
        if similar_degree > SIMILAR_DEGREE_NONE:
            angle = self.get_angle(in_edge, out_edge, reverse)
            if self.is_uturn_angle(angle):
                return True
        name1 = data1.get('names')
        name2 = data2.get('names')
        # 名称相同， 角度比较大的
        if name1 and name1 == name2:
            pass
            # print 'Not UTurn:', in_edge, out_edge, self.get_angle(in_edge, out_edge, reverse)
        return False

    def is_uturn_angle(self, angle, default_angle=ANGLE_30):
        # 小于30度/大于330度
        if angle < default_angle or angle > ANGLE_360 - default_angle:
            return True
        return False

    def is_same_road_code(self, path, road_code,
                          code_field=HWY_ROAD_CODE, reverse=False):
        nodes = self.get_main_link(path[-1], road_code, code_field,
                                   same_code=True, reverse=reverse)
        for node in nodes:
            if reverse:  # 逆
                f_link = path[1], path[0]
                t_link = node, path[-1]
            else:  # 顺
                f_link = path[0], path[1]
                t_link = path[-1], node
            if not self.is_same_updown(f_link, t_link):
                continue
            if self.check_regulation(path + [node], reverse):
                return True
            else:  # 路被规制
                continue
        return False

    def is_same_updown(self, f_link, t_link):
        f_u, f_v = f_link
        t_u, t_v = t_link
        f_updown = self._get_road_updown(f_u, f_v)
        t_updown = self._get_road_updown(t_u, t_v)
        if f_updown == t_updown:  # 注两都是空，也返回True
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

    def is_sapa_path(self, path, road_code=HWY_ROAD_CODE,
                     code_field=None, reverse=False):
        if not reverse:  # 正
            temp_path = path
        else:  # 逆
            temp_path = path[::-1]
        for u, v in zip(temp_path[:-1], temp_path[1:]):
            if self.is_sapa_link(u, v):
                return True
        return False
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

    def _is_both_dir_path(self, path, reverse):
        '''每条link都是双通行的'''
        if not reverse:  # 正
            temp_path = path
        else:  # 逆
            temp_path = path[::-1]
        for u, v in zip(temp_path[:-1], temp_path[1:]):
            if not self.has_edge(v, u):
                return False
        return True

    def _all_facil_path(self, u, v, road_code,
                        code_field=HWY_ROAD_CODE,
                        reverse=False, cutoff=MAX_CUT_OFF):
        ''''''
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
        # if source == 235629139:
        #    pass
        if self.is_hwy_inout(visited, reverse):  # 本线直接和一般道直接相连
            yield visited[1:], HWY_IC_TYPE_IC
        if self.is_jct(visited, road_code, code_field, reverse):  # 本线和本线直接相连
            for uturn_info in self.get_uturns(visited, road_code,
                                              code_field, reverse):
                uturn_flg = uturn_info[-1]
                if uturn_flg:
                    yield visited[1:], HWY_IC_TYPE_UTURN
                else:
                    yield visited[1:], HWY_IC_TYPE_JCT
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
            elif temp_path.count(child) > MAX_COUNT:
                continue
            elif not self.check_regulation(temp_path, reverse):
                continue
            elif child == visited[1]:
                path = visited + [child]
                if self.is_sapa_path(temp_path, road_code,
                                     code_field, reverse):
                    yield path[1:], HWY_IC_TYPE_PA
                    exist_sapa_facil = True
                continue
            # JCT之间有两个收费站(100.68357,13.64434)
            # elif self.get_tollgate_num(temp_path) >= MAX_TOLLGATE_NUM:
            #    continue
            elif len(visited) <= cutoff2:
                # 取得link
                if reverse:  # 逆
                    out_edge = (child, visited[-1])
                else:  # 顺
                    out_edge = (visited[-1], child)
                out_data = self[out_edge[0]][out_edge[1]]
                # 本线和SAPA link直接相连
                if(len(visited) == 2 and
                   self.is_sapa_link(out_edge[0], out_edge[1])):
                    # not self.has_edge(out_edge[1], out_edge[0])):
                    sapa_link = True
                if(len(visited) > 2 and
                   self.is_normal_inner_link(out_data) and  # 一般道Inner Link
                   not self._one_in_one_out(visited[-1], code_field)):
                    # 即, 当前是Inner link，起点有多条相连link(非一进一出)
                    if not self.is_hwy_inout(temp_path[1:-1], reverse):
                        # 前个点已经是出入口，不再收录(重复)
                        yield temp_path[1:-1], HWY_IC_TYPE_IC
                    continue
                elif self.is_jct(temp_path, road_code, code_field, reverse):
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
                    if self.get_tollgate_num(temp_path) < MAX_TOLLGATE_NUM:
                        if self.is_sapa_path(temp_path, road_code,
                                             code_field, reverse):
                            yield temp_path[1:], HWY_IC_TYPE_PA
                            exist_sapa_facil = True
                        else:
                            # 辅路、类辅路设施
                            yield temp_path[1:], HWY_IC_TYPE_SERVICE_ROAD
                    else:
                        pass
                        # print 'Tollgate Number > 1. path=%s' % temp_path
                # 和一般道交汇
                if self.is_hwy_inout(temp_path, reverse):
                    yield temp_path[1:], HWY_IC_TYPE_IC
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
                # raise nx.NetworkXError('Exist SAPA Link, but no SAPA Facility.'
                #                         'u=%s,v=%s' % (u, v))

    def _all_sapa_path(self, u, v, road_code,
                       code_field=HWY_ROAD_CODE,
                       reverse=False, cutoff=MAX_CUT_OFF/2):
        # 从SAPA点找到另外一个SAPA点(_all_facil_path2找不到路的情况下，才使用)
        if cutoff < 1:
            return
        if reverse:  # 逆
            visited = [v, u]
            stack = [self.predecessors_iter(u)]
        else:  # 顺
            visited = [u, v]
            stack = [iter(self[v])]
        while stack:
            children = stack[-1]
            child = next(children, None)
            temp_path = visited + [child]
            if child is None:
                stack.pop()
                visited.pop()
            elif not self.check_regulation(temp_path, reverse):
                continue
            elif child == visited[-2]:
                continue
            elif len(visited) < cutoff:
                if self.is_same_road_code(temp_path, road_code,  # 回到当前线路
                                          code_field, reverse):
                    yield temp_path[1:]
                else:
                    # ## 高速本线
                    if reverse:  # 逆
                        data = self[child][visited[-1]]
                    else:  # 顺
                        data = self[visited[-1]][child]
                    temp_road_code = data.get(code_field)
                    if temp_road_code:
                        continue
                    visited.append(child)
                    if reverse:  # 逆
                        stack.append(self.predecessors_iter(child))
                    else:  # 顺
                        stack.append(iter(self[child]))
            else:
                if self.is_same_road_code(temp_path, road_code,  # 回到当前线路
                                          code_field, reverse):
                    yield temp_path[1:]
                stack.pop()
                visited.pop()

    def _is_both_dir_sapa(self, u, v, reverse=False):
        if reverse:  # 逆
            if self.is_sapa_link(v, u) and self.has_edge(u, v):
                return True
        else:  # 顺
            if self.is_sapa_link(u, v) and self.has_edge(v, u):
                return True
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
            link_type = data.get(HWY_LINK_TYPE)
            if(self.has_edge(v, u) and  # # 双向
               link_type not in (HWY_LINK_TYPE_INNER,
                                 HWY_LINK_TYPE_RTURN,
                                 HWY_LINK_TYPE_LTURN,
                                 HWY_LINK_TYPE_SAPA,
                                 HWY_LINK_TYPE_RAMP,
                                 HWY_LINK_TYPE_JCT
                                 )):
                continue
            road_type = data.get(HWY_ROAD_TYPE)
            if road_type in HWY_ROAD_TYPE_HWY:
                if reverse:  # 逆
                    nodes.append(u)
                else:  # 顺
                    nodes.append(v)
            else:
                if(link_type in (HWY_LINK_TYPE_JCT,
                                 HWY_LINK_TYPE_RAMP,
                                 HWY_LINK_TYPE_SAPA) or
                   (self.is_normal_inner_link(data) and not ignore_inner)
                   ):
                    if reverse:  # 逆
                        nodes.append(u)
                    else:  # 顺
                        nodes.append(v)
        return nodes

    def get_main_link(self, node, road_code, code_field=HWY_ROAD_CODE,
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
            if road_code and same_code:
                if temp_road_code == road_code:
                    if reverse:  # 逆
                        nodes.append(u)
                    else:  # 顺
                        nodes.append(v)
            # 线路号不同
            elif road_code and not same_code:
                if temp_road_code != road_code:
                    if reverse:  # 逆
                        nodes.append(u)
                    else:  # 顺
                        nodes.append(v)
            else:   # 所有本线
                if reverse:  # 逆
                    nodes.append(u)
                else:  # 顺
                    nodes.append(v)
        return nodes

    def get_to_main_link_of_path(self, path, road_code,
                                 code_field=HWY_ROAD_CODE, reverse=False):
        main_nodes = []
        last_node = path[-1]
        nodes = self.get_main_link(last_node, road_code, code_field,
                                   False, reverse)
        for node in nodes:
            temp_path = path + [node]
            if self.check_regulation(temp_path, reverse):
                main_nodes.append(node)
        return main_nodes

    def get_normal_link(self, u, v, reverse=False, ignore_inner=False):
        '''高速(Ramp)相连的一般道、。
           reverse: False,顺车流；True,逆车流
        '''
        nodes = []
        if reverse:  # 逆
            edges_iter = self.in_edges_iter(u, True)
        else:  # 顺
            edges_iter = self.out_edges_iter(v, True)
        # in_edge = (u, v)
        for temp_u, temp_v, data in edges_iter:
            # out_edge = temp_u, temp_v
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
               road_type not in HWY_ROAD_TYPE_HWY and
               not (ignore_inner and self.is_normal_inner_link(data))  # 非inner
               ):
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

    def check_regulation(self, path, reverse=False):
        '''True: 路没被规制
           False: 路被规制
        '''
        if reverse:  # 逆
            data = self.get_edge_data(path[-1], path[-2])
        else:  # 顺
            data = self.get_edge_data(path[-2], path[-1])
        regulation_list = data.get(HWY_REGULATION)
        if regulation_list:
            for node_list in regulation_list:
                node_cnt = -1
                if len(node_list) > len(path):
                    continue
                match_flag = True
                while node_cnt + len(node_list) >= 0:
                    if node_list[node_cnt] != path[node_cnt]:
                        match_flag = False
                        break
                    node_cnt -= 1
                if match_flag:
                    return False  # 路被规制
        return True

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

    def _one_in_one_out(self, node, code_field=HWY_ROAD_CODE):
        '''点只有两条(不抱包括高速本线)相连link(一进一出)'''
        out_nodes = []
        in_nodes = []
        for node, v, data in self.out_edges_iter(node, True):
            if not data.get(code_field):  # 本线
                out_nodes.append(v)
        for u, node, data in self.in_edges_iter(node, True):
            if not data.get(code_field):  # 本线
                in_nodes.append(u)
        if(len(out_nodes) in (1, 2) and
           len(in_nodes) in (1, 2)):
            if len(set(out_nodes + in_nodes)) == 2:
                return True
        return False

    def get_tollgate_num(self, path):
        tollgate = set()
        for node in path[1:-1]:
            if self.is_tollgate(node):
                tollgate.add(node)
        return len(tollgate)

    def all_path_2_hwy_main(self, path, code_field=HWY_ROAD_CODE,
                            reverse=False, cutoff=25):
        '''通往高速本线的路径'''
        MAX_COUNT = 2
        if cutoff < 1:
            return
        source = path[-1]
        visited = path[:]
        if source not in self:
            return
        nodes = self._get_not_main_link(visited[-1], code_field, reverse, True)
        if not nodes:
            # 直接和高速本线相连
            main_node = self.get_main_link(visited[-1], None, code_field,
                                           False, reverse)
            for node in main_node:
                if self.check_regulation(visited + [node], reverse):
                    yield visited + [node]
            return
        stack = [iter(nodes)]
        while stack:
            children = stack[-1]
            child = next(children, None)
            temp_path = visited + [child]
            if child is None:
                stack.pop()
                visited.pop()
            elif(len(visited) >= 2 and child == visited[-2]):  # 折返
                # 即 不是断头路的最后一个点, 不能折返
                continue
            elif temp_path.count(child) + 1 > MAX_COUNT:
                continue
            elif not self.check_regulation(temp_path, reverse):
                continue
            # elif child == visited[1]:
            #    continue
            elif len(visited) <= cutoff:
                if len(visited) < cutoff:
                    main_node = self.get_main_link(child, None, code_field,
                                                   False, reverse)
                    for node in main_node:
                        if self.check_regulation(temp_path + [node], reverse):
                            yield temp_path + [node]
                    # 取得非本线
                    nodes = self._get_not_main_link(child, code_field,
                                                    reverse, True)
                    if nodes:
                        visited.append(child)
                        stack.append(iter(nodes))
                elif len(visited) == cutoff:
                    stack.pop()
                    visited.pop()

    def all_path_2_inout(self, path, code_field=HWY_ROAD_CODE,
                         reverse=False, cutoff=25):
        '''通往一般道的路径'''
        if cutoff < 1:
            return
        source = path[-1]
        visited = path[:]
        if source not in self:
            return
        if self.is_hwy_inout(path, reverse):
            yield path
        nodes = self._get_not_main_link(visited[-1], code_field, reverse, True)
        if not nodes:
            # 直接和高速本线相连
#             main_node = self.get_main_link(visited[-1], None, code_field,
#                                            False, reverse)
#             for node in main_node:
#                 if self.check_regulation(visited + [node], reverse):
#                     yield visited + [node]
            return
        stack = [iter(nodes)]
        while stack:
            children = stack[-1]
            child = next(children, None)
            temp_path = visited + [child]
            if child is None:
                stack.pop()
                visited.pop()
            elif(len(visited) >= 2 and child == visited[-2]):  # 折返
                # 即 不是断头路的最后一个点, 不能折返
                continue
            elif not self.check_regulation(temp_path, reverse):
                continue
            # elif self.is_jct(temp_path, None, code_field, reverse):  # 到本线
            #    continue
            elif len(visited) <= cutoff:
                if len(visited) < cutoff:
                    # 和一般道交汇
                    if self.is_hwy_inout(temp_path, reverse):
                        yield temp_path
                        continue
                    # 取得非本线
                    nodes = self._get_not_main_link(child, code_field,
                                                    reverse, True)
                    if nodes:
                        visited.append(child)
                        stack.append(iter(nodes))
                    else:
                        # 和一般道相连
                        if reverse:  # 逆
                            u = temp_path[-1]
                            v = temp_path[-2]
                        else:  # 顺
                            u = temp_path[-2]
                            v = temp_path[-1]
                        if self.get_normal_link(u, v, reverse):
                            yield temp_path
                elif len(visited) == cutoff:
                    stack.pop()
                    visited.pop()

    def get_next_street_path(self, path, reverse=False):
        u, v = path[-2], path[-1]
        visited = path[:]
        ignore_inner = True
        # 取得一般道
        normal_nodes = self.get_normal_link(u, v, reverse, ignore_inner)
        if normal_nodes:
            for node in normal_nodes:
                temp_path = visited + [node]
                if self.check_regulation(temp_path):
                    yield temp_path
        # 取得inner link
        nodes = self._get_normal_inner_link(u, v, reverse)
        if not nodes:
            return
        stack = [iter(nodes)]
        while stack:
            children = stack[-1]
            child = next(children, None)
            if child is None:
                stack.pop()
                visited.pop()
                continue
            # if child == visited[-2]:  # 折返
            #    continue
            if child in visited:
                continue
            temp_path = visited + [child]
            if not self.check_regulation(temp_path):
                continue
            # 取得一般道
            normal_nodes = self.get_normal_link(visited[-1], child,
                                                reverse, ignore_inner)
            if normal_nodes:
                for node in normal_nodes:
                    if self.check_regulation(temp_path):
                        yield temp_path + [node]
            # 取得inner link
            nodes = self._get_normal_inner_link(visited[-1], child,
                                                reverse)
            if nodes:
                visited.append(child)
                stack.append(iter(nodes))

    def _get_normal_inner_link(self, u, v, reverse):
        '''高速(Ramp)相连的一般道、。
           reverse: False,顺车流；True,逆车流
        '''
        nodes = []
        if reverse:  # 逆
            edges_iter = self.in_edges_iter(u, True)
        else:  # 顺
            edges_iter = self.out_edges_iter(v, True)
        for temp_u, temp_v, data in edges_iter:
            if self.is_normal_inner_link(data):
                if reverse:  # 逆
                    nodes.append(temp_u)
                else:  # 顺
                    nodes.append(temp_v)
        return nodes

    def exist_ic_link(self, path, reverse=False):
        '''Ramp/JCT/SAPA Link'''
        if len(path) < 2:
            return False
        if reverse:
            path = path[::-1]
            link_list = (zip(path[:-1], path[1:]))[::-1]
        else:
            link_list = zip(path[:-1], path[1:])
        has_sapa_link = False
        for u, v in link_list:
            data = self[u][v]
            link_type = data.get(HWY_LINK_TYPE)
            road_type = data.get(HWY_ROAD_TYPE)
            if link_type == HWY_LINK_TYPE_SAPA:
                if(road_type == HWY_ROAD_TYPE_HWY0 and
                   not self.has_edge(v, u)):
                    # 前面经过普通/双向的SAPA
                    if has_sapa_link:
                        has_sapa_link = True
                        # print 'Normal SAPA:', path
                        continue
                    else:
                        # self.log.info('%s' % path)
                        return True
                else:
                    has_sapa_link = True
                    continue
            if self.has_edge(v, u):  # 双向
                if has_sapa_link:
                    # 先经过SAPA link，再到到双向 link (Ramp link)
                    pass
                else:
                    continue
            if link_type in (HWY_LINK_TYPE_RAMP,
                             HWY_LINK_TYPE_JCT,):
                # 先经过一般道SAPA link/高速双向SAPA link
                if has_sapa_link:
                    # print path
                    pass
                else:
                    return True
        return False

    def exist_ramp_link(self, path, reverse=False):
        '''Ramp/JCT/SAPA Link'''
        if len(path) < 2:
            return False
        if reverse:
            path = path[::-1]
        for u, v in zip(path[:-1], path[1:]):
            data = self[u][v]
            link_type = data.get(HWY_LINK_TYPE)
            if link_type in (HWY_LINK_TYPE_RAMP,):
                return True
        return False

    def exist_hwy_main_link(self, path, reverse=False):
        if len(path) < 2:
            return False
        if reverse:
            path = path[::-1]
        for u, v in zip(path[:-1], path[1:]):
            data = self[u][v]
            road_type = data.get(HWY_ROAD_TYPE)
            link_type = data.get(HWY_LINK_TYPE)
            if(road_type in HWY_ROAD_TYPE_HWY and
               link_type in HWY_LINK_TYPE_MAIN):
                return True
        return False

    def first_is_inner_link(self, path, reverse=False):
        '''第一条link是一般道 Inner link.'''
        if len(path) < 2:
            return False
        if reverse:
            path = path[::-1]
        u, v = path[0], path[1]
        data = self[u][v]
        if self.is_normal_inner_link(data):
            return True
        return False

    def all_is_inner_link(self, path, reverse=False):
        '''所有link都是 Inner link.'''
        if len(path) < 2:
            return False
        if reverse:
            path = path[::-1]
        for u, v in zip(path[:-1], path[1:]):
            data = self[u][v]
            lint_type = data.get(HWY_LINK_TYPE)
            if lint_type not in (HWY_LINK_TYPE_INNER,
                                 HWY_LINK_TYPE_RTURN,
                                 HWY_LINK_TYPE_LTURN):
                return False
            road_type = data.get(HWY_ROAD_TYPE)
            if(road_type in HWY_ROAD_TYPE_HWY and
               not self.has_edge(v, u)):  # 单向
                return False
        return True

    def exit_nwy_inner_link(self, path, reverse=False):
        '''有一般道inner link'''
        if len(path) < 2:
            return False
        if reverse:
            path = path[::-1]
        for u, v in zip(path[:-1], path[1:]):
            data = self[u][v]
            if self.is_normal_inner_link(data):
                return True
        return False

    def get_next_links_for_path(self, path, reverse=False):
        node = path[-1]
        next_nodes = []
        if reverse:  # 逆
            edges_iter = self.in_edges_iter(node, False)
        else:  # 顺
            edges_iter = self.out_edges_iter(node, False)
        for temp_u, temp_v in edges_iter:
            if reverse:  # 逆
                next_node = temp_u
            else:
                next_node = temp_v
            if path[-2] == temp_v:  # 折返了
                continue
            temp_path = path + [next_node]
            if not self.check_regulation(temp_path, reverse):
                continue
            next_nodes.append(next_node)
        return next_nodes

    def is_cycle(self, nbunch):
        sub = self.subgraph(nbunch)
        for cycle in nx.simple_cycles(sub):
            if set(cycle) == set(nbunch):
                del sub
                return True
        del sub
        return False

    def is_urban_link(self, u, v):
        '''都市高速link
        '''
        if self.has_edge(u, v):
            data = self[u][v]
        elif self.has_edge(v, u):
            data = self[v][u]
        else:
            return False
        road_type = data.get(HWY_ROAD_TYPE)
        # 都市高速
        if(road_type == HWY_ROAD_TYPE_HWY1):
            return True
        return False


# =============================================================================
#
# =============================================================================
def is_cycle_path(path):
    '''线路是环型的'''
    if len(path) > 2 and path[0] == path[-1]:
        return True
    else:
        return False
