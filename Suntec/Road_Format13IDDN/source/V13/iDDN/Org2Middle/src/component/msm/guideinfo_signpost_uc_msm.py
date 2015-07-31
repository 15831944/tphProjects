# -*- coding: UTF8 -*-
'''
Created on 2014-08-12

@author: hongchenzai
'''
import re
from common import cache_file
from component.multi_lang_name import MultiLangName
from component.multi_lang_name import NAME_TYPE_OFFICIAL
from component.msm.dictionary_msm import LANGUAGE_CODE
from component.msm.dictionary_msm import comp_dictionary_msm
from component.mmi.guideinfo_signpost_uc_mmi import SignPostElementMmi
from component.ta.guideinfo_signpost_uc_ta import comp_guideinfo_signpost_uc_ta
LIND_END_CHAR = 'E'
EXIT_NO_END_CHAR = ','
INVALID_EXIT_NO = '-'
ERROR_EXIST_NOS = {'E6': None}


class comp_guideinfo_signpost_uc_msn(comp_guideinfo_signpost_uc_ta):
    '''Malsing方面
    '''

    def __init__(self):
        '''
        Constructor
        '''
        comp_guideinfo_signpost_uc_ta.__init__(self)

    def _Do(self):
        dict_rdf = comp_dictionary_msm()
        dict_rdf.set_language_code()
        # 名称、番号、出口番号
        self._make_signpost_element()
        # 合并以上所有的信息
        self._merge_all_info()
        dict_rdf.store_language_flag()
        self.__check_signpost()
        return 0

    def _make_signpost_element(self):
        self.log.info('Start Make SignPost element.')
        sqlcmd = """
        SELECT folder, link_id as out_link_id,
               rd_signs, l_country,
               gid as sign_id
          FROM org_processed_line AS line
          WHERE rd_signs IS NOT NULL AND rd_signs <> ''
          order by folder, out_link_id, sign_id;
        """
        self.CreateTable2('mid_temp_signpost')
        temp_file_obj = cache_file.open('signpost_element')  # 创建临时文件
        signs = self.get_batch_data(sqlcmd)
        for sign_info in signs:
            folder = sign_info[0]  # folder名/区域名
            out_link_id = int(sign_info[1])
            rd_signs = sign_info[2]
            country = sign_info[3]
            sign_id = sign_info[4]
            lang_code = LANGUAGE_CODE.get(country)
            link_end_pos = rd_signs.index(LIND_END_CHAR)
            in_link_id = int(rd_signs[:link_end_pos])
            exitno_end_pos = rd_signs.index(EXIT_NO_END_CHAR)
            exitno = rd_signs[link_end_pos + 1:exitno_end_pos].strip()
            if rd_signs[exitno_end_pos + 1] == 'T':
                name = rd_signs[exitno_end_pos + 2:]
            else:
                self.log.warning('Unkown Name Type. rd_signs=%s' % rd_signs)
            if name:
                signpost_name = MultiLangName(lang_code, name,
                                              NAME_TYPE_OFFICIAL)
            else:
                # print folder, out_link_id
                signpost_name = None
            multi_exitno = self._get_exit_no(exitno, lang_code)
            sign_post = SignPostElementMmi(sign_id, folder)
            sign_post.set_exit_no(multi_exitno)
            sign_post.set_signpost_name(signpost_name)
            if sign_post.is_empty():  # 名称、route_no和exit_no都为空
                self.log.warning('is empty folder=%s, out_link_id=%s.'
                                 % (folder, out_link_id))
                continue
            str_info = sign_post.to_string()
            self._store_name_to_temp_file(temp_file_obj, in_link_id,
                                          out_link_id, str_info)
        # ## 把名称导入数据库
        temp_file_obj.seek(0)
        self.pg.copy_from2(temp_file_obj, 'mid_temp_signpost')
        self.pg.commit2()
        # close file
        #temp_file_obj.close()
        cache_file.close(temp_file_obj,True)
        self.log.info('End Make SignPost element.')
        return 0

    def _get_exit_no(self, exitno, lang_code):
        exitno = exitno.strip()
        if exitno in ERROR_EXIST_NOS:
            return None
        if not exitno or exitno == INVALID_EXIT_NO:
            return None
        # check字母开头
        temp = re.findall('^\D', exitno)
        if temp:
            self.log.warning("Start Char of Exit No is not digit. exist_no=%"
                             % exitno)
        return MultiLangName(lang_code, exitno, NAME_TYPE_OFFICIAL)

    def _store_name_to_temp_file(self, file_obj, inlink, outlink, str_info):
        if file_obj:
            if not str_info:
                self.log.error('str_info is none')
                return
            file_obj.write('%s\t%s\t%s\n' % (inlink, outlink, str_info))
        return 0

    def _merge_all_info(self):
        "把SignPost的数据合并起来(Name, Route_NO, Exit_No, Path_Link)"
        self.CreateTable2('mid_get_connected_node')
        sqlcmd = """
        INSERT INTO signpost_uc_tbl(
                    id, nodeid,
                    inlinkid, outlinkid,
                    passlid, passlink_cnt,
                    sp_name, route_no1,
                    route_no2, route_no3,
                    route_no4, exit_no)
        (
        SELECT sign_id, nodeid,
               in_link_id, out_link_id,
               passlid, passlink_cnt,
               signpost_name, route_no1,
               route_no2, route_no3,
               route_no4, exit_no
        FROM (
          SELECT sign_id, mid_get_connected_node(b.new_link_id::bigint,
                                             c.new_link_id::bigint) nodeid,
               b.new_link_id as in_link_id, c.new_link_id as out_link_id,
               null as passlid, 0 as passlink_cnt,
               signpost_name, route_no1,
               route_no2, route_no3,
               route_no4, exit_no
          FROM mid_temp_signpost as a
          LEFT JOIN temp_topo_link AS b
          ON a.folder = b.folder and a.in_link_id = b.link_id
          LEFT JOIN temp_topo_link AS c
          ON a.folder = c.folder and a.out_link_id = c.link_id
        ) AS D
        WHERE nodeid is not null
        order by nodeid, in_link_id, out_link_id, sign_id
        );
        """
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
        return 0

    def __check_signpost(self):
        '''检查是否有漏做的看板情报。'''
        sqlcmd = """
        SELECT a.folder, a.link_id, rd_signs
          FROM org_processed_line AS A
          LEFT JOIN temp_topo_link AS b
          ON a.folder = b.folder and a.link_id = b.link_id
          WHERE rd_signs is not null
               and rd_signs <> ''
               and new_link_id not in (
                    select outlinkid
                      from signpost_uc_tbl
               );
        """
        for info in self.get_batch_data(sqlcmd):
            folder, link_id, rd_signs = info[0:3]
            self.log.warning('Lack SignPost.folder=%s, link_id=%s.rd_signs=%s'
                             % (folder, link_id, rd_signs))
