
import platform.TestCase

class CCheckTime(platform.TestCase.CTestCase):

    def _do(self):
        
        sqlcmd = """
            select count(*) from rdb_forecast_time where time < 0;
            """
        
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)
      
class CCheckTimeSlot(platform.TestCase.CTestCase):

    def _do(self):
        
        sqlcmd = """
            select count(*) from rdb_forecast_time where mod(time_slot,3) != 0;
            """
        
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)
    
class CCheckWeatherType(platform.TestCase.CTestCase):

    def _do(self):
        
        sqlcmd = """
            select count(*) from rdb_forecast_control where weather_type != 0;
            """
        
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)    
    
class CCheckDayType(platform.TestCase.CTestCase):

    def _do(self):
        
        sqlcmd = """
             select count(*) from rdb_forecast_control where day_type != 0;
            """
        
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)      

class CCheckStartEndDay(platform.TestCase.CTestCase):

    def _do(self):
        
        sqlcmd = """
            select count(*) from rdb_forecast_control 
            where start_day != 0 or end_day != 0;
            """
        
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)    
    
class CCheckTimeID_Weekday(platform.TestCase.CTestCase):

    def _do(self):
        
        sqlcmd = """
            select count(*) from rdb_forecast_control a
            left join (
                select distinct time_id from rdb_forecast_time 
            )b
            on a.time_id_weekday = b.time_id
            where a.time_id_weekday != 0 and b.time_id is null;
            """
            
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)       
    
class CCheckTimeIDValid_Weekday(platform.TestCase.CTestCase):

    def _do(self):
        
        sqlcmd = """
            select count(*) from rdb_forecast_control 
            where time_id_weekday = 0 or time_id_weekday is null;
            """
            
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0) 

class CCheckTimeIDValid_Weekend(platform.TestCase.CTestCase):

    def _do(self):
        
        sqlcmd = """
            select count(*) from rdb_forecast_control 
            where time_id_weekend is null;
            """
            
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)
            
class CCheckTimeID_Weekend(platform.TestCase.CTestCase):

    def _do(self):
        
        sqlcmd = """
            select count(*) from rdb_forecast_control a
            left join (
                select distinct time_id from rdb_forecast_time 
            )b
            on a.time_id_weekend = b.time_id
            where a.time_id_weekend != 0 and b.time_id is null;
            """
            
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0) 
        
class CCheckInfoID(platform.TestCase.CTestCase):

    def _do(self):
        
        sqlcmd = """
            select count(*) from rdb_forecast_link 
            where info_id = 0 and free_time is not null;
            """
            
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)

class CCheckFreeTimeValid_1(platform.TestCase.CTestCase):

    def _do(self):
        
        sqlcmd = """
            select count(*) from rdb_forecast_link 
            where free_time is not null and free_time > weekday_time;            
            """
            
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0) 
     
class CCheckFreeTimeValid_2(platform.TestCase.CTestCase):

    def _do(self):
        
        sqlcmd = """
            select count(*) from rdb_forecast_link 
            where free_time is not null and free_time > weekend_time; 
            """
            
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)  

class CCheckFreeTimeValid_3(platform.TestCase.CTestCase):

    def _do(self):
        
        sqlcmd = """
            select count(*) from rdb_forecast_link 
            where free_time is not null and free_time > average_time;
            """
            
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0) 
    
class CCheckFreeTimeValue(platform.TestCase.CTestCase):

    def _do(self):
        
        sqlcmd = """
            select count(*) from rdb_forecast_link 
            where free_time < 0 or free_time > 65535;
            """
            
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)     
    
class CCheckDir(platform.TestCase.CTestCase):

    def _do(self):
        
        sqlcmd = """
            select count(*) from rdb_forecast_link 
            where dir not in (0,1);
            """
            
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)      
    
class CCheckWeekdayTimeValue(platform.TestCase.CTestCase):

    def _do(self):
        
        sqlcmd = """
            select count(*) from rdb_forecast_link 
            where weekday_time < 0 or weekday_time > 65535;
            """
            
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)    
    
class CCheckWeekendTimeValue(platform.TestCase.CTestCase):

    def _do(self):
        
        sqlcmd = """
            select count(*) from rdb_forecast_link 
            where weekend_time < 0 or weekend_time > 65535;            
            """
            
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)    
     
class CCheckAverageTimeValue(platform.TestCase.CTestCase):

    def _do(self):
        
        sqlcmd = """
            select count(*) from rdb_forecast_link 
            where average_time < 0 or average_time > 65535;                                                                              
            """
            
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)
    
class CCheckInfoID_Fkey(platform.TestCase.CTestCase):

    def _do(self):
        
        sqlcmd = """
            select count(*) from rdb_forecast_link a
            left join rdb_forecast_control b
            on a.info_id = b.info_id
            where a.info_id != 0 and b.info_id is null;
            """
            
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0) 

class CCheckInfoIDValid(platform.TestCase.CTestCase):

    def _do(self):
        
        sqlcmd = """
            select count(*) from rdb_forecast_link
            where info_id is null;
            """
            
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)       
                                 