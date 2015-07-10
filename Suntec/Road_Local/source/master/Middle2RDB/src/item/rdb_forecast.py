# -*- coding: cp936 -*-


from base.rdb_ItemBase import ItemBase
from common import rdb_log
from common import rdb_common

class rdb_forecast(ItemBase):
    '''Traffic Forecast Data.
    '''

    @staticmethod
    def instance():
        proj_mapping = {
            ('ta','aus'):           rdb_forecast(),
            ('jpn'):                rdb_forecast(),
            ('jdb'):                rdb_forecast(),
            ('axf'):                rdb_forecast(),
            ('rdf'):                rdb_forecast(),
            ('rdf','sgp'):          rdb_forecast(),
            ('rdf','uc'):           rdb_forecast(),
            ('rdf','me8'):          rdb_forecast(),
            ('rdf','mea'):          rdb_forecast(),
            ('rdf','bra'):          rdb_forecast(),            
            ('nostra'):             rdb_forecast(),
            ('mmi'):                rdb_forecast(),
            ('msm'):                rdb_forecast(),
            ('ni'):                 rdb_forecast(),            
        }
        return rdb_common.getItem(proj_mapping)

    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self, 'Traffic Forecast Dummy')

    def Do_CreateTable(self):
        
        self.CreateTable2('rdb_forecast_link')
        self.CreateTable2('rdb_forecast_control')
        self.CreateTable2('rdb_forecast_time')
        self.CreateTable2('rdb_region_forecast_link_layer4')
        self.CreateTable2('rdb_region_forecast_link_layer6')
        self.CreateTable2('rdb_region_forecast_control_layer4')
        self.CreateTable2('rdb_region_forecast_control_layer6')
        self.CreateTable2('rdb_region_forecast_time_layer4')
        self.CreateTable2('rdb_region_forecast_time_layer6')                
        
