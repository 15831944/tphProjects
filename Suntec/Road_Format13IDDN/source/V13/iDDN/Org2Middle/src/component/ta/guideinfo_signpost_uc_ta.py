# -*- coding: UTF8 -*-
'''
Created on 2012-10-24

@author: hongchenzai
'''
from common import cache_file
from component.multi_lang_name import *
from component.ta.multi_lang_name_ta import MultiLangNameTa
from component.ta.dictionary_ta import ShieldTA
from component.guideinfo_signpost_uc import comp_guideinfo_signpost_uc
SIGN_POST_TYPE_EXIT_NO = 1
SIGN_POST_TYPE_SHIELD_ID = 2
SIGN_POST_TYPE_SHIELD_NUM = 3
SIGN_POST_TYPE_NAME = 4


class comp_guideinfo_signpost_uc_ta(comp_guideinfo_signpost_uc):
    '''TomTom方面
    '''

    def __init__(self):
        '''
        Constructor
        '''
        comp_guideinfo_signpost_uc.__init__(self)

    def _Do(self):
        from component.ta.dictionary_ta import comp_dictionary_ta
        dict_ta = comp_dictionary_ta()
        dict_ta.set_language_code()
        # 名称、番号、出口番号
        self._make_signpost_element()
        # SignPost的link序
        self._make_path_link()
        # 合并以上所有的信息
        self._merge_all_info()
        return 0

    def _make_signpost_element(self):
        self.log.info('Start Make SignPost element.')
        # 注：有TTS音素的名称：4G, 6T, 4I, 9D, 4E, RN
        # 前五个种别做成名称，4E做成出口番号，RN，不收录
        sqlcmd = """
        SELECT id::bigint,
               array_agg(seqnr) as  seqnrs,
               array_agg(destseq) as destseqs,
               array_agg(infotyp) as infotyps,
               array_agg(rnpart) as rnparts,
               array_agg(txtcont) as names,
               array_agg(txtcontlc) as lang_codes,
               array_agg(phoneme_info) as phoneme_infos
          FROM (
                SELECT id, seqnr, destseq, infotyp,
                       rnpart, txtcont, txtcontlc,
                       mid_get_phoneme(id::bigint, 2128, 0,   -- 2128: SignPost
                               txtcont, txtcontlc, infotyp) as phoneme_info
                  FROM org_si
                  order by id, seqnr, destseq, rnpart
          ) AS A
          GROUP BY id;
        """
        self.CreateTable2('mid_temp_signpost_element')
        temp_file_obj = cache_file.open('signpost_element')  # 创建临时文件
        shield_obj = ShieldTA()
        signs = self.get_batch_data(sqlcmd)
        for sign_info in signs:
            sign_id = sign_info[0]  # 看板id
            seqnr = sign_info[1]  # 方向番号
            destseqs = sign_info[2]  # 同个方向内顺序号
            infotyps = sign_info[3]  # 种别
            # rnparts = sign_info[3]  # 道路番号内部无素的序号
            names = sign_info[5]  # 名称、番号
            lang_codes = sign_info[6]  # 语种
            phoneme_infos = sign_info[7]  # 音素
            sign_post = SignPostElementTa(sign_id, seqnr)
            shield_id = None
            exit_no = None
            route_no = None
            signpost_name = None
            for destseq, infotyp, name, lang_code, phoneme_info in \
                    zip(destseqs, infotyps, names, lang_codes, phoneme_infos):
                sign_type = self._get_sign_type(infotyp)
                if not sign_type:  # 空不收录
                    continue
                name_type = self._cvt_name_type(sign_type)
                # ## 处理各种种别的数据
                if SIGN_POST_TYPE_SHIELD_ID == sign_type:  # shield id
                    # 取得shield id
                    shield_id = shield_obj.convert_shield_id(int(name))
                elif SIGN_POST_TYPE_SHIELD_NUM == sign_type:  # shield number
                    if shield_id:  # 盾牌号存在
                        shield_number = name
                        if not lang_code or lang_code == 'UND':
                            lang_code = MultiLangNameTa.get_lang_code_by_id(sign_id)
                        route_no = MultiLangShield(shield_id,
                                                   shield_number,
                                                   lang_code
                                                   )
                        sign_post.add_route_no(route_no)
                        shield_id = None
                    else:
                        # 盾牌和番号是成对出现的，而且盾牌在前
                        self.log.error('No Shield id. sign_id=%d, seqnr=%d, destseq=%d'
                                       % (sign_id, seqnr, destseq))
                elif SIGN_POST_TYPE_EXIT_NO == sign_type:  # 出口番号
                    if exit_no:
                        # 存在多个出口番号，其他出口番号做出口番号的别名
                        alter_exit_no = MultiLangNameTa(sign_id,
                                                        lang_code,
                                                        name,
                                                        name_type
                                                        )
                        # ## TTS音素
                        phoneme_list, language_list = \
                            alter_exit_no.split_phoneme_info(phoneme_info)
                        alter_exit_no.add_all_tts(phoneme_list,
                                                  language_list,
                                                  lang_code
                                                  )
                        exit_no.add_alter(alter_exit_no)
                    else:
                        # 第一个出口番号
                        exit_no = MultiLangNameTa(sign_id,
                                                  lang_code,
                                                  name,
                                                  name_type
                                                  )
                        # ## TTS音素
                        phoneme_list, language_list = \
                            exit_no.split_phoneme_info(phoneme_info)
                        exit_no.add_all_tts(phoneme_list,
                                            language_list,
                                            lang_code
                                            )
                elif SIGN_POST_TYPE_NAME == sign_type:  # 方面名称
                    if signpost_name:
                        # 其他名称番号
                        alter_sign_name = MultiLangNameTa(sign_id,
                                                          lang_code,
                                                          name,
                                                          name_type
                                                          )
                        # ## TTS音素
                        phoneme_list, language_list = \
                            alter_sign_name.split_phoneme_info(phoneme_info)
                        alter_sign_name.add_all_tts(phoneme_list,
                                                    language_list,
                                                    lang_code
                                                    )
                        signpost_name.add_alter(alter_sign_name)
                    else:  # 第一个名称
                        signpost_name = MultiLangNameTa(sign_id,
                                                        lang_code,
                                                        name,
                                                        name_type
                                                        )
                        # ## TTS音素
                        phoneme_list, language_list = \
                            signpost_name.split_phoneme_info(phoneme_info)
                        signpost_name.add_all_tts(phoneme_list,
                                                  language_list,
                                                  lang_code
                                                  )
                else:
                    continue
            if route_no or exit_no or signpost_name:
                sign_post.set_exit_no(exit_no)
                sign_post.set_signpost_name(signpost_name)
                str_info = sign_post.to_string()
                if str_info:
                    self._store_name_to_temp_file(temp_file_obj, str_info)
                else:
                    self.log.error('No SignPost Info. sign_id=%d' % sign_id)
            else:
                self.log.warning('No route_no/exit_no/signpost_name.sign_id=%d'
                                % (sign_id))
        # ## 把名称导入数据库
        temp_file_obj.seek(0)
        self.pg.copy_from2(temp_file_obj, 'mid_temp_signpost_element')
        self.pg.commit2()
        # close file
        #temp_file_obj.close()
        cache_file.close(temp_file_obj,True)
        self.log.info('End Make SignPost element.')
        return 0

    def _store_name_to_temp_file(self, file_obj, str_info):
        if file_obj:
            file_obj.write('%s\n' % str_info)
        return 0

    def _get_sign_type(self, infotyp):
        d = {"4E": SIGN_POST_TYPE_EXIT_NO,  # 出口番号
             "6W": SIGN_POST_TYPE_SHIELD_ID,
             "RV": SIGN_POST_TYPE_SHIELD_NUM,
             "RN": None,  # route num(当前不收录)
             "7G": None,  # direction(当前不收录)
             "7V": None,  # Validity Direction(不收录)
             "RJ": None,  # Route Name(不收录)
             "7A": None,  # Street Name Type(不收录)
             "4G": SIGN_POST_TYPE_NAME,  # Exit Name(做成名称)
             "4I": SIGN_POST_TYPE_NAME,  # Other(做成名称)
             "6T": SIGN_POST_TYPE_NAME,  # Street Name(做成名称)
             "9D": SIGN_POST_TYPE_NAME,  # Place Name(做成名称)
             "4H": None  # Pictogram(不收录)
             }
        # 值为空，不收录
        return d.get(infotyp)

    def _cvt_name_type(self, sign_type):
        if SIGN_POST_TYPE_EXIT_NO == sign_type:
            return 'office_name'
        if (SIGN_POST_TYPE_SHIELD_ID == sign_type or
            SIGN_POST_TYPE_SHIELD_NUM == sign_type):
            return 'shield'
        if SIGN_POST_TYPE_NAME == sign_type:
            return 'office_name'

    def _make_path_link(self):
        "找到SignPost对应的link序 (如：in_link_id, out_link_id, passlid, passlid_cnt)"
        # 同一个id中，
        # seqnr = 1的， 作为in_link_id
        # seqnr最大的，作为out_link_id
        # 处于以上两者之间的，作为pass link
        self.CreateTable2('mid_temp_signpost_passlink')
        # self.CreateFunction2('mid_make_signpost_path_links')
        temp_file_obj = cache_file.open('signpost_passlink')  # 创建临时文件
        sqlcmd = """
        SELECT id, array_agg(link_id) as link_ids
          from (
                SELECT id, seqnr, trpelid::bigint as link_id
                  FROM org_sp
                  order by id, seqnr
          ) as p
          group by id;
        """
        pathes = self.get_batch_data(sqlcmd)
        for path_info in pathes:
            sign_id = path_info[0]
            link_ids = path_info[1]
            if not sign_id:
                self.log.error('Error Sign ID.')
                continue
            if link_ids:
                in_link_id = link_ids[0]
                out_link_id = link_ids[-1]
                pass_link = '|'.join([str(p) for p in link_ids[1:-1]])
                pass_link_cnt = len(link_ids[1:-1])

                str_info = '%d\t%d\t%d\t%s\t%d' % (sign_id,
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

    def _merge_all_info(self):
        "把SignPost的数据合并起来(Name, Route_NO, Exit_No, Path_Link)"
        # 注：这里要把mid_temp_signpost_element放最前面, 因为有看板的无素没有收录。
        sqlcmd = """
        INSERT INTO signpost_uc_tbl(
                    id, nodeid, inlinkid,
                    outlinkid, passlid, passlink_cnt,
                    sp_name, route_no1, route_no2,
                    route_no3, route_no4, exit_no)
        (
        SELECT e.sign_id, org_sg.jnctid as nodeid, in_link_id,
               out_link_id, passlid, passlink_cnt,
               signpost_name, route_no1, route_no2,
               route_no3, route_no4, exit_no
          FROM mid_temp_signpost_element AS e
          LEFT JOIN mid_temp_signpost_passlink as p
          ON e.sign_id = p.sign_id
          LEFT JOIN org_sg  -- NODE ID
          ON e.sign_id = org_sg.id
          order by e.sign_id
        );
        """
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
        return 0


from component.rdf.guideinfo_singpost_uc_rdf import SignPostElement


#######################################################################
# ## SignPostElement
#######################################################################
class SignPostElementTa(SignPostElement):
    def __init__(self, sign_id, dest_number):
        '''
        Constructor
        '''
        SignPostElement.__init__(self, sign_id, None)
        self._sign_id = sign_id
        self._dest_number = dest_number
        self._exit_no = None  # 出口番号(MultLangName对象)

    def set_exit_no(self, exit_no_obj):
        if exit_no_obj:
            self._exit_no = exit_no_obj

    def to_string(self):
        if self._sign_id:
            rst_str = '%d' % self._sign_id
        else:
            return None
        if self._signpost_name:
            json_sp_name = self._signpost_name.json_format_dump()
        else:
            json_sp_name = ''
        rst_str += '\t%s' % json_sp_name

        rn_cnt = 0
        while rn_cnt < len(self._route_no) and rn_cnt < self.MAX_ROUTE_NO:
            rst_str += '\t%s' % self._route_no[rn_cnt].json_format_dump()
            rn_cnt += 1

        while rn_cnt < self.MAX_ROUTE_NO:
            rst_str += '\t%s' % ''
            rn_cnt += 1
        # 加上出口番号
        if self._exit_no:
            json_exit_no = self._exit_no.json_format_dump()
            if not json_exit_no:
                json_exit_no = ''
        else:
            json_exit_no = ''
        rst_str += '\t%s' % json_exit_no
        return rst_str
