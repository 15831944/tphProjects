# -*- coding: cp936 -*-
import os
import psycopg2
import struct
import shutil

# ����ά�ṩ��Ԫ�����ÿ�ַ���ͼ����Ӧ���ļ�������
typeAndNameDict = {'1':"2D(�б���)",
                   '2':"HEG",
                   '3':"HEG",
                   '4':"CRPG(�б���)",
                   '5':"CRCG",
                   '7':"3D(�б���)"}

# imgList�ϲ���һ��.dat�ļ��������destDir
# .dat �ĵ���������
# .dat �ļ���ʽ���壺
# |         |              |4bit����4bit�ڰױ�ʶ|               |             |   |
# |2B:0xFEFE|2B:image count|1B:image info   |4B:image offset|4B:image size|...|
def ComposePicsToDat(imgList, destDir, datInfo=1, outputDatName=''): 
    print "--------------------------------------------------------------------------------------" 
    if os.path.isdir(destDir) == False:
        os.mkdir(destDir)
    destFile = ''
    if(outputDatName == ''):
        # imgList�������ͼƬ����������Ӧ����ͬ�����Կ���ʹ�õ�һ������������dat�ĵ���
        destFile = os.path.join(destDir, os.path.splitext(imgList[0])[0] + '.dat')
    else:
        # һЩ�������֮�£���Ҫ��dat�ĵ���������������������Ӻ�׺�ȵȵȵȣ�����������outputDatName�ṩ��
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
        
        # ����Ԫ�����ļ��У��г���ά�ṩ�����С�pngͼƬ
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
            # ���ݸ��ֳ������ƣ������type����������ֵ�������.png�б���ȷ����������������patternͼƬ��arrowͼƬ��
            for onePng in totalPngInDisk:
                # ���Ǳ����У�ֱ��pass
                if(onePng.find(folder) == -1):
                    continue
                # ���Ǳ�type��ֱ��pass
                if(onePng.find(typeAndNameDict[jv_type]) == -1):
                    continue
                
                if(onePng.find('pattern') != -1 and onePng.find(patternno) != -1):
                    #�����ҵ� patternͼƬ��
                    bPatternIsFound = True
                    patternPngList.append(onePng)
                    
                if(onePng.find('arrow') != -1 and onePng.find(arrowno) != -1):
                    #�����ҵ� arrowͼƬ��
                    bArrowIsFound = True
                    arrowPngList.append(onePng)
            
            # ��patternͼƬ��arrowͼƬ����ͬʱ�ҵ�ʱ������dat   
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