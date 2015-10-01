# -*- coding: UTF8 -*-
'''
Created on 2013-12-9

@author: hongchenzai
'''


def open(temp_file_name, mode='a+', encoding='utf8',
         errors='strict', buffering=1):
    '''创建一个临时文件，用temp_file_name做基本名称，当前时间做后缀。并用可读写方式打开。'''
    if temp_file_name is not None and len(temp_file_name) > 0:
        import os
        import time
        temp_folder = os.path.join(os.getcwd(), 'cache')
        try:
            os.stat(temp_folder)  # 测试文件路径存不存在
        except:
            os.makedirs(temp_folder)  # 创建文件路径
        temp_file_name += '_' + time.strftime('%Y-%m-%d %H_%M_%S',
                                                time.localtime(time.time())
                                                )
        import codecs
        file_obj = codecs.open(os.path.join(temp_folder, temp_file_name),
                                   mode, encoding, errors, buffering)
        # Python自带的创建临时文件
        # tempfile.NamedTemporaryFile()
        return file_obj
    else:
        return None
    
def close(temp_file_name,flage = False):
       
    temp_file_name.close()
       
    if flage:
        import os
        try:                
            os.remove(temp_file_name.name)
        except Exception,e:          
            print Exception,":",e
        
    return 0
