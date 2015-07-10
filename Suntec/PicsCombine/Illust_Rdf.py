#encoding=utf-8
import os
import sys
import shutil
import common_functions
import xml.dom.minidom

# 解析svg文件，获取传入的outlinkid所关联的所有sarPng的名字。 
# 模拟svg工具生成sar图片时的命名规则，解析svg文件并生成sar图片名字后，返回名字列表。
def parseSvgAndGetSarNameListByOutlinkid(sarSvgFile, outlinkid=-1):
    sarPngList = []
    doc = xml.dom.minidom.parse(sarSvgFile)
    root = doc.documentElement
    svgFileName = root.getAttribute('id')
    
    panelIDList = [] 
    # 找出输入的outlink依赖到的所有panel的列表。
    # 当outlink等于-1时，列出所有panel。
    node_mr_routes =root.getElementsByTagName('mr:routes')[0]
    inlinkid = node_mr_routes.getAttribute('sourceLinkPVId')
    for subNode in node_mr_routes.childNodes:
        if subNode.nodeType != xml.dom.minidom.Node.ELEMENT_NODE:
            continue
        if subNode.tagName == 'mr:route':
            if outlinkid == -1 or outlinkid == subNode.getAttribute('destinationLinkPVId'):
                SVGObjectList = subNode.getElementsByTagName('mr:SVGObject')
                for oneSVGObject in SVGObjectList:
                    panelIDList.append(oneSVGObject.getAttribute('refId'))
    
    # 建立panelID到png名字的map
    map_PanelID2sarName = {}
    for subNode in root.childNodes:
        if subNode.nodeType != xml.dom.minidom.Node.ELEMENT_NODE:
            continue
        elif subNode.tagName == 'g':
            for subNode2 in subNode.childNodes:
                if subNode2.nodeType != xml.dom.minidom.Node.ELEMENT_NODE:
                    continue
                if subNode2.tagName == 'g':
                    for subNode3 in subNode2.childNodes:
                        if subNode3.nodeType != xml.dom.minidom.Node.ELEMENT_NODE:
                            continue
                        if subNode3.tagName == 'g':
                            panelID = subNode3.getAttribute('id')
                            strSignName = subNode2.getAttribute('id')
                            map_PanelID2sarName[panelID] = strSignName

    # 根据outlink依赖的panel列表以及panel列表与sar名的map表，找到相应的sar名。
    # 把这些sar名模拟svg工具的拼接规则，输出png名字。
    # 经调查，在使用svg工具解压sar相关的svg文件时：
    # 如果选择“all signs in one file”，输出的sar图名字与svg名相同。
    # 如果选择了“Each sign in separate file”，输出的sar图名字由svg名+'_'+sar名 拼接而成。
    # 本次工作选择了“Each sign in separate file”模式，故使用 svg名+'_'+sar名。
    for onePanel in panelIDList:
        if map_PanelID2sarName.has_key(onePanel):
            sarPngList.append(svgFileName + '_' + map_PanelID2sarName[onePanel])
            
    return sarPngList

# Here中，由svg工具解析出来的箭头图的名字如下：
# JV_BH_555171664_832635709_1.png
# JV_BH_555171664_1051944061_2.png
# JV_EG_808410386_N_808310985_2.png
# JV_EG_812539621_R_809033463_2.png
# 其中它的第3个字段是inlinkid。
def getInlinkIDFromArrowPicPath(arrowPicPath):
    str1 = os.path.split(arrowPicPath)[1]
    str2 = os.path.splitext(str1)[0]
    strSplit = str2.split('_')
    if strSplit[3].lower() == 'n' or strSplit[3].lower() == 'r':
        return strSplit[2] + '_' + strSplit[3]
    else:
        return strSplit[2]

def getOutlinkIDFromArrowPicPath(arrowPicPath):
    str1 = os.path.split(arrowPicPath)[1]
    str2 = os.path.splitext(str1)[0]
    strSplit = str2.split('_')
    if strSplit[3].lower() == 'n' or strSplit[3].lower() == 'r':
        return strSplit[4]
    else:
        return strSplit[3]
   
# Sar的Svg文件名字格式如下：
# SR_BH_555171664.svg
# SR_IL_834484798_N.svg
# SR_IL_834897523_R.svg
# 其中第2个字段是inlinkid。
def getInlinkIDFromSarSvgFile(sarSvgFilePath):
    str1 = os.path.split(sarSvgFilePath)[1]
    str2 = os.path.splitext(str1)[0]
    strSplit = str2.split('_')
    if len(strSplit) == 4:
        return strSplit[2] + '_' + strSplit[3]
    else:
        return strSplit[2]

