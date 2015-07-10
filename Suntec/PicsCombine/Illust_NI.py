# -*- coding: cp936 -*-
import os
import shutil
import psycopg2
import common_functions

# 在四维提供的元数据里，每种分歧图所对应的文件夹名字
typeAndNameDict = {'1':"2D(有背景)",
                   '2':"HEG",
                   '3':"HEG",
                   '4':"CRPG(有背景)",
                   '5':"CRCG",
                   '7':"3D(有背景)"}

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
        
        # 遍历元数据文件夹，列出四维提供的所有。png图片
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
            # 根据各种城市名称，分歧点type，分歧点名字 ，patternno，arrowno等条件从.png列表里确定满足所有条件的pattern图片和arrow图片。
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
        
        # 遍历元数据文件夹，列出四维提供的所有和signboard有关系的。png图片
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
            # 根据各种城市名称，分歧点type，分歧点名字 ，patternno，arrowno等条件从.png列表里确定满足所有条件的pattern图片和arrow图片。
            for onePng in totalPngInDisk:
                # 不是本城市，直接pass
                if(onePng.find(folder) == -1):
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