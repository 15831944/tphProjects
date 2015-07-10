# -*- coding: UTF8 -*-
'''
Created on 2015��4��30��

@author: PC_ZH
'''
from component.rdf.hwy.hwy_exit_poi_name_rdf import HwyPoiExitNameRDF
from component.rdf.multi_lang_name_rdf import MultiLangNameRDF
from component.component_base import comp_base
from common import cache_file

LANG_CODE_DICT = {'1': 'CHI', '3': 'ENG'}

class HwyExitSapaNameNi(HwyPoiExitNameRDF):
    ''' '''
    def __init__(self):
        ''' '''
        HwyPoiExitNameRDF.__init__(self)
        return

    def _DoCreateTable(self):
        self.CreateTable2('mid_temp_hwy_exit_poi_name')
        return 0

    def _DoCreateIndex(self):
        return 0

    def _DoCreateFunction(self):
        return 0

    def _Do(self):
        from component.default.dictionary import comp_dictionary
        dictionary = comp_dictionary()
        dictionary.set_language_code()

        self._make_exit_sapa_name()
        return 0

    def _make_exit_sapa_name(self):
        ''' '''
        self.log.info('start make exit sapa name...')
        temp_file_obj = cache_file.open('temp_exit_sapa_name')
        for nameinfo in self._get_exit_sapa_name():
            node_id = nameinfo[0]
            poi_id = nameinfo[1]
            poi_name = nameinfo[2]
            lang = nameinfo[3]

            name_list = poi_name.split('|')
            lang_list = lang.split('|')
            #以下！处理lang_code转换
            list_lan = list()
            for lan in lang_list:
                list_lan.append(LANG_CODE_DICT[lan])
            official_name = name_list[0]
            lang_code = list_lan[0]
            trans_langs = list_lan[1:]
            trans_names = name_list[1:]

            if not node_id:
                self.log.error('no node id')
                continue

            multi_name_obj = MultiLangNameRDF(lang_code, official_name)
            multi_name_obj = self.set_trans_name(multi_name_obj,
                                                 None,
                                                 trans_names,
                                                 trans_langs,
                                                 None,
                                                 None
                                                 )
            json_name = multi_name_obj.json_format_dump()
            if not json_name:
                self.log.error('no json name. node id is %s' % node_id)

            self._store_name_to_temp_file(temp_file_obj, node_id, poi_id,
                                          json_name)

        temp_file_obj.seek(0)
        self.pg.copy_from2(temp_file_obj, 'mid_temp_hwy_exit_poi_name')
        self.pg.commit2()
        cache_file.close(temp_file_obj, True)

        self.log.info('end make exit sapa name...')
        return 0

    def _get_exit_sapa_name(self):
        ''' '''
        sqlcmd = '''
        SELECT org_n.id AS node_id, poi_id, name, lang_code
        FROM org_poi
        LEFT JOIN
        (
            SELECT featid,
                   array_to_string(array_agg(name),'|')as name,
                   array_to_string(array_agg(language),'|')as lang_code
                   --array_to_string(array_agg(py),'|') as py
            FROM org_pname
            WHERE nametype = '9' -- 9: POI
            GROUP BY featid
        )AS pname
        ON poi_id = featid
        LEFT JOIN org_r
        ON linkid = id
        LEFT JOIN org_n
        ON ST_DWithin(org_n.the_geom, org_poi.the_geom,
                      0.000002694945852358566745)
        -- 8301: exit, 8302: enter, 8380: sa, 8381: PA
        WHERE org_poi.kind IN ('8301', '8302', '8380', '8381')
        ORDER BY node_id, poi_id

        '''
        return self.get_batch_data(sqlcmd)

    def _store_name_to_temp_file(self, file_obj, node_id, poi_id,
                                 json_name):
        ''' '''
        if file_obj:
            file_obj.write('%s\t%s\t%s\n' % (node_id, poi_id, json_name))
        return 0
