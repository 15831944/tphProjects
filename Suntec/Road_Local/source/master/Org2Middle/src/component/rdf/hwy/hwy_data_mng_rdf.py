# -*- coding: UTF-8 -*-
'''
Created on 2014-12-30

@author: hcz
'''
import component.component_base
import json
from common.database import BATCH_SIZE
from component.jdb.hwy.hwy_def import HWY_TILE_LAYER
from component.jdb.hwy.hwy_def import TILE_LAYER_14
from component.jdb.hwy.hwy_graph import ONE_WAY_POSITIVE
from component.jdb.hwy.hwy_graph import ONE_WAY_RERVERSE
from component.rdf.hwy.hwy_graph_rdf import HWY_EXIT_POI_NAME
from component.rdf.hwy.hwy_graph_rdf import HWY_EXIT_NAME
from component.rdf.hwy.hwy_graph_rdf import HWY_JUNCTION_NAME
from component.rdf.hwy.hwy_graph_rdf import HWY_TOLL_FLAG
from component.rdf.hwy.hwy_graph_rdf import HWY_NODE_NAME
from component.rdf.hwy.hwy_graph_rdf import HWY_LINK_TYPE
from component.rdf.hwy.hwy_graph_rdf import HWY_ROAD_TYPE
from component.rdf.hwy.hwy_graph_rdf import HWY_ROAD_CODE
from component.rdf.hwy.hwy_graph_rdf import HWY_UPDOWN_CODE
from component.rdf.hwy.hwy_graph_rdf import HWY_PATH_ID
from component.rdf.hwy.hwy_graph_rdf import HWY_LINK_LENGTH
from component.rdf.hwy.hwy_graph_rdf import HWY_FACIL_CLASS
from component.rdf.hwy.hwy_graph_rdf import HWY_DISP_CLASS
from component.rdf.hwy.hwy_graph_rdf import HWY_ORG_FACIL_ID
from component.rdf.hwy.hwy_graph_rdf import HWY_ROAD_NUMS
from component.rdf.hwy.hwy_graph_rdf import HWY_ROAD_NAMES
from component.rdf.hwy.hwy_graph_rdf import HWY_TILE_ID
from component.rdf.hwy.hwy_graph_rdf import HWY_S_NODE
from component.rdf.hwy.hwy_graph_rdf import HWY_E_NODE
from component.rdf.hwy.hwy_graph_rdf import HwyGraphRDF
from component.rdf.hwy.hwy_graph_rdf import HWY_REGULATION
from component.rdf.hwy.hwy_path_graph_rdf import HwyPathGraphRDF
from component.jdb.hwy.hwy_data_mng import HwyFacilInfo
from component.jdb.hwy.hwy_data_mng import TollFacilInfo
from component.rdf.hwy.hwy_def_rdf import HWY_INVALID_FACIL_ID_17CY, HWY_FALSE
from component.jdb.hwy.hwy_node_addinfo import HwyTollType
from component.rdf.hwy.hwy_node_addinfo_rdf import AddInfoDataRDF
NODE_TOLL_FLAG = 1  # 收费站


def get_road_name(json_name):
    road_names = []
    if not json_name:
        return road_names
    for names in json.loads(json_name):
        for name_dict in names:
            if name_dict.get("tts_type") == "not_tts":
                road_names.append(name_dict)
                break  # 翻译不要
    return road_names


def get_first_road_name(json_name):
    first_road_name = []
    if not json_name:
        return first_road_name
    for names in json.loads(json_name)[0:1]:
        for name_dict in names:
            if(name_dict.get("tts_type") == "not_tts"):
                first_road_name.append(name_dict)
    if first_road_name:
        return [first_road_name]
    else:
        return None


def get_road_number(json_number):
    road_numbers = []
    if not json_number:
        return road_numbers.append('')
    for numbers in json.loads(json_number.replace('\t', '\\t')):
        for num_dict in numbers:
            if num_dict.get("tts_type") == "not_tts":
                road_numbers.append(num_dict)
                break  # 翻译不要
    return road_numbers


