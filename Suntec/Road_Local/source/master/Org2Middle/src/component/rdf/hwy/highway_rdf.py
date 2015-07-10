# -*- coding: UTF-8 -*-
'''
Created on 2015-1-7

@author: hcz
'''
from component.jdb.hwy.hwy_ic_info import convert_tile_id
from component.jdb.hwy.highway import Highway
from component.jdb.hwy.hwy_data_mng import HwyFacilInfo
from component.jdb.hwy.hwy_def import HWY_INVALID_FACIL_ID
from component.jdb.hwy.hwy_def import IC_TYPE_INVALID
from component.jdb.hwy.hwy_def import INOUT_TYPE_NONE
from component.jdb.hwy.hwy_def import INOUT_TYPE_OUT
from component.jdb.hwy.hwy_def import INOUT_TYPE_IN
from component.rdf.hwy.hwy_def_rdf import HWY_UPDOWN_TYPE_UP
from component.rdf.hwy.hwy_def_rdf import HWY_TRUE
from component.rdf.hwy.hwy_def_rdf import HWY_FALSE
from component.rdf.hwy.hwy_def_rdf import HWY_TILE_BOUNDARY_NAME
from component.rdf.hwy.hwy_graph_rdf import is_cycle_path
from component.rdf.hwy.hwy_data_mng_rdf import HwyDataMngRDF
from component.rdf.hwy.hwy_route_rdf import HwyRouteRDF
from component.rdf.hwy.hwy_route_rdf import HwyRouteRDF_HKG
from component.rdf.hwy.hwy_facility_rdf import HwyFacilityRDF
from component.rdf.hwy.hwy_mapping_rdf import HwyMappingRDF
from component.rdf.hwy.hwy_ic_info_rdf import HwyICInfoRDF
from component.rdf.hwy.hwy_ic_info_rdf import HwyBoundaryOutInfoRDF
from component.rdf.hwy.hwy_ic_info_rdf import HwyBoundaryInInfoRDF
from component.rdf.multi_lang_name_rdf import MultiLangNameRDF
from component.rdf.hwy.hwy_exit_poi_name_rdf import HwyPoiExitNameRDF
from component.rdf.hwy.hwy_exit_name_rdf import HwyExitNameRDF
from component.rdf.hwy.hwy_sapa_info_rdf import HwySapaInfoRDF
from component.rdf.hwy.hwy_link_mapping import HwyLinkMapping
from component.rdf.hwy.hwy_service_info_rdf import HwyServiceInfoRDF
from component.rdf.hwy.hwy_store_rdf import HwyStoreRDF
from component.rdf.hwy.hwy_adjust_link_type_rdf import HwyAdjustLinkType
from common.common_func import getProjCountry


class HighwayRDF(Highway):
    '''
    classdocs
    '''

    def __init__(self, ItemName='HighwayRDF'):
        '''
        Constructor
        '''
        Highway.__init__(self, ItemName)
        self.hwy_route = None
        self.data_mng = None
        self.hwy_facil = None
        self.hwy_mapping = None
        self.hwy_exit_poi = None
        self.hwy_exit = None
        self.sapa_info = None
        self.link_id_mapping = None
        self.service_info = None
        self.store = None
        self.adjust_link = None

    def initialize(self):
        self.data_mng = HwyDataMngRDF.instance()
        self.data_mng.initialize()
        if getProjCountry().upper() == "HKG":  # 香港
            self.hwy_route = HwyRouteRDF_HKG(self.data_mng)
        else:
            self.hwy_route = HwyRouteRDF(self.data_mng)
        self.hwy_facil = HwyFacilityRDF(self.data_mng)
        self.hwy_mapping = HwyMappingRDF(self)
        self.hwy_exit_poi = HwyPoiExitNameRDF()
        self.hwy_exit = HwyExitNameRDF()
        self.sapa_info = HwySapaInfoRDF()
        self.link_id_mapping = HwyLinkMapping()
        self.service_info = HwyServiceInfoRDF()
        self.store = HwyStoreRDF()
        self.adjust_link = HwyAdjustLinkType()

    def _Do(self):
        self.initialize()
        if self.link_id_mapping:
            self.link_id_mapping.Make()  # ORG Link_id ==> Middle Link_id
        if self.hwy_exit_poi:  # 来源POI
            self.hwy_exit_poi.Make()
        if self.hwy_exit:  # 来源道路名称
            self.hwy_exit.Make()  # 出口番号/交叉点名称
        if self.sapa_info:
            self.sapa_info.Make()  # SAPA情报
        if self.data_mng:
            self.data_mng.Make()
            # load Highway Main Link.
            self.data_mng.load_hwy_main_link()
        # ## 高速路线
        self._make_hwy_route()
        if self.data_mng:
            # 加载高速road_code
            self.data_mng.load_hwy_road_code()
            # 加载元数据的设施号
            self.data_mng.load_org_facil_id()
        # 制作高速线路及设施(原始设施情报)
        self._make_hwy_facil()
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
            self.data_mng.load_hwy_ic_link()  # Temp
            self._make_ic_info()
        # 附加情报(最终)
