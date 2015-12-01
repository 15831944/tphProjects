#encoding=utf-8

import os

def main(srcDir):
    oFStreamPattern = open(os.path.join(os.getcwd() , "all_in_pattern.csv"), "w")
    oFStreamArrow = open(os.path.join(os.getcwd() , "all_in_arrow.csv"), "w")
    for curDir,dirNames,fileNames in os.walk(srcDir):
        for oneFile in fileNames:
            if os.path.splitext(oneFile)[1].lower() == '.png':
                print """'%s',""" % os.path.splitext(oneFile)[0]
    return

if __name__ == '__main__':
    main(r'C:\Users\hexin\Desktop\111')