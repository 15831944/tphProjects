# -*- coding: UTF8 -*-
'''
Created on 2014-2-13

@author: hongchenzai
'''
import re
from component.default.multi_lang_name import *
import common


#==============================================================================
# MultLangNameTa 名称类
#==============================================================================
class MultiLangNameTa(MultiLangName):
    '''处理RDF的多语言
    '''

    def __init__(self,
                 n_id=None,
                 lang_code=None,
                 str_name=None,
                 name_type=None,
                 tts_type='not_tts',
                 left_right='left_right'
                 ):
        '''
        Constructor
        '''
        MultiLangName.__init__(self, lang_code, str_name,
                               name_type, tts_type, left_right)
        self.n_id = n_id
        self.l_lang_code = None
        self.r_lang_code = None

    def set_lang_code(self,
                      left_lang_code=None,
                      right_lang_code=None
                      ):
        self.l_lang_code = left_lang_code
        self.r_lang_code = right_lang_code
        if self.l_lang_code and self.r_lang_code:
            return
        if not self.l_lang_code and self.r_lang_code:
            self.l_lang_code = self.r_lang_code
        elif self.l_lang_code and not self.r_lang_code:
            self.r_lang_code = self.l_lang_code
        else:
            self.l_lang_code = MultiLangNameTa.get_lang_code_by_id(self.n_id)
            self.r_lang_code = self.l_lang_code
            return

    @staticmethod
    def get_lang_code_by_id(n_id):
        d = {704: 'VIE',
             36: 'ENG',
             554: 'ENG',
             508: 'POR',
             894: 'ENG',
             426: 'ENG',
             516: 'ENG',
             72: 'ENG',
             710: 'ENG',
             716: 'ENG',
             748: 'ENG'
             }
        if n_id:
            return d.get((int(n_id) / (10 ** 10))%1000)

    def set_multi_name(self,
                       names,
                       lang_codes,
                       nametypes,
                       sols=None,
                       phoneme_infos=None
                       ):
        '设置名称'
        if not names:  # 无名称
            return
        if not sols:  # 没有指定左右侧
            sols = [None] * len(names)
        if not phoneme_infos:
            phoneme_infos = [None] * len(names)
        for name, temp_lang_code, name_type, sol, phoneme_info in \
                zip(names, lang_codes, nametypes, sols, phoneme_infos):
            if not name:
                print 'Name is None. id=%d' % self.n_id
                continue
            if not temp_lang_code:
                print 'lang_code is None. id=%d' % self.n_id
                continue
            if not name_type:
                print 'name_type is None. id=%d' % self.n_id
                continue
            left_right = self._cvt_left_right(sol)
            new_name_type = self._cvt_name_type(name_type)
            # 取得左右两边的语种
            lang_infos = self.get_side_lang_infos(temp_lang_code, sol)
            # TTS
            phoneme_list, language_list = \
                        self.split_phoneme_info(phoneme_info)
            for (left_right, lang_code) in lang_infos:
                if not self._str_name:  # 第一个名称
                    self._lang_code = lang_code
                    self._str_name = name
                    self._name_type = new_name_type
                    self._tts_type = TTS_TYPE_NOT_TTS
                    self._left_right = left_right
                    self.add_all_tts(phoneme_list, language_list, lang_code)
                else:  # 别名
                    temp_name = MultiLangNameTa(self.n_id,
                                                lang_code,
                                                name,
                                                new_name_type,
                                                TTS_TYPE_NOT_TTS,
                                                left_right
                                                )
                    temp_name.add_all_tts(phoneme_list,
                                          language_list,
                                          lang_code
                                          )
                    self.add_alter(temp_name)

    def set_multi_routenum(self, names, routenums, phoneme_infos=None):
        '''设置番号'''
        if not routenums:  # 无番号
            return
        ROUTE_NAME_TYPE = 4  # 番号种别
        name_type = self._cvt_name_type(ROUTE_NAME_TYPE)
        if not phoneme_infos:
            phoneme_infos = [None] * len(routenums)
        from component.ta.dictionary_ta import ShieldTA
        shield_obj = ShieldTA()
        for route_num, phoneme_info in zip(routenums, phoneme_infos):
            # 判断重复, 即名称中已经包含了番号
            if shield_obj.is_name_include_number(names, route_num):
                continue
            lang_infos = self.get_side_lang_infos()  # 原数据番号的语种都是Undefined
            # TTS
            phoneme_list, language_list = self.split_phoneme_info(phoneme_info)
            for (left_right, lang_code) in lang_infos:
                if not self._str_name:  # 没有名称，只有番号
                    self._lang_code = lang_code
                    self._str_name = route_num
                    self._name_type = name_type
                    self._tts_type = TTS_TYPE_NOT_TTS
                    self._left_right = left_right
                    self.add_all_tts(phoneme_list, language_list, lang_code)
                else:  # 别名
                    temp_name = MultiLangNameTa(self.n_id,
                                                lang_code,
                                                route_num,
                                                name_type,
                                                TTS_TYPE_NOT_TTS,
                                                left_right
                                                )
                    temp_name.add_all_tts(phoneme_list,
                                          language_list,
                                          lang_code
                                          )
                    self.add_alter(temp_name)

    def _cvt_left_right(self, sol=None):
        '''变换左右种别。'''
        side_type = {0: LEFT_RIGHT_B,  # 左右共用
                     1: LEFT_RIGHT_L,
                     2: LEFT_RIGHT_R
                     }
        if sol is None:  # 没有指定，默认返回左右共用
            return side_type.get(0)
        return side_type.get(sol)

    def get_side_lang_infos(self, lang_code=None, sol=None):
        '''返回左右种别及语种'''
        if not lang_code or lang_code == 'UND':  # 名称未定义
            if not sol or sol == 0:  # 左右共用
                if self.l_lang_code == self.r_lang_code:  # 左右名称相同
                    return [(self._cvt_left_right(0), self.l_lang_code)]
                else:
                    return [(self._cvt_left_right(1), self.l_lang_code),
                            (self._cvt_left_right(2), self.r_lang_code)]
            if sol == 1:  # left
                return [(self._cvt_left_right(sol), self.l_lang_code)]
            if sol == 2:  # right
                return [(self._cvt_left_right(sol), self.r_lang_code)]
        else:
            return [(self._cvt_left_right(sol), lang_code)]

    def _cvt_name_type(self, nametype):
        '''变换名称种别。'''
        OFFICE_NAME_TYPE = 1
        BRUNNEL_NAME_TYPE = 8
        ROUTE_NAME_TYPE = 4
        if nametype & OFFICE_NAME_TYPE > 0:  # official name
            return NAME_TYPE_OFFICIAL
        if nametype & BRUNNEL_NAME_TYPE > 0:  # Bridge/Tunnel
            return NAME_TYPE_BRIDGE
        if nametype & ROUTE_NAME_TYPE > 0:  # Bridge/Tunnel
            return NAME_TYPE_ROUTE_NUM
        # 16 = Street Name
        # 32 = Locality Name
        # 64 = Postal Street Name
        return NAME_TYPE_ALTER

    def add_all_tts(self, phoneme_list, language_list, match_code=None):
        str_country = common.common_func.getProjCountry()
        if str_country.lower() == 'aus':
            match_code = 'ENA'
                    
        for phoneme, lang_code in zip(phoneme_list, language_list):
            if phoneme and lang_code:
                # match_code为空，就不比较语种
                if not match_code or lang_code == match_code:
                    tts_obj = MultiLangNameTa(self.n_id,
                                              lang_code,
                                              phoneme,
                                              self._name_type,
                                              TTS_TYPE_PHONEME,
                                              self._left_right
                                              )
                    self.add_tts(tts_obj)
                    if match_code:  # 如果指定了code，只收一个
                        break
            else:
                print 'phoneme or lang_code is none. id=%d' % self.n_id

    def split_phoneme_info(self, phoneme_info):
        '''phoneme_info: [音素文本1|音素文本2|...][语种1|语种2|...]'''
        phoneme_list = list()
        language_list = list()
        if not phoneme_info:
            return phoneme_list, language_list
        p = re.compile(r'\[.+?\]')  # 取得两个中括号以及之间的字符串, ‘?’：表示非贪婪
        phoneme_info_list = p.findall(phoneme_info)
        phoneme_list = phoneme_info_list[0][1:-1].split('|')  # [1:-1]去掉"["和"]"
        language_list = phoneme_info_list[1][1:-1].split('|')
        return phoneme_list, language_list


