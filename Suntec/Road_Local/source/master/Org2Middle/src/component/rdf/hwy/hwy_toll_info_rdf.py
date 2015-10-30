# -*- coding: UTF-8 -*-
'''
Created on 2015-9-29

@author: hcz
'''
from component.jdb.hwy.hwy_toll_info import HwyTollInfo
from component.jdb.hwy.hwy_toll_info import HwyTollType
from component.jdb.hwy.hwy_def import IC_TYPE_TRUE
from component.jdb.hwy.hwy_def import IC_DEFAULT_VAL


# ===============================================================================
# HwyTollInfoRDF
# ===============================================================================
class HwyTollInfoRDF(HwyTollInfo):
    def __init__(self, ic_info, toll_facil,
                 facil, path_type,
                 conn_ic_no=None, conn_facil=None,
                 link_id=None):
        HwyTollInfo.__init__(self, ic_info, toll_facil,
                             facil, path_type,
                             conn_ic_no, conn_facil)
        self.link_id = link_id

    def set_toll_info(self):
        self._set_toll_class()
        self._set_updown()
        self._set_facility_id()
        self.toll_type = HwyTollTypeRDF(self)
        self.toll_type.set_type_info()

    def _set_facility_id(self):
        self.facility_id = self.ic_info.facility_id


# =============================================================================
# HwyTollType
# =============================================================================
class HwyTollTypeRDF(HwyTollType):
    def __init__(self, toll_info=None):
        HwyTollType.__init__(self, toll_info)

    def _set_toll_function(self):
        '''料金机能种别'''

    def _exist_mulit_facils(self, node_id):
        return False

    def _set_nest_kind(self):
        self.nest = IC_DEFAULT_VAL

    def _is_unique_toll(self, toll_facil):
        self.nest = IC_DEFAULT_VAL
