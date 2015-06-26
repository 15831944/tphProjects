# -*- coding: UTF-8 -*-
'''
Created on 2014-4-11

@author: hongchenzai
'''
from component.jdb.hwy.hwy_def import HWY_INVALID_FACIL_ID
from component.jdb.hwy.hwy_def import HWY_TILE_LAYER
from component.jdb.hwy.hwy_def import TILE_LAYER_14
from component.jdb.hwy.hwy_def import UPDOWN_TYPE_UP
from component.jdb.hwy.hwy_def import UPDOWN_TYPE_DOWN
from component.jdb.hwy.hwy_def import UPDOWN_TYPE_COMMON
from component.jdb.hwy.hwy_def import IC_TYPE_SA
from component.jdb.hwy.hwy_def import IC_TYPE_PA
from component.jdb.hwy.hwy_def import IC_TYPE_JCT
from component.jdb.hwy.hwy_def import IC_TYPE_RAMP
from component.jdb.hwy.hwy_def import IC_TYPE_IC
from component.jdb.hwy.hwy_def import IC_TYPE_TOLL
from component.jdb.hwy.hwy_def import IC_TYPE_SMART_IC
from component.jdb.hwy.hwy_def import INOUT_TYPE_IN
from component.jdb.hwy.hwy_def import INOUT_TYPE_OUT
from component.jdb.hwy.hwy_def import ROAD_ATTR_UP
from component.jdb.hwy.hwy_def import ROAD_ATTR_DOWN
from component.jdb.hwy.hwy_def import ROAD_ATTR_IN
from component.jdb.hwy.hwy_def import ROAD_ATTR_OUT
from component.jdb.hwy.hwy_def import ROAD_ATTR_WEST
from component.jdb.hwy.hwy_def import ROAD_ATTR_EAST
from component.jdb.hwy.hwy_def import IC_TYPE_TRUE
from component.jdb.hwy.hwy_def import IC_DEFAULT_VAL
from component.jdb.hwy.hwy_def import TOLL_FUNC_UTURN_CHECK
from component.jdb.hwy.hwy_data_mng import HwyDataMng
from component.jdb.hwy.hwy_toll_info import HwyTollInfo
from ctypes import c_int
DIRECTION_0 = 0
DIRECTION_1 = 1
DIRECTION_2 = 2
DIRECTION_3 = 3
DIRECTION_4 = 4
DIRECTION_5 = 5
DIRECTION_6 = 6
DIRECTION_7 = 7
DIRECTION_8 = 8
DIRECTION_9 = 9
DIRECTION_10 = 10
DIRECTION_11 = 11
DIRECTION_12 = 12
DIRECTION_13 = 13
DIRECTION_14 = 14
DIRECTION_15 = 15
# 边界点种别
BONUNDARY_TYPE_NONE = 0
BOUDARY_TYPE_IN = 1
BOUNDARY_TYPE_OUT = 2
# 接続方向
CONN_DIR_FORWARD = 0  # 正向
CONN_DIR_REVERSE = 1  # 逆向
# IC出入口附加情报node点和料金所的位置关系种别
ADD_NODE_POS_TYPE_NONE = -1  # 无料金所
ADD_NODE_POS_TYPE_EQUAL = 0  # 刚好在料金所处
ADD_NODE_POS_TYPE_FWD = 1   # 在料金所前方
ADD_NODE_POS_TYPE_BWD = 2  # 在料金所后方
pos_type_name = {ADD_NODE_POS_TYPE_NONE: "无料金所",
                 ADD_NODE_POS_TYPE_EQUAL: "刚好在料金所",
                 ADD_NODE_POS_TYPE_FWD: "在料金所前方",
                 ADD_NODE_POS_TYPE_BWD: "在料金所后方"
                 }


def convert_tile_id(tile_id_14):
    if tile_id_14 is None:
        return None
    SHIFT = TILE_LAYER_14 - HWY_TILE_LAYER
    MASK = (1 << HWY_TILE_LAYER) - 1
    tile_z = HWY_TILE_LAYER
    tile_x = (tile_id_14 >> (TILE_LAYER_14 + SHIFT)) & MASK
    tile_y = (tile_id_14 >> SHIFT) & MASK
    new_tile_id = tile_z << (TILE_LAYER_14 * 2)
    new_tile_id |= (tile_x << TILE_LAYER_14)
    new_tile_id |= tile_y
    sign_tile_id = c_int(new_tile_id).value
    return sign_tile_id


