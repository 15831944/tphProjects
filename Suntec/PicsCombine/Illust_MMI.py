# -*- coding: cp936 -*-
import os
import psycopg2
import shutil
import common_functions

sp_splitter = '_signpost_'
my_temp_splitter = '_xasdfaseeqweradf_'

# MMIӡ��Ԫ����������£�
# 1.Ԫ�����ṩ��2�ű���ͼ��һ�Ű���һ�ź�ҹ����90��patternͼ����1500+��sarͼ��
# 2.Ԫ�����ṩ��һ�����ݱ�org_jv_location�������¼��������patternͼ��load_lyr�ֶΣ�
#   ������sarͼ��sign_lyr�ֶΣ����Ժϲ���ĳ���ض�·�ڵķŴ�ͼ��
class MMiIllustMaker(object):

    def __init__(self, connStr):
        self.conn = psycopg2.connect(connStr)
        self.pgcur2 = self.conn.cursor()
        
    # ��patternͼ�ϲ�������ͼ�ϣ�����������˰����ҹ��patternͼ��
    # ��ͼƬ����patternͼ����������
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
            road_lyr = row[0] # patternͼ�ֶ�
            sky_lyr = row[1] # �����ҹ����ͼ�ֶ�            
            roadPicPath = os.path.join(srcDir, road_lyr)
            if(os.path.isfile(roadPicPath) == False):
                print "can't find road picture: " + road_lyr
                continue
            
            skyPicPath = os.path.join(srcDir, sky_lyr)
            if(os.path.isfile(skyPicPath) == False):
                print "can't find background picture: " + sky_lyr
                continue
            
            # �ϲ�����ͼ����patternͼ��������
            outputPic = os.path.join(destDir, road_lyr)
            cmd = "composite.exe -gravity north %s %s %s" % (roadPicPath, skyPicPath, outputPic)
            os.system(cmd)
              
    # ��signpost�ϲ���patternͼ��
    # �˴��ϲ���patternͼ�оͰ�����signpost��Ϣ��
    # Ϊ�˱�ʶ��ͼƬ����signpost��Ϣ��patternͼ���ַ����˱仯��
    # ����������""�����ֵ���������mmi������spotguide_tblʱ����������
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
            road_lyr = row[0] # roadͼ�ֶ�
            sign_lyr = row[1] # signpostͼ�ֶ�            
            roadPicPath = os.path.join(srcDir, road_lyr)
            if(os.path.isfile(roadPicPath) == False):
                print "can't find road picture: " + road_lyr
                continue
                
            signPicPath = os.path.join(srcDir, sign_lyr)
            if(os.path.isfile(signPicPath) == False):
                print "can't find road picture: " + sign_lyr
                continue
            
            # ��signpostͼ�ϲ���roadͼ����patternͼ���С�
            outputPic = road_lyr[:-4:] + my_temp_splitter + sign_lyr
            outputPic = os.path.join(destDir, outputPic)
            cmd = "composite.exe -gravity north %s %s %s" % (signPicPath, roadPicPath, outputPic)
            os.system(cmd)

    # 1.�������ݿ��е�distinct fm_edge, to_edge1, to_edge2, to_edge3, to_edge4
    #   ��5���ֶ�ȷ��һ��road����pattern��ͼ��һ��signpostͼ��
    # 2.����ÿ��patternͼ��signpostͼ�ҵ���һ���кϳɵĴ�signpost��patternͼ���õ�һ��
    #   patternͼ�б�
    # 3.�����patternͼ�б�ϲ���һ��dat��
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
            road_lyr_list = row[5] # road_lyr�б���������£����б��в�Ӧ�п�ֵ��
            sign_lyr_list = row[6] # sign_lyr�б����б������Noneֵ����Ӧ����Ǵ˵�û��sarͼ��
            
            fileList = []
            assert len(road_lyr_list) == len(sign_lyr_list)
            for onePattern, oneSar in zip(road_lyr_list, sign_lyr_list):
                if onePattern is None: # roadͼ����Ϊ�գ�����pass��
                    continue
                
                outputDatName = ''
                if oneSar is None:
                    outputDatName = onePattern
                else:
                    outputDatName = onePattern[:-4] + '_' + oneSar
                    
                patternPngPath = os.path.join(srcDir, outputDatName)
                if os.path.isfile(patternPngPath):
                    oneFile = common_functions.datSegmentInfo()
                    oneFile.datInfo = common_functions.DAY_AND_NIGHT_COMMON # ����һ�Ų����ְ����ҹ�ļ�ͷͼ
                    oneFile.imgPath = patternPngPath
                    fileList.append(oneFile)
            
            # �������ݱ���������ʱԼ����ʹ�õ�0��patternͼ���ֺ͵�0��sarͼ����ƴ��dat���֡�
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
            oneFile.datInfo = common_functions.DAY_AND_NIGHT_COMMON # ����һ�Ų����ְ����ҹ�ļ�ͷͼ
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















