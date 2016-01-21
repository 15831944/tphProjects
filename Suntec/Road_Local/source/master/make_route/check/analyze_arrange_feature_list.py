# -*- coding: UTF8 -*-

import os
import xlrd
from common import log
from check.TAP.tap_processor import *

class arrange_feature_list_comp():
    def __init__(self, feature_list_file = ''):
        self._feature_list_file = feature_list_file
        
        self.log = log.common_log.instance().logger("analyze_features_list")
        
    def init_list(self):
        if not os.path.isfile(self._feature_list_file):
            self.log.error("path %s is not file", self._feature_list_file)
            return -1
        
        self._result_list = list()
        if self.get_stat_dict() == -1:
            self.log.error("get %s error!", self._feature_list_file)
            return -1 

        return 0
    
    def get_result(self):
        return self._result_list
    
    def get_stat_dict(self):
        '''从excel表载入用例信息。'''
        objExcel = xlrd.open_workbook(self._feature_list_file)              
        # 取得所有sheet
        for sheet in objExcel.sheets():
            if sheet.name.lower() == 'rdb':
                field_format = sheet.row_values(0)
                big_feature_str = ''
                middle_feature_str = ''
                for i in range(3, sheet.nrows):
                    row_list = sheet.row_values(i)
                    temp_str = row_list[field_format.index('大机能')]
                    big_feature_str = temp_str if temp_str else big_feature_str
                    
                    temp_str = row_list[field_format.index('中机能')]
                    middle_feature_str = temp_str if temp_str else middle_feature_str
                    
                    key = big_feature_str + "::" + middle_feature_str + "::" + row_list[field_format.index('小机能')]
                    if row_list[field_format.index('Status')] == "Y":
                        self._result_list.append([TAP_OK, key])
                    elif row_list[field_format.index('Status')] == "N":
                        self._result_list.append([TAP_NG, key])
                    else:
                        self._result_list.append([TAP_SKIP, key])

        return 0
            
                  
            
                  