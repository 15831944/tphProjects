from common import log
import os
from common import git_code
import analyze_config


CONFIG_ROOT_PATH = os.getcwd()
CODE_PATH_ROOT = "C:\zhaojie\check"

class analyze_autocheck_comp():
    def __init__ (self, country = '', 
                        company = '', 
                        route_dbip = '',
                        route_dbname = '',
                        report_path = ''):
        self._country = country
        self._company = company
        self._route_dbip = route_dbip
        self._route_dbname = route_dbname
        self._report_path = report_path
        
        self.log = log.common_log.instance().logger("analyze_autocheck")
    def init_path(self):
        if not os.path.exists(self._report_path ) :
            self.log.error("file %s is not exist",self._report_path)
            return 0
        
        if not os.path.isdir(self._report_path ) :
            self.log.error("%s is not path",self._report_path)
            return 0
        
        for file_temp in os.listdir(self._report_path):
            if os.path.isfile(file_temp):
                os.remove(file_temp)
            elif os.path.isdir(file_temp):
                os.removedirs(file_temp)
            else:
                self.pg_log.error("%s type error!",file_temp)
                return 0   
            

        self._code_path = os.path.dirname(CONFIG_ROOT_PATH)
        if self._company == 'HERE':
            self._company_proj = 'rdf'
        elif self._company == 'SENSIS':
            self._company_proj = 'ta'
        else:
            self._company_proj = self._company.lower()
    
    def getdestpath(self):
        return os.path.join(CONFIG_ROOT_PATH, "config")
 
    def set_autocheck(self):
        if self.__alter_config_file(os.path.join(CONFIG_ROOT_PATH,"config\\autocheck.bat")) == -1:
            return -1
        
        if self.__alter_config_file(os.path.join(self._code_path,"AutoCheck\\src\\config.ini")) == -1:
            return -1
        
        return 0   
        
    def __alter_config_file(self, file_path):
        
        if file_path is None or len(file_path) < 1:
            return -1
        
        pg_key = "\' user=\'postgres\' password=\'pset123456\'\n"
        file_object = open(file_path, "r")
        list_temp = list()
        try:
            lines = file_object.readlines()
            for line in lines:
                #config.ini
                if line.find("db=host=") == 0:
                    list_temp.append("db=host=\'" + self._route_dbip + "\' dbname=\'" + self._route_dbname + pg_key)
                elif line.find("proj_name=") == 0:
                    list_temp.append("proj_name=" + self._company_proj + "\n")
                elif line.find("country_name=") == 0:
                    list_temp.append("country_name=" + self._country + "\n")
                elif line.find("caseid=") == 0:
                    list_temp.append("caseid=\n")
                elif line.find("json_path=") == 0:
                    list_temp.append("json_path=" + self._report_path + "\n")
                elif line.find("set basedir=") == 0:
                    list_temp.append("set basedir=" + self._code_path + "\n")
                else:
                    list_temp.append(line)                                            
        except:
            file_object.close()
            self.log.error("alter %s error!",file_path)
            return -1
        else:
            file_object.close()           
            try:
                os.remove(file_path)
                file_object = open(file_path, "w")
                file_object.writelines(list_temp)
                file_object.close()
            except:
                self.log.error("alter %s error!",file_path)
                return -1
            
            
        return 0
    
    def copy_result_code(self):
        dest_path_autocheck = os.path.join(CODE_PATH_ROOT, self._route_dbip + "_" + self._route_dbname + "\\AutoCheck")
        if not os.path.exists(dest_path_autocheck):
            os.makedirs(dest_path_autocheck)
        if copydir(os.path.join(self._code_path, "AutoCheck"), dest_path_autocheck) == -1:
            return -1
        
        dest_path_make_route = os.path.join(CODE_PATH_ROOT, self._route_dbip + "_" + self._route_dbname + "\\make_route")
        if not os.path.exists(dest_path_make_route):
            os.makedirs(dest_path_make_route)
        if copydir(os.path.join(self._code_path, "make_route"), dest_path_make_route) == -1:
            return -1
            
