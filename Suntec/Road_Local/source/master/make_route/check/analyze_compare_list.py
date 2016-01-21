# -*- coding: UTF8 -*-

import os
import xlrd
import re
from common import log
from check.TAP.tap_processor import *

class compare_list_comp():
    def __init__(self, compare_list_file = '', result_list_file = '', scal_compare = 0):
        self._compare_list_file = compare_list_file
        self._result_list_file = result_list_file
        self._scal_compare = scal_compare
        
        self.log = log.common_log.instance().logger("analyze_compare_list")
        
    def init_list(self):
        if not os.path.isfile(self._compare_list_file):
            self.log.error("path %s is not file", self._compare_list_file)
            return -1
            
        if not os.path.isfile(self._result_list_file):
            self.log.error("path %s is not file", self._result_list_file)
            return -1
        
        self._compare_list_dict = dict()
        if self.get_stat_dict() == -1:
            self.log.error("get %s error!", self._compare_list_file)
            return -1 
        
        self._result_list = list()
        if self.get_result_dict() == -1:
            self.log.error("get %s error!", self._result_list_file)
            return -1
        
        return 0
    
    def get_result(self):
        return self._result_list
        
    def get_result_dict(self):

        objExcel = xlrd.open_workbook(self._result_list_file)
        for sheet in objExcel.sheets():
            if sheet.name.lower() == 'stat_compare':
                field_format = sheet.row_values(0)
                for i in range(1, sheet.nrows):
                    row_list = sheet.row_values(i)
                    key_original = row_list[field_format.index('check代码')]
                    key = key_original.lower()
                    if key in self._compare_list_dict.keys():                                                                                 
                        if self._compare_list_dict[key][0] == 0 :
                            scale = abs(self._compare_list_dict[key][2])
                        else:
                            scale = abs(self._compare_list_dict[key][2]/self._compare_list_dict[key][0])
                        if scale <= self._scal_compare:
                            self._result_list.append([TAP_OK, key_original])
                        else:
                            self._result_list.append([TAP_NG, key_original])
                    else:
                        self._result_list.append([TAP_SKIP, key_original])
    
    def get_int(self, line_data):
        return line_data if type(line_data) in (float, int) else 0
    
    def get_stat_dict(self):
        '''从excel表载入用例信息。'''
        objExcel = xlrd.open_workbook(self._compare_list_file)              
        # 取得所有sheet
        for sheet in objExcel.sheets():
            if sheet.name.lower() == 'compare':
                field_format = sheet.row_values(0)
                special_num_list = [0,0,0]
                special_Flag = False
                special_key = ''
                for i in range(1, sheet.nrows):
                    row_list = sheet.row_values(i)
                    if row_list[field_format.index("Country")] == "ALL":
                        if row_list[field_format.index("type")] in ("shield", "language", "phoneme"):
                            special_Flag = True
                            special_key_temp = row_list[field_format.index('Table')] + "::" + \
                                               row_list[field_format.index('Field')] + "!=null&" + \
                                               row_list[field_format.index("type")] + "::count"
                            if not special_key or special_key == special_key_temp:
                                special_key = special_key_temp
                                special_num_list[0] += self.get_int(row_list[field_format.index('result_A')])
                                special_num_list[1] += self.get_int(row_list[field_format.index('result_B')])
                                special_num_list[2] += self.get_int(row_list[field_format.index('Differ')])
                            else:
                                self._compare_list_dict[special_key.lower()] = special_num_list
                                special_key = special_key_temp
                                special_num_list[0] = self.get_int(row_list[field_format.index('result_A')])
                                special_num_list[1] = self.get_int(row_list[field_format.index('result_B')])
                                special_num_list[2] = self.get_int(row_list[field_format.index('Differ')])
                            continue
                        
                        if special_Flag:
                            special_Flag = False
                            self._compare_list_dict[special_key.lower()] = special_num_list
                        
                        if row_list[field_format.index("Field")] == "*":
                            if row_list[field_format.index('type')] == "total":
                                key = row_list[field_format.index('Table')] + "::all::count" 
                            elif row_list[field_format.index('type')] == "length": 
                                key = row_list[field_format.index('Table')] + "::all::length"                         
                            else:
                                self.log.error("row %s error! ", row_list)
                        elif row_list[field_format.index("type")] == "bit1"  :                  
                            field_temp = row_list[field_format.index("Field")]
                            p = re.compile(r'\d\-\d')
                            match = p.findall(field_temp)[0]
                            first_num = int(match[0:match.find('-')])
                            end_num = int(match[match.find('-') + 1:])
                            if first_num == end_num:
                                key = row_list[field_format.index('Table')] + "::" + field_temp[:len(field_temp)-len(match)-2] + \
                                      "::bit(" + match + ")=true::count"
                            else:
                                key = row_list[field_format.index('Table')] + "::" + field_temp[:len(field_temp)-len(match)-2] + \
                                      "::bit(" + match + ")=" + row_list[field_format.index('Value')] + "::count"                                
                        elif row_list[field_format.index("type")] == "bit" :
                             key = row_list[field_format.index('Table')] + "::" + row_list[field_format.index("Field")] + "::" +\
                                      "bit(" + row_list[field_format.index('Value')] + ")=true::count"   
                        else:
                            key = row_list[field_format.index('Table')] + "::" + \
                                  row_list[field_format.index('Field')] + "=" + row_list[field_format.index('Value')] + "::" + \
                                  row_list[field_format.index('type')]
                            
                        self._compare_list_dict[key.lower()] = [self.get_int(row_list[field_format.index('result_A')]), \
                                                                self.get_int(row_list[field_format.index('result_B')]), \
                                                                self.get_int(row_list[field_format.index('Differ')])] 
                
                if special_Flag:
                    special_Flag = False
                    self._compare_list_dict[special_key.lower()] = special_num_list
        return 0
            
                  