class rdb_forecast_aus(ItemBase):
    
    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self, 'Traffic Forecast')
            
    def Do_CreateTable(self):

        self.CreateTable2('rdb_forecast_link')
        self.CreateTable2('rdb_forecast_control')
        self.CreateTable2('rdb_forecast_time')
        self.CreateTable2('rdb_region_forecast_link_layer4')
        self.CreateTable2('rdb_region_forecast_link_layer6')
        self.CreateTable2('rdb_region_forecast_control_layer4')
        self.CreateTable2('rdb_region_forecast_control_layer6')
        self.CreateTable2('rdb_region_forecast_time_layer4')
        self.CreateTable2('rdb_region_forecast_time_layer6')  
                    
    def Do(self):  
                  
        self.CreateIndex2('org_hsnp_network_id_idx1')
        self.CreateIndex2('org_hspr_forecast_id_idx')
        self.CreateIndex2('org_hspr_time_slot_idx')
        
        # Create temp tables.
        self._prepareForecast()
        
        # Links without periodic data.
        self._createForecastWithoutSlot()
        
        # Links with periodic data.
        self._createForecastWithSlot()
        
        self._createForecastRegion()

    def Do_CreatIndex(self):
        self.CreateIndex2('rdb_forecast_link_link_id_idx1')
        self.CreateIndex2('rdb_forecast_control_info_id_idx1')
        self.CreateIndex2('rdb_forecast_time_time_id_idx1')
        self.CreateIndex2('rdb_forecast_time_time_slot_idx1')
            
    def _prepareForecast(self):
        
        rdb_log.log(self.ItemName, 'preparing ----- start', 'info') 
        # ORG2RDB link with length.
        self.CreateTable2('temp_link_with_length')
        self.CreateIndex2('temp_link_with_length_org_link_id_idx')
        self.CreateIndex2('temp_link_with_length_rdb_link_id_idx')
        self.CreateIndex2('temp_link_with_length_s_fraction_idx')
        self.CreateIndex2('temp_link_with_length_e_fraction_idx')
        
        # Convert base speeds to travel time.
        self.CreateTable2('temp_forecast_link')
        self.CreateIndex2('temp_forecast_link_link_id_idx')
        self.CreateIndex2('temp_forecast_link_profile_flag_idx')

        # RDB links with periodic data.
        self.CreateTable2('temp_forecast_link_rdb_id')
        self.CreateIndex2('temp_forecast_link_rdb_id_link_id_idx')
        self.CreateIndex2('temp_forecast_link_rdb_id_dir_idx')
        
        # Create time slot table on a 15-min circle.
        self.CreateTable2('temp_forecast_time_org')
        self.CreateIndex2('temp_forecast_time_org_forecast_id_idx1')
        self.CreateIndex2('temp_forecast_time_org_time_slot_idx1')
        
        rdb_log.log(self.ItemName, 'preparing ----- end', 'info') 
                               
    def _createForecastWithoutSlot(self): 
        
        rdb_log.log(self.ItemName, 'creating links without slots ----- start', 'info')  
        # Insert into mapping table: ORG link was splitted into intact RDB links.
        sqlcmd = """
            insert into rdb_forecast_link(link_id,dir,weekday_time,weekend_time,average_time,type)
            select rdb_link_id
                ,case when flag = 't' and dir = 2 then 1
                    when flag = 't' and dir = 3 then 0
                    else dir - 2
                end as dir
                ,round((mid_meters/meters) * weekday_time) as weekday_time
                ,round((mid_meters/meters) * weekend_time) as weekend_time
                ,round((mid_meters/meters) * average_time) as average_time 
                ,1
            from ( 
                select a.*,b.rdb_link_id,b.s_fraction,b.e_fraction,b.flag,mid_meters
                from temp_forecast_link a
                left join temp_link_with_length b
                on a.link_id = b.org_link_id
                where a.profile_flag = 'f'
                and (b.s_fraction = 0 and b.e_fraction = 1) 
            ) c;
                          
            """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        # Insert into mapping table: ORG link was splitted into part of RDB links.
        sqlcmd = """
            insert into rdb_forecast_link(link_id,dir,weekday_time,weekend_time,average_time,type)
            select link_id,dir
                ,sum(weekday_time)
                ,sum(weekend_time)
                ,sum(average_time)
                ,2
            from (
                select d.* from (
                    select rdb_link_id as link_id,0 as info_id
                        ,case when flag = 't' and dir = 2 then 1
                            when flag = 't' and dir = 3 then 0
                            else dir - 2
                        end as dir
                        ,round((mid_meters/meters) * weekday_time) as weekday_time
                        ,round((mid_meters/meters) * weekend_time) as weekend_time
                        ,round((mid_meters/meters) * average_time) as average_time
                        ,profile_flag 
                        ,meters,mid_meters
                        ,s_fraction,e_fraction
                    from ( 
                        select a.*,b.rdb_link_id
                            ,s_fraction,e_fraction
                            ,b.flag
                            ,mid_meters
                        from temp_forecast_link a
                        left join temp_link_with_length b
                        on a.link_id = b.org_link_id
                        where profile_flag = 'f' 
                        and not (b.s_fraction = 0 and b.e_fraction = 1) 
                    ) c
                ) d
                left join temp_forecast_link_rdb_id e
                on d.link_id = e.link_id and d.dir = e.dir
                where e.link_id is null
            ) e group by link_id,dir;
                          
            """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
    
        rdb_log.log(self.ItemName, 'creating links without slots ----- end', 'info') 
        
    def _createForecastWithSlot(self):  

        rdb_log.log(self.ItemName, 'creating links with slots ----- start', 'info') 
        # Links with periodic data.
        self.CreateTable2('temp_forecast_link_with_slot')
        self.CreateIndex2('temp_forecast_link_with_slot_link_id_dir_idx1')
        self.CreateIndex2('temp_forecast_link_with_slot_profile_1_idx1')
        self.CreateIndex2('temp_forecast_link_with_slot_profile_2_idx1')
        self.CreateIndex2('temp_forecast_link_with_slot_profile_3_idx1')
        self.CreateIndex2('temp_forecast_link_with_slot_profile_4_idx1')
        self.CreateIndex2('temp_forecast_link_with_slot_profile_5_idx1')
        self.CreateIndex2('temp_forecast_link_with_slot_profile_6_idx1')
        self.CreateIndex2('temp_forecast_link_with_slot_profile_7_idx1')          
                
        # Time slot list.
        self.CreateTable2('temp_forecast_time_slot')
        self.CreateIndex2('temp_forecast_time_slot_time_slot_idx1')

        # Links with periodic data: time slots were unfolded.
        self.CreateTable2('temp_forecast_link_with_slot_main')
        self.CreateIndex2('temp_forecast_link_with_slot_main_link_id_dir_time_slot_idx1')
        
        # Links with periodic data: fractions and time slots were merged.
        self.CreateTable2('temp_forecast_link_with_slot_merge')
        self.CreateIndex2('temp_forecast_link_with_slot_merge_link_id_dir_idx')
        
        # Give a unique ID to time slots.
        self.CreateTable2('temp_forecast_time_distinct')
        # Give a unique ID to control table.
        self.CreateTable2('temp_forecast_control')
        self.CreateIndex2('temp_forecast_control_link_id_dir_idx')
                
        # Insert into time slot table.        
        sqlcmd = """
            insert into rdb_forecast_time(time_id,time_slot,time)
            select time_id
                ,unnest(time_slot_array) as time_slot
                ,unnest(time_array) as time
            from temp_forecast_time_distinct;       
            """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()                               

        # Insert into control table.
        sqlcmd = """
            insert into rdb_forecast_control(info_id,weather_type,day_type,time_id_weekday,time_id_weekend)
            select info_id,0 as weather_type,0 as day_type
                ,time_id_weekday,time_id_weekend
            from temp_forecast_control;
            """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        # Insert into mapping table.
        sqlcmd = """
            insert into rdb_forecast_link(link_id,dir,free_time,weekday_time,weekend_time,average_time,info_id,type)
            select a.link_id
                ,a.dir - 2
                ,a.free_time,a.weekday_time,a.weekend_time
                ,a.average_time
                ,b.info_id
                ,3
            from temp_forecast_link_with_slot_merge a
            left join temp_forecast_control b
            on a.time_slot_array = b.time_slot_array and a.weekday_per_array = b.weekday_per_array
            and a.weekend_per_array = b.weekend_per_array;
            """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        rdb_log.log(self.ItemName, 'creating links with slots ----- end', 'info') 
        
    def _createForecastRegion(self):   
        
        rdb_log.log(self.ItemName, 'creating region ----- start', 'info')
        
        proc_region = rdb_forecast_region(self, rdb_log)
        proc_region.Do('4')
        proc_region.Do('6')
       
        rdb_log.log(self.ItemName, 'creating region ----- end', 'info') 
        
