#encoding=utf-8
import os.path
import struct
import sys
import shutil

#==============================================================================
# comp_picture
#==============================================================================
class GeneratorPicBinary_Here(object):

    def __init__(self):
        return
    
    def __del__(self):
        return

    def makeJunctionResultTable(self, srcDir, destDir):
        #如果源文件夹不存在，报错,退出
        if os.path.isdir(srcDir) == False:
            print "source directory not found: " + srcDir
            print "process end."
            return
        if(os.path.exists(destDir) == True):
            shutil.rmtree(destDir)
        os.mkdir(destDir)

        dayPicList = []
        arrowFileList = []
        fileList = os.listdir(os.path.join(srcDir, "DAY")) #DAY文件夹下会提供.jpg背景图片和。png的箭头图片
        for fileIter in fileList:
            picNameSplit = os.path.splitext(fileIter)
            if(picNameSplit[1] == ".jpg"):
                dayPicList.append(fileIter) # .jpg后缀的是背景图
            elif(picNameSplit[1] == ".png"):
                arrowFileList.append(fileIter) # 。png后缀的是箭头图
            else:
                continue
            
        nightFileList = []
        fileList = []
        fileList = os.listdir(os.path.join(srcDir, "NIGHT"))
        for fileIter in fileList:
            picNameSplit = os.path.splitext(fileIter)
            if(picNameSplit[1] == ".jpg"):
                nightFileList.append(fileIter)
            else:
                continue
            
        for dayPic in dayPicList:
            try:
                nightFileList.index(dayPic) # DAY和NIGHT下的文件名字必定相同，因此要求必须能在NIGHT文件夹下找到对应的dayFile，否则报错
            except:
                print "    night file not found: " + os.path.join(srcDir, "NIGHT", picNameSplit[0] + ".jpg")
                continue
            else:
                print "--------------------------------------------------------------------------------------"
                print "processing " + dayPic

                # day and night illust
                dayPicFile = os.path.join(srcDir, "DAY", dayPic)            
                nightPicFile = os.path.join(srcDir, "NIGHT", dayPic)
                dayFStream = open(dayPicFile, 'rb')
                nightFStream = open(nightPicFile, 'rb')
                dayPicLen = os.path.getsize(dayPicFile)
                nightPicLen = os.path.getsize(nightPicFile)
                b_headerBuffer = struct.pack("<HHbiibii", 0xFEFE, 2, 1, 22, dayPicLen, 2, 22 + dayPicLen, nightPicLen)
                b_resultBuffer = b_headerBuffer + dayFStream.read() + nightFStream.read()
                dayFStream.close()
                nightFStream.close()
                
                destFile = os.path.join(destDir, dayPic.replace(".jpg", ".dat"))
                destFStream = open(destFile, 'wb')
                destFStream.write(b_resultBuffer)
                destFStream.close()
                
                print "    " + dayPicFile
                print "    " + nightPicFile
                print "        >>>>>>>>  " + destFile
        print "----------------------------------------------------------------------------------------------------"
        for arrowFile in arrowFileList:
            # 处理arrow图片
            arrowPicFile = os.path.join(srcDir, "DAY", arrowFile)
            arrowFStream = open(arrowPicFile, 'rb')
            arrowPicLen = os.path.getsize(arrowPicFile)
            a_headerBuffer = struct.pack("<HHbii", 0xFEFE, 1, 0, 13, arrowPicLen)
            a_resultBuffer = a_headerBuffer + arrowFStream.read()
            
            destArrowFile = os.path.splitext(arrowFile)[0]
            destArrowFile = destArrowFile[:len(destArrowFile)-2]
            destArrowFile = os.path.join(destDir, destArrowFile+".dat")
            destFStream = open(destArrowFile, 'wb')
            arrowFStream.close()
            destFStream.write(a_resultBuffer)
            destFStream.close()
            print "    " + arrowPicFile
            print "        >>>>>>>>  " + destArrowFile
            print "--------------------------------------------------------------------------------------"
        
        print "----------------------------------------------------------------------------------------------------"
        print "process end"
        return

def main():
    arglen = len(sys.argv)
    if(arglen <= 2):
        return
    else:
        ejvsrcdir = ""
        ejvdestdir = ""
        for i in range(1, arglen):
            arg = sys.argv[i]
            if(arg == "/srcdir"):
                ejvsrcdir = str(sys.argv[i+1])
            elif(arg == "/destdir"):
                ejvdestdir = str(sys.argv[i+1])
            else:
                pass
        test = GeneratorPicBinary_Here()
        test.makeJunctionResultTable(ejvsrcdir, ejvdestdir)
    return    

if __name__ == '__main__':
    main()