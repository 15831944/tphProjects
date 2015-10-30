# -*- coding: UTF-8 -*-
'''
Created on 2014-6-27

@author: zhangyongmu
'''
import io
import os
import component.component_base
import common.config
import xlrd


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
        self._loadPOICategory_ni()
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
        category_file_path = common.config.CConfig.instance().getPara('POI_Code')
        if category_file_path:
#            self.log.info('make temp_poi_category...')
            self.CreateTable2('temp_poi_category')
            for line in open(category_file_path):
                line = line.strip()
                if line[0] == '#':
                    continue
    
                fields = line.split(';')
                
                lens = len(fields)
                
                for i in range(lens):
                    if fields[i] in ('','null') :
                        fields[i]= None
                
                string = '%s,'*(lens-1)+'%s'
                sqlcmd = '''
                          insert into temp_poi_category values(%s);
                         '''  % string
    
                self.pg.execute(sqlcmd, fields)
            self.pg.commit() 
            
                                                 
    def _loadPOICategory_ni(self):
        self.log.info('make temp_poi_category...')
        category_file_code_path = common.config.CConfig.instance().getPara('POI_category_code')
        category_file_name_path = common.config.CConfig.instance().getPara('POI_category_name')
        
        self.log.info('make temp_poi_category_code...')
        self.__loadCategory_code_name(category_file_code_path,'code')
        
        self.log.info('make temp_poi_category_name...')
        self.__loadCategory_code_name(category_file_name_path,'name')

            
            
    def __loadCategory_code_name(self,category_file_path,types):
        
        if category_file_path:
            table_name = 'temp_poi_category_' + types
            self.CreateTable2(table_name)
            for line in open(category_file_path):
                line = line.strip()
                if line[0] == '#':
                    continue
    
                fields = line.split(';')
                
                lens = len(fields)
                
                for i in range(lens):
                    if fields[i] in ('','null') :
                        fields[i]= None
                
                string = '%s,'*(lens-1)+'%s'
                sqlcmd = '''
                          insert into  %s  values(%s);
                         '''  % (table_name,string)
    
                self.pg.execute(sqlcmd, fields)  
            self.pg.commit() 
        
        
              
  
    def _loadCategoryPriority(self):
        self.log.info('make temp_category_priority...')
        self.CreateTable2('temp_org_category_priority')
        category_priority_file_path = common.config.CConfig.instance().getPara('category_priority')
        
        data = xlrd.open_workbook(category_priority_file_path)
        table = data.sheets()[0]  
        nrows = table.nrows
        for i in range(nrows ):   
            line=table.row_values(i)
            if 'org_code' in line:
                continue 
            try:
                line=[int(line[0]),int(line[1])]
            except:
                pass
            line=[str(line[0]),int(line[1])] 
            
            sqlcmd= """
                        insert into temp_org_category_priority values(%s,%s)              
                    """
            self.pg.execute(sqlcmd,line)
        self.pg.commit()    
                 
        
        sqlcmd="""
                drop table if exists temp_category_priority;
                create table temp_category_priority
                as
                (
                    select per_code,category_priority
                    from
                    (                
                       select  per_code, 
                               (case when b.priority is null then '11'
                                    else b.priority end
                               ) as category_priority
                       from  temp_poi_category as a
                       left join temp_org_category_priority as b
                       on a.org_code::varchar = b.org_code::varchar
                         
                    ) as kk
                    order by category_priority::integer, per_code 
                )
               """  
        self.pg.execute(sqlcmd)
        self.pg.commit()
 
 
    