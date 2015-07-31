# -*- coding: utf-8 -*-
'''
Created on 2014-6-3

@author:  Xuqiliang
'''
IND = {
    'ai': 'a&I',
    'au': 'a&U',
    'dZ': 'd&Z',
    'oi': 'O&I',
    'tS': 't&S',
    ' ': '_',
    '"': "'",
    '*': '"',
    '?': '?',
    '@': '$',
    'E': 'e',
    'I': 'i',
    'J': 'n~',
    'N': 'nK',
    'S': 'S',
    'U': 'u',
    'a': 'a',
    'b': 'b',
    'd': 'd',
    'e': 'e',
    'f': 'f',
    'g': 'g',
    'h': 'h',
    'i': 'i',
    'j': 'j',
    'k': 'k',
    'l': 'l',
    'm': 'm',
    'n': 'n',
    'O': 'o',
    'o': 'o',
    'p': 'p',
    'r': 'r',
    's': 's',
    't': 't',
    'u': 'u',
    'w': 'w',
    'x': 'x',
    'z': 'z',
    '|': '.'
}

ESG = {
    'I@':'I&$',
    'OI':'O&I',
    'U@':'U&$',
    'aI':'a&I',
    'aU':'a&U',
    'dZ':'d&Z',
    'e@':'E&$',
    'tS':'t&S',
    '"':"'",
    '#':'_',
    '$':'.',
    '%':"'2",
    '*':'"',
    '3':'E0',
    '@':'$',
    'A':'A',
    'D':'D',
    'E':'E',
    'I':'I',
    'N':'nK',
    'O':'O',
    'S':'S',
    'T':'T',
    'U':'U',
    'V':'^',
    'Z':'Z',
    'b':'b',
    'c':'A+',
    'd':'d',
    'e':'e&I',
    'f':'f',
    'g':'g',
    'h':'h',
    'i':'i',
    'j':'j',
    'k':'k',
    'l':'l',
    'm':'m',
    'n':'n',
    'o':'o&U',
    'p':'p',
    'r':'R+',
    's':'s',
    't':'t',
    'u':'u',
    'v':'v',
    'w':'w',
    'z':'z',
    '{':'@',
    '|':'.',
	'e|r':'E.R+',
	'{|r':'E.R+',
	'@r':'E0R+',
    ' ':'_'
}

PHONES = {'IND': IND, 'SEN': ESG}

def nt_sampa_2_lh_plus(language, ntsamp_phoneme):
    """
    'language': IND, ESG. Other will be raise exception 'ValueError'
    'ntsamp_phoneme': the NT-Sampa phoneme string

    @ret: L&H+ phoneme string
    """

    phones = PHONES.get(language)
    if not phones:
        return ntsamp_phoneme

    len_of_phoneme = len(ntsamp_phoneme)
    if len_of_phoneme == 0:
        raise ValueError('ntsamp_phoneme is EMPTY')

    i = 0
    lhphone = ''
    while i < len_of_phoneme:
        trip_char = ntsamp_phoneme[i : i+3]
        if trip_char in phones.keys():
            lhphone += phones[trip_char]
            i += 3
            continue

        double_char = ntsamp_phoneme[i : i+2]
        if double_char in phones.keys():
            lhphone += phones[double_char]
            i += 2
            continue

        single_char = ntsamp_phoneme[i]
        if '*' == single_char and '"' != ntsamp_phoneme[i + 1]:
            raise ValueError('The nuclear stress(*), if present must immediately precede a primary stress(") symbol.')

        lhphone += phones[single_char]
        i += 1

    return lhphone


