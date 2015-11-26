# -*- coding: UTF8 -*-

from common import log
import os
from common import git_code

CONFIG_ROOT_PATH = os.path.join(os.getcwd(), "config")
DATA_ROOT_PATH = r"\\192.168.8.14\Private\Data_Authoring"


class analyze_config_comp():
    def __init__ (self, project = '',
                        country = '', 
                        company = '', 
                        binformat = '',
                        db_ip = '',
                        org_ver = '',
                        area_name = '',
                        dbversion_num = '',
                        db_rdb = '192.168.10.17'):
        self._project = project
        self._country = country
        self._company = company
        self._company_proj = ''
        self._binformat = binformat
        self._db_ip = db_ip
        self._org_ver = org_ver
        self._area_name = area_name
        self._dbname_str = ''
        self._last_db_ip = ''
        self._last_db_name = ''
        self._dbversion_num = dbversion_num
        self._id_fund_backup = ''
        self._jv_country = ''
        self._jv_flag = 'by_file'
        self._tran_country = ''
        self.log = log.common_log.instance().logger("analyze_config")
        self._idata_process_path = DATA_ROOT_PATH + "\\DB_Output\\" + self._project + "\\" + self._area_name + "\\" + self._country
        self._src_ip_pw = u'pset123456'
        self._dst_ip_pw = u'pset123456'
        self._rdb_ip_pw = u'pset123456'
        self._rdb_ip = db_rdb
        self._datastat_country = ''
        self._release_ip = ''
        self._release_db = ''
        self._check_dest_path = ''

    def init_path(self):
        #get dbver
        self._dbver = self.__Getconfig(self._binformat, os.path.join(CONFIG_ROOT_PATH, "config_binformat.txt"))
        #get org_ip org_db
        org_all= self.__Getconfig(self._country + "," + self._company + "," + self._org_ver, os.path.join(CONFIG_ROOT_PATH, "config_orgdata.txt"))
        try:         
            list_temp = org_all.split(",")
            self._org_ip = list_temp[0]
            self._org_db = list_temp[1]
            self.log.info("org_ip = %s, org_db = %s",self._org_ip,self._org_db)
        except:
            return -1
        
        temp_str = str(self._project + ',' + self._company + ',' + self._country + ',' + self._binformat).lower()
        last_release_all = self.__Getconfig(temp_str, os.path.join(CONFIG_ROOT_PATH, "last_db.txt"))
        try:         
            last_release_temp = last_release_all.split(",")
            self._release_ip = last_release_temp[0]
            self._release_db = last_release_temp[1]
            self.log.info("release_ip = %s, release_db = %s",self._release_ip,self._release_db)
        except:
            return -1
        
        
        self._codepath = os.path.dirname(os.getcwd())  
        #get dbname file
        self._dbname_file = os.path.join(self._idata_process_path, "db_name.txt")
        
        pg_git = git_code.git_code_dump(self._codepath)
        pg_git.code_dump()
        pg_git.code_copy()
        
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
            
        self._datastat_country = self._company_proj + "_" + self._tran_country
        
        if self.__getdbname() == -1:
            return -1
        
        dbname_str_path = self._dbname_str[len(self._country) + 1:]
        path_temp = self._idata_process_path + "\\" + dbname_str_path + "\\AUT_Road"
        self._org_backup = os.path.join(path_temp,self._org_db + ".backup")
        self._org_image = os.path.join(path_temp,"image")
        if not os.path.exists(self._org_image):
            os.makedirs(self._org_image)
            
        self._check_dest_path = os.path.join(path_temp,"check_route") 
        if not os.path.exists(self._check_dest_path):
            os.makedirs(self._check_dest_path)
                        
        self._id_fund_backup = os.path.join(path_temp,self._last_db_name + "_id_fund.backup")
                                        
        return 0
        
    def set_config(self):
        if self.__copy_db_config() == -1:
            return -1
        if self.__copy_code_config() == -1:
            return -1
        if self.__copy_run_config() == -1:
            return -1
        if self.__copy_DataStat_config() == -1:
            return -1
        if self.__copy_AutoArrange_config() == -1:
            return -1
        if self.__copy_Route_dump_config() == -1:
            return -1
        return 0
    
    def copy_result_code(self): 
        dest_path = os.path.join(os.path.dirname(self._org_image), 'route_code')
        if not os.path.exists(dest_path):
            os.makedirs(dest_path)
        if copydir(self._codepath, dest_path) == -1:
            return -1
        return 0
        
    
    def write_config(self):        
        
        try:           
            #db_name.txt
            file_obj = open(self._dbname_file,"a+")        
            lines_temp = self._dbname_str + "," + self._db_ip + "\n"
            file_obj.writelines(lines_temp)
        except:
            self.log.error("path %s error!",self._dbname_file)
            return -1
        finally:
            file_obj.close()
            
        return 0
    
    def CopyImage(self):
        if self._org_image == '' or self._org_image is None:
            return -1
        
        image_bak = os.path.join(os.path.dirname(self._org_image), "image_bak")
        if copydir(image_bak, self._org_image, False, '.dat') == -1:
            return -1
        return 0
    def getorg_ip(self):
        return self._org_ip
    
    def getorg_name(self):
        return self._org_db
        
    def __getdbname(self):
        
        try:           
            if not os.path.exists(os.path.dirname(self._dbname_file)):
                os.makedirs(os.path.dirname(self._dbname_file))
        except:
            self.log.error("path %s error!",self._dbname_file)
        
        file_obj = open(self._dbname_file, "a+")
        lines_target = list()
        dbname_front = self._country + "_" + self._company + "_" + self._org_ver + "_" + self._dbver
        dbname_back = 0
        try:
            lines = file_obj.readlines()
            if len(lines) == 0:
                lines_target.append("#db_name,db_ip\n")
                file_obj.writelines(lines_target)
            for line in lines:
                if line.find(dbname_front) == 0:
                    if dbname_back < self.__getdb_num(dbname_front, line):
                        dbname_back = self.__getdb_num(dbname_front, line)
                        list_line = line[0:-1].split(',')
                        self._last_db_name = list_line[0]
                        self._last_db_ip = list_line[1]
                        
            str_dbname_back = str(dbname_back + 1)
            if len(str_dbname_back) < 4:
                str_dbname_back = str_dbname_back.zfill(4)
                
            self._dbname_str = dbname_front + "_" + str_dbname_back + "_" + self._dbversion_num
            self.log.info("dest_ip = %s, dest_db = %s",self._db_ip,self._dbname_str)
        except:
            self.log.error("get config.txt path is error!")
            return -1
        else:
            file_obj.close()
                    
            return 0
    def __copy_db_config(self):
        list_temp = [u'data_dump_copy.sh', u'data_dump_copy_rdb.sh', 
                     u'data_restore_copy.sh', u'data_restore_copy_rdb.sh',
                     u'prepare_db.bat', u'restore_rdb.bat']
        
        for str_temp in list_temp:
            
            if self.__alter_db_config_file(os.path.join(CONFIG_ROOT_PATH, str_temp)) == -1:
                return -1

    def __alter_db_config_file(self, file_path):
        
        if file_path is None or len(file_path) < 1:
            return -1

        file_object = open(file_path, "r")
        list_temp = list()
        try:
            lines = file_object.readlines()
            for line in lines:
                if line.find("set putty_path=") == 0:                   
                    list_temp.append("set putty_path=" + CONFIG_ROOT_PATH + "\n")
                elif line.find("echo 'src_host=") == 0:                   
                    list_temp.append("echo 'src_host=" + self._org_ip + "'" + line[line.find(">"):])
                elif line.find("echo 'src_name=") == 0:
                    list_temp.append("echo 'src_name=" + self._org_db + "'" + line[line.find(">"):])
                elif line.find("set src_host=") == 0:                   
                    list_temp.append("set src_host=" + self._org_ip + "\n")
                elif line.find("set src_dbname=") == 0:
                    list_temp.append("set src_dbname=" + self._org_db + "\n")                    
                elif line.find("src_name=") == 0:
                    list_temp.append("src_name=" + self._org_db + "\n")                  
                elif line.find("set dest_host=") == 0:
                    list_temp.append("set dest_host=" + self._db_ip + "\n")
                elif line.find("echo 'dest_host=") == 0:
                    list_temp.append("echo 'dest_host=" + self._db_ip + "'" + line[line.find(">"):])                    
                elif line.find("dest_name=") == 0:
                    list_temp.append("dest_name=" + self._dbname_str + "\n")
                elif line.find("echo 'dest_name=") == 0:
                    list_temp.append("echo 'dest_name=" + self._dbname_str + "'" + line[line.find(">"):])                    
                elif line.find("set src_pw=") == 0:
                    list_temp.append("set src_pw=" + self._src_ip_pw + "\n")
                elif line.find("set dest_pw=") == 0:
                    list_temp.append("set dest_pw=" + self._dst_ip_pw + "\n")
                elif line.find("set dest_rdb_host=") == 0:
                    list_temp.append("set dest_rdb_host=" + self._rdb_ip + "\n")                                        
                elif line.find("set dest_rdb_pw=") == 0:
                    list_temp.append("set dest_rdb_pw=" + self._rdb_ip_pw + "\n")
                elif line.find("echo 'dest_rdb_host=") == 0:
                    list_temp.append("echo 'dest_rdb_host=" + self._rdb_ip + "'" + line[line.find(">"):])                     
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
        
    def __copy_run_config(self):
        if self.__alter_run_config_file(os.path.join(CONFIG_ROOT_PATH, "convert_db.bat")) == -1:
            return -1
