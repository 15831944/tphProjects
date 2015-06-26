# -*- coding: UTF8 -*-
'''
Created on 2013-10-10

@author: hongchenzai
'''
from base.rdb_ItemBase import ItemBase


class rdb_highway_sapa_info(ItemBase):
    '''SAPA信息。包括：插图，服务信息
    '''

    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self, 'Highway_SAPA')

    def Do(self):
        # 插图
        self._MakeIllust()
        # SAPA服务标志位
        self._MakeServiceInfo()
        return 0

    def _MakeIllust(self):
        'SAPA插图'
        self.CreateTable2('rdb_highway_illust_info')
        if self.pg.IsExistTable('highway_illust_info') != True:
            return 0
        sqlcmd = """
        INSERT INTO rdb_highway_illust_info(ic_no, image_id, tile_id)
        (
        select a.ic_no, b.gid as image_id, tile_id
          from highway_illust_info as a
          LEFT JOIN rdb_guideinfo_pic_blob_bytea as b
          on lower(a.image_id) = lower(b.image_id)
          LEFT JOIN rdb_highway_ic_info AS ic
          on a.ic_no = ic.ic_no
          order by ic_no, a.gid
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return 0

    def _MakeServiceInfo(self):
        'SAPA服务标志位。'
        self.CreateTable2('rdb_highway_service_info')
        if self.pg.IsExistTable('highway_service_info') != True:
            return 0
        sqlcmd = """
        INSERT INTO rdb_highway_service_info(
                    ic_no, dog_run, hwy_oasis,
                    public_telephone, vending_machine, handicapped_telephone,
                    handicapped_toilet, information, snack_corner,
                    nursing_room, gas_station, launderette,
                    coin_shower, toilet, rest_area,
                    shopping_corner, fax_service, postbox,
                    hwy_infor_terminal, atm, hwy_hot_springs,
                    nap_rest_area, restaurant, tile_id)
        (
        SELECT a.ic_no, dog_run, hwy_oasis,
               public_telephone, vending_machine, handicapped_telephone,
               handicapped_toilet, information, snack_corner,
               nursing_room, gas_station, launderette,
               coin_shower, toilet, rest_area,
               shopping_corner, fax_service, postbox,
               hwy_infor_terminal, atm, hwy_hot_springs,
               nap_rest_area, restaurant, tile_id
          FROM highway_service_info AS a
          LEFT JOIN highway_ic_info AS b
          ON a.ic_no = b.ic_no
          order by ic_no, a.gid
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('rdb_highway_service_info_ic_no_idx')
        return 0
