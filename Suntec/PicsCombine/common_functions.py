#encoding=utf-8
import os
import struct

DAY_AND_NIGHT_COMMON = 0
DAY_PATTERN = 1
NIGHT_PATTERN = 2
EVENING_PATTERN = 3
ARROW_WITH_POINT_LIST = 64

# dat具体格式请查看RDB_Format_NewForNavInfo_size_reduce.xls，“图片buffer数据结构”表
# 此类描述了将压制入dat的图片/二进制数据的详细信息。
class datSegmentInfo(object):
    def __init__(self):
        self.datInfo = DAY_AND_NIGHT_COMMON # 此文件的信息
        self.imgPath = '' # 要压缩的数据
        return

# 将fileList合并成一个.dat文件并输出至destDir
# *注意*fileList中的每个元素都必须是一个datSegmentInfo对象。
def ComposePicsToDat(fileList, destDir, outputDatName): 
    if os.path.isdir(destDir) == False:
        os.mkdir(destDir)
    
    imageCount = len(fileList)
    resultBuffer = struct.pack("<HH", 0xFEFE, imageCount)
    
    # 制作dat头信息。
    fileLenList = []
    for oneFile in fileList:
        imgLen = os.path.getsize(oneFile.imgPath)
        resultBuffer += struct.pack("<bii", oneFile.datInfo, 4+imageCount*9+sum(fileLenList), imgLen)
        fileLenList.append(imgLen)
    
    # 将每个文件读入dat的数据存放块。 
    for oneFile in fileList:
        with open(oneFile.imgPath, 'rb') as iFStream:
            resultBuffer += iFStream.read()
            print oneFile.imgPath + '\n'

    destFile = os.path.join(destDir, outputDatName + '.dat')
    with open(destFile, 'wb') as oFStream:
        oFStream.write(resultBuffer)
    print "    ==> " + destFile
    return
    

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
