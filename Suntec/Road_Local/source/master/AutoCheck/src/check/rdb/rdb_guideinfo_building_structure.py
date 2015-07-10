# -*- coding: UTF8 -*-
'''
Created on 2015-3-17

@author: wusheng
'''
import platform.TestCase
import json


class CCheckNodeID(platform.TestCase.CTestCase):
    '''node_id有效值检查'''
    def _do(self):
        sqlcmd = '''
            SELECT node_id FROM rdb_guideinfo_building_structure
            WHERE NOT EXISTS (SELECT node_id FROM rdb_node)
        '''
        self.pg.query(sqlcmd)
        row = self.pg.fetchone()
        if row == None:
            return True
        else:
            return False

class CCheckNodeTileId(platform.TestCase.CTestCase):
    '''check点的Tile Id'''
    def _do(self):
        sqlcmd = """
                SELECT count(*)
                  FROM rdb_guideinfo_building_structure AS A
                  left join rdb_tile_node
                  on A.node_id = tile_node_id and node_id_t <> tile_id
                  where tile_node_id is not null;
              """        
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row:
            if row[0] != 0:
                return False
            else:
                return True
        else:
            return False

class CCheckNumber(platform.TestCase.CTestCase):
    '''检查能关联到logmark的分歧点个数是否合理'''
    def _do(self):
        sqlcmd = """
               SELECT 
                (
                 SELECT COUNT(node_id)
                    from temp_guidence_node
                ) AS A,
                (
                SELECT count(*) FROM
                (
                  SELECT DISTINCT node_id
                    from rdb_guideinfo_building_structure
                ) AS A
                ) AS B
                """
                
        self.pg.query(sqlcmd)
        row = self.pg.fetchone()
        if row:
            if row[0] > row[1]:
                return True
            else:
                return False
        else:
            return False

class CCheckCoordinate(platform.TestCase.CTestCase):
    ''' 检查node的坐标与建筑物坐标之间的差值 '''
    def _do(self):
        sqlcmd = """
                  SELECT max(abs(G.centroid_lantitude/921600.0 - ST_Y(N.the_geom))),
                         max(ST_Distance_Sphere(ST_SetSRID(st_point(G.centroid_lontitude/921600.0, G.centroid_lantitude/921600.0), 4326), N.the_geom))                        
                  FROM rdb_guideinfo_building_structure AS G
                  LEFT JOIN rdb_node AS N
                  ON G.node_id = N.node_id;
                """
        self.pg.query(sqlcmd)
        row = self.pg.fetchone()
        #0.001度大概为100m，250m的正方形，中心到4个点的距离大概为350
        if  row[0] > 0.003 and row[1] > 400:
            return False
        else:
            return True
        
class CCheckName(platform.TestCase.CTestCase):
    ''' 检查building_name 是否为空'''
    def _do(self):
        sqlcmd = '''
            SELECT building_name FROM rdb_guideinfo_building_structure
            WHERE building_name IS NULL;
        '''       
        self.pg.query(sqlcmd)
        row = self.pg.fetchone()
        if row == None: #如果记录为空，则表示building_name = NULL的记录
            return True
        else:
            return False
        
class CCheckTypecode(platform.TestCase.CTestCase):
    ''' 检查Typecode的种类个数是否合理 '''
    def _do(self):
        sqlcmd = '''
            SELECT count(*)
            FROM
            (SELECT DISTINCT type_code FROM rdb_guideinfo_building_structure) AS A
        '''
        self.pg.query(sqlcmd)
        row = self.pg.fetchone()
        if row[0] <= 1:
            return False
        else:
            return True
        
        
class CCheckBuilding_name(platform.TestCase.CTestCase):
    '''  building_name为json格式，并且名称和音素都不为空'''
    def _do(self):
        
        sqlcmd = """
                SELECT building_name
                FROM rdb_guideinfo_building_structure
                where building_name is not null;
                """
              
        for row in self.pg.get_batch_data(sqlcmd):
            building_name = row[0]
            name_lists = json.loads(building_name)
            for name_list in name_lists:
                for name in name_list:
                    if not name.get('val'):
                        return False
        return True     
        