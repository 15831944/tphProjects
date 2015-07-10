# -*- coding: cp936 -*-
import os
import struct

# 在srcDir里找到fileNames里列出的所有图片名字
# 未能找到的图片输出至console窗口
def Function1(srcDir, fileNameList):
    if(os.path.exists(srcDir) == False):
        print "source directory does not exist: " + srcDir
        return
    
    totalFileList = []
    for curDir,dirNames,fileNames in os.walk(srcDir):
        for oneFile in fileNames:
                totalFileList.append(os.path.join(curDir, oneFile))
    
    sarCount = len(totalFileList)
    oFStream = open("C:\\My\\20150409_mea_pic\\illust_map_withsar\\sign_as_real.csv", "w")
    for fileInDisk in totalFileList:
        for fileWantToFind in fileNameList:
            if fileInDisk.find(fileWantToFind) > -1:
                print fileWantToFind + " has been found."
                oFStream.write(fileWantToFind + "," + fileWantToFind[6:] + "\n") # 写入到csv里，供signpost使用
                do_combination([fileInDisk], "C:\\My\\20150409_mea_pic\\illust_map_withsar\\pic\\sign")
    oFStream.close()   

    # 将srcDir里的imgList合并成一个.dat文件并输出至destDir
    # 输出文件名字使用get_output_image_name获得
    # .dat 文件格式定义：
    # |         |              |             |               |             |   |
    # |2B:0xFEFE|2B:image count|1B:image info|4B:image offset|4B:image size|...|
def do_combination(imgList, destDir): 
    print "--------------------------------------------------------------------------------------" 
    if os.path.isdir(destDir) == False:
        os.mkdir(destDir)
    xxxxxxx = os.path.split(imgList[0])[1]
    xxxxxxx = os.path.splitext(xxxxxxx)[0]
    destFile = os.path.join(destDir, xxxxxxx + ".dat")
    
    imageCount = len(imgList)
    resultBuffer = struct.pack("<HH", 0xFEFE, imageCount)

    imageLens = []
    for srcImg in imgList:
        imgLen = os.path.getsize(srcImg)
        resultBuffer += struct.pack("<bii", 1, 4+imageCount*9+sum(imageLens), imgLen)
        imageLens.append(imgLen)
        
    for srcImg in imgList:
        tempFS = open(srcImg, 'rb')
        resultBuffer += tempFS.read()
        print "    " + srcImg
        tempFS.close()
       
    outFS = open(destFile, 'wb')
    outFS.write(resultBuffer)
    outFS.close()
    print "    >>>>>>>>  " + destFile

fileNames = [ "SR_AE_544878351",  
            "SR_KW_999968836",  
            "SR_KW_550814145",  
            "SR_KW_1001756746",  
            "SR_MA_799416126_R",  
            "SR_MA_1052597578_N",  
            "SR_ZA_905989565",  
            "SR_ZA_905230750",  
            "SR_ZA_771350325",  
            "SR_ZA_867056645",  
            "SR_ZA_770586661",  
            "SR_ZA_770502432",  
            "SR_ZA_1071389196",  
            "SR_ZA_770548872_R",  
            "SR_ZA_807415890",  
            "SR_ZA_847503411",  
            "SR_ZA_860738524",  
            "SR_ZA_771412045",  
            "SR_AE_780464978",  
            "SR_AE_809462856",  
            "SR_IL_835255144_N",  
            "SR_ZA_1058315065",  
            "SR_IL_862445672",  
            "SR_IL_841208646",  
            "SR_ZA_1070227712",  
            "SR_ZA_1064251644_N",  
            "SR_IL_1074715735"]
if __name__ == '__main__':
    Function1('''C:\\My\\20150409_mea_pic\\2DS_2015Q1_MEA_OUTPUT''', fileNames)
    pass