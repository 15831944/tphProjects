# -*- coding: UTF-8 -*-
'''
Created on 2014-12-8

@author: hcz
'''


def strQ2B_jp(s):
    '''Translate Full Width To Half Width (JP).'''
    if s is None:
        return None
    s = unicode(s)
    t = ''
    for c in s:
        half_str = JP_WIDTH_2_HALF.get(c)  # 日本平假名
        if half_str is not None:
            t += half_str
        else:
            # 字符、数字
            half_str = Q2B_TRAN_TAB.get(c)
            if half_str is not None:
                t += half_str
            else:
                t += c
    return str(t)


def strB2Q_jp(s):
    '''Translate Half Width to Full Width(JP).'''
    if s is None:
        return None
    t = ''
    c_idx = 0
    str_len = len(s)
    while c_idx < str_len:
        if c_idx < str_len - 1:  # Not Last Char
            two_char = s[c_idx:(c_idx + 2)]
            full_char = JP_HALF_2_WIDTH.get(two_char)
            if full_char is not None:
                t += full_char
                c_idx += 2
                continue
        one_char = s[c_idx]
        full_char = JP_HALF_2_WIDTH.get(two_char)  # 日文
        if full_char is not None:
            t += full_char
        else:
            # 字符、数字
            full_char = Q2B_TRAN_TAB.get(one_char)
            if full_char is not None:
                t += full_char
            else:
                t += one_char
        c_idx += 1
    return str(t)

FULL_WIDTH_CHARS = (u'ＡＢＣＤＥＦＧＨＩＪＫＬＭＮＯＰＱＲＳＴＵＶＷＸＹＺ'
                    'ａｂｃｄｅｆｇｈｉｊｋｌｍｎｏｐｑｒｓｔｕｖｗｘｙｚ'
                    '１２３４５６７８９０'
                    )

HALF_WIDTH_CHARS = (u'ABCDEFGHIJKLMNOPQRSTUVWXYZ'
                    'abcdefghijklmnopqrstuvwxyz'
                    '1234567890'
                    )
# Full ==> Half
Q2B_TRAN_TAB = dict((a, b) for a, b in zip(FULL_WIDTH_CHARS, HALF_WIDTH_CHARS))
# Half ==> Full
B2Q_TRAN_TAB = dict((a, b) for a, b in zip(HALF_WIDTH_CHARS, FULL_WIDTH_CHARS))

# 全角==>半角
JP_WIDTH_2_HALF = {u'チ': u'ﾁ', u'ウ': u'ｳ',
                   u'テ': u'ﾃ', u'ツ': u'ﾂ',
                   u'ナ': u'ﾅ', u'ト': u'ﾄ',
                   u'ヌ': u'ﾇ', u'タ': u'ﾀ',
                   u'ノ': u'ﾉ', u'ネ': u'ﾈ',
                   u'ヒ': u'ﾋ', u'ハ': u'ﾊ',
                   u'ニ': u'ﾆ', u'バ': u'ﾊﾞ',
                   u'マ': u'ﾏ', u'ホ': u'ﾎ',
                   u'ド': u'ﾄﾞ', u'ミ': u'ﾐ',
                   u'モ': u'ﾓ', u'パ': u'ﾊﾟ',
                   u'ユ': u'ﾕ', u'ヤ': u'ﾔ',
                   u'ム': u'ﾑ', u'ヨ': u'ﾖ',
                   u'ル': u'ﾙ', u'リ': u'ﾘ',
                   u'ヅ': u'ﾂﾞ', u'ョ': u'ｮ',
                   u'ダ': u'ﾀﾞ', u'グ': u'ｸﾞ',
                   u'ゾ': u'ｿﾞ', u'レ': u'ﾚ',
                   u'ー': u'ｰ', u'ン': u'ﾝ',
                   u'ズ': u'ｽﾞ', u'ザ': u'ｻﾞ',
                   u'ワ': u'ﾜ', u'ボ': u'ﾎﾞ',
                   u'ゲ': u'ｹﾞ', u'ブ': u'ﾌﾞ',
                   u'ギ': u'ｷﾞ', u'ヘ': u'ﾍ',
                   u'ロ': u'ﾛ', u'ポ': u'ﾎﾟ',
                   u'フ': u'ﾌ', u'デ': u'ﾃﾞ',
                   u'プ': u'ﾌﾟ', u'ソ': u'ｿ',
                   u'ペ': u'ﾍﾟ', u'ヂ': u'ﾁﾞ',
                   u'ラ': u'ﾗ', u'ピ': u'ﾋﾟ',
                   u'ベ': u'ﾍﾞ', u'メ': u'ﾒ',
                   u'ゼ': u'ｾﾞ', u'ァ': u'ｧ',
                   u'ヲ': u'ｦ', u'ゥ': u'ｩ',
                   u'ィ': u'ｨ', u'ォ': u'ｫ',
                   u'ェ': u'ｪ', u'ュ': u'ｭ',
                   u'ャ': u'ｬ', u'ッ': u'ｯ',
                   u'ビ': u'ﾋﾞ', u'ア': u'ｱ',
                   u'ジ': u'ｼﾞ', u'ゴ': u'ｺﾞ',
                   u'イ': u'ｲ', u'オ': u'ｵ',
                   u'エ': u'ｴ', u'キ': u'ｷ',
                   u'カ': u'ｶ', u'ケ': u'ｹ',
                   u'ク': u'ｸ', u'サ': u'ｻ',
                   u'コ': u'ｺ', u'ス': u'ｽ',
                   u'シ': u'ｼ', u'ガ': u'ｶﾞ',
                   u'セ': u'ｾ',
                   }

