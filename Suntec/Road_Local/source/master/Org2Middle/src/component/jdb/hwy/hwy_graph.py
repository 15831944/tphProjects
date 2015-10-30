# -*- coding: UTF-8 -*-
'''
Created on 2014-3-25

@author: hongchenzai
'''
import networkx as nx
from component.jdb.hwy.hwy_def import IC_TYPE_IC
from component.jdb.hwy.hwy_def import IC_TYPE_RAMP
from component.jdb.hwy.hwy_def import IC_TYPE_JCT
from component.jdb.hwy.hwy_def import IC_TYPE_SA
from component.jdb.hwy.hwy_def import IC_TYPE_PA
from component.jdb.hwy.hwy_def import IC_TYPE_TOLL
from component.jdb.hwy.hwy_def import INOUT_TYPE_IN
from component.jdb.hwy.hwy_def import INOUT_TYPE_OUT
from component.jdb.hwy.hwy_def import ROAD_CODE_NO_TOLL
# link type
LINK_TYPE_MAIN = 1
LINK_TYPE_MAIN_SEPARATION = 2
LINK_TYPE_INNER = 4
LINK_TYPE_SR = 6  # 辅路
ROAD_TYPE_NARROW_ROAD2 = 9  # 细街路2
ONE_WAY_BOTH = 1  # 双方
ONE_WAY_POSITIVE = 2  # 正方向
ONE_WAY_RERVERSE = 3  # 逆方向
ONE_WAY_PROHIBITION = 4  # 禁止
MAX_CUT_OFF = 30
MAX_CUT_OFF_CHN = 40
MIN_CUT_OFF_CHN = 23
MIN_CUT_OFF = 20
MAX_CUT_OFF_NOTOLL = 20  # 无料区间


