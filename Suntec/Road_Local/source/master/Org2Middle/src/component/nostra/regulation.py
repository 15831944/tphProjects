# -*- coding: cp936 -*-
'''
Created on 2013-12-5

@author: liuxinxing
'''



import component.default.regulation

class comp_regulation_nostra(component.default.regulation.com_regulation):
    def __init__(self):
        '''
        Constructor
        '''
        component.default.regulation.com_regulation.__init__(self)
    
    def _Do(self):
        self.__convert_condition_table()
        self.__convert_regulation()
        self._deal_temp_patch_regulation_tbl()
        return 0
    
    def __convert_condition_table(self):
        self.log.info('Begin converting condition_regulation_tbl...')
        
        self.CreateTable2('temp_condition_regulation_tbl')
        self.CreateIndex2('temp_condition_regulation_tbl_day_time_idx')

        self.CreateFunction2('mid_convert_weekflag')
        sqlcmd = """
                insert into condition_regulation_tbl
                            (
                            "cond_id", "car_type", "day_of_week",
                            "start_year", "start_month", "start_day", "start_hour", "start_minute",
                            "end_year", "end_month", "end_day", "end_hour", "end_minute", "exclude_date"
                            )
                select  cond_id, -2 as car_type, day_of_week,
                        0 as start_year, 0 as start_month, 0 as start_day, 
                        (subtime_array[1] / 100) as start_hour,
                        (subtime_array[1] % 100) as start_minute,
                        0 as end_year, 0 as end_month, 0 as end_day, 
                        (subtime_array[2] / 100) as end_hour,
                        (subtime_array[2] % 100) as end_minute,
                        0 as exclude_date
                from
                (
                    select  cond_id, mid_convert_weekflag(day) as day_of_week, 
                            string_to_array(time,'-')::integer[] as subtime_array
                    from 
                    (
                        select  cond_id, day, 
                                (case when time is null then '0000-0000' else time end) as time
                        from temp_condition_regulation_tbl
                    )as a
                )as b
                ;
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.log.info('End converting condition_regulation_tbl.')
    
    def __convert_regulation(self):
        self.log.info('Begin converting regulation...')
        
        #
        self.CreateFunction2('mid_get_connect_node')
        self.CreateFunction2('mid_convert_regulation')
        
        self.pg.callproc('mid_convert_regulation')
        self.pg.commit2()
        
        self.log.info('End converting regulation.')
    
    def _DoCheckLogic(self):
        self.log.info('Check regulation...')
        
        self.CreateFunction2('mid_check_regulation_item')
        self.pg.callproc('mid_check_regulation_item')
        
        self.CreateFunction2('mid_check_regulation_condition')
        self.pg.callproc('mid_check_regulation_condition')
        
        self.log.info('Check regulation end.')
        return 0
    
    def _update_temp_regulation_patch_node_tbl(self):
        
        self.log.info('Begin converting regulation patch geometry to node...')
        
        self.CreateFunction2('update_temp_regulation_patch_node_tbl_nostra')
        self.pg.callproc('update_temp_regulation_patch_node_tbl_nostra')
        self.pg.commit2()
        
        self.log.info('End converting regulation patch geometry to node.')
        return 0
    
    def _update_temp_regulation_patch_link_tbl(self):
        
        self.log.info('Begin converting regulation patch node to link...')
        
        self.CreateFunction2('findpasslinkbybothnodes_nostra')
        self.CreateFunction2('update_temp_regulation_patch_link_tbl_nostra')
        self.pg.callproc('update_temp_regulation_patch_link_tbl_nostra')
        self.pg.commit2()
        
        self.log.info('End converting regulation patch node to link.')
        return 0