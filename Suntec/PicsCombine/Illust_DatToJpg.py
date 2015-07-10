#encoding=utf-8

import os
import shutil
import struct

def ConvertDatToImage_Dir(srcDir, destDir, bCopyOrgFile=True):
    if(os.path.exists(srcDir) == False):
        print "source directory does not exist: " + srcDir
        return
    if(os.path.exists(destDir) == True):
        shutil.rmtree(destDir)
    os.makedirs(destDir)
        
    for curDir,dirNames,fileNames in os.walk(srcDir):
        tempDestDir = curDir.replace(srcDir, destDir)
        if(os.path.isdir(tempDestDir) == False):
            os.makedirs(tempDestDir)
            print "created " + tempDestDir
            
        for oneFile in fileNames:
            if oneFile[-4:] == ".dat":
                ConvertDatToImage(os.path.join(srcDir, oneFile), tempDestDir)
                if bCopyOrgFile:
                    shutil.copyfile(os.path.join(srcDir, oneFile), os.path.join(tempDestDir, oneFile))
    return

def ConvertDatToImage(srcDatFile, destDir):
    # 处理arrow图片
    readStream = open(srcDatFile, 'rb')
    readBuf = readStream.read()
    buf6 = readBuf[0:4]
    hFEFE, nPicCount = struct.unpack("<HH", buf6)
    
    for iPic in range(0, nPicCount):
        buf9 = readBuf[4+9*iPic: 4+9*iPic+9]
        bPicInfo, iOffset, iSize = struct.unpack("<bii", buf9)
        newJpgName = '''%s_%d.jpg''' % (os.path.split(srcDatFile)[1][:-4], iPic)
        destFile = os.path.join(destDir, newJpgName)
        bufIPic = readBuf[iOffset: iOffset+iSize]     
        outFS = open(destFile, 'wb')
        outFS.write(bufIPic)
        outFS.close()
        
if __name__ == '__main__':
    ConvertDatToImage_Dir("C:\\My\\20150410_mmi_pic\\illust_pic", "C:\\My\\20150427_datTojpg\\jpg")