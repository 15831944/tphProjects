# -*- coding: UTF8 -*-
'''
Created on 2014-11-26

@author: hcz
'''
import re
STATE_IDX = 0
NUMBER_IDX = 1
MODIFIERS_IDX = 2
MAX_PARAM_IDX = 3
NUM_SPLIT_CHAR_HYPHEN = '-'
NUM_SPLIT_CHAR_BLANK = ' '
DIRECTION_SPLIT_CHAR = ' '
ROUTE_MODIFIERS = {'ALT': None,  # Alternate or Alternate Route
                   'BL': None,  # Business Loop
                   'BR': None,  # Business Route
                   'BYP': None,  # Bypass
                   'CONN': None,  # Connector
                   'EXT': None,  # Extension
                   'LINK': None,  # Link
                   'LOOP': None,  # Loop
                   'SCENIC': None,  # Scenic
                   'SPUR': None,  # Spur
                   'TOLL': None,  # Toll
                   'TRUCK': None,  # Truck Route
                   'BUS': None,  # 当前及下面是我们自己添加的，文档里没有
                   'KP': None,  # Keep
                   'EXPY': None,
                   'BELTWAY': None,
                   'BYPASS': None
                   }
# 方 向：东南西北
ROAD_DIRECTION = {'E': None,  # 东
                  'S': None,  # 南
                  'W': None,  # 西
                  'N': None,  # 北
                  'O': None,  # 西(法:Ouest, 西:Oeste)
                  }


