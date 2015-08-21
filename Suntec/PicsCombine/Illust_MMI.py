# -*- coding: cp936 -*-
import os
import psycopg2
import shutil
import common_functions

sp_splitter = '_signpost_'
my_temp_splitter = '_xasdfaseeqweradf_'

# MMI印度元数据情况如下：
# 1.元数据提供了2张背景图（一张白天一张黑夜），90张pattern图，和1500+张sar图。
# 2.元数据提供了一张数据表org_jv_location，里面记录了由哪张pattern图（load_lyr字段）
#   和哪张sar图（sign_lyr字段）可以合并成某个特定路口的放大图。
class MMiIllustMaker(object):

    def __init__(self, connStr):
        self.conn = psycopg2.connect(connStr)
        self.pgcur2 = self.conn.cursor()
        
    # 将pattern图合并到背景图上，即获得区分了白天黑夜的pattern图。
    # 新图片仍以pattern图的名字命名
    def compositeBackground(self, srcDir, destDir):
        if(os.path.isdir(srcDir) == False):
            print "can not find directory: %s." % srcDir
            return
        if(os.path.exists(destDir) == True):
            shutil.rmtree(destDir)
        shutil.copytree(srcDir, destDir)
        
        self.pgcur2.execute("SELECT distinct road_lyr, sky_lyr FROM org_jv_location")
        rows = self.pgcur2.fetchall()
        for row in rows:
            road_lyr = row[0] # pattern图字段
            sky_lyr = row[1] # 白天黑夜背景图字段            
            roadPicPath = os.path.join(srcDir, road_lyr)
            if(os.path.isfile(roadPicPath) == False):
                print "can't find road picture: " + road_lyr
                continue
            
            skyPicPath = os.path.join(srcDir, sky_lyr)
            if(os.path.isfile(skyPicPath) == False):
                print "can't find background picture: " + sky_lyr
                continue
            
            # 合并背景图，以pattern图名字命名
            outputPic = os.path.join(destDir, road_lyr)
            cmd = "composite.exe -gravity north %s %s %s" % (roadPicPath, skyPicPath, outputPic)
            os.system(cmd)
              
    # 将signpost合并到pattern图上
    # 此处合并后，pattern图中就包含了signpost信息。
    # 为了标识此图片包含signpost信息，pattern图名字发生了变化。
    # 新名字中以""新名字的命名依据mmi里制作spotguide_tbl时的命名规则。
    def compositeSignpost(self, srcDir, destDir):
        if(os.path.isdir(srcDir) == False):
            print "source directory not exist: " + srcDir
            return
        if(os.path.exists(destDir) == True):
            shutil.rmtree(destDir)
        shutil.copytree(srcDir, destDir)

        self.pgcur2.execute("SELECT distinct road_lyr, sign_lyr FROM org_jv_location WHERE sign_lyr is not null")
        rows = self.pgcur2.fetchall()
        for row in rows:
            road_lyr = row[0] # road图字段
            sign_lyr = row[1] # signpost图字段            
            roadPicPath = os.path.join(srcDir, road_lyr)
            if(os.path.isfile(roadPicPath) == False):
                print "can't find road picture: " + road_lyr
                continue
                
            signPicPath = os.path.join(srcDir, sign_lyr)
            if(os.path.isfile(signPicPath) == False):
                print "can't find road picture: " + sign_lyr
                continue
            
            # 将signpost图合并到road图（即pattern图）中。
            outputPic = road_lyr[:-4:] + my_temp_splitter + sign_lyr
            outputPic = os.path.join(destDir, outputPic)
            cmd = "composite.exe -gravity north %s %s %s" % (signPicPath, roadPicPath, outputPic)
            os.system(cmd)

    # 1.根据数据库中的distinct fm_edge, to_edge1, to_edge2, to_edge3, to_edge4
    #   这5个字段确定一组road（即pattern）图和一组signpost图。
    # 2.根据每个pattern图和signpost图找到上一步中合成的带signpost的pattern图，得到一个
    #   pattern图列表。
    # 3.将这个pattern图列表合并成一个dat。
    def makePatternDat(self, srcDir, destDir):
        if os.path.isdir(srcDir) == False:
            print "can not find directory: %s." % srcDir
            return
        if(os.path.exists(destDir) == True):
            shutil.rmtree(destDir)
        os.mkdir(destDir)
        
        self.pgcur2.execute(''' select distinct fm_edge, to_edge1, to_edge2, to_edge3, to_edge4,
                                    array_agg(road_lyr) as road_lyr_list,
                                    array_agg(sign_lyr) as sign_lyr_list 
                                from org_jv_location
                                group by fm_edge, to_edge1, to_edge2, to_edge3, to_edge4''')
        rows = self.pgcur2.fetchall()
        for row in rows:
            print "-----------------------------------------------------------------------------------------"
            # day and night illust
            road_lyr_list = row[5] # road_lyr列表，正常情况下，此列表中不应有空值。
            sign_lyr_list = row[6] # sign_lyr列表，此列表可能有None值，对应情况是此点没有sar图。
            
            fileList = []
            assert len(road_lyr_list) == len(sign_lyr_list)
            for onePattern, oneSar in zip(road_lyr_list, sign_lyr_list):
                if onePattern is None: # road图名字为空，出错，pass。
                    continue
                
                outputDatName = ''
                if oneSar is None:
                    outputDatName = onePattern
                else:
                    outputDatName = onePattern[:-4] + '_' + oneSar
                    
                patternPngPath = os.path.join(srcDir, outputDatName)
                if os.path.isfile(patternPngPath):
                    oneFile = common_functions.datSegmentInfo()
                    oneFile.datInfo = common_functions.DAY_AND_NIGHT_COMMON # 这是一张不区分白天黑夜的箭头图
                    oneFile.imgPath = patternPngPath
                    fileList.append(oneFile)
            
            # 已与数据表数据制作时约定：使用第0个pattern图名字和第0个sar图名字拼成dat名字。
            # guideinfo_spotguide_mmi.py: 144
            destDatName = road_lyr_list[0][:-4:] + sp_splitter + sign_lyr_list[1][:-4:]
            common_functions.ComposePicsToDat(fileList, destDir, destDatName)
    
     
    def makeArrowDat(self, srcDir, destDir):
        if os.path.isdir(srcDir) == False:
            print "input directory not exist! " + srcDir
            return
        if(os.path.exists(destDir) == True):
            shutil.rmtree(destDir)
        os.mkdir(destDir)
        
        strSqlCmd = ''' select arrow1 from org_jv_location where arrow1 is not null
                        union (select arrow2 from org_jv_location where arrow2 is not null)
                        union (select arrow3 from org_jv_location where arrow3 is not null)
                        union (select arrow4 from org_jv_location where arrow4 is not null)
                    '''
        self.pgcur2.execute(strSqlCmd)
        rows = self.pgcur2.fetchall()
        for row in rows:
            arrowPngName = row[0]
            arrowPngPath = os.path.join(srcDir, arrowPngName)
            if os.path.isfile(arrowPngPath) == False:
                continue
            oneFile = common_functions.datSegmentInfo()
            oneFile.datInfo = common_functions.DAY_AND_NIGHT_COMMON # 这是一张不区分白天黑夜的箭头图
            oneFile.imgPath = arrowPngPath
            common_functions.ComposePicsToDat([oneFile], destDir, arrowPngName[:-4:])
        return

if __name__ == '__main__':
    illustMaker = MMiIllustMaker(''' host='172.26.179.184'
                                     dbname='17cy_IND_MMI_CI'
                                     user='postgres' 
                                     password='pset123456' ''')
    illustMaker.compositeBackground(r"C:\My\20150410_mmi_pic\Pattern",
                                    r"C:\My\20150410_mmi_pic\Pattern_withbackground")
    illustMaker.compositeSignpost(r"C:\My\20150410_mmi_pic\Pattern_withbackground",
                                  r"C:\My\20150410_mmi_pic\Pattern_withsar")
    illustMaker.makePatternDat(r"C:\My\20150410_mmi_pic\Pattern_withsar",
                               r"C:\My\20150410_mmi_pic\illust\pattern")
    illustMaker.makeArrowDat(r"C:\My\20150410_mmi_pic\Pattern_withsar",
                             r"C:\My\20150410_mmi_pic\illust\arrow")
    pass