#==============================================================================
# MultiLangFeatureNameTa 行政名称类
#==============================================================================
class MultiLangAdminNameTa(MultiLangNameTa):
    '''处理RDF的多语言
    '''

    def __init__(self,
                 n_id=None,
                 lang_code=None,
                 str_name=None,
                 name_type=None,
                 tts_type=TTS_TYPE_NOT_TTS,
                 left_right=LEFT_RIGHT_B
                 ):
        '''
        Constructor
        '''
        MultiLangNameTa.__init__(self, n_id, lang_code, str_name,
                                 name_type, tts_type, left_right)

    def set_multi_name(self,
                       names,
                       lang_codes,
                       nametypes,
                       sols=None,
                       phoneme_infos=None
                       ):
        '设置名称'
        if not names:  # 无名称
            return
        if not sols:  # 没有指定左右侧
            sols = [None] * len(names)
        if not phoneme_infos:
            phoneme_infos = [None] * len(names)
        prev_name_type = None
        prev_left_right = None
        alter_name = None
        for name, lang_code, org_name_type, sol, phoneme_info in \
                zip(names, lang_codes, nametypes, sols, phoneme_infos):
            left_right = self._cvt_left_right(sol)
            name_type = self._cvt_name_type(org_name_type)
            # TTS
            phoneme_list, language_list = \
                        self.split_phoneme_info(phoneme_info)
            if not self._str_name:  # 第一个名称
                self._str_name = name
                self._lang_code = lang_code
                self._name_type = name_type
                self._left_right = left_right
                self._tts_type = TTS_TYPE_NOT_TTS
                prev_name_type = name_type
                prev_left_right = left_right
                self.add_all_tts(phoneme_list, language_list, lang_code)
                continue

            if (prev_name_type != name_type or  # 名称种别变化
                prev_left_right != left_right):  # 左右变化
                if alter_name:
                    self.add_alter(alter_name)
                alter_name = MultiLangAdminNameTa(self.n_id,
                                                  lang_code,
                                                  name,
                                                  name_type,
                                                  TTS_TYPE_NOT_TTS,
                                                  left_right
                                                  )
                alter_name.add_all_tts(phoneme_list, language_list, lang_code)
                prev_name_type = name_type
                prev_left_right = left_right
            else:  # 翻译
                trans_name = MultiLangAdminNameTa(self.n_id,
                                                  lang_code,
                                                  name,
                                                  name_type,
                                                  TTS_TYPE_NOT_TTS,
                                                  left_right
                                                  )
                trans_name.add_all_tts(phoneme_list, language_list, lang_code)
                if not alter_name:  # 别名不存在, 本名的翻译
                    self.add_trans(trans_name)
                else:  # 别名存在, 别名的翻译
                    alter_name.add_trans(trans_name)
        if alter_name:
            self.add_alter(alter_name)
        return

    def _cvt_name_type(self, nametype):
        '''行政名称种别变换'''
        if nametype == 'ON':
            return NAME_TYPE_OFFICIAL
        if nametype == 'AN':
            return NAME_TYPE_ALTER
        return None
