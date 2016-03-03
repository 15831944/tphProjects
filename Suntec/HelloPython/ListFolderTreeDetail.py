#encoding=utf-8
import os
import psycopg2

class NiPicTypeImport(object):

    def __init__(self, strIP, strDBName, strUser, strPassword):
        connStr = ''' host='%s' dbname='%s' user='%s' password='%s' ''' % \
                    (strIP, strDBName, strUser, strPassword)
        self.conn = psycopg2.connect(connStr)
        self.pg = self.conn.cursor()
        
    def listAll(self, strPathList, destFile):
        picTypeDict = {}
        for oneStrPath in strPathList:
            for curDir, subDirs, fileNames in os.walk(oneStrPath):
                ni_pic_type = ''
                curDirSplit = curDir.split('\\')
                if curDirSplit[-1] == 'arrow' or curDirSplit[-1] == 'pattern':
                    ni_pic_type = curDirSplit[-2]
                else:
                    continue

                for oneFile in fileNames:
                    if os.path.splitext(oneFile)[-1] == '.png':
                        picTypeDict[(os.path.splitext(oneFile)[-2])] = ni_pic_type

        with open(destFile, 'w') as oFStream:
            for pic_name, ni_pic_type in picTypeDict.items():
                oFStream.write("%s\t%s\n"%(pic_name, ni_pic_type))

    def dbImport(self, filePath):
        sqlcmd = """
drop table if exists temp_spotguide_ni_pic_type;
create table temp_spotguide_ni_pic_type
(pic_name varchar not null, ni_pic_type varchar not null);
"""
        self.pg.execute(sqlcmd)
        self.conn.commit()
        
        with open(filePath, 'a+') as iFStream:
            self.pg.copy_from(iFStream, 'temp_spotguide_ni_pic_type')
        self.conn.commit()
        
        sqlcmd = """
create index temp_spotguide_ni_pic_type_pic_name_idx
on temp_spotguide_ni_pic_type
using btree
(pic_name);
"""         
        self.pg.execute(sqlcmd)
        self.conn.commit()
        return

if __name__ == '__main__':
    bDayAndNight = False
    strIP = "192.168.10.20"
    strDBName = "C_NIMIF_15SUM_0082_0003"
    strUser = "postgres"
    strPassword = "pset123456"
    srcDir = [r"C:\My\illust_20151027_15summer_NI\Mainland", r"C:\My\illust_20151027_15summer_NI\HKMC"]
    destFile = r"C:\Users\hexin\Desktop\20150226_NI_PICtype_import\1.txt"
    
    picType = NiPicTypeImport(strIP, strDBName, strUser, strPassword)
    picType.listAll(srcDir, destFile)
    picType.dbImport(destFile)







