# -*- coding: UTF8 -*-
import json
import os
from common import log
from check import check_config


NAME_PROJECT        = "Proj"
NAME_COUNTRY        = "Country"
NAME_COMPANY        = "OrgCompany"
NAME_BINFORMAT      = "BinFormat"
NAME_ROUTE_DBIP     = "Route_DBIP"
NAME_ROUTE_DBNAME   = "Route_DBName"
NAME_AREA           = "Area"
NAME_BINFORMAT      = "BinFormat"

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

            if pg_check.copy_result_code() == -1:
                self._log_pg.error("copy code to idata error!")
                return -1                
#            if self.copy_result(pg_check.get_result()) == -1:
#                self._log_pg.error("copy result to %s!", self._end_path)
#                return -1 
            return 0
        except Exception,e:
            print Exception,":",e
            return -1
        
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
