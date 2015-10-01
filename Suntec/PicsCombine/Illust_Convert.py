# -*- coding: cp936 -*-
'''
Created on 2012-11-26
'''
import os
import sys
from PIL import Image

#遍历srcDir文件夹，将它下面的所有.jpg和.png进行降制
#将会生成一个名为destDir的文件夹，其子目录结构与srcDir的完全相同
def ConvertImage(srcDir, destDir, quality=0.5):
    if(os.path.exists(srcDir) == False):
        print "source directory does not exist: " + srcDir
        return
        
    for curDir,dirNames,fileNames in os.walk(srcDir):
        for oneFile in fileNames:
            if oneFile[-4:] == ".jpg" or oneFile[-4:] == ".png":    
                srcFile = os.path.join(curDir, oneFile)
                tempDestDir = curDir.replace(srcDir, destDir)
                if(os.path.isdir(tempDestDir) == False):
                    cmd = "md \"%s\"" % (tempDestDir)
                    os.system(cmd)
                    print "created " + tempDestDir
                
                image = Image.open(srcFile)
                resizedImage = image.resize((image.size[0]*quality, image.size[1]*quality), Image.BILINEAR)
                destFile = os.path.join(tempDestDir, oneFile)
                resizedImage.save(destFile)
                print "    " + srcFile
                print "        >>>>>>>>  " + destFile
    return

def main():
    arglen = len(sys.argv)
    if(arglen <= 2):
        return
    else:
        srcDir = ""
        destDir = ""
        quality = 0.5
        for i in range(1, arglen):
            arg = sys.argv[i]
            if(arg.lower() == "/srcdir"):
                srcDir = str(sys.argv[i+1])
            elif(arg.lower() == "/destdir"):
                destDir = str(sys.argv[i+1])
            elif(arg.lower() == "/quality"):
                destDir = str(sys.argv[i+1])
            else:
                pass
        ConvertImage(srcDir, destDir, quality)
    return

if __name__ == '__main__':
    main()