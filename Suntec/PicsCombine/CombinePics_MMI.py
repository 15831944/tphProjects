# -*- coding: cp936 -*-
import os
import psycopg2
import struct
import shutil

#==============================================================================
# comp_picture
#==============================================================================
class GeneratorPicBinary(object):

    def __init__(self):
        self.conn = psycopg2.connect(''' host='172.26.179.184'
                        dbname='17cy_IND_MMI_CI'
                        user='postgres' password='pset123456' ''')
        self.pgcur2 = self.conn.cursor()
        
    # 合并背景图
    # 名字未发生任何改变
    def compositeBackground(self, srcDir, destDir):
        if(os.path.isdir(srcDir) == False):
            print "source directory not exist: " + srcDir
            return
        if(os.path.exists(destDir) == True):
            shutil.rmtree(destDir)
        shutil.copytree(srcDir, destDir)
        
        self.pgcur2.execute("SELECT distinct road_lyr, sky_lyr FROM org_jv_location")
        rows = self.pgcur2.fetchall()
        for row in rows:
            road_ = row[0] # road图字段
            sky_ = row[1] # 背景图字段            
            roadPicPath = os.path.join(srcDir, road_)
            if(os.path.isfile(roadPicPath) == False):
                print "can't find road picture: " + road_
                continue
            
            skyPicPath = os.path.join(srcDir, sky_)
            if(os.path.isfile(skyPicPath) == False):
                print "can't find background picture: " + sky_
                continue
            
            # 合并背景图，以road图名字命名
            outputPic = os.path.join(destDir, road_)
            cmd = "composite.exe -gravity north %s %s %s" % (roadPicPath, skyPicPath, outputPic)
            os.system(cmd)
              
    # 合并signpost图
    # 有signpost的会生成一个新文件，命名如下：
    # JV_11.02.001.1.png， 与 JV_11.03.003.1.png 合并为 JV_11.02.001_JV_11.03.003.png
    # 箭头图和其他保持不变 
    def compositeSignpost(self, srcDir, destDir):
        if(os.path.isdir(srcDir) == False):
            print "source directory not exist: " + srcDir
            return
        if(os.path.exists(destDir) == True):
            shutil.rmtree(destDir)
        shutil.copytree(srcDir, destDir)

        self.pgcur2.execute("SELECT distinct sign_lyr, road_lyr FROM org_jv_location WHERE sign_lyr is not null")
        rows = self.pgcur2.fetchall()
        for row in rows:
            road_ = row[1] # road图字段
            sign_ = row[0] # signpost图字段            
            roadPicPath = os.path.join(srcDir, road_)
            if(os.path.isfile(roadPicPath) == False):
                print "can't find road picture: " + road_
                continue
                
            signPicPath = os.path.join(srcDir, sign_)
            if(os.path.isfile(signPicPath) == False):
                print "can't find road picture: " + sign_
                continue
            
            # 合并signpost图,以 road名+sign名 命名并输出到destDir
            # 删掉".1.png"
            outputPic = self.getNameByRoadAndSign(road_, sign_)
            outputPic = os.path.join(destDir, outputPic)
            cmd = "composite.exe -gravity north %s %s %s" % (signPicPath, roadPicPath, outputPic)
            os.system(cmd)

    def getNameByRoadAndSign(self, roadName, signName):
        return roadName[:-4:1] + '_'+ signName[:-4:1] + '.png'

    def makeJunctionViewIllust(self, srcDir, destDir):
        if os.path.isdir(srcDir) == False:
            print "input directory not exist! " + srcDir
            return
        if(os.path.exists(destDir) == True):
            shutil.rmtree(destDir)
        os.mkdir(destDir)
        
        # 过滤
        self.pgcur2.execute(''' select distinct fm_edge, to_edge1, to_edge2, to_edge3, to_edge4,
                                    array_agg(road_lyr) as road_lyr_list,
                                    array_agg(sign_lyr) as sign_lyr_list,
                                    array_agg(arrow1) as arrow1_list,
                                    array_agg(arrow2) as arrow2_list,
                                    array_agg(arrow3) as arrow3_list,
                                    array_agg(arrow4) as arrow4_list
                                from org_jv_location
                                group by fm_edge, to_edge1, to_edge2, to_edge3, to_edge4''')
        rows = self.pgcur2.fetchall()
        # 以下是MMI_india 10.0协议摘抄
        # JV_<Country ID>.<Pattern Code>.<pattern number>.<Time_code>.Png
        #     JV 
        #        stands for Junction View 
        #     <Country ID>
        #         2 character abbreviations according to ISO 3166.
        #     <pattern code>
        #         Pattern code will identify type of PNG among background, 
        #         Road design, arrows and Signpost.
        #             01 – Background
        #             02 - Road design
        #             03 - Signpost
        #             4S - Straight Arrow
        #             4L - Left Arrow
        #             4R - Right Arrow 
        #     <pattern number>
        #         Three digit pattern numbers is to identify different 
        #         designs for one pattern code. 
        #     <Time_code>
        #         is suffixed to understand if it is a day image or night image.
        #             Time_code = 1 represent Day
        #             Time_code = 2 represent Night
        for row in rows:
            print "-----------------------------------------------------------------------------------------"
            # day and night illust
            road_lyr_list = row[5] # road列表
            sign_lyr_list = row[6] # sign列表
            
            if(sign_lyr_list[0] is not None):    
                destFile = road_lyr_list[0][:-6:1] + '_' + sign_lyr_list[0][:-6:1] + '.dat'
                destFile = os.path.join(destDir, destFile)
                
                dayFileName = self.getNameByRoadAndSign(road_lyr_list[0], sign_lyr_list[0])
                dayPicPath = os.path.join(srcDir, dayFileName)
                if(os.path.exists(dayPicPath) == False):
                    print "day file not exist: " + dayPicPath
                    continue
                
                nightFileName = self.getNameByRoadAndSign(road_lyr_list[1], sign_lyr_list[1])
                nightPicPath = os.path.join(srcDir, nightFileName)
                if(os.path.exists(nightPicPath) == False):
                    print "night file not exist: " + nightPicPath
                    continue
            else:
                destFile = road_lyr_list[0][:-6:1] + '.dat'
                destFile = os.path.join(destDir, destFile)
                
                dayPicPath = os.path.join(srcDir, road_lyr_list[0])
                if(os.path.exists(dayPicPath) == False):
                    print "day file not exist: " + dayPicPath
                    continue
                
                nightPicPath = os.path.join(srcDir, road_lyr_list[1])
                if(os.path.exists(nightPicPath) == False):
                    print "night file not exist: " + nightPicPath
                    continue                
            
            dayFis = open(dayPicPath, 'rb')
            nightFis = open(nightPicPath, 'rb')
            dayPicLen = os.path.getsize(dayPicPath)
            nightPicLen = os.path.getsize(nightPicPath)
            headerBuffer = struct.pack("<HHbiibii", 0xFEFE, 2, 1, 22, \
                                       dayPicLen, 2, 22 + dayPicLen, \
                                       nightPicLen)
            resultBuffer = headerBuffer + dayFis.read() + nightFis.read()
            dayFis.close()
            nightFis.close()
            
            fos = open(destFile, 'wb')
            fos.write(resultBuffer)
            fos.close()
            print "day   picture: " + dayPicPath
            print "night picture: " + nightPicPath
            print "        >>>>>>>>  " + destFile
            
            # 合成箭头图
            # MMI协议里箭头图最大数量是4个，目前全部的数据里都只有2个，即后2个为空，为了考虑到3叉口等情况，这里进行了特殊处理
            # 经调查，MMI的黑白箭头图都相同，所以这里使用白天的箭头图作合成图片
            arrowPicList = [row[7][0], row[8][0], row[9][0], row[10][0]]
            while arrowPicList.count(None) != 0: 
                arrowPicList.remove(None)
            resultBuffer = struct.pack("<HH", 0xFEFE, len(arrowPicList))
        
            imageLens = []
            # 制作.dat数据头
            for arrowPic in arrowPicList:
                imgLen = os.path.getsize(os.path.join(srcDir, arrowPic))
                resultBuffer += struct.pack("<bii", 1, 4+len(arrowPicList)*9+sum(imageLens), imgLen)
                imageLens.append(imgLen)
            
            # 从每个图片读取数据流    
            for arrowPic in arrowPicList:
                tempFS = open(os.path.join(srcDir, arrowPic), 'rb')
                resultBuffer += tempFS.read()
                print "arrow: " + os.path.join(srcDir, arrowPic)
                tempFS.close()
            
#            destFile = 
            outFS = open(destFile, 'wb')
            print "    >>>>>>>>  " + destFile
            outFS.write(resultBuffer)
            outFS.close()

if __name__ == '__main__':
    test = GeneratorPicBinary()
#    test.compositeBackground("C:\\My\\20150410_mmi_pic\\Pattern", "C:\\My\\20150410_mmi_pic\\Pattern_background")
#    test.compositeSignpost("C:\\My\\20150410_mmi_pic\\Pattern_background", "C:\\My\\20150410_mmi_pic\\Pattern_signpost")
    test.makeJunctionViewIllust("C:\\My\\20150410_mmi_pic\\Pattern_signpost", "C:\\My\\20150410_mmi_pic\\illust_pic")
    pass