# -*- coding: UTF8 -*-
import json
import os
from common import log
from check import check_config
from check.analyze_compare_list import compare_list_comp
from check.analyze_arrange_feature_list import arrange_feature_list_comp
from check.TAP.tap_processor import *


NAME_PROJECT        = "Proj"
NAME_COUNTRY        = "Country"
NAME_COMPANY        = "OrgCompany"
NAME_BINFORMAT      = "BinFormat"
NAME_ROUTE_DBIP     = "Route_DBIP"
NAME_ROUTE_DBNAME   = "Route_DBName"
NAME_AREA           = "Area"
NAME_BINFORMAT      = "BinFormat"
RELEASE_DBNAME      = "release_dbname"

class check_route_com():
    
    def __init__(self, start_path, end_path):
        self._start_path = start_path
        self._end_path = end_path
        self._rec_msg_list = dict()
        self.pg_log = log.common_log.instance().logger("check_route")
    def do_task(self): 
        try:
            if not os.path.isfile(self._start_path) :
                self.pg_log.error("%s is not file",self._start_path)
                return 0
            if not os.path.exists(self._start_path) :
                self.pg_log.error("file %s is not exist",self._start_path)
                return 0
            
            file = open(self._start_path,'r')
            self._rec_msg_list = json.loads(file.read())
            self.pg_log.info("get file info!%s",self._rec_msg_list)
            file.close()
            
            if self.autocheck_run() == -1:
                return -1
   
        except Exception,e:
            self.pg_log.exception("error happened in route_server...")
            print Exception,":",e        

    def autocheck_run(self):
        
        pg_check = check_config.check_config_comp(self._rec_msg_list[NAME_PROJECT],
                                                  self._rec_msg_list[NAME_COUNTRY], 
                                                  self._rec_msg_list[NAME_COMPANY],
                                                  self._rec_msg_list[NAME_ROUTE_DBIP],
                                                  self._rec_msg_list[NAME_ROUTE_DBNAME],
                                                  self._rec_msg_list[NAME_AREA],
                                                  self._rec_msg_list[NAME_BINFORMAT])
        try: 
            if pg_check.init_path() == -1:
                self._log_pg.error("make %s path is error!", self._rec_msg_list[NAME_ACTION])
                return -1
            
            if pg_check.set_autocheck() == -1:
                self._log_pg.error("copy code to idata error!")
                return -1  
            
            if not os.system(os.path.join(pg_check.getdestpath(),"autocheck.bat")) == 0:
                self._log_pg.error("autocheck.bat error")
                return -1
           
            if self.comapre_feature_result(pg_check, pg_check.getdestpath(), pg_check.get_check_dest_path()) == -1:
                self._log_pg.error("get comapre_feature_result error!")
                return -1
            
            if pg_check.copy_result_code() == -1:
                self._log_pg.error("copy code to idata error!")
                return -1              
            
        except Exception,e:
            print Exception,":",e
            return -1
        
        return 0
        
    def copy_result(self, source_path):
        try:
            if not os.path.exists(self._end_path ) :
                os.makedirs(self._end_path)
                
            for file_temp in os.listdir(source_path):
                if os.path.isfile(file_temp):
                    if file_temp[-4:].lower() == '.tap':
                        file_name = os.path.basename(file_temp)
                        open(os.path.join(self._end_path, file_name),"wb").write(open(file_temp,"rb").read())
        except:
            return -1
        
    def comapre_feature_result(self, pg_check, config_path, dest_path):
        config_file = os.path.join(config_path, "AutoCheck_Route_Compare.xlsx")
        base_path = os.path.dirname(dest_path)       
        comapre_name = self._rec_msg_list[NAME_ROUTE_DBNAME] + "_statistic_VS_" + \
                    pg_check.get_release_db() + "_statistic"
        feature_name = "arrange_feature_list_" + self._rec_msg_list[NAME_ROUTE_DBIP] + "_" +\
                    self._rec_msg_list[NAME_ROUTE_DBNAME]
                    
        comapre_file, feature_file = self.get_file_from_path(base_path, comapre_name, feature_name)
        
        pg_check.get_tool_code()
        
        if not comapre_file:
            pg_check.set_compare_config()
            if not os.system(os.path.join(pg_check.getdestpath(),"DataStat_only.bat")) == 0:
                self._log_pg.error("DataStat_only.bat error")
                return -1
        pg_comapre = compare_list_comp(comapre_file, config_file, pg_check.get_scale_comapre())
        pg_comapre.init_list()
        pg_comapre_list = pg_comapre.get_result()
        self.write_tap(pg_comapre_list, self._rec_msg_list[NAME_ROUTE_DBNAME] + "_comapre.tap")
        
        if not feature_file:
            pg_check.set_feature_config()
            if not os.system(os.path.join(pg_check.getdestpath(),"AutoArrange.bat")) == 0:
                self._log_pg.error("AutoArrange.bat error")
                return -1
        pg_feature = arrange_feature_list_comp(feature_file)
        pg_feature.init_list()
        pg_feature_list = pg_feature.get_result()
        self.write_tap(pg_feature_list, self._rec_msg_list[NAME_ROUTE_DBNAME] + "_feature.tap")

    def get_file_from_path(self, path, comapre_name, feature_name):
        comapre_file = ""
        feature_file = ""
        comapre_check_file = ""
        feature_check_file = ""        
        
        for file in os.listdir(path):
            path_file = os.path.join(path,file)
            if os.path.isfile(path_file):
                if file.find(comapre_name) == 0:
                    comapre_file = path_file
                elif file.find(feature_name) == 0:
                    feature_file = path_file
                else:
                    continue
            if os.path.isdir(path_file):
                if file == "check":
                    comapre_check_file, feature_check_file = \
                        self.get_file_from_path(path_file, comapre_name, feature_name)
                else:
                    continue
        
        if not comapre_file:
            comapre_file = comapre_check_file
            
        if not feature_file:
            feature_file = feature_check_file
            
        return comapre_file, feature_file
    
    def write_tap(self, list_temp, file):
        path = os.path.join(os.path.dirname(os.getcwd()), "AutoCheck\\report", file)
        
        pg_ptocessor = TAP_Processor()
        pg_ptocessor.addResultList(list_temp)
        pg_ptocessor.dump2file(path)
        pg_ptocessor.clear()
        
        