#==============================================================================
# HwyGraph
#==============================================================================
class HwyGraph(nx.DiGraph):
    '''Highway图
    '''

    def __init__(self, data=None, **attr):
        '''
        Constructor
        '''
        nx.DiGraph.__init__(self, data, **attr)

    def all_paths_2_out(self, source, cutoff=MAX_CUT_OFF):
        return list(self.__all_paths_2_out(source, cutoff))

    def __all_paths_2_out(self, source, cutoff=MAX_CUT_OFF):
        '''到高速出口(一般道)'''
        if cutoff < 1:
            return
        if source not in self:
            return
        if self.is_hwy_inout(source) or self.is_on_not_hwy_main(source):
            return
        visited = [source]
        stack = [iter(self[source])]
        while stack:
            children = stack[-1]
            child = next(children, None)
            if child is None:
                stack.pop()
                visited.pop()
            elif len(visited) < cutoff:
                if self.is_hwy_inout(child):
                    yield visited + [child]
                elif self.is_on_not_hwy_main(child):
                    yield visited + [child]
                elif self.is_hwy_node(child):
                    pass
                else:
                    visited.append(child)
                    stack.append(iter(self[child]))
            else:  # len(visited) == cutoff:
                if self.is_hwy_inout(child):
                    yield visited + [child]
                stack.pop()
                visited.pop()

    def all_paths_2_in(self, source, cutoff=MAX_CUT_OFF):
        return list(self.__all_paths_2_in(source, cutoff))

    def __all_paths_2_in(self, source, cutoff=MAX_CUT_OFF):
        '''到高速入口(一般道)'''
        if cutoff < 1:
            return
        if source not in self:
            return
        if self.is_hwy_inout(source) or self.is_on_not_hwy_main(source):
            return
        visited = [source]
        stack = [self.predecessors_iter(source)]
        while stack:
            parents = stack[-1]
            parent = next(parents, None)
            if parent is None:
                stack.pop()
                visited.pop()
            elif len(visited) < cutoff:
                if self.is_hwy_inout(parent):
                    yield visited + [parent]
                elif self.is_on_not_hwy_main(parent):
                    yield visited + [parent]
                elif self.is_hwy_node(parent):
                    pass
                else:
                    visited.append(parent)
                    stack.append(self.predecessors_iter(parent))
            else:  # len(visited) == cutoff:
                if self.is_hwy_inout(parent):
                    yield visited + [parent]
                stack.pop()
                visited.pop()

    def all_paths_ic_out(self, source, target, cutoff=MAX_CUT_OFF):
        '''IC ==> IC Position.'''
        if source == target or target not in self:
            return self.all_paths_2_out(source)
        pathes = []
        for path in self._all_hwy_path(source, target, cutoff):
            del_flag = False
            for nodeid in path[1:-1]:
                if self.is_hwy_node(nodeid):
                    del_flag = True
                    break
            if not del_flag:
                pathes.append(path)
        #=========================================
        if not pathes:
            for path in all_paths(self, source, target):
                del_flag = False
                for nodeid in path[1:-1]:
                    if self.is_ic_inout(nodeid):
                        del_flag = True
                        break
                    if self.is_sapa(nodeid):
                        del_flag = True
                        break
                if not del_flag:
                    pathes.append(path)
        return pathes

    def all_paths_ic_in(self, source, target, cutoff=MAX_CUT_OFF):
        '''IC ==> IC Position.'''
        # IC设施所在点无 or 设施所在点有hwynode重合
        if source == target or target not in self:
            return self.all_paths_2_in(source)
        pathes = []
        for path in self._all_hwy_paths_reverse(source, target, cutoff):
            del_flag = False
            for nodeid in path[1:-1]:
                if self.is_hwy_node(nodeid):
                    del_flag = True
                    break
            if not del_flag:
                pathes.append(path)
        # =========================================
        if not pathes:
            for path in all_paths_reverse(self, source, target):
                del_flag = False
                for nodeid in path[1:-1]:
                    if self.is_ic_inout(nodeid):
                        del_flag = True
                        break
                    if self.is_sapa(nodeid):
                        del_flag = True
                        break
                if not del_flag:
                    pathes.append(path)
        return pathes

    def all_paths_jct(self, source, target, cutoff=MAX_CUT_OFF):
        '''JCT-Out To JCT-In Pathes'''
        pathes = []
        # for path in nx.all_simple_paths(self, source, target, cutoff):
        for path in self._all_hwy_path(source, target, cutoff):
            del_flag = False
            for nodeid in path[1:-1]:
                if(self.is_hwy_inout(nodeid) or
                   self.is_on_not_hwy_main(nodeid)):
                    del_flag = True
                    break
                if self.is_jct_inout(nodeid):
                    del_flag = True
                    break
            if del_flag:
                continue
            pathes.append(path)
        return pathes

    def all_paths_sapa(self, source, target, cutoff=MAX_CUT_OFF):
        ''''''
        pathes = []
        for path in all_paths(self, source, target, cutoff):
            del_flag = False
            for nodeid in path[1:-1]:
                if (self.is_hwy_inout(nodeid) or
                    self.is_on_not_hwy_main(nodeid)):
                    del_flag = True
                    break
                if self.is_hwy_node(nodeid):
                    del_flag = True
                    break
            if del_flag:
                continue
            pathes.append(path)
        return pathes

    def all_paths_2_sapa_in(self, source, cutoff=40):
        return list(self.__all_paths_2_sapa_in(source, cutoff))

    def __all_paths_2_sapa_in(self, source, cutoff=40):
        '''到SA入口'''
        if cutoff < 1:
            return
        if source not in self:
            return
        visited = [source]
        stack = [iter(self[source])]
        while stack:
            children = stack[-1]
            child = next(children, None)
            if child is None:
                stack.pop()
                visited.pop()
            elif len(visited) < cutoff:
                if self.is_sapa_in(child):
                    if self.check_updown(source, child):
                        yield visited + [child]
                    else:
                        pass
                elif self.is_ic_position(child):
                    pass
                elif self.is_on_not_hwy_main(child):
                    pass
                elif self.is_hwy_node(child):
                    pass
                else:
                    visited.append(child)
                    stack.append(iter(self[child]))
            else:  # len(visited) == cutoff:
                if self.is_sapa_in(child) and self.check_updown(source, child):
                    yield visited + [child]
                stack.pop()
                visited.pop()

    def all_paths_uturn(self, source, target, cutoff=40):
        return list(self.__all_paths_uturn(source, target, cutoff))

    def __all_paths_uturn(self, source, target, cutoff=40):
        '''U-turn Pathes'''
        if cutoff < 1:
            return
        visited = [source]
        stack = [iter(self[source])]
        while stack:
            children = stack[-1]
            child = next(children, None)
            if child is None:
                stack.pop()
                visited.pop()
            elif len(visited) < cutoff:
                if child == target:
                    yield visited + [target]
                elif self.is_hwy_inout(child):
                    pass
                elif self.is_on_not_hwy_main(child):
                    pass
                elif self.is_hwy_node(child):
                    pass
                else:
                    visited.append(child)
                    stack.append(iter(self[child]))
            else:  # len(visited) == cutoff:
                if child == target or target in children:
                    yield visited + [target]
                stack.pop()
                visited.pop()

    def get_inlinkids(self, nodeid):
        inlinks = []
        for parent, node in self.in_edges_iter(nodeid):
            inlinks.append(self.get_linkid(parent, node))
        return inlinks

    def get_node_tile(self, nodeid):
        return self.node[nodeid].get('tile_id')

    def get_in_tile(self, nodeid, roadcode=None):
        '''本线node的in tile'''
        in_tiles = []
        for parent, node, data in self.in_edges_iter(nodeid, True):
            tile_id = data.get('tile_id')
            if not tile_id:
                continue
            if roadcode:
                if data.get('road_code') == roadcode:
                    in_tiles.append(tile_id)
            else:
                in_tiles.append(tile_id)
        return in_tiles

    def get_out_tile(self, nodeid, roadcode=None):
        '''本线node的out tile'''
        out_tiles = []
        for node, child, data in self.out_edges_iter(nodeid, True):
            tile_id = data.get('tile_id')
            if not tile_id:
                continue
            if roadcode:
                if data.get('road_code') == roadcode:
                    out_tiles.append(tile_id)
            else:
                out_tiles.append(tile_id)
        return out_tiles

    def get_out_link_tile(self, nodeid, outlinkid):
        if nodeid not in self:
            return None
        for node, child, data in self.out_edges_iter(nodeid, True):
            tile_id = data.get('tile_id')
            if not tile_id:
                continue
            if data.get('link_id') == outlinkid:
                return tile_id
        return None

    def get_in_link_tile(self, nodeid, inlinkid):
        if nodeid not in self:
            return None
        for prant, node, data in self.in_edges_iter(nodeid, True):
            tile_id = data.get('tile_id')
            if not tile_id:
                continue
            if data.get('link_id') == inlinkid:
                return tile_id
        return None

    def get_main_inlinkids(self, node, roadcode=None):
        inlink = []
        for parent, node, data in self.in_edges_iter(node, True):
            if roadcode:
                if data.get('road_code') == roadcode:
                    inlink.append(self.get_linkid(parent, node))
            else:
                if (data.get('link_type') == LINK_TYPE_MAIN_SEPARATION
                    and data.get('hwymode_f')):
                    inlink.append(self.get_linkid(parent, node))
        return inlink

    def get_in_angle(self, node, roadcode=None):
        '''进入link的延长线和水平正东的夹角'''
        for u, v, data in self.in_edges_iter(node, True):
            if roadcode:
                if data.get('road_code') == roadcode:
                    if data.get('one_way') == ONE_WAY_POSITIVE:
                        return self._convert_2_in_angle(data.get('e_angle'))
                    elif data.get('one_way') == ONE_WAY_RERVERSE:
                        return self._convert_2_in_angle(data.get('s_angle'))
                    else:
                        raise nx.NetworkXError('Getting In Angle Failed.'
                                               ' node=%s' % node)
            else:
                if (data.get('link_type') == LINK_TYPE_MAIN_SEPARATION
                    and data.get('hwymode_f')):
                    if data.get('one_way') == ONE_WAY_POSITIVE:
                        return self._convert_2_in_angle(data.get('e_angle'))
                    elif data.get('one_way') == ONE_WAY_RERVERSE:
                        return self._convert_2_in_angle(data.get('s_angle'))
                    else:
                        raise nx.NetworkXError('Getting In Angle Failed.'
                                               ' node=%s' % node)

    def _convert_2_in_angle(self, angle):
        degree = self._convert_2_degree(angle)
        degree += 180
        while degree >= 360:
            degree -= 360
        return degree

    def get_out_angle(self, node, roadcode=None):
        '''退出link和水平正东的夹角'''
        for u, v, data in self.out_edges_iter(node, True):
            if roadcode:
                if data.get('road_code') == roadcode:
                    if data.get('one_way') == ONE_WAY_POSITIVE:
                        return self._convert_2_degree(data.get('s_angle'))
                    elif data.get('one_way') == ONE_WAY_RERVERSE:
                        return self._convert_2_degree(data.get('e_angle'))
                    else:
                        raise nx.NetworkXError('Getting out Angle Failed.'
                                               ' node=%s' % node)
            else:
                if (data.get('link_type') == LINK_TYPE_MAIN_SEPARATION
                    and data.get('hwymode_f')):
                    if data.get('one_way') == ONE_WAY_POSITIVE:
                        return self._convert_2_degree(data.get('s_angle'))
                    elif data.get('one_way') == ONE_WAY_RERVERSE:
                        return self._convert_2_degree(data.get('e_angle'))
                    else:
                        raise nx.NetworkXError('Getting out Angle Failed.'
                                               'node=%s' % node)

    def _convert_2_degree(self, angle):
        return (angle + 32768) * 360.0 / 65536

    def get_outlinkids(self, node):
        outlink = []
        for child in self.successors_iter(node):
            outlink.append(self.get_linkid(node, child))
        return outlink

    def get_outlinkids_except_narrow_road(self, node):
        '''取得所有脱 出link(不包括[细街路2])'''
        outlinks = []
        if node not in self:
            return []
        for node, child, data in self.out_edges_iter(node, True):
            if(data.get('road_type') != ROAD_TYPE_NARROW_ROAD2):
                outlinks.append(data.get('link_id'))
        return outlinks

    def get_main_outlinkids(self, node, roadcode=None):
        outlink = []
        if node not in self:
            return []
        for node, child, data in self.out_edges_iter(node, True):
            if roadcode:
                if data.get('road_code') == roadcode:
                    outlink.append(self.get_linkid(node, child))
            else:
                if(data.get('link_type') == LINK_TYPE_MAIN_SEPARATION
                   and data.get('hwymode_f')):
                    outlink.append(self.get_linkid(node, child))
        if not outlink and not roadcode:  # 没有指定roadcode
            # 主要是为road_code是【40万，50万】开头的本线link
            for node, child, data in self.out_edges_iter(node, True):
                if(data.get('link_type') == LINK_TYPE_MAIN_SEPARATION
                   and not data.get('road_code')):
                    outlink.append(self.get_linkid(node, child))
        return outlink

    def get_another_nodeid(self, node, link_id):
        '''取得link另一头的node_id'''
        for node_id, child, data in self.out_edges_iter(node, True):
            if data.get('link_id') == link_id:
                return child
        for parent, node_id, data in self.in_edges_iter(node, True):
            if data.get('link_id') == link_id:
                return parent
        return None

    def get_linkid(self, u, v):
        data = self.get_edge_data(u, v)
        if data:
            return data.get('link_id')
        return None

    def get_linkids(self, path, reverse=False):
        linkids = []
        if reverse:  # 逆
            for u, v in zip(path[0:-1], path[1:]):
                linkid = self.get_linkid(v, u)
                if linkid:
                    linkids.append(linkid)
                else:
                    raise nx.NetworkXError('No linkid for Edge(%s, %s).'
                                           % (u, v))
        else:  # 顺
            for u, v in zip(path[0:-1], path[1:]):
                linkid = self.get_linkid(u, v)
                if linkid:
                    linkids.append(linkid)
                else:
                    raise nx.NetworkXError('No linkid for Edge(%s, %s).'
                                           % (u, v))
        return linkids

    def get_length(self, u, v, key='length'):
        data = self.get_edge_data(u, v)
        if data:
            return data.get(key)

    def get_length_for_path(self, path, key='length'):
        sum_length = 0
        for u, v in zip(path[:-1], path[1:]):
            length = self.get_length(u, v, key)
            if length is None:
                raise nx.NetworkXError('No Distance for edge(%s, %s)' % (u, v))
            sum_length += length
        return sum_length

    def set_inout_flag(self, nodeid, inout):
        self.add_node(nodeid, inout_flag=inout)

    def set_ic_type(self, nodeid, types):
        self.add_node(nodeid, ic_type=types)

    def get_ic_type(self, nodeid):
        ic_types = self.node[nodeid].get('ic_type')
        return ic_types

    def set_updown(self, nodeid, updown_flg):
        self.add_node(nodeid, updown=updown_flg)

    def is_on_not_hwy_main(self, nodeid):
        '''非高速本线'''
        for (node, neighbor, data) in self.out_edges_iter(nodeid, True):
            hwymode_f = data.get('hwymode_f')
            if hwymode_f == 0:  # 非高速收费本线
                return True
            road_code = data.get('road_code')
            if road_code and road_code > ROAD_CODE_NO_TOLL:
                return True

        for (node, neighbor, data) in self.in_edges_iter(nodeid, True):
            hwymode_f = data.get('hwymode_f')
            if hwymode_f == 0:  # 非高速收费本线
                return True
            road_code = data.get('road_code')
            if road_code and road_code > ROAD_CODE_NO_TOLL:
                return True
        return False

    def is_hwy_inout(self, nodeid):
        node = self.node[nodeid]
