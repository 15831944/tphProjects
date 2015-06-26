# -*- coding: cp936 -*-
'''
Created on 2011-11-1

@author: hongchenzai
'''
import os
import common.common_func
import common.config

class SqlScriprt(object):
    __instance = None
    @staticmethod
    def instance():
        if SqlScriprt.__instance is None:
            SqlScriprt.__instance = SqlScriprt()
            SqlScriprt.__instance.init()
        return SqlScriprt.__instance
    
    def __init__(self):
        self.sql_srcipts = []

    def init(self):
        path = common.config.CConfig.instance().getPara('sql_script')
        proj_code = common.config.CConfig.instance().getPara('proj_name')
        self.LoadSqlScriprts(os.path.join(path,'Middle'))
        self.LoadSqlScriprts(os.path.join(path,proj_code))

    def LoadSqlScriprts(self, path):
        # 读common文件下的所有SQL，脚本文件
        for dirpath, dirnames, filenames in os.walk(path):
            for f in filenames:
                if f[-4:] == '.sql' :  # 后缀名为.sql的文件
                    file_path = os.path.join(dirpath, f)
                    lines = common.common_func.readlines(file_path)
                    self.sql_srcipts += lines

    def GetSqlScript(self, name):
        sqlcmd = ''
        found_flag = False
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
                    sqlcmd += line
                    found_flag = True
                    if lowline.find('function') >= 0:
                        function_flag = True

#                char_after_name = line[pos + len(name)]
#                # 关键后面跟的字符是空格or Tab键盘 or 换行 or '('
#                if  line.lower().find('create') >= 0 and \
#                    (char_after_name == '' or char_after_name == '\t' or char_after_name == '\n' or char_after_name == '('):
#                    sqlcmd    += line
#                    found_flag = True
#                    if line.lower().find('function') >= 0:
#                        function_flag = True
            else:
                sqlcmd += line
                if function_flag == True:
                    if line.find('$$;') >= 0 or line.find('$BODY$;') >= 0:  # '$$;' 函数结束 标志
                        break
                else:
                    if line.rfind(';') >= 0:  # ';' sql语名的结束 标志
                        break

        if sqlcmd == '':
            return None

        return sqlcmd
