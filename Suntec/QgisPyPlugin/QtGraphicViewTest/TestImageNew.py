import os
import Image
import sys
sys.path.append(r"C:\Users\hexin\.qgis2\python\plugins\RdbViewer")
import DatGetBinType

def main():
    srcDir = r"C:\Users\hexin\Desktop\20151218_jpg_png_get_size"
    errMsg = ['']
    for curDir, subDirs, fileNames in os.walk(srcDir):
        for oneFile in fileNames:
            imagePath = os.path.join(curDir, oneFile)
            imageType = DatGetBinType.GetBinaryDataType(errMsg, imagePath)
            if imageType == DatGetBinType.ImageType_Jpg or imageType == DatGetBinType.ImageType_Png:
                newImage = Image.open(imagePath)
                print imagePath
                print """correct size: %s, %s""" % (newImage.size[0], newImage.size[1])

                with open(imagePath, 'rb') as iFStream:
                    imageBuff = iFStream.read()
                    x,y = DatGetBinType.GetImageSize(imageBuff)
                    print """my size: %s, %s""" % (x, y)
                print """*********************************************"""
    return


if __name__ == '__main__':
    main()