'''
Created on 2013-01-16
@author: xuwenbo
'''

import platform.TestCase
import common.database


class CCheckAdminCodeCount(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                 SELECT count (ad_code) FROM rdb_admin_zone WHERE ad_order = 8
                 """

        self.pg.execute(sqlcmd)
        rdbAdminCount = self.pg.fetchone()[0]

        sqlcmd = """
                 SELECT count (ad_code) FROM
                 (
                     SELECT distinct ad_code FROM rdb_tile_admin_zone
                     WHERE ad_code <> -1
                 ) AS a
                 """

        self.pg.execute(sqlcmd)
        rdbTileAdminCount = self.pg.fetchone()[0]

        return rdbAdminCount == rdbTileAdminCount


class CCheckAdminNameValid_mmi (platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                 SELECT * FROM rdb_admin_zone
                 WHERE ad_name IS NULL
                 """

        self.pg.execute(sqlcmd)

        return   self.pg.getcnt() == 2
    
class CCheckAdminNameValid_ta (platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                 SELECT * FROM rdb_admin_zone
                 WHERE ad_name IS NULL
                 """

        self.pg.execute(sqlcmd)

        return   self.pg.getcnt() == 42
  
class CCheckAdminNameValid_jdb (platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                 SELECT * FROM rdb_admin_zone
                 WHERE length(ad_name) = 4
                 """

        self.pg.execute(sqlcmd)

        return   self.pg.getcnt() == 2
    
class CCheckAdminNameValid_msm (platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                 SELECT * FROM rdb_admin_zone
                 WHERE ad_name is null and ad_order = 8;
                 """

        self.pg.execute(sqlcmd)
        city_name_null = self.pg.getcnt();
        
        sqlcmd = """
                 SELECT * FROM rdb_admin_zone
                 WHERE ad_name is null and (ad_order = 1 or ad_order = 0);
                 """

        self.pg.execute(sqlcmd)
        state_name_null = self.pg.getcnt();

        return   city_name_null == 16 and state_name_null == 0
      
class CCheckAdminNameValid (platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                 SELECT * FROM rdb_admin_zone
                 WHERE ad_name IS NULL
                 """

        self.pg.execute(sqlcmd)

        return   self.pg.getcnt() == 0

class CCheckTileAdminGeomValid(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                 SELECT * FROM rdb_tile_admin_zone
                 WHERE the_geom IS NULL
                 """

        self.pg.execute(sqlcmd)

        return   self.pg.getcnt() == 0

class CCheckTileAdminGeomArea_TA(platform.TestCase.CTestCase):
    '''
    only check idn's area for test
    '''
    def _do(self):
        idnArea = 330631

        sqlcmd = """
                 SELECT sum (st_area (st_geogfromwkb (the_geom), FALSE))
                 FROM rdb_tile_admin_zone WHERE cast(ad_code/100000 as integer) IN
                 (
                  SELECT cast (cast(id/10000000000 as integer)%1000 as integer) FROM org_a0
                  WHERE order00 = 'VNM' group by id);
                 """
        self.pg.execute(sqlcmd)

        return   abs(self.pg.fetchone()[0] / 1000000 - idnArea) < 10000
class CCheckTileAdminGeomArea_RDF(platform.TestCase.CTestCase):
    '''
    only check idn's area for test
    '''
    def _do(self):
        idnArea = 1904569
        sqlcmd = '''
                select distinct iso_country_code
                from rdf_admin_hierarchy;
                '''
        rows = self.pg.get_batch_data(sqlcmd)
        
        for row in rows:    
            rdf_adminname = row[0]
            if rdf_adminname == 'SGP':
                idnArea = 682
            if rdf_adminname == 'IDN':
                idnArea = 1904569
            if rdf_adminname == 'USA':
                idnArea = 9932990
            if rdf_adminname == 'MYS':
                idnArea = 331902

            sqlcmd = """
                     SELECT st_area(st_collect(the_geom),false)
                     FROM rdb_tile_admin_zone WHERE ad_code IN
                     (
                      SELECT admin_place_id FROM rdf_admin_hierarchy
                      where iso_country_code = '[replace_name]');
                     """
            
            sqlcmd = sqlcmd.replace('[replace_name]', rdf_adminname)
            self.pg.execute(sqlcmd)
            
            if abs(self.pg.fetchone()[0] / 1000000 - idnArea) > 10000:
                return False
        
        return  True
    
class CCheckTileAdminGeomArea_MMI(platform.TestCase.CTestCase):
    '''
    only check idn's area for test
    '''
    def _do(self):
        idnArea = 3279831

        sqlcmd = """
                 SELECT sum (st_area (st_geogfromwkb (the_geom), FALSE))
                 FROM rdb_tile_admin_zone WHERE ad_code <> -1;
                 """
        self.pg.execute(sqlcmd)

        return   abs(self.pg.fetchone()[0] / 1000000 - idnArea) < 10000
class CCheckTileAdminGeomArea_jdb(platform.TestCase.CTestCase):
    '''
    only check idn's area for test
    '''
    def _do(self):
        idnArea = 498600

        sqlcmd = """
                 SELECT sum (st_area (st_geogfromwkb (the_geom), FALSE))
                 FROM rdb_tile_admin_zone WHERE ad_code <> -1;
                 """
        self.pg.execute(sqlcmd)

        return   abs(self.pg.fetchone()[0] / 1000000 - idnArea) < 10000
    
class CCheckTileAdminGeomArea_msm(platform.TestCase.CTestCase):
    '''
    only check idn's area for test
    '''
    def _do(self):
        idnArea = 462797

        sqlcmd = """
                 SELECT sum (st_area (st_geogfromwkb (the_geom), FALSE))
                 FROM rdb_tile_admin_zone WHERE ad_code <> -1;
                 """
        self.pg.execute(sqlcmd)

        return   abs(self.pg.fetchone()[0] / 1000000 - idnArea) < 10000
