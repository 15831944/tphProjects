# -*- coding: UTF8 -*-
'''
Created on 2011-12-22

@author: liuxinxing
'''
import platform.TestCase

        
class CCheckLinkIDValid(platform.TestCase.CTestCase):
    def _do(self):

            sqlcmd = """
                select count(*) from (
                    select * from (
                          select a.link_id as link1, b.link_id as link2
                          from rdb_cond_speed as a 
                          left join 
                          rdb_link b
                          on a.link_id = b.link_id
                    ) as c 
                    where c.link2 is null
                ) as a ;
                """
        
            self.pg.execute(sqlcmd)
            row = self.pg.fetchone()
            if row:
                if row[0] == 0:
                    return True
            return False          
                    
class CCheckLinkIDUnique(platform.TestCase.CTestCase):
    def _do(self):
 
            sqlcmd = """
                select count(*) from (
                    select link_id  from rdb_cond_speed 
                    group by link_id 
                    having count(link_id) > 1
                ) as a 
                """
        
            self.pg.execute(sqlcmd)
            row = self.pg.fetchone()
            if row:
                if row[0] == 0:
                    return True
            return False
                    
class CCheckTileID(platform.TestCase.CTestCase):  
    def _do(self):
        
        sqlcmd = """

                select * from (
                    select link.link_id as id_link,link.link_id_t as tile_link,speed.link_id as id_speed,speed.link_id_t as tile_speed 
                    from rdb_link link,rdb_cond_speed speed
                     where  link.link_id = speed.link_id
                ) as result
                where result.tile_link <> result.tile_speed;

                """
        
        if self.pg.execute(sqlcmd) == -1:
            return 0
        else:
            if self.pg.getcnt() == 0:
                return 1
            else:
                return 0

class CCheckPosSpeedValid(platform.TestCase.CTestCase):  
    def _do(self):
        
        sqlcmd = """
                select * from rdb_cond_speed 
                where pos_cond_speed <0 or pos_cond_speed >160
                """
        
        if self.pg.execute(sqlcmd) == -1:
            return 0
        else:
            if self.pg.getcnt() == 0:
                return 1
            else:
                return 0

class CCheckPosSpeedValid_RDF(platform.TestCase.CTestCase):  
    def _do(self):
        
        sqlcmd = """
                select * from rdb_cond_speed 
                where pos_cond_speed < -1 or pos_cond_speed >160
                """
        
        if self.pg.execute(sqlcmd) == -1:
            return 0
        else:
            if self.pg.getcnt() == 0:
                return 1
            else:
                return 0
                        
class CCheckPosSpeedRationality1(platform.TestCase.CTestCase):  
    def _do(self):
        
        sqlcmd = """
                select * from (
                    select * from (
                       select  a.link_id as linkid1,a.pos_cond_speed,b.link_id as linkid2 
                       from rdb_cond_speed as a
                       left join 
                       (
                           select link_id from rdb_link where one_way not in (1,2) 
                        ) as b
                       on a.link_id = b.link_id
                    ) as result1 
                    where result1.linkid2 is not null
                ) as result 
                where result.pos_cond_speed <> 0;

                """
        
        if self.pg.execute(sqlcmd) == -1:
            return 0
        else:
            if self.pg.getcnt() == 0:
                return 1
            else:
                return 0
            
class CCheckPosSpeedRationality2(platform.TestCase.CTestCase):  
    def _do(self):

            sqlcmd = """
                select val1,val2,val3 from (                
                    select  1 as id, count(*) as val1  from rdb_cond_speed where pos_cond_speed = 0 
                ) as tbl1
                inner join 
                ( 
                    select 1 as id, count(*) as val2 from  rdb_cond_speed where pos_cond_speed between 30 and 60 
                ) as tbl2
                on tbl1.id=tbl2.id 
                inner join
                ( 
                    select 1 as id, count(*) as val3 from  rdb_cond_speed where pos_cond_speed between 100 and 160 
                ) as tbl3
                on tbl1.id=tbl3.id         
                """

            self.pg.execute(sqlcmd)
            row = self.pg.fetchone()
            if row:
                if row[0] <> 0 and row[1] <> 0 and row[2] <> 0:
                    return True
                if row[0] == 0 and row[1] == 0 and row[2] == 0:
                    self.logger.warning('***Positive Speed Record is None.***')
                    return True
            return False

