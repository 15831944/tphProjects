# -*- coding: UTF8 -*-
'''
Created on 2012-3-29

@author: hongchenzai
'''
from component.component_base import comp_base
from component.rdf.multi_lang_name_rdf import MultiLangNameRDF
from common import cache_file


class HwyExitNameRDF(comp_base):
    '''Exit Name (Include junction name.)
    '''
    vce_language_code_map = None

    def __init__(self):
        '''
        Constructor
        '''
        comp_base.__init__(self, 'HwyExitName')

    def _DoCreateTable(self):
        # component.dictionary.comp_dictionary._DoCreateTable(self)
        self.CreateTable2('mid_temp_hwy_exit_name')
        return 0

    def _DoCreateFunction(self):
        return 0

    def _DoCreateIndex(self):
        self.CreateIndex2('mid_temp_hwy_exit_name_link_id_idx')

    def _Do(self):
        # 创建语言种别的中间表
        # self._InsertLanguages()
        # 设置语言code
        from component.default.dictionary import comp_dictionary
        dictionary = comp_dictionary()
        dictionary.set_language_code()
        # 道路名称
        self._make_hwy_exit_name()
        self._make_link_facil()
        return 0

    def _make_link_facil(self):
        return 0

    def _make_hwy_exit_name(self):
        temp_file_obj = cache_file.open('link_name')  # 创建临时文件
        prev_link_id = None
        curr_link_id = None
        prev_is_exit_name = None
        curr_is_exit_name = None
        prev_is_junction_name = None
        curr_is_junction_name = None
        multi_name_obj = None
        for nameinfo in self._get_exit_names():
            curr_link_id = nameinfo[0]
            ref_node_id = nameinfo[1]
            nonref_node_id = nameinfo[2]
            curr_is_exit_name = nameinfo[3]
            curr_is_junction_name = nameinfo[4]
            lang_code = nameinfo[5]  # 官方语言种别
            official_name = nameinfo[6]  # 官方语言种别道路名称
            trans_langs = nameinfo[7]  # 翻译语言种别
            trans_names = nameinfo[8]  # 翻译语言种别道路名称
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
                                                  ref_node_id,
                                                  nonref_node_id,
                                                  prev_is_exit_name,
                                                  prev_is_junction_name,
                                                  json_name)
                # ## 初始化
                multi_name_obj = MultiLangNameRDF(lang_code,
                                                  official_name)

                multi_name_obj = self.set_trans_name(multi_name_obj,
                                                     None,
                                                     trans_names,
                                                     trans_langs,
                                                     None,  # phonetic
                                                     None  # lang_codes
                                                     )
                prev_link_id = curr_link_id
                prev_is_exit_name = curr_is_exit_name
                prev_is_junction_name = curr_is_junction_name
            else:  # Link_id 没有变化
                if(prev_is_exit_name != curr_is_exit_name or
                   prev_is_junction_name != curr_is_junction_name):
                    json_name = multi_name_obj.json_format_dump()
                    if not json_name:
                        self.log.error('Json Name is none. linkid=%d' %
                                       prev_link_id)
                    # 存到本地的临时文件
                    self._store_name_to_temp_file(temp_file_obj,
                                                  prev_link_id,
                                                  ref_node_id,
                                                  nonref_node_id,
                                                  prev_is_exit_name,
                                                  prev_is_junction_name,
                                                  json_name)
                    multi_name_obj = MultiLangNameRDF(lang_code,
                                                      official_name)

                    multi_name_obj = self.set_trans_name(multi_name_obj,
                                                         None,
                                                         trans_names,
                                                         trans_langs,
                                                         None,  # phonetic
                                                         None  # lang_code
                                                         )
                    prev_link_id = curr_link_id
                    prev_is_exit_name = curr_is_exit_name
                    prev_is_junction_name = curr_is_junction_name
                else:
                    alter_name = MultiLangNameRDF(lang_code,
                                                  official_name)

                    alter_name = self.set_trans_name(alter_name,
                                                     None,
                                                     trans_names,
                                                     trans_langs,
                                                     None,
                                                     None
                                                     )
                    multi_name_obj.add_alter(alter_name)

        # ## 最后一条link的名称
        if multi_name_obj:
            json_name = multi_name_obj.json_format_dump()
            if not json_name:
                self.log.error('Json Name is none. linkid=%s' % prev_link_id)
            # 存到本地的临时文件
            self._store_name_to_temp_file(temp_file_obj,
                                          prev_link_id,
                                          ref_node_id,
                                          nonref_node_id,
                                          prev_is_exit_name,
                                          prev_is_junction_name,
                                          json_name)
        # ## 把名称导入数据库
        temp_file_obj.seek(0)
        self.pg.copy_from2(temp_file_obj, 'mid_temp_hwy_exit_name')
        self.pg.commit2()
        # close file
        # temp_file_obj.close()
        cache_file.close(temp_file_obj, True)
        self.CreateIndex2('temp_link_name_link_id_idx')
        self.log.info('End Make Link Name.')
        return 0

    def _get_exit_names(self):
        "道路名称(包括道路番号)字典"
        self.log.info('Start Make Exit Name.')
        sqlcmd = """
        SELECT
              m.link_id,
              link.ref_node_id,
              link.nonref_node_id,
              road_link.is_exit_name,
              road_link.is_junction_name,
              road_name.language_code,
              road_name.street_name,
              trans_group.trans_types,
              trans_group.trans_names
        FROM  rdf_road_name AS road_name
        LEFT JOIN rdf_road_link AS road_link
          ON road_name.road_name_id=road_link.road_name_id
        LEFT JOIN mid_temp_road_name_trans_group AS trans_group
          ON trans_group.road_name_id=road_name.road_name_id
        LEFT JOIN rdf_link AS link
          ON link.link_id=road_link.link_id
        LEFT JOIN rdf_nav_link as nav_link
          ON nav_link.link_id=link.link_id
        LEFT JOIN mid_link_mapping AS m
          ON m.org_link_id = nav_link.link_id
        WHERE road_link.is_exit_name='Y' or road_link.is_junction_name='Y'
        ORDER BY  link.link_id,
                  is_exit_name DESC,
                  is_junction_name DESC,
                  road_link_id;
        """
        return self.get_batch_data(sqlcmd)

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
                    if(len(phonetic_strings) > 1 and
                       len(set(phonetic_lang_codes)) > 1):  # 所有种别是相同
                        self.log.warning('No TTS, name=%s' % name)
                        pass
            multi_name.add_trans(trans_obj)
            name_cnt += 1
        return multi_name

    def _store_name_to_temp_file(self, file_obj, link_id,
                                 ref_node_id, nonref_node_id,
                                 is_exit_name, is_junction_name,
                                 json_name):
        if file_obj:
            file_obj.write('%d\t%d\t%d\t%s\t%s\t%s\n' %
                           (link_id, ref_node_id,
                            nonref_node_id, is_exit_name,
                            is_junction_name, json_name)
                           )
        return 0
