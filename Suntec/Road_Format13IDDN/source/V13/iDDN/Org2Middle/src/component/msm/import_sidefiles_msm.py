# -*- coding: UTF-8 -*-
'''
Created on 2014-8-13

@author: hcz
'''
from base.component_base import comp_base
import common
import io
import os
# import codecs


#==============================================================================
# comp_import_sidefiles_msm(Malsing)
#==============================================================================
class comp_import_sidefiles_msm(comp_base):
    '''
    '''

    def __init__(self):
        comp_base.__init__(self, 'sidefiles')

    def _DoCreateTable(self):
        return 0

    def _DoCreateIndex(self):
        return 0

    def _DoCreateFunction(self):
        return 0

    def _Do(self):
        self._import_road_replaced_name()
        pass

    def _import_road_replaced_name(self):
        '''被替换的道路名称'''
        self.CreateTable2('road_replaced_name')
        file_obj = self._open_file('replaced_name')
        if file_obj:
            file_obj.readline()  # 跳过第一行
            self.pg.copy_from2(file_obj, 'road_replaced_name',
                               columns=('org_name', 'replace_name',
                                        'reference')
                               )
            self.pg.commit2()

    def _open_file(self, name):
        path = common.GetPath(name)
        if not path:
            self.log.error('Does not indicate path for %s' % name)
            return None
        return open(path)
