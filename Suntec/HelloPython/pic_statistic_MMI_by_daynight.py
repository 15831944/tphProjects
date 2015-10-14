import os
import shutil

# 将印度的图片从原路径按照day/night关键字分别拷入不同的文件夹。
# 以便统计day和night的大小分别是多少。
if __name__ == '__main__':
    srcDir = r"C:\My\20150410_mmi_pic\Pattern_withsar"
    destDir = r"c:\my\0818"
    if os.path.isdir(destDir) == True:
        shutil.rmtree(destDir)
    os.mkdir(destDir)
    os.mkdir(os.path.join(destDir, 'day'))
    os.mkdir(os.path.join(destDir, 'night'))
    
    for curDir, subDirs, fileNames in os.walk(srcDir):
        for oneFile in fileNames:
            if oneFile.find("_xasdfaseeqweradf_") != -1:
                if oneFile[-5] == '1':
                    destFile = os.path.join(destDir, 'day', oneFile)
                    shutil.copyfile(os.path.join(curDir, oneFile),
                                    destFile)
                elif oneFile[-5] == '2':
                    shutil.copyfile(os.path.join(curDir, oneFile),
                                    os.path.join(destDir, 'night', oneFile))
                    
    pass