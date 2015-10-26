# -*- coding: UTF8 -*-
'''
Created on 2015-3-3

@author: hcz
'''
import common
from component.rdf.hwy.hwy_def_rdf import HWY_INVALID_FACIL_ID_17CY
from component.rdf.hwy.hwy_def_rdf import HWY_TRUE
from component.rdf.hwy.hwy_def_rdf import HWY_FALSE
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_SA
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_PA
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_JCT
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_RAMP
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_IC
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_TOLL
from component.rdf.hwy.hwy_def_rdf import HWY_INOUT_TYPE_IN
from component.rdf.hwy.hwy_def_rdf import HWY_INOUT_TYPE_OUT
from component.rdf.hwy.hwy_def_rdf import HWY_BOUNDARY_TYPE_IN
from component.rdf.hwy.hwy_def_rdf import HWY_BOUNDARY_TYPE_OUT
from component.jdb.hwy.hwy_ic_info import HwyConnInfo
from component.jdb.hwy.hwy_ic_info import HwyICInfo
from component.jdb.hwy.hwy_ic_info import HwyPathPoint
from component.rdf.hwy.hwy_data_mng_rdf import HwyDataMngRDF
from component.jdb.hwy.hwy_ic_info import CONN_DIR_FORWARD
from component.jdb.hwy.hwy_ic_info import CONN_DIR_REVERSE
from component.jdb.hwy.hwy_ic_info import convert_tile_id
from component.rdf.hwy.hwy_facility_rdf import HWY_PATH_TYPE_JCT
from component.rdf.hwy.hwy_facility_rdf import HWY_PATH_TYPE_UTURN


