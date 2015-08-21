# -*- coding: cp936 -*-
import os
import shutil
import struct
import common_functions

class IllustDataGenerator(object):
    def __init__(self):
        return
         
    def makeJunctionViewIllust(self, srcDir, destDir):
        if os.path.isdir(srcDir) == False:
            print "input directory not exist! " + srcDir
            return
        if(os.path.exists(destDir) == True):
            shutil.rmtree(destDir)
        os.mkdir(destDir)
        
        # 遍历元数据文件夹，列出四维提供的所有.png图片
        totalPngInDisk = []
        for curDir,dirNames,fileNames in os.walk(srcDir):
            for oneFile in fileNames:
                if(os.path.splitext(oneFile)[1].lower() == '.png'):
                    totalPngInDisk.append(os.path.join(curDir, oneFile))
        
        for onePng in totalPngInDisk:
            oneFile = common_functions.datSegmentInfo()
            oneFile.datInfo = common_functions.DAY_AND_NIGHT_COMMON # 中国仕向地的图片不区分白天黑夜
            oneFile.imgPath = onePng
            common_functions.ComposePicsToDat([oneFile], destDir, os.path.splitext(os.path.split(onePng)[1])[0])

    # pointlist相关
    def pointListDatSpec(self, arrowPicPath, ptListPath, destDir):
        if os.path.isdir(arrowPicPath) == False:
            print "can not find directory: %s." % arrowPicPath
            return
        if os.path.isdir(ptListPath) == False:
            print "can not find directory: %s." % ptListPath
            return
        if(os.path.exists(destDir) == True):
            shutil.rmtree(destDir)
        os.mkdir(destDir)
        
        arrowPicList = []
        for curDir, subDirs, fileNames in os.walk(arrowPicPath):
            if curDir.lower().find("arrow") != -1:
                for oneFile in fileNames:
                    arrowPicList.append(os.path.join(curDir, oneFile))
                    
        ptListFileList = []
        for curDir, subDirs, fileNames in os.walk(ptListPath):
            if curDir.lower().find("pointlist") != -1:
                for oneFile in fileNames:
                    ptListFileList.append(os.path.join(curDir, oneFile))
        
        for oneArrow in arrowPicList:
            for onePtListFile in ptListFileList:
                name1 = os.path.split(oneArrow)[1][:-4]
                name2 = os.path.split(onePtListFile)[1][:-8]
                if name1 == name2: # 此箭头图找到一份属于它的pointlist。
                    resultBuffer = ''
                    with open(onePtListFile, 'r') as ptListIFStream: 
                        line = ptListIFStream.readline().strip()
                        if not line: # 读取pointlist失败，跳过。
                            break
                        lineSplit = line.split('\t')
                        ptListRes = struct.pack("hh", 300, (len(lineSplit)-1)/2)
                        for i in range(1, len(lineSplit)):
                            ptListRes = ptListRes + struct.pack("h", int(lineSplit[i]))
                           
                    arrowPicLen = os.path.getsize(oneArrow)
                    ptListLen = len(ptListRes)           
                    resultBuffer = struct.pack("<HHbiibii", 0xFEFE, 2, common_functions.DAY_AND_NIGHT_COMMON, 22, \
                                               arrowPicLen, common_functions.ARROW_WITH_POINT_LIST, 22 + arrowPicLen, \
                                               ptListLen)
                    with open(oneArrow, 'rb') as arrowIFStream:
                        resultBuffer = resultBuffer + arrowIFStream.read() + ptListRes
                    
                    with open(os.path.join(destDir, name1+'.dat'), 'wb') as oFStream:
                        oFStream.write(resultBuffer)
                    break # 此箭头图已找到一份属于它的pointlist，结束循环。
        return         
            
if __name__ == '__main__':
    datMaker = IllustDataGenerator()
    datMaker.makeJunctionViewIllust(r"C:\Users\hexin\Desktop\20150812_ni_14Winter_pic\000.2Mainland\Mainland\3D",
                                    r"C:\Users\hexin\Desktop\20150812_ni_14Winter_pic\000.2Mainland\Mainland\3D_dat")
#    datMaker.pointListDatSpec(r"C:\Users\hexin\Desktop\20150812_ni_14Winter_pic\000.2Mainland_out\Mainland\3D\arrow",
#                              r"C:\Users\hexin\Desktop\20150812_ni_14Winter_pic\000.2Mainland_out\Mainland\3D\pointlist",
#                              r"C:\my\20150820_pointlist_dat")



















