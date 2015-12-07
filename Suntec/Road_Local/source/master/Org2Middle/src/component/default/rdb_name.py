# -*- coding: UTF8 -*-
'''
Created on 2015-5-8

@author: liuxinxing
'''

import json
import operator

import component.component_base
import common.config
import common.cache_file
import threading

from common.database import pg_client

class rdb_name_language_order(component.component_base.comp_base):
    '''
    classdocs
    '''
    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Name_filter')
        text_filter_condition = common.config.CConfig.instance().getPara('text_filter')
        self.temp_proj_name = common.config.CConfig.instance().getPara('proj_name')
        self.temp_proj_country = common.config.CConfig.instance().getPara('proj_country')
        temp_key = (self.temp_proj_name + "_" + self.temp_proj_country).upper()
        
        temp_dict = eval(text_filter_condition)
        if temp_dict is not None:
            cur_target_language_sort_condition = temp_dict[temp_key]
        
        self.__language_filter_dict = dict()
        if not self.__initialize_filter_condition(cur_target_language_sort_condition):
            self.log.error('make language filter condition error')
            
        self.__is_support_one_record_in_same_language = (common.config.CConfig.instance().getPara('support_same_language_one_record')).split()  
        self.__is_table_needs_one_record = True   
        
    def get_language_filter_dict(self):
        return self.__language_filter_dict
    
    def get_support_one_record_in_same_language_flag(self):
        return self.__is_support_one_record_in_same_language
    
    def get_current_table_needs_one_record_flag(self):
        return self.__is_table_needs_one_record
    
    def Do_CreateTable(self):
        if self.CreateTable2('temp_node_iso_country_code') == -1:
            return -1
    
    def __initialize_filter_condition(self, text_filter_condition):
        temp_filter_list = list()
        
        text_filter_condition_list = text_filter_condition.split('||')
        
        for item in text_filter_condition_list:
            pos_start = item.find('[')
            pos_end = item.find(']')
            temp_filter_list = []
            
            if pos_start <> -1 and pos_end <> -1:
                valid_language_code = item[pos_start+1:pos_end]
                temp_filter_list = valid_language_code.split(',')
                
            iso_country_code = item[:item.find(':')]
            
            self.__language_filter_dict[iso_country_code] = temp_filter_list
            
        return True
        
    def _Do(self):
        alter_list = [
#                      ('rdb_admin_zone', 'ad_name'),
#                      ('rdb_link', 'road_name'),
#                      ('rdb_link', 'road_number'),
#                      ('rdb_link_with_all_attri_view', 'road_name'),
#                      ('rdb_link_with_all_attri_view', 'road_number'),
#                      ('rdb_guideinfo_building_structure', 'building_name'),
#                      #('rdb_guideinfo_crossname', 'cross_name'),
#                      #('rdb_guideinfo_road_structure', 'structure_name'),
#                      #('rdb_guideinfo_signpost', 'sp_name'),
#                      ('rdb_guideinfo_signpost_uc', 'sp_name'),
#                      ('rdb_guideinfo_signpost_uc', 'route_no1'),
#                      ('rdb_guideinfo_signpost_uc', 'route_no2'),
#                      ('rdb_guideinfo_signpost_uc', 'route_no3'),
#                      ('rdb_guideinfo_signpost_uc', 'route_no4'),
#                      ('rdb_guideinfo_signpost_uc', 'exit_no'),
#                      ('rdb_guideinfo_towardname', 'toward_name'),
#                      ('rdb_region_link_layer4_tbl', 'road_name'),
#                      ('rdb_region_link_layer6_tbl', 'road_name'),
#                      ('rdb_region_link_layer8_tbl', 'road_name'),
                        ('link_tbl', 'road_name'),
                        ('link_tbl', 'road_number'),
                      ]
        for table_name, field_name in alter_list:
            if self.pg.IsExistTable(table_name):
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
        
        #judge gid
        sqlcmd = '''
                select *
                from   information_schema.columns
                where table_name = '%s' and column_name = 'gid';
                '''%table_name
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchone2()
        if not row:
            #gid not in table
            sqlcmd = '''
                    ALTER TABLE %s ADD COLUMN gid serial;
                    '''% table_name
            self.pg.execute2(sqlcmd)
            self.pg.commit2()
            
        (min_gid, max_gid) = self.pg.getMinMaxValue(table_name, 'gid')
        
        temp_file_obj = common.cache_file.open(update_table_name)
        
        if table_name == 'rdb_guideinfo_signpost_uc' or table_name == 'rdb_guideinfo_towardname' \
            or (table_name == 'link_tbl' and field_name == 'road_number'):
            self.__is_table_needs_one_record = False
            
        main_obj = CMultiPgExecute(min_gid, max_gid, 4, 1000000, temp_file_obj, table_name, field_name,update_table_name, \
                                   self.get_language_filter_dict(),\
                                   self.get_support_one_record_in_same_language_flag(),\
                                   self.get_current_table_needs_one_record_flag())
        main_obj.execute()
        
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
 
