# -*- coding: UTF8 -*-
'''
Created on 2012-9-10

@author: sunyifeng
@alter 2014.02.07 zhaojie
'''

from base import comp_base
import common
import time

class comp_admin_ta(comp_base):
    '''
    classdocs
    '''


    def __init__(self):
        '''
        Constructor 
        '''
        comp_base.__init__(self, 'Admin')
    
    #Creat table
    def _DoCreateTable(self):
        
        self.CreateTable2('temp_admin_order0')
        self.CreateTable2('temp_admin_order1')
        self.CreateTable2('temp_admin_order2')
        self.CreateTable2('temp_admin_order8')
        self.CreateTable2('temp_adminid_newandold')

        
        self.CreateTable2('mid_admin_zone')
        
        return 0
    #alter table    
    def _Do(self):
        self.__makeAdminOrder0_8()
        self.__makeAdminZone()
        
        #updata admin name
        self.__UpdatAdminName()
        
        return 0
        
    def __makeAdminOrder0_8(self):
            
        self.log.info('Begin make admin table 0_8.')
        
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
        
        self.log.info('End make admin table 0_8.')  
    
    def __makeAdminZone(self):
        self.log.info('making admin_zone info...')
        
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
        