#         if not node:
#             print nodeid
        if node and node.get('inout_flag'):
            return True
        else:
            return False

    def is_ic_inout(self, nodeid):
        ic_types = self.node[nodeid].get('ic_type')
        if (ic_types and
            (self.check_ic_type(ic_types, IC_TYPE_IC) or
             self.check_ic_type(ic_types, IC_TYPE_RAMP))
            ):
            return True
        else:
            return False

    def is_jct_inout(self, nodeid, inout=None):
        ic_types = self.node[nodeid].get('ic_type')
        if ic_types and self.check_ic_type(ic_types, IC_TYPE_JCT, inout):
            return True
        else:
            return False

    def is_sapa(self, nodeid):
        ic_types = self.node[nodeid].get('ic_type')
        if ic_types and (self.check_ic_type(ic_types, IC_TYPE_SA) or
                         self.check_ic_type(ic_types, IC_TYPE_PA)):
            return True
        else:
            return False

    def is_sapa_in(self, nodeid):
        ic_types = self.node[nodeid].get('ic_type')
        if (ic_types and
            (self.check_ic_type(ic_types, IC_TYPE_SA, INOUT_TYPE_IN)
             or self.check_ic_type(ic_types, IC_TYPE_PA, INOUT_TYPE_IN))
            ):
            return True
        else:
            return False

    def is_hwy_node(self, nodeid):
        ic_types = self.node[nodeid].get('ic_type')
        if ic_types and not self.check_ic_type(ic_types, IC_TYPE_TOLL):
            return True
        else:
            return False

    def is_ic_position(self, nodeid):
        '''IC设施所在点'''
        if self.node[nodeid].get('ic_position'):
            return True
        else:
            return False

    def is_toll(self, nodeid):
        ic_types = self.node[nodeid].get('ic_type')
        if ic_types and self.check_ic_type(ic_types, IC_TYPE_TOLL):
            return True
        else:
            return False

    def check_ic_type(self, ic_types, ic_type, inout=None):
        if inout is not None:
            return (ic_type, inout) in ic_types
        else:
            for temp_type, temp_inout in ic_types:
                if temp_type == ic_type:
                    return True
        return False

    def get_inout_type(self, node):
        ic_types = self.node[node].get('ic_type')
        temp = set()
        for ic_type, inout in ic_types:
            temp.add(inout)
        if len(temp) == 1:
            return temp.pop()
        elif len(temp) > 1:
            raise nx.NetworkXError('More InOut Type. node=%s' % node)

    def check_updown(self, source, target):
        s_updown = self.node[source].get('updown')
        t_updown = self.node[target].get('updown')
        return s_updown == t_updown

    def add_link(self, u, v, one_way, link_id, **attr):
        if one_way == ONE_WAY_BOTH:
            self.add_edge(u, v,
                          {'link_id': link_id, 'one_way': one_way}, **attr)
            self.add_edge(v, u,
                          {'link_id': link_id, 'one_way': one_way}, **attr)
        elif one_way == ONE_WAY_POSITIVE:
            self.add_edge(u, v,
                          {'link_id': link_id, 'one_way': one_way}, **attr)
        elif one_way == ONE_WAY_RERVERSE:
            self.add_edge(v, u,
                          {'link_id': link_id, 'one_way': one_way}, **attr)
        else:
            pass

    def dfs_path(self, node, roadcode=None):
        if node not in self:
            raise nx.NetworkXError('source node %s not in graph' % node)
        path = [node]
        prev_node = node
        for child, prant in dfs_edges_reverse(self, node):
            if prev_node == child:
                path.append(prant)
                prev_node = prant
            else:
                raise nx.NetworkXError('Exist branches in the HWY main road.')
        path.reverse()
        prev_node = node
        if path:
            first_node = path[0]
        else:
            first_node = None
        for prant, child in nx.dfs_edges(self, node):
            if prev_node == prant:
                # Circle
                if first_node == child:
                    path.insert(0, prant)
                    break
                path.append(child)
                prev_node = child
            else:
                raise nx.NetworkXError('Exist branches in the HWY main road.')
        return path

    def _all_hwy_path(self, source, target, cutoff=MAX_CUT_OFF):
        '''两点之间的高速上的所有路径(顺车流方向)'''
        if cutoff < 1:
            return
        if source not in self or target not in self:
            return
        visited = [source]
        stack = [iter(self[source])]
        while stack:
            children = stack[-1]
            child = next(children, None)
            if child is None:
                stack.pop()
                visited.pop()
            elif len(visited) < cutoff:
                if child == target:
                    yield visited + [child]
                elif self.is_hwy_inout(child):
                    pass
                elif self.is_on_not_hwy_main(child):  # 非高速本线
                    pass
                elif self.is_ic_position(child):
                    pass
                else:
                    visited.append(child)
                    stack.append(iter(self[child]))
            else:  # len(visited) == cutoff:
                if child == target:
                    yield visited + [child]
                stack.pop()
                visited.pop()

    def _all_hwy_paths_reverse(self, source, target, cutoff=MAX_CUT_OFF):
        '''两点之间的高速上的所有路径(逆车流方向)'''
        if cutoff < 1:
            return
        if source not in self or target not in self:
            return
        visited = [source]
        stack = [self.predecessors_iter(source)]
        while stack:
            parents = stack[-1]
            parent = next(parents, None)
            if parent is None:
                stack.pop()
                visited.pop()
            elif len(visited) < cutoff:
                if parent == target:
                    yield visited + [parent]
                elif self.is_hwy_inout(parent):
                    pass
                elif self.is_on_not_hwy_main(parent):  # 非高速本线
                    pass
                elif self.is_ic_position(parent):
                    pass
                else:
                    visited.append(parent)
                    stack.append(self.predecessors_iter(parent))
            else:  # len(visited) == cutoff:
                if parent == target:
                    yield visited + [parent]
                stack.pop()
                visited.pop()

    def all_paths_2_jct(self, source, cutoff=MAX_CUT_OFF_NOTOLL):
        '''JCT-Out 到所有邻近的 JCT-In'''
        pathes = list(self.__all_paths_2_jct(source, cutoff))
        return pathes

    def __all_paths_2_jct(self, source, cutoff=MAX_CUT_OFF_NOTOLL):
        '''JCT出口(分歧)到高速JCT入口(入口)'''
        if cutoff < 1:
            return
        if source not in self:
            return
        visited = [source]
        stack = [iter(self[source])]
        while stack:
            children = stack[-1]
            child = next(children, None)
            if child is None:
                stack.pop()
                visited.pop()
            elif len(visited) < cutoff:
                if self.is_jct_inout(child, INOUT_TYPE_IN):  # JCT IN
                    yield visited + [child]
                elif self.is_sapa_in(child):  # SAPA IN
                    yield visited + [child]
                elif self.is_hwy_inout(child):
                    pass
                elif self.is_on_not_hwy_main(child):
                    pass
                elif self.is_jct_inout(child, INOUT_TYPE_OUT):  # JCT OUT
                    pass
                elif self.is_hwy_inout(child):
                    pass
                else:
                    visited.append(child)
                    stack.append(iter(self[child]))
            else:  # len(visited) == cutoff:
                if self.is_jct_inout(child):
                    yield visited + [child]
                stack.pop()
                visited.pop()