class CCheckPosSpeedRationality2_RDF_SGP(platform.TestCase.CTestCase):  
    def _do(self):

            sqlcmd = """
                select val1,val2 from (                
                    select  1 as id, count(*) as val1  from rdb_cond_speed where pos_cond_speed = 0 
                ) as tbl1
                inner join 
                ( 
                    select 1 as id, count(*) as val2 from  rdb_cond_speed where pos_cond_speed between 30 and 60 
                ) as tbl2
                on tbl1.id=tbl2.id     
                """

            self.pg.execute(sqlcmd)
            row = self.pg.fetchone()
            if row:
                if row[0] <> 0 and row[1] <> 0:
                    return True
                if row[0] == 0 and row[1] == 0:
                    self.logger.warning('***Positive Speed Record is None.***')
                    return True
            return False
        
class CCheckPosSpeedRationality2_MMI(platform.TestCase.CTestCase):  
    def _do(self):

            sqlcmd = """
                select val1,val2,val3 from (                
                    select  1 as id, count(*) as val1  from rdb_cond_speed where pos_cond_speed = 0 
                ) as tbl1
                inner join 
                ( 
                    select 1 as id, count(*) as val2 from  rdb_cond_speed where pos_cond_speed between 30 and 60 
                ) as tbl2
                on tbl1.id=tbl2.id 
                inner join
                ( 
                    select 1 as id, count(*) as val3 from  rdb_cond_speed where pos_cond_speed between 100 and 160 
                ) as tbl3
                on tbl1.id=tbl3.id         
                """

            self.pg.execute(sqlcmd)
            row = self.pg.fetchone()
            if row:
                if row[1] <> 0 and row[2] <> 0:
                    return True
                if row[0] == 0 and row[1] == 0 and row[2] == 0:
                    self.logger.warning('***Positive Speed Record is None.***')
                    return True
            return False
        
class CCheckPosSpeedRationality2_Nostra(platform.TestCase.CTestCase):  
    def _do(self):

            sqlcmd = """
                select val1,val2,val3 from (                
                    select  1 as id, count(*) as val1  from rdb_cond_speed where pos_cond_speed = 0 
                ) as tbl1
                inner join 
                ( 
                    select 1 as id, count(*) as val2 from  rdb_cond_speed where pos_cond_speed between 30 and 60 
                ) as tbl2
                on tbl1.id=tbl2.id 
                inner join
                ( 
                    select 1 as id, count(*) as val3 from  rdb_cond_speed where pos_cond_speed between 100 and 160 
                ) as tbl3
                on tbl1.id=tbl3.id         
                """

            self.pg.execute(sqlcmd)
            row = self.pg.fetchone()
            if row:
                if row[1] <> 0 and row[2] <> 0:
                    return True
                if row[0] == 0 and row[1] == 0 and row[2] == 0:
                    self.logger.warning('***Positive Speed Record is None.***')
                    return True
            return False
        
class CCheckPosSpeedRationality2_Zenrin(platform.TestCase.CTestCase):  
    def _do(self):

            sqlcmd = """
                select val1,val2,val3 from (                
                    select  1 as id, count(*) as val1  from rdb_cond_speed where pos_cond_speed = 0 
                ) as tbl1
                inner join 
                ( 
                    select 1 as id, count(*) as val2 from  rdb_cond_speed where pos_cond_speed between 30 and 60 
                ) as tbl2
                on tbl1.id=tbl2.id 
                inner join
                ( 
                    select 1 as id, count(*) as val3 from  rdb_cond_speed where pos_cond_speed between 100 and 160 
                ) as tbl3
                on tbl1.id=tbl3.id         
                """

            self.pg.execute(sqlcmd)
            row = self.pg.fetchone()
            if row:
                if row[1] <> 0 and row[2] <> 0:
                    return True
                if row[0] == 0 and row[1] == 0 and row[2] == 0:
                    self.logger.warning('***Positive Speed Record is None.***')
                    return True
            return False
                
