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
from component.jdb.hwy.hwy_ic_info import pos_type_name
from component.jdb.hwy.hwy_ic_info import ADD_NODE_POS_TYPE_NONE
from component.rdf.hwy.hwy_data_mng_rdf import HwyDataMngRDF
from component.jdb.hwy.hwy_ic_info import CONN_DIR_FORWARD
from component.jdb.hwy.hwy_ic_info import CONN_DIR_REVERSE
from component.jdb.hwy.hwy_ic_info import convert_tile_id
from component.rdf.hwy.hwy_facility_rdf import HWY_PATH_TYPE_IC
from component.rdf.hwy.hwy_facility_rdf import HWY_PATH_TYPE_JCT
from component.rdf.hwy.hwy_facility_rdf import HWY_PATH_TYPE_SAPA
from component.rdf.hwy.hwy_facility_rdf import HWY_PATH_TYPE_UTURN
from component.rdf.hwy.hwy_data_mng_rdf import TollFacilInfoRDF
from component.rdf.hwy.hwy_toll_info_rdf import HwyTollInfoRDF


# ==============================================================================
# HwyICInfo
# ==============================================================================
class HwyICInfoRDF(HwyICInfo):
    '''
    IC情报
    '''

    def __init__(self, ic_no, facility_id, facil_list,
                 data_mng, item_name='HwyICInfoRDF'):
        '''
        Constructor
        '''
        HwyICInfo.__init__(self, ic_no, facility_id, facil_list)
        self.data_mng = data_mng
        self.log = common.log.common_log.instance().logger(item_name)
        self.main_in_node = None  # 本线进入node

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
            conn_facils = self._get_conn_facil(node, t_node, t_inout)
            if conn_facils:
                if len(conn_facils) > 1 and path_type != HWY_PATH_TYPE_UTURN:
                    self.log.warning('Conn_Facil > 1.node=%s' % t_node)
                    return
                for facil in conn_facils:
                    for path_info in path_infos:
                        path = path_info[0]
                        path_type = path_info[2]
                        yield facil, path, path_type
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
                return r_conn_facils
            return conn_facils
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
        return 0
        # 除了中国，其他仕向地不做。
        if self.tollgate == HWY_TRUE:  # 本线料金
            s_facil = self.first_facil
            node_id = self.first_facil.node_id
            # ## 取得收费站link
            # G = self.data_mng.get_graph()
            # road_code = s_facil.road_code
            # updown = s_facil.updown
            # if not self.data_mng.is_road_end_node(road_code, updown,
            #                                       node_id):
            #     out_node = self._get_main_out_node(node_id)
            #     link_id = G.get_linkid(node_id, out_node)
            # else:
            #     in_node = self._get_main_in_node(node_id)
            #     link_id = G.get_linkid(in_node, node_id)
            toll_facil = self._get_toll_facil(s_facil, node_id)
            if toll_facil:
                toll_info = HwyTollInfoRDF(self, toll_facil,
                                           self.first_facil, None)
                toll_info.set_toll_info()
                self.toll_infos.append(toll_info)
            else:
                self.log.error('No Toll Facility Info for node=%s' % node_id)
        else:
            out_toll_list = self._get_out_toll()
            in_toll_list = self._get_in_toll()
