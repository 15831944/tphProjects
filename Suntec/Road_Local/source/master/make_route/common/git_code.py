# -*- coding: UTF8 -*-

import os
from common import log
import analyze_config
CODE_ROOT_PATH = r"C:\zhaojie\git\Tool"

class git_code_dump():
    def __init__(self, code_path = ''):
        
        self._code_ver = ''
        self._config_code_path = code_path
        self.log = log.common_log.instance().logger("git_code_dump")
        
    def code_dump(self):        
        if not os.system(os.path.join(CODE_ROOT_PATH,"git_tool_code.bat")) == 0:
            self.log.error("git_tool_code %s error!", os.path.join(run_config_path,"git_tool_code.bat"))
            return -1
        
        return 0
    
    def code_copy(self):
        try:      
            code_datastat_temp = os.path.join(CODE_ROOT_PATH,"Road\\DataStat")
            code_datastat_path = os.path.join(self._config_code_path,"DataStat")       
            analyze_config.copydir(code_datastat_temp, code_datastat_path)
            
            code_dump_temp = os.path.join(CODE_ROOT_PATH,"Road\\dump")
            code_dump_path = os.path.join(self._config_code_path,"dump")        
            analyze_config.copydir(code_dump_temp, code_dump_path)
            
            code_dump_temp = os.path.join(CODE_ROOT_PATH,"Road\\AutoArrange")
            code_dump_path = os.path.join(self._config_code_path,"AutoArrange")        
            analyze_config.copydir(code_dump_temp, code_dump_path)
            
            code_dump_temp = os.path.join(CODE_ROOT_PATH,"Road\\Route_dump")
            code_dump_path = os.path.join(self._config_code_path,"Route_dump")        
            analyze_config.copydir(code_dump_temp, code_dump_path)
            
            code_tool_txt = CODE_ROOT_PATH + "\\tool_log.txt"
            code_tool_path = self._config_code_path + "\\tool_log.txt"
            open(code_tool_path,"wb").write(open(code_tool_txt,"rb").read())
        except:
            return -1
        
        return 0
