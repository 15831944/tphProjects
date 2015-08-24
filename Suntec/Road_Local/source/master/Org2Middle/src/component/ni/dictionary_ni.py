# -*- coding: UTF8 -*-

import component.default.dictionary
import common.cache_file
from component.default.multi_lang_name import *
from component.rdf.guideinfo_singpost_uc_rdf import SignPostElement
from component.ni.multi_lang_name_ni import MultiLangNameNI
SIGN_POST_TYPE_EXIT_NO = 1
SIGN_POST_TYPE_SHIELD_ID = 2
SIGN_POST_TYPE_SHIELD_NUM = 3
SIGN_POST_TYPE_NAME = 4

class comp_dictionary_ni(component.default.dictionary.comp_dictionary):
    '''字典(NI)
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.default.dictionary.comp_dictionary.__init__(self)
         
    def _DoCreateTable(self):
        
        self.CreateTable2('temp_link_name')
        self.CreateTable2('temp_link_shield')

    def _DoCreateFunction(self):
        
        self.CreateFunction2('mid_cnv_shield_ni')
        self.CreateFunction2('mid_convertstring')

    def _Do(self):
        # 创建语言种别的中间表
        self._InsertLanguages()
        # 设置语言code
        self.set_language_code()

        self._make_link_shield()
        self._make_link_name()
        
        self._make_admin_name()
        
        self._make_signpost_uc_name()

    def _make_link_name(self):
        '''道路名称(包括显示用番号)'''
        self.log.info('Make Link Name.')
        
        self.CreateIndex2('org_r_lname_route_id_idx')
        self.CreateIndex2('org_r_name_route_id_idx')
        self.CreateIndex2('org_r_phon_route_id_idx')
        
        sqlcmd = """
            select link_id
                ,array_agg(route_id) as route_id_array
                ,array_agg(name_type) as name_type_array
                ,array_agg(name_language) as name_language_array
                ,array_agg(name) as name_array 
                ,array_agg(phon_language) as phon_language_array                    
                ,array_agg(phon) as phon_array
            from (
                SELECT a.id as link_id
                    ,a.route_id
                    ,a.name_kind
                    ,case when a.name_kind = '1' and b.route_kind != '0' then 'route_num'
                        when a.name_kind = '1' then 'office_name'
                        else 'alter_name'
                    end as name_type
                    ,a.seq_nm
                    ,mid_convertstring(b.pathname,1) as name
                    ,b.language
                    ,case when b.language = '1' then 'CHI'
                        when b.language = '2' then 'CHT'
                        when b.language = '3' then 'ENG'
                        when b.language = '4' then 'POR'
                        else null
                    end as name_language
                    ,c.phon
                    ,c.phon_language
                FROM org_r_lname a
                left join org_r_name b
                on a.route_id = b.route_id and a.folder = b.folder
                left join (
                    select route_id
                        ,pathname as phon
                        ,case when phontype = '1' then 'PYM'
                              when phontype = '3' then 'PYT'
                              else null
                         end as phon_language 
                        ,'1'::text as phon_flag
                        , folder
                    from org_r_phon where phontype in ('1','3')
                    union
                    select route_id
                        ,pathname as phon
                        ,case when phontype = '1' then 'PYM'
                            when phontype = '3' then 'PYT'
                            else null
                        end as phon_language 
                        ,'2'::text as phon_flag
						,folder
                    from org_r_phon where phontype in ('1','3')
                ) c
                on b.route_id = c.route_id and b.language = c.phon_flag and b.folder = c.folder
                order by id,name_kind,seq_nm,language,phon_language
            ) m group by link_id;
        """
        
        asso_recs = self.pg.get_batch_data2(sqlcmd)
        
        temp_file_obj = common.cache_file.open('road_name')
        for asso_rec in asso_recs:
            link_id = asso_rec[0]
            json_name = component.default.multi_lang_name.MultiLangName.name_array_2_json_string_multi_phon(asso_rec[1], 
                                                                                                 asso_rec[2], 
                                                                                                 asso_rec[3], 
                                                                                                 asso_rec[4],
                                                                                                 asso_rec[5],
                                                                                                 asso_rec[6])
            temp_file_obj.write('%d\t%s\n' % (int(link_id), json_name))
        
        temp_file_obj.seek(0)
        self.pg.copy_from2(temp_file_obj, 'temp_link_name')
        self.pg.commit2()
        common.cache_file.close(temp_file_obj,True)

    def _make_link_shield(self):
        # 盾牌及番号
        self.log.info('Make Link Shield.')
        
        self.CreateIndex2('org_r_lname_route_id_idx')
        self.CreateIndex2('org_r_name_route_id_idx')
        self.CreateIndex2('org_r_phon_route_id_idx')
        
        sqlcmd = """
            select link_id
                ,array_agg(route_id) as route_id_array
                ,array_agg(name_type) as name_type_array
                ,array_agg(name_language) as name_language_array
                ,array_agg(name) as name_array 
                ,array_agg(phon_language) as phon_language_array                    
                ,array_agg(phon) as phon_array
            from (
                SELECT a.id as link_id
                    ,a.route_id
                    ,a.name_kind
                    ,case when a.name_kind = '1' and b.route_kind != '0' then 'shield'
                        when a.name_kind = '1' then 'office_name'
                        else 'alter_name'
                    end as name_type
                    ,a.seq_nm
                    ,mid_cnv_shield_ni(b.folder, b.route_kind, mid_convertstring(b.pathname,1)) as name
                    ,b.language
                    ,case when b.language = '1' then 'CHI'
                        when b.language = '2' then 'CHT'
                        when b.language = '3' then 'ENG'
                        when b.language = '4' then 'POR'
                        else null
                    end as name_language
                    ,c.phon
                    ,c.phon_language
                FROM org_r_lname a
                left join org_r_name b
                on a.route_id = b.route_id and a.folder = b.folder
                left join (
                    select route_id
                        ,pathname as phon
                        ,case when phontype = '1' then 'PYM'
                              when phontype = '3' then 'PYT'
                              else null
                         end as phon_language 
                        ,'1'::text as phon_flag
                        ,folder
                    from org_r_phon where phontype in ('1','3')
                    union
                    select route_id
                        ,pathname as phon
                        ,case when phontype = '1' then 'PYM'
                            when phontype = '3' then 'PYT'
                            else null
                        end as phon_language 
                        ,'2'::text as phon_flag
						,folder
                    from org_r_phon where phontype in ('1','3') 
                ) c
                on b.route_id = c.route_id and b.language = c.phon_flag and b.folder = c.folder
                where b.route_kind != '0'
                order by id,name_kind,seq_nm,language,phon_language
            ) m group by link_id;
        """
        
        asso_recs = self.pg.get_batch_data2(sqlcmd)
        
        temp_file_obj = common.cache_file.open('road_shield')
        for asso_rec in asso_recs:
            link_id = asso_rec[0]
            json_name = component.default.multi_lang_name.MultiLangName.name_array_2_json_string_multi_phon(asso_rec[1], 
                                                                                                 asso_rec[2], 
                                                                                                 asso_rec[3], 
                                                                                                 asso_rec[4],
                                                                                                 asso_rec[5],
                                                                                                 asso_rec[6])
            temp_file_obj.write('%d\t%s\n' % (int(link_id), json_name))
        
        temp_file_obj.seek(0)
        self.pg.copy_from2(temp_file_obj, 'temp_link_shield')
        self.pg.commit2()
        common.cache_file.close(temp_file_obj,True)
        
    def _get_ni_languages_code(self,language_codes):
        language_codes_temp = list()
     
        language_code_dict={'1':'CHI',
                            '2':'CHT',
                            '3':'ENG',
                            '4':'POR'
                            }
        for language_code in language_codes:
            language_codes_temp.append(language_code_dict.get(language_code))
        
        return language_codes_temp

    
    def _get_ni_language_phon(self,language_phons):
        language_phons_temp = list()
        
        language_phon_dict={'1':'PYM',
                            '2':'PYC',
                            '3':'PYT',
                            '4':'PYN'
                            }
        
        for language_phon in language_phons:
            language_phon_list = language_phon.split('#')
            language_phon_list_temp = list()
            for language_phon_one in language_phon_list:
                if language_phon_list_temp:
                    language_phon_list_temp += "#" + language_phon_dict.get(language_phon_one)
                else:
                    language_phon_list_temp = language_phon_dict.get(language_phon_one)
                        
            language_phons_temp.append(language_phon_list_temp)
            
        return language_phons_temp
    
    def _get_name_type(self, Name_flag, route_flag = '0'):
        
        if len(route_flag) > 0 and int(route_flag) >= 1 and int(route_flag)  <= 7:
            return 'route_num'
        if Name_flag == '1':  
            return 'office_name'
        elif Name_flag == '2' or Name_flag == '3':
            return 'alter_name'
        else:
            self.log.error("get name type error! %s",Name_flag)
        
    def _make_admin_name(self):
        self.log.info('Begin get admin name')
        
        self.CreateIndex2('org_fname_featid_idx')
        self.CreateIndex2('org_fname_phon_featid_idx')
        
        self.CreateTable2('temp_admin_name')
        sqlcmd = '''
                select admin_id, array_agg(name) as name_array, array_agg(language) as language_array,
                    array_agg(nameflag) as nameflag_array, array_agg(seq_nm) as name_ids,
                    array_agg(array_to_string(phontype_array, '#')) as phontype_arrays,
                    array_agg(array_to_string(name_py_array, '#')) as name_py_arrays
                from
                (
                    select distinct unnest(array[admincode, cityadcode, proadcode]) as admin_id
                    from org_admin
                )a
                left join 
                (
                    select featid, name, seq_nm, language, nameflag, 
                        array_agg(phontype) as phontype_array, 
                        array_agg(name_py) as name_py_array
                    from
                    (
                        select name.featid, name.name, name.py, name.seq_nm, name.language, 
                            name.nameflag, name_py.phontype, name_py.name as name_py
                        from org_fname as name
                        left join org_fname_phon as name_py
                        on name.featid = name_py.featid and name.seq_nm = name_py.seq_nm
                        and name.nametype = '14' and name.language in ('1','2') and name_py.phontype in ('1', '3')
                        where name.nametype = '14'
                        order by name.featid, name.seq_nm, name.language, name_py.phontype
                    )temp
                    group by featid, language, name, seq_nm, nameflag
                    order by featid, language, name, seq_nm, nameflag
                ) as b
                on a.admin_id = b.featid
                group by admin_id
                
                union
                
                select '100000' as admin_id, array['中国'] as name_array, array['1'] as language_array,
                    array['1'] as nameflag_array, array['1'] as name_ids, array[''], array[''];
                '''
            
        rows = self.pg.get_batch_data2(sqlcmd)
        
        temp_file_obj = common.cache_file.open('admin_name')
        for row in rows:
            name_id = row[0]
            name_arrays = row[1]
            name_languages = row[2]
            name_nameflags = row[3]
            name_ids = row[4]
            name_phontypes = row[5]
            name_pys = row[6]
            
            if not name_id:
                continue
            
            name_languages = self._get_ni_languages_code(name_languages)
            name_phontypes = self._get_ni_language_phon(name_phontypes)
            name_types = list()
            for name_nameflag in name_nameflags:
                name_types.append(self._get_name_type(name_nameflag))
            
            admin_name = None
            for name_array, name_language, name_type, name_phontype, name_py in \
                zip(name_arrays, name_languages, name_types, name_phontypes, name_pys):
                if admin_name:                   
                    alter_admin_name = MultiLangNameNI(name_id,
                                                      name_language,
                                                      name_array,
                                                      name_type
                                                      )
                    # ## TTS音素
                    phoneme_list, language_list = \
                        alter_admin_name.split_phoneme_info(name_phontype, name_py)
                    alter_admin_name.add_all_tts(phoneme_list,
                                                language_list,
                                                name_language
                                                )
                    admin_name.add_alter(alter_admin_name)
                else:  # 第一个名称
                    admin_name = MultiLangNameNI(name_id,
                                                 name_language,
                                                 name_array,
                                                 name_type
                                                    )
                    # ## TTS音素
                    phoneme_list, language_list = \
                        admin_name.split_phoneme_info(name_phontype, name_py)
                    admin_name.add_all_tts(phoneme_list,
                                              language_list,
                                              name_language
                                              )
            if admin_name:
                json_name = admin_name.json_format_dump()
                temp_file_obj.write('%d\t%s\n' % (int(name_id), json_name))       
        #
        temp_file_obj.seek(0)
        self.pg.copy_from2(temp_file_obj, 'temp_admin_name')
        self.pg.commit2()
        common.cache_file.close(temp_file_obj,True)
        
        self.CreateIndex2('temp_admin_name_admin_id_idx')
        
        return 0
    
    def _get_sign_type(self, sign_type, shield_type):
        
        if sign_type == '2':
            return SIGN_POST_TYPE_EXIT_NO
        elif shield_type == '0':
            return SIGN_POST_TYPE_NAME
        elif int(shield_type) >= 1 and int(shield_type) <= 7:
            return SIGN_POST_TYPE_SHIELD_NUM
        else:
            self.log.warning("get signpost_uc name type error! %s,%s",sign_type,shield_type)
            return None
    
    def _convert_shield_id(self, shield_type):
        shieldid_dict = {'1': '5100',
                         '2': '5101',
                         '3': '5102',
                         '4': '5103',
                         '5': '5104',
                         '6': '5105',
                         '7': '5106'
                         }
        return shieldid_dict.get(shield_type)
    
    def _add_all_tts(self, phoneme_list, language_list, match_code=None):
                    
        for phoneme, lang_code in zip(phoneme_list, language_list):
            if phoneme and lang_code:
                # match_code为空，就不比较语种
                if not match_code or lang_code == match_code:
                    tts_obj = MultiLangNameTa(self.n_id,
                                              lang_code,
                                              phoneme,
                                              self._name_type,
                                              TTS_TYPE_PHONEME,
                                              self._left_right
                                              )
                    self.add_tts(tts_obj)
                    if match_code:  # 如果指定了code，只收一个
                        break
            else:
                print 'phoneme or lang_code is none. id=%d' % self.n_id    
    
    def _make_signpost_uc_name(self):
        self.log.info('Begin get signpost_uc name')
        
        self.CreateIndex2('org_fname_featid_idx')
        self.CreateIndex2('org_fname_phon_featid_idx')
        self.CreateTable2('temp_signpost_uc_path')
        self.CreateIndex2('temp_signpost_uc_path_path_id_idx')
        self.CreateTable2('temp_signpost_uc_path_id')       
        self.CreateIndex2('temp_signpost_uc_path_id_fname_idx')
        self.CreateIndex2('temp_signpost_uc_path_id_path_id_idx')
        
        self.CreateTable2('temp_signpost_uc_name')
        sqlcmd = '''
            select path_id, array_agg(featid) as featid_array, array_agg(nametype) as nametype_array, 
                array_agg(sign_name) as names, array_agg(seq_nm) as seq_nums,
                array_agg(signnumflg) as signnumflgs, array_agg(signnametp) as signnametps,
                array_agg(language) as languages, array_agg(nameflag) as nameflags,
                array_agg(phontype_types) as phontype_types_array,
                array_agg(phon_names) as phon_names_array
            from
            (
                select b.path_id, featid, nametype, sign_name, seq_nm, signnumflg, signnametp, language, nameflag, 
                    phontype_types, phon_names
                from
                (
                    select a.featid, a.nametype, 
                        (case when a.nametype = '2' or (signnumflg::integer >= 1 and signnumflg::integer <= 7)
                                    then mid_convertstring(a.name, 1) 
                              else a.name end) as sign_name, 
                        a.seq_nm, a.signnumflg, a.signnametp, a.language, a.nameflag, 
                        array_to_string(array_agg(b.phontype), '#') as phontype_types, 
                        array_to_string(array_agg(b.name), '#') as phon_names
                    from org_fname as a
                    left join org_fname_phon as b
                    on a.featid = b.featid and a.nametype = b.nametype 
                        and a.seq_nm = b.seq_nm and a.language in ('1','2')
                        and b.phontype in ('1', '3')
                    where a.nametype in ('2', '3', '4')
                    group by a.featid, a.nametype, a.language, a.seq_nm, a.name, a.signnumflg,a.signnametp, a.nameflag
                    order by a.featid, a.nametype, a.language, a.seq_nm, a.name, a.signnumflg,a.signnametp, a.nameflag
                )a
                left join temp_signpost_uc_path_id as b
                on a.featid = b.fname_id and a.nametype = b.fname_type
                order by b.path_id, nametype, featid, seq_nm, language, nameflag, signnumflg, phontype_types
            )temp
            group by path_id;
                '''

        temp_file_obj = common.cache_file.open('signpost_uc_name')  # 创建临时文件
        signs = self.get_batch_data(sqlcmd)
        for sign_info in signs:
            sign_id = sign_info[0]  # 看板id
            featid_array = sign_info[1]
            nametype_arrays = sign_info[2]
            sign_names = sign_info[3]#名称
            seqnrs = sign_info[4]  # 同个方向内顺序号
            shield_types = sign_info[5]  # 种别
            out_types = sign_info[6]  # 名称、番号
            lang_codes = sign_info[7]  # 语种
            nameflags = sign_info[8]
            phontype_types = sign_info[9]
            phoneme_infos = sign_info[10]  # 音素
            
            lang_codes = self._get_ni_languages_code(lang_codes)
            phontype_types = self._get_ni_language_phon(phontype_types)
            
            sign_post = SignPostElementNI(sign_id, seqnrs)
            shield_id = None
            exit_no = None
            route_no = None
            signpost_name = None
            name_flag = list()
            for nametype_array, sign_name, seqnr, shield_type, out_type, lang_code, nameflag, phontype_type, phoneme_info in \
                zip(nametype_arrays, sign_names, seqnrs, shield_types, out_types, lang_codes, nameflags, phontype_types, phoneme_infos):
                sign_type = self._get_sign_type(nametype_array, shield_type)
                if not sign_type:  # 空不收录
                    continue
                #判断是否出现过
                list_temp = [sign_type, sign_name]
                if name_flag.count(list_temp) > 0:
                    continue
                else:
                    name_flag.append(list_temp)
                    
                name_type = self._get_name_type(nameflag, shield_type)
                
                if SIGN_POST_TYPE_SHIELD_NUM == sign_type:
                    shield_id = self._convert_shield_id(shield_type)
                    if not shield_id:
                        self.log.error('No Shield id. sign_id=%d, seqnr=%d, shield_type=%d'
                                       % (sign_id, seqnr, shield_type))
                    route_no = MultiLangShield(shield_id,
                                               sign_name,
                                               lang_code
                                               )
                    sign_post.add_route_no(route_no)
                    shield_id = None
                elif SIGN_POST_TYPE_EXIT_NO == sign_type:                    
                    if exit_no:
                        # 存在多个出口番号，其他出口番号做出口番号的别名
                        alter_exit_no = MultiLangNameNI(sign_id,
                                                       lang_code,
                                                       sign_name,
                                                       name_type
                                                       )
                        # ## TTS音素
                        phoneme_list, language_list = \
                            self._split_phoneme_info(phontype_type, phoneme_info)
                        alter_exit_no.add_all_tts(phoneme_list,
                                                  language_list,
                                                  lang_code
                                                  )
                        exit_no.add_alter(alter_exit_no)
                    else:
                        # 第一个出口番号
                        exit_no = MultiLangNameNI(sign_id,
                                                lang_code,
                                                sign_name,
                                                name_type
                                                )
                        # ## TTS音素
                        phoneme_list, language_list = \
                            exit_no.split_phoneme_info(phontype_type, phoneme_info)
                        exit_no.add_all_tts(phoneme_list,
                                            language_list,
                                            lang_code
                                            )
                elif SIGN_POST_TYPE_NAME == sign_type:  # 方面名称
                    if signpost_name:
                        # 其他名称番号
                        alter_sign_name = MultiLangNameNI(sign_id,
                                                          lang_code,
                                                          sign_name,
                                                          name_type
                                                          )
                        # ## TTS音素
                        phoneme_list, language_list = \
                            alter_sign_name.split_phoneme_info(phontype_type, phoneme_info)
                        alter_sign_name.add_all_tts(phoneme_list,
                                                    language_list,
                                                    lang_code
                                                    )
                        signpost_name.add_alter(alter_sign_name)
                    else:  # 第一个名称
                        signpost_name = MultiLangNameNI(sign_id,
                                                        lang_code,
                                                        sign_name,
                                                        name_type
                                                        )
                        # ## TTS音素
                        phoneme_list, language_list = \
                            signpost_name.split_phoneme_info(phontype_type, phoneme_info)
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
                    temp_file_obj.write('%s\n' % str_info)
                else:
                    self.log.error('No SignPost Info. sign_id=%d' % sign_id)
            else:
                self.log.warning('No route_no/exit_no/signpost_name.sign_id=%d'
                                % (sign_id))
        # ## 把名称导入数据库
        temp_file_obj.seek(0)
        self.pg.copy_from2(temp_file_obj, 'temp_signpost_uc_name')
        self.pg.commit2()
        # close file
        #temp_file_obj.close()
        common.cache_file.close(temp_file_obj,True)
        
        self.CreateIndex2('temp_signpost_uc_name_sign_id_idx')
        
        return 0
    
#######################################################################
# ## SignPostElement
#######################################################################
class SignPostElementNI(SignPostElement):
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
    
    