#        if self.__alter_run_config_file(os.path.join(CONFIG_ROOT_PATH, "autocheck.bat")) == -1:
#            return -1        
        if self.__alter_run_config_file(os.path.join(CONFIG_ROOT_PATH, "DataStat.bat")) == -1:
            return -1
        if self.__alter_run_config_file(os.path.join(CONFIG_ROOT_PATH, "AutoArrange.bat")) == -1:
            return -1
        if self.__alter_run_config_file(os.path.join(CONFIG_ROOT_PATH, "AutoRoute_dump.bat")) == -1:
            return -1        
                           
        return 0
    
    def __alter_run_config_file(self, file_path):
        
        if file_path is None or len(file_path) < 1:
            return -1

        file_object = open(file_path, "r")
        list_temp = list()
        try:
            lines = file_object.readlines()
            for line in lines:
                if line.find("set src_host=") == 0:                   
                    list_temp.append("set src_host=" + self._org_ip + "\n")
                elif line.find("set src_dbname=") == 0:
                    list_temp.append("set src_dbname=" + self._org_db + "\n")
                elif line.find("set dst_host=") == 0:
                    list_temp.append("set dst_host=" + self._db_ip + "\n")
                elif line.find("set dst_dbname=") == 0:
                    list_temp.append("set dst_dbname=" + self._dbname_str + "\n")
                elif line.find("set backupfile=") == 0:
                    list_temp.append("set backupfile=" + self._org_backup + "\n")
                elif line.find("set basedir=") == 0:
                    list_temp.append("set basedir=" + self._codepath + "\n")
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
    
    def __copy_code_config(self):          
        if self.__alter_code_config_file(os.path.join(self._codepath,"Org2Middle\\src\\DataBasePath.txt")) == -1:
            return -1
        if self.__alter_code_config_file(os.path.join(self._codepath,"Middle2RDB\\src\\DataBasePath.txt")) == -1:
            return -1