#         if len(out_toll_list) > 1:
#             print ('Out Toll > 1. node_id=%s, name=%s'
#                    % (self.first_facil.node_id, self.name))
#         if len(in_toll_list) > 1:
#             print ('In Toll > 1. node_id=%s, name=%s'
#                    % (self.first_facil.node_id, self.name))
            self.toll_infos += in_toll_list  # 入口
            self.toll_infos += out_toll_list  # 出口

    def _get_out_toll(self):
        '''出口(SAPA,IC, Ramp)料金'''
        out_toll_list = []
        for path_info in self.out_path_infos:
            node_lid = path_info[0]
            path_type = path_info[2]
            if path_type in (HWY_PATH_TYPE_JCT,
                             HWY_PATH_TYPE_UTURN):  # 接续料金, 放到接续情报处理
                continue
            toll_list = self.get_toll_info(self.first_facil, node_lid,
                                           path_type)
            for toll_info in toll_list:
                self.add_toll_info(out_toll_list, toll_info)
        return out_toll_list

    def _get_in_toll(self):
        '''出口(SAPA,IC, Ramp)料金'''
        in_toll_list = []
        if self.second_facil:
            in_facil = self.second_facil
        else:
            in_facil = self.first_facil
        for path_info in self.in_path_infos:
            node_lid = path_info[0]
            path_type = path_info[2]
            if path_type in (HWY_PATH_TYPE_JCT,
                             HWY_PATH_TYPE_UTURN):  # 接续料金, 放到接续情报处理
                continue
            if path_type in HWY_PATH_TYPE_SAPA:
                continue
            toll_list = self.get_toll_info(in_facil, node_lid,
                                           path_type)
            for toll_info in toll_list:
                self.add_toll_info(in_toll_list, toll_info)
        return in_toll_list

    def get_toll_info(self, s_facil, path, path_type,
                      conn_ic_no=None, conn_facil=None):
        toll_list = []
        return toll_list
        # 除中国，其他仕向地不做
        G = self.data_mng.get_graph()
        max_cnt = len(path)
        if path_type in (HWY_PATH_TYPE_JCT, HWY_PATH_TYPE_UTURN,
                         HWY_PATH_TYPE_SAPA):
            max_cnt = max_cnt - 1
        node_cnt = 1
        while node_cnt < max_cnt:
            node_id = path[node_cnt]
            if G.is_tollgate(node_id):
                # 取得收费站link
                # link_id = self._get_toll_link(path[0:max_cnt], node_cnt,
                #                               s_facil.inout)
                toll_facil = self._get_toll_facil(s_facil, node_id)
                if toll_facil:
                    toll_info = HwyTollInfoRDF(self, toll_facil,
                                               self.first_facil, path_type,
                                               conn_ic_no, conn_facil)
                    toll_info.set_toll_info()
                    self.add_toll_info(toll_list, toll_info)
            node_cnt += 1
        return toll_list

    def _get_toll_link(self, path, node_cnt, inout):
        u = path[node_cnt]
        G = self.data_mng.get_graph()
        link_id = None
        if inout == HWY_INOUT_TYPE_OUT:
            if node_cnt < len(path) - 1:
                v = path[node_cnt + 1]
                link_id = G.get_linkid(u, v)
            else:  # 到头了
                v = path[node_cnt - 1]
                link_id = G.get_linkid(v, u)
        elif inout == HWY_INOUT_TYPE_IN:
            v = path[node_cnt - 1]
        return link_id

    def _get_toll_facil(self, facil, node_id, conn_facil=None):
        temp_toll_facils = []
        toll_facil_infos = self.data_mng.get_toll_facil(node_id)
        if len(toll_facil_infos) == 1:
            return toll_facil_infos[0]
        for toll_facil in toll_facil_infos:
            if(facil.road_code == toll_facil.road_code and
               facil.road_point == toll_facil.road_point and
               facil.inout == toll_facil.inout):
                temp_toll_facils.append(toll_facil)
        if not temp_toll_facils:
            self.log.error('No Toll Facililty Info. node=%s, road_code=%s,'
                           'road_seq=%s, inout=%s' %
                           (node_id, facil.road_code,
                            facil.road_point, facil.inout))
            return None
        if len(temp_toll_facils) > 1:
            self.log.error('Toll Facililty Info Num > 1. node=%s,'
                           'road_code=%s, road_seq=%s, inout=%s' %
                           (node_id, facil.road_code, facil.road_point,
                            facil.inout))
        return temp_toll_facils[0]

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
        '''设施出口附加情报'''
        add_info_list = []
        return add_info_list  # 中国以外的不做附加情报

    def _get_in_node_add_info(self):
        '''设施入口附加情报'''
        add_info_list = []
        return add_info_list  # 中国以外的不做附加情报

    def _set_add_info(self, add_link, add_node, inout,
                      link_pos, pos_type, facilcls,
                      link_lid, node_lid, dir_s_node,
                      dir_e_node):
        add_info = {}
        add_info['add_link'] = add_link
        add_info['add_node'] = add_node
        add_info['in_out'] = inout
        # ## 求附加情报点和料金的关系
        pos_type = self._get_pos_type(node_lid, link_pos)
        add_info['pos_type'] = pos_type
        add_info['pos_type_name'] = pos_type_name.get(pos_type)
        add_info['facilcls'] = facilcls
        add_info['link_lid'] = ','.join([str(l) for l in link_lid])
        add_info['node_lid'] = ','.join([str(n) for n in node_lid])
        add_info['dir_s_node'] = dir_s_node
        add_info['dir_e_node'] = dir_e_node
        return add_info

    def _get_special_out_node_add_info(self, node_lid, link_lid):
        # 点JCT、出口直接和一般道相连
        if link_lid:
            return []
        add_node_link = []
        ic_node = self.first_facil.node_id
        facilcls = self.first_facil.facilcls
        if facilcls == HWY_IC_TYPE_JCT:  # 点JCT, 取out_link
            return []
        elif facilcls == HWY_IC_TYPE_IC:
            main_in_node = self._get_main_in_node(ic_node)
            if not main_in_node:
                return []
            node_lid = [main_in_node] + list(node_lid)
            # 取得高速相连的普通道，作为附加情报
            rst_add_info = self._get_nwy_out_node_add_info(node_lid)
            well_flg, add_node_link = rst_add_info
