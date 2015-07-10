#encoding=utf-8
import os
import shutil
import common_functions

class GeneratorPicBinary_Here(object):

    def __init__(self):
        return
    
    def __del__(self):
        return

    def makeEjvDat(self, srcDir, destDir):
        if os.path.isdir(srcDir) == False:
            print "source directory not found: " + srcDir
            return
        if(os.path.exists(destDir) == True):
            shutil.rmtree(destDir)
        os.mkdir(destDir)

        # 列出所有pattern图。
        # 由svg tool解压出来的图片文件夹结构最后一层应为DAY或NIGHT（在svg tool中选中制作黑白图）。
        # DAY包含白天的所有pattern图和arrow图。
        # NIGHT包含黑夜的所有pattern图和arrow图。
        # pattern图一般为.jpg（在svg tool中所设置），arrow图一般为.png（在svg tool中设置）。
        # 黑白pattern图有区别，黑白arrow图无区别。
        # 合并时将黑白pattern图合并成一个dat。
        # arrow图仅取白天的合并。
        # 完。
        dayPatternDict = {} # 白天pattern图列表。
        dayArrowDict = {} # 白天arrow图列表。
        nightPatternDict = {} # 黑夜pattern图列表。
        for curDir,dirNames,fileNames in os.walk(srcDir):
            if curDir.lower().find('driver_view'): # driver_view图片
                if curDir.lower().find('day') != -1: # 白天图
                    for oneFile in fileNames:
                        if oneFile[-4:] == ".jpg": # pattern图
                            dayPatternDict[oneFile] = os.path.join(curDir, oneFile).lower()
                        if oneFile[-4:] == ".png": # arrow图
                            dayArrowDict[oneFile] = os.path.join(curDir, oneFile).lower()
                if curDir.lower().find('night') != -1: # 黑夜图
                    for oneFile in fileNames:
                        if oneFile[-4:] == ".jpg": # pattern图
                            nightPatternDict[oneFile] = os.path.join(curDir, oneFile).lower()

            if curDir.lower().find('bird_view'): # bird_view图片
                if curDir.lower().find('day') != -1: # 白天图
                    for oneFile in fileNames:
                        if oneFile[-4:] == ".jpg": # pattern图
                            # 不存在同名的driver_view图，才使用bird_view图。
                            if dayPatternDict.get(oneFile) is None: 
                                dayPatternDict[oneFile] = os.path.join(curDir, oneFile).lower()
                        if oneFile[-4:] == ".png": # arrow图
                            # 不存在同名的driver_view图，才使用bird_view图。
                            if dayArrowDict.get(oneFile) is None:
                                dayArrowDict[oneFile] = os.path.join(curDir, oneFile).lower()
                if curDir.lower().find('night') != -1: # 黑夜图
                    for oneFile in fileNames:
                        if oneFile[-4:] == ".jpg": # pattern图
                            # 不存在同名的driver_view图，才使用bird_view图。
                            if nightPatternDict.get(oneFile) is None:
                                nightPatternDict[oneFile] = os.path.join(curDir, oneFile).lower()
        
        # 合并pattern图
        for dayPatternKey in dayPatternDict:
            if dayPatternKey in nightPatternDict:
                imageList = []
                imageList.append(dayPatternDict[dayPatternKey])
                imageList.append(nightPatternDict[dayPatternKey])
                dayPatternName = os.path.splitext(os.path.split(dayPatternDict[dayPatternKey])[1])[0]
                # dat命名式样：直接使用白天图名字命名dat
                common_functions.ComposePicsToDat(imageList, destDir, 1, dayPatternName)
                
        # 合并arrow图
        for dayArrowKey in dayArrowDict:
            dayArrowName = os.path.splitext(os.path.split(dayArrowDict[dayArrowKey])[1])[0]
            # dat命名式样：与spotguide_tbl表对应，删除后缀“_1”，“_2”等
            datName = dayArrowName[:-2]
            common_functions.ComposePicsToDat([dayArrowDict[dayArrowKey]], destDir, 1, datName)
        return

    # 过程：
    # 1.将pattern图拷贝到dest，此步骤保持了原pattern图的目录结构。
    # 2.将sar图合并到dest中，此做法不改变元数据。
    # pattern图和sar图的名字相同，找到名字相同的pattern图和sar图合并。
    def composeSarToPattern(self, srcPatternDir, srcSarDir, destDir):
        if os.path.isdir(srcPatternDir) == False\
        or os.path.isdir(srcSarDir) == False:
            return
        print "compose sar to pattern start."
        if(os.path.exists(destDir) == True):
            shutil.rmtree(destDir)
        shutil.copytree(srcPatternDir, destDir)
        
        # 从图片元数据中列出所有pattern图名字。
        ejvPatternList = []
        for curDir,dirNames,fileNames in os.walk(destDir):
            if curDir.lower().find("bird_view") != -1:
                # bird view中不合sar数据。
                continue
            for oneFile in fileNames:
                if oneFile[-4:] == ".jpg":
                    ejvPatternList.append(os.path.join(curDir, oneFile.lower()))
        
        # 从图片元数据中列出所有sar文件名。
        sarPngList = []
        for curDir,dirNames,fileNames in os.walk(srcSarDir):
            for oneFile in fileNames:
                if oneFile[-4:] == ".png":
                    sarPngList.append(os.path.join(curDir, oneFile))
                
        for sarPng in sarPngList:
            # sar图片名字应以SR_打头。
            # 例如某sar图SR_BH_555171664.png
            # 该sar图片对应的pattern图名字应为JV_BH_555171664.jpg
            # 把这俩图片合并。
            sarPngName = os.path.split(sarPng)[1]
            for ejvPatternPic in ejvPatternList:
                ejvJpgName = os.path.split(ejvPatternPic)[1]
                a = os.path.splitext(sarPngName)[0][3:].lower()
                b = os.path.splitext(ejvJpgName)[0][3:].lower()
                if (a == b):
                    destPic = ejvPatternPic # 将sar图合并到本图片。
                    cmd = "composite.exe -gravity north %s %s %s" % (sarPng, ejvPatternPic, destPic)
                    os.system(cmd)
                    print sarPng
                    print ejvPatternPic
                    print "    ==>" + destPic
                    break
                
        print "compose sar to pattern end."
        return
    
    def makeGjvDat(self, srcDir, destDir):
        if os.path.isdir(srcDir) == False:
            print "source directory not found: " + srcDir
            return
        if(os.path.exists(destDir) == True):
            shutil.rmtree(destDir)
        os.mkdir(destDir)

        # 列出所有pattern图。
        # 由svg tool解压出来的图片文件夹结构最后一层应为DAY或NIGHT（在svg tool中选中制作黑白图）。
        # DAY包含白天的所有pattern图和arrow图。
        # NIGHT包含黑夜的所有pattern图和arrow图。
        # pattern图一般为.jpg（在svg tool中所设置），arrow图一般为.png（在svg tool中设置）。
        # 黑白pattern图有区别，黑白arrow图无区别。
        # 合并时将黑白pattern图合并成一个dat。
        # arrow图仅取白天的合并。
        # 完。
        dayPatternDict = {} # 白天pattern图列表。
        dayArrowDict = {} # 白天arrow图列表。
        nightPatternDict = {} # 黑夜pattern图列表。
        for curDir,dirNames,fileNames in os.walk(srcDir):
            if curDir.lower().find('day') != -1: # 白天图
                for oneFile in fileNames:
                    if oneFile[-4:] == ".jpg": # pattern图
                        dayPatternDict[oneFile] = os.path.join(curDir, oneFile).lower()
                    if oneFile[-4:] == ".png": # arrow图
                        dayArrowDict[oneFile] = os.path.join(curDir, oneFile).lower()
            if curDir.lower().find('night') != -1: # 黑夜图
                for oneFile in fileNames:
                    if oneFile[-4:] == ".jpg": # pattern图
                        nightPatternDict[oneFile] = os.path.join(curDir, oneFile).lower()  
        
        # 合并pattern图
        for dayPatternKey in dayPatternDict:
            if dayPatternKey in nightPatternDict:
                imageList = []
                imageList.append(dayPatternDict[dayPatternKey])
                imageList.append(nightPatternDict[dayPatternKey])
                dayPatternName = os.path.splitext(os.path.split(dayPatternDict[dayPatternKey])[1])[0]
                # dat命名式样：直接使用白天pattern图名字命名dat
                common_functions.ComposePicsToDat(imageList, destDir, 1, dayPatternName)
                
        # 合并arrow图
        for dayArrowKey in dayArrowDict:
            dayArrowName = os.path.splitext(os.path.split(dayArrowDict[dayArrowKey])[1])[0]
            # dat命名式样：直接使用白天arrow图名字命名dat
            common_functions.ComposePicsToDat([dayArrowDict[dayArrowKey]], destDir, 1, dayArrowName)
        return

if __name__ == '__main__':
    test = GeneratorPicBinary_Here()
    #test.composeSarToPattern(r"C:\My\20150514_mea_pic_2014Q4\2DJ_2014Q4_MEA_svgout",
    #                         r"C:\My\20150514_mea_pic_2014Q4\2DS_2014Q4_MEA_svgout",
    #                         r"C:\My\20150514_mea_pic_2014Q4\2DJ_2014Q4_MEA_svgout_with_sar")
    test.makeEjvDat(r"C:\My\20150514_mea_pic_2014Q4\2DJ_2014Q4_MEA_svgout_with_sar", 
                    r"C:\My\20150514_mea_pic_2014Q4\2DJ_2014Q4_MEA_svgout_with_sar_dat")
    test.makeGjvDat(r"C:\My\20150514_mea_pic_2014Q4\2DGJ_2014Q4_MEA_svgout", 
                    r"C:\My\20150514_mea_pic_2014Q4\2DGJ_2014Q4_MEA_svgout_dat")

















