# -*- coding: UTF8 -*-
'''
Created on 2012-11-29

@author: hongchenzai
'''

import logging
import os
import sys
import time
import traceback

#######################################################################
## CLogger类
#######################################################################
class CLogger:
    '''Log类
    '''
    __instance = None
    @staticmethod
    def instance():
        if CLogger.__instance is None:
            CLogger.__instance = CLogger()
        return CLogger.__instance
        
    def __init__(self):
        self.dictLogger = {}
        
        log_folder = os.path.join(os.getcwd(), 'log')
        try:
            os.stat(log_folder)            # 测试文件路径存不存在
        except:
            os.makedirs(log_folder)        # 创建文件路径
        log_filename = 'log' + time.strftime('%Y-%m-%d %H_%M_%S',time.localtime(time.time())) + '.txt'
        self.logfile = os.path.join(log_folder, log_filename)
    
    def init(self):
        # handler
        self.objSysHandler = CCommonSysoutHandler()
        self.objLogHandler = CCommonFileHandler(self.logfile)
        
        # formatter
        self.objFormatter = logging.Formatter("%(asctime)s - %(levelname)s: %(name)s----%(message)s")
        self.objSysHandler.setFormatter(self.objFormatter)
        self.objLogHandler.setFormatter(self.objFormatter)
        
    def logger(self, name):
        if not self.dictLogger.has_key(name):
            objLogger = CCommonLogger(name)
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
  
#######################################################################
## CCommonLogger类
#######################################################################      
class CCommonLogger(logging.Logger):
    def __init__(self, name):
        logging.Logger.__init__(self, name)

#######################################################################
## CCommonSysoutHandler类
#######################################################################
class CCommonSysoutHandler(logging.StreamHandler):
    def __init__(self):
        logging.StreamHandler.__init__(self, sys.stdout)

#######################################################################
## CCommonFileHandler类
#######################################################################
class CCommonFileHandler(logging.FileHandler):
    def __init__(self, filename):
        logging.FileHandler.__init__(self, filename)
        