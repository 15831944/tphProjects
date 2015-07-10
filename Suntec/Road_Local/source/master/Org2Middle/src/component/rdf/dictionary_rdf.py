# -*- coding: UTF8 -*-
'''
Created on 2012-3-29

@author: hongchenzai
'''
import component.default.dictionary
from component.rdf.multi_lang_name_rdf import MultiLangNameRDF
from component.rdf.multi_lang_name_rdf import MultiLangShieldRDF
from common import cache_file
from component.rdf.route_num_parser import RouteNumParser
from component.rdf.route_num_parser import RouteNumParserNA
from component.rdf.route_num_parser import RouteNumParserHKG
from component.rdf.route_num_parser import RouteNumParserARA8
import re
import common

INVALID_SHIELD_ID = str(-1)
# 北美
NORTH_AMERICA_COUNTRYS = {'USA': None, 'CAN': '加拿大',
                          'MEX': '墨西哥', 'PRI': '波多黎各',
                          'VIR': '美属维尔京',
                          }
# 有翻译名称的国家
TRANSNAME_COUNTRYS = {'THA': '泰国', 'VNM': '越南',
                      'HKG': '香港', 'TWN': '台湾',
                      'BHR': '巴林', 'OMN': '阿曼',
                      'JOR': '约旦', 'KWT': '科威特',
                      'QAT': '卡塔尔', 'LBN': '黎巴嫩',
                      'SAU': '沙特阿拉伯', 'ARE': '阿拉伯联合酋长国',
                      'TWN': '台湾',
                      }


