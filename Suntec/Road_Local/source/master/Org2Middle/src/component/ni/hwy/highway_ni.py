# -*- coding: UTF8 -*-
'''
Created on 2015-4-29

@author: hcz
'''
from component.rdf.hwy.highway_rdf import HighwayRDF
from component.rdf.hwy.hwy_mapping_rdf import HwyMappingRDF
from component.rdf.hwy.hwy_service_info_rdf import HwyServiceInfoRDF
from component.rdf.hwy.hwy_store_rdf import HwyStoreRDF
from component.rdf.hwy.hwy_adjust_link_type_rdf import HwyAdjustLinkType
from component.ni.hwy.hwy_data_mng_ni import HwyDataMngNi
from component.ni.hwy.hwy_link_mapping_ni import HwyLinkMappingNi
from component.ni.hwy.hwy_exit_enter_poi_name_ni import HwyExitEnterNameNi
from component.ni.hwy.hwy_sapa_info_ni import HwySaPaInfoNi
from component.ni.hwy.hwy_facility_ni import HwyFacilityNi
from component.ni.hwy.hwy_route_ni import HwyRouteNi
from component.rdf.hwy.hwy_node_addinfo_rdf import HwyNodeAddInfoRDF
from component.rdf.hwy.hwy_tollgate_rdf import HwyTollgateRDF
from component.rdf.hwy.hwy_def_rdf import HWY_INVALID_FACIL_ID_17CY
from component.ni.hwy.hwy_ic_info_ni import HwyICInfoNI
from component.rdf.hwy.hwy_ic_info_rdf import HwyBoundaryOutInfoRDF
from component.rdf.hwy.hwy_ic_info_rdf import HwyBoundaryInInfoRDF


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
        self.hwy_route = HwyRouteNi(self.data_mng)
        self.hwy_facil = HwyFacilityNi(self.data_mng)
        self.hwy_mapping = HwyMappingRDF(self)
        self.service_info = HwyServiceInfoRDF()
        self.store = HwyStoreRDF()
        self.adjust_link = HwyAdjustLinkType()
        self.node_addinfo = HwyNodeAddInfoRDF(self.data_mng,
                                              'HwyNodeAddInfoNI')
        self.hwy_toll = None  # HwyTollgateRDF()

    def _make_ic_info(self):
        self.log.info('Making IC Info.')
        self.pg.connect1()
        self.pg.connect2()
        self.CreateTable2('highway_ic_info')
        self.CreateTable2('highway_path_point')
        self.CreateTable2('highway_conn_info')
        self.CreateTable2('highway_toll_info')
        self.CreateTable2('mid_hwy_node_add_info')
        # self.CreateTable2('mid_hwy_node_add_info')
        for ic_no, facility_id, facil_list in self.data_mng.get_ic_list():
            if facility_id != HWY_INVALID_FACIL_ID_17CY:  # 非边界点
                ic_info = HwyICInfoNI(ic_no, facility_id,
                                      facil_list, self.data_mng)
                ic_info.set_ic_info()  # 设置料金情报
                self._insert_ic_info(ic_info)
                self._insert_path_point(ic_info)
                self._insert_conn_info(ic_info)
                # 先不做收费站情报
                # self._insert_toll_info(ic_info)
                self._insert_temp_add_info(ic_info)
            else:
                # 边界出口点（离开当前Tile）
                ic_info = HwyBoundaryOutInfoRDF(ic_no, facility_id,
                                                facil_list, self.data_mng)
                ic_info.set_ic_info()
                self._insert_ic_info(ic_info)
                self._insert_path_point(ic_info)
                # 边界进入点（进入Tile的点）
                ic_info = HwyBoundaryInInfoRDF(ic_no, facility_id,
                                               facil_list, self.data_mng)
                ic_info.set_ic_info()
                self._insert_ic_info(ic_info)
                self._insert_path_point(ic_info)
        self.pg.commit1()
        self.CreateIndex2('highway_ic_info_ic_no_idx')
        self.CreateIndex2('highway_toll_info_ic_no_conn_ic_no_node_id_idx')
        self.CreateIndex2('highway_conn_info_ic_no_conn'
                          '_ic_no_conn_direction_idx')
        self._add_geom_column()
