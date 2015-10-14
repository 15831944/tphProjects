# -*- coding: cp936 -*-
# �����ļ����кܶ�������������idata��������Ϊȷ����ȷ���˴���������idata���������
# �������ȷ�����Ƕ�����ȷ���ӡ�
import os.path

interestArgList = [
             'gjvCsvPath',
             'ejvCsvPath',
             'sign_as_real_path',
             'all_sign_as_real_file_name',
             'illust',
             'pointlist',
             'ta_jv_sidefile',
             'ta_phoneme_sidefile',
             'forceguide_patch_full_path',
             'mdb_path'
             ]

if __name__ == '__main__':
    srcDir = r"C:\My\Road\source\master\Org2Middle\src\config"
    configFileList = []
    for curDir, subDirs, fileNames in os.walk(srcDir):
        for oneFile in fileNames:
            if oneFile.find(".txt") != -1:
                configFileList.append(os.path.join(curDir, oneFile))
    
    for i in range(0, len(configFileList)):
        oneConfigFile = configFileList[i]
        with open(oneConfigFile, 'r') as iFStream:
            argList = []
            lineList = iFStream.readlines()
            for oneLine in lineList:
                if oneLine[0] == '#': # ע�����������ע��
                    continue
                argList.append(oneLine.split('='))
            
            print "%d------------------------------------------"%i
            print "check file '%s' start." % oneConfigFile
            bOk = True
            for oneArg in argList:
                if oneArg[0] not in interestArgList: # �������ǹ�ע��������
                    continue
                
                if os.path.isdir(oneArg[1]) == True: # ���������ֵ��һ���ɷ��ʵĺϷ��ļ���·��
                    continue
                
                if os.path.isfile(oneArg[1]) == True: # ���������ֵ��һ���ɷ��ʵĺϷ��ļ�
                    continue
                    
                    bOk = False
                    print "error: %s, arg: %s, value: %s" %\
                    (oneConfigFile, oneArg[0], oneArg[1])
            
            if bOk:
                print "check file '%s' end, all args ok." % oneConfigFile
            else:
                print "check file '%s' end, error occurs." % oneConfigFile
            
            
                        
                    
            

    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    