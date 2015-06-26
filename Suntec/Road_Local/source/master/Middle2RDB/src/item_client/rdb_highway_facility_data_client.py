# -*- coding: cp936 -*-
'''
Created on 2012-2-7

@author: hongchenzai
'''
from base.rdb_ItemBase import ItemBase
from common import rdb_log

class rdb_highway_facility_data_client(ItemBase):
    '''
    classdocs
    '''
    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self, 'Highway_Facility_Client'
                          , 'rdb_highway_facility_data'
                          , 'gid'
                          , 'rdb_highway_facility_data_client'
                          , 'gid'
                          , False)
    
    def Do_CreateTable(self):
        self.CreateTable2('rdb_highway_facility_data_client')
        self.CreateTable2('rdb_highway_sa_pa_client')
        return 0

    def Do_CreatIndex(self):
        '创建相关表索引.'
        self.CreateIndex2('rdb_highway_facility_data_client_node_id_idx')
        # SA/PA
        self.CreateIndex2('rdb_highway_sa_pa_client_node_id_idx')
        return 0
    
    def Do(self):
        # 高速设施
        self._MakeFacility()
        # 高速服务区
        self._MakeSAPA()
        return 0
    
    def _MakeFacility(self):
        '''高速设施'''
        sqlcmd = """
                INSERT INTO rdb_highway_facility_data_client(
                            facility_id, node_id, node_id_t, 
                            separation, name_id, junction, pa, sa, ic, ramp, smart_ic, tollgate, 
                            dummy_tollgate, tollgate_type, service_info_flag)
                (
                SELECT facility_id, 
                       rdb_split_tileid(node_id), node_id_t, 
                       separation, name_id, junction, pa, sa, ic, ramp, smart_ic, tollgate, 
                       dummy_tollgate, tollgate_type, service_info_flag
                  FROM rdb_highway_facility_data
                  ORDER BY gid
                );
            """
        if self.pg.execute2(sqlcmd) != -1:
            self.pg.commit2()
        return 0
    
    def _MakeSAPA(self):
        '''高速服务区。'''
        sqlcmd = """
            INSERT INTO rdb_highway_sa_pa_client(
                        sapa_id, node_id, node_id_t, name_id, 
                        public_telephone, vending_machine, handicapped_telephone, toilet, 
                        handicapped_toilet, gas_station, natural_gas, nap_rest_area, 
                        rest_area, nursing_room, dinning, repair, shop, launderette, 
                        fax_service, coffee, post, hwy_infor_terminal, hwy_tot_springs, 
                        shower, image_id)
            (
            SELECT sapa_id,  
                   rdb_split_tileid(node_id), node_id_t, name_id, 
                   public_telephone, vending_machine, handicapped_telephone, toilet, 
                   handicapped_toilet, gas_station, natural_gas, nap_rest_area, 
                   rest_area, nursing_room, dinning, repair, shop, launderette, 
                   fax_service, coffee, post, hwy_infor_terminal, hwy_tot_springs, 
                   shower, image_id
              FROM rdb_highway_sa_pa
              ORDER BY gid
            );
            """
        if self.pg.execute2(sqlcmd) != -1:
            self.pg.commit2()
        return 0
    