class CCheckNegSpeedValid(platform.TestCase.CTestCase):  
    def _do(self):
        
        sqlcmd = """
                select * from rdb_cond_speed 
                where neg_cond_speed <0 or pos_cond_speed >160
                """
        
        if self.pg.execute(sqlcmd) == -1:
            return 0
        else:
            if self.pg.getcnt() == 0:
                return 1
            else:
                return 0

class CCheckNegSpeedValid_RDF(platform.TestCase.CTestCase):  
    def _do(self):
        
        sqlcmd = """
                select * from rdb_cond_speed 
                where neg_cond_speed < -1 or pos_cond_speed >160
                """
        
        if self.pg.execute(sqlcmd) == -1:
            return 0
        else:
            if self.pg.getcnt() == 0:
                return 1
            else:
                return 0
                        
class CCheckNegSpeedRationality1(platform.TestCase.CTestCase):  
    def _do(self):
        
        sqlcmd = """
                select * from (
                    select * from (
                       select  a.link_id as linkid1,a.neg_cond_speed,b.link_id as linkid2  
                       from rdb_cond_speed as a
                       left join 
                       (select link_id from rdb_link where one_way not in (1,3) ) as b
                       on a.link_id = b.link_id
                    ) as result1 
                    where result1.linkid2 is not null
                ) as result 
                where result.neg_cond_speed <> 0;   
                          
                """
        
        if self.pg.execute(sqlcmd) == -1:
            return 0
        else:
            if self.pg.getcnt() == 0:
                return 1
            else:
                return 0
            
class CCheckNegSpeedRationality2(platform.TestCase.CTestCase):  
    def _do(self):

            sqlcmd = """
                select val1,val2,val3 from (                
                    select  1 as id, count(*) as val1  from rdb_cond_speed where neg_cond_speed = 0 
                ) as tbl1
                inner join 
                ( 
                    select 1 as id, count(*) as val2 from  rdb_cond_speed where neg_cond_speed between 30 and 60 
                ) as tbl2
                on tbl1.id=tbl2.id 
                inner join
                ( 
                    select 1 as id, count(*) as val3 from  rdb_cond_speed where neg_cond_speed between 100 and 160 
                ) as tbl3
                on tbl1.id=tbl3.id         
                """

            self.pg.execute(sqlcmd)
            row = self.pg.fetchone()
            if row:
                if row[0] <> 0 and row[1] <> 0 and row[2] <> 0:
                    return True
                
                if row[0] == 0 and row[1] == 0 and row[2] == 0:
                    self.logger.warning('***Negative Speed Record is None.***')
                    return True
            return False

class CCheckNegSpeedRationality2_RDF_SGP(platform.TestCase.CTestCase):  
    def _do(self):

            sqlcmd = """
                select val1,val2 from (                
                    select  1 as id, count(*) as val1  from rdb_cond_speed where neg_cond_speed = 0 
                ) as tbl1
                inner join 
                ( 
                    select 1 as id, count(*) as val2 from  rdb_cond_speed where neg_cond_speed between 30 and 60 
                ) as tbl2
                on tbl1.id=tbl2.id         
                """

            self.pg.execute(sqlcmd)
            row = self.pg.fetchone()
            if row:
                if row[0] <> 0 and row[1] <> 0 :
                    return True
                
                if row[0] == 0 and row[1] == 0 :
                    self.logger.warning('***Negative Speed Record is None.***')
                    return True
            return False

