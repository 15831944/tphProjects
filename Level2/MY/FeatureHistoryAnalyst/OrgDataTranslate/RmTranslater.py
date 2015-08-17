# -*- coding: cp936 -*-

import os
import os.path
import shutil
import psycopg2
from CommonTranslater import CommonTranslater

# ���ɺ�Լ�����ࡣ
# ����Ʒ�֣�������
# ���׵�λ��10��/��
# ���۵�λ��Ԫ������ң�/��
# ��С�䶯��λ����Ԫ����
# �ǵ�ͣ����ȣ���һ�����ս����+��4%����֣����Ʒ�������ڻ����׷��տ��ƹ���취����ع涨
# ��Լ�����·ݣ�1��3��5��7��8��9��11��
# ����ʱ�䣺ÿ��һ�����壨����ʱ�� �������ճ��⣩
#          ���磺9:00-10:15 10:30-11:30 
#          ���磺1:30-3:00 
#          ��������ʱ�䣺21:00��23:30
# ������գ���Լ�����·ݵĵ�10��������
# ��󽻸��գ���Լ�����·ݵĵ�12��������
# ����Ʒ��������֣����Ʒ�������ڻ�����ϸ��
# ����ص㣺������ָ������ص�
# ��ͽ��ױ�֤�𣺺�Լ��ֵ��5%
# ���ʽ��ʵ�ｻ��
# ���״��룺RM
# �������ڣ�2012��12��28��
class RmTranslater(CommonTranslater):
    def __init__(self, connectStr):
        CommonTranslater.__init__(self, connectStr)
        return

    def __del__(self):
        return
    
    # ԭʼcsvת�����м�csv��
    # ԭʼ���ݰ����ڶ����ֳɶ��csv�ļ�������Щ�ļ�����Լ�ϳ����ɸ����csv�ļ���
    def orgCsv2MiddleCsv(self, srcDir, destDir):
        if os.path.isdir(srcDir) == False:
            print """The input file directory dose not exist. %s\n""" % srcCsvDir
        if os.path.isdir(destDir) == True:
            shutil.rmtree(destDir)
        os.mkdir(destDir)

        theDict = {}
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

if __name__ == "__main__":
    test = RmTranslater("host=127.0.0.1 dbname=zz user=postgres password=pset123456")
    test.orgCsv2MiddleCsv(r"""E:\features\tick""",
                          r"""E:\features\rm_output""")





