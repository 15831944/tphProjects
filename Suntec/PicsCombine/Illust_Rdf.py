#encoding=utf-8
import os
import shutil
import common_functions

class GeneratorPicBinary_Here(object):

    def __init__(self):
        return
    
    def __del__(self):
        return
    
    # 过程：
    # 1.将pattern图拷贝到dest，此步骤保持了原pattern图的目录结构。
    # 2.将sar图合并到dest中，此做法不改变元数据。
    # pattern图和sar图的名字相同，找到名字相同的pattern图和sar图合并。
    # srcPatternDir: pattern图的根目录
    # srcSarDir： sar图的根目录
    # destDir： 输出目录
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
            # 把该sar图片合并到对应的pattern图中。
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
                
        print "compose sar to pattern end."
        return
    
    # srcDir：ejv图的根目录
    # destDir：输出目录
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
            if curDir.lower().find('driver_view') != -1: # 当这张图片是driver_view图片时
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

            if curDir.lower().find('bird_view') != -1: # 当这张图片是bird_view的图片时
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
                common_functions.ComposePicsToDat(imageList, destDir, -1, dayPatternName)
                
        # 合并arrow图
        for dayArrowKey in dayArrowDict:
            dayArrowName = os.path.splitext(os.path.split(dayArrowDict[dayArrowKey])[1])[0]
            # dat命名式样：与spotguide_tbl表对应，删除后缀“_1”，“_2”等
            datName = dayArrowName[:-2]
            common_functions.ComposePicsToDat([dayArrowDict[dayArrowKey]], destDir, 0, datName)
        return

    # srcDir：gjv图的根目录
    # destDir： 输出目录
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
                common_functions.ComposePicsToDat(imageList, destDir, -1, dayPatternName)
                
        # 合并arrow图
        for dayArrowKey in dayArrowDict:
            dayArrowName = os.path.splitext(os.path.split(dayArrowDict[dayArrowKey])[1])[0]
            # dat命名式样：直接使用白天arrow图名字命名dat
            common_functions.ComposePicsToDat([dayArrowDict[dayArrowKey]], destDir, 0, dayArrowName)
        return
    
    # 作用：生成all_sar_file_names.csv
    # 做法：遍历SAR图的目录，列出所有SAR图片名字。
    # srcDir：sar png图的根目录
    # outputCSV：要输出的csv文件
    def make_all_sar_file_names(self, srcDir, outputCSV='all_sar_files_name.csv'):
        if os.path.isdir(srcDir) == False:
            print "source directory not found: " + srcDir
            return

        oFStream = open(outputCSV, "w")
        for curDir,dirNames,fileNames in os.walk(srcDir):
            for oneFile in fileNames:
                if oneFile[-4:] == '.png':
                    oFStream.write(oneFile + '\n')
        return
    
    # 作用1：生成sign_as_real.csv
    # 作用2：生成signpost对应的dat
    # 做法：
    # 1.从all_jv.csv里解析出仅有sar图（没有gjv图和ejv图）的项，判断能否找到对应的signpost图。
    # 2.把能找到图片的项写入sign_as_real.csv
    # 3.能找到图片的项作成dat。
    # sarSrcDir: sar png图的根目录
    # allJvCsv： all_jv.csv的全路径
    # signpost dat文件的输出路径
    # outputCSv： 要输出的csv文件
    def make_sign_as_real_csv(self, sarSrcDir, allJvCsv, destDir, outputCSV='sign_as_real.csv'):
        if os.path.isdir(sarSrcDir) == False:
            print "source directory not found: " + sarSrcDir
            return
        if os.path.isfile(allJvCsv) == False:
            print "source csv file not found: " + allJvCsv
            return
        if(os.path.exists(destDir) == True):
            shutil.rmtree(destDir)
        os.mkdir(destDir)
            
        ejvRecordList = common_functions.analyse_csv(allJvCsv)
        signpostList = []
        for ejvRecord in ejvRecordList:
            if ejvRecord['filename'] != '':
                continue
            if ejvRecord['ejv_filename'] != '':
                continue
            if ejvRecord['sar_filename'] == '':
                continue
            signpostList.append(ejvRecord['sar_filename'])
        
        signpostList = list(set(signpostList))
        
        sarPngList = []
        for curDir,dirNames,fileNames in os.walk(sarSrcDir):
            for oneFile in fileNames:
                sarPngList.append(os.path.join(curDir, oneFile))
        
        oFStream = open(outputCSV, "w")
        for signpost in signpostList:
            for sarPng in sarPngList:
                if os.path.splitext(signpost)[0].lower() ==\
                os.path.splitext(os.path.split(sarPng)[1])[0].lower():
                    common_functions.ComposePicsToDat([sarPng], destDir)
                    oFStream.write('''%s,%s\n''' % (os.path.splitext(signpost)[0],\
                                   '_'.join(os.path.splitext(signpost)[0].split('_')[2:])))
        oFStream.close()
        return
    
if __name__ == '__main__':
    test = GeneratorPicBinary_Here()
    test.composeSarToPattern(r"C:\My\20150610_ase_14Q4_pic\2DJ_ASE_14Q4_svgout",
                             r"C:\My\20150610_ase_14Q4_pic\2DS_ASE_14Q4_svgout",
                             r"C:\My\20150610_ase_14Q4_pic\2DJ_ASE_14Q4_svgout_withsar")
    test.makeEjvDat(r"C:\My\20150610_ase_14Q4_pic\2DJ_ASE_14Q4_svgout_withsar", 
                    r"C:\My\20150610_ase_14Q4_pic\2DJ_ASE_14Q4_svgout_withsar_dat")
    test.makeGjvDat(r"C:\My\20150610_ase_14Q4_pic\2DGJ_ASE_14Q4_svgout", 
                    r"C:\My\20150610_ase_14Q4_pic\2DGJ_ASE_14Q4_svgout_dat")
    test.make_all_sar_file_names(r"C:\My\20150610_ase_14Q4_pic\2DS_ASE_14Q4_svgout",
                                 r"C:\My\20150610_ase_14Q4_pic\illust\all_sar_files_name.csv")
    test.make_sign_as_real_csv(r"C:\My\20150610_ase_14Q4_pic\2DS_ASE_14Q4_svgout", 
                               r"C:\My\20150610_ase_14Q4_pic\illust\all_jv.csv",
                               r"C:\My\20150610_ase_14Q4_pic\illust\pic\sign",
                               r"C:\My\20150610_ase_14Q4_pic\illust\sign_as_real.csv")















