# -*- coding: UTF-8 -*-
'''
Created on 2015-7-2

@author: PC_ZH
'''

from component.rdf.hwy.highway_rdf import HighwayRDF
from component.rdf.hwy.hwy_adjust_link_type_rdf import HwyAdjustLinkType
from component.rdf.hwy.hwy_service_info_rdf import HwyServiceInfoRDF
from component.rdf.hwy.hwy_mapping_rdf import HwyMappingRDF
from component.zenrin.hwy.hwy_data_mng import HwyDataMngZenrin
from component.zenrin.hwy.hwy_route import HwyRouteZenrin
from component.zenrin.hwy.hwy_link_mapping import HwyLinkMappingZenrin
from component.zenrin.hwy.hwy_facility import HwyFacilityZenrin


class HighwayZenrin(HighwayRDF):
    ''' '''

    def __init__(self, ItemName='HighwayZenrin'):
        '''
        Constructor
        '''
        HighwayRDF.__init__(self, ItemName)

    def initialize(self):
        self.data_mng = HwyDataMngZenrin.instance()
        self.data_mng.initialize()
        self.hwy_route = HwyRouteZenrin(self.data_mng)
        self.hwy_facil = HwyFacilityZenrin(self.data_mng)
        self.hwy_mapping = HwyMappingRDF(self)
        self.hwy_exit_poi = None
        self.hwy_exit = None
        self.sapa_info = None
        self.link_id_mapping = HwyLinkMappingZenrin()
        self.service_info = HwyServiceInfoRDF()
        self.store = None
        self.adjust_link = HwyAdjustLinkType()
