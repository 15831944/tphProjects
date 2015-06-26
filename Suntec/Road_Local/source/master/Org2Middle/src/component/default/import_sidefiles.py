# -*- coding: UTF-8 -*-
'''
Created on 2014-6-6

@author: yuanrui
'''
import os
import codecs
import string

import component.component_base
import common.common_func

class import_csv_data(component.component_base.comp_base):
    
    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'ImportCSV')
    
    def _Do(self):

        csv_path = common.common_func.GetPath('csv_path')
        
        if csv_path:

            self.__load_csv_data(csv_path)
        else:
            return 0
    
    def __load_csv_data(self, path):
        
        listsub = os.listdir(path) 
        
        for sub in listsub:
            
            subpath = os.path.join(path, sub)
            self.pre_name = 'gewi'
                    
            if os.path.isfile(subpath):             
                self.__parse_csv_file(self.pre_name, subpath)
            else:
                for ssub in os.listdir(subpath):
                    ssubpath = os.path.join(subpath, ssub)
                    
                    if os.path.isfile(ssubpath):
                        self.__parse_csv_file(self.pre_name, ssubpath)    

    def __parse_csv_file(self, pre_name, file_full_path):
        
        file_name = os.path.basename(file_full_path)

        split_ext_name = (os.path.splitext(file_name))[0]

        # Get table name. if up one level folder exists, table name will be FolderName_FileName.
        if pre_name <> '':
            table_name = pre_name + '_' + split_ext_name[:len(split_ext_name)]
        else:
            table_name = split_ext_name[:len(split_ext_name)]
        
        split_str_list = [
                          ';',
                          '|',
                          ',',
                          '/']   
        
        objFile = codecs.open(file_full_path, 'r', 'utf-8')
        listline = objFile.readlines()
        objFile.close()

        sqlStr = 'drop table if exists ' + table_name + ';'
        sqlStr = sqlStr + ' create table ' + table_name + '('
        
        for split_str in split_str_list:                  
            column_set = str(listline[0]).encode('utf-8').strip('\n').strip('\r').split(split_str)
            if len(column_set) > 1:
                self.split_str = split_str
                break
                
        for i in range(0, len(column_set)):            
            if i == len(column_set) - 1:
                sqlStr = sqlStr + column_set[i] + ' character varying'
            else:
                sqlStr = sqlStr + column_set[i] + ' character varying,'                     
            i = i + 1        
        sqlStr = sqlStr + ');'

        sqlcmd = "select * from pg_tables where tablename = '" + string.lower(table_name) + "';"

        self.pg.execute2(sqlcmd)
        rowcnt = self.pg.getcnt2()            
        if rowcnt == 0:
            # Create table, columns come from the first line of csv file.            
            self.pg.execute2(sqlStr)
            self.pg.commit2()            

        # Get contents of table.
        tmpfile = codecs.open('temp.csv', 'w')
        for line in listline[1:]:
            tmpfile.writelines(line)           
        tmpfile.close()         
        
        # Insert contents to table.               
        copyFile = codecs.open('temp.csv', 'r')              
        self.pg.copy_from2(copyFile, table_name, self.split_str)
        self.pg.commit2()        
        copyFile.close()
        
        os.remove('temp.csv')