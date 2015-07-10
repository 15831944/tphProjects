# -*- coding: cp936 -*-
'''
Created on 2012-2-7

@author: hongchenzai
'''
from base.rdb_ItemBase import ItemBase
from common import rdb_log
RADIUS_OF_BUFFER = 250.0

class rdb_guideinfo_building_structure(ItemBase):
    '''
    classdocs
    '''
    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self, 'Building_Structure')
    
    def Do(self):
        self._makeGuidenceNode()
        self._associateNodeLogmark()
        self._makeBuildingStructure()

      
        
    def _makeGuidenceNode(self):
        rdb_log.log('Building_Structure', 'start to make guidence_node.', 'info')
        self.CreateTable2('temp_guidence_node')
        self.CreateFunction2('rdb_find_divided_node')
        self.pg.callproc('rdb_find_divided_node')
        self.pg.commit2()
        self.CreateIndex2('temp_guidence_node_node_polygon_idx')
        
    
    def _associateNodeLogmark(self):
        rdb_log.log('Building_Structure', 'create temp_asso_node_logmark table.', 'info')
        #self.CreateTable2('temp_asso_node_logmark')
        sqlcmd = """
        drop table if exists temp_asso_node_logmark; 
        CREATE TABLE temp_asso_node_logmark AS 
        select node_id,
        unnest((array_agg(poi_id))[1:5]) as poi_id,
        unnest((array_agg(type_code))[1:5]) as type_code,
        unnest((array_agg(building_name))[1:5]) as building_name,
        unnest((array_agg(the_geom))[1:5]) as the_geom,
        unnest((array_agg(distance))[1:5]) as distance
        from
        ( 
            SELECT 
                node_id, 
                ml.poi_id,
                ml.type_code, 
                ml.building_name, 
                ml.the_geom, 
                ST_Distance_Sphere(ml.the_geom, nt.the_geom) AS distance 
            FROM temp_guidence_node AS nt
            INNER JOIN mid_logmark AS ml
            ON ST_Intersects(nt.polygon, ml.the_geom)
            ORDER BY node_id, distance
        )AS A
        group by node_id;
        """
        self.pg.execute(sqlcmd)
        self.pg.commit2() 
        self.CreateIndex2('temp_asso_node_logmark_node_id_idx') 
    
    def _makeBuildingStructure(self):
        rdb_log.log('Building_Structure', 'create rdb_guideinfo_building_structure table.', 'info')
        self.CreateTable2('rdb_guideinfo_building_structure')
        sqlcmd = """
            INSERT INTO rdb_guideinfo_building_structure
            (node_id, node_id_t, type_code, centroid_lontitude, centroid_lantitude, building_name)
            SELECT rtn.tile_node_id, rtn.tile_id, type_code, ST_X(the_geom)*921600, ST_Y(the_geom)*921600, building_name
            FROM temp_asso_node_logmark AS t
            LEFT JOIN rdb_tile_node AS rtn
            ON t.node_id = rtn.old_node_id;
        """
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        #
        
        
        self.CreateIndex2('rdb_guideinfo_building_structure_in_link_id_node_id_idx')
        self.CreateIndex2('rdb_guideinfo_building_structure_node_id_idx')   
        self.CreateIndex2('rdb_guideinfo_building_structure_node_id_t_idx')   

        
        return 0