# 半角==>全角
JP_HALF_2_WIDTH = {u'ﾁ': u'チ', u'ｳ': u'ウ',
                   u'ﾃ': u'テ', u'ﾂ': u'ツ',
                   u'ﾅ': u'ナ', u'ﾄ': u'ト',
                   u'ﾇ': u'ヌ', u'ﾀ': u'タ',
                   u'ﾉ': u'ノ', u'ﾈ': u'ネ',
                   u'ﾋ': u'ヒ', u'ﾊ': u'ハ',
                   u'ﾆ': u'ニ', u'ﾊﾞ': u'バ',
                   u'ﾏ': u'マ', u'ﾎ': u'ホ',
                   u'ﾄﾞ': u'ド', u'ﾐ': u'ミ',
                   u'ﾓ': u'モ', u'ﾊﾟ': u'パ',
                   u'ﾕ': u'ユ', u'ﾔ': u'ヤ',
                   u'ﾑ': u'ム', u'ﾖ': u'ヨ',
                   u'ﾙ': u'ル', u'ﾘ': u'リ',
                   u'ﾂﾞ': u'ヅ', u'ｮ': u'ョ',
                   u'ﾀﾞ': u'ダ', u'ｸﾞ': u'グ',
                   u'ｿﾞ': u'ゾ', u'ﾚ': u'レ',
                   u'ｰ': u'ー', u'ﾝ': u'ン',
                   u'ｽﾞ': u'ズ', u'ｻﾞ': u'ザ',
                   u'ﾜ': u'ワ', u'ﾎﾞ': u'ボ',
                   u'ｹﾞ': u'ゲ', u'ﾌﾞ': u'ブ',
                   u'ｷﾞ': u'ギ', u'ﾍ': u'ヘ',
                   u'ﾛ': u'ロ', u'ﾎﾟ': u'ポ',
                   u'ﾌ': u'フ', u'ﾃﾞ': u'デ',
                   u'ﾌﾟ': u'プ', u'ｿ': u'ソ',
                   u'ﾍﾟ': u'ペ', u'ﾁﾞ': u'ヂ',
                   u'ﾗ': u'ラ', u'ﾋﾟ': u'ピ',
                   u'ﾍﾞ': u'ベ', u'ﾒ': u'メ',
                   u'ｾﾞ': u'ゼ', u'ｧ': u'ァ',
                   u'ｦ': u'ヲ', u'ｩ': u'ゥ',
                   u'ｨ': u'ィ', u'ｫ': u'ォ',
                   u'ｪ': u'ェ', u'ｭ': u'ュ',
                   u'ｬ': u'ャ', u'ｯ': u'ッ',
                   u'ﾋﾞ': u'ビ', u'ｱ': u'ア',
                   u'ｼﾞ': u'ジ', u'ｺﾞ': u'ゴ',
                   u'ｲ': u'イ', u'ｵ': u'オ',
                   u'ｴ': u'エ', u'ｷ': u'キ',
                   u'ｶ': u'カ', u'ｹ': u'ケ',
                   u'ｸ': u'ク', u'ｻ': u'サ',
                   u'ｺ': u'コ', u'ｽ': u'ス',
                   u'ｼ': u'シ', u'ｶﾞ': u'ガ',
                   u'ｾ': u'セ',
                   }
