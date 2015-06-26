# -*- coding: cp936 -*-
'''
Created on 2012-2-7

@author: hongchenzai
'''
from base.rdb_ItemBase import ItemBase
from common import rdb_log

class rdb_highway_facility_data(ItemBase):
    '''
    classdocs
    '''
    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self, 'Highway_Facility')
    
    def Do_CreateTable(self):
        self.CreateTable2('rdb_highway_facility_data')
        self.CreateTable2('rdb_highway_sa_pa')
        return 0

    def Do_CreatIndex(self):
        '创建相关表索引.'
        # 设施
        self.CreateIndex2('rdb_highway_facility_data_node_id_idx')
        # SA/PA 
        self.CreateIndex2('rdb_highway_sa_pa_node_id_idx')
        return 0
    
    def Do(self):
        # 高速设施
        self._MakeFacility()
        # 高速服务区
        self._MakeSAPA()
        return 0
    
    def _MakeFacility(self):
        '''高速设施'''
        if not self.pg.IsExistTable('highway_facility_tbl'):
            return 0
        sqlcmd = """
            INSERT INTO rdb_highway_facility_data(
                        facility_id,  node_id, node_id_t, 
                        separation, name_id, junction, pa, sa, ic, ramp, smart_ic, tollgate, 
                        dummy_tollgate, tollgate_type, service_info_flag)
            (
            SELECT facility_id,  
                   c.tile_node_id as node_id, c.tile_id as node_id_t, 
                   separation, name_id, junction, pa, sa, ic, ramp, smart_ic, tollgate, 
                   dummy_tollgate, tollgate_type, service_info_flag
            FROM highway_facility_tbl as a
            left join rdb_tile_node as c
            on a.node_id = c.old_node_id
            order by a.gid
            );
        """
        if self.pg.execute2(sqlcmd) != -1:
            self.pg.commit2()
        return 0
    
    def _MakeSAPA(self):
        '''高速服务区'''
        if not self.pg.IsExistTable('highway_sa_pa_tbl'):
            return 0
        sqlcmd = """
            INSERT INTO rdb_highway_sa_pa(
                        sapa_id, node_id, node_id_t, name_id, 
                        public_telephone, vending_machine, handicapped_telephone, toilet, 
                        handicapped_toilet, gas_station, natural_gas, nap_rest_area, 
                        rest_area, nursing_room, dinning, repair, shop, launderette, 
                        fax_service, coffee, post, hwy_infor_terminal, hwy_tot_springs, 
                        shower, image_id)
            (
            SELECT sapa_id, 
                   c.tile_node_id as node_id, c.tile_id as node_id_t, name_id, 
                   public_telephone, vending_machine, handicapped_telephone, toilet, 
                   handicapped_toilet, gas_station, natural_gas, nap_rest_area, 
                   rest_area, nursing_room, dinning, repair, shop, launderette, 
                   fax_service, coffee, post, hwy_infor_terminal, hwy_tot_springs, 
                   shower, image_id
              FROM highway_sa_pa_tbl as a
              left join rdb_tile_node as c
              on a.node_id = c.old_node_id
              order by a.gid
            );
            """
        if self.pg.execute2(sqlcmd) != -1:
            self.pg.commit2()
           
        return 0