# -*- coding: UTF-8 -*-
'''
Created on 2014-4-22

@author: hongchenzai
'''

#==============================================================================
# 常量
#==============================================================================
# 设施种别
IC_TYPE_INVALID = -1
IC_TYPE_SA = 1
IC_TYPE_PA = 2
IC_TYPE_JCT = 3
IC_TYPE_RAMP = 4
IC_TYPE_IC = 5
IC_TYPE_TOLL = 6
IC_TYPE_SMART_IC = 7
# in out
INOUT_TYPE_NONE = 0  # 区別なし
INOUT_TYPE_IN = 1  # 入口・合流
INOUT_TYPE_OUT = 2  # 出口・分岐
# up down
UPDOWN_TYPE_NONE = 0  # 方向なし
UPDOWN_TYPE_UP = 1
UPDOWN_TYPE_DOWN = 2
UPDOWN_TYPE_COMMON = 3  # 双方向共通

# 道路属性
ROAD_ATTR_NONE = 0  # 不明
ROAD_ATTR_UP = 1  # 上り
ROAD_ATTR_DOWN = 2  # 下り
ROAD_ATTR_IN = 3  # 内回り
ROAD_ATTR_OUT = 4  # 外回り
ROAD_ATTR_WEST = 5  # 西行
ROAD_ATTR_EAST = 6  # 東行き
ROAD_ATTR_NORTH = 7  # 北行き
ROAD_ATTR_SOUTH = 8  # 南行き
# Tile
HWY_TILE_LAYER = 10
TILE_LAYER_14 = 14
ID_SIZE = 32

HWY_INVALID_FACIL_ID = 16383  # 无效设施番号
INVALID_ROAD_NO = -1  # 无效道路番号
INVALID_IC_NO = 0  # 无效ic_no
IC_TYPE_TRUE = 1
IC_DEFAULT_VAL = 0
# 料金所種別コード
TOLL_CLASS_JOIN = 0  # 分岐合流ノード
TOLL_CLASS_MAIN = 1  # 本線上料金所
TOLL_CLASS_RAMP = 2  # ランプ上料金所
TOLL_CLASS_BARRIER = 3  # バリア
TOLL_CLASS_OUT_IN = 4  # 出入口ノード
# 料金所機能コード
TOLL_FUNC_NONE = 0  # 機能無し
TOLL_FUNC_TICKET = 1  # 発券機能
TOLL_FUNC_CAL = 2  # 精算機能
TOLL_FUNC_SINGLE = 3  # 単独料金所
TOLL_FUNC_SINGLE_TICKET = 4  # 単独料金所+発券機能
TOLL_FUNC_SINGLE_CAL = 5  # 単独料金所+精算機能
TOLL_FUNC_UTURN_CHECK = 6  # Uターン用検索機能
TOLL_FUNC_NO_TICKET = 7  # 単独料金無効発券機能
TOLL_FUNC_CAL_TICKET = 8  # 精算機能+発券機能

ROAD_CODE_NO_TOLL = 600000  # 无料金区间道路code

HWY_UPDOWN_DOWN = 0  # 下り
HWY_UPDOWN_UP = 1  # 上り
HWY_UPDOWN_RESERVED = 2
HWY_UPDOWN_COMMON = 3  # 上下線共有


def convert_updown(updown):
    # 0…下り、1…上り、2…RESERVED、3…上下線共有
    if updown == UPDOWN_TYPE_UP:
        return HWY_UPDOWN_UP
    elif updown == UPDOWN_TYPE_DOWN:
        return HWY_UPDOWN_DOWN
    elif updown == UPDOWN_TYPE_COMMON:
        return HWY_UPDOWN_COMMON
    else:
        return None