# ==============================================================================
# HwyICInfo
# ==============================================================================
class HwyICInfoRDF(HwyICInfo):
    '''
    IC情报
    '''

    def __init__(self, ic_no, facility_id, facil_list, data_mng):
        '''
        Constructor
        '''
        HwyICInfo.__init__(self, ic_no, facility_id, facil_list)
        self.data_mng = data_mng
        self.log = common.log.common_log.instance().logger('HwyICInfoRDF')

    def _set_distance(self):
        G = self.data_mng.get_graph()
        # 本线路径
        main_path = self.data_mng.get_main_path(self.facil_list[0].road_code,
                                                self.facil_list[0].updown)
        # 求设施内距离
        if len(self.facil_list) == 2:
            s_node = self.facil_list[0].node_id
            e_node = self.facil_list[1].node_id
            s_idx = main_path.index(s_node)
            e_idx = main_path.index(e_node, (s_idx + 1))
            inside_path = main_path[s_idx:(e_idx + 1)]
            if not inside_path:
                self.log.error('Error: no inside path.s_node=%s, e_node=%s'
                               % (s_node, e_node))
            self.inside_distance = G.get_length_for_path(inside_path)
            main_node = e_node
        else:
            self.inside_distance = 0
            main_node = self.facil_list[0].node_id
        # 求设施间距离
        self._set_between_distance(main_node, main_path)

    def _set_between_distance(self, main_node, main_path):
        '''设施间距离'''
        next_facil = self._get_facil(self.ic_no + 1)  # Get Next IC
        if next_facil:
            next_node_id = next_facil.node_id
            if main_node != next_node_id:
                G = self.data_mng.get_graph()
                s_idx = main_path.index(main_node)
                e_idx = main_path.index(next_node_id)
                between_path = main_path[s_idx:(e_idx + 1)]
                if not between_path:
                    self.log.error('No between path.s_node=%s, e_node=%s'
                                   % (main_node, next_node_id))
                self.between_distance = G.get_length_for_path(between_path)
        else:
            self.between_distance = 0

    def get_path_info(self):
        '''获取路径情报。'''
        if(self.facility_id == HWY_INVALID_FACIL_ID_17CY or  # Tile边界
           self.facil_list[0].facilcls == HWY_IC_TYPE_TOLL):  # 收费站
            return
        for facil in self.facil_list:
            if facil.inout == HWY_INOUT_TYPE_OUT:
                self.out_path_infos += self.data_mng.get_ic_path_info(facil)
            elif facil.inout == HWY_INOUT_TYPE_IN:
                self.in_path_infos += self.data_mng.get_ic_path_info(facil)
            else:
                self.log.error('No Path. node=%s' % facil.node_id)

    def _set_ic_type(self):
        if self.facility_id == HWY_INVALID_FACIL_ID_17CY:
            self.boundary = HWY_TRUE
        if self.facil_list[0].facilcls == HWY_IC_TYPE_SA:
            self.sa = HWY_TRUE
        elif self.facil_list[0].facilcls == HWY_IC_TYPE_PA:
            self.pa = HWY_TRUE
        elif self.facil_list[0].facilcls == HWY_IC_TYPE_JCT:
            self.jct = HWY_TRUE
        elif self.facil_list[0].facilcls == HWY_IC_TYPE_RAMP:
            self.ramp = HWY_TRUE
        elif self.facil_list[0].facilcls == HWY_IC_TYPE_IC:
            self.ic = HWY_TRUE
        if self.facil_list[0].facilcls == HWY_IC_TYPE_TOLL:
            self.tollgate = HWY_TRUE

    def _set_forward_flag(self):
        '''順方向継続有無フラグ--前方有并列设施'''
        next_ic_no = self.ic_no + 1
        next_facil = self._get_facil(next_ic_no)  # Get Next IC
        while self._check_boundary(next_facil):  # 跳过边界点
            next_ic_no += 1
            next_facil = self._get_facil(next_ic_no)
        if(next_facil and
           (self.data_mng.is_same_facil(self.facil_list[0], next_facil) or
            self._is_identical_facil(self.facil_list[0], next_facil))
           ):
            self.forward_flag = HWY_TRUE
        else:
            self.forward_flag = HWY_FALSE

    def _set_reverse_flag(self):
        '''逆方向継続有無フラグ--后方有并列设施'''
        prev_ic_no = self.ic_no - 1
        prev_facil = self._get_facil(prev_ic_no)
        while self._check_boundary(prev_facil):  # 跳过边界点
            prev_ic_no -= 1
            prev_facil = self._get_facil(prev_ic_no)
        if(prev_facil and
           (self.data_mng.is_same_facil(self.facil_list[0], prev_facil) or
            self._is_identical_facil(self.facil_list[0], prev_facil))
           ):
            self.reverse_flag = HWY_TRUE
        else:
            self.reverse_flag = HWY_FALSE

    def _check_boundary(self, facil):
        if facil:
            if facil.facility_id == HWY_INVALID_FACIL_ID_17CY:
                return True
        return False

    def _get_facil(self, ic_no):
        ic_no, facility_id, facil_list = self.data_mng.get_ic(ic_no)
        if not ic_no:
            return None
        facil = facil_list[0]
        if(facil.road_code == self.facil_list[0].road_code and
           facil.updown == self.facil_list[0].updown):
            return facil
        return None

    def _set_enter_direction(self):
        '''进入方位'''
        return 0

    def _set_road_no(self):
        road_code = self.facil_list[0].road_code
        self.road_no = self.data_mng.get_road_no(road_code)

    def _set_tile_id(self):
        self.tile_id = self._get_tile_id()
        if not self.tile_id:
            pass

    def _get_tile_id(self):
        node_id = self.facil_list[0].node_id
        road_code = self.first_facil.road_code
        # 点是处在tile边界
        if self.data_mng.is_boundary_node(node_id):
            # 出口时，边界点在后(边界点ic_no较小)，所以用点脱出link的Tile
            if self.first_facil.inout == HWY_INOUT_TYPE_OUT:
                # get out tile id
                tile_id = self._get_out_tile_id(node_id, road_code)
                self.log.info('out tile: %s, node=%s' % (tile_id, node_id))
            else:  # 入口/收费站时，边界点在前(边界点ic_no较大)，所以点的进入link的Tile
                # Get in tile id
                tile_id = self._get_in_tile_id(node_id, road_code)
                self.log.info('in tile: %s, node=%s' % (tile_id, node_id))
        else:
            updown = self.first_facil.updown
            # 起末点处，不做Tile边界点，即使该处在边界上
            # 道路起点，用out_link所在tile
            if self.data_mng.is_road_start_node(road_code, updown, node_id):
                tile_id = self._get_out_tile_id(node_id, road_code)
            # 道路终点，用in_link所在tile
            elif self.data_mng.is_road_end_node(road_code, updown, node_id):
                tile_id = self._get_in_tile_id(node_id, road_code)
            else:  # 一般设施
                tile_id = self._get_in_tile_id(node_id, road_code)
        return convert_tile_id(tile_id)

    def _get_in_tile_id(self, node_id, road_code):
        '''node点进入本线link的Tile.'''
        G = self.data_mng.get_graph()
        in_tiles = G.get_in_tile(node_id, road_code)
        if in_tiles:
            if len(in_tiles) == 1:
                return in_tiles[0]
            else:
                self.log.info('Error: Number of InTile > 1, nodeid=%s'
                              % node_id)
        else:
            self.log.info('Error: No InLink, nodeid=%s' % node_id)
        return None

    def _get_out_tile_id(self, node_id, road_code):
        '''node点脱出本线link的Tile.'''
        G = self.data_mng.get_graph()
        # outlinks = G.get_main_outlinkids(node_id)
        out_tiles = G.get_out_tile(node_id, road_code)
        if out_tiles:
            if len(out_tiles) == 1:
                return out_tiles[0]
            else:
                self.log.info('Error: Number of OutTile > 1, nodeid=%s'
                              % node_id)
        else:
            self.log.info('Error: No OutLink, nodeid=%s' % node_id)
        return None

    def _set_path_point_info(self):
        '''探索地点情報'''
        exit_path_point = self._get_exit_path_point()
        if exit_path_point:
            self.path_points += exit_path_point
        self._set_exit_flag()  # 出口标志
        enter_path_point = self._get_enter_path_point()
        if enter_path_point:
            self.path_points += enter_path_point
        self._set_enter_flag()  # 入口标志
        self.path_points += self._get_main_path_point()
        self.path_points += self._get_center_path_point()
        self.path_count = len(self.path_points)

    def get_path_point_info(self):
        return self.path_points

    def _set_conn_info(self):
        '''接続情報'''
        # ## 出口
        facil = self.first_facil
        node = facil.node_id
        for conn_info in self._get_conn_path_info(node, facil.inout):
            conn_facil, path, path_type = conn_info[0:3]
            if not conn_facil:
                pass
            hwy_conn = HwyConnInfoRDF(self, self.tile_id,
                                      facil, conn_facil,
                                      path, path_type)
            hwy_conn.set_conn_info()
            self._add_conn_info(hwy_conn)  # 多条路径取最短的
        # ## 入口
        if self.second_facil:
            facil = self.second_facil
            node = facil.node_id
            for conn_info in self._get_conn_path_info(node, facil.inout):
                conn_facil, path, path_type = conn_info[0:3]
                hwy_conn = HwyConnInfoRDF(self, self.tile_id,
                                          facil, conn_facil,
                                          path, path_type)
                hwy_conn.set_conn_info()
                self._add_conn_info(hwy_conn)
        self.conn_count = len(self.conn_infos)

    def _get_conn_path_info(self, node, inout=HWY_INOUT_TYPE_OUT):
        path_dict = {}
        path_infos = []
        if inout == HWY_INOUT_TYPE_OUT and self.out_path_infos:
            path_infos = self.out_path_infos
            t_inout = HWY_INOUT_TYPE_IN
        elif inout == HWY_INOUT_TYPE_IN and self.in_path_infos:
            path_infos = self.in_path_infos
            t_inout = HWY_INOUT_TYPE_OUT
        else:
            return
        for path_info in path_infos:
            node_lid = path_info[0]
            path_type = path_info[2]
            t_node = node_lid[-1]
            if path_type in (HWY_PATH_TYPE_JCT, HWY_PATH_TYPE_UTURN):
                if t_node in path_dict:
                    path_dict[t_node].append(path_info)
                else:
                    path_dict[t_node] = [path_info]
        for t_node, path_infos in path_dict.iteritems():
            conn_facil = self._get_conn_facil(node, t_node, t_inout)
            if conn_facil:
                for path_info in path_infos:
                    path = path_info[0]
                    path_type = path_info[2]
                    yield conn_facil, path, path_type
            else:
                # 没有接续设施
                self.log.error('No Conn_Facility. s_node=%s, t_node=%s'
                               % (node_lid[0], node_lid[-1]))

    def _get_conn_facil(self, s_node, t_node, inout):
        facil_cls = self.first_facil.facilcls
        facils = self.data_mng.get_hwy_facils_by_nodeid(t_node,
                                                        t_facilcls=facil_cls)
        conn_facils = []
        for facil in facils:
            if facil.inout == inout:
                conn_facils.append(facil)
        if conn_facils:
            if len(set(conn_facils)) > 1:
                r_conn_facils = self._get_conn_facil_reserve(s_node,
                                                             conn_facils)
                if len(set(r_conn_facils)) > 1:
                    self.log.error('No JCT Facility. node=%s' % t_node)
                else:
                    return r_conn_facils[0]
            return conn_facils[0]
        else:
            return []

    def _get_conn_facil_reserve(self, s_node, conn_facils):
        rst_conn_facils = []
        for facil in conn_facils:
            pathes = self.data_mng.get_ic_path_info(facil)
            for path_info in pathes:
                path = path_info[0]
                if s_node == path[-1]:
                    rst_conn_facils.append(facil)
        return rst_conn_facils

    def _set_toll_info(self):
        return

    def _set_enter_flag(self, enter_flag=HWY_TRUE):
        '''入口'''
        facil_cls = self.first_facil.facilcls
        inout = self.first_facil.inout
        if facil_cls in (HWY_IC_TYPE_IC, HWY_IC_TYPE_RAMP):
            if inout == HWY_INOUT_TYPE_IN:
                self.enter = HWY_TRUE
            elif self.second_facil:  # 第二个点，一定是入口
                self.enter = HWY_TRUE
            else:
                self.enter = HWY_FALSE
        else:
            self.enter = HWY_FALSE

    def _set_exit_flag(self, exit_flag=HWY_TRUE):
        '''出口'''
        facil_cls = self.first_facil.facilcls
        inout = self.first_facil.inout
        if facil_cls in (HWY_IC_TYPE_IC, HWY_IC_TYPE_RAMP):
            if inout == HWY_INOUT_TYPE_OUT:
                self.exit = HWY_TRUE
            else:
                self.exit = HWY_FALSE
        else:
            self.exit = HWY_FALSE

    def _get_exit_path_point(self):
        exit_list = []
        node_link_list = []
        inout = HWY_INOUT_TYPE_OUT
        for path_info in self.out_path_infos:
            node_lid, link_lid = path_info[0:2]
            if len(node_lid) < 2:  # 本线与本线直接相连、本线和一般道直接相连
                for (node, link) in self._get_2_main_link_of_jct(node_lid,
                                                                 inout):
                    if (node, link) not in node_link_list:
                        node_link_list.append((node, link))
                continue
            node = node_lid[1]
            link = link_lid[0]
            if (node, link) not in node_link_list:
                node_link_list.append((node, link))
        for (node, link) in node_link_list:
            point = HwyPathPoint(self, link, node)
            point.exit_flag = HWY_TRUE
            exit_list.append(point)
        return exit_list

    def _get_enter_path_point(self):
        enter_list = []
        node_link_list = []
        inout = HWY_INOUT_TYPE_IN
        for path_info in self.in_path_infos:
            node_lid, link_lid = path_info[0:2]
            if len(node_lid) < 2:  # 本线与本线直接相连、本线和一般道直接相连
                for (node, link) in self._get_2_main_link_of_jct(node_lid,
                                                                 inout):
                    if (node, link) not in node_link_list:
                        node_link_list.append((node, link))
                continue
            node = node_lid[1]
            link = link_lid[0]
            if (node, link) not in node_link_list:
                node_link_list.append((node, link))
        for (node, link) in node_link_list:
            point = HwyPathPoint(self, link, node)
            point.enter_flag = HWY_TRUE
            enter_list.append(point)
        return enter_list

    def _get_2_main_link_of_jct(self, node_lid, inout):
        G = self.data_mng.get_graph()
        if self.facil_list[0].facilcls != HWY_IC_TYPE_JCT:
            return
        road_code = self.first_facil.road_code
        if inout == HWY_INOUT_TYPE_OUT:
            reverse = False
            node_id = self.first_facil.node_id
            # 设施点所在本线link(进入)
            nodes = G.get_main_link(node_id, road_code,
                                    same_code=True, reverse=True)
        else:
            reverse = True
            if self.second_facil:
                node_id = self.second_facil.node_id
            else:
                node_id = self.first_facil.node_id
            # 设施点所在本线link(退出)
            nodes = G.get_main_link(node_id, road_code,
                                    same_code=True, reverse=False)
        if not nodes:
            self.log.error('No main_link. node=%s' % node_id)
            return
        if len(nodes) > 1:
            self.log.error('More than 1 main_link. node=%s' % node_id)
            return
        f_main_node = nodes[0]
        # 求接续本线
        path = [f_main_node] + list(node_lid)
        t_main_nodes = G.get_to_main_link_of_path(path, road_code,
                                                  reverse=reverse)
        for t_node in t_main_nodes:
            if reverse:  # 逆
                link_id = G.get_linkid(t_node, node_lid[-1])
            else:  # 顺
                link_id = G.get_linkid(node_lid[-1], t_node)
            yield t_node, link_id

    def _get_main_path_point(self):
        '''本线点：退出设施的本线link'''
        G = self.data_mng.get_graph()
        if self.second_facil:
            main_node = self.second_facil.node_id
        else:
            main_node = self.first_facil.node_id
        inlinks = G.get_main_outlinkids(main_node, self.first_facil.road_code)
        if inlinks:
            point = HwyPathPoint(self, inlinks[0], main_node)
            point.main_flag = HWY_TRUE
            return [point]
        return []

    def _get_center_path_point(self):
        '''表示点：进入设施的本线link'''
        G = self.data_mng.get_graph()
        node_id = self.first_facil.node_id
        road_code = self.first_facil.road_code
        inlinks = G.get_main_inlinkids(node_id, road_code)
        if inlinks:
            point = HwyPathPoint(self, inlinks[0], node_id)
            point.center_flag = HWY_TRUE
            return [point]
        else:
            return []

    def _get_position_path_point(self):
        return []

    def set_node_add_info(self):
        '''制作 临时附加情报表：并切短Path'''
        out_add_infos = self._get_out_node_add_info()
        in_add_infos = self._get_in_node_add_info()
        self.add_infos = out_add_infos + in_add_infos

    def _get_out_node_add_info(self):
        return []

    def _get_in_node_add_info(self):
        return []


