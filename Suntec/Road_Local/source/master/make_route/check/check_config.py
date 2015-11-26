from common import log
import os
from common import analyze_config


CONFIG_ROOT_PATH = os.path.join(os.getcwd(), "config")
DATA_ROOT_PATH = r"\\192.168.8.14\Private\Data_Authoring\DB_Output"

class check_config_comp():
    def __init__ (self, proj            = '',
                        country         = '', 
                        company         = '', 
                        route_dbip      = '',
                        route_dbname    = '',
                        area            = '',
                        binformat       = ''):
        self._proj = proj
        self._country = country
        self._company = company
        self._route_dbip = route_dbip
        self._route_dbname = route_dbname
        self._area = area
        self._binformat = binformat
        self._company_proj = ''
        self._tran_country = ''
        self._check_dest_path = ''
        self._code_path = ''
        
        self.log = log.common_log.instance().logger("analyze_autocheck")
    def init_path(self):
        if self._company == 'HERE':
            self._company_proj = 'rdf'
        elif self._company == 'SENSIS' or self._company == 'TOMTOM':
            self._company_proj = 'ta'
        elif self._company == 'NAVINFO' :
            self._company_proj = 'ni'            
        else:
            self._company_proj = self._company.lower()
            
        if self._binformat == 'F18' and self._company == 'HERE' and self._country == 'HKGMAC':
            self._tran_country = 'hkg'
            self._jv_flag = 'by_table'
        elif self._binformat == 'F18' and self._company == 'HERE' and self._country == 'SEA':
            self._tran_country = 'ase'
        elif self._binformat == 'F18' and self._company == 'HERE' and self._country == 'ME8':
            self._tran_country = 'mea' 
        elif self._binformat == 'F18' and self._company == 'NAVINFO' and self._country == 'CHN_DUMMY':
            self._tran_country = 'chn'                        
        else:
            self._tran_country = self._country
            
        self._code_path = os.path.dirname(os.getcwd())
        if self.delete_tap() == -1:
            return -1
        
        self._idata_process_path = DATA_ROOT_PATH + "\\" + self._proj + "\\" + self._area + "\\" + self._country
        dbname_str_path = self._route_dbname[len(self._country) + 1:]
        path_temp = self._idata_process_path + "\\" + dbname_str_path + "\\AUT_Road"           
        self._check_dest_path = os.path.join(path_temp,"check_route") 
        if not os.path.exists(self._check_dest_path):
            os.makedirs(self._check_dest_path)
    def delete_tap(self):
        path_tap = os.path.join(self._code_path, "AutoCheck", "report")
        if os.path.isdir(path_tap):
            for file_temp in os.listdir(path_tap):
                if os.path.isfile(file_temp):
                    os.remove(file_temp)
                elif os.path.isdir(file_temp):
                    os.removedirs(file_temp)
        return 0
        
    def set_autocheck(self):
        if self.__alter_config_file(os.path.join(CONFIG_ROOT_PATH,"autocheck.bat")) == -1:
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
                elif line.find("json_path=") == 0:
                    list_temp.append("json_path=" + r'../report' + "\n")
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
        dest_path_autocheck = os.path.join(self._check_dest_path, "AutoCheck")
        if not os.path.exists(dest_path_autocheck):
            os.makedirs(dest_path_autocheck)
        if analyze_config.copydir(os.path.join(self._code_path, "AutoCheck"), dest_path_autocheck) == -1:
            return -1
        
        open(os.path.join(self._check_dest_path, "log_check.txt"),"wb").write(\
                                    open(os.path.join(self._code_path, "log_check.txt"),"rb").read())
        
    def getdestpath(self):
        return CONFIG_ROOT_PATH
            
