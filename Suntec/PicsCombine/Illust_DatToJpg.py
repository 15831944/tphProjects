# -*- coding: cp936 -*-

import os
import shutil
import struct
import imghdr

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
            if os.path.splitext(oneFile)[1] == ".dat" or os.path.splitext(oneFile)[1] == "":
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
        
        bufIPic = readBuf[iOffset: iOffset+iSize]    
        strExt = ".jpg"
        if(imghdr.test_png(bufIPic, None)):
            strExt = ".png"
        
        newJpgName = '''%s_%d%s''' % (os.path.splitext(os.path.split(srcDatFile)[1])[0], iPic, strExt)
        destFile = os.path.join(destDir, newJpgName) 
        outFS = open(destFile, 'wb')
        outFS.write(bufIPic)
        outFS.close()
        
if __name__ == '__main__':
    dat = r"\\idata\Private\Data_Authoring\SourceData\AP_rdf\ASE\15Q1\illust\binary\2DIllust"
    pic = r"C:\My\20150610__ase15Q1_pic_check\dat_to_jpg"
    ConvertDatToImage_Dir(dat, pic)
    