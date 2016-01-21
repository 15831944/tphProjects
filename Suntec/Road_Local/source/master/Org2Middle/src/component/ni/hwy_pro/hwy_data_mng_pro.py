# -*- coding: UTF-8 -*-
'''
Created on 2015-10-26

@author: hcz
'''
import json
from component.rdf.hwy.hwy_data_mng_rdf import HwyDataMngRDF
from component.rdf.hwy.hwy_graph_rdf import HWY_PATH_ID
from component.jdb.hwy.hwy_graph import ONE_WAY_POSITIVE
from component.jdb.hwy.hwy_graph import ONE_WAY_RERVERSE
from component.jdb.hwy.hwy_graph import ONE_WAY_BOTH
from component.ni.hwy_pro.hwy_graph_pro import HwyGraphNiPro
from component.ni.hwy_pro.hwy_graph_pro import HWY_FIRST_ICS_LINK
from component.rdf.hwy.hwy_graph_rdf import HWY_ORG_FACIL_ID
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_IC, HWY_PATH_TYPE_IC


class HwyDataMngNiPro(HwyDataMngRDF):
    '''
    classdocs
    '''

    def __init__(self, ItemName='HwyDataMngNiPro'):
        '''
        Constructor
        '''
        HwyDataMngRDF.__init__(self, ItemName)

    @staticmethod
    def instance():
        if HwyDataMngRDF._instance is None:
            HwyDataMngRDF._instance = HwyDataMngNiPro()
        return HwyDataMngRDF._instance

    def initialize(self):
        self._graph = HwyGraphNiPro()  # 高速link图
        self._graph.set_data_mng(self)

    def load_hwy_main_link(self):
        '''load Highway Main Link.'''
        self.log.info('Start Loading Highway Main Link.')
        self.pg.connect2()
        sqlcmd = """
        SELECT a.link_id, a.s_node, a.e_node, a.one_way_code, a.link_type,
            a.road_type, a.display_class, a.toll, a.fazm, a.tazm, a.tile_id,
            a.length, '' AS road_name, '' AS road_number, b.extend_flag
        FROM mid_temp_hwy_main_link a
        LEFT JOIN link_tbl b
        ON a.link_id = b.link_id
        where b.const_st = false
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

    def get_line_name2(self, road_code):
        if not self._line_name_dict:
            self._load_line_name()
        return self._line_name_dict.get(road_code)

    def _load_line_name(self):
        sqlcmd = """
        select road_code, road_name, road_number
          from road_code_info
        """
        for road_code, road_name, road_number in self.get_batch_data(sqlcmd):
            road_name = self.json_name_2_list(road_name)
            road_number = self.json_name_2_list(road_number)
            self._line_name_dict[road_code] = (road_name, road_number)

    def json_name_2_list(self, json_name):
        road_names = []
        if not json_name:
            return road_names
        for names in json.loads(json_name):
            one_name = []
            for name_dict in names:
                if name_dict.get("tts_type") == "not_tts":
                    one_name.append(name_dict)
            road_names.append(one_name)
        return road_names

    def json_number_2_list(self, json_number):
        road_numbers = []
        if not json_number:
            return road_numbers.append('')
        for numbers in json.loads(json_number.replace('\t', '\\t')):
            one_num = list()
            for num_dict in numbers:
                if num_dict.get("tts_type") == "not_tts":
                    one_num.append(num_dict)
            road_numbers.append(one_num)
        return road_numbers

    def load_hwy_inout_link(self):
        '''load Highway Main Link.'''
        self.log.info('Start Loading Highway In/Out Link.')
        self.pg.connect2()
        sqlcmd = """
                SELECT DISTINCT a.link_id, a.s_node, a.e_node, a.one_way_code, 
                    a.link_type, a.road_type, a.display_class, a.toll, a.fazm, 
                    a.tazm, a.tile_id, a.length, a.road_name, a.road_number, b.extend_flag
                FROM mid_temp_hwy_inout_link a
                LEFT JOIN link_tbl b
		        ON a.link_id = b.link_id
		        where b.const_st = false
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

    def load_hwy_ic_link(self):
        '''load Highway Main Link.'''
        self.log.info('Start Loading Highway IC(Ramp/JCT/SAPA) Link.')
        self.pg.connect2()
        sqlcmd = """
                SELECT a.link_id, a.s_node, a.e_node, a.one_way_code, a.link_type, 
                    a.road_type, a.display_class, a.toll, a.fazm, a.tazm, a.tile_id, 
                    a.length, '' AS road_name, '' AS road_number, b.extend_flag
                FROM mid_temp_hwy_ic_link a
                LEFT JOIN link_tbl b
                ON a.link_id = b.link_id
                where b.const_st = false
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

    def _make_ic_links(self):
        # 加载ic link
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
          where (road_type in (0, 1) and
                 link_type not in (1, 2)) -- not main link
                 and const_st = false
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        # 加载road_type=14且link_type=7的link
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
          where (road_type =14 and link_type = 7) -- SAPA link
                and const_st = false
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
          where b.road_type not in (0, 8, 9, 12)
                and const_st = false
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
          where b.road_type not in (0, 8, 9, 12)
                and const_st = false
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('mid_temp_hwy_inout_link_the_geom_idx')

    def load_hwy_path_id(self):
        self.log.info('Load Hwy Path Id.')
        sqlcmd = """
        SELECT hw_pid, inlinkid as link_id,
               s_node, e_node, one_way_code
           FROM mid_hwy_org_hw_junction_mid_linkid as a
           LEFT JOIN link_tbl as b
           ON a.inlinkid = link_id
           where accesstype = 2 -- 出口设施
                 and const_st = false
        UNION
        SELECT hw_pid, outlinkid as link_id,
               s_node, e_node, one_way_code
           FROM mid_hwy_org_hw_junction_mid_linkid as a
           LEFT JOIN link_tbl as b
           ON a.outlinkid = link_id
           where accesstype = 1  -- 入口设施
                 and const_st = false
        ORDER BY link_id, hw_pid;
        """
        prev_link_id = None
        for (path_id, link_id, s_node,
             e_node, one_way) in self.get_batch_data(sqlcmd):
            if not link_id:
                continue
            if prev_link_id == link_id:
                self.log.error('link repeat. link_id=%s' % link_id)
                continue
            prev_link_id = link_id
            data = {HWY_PATH_ID: path_id}
            if one_way == ONE_WAY_POSITIVE:
                if self._graph.has_edge(s_node, e_node):
                    self._graph.add_edge(s_node, e_node, data)
            elif one_way == ONE_WAY_RERVERSE:
                if self._graph.has_edge(e_node, s_node):
                    self._graph.add_edge(e_node, s_node, data)
            elif one_way == ONE_WAY_BOTH:
                if self._graph.has_edge(s_node, e_node):
                    self._graph.add_edge(s_node, e_node, data)
                if self._graph.has_edge(e_node, s_node):
                    self._graph.add_edge(e_node, s_node, data)

    def load_first_ics_link(self):
        '''加载设施出入第一link。(load_hwy_ic_link之后调用)'''
        sqlcmd = """
        SELECT s_node, e_node, one_way_code
           FROM mid_hwy_org_hw_junction_mid_linkid as a
           LEFT JOIN link_tbl as b
           ON a.outlinkid = link_id
           where accesstype = 2 -- 出口设施的outlink
                 and const_st = false
        UNION
        SELECT s_node, e_node, one_way_code
           FROM mid_hwy_org_hw_junction_mid_linkid as a
           LEFT JOIN link_tbl as b
           ON a.inlinkid = link_id
           where accesstype = 1  -- 入口设施的inlink
                and const_st = false
        ORDER BY s_node, e_node
        """
        data = {HWY_FIRST_ICS_LINK: True}
        for (s_node, e_node, one_way) in self.get_batch_data(sqlcmd):
            if one_way == ONE_WAY_POSITIVE:
                if self._graph.has_edge(s_node, e_node):
                    self._graph.add_edge(s_node, e_node, data)
            elif one_way == ONE_WAY_RERVERSE:
                if self._graph.has_edge(e_node, s_node):
                    self._graph.add_edge(e_node, s_node, data)
            elif one_way == ONE_WAY_BOTH:
                if self._graph.has_edge(s_node, e_node):
                    self._graph.add_edge(s_node, e_node, data)
                if self._graph.has_edge(e_node, s_node):
                    self._graph.add_edge(e_node, s_node, data)

    def is_parallel_road(self, road_code, p_road_code):
        if not self._parallel_road_code:
            self._load_parallel_road()
        if((road_code, p_road_code) in self._parallel_road_code or
           (p_road_code, road_code) in self._parallel_road_code):
            return True
        return False

    def _load_parallel_road(self):
        sqlcmd = """
        select road_code, p_road_code
          from hwy_parallel_road_code
        """
        for road_code, p_road_code in self.get_batch_data(sqlcmd):
            self._parallel_road_code[(road_code, p_road_code)] = None

    def load_org_facil_id(self):
        '''加载元设施id'''
        self.log.info('Start Loading ORG Facility ID.')
        sqlcmd = """
        select  distinct node_id, array_agg(road_code)as road_code,
                array_agg(inout_c)as inout_c,
                array_agg(in_out_link) as in_out_link,
                array_agg(facilcls_c) as facilcls_c,
                array_agg(name) as name,
                array_agg(facility_id) as facility_id
        from(
            select distinct hw_pid::bigint as road_code, 1 as updown_c,
                   seq_nm::bigint, id::bigint as facility_id,
                   accesstype::bigint as inout_c, attr::bigint as facilcls_c,
                   nodeid::bigint as node_id, estab_item as cat_ids, b.name,
                   case
                     when accesstype::bigint = 1 then inlinkid::bigint
                     when accesstype::bigint = 2 then outlinkid::bigint
                     else 0
                   end as in_out_link
            from mid_hwy_org_hw_junction_mid_linkid_merged as a
            left join
            ( select featid::bigint,
                     array_to_string(array_agg(name),'|') as name
              from org_hw_fname
              group by featid
            ) as b
            on a.id = b.featid::bigint
            order by hw_pid::bigint, seq_nm::bigint
        )as mm
        group by node_id
        """
        self.pg.connect2()
        for row in self.get_batch_data(sqlcmd):
            (node, road_codes, inout_cs, in_out_links,
             facilcls_cs, facil_names, facility_ids) = row
            facil_info_list = zip(road_codes, facilcls_cs, inout_cs,
                                  in_out_links, facil_names, facility_ids)
            data = {HWY_ORG_FACIL_ID: facil_info_list}
            if node in self._graph:
                self._graph.add_node(node, data)
        self.log.info('End Loading ORG Facility ID.')

    def get_path_by_pathid(self, path_id, updown):
        '''中国专用数据road_code直接取path_id'''
        sqlcmd = """
        SELECT array_agg(node_id) as path
          FROM (
            SELECT road_code, node_id, seq_nm
              FROM hwy_link_road_code_info
              WHERE road_code = %s and updown = %s
              ORDER BY seq_nm
          ) AS a
          group by road_code;
        """
        self.pg.execute1(sqlcmd, (path_id, updown))
        row = self.pg.fetchone()
        if row:
            return row[0]
        else:
            return []

    def get_enter_facil_num(self, link_id):
        '''入口设施数'''
        facil_id_set = set()
        # # 取得前后方设施
        bwd_fwd_list = self.get_link_fwd_bwd_facil(link_id)
        fwd_list = bwd_fwd_list[1]
        # 后方设施数
        for fwd in fwd_list:
            temp_path_type = fwd.get('path_type')
            temp_facil_id = fwd.get('facility_id')
            if temp_facil_id and temp_path_type in (HWY_PATH_TYPE_IC,):
                facil_id_set.add(temp_facil_id)
        return len(facil_id_set)

    def get_exit_facil_num(self, link_id):
        '''出口设施数'''
        facil_id_set = set()
        # # 取得前后方设施
        bwd_fwd_list = self.get_link_fwd_bwd_facil(link_id)
        bwd_list = bwd_fwd_list[0]
        # 后方设施数
        for bwd in bwd_list:
            # temp_node_id = fwd.get('node_id')
            temp_path_type = bwd.get('path_type')
            temp_facil_id = bwd.get('facility_id')
            if temp_facil_id and temp_path_type in (HWY_PATH_TYPE_IC,):
                facil_id_set.add(temp_facil_id)
        return len(facil_id_set)

    def get_cycle_route_start_end(self):
        CYCLE_ROUTE_START_END = [(5788214, ),
                                 (5788233, ),
                                 (1297193, ),
                                 (1294109, ),
                                 (1193896, ),
                                 (1255163, ),
                                 (1749234, ),
                                 (1749231, ),
                                 (73883699, ),
                                 (73883698, ),
                                 (2349326, ),
                                 (2352085, ),
                                 (93607, ),
                                 (68587, ),
                                 (725094, ),
                                 (725092, ),
                                 (12182488, ),
                                 (12182496, ),
                                 (4156978, ),
                                 (4156976, ),
                                 (2944216, ),
                                 (2944212, ),
                                 (5484972, ),
                                 (5487995, ),
                                 (4034530, ),
                                 (4034532, ),
                                 (6737786, ),
                                 (6737789, ),
                                 (1151242, ),
                                 (13968055, ),
                                 (72978578, ),
                                 (72978566, ),
                                 (4161595, ),
                                 (4176068, ),
                                 (289944, ),
                                 (289946, ),
                                 (4724815, ),
                                 (4722492, ),
                                 (6744674, ),
                                 (6744677, ),
                                 (6045741, ),
                                 (6045742, ),
                                 ]
        return CYCLE_ROUTE_START_END
