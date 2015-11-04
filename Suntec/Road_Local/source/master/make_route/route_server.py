import json
import os
import time
import urllib2
import run_route
from common import log
NAME_STEP = 'Step_Action'
NAME_FLOW_CTL_SVR = 'FLOW_CTL_SVR'
NAME_ROUTE = 'ROUTE'
NAME_CASE_TYPE = "CaseType"
NAME_RESULT = "Result"
NAME_ACTION = "Action"
NAME_COUNTRY = "Country"

DB1 = '192.168.10.16'  #chn,ind,aus
DB2 = '192.168.10.18' #arg,me8,sea,saf8,sgp
DB3 = '192.168.10.10' #bra,hkg,twn

DB_dict = {DB1:0,DB2:0,DB3:0}

class ServerWorker_route():
    """ServerWorker"""
    def __init__(self, start_path, end_path):
        self._start_path = start_path
        self._end_path = end_path
        self.pg_log = log.common_log.instance().logger("route_server")
    
    def do_task(self):
        #
            
        try:
            if not os.path.isfile(self._start_path) :
                self.pg_log.error("%s is not file",self._start_path)
                return 0
            if not os.path.exists(self._start_path) :
                self.pg_log.error("file %s is not exist",self._start_path)
                return 0
            
            file = open(self._start_path,'r')
            rev_dict = json.loads(file.read())
            print rev_dict
            file.close()


            if not rev_dict.has_key(NAME_ACTION):
                rev_dict[NAME_ACTION] = 'build_data'                 
                        
            
            num = 0
            while True:
                if rev_dict[NAME_COUNTRY].lower() in ('chn', 'aus', 'ind', 'chn_dummy'):
                    DB = DB1
                elif rev_dict[NAME_COUNTRY].lower() in ('arg', 'me8', 'sea','saf8'):
                    DB = DB2
                elif rev_dict[NAME_COUNTRY].lower() in ('bra', 'hkgmac', 'twn','sgp'):
                    DB = DB3
                else:
                    self.pg_log.error("Country error!%s",rev_dict[NAME_COUNTRY].lower())
                    return 0
                pg_run = run_route.run_route_com(DB, rev_dict, self._end_path)
                back_msg = pg_run.run(num)
                if back_msg[NAME_RESULT] == 'OK':
                    back_msg.pop(NAME_RESULT)
                    
                    if rev_dict[NAME_ACTION] == 'build_data':
                        back_msg.pop(NAME_ACTION)                   
                        file = open(self._end_path,'w')
                        file.write(json.dumps(back_msg, ensure_ascii = False,indent = 4))
                        file.close()
                    return 0
                else:
                    return -1
                
                num += 1
                if pg_run.end():
                    break   
        except Exception,e:
            self.pg_log.exception("error happened in route_server...")
            print Exception,":",e
