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
        
        self._insertIntoRDB()
        self._deleteDummyLinks()
        
    def _insertIntoRDB(self):        
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
        
    def _deleteDummyLinks(self):
        
        '删除不位于起点camera与终点camera之间的safety zone link和accident zone link。' 

        self.CreateTable2('rdb_guideinfo_safety_zone_bak_delete_dummy')
        self.CreateTable2('rdb_guideinfo_safety_zone')
        
        self.CreateTable2('temp_guideinfo_safetyalert')        
        self.CreateFunction2('rdb_find_idx_in_array')
        self.CreateFunction2('rdb_del_dummy_safetyzone')  
     
        sqlcmd = """
            INSERT INTO rdb_guideinfo_safety_zone(
                safetyzone_id, link_id, link_id_t, speedlimit, speedunit_code, direction, safety_type, dis
            )        
            select safetyzone_id,
                unnest(string_to_array(link_id_list,'|'))::bigint as link_id,
                unnest(string_to_array(link_id_t_list,'|'))::int as link_id_t,
                unnest(string_to_array(speedlimit_list,'|'))::smallint as speedlimit,
                unnest(string_to_array(speedunit_code_list,'|'))::smallint as speedunit_code,
                unnest(string_to_array(direction_list,'|'))::smallint as direction,
                --unnest(string_to_array(type_list,'|')) as type,
                %TYPE as safety_type,
                unnest(string_to_array(s_dis_list,'|'))::smallint as dis
            from (
                select safetyzone_id,start_idx,end_idx,
                    rdb_del_dummy_safetyzone(gid_array,start_idx,end_idx) as gid_list,
                    rdb_del_dummy_safetyzone(link_id_array,start_idx,end_idx) as link_id_list,
                    rdb_del_dummy_safetyzone(link_id_t_array,start_idx,end_idx) as link_id_t_list,
                    rdb_del_dummy_safetyzone(speedlimit_array,start_idx,end_idx) as speedlimit_list,
                    rdb_del_dummy_safetyzone(speedunit_code_array,start_idx,end_idx) as speedunit_code_list,
                    rdb_del_dummy_safetyzone(direction_array,start_idx,end_idx) as direction_list,
                    rdb_del_dummy_safetyzone(type_array,start_idx,end_idx) as type_list,
                    rdb_del_dummy_safetyzone(s_dis_array,start_idx,end_idx) as s_dis_list,
                    rdb_del_dummy_safetyzone(e_dis_array,start_idx,end_idx) as e_dis_list
                from (
                    select safetyzone_id,gid_array,link_id_array,link_id_t_array,
                        speedlimit_array,speedunit_code_array,direction_array,
                        type_array,s_dis_array,e_dis_array,
                        rdb_find_idx_in_array(type_array,'%START') as start_idx,
                        rdb_find_idx_in_array(type_array,'%END') as end_idx
                    from (
                        select safetyzone_id,
                            array_to_string(array_agg(gid),'|') as gid_array,
                            array_to_string(array_agg(link_id),'|') as link_id_array,
                            array_to_string(array_agg(link_id_t),'|') as link_id_t_array,
                            array_to_string(array_agg(speedlimit),'|') as speedlimit_array,
                            array_to_string(array_agg(speedunit_code),'|') as speedunit_code_array,
                            array_to_string(array_agg(direction),'|') as direction_array,
                            array_to_string(array_agg(type),'|') as type_array,
                            array_to_string(array_agg(s_dis),'|') as s_dis_array,
                            array_to_string(array_agg(e_dis),'|') as e_dis_array
                        from (
                            select a.*,
                                case when b.link_id is null then -1
                                    else b.type
                                end as type,
                                case when b.s_dis is null then -1
                                    else b.s_dis
                                end as s_dis,
                                case when b.e_dis is null then -1
                                    else b.e_dis
                                end as e_dis 
                            from rdb_guideinfo_safety_zone_bak_delete_dummy a
                            left join temp_guideinfo_safetyalert b
                            on a.link_id = b.link_id and a.direction - 2 = b.dir and b.type in (%START,%END)
                            where a.safety_type = %TYPE
                            order by a.safetyzone_id,a.gid
                        ) c group by safetyzone_id
                    ) d
                ) e
            ) f;
        """ 
        
        '插入safety zone.'
        sqlcmd_safetyzone = sqlcmd.replace('%START', '4')
        sqlcmd_safetyzone = sqlcmd_safetyzone.replace('%END', '5')
        sqlcmd_safetyzone = sqlcmd_safetyzone.replace('%TYPE', '1')
        self.pg.execute2(sqlcmd_safetyzone)
        self.pg.commit2()
        
        '插入accident zone.'
        sqlcmd_accidentzone = sqlcmd.replace('%START', '10')
        sqlcmd_accidentzone = sqlcmd_accidentzone.replace('%END', '11')
        sqlcmd_accidentzone = sqlcmd_accidentzone.replace('%TYPE', '2')
        self.pg.execute2(sqlcmd_accidentzone)
        self.pg.commit2()         

        '插入school zone.'
        sqlcmd = """
            INSERT INTO rdb_guideinfo_safety_zone(
            safetyzone_id, link_id, link_id_t, speedlimit, speedunit_code, direction, safety_type, dis
            )
            SELECT a.safetyzone_id, b.tile_link_id, b.tile_id,
                a.speedlimit, a.speedunit_code, a.direction, a.safety_type, -1
            FROM safety_zone_tbl as a
            inner join rdb_tile_link as b
            on a.linkid = b.old_link_id
            where a.safety_type = 3
            order by a.gid
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()                         