class CMultiPgExecute():
    def __init__(self, min_gid, max_gid, thread_number, block_number, file_obj, table_name, field_name, update_table_name,\
                 language_filter_dict,is_support_one_record_in_same_language,is_table_needs_one_record):
        self.min_gid = min_gid
        self.max_gid = max_gid
        self.thread_number = thread_number
        self.block_number = block_number
        self.file_operator = file_obj
        self.table_name = table_name
        self.field_name = field_name
        self.update_table_name = update_table_name
        self.language_filter_dict = language_filter_dict
        self.is_support_one_record_in_same_language = is_support_one_record_in_same_language
        self.is_table_needs_one_record = is_table_needs_one_record
    
    def execute(self):
        self.sem = threading.Semaphore(self.thread_number)
        self.exception_flag = 0
        self.thread_list = {}
        
        self.mutex = threading.Lock()
        
        current_gid = self.min_gid
        while (not self.exception_flag) and (current_gid <= self.max_gid):
            from_gid = current_gid
            to_gid = current_gid + self.block_number - 1
            
            self.sem.acquire()
            objThread = threading.Thread(target=self.__do_alter, args=(from_gid, to_gid, self.table_name, self.field_name, self.file_operator, \
                                                                       self.update_table_name, self.mutex, \
                                                                       self.language_filter_dict,\
                                                                       self.is_support_one_record_in_same_language,\
                                                                       self.is_table_needs_one_record))
            self.thread_list[from_gid] = objThread
            objThread.start()
            
            current_gid = to_gid + 1
        
        for objThread in self.thread_list.values():
            objThread.join()
        
        if self.exception_flag:
            raise Exception, 'multi-thread exception'

    def __do_alter(self, from_gid, to_gid, table_name, field_name,file_operator, update_table_name, mutex_obj,\
                   language_filter_dict,is_support_one_record_in_same_language,is_table_needs_one_record):  
        try:
            try:
                name_operator = name_operator_class(from_gid, to_gid, table_name, field_name,file_operator,\
                                                     update_table_name, mutex_obj,\
                                                     language_filter_dict,\
                                                     is_support_one_record_in_same_language,\
                                                     is_table_needs_one_record)
                if name_operator is not None:
                    name_operator.run_name_modify()
            finally:
                name_operator.db_close()
                del self.thread_list[from_gid]
                self.sem.release()
        except:
            self.exception_flag = 1
            raise Exception, 'multi-thread exception'

class name_operator_class():
    def __init__(self, from_gid, to_gid, table_name, field_name,file_operator, update_table_name, mutex_obj, \
                 language_filter_dict, \
                 is_support_one_record_in_same_language,\
                 is_table_needs_one_record
                 ):
        self.from_gid = from_gid
        self.to_gid = to_gid
        self.table_name = table_name
        self.field_name = field_name
        self.file_operator = file_operator
        self.update_table_name = update_table_name
        self.mutex_obj = mutex_obj
        self.__language_filter_dict = language_filter_dict
        self.__is_support_one_record_in_same_language = is_support_one_record_in_same_language
        self.__is_table_needs_one_record = is_table_needs_one_record
    
    def run_name_modify(self):
        if self.table_name == 'link_tbl':
            sqlcmd = """
                        select gid, iso_country_code, %s
                        from %s
                        where %s is not null and (gid >=%s and gid <= %s)
                      """ % (self.field_name, self.table_name, self.field_name, self.from_gid, self.to_gid)
        else:
            pass
        
        self.__objPg = pg_client()
        self.__objPg.connect2()
        
        rec_list = self.__objPg.get_batch_data2(sqlcmd)
        
        for rec in rec_list:
            gid = rec[0]
            iso_country_code = rec[1]
            old_name = rec[2]
    
            is_need_to_input, new_name = self.__alterName(old_name, iso_country_code, self.field_name)
            if is_need_to_input :
                rec_string = '%s\t%s\n' % (str(gid), new_name)
                if self.mutex_obj.acquire():
                    self.file_operator.write(rec_string)
                    self.mutex_obj.release()
        
#        file_operator.seek(0)
#        objPg.copy_from2(file_operator, update_table_name)
#        objPg.commit2()
#        common.cache_file.close(file_operator,True)
    
    def db_close(self):
        self.__objPg.close2()
        
    def __alterName(self, old_name_string, iso_country_code, field_name='road_number'):
        
        target_language_array = self.__language_filter_dict.get(iso_country_code)
        if not target_language_array :
            target_language_array = self.__language_filter_dict.get('DEFAULT')
            
            if not target_language_array:
                target_language_array = []
                self.log.error('can not find the priority language array')
        #
        temp_name = []
        if field_name in ('road_number', 'route_no1', 'route_no2', 'route_no3', 'route_no4', 'toward_name'):
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
        