# -*- coding: cp936 -*-
'''
Created on 2014-6-3

@author: zhangpeng
'''
import component.component_base
import common.ntsamp_to_lhplus
from common import cache_file
from common.phoneme_change import *

class comp_rawdata_rdf(component.component_base.comp_base):
    '''
    modify the rawdata
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'rawdata')
        
    def _DoCreateTable(self):
        pass
        
    def _DoCreateIndex(self):
        pass  
        
    def _Do(self):
        #if not self.pg.IsExistTable('org_revise_flag'):
        #we always modify the phonetic table
        self.__fix_phoneme()
        #self.__set_revise_flag()
        return 0
    
    def __set_revise_flag(self):
        self.log.info('set flag to indicate that has revised rawdata ...')
        sqlcmd = '''
                 create table org_revise_flag
                 (
                     key  int PRIMARY KEY not null
                 );
                 '''
        self.pg.execute2( sqlcmd )
        self.pg.commit2()
        
    def __store_name_to_temp_file(self, file_obj,phonetic_strid, phonetic_string, phonetic_code, strmethod):
        if file_obj:
            file_obj.write('%s\t%s\t%s\t%s\n' % (phonetic_strid, phonetic_string,phonetic_code,strmethod))
        return 0
    
    def __get_parameters(self, country, language):
        if country == 'HONGKONG' and language == 'UKE':
            return "NTSAMPA", "LHPLAS"
        elif country == 'HONGKONG' and language == 'PYM':
            return "PYM", "PinYinTone"
        elif country == 'HONGKONG' and language == 'PYT':
            return "PYT", "PinYinTone"
        elif country == 'TAIWAN' and language == 'TWN':
            return "BPMF", "PinYinTone"
        elif country == 'SOURTHEASTASIA' and language == 'THA':
            return "NTSAMPA", "LHPLAS" 
        else:
            return None, None
        
    def __fix_phoneme(self):
        self.log.info('revise the vce_phonetic_text table ...')
        if not self.pg.IsExistTable('vce_phonetic_text_org'):
            sql = '''
                  alter table vce_phonetic_text RENAME TO  vce_phonetic_text_org;
                  '''
            self.pg.execute2(sql)
            self.pg.commit2()
        
        sql = '''
              drop table if exists vce_phonetic_text;
              CREATE TABLE vce_phonetic_text
              (
                  phonetic_id              bigint NOT NULL,
                  phonetic_string          character varying(512),
                  phonetic_language_code   character(3) NOT NULL,
                  transcription_method     character(1) NOT NULL,
                  CONSTRAINT pk_vce_phonetic PRIMARY KEY (phonetic_id)
              );
              '''
        self.pg.execute2(sql)
        self.pg.commit2()
        
        # 
        sqlcmd = """
            SELECT count(*)
              FROM vce_phonetic_text_org;
            """
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchone2()
        vce_count  = row[0]
        
        # get current country
        current_country = common.common_func.GetPath('proj_country').lower()
        if current_country == 'hkg':
            country = 'HONGKONG'
        elif current_country == 'twn':
            country = 'TAIWAN'
        elif current_country == 'ase':
            country = 'SOURTHEASTASIA'
        else:
            country = 'OTHERS'
            
        sql = r'''
              select phonetic_id, phonetic_string, phonetic_language_code, transcription_method 
                from vce_phonetic_text_org;
              '''
        if vce_count > 100000 :
            temp_file_obj = cache_file.open('vce_phonetic_text_new')
            phlist = self.get_batch_data(sql)
            for ph in phlist:
                fields = list(ph) 
#                fields[1] = common.ntsamp_to_lhplus.nt_sampa_2_lh_plus(ph[2], ph[1])
                parm1, parm2 = self.__get_parameters(country, ph[2])
                if parm1 is not None and parm2 is not None:
                    func = get_translator(country, ph[2], parm1, parm2)
                    fields[1] = func.change(ph[1])
                    
                    if ph[2] == 'PYM' or ph[2] == 'PYT':
                        funccheck = get_checker(country, ph[2], parm1, parm2)
                        if funccheck.check(fields[1]) <> 'True':
                            self.log.info('origin data :%s ,language: %s, transformed data:%s ,transformed data is error, delete it!' % (ph[1],ph[2], fields[1]))
                            fields[1] = ''
                        
                fields[1] = fields[1].replace('\\', '\\\\')
                self.__store_name_to_temp_file(temp_file_obj, fields[0], fields[1], fields[2], fields[3])
            
            # ## 
            temp_file_obj.seek(0)
            self.pg.copy_from2(temp_file_obj, 'vce_phonetic_text')
            self.pg.commit2()
            # close file
            cache_file.close(temp_file_obj,True)
        else:
            self.pg.execute2(sql)
            phlist = self.pg.fetchall2()
            sql2 = '''
                  insert into vce_phonetic_text values(%s,%s,%s,%s)
                  '''
            for ph in phlist:
                fields = list(ph)
#                fields[1] = common.ntsamp_to_lhplus.nt_sampa_2_lh_plus(ph[2], ph[1])
                parm1, parm2 = self.__get_parameters(country, ph[2])
                if parm1 is not None and parm2 is not None:
                    func = get_translator(country, ph[2], parm1, parm2)
                    fields[1] = func.change(ph[1])
                
                self.pg.execute2( sql2, fields )
            self.pg.commit2()

