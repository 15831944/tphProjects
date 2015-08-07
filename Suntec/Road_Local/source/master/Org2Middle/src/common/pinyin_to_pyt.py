#!/usr/bin/python

import re

reo_star = re.compile(r"\*|\*$")
reo_left_enm = re.compile("\(")
reo_right_enm = re.compile("\)")

strpyt = "\x1b\\\\toi=pyt\\\\"
strorth = "\x1b\\\\toi=orth\\\\"

strbegin = "\x1b\\toi=pyt\\"
strend = "\x1b\\toi=orth\\"

#define variable that check during change phoneme
#check_word = re.compile("\([A-Za-z]+\)")
#check_pinyin = re.compile("[A-Za-z]+[1-5]\x20")
#check_star = re.compile("\*")


#define variable that check after change phoneme
after_check_head = re.compile("\x1b\\\\toi=pyt\\\\")
after_check_tail = re.compile("\x1b\\\\toi=orth\\\\")
after_check_eng = re.compile("\x1b\\\\toi=orth\\\\[A-Za-z]+\x1b\\\\toi=pyt\\\\")
after_check_pinyin_madarin = re.compile("[A-Za-z]+[1-5]")
after_check_pinyin_cantonese = re.compile("[A-Za-z]+[1-6]")
after_check_continous_madarin = re.compile("[A-Za-z]+[1-5][A-Za-z]+[1-5]")
after_check_continous_cantonese = re.compile("[A-Za-z]+[1-6][A-Za-z]+[1-6]")

#def checkpyt(phoneme):
    #result = re.sub(check_word, "", phoneme)
    #result = re.sub(check_pinyin, "", result)
    #result = re.sub(check_star, "", result)
    #if  result == None or result.strip() == "":
        #return True
    #else:
        #return False


def pym_pyt_2_pinyintone_check(language, pinyin_phoneme):
    if language == 'PYM':
        after_check_pinyin = after_check_pinyin_madarin
        after_check_continous = after_check_continous_madarin
    elif language == 'PYT':
        after_check_pinyin = after_check_pinyin_cantonese
        after_check_continous = after_check_continous_cantonese
    else:
        raise ValueError('language is not supported! ' + language)

    len_of_phoneme = len(pinyin_phoneme)
    if len_of_phoneme == 0:
        return False

    list_continous = after_check_continous.findall(pinyin_phoneme)
    if list_continous:
        print "not null"
        return "False"+pinyin_phoneme
    result = re.sub(after_check_eng, "", pinyin_phoneme)
    result = re.sub(after_check_head, "", result)
    result = re.sub(after_check_tail, "", result)    
    result = re.sub(after_check_pinyin, "", result)
    result = re.sub(" ", "", result)

    if len(result) == 0:
        return "True"
    else:
        return "False"+pinyin_phoneme

def pinyin2pyt(pinyin_phoneme):
    #len_of_phoneme = len(pinyin_phoneme)
    #if len_of_phoneme == 0:
        #raise ValueError('pinyin_phoneme is EMPTY')

    #if False == checkpyt(pinyin_phoneme):
        #print ValueError('pinyin_phoneme is not standard')

    # tranfer the phoneme
    result = re.sub(reo_star, "", pinyin_phoneme)
    result = re.sub(reo_left_enm, strorth, result)
    result = re.sub(reo_right_enm, strpyt, result)

    # add the start / end mark
    result = strbegin + result + strend


    return result

def readpytfile(filename):
    len_of_filename = len(filename)
    if len_of_filename == 0:
        raise ValueError("filename is EMPTY")
    f = open(filename)
    line = f.readline()
    line = line.strip('\n')

    rd =  file('./test_pyt_result.txt','a+')
    while line:
        rd.write(pinyin2pyt(line))
        rd.write('\n')
        line = f.readline()
        line = line.strip('\n')
    rd.close()

def readresultfile(filename):
    len_of_filename = len(filename)
    if len_of_filename == 0:
        raise ValueError("filename is EMPTY")
    f = open(filename)
    line = f.readline()
    line = line.strip('\n')
    rd =  file('./test_pyt_check.txt','a+')
    lang = "PYT"
    while line:
        #return line
        rd.write(pym_pyt_2_pinyintone_check(lang, line))
        rd.write('\n')
        line = f.readline()
        line = line.strip('\n')
    rd.close()

class pinyinchange :
    def __init__(self, language):
        self.language = language

    def change(self, phoneme) :
        return pinyin2pyt(phoneme)


class pinyincheck :
    def __init__(self, language):
        self.language = language

    def check(self, phoneme):
        return pym_pyt_2_pinyintone_check(self.language, phoneme)

    def read(self, filename):
        return readpytfile(filename)

if __name__ == "__main__":
    #readpytfile("./test_pym.txt")
    #readresultfile("./test_pym_result.txt")
    #readpytfile("./test_pyt.txt")
    readresultfile("./test_pyt_result.txt")
    #readresultfile("./test_pym.txt")
    #readresultfile("./pym_result.txt")
    #readresultfile("./pyt_result.txt")
    #pinyin2pyt("zung1 gwok3 ngan4 hong4 * hoeng1 gong2 *lok6 fu3 fan1 hong4")
    #print ntsamapa2lhplus("Transatlantic * Reinsurance * Company")
