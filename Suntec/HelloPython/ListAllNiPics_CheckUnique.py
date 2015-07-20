#encoding=utf-8

import os

def main(srcDir):
    oFStreamPattern = open(os.path.join(os.getcwd() , "all_in_pattern.csv"), "w")
    oFStreamArrow = open(os.path.join(os.getcwd() , "all_in_arrow.csv"), "w")
    for curDir,dirNames,fileNames in os.walk(srcDir):
        for oneFile in fileNames:
            if os.path.splitext(oneFile)[1].lower() == '.png' and curDir.find('pattern') != -1:
                oFStreamPattern.write(curDir + ',' + oneFile + '\n')
            if os.path.splitext(oneFile)[1].lower() == '.png' and curDir.find('arrow') != -1:
                oFStreamArrow.write(curDir + ',' + oneFile + '\n')
    return

if __name__ == '__main__':
    main(r'C:\My\20150504-0507_ni_spotguide\17CY Sample data\data\GraphAndVoice')