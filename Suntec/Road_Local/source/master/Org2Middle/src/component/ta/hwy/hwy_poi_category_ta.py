# -*- coding: UTF-8 -*-
'''
Created on 2015-9-8

@author: PC_ZH
'''
from component.rdf.hwy.hwy_poi_category_rdf import HwyPoiCategory


class HwyPoiCategoryTa(HwyPoiCategory):

    def __init__(self):
        HwyPoiCategory.__init__(self, ItemName='HwyPoiCategoryTa')

    def _get_service_code(self):
        sqlcmd = '''
        select service,  per_code as category_id
        from(
                select service, category_id, genre, gen1, gen2
                from hwy_service_category_mapping as m
                left join temp_poi_category as p
                on m.category_id = p.per_code
                where genre = '1st genre'
                )as a
        right join temp_poi_category as b
        on a.gen1 = b.gen1

        union

        select service,  per_code as category_id
        from (
                select service, category_id, genre, gen1, gen2
                from hwy_service_category_mapping as m
                left join temp_poi_category as p
                on m.category_id = p.per_code
                where genre = '2nd genre'
             )as c
        right join temp_poi_category as d
        on c.gen1 = d.gen1 and c.gen2 = d.gen2;
        '''
        return self.get_batch_data(sqlcmd)
