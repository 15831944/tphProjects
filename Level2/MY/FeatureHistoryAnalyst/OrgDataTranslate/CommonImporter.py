# -*- coding: cp936 -*-

import os
import os.path
import shutil
import psycopg2

class CommonImporter(object):
    def __init__(self, connectStr):
        if connectStr == '' or connectStr == None:
            print """Connect string error: %s """ % connectStr
        self.conn = psycopg2.connect(connectStr)
        self.pg = self.conn.cursor()

    def __del__(self):
        return

    # ��ȡ��ȷ�ĺ�Լ�������1509������1601�������������Ϣ��
    # ԭʼ����û���ṩ��Լ���ֻ�ṩ��Լ�·ݣ����Լ��������ݵ����ڼ��㾫ȷ��Լ����·ݡ�
    # ����: RM07_20150731.csv
    # ����RM07�Ǻ�Լ����ֻ�ܻ���·ݣ��޷������ݣ�20150731�����ݲɼ����ڡ�
    # �ڻ������й涨��ɢ���޷����뽻���·ݣ��˴��Ĳɼ������ǣ����ɼ��·�>=��Լ�·ݣ���Լ���Ϊ�ɼ����+1��
    # ��RM05Ϊ����
    # 13��5�µ�ǰ10�����������ݣ�RM05_20130502��RM05_20130503��RM05_20130507��RM05_20130513��RM05_20130515
    # Ӧ����rm1305��Լ�������ڱ��ɼ������лᱻ�ɼ���rm1405��Լ�С�
    # ���ּ��㷽����Ȼ������һ��������������10�������յ����ݲ�Ӱ��ɢ�����ʿɽ��ܡ�
    def getExactContract(self, dataCsvName):
        strSplit = dataCsvName.split('_')
        if len(strSplit) != 2:
            print """There is one csv file with wrong name format: %s""" % dataCsvName

        curYear = int(strSplit[1][2:4])
        curMonth = int(strSplit[1][4:6])
        curDay = int(strSplit[1][6:])
        contractName = strSplit[0][:-2:]
        contractMonth = int(strSplit[0][-2:])

        if curMonth >= contractMonth:
            return """%s%02d%02d""" % (contractName, curYear+1, contractMonth)
        else:
            return """%s%02d%02d""" % (contractName, curYear, contractMonth)
       
    # ��ÿ����Լ�ļ��������ݿ�
    def middleCsvToDB(self, srcDir):
        if os.path.isdir(srcDir) == False:
            print """The input file directory dose not exist. %s\n""" % srcCsvDir

        # ����ԭʼ�����ļ��С�
        for curDir, subDirs, fileNames in os.walk(srcDir):
            for curFile in fileNames:
                if curFile[-4:] != '.csv': # ֻ����csv�ļ�
                    continue
                if curFile.lower().find('rm') == -1: # ֻ����rm��Լ��
                    continue
                if curFile.lower().find('mi_') != -1: # ������rmmi�ļ������ļ���¼����������Լ��
                    continue
                curContract = self.getExactContract(curFile.lower()[:-4:])
                curCsvPath = os.path.join(curDir, curFile).lower()
                self.importCsvToDB(curContract, curCsvPath)
        return

    # ���ú�Լ�����������ӵ����ݿ�
    def importCsvToDB(self, contractName, csvFilePath):
        copySqlCmd = """copy %s from '%s' delimiter as ',' csv quote as '"';"""
        self.createTableByNameIfNotExist(contractName)

        tempCsvFilePath = csvFilePath + "_temp"
        with open(tempCsvFilePath, "w") as oFStream:
            with open(csvFilePath, "r") as iFStream:
                lines = iFStream.readlines()[1:]
                while(True):
                    line = lines[-1]
                    if lines[-1] != "\n":
                        break
                    else:
                        del lines[-1]
                oFStream.writelines(lines)
        sqlCmd = copySqlCmd % (contractName, tempCsvFilePath)
        self.pg.execute(sqlCmd)
        self.conn.commit()
        os.remove(tempCsvFilePath)
        return
        
    # ����ú�Լ��Ӧ�ı����ڣ��򴴽�������ݱ�
    def createTableByNameIfNotExist(self, strTblName):
        self.pg.execute("""select count(*) from pg_class where relname = '%s';""" % strTblName)
        row = self.pg.fetchone()
        if row[0] >= 1: # �������������������ı��˳���
            return

        # �����ñ�
        strSqlCmd = """
                    drop table if exists %s;
                    create table %s
                    (
                      datet date,
                      timet time without time zone,
                      pricet numeric,
                      volt integer,
                      totalvolt integer,
                      positiont integer,
                      b1pricet numeric,
                      b1volt integer,
                      b2pricet numeric,
                      b2volt integer,
                      b3pricet numeric,
                      b3volt integer,
                      s1pricet numeric,
                      s1volt integer,
                      s2pricet numeric,
                      s2volt integer,
                      s3pricet numeric,
                      s3volt integer,
                      bst character(1)
                    )
                    """\
                    %(strTblName, strTblName)
        self.pg.execute(strSqlCmd)
        self.conn.commit()
        return
if __name__ == "__main__":
    test = CommonImporter("host=127.0.0.1 dbname=zz user=postgres password=pset123456")
    test.middleCsvToDB(r"""E:\features\tick""")