# ==============================================================================
# HwyICInfo
# ==============================================================================
class HwyICInfo(object):
    '''
    classdocs
    '''

    def __init__(self, ic_no, facility_id, facil_list):
        '''
        Constructor
        '''
        self.ic_no = ic_no
        self.updown = UPDOWN_TYPE_COMMON
        self.facility_id = facility_id
        self.facil_list = facil_list
        self.first_facil = facil_list[0]
        self.second_facil = None
        if len(facil_list) > 1:
            self.second_facil = facil_list[1]
        # 施設種別
        self.between_distance = IC_DEFAULT_VAL
        self.inside_distance = IC_DEFAULT_VAL
        self.enter = IC_DEFAULT_VAL
        self.exit = IC_DEFAULT_VAL
        self.tollgate = IC_DEFAULT_VAL
        self.jct = IC_DEFAULT_VAL
        self.pa = IC_DEFAULT_VAL
        self.sa = IC_DEFAULT_VAL
        self.ic = IC_DEFAULT_VAL
        self.ramp = IC_DEFAULT_VAL
        self.smart_ic = IC_DEFAULT_VAL
        self.barrier = IC_DEFAULT_VAL
        self.dummy = IC_DEFAULT_VAL
        self.boundary = IC_DEFAULT_VAL  # Tile境界上施設フラグ
        self.new = IC_DEFAULT_VAL
        self.unopen = IC_DEFAULT_VAL
        # 施設属性
        self.forward_flag = IC_DEFAULT_VAL
        self.reverse_flag = IC_DEFAULT_VAL
        self.toll_count = IC_DEFAULT_VAL
        self.enter_direction = IC_DEFAULT_VAL
        self.path_count = IC_DEFAULT_VAL  # 不包括设施所以位置
        self.path_points = []
        self.vics_count = IC_DEFAULT_VAL
        self.conn_count = IC_DEFAULT_VAL
        self.illust_count = IC_DEFAULT_VAL
        self.store_count = IC_DEFAULT_VAL
        self.servise_flag = IC_DEFAULT_VAL
        self.road_no = IC_DEFAULT_VAL  # 道路番号
        self.conn_tile_id = IC_DEFAULT_VAL
        self.tile_id = IC_DEFAULT_VAL
        self.name = facil_list[0].facil_name
        self.name_yomi = facil_list[0].name_yomi
        self.conn_infos = []
        # (设施信息, link_lid, node_lid, path_type)
        self.out_path_infos = []  # 出口路径信息
        self.in_path_infos = []  # 入口路径信息
        self.toll_infos = []  # 料金情报
        self.add_infos = []  # 附加情报

    def set_ic_info(self):
        self.get_path_info()  # 获取路径情报
        self._set_ic_type()
        self._set_updown()
        self._set_distance()
        self._set_enter_direction()
        self._set_direction_flag()  # 順逆方向継続有無フラグ
        # self._set_vics_info()
        self._set_road_no()
        # self._store_info()
        self._set_tile_id()
        self._set_conn_tile_id()
        self._set_conn_info()
        self._set_path_point_info()
        self._set_toll_info()
        self.set_node_add_info()

    def _set_distance(self):
        hwy_data = HwyDataMng.instance()
        G = hwy_data.get_graph()
        # 本线路径
        main_path = hwy_data.get_main_path(self.facil_list[0].road_code,
                                           self.facil_list[0].updown)
        # 求设施内距离
        if len(self.facil_list) == 2:
            s_node = self.facil_list[0].node_id
            e_node = self.facil_list[1].node_id
            s_idx = main_path.index(s_node)
            e_idx = main_path.index(e_node, (s_idx + 1))
            inside_path = main_path[s_idx:(e_idx + 1)]
            if not inside_path:
                print ('Error: no inside path.s_node=%s, e_node=%s'
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
                hwy_data = HwyDataMng.instance()
                G = hwy_data.get_graph()
                s_idx = main_path.index(main_node)
                e_idx = main_path.index(next_node_id)
                between_path = main_path[s_idx:(e_idx + 1)]
                if not between_path:
                    print 'Error: no between path.s_node=%s, e_node=%s' \
                          % (main_node, next_node_id)
                self.between_distance = G.get_length_for_path(between_path)
        else:
            self.between_distance = 0

    def _set_ic_type(self):
        if self.facility_id == HWY_INVALID_FACIL_ID:
            self.boundary = IC_TYPE_TRUE
        if self.facil_list[0].facilcls == IC_TYPE_SA:
            self.sa = IC_TYPE_TRUE
        elif self.facil_list[0].facilcls == IC_TYPE_PA:
            self.pa = IC_TYPE_TRUE
        elif self.facil_list[0].facilcls == IC_TYPE_JCT:
            self._set_jct_type()
        elif self.facil_list[0].facilcls == IC_TYPE_RAMP:
            if self.facil_list[0].facil_name.find('ＪＣＴ') >= 0:
                self.jct = IC_TYPE_TRUE
            else:
                self.ramp = IC_TYPE_TRUE
        elif self.facil_list[0].facilcls == IC_TYPE_IC:
            if self.facil_list[0].facil_name.find('ＪＣＴ') >= 0:
                self.jct = IC_TYPE_TRUE
            else:
                self.ic = IC_TYPE_TRUE
        elif self.facil_list[0].facilcls == IC_TYPE_SMART_IC:
            self.smart_ic = IC_TYPE_TRUE
        if self.facil_list[0].facilcls == IC_TYPE_TOLL:
            self.tollgate = IC_TYPE_TRUE
            if self.facil_list[0].tollclass == 3:
                self.barrier = IC_TYPE_TRUE
            if self.facil_list[0].dummyfacil:
                self.dummy = IC_TYPE_TRUE

    def _set_jct_type(self):
        if self.first_facil.facilcls != IC_TYPE_JCT:
            return
        if self.first_facil.facil_name.find('ランプ') >= 0:
            self.ramp = IC_TYPE_TRUE
            return
        if(self.first_facil.facil_name.find('ＩＣ') >= 0
           or self.first_facil.facil_name.find('出入口') >= 0):
            hwy_data = HwyDataMng.instance()
            node_id = self.first_facil.node_id
            # 当前设施点是道路的起末点
            if hwy_data.is_road_start_end_node(self.first_facil.road_code,
                                               self.first_facil.updown,
                                               node_id):
                if self._jct_has_toll():  # 有收费站
                    self.ic = IC_TYPE_TRUE
                    return
                # 取得JCT的种别为RAMP并列设施
                ramp_same_facils = self._get_ramp_same()
                # JCT有RAMP并设，无收费站
                if ramp_same_facils:
                    self.ramp = IC_TYPE_TRUE
                    # 把Ramp设施名称赋给JCT
                    self.name = ramp_same_facils[0].facil_name
                    self.name_yomi = ramp_same_facils[0].name_yomi
                    # ## LOG
                    if self.facil_list[0].facil_name.find('出入口') >= 0:
                        print ('Warning: Start/End JCT name is [xxx出入口], '
                               'JCT Name=%s' % self.first_facil.facil_name)
                    if len(ramp_same_facils) > 1:
                        print ('Warning: The Number of Same Ramp of '
                               'Start/End JCT > 1. JCT Name=%s' %
                               self.first_facil.facil_name)
                    return
            self.ic = IC_TYPE_TRUE
            return
        self.jct = IC_TYPE_TRUE

    def _jct_has_toll(self):
        # JCT设施有料金所
        hwy_data = HwyDataMng.instance()
        for facil, link_lid, node_lid, path_type in self.out_path_infos:
            for node_id in node_lid[1:-1]:
                if hwy_data.is_toll_node(node_id):
                    return True
        for facil, link_lid, node_lid, path_type in self.in_path_infos:
            for node_id in node_lid[1:-1]:
                if hwy_data.is_toll_node(node_id):
                    return True
        return False

    def _get_ramp_same(self):
        '''取得JCT的种别是RAMP的并列设施'''
        rst_facils = []
        if self.facil_list[0].facilcls != IC_TYPE_JCT:
            return rst_facils
        hwy_data = HwyDataMng.instance()
        # get facilities of this node
        node_id = self.facil_list[0].node_id
        facil_infos = hwy_data.get_hwy_facils_by_nodeid(node_id)
        for facil in facil_infos:
            # 种别为Ramp, 名称不是'xxxJCT'
            if(facil.facilcls == IC_TYPE_RAMP
               and facil.facil_name.find('ＪＣＴ') < 0):
                rst_facils.append(facil)
        return rst_facils

    def _check_boundary(self, facil):
        if facil:
            if facil.facility_id == HWY_INVALID_FACIL_ID:
                return True
        return False

    def _set_updown(self):
        # 0…下り、1…上り、2…RESERVED、3…上下線共有
        if self.facil_list[0].updown == UPDOWN_TYPE_UP:
            self.updown = 1
        elif self.facil_list[0].updown == UPDOWN_TYPE_DOWN:
            self.updown = 0
        elif self.facil_list[0].updown == UPDOWN_TYPE_COMMON:
            self.updown = UPDOWN_TYPE_COMMON
        else:
            self.updown = None

    def _set_direction_flag(self):
        '''順逆方向継続有無フラグ'''
        self._set_forward_flag()
        self._set_reverse_flag()

    def _set_forward_flag(self):
        hwy_data = HwyDataMng.instance()
        next_ic_no = self.ic_no + 1
        next_facil = self._get_facil(next_ic_no)  # Get Next IC
        while self._check_boundary(next_facil):  # 跳过边界点
            next_ic_no += 1
            next_facil = self._get_facil(next_ic_no)
        if (next_facil and
            (hwy_data.is_same_facil(self.facil_list[0], next_facil)
             or self._is_identical_facil(self.facil_list[0], next_facil))):
            self.forward_flag = IC_TYPE_TRUE
        else:
            self.forward_flag = IC_DEFAULT_VAL

    def _set_reverse_flag(self):
        hwy_data = HwyDataMng.instance()
        prev_ic_no = self.ic_no - 1
        prev_facil = self._get_facil(prev_ic_no)
        while self._check_boundary(prev_facil):  # 跳过边界点
            prev_ic_no -= 1
            prev_facil = self._get_facil(prev_ic_no)
        if (prev_facil and
            (hwy_data.is_same_facil(self.facil_list[0], prev_facil)
             or self._is_identical_facil(self.facil_list[0], prev_facil))):
            self.reverse_flag = IC_TYPE_TRUE
        else:
            self.reverse_flag = IC_DEFAULT_VAL

    def _get_facil(self, ic_no):
        hwy_data = HwyDataMng.instance()
        ic_no, facility_id, facil_list = hwy_data.get_ic(ic_no)
        if not ic_no:
            return None
        facil = facil_list[0]
        if (facil.road_code == self.facil_list[0].road_code
            and facil.updown == self.facil_list[0].updown):
            return facil
        return None

    def _is_identical_facil(self, prev_facil, next_facil):
        '''同个设施(包括出入口配对，连续两个出口，连续的两个入口)'''
        if not prev_facil and not next_facil:
            return False
        if (prev_facil.road_code == next_facil.road_code and
            prev_facil.road_point == next_facil.road_point and
            prev_facil.facilcls == next_facil.facilcls and
            prev_facil.updown == next_facil.updown):
            return True
        return False

    def _set_enter_direction(self):
        '''进入方位'''
        hwy_data = HwyDataMng.instance()
        G = hwy_data.get_graph()
        node_id = self.facil_list[0].node_id
        road_code = self.facil_list[0].road_code
        angle = G.get_in_angle(node_id, road_code)
        if not angle:
            angle = G.get_out_angle(node_id, road_code)
        if angle:
            # 逆时针==>顺时针
            angle = 360 - angle
            # 水平x转==>正北的夹角
            angle += 90
            while angle >= 360:
                angle -= 360
            if angle > 348.75 or angle <= 11.25:
                self.enter_direction = DIRECTION_0
            elif angle > 11.25 and angle <= 33.75:
                self.enter_direction = DIRECTION_1
            elif angle > 33.75 and angle <= 56.25:
                self.enter_direction = DIRECTION_2
            elif angle > 56.25 and angle <= 78.75:
                self.enter_direction = DIRECTION_3
            elif angle > 78.75 and angle <= 101.25:
                self.enter_direction = DIRECTION_4
            elif angle > 101.25 and angle <= 123.75:
                self.enter_direction = DIRECTION_5
            elif angle > 101.25 and angle <= 146.25:
                self.enter_direction = DIRECTION_6
            elif angle > 146.25 and angle <= 168.75:
                self.enter_direction = DIRECTION_7
            elif angle > 168.75 and angle <= 191.25:
                self.enter_direction = DIRECTION_8
            elif angle > 191.25 and angle <= 213.75:
                self.enter_direction = DIRECTION_9
            elif angle > 213.75 and angle <= 236.25:
                self.enter_direction = DIRECTION_10
            elif angle > 236.25 and angle <= 258.75:
                self.enter_direction = DIRECTION_11
            elif angle > 258.75 and angle <= 281.25:
                self.enter_direction = DIRECTION_12
            elif angle > 281.25 and angle <= 303.75:
                self.enter_direction = DIRECTION_13
            elif angle > 303.75 and angle <= 326.25:
                self.enter_direction = DIRECTION_14
            elif angle > 326.25 and angle <= 348.75:
                self.enter_direction = DIRECTION_15
            else:
                self.enter_direction = None
        else:
            self.enter_direction = None

    def get_conn_info(self):
        return self.conn_infos

    def get_path_info(self):
        '''获取路径情报。'''
        hwy_data = HwyDataMng.instance()
        if(self.facility_id == HWY_INVALID_FACIL_ID  # Tile边界
           or self.facil_list[0].facilcls == IC_TYPE_TOLL):  # 收费站
            return
        for facil in self.facil_list:
            if facil.inout == INOUT_TYPE_OUT:
                self.out_path_infos += hwy_data.get_ic_path_info(facil)
            elif facil.inout == INOUT_TYPE_IN:
                self.in_path_infos += hwy_data.get_ic_path_info(facil)
            else:
                print 'No Path. node=%s' % facil.node_id

    def _set_conn_info(self):
        '''接続情報'''
        hwy_data = HwyDataMng.instance()
        for facil in self.facil_list:
            conn_info_list = hwy_data.get_conn_infos(facil)
            for conn_facil, path, path_type in conn_info_list:
                hwy_conn = HwyConnInfo(self, self.tile_id,
                                       facil, conn_facil,
                                       path, path_type)
                hwy_conn.set_conn_info()
                self._add_conn_info(hwy_conn)
        self.conn_count = len(self.conn_infos)

    def _add_conn_info(self, conn_info):
        '''添加接续情报，有多条路径时只取得距离最短的.'''
        exist_flag = False
        for conn_idx in range(0, len(self.conn_infos)):
            temp_conn = self.conn_infos[conn_idx]
            if (temp_conn.conn_ic_no == conn_info.conn_ic_no and
                temp_conn.conn_direction == conn_info.conn_direction):
                if temp_conn.conn_length > conn_info.conn_length:
                    self.conn_infos.pop(conn_idx)
                    self.conn_infos.append(conn_info)
                    return
                exist_flag = True
                break
        if not exist_flag:
            self.conn_infos.append(conn_info)

    def _set_road_no(self):
        hwy_data = HwyDataMng.instance()
        self.road_no = hwy_data.get_road_no(self.facil_list[0].road_code)

    def _set_name(self):
        self.name = self.facil_list[0].facil_name

    def _set_conn_tile_id(self):
        self.conn_tile_id = self._get_conn_tile_id()

    def _get_conn_tile_id(self):
        '''接续先Tile_id'''
        return 0

    def _set_tile_id(self):
        self.tile_id = self._get_tile_id()

    def _get_tile_id(self):
        node_id = self.facil_list[0].node_id
        road_code = self.first_facil.road_code
        hwy_data = HwyDataMng.instance()
        # 点是处在tile边界
        if hwy_data.is_boundary_node(node_id):
            # 出口时，边界点在后(边界点ic_no较小)，所以用点脱出link的Tile
            if self.first_facil.inout == INOUT_TYPE_OUT:
                # get out tile id
                tile_id = self._get_out_tile_id(node_id, road_code)
                print ('out tile: %s, node=%s' % (tile_id, node_id))
            else:  # 入口/收费站时，边界点在前(边界点ic_no较大)，所以点的进入link的Tile
                # Get in tile id
                tile_id = self._get_in_tile_id(node_id, road_code)
                print ('in tile: %s, node=%s' % (tile_id, node_id))
        else:
            updown = self.first_facil.updown
            # 起末点处，不做Tile边界点，即使该处在边界上
            # 道路起点，用out_link所在tile
            if hwy_data.is_road_start_node(road_code, updown, node_id):
                tile_id = self._get_out_tile_id(node_id, road_code)
            # 道路终点，用in_link所在tile
            elif hwy_data.is_road_end_node(road_code, updown, node_id):
                tile_id = self._get_in_tile_id(node_id, road_code)
            else:
                G = hwy_data.get_graph()
                tile_id = G.get_node_tile(node_id)
        return convert_tile_id(tile_id)

    def _get_in_tile_id(self, node_id, road_code):
        '''node点进入本线link的Tile.'''
        hwy_data = HwyDataMng.instance()
        G = hwy_data.get_graph()
        in_tiles = G.get_in_tile(node_id, road_code)
        if in_tiles:
            if len(in_tiles) == 1:
                return in_tiles[0]
            else:
                print 'Error: Number of InTile > 1, nodeid=%s' % node_id
        else:
            print 'Error: No InLink, nodeid=%s' % node_id
        return None

    def _get_out_tile_id(self, node_id, road_code):
        '''node点脱出本线link的Tile.'''
        hwy_data = HwyDataMng.instance()
        G = hwy_data.get_graph()
        # outlinks = G.get_main_outlinkids(node_id)
        out_tiles = G.get_out_tile(node_id, road_code)
        if out_tiles:
            if len(out_tiles) == 1:
                return out_tiles[0]
            else:
                print 'Error: Number of OutTile > 1, nodeid=%s' % node_id
        else:
            print 'Error: No OutLink, nodeid=%s' % node_id
        return None

    def get_path_point_info(self):
        return self.path_points

    def _set_path_point_info(self):
        '''探索地点情報'''
        exit_path_point = self._get_exit_path_point()
        if exit_path_point:
            self._set_exit_flag(IC_TYPE_TRUE)
            self.path_points += exit_path_point
        enter_path_point = self._get_enter_path_point()
        if enter_path_point:
            self._set_enter_flag(IC_TYPE_TRUE)
            self.path_points += enter_path_point
        self.path_points += self._get_main_path_point()
        self.path_points += self._get_center_path_point()
        # 数目不包含设施所在点
        self.path_count = len(self.path_points)
        self.path_points += self._get_position_path_point()

    def _set_toll_info(self):
        hwy_data = HwyDataMng.instance()
        if self.tollgate == IC_TYPE_TRUE:  # 本线料金
            node = self.first_facil.node_id
            toll_facil_infos = hwy_data.get_toll_facil(node)
            if not toll_facil_infos:
                print 'Error: No Toll Facility Info.'
            if len(toll_facil_infos) > 1:
                print 'Error: Number of Toll Facility > 1.'
            toll_info = HwyTollInfo(self, toll_facil_infos[0],
                                    self.first_facil, None)
            toll_info.set_toll_info()
            self.toll_infos.append(toll_info)
            return
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
        for facil, link_lid, node_lid, path_type in self.out_path_infos:
            if path_type in ('JCT', 'UTURN'):  # 接续料金, 放到接续情报处理
                continue
            toll_list = self.get_toll_info(self.first_facil, node_lid,
                                           path_type)
            for toll_info in toll_list:
                self.add_toll_info(out_toll_list, toll_info)
        return out_toll_list

    def _get_in_toll(self):
        in_toll_list = []
        if self.second_facil:
            in_facil = self.second_facil
        else:
            in_facil = self.first_facil
        # 出口料金
        for facil, link_lid, node_lid, path_type in self.in_path_infos:
            if path_type in ('JCT', 'UTURN'):  # 接续料金, 放到接续情报处理
                continue
            toll_list = self.get_toll_info(in_facil, node_lid,
                                           path_type)
            for toll_info in toll_list:
                self.add_toll_info(in_toll_list, toll_info)
        return in_toll_list

    def _get_near_sapa_in_out(self, toll_node, sapa_facil):
        '''收费站靠近SAPA的出口，还是入口
        INOUT_TYPE_IN: 离SAPA入口(合流)近
        INOUT_TYPE_OUT: 离出口(分歧)近
        None: 出错
        '''
        if sapa_facil.facilcls not in (IC_TYPE_SA, IC_TYPE_PA):
            return None
        # 取得SAPA设施所在位置点
        hwy_data = HwyDataMng.instance()
        position_nodes = hwy_data.get_position_path_point(sapa_facil)
        if not position_nodes or len(position_nodes) > 1:
            return None
        position_node = position_nodes[0]
        if sapa_facil.inout == INOUT_TYPE_OUT:  # 出口・分岐
            path_infos = self.out_path_infos
        else:
            path_infos = self.in_path_infos
        for facil, link_lid, node_lid, path_type in path_infos:
            if path_type != 'SAPA':
                continue
            if (position_node in node_lid) and (toll_node in node_lid):
                pos_idx = node_lid.index(position_node)
                toll_idx = node_lid.index(toll_node)
                if toll_idx > pos_idx:
                    return INOUT_TYPE_IN
                else:
                    return INOUT_TYPE_OUT
        return None

    def _judge_sapa_toll(self, toll_facils, sapa_facil, path_type='SAPA'):
        if path_type != 'SAPA':
            return []
        rst_toll_facils = []
        for toll_facil in toll_facils:
            toll_node = toll_facil.node_id
            inout_type = self._get_near_sapa_in_out(toll_node, sapa_facil)
            if (inout_type == INOUT_TYPE_IN and
                sapa_facil.inout == INOUT_TYPE_IN):
                rst_toll_facils.append(toll_facil)
            elif (inout_type == INOUT_TYPE_OUT and
                  sapa_facil.inout == INOUT_TYPE_OUT):
                rst_toll_facils.append(toll_facil)
        return rst_toll_facils

    def get_toll_info(self, s_facil, path, path_type,
                      conn_ic_no=None, conn_facil=None):
        toll_list = []
        toll_facils = []
        hwy_data = HwyDataMng.instance()
        for node_id in path:
            if hwy_data.is_toll_node(node_id):
                toll_facil = self._get_toll_facil(s_facil, node_id, conn_facil)
                if not toll_facil:
                    print 'Error: No Toll Facility Info. node=%s' % node_id
                    continue
                if self.is_check_toll(toll_facil):  # 检札所
                    if path_type == 'UTURN':
                        toll_facils.append(toll_facil)
                    else:  # IC,JCT出入口
                        toll_facils = []
                        break
                else:
                    toll_facils.append(toll_facil)
        # SAPA内的收费站
        if path_type == 'SAPA' and toll_facils:
            # case: 永福ＰＡ
            toll_facils = self._judge_sapa_toll(toll_facils, s_facil,
                                                path_type)
#             print ('Toll in SAPA. node=%s, name=%s'
#                    % (s_facil.node_id, s_facil.facil_name))
        if len(toll_facils) == 1:
            toll_info = HwyTollInfo(self, toll_facils[0],
                                    s_facil, path_type,
                                    conn_ic_no, conn_facil)
            toll_info.set_toll_info()
            self.add_toll_info(toll_list, toll_info)
        else:
            dummy_toll_node = None
            # 取得dummy toll
            dummy_facils = self._get_dummy_toll_facill(toll_facils)
            if dummy_facils:
                dummy_toll_node = dummy_facils[0].node_id
                # 判断dummy和非dummy的番号相同
                if not self._check_dummy_toll_facil(toll_facils, dummy_facils):
                    print ('Error: Dummy番号不等非Dummy番号. node=%s, name=%s'
                           % (s_facil.node_id, s_facil.facil_name))
                if len(dummy_facils) > 1:
                    print ('Error: Dummy toll > 1. node=%s, name=%s'
                           % (s_facil.node_id, s_facil.facil_name))
            for toll_facil in toll_facils:
                if toll_facil.dummytoll != IC_TYPE_TRUE:
                    toll_info = HwyTollInfo(self, toll_facil,
                                            s_facil, path_type,
                                            conn_ic_no, conn_facil,
                                            dummy_toll_node)
                    toll_info.set_toll_info()
                    self.add_toll_info(toll_list, toll_info)
#             if len(toll_facils) > 2:
#                 print 'Toll Number > 2. node=%s' % self.first_facil.node_id
        return toll_list

    def _get_dummy_toll_facill(self, toll_facils):
        dummy_facils = []
        for facil in toll_facils:
            if facil.dummytoll == IC_TYPE_TRUE:
                dummy_facils.append(facil)
        return dummy_facils

    def _check_dummy_toll_facil(self, toll_facils, dummy_facils):
        '''判断dummy和非dummy的番号相同。'''
        for dummy_facil in dummy_facils:
            for toll_facil in toll_facils:
                if (toll_facil.road_code != dummy_facil.road_code or
                    toll_facil.road_point != dummy_facil.road_point):
                    print ('Dummy料金所和非Dummy料金所番号不同:'
                           'Dummy_node=%s, N_Dummy_node=%s,'
                           'Dummy_code=%s, N_Dummy_code=%s,'
                           'Dummy_seq=%s, N_Dummy_seq=%s' %
                        (toll_facil.node_id, dummy_facil.node_id,
                         toll_facil.road_code, dummy_facil.road_code,
                         toll_facil.road_point, dummy_facil.road_point
                        ))
                    return False
        return True

    def is_check_toll(self, toll_info):
        '''检札所(Uターン用検索機能)'''
        if not toll_info:
            return False
        return toll_info.tollfunc == TOLL_FUNC_UTURN_CHECK

    def add_toll_info(self, toll_list, toll_info):
        for toll in toll_list:
            if toll.node_id == toll_info.node_id:
                if toll.facility_id != toll_info.facility_id:
                    print 'Facility_id is not equal.'
                return
        toll_list.append(toll_info)

    def _get_toll_facil(self, facil, node_id, conn_facil=None):
        hwy_data = HwyDataMng.instance()
        toll_facil_infos = hwy_data.get_toll_facil(node_id)
        temp_toll_list = []
        if len(toll_facil_infos) == 1:
            return toll_facil_infos[0]
        else:
            temp_toll_list = self._compare_toll_facil_no(toll_facil_infos,
                                                         [facil])
            # 和接续设施番号一致的料金
            if not temp_toll_list and conn_facil:
                temp_toll_list = self._compare_toll_facil_no(toll_facil_infos,
                                                             [conn_facil])
            if not temp_toll_list:
                # 同个设施点的所有设施
                same_facils = hwy_data.get_hwy_facils_by_nodeid(facil.node_id)
                temp_toll_list = self._compare_toll_facil_no(toll_facil_infos,
                                                             same_facils)
            if not temp_toll_list and conn_facil:
                # 同个设施点的所有设施(JCT IN)
                conn_node = conn_facil.node_id
                same_facils = hwy_data.get_hwy_facils_by_nodeid(conn_node)
                temp_toll_list = self._compare_toll_facil_no(toll_facil_infos,
                                                             same_facils)
            if len(temp_toll_list) > 1:
                print 'Toll > 1, node=%s' % facil.node_id
            if temp_toll_list:
                return temp_toll_list[0]
        return None

    def _compare_toll_facil_no(self, toll_facil_list, facil_list):
        '''比较料金和设施的番号'''
        temp_toll_list = []
        for toll_facil in toll_facil_list:
            for facil in facil_list:
                if (facil.road_code == toll_facil.road_code and
                    facil.road_point == toll_facil.road_point):
                    temp_toll_list.append(toll_facil)
        return temp_toll_list

    def _set_enter_flag(self, enter_flag=IC_TYPE_TRUE):
        '''入口'''
        facil_cls = self.facil_list[0].facilcls
        if facil_cls in (IC_TYPE_IC, IC_TYPE_RAMP, IC_TYPE_SMART_IC):
            self.enter = enter_flag
        else:
            self.enter = IC_DEFAULT_VAL

    def _set_exit_flag(self, exit_flag=IC_TYPE_TRUE):
        '''出口'''
        facil_cls = self.facil_list[0].facilcls
        if facil_cls in (IC_TYPE_IC, IC_TYPE_RAMP, IC_TYPE_SMART_IC):
            self.exit = exit_flag
        else:
            self.exit = IC_DEFAULT_VAL

    def _get_exit_path_point(self):
        exit_list = []
        node_link_list = []
        for facil, link_lid, node_lid, path_type in self.out_path_infos:
            node = node_lid[1]
            link = link_lid[0]
            if (node, link) not in node_link_list:
#                 if path_type == 'UTURN':
#                     print ('UTURN Exit Path Point.'
#                            'facilname=%s,node=%s,link=%s'
#                            % (self.name, node, link))
                node_link_list.append((node, link))
        for (node, link) in node_link_list:
            point = HwyPathPoint(self, link, node)
            point.exit_flag = IC_TYPE_TRUE
            exit_list.append(point)
        return exit_list

    def _get_enter_path_point(self):
        facil = None
        enter_list = []
        node_link_list = []
        for facil, link_lid, node_lid, path_type in self.in_path_infos:
            if path_type == 'IC':
                node = node_lid[1]
                link = link_lid[0]
            else:  # JCT/SAPA/UTURN等
                node = node_lid[-2]
                link = link_lid[-1]
            if (node, link) not in node_link_list:
#                 if path_type == 'UTURN':
#                     print ('UTURN Enter PathPoint.'
#                            'facilname=%s,node=%s,link=%s'
#                            % (self.name, node, link))
                node_link_list.append((node, link))
        for (node, link) in node_link_list:
            point = HwyPathPoint(self, link, node)
            point.enter_flag = IC_TYPE_TRUE
            enter_list.append(point)
        return enter_list

    def _get_main_path_point(self):
        '''本线点：退出设施的本线link'''
        hwy_data = HwyDataMng.instance()
        G = hwy_data.get_graph()
        if self.second_facil:
            main_node = self.second_facil.node_id
        else:
            main_node = self.first_facil.node_id
        inlinks = G.get_main_outlinkids(main_node, self.first_facil.road_code)
        if inlinks:
            point = HwyPathPoint(self, inlinks[0], main_node)
            point.main_flag = IC_TYPE_TRUE
            return [point]
        return []

    def _get_center_path_point(self):
        '''表示点：进入设施的本线link'''
        hwy_data = HwyDataMng.instance()
        G = hwy_data.get_graph()
        node_id = self.first_facil.node_id
        road_code = self.first_facil.road_code
        inlinks = G.get_main_inlinkids(node_id, road_code)
        if inlinks:
            point = HwyPathPoint(self, inlinks[0], node_id)
            point.center_flag = IC_TYPE_TRUE
            return [point]
        else:
            return []

    def _get_position_path_point(self):
        '''设施所在位置(IC出入口、SAPA。 但是JCT的位置，复用JCT出入口Path Point)'''
        if self.first_facil.facilcls == IC_TYPE_JCT:
            return []
        pos_list = []
        hwy_data = HwyDataMng.instance()
        # 第一个设施点
        pos_points = hwy_data.get_position_path_point(self.first_facil)
        for node_id in pos_points:
            if not node_id:
                node_id = self.first_facil.node_id
                # case: 八草東ＩＣ
#                 print ('Warning: 没有设施所在位置. node=%s,name=%s'
#                        % (node_id, self.first_facil.facil_name))
            point = HwyPathPoint(self, None, node_id)
            point.pos_flag = IC_TYPE_TRUE
            if self.first_facil.inout == INOUT_TYPE_OUT:
                point.exit_flag = IC_TYPE_TRUE
            elif self.first_facil.inout == INOUT_TYPE_IN:
                point.enter_flag = IC_TYPE_TRUE
            pos_list.append(point)
        # 第二个设施点
        if self.second_facil:
            pos_points = hwy_data.get_position_path_point(self.second_facil)
            for node_id in pos_points:
                if not node_id:
                    node_id = self.first_facil.node_id
                    print 'Warning: No Position Point. node=%s' % node_id
                point = HwyPathPoint(self, None, node_id)
                point.pos_flag = IC_TYPE_TRUE
                if self.second_facil.inout == INOUT_TYPE_OUT:
                    point.exit_flag = IC_TYPE_TRUE
                elif self.second_facil.inout == INOUT_TYPE_IN:
                    point.enter_flag = IC_TYPE_TRUE
                pos_list.append(point)
        return pos_list

    def get_all_toll_info(self):
        toll_list = self.toll_infos
        for conn in self.conn_infos:
            toll_list += conn.get_toll_info()
        return toll_list

    def set_node_add_info(self):
        '''制作 临时附加情报表：并切短Path'''
        out_add_infos = self._get_out_node_add_info()
        in_add_infos = self._get_in_node_add_info()
        self.add_infos = out_add_infos + in_add_infos

    def _get_out_node_add_info(self):
        '''设施出口附加情报'''
        hwy_data = HwyDataMng.instance()
        G = hwy_data.get_graph()
        ic_node = self.first_facil.node_id
        add_info_list = []
        for facil, link_lid, node_lid, path_type in self.out_path_infos:
            # SAPA设施的IC路径， 如："亀山ＰＡ"
            if(self.first_facil.facilcls in (IC_TYPE_SA, IC_TYPE_PA) and
               path_type == 'IC'):
                if self.first_facil.facil_name != '亀山ＰＡ':
                    print 'SAPA设施的IC路径. name=%s' % self.first_facil.facil_name
                continue
            if path_type == 'UTURN':
                if len(self.out_path_infos) > 1:
                    continue
                else:  # UTURN， 是该设施的唯一路径
                    if self.first_facil.facilcls in (IC_TYPE_SA, IC_TYPE_PA):
                        path_type = 'SAPA'
                    else:
                        print 'Error: Unknown UTURN Path. node_id=%s' % ic_node
            end_pos = len(link_lid)
            if path_type == 'SAPA':
                sapa_pos = self.get_sapa_pos(node_lid)
                if sapa_pos < 0:
                    continue
                else:
                    end_pos = sapa_pos
            add_info = {}
            inout = INOUT_TYPE_OUT
            link_pos = self._get_add_info_link_pos(link_lid, end_pos,
                                                   ic_node, inout, path_type)
            if link_pos == len(link_lid):
                outlinkids = G.get_main_outlinkids(node_lid[-1])
                if outlinkids:
                    out_link = outlinkids[0]
                    last_node = G.get_another_nodeid(node_lid[-1], out_link)
                else:
                    print 'Error: None Out Link.'
                    continue
                temp_link_lid = link_lid + (out_link,)
                temp_node_lid = node_lid + (last_node,)
                add_info['add_link'] = out_link
                add_info['add_node'] = node_lid[-1]
            else:
                temp_link_lid = link_lid
                temp_node_lid = node_lid
                add_info['add_link'] = link_lid[link_pos]
                add_info['add_node'] = node_lid[link_pos]
            add_info['link_lid'] = ','.join([str(l) for l in temp_link_lid])
            add_info['node_lid'] = ','.join([str(n) for n in temp_node_lid])
            add_info['in_out'] = inout
            # ## 求附加情报点和料金的关系
            pos_type = self._get_pos_type(node_lid, link_pos)
            add_info['pos_type'] = pos_type
            add_info['pos_type_name'] = pos_type_name.get(pos_type)
            add_info['facilcls'] = self.facil_list[0].facilcls
            add_info_list.append(add_info)
        return add_info_list

    def _get_in_node_add_info(self):
        '''设施入口附加情报'''
        # 入口设施hwy_node
        if self.second_facil:
            ic_node = self.second_facil.node_id
        else:
            ic_node = self.first_facil.node_id
        add_info_list = []
        # 出口的附加情报
        for facil, link_lid, node_lid, path_type in self.in_path_infos:
            if path_type != 'IC':
                continue
            #link_lid = link_lid[::-1]  # 改成逆车流方向
            #node_lid = node_lid[::-1]
            add_info = {}
            end_pos = len(link_lid)
            inout = INOUT_TYPE_IN
            link_pos = self._get_add_info_link_pos(link_lid, end_pos,
                                                   ic_node, inout, path_type)
            if link_pos >= end_pos:
                link_pos = end_pos - 1
            add_info['add_link'] = link_lid[link_pos]
            add_info['add_node'] = node_lid[link_pos]
            add_info['link_lid'] = ','.join([str(l) for l in link_lid])
            add_info['node_lid'] = ','.join([str(n) for n in node_lid])
            add_info['in_out'] = INOUT_TYPE_IN
            # ## 求附加情报点和料金的关系
            pos_type = self._get_pos_type(node_lid, link_pos)
            add_info['pos_type'] = pos_type
            add_info['pos_type_name'] = pos_type_name.get(pos_type)
            add_info['facilcls'] = self.facil_list[0].facilcls
            add_info_list.append(add_info)
        return add_info_list

    def _get_add_info_link_pos(self, link_lid, end_pos,
                               ic_node, inout, path_type):
        '''附加情报link的位置
           Out(出口): 后方设施要唯一
           In(入口): 前方设施要唯一
        '''
        link_idx = 0
        while link_idx < end_pos:
            link_id = link_lid[link_idx]
            if inout == INOUT_TYPE_OUT:  # 出口
                # 取得link的后方所有设施
                ic_num = self._get_bwd_facil_num(None, link_id, path_type)
                if ic_num == 1:  # link后方就一个facility_id
                    break
                else:
                    # 取得link的后方所有设施(同点即并列)
                    ic_num = self._get_bwd_facil_num(ic_node,
                                                     link_id,
                                                     path_type)
                    if ic_num == 1:  # 无并列
                        # 判断前方设施在同个HWY点上
                        if self._is_fwd_facil_same_node(link_id):
                            #print 'fwd_ic_same_node, link_id=%s' % link_id
                            break
            else:  # 入口
                ic_num = self._get_fwd_facil_num(None, link_id)
                if ic_num == 1:  # link前方就一个facility_id
                    break
                else:
                    # 取得link的前方所有设施(并列)
                    ic_num = self._get_fwd_facil_num(ic_node, link_id)
                    if ic_num == 1:  # 无并列
                        if self._is_bwd_ic_same_node(link_id):
                            print 'bwd_ic_same_node, link_id=%s' % link_id
                            break
            link_idx += 1
        return link_idx

    def _get_pos_type(self, path, add_link_pos):
        # ## 求附加情报点和料金的关系
        pos_type = ADD_NODE_POS_TYPE_NONE  # 无料金所
        toll_pos = self._get_toll_node_pos(path)
        if toll_pos >= 0:
            toll_link_pos = toll_pos
            if add_link_pos < toll_link_pos:
                pos_type = ADD_NODE_POS_TYPE_BWD
            elif add_link_pos == toll_link_pos:
                pos_type = ADD_NODE_POS_TYPE_EQUAL
            else:
                pos_type = ADD_NODE_POS_TYPE_FWD
        return pos_type

    def _get_toll_node_pos(self, path):
        '''取得料金所在路径的位置'''
        if len(self.toll_infos) == 0:
            return -1
        for node_idx in range(0, len(path)):
            for toll_info in self.toll_infos:
                if path[node_idx] == toll_info.node_id:
                    return node_idx
        # 接续料金
        for node_idx in range(0, len(path)):
            for conn_info in self.conn_infos:
                toll_list = conn_info.get_toll_info()
                for toll_info in toll_list:
                    if path[node_idx] == toll_info.node_id:
                        return node_idx
        return -1

    def _get_bwd_facil_num(self, ic_node, link_id, path_type):
        '''后方设施数
           ic_node: 设施的HighWay Node点,
                    ic_node无指定，link后方所有设施数目;
                    ic_node指定，和ic_node同点的后方设施(并列)
           link_id: Path路径上的某一条link
        '''
        facil_id_set = set()
        # # 取得前后方设施
        hwy_data = HwyDataMng.instance()
        bwd_list, fwd_list = hwy_data.get_link_fwd_bwd_facil(link_id)
        if len(bwd_list) == 1:
            return 1
        # 后方设施数
        for bwd in bwd_list:
            temp_node_id = bwd.get('node_id')
            temp_path_type = bwd.get('path_type')
            temp_facil_id = bwd.get('facility_id')
            if ic_node:  # node指定
                if ic_node != temp_node_id:
                    continue
                if path_type in ('JCT'):
                    facil_id_set.add(temp_facil_id)
                else:
                    if temp_path_type != 'UTURN':
                        facil_id_set.add(temp_facil_id)
            else:  # node无指定
                if path_type in ('JCT'):
                    facil_id_set.add(temp_facil_id)
                else:
                    if temp_path_type != 'UTURN':
                        facil_id_set.add(temp_facil_id)
        # ## 测试用代码
#         if bwd_facil_num == 1:
#             temp_num = 0
#             facility_ic_diff = False
#             temp_facility_id = bwd_list[0].get('facility_id')
#             for bwd in bwd_list:
#                 other_facility_id = bwd.get('facility_id')
#                 other_path_type = bwd.get('path_type')
#                 if other_path_type != 'UTURN':
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

    def _get_fwd_facil_num(self, ic_node, link_id, path_type="IC"):
        '''前方设施数
           ic_node: 设施的HighWay Node点
           link_id: Path路径上的某一条link
           path_type: 路径种别:"IC", "JCT", "SAPA", "UTURN"
        '''
        facil_id_set = set()
        # # 取得前后方设施
        hwy_data = HwyDataMng.instance()
        bwd_list, fwd_list = hwy_data.get_link_fwd_bwd_facil(link_id)
        if len(bwd_list) == 1:
            return 1
        # 后方设施数
        for bwd in bwd_list:
            temp_node_id = bwd.get('node_id')
            temp_path_type = bwd.get('path_type')
            temp_facil_id = bwd.get('facility_id')
            if ic_node:  # node指定
                if ic_node != temp_node_id:
                    continue
                if path_type in ('JCT'):
                    facil_id_set.add(temp_facil_id)
                else:
                    if temp_path_type != 'UTURN':
                        facil_id_set.add(temp_facil_id)
            else:  # node无指定
                if path_type in ('JCT'):
                    facil_id_set.add(temp_facil_id)
                else:
                    if temp_path_type != 'UTURN':
                        facil_id_set.add(temp_facil_id)
        return len(facil_id_set)

    def _is_fwd_facil_same_node(self, link_id):
        '''前方多个设施在同个HwyNode点, 或者都是空(出入口)'''
        hwy_data = HwyDataMng.instance()
        bwd_list, fwd_list = hwy_data.get_link_fwd_bwd_facil(link_id)
        fwd_ic_node = set()
        for fwd in fwd_list:
            node_id = fwd.get('node_id')
            fwd_ic_node.add(node_id)
        return len(fwd_ic_node) == 1

    def _is_bwd_ic_same_node(self, link_id):
        '''后方多个设施在同个HwyNode点, 或者都是空(出入口)'''
        hwy_data = HwyDataMng.instance()
        bwd_list, fwd_list = hwy_data.get_link_fwd_bwd_facil(link_id)
        bwd_ic_node = set()
        for bwd in bwd_list:
            node_id = bwd.get('node_id')
            bwd_ic_node.add(node_id)
        return len(bwd_ic_node) == 1

    def _add_node_add_info(self, add_info):
        if add_info not in self.add_infos:
            self.add_infos.append(add_info)

    def get_add_info(self):
        return self.add_infos

    def get_sapa_pos(self, path):
        '''取得SAPA所在点的位置(index), 如果没有找到返回-1'''
        hwy_data = HwyDataMng.instance()
        for pos in range(0, len(path)):
            nodeid = path[pos]
            if hwy_data.is_sapa_pos_node(nodeid):
                return pos
        return -1


#==============================================================================
# 边界出口点（离开当前Tile）
#==============================================================================
class HwyBoundaryOutInfo(HwyICInfo):
    '''
    Tile Boundary(Out)
    '''
    def __init__(self, ic_no, facility_id, facil_list):
        HwyICInfo.__init__(self, ic_no, facility_id, facil_list)
        if not self.name_yomi:
            self.name_yomi = self.name

    def _set_distance(self):
        self.inside_distance = 0.0
        self.between_distance = 0.0

    def get_boundary_type(self):
        return BOUNDARY_TYPE_OUT

    def _get_tile_id(self):
        node_id = self.facil_list[0].node_id
        hwy_data = HwyDataMng.instance()
        G = hwy_data.get_graph()
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
        hwy_data = HwyDataMng.instance()
        G = hwy_data.get_graph()
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
        self.forward_flag = IC_DEFAULT_VAL

    def _set_reverse_flag(self):
        '''判断边界点的前一个设施和后一个设施是否并列'''
        hwy_data = HwyDataMng.instance()
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
            (hwy_data.is_same_facil(prev_facil, next_facil)
             or self._is_identical_facil(prev_facil, next_facil))):
            self.reverse_flag = IC_TYPE_TRUE
        else:
            self.reverse_flag = IC_DEFAULT_VAL

    def _get_exit_path_point(self):
        return []

    def _get_enter_path_point(self):
        return []

    def _get_main_path_point(self):
        # 边界出口，不做本线点
        return []

    def _get_center_path_point(self):
        '''表示点：进入设施的本线link'''
        hwy_data = HwyDataMng.instance()
        G = hwy_data.get_graph()
        node_id = self.first_facil.node_id
        road_code = self.first_facil.road_code
        inlinks = G.get_main_inlinkids(node_id, road_code)
        if inlinks:
            point = HwyPathPoint(self, inlinks[0], node_id)
            point.center_flag = IC_TYPE_TRUE
            return [point]
        else:
            print 'Error: No inlink for tile boundary %s' % node_id
            return []

    def _get_position_path_point(self):
        return []

    def _set_conn_info(self):
        pass


