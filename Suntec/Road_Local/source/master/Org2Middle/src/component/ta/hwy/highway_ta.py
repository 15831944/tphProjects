# -*- coding: UTF8 -*-
'''
Created on 2015-3-18

@author: hcz
'''
from component.rdf.hwy.highway_rdf import HighwayRDF
from component.ta.hwy.hwy_link_mapping_ta import HwyLinkMappingTa
from component.ta.hwy.hwy_data_mng_ta import HwyDataMngTa
from component.ta.hwy.hwy_route_ta import HwyRouteTa
from component.ta.hwy.hwy_facility_ta import HwyFacilityTa
from component.ta.hwy.hwy_facility_ta import HwyFacilityTaSaf
from component.rdf.hwy.hwy_mapping_rdf import HwyMappingRDF
from component.ta.hwy.hwy_exit_name_ta import HwyExitNameTa
from component.ta.hwy.hwy_sapa_info_ta import HwySapaInfoTa
from component.rdf.hwy.hwy_service_info_rdf import HwyServiceInfoRDF
from component.rdf.hwy.hwy_store_rdf import HwyStoreRDF
from component.rdf.hwy.hwy_adjust_link_type_rdf import HwyAdjustLinkType
from common.common_func import getProjCountry


class HighwayTa(HighwayRDF):
    '''
    Highway Data for TomTom or SENSIS
    '''

    def __init__(self):
        '''
        Constructor
        '''
        HighwayRDF.__init__(self, ItemName='HighwayTa')

    def initialize(self):
        self.link_id_mapping = HwyLinkMappingTa()
        self.hwy_exit = HwyExitNameTa()
        self.sapa_info = HwySapaInfoTa()
        self.data_mng = HwyDataMngTa.instance()
        self.data_mng.initialize()
        self.hwy_route = HwyRouteTa(self.data_mng)
        country = getProjCountry().upper()
        if country in ("SAF", "SAF8",):  # 南非
            self.hwy_facil = HwyFacilityTaSaf(self.data_mng)
        elif country in ("AUS",):  # 澳洲
            self.hwy_facil = HwyFacilityTa(self.data_mng)
        else:
            self.log.error('Wrong PROJ Country.')
        self.hwy_mapping = HwyMappingRDF(self)
        self.service_info = HwyServiceInfoRDF()
        self.store = HwyStoreRDF()
        self.adjust_link = HwyAdjustLinkType()
