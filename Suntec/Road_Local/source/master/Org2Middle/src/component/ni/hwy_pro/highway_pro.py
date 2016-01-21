# -*- coding: UTF-8 -*-
'''
Created on 2015-10-26

@author: hcz
'''
from component.rdf.hwy.hwy_def_rdf import HWY_INVALID_FACIL_ID_17CY
from component.rdf.hwy.highway_rdf import HighwayRDF
from component.rdf.hwy.hwy_adjust_link_type_rdf import HwyAdjustLinkType
from component.rdf.hwy.hwy_service_info_rdf import HwyServiceInfoRDF
from component.rdf.hwy.hwy_store_rdf import HwyStoreRDF
from component.rdf.hwy.hwy_tollgate_rdf import HwyTollgateRDF
from component.rdf.hwy.hwy_mapping_rdf import HwyMappingRDF
# from component.rdf.hwy.hwy_node_addinfo_rdf import HwyNodeAddInfoRDF
from component.ni.hwy_pro.hwy_data_mng_pro import HwyDataMngNiPro
from component.ni.hwy_pro.hwy_route_pro import HwyRouteNiPro
from component.ni.hwy_pro.hwy_link_mapping_pro import HwyLinkMappingNiPro
from component.ni.hwy_pro.hwy_facility_pro import HwyFacilityNiPro
from component.ni.hwy_pro.hwy_ic_info_pro import HwyICInfoNIPro
from component.rdf.hwy.hwy_ic_info_rdf import HwyBoundaryOutInfoRDF
from component.rdf.hwy.hwy_ic_info_rdf import HwyBoundaryInInfoRDF
from component.ni.hwy_pro.hwy_line_name_pro import HwyLineNameNiPro
from component.ni.hwy_pro.hwy_facil_name_pro import HwyFacilNameNiPro
from component.ni.hwy_pro.hwy_sapa_info_pro import HwySaPaInfoNiPro
from component.ni.hwy_pro.hwy_node_add_info_pro import HwyNodeAddInfoNiPro
from component.ni.hwy_pro.hwy_parallel_path_pro import HwyParallelPathNiPro


class HighwayNiPro(HighwayRDF):
    '''
    classdocs
    '''

    def __init__(self, ItemName='HighwayNiPro'):
        '''
        Constructor
        '''
        HighwayRDF.__init__(self, ItemName)
        self.hwy_parallel = None

    def initialize(self):
        self.link_id_mapping = HwyLinkMappingNiPro()
        self.hwy_facil_name = HwyFacilNameNiPro()
        self.hwy_line_name = HwyLineNameNiPro()
        self.data_mng = HwyDataMngNiPro.instance()
        if self.data_mng:
            self.data_mng.initialize()
        self.hwy_route = HwyRouteNiPro(self.data_mng)
        self.hwy_facil = HwyFacilityNiPro(self.data_mng)
        self.hwy_mapping = HwyMappingRDF(self)
        self.service_info = HwyServiceInfoRDF()
        self.store = HwyStoreRDF()
        self.adjust_link = HwyAdjustLinkType()
        self.node_addinfo = HwyNodeAddInfoNiPro(self.data_mng)
        self.sapa_info = HwySaPaInfoNiPro()
        self.hwy_toll = HwyTollgateRDF()
        self.hwy_parallel = HwyParallelPathNiPro()

    def _Do(self):
        self.initialize()
        from component.default.dictionary import comp_dictionary
        dictionary = comp_dictionary()
        dictionary.set_language_code()
        # ## 设施名称
        if self.hwy_facil_name:
            self.hwy_facil_name.Make()
            del self.hwy_facil_name
            self.hwy_facil_name = None
        # ## 原link id ==> Middle link id
        if self.link_id_mapping:
            self.link_id_mapping.Make()  # ORG Link_id ==> Middle Link_id
            del self.link_id_mapping
            self.link_id_mapping = None
        # ## 高速线路名称
        if self.hwy_line_name:
            self.hwy_line_name.Make()
            del self.hwy_line_name
            self.hwy_line_name = None
        if self.sapa_info:
            self.sapa_info.Make()
        if self.data_mng:
            self.data_mng.Make()
            # load Highway Main Link.
            self.data_mng.load_hwy_main_link()
            self.data_mng.load_hwy_ic_link()
            self.data_mng.load_first_ics_link()
            self.data_mng.load_hwy_path_id()
        else:
            self.log.error('Higway Data Manager is None.')
            return 0
        # ## 高速路线
        self._make_hwy_route()
        if self.hwy_parallel:
            self.hwy_parallel.Make()
        if self.data_mng:
            # 加载高速road_code
            self.data_mng.load_hwy_road_code()
            self.data_mng.load_hwy_regulation()
            self.data_mng.load_tollgate()
            self.data_mng.load_hwy_ic_link()
            self.data_mng.load_hwy_inout_link()
            self.data_mng.load_hwy_path_id()
            # 加载专用数据设施番号
            self.data_mng.load_org_facil_id()
        # 制作高速线路及设施(原始设施情报)
        self._make_hwy_facil()
        # 收费站情报
        if self.hwy_toll:
            self.hwy_toll.Make()
        # ## 道路情报
        self._make_road_no()  # 道路番号
        self._make_road_info()  # 道路情报
        # ## 设置ic_no
        if self.data_mng:
            self.data_mng.load_hwy_node_facilcls()  # 加载设施种别
        if self._make_ic_no():  # ic_no
            self._make_facility_id()  # 设施ID
        # Mapping
        if self.hwy_mapping:
            self.hwy_mapping.Make()
        # 设施情报
        if self.data_mng:
            self._make_ic_info()
        # 附加情报(最终)
        if self.node_addinfo:
            self.node_addinfo.Make()
        # 服务情报(For SAPA)
        if self.service_info:
            self.service_info.Make()
        # 店铺情报(For SAPA)
        if self.store:
            self.store.Make()
        # 调整link_type: Ramp ==> SAPA, Ramp ==> JCT
#         if self.adjust_link:
#             self.adjust_link.Make()

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
                ic_info = HwyICInfoNIPro(ic_no, facility_id,
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
