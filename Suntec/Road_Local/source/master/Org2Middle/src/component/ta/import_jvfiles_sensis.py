# -*- coding: UTF-8 -*-

import io
import os
import StringIO

import component.component_base
import common.common_func

class comp_import_siedfiles_sensis(component.component_base.comp_base):

    def __init__(self):

        component.component_base.comp_base.__init__(self, 'sidefiles')
        
    def _DoCreateTable(self):
        if self.CreateTable2('mid_sensis_jv_tbl') == -1:
            return -1
                                
        return 0    
    
    def _Do(self):
        # first step : import junction view from txt file
        if self._import_sensis_jv_data() == -1:
            self.log.error('Could not open sensis junction view files correctly!!!') 
            return -1
        
        return 0
    
    def _import_sensis_jv_data(self):
        sf_path = common.common_func.GetPath('ta_jv_sidefile')
        
        pg = self.pg
        pgcur = self.pg.pgcur2
        
        file_lists = self._access_files_in_folder(sf_path)
        
        for file in file_lists:
            f_jv = io.open(file, 'r', 8192, 'utf-8')
            if f_jv is None:
                return -1
            
            pgcur.copy_from(f_jv, 'mid_sensis_jv_tbl', '|', "", 8192)
            pg.commit2()
            f_jv.close()
            
        return 0
    
    def _access_files_in_folder(self, rootdir):
        file_list = []
        
        for parent,dirnames,filenames in os.walk(rootdir):    
            for filename in filenames:
                if filename is not None:
                    file_list.append(os.path.join(parent,filename)) 
                    
        return file_list
