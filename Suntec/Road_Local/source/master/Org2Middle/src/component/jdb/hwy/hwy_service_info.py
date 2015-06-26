# -*- coding: UTF-8 -*-
'''
Created on 2014-5-13

@author: hongchenzai
'''
import component.component_base


class HwyServiceInfo(component.component_base.comp_base):
    '''
    Service Info (服务情报)
    '''

    def __init__(self,):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'HwyServiceInfo')

    def _Do(self):
        self._make_service_info()
        self._update_service_flag()
        return 0

    def _make_service_info(self):
        self.CreateTable2('highway_service_info')
        sqlcmd = '''
        INSERT INTO highway_service_info(
                            ic_no, dog_run, hwy_oasis,
                            public_telephone, vending_machine, handicapped_telephone,
                            handicapped_toilet, information, snack_corner,
                            nursing_room, launderette, coin_shower,
                            toilet, rest_area, shopping_corner,
                            fax_service, postbox, hwy_infor_terminal,
                            atm, hwy_hot_springs, nap_rest_area,
                            restaurant)
        (
        SELECT ic_no, dogrun_f, hwyoasis_f,
               0 as public_telephone, 0 as vending_machine, 0 as handicapped_telephone,
               handicap_f, infodesk_f as information, cafeteria_f,
               0 as nursing_room, laundry_f, shower_f,
               toilet_f, resting_f, souvenir_f as shopping_corner,
               fax_f, mailbox_f, information_f as hwy_infor_terminal,
               cashservice_f, spa_f, nap_f,
               restaurant_f
          FROM access_point_4326 as srv
          LEFT JOIN (
                SELECT distinct ic_no, road_code, road_point, updown
                  FROM mid_hwy_ic_no
          ) as ic
          ON srv.road_code = ic.road_code
             and srv.road_seq = ic.road_point
             and srv.direction_c = ic.updown
          where hwymode_f = 1 and (
               dogrun_f = 1 or hwyoasis_f = 1 or
               handicap_f = 1 or information_f = 1 or cafeteria_f = 1 or
               laundry_f = 1 or shower_f = 1 or
               toilet_f = 1 or resting_f = 1 or souvenir_f = 1 or
               fax_f = 1 or mailbox_f = 1 or infodesk_f = 1 or
               cashservice_f = 1 or spa_f = 1 or nap_f = 1 or
               restaurant_f = 1)
               and ic_no is not null
          order by ic_no
        );
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _update_service_flag(self):
        '''更新IC情报表的service_flag'''
        sqlcmd = """
        UPDATE highway_ic_info
           SET servise_flag= 1
         WHERE ic_no in (
                SELECT ic_no from highway_service_info
         );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
