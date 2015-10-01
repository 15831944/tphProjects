# -*- coding: UTF8 -*-
'''
Created on 2012-11-29

@author: hongchenzai
'''

class CConfigReader(object):
    '''配置文件读取类
    '''
    __instance = None
    @staticmethod
    def instance():
        if CConfigReader.__instance is None:
            CConfigReader.__instance = CConfigReader()
        return CConfigReader.__instance
    
    def __init__(self):
        '''
        Constructor
        '''
        self.lines = []
    
    def load(self, strConfigPath='config.ini'):
        self.lines = self.readlines(strConfigPath)
    
    def getPara(self, name):
        path = ""
        for line in self.lines:
            if line.find(name + '=') == 0:
                path = line[len(name + '='):].strip()
                break
        return path

    def getProjName(self):
        return self.getPara('proj_name')

    def getCountryName(self):
        return self.getPara('country_name')
    
    def getDBName(self):
        return self.getDBInfo()[1]
    
    def getDBInfo(self):
        '''取得数据的名称和地址。'''
        db = self.getPara('db')

        for s in db.split():
            if s.find('host=') == 0:
                host = s[len("host='") : s.rfind("'")]
            if s.find('dbname=') == 0:
                dbname = s[len("dbname='") : s.rfind("'")]
            
        return (host, dbname)
    
    def readlines(self, path):
        lines = []
        file_object = open(path, 'r')
        
        try:
            lines = file_object.readlines()
        except:
            file_object.close()
            #Logger('common_func', "Doesn't exist file DataBasePath.txt")
            exit(1)
        finally:
            file_object.close()
     
        return lines
    