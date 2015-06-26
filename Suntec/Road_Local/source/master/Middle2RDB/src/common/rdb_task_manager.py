# -*- coding: cp936 -*-
'''
Created on 2012-3-15

@author: hongchenzai
'''
import rdb_task

class rdb_task_manager(object):
    '''
    1.根据输入的参数 -a: 所以有的task重新做,并先清空task log， -c: 接着上一次，往下做, -i: 不做某些Task
    2.每完成一个Task，就往task log写入记录
    '''

    def __init__(self):
        '''
        Constructor
        '''
        