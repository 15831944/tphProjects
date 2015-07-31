# -*- coding: UTF-8 -*-
'''
Created on 2IC_DEFAULT_VAL14-5-12

@author: hongchenzai
'''
from component.jdb.hwy.hwy_def import IC_TYPE_TOLL
from component.jdb.hwy.hwy_def import INOUT_TYPE_NONE
from component.jdb.hwy.hwy_def import INOUT_TYPE_IN
from component.jdb.hwy.hwy_def import INOUT_TYPE_OUT
from component.jdb.hwy.hwy_def import IC_TYPE_TRUE
from component.jdb.hwy.hwy_def import IC_DEFAULT_VAL
from component.jdb.hwy.hwy_def import TOLL_CLASS_MAIN
from component.jdb.hwy.hwy_def import TOLL_CLASS_RAMP
from component.jdb.hwy.hwy_def import TOLL_CLASS_BARRIER
from component.jdb.hwy.hwy_def import UPDOWN_TYPE_COMMON
from component.jdb.hwy.hwy_def import IC_TYPE_SA
from component.jdb.hwy.hwy_def import IC_TYPE_PA
from component.jdb.hwy.hwy_def import IC_TYPE_JCT
from component.jdb.hwy.hwy_def import IC_TYPE_RAMP
from component.jdb.hwy.hwy_def import IC_TYPE_IC
from component.jdb.hwy.hwy_def import IC_TYPE_SMART_IC
# 料金所機能コード
from component.jdb.hwy.hwy_def import TOLL_FUNC_TICKET
from component.jdb.hwy.hwy_def import TOLL_FUNC_CAL
from component.jdb.hwy.hwy_def import TOLL_FUNC_SINGLE
from component.jdb.hwy.hwy_def import TOLL_FUNC_SINGLE_TICKET
from component.jdb.hwy.hwy_def import TOLL_FUNC_SINGLE_CAL
from component.jdb.hwy.hwy_def import TOLL_FUNC_UTURN_CHECK
from component.jdb.hwy.hwy_def import TOLL_FUNC_NO_TICKET
from component.jdb.hwy.hwy_def import TOLL_FUNC_CAL_TICKET
from component.jdb.hwy.hwy_def import IC_TYPE_JCT
from component.jdb.hwy.hwy_def import convert_updown
from component.jdb.hwy.hwy_data_mng import HwyDataMng

# Toll Class
TOLL_CLS_ENTER = 1  # 入口
TOLL_CLS_EXIT = 2  # 出口
TOLL_CLS_PASS = 3  # 通过
TOLL_CLS_JCT = 4  # 分歧


class HwyTollInfo(object):
    '''
    Toll Information Class
    '''

    def __init__(self, ic_info, toll_facil,
                 facil, path_type,
                 conn_ic_no=None, conn_facil=None,
                 dummy_toll_node=None):
        '''
        Constructor
        '''
        self.toll_no = IC_DEFAULT_VAL
        self.ic_no = ic_info.ic_no
        self.conn_ic_no = conn_ic_no  # 接续ic_no(分歧料金)
        self.toll_class = None
        self.class_name = ''
        self.tollgate_count = 1
        self.toll_type = None  # HwyTollType object
        self.updown = None
        self.facility_id = None
        self.name_kanji = toll_facil.facil_name
        self.name_yomi = toll_facil.name_yomi
        self.road_code = toll_facil.road_code  # 料金道路
        self.road_seq = toll_facil.road_point  # 料金序列号
        self.node_id = toll_facil.node_id  # 料金点
        self.dummy_toll_node = dummy_toll_node  # dummy点
        # ##
        self.ic_info = ic_info
        self.toll_facil = toll_facil
        self.facil = facil
        self.path_type = path_type
        self.conn_facil = conn_facil

    def set_toll_info(self):
        self._set_toll_class()
        self._set_updown()
        self._set_facility_id()
        self.toll_type = HwyTollType(self)
        self.toll_type.set_type_info()

    def _set_updown(self):
        self.updown = convert_updown(self.toll_facil.updown)

    def _set_facility_id(self):
        hwy_data = HwyDataMng.instance()
        self.facility_id = hwy_data.get_facility_id(self.toll_facil.road_code,
                                                    self.toll_facil.road_point)

    def _set_toll_class(self):
        '''料金所类型（入口、出口，通过，分歧）。'''
        if self.ic_info.tollgate == IC_TYPE_TRUE:  # 本线通过料金
            self.toll_class = TOLL_CLS_PASS
        elif self.path_type in ('IC', 'SAPA'):
            if self.facil.inout == INOUT_TYPE_IN:
                self.toll_class = TOLL_CLS_ENTER
            elif self.facil.inout == INOUT_TYPE_OUT:
                self.toll_class = TOLL_CLS_EXIT
            else:
                self.toll_class = None
        elif self.path_type in ('JCT', 'UTURN'):
            self.toll_class = TOLL_CLS_JCT
        else:
            self.toll_class = None
        if self.toll_class:
            class_name_dict = {TOLL_CLS_ENTER: 'ENTER',
                               TOLL_CLS_EXIT: 'EXIT',
                               TOLL_CLS_PASS: 'PASS',
                               TOLL_CLS_JCT: 'JCT'
                               }
            self.class_name = class_name_dict.get(self.toll_class)


