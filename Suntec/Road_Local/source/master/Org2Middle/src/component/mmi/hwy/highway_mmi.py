# -*- coding: UTF8 -*-
'''
Created on 2015-04-10

@author: hcz
'''
from component.rdf.hwy.highway_rdf import HighwayRDF
from component.mmi.hwy.hwy_data_mng_mmi import HwyDataMngMMi
from component.rdf.hwy.hwy_route_rdf import HwyRouteRDF_HKG
from component.mmi.hwy.hwy_facility_mmi import HwyFacilityMmi
from component.rdf.hwy.hwy_mapping_rdf import HwyMappingRDF
from component.rdf.hwy.hwy_service_info_rdf import HwyServiceInfoRDF
from component.rdf.hwy.hwy_store_rdf import HwyStoreRDF
from component.rdf.hwy.hwy_adjust_link_type_rdf import HwyAdjustLinkType
from component.mmi.hwy.hwy_link_mapping_mmi import HwyLinkMappingMMi
from component.mmi.hwy.hwy_sapa_info_mmi import HwySapaInfoMMI
from component.rdf.hwy.hwy_def_rdf import ROUTE_DISTANCE_1600M


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
        self.sapa_info = HwySapaInfoMMI()
        self.data_mng = HwyDataMngMMi.instance()
        if self.data_mng:
            self.data_mng.initialize()
        self.hwy_route = HwyRouteRDF_HKG(self.data_mng,
                                         min_distance=ROUTE_DISTANCE_1600M,
                                         margin_dist=ROUTE_DISTANCE_1600M,
                                         ItemName='Highway_Route_Mmi')
        self.hwy_facil = HwyFacilityMmi(self.data_mng)
        self.hwy_mapping = HwyMappingRDF(self)
        self.service_info = HwyServiceInfoRDF()
        self.store = HwyStoreRDF()
        self.adjust_link = HwyAdjustLinkType()