#==============================================================================
# HwyGraphNoToll(无料区间)
#==============================================================================
class HwyGraphNoToll(HwyGraph):
    '''Highway图(无料区间)
    '''

    def __init__(self, data=None, **attr):
        '''
        Constructor
        '''
        HwyGraph.__init__(self, data, **attr)

    def all_paths_ic_out(self, source, target, cutoff=MAX_CUT_OFF_NOTOLL):
        pathes = list(self._all_hwy_path(source, target, cutoff))
        return pathes

    def all_paths_ic_in(self, source, target, cutoff=MAX_CUT_OFF_NOTOLL):
        '''IC ==> IC Position.'''
        return list(self._all_hwy_paths_reverse(source, target, cutoff))

    def all_paths_jct(self, source, target, cutoff=MAX_CUT_OFF_NOTOLL):
        '''JCT-Out To JCT-In Pathes'''
        pathes = list(self._all_hwy_path(source, target, cutoff))
        return pathes

    def all_paths_sapa(self, source, target, cutoff=MAX_CUT_OFF_NOTOLL):
        ''''''
        pathes = list(self._all_hwy_path(source, target, cutoff))
        return pathes

    def is_on_not_hwy_main(self, nodeid):
        '''非高速本线'''
        on_not_hwy_main = False
        for (node, neighbor, data) in self.out_edges_iter(nodeid, True):
            road_type = data.get('road_type')
            if road_type in (0, 1):
                return False
            road_code = data.get('road_code')
            if road_code > ROAD_CODE_NO_TOLL:
                return False
            link_type = data.get('link_type')
            if(link_type == LINK_TYPE_MAIN or
               link_type == LINK_TYPE_MAIN_SEPARATION):
                hwymode_f = data.get('hwymode_f')
                if hwymode_f == 1:
                    return False
                else:
                    if road_type != ROAD_TYPE_NARROW_ROAD2:
                        on_not_hwy_main = True
        for (node, neighbor, data) in self.in_edges_iter(nodeid, True):
            road_type = data.get('road_type')
            if road_type in (0, 1):
                return False
            link_type = data.get('link_type')
            if(link_type == LINK_TYPE_MAIN or
               link_type == LINK_TYPE_MAIN_SEPARATION):
                hwymode_f = data.get('hwymode_f')
                if hwymode_f == 1:
                    return False
                else:
                    if road_type != ROAD_TYPE_NARROW_ROAD2:
                        on_not_hwy_main = True
        return on_not_hwy_main

    def get_main_inlinkids(self, node, roadcode=None):
        inlink = []
        for parent, node, data in self.in_edges_iter(node, True):
            if roadcode:
                if data.get('road_code') == roadcode:
                    inlink.append(self.get_linkid(parent, node))
            else:
                if(data.get('link_type') == LINK_TYPE_MAIN_SEPARATION
                   and data.get('road_code') > ROAD_CODE_NO_TOLL):
                    inlink.append(self.get_linkid(parent, node))
        return inlink

    def get_main_outlinkids(self, node, roadcode=None):
        outlink = []
        if node not in self:
            return []
        for node, child, data in self.out_edges_iter(node, True):
            if roadcode:
                if data.get('road_code') == roadcode:
                    outlink.append(self.get_linkid(node, child))
            else:
                if(data.get('link_type') == LINK_TYPE_MAIN_SEPARATION
                   and data.get('road_code') > ROAD_CODE_NO_TOLL):
                    outlink.append(self.get_linkid(node, child))
        return outlink