class comp_dictionary_rdf(component.default.dictionary.comp_dictionary):
    '''字典(RDF)
    '''
    vce_language_code_map = None

    def __init__(self):
        '''
        Constructor
        '''
        component.default.dictionary.comp_dictionary.__init__(self)

    def _DoCreateTable(self):
        # component.dictionary.comp_dictionary._DoCreateTable(self)
        self.CreateTable2('language_tbl')
        self.CreateTable2('temp_link_name')
        self.CreateTable2('temp_link_shield')
        return 0

    def _DoCreateFunction(self):
        return 0

    def _Do(self):
        # 创建语言种别的中间表
        self._InsertLanguages()
        # 设置语言code
        self.set_language_code()
        # 对同个名称(包括番号)的多个翻译进行group by
        self._group_road_trans_name()
        # 对名称(包括番号)进行排序
        self._sort_road_name()
        # 道路名称音素(包括番号)
        self._make_road_phonetic()
        # 道路名称
        self._make_link_name()
        # 道路shield和番号
        self._make_link_shield()
        # ## feature名称(包括行政区)
        # 行政区名称音素
        self._make_feature_phonetic()
        self._make_feature_name()
        return 0

    def set_language_code(self):
        self.log.info('Set language Code.')
        if not MultiLangNameRDF.is_initialized():
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
            MultiLangNameRDF.set_language_code(code_dict)
        return 0

    def _make_feature_phonetic(self):
        '''行政区名称音素。'''
        self.CreateTable2('mid_temp_feature_phontetic')
        sqlcmd = """
        INSERT INTO mid_temp_feature_phontetic(name_id, phonetic_ids,
                        phonetic_strings, phonetic_language_codes)
        (
        SELECT name_id, array_agg(phonetic_id) as phonetic_ids,
               array_agg(phonetic_string) as phonetic_strings,
               array_agg(phonetic_language_code) as phonetic_language_codes
          FROM (
                SELECT name_id, phonetic.phonetic_id, preferred,
                       phonetic_string, phonetic_language_code,
                       transcription_method
                  FROM vce_feature_name
                  left join vce_phonetic_text as phonetic
                  ON vce_feature_name.phonetic_id = phonetic.phonetic_id
                  order by name_id, preferred desc,
                           transcription_method desc,
                           phonetic_id
          ) as a
          group by name_id
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _make_feature_name(self):
        self.log.info('Make Feature Name.')
        self.CreateTable2('mid_temp_feature_name')
        temp_file_obj = cache_file.open('feature_name')  # 创建临时文件
        sqlcmd = """
        SELECT feature_id, fns.name_id, language_code, name,
               trans_type_array, trans_name_array,
               iso_country_code,
               phonetic_strings,
               phonetic_language_codes
          FROM rdf_admin_hierarchy AS adm
          LEFT JOIN rdf_feature_names AS fns
          ON adm.admin_place_id = fns.feature_id
          LEFT JOIN rdf_feature_name as fn
          ON fns.name_id = fn.name_id
          LEFT JOIN (
              SELECT name_id,
                     array_agg(trans_type) as trans_type_array,
                     array_agg(trans_name) as trans_name_array
              FROM (
                -- Transliteration Name
                SELECT name_id,
                      transliteration_type AS trans_type,
                      "name" as trans_name
                  FROM rdf_feature_name_trans
                  order by name_id, transliteration_type
              ) AS A
              GROUP BY name_id
          ) as trans
          ON fn.name_id = trans.name_id
          LEFT JOIN mid_temp_feature_phontetic as tts
          ON tts.name_id = fns.name_id
          ORDER BY feature_id, is_exonym, name_type, fns.name_id;
        """
        features = self.get_batch_data(sqlcmd)
        curr_feature_id = None
        prev_feature_id = None
        feature_name = None
        lang_code_dict = list()
        for feature in features:
            curr_feature_id = feature[0]
            name_id = feature[1]
            lang_code = feature[2]
            name = feature[3]
            trans_types = feature[4]  # 翻译语言种别
            trans_names = feature[5]  # 翻译名称
            iso_country_code = feature[6]
            phonetic_strings = feature[7]
            phonetic_lang_codes = feature[8]
            
            if not curr_feature_id:
                self.log.error('No feature_id.')
                continue
            if not name_id:
                self.log.error('No name_id. feature_id=' % curr_feature_id)
                continue
            if prev_feature_id != curr_feature_id:  # feature变更
                if prev_feature_id:
                    json_name = feature_name.json_format_dump()
                    # 存到本地的临时文件
                    self._store_name_to_temp_file(temp_file_obj,
                                                  prev_feature_id,
                                                  json_name)
                # 初始化
                lang_code_dict = list()
                prev_feature_id = curr_feature_id
                lang_code_dict.append(lang_code)
                feature_name = MultiLangNameRDF(lang_code, name)
                if phonetic_strings:  # 有音素
                    tts_obj = self.get_tts_name(iso_country_code,
                                                lang_code,
                                                feature_name.get_name_type(),
                                                phonetic_strings,
                                                phonetic_lang_codes
                                                )
                    if tts_obj:
                        feature_name.set_tts(tts_obj)
                    else:
                        self.log.warning('No TTS. feature_id=%d, name_id=%d' %
                                         (prev_feature_id, name_id))
                feature_name = self.set_trans_name(feature_name,
                                                   iso_country_code,
                                                   trans_names,
                                                   trans_types,
                                                   phonetic_strings,
                                                   phonetic_lang_codes
                                                   )
            else:  # feature没有变更
                if lang_code in lang_code_dict:
                    is_office_name = 'N'
                else:
                    lang_code_dict.append(lang_code)
                    is_office_name = 'Y'
                name_type = self._get_name_type(is_office_name,None)
                
                alter_name = MultiLangNameRDF(lang_code, name, name_type)
                if phonetic_strings:  # 有音素
                    tts_obj = self.get_tts_name(iso_country_code,
                                                lang_code,
                                                feature_name.get_name_type(),
                                                phonetic_strings,
                                                phonetic_lang_codes
                                                )
                    if tts_obj:
                        alter_name.set_tts(tts_obj)
                    else:
                        self.log.warning('No TTS. feature_id=%d, name_id=%d' %
                                         (prev_feature_id, name_id))
                alter_name = self.set_trans_name(alter_name,
                                                  iso_country_code,
                                                  trans_names,
                                                  trans_types,
                                                  phonetic_strings,
                                                  phonetic_lang_codes
                                                  )
                # feature的别名，暂时做成翻译(即多语言，而不是多名称)
                feature_name.add_trans(alter_name)
        # 最后一条
        if feature_name:
            json_name = feature_name.json_format_dump()
            # 存到本地的临时文件
            self._store_name_to_temp_file(temp_file_obj,
                                          prev_feature_id,
                                          json_name)

        # ## 把名称导入数据库
        temp_file_obj.seek(0)
        self.pg.copy_from2(temp_file_obj, 'mid_temp_feature_name')
        self.pg.commit2()
        # close file
        #temp_file_obj.close()
        cache_file.close(temp_file_obj,True)
        self.log.info('End Make Link Name.')
        return 0

    def _group_road_trans_name(self):
        '''对同个名称(包括番号)的多个翻译进行group by. 名称之间用"|"分开。'''
        self.CreateTable2('mid_temp_road_name_trans_group')
        sqlcmd = """
        INSERT INTO mid_temp_road_name_trans_group(
                        road_name_id, trans_types, trans_names)
        (
        SELECT road_name_id,
               array_to_string(array_agg(trans_type), '|') AS trans_types,
               array_to_string(array_agg(trans_name), '|') AS trans_names
          FROM (
                SELECT road_name_id,
                       transliteration_type AS trans_type,
                       street_name as trans_name
                  FROM rdf_road_name_trans
                  order by road_name_id, transliteration_type
          ) AS A
          GROUP BY road_name_id
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return

    def _sort_road_name(self):
        '''对名称(包括番号)进行排序, 并把结果存到临时表mid_temp_road_name,同时生成一个gid做为优先序号'''
        self.log.info('Sort Road Name.')
        self.CreateTable2('mid_temp_road_name')
        # ## 先对名称进行排序
        # 排序规则：
        # 1) link_id
        # 2) 官名名称(is_name_on_roadsign的倒序)
        # 3) 道路名称优先 (street_type非空的为道路名称)
        # 4) is_junction_name,
        # 5) is_vanity_name,
        # 6) is_scenic_name,
        # 7) road_link_id
        sqlcmd = """
        INSERT INTO mid_temp_road_name(
                    link_id, road_link_id, road_name_id,
                    left_address_range_id, right_address_range_id,
                    address_type, is_exit_name, explicatable,
                    is_junction_name, is_name_on_roadsign,
                    is_postal_name, is_stale_name,
                    is_vanity_name, is_scenic_name,
                    route_type, street_type, language_code, street_name)
        (
        SELECT link_id, road_link_id, rdf_road_link.road_name_id,
               left_address_range_id, right_address_range_id,
               address_type, is_exit_name, explicatable,
               is_junction_name, is_name_on_roadsign,
               is_postal_name, is_stale_name,
               is_vanity_name, is_scenic_name,
               route_type, street_type, language_code, street_name
          FROM rdf_road_link
          left join rdf_road_name
          on rdf_road_link.road_name_id = rdf_road_name.road_name_id
          where rdf_road_link.road_name_id is not null  -- no name,
                and is_exit_name <> 'Y'                 -- Not exist name
                and rdf_road_name.route_type is null
          order by link_id,
                   is_name_on_roadsign desc, -- 'Y': offical name, 'N':其他名称
                   street_type,      -- street_type is null: not Street name
                   is_junction_name,
                   is_vanity_name,
                   is_scenic_name,
                   road_link_id
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        # ## 再对番号进行排序
        self._sort_road_number()
        self.CreateIndex2('mid_temp_road_name_link_id_idx')
        self.CreateIndex2('mid_temp_road_name_route_type_idx')

    def _sort_road_number(self):
        '''对番号进行排序'''
        self.log.info('Sort Road Number.')
        sqlcmd = """
        INSERT INTO mid_temp_road_name(
                    link_id, road_link_id, road_name_id,
                    left_address_range_id, right_address_range_id,
                    address_type, is_exit_name, explicatable,
                    is_junction_name, is_name_on_roadsign,
                    is_postal_name, is_stale_name,
                    is_vanity_name, is_scenic_name,
                    route_type, street_type, language_code, street_name)
        (
        SELECT link_id, road_link_id, rdf_road_link.road_name_id,
               left_address_range_id, right_address_range_id,
               address_type, is_exit_name, explicatable,
               is_junction_name, is_name_on_roadsign,
               is_postal_name, is_stale_name,
               is_vanity_name, is_scenic_name,
               route_type, street_type, language_code, street_name
          FROM rdf_road_link
          left join rdf_road_name
          on rdf_road_link.road_name_id = rdf_road_name.road_name_id
          where rdf_road_link.road_name_id is not null
                and is_exit_name <> 'Y'                   -- Not Not exist name
                and rdf_road_name.route_type is not null  -- Route Number,
          order by link_id,
                   route_type,
                   length(rdf_road_name.street_name),
                   street_name
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return 0

    def _make_road_phonetic(self):
        '''道路名称的音素。'''
        self.CreateTable2('mid_temp_road_phonetic')
        sqlcmd = """
        INSERT INTO mid_temp_road_phonetic(road_name_id, phonetic_ids,
                        phonetic_strings, phonetic_language_codes)
        (
        SELECT road_name_id,
               array_agg(phonetic_id) as phonetic_ids,
               array_agg(phonetic_string) as phonetic_strings,
               array_agg(phonetic_language_code) as phonetic_language_codes
          from (
                SELECT road_name_id, vce_road_name.phonetic_id, preferred,
                       phonetic_string, phonetic_language_code,
                       transcription_method
                  FROM vce_road_name
                  left join vce_phonetic_text
                  ON vce_road_name.phonetic_id = vce_phonetic_text.phonetic_id
                  order by road_name_id,
                           phonetic_language_code,
                           preferred desc,
                           transcription_method desc,
                           phonetic_id
          ) as a
          group by road_name_id
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _make_link_name(self):
        "道路名称(包括道路番号)字典"
        self.log.info('Make Link Name.')
        sqlcmd = """
        SELECT gid, name.link_id, language_code, street_name,
               trans_types, trans_names,
               route_type, is_name_on_roadsign,
               name.road_name_id,
               iso_country_code,
               phonetic_strings,
               phonetic_language_codes
          FROM mid_temp_road_name as name
          LEFT JOIN mid_temp_road_name_trans_group as trans
          ON name.road_name_id = trans.road_name_id
          LEFT JOIN temp_rdf_nav_link
          ON name.link_id = temp_rdf_nav_link.link_id
          LEFT JOIN mid_temp_road_phonetic as tts
          ON name.road_name_id = tts.road_name_id
          order by link_id, gid;
        """
        temp_file_obj = cache_file.open('link_name')  # 创建临时文件
        names = self.get_batch_data(sqlcmd)
        prev_link_id = None
        curr_link_id = None
        multi_name_obj = None
        for nameinfo in names:
            curr_link_id = nameinfo[1]
            lang_code = nameinfo[2]  # 官方语言种别
            official_name = nameinfo[3]  # 官方语言种别道路名称
            trans_langs = nameinfo[4]  # 翻译语言种别
            trans_names = nameinfo[5]  # 翻译语言种别道路名称
            route_type = nameinfo[6]  # 番号种别
            is_office_name = nameinfo[7]
            road_name_id = nameinfo[8]
            iso_country_code = nameinfo[9]
            phonetic_strings = nameinfo[10]  # TTS音素
            phonetic_lang_codes = nameinfo[11]  # 音素语言种别
            if not curr_link_id:  # link id不存在
                self.log.error("No Link ID.")
                continue

            if prev_link_id != curr_link_id:  # Link id 变更
                if prev_link_id:
                    # ## 保存上一条link的名称
                    json_name = multi_name_obj.json_format_dump()
                    if not json_name:
                        self.log.error('Json Name is none. linkid=%d' %
                                       prev_link_id)
                    # 存到本地的临时文件
                    self._store_name_to_temp_file(temp_file_obj,
                                                  prev_link_id,
                                                  json_name)
                # ## 初始化
                name_type = self._get_name_type(is_office_name,
                                                route_type,
                                                True)
                multi_name_obj = MultiLangNameRDF(lang_code,
                                                  official_name,
                                                  name_type)
                if phonetic_strings:  # 有音素
                    tts_obj = self.get_tts_name(iso_country_code,
                                                lang_code,
                                                name_type,
                                                phonetic_strings,
                                                phonetic_lang_codes
                                                )
                    if tts_obj:
                        multi_name_obj.set_tts(tts_obj)
                    else:
                        self.log.warning('No TTS. linkid=%d, name_id=%d' %
                                         (curr_link_id, road_name_id))
                multi_name_obj = self.set_trans_name(multi_name_obj,
                                                     iso_country_code,
                                                     trans_names,
                                                     trans_langs,
                                                     phonetic_strings,
                                                     phonetic_lang_codes
                                                     )
                prev_link_id = curr_link_id
            else:  # Link_id 没有变化
                name_type = self._get_name_type(is_office_name,
                                                route_type)
                alter_name = MultiLangNameRDF(lang_code,
                                              official_name,
                                              name_type)
                if phonetic_strings:
                    tts_obj = self.get_tts_name(iso_country_code,
                                                     lang_code,
                                                     name_type,
                                                     phonetic_strings,
                                                     phonetic_lang_codes)
                    if tts_obj:
                        alter_name.set_tts(tts_obj)
                    else:
                        self.log.warning('No TTS. linkid=%d, name_id=%d' %
                                         (curr_link_id, road_name_id))
                alter_name = self.set_trans_name(alter_name,
                                                 iso_country_code,
                                                 trans_names,
                                                 trans_langs,
                                                 phonetic_strings,
                                                 phonetic_lang_codes
                                                 )
                multi_name_obj.add_alter(alter_name)

        # ## 最后一条link的名称
        json_name = multi_name_obj.json_format_dump()
        if not json_name:
            self.log.error('Json Name is none. linkid=%s' % prev_link_id)
        # 存到本地的临时文件
        self._store_name_to_temp_file(temp_file_obj,
                                      prev_link_id,
                                      json_name)
        # ## 把名称导入数据库
        temp_file_obj.seek(0)
        self.pg.copy_from2(temp_file_obj, 'temp_link_name')
        self.pg.commit2()
        # close file
        #temp_file_obj.close()
        cache_file.close(temp_file_obj,True)
        
        self.CreateIndex2('temp_link_name_link_id_idx')
        
        self.log.info('End Make Link Name.')
        return 0

    def set_trans_name(self,
                       multi_name,
                       iso_country_code,
                       trans_names,
                       luaguage_codes,
                       phonetic_strings,
                       phonetic_lang_codes,
                       split_char='|'):
        if not trans_names:
            return multi_name
        if type(trans_names) is not list:
            if split_char:
                trans_names = trans_names.split(split_char)
                luaguage_codes = luaguage_codes.split(split_char)
            else:  # 当split_char为空or空格：不做切割
                trans_names = [trans_names]
                luaguage_codes = [luaguage_codes]

        name_num = len(trans_names)
        name_cnt = 0
        while name_cnt < name_num:
            name = trans_names[name_cnt]
            if not name:
                name_cnt += 1
                continue
            lang_code = luaguage_codes[name_cnt]
            trans_obj = MultiLangNameRDF(lang_code,
                                         name,
                                         multi_name.get_name_type(),
                                         'not_tts'
                                         )
            if phonetic_strings:
                tts_obj = self.get_tts_name(iso_country_code,
                                            lang_code,
                                            trans_obj.get_name_type(),
                                            phonetic_strings,
                                            phonetic_lang_codes
                                            )
                if tts_obj:
                    trans_obj.set_tts(tts_obj)
                else:
                    if (len(phonetic_strings) > 1 and
                        len(set(phonetic_lang_codes)) > 1):  # 所有种别是相同
                        self.log.warning('No TTS, name=%s' % name)
                        pass
            multi_name.add_trans(trans_obj)
            name_cnt += 1
        return multi_name

    def _get_name_type(self, is_office_name, route_type, first=False):
        if is_office_name == 'Y' and not route_type:  # 官方，且不是番号
            return 'office_name'
        if route_type:  # 番号
            return 'route_num'
        if first:  # 第一名称，做成官方名
            return 'office_name'
        # 其他 ==>别名
        return 'alter_name'

    def _store_name_to_temp_file(self, file_obj, nid, json_name):
        if file_obj:
            file_obj.write('%d\t%s\n' % (nid, json_name))
        return 0

    def _make_link_shield(self):
        '''制作道路番号字典。'''
        self.log.info('Make Link Shield.')
        # 从rdf_road_link和rdf_road_name，提起道路番号
        sqlcmd = """
        SELECT gid, name.link_id,
               language_code, street_name,
               trans_types, trans_names,
               route_type, iso_country_code,
               name.road_name_id
          FROM mid_temp_road_name as name
          LEFT JOIN mid_temp_road_name_trans_group as trans
          ON name.road_name_id = trans.road_name_id
          LEFT JOIN rdf_nav_link
          ON name.link_id = rdf_nav_link.link_id
          where route_type is not null
          order by link_id, gid;
        """
        temp_file_obj = cache_file.open('link_shield')  # 创建临时文件
        numbers = self.get_batch_data(sqlcmd)
        prev_link_id = None
        curr_link_id = None
        multi_shield_obj = None

        for number_info in numbers:
            curr_link_id = number_info[1]
            lang_code = number_info[2]  # 官方语言种别
            shield_number = number_info[3]  # 官方语言种别番号
            trans_langs = number_info[4]  # 翻译语言种别
            trans_names = number_info[5]  # 翻译语言种别道路名称
            route_type = number_info[6]  # 番号种别
            iso_country_code = number_info[7]
            if not curr_link_id:  # link id不存在
                self.log.error("No Link ID.")
                continue
            if shield_number.find("GO-070 QUADRA 1") == 0:
                shield_number = "GO-070"
            if iso_country_code in NORTH_AMERICA_COUNTRYS:
                rn_parser = RouteNumParserNA(shield_number)
            elif iso_country_code in ('HKG', 'MAC', 'TWN'):
                rn_parser = RouteNumParserHKG(shield_number)
            elif iso_country_code in ('BHR','OMN','JOR','KWT','QAT','LBN','SAU','ARE'):
                rn_parser = RouteNumParserARA8(shield_number)
            else:
                rn_parser = RouteNumParser(shield_number)
            rn_parser.do_parse()
            prefix = rn_parser.get_prefix()
            number = rn_parser.get_number()
            suffix = rn_parser.get_modifier()
            direction = rn_parser.get_dir()
            if not number:
                self.log.warning("Error Shield Number or Error RN."
                                 "LinkId=%d, RN=%s."
                                 % (curr_link_id, shield_number))
                continue
            if prefix == 'I' and route_type != 1 and iso_country_code == 'USA':
                route_type = 1
            shield_id = self.get_shield_id(iso_country_code,
                                           route_type,
                                           number,trans_langs,trans_names,lang_code,shield_number)
            if prev_link_id != curr_link_id:  # Link id 变更
                if prev_link_id:
                    # ## 保存上一条link的名称
                    # 上条link的最后一个名称
                    if multi_shield_obj:
                        json_name = multi_shield_obj.json_format_dump()
                        if not json_name:
                            self.log.error('Json Shield is none. linkid=%d'
                                           % prev_link_id)
                        # 存到本地的临时文件
                        self._store_name_to_temp_file(temp_file_obj,
                                                      prev_link_id,
                                                      json_name)
                if not shield_id:
                    multi_shield_obj = None
                    continue
                multi_shield_obj = MultiLangShieldRDF(shield_id,
                                                      number,
                                                      lang_code,
                                                      direction=direction,
                                                      prefix=prefix,
                                                      suffix=suffix)
                if trans_names and iso_country_code not in TRANSNAME_COUNTRYS:
                    self.log.warning('Exist Trans Route Num. country_code= %s'
                                     % iso_country_code)
                multi_shield_obj.set_trans(trans_names, trans_langs,
                                           rn_parser=rn_parser)
                prev_link_id = curr_link_id
            else:  # Link id 没有变化
                if not shield_id:
                    continue
                if not multi_shield_obj:
                    self.log.error('Error ShieldId, linkid=%s' % curr_link_id)
                alter_shield = MultiLangShieldRDF(shield_id,
                                                  number,
                                                  lang_code,
                                                  direction=direction,
                                                  prefix=prefix,
                                                  suffix=suffix)
                if trans_names and iso_country_code not in TRANSNAME_COUNTRYS:
                    self.log.warning('Exist Trans Route Num. country_code= %s'
                                     % iso_country_code)
                alter_shield.set_trans(trans_names, trans_langs,
                                       rn_parser=rn_parser)
                multi_shield_obj.add_alter(alter_shield)
        # ## 最后一条link的名称
        if multi_shield_obj:
            json_name = multi_shield_obj.json_format_dump()
            if not json_name:
                self.log.error('Json Shield is none. linkid=%s' % prev_link_id)
            # 存到本地的临时文件
            self._store_name_to_temp_file(temp_file_obj,
                                          prev_link_id,
                                          json_name)

        # ## 把名称导入数据库
        temp_file_obj.seek(0)
        self.pg.copy_from2(temp_file_obj, 'temp_link_shield')
        self.pg.commit2()
        # close file
        #temp_file_obj.close()
        cache_file.close(temp_file_obj, True)

        self.CreateIndex2('temp_link_shield_link_id_idx')

        self.log.info('End Make Link Shield Info.')
        return 0

    def get_shield_id(self, iso_country_code, route_type, route_num, trans_langs,trans_names, 
                      language_code, street_name):
        shield_id_dict = {"BRN": {1: 4121, 2: 4121,
                                  3: 4121, 4: 4121,
                                  5: 4121, 6: 4121},  # Brunei Darussalam
                          "CHN": {},
                          "HKG": {1: 4151},  # Hong Kong
                          "JPN": {},
                          "KOR": {},  # KOREAN
                          "LAO": {1: 4181, 2: 4181,
                                  3: 4181, 4: 4181,
                                  5: 4181, 6: 4181},  # Laos
                          "MAC": {},  # Macau
                          "MMR": {},  # Burma (Myanmar)
                          "MNG": {},  # Mongolia
                          "MYS": {1: 4101, 2: 4101,
                                  3: 4101, 4: 4101,
                                  5: 4101, 6: 4101},  # Malaysia
                          "PRK": {},  # Korea, Democratic People’s Republic
                          "SGP": {1: 4111, 2: 4111,
                                  3: 4111, 4: 4111,
                                  5: 4111, 6: 4111},  # Singapore
                          "THA": {1: 4001, 2: 4002,
                                  3: 4002, 4: 4002,
                                  5: 4002, 6: 4002},  # Thailand
                          "VNM": {1: 4141, 2: 4142,
                                  3: 4142, 4: 4142,
                                  5: 4142, 6: 4142},  # 越南
                          "TWN": {1: 4161, 2: 4162,  # 台湾
                                  3: 4163, 4: 4164,
                                  5: 4165},
                          "BHR": {1: 4601, 2: 4602,  # Bahrain巴林
                                  3: 4603, 4: 4604},
                          "OMN": {1: 4611, 2: 4612,  # Oman阿曼
                                  3: 4613, 4: 4614},
                          "JOR": {1: 4621, 2: 4622,  # Jordan约旦
                                  3: 4623, 4: 4624},
                          "KWT": {1: 4631, 2: 4632,  # Kuwait科威特
                                  3: 4633, 4: 4634},
                          "QAT": {1: 4641, 2: 4642,  # Qatar卡塔尔
                                  3: 4643, 4: 4644},
                          "LBN": {1: 4651, 2: 4652,  # Lebanon黎巴嫩
                                  3: 4653, 4: 4654},
                          "SAU": {1: 4661, 2: 4662,  # Saudi Arabia沙特阿拉伯
                                  3: 4663, 4: 4664},
                          # United Arab Emirates阿拉伯联合酋长国
                          "ARE": {1: 4671, 2: 4672,
                                  3: 4673, 4: 4674},
                          # ## 北美
                          "USA": {1: 101, 2: 102,
                                  3: 103, 4: 104},  # 美国
                          "CAN": {1: 801, 2: 802,
                                  3: 803},  # 加拿大
                          "MEX": {1: 901, 2: 902},  # 墨西哥
                          "PRI": {1: 921, 2: 922,
                                  3: 923, 4: 924},  # 波多黎各
                          "VIR": {4: 914},  # 美属维尔京群岛
                          # ## 南美
                          "BRA": {1: 3001, 2: 3002},  # 巴西
                          "ARG": {1: 3011, 2: 3012},  # Argentina
                          }
        if iso_country_code == "THA":  # 泰国
            if(route_num and
               (route_num.find('AH') == 0 or route_num.find('เอเอช') == 0)):
                # 亚洲道路
                return '4003'
        country_shield_dict = shield_id_dict.get(iso_country_code)
        if country_shield_dict:
            if iso_country_code in ['ARE','QAT','KWT','SAU','JOR']:
                shield_id = self.SpecialARA8AreaRouteNumberOperation(iso_country_code, 
                                                    language_code,
                                                    street_name,
                                                    route_type)
                if shield_id:
                    return str(shield_id)
                else:
                    self.log.error('Error iso_country_code = %d,trans_langs = %d, \
                        trans_names = %d,route_type = %d' 
                        % (iso_country_code, trans_langs, trans_names,route_type))
            else:
                shield_id = country_shield_dict.get(route_type)
                if shield_id:
                    return str(shield_id)
                else:
                    self.log.error('Error route_type = %d' % route_type)
        elif country_shield_dict is None:
            self.log.error('Unexpected iso_country_code = %s'
                           % iso_country_code)
        else:
            # 暂时番号没有确定的国家
            pass
        return INVALID_SHIELD_ID

    def get_tts_name(self,
                     iso_country_code,
                     lang_code,
                     name_type,
                     phonetic_strings,
                     phonetic_lang_codes):
        ''''返回音素的MultiLangNameRDF object。'''
        phonetic_list = self.get_phonetic_texts(
                                                iso_country_code,
                                                lang_code,
                                                phonetic_strings,
                                                phonetic_lang_codes
                                                )
        if phonetic_list:
            # [(phonetic_str, phonetic_lang_code)]
#             if len(phonetic_list) > 1:
#                 self.log.warning("phonetic_list>1, road_name_id=%d"
#                                   % road_name_id)
            phonetic_str = phonetic_list[0][0]
            phonetic_lang_code = phonetic_list[0][1]
            tts_obj = MultiLangNameRDF(phonetic_lang_code,
                                       phonetic_str,
                                       name_type,
                                       'phoneme')
            return tts_obj
        return None

    def get_phonetic_texts(self,
                           iso_country_code,
                           lang_code,
                           phonetic_strings,
                           phonetic_lang_codes):
        rst_phonetic_list = list()
        for tts_str, tts_code in zip(phonetic_strings, phonetic_lang_codes):
            if not tts_str or not tts_code:
                continue
            if self.check_vce_language_code_map(iso_country_code,
                                                lang_code,
                                                tts_code
                                                ):
                rst_phonetic_list.append((tts_str, tts_code))
        return rst_phonetic_list

    def check_vce_language_code_map(self,
                                    iso_country_code,
                                    language_code,
                                    phonetic_language_code):
        if not comp_dictionary_rdf.vce_language_code_map:
            if not self.set_language_code_map():
                return False

        if ((iso_country_code, language_code, phonetic_language_code) in
            comp_dictionary_rdf.vce_language_code_map):
            return True
        # 文本的languge_code和TTS的languge_code相同
        if language_code == phonetic_language_code:
            # print language_code, phonetic_language_code
            return True
        if language_code == 'ENG' and phonetic_language_code == 'UKE':
            return True
        #for sgp
        if language_code == 'CAT' and phonetic_language_code == 'SPA':
            return True
        
        return False

    def set_language_code_map(self):
        sqlcmd = """
        SELECT language_map_id, iso_country_code,
               language_code, phonetic_language_code,
               government_code
          FROM vce_language_code_map;
        """
        self.pg.connect1()
        self.pg.execute1(sqlcmd)
        rows = self.pg.fetchall()
        vce_map = dict()
        for row in rows:
            iso_country_code = row[1]
            language_code = row[2]
            phonetic_language_code = row[3]
            government_code = row[4]
            map_key = (iso_country_code, language_code, phonetic_language_code)
            if map_key == ('THA', 'THA', 'THE'):
                map_key = ('THA', 'THE', 'THE')
            if map_key in vce_map:
                vce_map[map_key].append(government_code)
            else:
                vce_map[map_key] = [government_code]
        comp_dictionary_rdf.vce_language_code_map = vce_map
        if vce_map:
            return True
        else:
            return False
        
    def FindCharacterInString(self, find_value, string):
        rtn_value = string.find(find_value)
        if rtn_value < 0:
            return False
        else:
            return True
        
    def SpecialARA8AreaRouteNumberOperation(self,iso_country_code, 
                                            language_code,
                                            street_name,
                                            routetype):
        #'ARE','QAT','KWT','SAU','JOR'
        if iso_country_code == 'ARE':
            if language_code == 'ENG':
                first_character = street_name[0]
                if first_character == 'M' and routetype == 1 :
                    return 4671
                elif first_character == 'E' and routetype == 1:
                    return 4672
                elif first_character == 'D' and routetype == 2:
                    return 4673
                elif first_character == 'S' and routetype == 2:
                    return 4674
                elif first_character == 'F' and routetype == 2:
                    return 4675
                elif first_character == 'A' and routetype == 2:
                    return 4676
                else:
                    return None
            elif language_code == 'ARA':
                if self.FindCharacterInString('م',street_name) and routetype == 1:
                    return 4671
                elif self.FindCharacterInString('إ',street_name) and routetype == 1:
                    return 4672
                elif self.FindCharacterInString('د',street_name) and routetype == 2:
                    return 4673
                elif self.FindCharacterInString('ش',street_name) and routetype == 2:
                    return 4674
                elif self.FindCharacterInString('ف',street_name) and routetype == 2:
                    return 4675
                elif self.FindCharacterInString('ع',street_name) and routetype == 2:
                    return 4676
                else:
                    return None
        elif iso_country_code == 'QAT':
            if language_code == 'ENG' :
                first_character = street_name[0]
                last_character = street_name[len(street_name)-1]
                if (first_character == 'M' or last_character == 'M') and routetype == 1:
                    return 4642
                elif (first_character == 'Q' or last_character == 'Q') and routetype == 1:
                    return 4641
                elif routetype == 2:
                    return 4643
                elif routetype == 3:
                    return 4644
                else:
                    return None  
            elif language_code == 'ARA':
                if self.FindCharacterInString('م',street_name) and routetype == 1:
                    return 4642
                elif self.FindCharacterInString('ق',street_name) and routetype == 1:
                    return 4641
                elif routetype == 2:
                    return 4643
                elif routetype == 3:
                    return 4644
                else:
                    return None 
        elif iso_country_code == 'KWT':
            pattern = re.compile('\d+')
            list = pattern.findall(street_name)
            list_section_cnt = len(list)
            
            if list_section_cnt > 1:
                return None
            
            character_cnt = len(list[0])
            if routetype == 1:
                return 4631
            elif routetype == 2:
                return 4632
            elif routetype == 3 and character_cnt in [1,3]:
                return 4633
            elif routetype == 3 and character_cnt == 2:
                return 4634
            else:
                return None
        elif iso_country_code == 'SAU':
            if routetype == 1:
                if language_code == 'ENG':
                    first_character = street_name[0]
                    if first_character == 'M':
                        return 4661
                    else:
                        return 4662
                elif language_code == 'ARA':
                    if self.FindCharacterInString('م',street_name) and routetype == 1: 
                        return 4661
                    else:
                        return 4662
            elif routetype == 2:
                return 4663
            elif routetype == 3:
                return 4664
            elif routetype == 4:
                return 4665
            else:
                return None
        elif iso_country_code == 'JOR': 
            if routetype == 1:
                if language_code == 'ENG':
                    first_character = street_name[0]
                    last_character = street_name[len(street_name)-1]
                    if (first_character == 'M' or last_character == 'M') and routetype == 1:
                        return 4622
                    elif routetype == 1:
                        return 4621
                    else:
                        return None
                elif language_code == 'ARA':
                    if self.FindCharacterInString('م',street_name) :
                        return 4622
                    else:
                        return 4621
                    
    
