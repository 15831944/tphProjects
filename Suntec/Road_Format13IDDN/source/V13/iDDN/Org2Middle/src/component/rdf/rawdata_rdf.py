# -*- coding: cp936 -*-
'''
Created on 2014-6-3

@author: zhangpeng
'''
from base import comp_base
import common.ntsamp_to_lhplus
from common import cache_file

class comp_rawdata_rdf(comp_base):
    '''
    modify the rawdata
    '''

    def __init__(self):
        '''
        Constructor
        '''
        comp_base.__init__(self, 'rawdata')
        
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
        
    def __fix_phoneme(self):
        self.log.info('revise the vce_phonetic_text table ...')
        if not self.pg.IsExistTable('vce_phonetic_text_org'):
            sql = '''
                  alter table vce_phonetic_text RENAME TO  vce_phonetic_text_org;
                  CREATE TABLE vce_phonetic_text
                  (
                      phonetic_id              bigint NOT NULL,
                      phonetic_string          character varying(250) NOT NULL,
                      phonetic_language_code   character(3) NOT NULL,
                      transcription_method     character(1) NOT NULL,
                      CONSTRAINT pk_vce_phonetic PRIMARY KEY (phonetic_id)
                  );
                  '''
            self.pg.execute2(sql)
            self.pg.commit2()
        
        self.pg.execute2('delete from vce_phonetic_text')
        self.pg.commit2()

            
        sql = '''
              select phonetic_id, phonetic_string, phonetic_language_code, transcription_method 
                from vce_phonetic_text_org;
              '''
        
        temp_file_obj = cache_file.open('vce_phonetic_text_new')
        phlist = self.get_batch_data(sql)
        for ph in phlist:
            fields = list(ph) 
            fields[1] = common.ntsamp_to_lhplus.nt_sampa_2_lh_plus(ph[2], ph[1])
            self.__store_name_to_temp_file(temp_file_obj, fields[0], fields[1], fields[2], fields[3])
        
        # ## 
        temp_file_obj.seek(0)
        self.pg.copy_from2(temp_file_obj, 'vce_phonetic_text')
        self.pg.commit2()
        # close file
        cache_file.close(temp_file_obj,True)
#        temp_file_obj.close()
            
#        while min_id <= max_id :
#            cur_id = min(min_id + 5000000,max_id)
#            self.pg.execute2(sql1,(min_id,cur_id))
#            min_id = cur_id + 1       
#            phlist = self.pg.fetchall2()
#            sql2 = '''
#                  insert into vce_phonetic_text values(%s,%s,%s,%s)
#                  '''
#            for ph in phlist:
#                fields = list(ph)
#                fields[1] = common.ntsamp_to_lhplus.nt_sampa_2_lh_plus(ph[2], ph[1])
#                
#                self.pg.execute2( sql2, fields )
#            self.pg.commit2()

