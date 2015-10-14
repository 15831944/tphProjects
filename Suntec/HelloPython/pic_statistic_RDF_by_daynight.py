#encoding=utf-8
import os
import shutil

# 将RDF的图片从原路径按照day/night关键字分别拷入不同的文件夹。
# 以便统计day和night的大小分别是多少。
if __name__ == '__main__':
    srcDir = r"C:\My\20150514_mea_2014Q4_pic\2DJ_2014Q4_MEA_svgout_withsar"
    destDir = r"C:\Users\hexin\Desktop\20150818_size\mea8"
    if os.path.isdir(destDir) == True:
        shutil.rmtree(destDir)
    os.mkdir(destDir)
    os.mkdir(os.path.join(destDir, 'day'))
    os.mkdir(os.path.join(destDir, 'night'))
    
    for curDir, subDirs, fileNames in os.walk(srcDir):
        if curDir.lower().find("day") != -1:
            for oneFile in fileNames:
                destFile = os.path.join(destDir, 'day', oneFile)
                shutil.copyfile(os.path.join(curDir, oneFile),
                                destFile)
        elif curDir.lower().find("night") != -1:
            for oneFile in fileNames:
                destFile = os.path.join(destDir, 'night', oneFile)
                shutil.copyfile(os.path.join(curDir, oneFile),
                                destFile)
                    
    pass