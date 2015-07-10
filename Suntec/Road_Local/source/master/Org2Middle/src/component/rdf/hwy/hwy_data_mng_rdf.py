# -*- coding: UTF-8 -*-
'''
Created on 2014-12-30

@author: hcz
'''
import component.component_base
import json
from common.database import BATCH_SIZE
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
from component.rdf.hwy.hwy_graph_rdf import HWY_PATH_ID
from component.rdf.hwy.hwy_graph_rdf import HWY_LINK_LENGTH
from component.rdf.hwy.hwy_graph_rdf import HWY_FACIL_CLASS
from component.rdf.hwy.hwy_graph_rdf import HWY_DISP_CLASS
from component.rdf.hwy.hwy_graph_rdf import HWY_ORG_FACIL_ID
from component.rdf.hwy.hwy_graph_rdf import HWY_1ST_ROAD_NAME
from component.rdf.hwy.hwy_graph_rdf import HWY_TILE_ID
from component.rdf.hwy.hwy_graph_rdf import HwyGraphRDF
from component.rdf.hwy.hwy_path_graph_rdf import HwyPathGraphRDF
from component.jdb.hwy.hwy_data_mng import HwyFacilInfo
from component.jdb.hwy.hwy_def import HWY_INVALID_FACIL_ID
NODE_TOLL_FLAG = 1  # 收费站


