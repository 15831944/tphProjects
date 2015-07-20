# -*- coding: cp936 -*-
'''
Created on 2012-11-26
@author: liuxinxing
'''
import os
import psycopg2

# 将sar元数据里的所有png图片名列出
# srcDir: sar元数据的路径
# outputCSV: all_sar_files_name.csv，要输出的文件名
def Function1(srcDir, outputCSV):
    if(os.path.exists(srcDir) == False):
        print "source directory does not exist: " + srcDir
        return
    
    sarList = []
    for curDir,dirNames,fileNames in os.walk(srcDir):
        for oneFile in fileNames:
            if oneFile[-4:] == ".svg":
                sarList.append(oneFile)
    
    sarCount = len(sarList)
    oFStream = open(outputCSV, "w")
    for sarFile in sarList:
        oFStream.write(sarFile + "\n")
    return

if __name__ == '__main__':
    Function1('''C:\\My\\20150514_mea_pic_2014Q4\\ori_data\\2DS_2014Q4_MEA''',
              '''C:\\My\\20150514_mea_pic_2014Q4\\sar_svg_list.csv''')
    pass