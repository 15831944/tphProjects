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
            ('default'):            rdb_forecast(),
            ('ta','aus'):           rdb_forecast_aus(),
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
        pass
                        
    def Do(self):  
        
        self.CreateIndex2('org_hsnp_network_id_idx1')
        self.CreateIndex2('org_hspr_forecast_id_idx')
        self.CreateIndex2('org_hspr_time_slot_idx')

        self._createForecast_14()

        self._createForecast_region('4')
        self._createForecast_region('6')
        if self.pg.IsExistTable('rdb_region_link_layer8_tbl'):
            self._createForecast_region('8')

        self._insertRDBTables()

    def _createForecast_14(self):
        self._prepareForecast_14()        
        # Links without periodic data.
        self._createForecastWithoutSlot_14()        
        # Links with periodic data.
        self._createForecastWithSlot_14()

    def _createForecast_region(self, X):
        self._prepareForecast_region(X) 
            
        # Region Links without periodic data.   
        self._createForecastWithoutSlot_region(X)          
        # Region Links with periodic data.           
        self._createForecastWithSlot_region(X)        
     
        
    def _prepareForecast_14(self):
        
        rdb_log.log(self.ItemName, 'preparing base data ----- start', 'info') 
        # ORG2RDB link with length.
        self.CreateTable2('temp_link_with_length')
        self.CreateIndex2('temp_link_with_length_org_link_id_idx')
        self.CreateIndex2('temp_link_with_length_s_fraction_idx')
        self.CreateIndex2('temp_link_with_length_e_fraction_idx')
        
        sqlcmd = """
            analyze org_hsnp;    
            analyze org_nw;
            """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
                
        # Convert base speeds to travel time.
        self.CreateTable2('temp_forecast_link_org')
        self.CreateIndex2('temp_forecast_link_org_link_id_idx')
        self.CreateIndex2('temp_forecast_link_org_profile_flag_idx')
                
        # Create time slot table on a 15-min circle.
        self.CreateTable2('temp_forecast_time_org')
        self.CreateIndex2('temp_forecast_time_org_profile_id_idx1')
        self.CreateIndex2('temp_forecast_time_org_time_slot_idx1')
        self.CreateIndex2('temp_forecast_time_org_profile_id_time_slot_idx1')

        # Time slot list.
        self.CreateTable2('temp_forecast_time_slot')
        self.CreateIndex2('temp_forecast_time_slot_time_slot_idx1')
        
        # RDB links with periodic data.
        self.CreateTable2('temp_forecast_link_rdb_id')
        self.CreateIndex2('temp_forecast_link_rdb_id_link_id_idx')
        self.CreateIndex2('temp_forecast_link_rdb_id_dir_idx')
                
        rdb_log.log(self.ItemName, 'preparing base data ----- end', 'info') 
                               
    def _createForecastWithoutSlot_14(self): 
        
        rdb_log.log(self.ItemName, 'creating base data without slots ----- start', 'info')  
        # Insert into mapping table: ORG link was splitted into intact RDB links.
        sqlcmd = """
            insert into rdb_forecast_link(link_id,link_id_t,dir,weekday_time,weekend_time,average_time,type)
            select rdb_link_id
                ,(rdb_link_id >> 32)
                ,case when flag = 't' and dir = 0 then 1
                    when flag = 't' and dir = 1 then 0
                    else dir
                end as dir
                ,round((mid_meters/meters) * weekday_time) as weekday_time
                ,round((mid_meters/meters) * weekend_time) as weekend_time
                ,round((mid_meters/meters) * average_time) as average_time
                ,11
            from ( 
                select a.*,b.rdb_link_id,b.s_fraction,b.e_fraction,b.flag,mid_meters
                from temp_forecast_link_org a
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
            insert into rdb_forecast_link(link_id,link_id_t,dir,weekday_time,weekend_time,average_time,type)
            select link_id,(link_id >> 32),dir
                ,sum(weekday_time)
                ,sum(weekend_time)
                ,sum(average_time)
                ,12
            from (
                select d.* from (
                    select rdb_link_id as link_id,0 as info_id
                        ,case when flag = 't' and dir = 0 then 1
                            when flag = 't' and dir = 1 then 0
                            else dir
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
                        from temp_forecast_link_org a
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
    
        rdb_log.log(self.ItemName, 'creating base data without slots ----- end', 'info') 
        
    def _createForecastWithSlot_14(self):  

        rdb_log.log(self.ItemName, 'creating base data with slots ----- start', 'info') 
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
        self.CreateIndex2('temp_forecast_link_with_slot_tile_id_idx1') 

        # Links with periodic data: time slots were unfolded.
        sqlcmd = """
            analyze temp_forecast_link_with_slot;
            analyze temp_forecast_time_org;
            analyze temp_forecast_time_slot;
            """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()            

        sqlcmd = """
            drop table if exists temp_forecast_link_with_slot_merge;
            CREATE TABLE temp_forecast_link_with_slot_merge
            (
              link_id bigint,
              dir smallint,
              free_time smallint,
              weekday_time smallint,
              weekend_time smallint,
              average_time smallint,
              time_slot_array smallint[],
              weekday_diff_array smallint[],
              weekend_diff_array smallint[]
            ); 
            """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        sqlcmd = """          
            insert into temp_forecast_link_with_slot_merge(
                link_id, dir, free_time, weekday_time, weekend_time, average_time
                , time_slot_array, weekday_diff_array, weekend_diff_array)
            select link_id,dir
                ,free_time
                ,weekday_time
                ,weekend_time
                ,average_time
                ,array_agg(time_slot) as time_slot_array 
                ,array_agg(weekday_sum - free_time) as weekday_diff_array
                ,array_agg(weekend_sum - free_time) as weekend_diff_array
            from (
                select link_id,dir
                    ,sum(free_time) as free_time
                    ,sum(weekday_time) as weekday_time
                    ,sum(weekend_time) as weekend_time
                    ,sum(average_time) as average_time
                    ,time_slot
                    ,sum(weekday) as weekday_sum
                    ,sum(weekend) as weekend_sum
                from (
                    select link_id,tile_id,org_link_id,dir,profile_flag
                        ,case when free_time is null and weekday_time <= weekend_time then weekday_time
                            when free_time is null and weekday_time >= weekend_time then weekend_time
                            else free_time
                        end as free_time
                        ,weekday_time,weekend_time,average_time
                        ,s_fraction
                        ,e_fraction
                        ,time_slot
                        ,case when time_1 is null or time_7 is null then weekend_time
                            else round((time_1 + time_7) / 2)
                        end as weekend
                        ,case when time_2 is null then weekday_time
                            else round((time_2 + time_3 + time_4 + time_5 + time_6) / 5) 
                        end as weekday
                    from (
                        select link_id,tile_id,org_link_id,dir,profile_flag
                            ,free_time,weekday_time,weekend_time,average_time,s_fraction,e_fraction
                            ,b.time_slot
                            ,case when b1.profile_id is not null then (a.free_time * b1.time / 100)
                                else null
                            end as time_1
                            ,case when b2.profile_id is not null then (a.free_time * b2.time / 100)
                                else null
                            end as time_2
                            ,case when b3.profile_id is not null then (a.free_time * b3.time / 100)
                                else null
                            end as time_3
                            ,case when b4.profile_id is not null then (a.free_time * b4.time / 100)
                                else null
                            end as time_4
                            ,case when b5.profile_id is not null then (a.free_time * b5.time / 100)
                                else null
                            end as time_5
                            ,case when b6.profile_id is not null then (a.free_time * b6.time / 100)
                                else null
                            end as time_6
                            ,case when b7.profile_id is not null then (a.free_time * b7.time / 100)
                                else null
                            end as time_7
                        from temp_forecast_link_with_slot a
                        left join temp_forecast_time_slot b
                        on true = true        
                        left join temp_forecast_time_org b1 
                        on a.profile_1 = b1.profile_id and b.time_slot = b1.time_slot
                        left join temp_forecast_time_org b2
                        on a.profile_2 = b2.profile_id and b.time_slot = b2.time_slot
                        left join temp_forecast_time_org b3 
                        on a.profile_3 = b3.profile_id and b.time_slot = b3.time_slot
                        left join temp_forecast_time_org b4
                        on a.profile_4 = b4.profile_id and b.time_slot = b4.time_slot
                        left join temp_forecast_time_org b5
                        on a.profile_5 = b5.profile_id and b.time_slot = b5.time_slot
                        left join temp_forecast_time_org b6
                        on a.profile_6 = b6.profile_id and b.time_slot = b6.time_slot
                        left join temp_forecast_time_org b7 
                        on a.profile_7 = b7.profile_id and b.time_slot = b7.time_slot
                        where id >= %s and id <= %s   
                    ) c   
                    order by link_id,dir,time_slot,s_fraction
                ) a
                group by link_id,dir,time_slot
                order by link_id,dir,free_time,weekday_time,weekend_time,average_time,time_slot
            ) b
            group by link_id,dir,free_time,weekday_time,weekend_time,average_time;    
            """
        
        (self.min_link, self.max_link) = self.pg.getMinMaxValue('temp_forecast_link_with_slot', 'id')        
        self.pg.multi_execute(sqlcmd, self.min_link, self.max_link, 3, 1000, self.log)
                
        self.CreateTable2('temp_forecast_link_with_slot_merge_link_id_idx')        
        self.CreateTable2('temp_forecast_link_with_slot_merge_link_id_dir_idx')
        
        rdb_log.log(self.ItemName, 'creating base data with slots ----- end', 'info')

    def _createForecastWithSlot_14_bak(self):  

        rdb_log.log(self.ItemName, 'creating base data with slots ----- start', 'info') 
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
        self.CreateIndex2('temp_forecast_link_with_slot_tile_id_idx1') 

        # Links with periodic data: time slots were unfolded.
        self.CreateTable2('temp_forecast_link_with_slot_main')
        sqlcmd = """
            analyze temp_forecast_link_with_slot;
            analyze temp_forecast_time_org;
            analyze temp_forecast_time_slot;
            """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()            
                               
        sqlcmd = """
            insert into temp_forecast_link_with_slot_main(link_id, org_link_id, dir, profile_flag, free_time, weekday_time, 
               weekend_time, average_time, s_fraction, e_fraction, time_slot, weekend, weekday)
            select link_id,org_link_id,dir,profile_flag
                ,case when free_time is null and weekday_time <= weekend_time then weekday_time
                    when free_time is null and weekday_time >= weekend_time then weekend_time
                    else free_time
                end as free_time
                ,weekday_time,weekend_time,average_time
                ,s_fraction
                ,e_fraction
                ,time_slot
                ,case when time_1 is null or time_7 is null then weekend_time
                    else round((time_1 + time_7) / 2)
                end as weekend
                ,case when time_2 is null then weekday_time
                    else round((time_2 + time_3 + time_4 + time_5 + time_6) / 5) 
                end as weekday
            from (
                select link_id,org_link_id,dir,profile_flag
                    ,free_time,weekday_time,weekend_time,average_time,s_fraction,e_fraction
                    ,b.time_slot
                    ,case when b1.profile_id is not null then (a.free_time * b1.time / 100)
                        else null
                    end as time_1
                    ,case when b2.profile_id is not null then (a.free_time * b2.time / 100)
                        else null
                    end as time_2
                    ,case when b3.profile_id is not null then (a.free_time * b3.time / 100)
                        else null
                    end as time_3
                    ,case when b4.profile_id is not null then (a.free_time * b4.time / 100)
                        else null
                    end as time_4
                    ,case when b5.profile_id is not null then (a.free_time * b5.time / 100)
                        else null
                    end as time_5
                    ,case when b6.profile_id is not null then (a.free_time * b6.time / 100)
                        else null
                    end as time_6
                    ,case when b7.profile_id is not null then (a.free_time * b7.time / 100)
                        else null
                    end as time_7
                from temp_forecast_link_with_slot a
                left join temp_forecast_time_slot b
                on true = true        
                left join temp_forecast_time_org b1 
                on a.profile_1 = b1.profile_id and b.time_slot = b1.time_slot
                left join temp_forecast_time_org b2
                on a.profile_2 = b2.profile_id and b.time_slot = b2.time_slot
                left join temp_forecast_time_org b3 
                on a.profile_3 = b3.profile_id and b.time_slot = b3.time_slot
                left join temp_forecast_time_org b4
                on a.profile_4 = b4.profile_id and b.time_slot = b4.time_slot
                left join temp_forecast_time_org b5
                on a.profile_5 = b5.profile_id and b.time_slot = b5.time_slot
                left join temp_forecast_time_org b6
                on a.profile_6 = b6.profile_id and b.time_slot = b6.time_slot
                left join temp_forecast_time_org b7 
                on a.profile_7 = b7.profile_id and b.time_slot = b7.time_slot
                where  (link_id >> 32) >= %s and (link_id >> 32) <= %s   
            ) c;     
            """
      
        (min_tile, max_tile) = self.pg.getMinMaxValue('temp_forecast_link_with_slot', '(link_id >> 32)')
        
        while min_tile <= max_tile:            
            cur_tile = min_tile + 10000            
            self.pg.execute2(sqlcmd, (min_tile, cur_tile))
            min_tile = cur_tile + 1           
        self.pg.commit2()

        sqlcmd = """
            analyze temp_forecast_link_with_slot_main;    
            """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
                
        self.CreateIndex2('temp_forecast_link_with_slot_main_link_id_dir_time_slot_s_fraction_idx1')
        self.CreateIndex2('temp_forecast_link_with_slot_main_tile_id_idx1')

        sqlcmd = """
            drop table if exists temp_forecast_link_with_slot_merge;
            CREATE TABLE temp_forecast_link_with_slot_merge
            (
              link_id bigint,
              dir smallint,
              free_time smallint,
              weekday_time smallint,
              weekend_time smallint,
              average_time smallint,
              time_slot_array smallint[],
              weekday_diff_array smallint[],
              weekend_diff_array smallint[]
            ); 
            """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        # Time is less than 15min.
        sqlcmd = """          
            drop table if exists temp_forecast_link_with_slot_more_than_15min;
            create table temp_forecast_link_with_slot_more_than_15min as
            select distinct link_id,dir from (
                select link_id,dir
                    ,sum(free_time)
                    ,sum(weekday_time)
                    ,sum(weekend_time)
                    ,sum(average_time)
                    ,time_slot
                    ,sum(weekday) as weekday_sum
                    ,sum(weekend) as weekend_sum
                from (
                    select * from temp_forecast_link_with_slot_main
                    order by link_id,dir,time_slot,s_fraction
                ) a
                group by link_id,dir,time_slot
            ) b 
            where not (weekday_sum <= 900 and weekend_sum <= 900);       

            analyze temp_forecast_link_with_slot_more_than_15min;
            
            CREATE INDEX temp_forecast_link_with_slot_more_than_15min_link_id_dir_idx
              ON temp_forecast_link_with_slot_more_than_15min
              USING btree
              (link_id, dir);
        
            insert into temp_forecast_link_with_slot_merge(
                link_id, dir, free_time, weekday_time, weekend_time, average_time
                , time_slot_array, weekday_diff_array, weekend_diff_array)
            select link_id,dir
                ,free_time
                ,weekday_time
                ,weekend_time
                ,average_time
                ,array_agg(time_slot) as time_slot_array 
                ,array_agg(weekday_sum - free_time) as weekday_diff_array
                ,array_agg(weekend_sum - free_time) as weekend_diff_array
            from (
                select link_id,dir
                    ,sum(free_time) as free_time
                    ,sum(weekday_time) as weekday_time
                    ,sum(weekend_time) as weekend_time
                    ,sum(average_time) as average_time
                    ,time_slot
                    ,sum(weekday) as weekday_sum
                    ,sum(weekend) as weekend_sum
                from (
                    select a.* from temp_forecast_link_with_slot_main a
                    left join temp_forecast_link_with_slot_more_than_15min b
                    on a.link_id = b.link_id and a.dir = b.dir
                    where b.link_id is null 
                    order by a.link_id,a.dir,a.time_slot,a.s_fraction
                ) a
                group by link_id,dir,time_slot
                order by link_id,dir,free_time,weekday_time,weekend_time,average_time,time_slot
            ) b
            group by link_id,dir,free_time,weekday_time,weekend_time,average_time;      
            """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()        
        
        # Time is more than 15min.      
        self.CreateTable2('temp_forecast_link_with_slot_main_merge')
        sqlcmd = """                        
            insert into temp_forecast_link_with_slot_main_merge(link_id, dir, free_time, weekday_time, weekend_time, average_time, 
               time_slot_array, seq_list_array, weekday_list_array, weekend_list_array)
            select link_id,dir
                ,free_time,weekday_time,weekend_time,average_time
                ,array_agg(time_slot) as time_slot_array    
                ,array_agg(seq_list) as seq_list_array
                ,array_agg(weekday_list) as weekday_list_array
                ,array_agg(weekend_list) as weekend_list_array
            from (
                select link_id,dir,time_slot
                    ,sum(free_time) as free_time
                    ,sum(weekday_time) as weekday_time
                    ,sum(weekend_time) as weekend_time
                    ,sum(average_time) as average_time
                    ,array_to_string(array_agg(seq),'|') as seq_list
                    ,array_to_string(array_agg(weekday),'|') as weekday_list
                    ,array_to_string(array_agg(weekend),'|') as weekend_list
                from (
                    select a.*,a.s_fraction as seq 
                    from temp_forecast_link_with_slot_main a
                    left join temp_forecast_link_with_slot_more_than_15min b
                    on a.link_id = b.link_id and a.dir = b.dir
                    where b.link_id is not null 
                    order by a.link_id,a.dir,a.time_slot,a.s_fraction
                ) a
                group by link_id,dir,time_slot
                order by link_id,dir,free_time,weekday_time,weekend_time,average_time,time_slot
            ) b group by link_id,dir,free_time,weekday_time,weekend_time,average_time;
            
            analyze temp_forecast_link_with_slot_main_merge;
            """      
        self.pg.execute2(sqlcmd)
        self.pg.commit2() 
          
        self.CreateFunction2('rdb_cal_link_forecast_time')
        sqlcmd = """
            select rdb_cal_link_forecast_time('temp_forecast_link_with_slot_main_merge','temp_forecast_link_with_slot_merge'); 
            analyze temp_forecast_link_with_slot_merge;      
            """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()                               

        self.CreateTable2('temp_forecast_link_with_slot_merge_link_id_idx')        
        self.CreateTable2('temp_forecast_link_with_slot_merge_link_id_dir_idx')
        
        rdb_log.log(self.ItemName, 'creating base data with slots ----- end', 'info')
                            
    def _prepareForecast_region(self, X):
        
        rdb_log.log(self.ItemName, 'preparing region data %s----- start'%X, 'info')
        # Create region link mapping table.
        sqlcmd = """
            drop table if exists temp_link_mapping_layer%X;
            create table temp_link_mapping_layer%X as
            select id,region_link_id
                ,unnest(link_id_14_array) as link_id_14
                ,unnest(link_dir_array) as link_dir
                ,unnest(seq_array) as seq
            from (
                select row_number() over() as id
                    ,region_link_id
                    ,array_agg(link_id_14) as link_id_14_array
                    ,array_agg(link_dir) as link_dir_array
                    ,array_agg(seq) as seq_array
                from (
                    select region_link_id
                        ,unnest(link_id_14) as link_id_14
                        ,unnest(link_dir_14) as link_dir
                        ,generate_series(1,len) as seq
                    from ( 
                    select *,array_length(link_id_14,1) as len
                    from rdb_region_layer%X_link_mapping
                    ) a
                ) b group by region_link_id
            ) c;
            
            analyze temp_link_mapping_layer%X;
            
            CREATE INDEX temp_link_mapping_layer%X_link_id_14_idx1
              ON temp_link_mapping_layer%X
              USING btree
              (link_id_14);
            CREATE INDEX temp_link_mapping_layer%X_id_idx1
              ON temp_link_mapping_layer%X
              USING btree
              (id);
        """
        sqlcmd = sqlcmd.replace('%X',X)
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        # Region link with periodic data. 
        sqlcmd = """
            drop table if exists temp_forecast_link_id_layer%X;
            create table temp_forecast_link_id_layer%X 
            as
            (
                select distinct region_link_id,dir
                from (
                     select a.region_link_id
                        ,case when a.link_dir = 'f' and b.dir = 0 then 1
                            when a.link_dir = 'f' and b.dir = 1 then 0
                            else b.dir
                        end as dir
                     from temp_link_mapping_layer%X a 
                     left join temp_forecast_link_with_slot b
                     on a.link_id_14 = b.link_id
                     where b.link_id is not null 
                ) c
            );
            
            CREATE INDEX temp_forecast_link_id_layer%X_region_link_id_dir_idx
              ON temp_forecast_link_id_layer%X
              USING btree
              (region_link_id,dir);                                   
        """
        sqlcmd = sqlcmd.replace('%X',X)
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        rdb_log.log(self.ItemName, 'preparing region data %s----- end'%X, 'info')
        
    def _createForecastWithoutSlot_region(self, X):
        
        rdb_log.log(self.ItemName, 'creating region data without slots %s ----- start'%X, 'info')
        # Insert into mapping table: region links without slots.
        sqlcmd = """
            insert into rdb_forecast_link(link_id,link_id_t,dir,weekday_time,weekend_time,average_time,type)
            select region_link_id as link_id
                ,(region_link_id >> 32)
                ,dir
                ,sum(weekday_time) as weekday_time
                ,sum(weekend_time) as weekend_time
                ,sum(average_time) as average_time
                ,(%X * 10) + 1
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
                    from temp_link_mapping_layer%X a 
                    left join rdb_forecast_link b
                    on a.link_id_14 = b.link_id
                    where b.link_id is not null
                ) m
                left join temp_forecast_link_id_layer%X n
                on m.region_link_id = n.region_link_id and m.dir = n.dir
                where n.region_link_id is null
            ) p group by region_link_id,dir;          
        """
        sqlcmd = sqlcmd.replace('%X',X)
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        rdb_log.log(self.ItemName, 'creating region data without slots %s----- end'%X, 'info')
        
    def _createForecastWithSlot_region(self, X):
        
        rdb_log.log(self.ItemName, 'creating region data with slots %s----- start'%X, 'info')        
        # Region links with periodic data: time slots were unfolded.

        sqlcmd = """
            analyze temp_forecast_link_id_layer%X;
            analyze temp_link_mapping_layer%X;
            analyze temp_forecast_link_with_slot_merge;                    
        """
        sqlcmd = sqlcmd.replace('%X',X)
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        sqlcmd = """
            drop table if exists temp_forecast_link_with_slot_merge_layer%X;
            CREATE TABLE temp_forecast_link_with_slot_merge_layer%X
            (
              link_id bigint,
              dir smallint,
              free_time smallint,
              weekday_time smallint,
              weekend_time smallint,
              average_time smallint,
              time_slot_array smallint[],
              weekday_diff_array smallint[],
              weekend_diff_array smallint[]
            );
        """
        sqlcmd = sqlcmd.replace('%X',X)
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        sqlcmd = """                    
            insert into temp_forecast_link_with_slot_merge_layer%X(
                link_id, dir, free_time, weekday_time, weekend_time, average_time
                , time_slot_array, weekday_diff_array, weekend_diff_array)
            select link_id,dir
                ,free_time
                ,weekday_time
                ,weekend_time
                ,average_time
                ,array_agg(time_slot) as time_slot_array 
                ,array_agg(weekday_sum - free_time) as weekday_diff_array
                ,array_agg(weekend_sum - free_time) as weekend_diff_array
            from (
                select link_id,dir
                    ,sum(free_time) as free_time
                    ,sum(weekday_time) as weekday_time
                    ,sum(weekend_time) as weekend_time
                    ,sum(average_time) as average_time
                    ,time_slot
                    ,sum(weekday) as weekday_sum
                    ,sum(weekend) as weekend_sum
                from (
                    select m.* from (
                        select id,link_id,org_link_id,seq,dir
                            ,free_time,weekday_time,weekend_time,average_time
                            ,unnest(time_slot_array) as time_slot
                            ,(unnest(weekday_diff_array) + free_time) as weekday
                            ,(unnest(weekend_diff_array) + free_time) as weekend
                        from (
                            select a.id
                                ,a.region_link_id as link_id
                                ,case when b.link_id is not null then b.link_id
                                    else c.link_id
                                end as org_link_id
                                ,a.seq
                                ,case when b.link_id is not null and a.link_dir = false and b.dir = 0 then 1
                                    when b.link_id is not null and a.link_dir = false and b.dir = 1 then 0
                                    when b.link_id is not null and a.link_dir = true then b.dir
                                    when c.link_id is not null and a.link_dir = false and c.dir = 0 then 1
                                    when c.link_id is not null and a.link_dir = false and c.dir = 1 then 0 
                                    when c.link_id is not null and a.link_dir = true then c.dir
                                    else null
                                end as dir
                                ,case when b.link_id is not null then b.free_time
                                    when b.link_id is null and c.weekday_time <= c.weekend_time then c.weekday_time 
                                    when b.link_id is null and c.weekend_time <= c.weekday_time then c.weekend_time
                                    else null
                                end as free_time
                                ,case when b.link_id is not null then b.weekday_time
                                    when c.link_id is not null then c.weekday_time
                                    else null
                                end as weekday_time
                                ,case when b.link_id is not null then b.weekend_time
                                    when c.link_id is not null then c.weekend_time
                                    else null
                                end as weekend_time
                                ,case when b.link_id is not null then b.average_time
                                    when c.link_id is not null then c.average_time
                                    else null
                                end as average_time
                                ,case when b.link_id is not null then b.time_slot_array
                                    else ( select array_agg(seq) from (
                                            select 1 as id,generate_series(0,287) as seq
                                        ) a group by id)
                                end as time_slot_array
                                ,case when b.link_id is not null then b.weekday_diff_array
                                    else ( select array_agg(id) from (
                                            select generate_series(0,287) as seq
                                                ,(
                                                    case when c.weekday_time <= c.weekend_time then 0
                                                        else c.weekday_time - c.weekend_time
                                                    end 
                                                ) as id
                                            ) a
                                        )
                                end as weekday_diff_array
                                ,case when b.link_id is not null then b.weekend_diff_array
                                    else ( select array_agg(id) from (
                                            select generate_series(0,287) as seq
                                                ,(
                                                    case when c.weekend_time <= c.weekday_time then 0
                                                        else c.weekend_time - c.weekday_time
                                                    end 
                                                ) as id
                                            ) a
                                        )
                                end as weekend_diff_array        
                            from  (
                                select *,0 as dir
                                from temp_link_mapping_layer%X
                                union
                                select *,1 as dir
                                from temp_link_mapping_layer%X  
                            ) a
                            left join temp_forecast_link_with_slot_merge b
                            on a.link_id_14 = b.link_id and a.dir = b.dir
                            left join rdb_forecast_link c
                            on a.link_id_14 = c.link_id and a.dir = c.dir and c.type in (11,12)
                            where (b.link_id is not null or c.link_id is not null)
                            and a.id >= %s and a.id <= %s 
                        ) d                    
                    ) m
                    left join temp_forecast_link_id_layer%X n
                    on m.link_id = n.region_link_id and m.dir = n.dir
                    where n.region_link_id is not null
                    order by m.link_id,m.dir,m.time_slot,m.seq
                ) a
                group by link_id,dir,time_slot
                order by link_id,dir,free_time,weekday_time,weekend_time,average_time,time_slot
            ) b
            group by link_id,dir,free_time,weekday_time,weekend_time,average_time;                                        
        """
        
        (self.min_link, self.max_link) = self.pg.getMinMaxValue('temp_link_mapping_layer%s'%X, '(id)')       
        sqlcmd = sqlcmd.replace('%X',X)           
        self.pg.multi_execute(sqlcmd, self.min_link, self.max_link, 3, 1000, self.log)
                                  
        sqlcmd = """
            analyze temp_forecast_link_with_slot_merge_layer%X;
            
            CREATE INDEX temp_forecast_link_with_slot_merge_layer%X_time_slot_array_weekday_diff_array_idx1
              ON temp_forecast_link_with_slot_merge_layer%X
              USING btree
              (time_slot_array,weekday_diff_array); 

            CREATE INDEX temp_forecast_link_with_slot_merge_layer%X_link_id_dir_idx1
              ON temp_forecast_link_with_slot_merge_layer%X
              USING btree
              (link_id,dir); 
            """
        sqlcmd = sqlcmd.replace('%X',X)
        self.pg.execute2(sqlcmd)
        self.pg.commit2() 

        rdb_log.log(self.ItemName, 'creating region data with slots %s----- end'%X, 'info')

    def _createForecastWithSlot_region_bak(self, X):
        
        rdb_log.log(self.ItemName, 'creating region data with slots %s----- start'%X, 'info')        
        # Region links with periodic data: time slots were unfolded.
        sqlcmd = """
            analyze rdb_forecast_link;
            
            drop table if exists temp_forecast_link_with_slot_main_layer%X cascade;
            create table temp_forecast_link_with_slot_main_layer%X as
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
                    ,a.seq
                    ,b.time_slot
                    ,b.weekday
                    ,b.weekend
                from temp_link_mapping_layer%X a 
                left join (
                    select link_id,dir
                        ,free_time,weekday_time,weekend_time,average_time
                        ,unnest(time_slot_array) as time_slot
                        ,(unnest(weekday_diff_array) + free_time) as weekday
                        ,(unnest(weekend_diff_array) + free_time) as weekend
                    from temp_forecast_link_with_slot_merge
                    union all
                    select p.link_id,dir
                        ,p.free_time,p.weekday_time,p.weekend_time,p.average_time
                        ,q.time_slot
                        ,p.weekday_time as weekday
                        ,p.weekend_time as weekend
                    from rdb_forecast_link p
                    left join temp_forecast_time_slot q
                    on true = true
                    where p.link_id < 0 and p.type in (11,12)
                ) b
                on a.link_id_14 = b.link_id
                where b.link_id is not null
            ) m
            left join temp_forecast_link_id_layer%X n
            on m.link_id = n.region_link_id and m.dir = n.dir
            where n.region_link_id is not null;
            
            analyze temp_forecast_link_with_slot_main_layer%X;
            
            CREATE INDEX temp_forecast_link_with_slot_main_layer%X_link_id_dir_time_slot_seq_idx1
              ON temp_forecast_link_with_slot_main_layer%X
              USING btree
              (link_id,dir,time_slot,seq);                               
        """
        sqlcmd = sqlcmd.replace('%X',X)
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        sqlcmd = """
            drop table if exists temp_forecast_link_with_slot_merge_layer%X;
            CREATE TABLE temp_forecast_link_with_slot_merge_layer%X
            (
              link_id bigint,
              dir smallint,
              free_time smallint,
              weekday_time smallint,
              weekend_time smallint,
              average_time smallint,
              time_slot_array smallint[],
              weekday_diff_array smallint[],
              weekend_diff_array smallint[]
            );
            
            drop table if exists temp_forecast_link_with_slot_more_than_15min_layer%X;
            create table temp_forecast_link_with_slot_more_than_15min_layer%X as
            select distinct link_id,dir from (
                select link_id,dir
                    ,sum(free_time)
                    ,sum(weekday_time)
                    ,sum(weekend_time)
                    ,sum(average_time)
                    ,time_slot
                    ,sum(weekday) as weekday_sum
                    ,sum(weekend) as weekend_sum
                from (
                    select * from temp_forecast_link_with_slot_main_layer%X 
                    order by link_id,dir,time_slot,seq
                ) a
                group by link_id,dir,time_slot
            ) b 
            where not (weekday_sum <= 900 and weekend_sum <= 900);            

            CREATE INDEX temp_forecast_link_with_slot_more_than_15min_layer%X_link_id_dir_idx
              ON temp_forecast_link_with_slot_more_than_15min_layer%X
              USING btree
              (link_id, dir);

            insert into temp_forecast_link_with_slot_merge_layer%X(
                link_id, dir, free_time, weekday_time, weekend_time, average_time
                , time_slot_array, weekday_diff_array, weekend_diff_array)
            select link_id,dir
                ,free_time
                ,weekday_time
                ,weekend_time
                ,average_time
                ,array_agg(time_slot) as time_slot_array 
                ,array_agg(weekday_sum - free_time) as weekday_diff_array
                ,array_agg(weekend_sum - free_time) as weekend_diff_array
            from (
                select link_id,dir
                    ,sum(free_time) as free_time
                    ,sum(weekday_time) as weekday_time
                    ,sum(weekend_time) as weekend_time
                    ,sum(average_time) as average_time
                    ,time_slot
                    ,sum(weekday) as weekday_sum
                    ,sum(weekend) as weekend_sum
                from (
                    select a.* from temp_forecast_link_with_slot_main_layer%X a
                    left join temp_forecast_link_with_slot_more_than_15min_layer%X b
                    on a.link_id = b.link_id and a.dir = b.dir
                    where b.link_id is null 
                    order by a.link_id,a.dir,a.time_slot,a.seq
                ) a
                group by link_id,dir,time_slot
                order by link_id,dir,free_time,weekday_time,weekend_time,average_time,time_slot
            ) b
            group by link_id,dir,free_time,weekday_time,weekend_time,average_time;                                          
        """
        sqlcmd = sqlcmd.replace('%X',X)
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
                                  
        sqlcmd = """
            drop table if exists temp_forecast_link_with_slot_main_merge_layer%X;
            create table temp_forecast_link_with_slot_main_merge_layer%X 
            as
            (
                select link_id,dir
                    ,free_time,weekday_time,weekend_time,average_time
                    ,array_agg(time_slot) as time_slot_array    
                    ,array_agg(seq_list) as seq_list_array
                    ,array_agg(weekday_list) as weekday_list_array
                    ,array_agg(weekend_list) as weekend_list_array
                from (
                    select link_id,dir,time_slot
                        ,sum(free_time) as free_time
                        ,sum(weekday_time) as weekday_time
                        ,sum(weekend_time) as weekend_time
                        ,sum(average_time) as average_time
                        ,array_to_string(array_agg(seq),'|') as seq_list
                        ,array_to_string(array_agg(weekday),'|') as weekday_list
                        ,array_to_string(array_agg(weekend),'|') as weekend_list
                    from (
                        select a.*
                        from temp_forecast_link_with_slot_main_layer%X a
                        left join temp_forecast_link_with_slot_more_than_15min_layer%X b
                        on a.link_id = b.link_id and a.dir = b.dir
                        where b.link_id is not null 
                        order by a.link_id,a.dir,a.time_slot,a.seq
                    ) a
                    group by link_id,dir,time_slot
                    order by link_id,dir,free_time,weekday_time,weekend_time,average_time,time_slot
                ) b group by link_id,dir,free_time,weekday_time,weekend_time,average_time
            );
            
            analyze temp_forecast_link_with_slot_main_merge_layer%X;
        """
        sqlcmd = sqlcmd.replace('%X',X)
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        # Region links with periodic data: fractions are merged.
        self.CreateFunction2('rdb_cal_link_forecast_time')
        sqlcmd = """
            select rdb_cal_link_forecast_time('temp_forecast_link_with_slot_main_merge_layer%X','temp_forecast_link_with_slot_merge_layer%X');       

            analyze temp_forecast_link_with_slot_merge_layer%X;
            
            CREATE INDEX temp_forecast_link_with_slot_merge_layer%X_time_slot_array_weekday_diff_array_idx1
              ON temp_forecast_link_with_slot_merge_layer%X
              USING btree
              (time_slot_array,weekday_diff_array); 

            CREATE INDEX temp_forecast_link_with_slot_merge_layer%X_link_id_dir_idx1
              ON temp_forecast_link_with_slot_merge_layer%X
              USING btree
              (link_id,dir); 
            """
        sqlcmd = sqlcmd.replace('%X',X)
        self.pg.execute2(sqlcmd)
        self.pg.commit2() 

        rdb_log.log(self.ItemName, 'creating region data with slots %s----- end'%X, 'info')
                
    def _insertRDBTables(self): 

        rdb_log.log(self.ItemName, 'inserting into RDB tables ----- start', 'info') 
        # Give a unique ID to time slots.
        self.CreateTable2('temp_forecast_time_distinct')
        self.CreateIndex2('temp_forecast_time_distinct_time_slot_array_time_array_idx')
        
        # Give a unique ID to control table.
        self.CreateTable2('temp_forecast_control')    
        self.CreateIndex2('temp_forecast_control_time_slot_array_weekday_diff_array_wee_idx')
              
        # Insert into time slot table.        
        sqlcmd = """
            analyze temp_forecast_time_distinct;
            
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
            analyze temp_forecast_control;
            
            insert into rdb_forecast_control(
                info_id,time_id_weekday,time_id_weekend)
            select info_id
                ,time_id_weekday,time_id_weekend
            from temp_forecast_control;
            """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        # Insert into mapping table.
        sqlcmd = """
            insert into rdb_forecast_link(link_id,link_id_t,dir,free_time,weekday_time,weekend_time,average_time,info_id,type)
            select a.link_id
                ,(a.link_id >> 32)
                ,a.dir
                ,a.free_time,a.weekday_time,a.weekend_time
                ,a.average_time
                ,b.info_id
                ,type
            from (
                    select *,13 as type from temp_forecast_link_with_slot_merge
                    union all
                    select *,43 as type from temp_forecast_link_with_slot_merge_layer4
                    union all
                    select *,63 as type from temp_forecast_link_with_slot_merge_layer6
                    union all
                    select *,83 as type from temp_forecast_link_with_slot_merge_layer8                    
            ) a
            left join temp_forecast_control b
            on a.time_slot_array = b.time_slot_array and a.weekday_diff_array = b.weekday_diff_array
            and a.weekend_diff_array = b.weekend_diff_array;
            """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        rdb_log.log(self.ItemName, 'inserting into RDB tables ----- end', 'info') 
        

    def Do_CreatIndex(self):
        
        self.CreateIndex2('rdb_forecast_link_link_id_idx1')
        self.CreateIndex2('rdb_forecast_link_link_id_t_idx')
        self.CreateIndex2('rdb_forecast_control_info_id_idx1')
        self.CreateIndex2('rdb_forecast_time_time_id_idx1')

        sqlcmd = """
        cluster rdb_forecast_link using rdb_forecast_link_link_id_t_idx;
        analyze rdb_forecast_link;
        cluster rdb_forecast_control using rdb_forecast_control_info_id_idx1;
        analyze rdb_forecast_control;
            """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()                    
        
        