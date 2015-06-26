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
        # ��common�ļ��µ�����SQL���ű��ļ�
        for dirpath, dirnames, filenames in os.walk(path):
            for f in filenames:
                if f[-4:] == '.sql' :  # ��׺��Ϊ.sql���ļ�
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
#                # �ؼ���������ַ��ǿո�or Tab���� or ���� or '('
#                if  line.lower().find('create') >= 0 and \
#                    (char_after_name == '' or char_after_name == '\t' or char_after_name == '\n' or char_after_name == '('):
#                    sqlcmd    += line
#                    found_flag = True
#                    if line.lower().find('function') >= 0:
#                        function_flag = True
            else:
                sqlcmd += line
                if function_flag == True:
                    if line.find('$$;') >= 0 or line.find('$BODY$;') >= 0:  # '$$;' �������� ��־
                        break
                else:
                    if line.rfind(';') >= 0:  # ';' sql�����Ľ��� ��־
                        break

        if sqlcmd == '':
            return None

        return sqlcmd
