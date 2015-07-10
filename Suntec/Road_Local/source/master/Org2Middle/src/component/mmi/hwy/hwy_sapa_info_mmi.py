# -*- coding: UTF8 -*-
'''
Created on 2015��4��9��

@author: PC_ZH
'''
from component.component_base import comp_base
from component.default.multi_lang_name import MultiLangName
from common import cache_file
from component.default.multi_lang_name import NAME_TYPE_ALTER
from component.default.multi_lang_name import NAME_TYPE_OFFICIAL
from component.default.multi_lang_name import TTS_TYPE_PHONEME
from component.mmi.dictionary_mmi import MMI_OFFICIAL_LANG_CODE
from component.mmi.dictionary_mmi import REGIONAL_LANG_CODE


class HwySapaInfoMMI(comp_base):

    def __init__(self):
        ''' '''
        comp_base.__init__(self, 'HwySapaInfoMMI')

    def _DoCreateTable(self):
        self.CreateTable2('mid_temp_hwy_poi_regional_name_group')
        self.CreateTable2('mid_temp_sapa_store_info')
        self.CreateTable2('mid_temp_poi_link')
        self.CreateTable2('mid_temp_hwy_sapa_name')
        self.CreateTable2('hwy_chain_name')
        return 0

    def _DoCreateIndex(self):
        self.CreateIndex2('org_regional_data_id_idx')
        self.CreateIndex2('org_regional_data_table_idx')
        self.CreateIndex2('org_poi_point_uid_bigint_idx')
        self.CreateIndex2('mid_temp_poi_link_poi_id_idx')
        return 0

    def _Do(self):
        from component.default.dictionary import comp_dictionary
        dictionary = comp_dictionary()
        dictionary.set_language_code()

        self._group_poi_trans_name()
        # POI 关联 Link
        self._make_hwy_poi_link()
        # 道路名称
        self._make_hwy_sapa_name()
        self._make_hwy_sapa_store_info()
        return 0

    def _group_poi_trans_name(self):
        '''regional name 按 POI_ID合并 '''
        self.log.info('start make mid_temp_hwy_poi_regional_name_group')
        sqlcmd = '''
        INSERT INTO mid_temp_hwy_poi_regional_name_group(
                                      id, types, name_englishs,
                                      name_regionals, regional_lang_types)
        (
        SELECT id::BIGINT,
               array_agg("type") as types,
               array_agg(name_english) as name_englishs,
               array_agg(name_regional) as name_regionals,
               array_agg(regional_lang_type) as regional_lang_types
          from (
                SELECT id, "table", "type", name_english,
                       name_regional, regional_lang_type
                  FROM org_regional_data
                  where "table" = 'POI'
                  order by id, regional_lang_type,
                           "type" = 'MN' DESC, -- official name
                           gid
          ) as a
          group by id
        );
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.log.info('end make mid_temp_hwy_sapa_name_group')
        return 0

    def _make_hwy_poi_link(self):
        '''POI 关联 Link.'''
        self.log.info('start make mid_temp_poi_link')
        sqlcmd = '''
        INSERT INTO mid_temp_poi_link(poi_id, link_id)
        (
        SELECT distinct uid, m.link_id
          FROM org_poi_point
          LEFT JOIN mid_link_mapping AS m
          ON org_poi_point.edge_id = m.org_link_id
          ORDER BY uid
        );
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.log.info('end make mid_temp_poi_link')
        return 0

    def _make_hwy_sapa_store_info(self):
        self.log.info('start make hwy sapa store info ')

        self.log.info('end make hwy sapa store info')
        return 0

    def _make_hwy_sapa_name(self):
        self.log.info('start make mid_temp_hwy_sapa_name')
        sqlcmd = """
        SELECT uid,
               std_name,
               alt_name,
               group_reg.types,
               group_reg.name_englishs,
               name_regionals,
               regional_lang_types
        FROM org_poi_point
        LEFT JOIN mid_temp_hwy_poi_regional_name_group AS group_reg
        ON org_poi_point.uid = group_reg.id
        WHERE cat_code = 'TRNRST'  -- Rest Area
        ORDER BY uid
        """
        temp_file_obj = cache_file.open('link_name')  # 创建临时文件
        # self.CreateTable2('temp_link_name')
        names = self.get_batch_data(sqlcmd)
        for name_info in names:
            poi_id = name_info[0]
            official_name = name_info[1]
            alt_names = name_info[2]
            lang_code = MMI_OFFICIAL_LANG_CODE
            # ## regional
            regional_name_types = name_info[3]
            regional_name_engs = name_info[4]
            regional_names = name_info[5]
            regional_lang_types = name_info[6]
            if not poi_id:
                self.log.error('poi id is None.')
                continue
            # 有其他种别的小语种名称
            if regional_name_types:
                s2 = set(regional_name_types)
                if len(s2) > 1:
                    self.log.warning('Exist other regional name type. type=%s'
                                     % s2)

            reg_name_dict = self._get_regional_dict(regional_name_types,
                                                    regional_name_engs,
                                                    regional_names,
                                                    regional_lang_types
                                                    )
            # ## 官方名称
            ml_name = self._get_multi_name(lang_code,
                                           official_name,
                                           NAME_TYPE_OFFICIAL,
                                           ['MN'],
                                           None,
                                           reg_name_dict,
                                           )
            if official_name:
                if regional_names and not ml_name.has_trans():
                    self.log.warning("Can't find Regional Name. poi_id=%s"
                                     % poi_id)
                    key = ('MN', official_name.lower())
                    self._set_regional_name_fuzzy(ml_name,
                                                  reg_name_dict,
                                                  key
                                                  )

            # ## Alter Name
            if alt_names:
                # Multiple names with ";" separation
                for alt_name in alt_names.split(';'):
                    ml_alt = self._get_multi_name(lang_code,
                                                  alt_name,
                                                  NAME_TYPE_ALTER,
                                                  ['AN'],
                                                  None,
                                                  reg_name_dict,
                                                  )
                    if ml_name:
                        ml_name.add_alter(ml_alt)
                    else:
                        ml_name = ml_alt

            json_name = ml_name.json_format_dump()
            if not json_name:
                self.log.error('Json Name is none. poi_id=%d' % poi_id)
            # 存到本地的临时文件
            self._store_name_to_temp_file(temp_file_obj,
                                          poi_id,
                                          json_name
                                          )
        # ## 把名称导入数据库
        temp_file_obj.seek(0)
        self.pg.copy_from2(temp_file_obj, 'mid_temp_hwy_sapa_name')
        self.pg.commit2()
        # close file
        # temp_file_obj.close()
        cache_file.close(temp_file_obj, True)
        self.log.info('end make mid_temp_hwy_sapa_name')
        return 0

    def _get_regional_dict(self,
                           name_types,
                           name_engs,
                           regional_names,
                           lang_types
                           ):
        reg_name_dict = dict()
        if not (name_types or name_engs or regional_names or lang_types):
            return reg_name_dict
        for name_type, name_eng, reg_name, reg_lang_type in \
            zip(name_types, name_engs, regional_names, lang_types):
            lang_code = REGIONAL_LANG_CODE.get(reg_lang_type)
            key = (name_type, name_eng.lower())
            reg_name_info = (lang_code, reg_name)
            info_list = reg_name_dict.get(key)
            if info_list:
                if reg_name_info in info_list:
                    self.log.warning('Region Name repeat for name:%s'
                                     % name_eng)
                else:
                    info_list.append(reg_name_info)
            else:
                reg_name_dict[key] = [reg_name_info]
        return reg_name_dict

    def _get_multi_name(self,
                        lang_code,
                        str_name,
                        name_type,
                        org_name_types=None,
                        phoneme_dict=None,
                        reg_name_dict=None
                        ):
        ml_name = None
        if str_name:
            ml_name = MultiLangName(lang_code,
                                    str_name,
                                    name_type
                                    )
            if org_name_types:
                for org_type in org_name_types:
                    key = (org_type, str_name.lower())
                    self._set_phoneme(ml_name, phoneme_dict, key)
                    # regional name做成官方语的翻译
                    self._set_regional_name(ml_name, reg_name_dict, key)
        return ml_name

    def _set_phoneme(self, ml_name, phoneme_dict, key):
        # key = ('MN', official_name)
        if not phoneme_dict:
            return
        phoneme_text = phoneme_dict.get(key)
        if phoneme_text:
            tts = MultiLangName(ml_name.get_lang_code(),
                                phoneme_text,
                                ml_name.get_name_type(),
                                TTS_TYPE_PHONEME
                                )
            ml_name.set_tts(tts)

    def _set_regional_name(self, ml_name, reg_name_dict, key):
        if not reg_name_dict:
            return
        name_info_list = reg_name_dict.get(key)
        if name_info_list:
            for lang_code, reg_name in name_info_list:
                trans = MultiLangName(lang_code,
                                      reg_name,
                                      ml_name.get_name_type()
                                      )
                ml_name.add_trans(trans)

    def _set_regional_name_fuzzy(self, ml_name, reg_name_dict, key):
        '''模糊搜索'''
        for reg_key, name_info_list in reg_name_dict.iteritems():
            if(reg_key[0] == key[0]   # 名称种别相同
               and key[1].find(reg_key[1]) == 0):
                if name_info_list:
                    for lang_code, reg_name in name_info_list:
                        trans = MultiLangName(lang_code,
                                              reg_name,
                                              ml_name.get_name_type()
                                              )
                        ml_name.add_trans(trans)

    def _store_name_to_temp_file(self, file_obj, nid, json_name):
        if file_obj:
            file_obj.write('%d\t%s\n' % (nid, json_name))
        return 0
