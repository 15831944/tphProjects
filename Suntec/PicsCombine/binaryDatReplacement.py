# -*- coding: cp936 -*-
import psycopg2
import shutil
import os

# �е�ʱ����Ҫ�滻dat����Ϊ��
# 1.�滻��idata��Ȼ����ci���˲�����򵥣�ֱ�ӿ�����idata�������ɡ�
# 2.�����ɶ������ļ�����Ҫ��ѯrdb_guideinfo_pic_blob_bytea�����gid���滻����ļ������ļ�����ɴ˹�����

class IllustDataGenerator(object):
    def __init__(self, dbIP, dbName, userName, password):
        self.conn = psycopg2.connect(''' host='%s' dbname='%s' user='%s' password='%s' ''' % \
                                     (dbIP, dbName, userName, password))
        self.pg = self.conn.cursor()
    
    # ����dat�����֣���rdb_guideinfo_pic_blob_bytea��������map��gid��ӳ������ݴ�ӳ�������xxx.dat�޸ĳ�һ����������
    # ���ļ����滻��hyp���ɵĶ������ļ��С�   
    def generatePictureNameToPicBlobGidMap(self, datDir):
        if(os.path.isdir(datDir) == False):
            print "can not find directory: %s" % datDir
            return
        
        datName2PicBlobGidMap = {}
        for curDir, subDirs, fileNames in os.walk(datDir):
            for oneFile in fileNames:
                if oneFile[-4:] == ".dat": # ����һ��dat�ļ�
                    datName2PicBlobGidMap[oneFile[:-4]] = -1  
                              
        str1 = """select gid from rdb_guideinfo_pic_blob_bytea where image_id='%s'"""
        for oneKey in datName2PicBlobGidMap:
            sqlcmd = str1 % oneKey
            self.pg.execute(sqlcmd)
            datName2PicBlobGidMap[oneKey] = self.pg.fetchone()[0]
        
        return datName2PicBlobGidMap
    
    # ����dat���ֵ������������Ķ������ļ���ӳ�����dat�ļ��ֶ������ɸ��������������Ķ������ļ����Է����滻��hyp�������ļ��С�
    # datFileDir��Ҫ���µ�dat�Ĵ��·��
    # datName2PicBlobGidMap��dat��gid��ӳ���
    # destDir��ת���ɴ������������ļ�������Ŀ¼
    def renameDatToNumOnlyFile(self, datFileDir, datName2PicBlobGidMap, destDir):
        if(os.path.isdir(datFileDir) == False):
            print "can not find directory: %s" % datFileDir
            return
        if(os.path.isdir(destDir) == True):
            shutil.rmtree(destDir)
        os.mkdir(destDir)
        
        for curDir, subDirs, fileNames in os.walk(datFileDir):
            for oneFile in fileNames:
                datName = os.path.splitext(oneFile)[0]
                x1 = os.path.join(datFileDir, oneFile)
                x2 = os.path.join(destDir, str(datName2PicBlobGidMap[datName]))
                shutil.copyfile(x1, x2)
        return
        
if __name__ == '__main__':
    dbIP = '''172.26.11.234'''
    dbName = '''CHN_NAVINFO_2015Q1_0082_0003_102'''
    userName = '''postgres'''
    password = '''pset123456'''
    datMaker = IllustDataGenerator(dbIP, dbName, userName, password)
    datFileDir = r"C:\My\20150820_pointlist_dat"
    destDir = r"C:\My\20150820_pointlist_dat_num_only"
    datName2PicBlobGidMap = datMaker.generatePictureNameToPicBlobGidMap(datFileDir)
    datMaker.renameDatToNumOnlyFile(datFileDir, datName2PicBlobGidMap, destDir)
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    