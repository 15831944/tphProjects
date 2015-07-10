# -*- coding: cp936 -*-
'''
Created on 2015-5-6
@author: tangpinghui
'''
import os
import common_functions

# 从gjv.csv文件中解析出所有的gjv_xxxxx。svg 并与 元数据中的svg列表进行比较，列出未找到的项目。
def CheckAllGjvSvgExist(gjvCsvFile, srcGjvSvgDir):
    if(os.path.isfile(gjvCsvFile) == False):
        print "source file does not exist: " + gjvCsvFile
        return
    
    # 找出csv里记录的gjv名
    gjvRecordList = common_functions.analyse_csv(gjvCsvFile)
    gjvNameList = []
    for oneRecord in gjvRecordList:
        if(oneRecord['filename'] != ''):
            gjvNameList.append('gjv_' + oneRecord['filename'].lower())
    fixedGjvNameList = list(set(gjvNameList)) #去重
    
    # 从图片元数据中列出所有gjv文件名    
    gjvFileList = []
    for curDir,dirNames,fileNames in os.walk(srcGjvSvgDir):
        for oneFile in fileNames:
            if oneFile[-4:] == ".svg":
                gjvFileList.append(oneFile.lower())
  
    # 比较两个数组，并列出未找到的项目 
    notFoundGjvList = []
    for oneGjvName in fixedGjvNameList:
        if oneGjvName not in gjvFileList:
            notFoundGjvList.append(oneGjvName)
    return notFoundGjvList

# 从ejv.csv文件中解析出所有的ejv_xxxxx。svg 并与 元数据中的svg列表进行比较，列出未找到的项目。
def CheckAllEjvSvgExist(ejvCsvFile, srcEjvSvgDir):
    if(os.path.isfile(ejvCsvFile) == False):
        print "source file does not exist: " + ejvCsvFile
        return
    
    # 找出csv里记录的ejv名
    ejvRecordList = common_functions.analyse_csv(ejvCsvFile)
    ejvNameList = []
    for oneRecord in ejvRecordList:
        if(oneRecord['ejv_filename'] != ''):
            ejvNameList.append(oneRecord['ejv_filename'].lower())
    fixedEjvNameList = list(set(ejvNameList)) #去重
    
    # 从图片元数据中列出所有ejv文件名
    ejvFileList = []
    for curDir,dirNames,fileNames in os.walk(srcEjvSvgDir):
        for oneFile in fileNames:
            if oneFile[-4:] == ".svg":
                ejvFileList.append(oneFile.lower())
    
    # 比较两个数组，并列出未找到的项目
    notFoundEjvList = []
    for oneEjvName in fixedEjvNameList:
        if oneEjvName not in ejvFileList:
            notFoundEjvList.append(oneEjvName)
    return notFoundEjvList

# 从ejv.csv文件中解析出所有的sar_xxxxx。svg 并与 元数据中的svg列表进行比较，列出未找到的项目。
def CheckAllSarSvgExist(ejvCsvFile, srcSarSvgDir):
    if(os.path.isfile(ejvCsvFile) == False):
        print "source file does not exist: " + ejvCsvFile
        return
    
    # 找出csv里记录的sar名
    ejvRecordList = common_functions.analyse_csv(ejvCsvFile)
    sarNameList = []
    for oneRecord in ejvRecordList:
        if(oneRecord['sar_filename'] != ''):
            sarNameList.append(oneRecord['sar_filename'])
    fixedEjvNameList = list(set(sarNameList)) #去重
    
    # 从图片元数据中列出所有sar文件名
    sarFileList = []
    for curDir,dirNames,fileNames in os.walk(srcSarSvgDir):
        for oneFile in fileNames:
            if oneFile[-4:] == ".svg":
                sarFileList.append(oneFile)
    
    # 比较两个数组，并列出未找到的项目
    notFoundSarList = []
    for oneSarName in fixedEjvNameList:
        if oneSarName not in sarFileList:
            notFoundSarList.append(oneSarName)
    return notFoundSarList



def main():
    notFoundGjvList = CheckAllGjvSvgExist(r"C:\My\20150514_mea_pic_2014Q4\ori_data\VMAM14400WVM000DGNJV\Q414_MEA_2D_Generalized_Junctions_2D_Generalized_Signs_LAT.csv",
                                          r"C:\My\20150514_mea_pic_2014Q4\ori_data\2DGJ_2014Q4_MEA")
    notFoundEjvList = CheckAllEjvSvgExist(r"C:\My\20150514_mea_pic_2014Q4\ori_data\VMAM14400WVM000DDK00\Q414_MEA_2DJV_lat.csv",
                                          r"C:\My\20150514_mea_pic_2014Q4\ori_data\2DJ_2014Q4_MEA")
    notFoundSarList = CheckAllSarSvgExist(r"C:\My\20150514_mea_pic_2014Q4\ori_data\VMAM14400WVM000DDK00\Q414_MEA_2DJV_lat.csv",
                                          r"C:\My\20150514_mea_pic_2014Q4\ori_data\2DS_2014Q4_MEA")
    return
if __name__ == '__main__':
    main()











