# -*- coding: UTF8 -*-
'''
Created on 2012-11-29

@author: hongchenzai
'''
import os

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
        self.shield_lines = ''
        self.country_language_code_dict = {}
    
    def load(self, strConfigPath='config.ini'):
        self.lines = self.readlines(strConfigPath)
    
    def getPara(self, name, lines = None):
        if not lines:
            lines = self.lines
            
        path = ""
        for line in lines:
            if line.find(name + '=') == 0:
                path = line[len(name + '='):].strip()
                break
        return path

    def getProjName(self):
        return self.getPara('proj_name')

    def getCountryName(self):
        return self.getPara('country_name')
    
    def load_shield(self):
        checklist_path = self.getPara('checklist')
        file_temp = os.path.join(os.path.dirname(checklist_path), 'shield_id.ini')        
        self.shield_lines = self.readlines(file_temp)
    
    def load_country_language_code(self):
        language_lines = []
        checklist_path = self.getPara('text_filter_path')
        file_temp_country = os.path.join(checklist_path, 'config_' + self.getProjName().lower() + '_' + self.getCountryName().lower() + '.txt')
        if os.path.exists(file_temp_country):
            language_lines += self.readlines(file_temp_country)        
        file_temp = os.path.join(checklist_path, 'config_' + self.getProjName().lower() + '.txt')
        if os.path.exists(file_temp):
            language_lines += self.readlines(file_temp)
        temp_key = (self.getProjName() + "_" + self.getCountryName()).upper()
        temp_dict = eval(self.getPara('text_filter', language_lines))
        if temp_dict is None:
            return []
        else:
            text_filter_list = temp_dict[temp_key].split('||')
            for text_filter in text_filter_list:
                key = text_filter[:text_filter.find(':')].upper()
                value = text_filter[text_filter.find('[') + 1:text_filter.find(']')].upper()
                self.country_language_code_dict[key] = value.split(',')

    def getShield_id_list(self, country_name = ''):
        if len(self.shield_lines) == 0:
            self.load_shield()
        
        shield_id = self.getPara(country_name.lower(), self.shield_lines)
        return shield_id.split(',')
        
    
    def getcountry_language_code(self, country_name = ''):
        if len(self.country_language_code_dict) == 0:
            self.load_country_language_code()
        if self.country_language_code_dict.has_key(country_name):
            return self.country_language_code_dict[country_name]
        else:
            return self.country_language_code_dict['DEFAULT']
            
   
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
    