# -*- coding: UTF8 -*-
'''
Created on 2014-3-5

@author: hongchenzai
'''
import component.default.dictionary
from common import cache_file
from component.default.multi_lang_name import NAME_TYPE_ALTER
from component.default.multi_lang_name import NAME_TYPE_OFFICIAL
from component.default.multi_lang_name import TTS_TYPE_PHONEME
from component.default.multi_lang_name import NAME_TYPE_ROUTE_NUM
from component.default.multi_lang_name import MultiLangName
from component.default.multi_lang_name import MultiLangShield
from component.ta.dictionary_ta import ShieldTA
MMI_OFFICIAL_LANG_CODE = 'ENG'
REGIONAL_LANG_CODE = {'Hindi': 'HIN',
                      'Malayalam': 'MAL',
                      'Punjabi': 'PAN',
                      'Tamil': 'TAM'
                      }


#==============================================================================
# comp_dictionary_ta
#==============================================================================
class comp_dictionary_mmi(component.default.dictionary.comp_dictionary):
    '''字典(TomTom)
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.default.dictionary.comp_dictionary.__init__(self)

    def _DoCreateTable(self):
        return 0

    def _DoCreateFunction(self):
        return 0

    def _Do(self):
        # 创建语言种别的中间表
        self._InsertLanguages()
        # 设置语言code
        self.set_language_code()

        self._make_link_shield()
        self._group_link_name_phoneme()
        self._group_link_regional_name()
        self._make_link_name()
        
        self._group_admin_name_phoneme()
        self._group_admin_regional_name()
        self._make_admin_name()
        return 0

    def _group_link_name_phoneme(self):
        '''道路名称音素按id合并。'''
        self.CreateTable2('mid_temp_link_name_phoneme')
        sqlcmd = """
        INSERT INTO mid_temp_link_name_phoneme(
                 id, types, name_englishs, name_nuances)
        (
        SELECT id::BIGINT,
               array_agg("type") as types,
               array_agg(name_english) as name_englishs,
               array_agg(name_nuance) as name_nuances
          FROM (
                SELECT id, "table", "type",
                       name_english, name_nuance
                  FROM org_phoneme
                  where "table" = 'City_nw_gc'
                  order by id,
                           "type" = 'MN' DESC, -- official name
                           gid
          ) AS a
          group by id
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
    
    def _group_admin_name_phoneme(self):
        '''道路名称音素按id合并。'''
        self.CreateTable2('mid_temp_admin_name_phoneme')
        sqlcmd = """
        INSERT INTO mid_temp_admin_name_phoneme(
                 id, types, name_englishs, name_nuances)
        (
        SELECT id::BIGINT,
               array_agg("type") as types,
               array_agg(name_english) as name_englishs,
               array_agg(name_nuance) as name_nuances
          FROM (
                SELECT id, "table", "type",
                       name_english, name_nuance
                  FROM org_phoneme
                  where "table" = 'Area'
                  order by id,
                           "type" = 'MN' DESC, -- official name
                           gid
          ) AS a
          group by id
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _group_link_regional_name(self):
        '''道路regional名称按id合并。'''
        self.CreateTable2('mid_temp_link_regional_name')
        sqlcmd = """
        INSERT INTO mid_temp_link_regional_name(
            id, types, name_englishs, name_regionals, regional_lang_types)
        (
        SELECT id::BIGINT,
               array_agg("type") as types,
               array_agg(name_english) as name_englishs,
               array_agg(name_regional) as name_regionals,
               array_agg(regional_lang_type) as regional_lang_types
          from (
                SELECT id, "table", "type", name_english,
                       name_regional, regional_lang_type, admin_name
                  FROM org_regional_data
                  where "table" = 'City_nw_gc'
                  order by id,
                           regional_lang_type,
                           "type" = 'MN' DESC, -- official name
                           gid
          ) as a
          group by id
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
    def _group_admin_regional_name(self):
        '''道路regional名称按id合并。'''
        self.CreateTable2('mid_temp_admin_regional_name')
        sqlcmd = """
        INSERT INTO mid_temp_admin_regional_name(
            id, types, name_englishs, name_regionals, regional_lang_types)
        (
        SELECT id::BIGINT,
               array_agg("type") as types,
               array_agg(name_english) as name_englishs,
               array_agg(name_regional) as name_regionals,
               array_agg(regional_lang_type) as regional_lang_types
          from (
                SELECT id, "table", "type", name_english,
                       name_regional, regional_lang_type, admin_name
                  FROM org_regional_data
                  where "table" = 'Area'
                  order by id,
                           regional_lang_type,
                           "type" = 'MN' DESC, -- official name
                           gid
          ) as a
          group by id
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _make_link_name(self):
        '''道路名称(包括显示用番号)'''
        self.log.info('Make Link Name.')
        sqlcmd = """
        SELECT link.id::BIGINT, "name", pop_name,
               alt_name, namelc, routenum,
               phoneme.types as phoneme_types,
               phoneme.name_englishs as phoneme_name_engs,
               phoneme.name_nuances as phoneme_nuances,
               regional.types as regional_name_types,
               regional.name_englishs as regional_name_engs,
               name_regionals as regional_names,
               regional_lang_types
          FROM (
                SELECT id, "name", base_nme, pop_name,
                       alt_name, namelc, routenum
                  FROM org_city_nw_gc_polyline
                  WHERE "name" is not null
                        or pop_name is not null
                        or alt_name is not null
                        or routenum is not null
          ) AS link
          FULL JOIN mid_temp_link_name_phoneme AS phoneme
          ON link.id = phoneme.id
          FULL JOIN mid_temp_link_regional_name as regional
          on link.id = regional.id
          ;
        """
        temp_file_obj = cache_file.open('link_name')  # 创建临时文件
        self.CreateTable2('temp_link_name')
        names = self.get_batch_data(sqlcmd)
        for name_info in names:
            link_id = name_info[0]
            official_name = name_info[1]
            pop_name = name_info[2]
            alt_names = name_info[3]
            lang_code = name_info[4]
            routenum = name_info[5]
            # ## 音素
            phoneme_name_types = name_info[6]  # MN:official, AN: Alter
            phoneme_name_engs = name_info[7]
            phoneme_nuances = name_info[8]
            # ## regional
            regional_name_types = name_info[9]
            regional_name_engs = name_info[10]
            regional_names = name_info[11]
            regional_lang_types = name_info[12]
            if not link_id:
                self.log.error('Link id is None.')
                continue
            # 有其他种别的Phoneme
            if phoneme_name_types:
                s1 = set(phoneme_name_types)
                if len(s1) > 1:
                    self.log.warning('Exist other name type. type=%s' % s1)
            # 有其他种别的小语种名称
            if regional_name_types:
                s2 = set(regional_name_types)
                if len(s2) > 1:
                    self.log.warning('Exist other regional name type. type=%s'
                                     % s2)
            phoneme_dict = self._get_phoneme_dict(phoneme_name_types,
                                                  phoneme_name_engs,
                                                  phoneme_nuances
                                                  )
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
                                           phoneme_dict,
                                           reg_name_dict,
                                           )
            if official_name:
                # 没有做成到TTS
                if phoneme_nuances and not ml_name.has_tts():
                    self.log.warning("Can't find phoneme text. LinkId=%s"
                                     % link_id)
                if regional_names and not ml_name.has_trans():
                    self.log.warning("Can't find Regional Name. LinkId=%s"
                                     % link_id)
                    key = ('MN', official_name.lower())
                    self._set_regional_name_fuzzy(ml_name,
                                                  reg_name_dict,
                                                  key
                                                  )

            # ## POP Name
            ml_pop = self._get_multi_name(lang_code,
                                          pop_name,
                                          NAME_TYPE_ALTER,
                                          ['MN', 'AN'],
                                          phoneme_dict,
                                          reg_name_dict,
                                          )
            if ml_name:
                ml_name.add_alter(ml_pop)
            else:
                ml_name = ml_pop
            # ## Alter Name
            if alt_names:
                # Multiple names with ";" separation
                for alt_name in alt_names.split(';'):
                    ml_alt = self._get_multi_name(lang_code,
                                                  alt_name,
                                                  NAME_TYPE_ALTER,
                                                  ['AN'],
                                                  phoneme_dict,
                                                  reg_name_dict,
                                                  )
                    if ml_name:
                        ml_name.add_alter(ml_alt)
                    else:
                        ml_name = ml_alt
            if routenum:
                shield = ShieldMmi()
                if shield.is_name_include_number([official_name], routenum):
                    pass
                else:
                    ml_num = MultiLangName(lang_code,
                                           routenum,
                                           NAME_TYPE_ROUTE_NUM
                                           )
                    if ml_name:
                        ml_name.add_alter(ml_num)
                    else:
                        ml_name = ml_num
            json_name = ml_name.json_format_dump()
            if not json_name:
                self.log.error('Json Name is none. linkid=%d' % link_id)
            # 存到本地的临时文件
            self._store_name_to_temp_file(temp_file_obj,
                                          link_id,
                                          json_name
                                          )
        # ## 把名称导入数据库
        temp_file_obj.seek(0)
        self.pg.copy_from2(temp_file_obj, 'temp_link_name')
        self.pg.commit2()
        # close file
        #temp_file_obj.close()
        cache_file.close(temp_file_obj,True)
        
        self.log.info('End Make Link Name.')
        return 0

    def _make_link_shield(self):
        # 盾牌及番号
        self.log.info('Make Link Shield.')
        temp_shield_file = cache_file.open('link_shield')  # 创建临时文件
        self.CreateTable2('temp_link_shield')
        sqlcmd = """
        SELECT id::BIGINT, namelc, routenum
          FROM org_city_nw_gc_polyline
          WHERE routenum is not null;
        """
        numbers = self.get_batch_data(sqlcmd)
        shield_obj = ShieldMmi()
        for number_info in numbers:
            link_id = number_info[0]
            lang_code = number_info[1]
            routenum = number_info[2]  # 道路番号
            shieldid = shield_obj.convert_shield_id(routenum)
            if not shieldid:
                self.log.error('ShieldID is none. linkid=%d' % link_id)
                continue
            ml_shield = MultiLangShield(shieldid, routenum, lang_code)
            if ml_shield:
                # ## shield number
                json_name = ml_shield.json_format_dump()
                if not json_name:
                    self.log.error('Json Shield is none. linkid=%d' % link_id)
                # 存到本地的临时文件
                self._store_name_to_temp_file(temp_shield_file,
                                              link_id,
                                              json_name
                                              )
        # ## 把shield导入数据库
        temp_shield_file.seek(0)
        self.pg.copy_from2(temp_shield_file, 'temp_link_shield')
        self.pg.commit2()
        # close file
        #temp_shield_file.close()
        cache_file.close(temp_shield_file,True)
        self.log.info('End Make Link Shield Info.')
        return 0
    
    def _make_admin_name(self):          
        self.log.info('begin make admin_name ..')
        sqlcmd = '''
                 select temp_all.id, name, name_alt, country_name,
                        phoneme.types as phoneme_types,
                        phoneme.name_englishs as phoneme_name_engs,
                        phoneme.name_nuances as phoneme_nuances,
                        regional.types as regional_name_types,
                        regional.name_englishs as regional_name_engs,
                        name_regionals as regional_names,
                        regional_lang_types
                 from
                 (   
                     select id,"name" as name,"names" as name_alt,"name" as country_name
                     from 
                     (
                         select id,"name","names"
                         from org_area
                         where kind = '10'
                     )as temp
                     
                     union
                     
                     select a.id,a.stt_nme as name,a.stt_alt as name_alt,b."name" as country_name
                     from 
                     (
                         select stt_id as id,stt_nme,stt_alt,parent_id
                         from org_state_region
                         where kind = '9'
                         group by stt_id,stt_nme,stt_alt,parent_id
                         order by stt_id,stt_nme,stt_alt,parent_id
                     )as a
                     left join
                     (
                         select id,"name"
                         from org_area
                         where kind = '10'
                         group by id,"name"
                         order by id,"name"
                     )as b
                     on a.parent_id = b.id
                     
                     union
                     
                     select dst_id as id,dst_nme as name,dst_alt as name_alt,d."name" as country_name
                     from org_district_region as c
                     left join
                     (
                         select state.id,country."name"
                         from 
                         (
                             select stt_id as id,parent_id
                             from org_state_region
                             where kind = '9'
                             group by stt_id,parent_id
                             order by stt_id,parent_id
                         )as state
                         left join
                         (
                             select id,"name"
                             from org_area
                             where kind = '10'
                             group by id,"name"
                             order by id,"name"
                         )as country
                         on state.parent_id = country.id
                     )as d
                     on c.parent_id = d.id              
                     group by dst_id,dst_nme,dst_alt,d."name"
                 )as temp_all
                 left join mid_temp_admin_name_phoneme as phoneme
                 on temp_all.id = phoneme.id
                 left join mid_temp_admin_regional_name as regional
                 on temp_all.id = regional.id                 
                 where (name is not null) or (name_alt is not null)    
                 order by id,name,name_alt,country_name;
                 '''
        self.CreateTable2('temp_admin_name')
        temp_file_obj = cache_file.open('admin_name')
        rows = self.get_batch_data(sqlcmd) 
        for row in rows:
            id = int(row[0])
            official_name = row[1]
            alt_names = row[2]
            country_name = row[3]
            # ## 音素
            phoneme_name_types = row[4]  # MN:official, AN: Alter
            phoneme_name_engs = row[5]
            phoneme_nuances = row[6]
            # ## regional
            regional_name_types = row[7]
            regional_name_engs = row[8]
            regional_names = row[9]
            regional_lang_types = row[10]
            
            if not id:
                self.log.error('admin id is None.')
                continue
            
            phoneme_dict = self._get_phoneme_dict(phoneme_name_types,
                                                  phoneme_name_engs,
                                                  phoneme_nuances
                                                  )
            reg_name_dict = self._get_regional_dict(regional_name_types,
                                                    regional_name_engs,
                                                    regional_names,
                                                    regional_lang_types
                                                    )
            # ## 官方名称
            ml_name = self._get_multi_name(MMI_OFFICIAL_LANG_CODE,
                                           official_name,
                                           NAME_TYPE_OFFICIAL,
                                           ['MN'],
                                           phoneme_dict,
                                           reg_name_dict,
                                           )
            if official_name:
                # 没有做成到TTS
                if phoneme_nuances and not ml_name.has_tts():
                    self.log.warning("Can't find phoneme text. LinkId=%s"
                                     % link_id)
                if regional_names and not ml_name.has_trans():
                    self.log.warning("Can't find Regional Name. LinkId=%s"
                                     % link_id)
                    key = ('MN', official_name.lower())
                    self._set_regional_name_fuzzy(ml_name,
                                                  reg_name_dict,
                                                  key
                                                  )
            # ## Alter Name
            if alt_names:
                # Multiple names with ";" separation
                for alt_name in alt_names.split(';'):
                    ml_alt = self._get_multi_name(MMI_OFFICIAL_LANG_CODE,
                                                  alt_name,
                                                  NAME_TYPE_ALTER,
                                                  ['AN'],
                                                  phoneme_dict,
                                                  reg_name_dict,
                                                  )
                    if ml_name:
                        ml_name.add_alter(ml_alt)
                    else:
                        ml_name = ml_alt
                        
            json_name = ml_name.json_format_dump()
            if not json_name:
                self.log.error('Json Name is none. admin id=%d' % link_id)
            # 存到本地的临时文件
            self._store_name_to_temp_file(temp_file_obj,
                                          id,
                                          json_name
                                          )
        # ## 把名称导入数据库
        temp_file_obj.seek(0)
        self.pg.copy_from2(temp_file_obj, 'temp_admin_name')
        self.pg.commit2()
        cache_file.close(temp_file_obj,True)
        
        self.log.info('end admin_name ..')

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
            if (reg_key[0] == key[0]   # 名称种别相同
                and key[1].find(reg_key[1]) == 0):
                if name_info_list:
                    for lang_code, reg_name in name_info_list:
                        trans = MultiLangName(lang_code,
                                              reg_name,
                                              ml_name.get_name_type()
                                              )
                        ml_name.add_trans(trans)

    def _get_phoneme_dict(self, name_types, name_engs, nuances):
        phoneme_dict = dict()
        if not (name_types or name_engs or nuances):
            return phoneme_dict
        for name_type, name_eng, nuance in \
            zip(name_types, name_engs, nuances):
            key = (name_type, name_eng.lower())
            if (key in phoneme_dict
                and phoneme_dict.get(key) != nuance):
                self.log.waring('Nuance repeat for name:%s' % name_eng)
            else:
                phoneme_dict[key] = nuance
        return phoneme_dict

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
                    self.log.waring('Region Name repeat for name:%s'
                                    % name_eng)
                else:
                    info_list.append(reg_name_info)
            else:
                reg_name_dict[key] = [reg_name_info]
        return reg_name_dict

    def _store_name_to_temp_file(self, file_obj, nid, json_name):
        if file_obj:
            file_obj.write('%d\t%s\n' % (nid, json_name))
        return 0


#==============================================================================
# ShieldMmi
#==============================================================================
class ShieldMmi(ShieldTA):
    def __init__(self):
        '''
        Constructor
        '''
        ShieldTA.__init__(self)

    def convert_shield_id(self, routenum):
        shieldid_dict = {'NE': '4171',  # National Expressway
                         'AH': '4172',  # Asian Highway
                         'NH': '4173',  # National Highway
                         'SH': '4174'  # State highway
                         }
        if routenum:  # 'NE 1', 'NH 7', 'SH 13'
            shieldid = routenum.split(' ')[0]
            return shieldid_dict.get(shieldid)
        return None
