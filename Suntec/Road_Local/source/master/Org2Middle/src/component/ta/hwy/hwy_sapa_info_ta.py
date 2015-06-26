# -*- coding: UTF8 -*-
'''
Created on 2015年3月20日

@author: PC_ZH
'''


from component.rdf.hwy.hwy_sapa_info_rdf import HwySapaInfoRDF
from component.ta.multi_lang_name_ta import MultiLangNameTa
from common import cache_file
from component.default.multi_lang_name import NAME_TYPE_OFFICIAL
from common.common_func import getProjCountry
DEFAULT_LANG_CODES = {'aus': 'ENG'}


class HwySapaInfoTa(HwySapaInfoRDF):
    '''hwy sapa info'''

    def __init__(self):
        '''construct'''
        HwySapaInfoRDF.__init__(self)

    def _DoCreateTalbe(self):
        self.CreateIndex2('mid_temp_sapa_store_info')
        self.CreateIndex2('mid_temp_sapa_link')
        self.CreateIndex2('mid_temp_hwy_sapa_name')
        return 0

    def _DoCreateFunction(self):

        return 0

    def _DoCreateIndex(self):

        return 0

    def _Do(self):
        from component.default.dictionary import comp_dictionary
        dictionary = comp_dictionary()
        dictionary.set_language_code()

        # 道路名称
        self._make_hwy_sapa_name()
        self._make_hwy_sapa_link()
        self._make_hwy_sapa_store_info()
        return 0

    def _make_hwy_sapa_link(self):
        self.log.info('Make mid_temp_sapa_link')
        self.CreateIndex2('org_mnpoi_pi_cltrpelid_idx')
        sqlcmd = '''
        INSERT INTO mid_temp_sapa_link(poi_id, link_id)
        (
          SELECT id, link_id
          FROM org_mnpoi_pi AS poi
          LEFT JOIN mid_link_mapping AS m
          ON poi.cltrpelid = m.org_link_id
          WHERE poi.feattyp not in (7230, 9920)
          ORDER BY poi.id
        )
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.log.info('End mid_temp_sapa_link')
        return 0

    def _make_hwy_sapa_store_info(self):
#         self.log.info('Make mid_temp_hwy_sapa_info')
#         sqlcmd = '''
#         INSERT INTO mid_temp_sapa_store_info(poi_id, child_poi_id, cat_id)
#         (
#             SELECT pr.belpoiid, pr.poiid, pr.poityp
#             FROM org_pipr AS pr
#         )
#         '''
#         self.pg.execute2(sqlcmd)
#         self.pg.commit2()
#         self.log.info('End make mid_temp_hwy_sapa_info')
        return 0

    def _make_hwy_sapa_name(self):
        self.log.info('Make mid_temp_hwy_sapa_name')
        sqlcmd = '''
        SELECT id, name, lancd
        FROM org_mnpoi_pi
        WHERE feattyp in (7358, 7395) AND name is not null
        '''
        temp_file_obj = cache_file.open('hwy_sapa_name')
        contry_name = getProjCountry()
        names = self.get_batch_data(sqlcmd)
        default_lang_code = DEFAULT_LANG_CODES.get(contry_name.lower())
        for name_info in names:
            poi_id = name_info[0]
            name = name_info[1]
            lang_code = name_info[2]
            name_type = NAME_TYPE_OFFICIAL
            if not name:
                self.log.warning('No name and number. id=%d' % poi_id)
                continue
            if not lang_code:
                lang_code = default_lang_code
            if not lang_code:
                self.log.error('language code is none.poi_id=%d' % poi_id)
            multi_name = MultiLangNameTa(poi_id,
                                         lang_code,
                                         name,
                                         name_type)

            json_name = multi_name.json_format_dump()
            if not json_name:
                self.log.error('Json Name is none. poi_id=%d' % poi_id)

            self._store_name_to_temp_file(temp_file_obj, poi_id,
                                          json_name)
        temp_file_obj.seek(0)
        self.pg.copy_from2(temp_file_obj, 'mid_temp_hwy_sapa_name')
        self.pg.commit2()

        cache_file.close(temp_file_obj, True)

        self.log.info('End make mid_temp_hwy_sapa_name')
        return 0
