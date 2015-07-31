# -*- coding: UTF-8 -*-
'''
Created on 2014-4-16

@author: hongchenzai
'''
from component.jdb.hwy.hwy_def import UPDOWN_TYPE_UP
from component.jdb.hwy.hwy_def import UPDOWN_TYPE_DOWN
# 道路属性
from component.jdb.hwy.hwy_def import ROAD_ATTR_NONE
from component.jdb.hwy.hwy_def import ROAD_ATTR_UP
from component.jdb.hwy.hwy_def import ROAD_ATTR_DOWN
from component.jdb.hwy.hwy_def import ROAD_ATTR_IN
from component.jdb.hwy.hwy_def import ROAD_ATTR_OUT
from component.jdb.hwy.hwy_def import ROAD_ATTR_WEST
from component.jdb.hwy.hwy_def import ROAD_ATTR_EAST

# 路線方向タイプコード
LINE_DIR_NONE = 0  # タイプなし
LINE_DIR_UPDOWN = 1  # 上り・下り
LINE_DIR_INOUT = 2  # 内回り・外回り
LINE_DIR_EW = 3  # 西行き・東行き


class HwyRoadInfo(object):
    '''
    classdocs
    '''

    def __init__(self, road_no, lineclass_c, road_type,
                 linedir_type, updown, cycle_flag,
                 name_kanji, name_yomi):
        '''
        Constructor
        '''
        self.road_no = road_no
        self.road_kind = road_type
        self.iddn_road_kind = lineclass_c - 1
        self.road_attr = self.convert_road_attr(linedir_type, updown)
        self.loop = cycle_flag
        self.new = 0
        self.un_open = 0
        self.name_kanji = self._delete_numeric(name_kanji)
        self.name_yomi = self._delete_numeric(name_yomi)
        self.updown = self.convert_updown(updown)

    def convert_road_attr(self, linedir_type, updown):
        atrr_dict = {(LINE_DIR_UPDOWN, UPDOWN_TYPE_UP): ROAD_ATTR_UP,
                     (LINE_DIR_UPDOWN, UPDOWN_TYPE_DOWN): ROAD_ATTR_DOWN,
                     (LINE_DIR_INOUT, UPDOWN_TYPE_UP): ROAD_ATTR_IN,
                     (LINE_DIR_INOUT, UPDOWN_TYPE_DOWN): ROAD_ATTR_OUT,
                     (LINE_DIR_EW, UPDOWN_TYPE_UP): ROAD_ATTR_WEST,
                     (LINE_DIR_EW, UPDOWN_TYPE_DOWN): ROAD_ATTR_EAST,
                     }
        return atrr_dict.get((linedir_type, updown))

    def convert_updown(self, updown):
        # 0…下り、1…上り、2…RESERVED、3…上下線共有
        if updown == UPDOWN_TYPE_UP:
            return 1
        elif updown == UPDOWN_TYPE_DOWN:
            return 0
        else:
            return None

    def _delete_numeric(self, name):
        if name[-2:] in ('２'):
            return name.strip('２')
        if name[-1:] in ('2'):
            return name.strip('2')
        if name[-2:] in ('０', '１', '３', '４', '５', '６', '７', '８', '９'):
            print 'Error: name include other numeric. name=%s' % name
        if name[-1:] in ('0', '1', '2', '3', '4', '5', '6', '7', '8', '9'):
            print 'Error: name include other numeric. name=%s' % name
        return name

