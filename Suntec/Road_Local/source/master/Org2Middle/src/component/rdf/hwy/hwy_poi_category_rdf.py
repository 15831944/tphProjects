# -*- coding: UTF-8 -*-
'''
Created on 2015-9-7

@author: PC_ZH
'''
import common
import xlrd
import component.component_base
HWY_SR_RESTAURANT = 'Restaurant'
HWY_SR_REST_AREA = 'Rest area'
HWY_SR_SHOPPING_CORNER = 'Shopping corner'
HWY_SR_GAS_STATION = 'Gas station'
HWY_SR_POST_BOX = 'Post box'
HWY_SR_INFO = 'Info'
HWY_SR_TOILET = 'Toilet'
HWY_SR_ATM = 'ATM'


class HwyPoiCategory(component.component_base.comp_base):

    def __init__(self, ItemName='HwyPoiCategory'):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, ItemName)

    def _Do(self):
        self._make_poi_category_mapping()

    def _insert_service_category_mapping(self, para):
        sqlcmd = '''
        INSERT INTO hwy_service_category_mapping(service,
                                                  genre,
                                                  service_name,
                                                  category_id)
        VALUES(%s, %s, %s, %s)
        '''
        self.pg.execute2(sqlcmd, para)

    def _get_category_id(self, service_name, genre):
        if genre == '1st genre':
            sqlcmd = '''
            select distinct per_code
            from temp_poi_category
            where trim(name) = %s and
                  gen2 = 0
            '''
        elif genre == '2nd genre':
            sqlcmd = '''
            select distinct per_code
            from temp_poi_category
            where trim(name) = %s and
                  gen3 = 0
            '''
        else:
            sqlcmd = '''
            select distinct per_code
            from temp_poi_category
            where trim(name) = %s
            '''
        self.pg.execute2(sqlcmd, (service_name,))
        category_id = self.pg.fetchall2()
        if category_id:
            if len(category_id) > 1:
                self.log.error('more than one category_id,service_name=%s'
                                % service_name)
                return None
            return category_id[0]
        return None

    def _make_poi_category_mapping(self):
        '''load poi_category.xls into temp_poi_category_mapping '''
        obj_config = common.config.CConfig.instance()
        poi_category_path = obj_config.getPara('hwy_poi_category')
        if poi_category_path:
            self.CreateTable2('hwy_service_category_mapping')
            wb = xlrd.open_workbook(poi_category_path)
            ws = wb.sheet_by_name('config')
            for row_index in xrange(1, ws.nrows):
                row_data = list()
                row_data = ws.row_values(row_index)
                if row_data:
                    service = row_data[0].strip()
                    genre = row_data[1].strip()
                    service_name = row_data[2].strip()
                    category_id = self._get_category_id(service_name, genre)
                    if not category_id:
                        if not service_name:
                            self.log.warning('No category id of service_name '
                                             '= %s' % service)
                        else:
                            self.log.error('No category id of service '
                                           '= %s' % service_name)
                    self._insert_service_category_mapping((service,
                                                           genre,
                                                           service_name,
                                                           category_id))
            self.pg.commit2()
        else:
            self.log.error('Dose not indicate poi_category_path.')

    def get_all_service_dict(self):
        restaurant_dict = dict()
        gas_station_dict = dict()
        rest_area_dict = dict()
        shopping_corner_dict = dict()
        post_box_dict = dict()
        atm_dict = dict()
        toilet_dict = dict()
        info_dict = dict()
        undefined_dict = dict()
        for row in self._get_service_info():
            (service, code) = row
            if not code:
                continue
            if service == HWY_SR_RESTAURANT:
                if code not in restaurant_dict.keys():
                    restaurant_dict[code] = service
            elif service == HWY_SR_GAS_STATION:
                if code not in gas_station_dict.keys():
                    gas_station_dict[code] = service
            elif service == HWY_SR_REST_AREA:
                if code not in rest_area_dict.keys():
                    rest_area_dict[code] = service
            elif service == HWY_SR_SHOPPING_CORNER:
                if code not in shopping_corner_dict.keys():
                    shopping_corner_dict[code] = service
            elif service == HWY_SR_POST_BOX:
                if code not in post_box_dict.keys():
                    post_box_dict[code] = service
            elif service == HWY_SR_INFO:
                if code not in info_dict.keys():
                    info_dict[code] = service
            elif service == HWY_SR_TOILET:
                if code not in toilet_dict.keys():
                    toilet_dict[code] = service
            elif service == HWY_SR_ATM:
                if code not in atm_dict.keys():
                    atm_dict[code] = service
            else:
                if code not in undefined_dict.keys():
                    undefined_dict[code] = service
        return (restaurant_dict, gas_station_dict,
                rest_area_dict, shopping_corner_dict,
                post_box_dict, info_dict, toilet_dict,
                atm_dict, undefined_dict)

    def _get_service_code(self):
        sqlcmd = '''
        select service,  org_code
        from(
                select service, category_id, genre, gen1, gen2
                from hwy_service_category_mapping as m
                left join temp_poi_category as p
                on m.service_name = p.name
                where genre = '1st genre'
                )as a
        right join temp_poi_category as b
        on a.gen1 = b.gen1

        union

        select service, org_code
        from (
                select service, category_id, genre, gen1, gen2
                from hwy_service_category_mapping as m
                left join temp_poi_category as p
                on m.service_name = p.name
                where genre = '2nd genre'
             )as c
        right join temp_poi_category as d
        on c.gen1 = d.gen1 and c.gen2 = d.gen2
        '''
        return self.get_batch_data(sqlcmd)

    def _get_service_info(self):
        self.log.info('Get service Info.')
        for row in self._get_service_code():
            service = row[0]
            org_code = row[1]
            if org_code:
                yield service, org_code