#==============================================================================
# HwyTollType
#==============================================================================
class HwyTollType(object):
    def __init__(self, toll_info=None):
        '''
        Constructor
        '''
        # ## 施設種別
        self.etc_antenna = IC_DEFAULT_VAL   # ETC天线
        self.enter = IC_DEFAULT_VAL
        self.exit = IC_DEFAULT_VAL
        self.jct = IC_DEFAULT_VAL
        self.sa_pa = IC_DEFAULT_VAL
        self.gate = IC_DEFAULT_VAL  # ゲート/バリア
        self.unopen = IC_DEFAULT_VAL  # 未開通施設フラグ
        self.dummy_facil = IC_DEFAULT_VAL  # 架空施設フラグ
        # ## 料金所種別
        self.non_ticket_gate = IC_DEFAULT_VAL  # 単独料金無効券発券所フラグ
        self.check_gate = IC_DEFAULT_VAL  # 検札所フラグ
        self.single_gate = IC_DEFAULT_VAL  # 単独料金所フラグ
        self.cal_gate = IC_DEFAULT_VAL  # 清算所フラグ
        self.ticket_gate = IC_DEFAULT_VAL  # 発券所フラグ
        self.nest = IC_DEFAULT_VAL  # 入れ子料金所フラグ
        self.uturn = IC_DEFAULT_VAL  # Ｕターン判定用施設フラグ
        self.not_guide = IC_DEFAULT_VAL  # 非案内フラグ
        self.normal_toll = IC_DEFAULT_VAL  # 一般料金対応フラグ
        self.etc_toll = IC_DEFAULT_VAL  # ETC料金対応フラグ
        self.etc_section = IC_DEFAULT_VAL  # ETC料金区間フラグ
        self.toll_info = toll_info

    def set_type_info(self):
        self._set_toll_class()
        self._set_toll_function()
        # 入れ子料金所フラグ
        self._set_nest_kind()

    def set_facil_type(self, facilcls, inout):
        if facilcls in (IC_TYPE_SA, IC_TYPE_PA):
            self.sa_pa = IC_TYPE_TRUE
        elif facilcls == IC_TYPE_JCT:
            self.jct = IC_TYPE_TRUE
        elif facilcls in (IC_TYPE_RAMP, IC_TYPE_IC, IC_TYPE_SMART_IC):
            if inout == INOUT_TYPE_OUT:
                self.exit = IC_TYPE_TRUE
            elif inout == INOUT_TYPE_IN:
                self.enter = IC_TYPE_TRUE
            else:
                self.enter = None
                self.exit = None
        else:
            self.sa_pa = None
            self.jct = None
            self.enter = None
            self.exit = None

    def _set_toll_class(self):
        '''料金种别'''
        toll_facil = self.toll_info.toll_facil
        if toll_facil.dummyfacil == IC_TYPE_TRUE:
            self.dummy_facil = IC_TYPE_TRUE
        else:
            self.dummy_facil = IC_DEFAULT_VAL
        return

    def _set_toll_function(self):
        '''料金机能种别'''
        toll_facil = self.toll_info.toll_facil
        if toll_facil.tollfunc == TOLL_FUNC_TICKET:  # 発券機能
            self.ticket_gate = IC_TYPE_TRUE
        elif toll_facil.tollfunc == TOLL_FUNC_CAL:  # 精算機能
            self.cal_gate = IC_TYPE_TRUE
        elif toll_facil.tollfunc == TOLL_FUNC_SINGLE:   # 単独料金所
            self.single_gate = IC_TYPE_TRUE
        elif toll_facil.tollfunc == TOLL_FUNC_SINGLE_TICKET:  # 単独料金所+発券機能
            self.single_gate = IC_TYPE_TRUE
            self.ticket_gate = IC_TYPE_TRUE
        elif toll_facil.tollfunc == TOLL_FUNC_SINGLE_CAL:  # 単独料金所+精算機能
            self.single_gate = IC_TYPE_TRUE
            self.cal_gate = IC_TYPE_TRUE
        elif toll_facil.tollfunc == TOLL_FUNC_UTURN_CHECK:  # 検札所
            self.check_gate = IC_TYPE_TRUE
        elif toll_facil.tollfunc == TOLL_FUNC_NO_TICKET:  # 単独料金無効発券機能
            self.non_ticket_gate = IC_TYPE_TRUE
        elif toll_facil.tollfunc == TOLL_FUNC_CAL_TICKET:  # 精算機能+発券機能
            self.cal_gate = IC_TYPE_TRUE
            self.ticket_gate = IC_TYPE_TRUE
        else:
            self.ticket_gate = None
            self.cal_gate = None
            self.single_gate = None
            self.check_gate = None
            self.non_ticket_gate = None

        if toll_facil.dummytoll == IC_TYPE_TRUE:
            self.not_guide = IC_TYPE_TRUE
        else:
            self.not_guide = IC_DEFAULT_VAL
        self.normal_toll = IC_TYPE_TRUE

    def _set_nest_kind(self):
        '''入れ子料金所フラグ
           toll_facil: 料金所在点的设施
           facil: JCT OUT的情报
           conn_facil: JCT IN的情报
        '''
        ic_info = self.toll_info.ic_info
        toll_facil = self.toll_info.toll_facil
        conn_facil = self.toll_info.conn_facil
        if not conn_facil:
            self.nest = IC_DEFAULT_VAL
        else:
            facil = ic_info.first_facil
            if (ic_info.ic == IC_TYPE_TRUE and
                facil.facilcls == IC_TYPE_JCT and
                toll_facil.road_point != facil.road_point and
                toll_facil.road_point != conn_facil.road_point):
                if self._is_unique_toll(toll_facil):
                    self.nest = IC_TYPE_TRUE
                else:
                    self.nest = IC_DEFAULT_VAL
            else:
                self.nest = IC_DEFAULT_VAL

    def _is_unique_toll(self, toll_facil):
        '''本线料金所番号是唯一的'''
        hwy_data = HwyDataMng.instance()
        # 对侧有相同番号的料金所
        updown = toll_facil.updown ^ UPDOWN_TYPE_COMMON
        facils = hwy_data.get_hwy_facils(toll_facil.road_code,
                                         toll_facil.road_point,
                                         updown,
                                         INOUT_TYPE_NONE)
        if facils:
            return False
        # 同侧相同番号的料金所
        facils = hwy_data.get_hwy_facils(toll_facil.road_code,
                                         toll_facil.road_point,
                                         toll_facil.updown,
                                         INOUT_TYPE_NONE)
        if facils:
            if len(facils) == 1:
                return True
        return False
