# -*- coding: UTF8 -*-
import json
import os
from common import log
from common import analyze_config
from common import analyze_autocheck


NAME_PROJECT = "Proj"
NAME_COUNTRY = "Country"
NAME_COMPANY = "OrgCompany"
NAME_BINFORMAT = "BinFormat"
NAME_ORGVER = "OrgVer"
NAME_RESULT = "Result"
NAME_ROUTE_DBIP = "Route_DBIP"
NAME_ROUTE_DBNAME = "Route_DBName"
NAME_AREA = "Area"
NAME_ACTION = "Action"
NAME_CASE_ID = "Case_ID"
NAME_CHECK_RESULT = "Result_Data"
NAME_FINISH = "Finish"
NAME_VERSION = "DBVersion"
#NAME_CASE_TYPE = "CaseType"
NAME_ORG_DB_IP = 'ORG_DB_IP'
NAME_ORG_DB_NAME = 'ORG_DB_Name'
ROUTEPUB_DBIP='RoutePub_DBIP'
ROUTEPUB_DBNAME='RoutePub_DBName'
DB_RDB = '192.168.10.17'#release ip


log.common_log.instance()               

class run_route_com():
    
    def __init__(self, dbip_dest, rev_msg, dest_path):       
        self._dbip_dest = dbip_dest
        self._rec_msg_list = None
        self._send_msg_list = dict()
        try:          
#            self._rec_msg_list = json.loads(rev_msg)
            self._rec_msg_list = rev_msg
        except Exception,e:
            log.common_log.instance().logger("run_route").exception("error happened in loading reveice message!")
            print Exception,":",e
        
        self._run_num = 0
        self._send_msg_list = self._rec_msg_list
        self._dest_path = dest_path
        self._log_pg = log.common_log.instance().logger(self._rec_msg_list[NAME_ACTION])
        
        self._run_end = False
        
    def run(self, num):
        self._log_pg.info("get message %s",self._rec_msg_list)
        self._log_pg.info("start run %s!",self._rec_msg_list[NAME_ACTION])
        self._run_end = True
        
        self._run_num = num
        if self._rec_msg_list[NAME_ACTION].lower() == 'build_data' :
            return self.build_run()
        elif self._rec_msg_list[NAME_ACTION].lower() == 'check_data' :
            return self.autocheck_run(num)
        else:
            return self.getSendmsg("NG")
    def autocheck_run(self, num):
        
        caseid_list = self._rec_msg_list[NAME_CASE_ID]
        
        finish_temp = 'N'
        if num == (len(caseid_list) - 1) :
            self._run_end = True
            finish_temp = 'Y'
            
        autocheck_list = dict()
        autocheck_list[caseid_list[num]] = None
        
        pg_check = analyze_autocheck.analyze_autocheck_comp(self._rec_msg_list[NAME_COUNTRY], 
                                                            self._rec_msg_list[NAME_COMPANY],
                                                            self._rec_msg_list[NAME_ROUTE_DBIP],
                                                            self._rec_msg_list[NAME_ROUTE_DBNAME],
                                                            self._dest_path)
        try: 
            if pg_check.init_path() == -1:
                self._log_pg.error("make %s path is error!", self._rec_msg_list[NAME_ACTION])
                return self.getSendmsg("NG", check_result = autocheck_list, strFinish = finish_temp)
            
            if pg_check.set_autocheck() == -1:
                self._log_pg.error("make %s autocheck is error!", self._rec_msg_list[NAME_ACTION])
                return self.getSendmsg("NG", check_result = autocheck_list, strFinish = finish_temp)

            if not os.system(os.path.join(pg_check.getdestpath(),"autocheck.bat")) == 0:
                self._log_pg.error("autocheck.bat error")
                return self.getSendmsg("NG", check_result = autocheck_list, strFinish = finish_temp)

            if pg_check.copy_result_code() == -1:
                self._log_pg.error("copy code to idata error!")
                return self.getSendmsg("NG", check_result = autocheck_list, strFinish = finish_temp)                
                
