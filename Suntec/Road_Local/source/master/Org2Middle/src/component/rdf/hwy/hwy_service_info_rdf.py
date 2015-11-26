# -*- coding: UTF-8 -*-
'''
Created on 2015-3-14

@author: hcz
'''
from component.jdb.hwy.hwy_service_info import HwyServiceInfo


class HwyServiceInfoRDF(HwyServiceInfo):
    '''
    Service Info (服务情报)
    '''

    def __init__(self,):
        '''
        Constructor
        '''
        HwyServiceInfo.__init__(self)

    def _Do(self):
        self._make_service_info()  # SAPA
        self._make_ic_service_info()  # IC出口
        self._update_service_flag()
        return 0

    def _make_service_info(self):
        self.log.info('Make SAPA Service Info.')
        self.CreateTable2('highway_service_info')
        sqlcmd = '''
        INSERT INTO highway_service_info(
                      ic_no, dog_run, hwy_oasis,
                      public_telephone, vending_machine, handicapped_telephone,
                      handicapped_toilet, information, snack_corner,
                      nursing_room, gas_station, launderette,
                      coin_shower, toilet, rest_area,
                      shopping_corner, fax_service, postbox,
                      hwy_infor_terminal, atm, hwy_hot_springs,
                      nap_rest_area, restaurant)
        (
        SELECT distinct ic_no, 0 dog_run, 0 hwy_oasis,
               0 public_telephone, 0 vending_machine, 0 handicapped_telephone,
               0 handicapped_toilet, information, 0 snack_corner,
               0 nursing_room, gas_station, 0 launderette,
               0 coin_shower, toilet, rest_area,
               shopping_corner, 0 fax_service, postbox,
               0 hwy_infor_terminal, atm, 0 hwy_hot_springs,
               0 nap_rest_area, restaurant
          FROM hwy_service AS a
          LEFT JOIN mid_hwy_ic_no AS b
          ON a.road_code = b.road_code and
             a.road_seq = b.road_seq and
             a.updown_c = b.updown
          ORDER BY ic_no
        );
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _make_ic_service_info(self):
        '''IC出口服务情报'''
        self.log.info('Make IC Service Info.')
        sqlcmd = '''
        INSERT INTO highway_service_info(
                      ic_no, dog_run, hwy_oasis,
                      public_telephone, vending_machine, handicapped_telephone,
                      handicapped_toilet, information, snack_corner,
                      nursing_room, gas_station, launderette,
                      coin_shower, toilet, rest_area,
                      shopping_corner, fax_service, postbox,
                      hwy_infor_terminal, atm, hwy_hot_springs,
                      nap_rest_area, restaurant)
        (
        SELECT distinct ic_no, 0 dog_run, 0 hwy_oasis,
               0 public_telephone, 0 vending_machine, 0 handicapped_telephone,
               0 handicapped_toilet, information, 0 snack_corner,
               0 nursing_room, gas_station, 0 launderette,
               0 coin_shower, toilet, rest_area,
               shopping_corner, 0 fax_service, postbox,
               0 hwy_infor_terminal, atm, 0 hwy_hot_springs,
               0 nap_rest_area, restaurant
          FROM hwy_ic_service AS a
          LEFT JOIN mid_hwy_ic_no AS b
          ON a.road_code = b.road_code and
             a.road_seq = b.road_seq and
             a.updown_c = b.updown and
             a.node_id = b.node_id
          ORDER BY ic_no
        );
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
