# -*- coding: UTF8 -*-
'''
Created on 2015-5-8

@author: liuxinxing
'''

import json
import operator

from base.rdb_ItemBase import ItemBase
from common import rdb_common
import common.cache_file

class rdb_name_language_order(ItemBase):
    '''
    classdocs
    '''
    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self, 'Name')
        text_filter_condition = rdb_common.GetPath('text_filter')
        temp_proj_name = rdb_common.GetPath('proj_name')
        temp_proj_country = rdb_common.GetPath('proj_country')
        temp_key = (temp_proj_name + "_" + temp_proj_country).upper()
        
        temp_dict = eval(text_filter_condition)
        if temp_dict is not None:
            cur_target_language_sort_condition = temp_dict[temp_key]
        
        self.__language_filter_dict = dict()
        if not self.__initialize_filter_condition(cur_target_language_sort_condition):
            self.log.error('make language filter condition error')
            
        self.__is_support_one_record_in_same_language = (rdb_common.GetPath('support_same_language_one_record')).split()  
        self.__is_table_needs_one_record = True        
    
    def Do_CreateTable(self):
        if self.CreateTable2('temp_node_iso_country_code') == -1:
            return -1
    
    def __initialize_filter_condition(self, text_filter_condition):
        temp_filter_list = list()
        temp_list = list()
        
        text_filter_condition_list = text_filter_condition.split('||')
        
        for item in text_filter_condition_list:
            pos_start = item.find('[')
            pos_end = item.find(']')
            temp_filter_list = []
            temp_list = []
            
            if pos_start <> -1 and pos_end <> -1:
                valid_language_code = item[pos_start+1:pos_end]
                temp_filter_list = valid_language_code.split(',')
                
            iso_country_code = item[:item.find(':')]
            
            self.__language_filter_dict[iso_country_code] = temp_filter_list
            
        return True
        
    def Do(self):
        alter_list = [
                      ('rdb_admin_zone', 'ad_name'),
                      ('rdb_link', 'road_name'),
                      ('rdb_link', 'road_number'),
                      ('rdb_link_with_all_attri_view', 'road_name'),
                      ('rdb_link_with_all_attri_view', 'road_number'),
                      ('rdb_guideinfo_building_structure', 'building_name'),
                      #('rdb_guideinfo_crossname', 'cross_name'),
                      #('rdb_guideinfo_road_structure', 'structure_name'),
                      #('rdb_guideinfo_signpost', 'sp_name'),
                      ('rdb_guideinfo_signpost_uc', 'sp_name'),
                      ('rdb_guideinfo_signpost_uc', 'route_no1'),
                      ('rdb_guideinfo_signpost_uc', 'route_no2'),
                      ('rdb_guideinfo_signpost_uc', 'route_no3'),
                      ('rdb_guideinfo_signpost_uc', 'route_no4'),
                      ('rdb_guideinfo_signpost_uc', 'exit_no'),
                      ('rdb_guideinfo_towardname', 'toward_name'),
                      ('rdb_region_link_layer4_tbl', 'road_name'),
                      ('rdb_region_link_layer6_tbl', 'road_name'),
                      ]
        for table_name, field_name in alter_list:
            self.__alterTableField(table_name, field_name)
    
    def __alterTableField(self, table_name, field_name):
        self.log.info('alter language order for %s of %s...' % (field_name, table_name))
        
        # get new name
        update_table_name = 'temp_language_%s_%s' % (table_name, field_name)
        sqlcmd = """
                drop table if exists %s;
                create table %s
                (
                    gid         integer,
                    new_name    varchar
                );
                """ % (update_table_name, update_table_name)
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        if table_name == 'rdb_link':
            sqlcmd = """
                        select gid, iso_country_code, %s
                        from %s
                        where %s is not null
                      """ % (field_name, table_name, field_name)
        elif table_name == 'rdb_link_with_all_attri_view':
            sqlcmd = """
                        update rdb_link_with_all_attri_view as a set %s = b.%s
                        from rdb_link as b
                        where a.link_id = b.link_id;
                      """ % (field_name, field_name)
            self.pg.execute2(sqlcmd)
            self.pg.commit2()
            return
        elif table_name == 'rdb_region_link_layer4_tbl':
            sqlcmd = """
                        update rdb_region_link_layer4_tbl as a set %s = b.%s
                        from 
                        (
                            select a.link_id, c.%s
                            from rdb_region_link_layer4_tbl as a
                            left join rdb_region_layer4_link_mapping as b
                            on a.link_id = b.region_link_id
                            left join rdb_link as c
                            on b.link_id_14[1] = c.link_id
                        ) as b
                        where a.link_id = b.link_id;
                      """ % (field_name, field_name, field_name)
            self.pg.execute2(sqlcmd)
            self.pg.commit2()
            return
        elif table_name == 'rdb_region_link_layer6_tbl':
            sqlcmd = """
                        update rdb_region_link_layer6_tbl as a set %s = b.%s
                        from 
                        (
                            select a.link_id, c.%s
                            from rdb_region_link_layer6_tbl as a
                            left join rdb_region_layer6_link_mapping as b
                            on a.link_id = b.region_link_id
                            left join rdb_link as c
                            on b.link_id_14[1] = c.link_id
                        ) as b
                        where a.link_id = b.link_id;
                      """ % (field_name, field_name, field_name)
            self.pg.execute2(sqlcmd)
            self.pg.commit2()
            return
        elif table_name == 'rdb_admin_zone':
            sqlcmd = """
                        select  gid, 
                                b.iso_country_code, 
                                %s
                        from %s as a
                        left join rdb_admin_order0_iso_country_mapping as b
                        on a.order0_id = b.order0_id
                        where %s is not null
                      """ % (field_name, table_name, field_name)
        elif table_name.startswith('rdb_guideinfo'):
            if not self.__check_gid_column_exist(table_name):
                sqlcmd = """
                        ALTER TABLE %s ADD COLUMN gid bigint;
                            
                        update %s
                        set gid = guideinfo_id;
                """ %(table_name,table_name)
            
                self.pg.execute2(sqlcmd)
                self.pg.commit2()
            
            sqlcmd = """
                        select a.gid, case when b.iso_country_code is not null then b.iso_country_code else c.iso_country_code end, %s
                        from %s as a
                        left join rdb_link as b
                        on a.in_link_id = b.link_id
                        left join temp_node_iso_country_code as c
                        on a.node_id = c.node_id
                        where a.%s is not null
                      """ % (field_name, table_name, field_name)
        else:
            pass
        
        temp_file_obj = common.cache_file.open(update_table_name)
        rec_list = self.pg.get_batch_data2(sqlcmd)
        
        if table_name == 'rdb_guideinfo_signpost_uc' or table_name == 'rdb_guideinfo_towardname':
            self.__is_table_needs_one_record = False

        for rec in rec_list:
            gid = rec[0]
            iso_country_code = rec[1]
            old_name = rec[2]

            is_need_to_input, new_name = self.__alterName(old_name, iso_country_code, field_name)
            if is_need_to_input :
                rec_string = '%s\t%s\n' % (str(gid), new_name)
                temp_file_obj.write(rec_string)
        
        temp_file_obj.seek(0)
        self.pg.copy_from2(temp_file_obj, update_table_name)
        self.pg.commit2()
        common.cache_file.close(temp_file_obj,True)
        sqlcmd = """
                create index %s_gid_idx
                on %s
                using btree
                (gid);
                """ % (update_table_name, update_table_name)
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        # update target table
        sqlcmd = """
                alter table %s rename column %s to %s_bak;
                alter table %s add column %s varchar;
                update %s as a set %s = b.new_name
                from 
                (
                    select  a.gid, 
                            (case when b.gid is null then a.%s_bak else b.new_name end) as new_name
                    from %s as a
                    left join %s as b
                    on a.gid = b.gid
                )as b
                where a.gid = b.gid
                """ % (table_name, field_name, field_name, 
                       table_name, field_name, 
                       table_name, field_name, field_name, table_name, update_table_name)
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        return 0
    
    def __check_gid_column_exist(self, str_table_name):
        sqlcmd = """
                select count(*)
                from information_schema.columns 
                where table_name = '%s' and column_name='gid';
                """ %(str_table_name)
                
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchone2()
        
        if row:
            return True if (row[0] > 0) else False
    
    def __alterName(self, old_name_string, iso_country_code, field_name='road_number'):
        
        target_language_array = self.__language_filter_dict.get(iso_country_code)
        if not target_language_array :
            target_language_array = self.__language_filter_dict.get('DEFAULT')
            
            if not target_language_array:
                target_language_array = []
                self.log.error('can not find the priority language array')
        #
        temp_name = []
        if field_name in ('road_number', 'route_no1', 'route_no2', 'route_no3', 'route_no4'):
            old_name_string = old_name_string.replace('\t', '\\t')
            #old_name_string = old_name_string.replace('"', '\\"')
        old_name = json.loads(old_name_string, encoding='utf8')
        for one_name in old_name:
            (prior, one_name_new) = self.__moveLanguageFirst(iso_country_code, one_name, target_language_array)
            if prior is not None:
                temp_name.append((prior, one_name_new))
                
        if len(temp_name) == 0:
            return False, temp_name
        
        temp_name.sort(key=operator.itemgetter(0))
        
        temp_used_language_list = []
        
        # add operation to support only one record in the same language
        new_name = []
        if self.__is_support_one_record_in_same_language and self.__is_table_needs_one_record:
            for (prior, one_name_new) in temp_name:
                if temp_used_language_list.count(one_name_new[0]['lang']) == 0:
                    temp_used_language_list.append(one_name_new[0]['lang'])
                    new_name.append(one_name_new)
                else:
                    pass
        else:
            for (prior, one_name_new) in temp_name:
                new_name.append(one_name_new)
            
        new_name_string = json.dumps(new_name, ensure_ascii=False, encoding='utf8', sort_keys=True)
        if field_name in ('road_number', 'route_no1', 'route_no2', 'route_no3', 'route_no4'):
            #new_name_string = new_name_string.replace('\\', '\\\\')
            new_name_string = new_name_string.replace('"', '\\"')
        else:
            new_name_string = new_name_string.replace('\\', '\\\\')
            new_name_string = new_name_string.replace('"', '\\"')
        return True, new_name_string
    
    def __judgment_valid_language(self, current_language, valid_language_list):
        return True if valid_language_list.count(current_language) <> 0 else False
    
    def __moveLanguageFirst(self, iso_country_code, one_name, target_language_array=['FRE']):
        # name prior
        # prior = 1: offical=true, has_target_language==true
        # prior = 2: offical=true, has_target_language==false
        # prior = 3: offical=false, has_target_language==true
        # prior = 4: offical=false, has_target_language==false
        prior = 1
        temp_name = []
        
        country_language_code_list = self.__language_filter_dict.get(iso_country_code)
        if not country_language_code_list:
            country_language_code_list = self.__language_filter_dict.get('DEFAULT')
            if not country_language_code_list:
                self.log.error('can not find the priority language array')
                return (None, temp_name)
                
        for one_language in one_name:
            # add language filter
            if not self.__judgment_valid_language(one_language['lang'], country_language_code_list):
                continue
                
            if one_language['type'] == 'office_name':
                base_prior = 1
                if one_language['lang'] in target_language_array:
                    # the more important in the front of language list 
                    pos = target_language_array.index(one_language['lang']) 
                    prior = base_prior + pos
                else:
                    self.log.error('find the language we do not want it!')
            else:
                base_prior = 100
                if one_language['lang'] in target_language_array:
                    pos = target_language_array.index(one_language['lang'])
                    prior = base_prior + pos
                else:
                    self.log.error('find the language we do not want it!')
                    
            temp_name.append((prior, one_language))
            
        if len(temp_name) == 0:
            return (None, None)
        
        temp_name.sort(key=operator.itemgetter(0))
        
        new_one_name = []
        for (prior, one_language) in temp_name:
            new_one_name.append(one_language)
            
#        try:
#            temp= temp_name[0][0]
#        except:
#            print '%s , here is error!' %(one_name)
        return (temp_name[0][0], new_one_name)
    
        