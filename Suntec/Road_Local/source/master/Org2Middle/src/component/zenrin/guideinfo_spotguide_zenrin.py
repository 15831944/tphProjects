# -*- coding: cp936 -*-
'''
Created on 2015-06-23

@author: tangpinghui
'''
from component.default.guideinfo_spotguide import comp_guideinfo_spotguide

class comp_guideinfo_spotguide_mmi(comp_guideinfo_spotguide):
    '''
    This class is used for zenrin spotguide
    '''

    def __init__(self):
        comp_guideinfo_spotguide.__init__(self)

    def _DoCreateTable(self):
        return 0

    def _DoCreatIndex(self):

        return 0

    def _DoCreateFunction(self):
        return 0

    def _Do(self):
        return















 