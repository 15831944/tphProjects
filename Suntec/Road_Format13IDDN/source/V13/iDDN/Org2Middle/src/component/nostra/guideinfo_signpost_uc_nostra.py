# -*- coding: UTF8 -*-
'''
Created on 2013-12-10

@author: hongchenzai
'''
from common import cache_file
from component.nostra.mult_lang_name_nostra import MultLangName
from component.guideinfo_signpost_uc import comp_guideinfo_signpost_uc


class comp_guideinfo_signpost_uc_nostra(comp_guideinfo_signpost_uc):
    '''方面看板(海外)
    '''

    def __init__(self):
        '''
        Constructor
        '''
        comp_guideinfo_signpost_uc.__init__(self)

    def _Do(self):
        # 从TollPlaza抽取名称
        self._make_toll_plaza_name()
        # 从JunctionView抽取名称
        self._make_jv_name()
        # 把TollPlaza和JV的名称，插到SignPost_uc表单
        self._insert_to_signpost()
        return 0

    def _make_toll_plaza_name(self):
        '''从Toll Plaza抽取名称'''
        self.CreateFunction2('mid_get_connect_node')
        sqlcmd = """
        SELECT array_agg(gid) as gids,
               from_arc as inlinkid,
               to_arc as outlinkid,
               ARRAY_AGG(exit_namt) AS exit_namts,
               ARRAY_AGG(exit_name) AS exit_names,
               ARRAY_AGG(tts_namt) AS tts_namts,
               nodeid,
               in_s_node, in_e_node,
               out_s_node, out_e_node
          FROM (
                SELECT org_tollplaza.gid, from_arc::bigint, to_arc::bigint,
                       exit_namt, exit_name, tts_namt,
                       mid_get_connect_node(from_arc::bigint, to_arc::bigint) as nodeid,
                       in_l.s_node as in_s_node, in_l.e_node as in_e_node,
                       out_l.s_node as out_s_node, out_l.e_node as out_e_node
                  FROM org_tollplaza
                  left join link_tbl as in_l
                  ON from_arc::bigint = in_l.link_id
                  left join link_tbl as out_l
                  ON to_arc::bigint = out_l.link_id
                  order by from_arc, to_arc, gid
          ) as A
          group by from_arc, to_arc, nodeid,
                   in_s_node, in_e_node, out_s_node, out_e_node
          order by from_arc, to_arc, nodeid;
        """
        temp_file_obj = cache_file.open('temp_toll_plaza_name')  # 创建临时文件
        self.CreateTable2('temp_toll_plaza_name')
        self._make_temp_name(sqlcmd, temp_file_obj, 'temp_toll_plaza_name')
        # close file
        #temp_file_obj.close()
        cache_file.close(temp_file_obj,True)
        return 0

    def _make_jv_name(self):
        '''从JunctionView抽取名称'''
        sqlcmd = """
         SELECT array_agg(gid),
               inlinkid,
               outlinkid,
               array_agg(th_roadname) as th_roadnames,
               array_agg(en_roadname) as en_roadnames,
               array_agg(tts_namt) as tts_namts,
               node_id,
               in_s_node, in_e_node,
               out_s_node, out_e_node
          FROM (
                SELECT org_junctionview.gid, arc1::bigint AS inlinkid,
                       arc2::bigint as outlinkid,
                       th_roadname, en_roadname, ''::text as tts_namt,
                       mid_get_connect_node(arc1::bigint,
                                           arc2::bigint) as node_id,
                       in_l.s_node as in_s_node, in_l.e_node as in_e_node,
                       out_l.s_node as out_s_node, out_l.e_node as out_e_node
                  FROM org_junctionview
                  left join (
                    SELECT pic_name, first_arrow_name as arrow_name,
                           trim(th_roadname1, '') as th_roadname,
                           trim(en_roadname1) as en_roadname
                      FROM temp_junctionview_name_sort
                    union

                    SELECT pic_name, second_arrow_name as arrow_name,
                           trim(th_roadname2, '') as th_roadname,
                           trim(en_roadname2) as en_roadname
                      FROM temp_junctionview_name_sort
                  ) as jv_name
                  on day_pic = pic_name and arrowimg = arrow_name
                  left join link_tbl as in_l
                  ON arc1 = in_l.link_id
                  left join link_tbl as out_l
                  ON arc2 = out_l.link_id
                  order by arc1, arc2, gid
          ) AS A
          GROUP BY inlinkid, outlinkid, node_id,
                   in_s_node, in_e_node, out_s_node, out_e_node
          ORDER BY inlinkid, outlinkid, node_id;
        """
        temp_file_obj = cache_file.open('temp_jv_name')  # 创建临时文件
        self.CreateTable2('temp_jv_name')
        self._make_temp_name(sqlcmd, temp_file_obj, 'temp_jv_name')
        # close file
        #temp_file_obj.close()
        cache_file.close(temp_file_obj,True)
        return 0

    def _insert_to_signpost(self):
        '''把TollPlaza和JV的名称，插到SignPost_uc表单。'''
        # ##TollPlaza
        sqlcmd = """
        insert into signpost_uc_tbl(id, nodeid, inlinkid, outlinkid,
            passlid, passlink_cnt, sp_name, route_no1, route_no2,
            route_no3, route_no4, exit_no)
        (
        select gid, nodeid, inlinkid, outlinkid, passlid,
               passlink_cnt, sp_name, NULL as route_no1,
               NULL as route_no2, NULL as route_no3,
               NULL as route_no4, NULL as exit_no
          from temp_toll_plaza_name
          order by gid
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        # ## JV
        # 1) 如果同个方向已经有TollPlaza名称，就不收录JV名称
        # 2) JV名称的ID, 从50000开始
        sqlcmd = """
        INSERT INTO signpost_uc_tbl(id, nodeid, inlinkid, outlinkid,
            passlid, passlink_cnt, sp_name, route_no1, route_no2,
            route_no3, route_no4, exit_no)
        (
        select gid + 50000, nodeid, inlinkid, outlinkid, passlid,
               passlink_cnt, sp_name, NULL as route_no1,
               NULL as route_no2, NULL as route_no3,
               NULL as route_no4, NULL as exit_no
          FROM temp_jv_name
          where (nodeid, inlinkid, outlinkid) not in (
            select nodeid, inlinkid, outlinkid
              from temp_toll_plaza_name
          )
          order by gid
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        # 创建唯一索引，以避免同个方向有多个记录
        self.CreateIndex2('signpost_uc_tbl_nodeid_inlinkid_outlinkid_idx')
        return 0

    def _make_temp_name(self, sqlcmd, temp_file_obj, table_name):
        signpost_info_batch = self.get_batch_data(sqlcmd)
        mult_lang_obj = MultLangName()
        import re
        for signpost_info in signpost_info_batch:
            sp_id = signpost_info[0]
            in_link_id = signpost_info[1]
            out_link_id = signpost_info[2]
            name_thas = signpost_info[3]
            name_thes = signpost_info[4]
            name_ttss = signpost_info[5]
            node_id = signpost_info[6]
            in_s_node = signpost_info[7]
            in_e_node = signpost_info[8]
            out_s_node = signpost_info[9]
            out_e_node = signpost_info[10]
            pass_link = ''
            pass_link_cnt = 0
            # in_link和out_link有空，或者它们相等
            if self._check_org_inout_link(in_link_id, out_link_id) == False:
                self.log.error("Error Link: in_link=%s, out_link=%s"
                               % (in_link_id, out_link_id))
                continue
            name_thas, name_thes, name_ttss = \
                self._delete_repeat(name_thas, name_thes, name_ttss)
            name_list = []
            for name_tha, name_the, name_tts in zip(name_thas,
                                                    name_thes,
                                                    name_ttss
                                                    ):
                # ## 几个连续的空白，变成一个空格
                name_tha = re.sub(r'\s+', ' ', name_tha)
                name_the = re.sub(r'\s+', ' ', name_the)
                name_tts = re.sub(r'\s+', ' ', name_tts)
                name_the = name_the.replace('–', '-')  # 这两个杠是不同的
                # 'U - Turn'/'U -Turn'/'U- Turn' 去掉中间的格空变成'U-Turn'
                name_the = re.sub(r'[U|u]\s*-\s*[T|t]urn', 'U-Turn', name_the)
                # 没有名称
                if (not name_tha) and (not name_the):
                    # self.log.warning('No Name for %s, inlink=%s, outlink=%s'
                    #                 % (table_name, in_link_id, out_link_id))
                    continue
                # 缺少泰文
                if not name_tha:
                    self.log.warning('Lack Thai Name for %s, '
                                     'inlink=%s, outlink=%s'
                                     % (table_name, in_link_id, out_link_id))
                # 缺少英文
                if not name_the:
                    self.log.warning('Lack Eng Name for %s, '
                                     'inlink=%s, outlink=%s'
                                     % (table_name, in_link_id, out_link_id))
                # in_link和out_link没有交点
                if not node_id:
                    from component import link_graph as lg
                    expand_box = lg.get_daufalt_expand_box(in_link_id,
                                                           out_link_id)
                    graph_obj = lg.LinkGraph()
                    paths = lg.all_shortest_paths_in_expand_box(graph_obj,
                                                       expand_box,
                                                       (in_s_node, in_e_node),
                                                       (out_s_node, out_e_node)
                                                       )
                    if not paths:
                        self.log.error("Node is none: in_link=%s, out_link=%s"
                                       % (in_link_id, out_link_id))
                        continue
                    node_id = paths[0][0]
                    if not node_id:
                        self.log.error('No NodeId.')
                    pass_link = graph_obj.get_linkid_of_path(paths[0])
                    if pass_link:
                        if None in pass_link:
                            self.log.error('Error PassLink.')
                        pass_link_cnt = len(pass_link)
                        pass_link = '|'.join(pass_link)
                # 判断名称是"U-Turn"、"U-Turn Bridge"、"Frontage Road (Exit)"
                if self._is_uturn_frontage_name(name_the):
                    pass
                else:
                    # 名称做成多语言
                    name = mult_lang_obj.convert_names_to_list(name_tha,
                                                               name_the,
                                                               name_tts,
                                                               'office_name'
                                                               )
                    name_list += name
            if name_list:
                json_name = mult_lang_obj.json_format_dump2(name_list)
                # 保存到临时文件
                if json_name:
                    temp_file_obj.write('%d\t%d\t%d\t%d\t%s\t%d\t%s\n'
                                        % (sp_id[0],  # Gid
                                           node_id,
                                           in_link_id,
                                           out_link_id,
                                           pass_link,
                                           pass_link_cnt,
                                           json_name)
                                        )
                else:
                    self.log.error("JSON name is None. sp_id=%s" % sp_id)
        # ## 导入数据库
        temp_file_obj.seek(0)
        self.pg.copy_from2(temp_file_obj, table_name)
        self.pg.commit2()
        return 0

    def _delete_repeat(self, name_thas, name_thes, name_ttss):
        name_num = len(name_thas)
        if name_num == 1:
            return name_thas, name_thes, name_ttss
        rst_thas = name_thas[0:1]
        rst_thes = name_thes[0:1]
        rst_ttss = name_ttss[0:1]
        for name_cnt in range(1, name_num):
            name_tha = name_thas[name_cnt]
            name_the = name_thes[name_cnt]
            name_tts = name_ttss[name_cnt]
            # 为空
            if not name_tha and not name_the and not name_tts:
                continue
            # 重复
            if self._is_repeat(zip(rst_thas, rst_thes, rst_ttss),
                               (name_tha, name_the, name_tts)
                               ):
                continue
            # 英文重复(即泰文orTTS不重复)
            if self._is_repeat(rst_thes, name_the):
                self.log.warning("Just English name is repeat. "
                                 "name_thas=%s, name_thes=%s"
                                 % (name_thas, name_thes))
                continue
            rst_thas.append(name_tha)
            rst_thes.append(name_the)
            rst_ttss.append(name_tts)
        return rst_thas, rst_thes, rst_ttss

    def _is_repeat(self, source, dest):
        if dest in source:
            return True
        else:
            return False

    def _check_org_inout_link(self, in_link, out_link):
        '''名称'''
        if (not in_link) or (not out_link):  # in_link 或者 out_link有空
            return False
        if in_link == out_link:
            return False
        return True

    def _is_uturn_frontage_name(self, signpost_name):
        '''名称是："U-Turn"、"U-Turn Bridge"、"Frontage Road (Exit)"。'''
        spec_dict = ["Exit U-Turn Bridge",
                     "U-turn Bridge",
                     "U-Turn",
                     "Frontage Road (Exit)",
                     "Exit",
                     "Frontage Rd.",
                     ]
        signpost_name = signpost_name.replace('–', '-')  # 这两个杠是不同的
        low_sp_name = signpost_name.lower()
        for spec_name in spec_dict:
            low_spec_name = spec_name.lower()
            if low_sp_name.find(low_spec_name) == 0:
                return True

            if low_sp_name.find(low_spec_name) > 0:
                self.log.warning("SP Name include U-Turn/Frontage. "
                                 "signpost_name=%s" % signpost_name)
        return False