# =============================================================================
# 边界出口点（离开当前Tile）
# =============================================================================
class HwyBoundaryOutInfoRDF(HwyICInfoRDF):
    '''
    Tile Boundary(Out)
    '''
    def __init__(self, ic_no, facility_id, facil_list, data_mng):
        HwyICInfoRDF.__init__(self, ic_no, facility_id, facil_list, data_mng)

    def _set_distance(self):
        self.inside_distance = 0.0
        self.between_distance = 0.0

    def get_boundary_type(self):
        return HWY_BOUNDARY_TYPE_OUT

    def _get_tile_id(self):
        node_id = self.facil_list[0].node_id
        G = self.data_mng.get_graph()
        in_tiles = G.get_in_tile(node_id, self.facil_list[0].road_code)
        if in_tiles:
            if len(in_tiles) == 1:
                return convert_tile_id(in_tiles[0])
            else:
                print 'Error: Number of InTile > 1, nodeid=%s' % node_id
        else:
            print 'Error: No InLink, nodeid=%s' % node_id
        return None

    def _get_conn_tile_id(self):
        '''接续先Tile_id'''
        node_id = self.facil_list[0].node_id
        G = self.data_mng.get_graph()
        # outlinks = G.get_main_outlinkids(node_id)
        out_tiles = G.get_out_tile(node_id, self.facil_list[0].road_code)
        if out_tiles:
            if len(out_tiles) == 1:
                return convert_tile_id(out_tiles[0])
            else:
                print 'Error: Number of OutTile > 1, nodeid=%s' % node_id
        else:
            print 'Error: No OutLink, nodeid=%s' % node_id
        return None

    def _set_forward_flag(self):
        self.forward_flag = HWY_FALSE

    def _set_reverse_flag(self):
        '''判断边界点的前一个设施和后一个设施是否并列'''
        # 前一个设施
        prev_ic_no = self.ic_no - 1
        prev_facil = self._get_facil(prev_ic_no)
        while self._check_boundary(prev_facil):
            prev_ic_no -= 1
            prev_facil = self._get_facil(prev_ic_no)
        # 后一个设施
        next_ic_no = self.ic_no + 1
        next_facil = self._get_facil(next_ic_no)
        while self._check_boundary(next_facil):
            next_ic_no += 1
            next_facil = self._get_facil(next_ic_no)
        if (prev_facil and next_facil and
            (self.data_mng.is_same_facil(prev_facil, next_facil)
             or self._is_identical_facil(prev_facil, next_facil))):
            self.reverse_flag = HWY_TRUE
        else:
            self.reverse_flag = HWY_FALSE

    def _get_exit_path_point(self):
        return []

    def _get_enter_path_point(self):
        return []

    def _get_main_path_point(self):
        # 边界出口，不做本线点
        return []

    def _get_center_path_point(self):
        '''表示点：进入设施的本线link'''
        G = self.data_mng.get_graph()
        node_id = self.first_facil.node_id
        road_code = self.first_facil.road_code
        inlinks = G.get_main_inlinkids(node_id, road_code)
        if inlinks:
            point = HwyPathPoint(self, inlinks[0], node_id)
            point.center_flag = HWY_TRUE
            return [point]
        else:
            print 'Error: No inlink for tile boundary %s' % node_id
            return []

    def _get_position_path_point(self):
        return []

    def _set_conn_info(self):
        pass


