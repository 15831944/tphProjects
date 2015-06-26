# -*- coding: UTF8 -*-
'''
Created on 2015-3-19

@author: zhaojie
'''
import platform.TestCase
from check.rdb import rdb_common_check

class CCheckPassLinkCount(rdb_common_check.CCheckPassLinkCount):
    def __init__(self, suite, caseinfo):
        rdb_common_check.CCheckPassLinkCount.__init__(self, suite, caseinfo, 'rdb_guideinfo_hook_turn_client', 'passlink_count')
        pass