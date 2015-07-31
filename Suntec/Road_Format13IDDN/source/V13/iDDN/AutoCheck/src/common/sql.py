# -*- coding: UTF8 -*-
'''
Created on 2012-12-3

@author: yuanrui
'''

import os
import common.ConfigReader


class CSqlLoader(object):
    def __init__(self):
        self.sql_srcipts = []
    
    def init(self):
        path = common.ConfigReader.CConfigReader.instance().getPara('sql_script')
        self.LoadSqlScriprts(path)
    
    def LoadSqlScriprts(self, path):        
        # 读common文件下的所有SQL，脚本文件
        for dirpath, dirnames, filenames in os.walk(path):
            for f in filenames:
                if f[-4:] == '.sql' :  # 后缀名为.sql的文件
                    file_path = os.path.join(dirpath, f)
                    lines     = common.ConfigReader.CConfigReader.instance().readlines(file_path)
                    self.sql_srcipts += lines
                
    def GetSqlScript(self, name):
        sqlcmd        = ''
        found_flag    = False
        function_flag = False
        
        for line in self.sql_srcipts:
            if found_flag == False:
                lowline = line.lower()
                lowname = name.lower()
                
                pos = lowline.find(lowname)
                if pos < 0:
                    continue
                
                if lowline.find('create') == -1:
                    continue
                
                if lowline.find("(") != -1:
                    lowline = lowline[:line.find("(")]
                
                keyword = lowline.split()[-1]
                if keyword == lowname:
                    sqlcmd    += line
                    found_flag = True
                    if lowline.find('function') >= 0:
                        function_flag = True

            else:
                sqlcmd += line
                if function_flag == True:
                    if line.find('$$;') >=0 or line.find('$BODY$;') >=0:           # '$$;' 函数结束 标志
                        break
                else:
                    if line.rfind(';') >=0:     # ';' sql语名的结束 标志
                        break
        
        if sqlcmd == '':
            return None
          
        return sqlcmd
