# -*- coding: UTF8 -*-
'''
Created on 2014-1-7

@author: hongchenzai
'''
SPLIT_CHR = '|'
SHIELD_SPLIT_CHR = '\t'
from component.multi_lang_name import MultiLangName


#==============================================================================
# MultLangNameRDF 名称类
#==============================================================================
class MultiLangNameRDF(MultiLangName):
    '''处理RDF的多语言
    '''

    def __init__(self, lang_code, name, name_type='office_name',
                 tts_type='not_tts', left_right='left_right'):
        '''
        Constructor
        '''
        MultiLangName.__init__(self,
                              self._change_language_code(lang_code),
                              name, name_type,
                              tts_type, left_right)

#     def _check_lang_code(self, lang_code):
#         '''判断language code是否正确。'''
#         lang_code_dict = {'CHN': '',
#                           'CHT': '',
#                           'ENG': '',
#                           'IND': '',
#                           'MAY': '',
#                           'POR': '',
#                           'THA': '',
#                           'THE': '',
#                           'VIE': '',
#                           'VIX': ''
#                           }
#         # language code 在字典表里
#         return lang_code in lang_code_dict

    def _change_language_code(self, lang_code):
        if lang_code == 'WEN':  # World English.
            return 'ENG'
        else:
            return lang_code

    def add_trans_name(self, lang_code, name):
        if not name or not lang_code:
            return
        trans_obj = MultiLangNameRDF(lang_code, name, self._name_type,
                                     self._tts_type, self._left_right)
        self.add_trans(trans_obj)

    def set_trans(self, trans_names, language_codes, split_char=SPLIT_CHR):
        if not trans_names:
            return
        if type(trans_names) is not list:
            if split_char:
                trans_names = trans_names.split(split_char)
                luaguage_codes = language_codes.split(split_char)
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
            trans_obj = MultiLangNameRDF(lang_code, name, self._name_type,
                                         self._tts_type, self._left_right)
            self.add_trans(trans_obj)
            name_cnt += 1


##################################################################
# ## 盾牌类
##################################################################
class MultiLangShieldRDF(MultiLangNameRDF):
    def __init__(self, shieldid, shield_number, laug_code,
                 tts_type='not_tts', left_right='left_right',
                 dirction=''):
        '''
        Constructor
        '''
        self.shield_id = shieldid
        self.shield_number = shield_number
        self.dirction = dirction  # 道路放迥
        if not self.shield_id or not self.shield_number:
            # print "error: shield_id=%s, shield_number=%s"
            pass
        str_shield = self.shield_id + SHIELD_SPLIT_CHR + self.shield_number
        if self.dirction:
            str_shield += SHIELD_SPLIT_CHR + self.dirction
        name_type = 'shield'
        MultiLangNameRDF.__init__(self, laug_code, str_shield, name_type)

    def set_trans(self, trans_numbers, luaguage_codes, split_char=SPLIT_CHR):
        if not trans_numbers:
            return
        if type(trans_numbers) is not list:
            if split_char:
                trans_numbers = trans_numbers.split(SPLIT_CHR)
                lang_codes = luaguage_codes.split(SPLIT_CHR)
            else:  # 当split_char为空or空格：不做切割
                trans_numbers = [trans_numbers]
                lang_codes = [luaguage_codes]

        name_num = len(trans_numbers)
        name_cnt = 0
        while name_cnt < name_num:
            number = trans_numbers[name_cnt]
            if not number:
                continue
            lang_code = lang_codes[name_cnt]
            trans_shield = MultiLangShieldRDF(self.shield_id,
                                             number,
                                             lang_code,
                                             self._tts_type,
                                             self._left_right)
            self.add_trans(trans_shield)
            name_cnt += 1