class rdb_forecast_region():
    '''region forecast.
    '''
    def  __init__(self, base, log):
        '''
        Constructor
        '''
        self.item = base
        self.pg = self.item.pg
        rdb_log = log
            
    def Do(self,X):
        
        rdb_log.log('Traffic Forecast', 'creating region %s ----- start'%X, 'info')        
        self._prepareRegion(X)         
        self._createRegionForecastWithoutSlot(X)        
        self._createRegionForecastWithSlot(X)
        
    def _prepareRegion(self, X):
        # Create region link mapping table.
        sqlcmd = """
            drop table if exists temp_region_link_mapping_layer%X;
            create table temp_region_link_mapping_layer%X as
            select region_link_id
                ,unnest(link_id_14) as link_id_14
                ,unnest(link_dir_14) as link_dir
                ,generate_series(1,len) as seq
            from ( 
                select *,array_length(link_id_14,1) as len
                from rdb_region_layer%X_link_mapping
            ) a;
                       
            CREATE INDEX temp_region_link_mapping_layer%X_link_id_14_idx1
              ON temp_region_link_mapping_layer%X
              USING btree
              (link_id_14);                     
        """
        sqlcmd = sqlcmd.replace('%X',X)
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        # Region link with periodic data. 
        sqlcmd = """
            drop table if exists temp_region_forecast_link_id_layer%X;
            create table temp_region_forecast_link_id_layer%X 
            as
            (
                select distinct region_link_id,dir
                from (
                     select a.region_link_id
                        ,case when a.link_dir = 'f' and b.dir = 0 then 1
                            when a.link_dir = 'f' and b.dir = 1 then 0
                            else b.dir
                        end as dir
                     from temp_region_link_mapping_layer%X a 
                     left join rdb_forecast_link b
                     on a.link_id_14 = b.link_id
                     where b.info_id is not null 
                ) c
            );
            
            CREATE INDEX temp_region_forecast_link_id_layer%X_region_link_id_dir_idx
              ON temp_region_forecast_link_id_layer%X
              USING btree
              (region_link_id,dir);                                   
        """
        sqlcmd = sqlcmd.replace('%X',X)
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _createRegionForecastWithoutSlot(self, X):
        
        # Insert into mapping table.
        sqlcmd = """
            insert into rdb_region_forecast_link_layer%X(link_id,dir,weekday_time,weekend_time,average_time,type)
            select region_link_id as link_id
                ,dir
                ,sum(weekday_time) as weekday_time
                ,sum(weekend_time) as weekend_time
                ,sum(average_time) as average_time
                ,2
            from (
                select m.* from (
                    select a.region_link_id,b.link_id
                        ,case when a.link_dir = false and b.dir = 0 then 1
                            when a.link_dir = false and b.dir = 1 then 0
                            else b.dir
                        end as dir
                        ,b.free_time    
                        ,b.weekday_time
                        ,b.weekend_time
                        ,b.average_time
                    from temp_region_link_mapping_layer%X a 
                    left join rdb_forecast_link b
                    on a.link_id_14 = b.link_id
                    where b.link_id is not null
                ) m
                left join temp_region_forecast_link_id_layer%X n
                on m.region_link_id = n.region_link_id and m.dir = n.dir
                where n.region_link_id is null
            ) p group by region_link_id,dir;          
        """
        sqlcmd = sqlcmd.replace('%X',X)
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _createRegionForecastWithSlot(self, X):
        
        # Region links with periodic data: time slots were unfolded.
        sqlcmd = """
            drop table if exists temp_region_forecast_link_main_layer%X;
            create table temp_region_forecast_link_main_layer%X as
            select m.* from (
                select a.region_link_id as link_id
                    ,b.link_id as org_link_id
                    ,case when a.link_dir = false and b.dir = 0 then 1
                        when a.link_dir = false and b.dir = 1 then 0
                        else b.dir
                    end as dir
                    ,case when b.free_time is null and weekday_time <= weekend_time then weekday_time
                        when b.free_time is null and weekday_time >= weekend_time then weekend_time
                        else b.free_time
                    end as free_time    
                    ,b.weekday_time
                    ,b.weekend_time
                    ,b.average_time
                    ,f.time_slot
                    ,case when b.info_id is null then b.weekday_time
                        else (d.time + b.free_time)
                    end as weekday
                    ,case when b.info_id is null then b.weekend_time
                        else (e.time + b.free_time)
                    end as weekend
                from temp_region_link_mapping_layer%X a 
                left join rdb_forecast_link b
                on a.link_id_14 = b.link_id
                left join rdb_forecast_control c
                on b.info_id = c.info_id
                left join temp_forecast_time_slot f
                on true = true
                left join rdb_forecast_time d
                on c.time_id_weekday = d.time_id and d.time_slot = f.time_slot
                left join rdb_forecast_time e
                on c.time_id_weekend = e.time_id and e.time_slot = f.time_slot
                where b.link_id is not null
            ) m
            left join temp_region_forecast_link_id_layer%X n
            on m.link_id = n.region_link_id and m.dir = n.dir
            where n.region_link_id is not null;
            
            CREATE INDEX temp_region_forecast_link_main_layer%X_link_id_dir_time_slot_idx1
              ON temp_region_forecast_link_main_layer%X
              USING btree
              (link_id,dir,time_slot);                               
        """
        sqlcmd = sqlcmd.replace('%X',X)
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        # Region links with periodic data: fractions and time slots were merged.                           
        sqlcmd = """
            drop table if exists temp_region_forecast_link_merge_layer%X;
            create table temp_region_forecast_link_merge_layer%X as    
            (
                select link_id,dir,free_time,weekday_time,weekend_time,average_time
                    ,array_agg(time_slot) as time_slot_array
                    ,array_agg(weekend_per) as weekend_per_array
                    ,array_agg(weekday_per) as weekday_per_array
                from (
                    select link_id,dir,time_slot
                        ,free_time,weekday_time,weekend_time,average_time
                        ,case when round(weekend - free_time) >= 0 then round(weekend - free_time)
                            else null
                        end as weekend_per
                        ,case when round(weekday - free_time) >= 0 then round(weekday - free_time)
                            else null
                        end as weekday_per
                    from (
                        select link_id,dir,time_slot
                            ,sum(free_time) as free_time
                            ,sum(weekday_time) as weekday_time
                            ,sum(weekend_time) as weekend_time
                            ,sum(average_time) as average_time
                            ,sum(weekend) as weekend
                            ,sum(weekday) as weekday
                        from temp_region_forecast_link_main_layer%X 
                        group by link_id,dir,time_slot
                    ) e order by link_id,dir,free_time,weekday_time,weekend_time,average_time,time_slot
                ) f group by link_id,dir,free_time,weekday_time,weekend_time,average_time
            );  
            
            CREATE INDEX temp_region_forecast_link_merge_layer%X_time_slot_array_weekday_per_array_idx1
              ON temp_region_forecast_link_merge_layer%X
              USING btree
              (time_slot_array,weekday_per_array); 

            CREATE INDEX temp_region_forecast_link_merge_layer%X_link_id_dir_idx1
              ON temp_region_forecast_link_merge_layer%X
              USING btree
              (link_id,dir);                                                     
        """
        sqlcmd = sqlcmd.replace('%X',X)
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        # Create temp table. 
        sqlcmd = """
            drop table if exists temp_region_forecast_time_distinct_layer%X;
            create table temp_region_forecast_time_distinct_layer%X
            as
            (
                select ROW_NUMBER() OVER(ORDER BY time_slot_array,time_array) as time_id
                    ,time_slot_array,time_array
                from (
                    select distinct time_slot_array,weekend_per_array as time_array
                    from temp_region_forecast_link_merge_layer%X 
                    where weekend_per_array[1] is not null
                    union
                    select distinct time_slot_array,weekday_per_array as time_array
                    from temp_region_forecast_link_merge_layer%X 
                    where weekday_per_array[1] is not null
                ) a
            );
            
            drop table if exists temp_region_forecast_control_layer%X;
            create table temp_region_forecast_control_layer%X 
            as
            (
                select ROW_NUMBER() OVER(ORDER BY time_slot_array,weekday_per_array,weekend_per_array) as info_id,*
                from (
                    select distinct a.time_slot_array
                        ,a.weekday_per_array
                        ,a.weekend_per_array
                        ,b.time_id as time_id_weekday
                        ,c.time_id as time_id_weekend
                    from temp_region_forecast_link_merge_layer%X a
                    left join temp_region_forecast_time_distinct_layer%X b
                    on a.time_slot_array = b.time_slot_array and a.weekday_per_array = b.time_array
                    left join temp_region_forecast_time_distinct_layer%X c
                    on a.time_slot_array = c.time_slot_array and a.weekend_per_array = c.time_array
                ) d
            );

            CREATE INDEX temp_region_forecast_control_layer%X_link_id_dir_idx1
              ON temp_region_forecast_control_layer%X
              USING btree
              (link_id,dir);             
        """
        sqlcmd = sqlcmd.replace('%X',X)
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        # Insert into RDB tables.
        sqlcmd = """
            insert into rdb_region_forecast_time_layer%X(time_id,time_slot,time)
            select time_id
                ,unnest(time_slot_array) as time_slot
                ,unnest(time_array) as time
            from temp_region_forecast_time_distinct_layer%X;   

            insert into rdb_region_forecast_control_layer%X(info_id,weather_type,day_type,time_id_weekday,time_id_weekend)
            select info_id,weather_type,day_type,time_id_weekday,time_id_weekend
            from temp_region_forecast_control_layer%X;
                       
            insert into rdb_region_forecast_link_layer%X(link_id,dir,free_time,weekday_time,weekend_time,average_time,info_id,type)
            select a.link_id
                ,a.dir
                ,a.free_time,a.weekday_time,a.weekend_time
                ,a.average_time
                ,b.info_id
                ,3
            from temp_region_forecast_link_merge_layer%X a
            left join temp_region_forecast_control_layer%X b
            on a.time_slot_array = b.time_slot_array and a.weekday_per_array = b.weekday_per_array
            and a.weekend_per_array = b.weekend_per_array;
        """
        sqlcmd = sqlcmd.replace('%X',X)
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
                     
        
        
        