#==============================================================================
# 边界进入点（进入Tile的点）
#==============================================================================
class HwyBoundaryInInfo(HwyBoundaryOutInfo):
    '''
    Tile Boundary(IN)
    '''
    def __init__(self, ic_no, facility_id, facil_list):
        HwyBoundaryOutInfo.__init__(self, ic_no, facility_id, facil_list)

    def _set_distance(self):
        self.inside_distance = 0.0
        main_node = self.first_facil.node_id
        hwy_data = HwyDataMng.instance()
        # 本线路径
        main_path = hwy_data.get_main_path(self.facil_list[0].road_code,
                                           self.facil_list[0].updown)
        self._set_between_distance(main_node, main_path)

    def _set_conn_tile_id(self):
        # 入口的tile和出口的tile，刚好调换一下
        self.conn_tile_id = self._get_tile_id()

    def _set_tile_id(self):
        self.tile_id = self._get_conn_tile_id()

    def _set_forward_flag(self):
        '''判断边界点的前一个设施和后一个设施是否并列'''
        hwy_data = HwyDataMng.instance()
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
            (hwy_data.is_same_facil(prev_facil, next_facil)
             or self._is_identical_facil(prev_facil, next_facil))):
            self.forward_flag = IC_TYPE_TRUE
        else:
            self.forward_flag = IC_DEFAULT_VAL

    def _set_reverse_flag(self):
        self.reverse_flag = IC_DEFAULT_VAL

    def _get_main_path_point(self):
        '''本线点：退出设施的本线link'''
        hwy_data = HwyDataMng.instance()
        G = hwy_data.get_graph()
        node_id = self.first_facil.node_id
        road_code = self.first_facil.road_code
        inlinks = G.get_main_outlinkids(node_id, road_code)
        if inlinks:
            point = HwyPathPoint(self, inlinks[0], node_id)
            point.main_flag = IC_TYPE_TRUE
            return [point]
        else:
            print 'Error: No OutLink for tile boundary %s' % node_id
        return []

    def _get_center_path_point(self):
        # 边界入口，不做表示点
        return []


