#encoding=utf-8
import os

if __name__ == '__main__':
    
    strDirOriginal = r"C:\My\illust_all\illust_20151027_15summer_NI\Mainland"
    strDirStep1 = r"C:\My\illust_all\illust_20151027_15summer_NI\Mainland_step1_png_2_jpg"
    strDirStep2 = r"C:\My\illust_all\illust_20151027_15summer_NI\Mainland_step2_compress_pattern"
    for curDir, subDirs, fileNames in os.walk(strDirOriginal):
        totalSize = 0
        for oneFile in fileNames:
            totalSize += os.path.getsize(os.path.join(curDir, oneFile))
        print "%s: %d" % (curDir, totalSize)

    print "------------------------------------------------------------------------"
    for curDir, subDirs, fileNames in os.walk(strDirStep1):
        totalSize = 0
        for oneFile in fileNames:
            totalSize += os.path.getsize(os.path.join(curDir, oneFile))
        print "%s: %d" % (curDir, totalSize)
        
    print "------------------------------------------------------------------------"
    for curDir, subDirs, fileNames in os.walk(strDirStep2):
        totalSize = 0
        for oneFile in fileNames:
            totalSize += os.path.getsize(os.path.join(curDir, oneFile))
        print "%s: %d" % (curDir, totalSize)






