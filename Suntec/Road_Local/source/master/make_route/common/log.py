# -*- coding: UTF8 -*-

import logging
import os
import sys
import time

class common_log:
    __instance = None
    @staticmethod
    def instance():
        if common_log.__instance is None:
            common_log.__instance = common_log()
        return common_log.__instance
        
    def __init__(self):
        self.dictLogger = {}
        
        log_folder = os.path.join(os.getcwd(), 'log')
        if not os.path.exists(log_folder):
            os.makedirs(log_folder)        # �����ļ�·��
        log_filename = 'log' + time.strftime('%Y-%m-%d %H_%M_%S',time.localtime(time.time())) + '.txt'
        self.logfile = os.path.join(log_folder, log_filename)
    
    def init(self):
        # handler
        self.objSysHandler = common_sysout_handler()
        self.objLogHandler = common_file_handler(self.logfile)
        
        # formatter
        self.objFormatter = logging.Formatter("%(asctime)s - %(levelname)s: %(name)s----%(message)s")
        self.objSysHandler.setFormatter(self.objFormatter)
        self.objLogHandler.setFormatter(self.objFormatter)
        
    def logger(self, name):
        if not self.dictLogger.has_key(name):
            objLogger = common_logger(name)
            objLogger.addHandler(self.objSysHandler)
            objLogger.addHandler(self.objLogHandler)
            self.dictLogger[name] = objLogger
        return self.dictLogger[name]
    
    def end(self):
        for logger in self.dictLogger.values():
            logger.removeHandler(self.objLogHandler)
            logger.removeHandler(self.objSysHandler)
        self.objLogHandler.close()
    
    def get_logfile(self):
        return self.logfile
        
class common_logger(logging.Logger):
    def __init__(self, name):
        logging.Logger.__init__(self, name)

class common_sysout_handler(logging.StreamHandler):
    def __init__(self):
        logging.StreamHandler.__init__(self, sys.stdout)

class common_file_handler(logging.FileHandler):
    def __init__(self, filename):
        logging.FileHandler.__init__(self, filename)
        