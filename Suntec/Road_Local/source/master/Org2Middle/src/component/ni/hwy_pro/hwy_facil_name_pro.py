# -*- coding: UTF-8 -*-
'''
Created on 2015-10-26

@author: PC_ZH
'''
from common import cache_file
from component.rdf.multi_lang_name_rdf import MultiLangNameRDF
from component.rdf.hwy.hwy_exit_name_rdf import HwyExitNameRDF
LANG_CODE_DICT = {1: 'CHI',  # 中文简单体
                  2: 'CHT',  # 中文简繁体
                  3: 'ENG',  # 英文
                  }


class HwyFacilNameNiPro(HwyExitNameRDF):
    '''设施名称'''

    def __init__(self):
        HwyExitNameRDF.__init__(self, item_name='HwyFacilNameNiPro')

    def _DoCreateTable(self):
        self.CreateTable2('mid_temp_hwy_exit_name_ni')
        return 0

    def _DoCreateFunction(self):
        return 0

    def _DoCreateIndex(self):
        return 0

    def _Do(self):
        self._make_hwy_facil_name()

    def _make_hwy_facil_name(self):
        self.log.info('start make hwy exit name')
        temp_file_obj = cache_file.open('link_name')
        multi_name_obj = None
        alter_name_obj = None
        for one_junction_name in self._get_exit_name():
            # one_junction_name = ('3001', [1, 2, 3],
            #                       [1|3,1|3,1|3],
            #                       [经公桥|Jinggong Brg,Ｇ２０６|G206,东亚|Dongya])
            (featid, seq_nm_list, language_list,
             names_list) = one_junction_name
            name_list = zip(seq_nm_list, language_list, names_list)
            name_list.sort(cmp=lambda x, y: cmp(x[0], y[0]), reverse=False)
            for name_info in name_list:
                seq_nm = name_info[0]
                language = name_info[1].split('|')
                name = name_info[2].split('|')
                one_names = zip(language, name)
                one_names.sort(cmp=lambda x, y: cmp(x[0], y[0]), reverse=False)
                if int(seq_nm) == 1:
                    for index in xrange(0, len(one_names)):
                        (org_lang, name) = one_names[index]
                        lang = LANG_CODE_DICT.get(int(org_lang))
                        if not lang:
                            self.log.error('Unknown language code.')
                            continue
                        if index == 0:
                            if lang not in ('CHI', 'CHT'):
                                self.log.error('no CHI or CHT name')
                                break
                            multi_name_obj = MultiLangNameRDF(lang, name)
                        else:
                            multi_name_obj = self.set_trans_name(multi_name_obj,
                                                                 None,
                                                                 name,
                                                                 lang,
                                                                 None,  # phone
                                                                 None   # lang
                                                                 )
                else:
                    for index in xrange(0, len(one_names)):
                        (org_lang, name) = one_names[index]
                        lang = LANG_CODE_DICT.get(int(org_lang))
                        if not lang:
                            self.log.error('Unknown language_code=%s.'
                                           % org_lang)
                            continue
                        if index == 0:
                            if lang not in ('CHI', 'CHT'):
                                self.log.error('no CHI or CHT name')
                                break
                            alter_name_obj = MultiLangNameRDF(lang, name)
                        else:
                            alter_name_obj = self.set_trans_name(alter_name_obj,
                                                                 None,
                                                                 name,
                                                                 lang,
                                                                 None,  # phonetic
                                                                 None   # lang_code
                                                                 )
                    multi_name_obj.add_alter(alter_name_obj)
            if multi_name_obj:
                json_name = multi_name_obj.json_format_dump()
                if not json_name:
                    self.log.error('no json name o.O')
                # 存到本地的临时文件
                self._store_name_to_temp_file(temp_file_obj,
                                              featid,
                                              json_name)
        # ## 把名称导入数据库
        temp_file_obj.seek(0)
        self.pg.copy_from2(temp_file_obj, 'mid_temp_hwy_exit_name_ni')
        self.pg.commit2()

        cache_file.close(temp_file_obj, True)
        self.log.info('end make hwy exit name')
        return 0

    def _store_name_to_temp_file(self, file_obj, featid, json_name):
        if file_obj:
            file_obj.write('%s\t%s\n' % (featid, json_name))
        return 0

    def _get_exit_name(self):
        '''get junction id and junction name'''
        sqlcmd = '''
        select featid,
               array_agg(seq_nm),
               array_agg(language),
               array_agg(names)
        from(
            selecT featid, seq_nm,
                   array_to_string(array_agg(language), '|')as language,
                   array_to_string(array_agg(name), '|') as names
            from
            (
                   select featid::bigint, seq_nm::bigint,
                          language::bigint, name
                   from org_hw_fname
                   order by featid::bigint, seq_nm::bigint,
                            language::bigint
            )as org_hw_fname
            group by featid, seq_nm
            order by featid, seq_nm
        ) as m
        group by featid
        '''
        return self.pg.get_batch_data2(sqlcmd)
