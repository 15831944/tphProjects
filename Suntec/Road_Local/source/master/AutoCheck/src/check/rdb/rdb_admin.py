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
        strcountry_name = pro_name.getCountryName()
        
        if strpro_name.lower() == 'rdf' :
            sqlcmd = '''
                select distinct iso_country_code
                from rdf_admin_hierarchy;
                '''
            rows = self.pg.get_batch_data(sqlcmd)
        
            for row in rows:    
                rdf_adminname = row[0]
                if rdf_adminname == 'VNM':
                    rdbTileAdminCount = rdbTileAdminCount + 1
                elif rdf_adminname == 'PHL':
                    rdbTileAdminCount = rdbTileAdminCount + 1
                elif rdf_adminname == 'ARG':
                    rdbTileAdminCount = rdbTileAdminCount + 2
                elif rdf_adminname == 'PAN':
                    rdbTileAdminCount = rdbTileAdminCount + 1
                    
        elif strpro_name.lower() == 'ni' and  strcountry_name.lower() == 'chn':
            
            return (rdbAdminCount == rdbTileAdminCount) | (rdbAdminCount == rdbTileAdminCount + 5)
 
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
        elif strpro_name.lower() == 'saf8':
            return num == 662
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
                if rdf_adminname == 'URY':
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
    
class CCheckTileAdminGeomArea(platform.TestCase.CTestCase):
    def _do(self):
        pro_name = common.ConfigReader.CConfigReader.instance()
        strArea = pro_name.getCountryName()
        if strArea.lower() == 'twn':
            twn_area = 36192 * 1000000        
            sqlcmd = """
                     SELECT sum (st_area (st_geogfromwkb (the_geom), FALSE))
                     FROM rdb_tile_admin_zone WHERE ad_code <> -1;
                     """
            sum_area = self.pg.getOnlyQueryResult(sqlcmd)
            diff_per = abs(sum_area - twn_area) / twn_area
            return (diff_per <= 0.05)
    
class CCheckTileAdminGeomArea_TA(platform.TestCase.CTestCase):
    '''
    only check ZAF's area for test
    '''
    def _do(self):
            
        sqlcmd = '''
                select distinct order00
                from org_a0
                where name is not null and name <> 'Outer World';
                '''
        rows = self.pg.get_batch_data(sqlcmd)
        
        for row in rows:    
            TA_adminname = row[0]
            if TA_adminname == 'MOZ':
                idnArea = 799400 * 1000000            
            elif TA_adminname == 'NAM':
                idnArea = 824269 * 1000000
            elif TA_adminname == 'LSO':
                idnArea = 30344 * 1000000
            elif TA_adminname == 'ZWE':
                idnArea = 390000.0587 * 1000000
            elif TA_adminname == 'SWZ':
                idnArea = 17363 * 1000000
            elif TA_adminname == 'BWA':
                idnArea = 581730 * 1000000
            elif TA_adminname == 'ZAF':
                idnArea = 1221037 * 1000000        
            elif TA_adminname == 'ZMB':
                idnArea = 750000 * 1000000
            elif TA_adminname == 'VNM':
                idnArea = 330631 * 1000000
            elif TA_adminname == 'AUS':
                idnArea = 7703874 * 1000000
            elif TA_adminname == 'NZL':
                idnArea = 267499 * 1000000                                     
            else:
                return False
                                
                
            sqlcmd = """
                    select st_area(st_collect(the_geom),false)
                    from
                    (
                        select distinct ((id/10000000000)::integer)%1000*100000 + 1 as code
                        from org_a0
                        where order00 = '[replace_name]'
                    )a
                    left join rdb_admin_zone as b
                    on a.code = b.order0_id and b.ad_order = 8
                    left join rdb_tile_admin_zone as c
                    on b.ad_code = c.ad_code;                    
                     """
            
            sqlcmd = sqlcmd.replace('[replace_name]', TA_adminname)
            
            sum_area = self.pg.getOnlyQueryResult(sqlcmd)
            diff_per = abs(sum_area - idnArea) / idnArea
            
            if diff_per > 0.05:
                return False
        
        return  True
   
