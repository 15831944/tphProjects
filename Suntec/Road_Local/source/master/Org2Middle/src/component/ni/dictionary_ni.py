# -*- coding: UTF8 -*-

import component.default.dictionary
import common.cache_file
from component.default.multi_lang_name import MultiLangName

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

    def _make_link_name(self):
        '''道路名称(包括显示用番号)'''
        self.log.info('Make Link Name.')
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
                on a.route_id = b.route_id
                left join (
                    select route_id
                        ,pathname as phon
                        ,case when phontype = '1' then 'PYM'
                            when phontype = '3' then 'PYT'
                            else null
                        end as phon_language 
                        ,'1'::text as phon_flag
                    from org_r_phon where phontype in ('1','3')
                    order by phontype
                ) c
                on a.route_id = c.route_id and b.language = c.phon_flag
                order by id,seq_nm,language,name_kind
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
                    ,mid_cnv_shield_ni(b.route_kind, mid_convertstring(b.pathname,1)) as name
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
                on a.route_id = b.route_id
                left join (
                    select route_id
                        ,pathname as phon
                        ,case when phontype = '1' then 'PYM'
                            when phontype = '3' then 'PYT'
                            else null
                        end as phon_language 
                        ,'1'::text as phon_flag
                    from org_r_phon where phontype in ('1','3')
                    order by phontype
                ) c
                on a.route_id = c.route_id and b.language = c.phon_flag
                where b.route_kind != '0'
                order by id,seq_nm,language,name_kind
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
            language_phon_list = language_phon.split('|')
            language_phon_list_temp = list()
            for language_phon_one in language_phon_list:
                if language_phon_list_temp:
                    language_phon_list_temp += "|" + language_phon_dict.get(language_phon_one)
                else:
                    language_phon_list_temp = language_phon_dict.get(language_phon_one)
                        
            language_phons_temp.append(language_phon_list_temp)
            
        return language_phons_temp
    
    def _get_name_type(self, Name_flag, route_flag = None):
        if route_flag:  # 番号
            return 'route_num'
        if Name_flag == '1':  
            return 'office_name'
        elif Name_flag == '2' or Name_flag == '3':
            return 'alter_name'
        else:
            self.log.error("get name type errpr! %s",Name_flag)
        
    def _make_admin_name(self):
        self.log.info('Begin get admin name')
        
        self.CreateTable2('temp_admin_name')
        sqlcmd = '''
                select admin_id, array_agg(name) as name_array, array_agg(language) as language_array,
                    array_agg(nameflag) as nameflag_array, array_agg(seq_nm) as name_ids,
                    array_agg(array_to_string(phontype_array, '|')) as phontype_arrays,
                    array_agg(array_to_string(name_py_array, '|')) as name_py_arrays
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
            name_type = list()
            for name_nameflag in name_nameflags:
                name_type.append(self._get_name_type(name_nameflag))
            
            
            json_name = component.default.multi_lang_name.\
                MultiLangName.name_array_2_json_string_multi_phon(name_ids,
                                                                  name_type,
                                                                  name_languages,
                                                                  name_arrays, 
                                                                  name_phontypes,
                                                                  name_pys)
                
            temp_file_obj.write('%d\t%s\n' % (int(name_id), json_name))
        
        #
        temp_file_obj.seek(0)
        self.pg.copy_from2(temp_file_obj, 'temp_admin_name')
        self.pg.commit2()
        common.cache_file.close(temp_file_obj,True)
        
        self.CreateIndex2('temp_admin_name_admin_id_idx')
        
        return 0
    
    

