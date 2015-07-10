# -*- coding: cp936 -*-
import os
import psycopg2
import struct
import shutil

# 在四维提供的元数据里，每种分歧图所对应的文件夹名字
typeAndNameDict = {'1':"2D(有背景)",
                   '2':"HEG",
                   '3':"HEG",
                   '4':"CRPG(有背景)",
                   '5':"CRCG",
                   '7':"3D(有背景)"}

# imgList合并成一个.dat文件并输出至destDir
# .dat 文档命名规则：
# .dat 文件格式定义：
# |         |              |4bit语言4bit黑白标识|               |             |   |
# |2B:0xFEFE|2B:image count|1B:image info   |4B:image offset|4B:image size|...|
def ComposePicsToDat(imgList, destDir, datInfo=1, outputDatName=''): 
    print "--------------------------------------------------------------------------------------" 
    if os.path.isdir(destDir) == False:
        os.mkdir(destDir)
    destFile = ''
    if(outputDatName == ''):
        # imgList里的所有图片理论上名字应该相同，所以可以使用第一张名字来命名dat文档。
        destFile = os.path.join(destDir, os.path.splitext(imgList[0])[0] + '.dat')
    else:
        # 一些特殊情况之下，需要给dat文档进行特殊命名，例如添加后缀等等等等，特殊命名由outputDatName提供。
        destFile = os.path.join(destDir, outputDatName + '.dat')
    
    imageCount = len(imgList)
    resultBuffer = struct.pack("<HH", 0xFEFE, imageCount)
    imageLens = []
    for img in imgList:
        imgLen = os.path.getsize(img)
        print img
        resultBuffer += struct.pack("<bii", datInfo, 4+imageCount*9+sum(imageLens), imgLen)
        imageLens.append(imgLen)
        
    for img in imgList:
        tempFS = open(img, 'rb')
        resultBuffer += tempFS.read()
        tempFS.close()
       
    outFS = open(destFile, 'wb')
    outFS.write(resultBuffer)
    outFS.close()
    print "    >>>>>>>>  " + destFile

class GenerateIllustData(object):
    def __init__(self):
        self.conn = psycopg2.connect(''' host='172.26.179.190' dbname='C_NIMIF_Sample_TMP'
                                         user='postgres' password='pset123456' ''')
        self.pgcur2 = self.conn.cursor()
         
    def makeJunctionViewIllust(self, srcDir, destDir):
        if os.path.isdir(srcDir) == False:
            print "input directory not exist! " + srcDir
            return
        if(os.path.exists(destDir) == True):
            shutil.rmtree(destDir)
        os.mkdir(destDir)
        
        # 遍历元数据文件夹，列出四维提供的所有。png图片
        totalPngInDisk = []
        for curDir,dirNames,fileNames in os.walk(srcDir):
            for oneFile in fileNames:
                if(os.path.splitext(oneFile)[1].lower() == 'png'):
                    totalPngInDisk.append(os.path.join(curDir, oneFile))
        
        self.pgcur2.execute('''select patternno, arrowno, "type", folder from org_br''')
        rows = self.pgcur2.fetchall()
        for row in rows:
            patternno = row[0]
            arrowno = row[1]
            jv_type = row[2]
            folder = row[3]
            
            bPatternIsFound = False
            patternPngList = []
            arrowPngList = []
            # 根据各种城市名称，分歧点type，分歧点名字等条件从.png列表里确定满足所有条件的pattern图片和arrow图片。
            for onePng in totalPngInDisk:
                # 不是本城市，直接pass
                if(onePng.find(folder) == -1):
                    continue
                # 不是本type，直接pass
                if(onePng.find(typeAndNameDict[jv_type]) == -1):
                    continue
                
                if(onePng.find('pattern') != -1 and onePng.find(patternno) != -1):
                    #终于找到 pattern图片了
                    bPatternIsFound = True
                    patternPngList.append(onePng)
                    
                if(onePng.find('arrow') != -1 and onePng.find(arrowno) != -1):
                    #终于找到 arrow图片了
                    bArrowIsFound = True
                    arrowPngList.append(onePng)
            
            # 当pattern图片和arrow图片都能同时找到时，才做dat   
            if bPatternIsFound and bArrowIsFound:
                # 
                ComposePicsToDat(patternPngList, destDir, 1, patternno+'_'+folder)
                ComposePicsToDat(arrowPngList, destDir, 1, arrowno+'_'+folder)
                
            if(bPatternIsFound == False):
                print '''pattern picture cannot found: %s\%s\%s''' % (folder,typeAndNameDict[jv_type],patternno)
            if(bArrowIsFound == False):
                print '''arrow picture cannot found: %s\%s\%s''' % (folder,typeAndNameDict[jv_type],arrowno)



                  
            
if __name__ == '__main__':
    print 'hello world!'
    return