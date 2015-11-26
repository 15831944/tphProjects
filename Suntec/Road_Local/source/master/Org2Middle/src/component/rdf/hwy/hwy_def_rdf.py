# -*- coding: UTF-8 -*-
'''
Created on 2015-1-29

@author: hcz
'''
SIMILAR_DEGREE_NUM = 0x20  # 番号相似
SIMILAR_DEGREE_NAME = 0x10  # 名称相似
SIMILAR_DEGREE_NO_NUM_NAME = 0x08  # 名称和名称都是空
SIMILAR_DEGREE_ROAD_TYPE = 0x04  # road_type相同
SIMILAR_DEGREE_LINK_TYPE = 0x02  # link_type相同
SIMILAR_DEGREE_NONE = 0x00  # 都不相同

ANGLE_10 = 1820  # 10度
ANGLE_15 = 2730  # 15度
ANGLE_30 = 5461  # 30度
ANGLE_35 = 6371  # 35度
ANGLE_45 = 8192  # 45度
ANGLE_60 = 10922  # 60度
ANGLE_75 = 13653  # 75度
ANGLE_80 = 14564  # 80度
ANGLE_90 = 16384  # 90度
ANGLE_100 = 23665  # 100度
ANGLE_135 = 24576  # 135度
ANGLE_180 = 32768  # 180度
ANGLE_270 = 49152  # 270度
ANGLE_280 = 50972  # 280度
ANGLE_285 = 51880  # 285度
ANGLE_315 = 57344  # 315度
ANGLE_300 = 54614  # 300度
ANGLE_360 = 65536  # 360度

HWY_ROAD_TYPE_HWY0 = 0
HWY_ROAD_TYPE_HWY1 = 1
HWY_ROAD_TYPE_HWY = (HWY_ROAD_TYPE_HWY0,)

HWY_LINK_TYPE_MAIN1 = 1
HWY_LINK_TYPE_MAIN2 = 2
HWY_LINK_TYPE_JCT = 3
HWY_LINK_TYPE_INNER = 4
HWY_LINK_TYPE_RAMP = 5
HWY_LINK_TYPE_SIDE = 6  # 侧道
HWY_LINK_TYPE_SAPA = 7
HWY_LINK_TYPE_RTURN = 8
HWY_LINK_TYPE_LTURN = 9
HWY_LINK_TYPE_RL = (HWY_LINK_TYPE_RTURN, HWY_LINK_TYPE_LTURN)
HWY_LINK_TYPE_MAIN = (HWY_LINK_TYPE_MAIN1, HWY_LINK_TYPE_MAIN2)
HWY_LINK_TYPE_INNER_ALL = (HWY_LINK_TYPE_INNER,
                           HWY_LINK_TYPE_RTURN,
                           HWY_LINK_TYPE_LTURN)

ROUTE_DISTANCE_1000M = 1000  # 1000米
ROUTE_DISTANCE_1500M = 1500  # 1500米
ROUTE_DISTANCE_1600M = 1600  # 1600米
ROUTE_DISTANCE_2000M = 2000  # 2000米
ROUTE_DISTANCE_2500M = 2500  # 2500米

HWY_TRUE = 1
HWY_FALSE = 0
HWY_NAME_SPLIT = '|'

HWY_PATH_CONN_TYPE_NONE = 0  # 无相接
HWY_PATH_CONN_TYPE_S = 1  # 起点相接
HWY_PATH_CONN_TYPE_E = 2  # 终点相接
HWY_PATH_CONN_TYPE_SE = 3  # 两头相接

LAST_SEQ_NUM = 9999

# 设施种别
HWY_IC_TYPE_SA = 1  # SA
HWY_IC_TYPE_PA = 2  # PA
HWY_IC_TYPE_JCT = 3  # JCT
HWY_IC_TYPE_RAMP = 4  # RAMP
HWY_IC_TYPE_IC = 5  # IC
HWY_IC_TYPE_TOLL = 6  # 收费站
HWY_IC_TYPE_SMART_IC = 7  # SMART IC
HWY_IC_TYPE_VIRTUAl_JCT = 9  # 虚拟JCT
HWY_IC_TYPE_UTURN = 10
HWY_IC_TYPE_SERVICE_ROAD = 11  # 辅路、类辅路设施
HWY_IC_TYPE_URBAN_JCT = 12 # 高速本线到都市高速的设施

# 出入口
HWY_INOUT_TYPE_NONE = 0  # 无
HWY_INOUT_TYPE_IN = 1  # 入口・合流
HWY_INOUT_TYPE_OUT = 2  # 出口・分岐
# up down
HWY_UPDOWN_TYPE_NONE = 0  # 方向なし
HWY_UPDOWN_TYPE_UP = 1
HWY_UPDOWN_TYPE_DOWN = 2
HWY_UPDOWN_TYPE_COMMON = 3  # 双方向共通

HWY_BOUNDARY_TYPE_IN = 1
HWY_BOUNDARY_TYPE_OUT = 2

# 路径种别
HWY_PATH_TYPE_MAIN = 'MAIN'
HWY_PATH_TYPE_IC = 'IC'
HWY_PATH_TYPE_SR = 'SrvRoad'  # 辅路
HWY_PATH_TYPE_SR_SAPA = 'SrvRoad_SAPA'
HWY_PATH_TYPE_SR_JCT = 'SrvRoad_JCT'

HWY_TILE_BOUNDARY_NAME = '(Tile Boundary)'
HWY_INVALID_FACIL_ID_17CY = 0x3FFFFF  # 无效设施番号(17cy Tile边界使用)
