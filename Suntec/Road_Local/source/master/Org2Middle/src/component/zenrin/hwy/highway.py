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
from component.rdf.hwy.hwy_def_rdf import HWY_INVALID_FACIL_ID_17CY


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

    def _make_facility_id(self):
        '''生成1到N的设施番号，边界点没有设施号'''
        self.CreateTable2('mid_hwy_facility_id')
        sqlcmd = """
        INSERT INTO mid_hwy_facility_id(road_code, updown_c, road_seq)
        (
        SELECT distinct road_code, updown_c, road_seq
          from hwy_node
          order by road_code, updown_c, road_seq
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('mid_hwy_facility_id_road_code_road_seq_idx')

        # 更新边界点的设施id, 边界点的设施id设成[无效设施id]
        sqlcmd = """
        UPDATE mid_hwy_ic_no set facility_id = %s
          WHERE road_seq = %s;
        """
        self.pg.execute2(sqlcmd, (HWY_INVALID_FACIL_ID_17CY,
                                  HWY_INVALID_FACIL_ID_17CY))
        self.pg.commit2()

        # 更新ic_no表的设施id
        sqlcmd = """
        UPDATE mid_hwy_ic_no as ic set facility_id = facil.facility_id
          FROM mid_hwy_facility_id as facil
          WHERE ic.road_code = facil.road_code
                and ic.road_seq = facil.road_seq;
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
