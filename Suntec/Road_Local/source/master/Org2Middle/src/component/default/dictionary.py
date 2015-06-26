# -*- coding: cp936 -*-
'''
Created on 2012-3-29

@author: hongchenzai
'''
import common.common_func
import component.component_base
from component.default.multi_lang_name import SHIELD_SPLIT_CHR
from component.default.multi_lang_name import NAME_PRIORITY
from component.default.multi_lang_name import MultiLangName
from component.default.multi_lang_name import NAME_TYPE_ROUTE_NUM
from component.default.multi_lang_name import NAME_TYPE_ALTER
from component.default.multi_lang_name import NAME_TYPE_OFFICIAL


class comp_dictionary(component.component_base.comp_base):
    '''字典
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Dictionary')

    def _DoCreateTable(self):
        # 名称字典表
        self.CreateTable2('name_dictionary_tbl')
        self.CreateTable2('language_tbl')
        # link和名称的临时关联表
        self.CreateTable2('temp_link_name')
        # 道路番号和名称的临时关联表
        self.CreateTable2('temp_link_no')
        # node和名称的临时关联表
        self.CreateTable2('temp_node_name')
        # 方面名称和名称的临时关联表
        self.CreateTable2('temp_toward_name')
        # 方面看板和名称的临时关联表
        self.CreateTable2('temp_signpost_name')
        # POI名称和名称的临时关联表
        self.CreateTable2('temp_poi_name')
        return 0

    def _Do(self):
        # 创建语言种别的中间表
        self._InsertLanguages()

        # 道路名称字典
        self._MakeLinkName()
        # 道路番号字典
        self._MakeRoadNumber()
        # 交叉点名称字典
        self._MakeCrossName()
        # 方面名称字典
        self._MakeTowardName()
        # 方面看板名称字典
        self._MakeSignPostName()
        # POI名称字典
        self._MakePOIName()
        return 0

    def _MakeLinkName(self):
        "道路名称字典"
        return 0

    def _MakeRoadNumber(self):
        "道路名称字典"
        return 0

    def _MakeCrossName(self):
        "交叉点名称字典"
        return 0

    def _MakeTowardName(self):
        "方面名称字典"
        return 0

    def _MakeSignPostName(self):
        "方面看板名称字典"
        return 0

    def _MakePOIName(self):
        "POI名称字典"
        return 0

    def _InsertLanguages(self):
        '往language_tbl表，插入语言记录。'
        path = common.common_func.GetPath('language_table')
        self.CreateTable2('language_tbl')
        language_list = common.common_func.GetAllLanguages(path)
        sqlcmd = """
                     INSERT INTO language_tbl(
                                        language_id
                                      , l_full_name
                                      , l_talbe
                                      , pronunciation
                                      , p_table
                                      , language_code
                                      , language_code_client
                                      , language_id_client)
                            VALUES (%s, %s, %s, %s,
                                    %s, %s, %s, %s);
                     """
        for recod in language_list:
            self.pg.execute2(sqlcmd, recod)
        self.pg.commit2()

        self.CreateIndex2('language_tbl_language_code_idx')
        self.CreateTable2('language_tbl_l_full_name_idx')
        self.CreateTable2('language_tbl_l_talbe_idx')

    def set_language_code(self):
        from component.default.multi_lang_name import MultiLangName
        if not MultiLangName.is_initialized():
            self.log.info('Set language Code.')
            self.pg.connect2()
            sqlcmd = """
            select language_code
               from language_tbl
               order by language_code;
            """
            code_dict = dict()
            language_codes = self.get_batch_data(sqlcmd)
            for lang_info in language_codes:
                lang_code = lang_info[0]
                code_dict[lang_code] = None
            MultiLangName.set_language_code(code_dict)
        return 0

    def store_language_flag(self):
        '''保存当前地图使用语言.'''
        from component.default.multi_lang_name import MultiLangName
        sqlcmd = """UPDATE language_tbl SET exist_flag = True
                    WHERE language_code = %s;
                """
        languages = MultiLangName.get_language_code()
        for lang_code, flag in languages.iteritems():
            if flag:
                self.pg.execute2(sqlcmd, (lang_code, ))
        self.pg.commit2()

    def merge_links_name(self, link_list):
        road_name_list = []
        road_number_list = []
        self.pg.connect2()
        for road_names, road_numbers in self._get_links_name_number(link_list):
            if road_names:
                self._merge_name(road_name_list, road_names)
            if road_numbers:
                self._merge_number(road_number_list, road_numbers)
        # 把番号追加到名称的结尾
        self._append_num_2_name(road_number_list, road_name_list)
        json_name = MultiLangName.json_format_dump2(road_name_list)
        json_shield = MultiLangName.json_format_dump2(road_number_list)
        return json_name, json_shield

    def _merge_name(self, road_name_list, road_names):
        '''合并道路名称'''
        for road_name in road_names:
            if not road_name_list:
                road_name_list.append(road_name)
                continue
            name_type = road_name[0].get('type')
            if name_type == NAME_TYPE_ROUTE_NUM:
                continue
            if road_name in road_name_list:
                continue
            name_pri = NAME_PRIORITY.get(name_type)
            name_cnt = 0
            while name_cnt < len(road_name_list):
                temp_name = road_name_list[name_cnt]
                temp_name_type = temp_name[0].get('type')
                temp_name_pri = NAME_PRIORITY.get(temp_name_type)
                if name_pri < temp_name_pri:  # 值越小等级越高
                    break
                name_cnt = name_cnt + 1
            # 如果种别是官方名称，改到成别名
            if name_type == NAME_TYPE_OFFICIAL:
                for name_dict in road_name:
                    name_dict['type'] = NAME_TYPE_ALTER
            road_name_list.insert(name_cnt, road_name)

    def _merge_number(self, road_number_list, road_numbers):
        '''合并番号'''
        for road_number in road_numbers:
            if not road_number_list:
                road_number_list.append(road_number)
                continue
            if road_number in road_number_list:
                continue
            number_info = road_number[0].get('val').split(SHIELD_SPLIT_CHR)
            shield_id = number_info[0]
            number = number_info[1]
            num_cnt = 0
            while num_cnt < len(road_number_list):
                temp_road_num = road_number_list[num_cnt]
                num_info = temp_road_num.get('val').split(SHIELD_SPLIT_CHR)
                temp_shield_id = num_info[0]
                temp_number = num_info[1]
                if shield_id < temp_shield_id:  # 值越小等级越高
                    break
                elif shield_id == temp_shield_id:
                    if len(number) < len(temp_number):
                        break
                    elif (len(number) == len(temp_number)
                          and number < temp_number):
                        break
                    else:
                        pass
                else:
                    pass
                num_cnt = num_cnt + 1
            road_number_list.insert(num_cnt, road_number)

    def _append_num_2_name(self, road_number_list, road_name_list):
        for road_number in road_number_list:
            import copy
            road_num = copy.deepcopy(road_number)
            for num_dict in road_num:
                number = num_dict.get('val').split(SHIELD_SPLIT_CHR)[1]
                num_dict['val'] = number
                num_dict['type'] = NAME_TYPE_ROUTE_NUM
            road_name_list.append(road_num)

    def _get_links_name_number(self, link_list):
        sqlcmd = """
        SELECT road_name, road_number
          FROM link_tbl
          where link_id in (links)
          order by link_id;
        """
        str_link_lid = str(tuple(link_list)).replace('L', '')
        sqlcmd = sqlcmd.replace('(links)', str_link_lid)
        self.pg.execute2(sqlcmd)
        import json
        for info in self.pg.fetchall2():
            if info[0]:
                road_names = json.loads(info[0])
            else:
                road_names = []
            if info[1]:
                road_numbers = json.loads(info[1].replace('\t', '\\t'))
            else:
                road_numbers = []
            yield road_names, road_numbers
