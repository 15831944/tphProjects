# -*- coding: cp936 -*-
'''
Created on 2015-5-6
@author: tangpinghui
'''
import os
import common_functions

# ��gjv.csv�ļ��н��������е�gjv_xxxxx��svg ���� Ԫ�����е�svg�б���бȽϣ��г�δ�ҵ�����Ŀ��
def CheckAllGjvSvgExist(gjvCsvFile, srcGjvSvgDir):
    if(os.path.isfile(gjvCsvFile) == False):
        print "source file does not exist: " + gjvCsvFile
        return
    
    # �ҳ�csv���¼��gjv��
    gjvRecordList = common_functions.analyse_csv(gjvCsvFile)
    gjvNameList = []
    for oneRecord in gjvRecordList:
        if(oneRecord['filename'] != ''):
            gjvNameList.append('gjv_' + oneRecord['filename'].lower())
    fixedGjvNameList = list(set(gjvNameList)) #ȥ��
    
    # ��ͼƬԪ�������г�����gjv�ļ���    
    gjvFileList = []
    for curDir,dirNames,fileNames in os.walk(srcGjvSvgDir):
        for oneFile in fileNames:
            if oneFile[-4:] == ".svg":
                gjvFileList.append(oneFile.lower())
  
    # �Ƚ��������飬���г�δ�ҵ�����Ŀ 
    notFoundGjvList = []
    for oneGjvName in fixedGjvNameList:
        if oneGjvName not in gjvFileList:
            notFoundGjvList.append(oneGjvName)
    return notFoundGjvList

# ��ejv.csv�ļ��н��������е�ejv_xxxxx��svg ���� Ԫ�����е�svg�б���бȽϣ��г�δ�ҵ�����Ŀ��
def CheckAllEjvSvgExist(ejvCsvFile, srcEjvSvgDir):
    if(os.path.isfile(ejvCsvFile) == False):
        print "source file does not exist: " + ejvCsvFile
        return
    
    # �ҳ�csv���¼��ejv��
    ejvRecordList = common_functions.analyse_csv(ejvCsvFile)
    ejvNameList = []
    for oneRecord in ejvRecordList:
        if(oneRecord['ejv_filename'] != ''):
            ejvNameList.append(oneRecord['ejv_filename'].lower())
    fixedEjvNameList = list(set(ejvNameList)) #ȥ��
    
    # ��ͼƬԪ�������г�����ejv�ļ���
    ejvFileList = []
    for curDir,dirNames,fileNames in os.walk(srcEjvSvgDir):
        for oneFile in fileNames:
            if oneFile[-4:] == ".svg":
                ejvFileList.append(oneFile.lower())
    
    # �Ƚ��������飬���г�δ�ҵ�����Ŀ
    notFoundEjvList = []
    for oneEjvName in fixedEjvNameList:
        if oneEjvName not in ejvFileList:
            notFoundEjvList.append(oneEjvName)
    return notFoundEjvList

# ��ejv.csv�ļ��н��������е�sar_xxxxx��svg ���� Ԫ�����е�svg�б���бȽϣ��г�δ�ҵ�����Ŀ��
def CheckAllSarSvgExist(ejvCsvFile, srcSarSvgDir):
    if(os.path.isfile(ejvCsvFile) == False):
        print "source file does not exist: " + ejvCsvFile
        return
    
    # �ҳ�csv���¼��sar��
    ejvRecordList = common_functions.analyse_csv(ejvCsvFile)
    sarNameList = []
    for oneRecord in ejvRecordList:
        if(oneRecord['sar_filename'] != ''):
            sarNameList.append(oneRecord['sar_filename'])
    fixedEjvNameList = list(set(sarNameList)) #ȥ��
    
    # ��ͼƬԪ�������г�����sar�ļ���
    sarFileList = []
    for curDir,dirNames,fileNames in os.walk(srcSarSvgDir):
        for oneFile in fileNames:
            if oneFile[-4:] == ".svg":
                sarFileList.append(oneFile)
    
    # �Ƚ��������飬���г�δ�ҵ�����Ŀ
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











