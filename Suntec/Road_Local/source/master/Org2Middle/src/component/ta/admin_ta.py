# -*- coding: UTF8 -*-
'''
Created on 2012-9-10

@author: sunyifeng
@alter 2014.02.07 zhaojie
'''

import component.component_base

class comp_admin_ta(component.component_base.comp_base):
    '''
    classdocs
    '''


    def __init__(self):
        '''
        Constructor 
        '''
        component.component_base.comp_base.__init__(self, 'Admin')
    
    def _Do(self):
        self.__makeAdminOrder0_8()
        self.__makeAdminZone()
        self.__UpdatAdminName()
        self.__makeAdminTimeZone()
        return 0
    
    def __makeAdminOrder0_8(self):
        self.log.info('Begin make admin table 0_8.')
        
        self.CreateTable2('temp_adminid_newandold')
        self.CreateTable2('temp_admin_order0')
        self.CreateTable2('temp_admin_order1')
        self.CreateTable2('temp_admin_order2')
        self.CreateTable2('temp_admin_order8')
        
        #VNM country function
        self.CreateFunction2('mid_alter_VN_arder0_8')
        self.CreateFunction2('mid_Get_VN_UpOrder')
        
        #Universal function
        #self.CreateFunction2('mid_Get_BigIDToInt')
        self.CreateFunction2('mid_admin_GetNewID')
        self.CreateFunction2('mid_admin_SetNewID')
        self.CreateFunction2('mid_judge_country_array')
        
        self.CreateFunction2('mid_into_ID_NewOLd')
        
        self.CreateFunction2('mid_alter_arder0_8')          
        self.pg.callproc('mid_alter_arder0_8')
        self.pg.commit2()
        self.CreateIndex2('temp_adminid_newandold_id_old_idx')
        
        #for saf
        sqlcmd = """
                update temp_admin_order2 as a
                set order1_id = b.order1_id
                from
                (
                    select a.order2_id, b.order1_id
                    from temp_admin_order2 as a
                    left join temp_admin_order1 as b
                    on ST_Contains(b.the_geom, a.the_geom)
                    where a.order1_id = 0
                )as b
                where a.order1_id = 0;
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.log.info('End make admin table 0_8.')  
    
    def __makeAdminZone(self):
        self.log.info('making admin_zone info...')
        
        self.CreateTable2('mid_admin_zone')
        
        #VNM country function
        self.CreateFunction2('mid_insert_mid_admin_zone_VN')
        
        #Universal function
        self.CreateFunction2('mid_judge_country_array')
        self.CreateFunction2('mid_insert_mid_admin_zone')
        
        self.pg.callproc('mid_insert_mid_admin_zone')
        
        self.pg.commit2()
        
        #creat idx
        self.CreateIndex2('mid_admin_zone_ad_code_idx')
        self.CreateIndex2('mid_admin_zone_ad_order_idx')
        self.CreateIndex2('mid_admin_zone_order0_id_idx')
        self.CreateIndex2('mid_admin_zone_order1_id_idx')
        self.CreateIndex2('mid_admin_zone_order2_id_idx')
        self.CreateIndex2('mid_admin_zone_order8_id_idx')
        self.CreateIndex2('mid_admin_zone_the_geom_idx')
        
        self.log.info('making admin_zone info OK.')
        
    def __UpdatAdminName(self):
        self.log.info('update Adminname begin...')
        sqlcmd = """
                    update mid_admin_zone set ad_name = mid_temp_admin_name.admin_name
                    from mid_temp_admin_name
                    where cast(mid_admin_zone.ad_name as bigint) = mid_temp_admin_name.admin_id;
                 """   
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.log.info('update Adminname end...')
    
    def __makeAdminTimeZone(self):
        self.log.info('making admin time zone begin...')
        
        self.CreateTable2('mid_admin_summer_time')
        self.CreateTable2('temp_admin_timedom_mapping')
        self.CreateFunction2('mid_convert_summer_time')
        self.pg.callproc('mid_convert_summer_time')
        self.pg.commit2()
        
        # do admin's time_zone & summer_time
        sqlcmd = """
            update mid_admin_zone as a 
            set time_zone = b.time_zone, 
                summer_time_id = b.summer_time_id
            from
            (
                select  distinct
                        a.new_id as ad_code,
                        (replace(b.attvalue, '.30', '.50')::float * 10 + 120) as time_zone,
                        d.summer_time_id
                from temp_adminid_newandold as a
                left join org_ae as b
                on a.id_old = b.id and b.atttyp = 'TZ'
                left join org_ae as c
                on a.id_old = c.id and c.atttyp = 'VP'
                left join temp_admin_timedom_mapping as d
                on c.attvalue = d.timedom
                where b.id is not null or c.id is not null
            )as b
            where a.ad_code = b.ad_code;
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        # update admin's time_zone & summer_time
        sqlcmd = """
            update mid_admin_zone as a 
            set [time_attr] = b.[time_attr]
            from
            (
                select ad_code, [time_attr]
                from mid_admin_zone
                where ad_order = 0 and [time_attr] is not null
            )as b
            where a.ad_order = 1 and a.[time_attr] is null and a.order0_id = b.ad_code;
            ;
            
            update mid_admin_zone as a 
            set [time_attr] = b.[time_attr]
            from
            (
                select ad_code, [time_attr]
                from mid_admin_zone
                where ad_order = 1 and [time_attr] is not null
            )as b
            where a.ad_order = 2 and a.[time_attr] is null and a.order1_id = b.ad_code;
            ;
            
            update mid_admin_zone as a 
            set [time_attr] = b.[time_attr]
            from
            (
                select ad_code, [time_attr]
                from mid_admin_zone
                where ad_order in (1,2) and [time_attr] is not null
            )as b
            where  a.ad_order = 8 and a.[time_attr] is null 
                   and 
                   (
                       (a.order2_id = b.ad_code)
                       or
                       (a.order2_id is null and a.order1_id = b.ad_code)
                   )
            ;
        """
        for time_attr in ['time_zone', 'summer_time_id']:
            self.pg.execute2(sqlcmd.replace('[time_attr]', time_attr))
            self.pg.commit2()
        
        self.log.info('making admin time zone end.')
        