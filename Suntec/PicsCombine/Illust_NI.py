# -*- coding: cp936 -*-
import os
import shutil
import psycopg2
import common_functions

# ����ά�ṩ��Ԫ�����ÿ�ַ���ͼ����Ӧ���ļ�������
typeAndNameDict = {'1':"2D(�б���)",
                   '2':"HEG",
                   '3':"HEG",
                   '4':"CRPG(�б���)",
                   '5':"CRCG",
                   '7':"3D(�б���)"}

class IllustDataGenerator(object):
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
                if(os.path.splitext(oneFile)[1].lower() == '.png'):
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
            # ���ݸ��ֳ������ƣ������type����������� ��patternno��arrowno��������.png�б���ȷ����������������patternͼƬ��arrowͼƬ��
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
                common_functions.ComposePicsToDat(patternPngList, destDir, 1, patternno+'_'+folder)
                common_functions.ComposePicsToDat(arrowPngList, destDir, 1, arrowno+'_'+folder)
                
            if(bPatternIsFound == False):
                print '''pattern picture cannot found: %s\%s\%s''' % (folder,typeAndNameDict[jv_type],patternno)
            if(bArrowIsFound == False):
                print '''arrow picture cannot found: %s\%s\%s''' % (folder,typeAndNameDict[jv_type],arrowno)

    def makeSignboardIllust(self, srcDir, destDir):
        if os.path.isdir(srcDir) == False:
            print "input directory not exist! " + srcDir
            return
        if(os.path.exists(destDir) == True):
            shutil.rmtree(destDir)
        os.mkdir(destDir)
        
        # ����Ԫ�����ļ��У��г���ά�ṩ�����к�signboard�й�ϵ�ġ�pngͼƬ
        totalPngInDisk = []
        for curDir,dirNames,fileNames in os.walk(srcDir):
            if curDir.lower().find("signboard") != -1:
                for oneFile in fileNames:
                    if(os.path.splitext(oneFile)[1].lower() == '.png'):
                        totalPngInDisk.append(os.path.join(curDir, oneFile))
        
        self.pgcur2.execute('''select patternno, arrowno, folder from org_signboard''')
        rows = self.pgcur2.fetchall()
        for row in rows:
            patternno = row[0]
            arrowno = row[1]
            folder = row[2]
            
            bPatternIsFound = False
            patternPngList = []
            arrowPngList = []
            # ���ݸ��ֳ������ƣ������type����������� ��patternno��arrowno��������.png�б���ȷ����������������patternͼƬ��arrowͼƬ��
            for onePng in totalPngInDisk:
                # ���Ǳ����У�ֱ��pass
                if(onePng.find(folder) == -1):
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
                common_functions.ComposePicsToDat(patternPngList, destDir, 1, patternno+'_'+folder)
                common_functions.ComposePicsToDat(arrowPngList, destDir, 1, arrowno+'_'+folder)
                
            if(bPatternIsFound == False):
                print '''pattern picture cannot found: %s\%s\%s''' % (folder, patternno)
            if(bArrowIsFound == False):
                print '''arrow picture cannot found: %s\%s\%s''' % (folder, arrowno)

                  
            
if __name__ == '__main__':
    datMaker = IllustDataGenerator()
    datMaker.makeJunctionViewIllust("C:\\My\\20150504-0511_ni_spotguide\\17CY Sample data_2\\data\\GraphAndVoice",
                                    "C:\\My\\20150504-0511_ni_spotguide\\road_dat_output")
    datMaker.makeSignboardIllust("C:\\My\\20150504-0511_ni_spotguide\\17CY Sample data_2\\data\\GraphAndVoice",
                                 "C:\\My\\20150504-0511_ni_spotguide\\signboard_dat_output")