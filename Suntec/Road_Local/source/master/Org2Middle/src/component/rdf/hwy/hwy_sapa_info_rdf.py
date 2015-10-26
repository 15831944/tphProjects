# -*- coding: UTF8 -*-
'''
Created on 2015��3��9��

@author: PC_ZH
'''

from component.component_base import comp_base
from component.rdf.multi_lang_name_rdf import MultiLangNameRDF
from common import cache_file


class HwySapaInfoRDF(comp_base):
    '''SAPA Name
    '''

    def __init__(self, ItemName='HwySapaInfoRDF'):
        '''
        Constructor
        '''
        comp_base.__init__(self, ItemName)

    def _DoCreateTable(self):
        self.CreateTable2('mid_temp_hwy_sapa_name')
        self.CreateTable2('mid_temp_poi_link')
        self.CreateTable2('mid_temp_sapa_store_info')
        self.CreateTable2('hwy_chain_name')
        return 0

    def _DoCreateFunction(self):
        return 0

    def _DoCreateIndex(self):
        self.CreateIndex2('mid_temp_sapa_store_info_poi_id_idx')
        self.CreateIndex2('mid_temp_poi_link_poi_id_idx')
        self.CreateIndex2('mid_temp_hwy_sapa_name_poi_id_idx')
        return 0

    def _Do(self):
        from component.default.dictionary import comp_dictionary
        dictionary = comp_dictionary()
        dictionary.set_language_code()
        #
        self._group_poi_trans_name()
        # POI关联link
        self._make_hwy_poi_link()
        # 道路名称
        self._make_hwy_sapa_name()
        self._make_hwy_sapa_store_info()
        self._make_hwy_store_name()
        return 0

    def _group_poi_trans_name(self):
        '''对同个名称(包括番号)的多个翻译进行group by. 名称之间用"|"分开。'''
        self.CreateTable2('mid_temp_hwy_sapa_name_group')
        sqlcmd = """
        INSERT INTO mid_temp_hwy_sapa_name_group(
                        name_id, trans_types, trans_names)
        (
        SELECT name_id,
               array_to_string(array_agg(trans_type), '|') AS trans_types,
               array_to_string(array_agg(trans_name), '|') AS trans_names
          FROM (
                SELECT name_id,
                       transliteration_type AS trans_type,
                       name as trans_name
                  FROM rdf_poi_name_trans
                  order by name_id, transliteration_type
          ) AS A
          GROUP BY name_id
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return

    def _make_hwy_sapa_name(self):
        temp_file_obj = cache_file.open('sapa_name')  # 创建临时文件
        curr_poi_id = None
        prev_poi_id = None
        multi_name_obj = None
        for nameinfo in self._get_sapa_names():
            curr_poi_id = nameinfo[0]
            lang_code = nameinfo[2]
            official_name = nameinfo[3]
            trans_langs = nameinfo[4]
            trans_names = nameinfo[5]

            if not curr_poi_id:  # POI id不存在
                self.log.error("No poi ID.")
                continue
            if prev_poi_id != curr_poi_id:  # POI id 变更
                if prev_poi_id:
                    # ## 保存上一POI的名称
                    json_name = multi_name_obj.json_format_dump()
                    if not json_name:
                        self.log.error('Json Name is none. poi_id=%d' %
                                       prev_poi_id)
                    # 存到本地的临时文件
                    self._store_name_to_temp_file(temp_file_obj,
                                                  prev_poi_id,
                                                  json_name)
                # ## 初始化
                multi_name_obj = MultiLangNameRDF(lang_code,
                                                  official_name)

                multi_name_obj = self.set_trans_name(multi_name_obj,
                                                     None,
                                                     trans_names,
                                                     trans_langs,
                                                     None,  # phonetic_strings
                                                     None
                                                     )
                prev_poi_id = curr_poi_id
            else:  # POI_id 没有变化
                alter_name = MultiLangNameRDF(lang_code,
                                              official_name
                                              )

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
                self.log.error('Json Name is none. pio_id=%s' % prev_poi_id)
            # 存到本地的临时文件
            self._store_name_to_temp_file(temp_file_obj,
                                          prev_poi_id,
                                          json_name)
        # ## 把名称导入数据库
        temp_file_obj.seek(0)
        self.pg.copy_from2(temp_file_obj, 'mid_temp_hwy_sapa_name')
        self.pg.commit2()
        # close file
        # temp_file_obj.close()
        cache_file.close(temp_file_obj, True)
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

    def _get_sapa_names(self):
        "道路名称(包括道路番号)字典"
        self.log.info('Start Make sapa Name.')
        sqlcmd = """
            SELECT
                 names.poi_id,
                 name.name_id,
                 name.language_code,
                 name.name,
                 grp.trans_types,
                 grp.trans_names
            FROM rdf_poi_names     AS names
            LEFT JOIN rdf_poi
              on names.poi_id=rdf_poi.poi_id
            LEFT JOIN rdf_poi_name AS name
              ON names.name_id=name.name_id
            LEFT JOIN mid_temp_hwy_sapa_name_group AS grp
              ON name.name_id=grp.name_id
            WHERE rdf_poi.cat_id=7897
            ORDER BY
                    poi_id,
                    name_id
        """
        return self.get_batch_data(sqlcmd)

    def _store_name_to_temp_file(self, file_obj, poi_id, json_name):
        if file_obj:
            file_obj.write('%d\t%s\n' % (poi_id, json_name))
        return 0

    def _make_hwy_poi_link(self):
        '''
        '''
        self.log.info('Start Make SAPA link.')
        sqlcmd = '''
        INSERT INTO mid_temp_poi_link(poi_id, link_id)
        (
        SELECT distinct rest.poi_id, m.link_id
          FROM rdf_poi_rest_area AS rest
          LEFT JOIN rdf_poi_address as poi_address
          ON  poi_address.poi_id = rest.poi_id
          LEFT JOIN rdf_location AS location
          ON location.location_id = poi_address.location_id
          LEFT JOIN mid_link_mapping as m
          ON m.org_link_id = location.link_id
          ORDER BY rest.poi_id
        );
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return

    def _make_hwy_sapa_store_info(self):
        ''''''
        self.log.info('Start Make sapa store info.')
        sqlcmd = '''
        INSERT INTO mid_temp_sapa_store_info(poi_id, child_poi_id,
                                             chain_id, cat_id, subcat)
        (
          SELECT distinct rest.poi_id, child_poi_id,
                 chain_id, cat_id, subcategory as subcat
           FROM rdf_poi_rest_area AS rest
           INNER JOIN rdf_poi_children AS child
            ON rest.poi_id = child.poi_id
           LEFT JOIN rdf_poi_chains AS chains
            ON child.child_poi_id = chains.poi_id
           LEFT JOIN rdf_poi
            ON child.child_poi_id = rdf_poi.poi_id
           LEFT JOIN rdf_poi_subcategory as sub
            ON child.child_poi_id = sub.poi_id
           ORDER BY rest.poi_id, child_poi_id, chain_id, cat_id
        );
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return

    def _make_hwy_store_name(self):
        '''Store or Chain Name'''
        sqlcmd = """
        INSERT INTO hwy_chain_name(u_code, cat_id, sub_cat,
                                   chain_id, chain_name, language_code)
        (
        SELECT per_code, cat_id, subcat,
               chain_id, chain_name, language_code
        FROM (
            SELECT distinct per_code, a.cat_id,
                   (case when subcat is null then ''
                else subcat::character varying end) as subcat,
                   b.chain_id, b.name as chain_name, language_code
              FROM mid_temp_sapa_store_info as a
              LEFT JOIN rdf_chain_name as b
              ON a.chain_id::bigint = b.chain_id
              INNER JOIN temp_poi_category as cat
              ON b.chain_id = cat.chain and
                 a.cat_id::bigint = cat.org_code
        ) AS c
        ORDER BY chain_id,
                 language_code = 'ENG' DESC, -- English first
                 language_code = 'CHI' DESC, -- Chinese second
                 language_code = 'CHT' DESC,
                 language_code
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
