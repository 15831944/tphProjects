# -*- coding: cp936 -*-
'''
Created on 2011-11-1

@author: hongchenzai
'''
import common
import os


class SqlScriprt(object):
    def __init__(self):
        self.sql_srcipts = []

    def init(self):
        path = common.GetPath('sql_script')
        path = os.path.join(path, 'Middle')
        self.LoadSqlScriprts(path)

    def LoadSqlScriprts(self, path):
        # ��common�ļ��µ�����SQL���ű��ļ�
        for dirpath, dirnames, filenames in os.walk(path):
            for f in filenames:
                if f[-4:] == '.sql' :  # ��׺��Ϊ.sql���ļ�
                    file_path = os.path.join(dirpath, f)
                    lines = common.readlines(file_path)
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

class SqlScriprt_jpn(SqlScriprt):
    def __init__(self):
        SqlScriprt.__init__(self)

    def init(self):
        SqlScriprt.init(self)

        path = common.GetPath('sql_script')
        path = os.path.join(path, 'jpn')
        self.LoadSqlScriprts(path)

class SqlScriprt_jdb(SqlScriprt):
    def __init__(self):
        SqlScriprt.__init__(self)

    def init(self):
        SqlScriprt.init(self)

        path = common.GetPath('sql_script')
        path = os.path.join(path, 'jdb')
        self.LoadSqlScriprts(path)

class SqlScriprt_chn(SqlScriprt):
    def __init__(self):
        SqlScriprt.__init__(self)

    def init(self):
        SqlScriprt.init(self)

        path = common.GetPath('sql_script')
        path = os.path.join(path, 'chn')
        self.LoadSqlScriprts(path)

class SqlScriprt_rdf(SqlScriprt):
    def __init__(self):
        SqlScriprt.__init__(self)

    def init(self):
        SqlScriprt.init(self)

        path = common.GetPath('sql_script')
        path = os.path.join(path, 'rdf')
        self.LoadSqlScriprts(path)

class SqlScriprt_axf(SqlScriprt):
    def __init__(self):
        SqlScriprt.__init__(self)

    def init(self):
        SqlScriprt.init(self)

        path = common.GetPath('sql_script')
        path = os.path.join(path, 'axf')
        self.LoadSqlScriprts(path)

class SqlScriprt_ta(SqlScriprt):
    def __init__(self):
        SqlScriprt.__init__(self)

    def init(self):
        SqlScriprt.init(self)

        path = common.GetPath('sql_script')
        path = os.path.join(path, 'ta')
        self.LoadSqlScriprts(path)


class SqlScriprt_nostra(SqlScriprt):
    def __init__(self):
        SqlScriprt.__init__(self)

    def init(self):
        SqlScriprt.init(self)

        path = common.GetPath('sql_script')
        path = os.path.join(path, 'nostra')
        self.LoadSqlScriprts(path)


class SqlScriprt_mmi(SqlScriprt):
    def __init__(self):
        SqlScriprt.__init__(self)

    def init(self):
        SqlScriprt.init(self)

        path = common.GetPath('sql_script')
        path = os.path.join(path, 'mmi')
        self.LoadSqlScriprts(path)


class SqlScriprt_msm(SqlScriprt):
    def __init__(self):
        SqlScriprt.__init__(self)

    def init(self):
        SqlScriprt.init(self)

        path = common.GetPath('sql_script')
        path = os.path.join(path, 'msm')
        self.LoadSqlScriprts(path)