class CCheckNegSpeedRationality2_MMI(platform.TestCase.CTestCase):  
    def _do(self):

            sqlcmd = """
                select val1,val2,val3 from (                
                    select  1 as id, count(*) as val1  from rdb_cond_speed where pos_cond_speed = 0 
                ) as tbl1
                inner join 
                ( 
                    select 1 as id, count(*) as val2 from  rdb_cond_speed where pos_cond_speed between 30 and 60 
                ) as tbl2
                on tbl1.id=tbl2.id 
                inner join
                ( 
                    select 1 as id, count(*) as val3 from  rdb_cond_speed where pos_cond_speed between 100 and 160 
                ) as tbl3
                on tbl1.id=tbl3.id         
                """

            self.pg.execute(sqlcmd)
            row = self.pg.fetchone()
            if row:
                if row[1] <> 0 and row[2] <> 0:
                    return True
                if row[0] == 0 and row[1] == 0 and row[2] == 0:
                    self.logger.warning('***Negative Speed Record is None.***')
                    return True
            return False
        
class CCheckNegSpeedRationality2_Nostra(platform.TestCase.CTestCase):  
    def _do(self):

            sqlcmd = """
                select val1,val2,val3 from (                
                    select  1 as id, count(*) as val1  from rdb_cond_speed where pos_cond_speed = 0 
                ) as tbl1
                inner join 
                ( 
                    select 1 as id, count(*) as val2 from  rdb_cond_speed where pos_cond_speed between 30 and 60 
                ) as tbl2
                on tbl1.id=tbl2.id 
                inner join
                ( 
                    select 1 as id, count(*) as val3 from  rdb_cond_speed where pos_cond_speed between 100 and 160 
                ) as tbl3
                on tbl1.id=tbl3.id         
                """

            self.pg.execute(sqlcmd)
            row = self.pg.fetchone()
            if row:
                if row[1] <> 0 and row[2] <> 0:
                    return True
                if row[0] == 0 and row[1] == 0 and row[2] == 0:
                    self.logger.warning('***Negative Speed Record is None.***')
                    return True
            return False
        
class CCheckNegSpeedRationality2_Zenrin(platform.TestCase.CTestCase):  
    def _do(self):

            sqlcmd = """
                select val1,val2,val3 from (                
                    select  1 as id, count(*) as val1  from rdb_cond_speed where pos_cond_speed = 0 
                ) as tbl1
                inner join 
                ( 
                    select 1 as id, count(*) as val2 from  rdb_cond_speed where pos_cond_speed between 30 and 60 
                ) as tbl2
                on tbl1.id=tbl2.id 
                inner join
                ( 
                    select 1 as id, count(*) as val3 from  rdb_cond_speed where pos_cond_speed between 100 and 160 
                ) as tbl3
                on tbl1.id=tbl3.id         
                """

            self.pg.execute(sqlcmd)
            row = self.pg.fetchone()
            if row:
                if row[0] <> 0 and row[1] <> 0:
                    return True
                if row[0] == 0 and row[1] == 0 and row[2] == 0:
                    self.logger.warning('***Negative Speed Record is None.***')
                    return True
            return False
                
class CCheckUnitValid(platform.TestCase.CTestCase):  
    def _do(self):
        
        sqlcmd = """
                select  *  from rdb_cond_speed where unit not in (0,1);   
                          
                """
        
        if self.pg.execute(sqlcmd) == -1:
            return 0
        else:
            if self.pg.getcnt() == 0:
                return 1
            else:
                return 0

class CCheckPosSpeedsourceValid(platform.TestCase.CTestCase):
    def _do(self):

        sqlcmd = """
            select count(*) from rdb_cond_speed 
            where pos_cond_speed_source not in (0,1,2,3);
                """
        return 0 == self.pg.getOnlyQueryResult(sqlcmd)            

class CCheckNegSpeedsourceValid(platform.TestCase.CTestCase):
    def _do(self):

        sqlcmd = """
            select count(*) from rdb_cond_speed 
            where neg_cond_speed_source not in (0,1,2,3);
                """
        return 0 == self.pg.getOnlyQueryResult(sqlcmd) 
    