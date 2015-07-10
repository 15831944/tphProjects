# -*- coding: UTF-8 -*-
'''
Created on 2015-4-29

@author: hcz
'''
from component.rdf.hwy.hwy_data_mng_rdf import HwyDataMngRDF
from component.rdf.hwy.hwy_graph_rdf import HWY_EXIT_POI_NAME
from component.rdf.hwy.hwy_graph_rdf import HWY_ORG_FACIL_ID
from component.ni.hwy.hwy_graphy_ni import HwyGraphNi
from component.rdf.hwy.hwy_path_graph_rdf import HwyPathGraphRDF


class HwyDataMngNi(HwyDataMngRDF):
    '''
    Highway Data Manager(NavInfo)
    '''

    def __init__(self, ItemName='HwyDataMngNi'):
        '''
        Constructor
        '''
        HwyDataMngRDF.__init__(self, ItemName)

    @staticmethod
    def instance():
        if HwyDataMngRDF._instance is None:
            HwyDataMngRDF._instance = HwyDataMngNi()
        return HwyDataMngRDF._instance

    def initialize(self):
        self._graph = HwyGraphNi()  # 高速link图
        self._path_graph = HwyPathGraphRDF(self._graph)  # 高速路径图

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

    def load_exit_poi_name(self):
        '''加载HWY exit POI Name.'''
        if not self.pg.IsExistTable('mid_temp_hwy_exit_enter_poi_name_ni'):
            self.log.warning('No Table mid_temp_hwy_exit_enter_poi_name_ni.')
            return
        # 下面的SQL代码时临时
        sqlcmd = """
        SELECT node_id, array_agg(name) as names
          FROM (
                SELECT node_id, name
                  FROM mid_temp_hwy_exit_enter_poi_name_ni as a
                  where kind = 'Exit'
                  order by node_id, poi_id
          ) AS a
          group by node_id;
        """
        for exit_info in self.get_batch_data(sqlcmd):
            node_id, names = exit_info
            temp_names = []
            temp_names.append(names[0][1:-1])  # 去掉头尾的[]
            for name in names[1:]:
                if name[1:-1] not in temp_names:
                    temp_names.append(name[1:-1])
            str_name = ','.join(temp_names)
            str_name = '[' + str_name + ']'
            data = {HWY_EXIT_POI_NAME: str_name}
            if node_id in self._graph:
                self._graph.add_node(node_id, data)

    def load_exit_name(self):
        '''加载出口番号'''
        return 0

    def load_junction_name(self):
        pass

    def load_org_facil_id(self):
        '''加载元设施id'''
        sqlcmd = """
        SELECT node_id, poi_id
          FROM mid_temp_hwy_exit_enter_poi_name_ni
        """
        for facil_info in self.get_batch_data(sqlcmd):
            node, facil_id = facil_info
            data = {HWY_ORG_FACIL_ID: facil_id}
            if node in self._graph:
                self._graph.add_node(node, data)

    def _make_hwy_inout_of_inner_link(self):
        return 0

    def get_cycle_route_start_end(self):
        CYCLE_ROUTE_START_END = [(356871, ),  # 中国(14)
                                 (356838, ),
                                 (2352085, ),
                                 (2352077, ),
                                 (1294109, ),
                                 (1297193, ),
                                 (5788233, ),
                                 (5788214, ),
                                 (4722492, ),
                                 (4724815, ),
                                 (392192, ),
                                 (392185, ),
                                 (4156976, ),
                                 (4156978, ),
                                 (59565502834, ),  # 中国(11)
                                 (59565502840, ),
                                 (44463603202, ),
                                 (44463603195, ),
                                 (68665601146, ),
                                 (68665600091, ),
                                 (34535300044, ),
                                 (34535300072, ),
                                 (51494001397, ),
                                 (51494001606, )
                                 ]
        return CYCLE_ROUTE_START_END
