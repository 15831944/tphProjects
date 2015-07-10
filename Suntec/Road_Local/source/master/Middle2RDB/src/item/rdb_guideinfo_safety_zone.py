# -*- coding: cp936 -*-
'''
Created on 2012-2-7

@author: hongchenzai
'''
from base.rdb_ItemBase import ItemBase

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
            where a.safety_type=3
            order by a.gid
        """

        self.log.info('Start making rdb_guideinfo_safety_zone') 
        
        if self.pg.execute2(sqlcmd) == -1:
            self.log.error('making rdb_guideinfo_safety_zone data failed.')
            return -1
        
        self.pg.commit2()
        
        sqlcmd = """
            SELECT array_agg(a.gid),safetyzone_id, 
            array_agg(case when direction=2 then b.s_node else b.e_node end) as start_node_arr,
                    array_agg(case when direction=3 then b.s_node else b.e_node end) as end_node_arr
            FROM safety_zone_tbl a
            join link_tbl b
            on a.linkid=b.link_id
            where safety_type<>3
            group by safetyzone_id
            order by safetyzone_id
        """
        
        self.pg.execute2(sqlcmd)
        results=self.pg.fetchall2()
        
        sqlcmd_insert='''
            INSERT INTO rdb_guideinfo_safety_zone(
            safetyzone_id, link_id, link_id_t, speedlimit, speedunit_code, direction, safety_type
            )
            SELECT a.safetyzone_id, b.tile_link_id, b.tile_id,
                a.speedlimit, a.speedunit_code, a.direction, a.safety_type
            FROM safety_zone_tbl as a
            inner join rdb_tile_link as b
            on a.linkid = b.old_link_id
            where a.gid=%d
                        '''
    
        
        for result in results:
            gid_arr=result[0]
            start_node_arr=result[2]
            end_node_arr=result[3]
            node_id=filter(lambda x:x not in end_node_arr,start_node_arr)
            if len(node_id)<>1:
                self.log.error('making rdb_guideinfo_safety_zone data failed. Safety zone or Blackspot not a line sequence!')
                return -1
            node_id=node_id[0]
            idx=start_node_arr.index(node_id)
            for i in range(len(start_node_arr)-1):
                self.pg.execute2(sqlcmd_insert%gid_arr[idx])
                node_id_next=end_node_arr[idx]
                if start_node_arr.count(node_id_next)<>1:
                    self.log.error('making rdb_guideinfo_safety_zone data failed. Safety zone or Blackspot not a line sequence!')
                    return -1
                idx=start_node_arr.index(node_id_next)
                node_id=node_id_next
            self.pg.execute2(sqlcmd_insert%gid_arr[idx])
        self.pg.commit2()
                
    
        self.log.info('succeeded to make rdb_guideinfo_safety_zone.')
        
        
