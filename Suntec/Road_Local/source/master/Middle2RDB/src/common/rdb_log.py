# -*- coding: cp936 -*-
'''
Created on 2011-9-29

@author: hongchenzai
'''

import logging
import os
import sys
import time
import traceback
log_path_file = ''

def init():
    global log_path_file
    log_path = os.path.join(os.getcwd(), 'log')
    try:
        os.stat(log_path)            # 测试文件路径存不存在
    except:
        os.makedirs(log_path)        # 创建文件路径
    logfile = 'log' + time.strftime('%Y-%m-%d %H_%M_%S',time.localtime(time.time())) + '.txt'
    log_path_file = os.path.join(log_path, logfile)
    logging.basicConfig(filename = os.path.join(log_path, logfile), \
                        level = logging.INFO, filemode = 'a', format = '%(asctime)s - %(levelname)s: %(message)s') 

    #logging.basicConfig(level = logging.INFO, format = '%(asctime)s - %(levelname)s: %(message)s')
    print '===================================Start==========================================='
    logging.info('===================================Start===========================================')
    
    return 0

def log(module_name, message, level):
    """Output the log, 
    level has the value: 'debug', 'warning', 'info', 'error' 
    """
    if level == 'debug':
        print time.strftime('%Y-%m-%d %H:%M.%S',time.localtime(time.time())), 'debug: ' + module_name + '----' + message
        logging.info(module_name + '----' + message)
    elif level == 'warning':
        print time.strftime('%Y-%m-%d %H:%M.%S',time.localtime(time.time())), 'warning: ' + module_name + '----' + message
        logging.warn(module_name + '----' + message)
    elif level == 'info':
        print time.strftime('%Y-%m-%d %H:%M.%S',time.localtime(time.time())), 'info: ' + module_name + '----' + message
        logging.info(module_name + '----' + message)
    elif level == 'exception':
        print time.strftime('%Y-%m-%d %H:%M.%S',time.localtime(time.time())), 'error: ' + module_name + '----' + message
        print traceback.format_exc()
        logging.exception(module_name + '----' + message)
    else : # error
        print time.strftime('%Y-%m-%d %H:%M.%S',time.localtime(time.time())), 'error: ' + module_name + '----' + message
        logging.error(module_name + '----' + message)
    sys.stdout.flush()#fix bug: console output delay
    return 0

def end():
    print '====================================End============================================'
    logging.info('=====================================End=============================================')
    return 0

def get_logfile():
    '返回log文件的全路径及文件名.'
    global log_path_file
    return log_path_file
