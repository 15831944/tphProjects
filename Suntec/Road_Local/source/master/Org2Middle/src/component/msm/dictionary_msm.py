# -*- coding: UTF8 -*-
'''
Created on  2014-08-11

@author: hongchenzai
'''
import re
import component.default.dictionary
from common import cache_file
from component.default.multi_lang_name import NAME_TYPE_ALTER
from component.default.multi_lang_name import NAME_TYPE_OFFICIAL
from component.default.multi_lang_name import NAME_TYPE_ROUTE_NUM
from component.default.multi_lang_name import MultiLangName
from component.default.multi_lang_name import MultiLangShield

NAME_SPIT_CHAR = '|'  # 名称分割符
NUM_SPECIAL_CHAR = '{'  # 番号特征符
MIN_NAME_LEN = 4
MAX_NUMBER_LEN = 4
# 国家和语种的对应表
LANGUAGE_CODE = {'Malaysia@MYS': 'MAY',  # 马来
                 'Brunei@BRN': 'MAY',  # 文莱
                 'Singapore@SGP': 'ENG'  # 新加坡
                 }
NAME_REPLACE_DICT = {}


#==============================================================================
# comp_dictionary_msm
#==============================================================================
class comp_dictionary_msm(component.default.dictionary.comp_dictionary):
    '''
    Name dictionary for Malsing.
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.default.dictionary.comp_dictionary.__init__(self)

    def _DoCreateTable(self):
        self.CreateTable2('temp_link_name')
        self.CreateTable2('temp_link_shield')
        return 0

    def _Do(self):
        # 创建语言种别的中间表
        self._InsertLanguages()
        # 设置语言code
        self.set_language_code()
        # 加载替换名称
        self._load_replaced_name()
        # 道路名称和番号(shield)
        self._make_link_name_number()
        # 保存当前地图使用语言
        self.store_language_flag()
        return 0

    def _load_replaced_name(self):
        '''加载替换名称'''
        sqlcmd = """
                SELECT org_name, replace_name
                  FROM road_replaced_name;
                """
        for name in self.get_batch_data(sqlcmd):
            org_name = name[0]
            replace_name = name[1]
            NAME_REPLACE_DICT[org_name] = replace_name

    def _make_link_name_number(self):
        '''道路名称和番号.'''
        temp_file_obj = cache_file.open('link_name')  # 创建临时文件(名称)
        temp_shield_file = cache_file.open('link_shield')  # 创建临时文件(番号)
        for name_info in self._get_name_info():
            link_id = name_info[0]
            org_name = name_info[1]
            l_country = name_info[2]
            r_country = name_info[3]
            if l_country != r_country:
                self.log.error('l_country != r_country. link')
                continue
            lang_code = LANGUAGE_CODE.get(l_country)
            if not lang_code:
                self.log.error('No Language code. country=%s' % l_country)
                continue
            if org_name in NAME_REPLACE_DICT:
                org_name = NAME_REPLACE_DICT.get(org_name)
            # 如果被替换成空，代表错误的，并要被删除的名称
            if not org_name:
                continue
            org_name_list = org_name.split(NAME_SPIT_CHAR)
            # 把名称， 拆成名称list和番号list
            name_list = self._get_name_and_number(org_name_list, l_country)
            road_number_list, road_name_list = name_list
            # ## 取得多语言多个名称的名称和番号
            multi_name = self._get_multi_name(road_name_list, road_number_list,
                                              lang_code)
            multi_number = self._get_multi_number(road_number_list, lang_code)
            if multi_name:
                json_name = multi_name.json_format_dump()
                # 存到本地的临时文件
                self._store_name_to_temp_file(temp_file_obj,
                                              link_id,
                                              json_name
                                              )
            if multi_number:
                json_name = multi_number.json_format_dump()
                # 存到本地的临时文件
                self._store_name_to_temp_file(temp_shield_file,
                                              link_id,
                                              json_name
                                              )
        # ## 把名称导入数据库
        temp_file_obj.seek(0)
        self.pg.copy_from2(temp_file_obj, 'temp_link_name')
        self.pg.commit2()
        #temp_file_obj.close()
        cache_file.close(temp_file_obj,True)
        # ## 把shield导入数据库
        temp_shield_file.seek(0)
        self.pg.copy_from2(temp_shield_file, 'temp_link_shield')
        self.pg.commit2()
        #temp_shield_file.close()
        cache_file.close(temp_shield_file,True)
        self.log.info('End Make Link Name and Shield Info.')

    def _get_name_and_number(self, name_list, country):
        road_number_list = []
        road_name_list = []
        for name in name_list:
            if self._is_road_number(name):  # 番号
                shield_id, road_number = \
                    self._get_road_number_info(name, country)
                if not shield_id or not road_number:
                    continue
                if len(road_number) > MAX_NUMBER_LEN:
                    if road_number != 'Q145X':
                        self.log.warning('Road Number is too long.'
                                         'road_number=%s' % road_number)
                if road_number.find(' ') > 0:
                    self.log.error('Blank in Road Number. road_number=%s'
                                   % road_number)
                if self._is_digit_plus_alphabet(road_number):
                    if road_number != 'Q145X':
                        self.log.error('ErrorRoad Number. road_number=%s'
                                       % road_number)
                if self._exist_unknown_word(road_number):
                    '''非常规字符'''
                    self.log.error('Unknown Word. road_number=%s'
                                   % road_number)
                if (shield_id, road_number) not in road_number_list:
                    road_number_list.append((shield_id, road_number))
            else:
                road_name = name.strip()
                # 删除全是数字的名称， 如：'0'， '3685'
                if self._all_is_digit(road_name):
                    if len(name_list) > 1 and name_list.index(road_name) == 0:
                        self.log.error('Error name: %s' % name_list)
                    continue
                # 删除名称长度小于最小名称长度(4)的名称
                if len(road_name) < MIN_NAME_LEN:
                    continue
                # 名称带有下列单词 , 如: 'Exit', 'To', 'Ramp', Entrance
                if self._exist_special_word(road_name):
                    continue
                if self._exist_backslash(road_name):
                    self.log.error('Name exist "\\". name=%s' % road_name)
                    continue
                if road_name not in road_name_list:  # 删除重复的名称
                    road_name_list.append(road_name)
        return road_number_list, road_name_list

    def _get_multi_name(self, road_name_list, road_number_list, lang_code):
        '''取得多语言多个名称的名称'''
        multi_name = None
        # ## 名称
        for name in road_name_list:
            if not multi_name:
                multi_name = MultiLangName(lang_code, name,
                                           NAME_TYPE_OFFICIAL)
            else:
                alte_name = MultiLangName(lang_code, name,
                                          NAME_TYPE_ALTER)
                multi_name.add_alter(alte_name)
        # ## 番号
        for shieldid, number in road_number_list:
            # ## 把番号添加名称
            if number in road_name_list:
                continue
            if not multi_name:
                multi_name = MultiLangName(lang_code, number,
                                           NAME_TYPE_ROUTE_NUM)
            else:
                alte_name = MultiLangName(lang_code, number,
                                          NAME_TYPE_ROUTE_NUM)
                multi_name.add_alter(alte_name)
        return multi_name

    def _get_multi_number(self, road_number_list, lang_code):
        multi_number = None
        # ## 番号
        for shieldid, number in road_number_list:
            if not multi_number:
                multi_number = MultiLangShield(shieldid, number,
                                               lang_code)
            else:
                alte_number = MultiLangShield(shieldid, number,
                                              lang_code)
                multi_number.add_alter(alte_number)
        return multi_number

    def _all_is_digit(self, name):
        '''全是数字'''
        s = name.strip()
        temp = re.findall(r'\D', s)
        if temp:
            return False
        else:
            return True

    def _exist_special_word(self, name):
        '''名称带有单词：[Exit, exit, To, to, Ramp,
                      ramp, Entrance, entrance, U-turn]
        '''
        words = [r'\Wexit\W',  # Exit/exit
                 r'\Wexiting\W',  # Exiting
                 r'\Wto\W',   # To/to
                 r'\Wramps*\W',  # Ramp/ramp/Ramps
                 r'\Wentrance\W',  # Entrance/entrance
                 r'\Wenter\W',  # Enter
                 r'\Wentering\W',  # Entering
                 # U-turn/u-turn/U-turns/U turn/Left turn/
                 # Left-turn/Right-turn/Right turn/UTurn==
                 r'\Wu*turns?\W',
                 r'\Wrightturns?\W',  # RightTurn
                 r'\Wleftturns?\W',  # LeftTurn
                 r'\Wvehicles\W',  # Vehicles
                 ]
        name = ' ' + name + ' '
        for word in words:
            p = re.compile(word, re.I)  # 忽略大小写
            temp = p.findall(name)
            if temp:
                # self.log.info(name)
                return True
        return False

    def _exist_backslash(self, name):
        '''存在反斜杠"\"'''
        if name.find('\\') >= 0:
            return True
        return False

    def _get_road_number_info(self, name, country):
        '''取得番号和shield_id'''
        if name.count(NUM_SPECIAL_CHAR) > 1:  # 番号个数大于1
            self.log.error('Road Number > 1.')
            return None, None
        if name[0] != NUM_SPECIAL_CHAR:
            self.log.error("name[0] != '{'. name=%s" % name)
            return None, None
        hwy_icon = name[0:2]
        shield_id = self._get_shield_id(hwy_icon, country)
        if not shield_id:
            self.log.error('No Shield ID. name=%s,country=%s'
                           % (name, country))
            return None, None
        road_number = self._get_road_number(name)
        if self._exist_brackets(road_number):
            self.log.warning('Exist Brackets.name=%s' % name)
        road_number = road_number.strip()
        return shield_id, road_number

    def _get_road_number(self, name):
        number = name[2:]
        # 去掉番号中的'.'
        number = number.replace('.', '')
        # ## 去掉'()'及里面的内容
        brackets = re.findall(r'\(.*\)', number)
        for bracket in brackets:
            number = number.replace(bracket, '')
        # 去掉'-'
        pos = number.find('-')
        if pos >= 0:
            number = number[:pos]
        return number

    def _get_shield_id(self, hwy_icon, country):
        shield_dict = {'Malaysia@MYS': {'{M': '4101',
                                        '{P': '4102',
                                        '{O': '4103'
                                        },
                       'Brunei@BRN': {'{O': '4103'},
                       'Singapore@SGP': {'{M': '4111',
                                         '{P': '4111',
                                         '{O': '4111'
                                         }
                       }
        country_dist = shield_dict.get(country)
        if country_dist:
            return country_dist.get(hwy_icon)
        else:
            return None

    def _exist_brackets(self, s):
        '''有括号'''
        if s.find('(') >= 0 or s.find(')') >= 0:
            return True
        else:
            return False

    def _is_road_number(self, name):
        if name.find(NUM_SPECIAL_CHAR) >= 0:
            return True
        else:
            return False

    def _is_digit_plus_alphabet(self, number):
        '''数字+字母'''
        temp = re.findall(r'\D*\d+\D+', number)
        if temp:
            return True
        return False

    def _exist_unknown_word(self, number):
        temp = re.findall(r'\W', number)
        if temp:
            return True
        return False

    def _get_name_info(self):
        sqlcmd = '''
        SELECT new_link_id, name, l_country, r_country
          FROM org_processed_line AS line
          LEFT JOIN temp_topo_link AS link
          ON line.folder = link.folder and line.link_id = link.link_id
          WHERE name is not null AND name <> ''
          order by new_link_id
        '''
        return self.pg.get_batch_data2(sqlcmd)

    def _store_name_to_temp_file(self, file_obj, nid, json_name):
        if file_obj:
            file_obj.write('%d\t%s\n' % (nid, json_name))
        return 0
