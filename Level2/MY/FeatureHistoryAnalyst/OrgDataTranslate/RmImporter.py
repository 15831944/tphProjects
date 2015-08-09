# -*- coding: cp936 -*-

import os
import os.path
import shutil
import psycopg2
from CommonImporter import CommonImporter

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
class RmImporter(CommonImporter):
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
                # 只处理csv文件
                if curFile[-4:] != '.csv':
                    continue
                # 只处理rm合约。
                if curFile.lower().find('rm01') == -1 and\
                   curFile.lower().find('rm05') == -1 and\
                   curFile.lower().find('rm07') == -1 and\
                   curFile.lower().find('rm09') == -1 and\
                   curFile.lower().find('rm11') == -1: 
                    continue
                curContract = curFile.lower()[0:4]
                curFilePath = os.path.join(curDir, curFile).lower()
                if theDict.has_key(curContract) == False:
                    theDict[curContract] = [curFilePath]
                else:
                    theDict[curContract].append(curFilePath)

        for curContract, tickCsvList in theDict.items():
            outputCsvFile = os.path.join(destDir, curContract) + '.csv'
            with open(outputCsvFile, 'w+') as oFStream:
                tickCsvList.sort()
                for oneTickCsv in tickCsvList:
                    with open(oneTickCsv, 'r') as iFStream:
                        lines = iFStream.readlines()[1:]
                        while(True):
                            line = lines[-1]
                            if lines[-1] != "\n":
                                break
                            else:
                                del lines[-1]
                        oFStream.writelines(lines)

if __name__ == "__main__":
    test = RmImporter("host=127.0.0.1 dbname=zz user=postgres password=pset123456")
    test.orgCsv2MiddleCsv(r"""E:\features\tick""",
                          r"""E:\features\rm_output""")





