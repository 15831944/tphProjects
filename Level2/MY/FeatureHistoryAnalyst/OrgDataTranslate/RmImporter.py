# -*- coding: cp936 -*-

import os
import os.path
import shutil
import psycopg2
from CommonImporter import CommonImporter

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
class RmImporter(CommonImporter):
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
                # ֻ����csv�ļ�
                if curFile[-4:] != '.csv':
                    continue
                # ֻ����rm��Լ��
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





