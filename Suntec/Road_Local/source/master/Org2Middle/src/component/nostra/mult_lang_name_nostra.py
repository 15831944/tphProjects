# -*- coding: UTF8 -*-
'''
Created on 2013-12-7

@author: hongchenzai
'''


class MultLangName(object):
    '''处理泰国多语言
    '''

    def __init__(self):
        '''
        Constructor
        '''

    def json_format_dump(self, name_tha, name_the,
                         name_tts='', name_type='office_name'):
        '''returns string like this:
        "[[
        {"lang": "THA", "left_right": "left_right", "type": "office_name",
         "tts_type":"not_tts", "val": "thailand_Name"},
        {"lang": "THA_TTS", "left_right": "left_right", "type": "office_name",
         "tts_type":"text", "val": "thailand_Name"},
        {"lang": "THE", "left_right": "left_right", "type": "office_name",
         "tts_type":"not_tts", "val": "English_Name"},
        ]]
        "
        '''
        name_list = self.convert_names_to_list(name_tha, name_the,
                                               name_tts, name_type)
        if name_list:
            import json
            json_name = json.dumps(name_list, ensure_ascii=False,
                                   encoding='utf8', sort_keys=True)
            return json_name
        else:
            return None

    def json_format_dump2(self, name_list):
        '''
        returns string like this:
        "[[
        {"lang": "THA", "left_right": "left_right", "type": "office_name",
         "tts_type":"not_tts", "val": "thailand_Name"},
        {"lang": "THA_TTS", "left_right": "left_right", "type": "office_name",
         "tts_type":"text", "val": "tts text"},
        {"lang": "THE", "left_right": "left_right", "type": "office_name",
         "tts_type":"not_tts", "val": "English_Name"},
        ]]
        "
        '''
        if name_list:
            import json
            json_name = json.dumps(name_list, ensure_ascii=False,
                                   encoding='utf8', sort_keys=True)
            return json_name
        else:
            return None

    def convert_names_to_list(self, name_tha, name_the,
                              name_tts='', name_type='office_name'):
        '''returns name list, like this:
        [[
        {"lang": "THA", "left_right": "left_right", "type": "office_name",
         "tts_type":"not_tts", "val": "thailand_Name"},
        {"lang": "THA", "left_right": "left_right", "type": "office_name",
         "tts_type":"text", "val": "tts text"},
        {"lang": "THE", "left_right": "left_right", "type": "office_name",
         "tts_type":"not_tts", "val": "English_Name"},
        ]]
        '''
        name_list = []
        single_name_mult_lang = []
        if (not name_tha) and (not name_the):  # 泰文、英文为空
            return []
        # 有TTS, 无泰文
        if (not name_tha) and name_tts:
            print 'Error: No Thai Name, name_tha = %s, name_tts = %s' \
                % (name_tha, name_tts)
            return []
        # 泰文
        if name_tha:
            tts_type = 'not_tts'
            name_tha_disc = self.convert_name_to_dict('THA',
                                                      name_tha,
                                                      name_type,
                                                      tts_type
                                                      )
            if name_tha_disc:
                single_name_mult_lang.append(name_tha_disc)

            tts_type = 'text'  # TTS纯文本
            name_tts_disc = self.convert_name_to_dict('THA',
                                                      name_tts,
                                                      name_type,
                                                      tts_type
                                                      )
            if name_tts_disc:
                single_name_mult_lang.append(name_tts_disc)
        # 英文
        if name_the:
            tts_type = 'not_tts'
            name_the_disc = self.convert_name_to_dict('THE',
                                                      name_the,
                                                      name_type,
                                                      tts_type
                                                      )
            if name_the_disc:
                single_name_mult_lang.append(name_the_disc)

        name_list.append(single_name_mult_lang)
        return name_list

    def convert_name_to_dict(self, lang_code, str_name, name_type,
                             tts_type='not_tts', left_right='left_right'):
            if not str_name:  # 无名称
                return None
            return {"lang": lang_code,
                    "left_right": left_right,  # 左右共用
                    "type": name_type,
                    "tts_type": tts_type,
                    "val": str_name
                    }