class HwyDataMngRDF(component.component_base.comp_base):
    '''
    Highway Data Manager(RDF)
    '''
    _instance = None

    @staticmethod
    def instance():
        if HwyDataMngRDF._instance is None:
            HwyDataMngRDF._instance = HwyDataMngRDF()
        return HwyDataMngRDF._instance

    def __init__(self, ItemName='HwyDataMngRDF'):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, ItemName)
        self._road_no_dict = {}
        self.__main_path_dict = {}
        self.__boundary_node_dict = {}
        self.__road_start_end_node = {}
        self.__rest_area_dict = {}
        self.__ic_list = []
        self._graph = None
        self._path_graph = None
        self.__link_fwd_bwd_dict = {}  # link的前方设施/后方设施
        self.__toll_facil_dict = {}
        self._sapa_postion_dict = {}
        self._add_link_dict = {}
        self._side_road_dict = {}

    def initialize(self):
        self._graph = HwyGraphRDF()  # 高速link图
        self._path_graph = HwyPathGraphRDF(self._graph)  # 高速路径图

    def get_graph(self):
        return self._graph

    def get_path_graph(self):
        return self._path_graph

    def del_graph(self):
        del self._graph
        self._graph = None

    def del_path_graph(self):
        del self._path_graph
        self._path_graph = None

    def _Do(self):
        # 高速本线
        self._make_main_links()
        # Ramp/SAPA Link
        self._make_ic_links()
        # 和高速相连的普通道路。
        self._make_hwy_inout_links()
        # 高速相关所有link
        self._make_hwy_relation_link()
        # 高速相关规制
        self._make_hwy_relation_regulation()
        return 0

    def _make_main_links(self):
        self.log.info('Make main links.')
        self.pg.CreateTable2_ByName('mid_temp_hwy_main_link')
        sqlcmd = """
        INSERT INTO mid_temp_hwy_main_link(
                    link_id, s_node, e_node, one_way_code,
                    link_type, road_type, display_class, toll,
                    fazm, tazm, tile_id, length,
                    road_name, road_number, the_geom)
        (
        SELECT link_id, s_node, e_node, one_way_code,
               link_type, road_type, display_class, toll,
               fazm, tazm, tile_id, length,
               road_name, road_number, the_geom
          FROM link_tbl
          WHERE road_type in (0) and link_type in (1, 2, 4)
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('mid_temp_hwy_main_link_the_geom_idx')

    def _make_ic_links(self):
        self.log.info('Make ICs links.')
        self.pg.CreateTable2_ByName('mid_temp_hwy_ic_link')
        sqlcmd = """
        INSERT INTO mid_temp_hwy_ic_link(
                    link_id, s_node, e_node, one_way_code,
                    link_type, road_type, display_class, toll,
                    fazm, tazm, tile_id, length,
                    road_name, road_number, the_geom)
        (
        SELECT link_id, s_node, e_node, one_way_code,
               link_type, road_type, display_class, toll,
               fazm, tazm, tile_id, length,
               road_name, road_number, the_geom
          FROM link_tbl
          where (road_type = 0 and link_type not in (1, 2)) -- not main link
                or link_type = 7
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('mid_temp_hwy_ic_link_the_geom_idx')

    def _make_hwy_inout_links(self):
        '''和高速相连的普通道路。'''
        self.pg.CreateTable2_ByName('mid_temp_hwy_inout_link')
        # 和Ramp、SAPA相连的普通道路
        sqlcmd = """
        INSERT INTO mid_temp_hwy_inout_link(
                     link_id, s_node, e_node, one_way_code,
                     link_type, road_type, display_class, toll,
                     fazm, tazm, tile_id, length,
                     road_name, road_number, the_geom)
        (
        SELECT distinct b.link_id, b.s_node, b.e_node, b.one_way_code,
               b.link_type, b.road_type, b.display_class, b.toll,
               b.fazm, b.tazm, b.tile_id, b.length,
               b.road_name, b.road_number,
               b.the_geom
          FROM mid_temp_hwy_ic_link as a
          INNER JOIN link_tbl as b
          ON a.s_node = b.s_node or a.s_node = b.e_node or
             a.e_node = b.s_node or a.e_node = b.e_node
          where b.road_type not in (0, 8, 9, 12) and
                b.link_type not in (3, 5, 7)
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        # 和本线相连的的一般道
        sqlcmd = """
        INSERT INTO mid_temp_hwy_inout_link(
                     link_id, s_node, e_node, one_way_code,
                     link_type, road_type, display_class, toll,
                     fazm, tazm, tile_id, length,
                     road_name, road_number, the_geom)
        (
        SELECT distinct b.link_id, b.s_node, b.e_node, b.one_way_code,
               b.link_type, b.road_type, b.display_class, b.toll,
               b.fazm, b.tazm, b.tile_id, b.length,
               b.road_name, b.road_number, b.the_geom
          FROM mid_temp_hwy_main_link as a
          INNER JOIN link_tbl as b
          ON a.s_node = b.s_node or a.s_node = b.e_node or
             a.e_node = b.s_node or a.e_node = b.e_node
          where b.road_type not in (0, 8, 9, 12) and
                b.link_type not in (3, 5, 7)
        )
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        # Inner Link相交的link
        self._make_hwy_inout_of_inner_link()
        self.CreateIndex2('mid_temp_hwy_inout_link_the_geom_idx')

    def _make_hwy_inout_of_inner_link(self):
        '''Inner Link相交的link.'''
        sqlcmd = """
        INSERT INTO mid_temp_hwy_inout_link(
             link_id, s_node, e_node, one_way_code,
             link_type, road_type, display_class, toll,
             fazm, tazm, tile_id, length,
             road_name, road_number, the_geom)
        (
        SELECT  b.link_id, b.s_node, b.e_node, b.one_way_code,
                b.link_type, b.road_type, b.display_class, b.toll,
                b.fazm, b.tazm, b.tile_id, b.length,
                b.road_name, b.road_number, b.the_geom
          FROM mid_temp_hwy_inout_link as a
          INNER JOIN link_tbl as b
          ON a.s_node = b.s_node or a.s_node = b.e_node or
             a.e_node = b.s_node or a.e_node = b.e_node
          LEFT JOIN mid_temp_hwy_inout_link as c
          ON b.link_id = c.link_id
          WHERE a.link_type in (4, 8, 9) and -- Inner Link/Left/Right
                c.link_id is null and        -- Does not included
                b.road_type not in (0, 8, 9, 12) and
                b.link_type not in (3, 5, 7)
        );
        """
        while True:
            self.pg.execute2(sqlcmd)
            self.pg.commit2()
            if not self._exist_not_include_link():
                break

    def _make_hwy_relation_link(self):
        '''Highway相关所有link'''
        self.CreateTable2('mid_temp_hwy_relation_link')
        sqlcmd = """
        INSERT INTO mid_temp_hwy_relation_link(
               link_id, s_node, e_node, one_way_code,
               link_type, road_type, display_class, toll,
               fazm, tazm, tile_id, length,
               road_name, road_number)
        (
        SELECT link_id, s_node, e_node, one_way_code,
               link_type, road_type, display_class, toll,
               fazm, tazm, tile_id, length,
               road_name, road_number
          FROM mid_temp_hwy_main_link

        UNION
        SELECT link_id, s_node, e_node, one_way_code,
               link_type, road_type, display_class, toll,
               fazm, tazm, tile_id, length,
               road_name, road_number
          FROM mid_temp_hwy_ic_link

        UNION
        SELECT link_id, s_node, e_node, one_way_code,
               link_type, road_type, display_class, toll,
               fazm, tazm, tile_id, length,
               road_name, road_number
          FROM mid_temp_hwy_inout_link
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _make_hwy_relation_regulation(self):
        '''高速相关规制'''
        self.CreateTable2('mid_temp_hwy_relation_regulation')
        sqlcmd = """
        INSERT INTO mid_temp_hwy_relation_regulation(
                              regulation_id, cond_id, link_lid, edges)
        (
        SELECT regulation_id, cond_id, link_lid, edges
          FROM (
            SELECT regulation_id, cond_id,
                   array_agg(link_id) as link_lid,
                   array_agg(edge) as edges,
                   array_agg(in_hwy_flag) as hwy_flags
              FROM (
                SELECT distinct a.regulation_id, c.link_id, seq_num, cond_id,
                       '(' || s_node::character varying ||
                       ',' || e_node::character varying || ')' as edge,
                       (case
                       when c.link_id is not null then True
                       else False end) as in_hwy_flag
                  FROM regulation_item_tbl as a
                  inner join regulation_relation_tbl as b
                  ON a.regulation_id = b.regulation_id
                  LEFT JOIN mid_temp_hwy_relation_link AS c
                  ON linkid = c.link_id
                  where condtype = 1 and linkid is not null
                  ORDER BY regulation_id, cond_id, seq_num
              ) AS d
              GROUP by regulation_id, cond_id
          ) AS e
          -- All link are in Highway(mid_temp_hwy_relation_link)
          where True = all(hwy_flags)
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _exist_not_include_link(self):
        sqlcmd = """
        SELECT count(b.link_id)
          FROM mid_temp_hwy_inout_link as a
          INNER JOIN link_tbl as b
          ON a.s_node = b.s_node or a.s_node = b.e_node or
             a.e_node = b.s_node or a.e_node = b.e_node
          LEFT JOIN mid_temp_hwy_inout_link as c
          ON b.link_id = c.link_id
          WHERE a.link_type in (4, 8, 9) and -- Inner Link/Left/Right
                c.link_id is null and        -- Does not included
                b.road_type not in (0, 1, 8, 9, 12) and
                b.link_type not in (3, 5, 7)
        """
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchone2()
        if row:
            if row[0]:
                return True
        return False

    def load_hwy_main_link(self):
        '''load Highway Main Link.'''
        self.log.info('Start Loading Highway Main Link.')
        self.pg.connect2()
        sqlcmd = """
        SELECT link_id, s_node, e_node, one_way_code,
               link_type, road_type, display_class, toll,
               fazm, tazm, tile_id, length,
               road_name, road_number
          FROM mid_temp_hwy_main_link
        """
        for link_info in self._get_link_attr(sqlcmd):
            (link_id, s_node, e_node, one_way,
             s_angle, e_angle, link_attr) = link_info
            self._graph.add_link(s_node, e_node,
                                 one_way, link_id,
                                 s_angle, e_angle,
                                 **link_attr
                                 )
        self.log.info('End Loading Highway Main Link.')

    def load_hwy_ic_link(self):
        '''load Highway Main Link.'''
        self.log.info('Start Loading Highway IC(Ramp/JCT/SAPA) Link.')
        self.pg.connect2()
        sqlcmd = """
        SELECT link_id, s_node, e_node, one_way_code,
               link_type, road_type, display_class, toll,
               fazm, tazm, tile_id, length,
               road_name, road_number
          FROM mid_temp_hwy_ic_link;
        """
        for link_info in self._get_link_attr(sqlcmd):
            (link_id, s_node, e_node, one_way,
             s_angle, e_angle, link_attr) = link_info
            self._graph.add_link(s_node, e_node,
                                 one_way, link_id,
                                 s_angle, e_angle,
                                 **link_attr
                                 )
        self.log.info('End Loading Highway IC(Ramp/JCT/SAPA) Link.')

    def load_hwy_inout_link(self):
        '''load Highway Main Link.'''
        self.log.info('Start Loading Highway In/Out Link.')
        self.pg.connect2()
        sqlcmd = """
        SELECT DISTINCT link_id, s_node, e_node, one_way_code,
               link_type, road_type, display_class, toll,
               fazm, tazm, tile_id, length,
               road_name, road_number
          FROM mid_temp_hwy_inout_link;
        """
        for link_info in self._get_link_attr(sqlcmd):
            (link_id, s_node, e_node, one_way,
             s_angle, e_angle, link_attr) = link_info
            self._graph.add_link(s_node, e_node,
                                 one_way, link_id,
                                 s_angle, e_angle,
                                 **link_attr
                                 )
        self.log.info('End Loading Highway In/Out Link.')

    def load_hwy_regulation(self):
        '''规制'''
        self.log.info('Load highway regulation info.')
        sqlcmd = """
        select edges
          from mid_temp_hwy_relation_regulation
          where cond_id is null;
        """
        for temp_edges in self.get_batch_data(sqlcmd):
            node_list = []
            edges = []
            # ## 这个边是按Star Node到End Node排序==>转换成按规制方向排序
            # 字符串转成整数
            for edge in temp_edges[0]:
                edges.append(eval(edge))
            # 第一条link和第二条link的交点
            inter_node = set(edges[0]).intersection(set(edges[1]))
            if edges[0][0] in inter_node:
                node_list += list(edges[0][::-1])
            elif edges[0][1] in inter_node:
                node_list += list(edges[0])
            else:
                self.log.error('Dose not Intersect. edge1=%s, edges2=%s'
                               % (edges[0], edges[1]))
                continue
            # 其他link
            for edge_cnt in range(1, len(edges)):
                if edges[edge_cnt][0] == node_list[-1]:
                    node_list.append(edges[edge_cnt][1])
                elif edges[edge_cnt][1] == node_list[-1]:
                    node_list.append(edges[edge_cnt][0])
                else:
                    self.log.error('Dose not Intersect. '
                                   'edge1=(%s, %s), edges2=%s' %
                                   (node_list[-2], node_list[-1],
                                    edges[edge_cnt]))
                    break
            # ## 规制情报存到最后一条link
            u, v = node_list[-2], node_list[-1]
            if self._graph.has_edge(u, v):
                data = self._graph.get_edge_data(u, v)
                if data:
                    regulation_list = data.get(HWY_REGULATION)
                    if regulation_list:
                        regulation_list.append(node_list)
                    else:
                        data = {HWY_REGULATION: [node_list]}
                        self._graph.add_edge(u, v, data)
                else:
                    data = {HWY_REGULATION: [node_list]}
                    self._graph.add_edge(u, v, data)
            # ## 规制情报存到第一条link(逆序)
            u, v = node_list[0], node_list[1]
            if self._graph.has_edge(u, v):
                data = self._graph.get_edge_data(u, v)
                if data:
                    regulation_list = data.get(HWY_REGULATION)
                    if regulation_list:
                        regulation_list.append(node_list[::-1])
                    else:
                        data = {HWY_REGULATION: [node_list[::-1]]}
                        self._graph.add_edge(u, v, data)
                else:
                    data = {HWY_REGULATION: [node_list[::-1]]}
                    self._graph.add_edge(u, v, data)

    def _get_link_attr(self, sqlcmd):
        for link_info in self.pg.get_batch_data2(sqlcmd, BATCH_SIZE):
            link_id = link_info[0]
            s_node = link_info[1]
            e_node = link_info[2]
            one_way = link_info[3]
            s_angle = link_info[8]
            e_angle = link_info[9]
            link_attr = {}
            link_attr[HWY_LINK_TYPE] = link_info[4]
            link_attr[HWY_ROAD_TYPE] = link_info[5]
            link_attr[HWY_DISP_CLASS] = link_info[6]
            link_attr["toll"] = link_info[7]
            # link_attr["fazm"] = link_info[8]
            # link_attr["tazm"] = link_info[9]
            link_attr[HWY_TILE_ID] = link_info[10]
            link_attr[HWY_LINK_LENGTH] = link_info[11]
            link_attr[HWY_ROAD_NAMES] = get_road_name(link_info[12])
            # link_attr[HWY_1ST_ROAD_NAME] = get_first_road_name(link_info[12])
            link_attr[HWY_ROAD_NUMS] = get_road_number(link_info[13])
            yield link_id, s_node, e_node, one_way, s_angle, e_angle, link_attr

    def load_hwy_path(self):
        self.pg.connect2()
        sqlcmd = """
        SELECT a.path_id, length, path
          FROM mid_temp_hwy_main_path_attr as a
          LEFT JOIN (
            SELECT path_id, array_agg(node_id) as path
              FROM (
                select path_id, node_id, seq_nm
                  FROM mid_temp_hwy_main_path
                  ORDER BY path_id, seq_nm
              ) as b
              group by path_id
          ) as b
          ON a.path_id = b.path_id
          where side_path_flg <> 1 and delete_flag <> 1;
        """
        self._path_graph = HwyPathGraphRDF(self._graph)
        for path_info in self.get_batch_data(sqlcmd):
            path_id, length, path = path_info
            self._path_graph.add_path(path_id, length, path)

    def load_exit_poi_name(self):
        '''加载HWY exit POI Name.'''
        if not self.pg.IsExistTable('mid_temp_hwy_exit_poi_name'):
            self.log.warning('No Table mid_temp_hwy_exit_poi_name.')
            return
        # 下面的SQL代码时临时
        sqlcmd = """
        SELECT node_id, name
          FROM mid_temp_hwy_exit_poi_name;
        """
        for exit_info in self.get_batch_data(sqlcmd):
            node_id, name = exit_info
            data = {HWY_EXIT_POI_NAME: name}
            if node_id in self._graph:
                self._graph.add_node(node_id, data)

    def load_exit_name(self):
        '''加载出口番号'''
        # 下面的SQL代码时临时
        sqlcmd = """
        SELECT s_node, e_node, one_way_code, name
          FROM mid_temp_hwy_exit_name as a
          INNER JOIN link_tbl as b
          ON a.link_id = b.link_id
          where is_exit_name = 'Y';
        """
        for exit_info in self.get_batch_data(sqlcmd):
            s_node, e_node, one_way_code, exit_name = exit_info
            data = {HWY_EXIT_NAME: exit_name}
            # data = {"exit_name": exit_name}
            if one_way_code == ONE_WAY_POSITIVE:
                if s_node in self._graph:
                    self._graph.add_node(s_node, data)
            elif one_way_code == ONE_WAY_RERVERSE:
                if e_node in self._graph:
                    self._graph.add_node(e_node, data)

    def load_junction_name(self):
        if not self.pg.IsExistTable('mid_temp_hwy_exit_name'):
            self.log.error('No table mid_temp_hwy_exit_name')
            return 0
        '''加载出口番号'''
        # 下面的SQL代码时临时
        sqlcmd = """
        SELECT s_node, e_node, name
          FROM mid_temp_hwy_exit_name as a
          INNER JOIN link_tbl as b
          ON a.link_id = b.link_id
          where is_junction_name = 'Y';
        """
        for exit_info in self.get_batch_data(sqlcmd):
            s_node, e_node, junction_name = exit_info
            data = {HWY_JUNCTION_NAME: junction_name}
            if s_node in self._graph:
                self._graph.add_node(s_node, data)
            if e_node in self._graph:
                self._graph.add_node(e_node, data)

    def load_tollgate(self):
        '''加载收费站'''
        sqlcmd = """
        SELECT distinct node_id, toward_name
          FROM node_tbl
          LEFT JOIN towardname_tbl
          ON node_tbl.node_id = towardname_tbl.nodeid and
             guideattr = 7 and namekind = 2
          where toll_flag = 1;
        """
        for node_id, node_name in self.get_batch_data(sqlcmd):
            data = {HWY_TOLL_FLAG: NODE_TOLL_FLAG,
                    HWY_NODE_NAME: node_name}
            self._graph.add_node(node_id, data)

    def load_org_facil_id(self):
        '''加载元设施id'''
        if not self.pg.IsExistTable('mid_node_facil'):
            self.log.warning('No table mid_node_facil.')
            return 0
        sqlcmd = """
        SELECT node_id, facil_id
          FROM mid_node_facil as a
        """
        for facil_info in self.get_batch_data(sqlcmd):
            node, facil_id = facil_info
            data = {HWY_ORG_FACIL_ID: facil_id}
            if node in self._graph:
                self._graph.add_node(node, data)

    def load_hwy_path_id(self):
        sqlcmd = """
        SELECT a.path_id, path
          FROM mid_temp_hwy_main_path_attr as a
          LEFT JOIN (
            SELECT path_id, array_agg(node_id) as path
              FROM (
                select path_id, node_id, seq_nm
                  FROM mid_temp_hwy_main_path
                  ORDER BY path_id, seq_nm
              ) as b
              group by path_id
          ) as b
          ON a.path_id = b.path_id
          where delete_flag = 0;
        """
        for path_id, path in self.get_batch_data(sqlcmd):
            for u, v in zip(path[0:-1], path[1:]):
                data = {HWY_PATH_ID: path_id}
                self._graph.add_edge(u, v, data)

    def load_hwy_road_code(self):
        self.log.info('Start Loading HYY Road Code.')
        self.pg.connect2()
        if not self.pg.IsExistTable('hwy_link_road_code_info'):
            self.log.error('No table hwy_link_road_code_info.')
            return
        sqlcmd = """
        SELECT road_code, updown, array_agg(node_id) as path
          FROM (
            SELECT road_code, updown, node_id
              FROM hwy_link_road_code_info
              ORDER BY road_code, updown, seq_nm
          ) as a
          GROUP BY road_code, updown;
        """
        for road_code, updown, path in self.get_batch_data(sqlcmd):
            for u, v in zip(path[0:-1], path[1:]):
                data = {HWY_ROAD_CODE: road_code,
                        HWY_UPDOWN_CODE: updown}
                self._graph.add_edge(u, v, data)
        self.log.info('End Loading HYY Road Code.')

    def load_hwy_node_facilcls(self):
        '''加载设施种别'''
        self.log.info('Start Loading HWY Node.')
        self.pg.connect2()
        if not self.pg.IsExistTable('hwy_node'):
            self.log.error('No Table hwy_node.')
            return
        sqlcmd = """
        SELECT node_id, array_agg(facilcls_c) as facilcls_array
          FROM hwy_node
          GROUP BY node_id;
        """
        self.pg.connect2()
        for node, facilcls_list in self.get_batch_data(sqlcmd):
            attr_dict = {HWY_FACIL_CLASS: facilcls_list}
            self._graph.add_node(node, attr_dict)
        self.log.info('End Loading HWY Node.')

    def get_side_path(self):
        sqlcmd = """
        SELECT path_id, path, other_path_id, other_path
          FROM (-- Get path
            SELECT path_id, array_agg(node_id) as path
              FROM (
                select path_id, node_id, seq_nm
                  FROM mid_temp_hwy_main_path
                  ORDER BY path_id, seq_nm
              ) as b
              group by path_id
          ) as curr
          INNER JOIN (
            SELECT path_id as other_path_id, array_agg(node_id) as other_path
              FROM (
                select path_id, node_id, seq_nm
                  FROM mid_temp_hwy_main_path
                  ORDER BY path_id, seq_nm
              ) as b
              group by path_id
          ) as other
          ON curr.path_id <> other.other_path_id and
             path[1] = any(other_path) and
             path[array_upper(path, 1)] = any(other_path)
        """
        # return self.get_batch_data(sqlcmd)
        self.pg.execute2(sqlcmd)
        return self.pg.fetchall2()

    def get_path_distance(self, max_dist, min_dist=0):
        sqlcmd = """
        SELECT a.path_id, length, path, side_path_flg
          FROM mid_temp_hwy_main_path_attr as a
          LEFT JOIN (
            SELECT path_id, array_agg(node_id) as path
              FROM (
                select path_id, node_id, seq_nm
                  FROM mid_temp_hwy_main_path
                  ORDER BY path_id, seq_nm
              ) as b
              group by path_id
          ) as b
          ON a.path_id = b.path_id
          where length >= %s and length <= %s
                and delete_flag = 0;
        """
        return self.get_batch_data(sqlcmd, (min_dist, max_dist))

    def get_path(self):
        sqlcmd = """
        SELECT a.path_id, path, side_path_flg
          FROM mid_temp_hwy_main_path_attr as a
          LEFT JOIN (
            SELECT path_id, array_agg(node_id) as path
              FROM (
                select path_id, node_id, seq_nm
                  FROM mid_temp_hwy_main_path
                  ORDER BY path_id, seq_nm
              ) as b
              group by path_id
          ) as b
          ON a.path_id = b.path_id
          where delete_flag = 0  -- not delete path
          ORDER BY a.path_id;
        """
        return self.get_batch_data(sqlcmd)

    def get_road_code_path(self):
        sqlcmd = """
        SELECT road_code, updown, array_agg(node_id) as path
          FROM (
            SELECT road_code, updown, node_id
              FROM hwy_link_road_code_info
              ORDER BY road_code, updown, seq_nm
          ) as a
          GROUP BY road_code, updown
          ORDER BY road_code, updown;
        """
        return self.get_batch_data(sqlcmd)

    def get_max_path_id(self):
        sqlcmd = """
        SELECT max(path_id)
          from mid_temp_hwy_main_path_attr;
        """
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchone2()
        if row and row[0]:
            return row[0]
        return 0

    def get_cycle_route_start_end(self):
        CYCLE_ROUTE_START_END = [(652209367, 652209471),  # 中东
                                 (652209549, 652209497),
                                 (866099728, 866094774),  # 阿根廷
                                 (866107699, 866108659),
                                 ]
        return CYCLE_ROUTE_START_END

    def get_hwy_facils_by_nodeid(self, node_id, t_roadcode=None,
                                 t_facilcls=None):
        '''[HwyFacility]'''
        facils = []
        sqlcmd = """
        SELECT road_code, road_seq, updown_c,
               inout_c, facilcls_c, facil_name
          FROM hwy_node
          where node_id = %s
          order by road_code, road_seq
        """
        self.pg.execute2(sqlcmd, (node_id,))
        data = self.pg.fetchall2()
        for facil_info in data:
            roadcode = facil_info[0]
            roadpoint = facil_info[1]
            updown = facil_info[2]
            inout = facil_info[3]
            facilcls = facil_info[4]
            facil_name = facil_info[5]
            if t_roadcode and t_roadcode != roadcode:
                continue
            if t_facilcls and t_facilcls != facilcls:
                continue
            facil_info = HwyFacilInfo(roadcode, roadpoint, facilcls,
                                      updown, node_id, inout,
                                      facil_name)
            facils.append(facil_info)
        return facils

    def get_main_path(self, road_code, updown):
        key = (road_code, updown)
        if not self.__main_path_dict:
            self.__load_main_path()
        if self.__main_path_dict:
            return self.__main_path_dict.get(key)
        return None

    def get_ic_no(self, facil):
        sqlcmd = """
        SELECT ic_no
          FROM mid_hwy_ic_no
          where node_id = %s and road_code = %s
                and road_seq = %s and updown = %s;
        """
        self.pg.execute2(sqlcmd, (facil.node_id, facil.road_code,
                                  facil.road_point, facil.updown))
        row = self.pg.fetchone2()
        if row:
            return row[0]
        return None

    def is_road_start_node(self, road_code, updown, node_id):
        '''道路的起始点'''
        if not node_id:
            return False
        self.__load_road_start_end_node()
        key = (road_code, updown)
        s_e_nodes = self.__road_start_end_node.get(key)
        if s_e_nodes and node_id == s_e_nodes[0]:
            return True
        return False

    def __load_road_start_end_node(self):
        if self.__road_start_end_node:
            return
        self.log.info('Start Loading Road Start/End Node.')
        sqlcmd = """
        SELECT road_code, updown, array_agg(node_id) as node_lid
          FROM (
            SELECT road_code, updown, node_id
              FROM hwy_link_road_code_info
              ORDER BY road_code, updown, seq_nm
          ) AS a
          GROUP BY road_code, updown
        """
        self.pg.execute2(sqlcmd)
        for row in self.pg.fetchall2():
            road_code, updown, node_lid = row[0:3]
            start_node = node_lid[0]
            end_node = node_lid[-1]
            key = (road_code, updown)
            self.__road_start_end_node[key] = (start_node, end_node)
        self.log.info('End Loading Road Start/End Node.')

    def is_road_end_node(self, road_code, updown, node_id):
        '''道路的起始点'''
        if not node_id:
            return False
        self.__load_road_start_end_node()
        key = (road_code, updown)
        s_e_nodes = self.__road_start_end_node.get(key)
        if s_e_nodes and node_id == s_e_nodes[-1]:
            return True
        return False

    def __load_main_path(self):
        sqlcmd = """
        SELECT road_code, updown, array_agg(node_id)
          FROM (
            SELECT road_code, updown, node_id
              FROM hwy_link_road_code_info
              ORDER BY road_code, updown, seq_nm
          ) AS a
          GROUP BY road_code, updown;
        """
        self.pg.execute2(sqlcmd)
        data = self.pg.fetchall2()
        for main_path in data:
            road_code = main_path[0]
            updown = main_path[1]
            path = main_path[2]
            key = (road_code, updown)
            self.__main_path_dict[key] = path

    def get_road_no(self, road_code):
        if not self._road_no_dict:
            self._load_road_no()
        return self._road_no_dict.get(road_code)

    def _load_road_no(self):
        sqlcmd = """
        SELECT road_no, road_code
          FROM mid_hwy_road_no;
        """
        self.pg.execute2(sqlcmd)
        for row in self.pg.fetchall2():
            road_no = row[0]
            road_code = row[1]
            self._road_no_dict[road_code] = road_no

    def get_ic_nos_by_node(self, node, road_code):
        sqlcmd = """
        SELECT array_agg(ic_no) as ic_nos
          FROM mid_hwy_ic_no
          where node_id = %s and road_code = %s
          GROUP BY node_id
        """
        self.pg.execute2(sqlcmd, (node, road_code))
        row = self.pg.fetchone2()
        if row:
            return row[0]
        return []

    def get_ic_path_info(self, facil):
        path_infos = []
        sqlcmd = """
        SELECT node_lid, link_lid, path_type
          FROM mid_temp_hwy_ic_path
          where road_code =%s and road_seq = %s
                and node_id = %s and inout_c = %s
                and facilcls_c <> 10     -- Not U-turn
          order by node_lid, link_lid
        """
        self.pg.execute2(sqlcmd, (facil.road_code, facil.road_point,
                                  facil.node_id, facil.inout)
                         )
        data = self.pg.fetchall2()
        for row in data:
            node_lid, link_lid = tuple(), tuple()
            if row[0]:
                node_lid = eval(row[0] + ',')
            if row[1]:
                link_lid = eval(row[1] + ',')
            path_type = row[2]
            path_infos.append((node_lid, link_lid, path_type))
        return path_infos

    def is_same_facil(self, s_facil, t_facil):
        '''判断两个设施是否并列设施'''
        if not s_facil or not t_facil:
            return False
        same_facils = self.get_same_facil(s_facil.road_code,
                                          s_facil.road_point,
                                          s_facil.updown,
                                          s_facil.node_id)
        for same in same_facils:
            if(same.road_code == t_facil.road_code and
               same.road_point == t_facil.road_point and
               same.node_id == t_facil.node_id):
                return True
        return False

    def get_same_facil(self, roadcode, roadpoint, updown, node_id):
        sqlcmd = """
        SELECT DISTINCT road_code, road_seq, updown_c, node_id
          FROM hwy_same_info
          where road_code = %s and p_road_seq = %s
                and updown_c = %s and p_node_id = %s
        """
        same_facils = []
        self.pg.execute2(sqlcmd, (roadcode, roadpoint, updown, node_id))
        for facil_info in self.pg.fetchall2():
            roadcode = facil_info[0]
            roadpoint = facil_info[1]
            updown = facil_info[2]
            node_id = facil_info[3]
            facilcls = None
            inout = None
            facil = HwyFacilInfo(roadcode, roadpoint, facilcls,
                                 updown, node_id, inout,
                                 '')
            same_facils.append(facil)
        if same_facils:
            return same_facils
        # ## 父设施的并列设施
        ###################################################################
        parents = self.get_parent_facil(roadcode, roadpoint, updown, node_id)
        if not parents:
            return []
        if len(parents) > 1:
            self.log.warning('Number of parent facility > 1. '
                             'roadcode=%s, roadseq=%s, node_id=%s'
                             % (roadcode, roadpoint, node_id))
        same_facils = list(parents)
        p = parents[0]
        p_same_facils = self.get_same_facil(p.road_code,
                                            p.road_point,
                                            p.updown,
                                            p.node_id)
        for same_facil in p_same_facils:
            if(same_facil.road_code == roadcode and
               same_facil.road_point == roadpoint and
               same_facil.node_id == node_id):
                pass
            else:
                same_facils.append(same_facil)
        return same_facils

    def get_parent_facil(self, roadcode, roadpoint, updown, node_id):
        sqlcmd = """
        SELECT DISTINCT road_code, p_road_seq, updown_c, p_node_id
          FROM hwy_same_info
          where road_code = %s and road_seq = %s
            and updown_c = %s and node_id = %s
        """
        self.pg.execute2(sqlcmd, (roadcode, roadpoint, updown, node_id))
        parent_facils = []
        for facil_info in self.pg.fetchall2():
            p_roadcode = facil_info[0]
            p_roadpoint = facil_info[1]
            p_updown = facil_info[2]
            p_node_id = facil_info[3]
            facilcls = None
            inout = None
            facil = HwyFacilInfo(p_roadcode, p_roadpoint, facilcls,
                                 p_updown, p_node_id, inout,
                                 '')
            parent_facils.append(facil)
        return parent_facils

    def is_boundary_node(self, node_id):
        self.__load_boundary_node()
        if not self.__boundary_node_dict:
            if self._exist_boundary():
                self.log.error('No Tile Node.')
        if node_id in self.__boundary_node_dict:
            return True
        return False

    def __load_boundary_node(self):
        if not self.__boundary_node_dict:
            sqlcmd = """
            SELECT distinct node_id
              FROM mid_hwy_ic_no
              WHERE facility_id = %s
              ORDER BY node_id
            """
            self.pg.execute2(sqlcmd, (HWY_INVALID_FACIL_ID_17CY, ))
            for row in self.pg.fetchall2():
                node_id = row[0]
                self.__boundary_node_dict[node_id] = None

    def _exist_boundary(self):
        '''判定所有线路没有跨tile.'''
        sqlcmd = """
        SELECT count(*)
          FROM (
            SELECT road_code, updown
              from (
                SELECT distinct road_code, updown,
                       ((tile_id >> %s) >> %s) & %s as x,
                       (tile_id >> %s) & %s as y
                  FROM hwy_link_road_code_info as a
                  LEFT JOIN link_tbl as b
                  ON a.link_id = b.link_id
                  where a.link_id is not null
            ) as c
            GROUP BY road_code, updown
            having count(*) > 1
         ) AS d;
        """
        SHIFT = TILE_LAYER_14 - HWY_TILE_LAYER
        MASK = (1 << HWY_TILE_LAYER) - 1
        self.pg.execute2(sqlcmd, (TILE_LAYER_14, SHIFT, MASK,
                                  SHIFT, MASK))
        row = self.pg.fetchone2()
        if row and row[0] == 0:
            return False
        return True

    def get_ic_list(self):
        if not self.__ic_list:
            self._load_ic()
        return self.__ic_list

    def get_ic(self, ic_no):
        if not self.__ic_list:
            self._load_ic()
        if self.__ic_list and ic_no > 0 and ic_no <= len(self.__ic_list):
            return self.__ic_list[ic_no - 1]
        else:
            return None, None, None

    def _load_ic(self):
        sqlcmd = """
        SELECT ic_no, facility_id, road_code,
               road_seq, updown, facilclass_c,
               facil_name,
               array_agg(inout_c) as inout_cs,
               array_agg(node_id) as node_ids
          FROM (
            SELECT ic_no, facility_id, road_code, road_seq,
                   updown, facilclass_c, facil_name, inout_c,
                   node_id
              FROM mid_hwy_ic_no
              ORDER BY ic_no, gid
          ) as a
          GROUP BY ic_no, road_code, road_seq, updown,
                   facilclass_c, facil_name, facility_id
          ORDER BY ic_no
        """
        self.pg.execute2(sqlcmd)
        data = self.pg.fetchall2()
        for ic in data:
            ic_no, facility_id, roadcode, roadpoint = ic[0:4]
            updown, facilcls, facil_name = ic[4:7]
            inouts, nodeids = ic[7:9]
            facil_list = []
            for nodeid, inout in zip(nodeids, inouts):
                # get facil info
                facil = HwyFacilInfo(roadcode, roadpoint, facilcls,
                                     updown, nodeid, inout,
                                     facil_name, '', facility_id)
                facil_list.append(facil)
            self.__ic_list.append((ic_no, facility_id, facil_list))

    def get_sapa_name(self, road_code, road_seq):
        sqlcmd = """
        SELECT sapa_name
          FROM mid_temp_hwy_sapa_info
          where road_code = %s and road_seq = %s
        """
        self.pg.execute2(sqlcmd, (road_code, road_seq))
        row = self.pg.fetchone2()
        if row:
            return row[0]
        return None

    def get_main_link_fb_facil(self, link_id, reverse=False):
        '''本线link的前后设施'''
        sqlcmd = """
        SELECT forward_ic_no, backward_ic_no
          FROM highway_mapping
          where link_id = %s;
        """
        self.pg.execute2(sqlcmd, (link_id,))
        row = self.pg.fetchone2()
        if row:
            forward_ic_no, backward_ic_no = row[0:2]
            if reverse:  # 后方
                return backward_ic_no
            else:
                return forward_ic_no
        return None

    def get_path_id(self, road_code):
        sqlcmd = """
        select path_id
          from road_code_info
          where road_code = %s;
        """
        self.pg.execute1(sqlcmd, (road_code,))
        row = self.pg.fetchone()
        return row[0]

    def get_line_name(self, road_code):
        sqlcmd = """
        select road_name, road_number
          from road_code_info
          where road_code = %s
        """
        self.pg.execute1(sqlcmd, (road_code,))
        row = self.pg.fetchone()
        road_name, road_number = row[0:2]
        if road_name:
            return road_name
        elif road_number:
            return road_number
        else:
            return None

    def get_path_by_pathid(self, path_id, updown):
        sqlcmd = """
        SELECT array_agg(node_id) as path
          FROM (
            SELECT path_id, node_id, seq_nm
              FROM mid_temp_hwy_main_path
              WHERE path_id = %s and updown = %s
              ORDER BY seq_nm
          ) AS a
          group by path_id;
        """
        self.pg.execute1(sqlcmd, (path_id, updown))
        row = self.pg.fetchone()
        return row[0]

    def is_sapa_pos_node(self, nodeid):
        '''判断nodeid是不是SAPA设施所在点'''
        return False
        if not self.__sapa_dict:
            self.__load_sapa_postion()
        if nodeid in self.__sapa_dict:
            return True
        else:
            return False

    def is_in_hwy_mapping(self, link_id):
        if not self.__link_fwd_bwd_dict:
            self._load_link_fwd_bwd_facil()
        if link_id in self.__link_fwd_bwd_dict:
            return True
        return False

    def get_link_fwd_bwd_facil(self, link_id):
        '''取得link的前后设施情报。'''
        if not self.__link_fwd_bwd_dict:
            self._load_link_fwd_bwd_facil()
        bwd_fwd_facil = self.__link_fwd_bwd_dict.get(link_id)
        if bwd_fwd_facil:
            bwd_list = bwd_fwd_facil[0]
            fwd_list = bwd_fwd_facil[1]
            return bwd_list, fwd_list
        else:
            return [], []

    def _load_link_fwd_bwd_facil(self):
        sqlcmd = """
        select a.link_id,
               array_agg(bwd_node_id) as bwd_node_ids,
               array_agg(bwd_ic_no) as bwd_ic_nos,
               array_agg(bwd_facility_id) as bwd_facility_ids,
               array_agg(fwd_node_id) as fwd_node_ids,
               array_agg(fwd_ic_no) as fwd_ic_nos,
               array_agg(bwd_facility_id) as fwd_facility_ids,
               array_agg(path_type) as path_types,
               link_type
          from mid_temp_hwy_ic_link_mapping as a
          LEFT JOIN link_tbl
          ON a.link_id = link_tbl.link_id
          group by a.link_id, link_type
        """
        self.pg.execute2(sqlcmd)
        data = self.pg.fetchall2()
        for info in data:
            link_id = info[0]
            bwd_node_ids, bwd_ic_nos, bwd_facility_ids = info[1:4]
            fwd_node_ids, fwd_ic_nos, fwd_facility_ids = info[4:7]
            path_types, link_type = info[7:9]
            # 后方设施及料金
            bwd_list = []
            fwd_list = []
            for (bwd_node_id, bwd_ic_no, bwd_facility_id,
                 fwd_node_id, fwd_ic_no, fwd_facility_id, path_type) in \
                zip(bwd_node_ids, bwd_ic_nos, bwd_facility_ids,
                    fwd_node_ids, fwd_ic_nos, fwd_facility_ids, path_types):
                bwd = {'node_id': bwd_node_id,
                       'ic_no': bwd_ic_no,
                       'facility_id': bwd_facility_id,
                       'path_type': path_type}
                if bwd not in bwd_list:
                    bwd_list.append(bwd)
                fwd = {'node_id': fwd_node_id,
                       'ic_no': fwd_ic_no,
                       'facility_id': fwd_facility_id,
                       'path_type': path_type}
                if fwd not in fwd_list:
                    fwd_list.append(fwd)
            self.__link_fwd_bwd_dict[link_id] = [bwd_list, fwd_list, link_type]

    def load_signpost(self):
        return

    def get_toll_facil_by_link(self, link_id):
        if not self.__toll_facil_dict:
            self._load_toll_facil()
        return self.__toll_facil_dict[link_id]

    def _load_toll_facil(self):
        '''加载料金信息。'''
        self.log.info('Start Load Toll Facil Info.')
        sqlcmd = """
        SELECT link_id,
               array_agg(node_id) as node_ids,
               array_agg(road_code) as road_codes,
               array_agg(road_seq) as road_seq,
               array_agg(updown_c) as updown_cs,
               array_agg(inout_c) as inout_cs,
               array_agg(facilcls_c) as facilcls_cs,
               array_agg(facil_name) as facil_names
          FROM hwy_tollgate
          GROUP BY link_id;
        """
        self.pg.execute2(sqlcmd)
        data = self.pg.fetchall2()
        for info in data:
            link_id = info[0]
            node_ids = info[1]
            if set(node_ids) > 1:
                self.log.error('Num of Node > 1. toll_node=%s' % node_ids)
            node_id = node_ids[0]
            toll_info_list = []
            for (roadcode, roadpoint, updown,
                 inout, facilcls, toll_name) in zip(*info[2:]):
                toll_info = TollFacilInfoRDF(roadcode, roadpoint, facilcls,
                                             updown, node_id, inout,
                                             toll_name)
                toll_info_list.append(toll_info)
            self.__toll_facil_dict[link_id] = toll_info_list
        self.log.info('End Load Toll Facil Info.')

    def get_sapa_postion(self, road_code, road_seq, updown):
        if not self._sapa_postion_dict:
            self._load_sapa_poi_link()
        key = road_code, road_seq, updown
        position = self._sapa_postion_dict.get(key)
        if position:
            node_id, link_id = position
            return node_id, link_id
        else:
            return None, None

    def _load_sapa_poi_link(self):
        sqlcmd = """
        SELECT road_code, road_seq, updown_c,
               node_id, link_id
          FROM hwy_facil_position
          ORDER BY road_code, road_seq, updown_c;
        """
        for (road_code, road_seq, updown_c, node_id,
             link_id) in self.get_batch_data(sqlcmd):
            key = road_code, road_seq, updown_c
            self._sapa_postion_dict[key] = (node_id, link_id)

    def is_add_info_link(self, link_id):
        # if not self._add_link_dict:
        #    self.load_add_info_link()
        if link_id in self._add_link_dict:
            return True
        return False

    def load_add_info_link(self):
        sqlcmd = """
        select distinct add_link_id
          from mid_hwy_node_add_info;
        """
        for row in self.get_batch_data(sqlcmd):
            add_link_id = row[0]
            self._add_link_dict[add_link_id] = None

    def _get_add_info_by_link(self, link_id):
        sqlcmd = """
        SELECT link_id, node_id, toll_flag,  -- [0:3]
               no_toll_money, facility_num, up_down, -- [3:6]
               facility_id, seq_num, dir_s_node, dir_e_node,  -- [3:10]
               etc_antenna, enter, exit, jct, -- [10:14]
               sapa, gate, un_open, dummy,  -- [14:18]
               toll_type_num, non_ticket_gate, check_gate,single_gate,--[18:22]
               cal_gate, ticket_gate, nest, uturn, -- [22:26]
               not_guide, normal_toll, etc_toll, etc_section, -- [26:30]
               name, tile_id, no_toll_flag  -- [30:33]
          FROM highway_node_add_info
          where link_id = %s
        """
        self.pg.execute2(sqlcmd, (link_id,))
        add_info_list = []
        for row in self.pg.fetchall2():
            add_link_id, add_node_id, toll_flag= row[0:3]
            no_toll_money, facility_num, up_down = row[3:6]
            facility_id, seq_num, dir_s_node, dir_e_node = row[6:10]
            toll_type = HwyTollType()
            (toll_type.etc_antenna, toll_type.enter,
             toll_type.exit, toll_type.jct,
             toll_type.sa_pa, toll_type.gate,
             toll_type.unopen, toll_type.dummy_facil,
             toll_type_num, toll_type.non_ticket_gate,
             toll_type.check_gate, toll_type.single_gate,
             toll_type.cal_gate, toll_type.ticket_gate,
             toll_type.nest, toll_type.uturn,
             toll_type.not_guide, toll_type.normal_toll,
             toll_type.etc_toll, toll_type.etc_section) = row[10:30]
            facil_name, tile_id, no_toll_flag = row[30:33]
            add_info = AddInfoDataRDF(add_link_id, add_node_id,
                                      toll_flag, no_toll_money,
                                      facility_num, up_down,
                                      facility_id, facil_name,
                                      seq_num, toll_type_num,
                                      toll_type, tile_id,
                                      dir_s_node, dir_e_node,
                                      no_toll_flag)
            add_info_list.append(add_info)
        return add_info_list

    def get_side_road(self, main_roadcode, main_updown):
        '''侧道/辅路'''
        if not self._side_road_dict:
            self._load_side_road()
        key = main_roadcode, main_updown
        return self._side_road_dict.get(key)

    def _load_side_road(self):
        sqlcmd = """
        SELECT main_road_code, main_updown,
               side_road_code, side_updown, dummy_road
          FROM hwy_main_side_road_relation
          order by main_road_code, main_updown,
                   side_road_code, side_updown
        """
        for rel_info in self.get_batch_data(sqlcmd):
            main_road_code, main_updown = rel_info[0:2]
            side_road_code, side_updown, dummy_road = rel_info[2:5]
            key = (main_road_code, main_updown)
            val = (side_road_code, side_updown, dummy_road)
            if key in self._side_road_dict:
                self._side_road_dict[key].append(val)
            else:
                self._side_road_dict[key] = [val]


# =============================================================================
# TollFacilInfo(料金信息)
# =============================================================================
class TollFacilInfoRDF(TollFacilInfo):
    def __init__(self, roadcode, roadpoint, facilcls,
                 updown, nodeid, inout,
                 facil_name, link_list):
        '''
        Constructor
        '''
        TollFacilInfo.__init__(self, roadcode, roadpoint, facilcls,
                               updown, nodeid, inout,
                               facil_name, name_yomi=None, tollclass=HWY_FALSE,
                               dummytoll=HWY_FALSE, tollfunc_c=HWY_FALSE,
                               guide_f=HWY_FALSE, tollgate_lane=HWY_FALSE,
                               dummyfacil=HWY_FALSE
                               )

# 加洲
California_BOX = """ WHERE the_geom && ST_SetSRID(ST_MakeBox2D(ST_Point(-119.040,34.586),
                                                                       ST_Point(-117.029,33.026)),
                                                               4326)
                 """
