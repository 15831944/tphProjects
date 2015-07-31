# -*- coding: UTF8 -*-
'''
Created on 2012-12-5

@author: zym
'''
import base
import codecs
from common.common_func import GetPath


class comp_illustname_nostra(base.component_base.comp_base):
    '''
    make link_tbl
    '''
    def __init__(self):
        '''
        Constructor
        '''
        base.component_base.comp_base.__init__(self, 'illustname')

    def _DoCreateTable(self):

        if self.CreateTable2('temp_junctionview_name_sort') == -1:
            return -1

        return 0

    def _Do(self):
        tempcolums = (
                    'pic_name', 'first_arrow_name',
                    'second_arrow_name', 'en_roadname1',
                    'en_roadname2', 'th_roadname1', 'th_roadname2'
                    )
        filepath = GetPath('illust_name_mapping')
        objFile = codecs.open(filepath, 'r', 'utf8')
        self.pg.copy_from2(objFile,
                           'temp_junctionview_name_sort',
                           '\t', "",
                           8192,
                           tempcolums)
        self.pg.commit2()