#         node_addinfo = HwyNodeAddInfo()
#         node_addinfo.Make()
        # 服务情报(For SAPA)
        if self.service_info:
            self.service_info.Make()
        # 店铺情报(For SAPA)
        if self.store:
            self.store.Make()
        # 调整link_type: Ramp ==> SAPA, Ramp ==> JCT
        if self.adjust_link:
            self.adjust_link.Make()

    def _make_hwy_route(self):
        '''高速路线'''
        if self.hwy_route:
            self.hwy_route.Make()

    def _make_hwy_facil(self):
        # ########################################
        # temp
        if self.hwy_facil:
            self.data_mng.load_hwy_ic_link()
            self.data_mng.load_exit_name()
            self.data_mng.load_junction_name()
            self.data_mng.load_tollgate()
            self.data_mng.load_hwy_inout_link()
            # ########################################
            self.hwy_facil.Make()

    def _make_ic_no(self):
        if not self.data_mng:
            return False
        self.log.info('Start Make IC NO.')
        self.CreateTable2('mid_hwy_ic_no')
        self.pg.connect1()
        curr_idx = 0
        ic_no = 1
        data = self.data_mng.get_road_code_path()
        for hwy in data:
            road_code = hwy[0]
            updown = HWY_UPDOWN_TYPE_UP
            node_list = hwy[1]
            cycle_flag = is_cycle_path(node_list)
            if cycle_flag:
                node_list.pop()
            ic_list = self._get_ic_list(road_code, updown,
                                        node_list, cycle_flag)
            curr_idx = 0
            while curr_idx < len(ic_list):
                curr_facil = ic_list[curr_idx]
                next_idx = curr_idx + 1
                if next_idx == len(ic_list):
                    pass
                if curr_facil.boundary_flag and not curr_facil.road_point:
                    # 边界点
                    pass
                elif next_idx < len(ic_list):
                    while next_idx < len(ic_list):
                        next_facil = ic_list[next_idx]
                        if(curr_facil.road_code == next_facil.road_code and
                           curr_facil.road_point == next_facil.road_point and
                           curr_facil.facilcls == next_facil.facilcls):
                            if(curr_facil.inout == INOUT_TYPE_OUT and
                               next_facil.inout == INOUT_TYPE_IN):
                                next_idx += 1
                            else:
                                pass
                        break
                else:  # 最后一个设施
                    pass
                self._insert_ic_no(ic_no, ic_list[curr_idx:next_idx])
                ic_no += 1
                curr_idx = next_idx
        self.pg.commit1()
