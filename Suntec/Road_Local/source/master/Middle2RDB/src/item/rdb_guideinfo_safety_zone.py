# -*- coding: cp936 -*-
'''
Created on 2012-2-7

@author: hongchenzai
'''
from base.rdb_ItemBase import ItemBase
from common import rdb_log

class rdb_guideinfo_safety_zone(ItemBase):
    '''
    classdocs
    '''
    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self, 'Safety_Zone')
    
    def Do_CreateTable(self):
        self.CreateTable2('rdb_guideinfo_safety_zone')
        return 0

    def Do_CreatIndex(self):
        '创建相关表索引.'
        self.CreateIndex2('rdb_guideinfo_safety_zone_link_id_idx')
        return 0
    
    def Do(self):
        
        sqlcmd = """
            INSERT INTO rdb_guideinfo_safety_zone(
            safetyzone_id, link_id, link_id_t, speedlimit, speedunit_code, direction, safety_type
            )
            SELECT a.safetyzone_id, b.tile_link_id, b.tile_id,
                a.speedlimit, a.speedunit_code, a.direction, a.safety_type
            FROM safety_zone_tbl as a
            inner join rdb_tile_link as b
            on a.linkid = b.old_link_id
            order by a.gid
        """
        
        rdb_log.log(self.ItemName, 'Start making rdb_guideinfo_safety_zone', 'info') 
        
        if self.pg.execute2(sqlcmd) == -1:
            rdb_log.log(self.ItemName, 'making rdb_guideinfo_safety_zone data failed.', 'error')
            return -1
        
        self.pg.commit2()
    
        rdb_log.log(self.ItemName, 'succeeded to make rdb_guideinfo_safety_zone.', 'info')
        
        
