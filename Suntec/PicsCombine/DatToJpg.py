#encoding=utf-8
'''
Created on 2012-11-26
@author: liuxinxing
'''
import os
import sys
from PIL import Image

def ConvertDatToImage1(srcDir, destDir):
    if(os.path.exists(srcDir) == False):
        print "source directory does not exist: " + srcDir
        return
        
    for curDir,dirNames,fileNames in os.walk(srcDir):
        for oneFile in fileNames:
            if oneFile[-4:] == ".dat":    
                srcFile = os.path.join(curDir, oneFile)
                tempDestDir = curDir.replace(srcDir, destDir)
                if(os.path.isdir(tempDestDir) == False):
                    cmd = "md \"%s\"" % (tempDestDir)
                    os.system(cmd)
                    print "created " + tempDestDir
            
    return

def ConvertDatToImage(datFile, destDir):
    # 处理arrow图片
    readStream = open(datFile, 'rb')
    read = readStream.read()
    