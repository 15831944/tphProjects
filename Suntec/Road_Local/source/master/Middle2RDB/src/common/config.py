# -*- coding: UTF8 -*-
'''
Created on 2015-1-22

@author: liuxinxing
'''

class CConfig(object):
    __instance = None
    @staticmethod
    def instance():
        if CConfig.__instance is None:
            CConfig.__instance = CConfig()
            CConfig.__instance.load('DataBasePath.txt')
        return CConfig.__instance
    
    def __init__(self):
        '''
        Constructor
        '''
        self.lines = []
        self.paras = {}
    
    def load(self, strConfigPath='config.ini'):
        self.lines = self.readlines(strConfigPath)
        for line in self.lines:
            if line[0] in ('#', ' '):
                pass
            else:
                split_pos = line.find('=')
                if split_pos > 0:
                    key = line[:split_pos]
                    value = line[split_pos+1:].strip()
                    self.paras[key] = value
    
    def getPara(self, name):
        if self.paras.has_key(name):
            return self.paras[name]
        else:
            return ''

    def getProjName(self):
        return self.getPara('proj_name')

    def getProjCountry(self):
        return self.getPara('proj_country')
    
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
    