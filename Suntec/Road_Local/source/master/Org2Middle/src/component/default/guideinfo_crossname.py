'''
Created on 2012-2-23

@author: sunyifeng
'''

import io
import os

import component.component_base

class comp_guideinfo_crossname(component.component_base.comp_base):

    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'CrossName')

    def _DoCreateTable(self):
        self.CreateTable2('crossname_tbl')
        return 0

    def _DoCreateFunction(self):
        return 0

    def _Do(self):
        return 0