#        if self.__alter_code_config_file(os.path.join(self._codepath,"AutoCheck\\src\\config.ini")) == -1:
#            return -1        
      
        return 0
    
    def __alter_code_config_file(self, file_path):
        
        if file_path is None or len(file_path) < 1:
            return -1
        dbname_str_path = self._dbname_str[len(self._country) + 1:]
        aut_road_path = self._idata_process_path + "\\" + dbname_str_path + "\\AUT_Road"
        pg_key = "\' user=\'postgres\' password=\'pset123456\'\n"
        file_object = open(file_path, "r")
        list_temp = list()
        try:
            lines = file_object.readlines()
            for line in lines:      
                #DataBasePath.txt            
                if line.find("db1=host=") == 0:
                    list_temp.append("db1=host=\'" + self._db_ip + "\' dbname=\'" + self._dbname_str + pg_key)
                elif line.find("db2=host=") == 0:
                    list_temp.append("db2=host=\'" + self._db_ip + "\' dbname=\'" + self._dbname_str + pg_key) 
                elif line.find("db=host=") == 0:
                    list_temp.append("db=host=\'" + self._db_ip + "\' dbname=\'" + self._dbname_str + pg_key) 
                elif line.find("proj_name=") == 0:
                    list_temp.append("proj_name=" + self._company_proj + "\n")
                elif line.find("proj_country=") == 0:
                    list_temp.append("proj_country=" + self._tran_country + "\n")
                elif line.find("country_name=") == 0:
                    list_temp.append("country_name=" + self._tran_country + "\n")
                elif line.find("json_path=") == 0:
                    list_temp.append("json_path=" + self._check_dest_path + "\n")                                       
                elif line.find("version=") == 0:
                    list_temp.append("version=" + self._dbversion_num + "\n")                
                elif line.find("illust_binary=") == 0:
                    list_temp.append("illust_binary=" + os.path.join(os.path.dirname(self._org_image),"image_bak") + "\n")                 
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

    def __copy_Route_dump_config(self):
        if self.__alter_Route_dump_config_file(os.path.join(self._codepath,"Route_dump\\config\\config.ini")) == -1:
            return -1
        
    def __alter_Route_dump_config_file(self, file_path):
        
        if file_path is None or len(file_path) < 1:
            return -1
        dbname_str_path = self._dbname_str[len(self._country) + 1:]
        path = self._idata_process_path + "\\" + dbname_str_path + "\\AUT_Road\\"
        pg_key = "\' user=\'postgres\' password=\'pset123456\'\n"
        file_object = open(file_path, "r")
        list_temp = list()
        try:
            lines = file_object.readlines()
            for line in lines:
                if line.find("dbname=") == 0:
                    list_temp.append("dbname=" + self._dbname_str + "\n")
                elif line.find("host=") == 0:                   
                    list_temp.append("host=" + self._db_ip + "\n")
                elif line.find("db_company=") == 0:
                    list_temp.append("db_company=" + self._company_proj + "\n")
                elif line.find("db_country=") == 0:
                    list_temp.append("db_country=" + self._tran_country + "\n")                                                                                                                
                elif line.find("reportPath=") == 0:
                    list_temp.append("reportPath=" + path + "\n")
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
    
    def __copy_AutoArrange_config(self):
        if self.__alter_AutoArrange_config_file(os.path.join(self._codepath,"AutoArrange\\src\\config.ini")) == -1:
            return -1
        
    def __alter_AutoArrange_config_file(self, file_path):
        
        if file_path is None or len(file_path) < 1:
            return -1
        dbname_str_path = self._dbname_str[len(self._country) + 1:]
        path = self._idata_process_path + "\\" + dbname_str_path + "\\AUT_Road\\"
        pg_key = "\' user=\'postgres\' password=\'pset123456\'\n"
        file_object = open(file_path, "r")
        list_temp = list()
        try:
            lines = file_object.readlines()
            for line in lines:
                if line.find("db=host=") == 0:                   
                    list_temp.append("db=host=\'" + self._db_ip + "\' dbname=\'" + self._dbname_str + pg_key)
                elif line.find("proj_name=") == 0:
                    list_temp.append("proj_name=" + self._company_proj + "\n")
                elif line.find("proj_country=") == 0:
                    if self._tran_country.lower() == 'bra':
                        list_temp.append("proj_country=" + 'bra5' + "\n")
                    else:
                        list_temp.append("proj_country=" + self._tran_country + "\n")                                                                                                                
                elif line.find("report_path=") == 0:
                    list_temp.append("report_path=" + path + "\n")
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
        
    def __copy_DataStat_config(self):
        if self.__alter_DataStat_config_file(os.path.join(self._codepath,"DataStat\\config\\config.ini")) == -1:
            return -1
        if self.__alter_DataStat_config_file(os.path.join(self._codepath,"dump\\config\\config.ini")) == -1:
            return -1
        
    def __alter_DataStat_config_file(self, file_path):
        
        if file_path is None or len(file_path) < 1:
            return -1
        dbname_str_path = self._dbname_str[len(self._country) + 1:]
        path = self._idata_process_path + "\\" + dbname_str_path + "\\AUT_Road\\"
        pg_key = "\' user=\'postgres\' password=\'pset123456\'\n"
        file_object = open(file_path, "r")
        list_temp = list()
        try:
            lines = file_object.readlines()
            for line in lines:
                if line.find("name=") == 0:                   
                    list_temp.append("name=" + self._dbname_str + "\n")
                elif line.find("host=") == 0:
                    list_temp.append("host=" + self._db_ip + "\n")
                elif line.find("reportPath=") == 0:
                    list_temp.append("reportPath=" + path + "\n")
                elif line.find("reportType=") == 0:
                    list_temp.append("reportType=json,xls\n")
                elif line.find("proj_country=") == 0:
                    list_temp.append("proj_country=" + self._datastat_country.upper() + "\n")                      
                elif line.find("db1=host=") == 0:
                    list_temp.append("db1=host=\'" + self._db_ip + "\' dbname=\'" + self._dbname_str + pg_key)
                elif line.find("db2=host=") == 0 or line.find("#db2=host=") == 0:
                    if len(self._release_ip) > 0 and len(self._release_db) > 0:
                        list_temp.append("db2=host=\'" + self._release_ip + "\' dbname=\'" + self._release_db + pg_key)
                    else:
                        list_temp.append("#db2=host=\'" + self._release_ip + "\' dbname=\'" + self._release_db + pg_key)
                        
                elif line.find("configPath=") == 0:
                    if self._binformat == 'F16':
                        list_temp.append("configPath=..\config\stat_list_tmap.xls" + "\n")
                    elif self._binformat == 'F18':
                        list_temp.append("configPath=..\config\stat_list_17cy.xls" + "\n")                                                         
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
    
    def __alter_config_file(self, file_path):
        
        if file_path is None or len(file_path) < 1:
            return -1
        dbname_str_path = self._dbname_str[len(self._country) + 1:]
        aut_road_path = self._idata_process_path + "\\" + dbname_str_path + "\\AUT_Road"
        pg_key = "\' user=\'postgres\' password=\'pset123456\'\n"
        file_object = open(file_path, "r")
        list_temp = list()
        try:
            lines = file_object.readlines()
            for line in lines:
                #prepare_db.bat
                if line.find("set src_host=") == 0:                   
                    list_temp.append("set src_host=" + self._org_ip + "\n")
                elif line.find("set src_dbname=") == 0:
                    list_temp.append("set src_dbname=" + self._org_db + "\n")
                elif line.find("set dst_host=") == 0:
                    list_temp.append("set dst_host=" + self._db_ip + "\n")
                elif line.find("set dst_dbname=") == 0:
                    list_temp.append("set dst_dbname=" + self._dbname_str + "\n")
                elif line.find("set backupfile=") == 0:
                    list_temp.append("set backupfile=" + self._org_backup + "\n")
                #get_id_fund.bat
                elif line.find("set last_host=") == 0:                   
                    list_temp.append("set last_host=" + self._last_db_ip + "\n")
                elif line.find("set last_dbname=") == 0:
                    list_temp.append("set last_dbname=" + self._last_db_name + "\n")
                elif line.find("set current_host=") == 0:
                    list_temp.append("set current_host=" + self._db_ip + "\n")
                elif line.find("set current_dbname=") == 0:
                    list_temp.append("set current_dbname=" + self._dbname_str + "\n")
                elif line.find("set backupfile_id_fund=") == 0:
                    list_temp.append("set backupfile_id_fund=" + self._id_fund_backup + "\n")
                elif line.find("pg_dump") == 0:
                    if os.path.basename(file_path) == "prepare_db.bat":                       
                        if os.path.exists(self._org_backup):
                            continue
                        else:
                            list_temp.append(line)
                    elif os.path.basename(file_path) == "get_id_fund.bat":
                        if os.path.exists(self._id_fund_backup):
                            continue
                        else:
                            list_temp.append(line)
                    else:
                        file_object.close()
                        self.log.error("alter %s error!",file_path)
                        return -1                
                #DataBasePath.txt            
                elif line.find("db1=host=") == 0:
                    list_temp.append("db1=host=\'" + self._db_ip + "\' dbname=\'" + self._dbname_str + pg_key)
                elif line.find("db2=host=") == 0:
                    list_temp.append("db2=host=\'" + self._db_ip + "\' dbname=\'" + self._dbname_str + pg_key) 
                elif line.find("mdb_path=") == 0:
                    list_temp.append("mdb_path=" + os.path.join(self._org_image,"mdb") + "\n")                                                       
