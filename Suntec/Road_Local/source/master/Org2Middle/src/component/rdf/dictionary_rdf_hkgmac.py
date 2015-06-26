# -*- coding: UTF8 -*-
'''
Created on 2015-4-2

@author: zhaojie
'''
from component.rdf.dictionary_rdf import comp_dictionary_rdf
from component.rdf.multi_lang_name_rdf import MultiLangNameRDF

class comp_dictionary_rdf_hkg_mac(comp_dictionary_rdf):
    
    def __init__(self):
        '''
        Constructor
        '''
        comp_dictionary_rdf.__init__(self)
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
        tts_obj_all = None
        for phonetic in phonetic_list:
            phonetic_str = phonetic[0]
            phonetic_lang_code = phonetic[1]
            tts_obj = MultiLangNameRDF(phonetic_lang_code,
                                       phonetic_str,
                                       name_type,
                                       'phoneme')
            if tts_obj_all:
                tts_obj_all.add_tts(tts_obj)
            else:
                tts_obj_all = tts_obj
        return tts_obj_all
    
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
#                        self.log.warning('No TTS, name=%s' % name)
                        pass
            multi_name.add_trans(trans_obj)
            name_cnt += 1
        return multi_name