class CCheckTileAdminGeomArea_CHINA(platform.TestCase.CTestCase):
    '''
    only check china's area for test
    '''
    def _do(self):
        
        idnArea = 9571717 * 1000000          
        sqlcmd = """
                 SELECT sum (st_area (st_geogfromwkb (the_geom), FALSE))
                 FROM rdb_tile_admin_zone WHERE ad_code <> -1;
                 """
        sum_area = self.pg.getOnlyQueryResult(sqlcmd)
        diff_per = abs(sum_area - idnArea) / idnArea
        
        return (diff_per <= 0.05)
            
class CCheckTileAdminGeomArea_RDF(platform.TestCase.CTestCase):
    '''
    only check idn's area for test
    '''
    def _do(self):
        sqlcmd = '''
                select distinct iso_country_code
                from rdf_country;
                '''
        rows = self.pg.get_batch_data(sqlcmd)
        
        for row in rows:    
            rdf_adminname = row[0]
            if rdf_adminname == 'SGP':
                idnArea = 718.3 * 1000000                
            elif rdf_adminname == 'THA':
                idnArea = 513120 * 1000000    
            elif rdf_adminname == 'IDN':
                idnArea = 1904569 * 1000000    
            elif rdf_adminname == 'BRN':
                idnArea = 5765 * 1000000    
            elif rdf_adminname == 'MYS':
                idnArea = 330257 * 1000000    
            elif rdf_adminname == 'PHL':
                idnArea = 299700 * 1000000    
            elif rdf_adminname == 'VNM':
                idnArea = 329556 * 1000000            
            elif rdf_adminname == 'USA':
                idnArea = 9932990 * 1000000    
            elif rdf_adminname == 'HKG':
                idnArea = 2755 * 1000000    
            elif rdf_adminname == 'MAC':
                idnArea = 62 * 1000000    
            elif rdf_adminname == 'TWN':
                idnArea = 36664 * 1000000    
            elif rdf_adminname == 'BHR':
                idnArea = 775 * 1000000    
            elif rdf_adminname == 'OMN':
                idnArea = 309168 * 1000000    
            elif rdf_adminname == 'JOR':
                idnArea = 89277 * 1000000    
            elif rdf_adminname == 'KWT':
                idnArea = 17332 * 1000000    
            elif rdf_adminname == 'QAT':
                idnArea = 11615 * 1000000    
            elif rdf_adminname == 'LBN':
                idnArea = 10219 * 1000000    
            elif rdf_adminname == 'SAU':
                idnArea = 1928884 * 1000000    
            elif rdf_adminname == 'ARE':
                idnArea = 71504 * 1000000    
            elif rdf_adminname == 'ZAF':
                idnArea = 1221182 * 1000000    
            elif rdf_adminname == 'LSO':
                idnArea = 30576 * 1000000    
            elif rdf_adminname == 'NAM':
                idnArea = 826115 * 1000000    
            elif rdf_adminname == 'SWZ':
                idnArea = 17396 * 1000000    
            elif rdf_adminname == 'BWA':
                idnArea = 579564 * 1000000    
            elif rdf_adminname == 'BRA':
                idnArea = 8547404 * 1000000    
            elif rdf_adminname == 'ARG':
                idnArea = 2793791 * 1000000     
            elif rdf_adminname == 'URY':
                idnArea = 193509 * 1000000                    
            elif rdf_adminname == 'PRY':
                idnArea = 406800 * 1000000     
            elif rdf_adminname == 'CRI':
                idnArea = 51100 * 1000000    
            elif rdf_adminname == 'PAN':
                idnArea = 75517 * 1000000     
                         
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
            
            sum_area = self.pg.getOnlyQueryResult(sqlcmd)
            diff_per = abs(sum_area - idnArea) / idnArea
            
            if diff_per > 0.05:
                return False
        
        return  True
    
class CCheckTileAdminGeomArea_MMI(platform.TestCase.CTestCase):
    '''
    only check idn's area for test
    '''
    def _do(self):
        idnArea = 3279831 * 1000000    

        sqlcmd = """
                 SELECT sum (st_area (st_geogfromwkb (the_geom), FALSE))
                 FROM rdb_tile_admin_zone WHERE ad_code <> -1;
                 """
        sum_area = self.pg.getOnlyQueryResult(sqlcmd)
        diff_per = abs(sum_area - idnArea) / idnArea
        
        return (diff_per <= 0.05)
    
