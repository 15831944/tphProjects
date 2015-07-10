# -*- coding: UTF8 -*-
'''
Created on 2015-5-12

@author: zhaojie
'''
import re
from component.default.multi_lang_name import *
import common


#==============================================================================
# MultLangNameTa 名称类
#==============================================================================
class MultiLangNameNI(MultiLangName):
    '''处理NI的多语言
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

    def add_all_tts(self, phoneme_list, language_list, match_code=None):
                    
        for phoneme, lang_code in zip(phoneme_list, language_list):
            if phoneme and lang_code:
                # match_code为空，就不比较语种
                
                tts_obj = MultiLangNameNI(self.n_id,
                                          lang_code,
                                          phoneme,
                                          self._name_type,
                                          TTS_TYPE_PHONEME,
                                          self._left_right
                                          )
                self.add_tts(tts_obj)
            else:
                print 'phoneme or lang_code is none. id=%d' % self.n_id

    def split_phoneme_info(self, phontype_type, phoneme_info):
        '''phoneme_info: [音素文本1|音素文本2|...][语种1|语种2|...]'''
        phoneme_list = list()
        language_list = list()
        if not phoneme_info:
            return phoneme_list, language_list
        
        language_list = phontype_type.split('#')
        phoneme_list = phoneme_info.split('#')
        return phoneme_list, language_list