class GeneratorPicBinary_Here(object):

    def __init__(self):
        return
    
    def __del__(self):
        return
    
    # 此函数已废弃：根据要求，sar图已不再合并到背景图，而是合并到相对应的箭头图。
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
    
    # 作用：
    # 把svg工具导出的sar图合并到arrow图中，此工作仅针对于ejv相关的图片处理。
    # 业务定义：
    # 同一条outlink对应的arrow图和sar图合并成一张图片，作为arrow图提供给机能组。
    # 问题难点：
    # 1. svg工具导出arrow图时，同一条outlink对应的所有箭头都已经合并输出到一张arrow图里，并以inlink+outlink命名。
    # 2. svg工具导出sar图时，同一条outlink对应的所有sar无法合并输出到一张sar图里，只能把它们分别导出到独立的sar图，
    #    并以inlink+sar_name命名。
    # 3. 此时要根据outlink将一张箭头图和它对应的所有sar图（可能没有，也可能有一张或多张）对应，只能通过解析svg信息，建立
    #    起outlink到sar_name的对应关系，再分别找到各个独立的sar图，将它们与箭头图都合并到一起，最后作成一张由outlink
    #    确定的由箭头+sar组成的图片。
    #
    # 过程：
    # 1.将ejv图拷贝到dest，此步骤保持了原ejv图的目录结构。
    # 2.将sar图合并到拷贝后的ejv图中，此做法不会改变原始数据，方便日后调查bug。
    # srcEjvDir: ejv图的根目录
    # srcSarPicDir： sar图的根目录
    # srcSarSvgDir: sar对应的svg文件的根目录，需要用到这些svg文件来建立arrow图到sar图的对应关系。
    # destDir： 输出目录
    def composeSarToArrow(self, srcEjvDir, srcSarDir, srcSarSvgDir, destDir):
        if os.path.isdir(srcEjvDir) == False\
        or os.path.isdir(srcSarDir) == False:
            return
        print "compose sar to arrow start."
        #if(os.path.exists(destDir) == True):
        #    shutil.rmtree(destDir)
        #shutil.copytree(srcEjvDir, destDir)
        # 从图片元数据中列出所有arrow图的文件全路径。
        arrowPicList = []
        for curDir,dirNames,fileNames in os.walk(destDir):
            if curDir.lower().find("bird_view") != -1:
                # bird view中不合sar数据。
                continue
            for oneFile in fileNames:
                if oneFile[-4:] == ".png": # 箭头图是png图。
                    arrowPicList.append(os.path.join(curDir, oneFile.lower()))
                    
        # 从图片元数据中列出所有sar图片的文件安全路径。
        sarPngList = []
        for curDir,dirNames,fileNames in os.walk(srcSarDir):
            for oneFile in fileNames:
                if oneFile[-4:] == ".png":
                    sarPngList.append(os.path.join(curDir, oneFile))
        
        # 从图片元数据中列出所有sar对应的svg文件全路径。
        sarSvgList = []
        for curDir,dirNames,fileNames in os.walk(srcSarSvgDir):
            if curDir.find('4x3') == -1:
                continue
            for oneFile in fileNames:
                if oneFile[-4:] == ".svg":
                    sarSvgList.append(os.path.join(curDir, oneFile))
                    
        # 遍历arrow图文件，尝试找到对应的sar.svg文件。
        # 判断条件：arrow图文件名和svg文件名中必定都包含了inlinkid，根据两者的inlinkid是否相等判断是否相关联。
        oFStream = open("c:\\a.txt", "w") # 仅为了check结果。
        for oneArrowPic in arrowPicList:
            for oneSvgFile in sarSvgList:
                if getInlinkIDFromArrowPicPath(oneArrowPic) == \
                   getInlinkIDFromSarSvgFile(oneSvgFile): # 两者包含的inlinkid相等
                    # 分析svg文件，获得该arrow对应的所有sar名字。
                    sarNameList = parseSvgAndGetSarNameListByOutlinkid(oneSvgFile, \
                                                                 getOutlinkIDFromArrowPicPath(oneArrowPic))
                    # 再尝试从实际sar图片列表中找到每个sar名字对应的图片，并把它们一一合并到当前arrow图里。
                    for oneSarName in sarNameList: # 遍历svg中获得的sar名字列表
                        for oneSarPng in sarPngList:
                            if os.path.split(oneSarPng)[1][:-4:] == oneSarName: # 根据sar名字找到了对应的sar图。
                                cmd = "composite.exe -gravity north %s %s %s" %\
                                 (oneSarPng, oneArrowPic, oneArrowPic)
                                os.system(cmd)
                                print """composed\n\t%s\nto\n\t%s""" % (oneSarPng, oneArrowPic)
                                oFStream.write(oneSarName + '\n') # 仅为了check结果。
                                
                                break # 一个sar名字只可能找到一张图片，故找到后即可中断。
                    break # 一个arrow图只可能找到一个sar的svg文件，故找到后即可中断。
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