#                elif line.find("jv_country=") == 0:
#                    list_temp.append("jv_country=" + self._country + "\n")
                elif line.find("proj_name=") == 0:
                    if self._binformat == 'F18':
                        list_temp.append("proj_name=" + self._company_proj + "\n")
                    else:
                        list_temp.append("proj_name=proj_" + self._company_proj + "\n")
                elif line.find("proj_country=") == 0:
                    list_temp.append("proj_country=" + self._tran_country + "\n")
                elif line.find("version=") == 0:
                    list_temp.append("version=" + self._dbversion_num + "\n")                
                elif line.find("illust=") == 0:
                    list_temp.append("illust=" + os.path.join(self._org_image,"illust") + "\n")
                elif line.find("get_id_fund_batch=") == 0:
                    if self._country == "JPN":                      
                        if len(self._last_db_ip) > 0 and len(self._last_db_name) > 0:                        
                            list_temp.append("get_id_fund_batch=" + os.path.join(self._codepath,"Middle2RDB\\src\\cache\\get_id_fund.bat") + "\n")
                        else:
                            list_temp.append("#get_id_fund_batch=")
                    else:
                        list_temp.append("#get_id_fund_batch=")
                #config\\config_        
                elif line.find("junctionview_data=") == 0:
                    list_temp.append("junctionview_data=" + self._org_image + "\n")
                elif line.find("sign_as_real_path=") == 0:
                    list_temp.append("sign_as_real_path=" + self._org_image + "\\sign_as_real.csv" + "\n")
                elif line.find("all_sign_as_real_file_name=") == 0:
                    list_temp.append("all_sign_as_real_file_name=" + self._org_image + "\\all_sar_files_name.csv" + "\n")
                elif line.find("jv_flag=") == 0:
                    list_temp.append("jv_flag=" + self._jv_flag + "\n")                    
                elif line.find("jv_country=") == 0:
                    list_temp.append("jv_country=" + self._jv_country + "\n")
                elif line.find("forceguide_patch_full_path=") == 0 or line.find("#forceguide_patch_full_path=") == 0:
                    if self._company == 'HERE' and self._binformat == 'F16':                    
                        list_temp.append("forceguide_patch_full_path=" + self._org_image + "\\temp_patch_force_guide_tbl.csv" + "\n")
                    else:
                        list_temp.append(line)                  
                #config.ini
                elif line.find("db=host=") == 0:
                    list_temp.append("db=host=\'" + self._db_ip + "\' dbname=\'" + self._dbname_str + pg_key)
                elif line.find("country_name=") == 0:
                    list_temp.append("country_name=" + self._country + "\n")
                #tool\\config.txt
                elif line.find("json_path=") == 0:
                    list_temp.append("json_path=" + aut_road_path+ "\n")
                elif line.find("name=") == 0:
                    list_temp.append("name=" + self._dbname_str+ "\n")
                elif line.find("host=") == 0:
                    list_temp.append("host=" + self._db_ip+ "\n")                             
                #convert_db.bat,autocheck.bat
                elif line.find("set basedir=") == 0:
                    list_temp.append("set basedir=" + self._codepath + "\n")
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
    
    def __getdb_num(self, dbname_front, line):
        return int(line[len(dbname_front)+1:len(dbname_front)+5])
    
    def getdestpath(self):
        return CONFIG_ROOT_PATH
    
    def getdbver(self):
        return self._dbver
    
    def getcode_config_path(self):
        return self._code_config
    
    def getdbname_str(self):
        return self._dbname_str 

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
    
    def __GetOrgDir(self):
        if self._company == '' or self._country == '' or DATA_ROOT_PATH is None:
            return -1
        self._org_backup = ''
        self._org_image = ''
        org_backup_temp = self._country + "\\" + self._company + "\\" + self._company + "_" + self._org_ver
        data_root_path_org = os.path.join(DATA_ROOT_PATH,"Data_Original")
        try:
            for f in os.listdir(data_root_path_org):
                path_temp = os.path.join(data_root_path_org,f)
                for f_country in os.listdir(path_temp):
                    if f_country == self._area_name:
                        path_temp = os.path.join(path_temp,f_country)
                        path_temp = os.path.join(path_temp,org_backup_temp)
                        self._org_backup = os.path.join(path_temp,self._org_db + ".backup")
                        self._org_image = os.path.join(path_temp,"image")
                        self._id_fund_backup = os.path.join(path_temp,self._last_db_name + "_id_fund.backup")
                        return 0
        except:
            self.log.error("get org path %s %s in %s error",self._company,self._country,data_root_path_org)
            return -1
        
        return 0

#copy dir ,if file appear in target_path,no copy    
def copydir(source_path, target_path, dir_copy_flage = True, exclude_file = ''):
    if source_path is None or target_path is None or len(source_path) < 1 or len(target_path) < 1:
        return -1
    
    if not os.path.exists(target_path):
        os.makedirs(target_path)
    
    try:    
        for f in os.listdir(source_path):
            source_path_file = os.path.join(source_path,f)
            target_path_file = os.path.join(target_path,f)

            
            if os.path.isdir(source_path_file):
                if dir_copy_flage:
                    copydir(source_path_file,target_path_file,dir_copy_flage,exclude_file)
                else:
                    copydir(source_path_file,target_path, dir_copy_flage,exclude_file)
            
            if os.path.isfile(source_path_file):
                if os.path.exists(target_path_file):
                    os.remove(target_path_file)
                    
                if exclude_file == '':
                    open(target_path_file,"wb").write(open(source_path_file,"rb").read())
                else:
                    file_name_temp = os.path.basename(source_path_file)
                    if (file_name_temp[-len(exclude_file):]).lower() == exclude_file.lower():
                        continue
                    else:
                        open(target_path_file,"wb").write(open(source_path_file,"rb").read())
                    
    except:
        return -1
    
    return 0