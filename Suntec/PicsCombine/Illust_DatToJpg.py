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
            if os.path.splitext(oneFile)[1] == ".dat":
                ConvertDatToImage(os.path.join(curDir, oneFile), tempDestDir)
                if bCopyOrgFile:
                    shutil.copyfile(os.path.join(curDir, oneFile), os.path.join(tempDestDir, oneFile))
    return

def ConvertDatToImage(srcDatFile, destDir):
    readStream = open(srcDatFile, 'rb')
    readBuf = readStream.read()
    buf6 = readBuf[0:4]
    hFEFE, nPicCount = struct.unpack("<HH", buf6)
    
    for iPic in range(0, nPicCount):
        buf9 = readBuf[4+9*iPic: 4+9*iPic+9]
        bPicInfo, iOffset, iSize = struct.unpack("<bii", buf9)
        
        strExt = ".jpg"
        # png图片一般以 0x89 0x50 0x4E 0x47开头。
        if bufIPic[0] == 0x89 and bufIPic[1] == 0x50 and bufIPic[2] == 0x4E and bufIPic[3] == 0x47:
            strExt = ".png"
        
        # jpg图片一般以 0xFF 0xD8 0xFF 0xE0开头。
        if bufIPic[0] == 0xFF and bufIPic[1] == 0xD8 and bufIPic[2] == 0xFF and bufIPic[3] == 0xE0:
            strExt = ".jpg"
        
        newJpgName = '''%s_%d%s''' % (os.path.splitext(os.path.split(srcDatFile)[1])[0], iPic, strExt)
        destFile = os.path.join(destDir, newJpgName)
        bufIPic = readBuf[iOffset: iOffset+iSize]     
        outFS = open(destFile, 'wb')
        outFS.write(bufIPic)
        outFS.close()
        
if __name__ == '__main__':
    dat = r'E:\Test\ni_pic\phase2\guangzhou\SignBoard\pattern'
    pic = r'E:\Test\ni_pic\phase3\guangzhou\SignBoard\pattern'
    ConvertDatToImage_Dir(dat, pic)
    