# ==============================================================================
# RouteNumParser
# ==============================================================================
class RouteNumParser(object):
    '''番号分割解析类: 前缀、番号、方向、后缀修饰符(modifier)
    '''

    def __init__(self, route_num=''):
        '''
        Constructor
        '''
        self.route_num = route_num
        self.prefix = ''
        self.number = ''
        self.dir = ''
        self.modifier = ''

    def _initialize(self):
        self.prefix = ''
        self.number = ''
        self.dir = ''
        self.modifier = ''

    def set_route_num(self, route_num):
        self.route_num = route_num
        self._initialize()

    def get_prefix(self):
        return self.prefix

    def get_number(self):
        return self.number

    def get_dir(self):
        return self.dir

    def get_modifier(self):
        return self.modifier

    def add_number(self, s, split_char=NUM_SPLIT_CHAR_HYPHEN):
        if self.number:
            self.number += split_char + s
        else:
            self.number = s

    def add_modifier(self, s, split_char=NUM_SPLIT_CHAR_HYPHEN):
        if self.modifier:
            self.modifier += split_char + s
        else:
            self.modifier = s

    def do_parse(self):
        # ## 用'-'分割，如："NE-55B-SPUR"
        params = self.route_num.split(NUM_SPLIT_CHAR_HYPHEN)
        param_num = len(params)
        if param_num > NUMBER_IDX:
            self.prefix = params[STATE_IDX]  # 州/路的等级：'I-'
            param_idx = NUMBER_IDX
            # ## 第二个到倒数第二个元素
            for param_idx in range(NUMBER_IDX, param_num-1):
                p = params[param_idx]
                if param_idx == NUMBER_IDX:  # 第二个元素只能给番号
                    self.add_number(p)
                    continue
                # 第二个之后元素
                if not self._is_modifier(p):
                    self.add_number(p)
                else:
                    self.add_modifier(p)
            # ## 最后一个元素
            # 最后一个元素是第二个元素， 也就是只两个元素，即一个'-'，如："CR-302 N"
            if param_num == NUMBER_IDX + 1:
                num_modifier_dir = params[-1].split(DIRECTION_SPLIT_CHAR)
                self.number = num_modifier_dir[0]
                if len(num_modifier_dir) == 1:
                    pass
                elif len(num_modifier_dir) == 2:
                    if num_modifier_dir[1] in ROAD_DIRECTION:  # 方向
                        self.dir = num_modifier_dir[1]
                    else:  # [US-287 BUS]中的[287 BUS]
                        if self._is_modifier(num_modifier_dir[1]):
                            self.add_modifier(num_modifier_dir[1])
                        else:
                            self.add_number(num_modifier_dir[1],
                                            DIRECTION_SPLIT_CHAR)
                elif len(num_modifier_dir) == 3:  # [US-287 BUS N]中[287 BUS N]
                    if self._is_direction(num_modifier_dir[2]):  # 方向
                        self.dir = num_modifier_dir[2]
                        if self._is_modifier(num_modifier_dir[1]):
                            self.add_modifier(num_modifier_dir[1])
                        else:
                            self.add_number(num_modifier_dir[1],
                                            DIRECTION_SPLIT_CHAR)
                    else:
                        print 'Error num_modifier_dir > 2, %s' % self.route_num
                        self._parse_error()
                else:
                    print 'Error num_modifier_dir > 3, %s' % self.route_num
                    self._parse_error()
            # 最后元素不是第二个，即两个'-'及以上
            # 如: "NE-93E-LINK","TX-256-LOOP-TRUCK E","US-1-9-TRUCK"
            else:
                split_char = DIRECTION_SPLIT_CHAR
                num_modifier_dir = params[-1].split(split_char)
                if self.modifier or self._is_modifier(num_modifier_dir[0]):
                    self.add_modifier(num_modifier_dir[0])
                else:
                    self.add_number(num_modifier_dir[0])
                if len(num_modifier_dir) == 1:
                    pass
                elif len(num_modifier_dir) == 2:
                    if self._is_direction(num_modifier_dir[1]):  # check方向是否正确
                        self.dir = num_modifier_dir[1]
                    elif(self._is_modifier(num_modifier_dir[1])
                         or self.modifier):
                        self.add_modifier(num_modifier_dir[1])
                    else:
                        self.add_number(num_modifier_dir[1], split_char)
                elif len(num_modifier_dir) == 3:
                    if self._is_direction(num_modifier_dir[2]):  # check方向是否正确
                        self.dir = num_modifier_dir[2]
                        if(self._is_modifier(num_modifier_dir[1])
                           or self.modifier):
                            self.add_modifier(num_modifier_dir[1])
                        else:
                            self.add_number(num_modifier_dir[1], split_char)
                    else:
                        print 'Error num_modifier_dir > 3, %s' % self.route_num
                else:
                    print 'Error num_modifier_dir > 3, %s' % self.route_num
        elif len(params) < NUMBER_IDX + 1:
            # 用空格切割
            params = self.route_num.split(NUM_SPLIT_CHAR_BLANK)
            param_num = len(params)
            if param_num < NUMBER_IDX + 1:  # 少于两个元素
                self._deal_with_only_num(self.route_num)
                return
            elif param_num == NUMBER_IDX + 1:  # 两个元素
                if self._is_direction(params[-1]):
                    self.number = params[0]
                    self.dir = params[-1]
                else:
                    self.prefix = params[0]
                    self.number = params[-1]
                return
            elif param_num > MAX_PARAM_IDX + 1:  # 超过了四个元素
                print 'Error length of route_num > 4. %s' % self.route_num
                self._parse_error()
                return
            self.prefix = params[STATE_IDX]  # 州/路的等级：'I-'
            self.number = params[NUMBER_IDX]  # 番号
            # ## 三个、四个元素
            if param_num == MODIFIERS_IDX + 1:  # 三个元素
                if params[MODIFIERS_IDX] in ROAD_DIRECTION:  # 最后一个元素是方向
                    self.dir = params[MODIFIERS_IDX]
                else:  # 最后一个元素不是方向
                    self.modifier = params[MODIFIERS_IDX]
            elif param_num == MAX_PARAM_IDX + 1:  # 四个元素
                if params[-1] not in ROAD_DIRECTION:  # 最后一个元素不是方向
                    print 'Error DIR. %s' % self.route_num
                    self._parse_error()
                    return
                else:
                    self.modifier = params[MODIFIERS_IDX]
                    self.dir = params[-1]
        # 把非modifier合并到number
        self.merge_num_modifier()

    def _deal_with_only_num(self, route_num):
        '''只有番号，没有其他(前后缀，方向)'''
        self.number = self.route_num
        
    def _split_number(self, input_value):
        pattern = re.compile('\d+')
        return pattern.findall(input_value)
    
    def _search_number_range(self,input_value):
        number = (self._split_number(input_value))[0]
        pure_num_start = input_value.find(number)
        pure_num_end = pure_num_start + len(number)
        return (pure_num_start, pure_num_end)  

    def merge_num_modifier(self):
        '''把非modifier合并到number.如CR-60-221中的221就不是modifier，所要合并到番号里'''
        if self.modifier:
            if self.modifier in ROUTE_MODIFIERS:
                return False
            # 有字符'/', 如："8/10"
            if self.modifier.find('/') >= 0:
                return False
            # 数字开头，如: '3A', '141'
            t = re.search('^\d+', self.modifier)
            if t:
                # 番号+'-'+Modifier
                num_modifier = (self.number + NUM_SPLIT_CHAR_HYPHEN
                                + self.modifier)
                if self.route_num.find(num_modifier) >= 0:
                    self.number = num_modifier
                    self.modifier = ''
                    return True
                else:
                    # 番号+空格+Modifier
                    num_modifier = (self.number + NUM_SPLIT_CHAR_BLANK
                                    + self.modifier)
                    if self.route_num.find(num_modifier) >= 0:
                        self.number = num_modifier
                        self.modifier = ''
                        return True
            return False

    def _is_modifier(self, s):
        if s in ROUTE_MODIFIERS:
            return True
