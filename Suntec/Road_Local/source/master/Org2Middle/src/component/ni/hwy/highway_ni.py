# -*- coding: UTF8 -*-
'''
Created on 2015-4-29

@author: hcz
'''
from component.rdf.hwy.highway_rdf import HighwayRDF
from component.ni.hwy.hwy_link_mapping_ni import HwyLinkMappingNi


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
        self.hwy_exit = None  # HwyExitNameTa()
        self.sapa_info = None  # HwySapaInfoTa()
        self.data_mng = None  # HwyDataMngTa.instance()
        if self.data_mng:
            self.data_mng.initialize()
        self.hwy_route = None  # HwyRouteTa(self.data_mng)
        self.hwy_facil = None  # HwyFacilityTa(self.data_mng)
        self.hwy_mapping = None  # HwyMappingRDF(self)
        self.service_info = None  # HwyServiceInfoRDF()
        self.store = None  # HwyStoreRDF()
        self.adjust_link = None  # HwyAdjustLinkType()
