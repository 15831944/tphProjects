from common import log
import os
from common import analyze_config
from common import git_code


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
        self._release_ip = ''
        self._release_db = ''
        self._scale_comapre = 0
        
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
            
        temp_str = str(self._proj + ',' + self._company + ',' + self._country + ',' + self._binformat).lower()
        last_release_all = self.__Getconfig(temp_str, os.path.join(CONFIG_ROOT_PATH, "last_db.txt"))
        try:         
            last_release_temp = last_release_all.split(",")
            self._release_ip = last_release_temp[0]
            self._release_db = last_release_temp[1]
            self.log.info("release_ip = %s, release_db = %s",self._release_ip,self._release_db)
        except:
            return -1
        
        route_dbname_list = self._route_dbname.split("_")
        release_db_list = self._release_db.split("_")
        
        if route_dbname_list[2] == release_db_list[2]:
            self._scale_comapre = 0.1
        else:
            self._scale_comapre = 0.3   
    
    def get_scale_comapre(self):
        return self._scale_comapre
    
    def delete_tap(self):
        path_tap = os.path.join(self._code_path, "AutoCheck", "report")
        if os.path.isdir(path_tap):
            for file_temp in os.listdir(path_tap):
                path_file = os.path.join(path_tap, file_temp)               
                if os.path.isfile(path_file):
                    os.remove(path_file)
                elif os.path.isdir(path_file):
                    os.removedirs(path_file)
        return 0
        
    def set_autocheck(self):
        if self.__alter_config_file(os.path.join(CONFIG_ROOT_PATH,"autocheck.bat")) == -1:
            return -1
        
        if self.__alter_config_file(os.path.join(self._code_path,"AutoCheck\\src\\config.ini")) == -1:
            return -1
        
        return 0
    
    def get_tool_code(self):
        pg_git = git_code.git_code_dump(os.path.dirname(os.getcwd()))
        pg_git.code_dump()
        pg_git.code_copy()
        
    def __alter_compare_config_file(self, file_path, Flag = False):
        
        if file_path is None or len(file_path) < 1:
            return -1

        pg_key = "\' user=\'postgres\' password=\'pset123456\'\n"
        file_object = open(file_path, "r")
        list_temp = list()
        try:
            lines = file_object.readlines()
            for line in lines:
                if line.find("set basedir=") == 0:
                    list_temp.append("set basedir=" + self._code_path + "\n")
                elif line.find("db=host=") == 0:                   
                    list_temp.append("db=host=\'" + self._route_dbip + "\' dbname=\'" + self._route_dbname + pg_key)
                elif line.find("db1=host=") == 0:                   
                    list_temp.append("db1=host=\'" + self._route_dbip + "\' dbname=\'" + self._route_dbname + pg_key)
                elif line.find("db2=host=") == 0 or line.find("#db2=host=") == 0:                   
                    if len(self._release_ip) > 0 and len(self._release_db) > 0:
                        list_temp.append("db2=host=\'" + self._release_ip + "\' dbname=\'" + self._release_db + pg_key)
                    else:
                        list_temp.append("#db2=host=\'" + self._release_ip + "\' dbname=\'" + self._release_db + pg_key)                    
                elif line.find("proj_name=") == 0:
                    list_temp.append("proj_name=" + self._company_proj + "\n")
                elif line.find("proj_country=") == 0:
                    if Flag:
                        list_temp.append("proj_country=" + (self._company_proj + "_" + self._tran_country).upper() + "\n")
                    else:
                        if self._tran_country.lower() == 'bra':
                            list_temp.append("proj_country=" + 'bra5' + "\n")
                        else:
                            list_temp.append("proj_country=" + self._tran_country + "\n")                                                                                                                
                elif line.find("report_path=") == 0:
                    list_temp.append("report_path=" + self._check_dest_path + "\n")
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
        
    def set_compare_config(self):
        if self.__alter_compare_config_file(os.path.join(CONFIG_ROOT_PATH, "DataStat_only.bat")) == -1:
            return -1
        if self.__alter_compare_config_file(os.path.join(self._code_path,"DataStat\\config\\config.ini"), True) == -1:
            return -1
        
    def set_feature_config(self):
        if self.__alter_compare_config_file(os.path.join(CONFIG_ROOT_PATH, "AutoArrange.bat")) == -1:
            return -1
        if self.__alter_compare_config_file(os.path.join(self._code_path, "AutoArrange\\src\\config.ini")) == -1:
            return -1
        
            
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
                    list_temp.append("country_name=" + self._tran_country + "\n")
                elif line.find("json_path=") == 0:
                    list_temp.append("json_path=" + r'../report' + "\n")
                elif line.find("checklist=") == 0:
                    list_temp.append("checklist=" + r'../config/rdb_check_list.xls' + "\n")                    
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
    
    def get_check_dest_path(self):
        return self._check_dest_path

    def get_release_db(self):
        return self._release_db
        
        #get dbname
    def __Getconfig(self,name,file_path):
        if file_path is None or name is None or len(file_path) < 1 or len(name) < 1:
            return -1
        DBver       = ''
        file_object = open(file_path, "r")
    
        try:
            lines = file_object.readlines()
            for line in lines:
                if line.find(name + "=") == 0:
                    DBver = line[len(name + "="):].strip()
                    break
        except:
            self.log.error("get %s from %s error!",name,file_path)
            return -1
        finally:
            file_object.close()
            
        return DBver  
            
