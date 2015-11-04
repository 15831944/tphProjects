# -*- coding: UTF-8 -*-
'''
Created on 2015-10-27

@author: PC_ZH
'''
from common import cache_file
from component.component_base import comp_base
from component.rdf.multi_lang_name_rdf import MultiLangNameRDF
from component.rdf.hwy.hwy_exit_name_rdf import HwyExitNameRDF
from component.ni.hwy_pro.hwy_facil_name_pro import LANG_CODE_DICT


class HwyLineNameNiPro(HwyExitNameRDF):

    def __init__(self, item_name='HwyPathNameNiPro'):
        comp_base.__init__(self, item_name)

    def _DoCreateTable(self):
        self.CreateTable2('mid_temp_hwy_path_name_ni')
        return 0

    def _DoCreateFunction(self):
        return 0

    def _DoCreateIndex(self):
        return 0

    def _Do(self):
        # 创建语言种别的中间表
        # self._InsertLanguages()
        # 设置语言code
        from component.default.dictionary import comp_dictionary
        dictionary = comp_dictionary()
        dictionary.set_language_code()

        self._make_hwy_path_name()

    def _make_hwy_path_name(self):
        ''' make hwy path name'''
        self.log.info('start make hwy path name')
        temp_file_obj = cache_file.open('path_name')
        multi_name_obj = None
        for row in self._get_hwy_path_name():
            path_id = row[0]
            lang_list = row[1]
            path_name_list = row[2]
            one_name_list = zip(lang_list, path_name_list)
            for index in xrange(0, len(one_name_list)):
                (org_lang, name) = one_name_list[index]
                lang = LANG_CODE_DICT.get(org_lang)
                if not lang:
                    self.log.error('Unknown language_code=%s' % org_lang)
                    continue
                if index == 0:
                    if lang not in ('CHI', 'CHT'):
                        self.log.error('no CHI or CHT name')
                    multi_name_obj = MultiLangNameRDF(lang, name)
                else:
                    multi_name_obj = self.set_trans_name(multi_name_obj,
                                                         None,
                                                         name,
                                                         lang,
                                                         None,  # phonetic
                                                         None   # lang_code
                                                         )
            if multi_name_obj:
                json_name = multi_name_obj.json_format_dump()
                if not json_name:
                    self.log.error('no json name')
                self._store_name_to_temp_file(temp_file_obj,
                                              path_id,
                                              json_name)
        # ## 把名称导入数据库
        temp_file_obj.seek(0)
        self.pg.copy_from2(temp_file_obj, 'mid_temp_hwy_path_name_ni')
        self.pg.commit2()

        cache_file.close(temp_file_obj, True)
        self.log.info('end make hwy path name')
        return 0

    def _get_hwy_path_name(self):
        sqlcmd = '''
        select distinct hw_pid, array_agg(language), array_agg(pathname)
        from(
            select distinct a.hw_pid::bigint, b.language::bigint, b.pathname
            from org_hw_info as a
            INNER join org_r_name as b
            on a.route_id = b.route_id
            order by a.hw_pid::bigint, b.language::bigint
           )as c
        group by hw_pid
        order by hw_pid
        '''
        return self.pg.get_batch_data2(sqlcmd)

    def _store_name_to_temp_file(self, file_obj, featid, json_name):
        if file_obj:
            file_obj.write('%s\t%s\n' % (featid, json_name))
        return 0
