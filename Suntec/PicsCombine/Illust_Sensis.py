#encoding=utf-8
import os.path
import struct
import shutil

class GeneratorPicBinary_Sensis(object):
    def __init__(self):
        return
    
    def combine_images(self, srcDir, destDir):
        if os.path.isdir(srcDir) == False:
            print """can not find src dir: %s""" % srcDir
            return
        if(os.path.exists(destDir) == True):
            shutil.rmtree(destDir)
        os.mkdir(destDir)
        
        for curDir,dirNames,fileNames in os.walk(srcDir):
            fileNames.sort(self.image_name_comparation)
            imgList = []
            for img in fileNames:
                if len(imgList) <= 0:
                    imgList.append(img)
                else:
                    if self.is_similar_images(imgList[0], img):
                        imgList.append(img)
                    elif len(imgList) >=1:
                        self.do_combination(srcDir, imgList, destDir)
                        imgList = []
                        imgList.append(img)
                    else:
                        imgList = []
                        imgList.append(img)
            if len(fileNames) >= 1:
                self.do_combination(srcDir, imgList, destDir)
        
        # 将srcDir里的imgList合并成一个.dat文件并输出至destDir
        # 输出文件名字使用get_output_image_name获得
        # .dat 文件格式定义：
        # |         |              |             |               |             |   |
        # |2B:0xFEFE|2B:image count|1B:image info|4B:image offset|4B:image size|...|
    def do_combination(self, srcDir, imgList, destDir): 
        print "--------------------------------------------------------------------------------------" 
        if os.path.isdir(destDir) == False:
            os.mkdir(destDir)
        destFile = os.path.join(destDir, self.get_output_image_name(imgList))
        
        imageCount = len(imgList)
        resultBuffer = struct.pack("<HH", 0xFEFE, imageCount)
    
        imageLens = []
        for img in imgList:
            srcImg = os.path.join(srcDir, img)
            imgLen = os.path.getsize(srcImg)
            print srcImg
            resultBuffer += struct.pack("<bii", self.get_image_information(img), \
                                        4+imageCount*9+sum(imageLens), imgLen)
            imageLens.append(imgLen)
            
        for img in imgList:
            tempFS = open(os.path.join(srcDir, img), 'rb')
            resultBuffer += tempFS.read()
            tempFS.close()
           
        outFS = open(destFile, 'wb')
        outFS.write(resultBuffer)
        outFS.close()
        print "    >>>>>>>>  " + destFile
           
        # 1. compare left part
        # 2. compare right part
        # 3. compare segment 3  
    def image_name_comparation(self, image1, image2):
        image1Split = image1.split('_')
        image2Split = image2.split('_')
        try:
            leftPart1 = '_'.join([image1Split[0], image1Split[1], image1Split[2]])
            leftPart2 = '_'.join([image2Split[0], image2Split[1], image2Split[2]])
        except: 
            print image1Split, image2Split
            return 
        if leftPart1 < leftPart2:
            return -1
        elif leftPart1 == leftPart2:
            rightPart1 = '_'.join([image1Split[4], image1Split[5]])
            rightPart2 = '_'.join([image2Split[4], image2Split[5]])
            if rightPart1 < rightPart2:
                return -1
            elif rightPart1 == rightPart2:
                midPart1 = '_'.join([image1Split[3]])
                midPart2 = '_'.join([image2Split[3]])
                if midPart1 < midPart2:
                    return -1
                else:
                    return 1
            else:
                return 1
        else:
            return 1 
    
        # 图片名字样例： mngdf_act_01_day_320x240_left.jpg
        # 图片分组要求: 除了第[3]个字段外其他字段均相同
        # 如  mngdf_act_01_day_320x240_left.jpg 
        # 和  mngdf_act_01_night_320x240_left.jpg 为同组图片
    def is_similar_images(self, image1, image2):
        image1Split = image1.split('_')
        image2Split = image2.split('_')
        if len(image1Split) != len(image2Split):
            return False
        if image1Split[0] != image2Split[0]: # "mngdf"
            return False
        if image1Split[1] != image2Split[1]: # "act"
            return False
        if image1Split[2] != image2Split[2]: # "01"
            return False
        if image1Split[3] == image2Split[3]: # "day"  <-->  "night"
            return False
        if image1Split[4] != image2Split[4]: # "320x240"
            return False
        if image1Split[5] != image2Split[5]: # "left.jpg"
            return False
        return True
    
        # 图片规格
        # bit7~4: 0:无语言, 1:英文, 2:泰文
        # bit3~0: 0:共用 , 1:白天, 2:黑夜, 3:傍晚
    def get_image_information(self, image):
        imageSplit = image.split('_')
        if imageSplit[3] == 'day':
            return 1
        elif imageSplit[3] == 'night':
            return 2
        else:
            return 0
        
        # 图片名字样例： mngdf_act_01_day_320x240_left.jpg
        # 由以上图片取的.dat文档名：mngdf_act_01_left.dat
        # 删除了字段  [0]:mngdf, [3]:day, [4]:320x240
    def get_output_image_name(self, images):
        image = images[0]
        imageSplit = image.split('_')
        if len(imageSplit) < 6:
            return ''
        del imageSplit[4]
        del imageSplit[3]
        del imageSplit[0]
        return '_'.join(imageSplit).replace(".jpg", ".dat")

if __name__ == '__main__':
    test = GeneratorPicBinary_Sensis()
    test.combine_images(r"C:\Users\hexin\Desktop\20150730_aus_pic_day_night\320x240", 
                        r"C:\Users\hexin\Desktop\20150730_aus_pic_day_night\320x240_dat")

































