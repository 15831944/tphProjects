# -*- coding: UTF8 -*-
'''
Created on 2015-06-23

@author: tangpinghui
'''

from component.default.guideinfo_signpost_uc import comp_guideinfo_signpost_uc

class comp_guideinfo_signpost_uc_mmi(comp_guideinfo_signpost_uc):
    '''
            此文件主要处理台湾哲林的signpost_uc数据。
    '''

    def __init__(self):
        comp_guideinfo_signpost_uc.__init__(self)

    def _Do(self):
        return 0