#            autocheck_list = json.load(file(os.path.join(pg_check.get_destjsonpath(), str(caseid_list[num]))))
            return self.getSendmsg("OK", check_result = autocheck_list, strFinish = finish_temp)
        except Exception,e:
            print Exception,":",e
            self._log_pg.exception("make %s is error!", self._rec_msg_list[NAME_ACTION])
            return self.getSendmsg("NG", check_result = autocheck_list, strFinish = finish_temp)         
            
    
    def build_run(self):
              
        pg_build = analyze_config.analyze_config_comp(self._rec_msg_list[NAME_PROJECT],
                                                      self._rec_msg_list[NAME_COUNTRY],
                                                      self._rec_msg_list[NAME_COMPANY],
                                                      self._rec_msg_list[NAME_BINFORMAT],
                                                      self._dbip_dest,
                                                      self._rec_msg_list[NAME_ORGVER],
                                                      self._rec_msg_list[NAME_AREA],
                                                      self._rec_msg_list[NAME_VERSION],
                                                      DB_RDB)
        
        try:
            if pg_build.init_path() == -1:          
                self._log_pg.error("make init config is error!")
                return self.getSendmsg("NG") 
        
            if pg_build.set_config() == -1:
                self._log_pg.error("make set config is error!")
                return self.getSendmsg("NG")
                
            if not os.system(os.path.join(pg_build.getdestpath(),"prepare_db.bat")) == 0:
                self._log_pg.error("run prepare_db.bat error")
                return self.getSendmsg("NG")
            
            if not os.system(os.path.join(pg_build.getdestpath(),"convert_db.bat")) == 0:
                self._log_pg.error("convert_db.bat error")
                return self.getSendmsg("NG")
            if pg_build.CopyImage() == -1:
                self._log_pg.error("copy image error!")
                return self.getSendmsg("NG")
            
            if pg_build.write_config() == -1:
                self._log_pg.error("write config error!")
                return self.getSendmsg("NG")
            
            if not os.system(os.path.join(pg_build.getdestpath(),"DataStat.bat")) == 0:
                self._log_pg.error("DataStat.bat error")
                return self.getSendmsg("NG")
            
            if not os.system(os.path.join(pg_build.getdestpath(),"AutoArrange.bat")) == 0:
                self._log_pg.error("AutoArrange.bat error")
                return self.getSendmsg("NG")
            
            if not os.system(os.path.join(pg_build.getdestpath(),"AutoRoute_dump.bat")) == 0:
                self._log_pg.error("AutoRoute_dump.bat error")
                return self.getSendmsg("NG")            
            
            if pg_build.copy_result_code() == -1:
                self._log_pg.error("copy code error!")
                return self.getSendmsg("NG")
            
            if not os.system(os.path.join(pg_build.getdestpath(),"restore_rdb.bat")) == 0:
                self._log_pg.error("restore db to 192.168.10.17 error")
                return self.getSendmsg("NG")
       
        except Exception,e:
            print Exception,":",e
            self._log_pg.exception("make %s is error!", self._rec_msg_list[NAME_ACTION])
            return self.getSendmsg("NG")

        self._log_pg.info("end run route!")        
        return self.getSendmsg("OK", dbname = pg_build.getdbname_str(), org_db_ip = pg_build.getorg_ip(),\
                               org_db_name = pg_build.getorg_name())
    
    def getSendmsg(self, result, dbname = '', check_result = '', strFinish = '', \
                   org_db_ip = '', org_db_name = ''):
        if len(dbname) > 0:
            self._send_msg_list[NAME_ROUTE_DBIP] = self._dbip_dest
            self._send_msg_list[NAME_ROUTE_DBNAME] = dbname
            self._send_msg_list[ROUTEPUB_DBIP] = DB_RDB
            self._send_msg_list[ROUTEPUB_DBNAME] = dbname
            self._log_pg.info("RoutePub_DBIP = %s, RoutePub_DBName = %s",DB_RDB,dbname)
            
        if len(org_db_ip) > 0:
            self._send_msg_list[NAME_ORG_DB_IP] = org_db_ip
            self._send_msg_list[NAME_ORG_DB_NAME] = org_db_name
            
            
        if len(check_result) > 0:
            self._send_msg_list[NAME_CHECK_RESULT] = check_result
            
        if len(strFinish) > 0:
            self._send_msg_list[NAME_FINISH] = strFinish
            
        self._send_msg_list[NAME_RESULT] = result
#        return self.Sendmsg_to_json()
#        self._send_msg_list.pop(NAME_ACTION)
        return self._send_msg_list

#    def Sendmsg_to_json(self):
#        
#        self._log_pg.info("send message %s to server!",self._send_msg_list)
#        json_dest = None
#        try:
#            json_dest = json.dumps(self._send_msg_list)
#        except Exception,e:
#            self._log_pg.exception("error happened in dump send message!")
#            print Exception,":",e
#            
#        return json_dest
    
    def end(self):
        return self._run_end
        
    