#         self.CreateIndex2('mid_hwy_ic_no_road_code_road_point_idx')
#         self.CreateIndex2('mid_hwy_ic_no_node_id_road_code'
#                           '_road_point_updown_idx')
        self.log.info('End Make IC NO.')
        return True

    def _get_all_route_nodes(self):
        '''取得线路上的所有点'''
        sqlcmd = """
        SELECT road_code, array_agg(node_id) as node_lid
          FROM (
            select road_code, node_id
              from hwy_link_road_code_info
              order by road_code, gid
          ) AS a
          group by road_code;
        """
        return self.pg.get_batch_data2(sqlcmd)

    def _get_ic_list(self, road_code, updown, node_list, cycle_flag):
        G = self.data_mng.get_graph()
        ic_group = []  # 一个点上的所有设施做为一个元素
        for node_idx in range(0, len(node_list)):
            node_id = node_list[node_idx]
            if node_idx == 0 or node_idx == len(node_list) - 1:
                # 起末点，不做边界点
                boundary_flag = False
            else:
                # 判断第十层Tile边界
                boundary_flag = self.is_hwy_tile_boundary(node_id, road_code)
            if boundary_flag:
                ml_object = MultiLangNameRDF('ENG', HWY_TILE_BOUNDARY_NAME)
                bd_name = ml_object.json_format_dump()
                boundary_facil_info = HwyFacilInfo(road_code,
                                                   HWY_INVALID_FACIL_ID,
                                                   IC_TYPE_INVALID,
                                                   updown,
                                                   node_id,
                                                   INOUT_TYPE_NONE,
                                                   bd_name)
                boundary_facil_info.boundary_flag = boundary_flag
            toll_flag = G.is_tollgate(node_id)
            if G.is_hwy_node(node_id) or toll_flag:
                # get facilities of this node
                facil_infos = self.data_mng.get_hwy_facils_by_nodeid(node_id,
                                                                     road_code)
                if boundary_flag:
                    facil_infos.append(boundary_facil_info)
                # 对当前点的多个设施进行排序
                facil_infos = self._sort_facils(facil_infos)
                if facil_infos:
                    ic_group.append(facil_infos)
            elif boundary_flag:  # 边界
                ic_group.append([boundary_facil_info])
        ic_list = []
        # ## 重排头尾设施
        # 第一个设施(同个点上可能有多个)
        ic_list += self._sort_start_facils(ic_group[0], cycle_flag)
        # 最后一个设施(同个点上可能有多个)
        ic_group[-1] = self._sort_end_facils(ic_group[-1], cycle_flag)
        for ic_group_idx in range(1, len(ic_group) - 1):
            ic_list += ic_group[ic_group_idx]
        ic_list += ic_group[-1]
        return ic_list

    def _sort_facils(self, facil_infos):
        '''对同个点的设施进行排序。按设施种别排序'''
        ic_list = []
        if not facil_infos:
            return []
        # 按road_seq排序
        facil_infos = self._sort_facils_by_seq(facil_infos)
        # ## 入口
        sapa_outs = self.get_sapa_in(facil_infos)
        for sapa_in in sapa_outs:
            ic_list.append(sapa_in)
        jct_ins = self.get_jct_in(facil_infos)
        for jct_in in jct_ins:
            ic_list.append(jct_in)
        ic_ins = self.get_ic_in(facil_infos)
        for ic_in in ic_ins:
            ic_list.append(ic_in)
        # ## 料金所
        tolls = self.get_tolls(facil_infos)
        for toll in tolls:
            ic_list.append(toll)
        # 边界点
        for facil_info in facil_infos:
            if facil_info.boundary_flag:
                ic_list.append(facil_info)
        # ## 出口
        ic_outs = self.get_ic_out(facil_infos)
        for ic_out in ic_outs:
            ic_list.append(ic_out)
        jct_outs = self.get_jct_out(facil_infos)
        for jct_out in jct_outs:
            ic_list.append(jct_out)
        sapa_outs = self.get_sapa_out(facil_infos)
        for sapa_out in sapa_outs:
            ic_list.append(sapa_out)
        return ic_list

    def is_hwy_tile_boundary(self, node_id, roadcode):
        '''判断是不是第十层边界: 进入本线link的TileId与出去本线link的TielId不同。'''
        G = self.data_mng.get_graph()
        in_tile, out_tile = None, None
        in_tiles = G.get_in_tile(node_id, roadcode)
        if in_tiles:
            if len(in_tiles) > 1:
                self.log.warning('Main InTile > 1,boundary_node=%s' % node_id)
            in_tile = convert_tile_id(in_tiles[0])
        out_tiles = G.get_out_tile(node_id, roadcode)
        if out_tiles:
            if len(out_tiles) > 1:
                self.log.warning('Main OutTile > 1,boundary_node=%s' % node_id)
            out_tile = convert_tile_id(out_tiles[0])
        if not in_tile or not out_tile:
            self.log.warning('No InTile or No OutTile. boundary_node=%s'
                             % node_id)
            return False
        return in_tile != out_tile

    def _make_facility_id(self):
        '''生成1到N的设施番号，边界点没有设施号'''
        self.CreateTable2('mid_hwy_facility_id')
        sqlcmd = """
        INSERT INTO mid_hwy_facility_id(road_code, road_seq)
        (
        SELECT distinct road_code, road_seq
          from hwy_node
          order by road_code, road_seq
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
        self.pg.execute2(sqlcmd, (HWY_INVALID_FACIL_ID, HWY_INVALID_FACIL_ID))
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

    def _make_road_no(self):
        '''道路番号(上下共用)'''
        self.log.info('Start Make Road No.')
        self.pg.connect2()
        if not self.pg.IsExistTable('hwy_link_road_code_info'):
            self.log.error('No table hwy_link_road_code_info.')
            return 0
        self.CreateTable2('mid_hwy_road_no')
        # 当一条道路有多种road_type时，取link数据最多的
        sqlcmd = """
        INSERT INTO mid_hwy_road_no(road_code, road_type)
        (
        SELECT road_code,  (array_agg(road_type))[1] as road_type
          FROM (
            SELECT road_code, road_type, count(road_type) as link_num
              FROM hwy_link_road_code_info as a
              LEFT JOIN link_tbl as b
              on a.link_id = b.link_id
              where a.link_id is not null
              group by road_code, road_type
              order by road_code, link_num DESC
          ) AS b
          group by road_code
          order by road_code
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.log.info('End Make Road No.')

    def _make_road_info(self):
        '''道路情报'''
        self.log.info('Start Make Road Info.')
        if not self.pg.IsExistTable('mid_hwy_road_no'):
            self.log.error('No table mid_hwy_road_no.')
            return 0
        if not self.pg.IsExistTable('hwy_link_road_code_info'):
            self.log.error('No table hwy_link_road_code_info.')
            return 0
        self.CreateTable2('highway_road_info')
        self.pg.connect1()
        sqlcmd = """
        INSERT INTO highway_road_info(
                    road_no, iddn_road_kind, road_kind,
                    road_attr, loop, "new",
                    un_open, name, up_down,
                    road_number)
            VALUES (%s, %s, %s,
                    %s, %s, %s,
                    %s, %s, %s,
                    %s);
        """
        data = self.__get_road_info()
        for info in data:
            road_no, road_kind, path, road_name, road_number = info[0:5]
            updown, road_attr = HWY_UPDOWN_TYPE_UP, HWY_UPDOWN_TYPE_UP  # 上行
            iddn_road_kind = 0  # 高速
            if is_cycle_path(path):
                loop = HWY_TRUE
            else:
                loop = HWY_FALSE
            new, un_open = HWY_FALSE, HWY_FALSE
            self.pg.execute1(sqlcmd, (road_no, iddn_road_kind, road_kind,
                                      road_attr, loop, new,
                                      un_open, road_name, updown,
                                      road_number))
        self.pg.commit1()
        self.log.info('Start End Road Info.')

    def __get_road_info(self):
        sqlcmd = """
        SELECT road_no, road_type, path, road_name, road_number
          FROM  mid_hwy_road_no AS a
          LEFT JOIN (
            SELECT road_code, ARRAY_AGG(node_id) AS path
              FROM (
                SELECT road_code, node_id, seq_nm
                  FROM hwy_link_road_code_info
                  ORDER BY road_code, seq_nm
             ) AS A
             GROUP BY road_code
          ) AS b
          ON a.road_code = b.road_code
          LEFT JOIN road_code_info as c
          ON a.road_code = c.road_code
        """
        return self.get_batch_data(sqlcmd)

    def _make_ic_info(self):
        self.log.info('Making IC Info.')
        self.pg.connect1()
        self.pg.connect2()
        self.CreateTable2('highway_ic_info')
        self.CreateTable2('highway_path_point')
        self.CreateTable2('highway_conn_info')
        self.CreateTable2('highway_toll_info')
        # self.CreateTable2('mid_hwy_node_add_info')
        for ic_no, facility_id, facil_list in self.data_mng.get_ic_list():
            if facility_id != HWY_INVALID_FACIL_ID:  # 非边界点
                ic_info = HwyICInfoRDF(ic_no, facility_id,
                                       facil_list, self.data_mng)
                ic_info.set_ic_info()  # 设置料金情报
                self._insert_ic_info(ic_info)
                self._insert_path_point(ic_info)
                self._insert_conn_info(ic_info)
                # self.__insert_toll_info(ic_info)
                # self.__insert_temp_add_info(ic_info)
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
        # self._add_geom_column()

# ==========================================================================
# 插入
# ==========================================================================
    def _insert_ic_no(self, ic_no, facil_info_list):
        sqlcmd = """
        INSERT INTO mid_hwy_ic_no(
                    ic_no, node_id,
                    road_code, road_seq,
                    updown, facilclass_c,
                    inout_c, facil_name)
            VALUES (%s, %s,
                    %s, %s,
                    %s, %s,
                    %s, %s);
        """
        for facil in facil_info_list:
            self.pg.execute1(sqlcmd, (ic_no, facil.node_id,
                                      facil.road_code, facil.road_point,
                                      facil.updown, facil.facilcls,
                                      facil.inout, facil.facil_name))

    def _insert_ic_info(self, ic):
        sqlcmd = """
        INSERT INTO highway_ic_info(
                    ic_no, up_down, facility_id,
                    between_distance, inside_distance, enter,
                    exit, tollgate, jct,
                    pa, sa, ic,
                    ramp, smart_ic, barrier,
                    dummy, boundary, "new",
                    unopen, forward_flag, reverse_flag,
                    toll_count, enter_direction, path_count,
                    vics_count, conn_count, illust_count,
                    store_count, servise_flag, road_no,
                    "name", conn_tile_id, tile_id,
                    node_id)
            VALUES (%s, %s, %s,
                    %s, %s, %s,
                    %s, %s, %s,
                    %s, %s, %s,
                    %s, %s, %s,
                    %s, %s, %s,
                    %s, %s, %s,
                    %s, %s, %s,
                    %s, %s, %s,
                    %s, %s, %s,
                    %s, %s, %s,
                    %s);
        """
        params = (ic.ic_no, ic.updown, ic.facility_id,
                  ic.between_distance, ic.inside_distance, ic.enter,
                  ic.exit, ic.tollgate, ic.jct,
                  ic.pa, ic.sa, ic.ic,
                  ic.ramp, ic.smart_ic, ic.barrier,
                  ic.dummy, ic.boundary, ic.new,
                  ic.unopen, ic.forward_flag, ic.reverse_flag,
                  ic.toll_count, ic.enter_direction, ic.path_count,
                  ic.vics_count, ic.conn_count, ic.illust_count,
                  ic.store_count, ic.servise_flag, ic.road_no,
                  ic.name, ic.conn_tile_id, ic.tile_id,
                  ic.facil_list[0].node_id)
        self.pg.execute1(sqlcmd, params)

    def _insert_path_point(self, ic):
        sqlcmd = """
        INSERT INTO highway_path_point(
                    ic_no, enter_flag,
                    exit_flag, main_flag,
                    center_flag, new_flag,
                    unopen_flag, pos_flag,
                    link_id, node_id,
                    tile_id)
            VALUES (%s, %s,
                    %s, %s,
                    %s, %s,
                    %s, %s,
                    %s, %s,
                    %s);
        """
        for point in ic.path_points:
            self.pg.execute1(sqlcmd, (point.ic_no, point.enter_flag,
                                      point.exit_flag, point.main_flag,
                                      point.center_flag, point.new_flag,
                                      point.unopen_flag, point.pos_flag,
                                      point.link_id, point.node_id,
                                      point.tile_id))
        # self.pg.commit1()

    def _insert_conn_info(self, ic_info):
        sqlcmd = """
        INSERT INTO highway_conn_info(
                    ic_no, road_attr,
                    conn_direction, same_road_flag,
                    tile_change_flag, uturn_flag,
                    new_flag, unopen_flag,
                    vics_flag, toll_flag,
                    conn_road_no, conn_ic_no,
                    conn_link_length, conn_tile_id,
                    toll_count, vics_count,
                    tile_id, ic_name,
                    conn_ic_name)
            VALUES (%s, %s, %s, %s,
                    %s, %s, %s, %s,
                    %s, %s, %s, %s,
                    %s, %s, %s, %s,
                    %s, %s, %s);
        """
        for conn in ic_info.get_conn_info():
            self.pg.execute1(sqlcmd, (conn.ic_no, conn.road_attr,
                                      conn.conn_direction, conn.same_road_flag,
                                      conn.tile_change_flag, conn.uturn_flag,
                                      conn.new_flag, conn.unopen_flag,
                                      conn.vics_flag, conn.toll_flag,
                                      conn.conn_road_no, conn.conn_ic_no,
                                      conn.conn_length, conn.conn_tile_id,
                                      conn.toll_count, conn.vics_count,
                                      conn.tile_id, conn.ic_name,
                                      conn.conn_ic_name)
                             )
        # self.pg.commit1()
