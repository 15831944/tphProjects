# -*- coding: UTF8 -*-
'''
Created on 2015-5-26

@author: hcz
'''
from component.rdf.hwy.hwy_exit_poi_name_rdf import HwyPoiExitNameRDF
from component.rdf.multi_lang_name_rdf import MultiLangNameRDF
from common import cache_file
LANG_CODE_DICT = {'1': 'CHI', '3': 'ENG'}
POI_TYPE_DICT = {'8301': 'Exit', '8302': 'enter'}


class HwyExitEnterNameNi(HwyPoiExitNameRDF):
    '''
    Exit/Enter POI name.
    '''

    def __init__(self, item_name='HwyExitEnterNameNi'):
        '''
        Constructor
        '''
        HwyPoiExitNameRDF.__init__(self, item_name)

    def _DoCreateTable(self):
        self.CreateTable2('mid_temp_hwy_exit_enter_poi_name_ni')
        return 0

    def _DoCreateIndex(self):
        return 0

    def _DoCreateFunction(self):
        return 0

    def _Do(self):
        self.CreateIndex2('org_poi_poi_id_idx')
        self.CreateIndex2('org_n_the_geom_idx')
        # 出入口名称
        self._make_enter_exit_name()
        return 0

    def _make_enter_exit_name(self):
        '''出入口名称'''
        self.log.info('start make exit sapa name...')
        temp_file_obj = cache_file.open('temp_exit_sapa_name')
        for nameinfo in self._get_enter_exit_name():
            node_id = nameinfo[0]
            poi_id = nameinfo[1]
            poi_kind = nameinfo[2]
            poi_names = nameinfo[3]
            lang_codes = nameinfo[4]
            if not node_id:
                self.log.error('no node id')
                continue
            kind = POI_TYPE_DICT.get(poi_kind)
            json_name = self._get_json_name(lang_codes, poi_names)
            if not json_name:
                self.log.error('No json name. node id is %s' % node_id)
            self._store_name_to_temp_file(temp_file_obj, node_id,
                                          poi_id, kind, json_name)
        temp_file_obj.seek(0)
        self.pg.copy_from2(temp_file_obj,
                           'mid_temp_hwy_exit_enter_poi_name_ni')
        self.pg.commit2()
        cache_file.close(temp_file_obj, True)
        self.log.info('end make exit sapa name...')
        return 0

    def _get_json_name(self, lang_codes, poi_names):
        name_list = poi_names.split('|')
        lang_list = lang_codes.split('|')
        # 以下！处理lang_code转换
        list_lan = list()
        for lan in lang_list:
            list_lan.append(LANG_CODE_DICT[lan])
        official_name = name_list[0]
        lang_code = list_lan[0]
        trans_langs = list_lan[1:]
        trans_names = name_list[1:]
        multi_name_obj = MultiLangNameRDF(lang_code, official_name)
        multi_name_obj = self.set_trans_name(multi_name_obj,
                                             None,
                                             trans_names,
                                             trans_langs,
                                             None,
                                             None
                                             )
        json_name = multi_name_obj.json_format_dump()
        return json_name

    def _get_enter_exit_name(self):
        ''' '''
        sqlcmd = '''
        SELECT node_id, poi_id, kind,
               array_to_string(array_agg(name),'|') as names,
               array_to_string(array_agg(language),'|')as lang_codes
          FROM (
                SELECT org_n.id AS node_id, poi_id, org_poi.kind,
                       name, language
                FROM org_poi
                LEFT JOIN org_pname
                ON poi_id = featid
                LEFT JOIN org_r
                ON linkid = id
                LEFT JOIN org_n
                ON ST_DWithin(org_n.the_geom, org_poi.the_geom,
                              0.000002694945852358566745)
                -- 8301: exit, 8302: enter
                WHERE org_poi.kind IN ('8301', '8302')
                    and nametype = '9' -- 9: POI
                ORDER BY org_n.id, poi_id, org_poi.kind, language
          ) AS A
          GROUP BY node_id, poi_id, kind
        '''
        return self.get_batch_data(sqlcmd)

    def _store_name_to_temp_file(self, file_obj, node_id,
                                 poi_id, kind, json_name):
        ''' '''
        if file_obj:
            file_obj.write('%s\t%s\t%s\t%s\n' % (node_id, poi_id,
                                                 kind, json_name))
        return 0