#==============================================================================
# all_paths, 起点souce到终点target的所有路径
#==============================================================================
def all_paths(G, source, target, cutoff=MAX_CUT_OFF):
    if source not in G:
        raise nx.NetworkXError('source node %s not in graph' % source)
    if target not in G:
        raise nx.NetworkXError('target node %s not in graph' % target)
    return _all_paths(G, source, target, cutoff)


def _all_paths(G, source, target, cutoff=MAX_CUT_OFF):
    if cutoff < 1:
        return
    visited = [source]
    stack = [iter(G[source])]
    while stack:
        children = stack[-1]
        child = next(children, None)
        if child is None:
            stack.pop()
            visited.pop()
        elif len(visited) < cutoff:
            if child == target:
                yield visited + [target]
            # ## node点不能重复: hcz
            # elif child not in visited:
            #    visited.append(child)
            #    stack.append(iter(G[child]))
            else:
                visited.append(child)
                stack.append(iter(G[child]))
        else:  # len(visited) == cutoff:
            if child == target or target in children:
                yield visited + [target]
            stack.pop()
            visited.pop()


#==============================================================================
# all_paths_reverse, 起点souce到终点target的所有路径
#==============================================================================
def all_paths_reverse(G, source, target, cutoff=MAX_CUT_OFF):
    if source not in G:
        raise nx.NetworkXError('source node %s not in graph' % source)
    if target not in G:
        raise nx.NetworkXError('target node %s not in graph' % target)
    return _all_paths_reverse(G, source, target, cutoff)