# =============================================================================
# 边界进入点（进入Tile的点）
# =============================================================================
class HwyBoundaryInInfoRDF(HwyBoundaryOutInfoRDF):
    '''
    Tile Boundary(IN)
    '''
    def __init__(self, ic_no, facility_id, facil_list, data_mng):
        HwyBoundaryOutInfoRDF.__init__(self, ic_no, facility_id,
                                       facil_list, data_mng)

    def _set_distance(self):
        self.inside_distance = 0.0
        main_node = self.first_facil.node_id
        # 本线路径
        main_path = self.data_mng.get_main_path(self.facil_list[0].road_code,
                                                self.facil_list[0].updown)
        self._set_between_distance(main_node, main_path)

    def _set_conn_tile_id(self):
        # 入口的tile和出口的tile，刚好调换一下
        self.conn_tile_id = self._get_tile_id()

    def _set_tile_id(self):
        self.tile_id = self._get_conn_tile_id()

    def _set_forward_flag(self):
        '''判断边界点的前一个设施和后一个设施是否并列'''
        # 前一个设施
        prev_ic_no = self.ic_no - 1
        prev_facil = self._get_facil(prev_ic_no)
        while self._check_boundary(prev_facil):
            prev_ic_no -= 1
            prev_facil = self._get_facil(prev_ic_no)
        # 后一个设施
        next_ic_no = self.ic_no + 1
        next_facil = self._get_facil(next_ic_no)
        while self._check_boundary(next_facil):
            next_ic_no += 1
            next_facil = self._get_facil(next_ic_no)
        if (prev_facil and next_facil and
            (self.data_mng.is_same_facil(prev_facil, next_facil)
             or self._is_identical_facil(prev_facil, next_facil))):
            self.forward_flag = HWY_TRUE
        else:
            self.forward_flag = HWY_FALSE

    def _set_reverse_flag(self):
        self.reverse_flag = HWY_FALSE

    def _get_main_path_point(self):
        '''本线点：退出设施的本线link'''
        G = self.data_mng.get_graph()
        node_id = self.first_facil.node_id
        road_code = self.first_facil.road_code
        inlinks = G.get_main_outlinkids(node_id, road_code)
        if inlinks:
            point = HwyPathPoint(self, inlinks[0], node_id)
            point.main_flag = HWY_TRUE
            return [point]
        else:
            print 'Error: No OutLink for tile boundary %s' % node_id
        return []

    def _get_center_path_point(self):
        # 边界入口，不做表示点
        return []