#             if not add_node_link:
#                 if well_flg:  # 井、十、T字型
#                     # 该设施有无并设
#                     # ...
#                     # 取进入本线link id
#                     in_link_id = G.get_linkid(main_in_node, ic_node)
#                     return [(ic_node, in_link_id)]
        return add_node_link

    def _get_special_in_node_add_info(self, node_lid, link_lid):
        # 点JCT、出口直接和一般道相连
        if link_lid:
            return
        add_node_link = []
        ic_node = self.first_facil.node_id
        facilcls = self.first_facil.facilcls
        main_out_node = self._get_main_out_node(ic_node)
        if not main_out_node:
            return []
        if facilcls == HWY_IC_TYPE_JCT:  # 点JCT, 取out_link
            return []
        elif facilcls == HWY_IC_TYPE_IC:
            node_lid = [main_out_node] + list(node_lid)
            # 取得高速相连的普通道，作为附加情报
            rst_add_info = self._get_nwy_in_node_add_info(node_lid)
            well_flg, add_node_link = rst_add_info
#             if not add_node_link:
#                 if well_flg:  # 井、十、T字型
#                     # 该设施有无并设
#                     # ...
#                     # 取进入本线link id
#                     out_link_id = G.get_linkid(ic_node, main_out_node)
#                     return [(ic_node, out_link_id)]
        return add_node_link

    def _get_well_cross_add_info(self, node_id, inout):
        '''出口进入，[井]、[十]，[T]字型的路'''
        road_code = self.first_facil.road_code
        updown = self.first_facil.updown
        # 高速线路终点
        if inout == HWY_INOUT_TYPE_OUT:
            if not self.data_mng.is_road_end_node(road_code,
                                                  updown,
                                                  node_id):
                return []
        elif inout == HWY_INOUT_TYPE_IN:  # 高速线路起点
            if not self.data_mng.is_road_start_node(road_code, updown,
                                                    node_id):
                return []
        else:  # 收费站
            return []
        if inout == HWY_INOUT_TYPE_OUT:  # 出口/结尾
            # 取本线进入link(其实是上个点)
            main_in_node = self._get_main_in_node(node_id)
            if not main_in_node:
                self.log.error('No Main In Node. node=%s' % node_id)
                return []
            node_lid = [main_in_node, node_id]
            ignore_mapping = True
            rst_add_info = self._get_nwy_out_node_add_info(node_lid,
                                                           ignore_mapping)
            well_flg, add_link_nodes = rst_add_info
            if not add_link_nodes:  # 空
                if well_flg:
                    G = self.data_mng.get_graph()
                    in_link_id = G.get_linkid(main_in_node, node_id)
                    dir_s_node = main_in_node
                    dir_e_node = node_id
                    return [(node_id, in_link_id, dir_s_node, dir_e_node)]
        else:  # 入口/开头
            # 取本线退入link(其实上下个点)
            main_out_node = self._get_main_out_node(node_id)
            if not main_out_node:
                self.log.error('No Main Out Node. node=%s' % node_id)
                return []
            node_lid = [main_out_node, node_id]
            ignore_mapping = True
            rst_add_info = self._get_nwy_in_node_add_info(node_lid,
                                                          ignore_mapping)
            well_flg, add_link_nodes = rst_add_info
            if not add_link_nodes:  # 空
                if well_flg:
                    G = self.data_mng.get_graph()
                    in_link_id = G.get_linkid(node_id, main_out_node)
                    dir_s_node = node_id
                    dir_e_node = main_out_node
                    return [(node_id, in_link_id, dir_s_node, dir_e_node)]
        return []

    def _get_main_in_node(self, nodeid):
        # 取本线进入link(其实是上个点)
        road_code = self.first_facil.road_code
        G = self.data_mng.get_graph()
        in_nodes = G.get_main_link(nodeid, road_code,
                                   same_code=True, reverse=True)
        if in_nodes:
            if len(in_nodes) == 1:
                return in_nodes[0]
            else:
                self.log.error('Main in link Number > 1. node=%s'
                               % nodeid)
                return None
        else:
            self.log.error('Main in link Number = 0. node=%s' % nodeid)
            return None

    def _get_main_out_node(self, nodeid):
        # 取本线进入link(其实是上个点)
        road_code = self.first_facil.road_code
        G = self.data_mng.get_graph()
        out_nodes = G.get_main_link(nodeid, road_code,
                                    same_code=True, reverse=False)
        if out_nodes:
            if len(out_nodes) == 1:
                return out_nodes[0]
            else:
                self.log.error('Main Out link Number > 1. node=%s'
                               % nodeid)
                return None
        else:
            self.log.error('Main Out link Number = 0. node=%s' % nodeid)
            return None

    def _get_nwy_out_node_add_info(self, node_lid, ignore_mapping=True,
                                   ramp=False):
        '''取得高速相连的普通道，作为附加情报
           @ignore_mapping=True, 包括收集到Hwy mapping的link
           @ramp True: 取Ramp相连的一般道， False: 本线相连的一般道
        '''
        add_link_node = []
        node = node_lid[-1]
        well_flag = False
        G = self.data_mng.get_graph()
        # 取得退出link
        for out_u, out_v in G.out_edges_iter(node):
            link_id = G.get_linkid(out_u, out_v)
            if(ignore_mapping or
               not self.data_mng.is_in_hwy_mapping(link_id)):
                path = [node_lid[-2], node, out_v]
                if G.check_regulation(path, reverse=False):
                    for in_u, in_v in G.in_edges_iter(node):
                        if (in_u, in_v) == (out_v, out_u):  # 折返
                            continue
                        if ramp and self.data_mng.is_in_hwy_mapping(link_id):
                            continue
                        # 该in_link不是当前本线link
                        path = [out_v, in_v, in_u]
                        in_link_id = G.get_linkid(in_u, in_v)
                        if((in_u, in_v) != (node_lid[-2], node) and
                           G.check_regulation(path, reverse=True)):
                            if self.data_mng.is_in_hwy_mapping(in_link_id):
                                # 前方一个设施或无
                                if(self._get_fwd_facil_num(None,
                                                           link_id) <= 1 and
                                   self._get_bwd_facil_num(None,
                                                           link_id) <= 1):
                                    pass
                                else:
                                    well_flag = True
                                    return well_flag, []
                            else:
                                well_flag = True
                                return well_flag, []
                        if not self.data_mng.is_in_hwy_mapping(link_id):
                            add_link_node.append((node, link_id, out_u, out_v))
        return well_flag, add_link_node

    def _get_nwy_in_node_add_info(self, node_lid, ignore_mapping=True,
                                  ramp=False):
        '''取得高速相连的普通道，作为附加情报
           @ignore_mapping=True, 包括收集到Hwy mapping的link
           @ramp True: 取Ramp相连的一般道， False: 本线相连的一般道
        '''
        add_link_node = []
        node = node_lid[-1]
        well_flag = False
        G = self.data_mng.get_graph()
        # 取得进入出link
        for in_u, in_v in G.in_edges_iter(node):
            link_id = G.get_linkid(in_u, in_v)
            if(ignore_mapping or
               not self.data_mng.is_in_hwy_mapping(link_id)):
                path = [node_lid[-2], node, in_u]
                if G.check_regulation(path, reverse=True):
                    for out_u, out_v in G.out_edges_iter(node):
                        if (out_v, out_u) == (in_v, in_u):  # 折返
                            continue
                        if ramp and self.data_mng.is_in_hwy_mapping(link_id):
                            continue
                        path = [in_u, node, out_v]
                        out_link_id = G.get_linkid(in_u, in_v)
                        # 该out_link不是当前本线link,
                        if((out_v, out_u) != (node_lid[-2], node) and
                           G.check_regulation(path, reverse=False)):
                            if self.data_mng.is_in_hwy_mapping(out_link_id):
                                # 后方一个设施或无
                                if(self._get_fwd_facil_num(None,
                                                           link_id) <= 1 and
                                   self._get_bwd_facil_num(None,
                                                           link_id) <= 1):
                                    pass
                                else:
                                    well_flag = True
                                    return well_flag, []
                            else:
                                well_flag = True
                                return well_flag, []
                            well_flag = True
                            return well_flag, []
                        else:
                            if not self.data_mng.is_in_hwy_mapping(link_id):
                                add_link_node.append((node, link_id,
                                                      in_v, in_u))
        return well_flag, add_link_node

    def _get_add_info_link_pos(self, link_lid, end_pos,
                               ic_node, inout, path_type):
        '''附加情报link的位置
           Out(出口): 后方设施要唯一
           In(入口): 前方设施要唯一
        '''
        link_idx = 0
        while link_idx < end_pos:
            link_id = link_lid[link_idx]
            next_idx = link_idx
            if inout == HWY_INOUT_TYPE_OUT:  # 出口
                # 取得link的后方所有设施
                ic_num = self._get_bwd_facil_num(None, link_id, path_type)
                if ic_num == 1:  # link后方就一个facility_id
                    break
                    if self._all_link_is_ic_path(link_lid[next_idx:]):
                        break
                else:
                    ic_num = self._get_bwd_facil_num(ic_node, link_id,
                                                     path_type)
                    if ic_num == 1:
                        # 判断前方设施在同个HWY点上
                        if self._is_fwd_facil_same_node(link_id):
                            break
                            if self._all_link_is_ic_path(link_lid[next_idx:]):
                                break
                    # if self._is_fwd_facil_None(link_id):  # 前方设施为空
                    #    break
            else:  # 入口
                ic_num = self._get_fwd_facil_num(None, link_id, path_type)
                if ic_num == 1:  # link前方就一个facility_id
                    break
                    if self._all_link_is_ic_path(link_lid[next_idx:]):
                        break
                else:
                    pass
                    # 取得link的前方所有设施(并列)
                    ic_num = self._get_fwd_facil_num(ic_node, link_id,
                                                     path_type)
                    if ic_num == 1:  # 无并列
                        if self._is_bwd_ic_same_node(link_id):
                            break
                            if self._all_link_is_ic_path(link_lid[next_idx:]):
                                break
                    # if self._is_bwd_facil_None(link_id):  # 后方设施为空
                    #    break
            link_idx += 1
        return link_idx

    def _all_link_is_ic_path(self, link_lid):
        ic_type = set([HWY_PATH_TYPE_IC])
        for link_id in link_lid:
            path_types = self._get_link_path_type(link_id)
            # path_types.difference_update([HWY_PATH_TYPE_UTURN])
            if path_types == ic_type:
                continue
            else:
                return False
        return True

    def _get_link_path_type(self, link_id):
        bwd_fwd_list = self.data_mng.get_link_fwd_bwd_facil(link_id)
        bwd_list, fwd_list = bwd_fwd_list
        path_type_set = set()
        for bwd in bwd_list:
            path_type = bwd.get('path_type')
            path_type_set.add(path_type)
        for fwd in fwd_list:
            path_type = fwd.get('path_type')
            path_type_set.add(path_type)
        return path_type_set

    def _is_bwd_ic_same_node(self, link_id):
        '''后方多个设施在同个HwyNode点, 或者都是空(出入口)'''
        bwd_fwd_list = self.data_mng.get_link_fwd_bwd_facil(link_id)
        bwd_list = bwd_fwd_list[0]
        bwd_ic_node = set()
        for bwd in bwd_list:
            node_id = bwd.get('node_id')
            bwd_ic_node.add(node_id)
        return len(bwd_ic_node) == 1

    def _is_fwd_facil_same_node(self, link_id):
        '''前方多个设施在同个HwyNode点, 或者都是空(出入口)'''
        bwd_fwd_list = self.data_mng.get_link_fwd_bwd_facil(link_id)
        fwd_list = bwd_fwd_list[1]
        fwd_ic_node = set()
        for fwd in fwd_list:
            node_id = fwd.get('node_id')
            fwd_ic_node.add(node_id)
        return len(fwd_ic_node) == 1

    def _is_fwd_facil_same_cls(self, link_id):
        '''前方设施种别相同'''
        bwd_fwd_list = self.data_mng.get_link_fwd_bwd_facil(link_id)
        fwd_list = bwd_fwd_list[1]
        facil_cls_set = set()
        for fwd in fwd_list:
            ic_no = fwd.get('ic_no')
            if ic_no:
                ic_facil_info = self.data_mng.get_ic(ic_no)
                facil_list = ic_facil_info[2]
                facil_cls_set.add(facil_list[0].facilcls)
            else:
                facil_cls_set.add(None)  # 前方为空：IC出口
        if len(facil_cls_set) <= 1:
            return True

    def _is_bwd_facil_same_cls(self, link_id):
        '''前方设施种别相同'''
        bwd_fwd_list = self.data_mng.get_link_fwd_bwd_facil(link_id)
        bwd_list = bwd_fwd_list[0]
        facil_cls_set = set()
        for fwd in bwd_list:
            ic_no = fwd.get('ic_no')
            if ic_no:
                ic_facil_info = self.data_mng.get_ic(ic_no)
                facil_list = ic_facil_info[2]
                facil_cls_set.add(facil_list[0].facilcls)
            else:
                facil_cls_set.add(None)  # 前方为空：IC出口
        if len(facil_cls_set) <= 1:
            return True

    def _is_fwd_facil_None(self, link_id):
        '''前方设施为空'''
        bwd_fwd_list = self.data_mng.get_link_fwd_bwd_facil(link_id)
        fwd_list = bwd_fwd_list[1]
        for fwd in fwd_list:
            if fwd.get('ic_no'):
                return False
        return True

    def _is_bwd_facil_None(self, link_id):
        '''前方设施为空'''
        bwd_fwd_list = self.data_mng.get_link_fwd_bwd_facil(link_id)
        bwd_list = bwd_fwd_list[0]
        for bwd in bwd_list:
            if bwd.get('ic_no'):
                return False
        return True

    def _is_same_facilcls(self, link_id, reserve=False):
        '''@reserve: False， 该link的前方设施种别相同
           @reserve: True， 该link的后方设施种别相同
        '''
        hwy_data = self.data_mng
        bwd_list, fwd_list = hwy_data.get_link_fwd_bwd_facil(link_id)
        fwd_ic_node = set()
        if reserve:  # 后方
            for bwd in bwd_list:
                node_id = bwd.get('node_id')
                # ic_no, facility_id, facil_list = self.data_mng.get_ic(ic_no)
                fwd_ic_node.add(node_id)
        else:
            for fwd in fwd_list:
                node_id = fwd.get('node_id')
                fwd_ic_node.add(node_id)
        return len(fwd_ic_node) == 1

    def _get_bwd_facil_num(self, ic_node, link_id,
                           path_type=HWY_PATH_TYPE_IC):
        '''后方设施数
           ic_node: 设施的HighWay Node点,
                    ic_node无指定，link后方所有设施数目;
                    ic_node指定，和ic_node同点的后方设施(并列)
           link_id: Path路径上的某一条link
        '''
        facil_id_set = set()
        # # 取得前后方设施
        hwy_data = self.data_mng
        bwd_fwd_list = hwy_data.get_link_fwd_bwd_facil(link_id)
        bwd_list = bwd_fwd_list[0]
        if len(bwd_list) == 1:
            return 1
        # 后方设施数
        for bwd in bwd_list:
            temp_node_id = bwd.get('node_id')
            temp_path_type = bwd.get('path_type')
            temp_facil_id = bwd.get('facility_id')
            if ic_node:  # node指定
                if ic_node != temp_node_id and temp_node_id:
                    continue
                if path_type in (HWY_PATH_TYPE_JCT,):
                    facil_id_set.add(temp_facil_id)
                else:
                    if temp_path_type != HWY_PATH_TYPE_UTURN:
                        facil_id_set.add(temp_facil_id)
            else:  # node无指定
                if path_type in (HWY_PATH_TYPE_JCT,):
                    facil_id_set.add(temp_facil_id)
                else:
                    if temp_path_type != HWY_PATH_TYPE_UTURN:
                        facil_id_set.add(temp_facil_id)
        # ## 测试用代码