#         if self._is_direction(s):
#             return False
        # 有字符'/', 如："8/10"
        if s.find('/') >= 0:
            return True
        # 数字开头，如: '3A', '141'
        t = re.search('^\d+', s)
        if t:
            return False
        return True

    def _is_direction(self, s):
        if s in ROAD_DIRECTION:
            return True
        else:
            return False
    
    def _is_pure_number(self,s):
        return s.isdigit()

    def is_hov_modifier(self):
        if self.modifier == 'HOV':
            return True
        else:
            return False

    def _parse_error(self):
        self._initialize()

# 替换番号
REPLACE_NUMBER = {'CR129D': ['CR', '129D'],  # [州，番号]
                  'E1460': ['', 'E1460'],
                  'E1250': ['', 'E1250'],
                  }


# ==============================================================================
# RouteNumParserNA(北美)
# ==============================================================================
class RouteNumParserNA(RouteNumParser):
    '''番号分割解析类: 前缀、番号、方向、后缀修饰符(modifier)
    '''

    def __init__(self, route_num=''):
        RouteNumParser.__init__(self, route_num)

    def _deal_with_only_num(self, route_num):
        '''只有番号，没有其他(前后缀，方向)'''
        params = REPLACE_NUMBER.get(route_num)
        if not params or len(params) < NUMBER_IDX + 1:  # 少于两个元素
            print 'Error number of params < 2. %s' % self.route_num
            self._parse_error()
        else:
            self.prefix = params[STATE_IDX]  # 州/路的等级：'CR-'
            self.number = params[NUMBER_IDX]  # 番号


# ==============================================================================
# RouteNumParserNA(香港)
# ==============================================================================
class RouteNumParserHKG(RouteNumParser):
    '''番号分割解析类: 前缀、番号、方向、后缀修饰符(modifier)
    '''

    def __init__(self, route_num=''):
        RouteNumParser.__init__(self, route_num)

    def do_parse(self):
        self.number = self.route_num
        

class RouteNumParserARA8(RouteNumParser):
    def __init__(self, route_num=''):
        RouteNumParser.__init__(self, route_num.strip())
        
    def __split_info_section(self):
        return self.route_num.split(NUM_SPLIT_CHAR_BLANK)
    
    def __has_number_in_section(self, info):
        pattern = re.compile('\d+')
        return len(pattern.findall(info)) <> 0        
        
    def do_parse(self):
        '''
            input case: 
            case one:"إ102"  --data + character, without space
            case two:"ع 136" --data + character, with space
            case three: "10" --pure data
            case four: "1 N" --data + space + direction
            case five: "جدة 10 N" --data + character + direction
        '''
        route_info_section = self.__split_info_section()
        
        info_section_cnt = len(route_info_section)
        
        if info_section_cnt == 1:
            self.number = route_info_section[0]
        else:
            for i in range(0, info_section_cnt):
                if self.__has_number_in_section(route_info_section[i]):
                    self.number = route_info_section[i]
                elif route_info_section[i] in ROAD_DIRECTION:
                    self.dir =  route_info_section[i]


# ==============================================================================
# RouteNumParserLeft(如： 阿拉伯语--和其他语言不同，阿拉伯语前缀的词是放后面的)
# ==============================================================================
# class RouteNumParserLeft(RouteNumParser):
#     '''番号分割解析类: 前缀、番号、方向、后缀修饰符(modifier)
#     '''
# 
#     def __init__(self, route_num=''):
#         RouteNumParser.__init__(self, route_num)
# 
#     def do_parse(self):
#         # ## 用空格分割，如：[5351], [513 N], [طريق 33] ,[جدة 35 E]
#         params = self.route_num.split(NUM_SPLIT_CHAR_BLANK)
#         param_num = len(params)
#         if param_num == 1:
#             self.number = params[0]
#         elif param_num == 3:  # 三个元素
#             self.number = params[0]
#             self.prefix = params[1]
#             self.dir = params[2]
#         elif param_num == 2:  # 两个无素
#             self.number = params[0]
#             if self._is_direction(params[1]):
#                 self.dir = params[1]
#             else:
#                 self.prefix = params[1]
#         else:
#             self._parse_error()
