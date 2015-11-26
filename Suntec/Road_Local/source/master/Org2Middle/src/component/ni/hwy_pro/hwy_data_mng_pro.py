# -*- coding: UTF-8 -*-
'''
Created on 2015-10-26

@author: hcz
'''
from component.rdf.hwy.hwy_data_mng_rdf import HwyDataMngRDF
from component.rdf.hwy.hwy_graph_rdf import HWY_PATH_ID
from component.jdb.hwy.hwy_graph import ONE_WAY_POSITIVE
from component.jdb.hwy.hwy_graph import ONE_WAY_RERVERSE
from component.jdb.hwy.hwy_graph import ONE_WAY_BOTH
from component.ni.hwy_pro.hwy_graph_pro import HwyGraphNiPro
from component.ni.hwy_pro.hwy_graph_pro import HWY_FIRST_ICS_LINK
from component.rdf.hwy.hwy_graph_rdf import HWY_ORG_FACIL_ID


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
        UNION
        SELECT hw_pid, outlinkid as link_id,
               s_node, e_node, one_way_code
           FROM mid_hwy_org_hw_junction_mid_linkid as a
           LEFT JOIN link_tbl as b
           ON a.outlinkid = link_id
           where accesstype = 1  -- 入口设施
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
        UNION
        SELECT s_node, e_node, one_way_code
           FROM mid_hwy_org_hw_junction_mid_linkid as a
           LEFT JOIN link_tbl as b
           ON a.inlinkid = link_id
           where accesstype = 1  -- 入口设施的inlink
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
            from mid_hwy_org_hw_junction_mid_linkid as a
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
#             facil_dict = dict()
#             facil_name_dict = dict()
#             for facil_info in facil_info_list:
#                 (road_code, inout_c, inout_link, facilcls_c,
#                  facil_name, facility_id) = facil_info
#                 facil_key = (road_code, facilcls_c, inout_c, inout_link)
#                 facil_name_key = (road_code, facilcls_c, inout_c)
#                 f_name = facil_name
#                 f_id = facility_id
#                 if facil_key in facil_dict.keys():
#                     self.log.error('more than one facil key.node=%s' % node)
#                     continue
#                 else:
#                     if (facil_name_key in facil_name_dict.keys() and
#                         facil_name_dict[facil_name_key] != f_name):
#                         self.log.error('more than one facil name=%s' % node)
#                         continue
#                     else:
#                         facil_name_dict[facil_name_key] = f_name
#                     facil_dict[facil_key] = f_id
            data = {HWY_ORG_FACIL_ID: facil_info_list}
            if node in self._graph:
                self._graph.add_node(node, data)
        self.log.info('End Loading ORG Facility ID.')
