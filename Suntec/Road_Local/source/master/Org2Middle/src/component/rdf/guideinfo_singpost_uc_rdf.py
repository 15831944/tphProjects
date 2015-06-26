# -*- coding: UTF8 -*-
'''
Created on 2013-12-23

@author: hongchenzai
'''
from common import cache_file
from component.rdf.multi_lang_name_rdf import MultiLangNameRDF
from component.rdf.multi_lang_name_rdf import MultiLangShieldRDF
from component.rdf.dictionary_rdf import comp_dictionary_rdf
from component.rdf.dictionary_rdf_hkgmac import comp_dictionary_rdf_hkg_mac
import common.common_func

from component.default.guideinfo_signpost_uc import comp_guideinfo_signpost_uc


class comp_guideinfo_signpost_uc_rdf(comp_guideinfo_signpost_uc):
    '''方面看板(海外)
    '''

    def __init__(self):
        '''
        Constructor
        '''
        comp_guideinfo_signpost_uc.__init__(self)

    def _DoCreateTable(self):
        self.CreateTable2('signpost_uc_tbl')
        return 0

    def _DoCreateIndex(self):
        # 由于中东link合并之后，会有inlink和outlink重叠的记录，
        # 造成代码执行过程出错，所以把重叠检查放到AutoCheck模块
        self.CreateIndex2('signpost_uc_tbl_inlinkid_nodeid_outlinkid_idx')
        return 0

    def _Do(self):
        # 初始化language code
        # from component.rdf.dictionary_rdf import comp_dictionary_rdf
        self.__dict_rdf = None
        if common.common_func.JudgementCountry('hkg','rdf') \
           or common.common_func.JudgementCountry('mac','rdf'):
            self.__dict_rdf = comp_dictionary_rdf_hkg_mac()
        else:
            self.__dict_rdf = comp_dictionary_rdf()
        self.__dict_rdf.set_language_code()
        # ## 方向名称和番号
        self._group_trans_name()
        self._make_signpost_element_phonetic()
        self._make_signpost_element()
        # ## 出口番号
        self._group_trans_exit_no()
        self._make_signpost_exit_no_phonetic()
        self._make_signpost_exit_no()
        # ## 制作PassLink
        # link最少
        # self._make_signpost_passlink()
        # 距离最短
        self._make_short_distance_passlink()
        # ## 把所有SignPost信息都做合起来
        self._merge_all_sigpost_info()
        # ## InLink往前推
        self._push_forward_inlink()
        return 0

    def _group_trans_name(self):
        '''一个名称对应的多个翻译，合并成一条记录，名称和名称之间，或者种别和种别间用‘|’分开。'''
        self.CreateTable2('mid_temp_sign_element_trans_group')
        sqlcmd = """
        INSERT INTO mid_temp_sign_element_trans_group(sign_element_id,
                            trans_types, trans_names)
        (
        SELECT sign_element_id,
               array_to_string(array_agg(trans_type), '|') as trans_types,
               array_to_string(array_agg("text"), '|') as trans_names
          FROM (
            SELECT sign_element_id,  "text",
                   transliteration_type as trans_type
              FROM rdf_sign_element_trans
              order by sign_element_id, transliteration_type, "text"
          ) AS A
          GROUP BY sign_element_id
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return 0

    def _make_signpost_element_phonetic(self):
        self.CreateTable2('mid_temp_sign_element_phonetic')
        sqlcmd = """
        INSERT INTO mid_temp_sign_element_phonetic(
                        sign_element_id,
                        phonetic_ids, phonetic_strings,
                        phonetic_language_codes)
        (
        SELECT sign_element_id,
               array_agg(phonetic_id) as phonetic_ids,
               array_agg(phonetic_string) as phonetic_strings,
               array_agg(phonetic_language_code) as phonetic_language_codes
          from (
                SELECT sign_element_id, vce_sign_element.phonetic_id,
                       preferred, phonetic_string,
                       phonetic_language_code, transcription_method
                  FROM vce_sign_element
                  left join vce_phonetic_text as phonetic
                  ON vce_sign_element.phonetic_id = phonetic.phonetic_id
                  order by sign_element_id,
                           phonetic_language_code,
                           preferred desc,
                           transcription_method desc,
                           phonetic_id
          ) as a
          group by sign_element_id
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _make_signpost_element(self):
        self.CreateTable2('mid_temp_signpost_element')
        temp_file_obj = cache_file.open('signpost_element')  # 创建临时文件
        # 排序：sign_id, destination_number(方向ID), entry_number
        # 同个方向上的名称做group (sign_id, destination_number)
        sqlcmd = """
            SELECT element.sign_id,
                   element.destination_number,
                   entry_number,
                   text_type,
                   element."text" as official_name,
                   element.language_code,
                   direction_code,
                   entry_type,
                   text_number,
                   trans_types,
                   trans_group.trans_names as trans_names,
                   iso_country_code,
                   phonetic_strings,
                   phonetic_language_codes
              FROM rdf_sign_element as element
              LEFT JOIN mid_temp_sign_element_trans_group as trans_group
              ON element.sign_element_id = trans_group.sign_element_id
              LEFT JOIN rdf_sign_destination AS dest
              ON element.sign_id = dest.sign_id and
                  element.destination_number = dest.destination_number
              LEFT JOIN temp_rdf_nav_link
              on dest_link_id = link_id
              LEFT JOIN mid_temp_sign_element_phonetic as phonetic
              ON element.sign_element_id = phonetic.sign_element_id
              order by sign_id, destination_number, entry_number
              ;
        """
        signs = self.get_batch_data(sqlcmd)
        from component.rdf.dictionary_rdf import INVALID_SHIELD_ID
        prev_sign_id = None
        curr_sign_id = None
        prev_dest_num = None
        curr_dest_num = None
        signpost = None
        signpost_name = None
#        self.__dict_rdf = comp_dictionary_rdf()
        for sign_info in signs:
            curr_sign_id = sign_info[0]
            curr_dest_num = sign_info[1]  # 方向ID
            # entry_number = sign_info[2]  # 同个方向上的名称序号
            text_type = sign_info[3]  # T: 名称， R: 番号
            official_name = sign_info[4]  # 官方语言下的文本
            lang_code = sign_info[5]  # 官方语言种别——ISO Country Code
            # dir_code = sign_info[6]  # 方向， E, S, W, N, '-'
            trans_types = sign_info[9]  # 翻译语言种别
            trans_names = sign_info[10]  # 翻译文本
            iso_country_code = sign_info[11]
            phonetic_strings = sign_info[12]
            phonetic_lang_codes = sign_info[13]
            if not curr_sign_id:
                self.log.error('None sign_id.')
                continue
            if not curr_dest_num:
                self.log.error('None destination_number. sign_id=%d'
                               % curr_sign_id)
                continue
            if (prev_sign_id != curr_sign_id or  # 不同的看板
                prev_dest_num != curr_dest_num):  # 不同的方向
                # 保存上一条
                if prev_dest_num:
                    signpost.set_signpost_name(signpost_name)
                    str_info = signpost.to_string()
                    self._store_name_to_temp_file(temp_file_obj, str_info)
                # 新的一条
                prev_sign_id = curr_sign_id
                prev_dest_num = curr_dest_num
                signpost = SignPostElement(curr_sign_id, curr_dest_num)
                signpost_name = None

            # 名称不存在，或为空
            if not official_name:
                self.log.error('None Name. sign_id=%d.' % prev_sign_id)
                continue
            # ## 番号
            if self._is_route_number(text_type):
                if iso_country_code == 'SGP':
                    shield_id = 4111
                else:
                    shield_id = INVALID_SHIELD_ID
                shield_number = official_name
                shield_obj = MultiLangShieldRDF(shield_id,
                                                shield_number,
                                                lang_code
                                                )
                shield_obj.set_trans(trans_names, trans_types)
                # ## shield不做TTS
#                 shield_obj = self.__dict_rdf.set_trans_name(shield_obj,
#                                                      iso_country_code,
#                                                      trans_names,
#                                                      trans_types,
#                                                      phonetic_strings,
#                                                      phonetic_lang_codes,
#                                                     )
                signpost.add_route_no(shield_obj)
            else:  # 名称
                name_type = 'office_name'
                if signpost_name:  # 同个方向上的其他名称
                    trans_name_obj = MultiLangNameRDF(lang_code,
                                                      official_name,
                                                      name_type)
                    if phonetic_strings:  # 有音素
                        tts_obj = self.__dict_rdf.get_tts_name(iso_country_code,
                                                        lang_code,
                                                        name_type,
                                                        phonetic_strings,
                                                        phonetic_lang_codes
                                                        )
                        if tts_obj:
                            trans_name_obj.set_tts(tts_obj)
                        else:
                            self.log.warning('No TTS.sign_id=%d,dest_number=%d'
                                              % (curr_sign_id, curr_dest_num))
                    trans_name_obj = self.__dict_rdf.set_trans_name(trans_name_obj,
                                                       iso_country_code,
                                                       trans_names,
                                                       trans_types,
                                                       phonetic_strings,
                                                       phonetic_lang_codes,
                                                       )
                    signpost_name.add_alter(trans_name_obj)
                else:  # 当前方向上的第一名称
                    signpost_name = MultiLangNameRDF(lang_code,
                                                     official_name,
                                                     name_type)
                    if phonetic_strings:  # 有音素
                        name_type = signpost_name.get_name_type()
                        tts_obj = self.__dict_rdf.get_tts_name(iso_country_code,
                                                        lang_code,
                                                        name_type,
                                                        phonetic_strings,
                                                        phonetic_lang_codes
                                                        )
                        if tts_obj:
                            signpost_name.set_tts(tts_obj)
                        else:
                            self.log.warning('No TTS.sign_id=%d,dest_number=%d'
                                              % (curr_sign_id, curr_dest_num))

                    signpost_name = self.__dict_rdf.set_trans_name(signpost_name,
                                                            iso_country_code,
                                                            trans_names,
                                                            trans_types,
                                                            phonetic_strings,
                                                            phonetic_lang_codes
                                                            )
        # 最后一条
        signpost.set_signpost_name(signpost_name)
        str_info = signpost.to_string()
        self._store_name_to_temp_file(temp_file_obj, str_info)

        # ## 把名称导入数据库
        temp_file_obj.seek(0)
        self.pg.copy_from2(temp_file_obj, 'mid_temp_signpost_element')
        self.pg.commit2()
        # close file
        #temp_file_obj.close()
        cache_file.close(temp_file_obj,True)
        return 0

    def _is_route_number(self, text_type):
        if text_type == 'R':
            return True
        elif text_type == 'T':
            return False
        else:
            return None

    def _group_trans_exit_no(self):
        self.CreateTable2('mid_temp_sign_destination_trans_group')
        sqlcmd = """
        INSERT INTO mid_temp_sign_destination_trans_group(sign_id,
                    destination_number, trans_exit_langs, trans_exit_nums)
        (
        SELECT sign_id, destination_number,
               array_to_string(array_agg(trans_type), '|') as trans_exit_langs,
               array_to_string(array_agg(exit_number), '|') as trans_exit_nums
          FROM (
            SELECT sign_id, destination_number,
                  transliteration_type as trans_type, exit_number
              FROM rdf_sign_destination_trans
              order by sign_id, destination_number, transliteration_type
          ) AS A
          GROUP BY sign_id, destination_number
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _make_signpost_exit_no_phonetic(self):
        self.CreateTable2('mid_temp_sign_destination_phonetic')
        sqlcmd = """
        INSERT INTO mid_temp_sign_destination_phonetic(
                        sign_id, destination_number,
                        phonetic_ids, phonetic_strings,
                        phonetic_language_codes)
        (
        SELECT sign_id, destination_number,
               array_agg(phonetic_id) as phonetic_ids,
               array_agg(phonetic_string) as phonetic_strings,
               array_agg(phonetic_language_code) as phonetic_language_codes
          from (
                SELECT sign_id, destination_number,
                       vce_sign_destination.phonetic_id, preferred,
                       phonetic_string, phonetic_language_code,
                       transcription_method
                  FROM vce_sign_destination
                  left join vce_phonetic_text as phonetic
                  ON vce_sign_destination.phonetic_id = phonetic.phonetic_id
                  order by sign_id, destination_number,
                           phonetic_language_code,
                           preferred desc,
                           transcription_method desc,
                           phonetic_id
          ) as a
          group by sign_id, destination_number
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _make_signpost_exit_no(self):
        self.CreateTable2('mid_temp_signpost_exit_no')
        temp_file_obj = cache_file.open('signpost_exit_no')  # 创建临时文件
        sqlcmd = """
        SELECT dest.sign_id, dest.destination_number,
               exit_number, language_code,
               alt_exit_number,
               trans_exit_langs,
               trans_exit_nums,
               iso_country_code,
               phonetic_strings,
               phonetic_language_codes
          FROM rdf_sign_destination as dest
          left join mid_temp_sign_destination_trans_group as trans_group
          ON dest.sign_id = trans_group.sign_id
             and dest.destination_number = trans_group.destination_number
          LEFT JOIN temp_rdf_nav_link
          on dest_link_id = link_id
          LEFT JOIN mid_temp_sign_destination_phonetic AS phonetic
          ON dest.sign_id = phonetic.sign_id AND
             dest.destination_number = phonetic.destination_number
          WHERE exit_number is not null
          ORDER BY dest.sign_id, dest.destination_number;
        """
#        self.__dict_rdf = comp_dictionary_rdf()
        exits = self.get_batch_data(sqlcmd)
        for exit_info in exits:
            sign_id = exit_info[0]
            dest_number = exit_info[1]
            exit_number = exit_info[2]
            lang_code = exit_info[3]
            alt_exit_number = exit_info[4]
            trans_langs = exit_info[5]  # 翻译语言
            trans_exit_nums = exit_info[6]  # 翻译文本
            iso_country_code = exit_info[7]
            phonetic_strings = exit_info[8]
            phonetic_lang_codes = exit_info[9]
            sign_post_exit = SignPostExit(sign_id, dest_number)
            name_type = 'office_name'
            if exit_number:
                exit_no = MultiLangNameRDF(lang_code, exit_number, name_type)
                if phonetic_strings:  # 有音素
                    tts_obj = self.__dict_rdf.get_tts_name(iso_country_code,
                                                lang_code,
                                                exit_no.get_name_type(),
                                                phonetic_strings,
                                                phonetic_lang_codes
                                                )
                    if tts_obj:
                        exit_no.set_tts(tts_obj)
                    else:
                        self.log.warning('No TTS. sign_id=%d, dest_number=%d' %
                                         (sign_id, dest_number))
                exit_no = self.__dict_rdf.set_trans_name(exit_no,
                                                  iso_country_code,
                                                  trans_exit_nums,
                                                  trans_langs,
                                                  phonetic_strings,
                                                  phonetic_lang_codes,
                                                  )
                # exit_no.set_trans(trans_exit_nums, trans_langs)
                sign_post_exit.set_exit_no(exit_no)
            else:
                continue
            if alt_exit_number:
                name_type = 'alter_name'
                alt_exit_no = MultiLangNameRDF(lang_code,
                                               alt_exit_number,
                                               name_type)

                sign_post_exit.set_alter_exit_no(alt_exit_no)
            str_info = sign_post_exit.to_string()
            self._store_name_to_temp_file(temp_file_obj, str_info)

        # ## 把名称导入数据库
        temp_file_obj.seek(0)
        self.pg.copy_from2(temp_file_obj, 'mid_temp_signpost_exit_no')
        self.pg.commit2()
        # close file
        #temp_file_obj.close()
        cache_file.close(temp_file_obj,True)
        return 0

    def _make_signpost_passlink(self):
        self.CreateFunction2('mid_findpasslinkbybothlinks')
        self.CreateFunction2('mid_get_connected_node')
        self.CreateTable2('mid_temp_signpost_passlink')
        sqlcmd = """
        SELECT sign_id, destination_number,
               originating_link_id, dest_link_id,
               pass_link,
               mid_get_connected_node(
                    (regexp_split_to_array(pass_link, E'\\\|+'))[1]::bigint,
                    (regexp_split_to_array(pass_link, E'\\\|+'))[2]::bigint
                    ) as node_id
          FROM (
                SELECT origin.sign_id, destination_number,
                       originating_link_id, dest_link_id,
                       mid_findpasslinkbybothlinks(
                               originating_link_id,
                               dest_link_id,
                               a.s_node, a.e_node,
                               1, 20
                               ) as pass_link  -- include: inlink,outlink
                  FROM rdf_sign_origin AS origin
                  LEFT JOIN rdf_sign_destination as dest
                  ON origin.sign_id = dest.sign_id
                  left join link_tbl as a
                  on originating_link_id = a.link_id
          ) AS A;
        """
        temp_file_obj = cache_file.open('signpost_passlink')  # 创建临时文件
        signs = self.get_batch_data(sqlcmd, 1024)
        for sign_info in signs:
            sign_id = sign_info[0]
            dest_number = sign_info[1]
            in_link_id = sign_info[2]
            out_link_id = sign_info[3]
            pass_link = sign_info[4]  # 包含InLink和OutLink
            node_id = sign_info[5]
            if not pass_link or not node_id:  # 没有找到路
                self.log.error("Can't find the path. inlink=%d, outlink=%d"
                               % (in_link_id, out_link_id))
                continue
            pass_link = pass_link.split('|')[1:-1]  # 去掉InLink和OutLink
            pass_link_cnt = len(pass_link)
            if pass_link:
                pass_link = '|'.join(pass_link)
            else:
                pass_link = ''
            str_info = '%d\t%d\t%d\t%d\t%d\t%s\t%d' % (sign_id,
                                                       dest_number,
                                                       node_id,
                                                       in_link_id,
                                                       out_link_id,
                                                       pass_link,
                                                       pass_link_cnt
                                                       )
            self._store_name_to_temp_file(temp_file_obj, str_info)
        # ## 把名称导入数据库
        temp_file_obj.seek(0)
        self.pg.copy_from2(temp_file_obj, 'mid_temp_signpost_passlink')
        self.pg.commit2()
        # close file
        #temp_file_obj.close()
        cache_file.close(temp_file_obj,True)
        return 0

    def _make_short_distance_passlink(self):
        '距离最短'
        self.log.info('Make PassLink.')
        self.CreateFunction2('mid_get_expand_box')  # 包含两条link的扩大区域
        self.CreateTable2('mid_temp_signpost_passlink_shortest_distance')
        temp_file_obj = cache_file.open('signpost_passlink_short_distance')
        sqlcmd = """
        SELECT origin.sign_id, destination_number,
               originating_link_id, dest_link_id,
               in_l.s_node, in_l.e_node,
               out_l.s_node, out_l.e_node,
               ST_ASEWKT(mid_get_expand_box(originating_link_id,
                                            dest_link_id,
                                            %s)
                        )
          FROM rdf_sign_origin AS origin
          LEFT JOIN rdf_sign_destination as dest
          ON origin.sign_id = dest.sign_id
          left join link_tbl as in_l
          ON originating_link_id = in_l.link_id
          left join link_tbl as out_l
          ON dest_link_id = out_l.link_id
          ;
        """
        from component.default import link_graph as lg
        datas = self.get_batch_data(sqlcmd, (lg.UNITS_TO_EXPAND,))
        for data in datas:
            sign_id = data[0]
            dest_number = data[1]
            in_link_id = data[2]
            out_link_id = data[3]
            in_s_node = data[4]
            in_e_node = data[5]
            out_s_node = data[6]
            out_e_node = data[7]
            expand_box = data[8]
            graph_obj = lg.LinkGraph()
            paths = lg.all_shortest_paths_in_expand_box(graph_obj,
                                                       expand_box,
                                                       (in_s_node, in_e_node),
                                                       (out_s_node, out_e_node)
                                                       )
            if not paths:
                self.log.warning("Can't find the path. inlink=%d, outlink=%d" %
                                 (in_link_id, out_link_id))
                continue

            if len(paths) > 1:  # 存在多条最短路径
                self.log.warning('Shortest Paths Number>1.Inlink=%d,outlink=%d'
                                % (in_link_id, out_link_id))
            node_id = paths[0][0]
            if not node_id:
                self.log.error('No NodeId.')
            pass_link = graph_obj.get_linkid_of_path(paths[0])
            if pass_link:
                if None in pass_link:
                    self.log.error('Error PassLink.')
                pass_link_cnt = len(pass_link)
                pass_link = '|'.join(pass_link)
            else:
                pass_link = ''
                pass_link_cnt = 0
            str_info = '%d\t%d\t%d\t%d\t%d\t%s\t%d' % (sign_id,
                                                       dest_number,
                                                       node_id,
                                                       in_link_id,
                                                       out_link_id,
                                                       pass_link,
                                                       pass_link_cnt
                                                       )
            self._store_name_to_temp_file(temp_file_obj, str_info)
        # ## 把名称导入数据库
        temp_file_obj.seek(0)
        self.pg.copy_from2(temp_file_obj,
                           'mid_temp_signpost_passlink_shortest_distance')
        self.pg.commit2()
        # close file
        #temp_file_obj.close()
        cache_file.close(temp_file_obj,True)
        return 0

    def _merge_all_sigpost_info(self):
        '''把所有SignPost信息都做合起来。'''
        sqlcmd = """
        INSERT INTO signpost_uc_tbl(
                    id, nodeid, inlinkid, outlinkid,
                    passlid, passlink_cnt, sp_name, route_no1,
                    route_no2, route_no3, route_no4, exit_no)
        (
        SELECT (p.sign_id * 10 + p.destination_number) as id, nodeid,
               inlinkid, outlinkid, passlid, passlink_cnt,
               signpost_name, route_no1, route_no2, route_no3,
                route_no4, exit_no
          FROM mid_temp_signpost_passlink_shortest_distance AS p
          LEFT JOIN mid_temp_signpost_element AS e
          ON p.sign_id = e.sign_id and
             p.destination_number = e.destination_number
          LEFT JOIN mid_temp_signpost_exit_no  AS no
          ON p.sign_id = no.sign_id and
             p.destination_number = no.destination_number
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _store_name_to_temp_file(self, file_obj, str_info):
        if file_obj:
            file_obj.write('%s\n' % str_info)
        return 0

    def _push_forward_inlink(self):
        self.log.info('Push Forward InLink.')
        self.CreateFunction2('mid_get_connected_node')
        self.CreateFunction2('mid_get_link_type')
        self.CreateFunction2('mid_get_other_branch_links')
        self.CreateFunction2('mid_has_same_name')  # 判断两条link名称是否相同
        # InLink往前推(不能再往前推的情况，返回空)
        # 往前推，停止的条件：
        # 1. 到达out link; 2. 到达inner Link
        # 3. 有分歧，并且名称变化; 4. 本线转到Ramp/JCT/SA
        self.CreateFunction2('mid_push_forward_inlink')
        # 备份signpost_uc_tbl
        self._bakup_signpost_uc()
        # 注： 如果新路径有重叠，只更新老路径最短的那条记录
        sqlcmd = """
        SELECT array_agg(gid), array_agg(inlinkid),
               outlinkid, new_path
          FROM (
                SELECT gid, inlinkid, outlinkid,
                       mid_push_forward_inlink(inlinkid, outlinkid, nodeid,
                                        passlid, passlink_cnt) as new_path,
                       passlink_cnt
                  FROM signpost_uc_tbl
                  where passlid is not null
                  order by new_path, outlinkid, passlink_cnt, gid
          ) AS A
          where new_path is not null
          group by outlinkid, new_path
        """
        datas = self.get_batch_data(sqlcmd)
        for data in datas:
            gid = data[0][0]
            out_link_id = data[2]
            new_path = data[3]
            if not new_path:  # 不能往前推
                continue
            new_inlink_id = new_path[0]
            new_passlink_cnt = len(new_path) - 1
            new_passlink = new_path[1:]
            # 对于新的InLink/OutLink，数据库里已经存在
            if self._is_exist(new_inlink_id, out_link_id):
                # 不更新，即不往前推。
                # print new_inlink_id, out_link_id
                continue
            else:
                # 更新InLink, node, PassLink
                self._update_link(gid,
                                  new_inlink_id,
                                  out_link_id,
                                  new_passlink,
                                  new_passlink_cnt
                                  )

    def _is_exist(self, new_inlink_id, out_link_id):
        sqlcmd = """
        select count(gid)
          from signpost_uc_tbl
          where inlinkid = %s and outlinkid = %s;
        """
        self.pg.connect1()
        self.pg.execute1(sqlcmd, (new_inlink_id, out_link_id))
        row = self.pg.fetchone()
        if row:
            if row[0]:
                return True
        return False

    def _update_link(self, gid, new_inlink_id, out_link_id,
                     new_passlink, new_passlink_cnt):
        sqlcmd = """
        UPDATE signpost_uc_tbl
             SET nodeid = mid_get_connected_node(%s, %s),
                 inlinkid = %s, passlid = %s, passlink_cnt = %s
          WHERE gid = %s;
        """

        if new_passlink:
            passlid = '|'.join([str(p) for p in new_passlink])
            first_outlink = new_passlink[0]
        else:
            passlid = None
            first_outlink = out_link_id
        self.pg.connect1()
        self.pg.execute(sqlcmd, (new_inlink_id,
                                 first_outlink,
                                 new_inlink_id,
                                 passlid,
                                 new_passlink_cnt,
                                 gid
                                 )
                        )
        self.pg.commit1()
        return

    def _bakup_signpost_uc(self):
        '备份signpost_uc_tbl'
        self.CreateTable2('signpost_uc_tbl_bak_push_inlink')
        sqlcmd = """
        INSERT INTO signpost_uc_tbl_bak_push_inlink(
                    gid, id, nodeid, inlinkid,
                    outlinkid, passlid, passlink_cnt, sp_name,
                    route_no1, route_no2, route_no3, route_no4,
                    exit_no)
        (
        SELECT gid, id, nodeid, inlinkid,
               outlinkid, passlid, passlink_cnt, sp_name,
               route_no1, route_no2, route_no3, route_no4,
               exit_no
          FROM signpost_uc_tbl
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()


#######################################################################
# ##
#######################################################################
class SignPostElement(object):
    def __init__(self, sign_id, dest_number):
        '''
        Constructor
        '''
        self.MAX_ROUTE_NO = 4  # 番号的最大数
        self._sign_id = sign_id
        self._dest_number = dest_number
        self._signpost_name = None  # 方面名称(MultLangNameRDF对象), 一个方面可以有多个名称
        self._route_no = list()  # route番号(MultLangShieldRDF对象list)

    def set_signpost_name(self, signpost_name):
        self._signpost_name = signpost_name

    def add_route_no(self, shield_obj):
        if shield_obj:
            self._route_no.append(shield_obj)

    def to_string(self):
        if self._signpost_name:
            json_sp_name = self._signpost_name.json_format_dump()
        else:
            json_sp_name = ''
        rst_str = '%d\t%d\t%s' % (self._sign_id,
                                  self._dest_number,
                                  json_sp_name)
        rn_cnt = 0
        while rn_cnt < len(self._route_no) and rn_cnt < self.MAX_ROUTE_NO:
            rst_str += '\t%s' % self._route_no[rn_cnt].json_format_dump()
            rn_cnt += 1

        while rn_cnt < self.MAX_ROUTE_NO:
            rst_str += '\t%s' % ''
            rn_cnt += 1
        return rst_str


#######################################################################
# ##
#######################################################################
class SignPostExit(object):
    def __init__(self, sign_id, dest_number):
        '''
        Constructor
        '''
        self._sign_id = sign_id
        self._dest_number = dest_number
        self._exit_no = None  # 出口番号(MultLangNameRDF对象)
        self._alt_exit_no = None  # 出口番号别名(MultLangNameRDF对象)

    def set_exit_no(self, name_obj):
        self._exit_no = name_obj

    def set_alter_exit_no(self, name_obj):
        self._alt_exit_no = name_obj
        if self._alt_exit_no:
            self._exit_no.add_alter(self._alt_exit_no)

    def to_string(self):
        if self._exit_no:
            exit_json_str = self._exit_no.json_format_dump()
        else:
            exit_json_str = ''
        return '%d\t%d\t%s' % (self._sign_id, self._dest_number, exit_json_str)
