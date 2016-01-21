# -*- coding: UTF-8 -*-
'''
Created on 2015-4-10

@author: hcz
'''
from component.rdf.hwy.hwy_data_mng_rdf import HwyDataMngRDF


class HwyDataMngMMi(HwyDataMngRDF):
    '''
    Highway Data Manager for Mmi
    '''

    def __init__(self):
        '''
        Constructor
        '''
        HwyDataMngRDF.__init__(self, ItemName='HwyDataMngMMi')

    @staticmethod
    def instance():
        if HwyDataMngRDF._instance is None:
            HwyDataMngRDF._instance = HwyDataMngMMi()
        return HwyDataMngRDF._instance

    def load_hwy_main_link(self):
        '''load Highway Main Link.'''
        self.log.info('Start Loading Highway Main Link.')
        self.pg.connect2()
        sqlcmd = """
                SELECT a.link_id, a.s_node, a.e_node, a.one_way_code, a.link_type, 
                    a.road_type, a.display_class, a.toll, a.fazm, a.tazm, a.tile_id, 
                    a.length, a.road_name, a.road_number, b.extend_flag
                FROM mid_temp_hwy_main_link a
                LEFT JOIN link_tbl b
                    ON a.link_id = b.link_id
                where const_st = false
            """
        # sqlcmd += New_Delhi_BOX
        for link_info in self._get_link_attr(sqlcmd):
            (link_id, s_node, e_node, one_way,
             s_angle, e_angle, link_attr) = link_info
            self._graph.add_link(s_node, e_node,
                                 one_way, link_id,
                                 s_angle, e_angle,
                                 **link_attr
                                 )
        self.log.info('End Loading Highway Main Link.')

    def load_exit_name(self):
        # 印度没有出口名称
        pass

    def load_exit_poi_name(self):
        # 印度没有Exit POI
        pass

    def load_org_facil_id(self):
        pass

    def load_junction_name(self):
        pass
# 新德里
New_Delhi_BOX = """ WHERE the_geom && ST_SetSRID(ST_MakeBox2D(ST_Point(75.492,30.223),
                                                               ST_Point(79.0364,26.7549)),
                                                               4326)
                 """
