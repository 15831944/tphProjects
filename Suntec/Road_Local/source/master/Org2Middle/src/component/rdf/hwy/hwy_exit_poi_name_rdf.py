# -*- coding: UTF8 -*-
'''
Created on 2015-4-1

@author: zhangheng
'''
from component.rdf.hwy.hwy_exit_name_rdf import HwyExitNameRDF
from component.rdf.multi_lang_name_rdf import MultiLangNameRDF
from common import cache_file


class HwyPoiExitNameRDF(HwyExitNameRDF):
    '''HWY Exit POI Name.
    '''
    # vce_language_code_map = None

    def __init__(self):
        '''
        Constructor
        '''
        HwyExitNameRDF.__init__(self)

    def _DoCreateTable(self):
        self.CreateTable2('mid_temp_hwy_exit_poi_name_trans_group')
        self.CreateTable2('mid_temp_hwy_exit_poi_name')
        return 0

    def _DoCreateFunction(self):
        return 0

    def _DoCreateIndex(self):
        # self.CreateIndex2('mid_temp_hwy_exit_name_link_id_idx')
        return 0

    def _Do(self):

        from component.default.dictionary import comp_dictionary
        dictionary = comp_dictionary()
        dictionary.set_language_code()
        self._make_hwy_poi_name_group()
        self._make_node_facil()
        self._make_hwy_exit_poi_name()
        return 0

    def _make_hwy_poi_name_group(self):
        '''对同个名称(包括番号)的多个翻译进行group by. 名称之间用"|"分开。'''
        self.log.info('start make mid_temp_hwy_exit_poi_name_trans_group')
        sqlcmd = """
        INSERT INTO mid_temp_hwy_exit_poi_name_trans_group(
                        name_id, trans_type, trans_name)
        (
        SELECT name_id,
               array_to_string(array_agg(trans_type), '|') AS trans_types,
               array_to_string(array_agg(trans_name), '|') AS trans_names
          FROM (
                SELECT name_id,
                       transliteration_type AS trans_type,
                       name as trans_name
                  FROM rdf_poi_name_trans
                  order by name_id, transliteration_type
          ) AS A
          GROUP BY name_id
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.log.info('end make mid_temp_hwy_exit_poi_name_trans_group')
        return

    def _make_hwy_exit_poi_name(self):
        '''
        '''
        self.log.info('start make mid_temp_hwy_exit_poi_name')
        sqlcmd = '''
        SELECT distinct node_id, poi.poi_id,
                rdf_poi_name.language_code,
                rdf_poi_name.name,
                trans_group.trans_type,
                trans_group.name
         FROM mid_temp_hwy_exit_poi as poi
         LEFT JOIN rdf_poi_names
         ON poi.poi_id = rdf_poi_names.poi_id
         LEFT JOIN rdf_poi_name
         ON rdf_poi_names.name_id = rdf_poi_name.name_id
         LEFT JOIN mid_temp_hwy_exit_poi_name_trans_group AS trans_group
         ON rdf_poi_names.name_id = trans_group.name_id
         LEFT JOIN rdf_poi_address
         ON poi.poi_id = rdf_poi_address.poi_id
         LEFT JOIN wkt_location
         ON rdf_poi_address.location_id = wkt_location.location_id
         LEFT JOIN node_tbl
         ON  ST_DWithin(node_tbl.the_geom, poi.the_geom,
                        0.000002694945852358566745)
         ORDER BY node_id, poi.poi_id, language_code
        '''
        temp_file_obj = cache_file.open('hwy_exit_poi_name')
        names = self.get_batch_data(sqlcmd)
        multi_name = None
        pre_node_id = None
        curr_node_id = None
        for nameinfo in names:
            curr_node_id = nameinfo[0]
            poi_id = nameinfo[1]
            lang_code = nameinfo[2]
            official_name = nameinfo[3]
            trans_type = nameinfo[4]
            trans_name = nameinfo[5]
            if not curr_node_id:  # POI id不存在
                self.log.error("No node ID.")
                continue
            if pre_node_id != curr_node_id:
                if pre_node_id:
                    json_name = multi_name.json_format_dump()
                    if not json_name:
                        self.log.error('Json Name is none. node_id=%d'
                                       % curr_node_id)

                    self._store_name_to_temp_file(temp_file_obj, pre_node_id,
                                                  poi_id, json_name)
                # 初始化
                multi_name = MultiLangNameRDF(lang_code, official_name)
                multi_name = self.set_trans_name(multi_name, None, trans_name,
                                                 trans_type, None, None)
                pre_node_id = curr_node_id

            else:
                alter_name = MultiLangNameRDF(lang_code, official_name)
                alter_name = self.set_trans_name(alter_name, None, trans_name,
                                                 trans_type, None, None)
                multi_name.add_alter(alter_name)

        # ## 最后一个node的名称
        if multi_name:
            json_name = multi_name.json_format_dump()
            if not json_name:
                self.log.error('Json Name is none. nodeid=%s, poi_id=%s'
                               % (curr_node_id, poi_id))
            self._store_name_to_temp_file(temp_file_obj, pre_node_id,
                                          poi_id, json_name)
        # ## 把名称导入数据库
        temp_file_obj.seek(0)
        self.pg.copy_from2(temp_file_obj, 'mid_temp_hwy_exit_poi_name')
        self.pg.commit2()
        cache_file.close(temp_file_obj, True)
        self.log.info('end make mid_temp_hwy_exit_poi_name')
        return 0

    def _store_name_to_temp_file(self, file_obj, node_id, poi_id, json_name):
        if file_obj:
            file_obj.write('%d\t%d\t%s\n' % (node_id, poi_id, json_name))
        return 0
