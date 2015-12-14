# -*- coding: cp936 -*-
'''
Created on 2015-3-11

@author: zhengchao
'''
from base.rdb_ItemBase import ItemBase
from common import rdb_log

class rdb_guideinfo_safetyalert(ItemBase):
    '''
    classdocs
    '''
    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self, 'Safety_Alert')
    
    def Do_CreateTable(self):
        self.CreateTable2('rdb_guideinfo_safetyalert')
        return 0

    def Do_CreatIndex(self):
        '创建相关表索引.'
        #self.CreateIndex2('')
        return 0
    
    def Do(self):
        
        sqlcmd = """
                insert into rdb_guideinfo_safetyalert
                (lon, lat, link_id, link_id_t, link_id_r, type, angle, dir, 
                       s_dis, e_dis, speed, speed_unit)
                select lon,lat,b.tile_link_id,b.tile_id,c.tile_link_id,type,angle,dir,s_dis,e_dis, speed, speed_unit
                from 
                (       
                    select distinct *
                    from 
                    (       
                        SELECT lon, lat, link_id, orglink_id, type, angle, dir, 
                               s_dis, e_dis, speed, speed_unit
                          FROM safety_alert_tbl
                          where type not in (10,11)  -- high accident zone not included
                    )a
                )a
                left join rdb_tile_link b
                on a.link_id=b.old_link_id
                left join rdb_tile_link c
                on a.orglink_id=c.old_link_id
        """

        rdb_log.log(self.ItemName, 'Start making rdb_guideinfo_safetyalert', 'info') 
        
        if self.pg.execute2(sqlcmd) == -1:
            rdb_log.log(self.ItemName, 'making rdb_guideinfo_safetyalert data failed.', 'error')
            return -1
        
        self.pg.commit2()
    
        rdb_log.log(self.ItemName, 'succeeded to make rdb_guideinfo_safetyalert.', 'info')
        
        
