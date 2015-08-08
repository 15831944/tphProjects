# -*- coding: cp936 -*-

import os
import os.path
import shutil
import psycopg2
from CommonTranslater import CommonTranslater

# 菜粕合约处理类。
# 交易品种：菜籽粕
# 交易单位：10吨/手
# 报价单位：元（人民币）/吨
# 最小变动价位：１元／吨
# 涨跌停板幅度：上一交易日结算价+—4%及《郑州商品交易所期货交易风险控制管理办法》相关规定
# 合约交割月份：1、3、5、7、8、9、11月
# 交易时间：每周一至周五（北京时间 法定假日除外）
#          上午：9:00-10:15 10:30-11:30 
#          下午：1:30-3:00 
#          连续交易时间：21:00至23:30
# 最后交易日：合约交割月份的第10个交易日
# 最后交割日：合约交割月份的第12个交易日
# 交割品级：见《郑州商品交易所期货交割细则》
# 交割地点：交易所指定交割地点
# 最低交易保证金：合约价值的5%
# 交割方式：实物交割
# 交易代码：RM
# 上市日期：2012年12月28日
class RmTranslater(CommonTranslater):
    def __init__(self, connectStr):
        CommonTranslater.__init__(self, connectStr)
        return

    def __del__(self):
        return
    
    # 原始csv转化至中间csv。
    # 原始数据按日期独立分成多个csv文件，将这些文件按合约合成若干个大的csv文件。
    def orgCsv2MiddleCsv(self, srcDir, destDir):
        if os.path.isdir(srcDir) == False:
            print """The input file directory dose not exist. %s\n""" % srcCsvDir
        if os.path.isdir(destDir) == True:
            shutil.rmtree(destDir)
        os.mkdir(destDir)

        theDict = {}
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
                curFilePath = os.path.join(curDir, curFile).lower()
                if theDict.has_key(curContract) == False:
                    theDict[curContract] = [curFilePath]
                else:
                    theDict[curContract].append(curFilePath)

        for curContract, tickCsvList in theDict.items():
            outputCsvFile = os.path.join(destDir, curContract) + '.csv'
            with open(outputCsvFile, 'w+') as oFStream:
                for oneTickCsv in tickCsvList:
                    with open(oneTickCsv, 'r') as iFStream:
                        lines = iFStream.readlines()[1:]
                        while(True):
                            line = lines[-1]
                            if lines[-1] != "\n":
                                break
                            del lines[-1]
                        oFStream.writelines(lines)

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

if __name__ == "__main__":
    test = RmTranslater("host=127.0.0.1 dbname=zz user=postgres password=pset123456")
    test.orgCsv2MiddleCsv(r"""E:\features\tick""",
                          r"""E:\features\rm_output""")





