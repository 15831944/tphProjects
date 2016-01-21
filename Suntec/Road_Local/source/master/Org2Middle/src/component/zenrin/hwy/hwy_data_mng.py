# -*- coding: UTF-8 -*-
'''
Created on 2015-6-29

@author: PC_ZH
'''
from component.rdf.hwy.hwy_data_mng_rdf import HwyDataMngRDF
from component.rdf.hwy.hwy_path_graph_rdf import HwyPathGraphRDF
from component.zenrin.hwy.hwy_graph import HwyGraphZenrin
from component.rdf.hwy.hwy_graph_rdf  import HWY_UPDOWN_CODE
from component.rdf.hwy.hwy_graph_rdf import HWY_ROAD_CODE
from component.rdf.hwy.hwy_graph_rdf import HWY_ORG_FACIL_INFO


class HwyDataMngZenrin(HwyDataMngRDF):
    ''' '''
    _instance = None

    @staticmethod
    def instance():
        if HwyDataMngZenrin._instance is None:
            HwyDataMngZenrin._instance = HwyDataMngZenrin()
        return HwyDataMngZenrin._instance

    def __init__(self, ItemName='HwyDataMngZenrin'):
        ''' '''
        HwyDataMngRDF.__init__(self, ItemName)
        self._graph = None
        self._path_graph = None

    def initialize(self):
        self._graph = HwyGraphZenrin()  # 高速link图
        self._path_graph = HwyPathGraphRDF(self._graph)  # 高速路径图

    def load_hwy_road_code(self):
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

    def load_exit_name(self):
        '''加载出口番号'''
        self.log.info('load exit name ')
        pass

    def load_junction_name(self):
        self.log.info('load junction name')
        pass

    def load_tollgate(self):
        self.log.info('load tollgate')
        pass

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
          where (road_type in (0, 1) and
                 link_type not in (1, 2))-- not main link
                and const_st = false
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('mid_temp_hwy_ic_link_the_geom_idx')

    def get_road_code_path(self):
        sqlcmd = """
        SELECT road_code, updown, array_agg(node_id) as path
          FROM (
            SELECT road_code, updown, node_id
              FROM hwy_link_road_code_info
              ORDER BY road_code, seq_nm
          ) as a
          GROUP BY road_code, updown
          ORDER BY road_code, updown
        """
        return self.get_batch_data(sqlcmd)

    def load_org_facil_id(self):
        '''加载元设施id'''
        if not self.pg.IsExistTable('mid_hwy_org_facility_node'):
            self.log.warning('No table mid_hwy_org_facility_node.')
            return 0
        sqlcmd = """
        SELECT node_id,
               array_agg(path_id) as path_lid,
               array_agg(facility_id)as facility_lid ,
               array_agg(pnttype) as pnttype_lid
        FROM mid_hwy_org_facility_node as a
        LEFT JOIN org_highwaypoint as b
        on a.facility_id = b.pntid
        group by node_id
        """
        for facil_info in self.get_batch_data(sqlcmd):
            node, path_lid, facil_lid, pnttype_lid = facil_info
            data = {HWY_ORG_FACIL_INFO: (path_lid, facil_lid, pnttype_lid)}
            if node in self._graph:
                self._graph.add_node(node, data)
