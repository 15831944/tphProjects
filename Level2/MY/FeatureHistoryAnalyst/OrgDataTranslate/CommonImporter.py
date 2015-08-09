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

    # 获取精确的合约，如菜粕1509，菜粕1601，包含了年份信息。
    # 原始数据没有提供合约年份只提供合约月份，需自己根据数据的日期计算精确合约年份月份。
    # 例如: RM07_20150731.csv
    # 其中RM07是合约名，只能获得月份，无法获得年份；20150731是数据采集日期。
    # 期货交易中规定，散户无法进入交割月份，此处的采集规则是，若采集月份>=合约月份，合约年份为采集年份+1。
    # 以RM05为例：
    # 13年5月的前10个交易日数据：RM05_20130502，RM05_20130503，RM05_20130507，RM05_20130513，RM05_20130515
    # 应属于rm1305合约，但是在本采集过程中会被采集到rm1405合约中。
    # 这种计算方法虽然产生了一定的误差，但由于这10个交易日的数据不影响散户，故可接受。
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
       
    # 将每个合约文件导入数据库
    def middleCsvToDB(self, srcDir):
        if os.path.isdir(srcDir) == False:
            print """The input file directory dose not exist. %s\n""" % srcCsvDir

        # 遍历原始数据文件夹。
        for curDir, subDirs, fileNames in os.walk(srcDir):
            for curFile in fileNames:
                if curFile[-4:] != '.csv': # 只处理csv文件
                    continue
                if curFile.lower().find('rm') == -1: # 只处理rm合约。
                    continue
                if curFile.lower().find('mi_') != -1: # 不处理rmmi文件，此文件记录的是主连合约。
                    continue
                curContract = self.getExactContract(curFile.lower()[:-4:])
                curCsvPath = os.path.join(curDir, curFile).lower()
                self.importCsvToDB(curContract, curCsvPath)
        return

    # 将该合约的相关内容添加到数据库
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
        
    # 如果该合约对应的表不存在，则创建相关数据表。
    def createTableByNameIfNotExist(self, strTblName):
        self.pg.execute("""select count(*) from pg_class where relname = '%s';""" % strTblName)
        row = self.pg.fetchone()
        if row[0] >= 1: # 存在以输入名字命名的表，退出。
            return

        # 创建该表。
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





