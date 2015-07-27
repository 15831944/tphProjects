# -*- coding: UTF8 -*-
'''
Created on 2014-1-7

@author: hongchenzai
'''
import json
import common.log

SHIELD_SPLIT_CHR = '\t'
# 音素种别
TTS_TYPE_NOT_TTS = 'not_tts'  # 非TTS, 普通的文本
TTS_TYPE_TEXT = 'text'
TTS_TYPE_PHONEME = 'phoneme'
TTS_TYPE_PT = 'phoneme_text'
# 左右
LEFT_RIGHT_REVERSE = 'reverse'
LEFT_RIGHT_L = 'left_name'
LEFT_RIGHT_R = 'right_name'
LEFT_RIGHT_B = 'left_right'  # 左右共用
# 名称种别
NAME_TYPE_OFFICIAL = 'office_name'
NAME_TYPE_SHIELD = 'shield'
NAME_TYPE_ROUTE_NUM = 'route_num'
NAME_TYPE_ALTER = 'alter_name'
NAME_TYPE_BRIDGE = 'bridge_name'
NAME_PRIORITY = {NAME_TYPE_OFFICIAL: 1,
                 NAME_TYPE_ALTER: 2,
                 NAME_TYPE_BRIDGE: 3,
                 NAME_TYPE_ROUTE_NUM: 4}


#==============================================================================
# ## 多语言种别名称
#==============================================================================
class MultiLangName(object):
    '''处理多语言
    '''
    _language_code_dict = {}
    _left_right_dict = {LEFT_RIGHT_REVERSE: 0,
                        LEFT_RIGHT_L: 1,
                        LEFT_RIGHT_R: 2,
                        LEFT_RIGHT_B: 3
                        }
    _tts_type_dict = {TTS_TYPE_NOT_TTS: 0,
                      TTS_TYPE_TEXT: 1,
                      TTS_TYPE_PHONEME: 2,
                      TTS_TYPE_PT: 3
                      }
    _name_type_dict = {NAME_TYPE_OFFICIAL: 1,
                       NAME_TYPE_SHIELD: 2,
                       NAME_TYPE_ROUTE_NUM: 3,
                       NAME_TYPE_ALTER: 4,
                       NAME_TYPE_BRIDGE: 5
                       }

    @staticmethod
    def set_language_code(code_dict):
        if not MultiLangName._language_code_dict:
            MultiLangName._language_code_dict = code_dict

    @staticmethod
    def get_language_code():
        return MultiLangName._language_code_dict

    @staticmethod
    def is_initialized():
        if MultiLangName._language_code_dict:
            return True
        else:
            return False
    
    @staticmethod
    def initialize():
        import common.database
        pg = common.database.pg_client()
        pg.connect2()
        sqlcmd = """
        select language_code
           from language_tbl
           order by language_code;
        """
        code_dict = dict()
        language_codes = pg.get_batch_data2(sqlcmd)
        for lang_info in language_codes:
            lang_code = lang_info[0]
            code_dict[lang_code] = None
        MultiLangName.set_language_code(code_dict)
    
    @staticmethod
    def name_array_2_json_string(name_id_array, language_code_array, name_array, phonetic_string_array):
        objMultiLangName = MultiLangName()
        pre_name_id = None
        for (name_id, language_code, name, phonetic_string) in zip(name_id_array, 
                                                                   language_code_array, 
                                                                   name_array, 
                                                                   phonetic_string_array):
            #print name_id, language_code, name, phonetic_string
            if pre_name_id is None:
                # offical name
                objOneName = objMultiLangName
                objOneName.setOfficalName(language_code, 
                                          name, 
                                          NAME_TYPE_OFFICIAL)
                if phonetic_string is not None:
                    objTTS = MultiLangName(language_code,
                                           phonetic_string,
                                           NAME_TYPE_OFFICIAL,
                                           TTS_TYPE_PHONEME)
                    objOneName.set_tts(objTTS)
            elif pre_name_id != name_id:
                # alter name
                objOneName = MultiLangName(language_code, 
                                           name, 
                                           NAME_TYPE_ALTER)
                objMultiLangName.add_alter(objOneName)
                
                if phonetic_string is not None:
                    objTTS = MultiLangName(language_code,
                                           phonetic_string,
                                           NAME_TYPE_ALTER,
                                           TTS_TYPE_PHONEME)
                    objOneName.set_tts(objTTS)
            else:
                # trans name
                objTransName = MultiLangName(language_code, 
                                             name, 
                                             objOneName.get_name_type())
                objOneName.add_trans(objTransName)
                
                if phonetic_string is not None:
                    objTTS = MultiLangName(language_code,
                                           phonetic_string,
                                           objOneName.get_name_type(),
                                           TTS_TYPE_PHONEME)
                    objTransName.set_tts(objTTS)
            
            #
            pre_name_id = name_id
        
        # 
        return objMultiLangName.json_format_dump()

    @staticmethod
    #===============================================================================
    # Input structure: 
    #    --> name_id,name_type,language_code,name,phonetic_language_code,phonetic_name    
    # 
    # It should be ordered by: 
    #    --> name_id,language_code,name_type,phonetic_language_code,phonetic_name
    # 
    # If one name has two or more phonetic names, then these phonetic names shouled be unfolded as following:
    #    ---> {name1_lang1,name1_lang1,...} VS {phon_lang1,phon_lang2,...}
    #===============================================================================
    
    def name_array_2_json_string_multi_phon(name_id_array, name_type_array, language_code_array, \
                    name_array, phonetic_lang_array, phonetic_string_array):
        objMultiLangName = MultiLangName()
        pre_name_id = None
        for (name_id, name_type, language_code, name, phonetic_lang, phonetic_string) in zip(name_id_array, 
                                                                   name_type_array,
                                                                   language_code_array, 
                                                                   name_array, 
                                                                   phonetic_lang_array,
                                                                   phonetic_string_array):