class GeneratorPicBinary_Here_Checker(object):

    def __init__(self):
        return
    
    def __del__(self):
        return
    
    # 从给定的svg文件夹中的每个svg文件里分析出它会输出的所有sar图片的名字，做成列表1
    # composedSarList应是在合并图片时返回的一个列表，记做列表2
    # 理论上说，列表1应该与列表2相等。
    def composeSarToArrow_check(self, srcSarSvgDir, outputResultTxt):
        
        svgFileList= []
        # 从输入的svg文件夹列出所有svg。
        for curDir, subDirs, fileNames in os.walk(srcSarSvgDir):
            if curDir.find("4x3") == -1: # 只关注4x3的svg
                continue
            for oneFile in fileNames:
                if oneFile[-4:] == ".svg":
                    svgFileList.append(os.path.join(curDir, oneFile))
        
        # 从找到的所有svg中分析出它们所包含的所有sar图片的名字，合成一个列表。  
        sarNameListFromSvg = []    
        for oneSvg in svgFileList:
            sarNameListFromSvg += parseSvgAndGetSarNameListByOutlinkid(oneSvg)
        
        oFStream = open("c:\\b.txt", "w")
        for oneSarName in sarNameListFromSvg:
            oFStream.write(oneSarName + '\n')
        oFStream.close()
        return



if __name__ == '__main__':
    test = GeneratorPicBinary_Here()
    test_checker = GeneratorPicBinary_Here_Checker()
    test.composeSarToArrow(r"C:\My\20150514_mea_2014Q4_pic\2DJ_2014Q4_MEA_svgout",
                           r"C:\My\20150514_mea_2014Q4_pic\2DS_2014Q4_MEA_svgout",
                           r"C:\My\20150514_mea_2014Q4_pic\2DS_2014Q4_MEA",
                           r"C:\My\20150514_mea_2014Q4_pic\2DJ_2014Q4_MEA_svgout_withsar")
    test_checker.composeSarToArrow_check(r"C:\My\20150514_mea_2014Q4_pic\2DS_2014Q4_MEA",
                                         "c:\\a.txt")
    
#    test.composeSarToPattern(r"C:\My\20150610_ase_14Q4_pic\2DJ_ASE_14Q4_svgout",
#                             r"C:\My\20150610_ase_14Q4_pic\2DS_ASE_14Q4_svgout",
#                             r"C:\My\20150610_ase_14Q4_pic\2DJ_ASE_14Q4_svgout_withsar")
#    test.makeEjvDat(r"C:\My\20150610_ase_14Q4_pic\2DJ_ASE_14Q4_svgout_withsar", 
#                    r"C:\My\20150610_ase_14Q4_pic\2DJ_ASE_14Q4_svgout_withsar_dat")
#    test.makeGjvDat(r"C:\My\20150610_ase_14Q4_pic\2DGJ_ASE_14Q4_svgout", 
#                    r"C:\My\20150610_ase_14Q4_pic\2DGJ_ASE_14Q4_svgout_dat")
#    test.make_all_sar_file_names(r"C:\My\20150610_ase_14Q4_pic\2DS_ASE_14Q4_svgout",
#                                 r"C:\My\20150610_ase_14Q4_pic\illust\all_sar_files_name.csv")
#    test.make_sign_as_real_csv(r"C:\My\20150610_ase_14Q4_pic\2DS_ASE_14Q4_svgout", 
#                               r"C:\My\20150610_ase_14Q4_pic\illust\all_jv.csv",
#                               r"C:\My\20150610_ase_14Q4_pic\illust\pic\sign",
#                               r"C:\My\20150610_ase_14Q4_pic\illust\sign_as_real.csv")
    
    















