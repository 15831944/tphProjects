# -*- coding: cp936 -*-
'''
Created on 2012-6-1

@author: hongchenzai
'''
import threading
#from base.rdb_ItemBase import ItemBase

class rdb_thread(threading.Thread):
    '''RDB thread
    '''
    def __init__(self, item = None):
        threading.Thread.__init__(self)
        self.rdb_item = item
        
    def run(self):
        if self.rdb_item != None:
            self.rdb_item.Make()
        else:
            print "self.rdb_item is None."