#            print name_id, name_type, language_code, name, phonetic_lang, phonetic_string
            if pre_name_id is None :
                # offical name
                objOneName = objMultiLangName
                objOneLangName = objOneName
                objOneName.setOfficalName(language_code, 
                                          name, 
                                          name_type)
                if phonetic_string is not None and len(phonetic_string) > 0 :
                    objTTS = MultiLangName(phonetic_lang,
                                           phonetic_string,
                                           name_type,
                                           TTS_TYPE_PHONEME)
                    objOneLangName.add_tts(objTTS)
                        
            elif pre_name_id != name_id:
                # alter name
                objOneName = MultiLangName(language_code, 
                                           name, 
                                           name_type)
                objOneLangName = objOneName
                objMultiLangName.add_alter(objOneName)
                
                if phonetic_string is not None and len(phonetic_string) > 0:
                    objTTS = MultiLangName(phonetic_lang,
                                           phonetic_string,
                                           name_type,
                                           TTS_TYPE_PHONEME)
                    objOneLangName.add_tts(objTTS)
                    
            elif pre_name_id == name_id and pre_lang == language_code and pre_name == name:
                # pass same name, add another tts.
                if phonetic_string is not None and len(phonetic_string) > 0:
                    objTTS = MultiLangName(phonetic_lang,
                                           phonetic_string,
                                           objOneName.get_name_type(),
                                           TTS_TYPE_PHONEME)
                    objOneLangName.add_tts(objTTS)
                                                  
            else:
                # trans name
                objTransName = MultiLangName(language_code, 
                                             name, 
                                             objOneName.get_name_type())
                objOneLangName = objTransName
                objOneName.add_trans(objTransName)
                
                if phonetic_string is not None and len(phonetic_string) > 0:
                    objTTS = MultiLangName(phonetic_lang,
                                           phonetic_string,
                                           objOneName.get_name_type(),
                                           TTS_TYPE_PHONEME)
                    objOneLangName.add_tts(objTTS)
            
            pre_name_id = name_id
            pre_lang = language_code
            pre_name = name
 
        return objMultiLangName.json_format_dump()
    
    def __init__(self,
                 lang_code=None,
                 str_name=None,
                 name_type=None,
                 tts_type=TTS_TYPE_NOT_TTS,
                 left_right=LEFT_RIGHT_B
                 ):
        '''
        Constructor
        '''
        self._lang_code = lang_code
        self._str_name = str_name
        self._name_type = name_type
        self._tts_type = tts_type
        self._left_right = left_right
        self._tts_objs = list()  # TTS(MultLangName对象)
        self._trans_objs = list()  # 翻译(MultLangName对象)
        self._alter_objbs = list()  # 别名、桥名等其他名称
    
    def setOfficalName(self, lang_code, str_name, name_type):
        self._lang_code = lang_code
        self._str_name = str_name
        self._name_type = name_type

    def get_name_type(self):
        return self._name_type

    def get_lang_code(self):
        return self._lang_code

    def _check_lang_code(self, lang_code):
        '''判断language code是否正确。'''
        if MultiLangName._language_code_dict:
            return lang_code in MultiLangName._language_code_dict
        return False

    def _set_language_flag(self, lang_code):
        MultiLangName._language_code_dict[lang_code] = True

    def add_trans(self, trans_obj):
        '添加翻译名称.'
        if trans_obj:
            self._trans_objs.append(trans_obj)

    def add_alter(self, alter_obj):
        if alter_obj:
            self._alter_objbs.append(alter_obj)

    def add_tts(self, tts_obj):
        if tts_obj:
            self._tts_objs.append(tts_obj)

    def set_tts(self, tts_obj):
        if tts_obj:
            self._tts_objs = [tts_obj]
        else:
            self._tts_objs = list()

    def has_tts(self):
        if self._tts_objs:
            return True
        else:
            return False

    def has_trans(self):
        if self._trans_objs:
            return True
        return False

    def json_format_dump(self):
        one_name = self.get_dict_list()
        multi_name = list()
        if one_name:
            multi_name.append(one_name)
        else:
            # print log here...
            return None

        # ## 别名
        for alter_name in self._alter_objbs:
            one_name = alter_name.get_dict_list()
            if one_name:
                multi_name.append(one_name)
            else:
                # print log here...
                return None
        if multi_name:
            json_name = json.dumps(multi_name, ensure_ascii=False,
                                   encoding='utf8', sort_keys=True)
            return json_name
        else:
            # print log here...
            return None

    @staticmethod
    def json_format_dump2(ml_name_dict_list):
        json_name = ''
        if not ml_name_dict_list:
            return json_name
        for one_multi_name in ml_name_dict_list:
            for name_dict in one_multi_name:
                side_type = name_dict.get('left_right')
                if side_type not in MultiLangName._left_right_dict:
                    errorInfo = 'Error left_right.'
                    common.log.common_log.instance().logger('MultiLang').error(errorInfo)
                    return json_name
                tts_type = name_dict.get('tts_type')
                if tts_type not in MultiLangName._tts_type_dict:
                    errorInfo = 'Error TTS type.'
                    common.log.common_log.instance().logger('MultiLang').error(errorInfo)
                    return json_name
                name_type = name_dict.get('type')
                if name_type not in MultiLangName._name_type_dict:
                    errorInfo = 'Error Name type.'
                    common.log.common_log.instance().logger('MultiLang').error(errorInfo)
                    return json_name
                lang_code = name_dict.get('lang')
                if lang_code not in MultiLangName._language_code_dict:
                    errorInfo = 'Error: %s' % lang_code
                    common.log.common_log.instance().logger('MultiLang').error(errorInfo)
                    return json_name
        json_name = json.dumps(ml_name_dict_list, ensure_ascii=False,
                               encoding='utf8', sort_keys=True)
        return json_name

    def get_dict_list(self):
        dict_list = list()
        # ## 官方语言名称
        name_dict = self._name_to_dict()
        if name_dict:
            dict_list.append(name_dict)
        else:
            # print 'print log here.'
            return dict_list  # 空
        # ## TTS
        for tts in self._tts_objs:
            name_dict = tts.get_dict_list()
            if name_dict:
                dict_list += name_dict
            else:
                return list()  # 空
        # ## 翻译
        for name in self._trans_objs:
            trans_dict_list = name.get_dict_list()
            dict_list += trans_dict_list
        return dict_list

    def convert_name_to_dict(self,
                             lang_code,
                             str_name,
                             name_type,
                             tts_type=TTS_TYPE_NOT_TTS,
                             left_right=LEFT_RIGHT_B
                             ):
        if not str_name or not name_type or not lang_code:
            return None
        if not self._check_lang_code(lang_code):
            return None
        else:
            self._set_language_flag(lang_code)
        return {"lang": lang_code,
                "left_right": left_right,  # 左右共用
                "type": name_type,
                "tts_type": tts_type,
                "val": str_name
                }

    def _name_to_dict(self):
        if self._left_right not in MultiLangName._left_right_dict:
            errorInfo = 'Error left_right.'
            common.log.common_log.instance().logger('MultiLang').error(errorInfo)
            return None
        if self._tts_type not in MultiLangName._tts_type_dict:
            errorInfo = 'Error TTS type.'
            common.log.common_log.instance().logger('MultiLang').error(errorInfo)
            return None
        if self._name_type not in MultiLangName._name_type_dict:
            errorInfo = 'Error Name type.'
            common.log.common_log.instance().logger('MultiLang').error(errorInfo)
            return None

        # if not self._lang_code in MultLangName._language_code_dict:
        if not self._check_lang_code(self._lang_code):
            errorInfo = 'Error: %s' % self._lang_code
            common.log.common_log.instance().logger('MultiLang').error(errorInfo)
            return None
        else:
            self._set_language_flag(self._lang_code)
        name = self._str_name.replace('\\', '\\\\')
        # 双引号替换成\\"， 不然json.loads的时候会出错
        name = name.replace('"', '\\"')
        return {"lang": self._lang_code,
                "left_right": self._left_right,  # 左右共用
                "type": self._name_type,
                "tts_type": self._tts_type,
                "val": name
                }


##################################################################
# ## 盾牌类
##################################################################
class MultiLangShield(MultiLangName):

    def __init__(self,
                 shieldid,
                 shield_number,
                 lang_code,
                 tts_type=TTS_TYPE_NOT_TTS,
                 left_right=LEFT_RIGHT_B,
                 dirction=''):
        '''
        Constructor
        '''
        self.shield_id = shieldid
        self.shield_number = shield_number
        self.dirction = dirction  # 道路方向
        if not self.shield_id or not self.shield_number:
            # print "error: shield_id=%s, shield_number=%s"
            pass
        str_shield = self.shield_id + SHIELD_SPLIT_CHR + self.shield_number
        if self.dirction:
            str_shield += SHIELD_SPLIT_CHR + self.dirction
        MultiLangName.__init__(self, lang_code, str_shield, NAME_TYPE_SHIELD)
