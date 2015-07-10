#encoding=utf-8
import os
import struct

# 将csv文件的列名作为键，将每行内容作为值。
# 每行数据将作成一个字典，返回字典的列表
# srcCsvFile -> 要解析的csv文件路径 
# 出错时返空列表
def analyse_csv(srcCsvFile):
    if os.path.exists(srcCsvFile) == False:
        return []

    inFStream = open(srcCsvFile)
    listline = inFStream.readlines()
    inFStream.close()
    
    #解析csv表头，用作字典的键
    csvHeader = listline[0].lower().split(",")
    csvDataList = []
    for line in listline[1:]:
        line = line.strip()
        if line:
            #解析出每行内容
            listrow = line.split(",")
            #以列名为键，对应列的内容为值生成键值对
            record = dict(map(lambda x,y:[x,y], csvHeader,listrow))
            csvDataList.append(record)
    return csvDataList

# imgList合并成一个.dat文件并输出至destDir
# .dat 文件格式定义：
# |         |              |4bit语言4bit黑白标识|               |             |   |
# |2B:0xFEFE|2B:image count|1B:image info    |4B:image offset|4B:image size|...|
def ComposePicsToDat(imgList, destDir, datInfo=1, outputDatName=''): 
    if os.path.isdir(destDir) == False:
        os.mkdir(destDir)
    destFile = ''
    if(outputDatName == ''):
        # imgList里的所有图片理论上名字应该相同，所以可以使用第一张名字来命名dat文档。
        imgName = os.path.split(imgList[0])[1]
        destFile = os.path.join(destDir, os.path.splitext(imgName)[0] + '.dat')
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
    print "    ==> " + destFile
    return
    
    