def _all_paths_reverse(G, source, target, cutoff=MAX_CUT_OFF):
    if cutoff < 1:
        return
    visited = [source]
    stack = [G.predecessors_iter(source)]
    while stack:
        parents = stack[-1]
        parent = next(parents, None)
        #print visited + [parent]
        if parent is None:
            stack.pop()
            visited.pop()
        elif len(visited) < cutoff:
            if parent == target:
                yield visited + [target]
            else:
                visited.append(parent)
                stack.append(G.predecessors_iter(parent))
        else:  # len(visited) == cutoff:
            if parent == target or target in parents:
                yield visited + [target]
            stack.pop()
            visited.pop()


#==============================================================================
# 倒深度探索
#==============================================================================
def dfs_edges_reverse(G, source=None):
    """Produce reverse edges in a depth-first-search starting at source."""
    if source is None:
        # produce edges for all components
        nodes = G
    else:
        # produce edges for components with source
        nodes = [source]
    visited = set()
    for start in nodes:
        if start in visited:
            continue
        visited.add(start)
        stack = [(start, G.predecessors_iter(start))]
        while stack:
            child, predecessors = stack[-1]
            try:
                parent = next(predecessors)
                if parent not in visited:
                    yield child, parent
                    visited.add(parent)
                    stack.append((parent, G.predecessors_iter(parent)))
            except StopIteration:
                stack.pop()


# =============================================================================
# 如果有环的话，只能绕一圈
# =============================================================================
def repeat_cycle(nodes):
    '''只绕一圈: 只是简单判断一点出现三次，所以同个点如果出现在两个圈里，也被认为是出现三次。'''
    if not nodes:
        return True
    for node in nodes:
        if nodes.count(node) >= 3:
            return False
    return True


# =============================================================================
# 判断路径包含
# =============================================================================
def exist_cycle(nodes):
    '''存在环划'''
    if get_simple_cycle(nodes):
        return True
    return False


# =============================================================================
# 取得所有环
# =============================================================================
def get_simple_cycle(nodes):
    G = nx.DiGraph()
    G.add_path(nodes)
    return list(nx.simple_cycles(G))
