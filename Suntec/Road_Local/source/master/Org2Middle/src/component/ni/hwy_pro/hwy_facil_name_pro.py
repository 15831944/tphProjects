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
        self.CreateTable2('temp_hwy_facil_name_ni')
        return 0

    def _DoCreateFunction(self):
        return 0

    def _DoCreateIndex(self):
        return 0

    def _Do(self):
        # 备份urban jct入口设施
        self._urban_jct_enter()
        # 备份非点JCT名称，JCT出口没有名称入口有名称
        self._jct_exit_name()
        # 制作设施名称
        self._make_hwy_facil_name()
        # ## 收费站名称
        # 收费站POI与node的关系表
        self._make_tollgate_poi_node_relation()
        self._make_tollgate_name()

    def _jct_exit_name(self):
        '''备份非点JCT名称，JCT出口没有名称入口有名称'''
        self.log.info('Start make temp_hwy_jct_exit_name_ni')
        self.CreateTable2('temp_hwy_jct_exit_name_ni')
        sqlcmd = '''
        INSERT INTO temp_hwy_jct_exit_name_ni
        (
            SELECT distinct junc.s_junc_pid::bigint, fname.seq_nm::bigint,
                   fname.language::integer, fname.name
            FROM(
                   select distinct s_junc_pid::bigint, e_junc_pid::bigint,
                          c.seq_nm::bigint
                   FROM org_hw_jct AS a
                   LEFT JOIN mid_hwy_org_hw_junction_mid_linkid as j
                   ON a.s_junc_pid::bigint = j.id
                   inner JOIN link_tbl
                   on j.inlinkid = link_id and road_type = 0
                   LEFT JOIN org_hw_fname as b
                   ON a.s_junc_pid::bigint = b.featid::bigint
                   left join org_hw_fname as c
                   ON a.e_junc_pid::bigint = c.featid::bigint
                   where b.name is null and c.name is not null and
                     passlid is not null and passlid <> '' and
                     (c.name like '%互通%' or
                      c.name like '%立交%' or
                      c.name like '%枢纽%' or
                      c.name like '%连接线%' or
                      c.name like '%联络线%' or
                      c.name like '%桥%'
                      )--name_match_str--c.name like '%公路'
                )as junc
            inner join org_hw_fname as fname
            on junc.e_junc_pid = fname.featid::bigint and
               junc.seq_nm = fname.seq_nm::bigint
            order by junc.s_junc_pid::bigint, fname.seq_nm::bigint,
                  fname.language::integer
        )
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.log.info('End make temp_hwy_jct_exit_name_ni')

    def _urban_jct_enter(self):
        '''备份urban jct入口设施(urban jct:一头高速本线，一头城市高速)'''
        self.log.info('Start make temp_hwy_urban_jct_enter_ni')
        self.CreateTable2('temp_hwy_urban_jct_enter_ni')
        sqlcmd = '''
        INSERT INTO temp_hwy_urban_jct_enter_ni(junc_id)
        (
            select e_junc_pid::bigint as junc_pid
            from(
                select a.id, s_junc_pid, s_junc.inlinkid,
                       e_junc_pid, e_junc.outlinkid
                from org_hw_jct as a
                left join mid_hwy_org_hw_junction_mid_linkid as s_junc
                on a.s_junc_pid::bigint = s_junc.id
                left join mid_hwy_org_hw_junction_mid_linkid as e_junc
                on a.e_junc_pid::bigint = e_junc.id
                )as b
            left join link_tbl as link1
            on b.inlinkid = link1.link_id
            left join link_tbl as link2
            on b.outlinkid = link2.link_id
            where link1.road_type is not null and
                  link2.road_type is not null and
                  link1.road_type = 1 and link2.road_type = 0
        )
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.log.info('End make temp_hwy_urban_jct_enter_ni')

    def _make_hwy_facil_name(self):
        self.log.info('start make hwy facil name')
        temp_file_obj = cache_file.open('facil_name')
        multi_name_obj = None
        alter_name_obj = None
        for one_junction_name in self._get_facil_name():
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
        self.pg.copy_from2(temp_file_obj, 'temp_hwy_facil_name_ni')
        self.pg.commit2()

        cache_file.close(temp_file_obj, True)
        self.log.info('end make hwy facil name')
        return 0

    def _store_name_to_temp_file(self, file_obj, featid, json_name):
        if file_obj:
            file_obj.write('%s\t%s\n' % (featid, json_name))
        return 0

    def _get_facil_name(self):
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
               select featid, seq_nm, language, name
               from(
                   -- 非JCT入口设施名称
                   select featid::bigint, seq_nm::bigint,
                          language::bigint, name
                   from org_hw_fname as a
                   where not exists(select * from org_hw_junction
                                    where attr = '4' and
                                    accesstype = '1' and
                                    id = a.featid)--过滤后条目数201280
                    union
                    -- urban jct入口设施名称
                    select featid::bigint, seq_nm::bigint,
                           language::bigint, name
                    from org_hw_fname as fname
                    inner join temp_hwy_urban_jct_enter_ni as jct_enter
                    on fname.featid::bigint = jct_enter.junc_id

                    union

                    select distinct junc_id, seq_nm, language, name
                    from temp_hwy_jct_exit_name_ni
                    --非点JCT且非urban jct名称，JCT出口没有名称入口有名称
                )as f_name
                order by featid, seq_nm, language
            )as org_hw_fname
            group by featid, seq_nm
            order by featid, seq_nm
        ) as m
        group by featid
        '''
        return self.pg.get_batch_data2(sqlcmd)

    def _make_tollgate_poi_node_relation(self):
        '''收费站与node关系'''
        self.CreateTable2('mid_temp_toll_poi_relation')
        sqlcmd = """
        INSERT INTO mid_temp_toll_poi_relation(poi_id, link_id, node_id, road_type)
        (
        SELECT poi_id, link_id, e.node_id, road_type
          FROM (
            SELECT DISTINCT poi_id, link_id, road_type,
                   (case when point <= 0.5 then s_node
                    else e_node end) as node_id
             FROM (
                 SELECT distinct poi_id, c.link_id, s_node, e_node, road_type,
                    ST_Line_Locate_Point(c.the_geom, a.the_geom) as point,
                    linkid
                  FROM (
                    SELECT poi_id::bigint, linkid::bigint, the_geom
                      FROM org_poi as a
                      where a.kind in ('230209') -- Tollgate POI
                  ) AS a
                  LEFT join mid_link_mapping as b
                  on linkid = b.org_link_id
                  LEFT JOIN link_tbl as c
                  ON b.link_id = c.link_id
              ) AS d
          ) AS e
          INNER join (
            SELECT node_id, toll_flag
              from node_tbl
              where toll_flag = 1
          ) as t
          on e.node_id = t.node_id
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('mid_temp_toll_poi_relation_node_id_idx')

    def _make_tollgate_name(self):
        '''高速收费站名称'''
        self.log.info('Start make hwy toll name')
        self.CreateTable2('hwy_tollgate_name')
        temp_file_obj = cache_file.open('temp_toll_facil_name')
        multi_name_obj = None
        alter_name_obj = None
        for one_junction_name in self._get_tollgate_names():
            (node_id, seq_nm_list, language_list,
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
                                              node_id,
                                              json_name)
        # ## 把名称导入数据库
        temp_file_obj.seek(0)
        self.pg.copy_from2(temp_file_obj, 'hwy_tollgate_name')
        self.pg.commit2()

        cache_file.close(temp_file_obj, True)
        self.log.info('end make hwy toll name')

    def _get_tollgate_names(self):
        sqlcmd = """
        SELECT node_id, array_agg(seq_nm),
               array_agg(language), array_agg(names)
          from (
            SELECT node_id, seq_nm,
                   array_to_string(array_agg(language), '|')as language,
                   array_to_string(array_agg(name), '|') as names
              FROM (
                SELECT node_id, seq_nm, name, language
                  FROM mid_temp_toll_poi_relation AS a
                  INNER JOIN org_pname as b
                  ON poi_id = featid::bigint and b.nametype = '9'
                  where road_type = 0
                  order by node_id, seq_nm, language
              ) AS c
              GROUP BY node_id, seq_nm
              order by node_id, seq_nm
          ) as d
          group by node_id
          order by node_id
        """
        return self.get_batch_data(sqlcmd)
