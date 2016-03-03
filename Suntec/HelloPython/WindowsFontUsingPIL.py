#encoding=utf-8
import os
import re

class FontEnumerator(object):
    def __init__(self):
        self.mFontSet = set()
        for curDir, dirNames, fileNames in os.walk(r"C:\Windows\Fonts"):
            for oneFile in fileNames:
                self.mFontSet.add(oneFile)
    
    def isFontExistInSystem(self, strFont):
        strPattern = ''
        for x in strFont:
            strPattern += x
            strPattern += '(\s)?'
        pattern = re.compile(strPattern, re.X | re.I)
        for oneFont in self.mFontSet:
            if pattern.search(oneFont):
                return True
        return False
    
    def printAllFont(self):
        for x in self.mFontSet:
            print x

if __name__ == '__main__':
    fontEnumerator = FontEnumerator()
    fontList = ['''ArialNarrow''',
                '''HelveticaLTStd''',
                '''Sarun'sThangLuang''',
                '''Arial''']
    for oneFont in fontList:
        print oneFont + ': ' + str(fontEnumerator.isFontExistInSystem(oneFont))












