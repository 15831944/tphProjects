# -*- coding: cp936 -*-
import psycopg2
import shutil
import os

# 有的时候需要替换dat，分为：
# 1.替换至idata，然后跑ci，此步骤最简单，直接拷贝至idata机器即可。
# 2.已作成二进制文件，需要查询rdb_guideinfo_pic_blob_bytea表，获得gid并替换相关文件，本文件正完成此工作。

class IllustDataGenerator(object):
    def __init__(self, dbIP, dbName, userName, password):
        self.conn = psycopg2.connect(''' host='%s' dbname='%s' user='%s' password='%s' ''' % \
                                     (dbIP, dbName, userName, password))
        self.pg = self.conn.cursor()
    
    # 根据dat的名字，从rdb_guideinfo_pic_blob_bytea表中生成map到gid的映射表，根据此映射表将所有xxx.dat修改成一个数字命名
    # 的文件后，替换到hyp作成的二进制文件中。   
    def generatePictureNameToPicBlobGidMap(self, datDir):
        if(os.path.isdir(datDir) == False):
            print "can not find directory: %s" % datDir
            return
        
        datName2PicBlobGidMap = {}
        for curDir, subDirs, fileNames in os.walk(datDir):
            for oneFile in fileNames:
                if oneFile[-4:] == ".dat": # 这是一个dat文件
                    datName2PicBlobGidMap[oneFile[:-4]] = -1  
                              
        str1 = """select gid from rdb_guideinfo_pic_blob_bytea where image_id='%s'"""
        for oneKey in datName2PicBlobGidMap:
            sqlcmd = str1 % oneKey
            self.pg.execute(sqlcmd)
            datName2PicBlobGidMap[oneKey] = self.pg.fetchone()[0]
        
        return datName2PicBlobGidMap
    
    # 根据dat名字到纯数字命名的二进制文件的映射表，将dat文件手动制作成各个纯数字命名的二进制文件，以方便替换到hyp制作的文件中。
    # datFileDir：要更新的dat的存放路径
    # datName2PicBlobGidMap：dat到gid的映射表
    # destDir：转化成纯数字命名的文件后的输出目录
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
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    