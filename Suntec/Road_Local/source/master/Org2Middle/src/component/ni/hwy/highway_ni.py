# -*- coding: UTF8 -*-
'''
Created on 2015-4-29

@author: hcz
'''
from component.rdf.hwy.highway_rdf import HighwayRDF
from component.rdf.hwy.hwy_route_rdf import HwyRouteRDF_HKG
from component.rdf.hwy.hwy_mapping_rdf import HwyMappingRDF
from component.rdf.hwy.hwy_service_info_rdf import HwyServiceInfoRDF
from component.rdf.hwy.hwy_store_rdf import HwyStoreRDF
from component.rdf.hwy.hwy_adjust_link_type_rdf import HwyAdjustLinkType
from component.ni.hwy.hwy_data_mng_ni import HwyDataMngNi
from component.ni.hwy.hwy_link_mapping_ni import HwyLinkMappingNi
from component.ni.hwy.hwy_exit_enter_poi_name_ni import HwyExitEnterNameNi
from component.ni.hwy.hwy_sapa_info_ni import HwySaPaInfoNi
from component.ni.hwy.hwy_facility_ni import HwyFacilityNi


class HighwayNi(HighwayRDF):
    '''Highway For China NavInfo
    '''

    def __init__(self, ItemName='HighwayNi'):
        '''
        Constructor
        '''
        HighwayRDF.__init__(self, ItemName)

    def initialize(self):
        self.link_id_mapping = HwyLinkMappingNi()
        self.hwy_exit_poi = HwyExitEnterNameNi()
        self.hwy_exit = None
        self.sapa_info = HwySaPaInfoNi()
        self.data_mng = HwyDataMngNi.instance()
        if self.data_mng:
            self.data_mng.initialize()
        self.hwy_route = HwyRouteRDF_HKG(self.data_mng,
                                         ItemName='HwyRouteNi')
        self.hwy_facil = HwyFacilityNi(self.data_mng)
        self.hwy_mapping = HwyMappingRDF(self)
        self.service_info = HwyServiceInfoRDF()
        self.store = HwyStoreRDF()
        self.adjust_link = HwyAdjustLinkType()
