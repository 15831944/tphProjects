# -*- coding: UTF8 -*-
'''
Created on 2015-3-11

@author: hcz
'''
from component.component_base import comp_base


class HwyPrepareRDF(comp_base):
    '''
    Hwy Prepare
    '''

    def __init__(self, itemname='HwyPrepareRDF'):
        '''
        Constructor
        '''
        comp_base.__init__(self, itemname)

    def _DoCreateTable(self):
        self.CreateTable2('mid_temp_hwy_exit_poi')
        return 0

    def _DoCreateIndex(self):
        self.CreateIndex2('mid_temp_hwy_exit_poi_poi_id_idx')
        self.CreateIndex2('mid_temp_hwy_exit_poi_the_geom_idx')
        return 0

    def _Do(self):
        self._make_hwy_exit_poi()
        self._make_all_hwy_node()
        return 0

    def _make_hwy_exit_poi(self):
        '''
        make table mid_temp_hwy_poi_exit
        '''
        self.log.info('start make mid_temp_hwy_exit_poi')
        sqlcmd = '''
        INSERT INTO mid_temp_hwy_exit_poi(poi_id, cat_id, the_geom)
        (
            SELECT org_poi.poi_id, cat_id,
                   ST_GeometryFromText("location", 4326) AS the_geom
             FROM
             (
               SELECT poi_id, cat_id
               FROM rdf_poi
               WHERE cat_id = 9592 --9592:HWY EXIT
             ) AS org_poi
             LEFT JOIN rdf_poi_address
             ON org_poi.poi_id = rdf_poi_address.poi_id
             LEFT JOIN wkt_location
             ON rdf_poi_address.location_id = wkt_location.location_id
             ORDER BY org_poi.poi_id
        );
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.log.info('end make mid_temp_hwy_exit_poi')
        return 0

    def _make_all_hwy_node(self):
        # 高速特征点(高速出入口点和高速设施所在点)
        self.CreateTable2('mid_all_highway_node')
        sqlcmd = '''
        INSERT INTO mid_all_highway_node(node_id)
        (
            SELECT distinct node_id
            FROM mid_temp_hwy_exit_poi
            LEFT JOIN node_tbl
            ON  ST_DWithin(node_tbl.the_geom, mid_temp_hwy_exit_poi.the_geom,
                           0.000002694945852358566745)
            ORDER BY node_id
        );
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        # 出口名称/交叉点名称所在link的起末点
        self._make_exit_node()
        return 0

    def _make_exit_node(self):
        '''出口名称/交叉点名称所在link的起末点'''
        sqlcmd = '''
        INSERT INTO mid_all_highway_node(node_id)
        (
        SELECT ref_node_id
        FROM  rdf_road_name AS road_name
        LEFT JOIN rdf_road_link AS road_link
        ON road_name.road_name_id = road_link.road_name_id
        LEFT JOIN rdf_link
        ON rdf_link.link_id = road_link.link_id
        LEFT JOIN mid_all_highway_node as hwy_node
        ON ref_node_id = node_id
        WHERE (road_link.is_exit_name='Y' or road_link.is_junction_name='Y')
              and hwy_node.node_id is null

        union

        SELECT nonref_node_id
        FROM  rdf_road_name AS road_name
        LEFT JOIN rdf_road_link AS road_link
        ON road_name.road_name_id = road_link.road_name_id
        LEFT JOIN rdf_link
        ON rdf_link.link_id = road_link.link_id
        LEFT JOIN mid_all_highway_node as hwy_node
        ON nonref_node_id = node_id
        WHERE (road_link.is_exit_name='Y' or road_link.is_junction_name='Y')
              and hwy_node.node_id is null
        );
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
