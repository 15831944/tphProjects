# -*- coding: UTF8 -*-
'''
Created on 2015-04-10

@author: hcz
'''
from component.rdf.hwy.highway import HighwayRDF
from component.rdf.hwy.hwy_data_mng_rdf import HwyDataMngRDF
from component.rdf.hwy.hwy_route_rdf import HwyRouteRDF
from component.rdf.hwy.hwy_facility_rdf import HwyFacilityRDF
from component.rdf.hwy.hwy_mapping_rdf import HwyMappingRDF
from component.rdf.hwy.hwy_service_info_rdf import HwyServiceInfoRDF
from component.rdf.hwy.hwy_store_rdf import HwyStoreRDF
from component.rdf.hwy.hwy_adjust_link_type_rdf import HwyAdjustLinkType
from component.mmi.hwy.hwy_link_mapping_mmi import HwyLinkMappingMMi


class HighwayMMi(HighwayRDF):
    '''
    Highway Data for MMi
    '''

    def __init__(self):
        '''
        Constructor
        '''
        HighwayRDF.__init__(self)

    def initialize(self):
        self.link_id_mapping = HwyLinkMappingMMi()
        self.hwy_exit = None
        self.sapa_info = None
        self.data_mng = None  # HwyDataMngRDF.instance()
        if self.data_mng:
            self.data_mng.initialize()
        self.hwy_route = None  # HwyRouteRDF(self.data_mng)
        self.hwy_facil = None  # HwyFacilityRDF(self.data_mng)
        self.hwy_mapping = None  # HwyMappingRDF(self)
        self.service_info = None  # HwyServiceInfoRDF()
        self.store = None  # HwyStoreRDF()
        self.adjust_link = None  # HwyAdjustLinkType()
