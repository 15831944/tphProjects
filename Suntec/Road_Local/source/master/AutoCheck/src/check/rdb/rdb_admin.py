'''
Created on 2013-01-16
@author: xuwenbo
'''

import platform.TestCase
import common.database
import common.ConfigReader


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
        
        pro_name = common.ConfigReader.CConfigReader.instance()
        strpro_name = pro_name.getProjName()
        
        if strpro_name.lower() == 'rdf' :
            sqlcmd = '''
                select distinct iso_country_code
                from rdf_admin_hierarchy;
                '''
            rows = self.pg.get_batch_data(sqlcmd)
        
            for row in rows:    
                rdf_adminname = row[0]
#                if rdf_adminname == 'MYS':
#                    rdbTileAdminCount = rdbTileAdminCount + 2
#                elif rdf_adminname == 'HKG':
#                    rdbTileAdminCount = rdbTileAdminCount + 1
                if rdf_adminname == 'VNM':
                    rdbTileAdminCount = rdbTileAdminCount + 1
                elif rdf_adminname == 'ARG':
                    rdbTileAdminCount = rdbTileAdminCount + 2       

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
        num = self.pg.getcnt()
        
        pro_name = common.ConfigReader.CConfigReader.instance()
        strpro_name = pro_name.getCountryName()
        
        if strpro_name.lower() == 'vnm':
            return num == 42
        else:
            return num == 0
  
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
        
        name_null_num = self.pg.getcnt()
        
        pro_name = common.ConfigReader.CConfigReader.instance()
        strpro_name = pro_name.getProjName()
        
        if strpro_name.lower() == 'rdf' :
            sqlcmd = '''
                select distinct iso_country_code
                from rdf_admin_hierarchy;
                '''
            rows = self.pg.get_batch_data(sqlcmd)
        
            for row in rows:    
                rdf_adminname = row[0]
                if rdf_adminname == 'TWN':
                    name_null_num = name_null_num - 1

        return   name_null_num == 0

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
        idnArea = 0
        
        pro_name = common.ConfigReader.CConfigReader.instance()
        strpro_name = pro_name.getCountryName()
        
        if strpro_name.lower() == 'vnm':           
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
        sqlcmd = """
                select distinct order00
                from org_a0
                where name is not null and name <> 'Outer World'
                """
        rows = self.pg.get_batch_data(sqlcmd)
        idnArea = 0
        for row in rows:
            if row[0].lower() == 'aus':
                idnArea += 7703874
            elif row[0].lower() == 'nzl':
                idnArea += 267499
            else:
                return False
            
        sqlcmd = """
                 SELECT sum (st_area (st_geogfromwkb (the_geom), FALSE))
                 FROM rdb_tile_admin_zone WHERE ad_code <> -1;
                 """
        self.pg.execute(sqlcmd)
        return   abs(self.pg.fetchone()[0] / 1000000 - idnArea) < 10000
    
class CCheckTileAdminGeomArea_CHINA(platform.TestCase.CTestCase):
    '''
    only check china's area for test
    '''
    def _do(self):
        
        idnArea = 9571717            
        sqlcmd = """
                 SELECT sum (st_area (st_geogfromwkb (the_geom), FALSE))
                 FROM rdb_tile_admin_zone WHERE ad_code <> -1;
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
            elif rdf_adminname == 'IDN':
                idnArea = 1904569
            elif rdf_adminname == 'USA':
                idnArea = 9932990
            elif rdf_adminname == 'MYS':
                idnArea = 331902
            elif rdf_adminname == 'HKG':
                idnArea = 1261
            elif rdf_adminname == 'MAC':
                idnArea = 29
            elif rdf_adminname == 'TWN':
                idnArea = 36664
            elif rdf_adminname == 'VNM':
                idnArea = 330750
            elif rdf_adminname == 'BHR':
                idnArea = 775
            elif rdf_adminname == 'OMN':
                idnArea = 309168
            elif rdf_adminname == 'JOR':
                idnArea = 89277
            elif rdf_adminname == 'KWT':
                idnArea = 17332
            elif rdf_adminname == 'QAT':
                idnArea = 11615
            elif rdf_adminname == 'LBN':
                idnArea = 10219
            elif rdf_adminname == 'SAU':
                idnArea = 1928884
            elif rdf_adminname == 'ARE':
                idnArea = 71504
            elif rdf_adminname == 'ZAF':
                idnArea = 1221182
            elif rdf_adminname == 'LSO':
                idnArea = 30576
            elif rdf_adminname == 'NAM':
                idnArea = 826115
            elif rdf_adminname == 'SWZ':
                idnArea = 17396
            elif rdf_adminname == 'BWA':
                idnArea = 579564
            elif rdf_adminname == 'BRA':
                idnArea = 8547404
            elif rdf_adminname == 'ARG':
                idnArea = 2793791
            else:
                return False
                                
                
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

class CCheckAdminTimeZone(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                 SELECT count(*)
                 FROM rdb_admin_zone 
                 WHERE not (time_zone >= -1 and time_zone <= 240)
                 """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return   (rec_count == 0)

class CCheckAdminSummerTimeID(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                 SELECT count(a.ad_code)
                 FROM rdb_admin_zone as a
                 left join rdb_admin_summer_time as b
                 on a.summer_time_id = b.summer_time_id
                 WHERE a.summer_time_id != 0 and b.summer_time_id is null
                 """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return   (rec_count == 0)

class CCheckAdminSummerTime(platform.TestCase.CTestCase):
    def _do(self):
        try:
            self.pg.CreateFunction_ByName('rdb_check_admin_summer_time')
            self.pg.callproc('rdb_check_admin_summer_time')
            return True
        except:
            self.logger.exception('fail to check...')
            return False

