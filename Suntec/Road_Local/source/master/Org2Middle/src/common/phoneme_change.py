# -*- coding:utf-8 -*-
#!/usr/bin/python

APL_VER='17CY'
TTS_ENGINE_VER='VE1.5'
NT_SAMPA_VER='7.0'
TOOL_VER='0.9'

#TOOL_VER from 0.8 to 0.9,香港地区的拼音转换音素头修改为<TTS_PYT>

#from phoneme_language_table import AREA_TABLE
import phoneme_language_table
import pinyin_to_pyt
import ntsamp_to_lhplus

# translate table
_translate_table = {
   #  仕向       语言    源音素类型 目标类型
    ("HONGKONG",        "UKE",  "NTSAMPA", "LHPLAS"     ) : (ntsamp_to_lhplus.ntssampachange("UKE"),    None),
    ("HONGKONG",        "PYM",  "PYM",     "PinYinTone" ) : (pinyin_to_pyt.pinyinchange("PYM"),         pinyin_to_pyt.pinyincheck("PYM")),
    ("HONGKONG",        "PYT",  "PYT",     "PinYinTone" ) : (pinyin_to_pyt.pinyinchange("PYT"),         pinyin_to_pyt.pinyincheck("PYT")),
    ("TAIWAN",          "TWN",  "BPMF",    "PinYinTone" ) : (ntsamp_to_lhplus.ntssampachange("TWN"),    None),
#    ("SOURTHEASTASIA",  "UKE",  "NTSAMPA", "LHPLAS"     ) : (ntsamp_to_lhplus.ntssampachange("UKE"),    None),
    ("SOURTHEASTASIA",  "THA",  "NTSAMPA", "LHPLAS"     ) : (ntsamp_to_lhplus.ntssampachange("THA"),    None),

    #  TODO: 只有PYT和PYM提供了转换后的数据Check功能。其他待补充！
    #  TODO: 印度，南非, 中东, 南美, 澳洲不需要转换
    #  TODO: 中国的不需要转换
    #  TODO: 泰国英语的需要转换待定
    #  TODO: 新加坡英语不需要转换
}

def get_translator(area, language, from_format, to_format) :
    return _translate_table[(area, language, from_format, to_format)][0]

def get_checker(area, language, from_format, to_format):
    return _translate_table[(area, language, from_format, to_format)][1]

if __name__ == "__main__":
    func = get_translator("HONGKONG", "UKE", "NTSAMPA", "LHPLAS")
    print func.change("s a")

    func = get_translator("SOURTHEASTASIA", "THA", "NTSAMPA", "LHPLAS")
    print func.change("M:a?_4 a r?_4 _1?|?_5| _5|?_5| ")

    func = get_translator("HONGKONG", "PYM", "PYM", "PinYinTone")
    print func.change("long2 er4 ji2 wu3 hao4 huo4 gui4 ma3 tou2 (test)")

    func = get_translator("TAIWAN", "TWN", "BPMF", "PinYinTone")
    print func.change("ㄍㄨㄥˉ ㄉㄠˋ ㄨˇ ㄌㄨˋ ㄧˉ ㄉㄨㄢˋ")

    funccheck = get_checker("HONGKONG", "PYM", "PYM", "PinYinTone")
    print funccheck.check("meng1")