class CCheckTileAdminGeomArea_jdb(platform.TestCase.CTestCase):
    '''
    only check idn's area for test
    '''
    def _do(self):
        idnArea = 498600 * 1000000    


        sqlcmd = """
                 SELECT sum (st_area (st_geogfromwkb (the_geom), FALSE))
                 FROM rdb_tile_admin_zone WHERE ad_code <> -1;
                 """
        sum_area = self.pg.getOnlyQueryResult(sqlcmd)
        diff_per = abs(sum_area - idnArea) / idnArea
        
        return (diff_per <= 0.05)
    
class CCheckTileAdminGeomArea_msm(platform.TestCase.CTestCase):
    '''
    only check idn's area for test
    '''
    def _do(self):
        idnArea = 462797 * 1000000  

        sqlcmd = """
                 SELECT sum (st_area (st_geogfromwkb (the_geom), FALSE))
                 FROM rdb_tile_admin_zone WHERE ad_code <> -1;
                 """
        sum_area = self.pg.getOnlyQueryResult(sqlcmd)
        diff_per = abs(sum_area - idnArea) / idnArea
        
        return (diff_per <= 0.05)

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
        
class CCheckAdminOrder(platform.TestCase.CTestCase):
    def _do(self):
        pro_name = common.ConfigReader.CConfigReader.instance()
        strArea = pro_name.getCountryName()
        rec_count = 0
        if strArea.lower() == 'twn':
            sqlcmd = '''
                    select count(*)
                    from
                    (
                     select or8.*
                     from rdb_admin_zone as or8
                     left join rdb_admin_zone as or1
                     on or8.order1_id = or1.ad_code and or1.ad_order = 1   
                     where or8.ad_order = 8 and  or1.ad_code is null 
                    )as tbl
                    '''
            rec_count = self.pg.getOnlyQueryResult(sqlcmd)
            return   (rec_count == 0) 
        
        else:        
            exception_order0_id_sqlcmd ='''
                select count(*) from rdb_admin_zone
                where order0_id not in
                (
                select ad_code 
                from rdb_admin_zone
                where ad_order = 0
                )  
                '''  
            rec_count = self.pg.getOnlyQueryResult(exception_order0_id_sqlcmd) 
            
            if  rec_count>0:
                return False   
                           
            sqlcmd = '''
                     select ad_code 
                     from rdb_admin_zone
                     where ad_order = 0
                     '''
            rows = self.pg.get_batch_data(sqlcmd)
            
            for row in rows:    
                countrycode = row[0]
                           
                check_order2_exist_sqlcmd = '''
                         select count(*) from rdb_admin_zone
                         where ad_order = 2 and order0_id = %d 
                         '''%countrycode

                self.pg.execute(check_order2_exist_sqlcmd)
                check_order2_exist__count = self.pg.getOnlyQueryResult(check_order2_exist_sqlcmd)
                
                if check_order2_exist__count > 0:
    
                    sqlcmd = '''
                             select count(*)
                             from
                             (
                                 select or8.* from 
                                 rdb_admin_zone as or8
                                 left join rdb_admin_zone as or0
                                 on or8.order0_id = or0.ad_code and or0.ad_order = 0
                                 left join rdb_admin_zone as or1
                                 on or8.order1_id = or1.ad_code and or1.ad_order = 1
                                 left join rdb_admin_zone as or2
                                 on or8.order2_id = or2.ad_code and or2.ad_order = 2
                                 where or8.order0_id = %d 
                                       and  
                                       or8.ad_order = 8 
                                       and 
                                       (
                                           or0.ad_code is null 
                                           or 
                                           or1.ad_code is null 
                                           or 
                                           (or8.order2_id is not null and or2.ad_code is null)
                                         )
                                 union
                                 
                                 select or2.* from 
                                 rdb_admin_zone as or2
                                 left join rdb_admin_zone as or0
                                 on or2.order0_id = or0.ad_code and or0.ad_order = 0
                                 left join rdb_admin_zone as or1
                                 on or2.order1_id = or1.ad_code and or1.ad_order = 1
                                 where or2.order0_id = %d and or2.ad_order = 2 and (or0.ad_code is null or or1.ad_code is null)
                                 
                                 union
                                 
                                 select or1.* from 
                                 rdb_admin_zone as or1
                                 left join rdb_admin_zone as or0
                                 on or1.order0_id = or0.ad_code and or0.ad_order = 0
                                 where or1.order0_id = %d and or1.ad_order = 1 and or0.ad_code is null
                             )as tbl
                                                
                             ''' %(countrycode,countrycode,countrycode)
                else:
                    sqlcmd = '''
                             select count(*)
                             from
                             (
                                 select or8.* from 
                                 rdb_admin_zone as or8
                                 left join rdb_admin_zone as or0
                                 on or8.order0_id = or0.ad_code and or0.ad_order = 0
                                 left join rdb_admin_zone as or1
                                 on or8.order1_id = or1.ad_code and or1.ad_order = 1
                                 where or8.order0_id = %d and or8.ad_order = 8 and (or0.ad_code is null or or1.ad_code is null) 
                                                           
                                 union
                                 
                                 select or1.* from 
                                 rdb_admin_zone as or1
                                 left join rdb_admin_zone as or0
                                 on or1.order0_id = or0.ad_code and or0.ad_order = 0
                                 where or1.order0_id = %d and or1.ad_order = 1 and (or0.ad_code is null)
                             )as tbl
                                                
                             ''' %(countrycode,countrycode)
                             
                rec_count = self.pg.getOnlyQueryResult(sqlcmd)
                
                if rec_count >0:
                    return False
                         
            return  True      
