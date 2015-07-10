# -*- coding: UTF-8 -*-
'''
Created on 2014-6-27

@author: zhangyongmu
'''
import io
import os
import component.component_base
import common.config

class comp_guideinfo_building(component.component_base.comp_base):
    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Guideinfo_building')

    def _DoCreateFunction(self):
        pass
    
    def _DoCreateTable(self):
        self.CreateTable2('building_structure')
        self.CreateTable2('mid_logmark')
        return 0
    
    def _Do(self):
        
        return 0
    
    def _loadBrandIcon(self):                            
        self.log.info('make temp_brand_icon...')
        self.CreateTable2('temp_brand_icon')
        brand_icon_file_path = common.config.CConfig.instance().getPara('brand_icon')
        for line in open(brand_icon_file_path):
                
                if line[0] == '#':
                    continue
                line = line.strip()
                fields=[]
                fields.append(line)               
                sqlcmd = '''
                          insert into temp_brand_icon values(%s);
                         '''
                self.pg.execute(sqlcmd, fields)
        self.pg.commit()
                                      
        
    def _loadPOICategory(self):
        self.log.info('make temp_poi_category...')
        self.CreateTable2('temp_poi_category')
        
        category_file_path = common.config.CConfig.instance().getPara('POI_Code')
        for line in open(category_file_path):
            line = line.strip()
            if line[0] == '#':
                continue
            fields = line.split(';')
            lens = len(fields)
            string = '%s,'*(lens-1)+'%s'
            sqlcmd = '''
                      insert into temp_poi_category values(%s);
                     '''  % string
            self.pg.execute(sqlcmd, fields)
        self.pg.commit()
        
    
    def _loadCategoryPriority(self):
        self.log.info('make temp_category_priority...')
        self.CreateTable2('temp_category_priority')
        category_priority_file_path = common.config.CConfig.instance().getPara('category_priority')
        for line in open(category_priority_file_path):            
            line = line.strip()               
            if line[0] == '#':
                continue
            fields = line.split(';')              
            sqlcmd = '''
                      insert into temp_category_priority values(%s,%s);
                     '''
            self.pg.execute(sqlcmd, fields)
        self.pg.commit()  
 
 
 
 
 
 
 
    