#         if bwd_facil_num == 1:
#             temp_num = 0
#             facility_ic_diff = False
#             temp_facility_id = bwd_list[0].get('facility_id')
#             for bwd in bwd_list:
#                 other_facility_id = bwd.get('facility_id')
#                 other_path_type = bwd.get('path_type')
#                 if other_path_type != HWY_PATH_TYPE_UTURN:
#                     temp_num += 1
#                 if temp_facility_id != bwd.get('facility_id'):
#                     facility_ic_diff = True
#             if temp_num > 1 and ic_no_diff:
#                 print ('后方HwyNode点不、番号不同的设施数 > 1. ic_node=%s,link_id=%s'
#                        % (ic_node, link_id))
#             if temp_num > 1 and not facility_ic_diff:  # 这种case没有
#                 print ('后方HwyNode点不同、番号相同的设施数 > 1. ic_node=%s,link_id=%s'
#                        % (facil_id, link_id))
        return len(facil_id_set)

    def _get_fwd_facil_num(self, ic_node, link_id,
                           path_type=HWY_PATH_TYPE_IC):
        '''前方设施数
           ic_node: 设施的HighWay Node点
           link_id: Path路径上的某一条link
           path_type: 路径种别:"IC", "JCT", "SAPA", "UTURN"
        '''
        facil_id_set = set()
        # # 取得前后方设施
        hwy_data = self.data_mng
        bwd_fwd_list = hwy_data.get_link_fwd_bwd_facil(link_id)
        fwd_list = bwd_fwd_list[1]
        if len(fwd_list) == 1:
            return 1
        # 后方设施数
        for fwd in fwd_list:
            temp_node_id = fwd.get('node_id')
            temp_path_type = fwd.get('path_type')
            temp_facil_id = fwd.get('facility_id')
            if ic_node:  # node指定
                if ic_node != temp_node_id and temp_node_id:
                    continue
                if path_type in (HWY_PATH_TYPE_JCT,):
                    facil_id_set.add(temp_facil_id)
                else:
                    if temp_path_type != HWY_PATH_TYPE_UTURN:
                        facil_id_set.add(temp_facil_id)
            else:  # node无指定
                if path_type in (HWY_IC_TYPE_JCT,):
                    facil_id_set.add(temp_facil_id)
                else:
                    if temp_path_type != HWY_PATH_TYPE_UTURN:
                        facil_id_set.add(temp_facil_id)
        return len(facil_id_set)

    def get_sapa_pos(self, node_list, link_list):
        '''取得SAPA所在点的位置(index), 如果没有找到返回-1'''
        hwy_data = self.data_mng
        road_code = self.first_facil.road_code
        road_seq = self.first_facil.road_point
        updown = self.first_facil.updown
        sapa_node, sapa_link = hwy_data.get_sapa_postion(road_code,
                                                         road_seq,
                                                         updown)
        if not sapa_link:  # 没有SAPA POI
            return len(node_list) - 1, None, None
        for pos in range(0, len(link_list)):
            if sapa_link == link_list[pos]:
                return pos, sapa_link, sapa_node
        return -1, None, None


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
                self.log.error('Error: Number of InTile > 1, nodeid=%s'
                               % node_id)
        else:
            self.log.error('Error: No InLink, nodeid=%s' % node_id)
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
                self.log.error('Number of OutTile > 1, nodeid=%s'
                               % node_id)
        else:
            self.log.error('No OutLink, nodeid=%s' % node_id)
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
            (self.data_mng.is_same_facil(prev_facil, next_facil) or
             self._is_identical_facil(prev_facil, next_facil))):
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
            self.log.error('Error: No inlink for tile boundary %s'
                           % node_id)
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
            (self.data_mng.is_same_facil(prev_facil, next_facil) or
             self._is_identical_facil(prev_facil, next_facil))):
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
            self.log.error('No OutLink for tile boundary %s' % node_id)
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
                self.ic_info.log.info('out tile: %s, node=%s'
                                      % (conn_tile_id, conn_node_id))
            else:  # 入口/收费站时，边界点在前(边界点ic_no较大)，所以点的进入link的Tile
                # Get in tile id
                conn_tile_id = self.ic_info._get_in_tile_id(conn_node_id,
                                                            conn_road_code)
                self.ic_info.log.info('in tile: %s, node=%s'
                                      % (conn_tile_id, conn_node_id))
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
        # JCT入口不做料金, 即料金只做在JCT出口
        if self.conn_direction != CONN_DIR_FORWARD:
            return
        self.toll_list = self.ic_info.get_toll_info(self.s_facil,
                                                    self.path,
                                                    self.path_type,
                                                    self.conn_ic_no,
                                                    self.t_facil)