class CCheckAdOrder_relation(platform.TestCase.CTestCase):
    def _do(self):
        
        pro_name = common.ConfigReader.CConfigReader.instance()
        strArea = pro_name.getCountryName()
        rec_count = 0
        if strArea.lower() == 'twn':
            return True
        else:
            sqlcmd = '''
                     select ad_code 
                     from rdb_admin_zone
                     where ad_order = 0
                     '''
                     
            rows = self.pg.get_batch_data(sqlcmd)
        
            for row in rows:    
                countrycode = row[0]
                           
                check_order2_exist_sql = '''
                         select count(*) from rdb_admin_zone
                         where ad_order = 2 and order0_id = %d
                         '''%countrycode
                
                check_order2_exist__count = self.pg.getOnlyQueryResult(check_order2_exist_sql)
                
                ng_cnt = 0
        
                if check_order2_exist__count > 0:
                    sqlcmd ='''
                            select count(*)
                            from rdb_admin_zone as or8
                            left join rdb_admin_zone as or2
                            on or8.order2_id = or2.ad_code and or2.ad_order = 2
                            where or8.order0_id = %d and or8.ad_order = 8 and (or8.order1_id <> or2.order1_id or or8.order0_id <> or2.order0_id)
                             '''%countrycode
                             
                    ng_cnt = self.pg.getOnlyQueryResult(sqlcmd)
                    
                    sqlcmd ='''
                            select count(*)
                            from rdb_admin_zone as or2
                            left join rdb_admin_zone as or1
                            on or2.order1_id = or1.ad_code and or1.ad_order = 1
                            where or2.order0_id = %d and or2.ad_order = 2 and or2.order0_id <> or1.order0_id
                            '''%countrycode
                            
                    ng_cnt = ng_cnt+self.pg.getOnlyQueryResult(sqlcmd)
                     
                else:
                    sqlcmd ='''
                             select count(*)
                             from rdb_admin_zone as or8
                             left join rdb_admin_zone as or1
                             on or8.order1_id = or1.ad_code and or1.ad_order = 1
                             where or8.order0_id = %d and or8.ad_order = 8 and or1.order0_id <> or8.order0_id
                             '''%countrycode
                    ng_cnt = self.pg.getOnlyQueryResult(sqlcmd)
                    
                if ng_cnt > 0 :
                    return False
                
        return True
    
class CCheckAdCode_Eaqual_OrderId(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                 select count(*) from 
                 (
                     select ad_code, 
                     case ad_order
                     when 0 then order0_id
                     when 1 then order1_id
                     when 2 then order2_id
                     when 8 then order8_id
                     end as order_id 
                     from rdb_admin_zone                 
                 ) as tbl
                 where ad_code <> order_id or order_id is null        
                 '''
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        
        return  (rec_count == 0)
    
class CCheckAdmin_order8_time_zone(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                SELECT count(*)
                FROM rdb_admin_zone
                where ad_order = 8 and time_zone = -1;
                 '''
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        
        return  (rec_count == 0)