def get_road_name(json_name):
    road_names = []
    if not json_name:
        return road_names
    for names in json.loads(json_name):
        for name_dict in names:
            if(name_dict.get("tts_type") == "not_tts" and
               name_dict.get("type") != "route_num"):
                road_names.append(name_dict.get("val"))
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
                road_numbers.append(num_dict.get("val"))
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
        return 0

    def _make_main_links(self):
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
          WHERE road_type in (0, 1) and link_type in (1, 2, 4)
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('mid_temp_hwy_main_link_the_geom_idx')

    def _make_ic_links(self):
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
                     fazm, tazm, tile_id, road_name,
                     road_number, the_geom)
        (
        SELECT distinct b.link_id, b.s_node, b.e_node, b.one_way_code,
               b.link_type, b.road_type, b.display_class, b.toll,
               b.fazm, b.tazm, b.tile_id, b.road_name, b.road_number,
               b.the_geom
          FROM mid_temp_hwy_ic_link as a
          INNER JOIN link_tbl as b
          ON a.s_node = b.s_node or a.s_node = b.e_node or
             a.e_node = b.s_node or a.e_node = b.e_node
          where b.road_type not in (0, 1, 8, 9) and
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
                     fazm, tazm, tile_id, road_name,
                     road_number, the_geom)
        (
        SELECT distinct b.link_id, b.s_node, b.e_node, b.one_way_code,
               b.link_type, b.road_type, b.display_class, b.toll,
               b.fazm, b.tazm, b.tile_id, b.road_name, b.road_number,
               b.the_geom
          FROM mid_temp_hwy_main_link as a
          INNER JOIN link_tbl as b
          ON a.s_node = b.s_node or a.s_node = b.e_node or
             a.e_node = b.s_node or a.e_node = b.e_node
          where b.road_type not in (0, 1, 8, 9) and
                b.link_type not in (3, 5, 7)
        )
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('mid_temp_hwy_inout_link_the_geom_idx')

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

    def load_hwy_inout_link(self):
        '''load Highway Main Link.'''
        self.pg.connect2()
        sqlcmd = """
        SELECT DISTINCT link_id, s_node, e_node, one_way_code,
               link_type, road_type, display_class, toll,
               fazm, tazm, tile_id, null as length,
               road_name,  road_number
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
            link_attr["names"] = get_road_name(link_info[12])
            link_attr[HWY_1ST_ROAD_NAME] = get_first_road_name(link_info[12])
            link_attr["numbers"] = get_road_number(link_info[13])
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
        SELECT node_id, node_name
          FROM node_tbl
          where toll_flag = %s;
        """
        params = (NODE_TOLL_FLAG,)
        for node_id, node_name in self.get_batch_data(sqlcmd, params):
            if node_id in self._graph:
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
        self.pg.connect2()
        if not self.pg.IsExistTable('hwy_link_road_code_info'):
            self.log.error('No table hwy_link_road_code_info.')
            return
        sqlcmd = """
        SELECT road_code, array_agg(node_id) as path
          FROM (
            SELECT road_code, node_id
              FROM hwy_link_road_code_info
              ORDER BY road_code, seq_nm
          ) as a
          GROUP BY road_code;
        """
        for road_code, path in self.get_batch_data(sqlcmd):
            for u, v in zip(path[0:-1], path[1:]):
                data = {HWY_ROAD_CODE: road_code}
                self._graph.add_edge(u, v, data)

    def load_hwy_node_facilcls(self):
        '''加载设施种别'''
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
          where length >= %s and length <= %s
                and delete_flag = 0;
        """
        return self.get_batch_data(sqlcmd, (min_dist, max_dist))

    def get_path(self):
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
          where delete_flag = 0;  -- not delete path
        """
        return self.get_batch_data(sqlcmd)

    def get_road_code_path(self):
        sqlcmd = """
        SELECT road_code, array_agg(node_id) as path
          FROM (
            SELECT road_code, node_id
              FROM hwy_link_road_code_info
              ORDER BY road_code, seq_nm
          ) as a
          GROUP BY road_code
          ORDER BY road_code;
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
                                 # (起点, 终点)
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
        key = road_code
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
        key = road_code
        s_e_nodes = self.__road_start_end_node.get(key)
        if s_e_nodes and node_id == s_e_nodes[0]:
            return True
        return False

    def __load_road_start_end_node(self):
        if self.__road_start_end_node:
            return
        sqlcmd = """
        SELECT road_code, array_agg(node_id) as node_lid
          FROM (
            SELECT road_code, node_id
              FROM hwy_link_road_code_info
              ORDER BY road_code, seq_nm
          ) AS a
          GROUP BY road_code
        """
        self.pg.execute2(sqlcmd)
        for row in self.pg.fetchall2():
            road_code, node_lid = row[0:2]
            start_node = node_lid[0]
            end_node = node_lid[-1]
            key = road_code
            self.__road_start_end_node[key] = (start_node, end_node)

    def is_road_end_node(self, road_code, updown, node_id):
        '''道路的起始点'''
        if not node_id:
            return False
        self.__load_road_start_end_node()
        key = road_code
        s_e_nodes = self.__road_start_end_node.get(key)
        if s_e_nodes and node_id == s_e_nodes[-1]:
            return True
        return False

    def __load_main_path(self):
        sqlcmd = """
        SELECT road_code, array_agg(node_id)
          FROM (
            SELECT road_code, node_id
              FROM hwy_link_road_code_info
              ORDER BY road_code, seq_nm
          ) AS a
          GROUP BY road_code;
        """
        self.pg.execute2(sqlcmd)
        data = self.pg.fetchall2()
        for main_path in data:
            road_code = main_path[0]
            path = main_path[1]
            key = road_code
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
            self._road_no_dict[road_no] = road_code

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
        SELECT node_lid, link_lid
          FROM mid_temp_hwy_ic_path
          where road_code =%s and road_seq = %s
                and node_id = %s and inout_c = %s
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
            path_infos.append((node_lid, link_lid))
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
            self.pg.execute2(sqlcmd, (HWY_INVALID_FACIL_ID, ))
            for row in self.pg.fetchall2():
                node_id = row[0]
                self.__boundary_node_dict[node_id] = None

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

# 加洲
California_BOX = """ WHERE the_geom && ST_SetSRID(ST_MakeBox2D(ST_Point(-119.040,34.586),
                                                                       ST_Point(-117.029,33.026)),
                                                               4326)
                 """
