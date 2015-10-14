# -*- coding: cp936 -*-
# 配置文件中有很多配置项配置至idata服务器，为确保正确，此处检查各个与idata服务器相关
# 的配置项，确保它们都能正确连接。
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
                if oneLine[0] == '#': # 注释配置项，不关注。
                    continue
                argList.append(oneLine.split('='))
            
            print "%d------------------------------------------"%i
            print "check file '%s' start." % oneConfigFile
            bOk = True
            for oneArg in argList:
                if oneArg[0] not in interestArgList: # 不是我们关注的配置项
                    continue
                
                if os.path.isdir(oneArg[1]) == True: # 此配置项的值是一个可访问的合法文件夹路径
                    continue
                
                if os.path.isfile(oneArg[1]) == True: # 此配置项的值是一个可访问的合法文件
                    continue
                    
                    bOk = False
                    print "error: %s, arg: %s, value: %s" %\
                    (oneConfigFile, oneArg[0], oneArg[1])
            
            if bOk:
                print "check file '%s' end, all args ok." % oneConfigFile
            else:
                print "check file '%s' end, error occurs." % oneConfigFile
            
            
                        
                    
            

    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    