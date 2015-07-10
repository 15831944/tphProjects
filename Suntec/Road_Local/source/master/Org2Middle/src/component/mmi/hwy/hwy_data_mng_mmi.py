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
        HwyDataMngRDF.__init__(self)

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
        SELECT link_id, s_node, e_node, one_way_code,
               link_type, road_type, display_class, toll,
               fazm, tazm, tile_id, length,
               road_name, road_number
          FROM mid_temp_hwy_main_link
        """
        sqlcmd += New_Delhi_BOX
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
        pass

# 新德里
New_Delhi_BOX = """ WHERE the_geom && ST_SetSRID(ST_MakeBox2D(ST_Point(75.492,30.223),
                                                               ST_Point(79.0364,26.7549)),
                                                               4326)
                 """