#=============================================================================
# 探索地点
#=============================================================================
class HwyPathPoint(object):
    '''
    Path Point
    '''
    def __init__(self, ic_info, link_id, node_id):
        self.ic_no = ic_info.ic_no
        self.enter_flag = IC_DEFAULT_VAL
        self.exit_flag = IC_DEFAULT_VAL
        self.main_flag = IC_DEFAULT_VAL  # 本线，离开设施的第一条link
        self.center_flag = IC_DEFAULT_VAL  # 本线，进入设施的link
        self.new_flag = IC_DEFAULT_VAL
        self.unopen_flag = IC_DEFAULT_VAL
        self.pos_flag = IC_DEFAULT_VAL  # IC出入口，SAP等的设施所在位置(iDDN用)
        self.link_id = link_id
        self.node_id = node_id  # 求坐标用
        self.tile_id = ic_info.tile_id


#==============================================================================
# 高速接続情報
#==============================================================================
class HwyConnInfo(object):
    '''高速接続情報'''
    def __init__(self, ic_info, tile_id, s_facil, t_facil, path, path_type):
        self.ic_no = ic_info.ic_no
        self.road_attr = IC_DEFAULT_VAL   # conn_ic所在道路的道路属性code
        self.conn_direction = IC_DEFAULT_VAL  # 接続方向フラグ, 0: 正， 1: 逆
        self.same_road_flag = IC_DEFAULT_VAL  # 同一路線接続フラグ
        self.tile_change_flag = IC_DEFAULT_VAL  # 接続先Zone変化フラグ
        self.uturn_flag = IC_DEFAULT_VAL  # 新規施設フラグ
        self.new_flag = IC_DEFAULT_VAL  # 未開通フラグ
        self.unopen_flag = IC_DEFAULT_VAL
        self.vics_flag = IC_DEFAULT_VAL
        self.toll_flag = IC_DEFAULT_VAL  # 料金情報有無フラグ
        self.conn_road_no = None  # 接続先道路番号
        self.conn_ic_no = None  # 接続先ＩＣ等番号
        self.conn_length = None
        self.toll_count = IC_DEFAULT_VAL
        self.vics_count = IC_DEFAULT_VAL
        self.conn_tile_id = None
        self.tile_id = tile_id
        # 设施信息
        self.s_facil = s_facil
        self.t_facil = t_facil
        self.path_type = path_type
        self.path = path
        self.ic_name = self.s_facil.facil_name
        self.conn_ic_name = self.t_facil.facil_name
        self.ic_info = ic_info
        self.toll_list = []  # 料金情报

    def set_conn_info(self):
        self._set_road_attr()
        self._set_uturn()
        self._set_conn_direction()
        self._set_same_road_flag()
        self._set_conn_road_no()
        self._set_conn_ic_no()
        self._set_conn_length()
        self._set_conn_tile_id()
        self._set_toll_info()  # 料金情报

    def _set_road_attr(self):
        '''conn_ic所在道路的道路属性code'''
        hwy_data = HwyDataMng.instance()
        road_code = self.t_facil.road_code
        attr_list = hwy_data.get_road_attrs(road_code)
        attr_dict = {UPDOWN_TYPE_UP: (ROAD_ATTR_UP,
                                      ROAD_ATTR_IN,
                                      ROAD_ATTR_WEST),
                     UPDOWN_TYPE_DOWN: (ROAD_ATTR_DOWN,
                                        ROAD_ATTR_OUT,
                                        ROAD_ATTR_EAST)
                     }
        attr_map = attr_dict.get(self.t_facil.updown)
        for attr in attr_list:
            if attr in attr_map:
                self.road_attr = attr
                break

    def _set_conn_direction(self):
        '''接続方向フラグ，顺车流方向(JCT出口到JCT入口)： 0， 反之: 1'''
        if self.s_facil.inout == INOUT_TYPE_OUT:
            self.conn_direction = CONN_DIR_FORWARD
        elif self.s_facil.inout == INOUT_TYPE_IN:
            self.conn_direction = CONN_DIR_REVERSE
        else:
            self.conn_direction = None

    def _set_uturn(self):
        if self.path_type == 'UTURN':
            self.uturn_flag = IC_TYPE_TRUE
        else:
            self.uturn_flag = IC_DEFAULT_VAL

    def _set_same_road_flag(self):
        '''同一路線接続フラグ'''
        # 1. Uturn
        if self.uturn_flag:
            self.same_road_flag = IC_TYPE_TRUE
            return
        # 2. 环形的起末，注：环形的起是JCT In, 末是JCT Out
        if (self.s_facil.road_code == self.t_facil.road_code):
            # print self.s_facil.road_code
            self.same_road_flag = IC_TYPE_TRUE
            return
        # 3. 高速上，两根并行前进的本线
        # 如：roadcode: 201080,"中央自動車道" 和 roadcode:209010, "中央自動車道２"
        s_roadcode = self.s_facil.road_code
        t_roadcode = self.t_facil.road_code
        if (self.is_second_roadcode(s_roadcode) or
            self.is_second_roadcode(t_roadcode)):
            hwy_data = HwyDataMng.instance()
            s_name_kanji, name_yomi = hwy_data.get_road_name(s_roadcode)
            t_name_kanji, name_yomi = hwy_data.get_road_name(t_roadcode)
            s_name_kanji = s_name_kanji.replace('２', '2')
            s_name_kanji = s_name_kanji.replace('３', '3')
            t_name_kanji = t_name_kanji.replace('２', '2')
            t_name_kanji = t_name_kanji.replace('３', '3')
            if s_name_kanji[-1] in ('2', '3'):
                if s_name_kanji[:-1] == t_name_kanji:
                    self.same_road_flag = IC_TYPE_TRUE
                    return
            if t_name_kanji[-1] in ('2', '3'):
                if t_name_kanji[:-1] == s_name_kanji:
                    self.same_road_flag = IC_TYPE_TRUE
                    return
            # print ('Not Same. s_roadcode=%s, s_roadname=%s,'
            #       't_roadcode=%s, t_road_name=%s.'
            #       % (s_roadcode, s_name_kanji, t_roadcode, t_name_kanji))

    def is_second_roadcode(self, roadcode):
        '''like: 209010'''
        return roadcode / 1000 % 10 == 9

    def _set_conn_road_no(self):
        '''接続先道路番号'''
        hwy_data = HwyDataMng.instance()
        road_code = self.t_facil.road_code
        self.conn_road_no = hwy_data.get_road_no(road_code)

    def _set_conn_ic_no(self):
        hwy_data = HwyDataMng.instance()
        self.conn_ic_no = hwy_data.get_ic_no(self.t_facil)

    def _set_conn_length(self):
        hwy_data = HwyDataMng.instance()
        G = hwy_data.get_graph()
        #print self.path
        self.conn_length = G.get_length_for_path(self.path)
        #print self.conn_length

    def _set_conn_tile_id(self):
        conn_node_id = self.t_facil.node_id
        conn_road_code = self.t_facil.road_code
        hwy_data = HwyDataMng.instance()
        G = hwy_data.get_graph()
        # 接续点是处在tile边界
        if hwy_data.is_boundary_node(conn_node_id):
            # 出口时，边界点在后(边界点ic_no较小)，所以用点脱出link的Tile
            if self.first_facil.inout == INOUT_TYPE_OUT:
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
            if hwy_data.is_road_start_node(conn_road_code, updown,
                                           conn_node_id):
                conn_tile_id = self.ic_info._get_out_tile_id(conn_node_id,
                                                             conn_road_code)
            # 道路终点，用in_link所在tile
            elif hwy_data.is_road_end_node(conn_road_code, updown,
                                           conn_node_id):
                conn_tile_id = self.ic_info._get_in_tile_id(conn_node_id,
                                                            conn_road_code)
            else:
                conn_tile_id = G.get_node_tile(conn_node_id)
        if conn_tile_id != G.get_node_tile(conn_node_id):
            print 'conn_tile_id != node_tile id.'
        self.conn_tile_id = convert_tile_id(conn_tile_id)
        if self.conn_tile_id != self.tile_id:
            self.tile_change_flag = IC_TYPE_TRUE
        else:
            self.tile_change_flag = IC_DEFAULT_VAL

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
        # case: 金沢自然公園連絡路
#         if len(self.toll_list) > 1:
#             print 'JCT:', self.s_facil.node_id, self.s_facil.facil_name

    def get_toll_info(self):
        return self.toll_list


#==============================================================================
# 无料区间设施情报
#==============================================================================
class HwyICInfoNoToll(HwyICInfo):
    '''无料区间设施情报
    '''

    def __init__(self, facility_id, facil_info):
        '''
        Constructor
        '''
        HwyICInfo.__init__(self, None, facility_id, [facil_info])