# =============================================================================
# 高速接続情報
# =============================================================================
class HwyConnInfoRDF(HwyConnInfo):
    def __init__(self, ic_info, tile_id, s_facil, t_facil, path, path_type):
        HwyConnInfo.__init__(self, ic_info, tile_id,
                             s_facil, t_facil,
                             path, path_type)
        self.data_mng = ic_info.data_mng

    def _set_road_attr(self):
        '''conn_ic所在道路的道路属性code'''
        self.road_attr = self.t_facil.updown

    def _set_conn_direction(self):
        '''接続方向フラグ，顺车流方向(JCT出口到JCT入口)： 0， 反之: 1'''
        if self.s_facil.inout == HWY_INOUT_TYPE_OUT:
            self.conn_direction = CONN_DIR_FORWARD
        elif self.s_facil.inout == HWY_INOUT_TYPE_IN:
            self.conn_direction = CONN_DIR_REVERSE
        else:
            self.conn_direction = None

    def _set_uturn(self):
        if self.path_type == HWY_PATH_TYPE_UTURN:
            self.uturn_flag = HWY_TRUE
        else:
            self.uturn_flag = HWY_FALSE

    def _set_same_road_flag(self):
        '''同一路線接続フラグ'''
        # 1. Uturn
        if self.uturn_flag:
            self.same_road_flag = HWY_TRUE
            return
        # 2. 环形的起末，注：环形的起是JCT In, 末是JCT Out
        if (self.s_facil.road_code == self.t_facil.road_code):
            # print self.s_facil.road_code
            self.same_road_flag = HWY_TRUE
            return

    def _set_conn_road_no(self):
        '''接続先道路番号'''
        hwy_data = self.data_mng.instance()
        road_code = self.t_facil.road_code
        self.conn_road_no = hwy_data.get_road_no(road_code)

    def _set_conn_ic_no(self):
        self.conn_ic_no = self.data_mng.get_ic_no(self.t_facil)

    def _set_conn_length(self):
        G = self.data_mng.get_graph()
        if self.s_facil.inout == HWY_INOUT_TYPE_OUT:  # 出口、正向
            path = list(self.path)
        else:  # 入口、逆向
            path = list(self.path)
            path.reverse()
        self.conn_length = G.get_length_for_path(path)

    def _set_conn_tile_id(self):
        conn_node_id = self.t_facil.node_id
        conn_road_code = self.t_facil.road_code
        # 接续点是处在tile边界
        if self.data_mng.is_boundary_node(conn_node_id):
            # 出口时，边界点在后(边界点ic_no较小)，所以用点脱出link的Tile
            if self.t_facil.inout == HWY_INOUT_TYPE_OUT:
                # Get out tile id
                conn_tile_id = self.ic_info._get_out_tile_id(conn_node_id,
                                                             conn_road_code)
                print ('out tile: %s, node=%s'
                       % (conn_tile_id, conn_node_id))
            else:  # 入口/收费站时，边界点在前(边界点ic_no较大)，所以点的进入link的Tile
                # Get in tile id
                conn_tile_id = self.ic_info._get_in_tile_id(conn_node_id,
                                                            conn_road_code)
                print ('in tile: %s, node=%s' % (conn_tile_id, conn_node_id))
        else:
            updown = self.t_facil.updown
            # 起末点处，不做Tile边界点，即使该处在边界上
            # 道路起点，用out_link所在tile
            if self.data_mng.is_road_start_node(conn_road_code,
                                                updown,
                                                conn_node_id):
                conn_tile_id = self.ic_info._get_out_tile_id(conn_node_id,
                                                             conn_road_code)
            # 道路终点，用in_link所在tile
            elif self.data_mng.is_road_end_node(conn_road_code,
                                                updown, conn_node_id):
                conn_tile_id = self.ic_info._get_in_tile_id(conn_node_id,
                                                            conn_road_code)
            else:
                conn_tile_id = self.ic_info._get_in_tile_id(conn_node_id,
                                                            conn_road_code)
        # if conn_tile_id != G.get_node_tile(conn_node_id):
        #    print 'conn_tile_id != node_tile id.'
        self.conn_tile_id = convert_tile_id(conn_tile_id)
        if self.conn_tile_id != self.tile_id:
            self.tile_change_flag = HWY_TRUE
        else:
            self.tile_change_flag = HWY_FALSE

    def _set_toll_info(self):
        '''料金情报'''
        return
