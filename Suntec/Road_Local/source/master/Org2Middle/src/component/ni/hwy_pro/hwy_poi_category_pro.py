# -*- coding: UTF-8 -*-
'''
Created on 2015-9-7

@author: PC_ZH
'''
from component.rdf.hwy.hwy_poi_category_rdf import HwyPoiCategory


class HwyPoiCategoryPro(HwyPoiCategory):

    def __init__(self):
        HwyPoiCategory.__init__(self, ItemName='HwyPoiCategoryPro')

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

    def _get_service_code(self):
        sqlcmd = '''
        select service,  org_code1
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

        select service, org_code1
        from (
                select service, category_id, genre, gen1, gen2
                from hwy_service_category_mapping as m
                left join temp_poi_category as p
                on m.category_id = p.per_code
                --on m.service_name = p.name
                where genre = '2nd genre'
             )as c
        right join temp_poi_category as d
        on c.gen1 = d.gen1 and c.gen2 = d.gen2

        union
        select service,  org_code2
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

        select service, org_code2
        from (
                select service, category_id, genre, gen1, gen2
                from hwy_service_category_mapping as m
                left join temp_poi_category as p
                on m.category_id = p.per_code
                where genre = '2nd genre'
             )as c
        right join temp_poi_category as d
        on c.gen1 = d.gen1 and c.gen2 = d.gen2

        '''
        return self.get_batch_data(sqlcmd)
