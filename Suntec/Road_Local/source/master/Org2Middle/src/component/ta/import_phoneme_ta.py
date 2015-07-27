# -*- coding: UTF8 -*-
'''
Created on 2014-1-28

@author: hongchenzai
'''
import component.component_base
from common.dirwalker import DirWalker
import os

SEP_CHAR = '|'
SEP_CHAR_TAB = '\t'


class comp_import_phoneme_ta(component.component_base.comp_base):
    '''导入音素数据
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self,
                                               'Import_Phoneme')
        self.all_files = list()
        self.vm_dict = {'org_vm_pt': ('_pt.txt', SEP_CHAR_TAB),
                        'org_vm_nefa': ('_nefa.txt', SEP_CHAR),
                        'org_vm_foa': ('_foa.txt', SEP_CHAR),
                        'org_vm_ne': ('_ne.txt', SEP_CHAR),
                        'org_vm_foa_area': ('_foa_area.txt', SEP_CHAR),
                        'org_vm_comp': ('_comp.txt', SEP_CHAR),
                        'org_vm_sh': ('_sh.txt', SEP_CHAR),
                        'org_vm_area': ('_area.txt', SEP_CHAR),
                        'org_vm_pt_label': ('', SEP_CHAR),
                        'org_vm_geovariant': ('', SEP_CHAR),
                        'org_vm_alphabet_classification': ('', SEP_CHAR),
                        }

    def _DoCreateIndex(self):
        # ne
        self.CreateIndex2('org_vm_ne_ptid_idx')
        # nefa
        self.CreateIndex2('fki_nefa_foa')
        self.CreateIndex2('fki_nefa_ne')
        self.CreateIndex2('org_vm_nefa_featclass_idx')

        # pt
        self.CreateIndex2('org_vm_pt_ptid_idx')
        self.CreateIndex2('org_vm_pt_ptid_lanphonset_idx')
        # 外键
        sqlcmd = """
        ALTER TABLE org_vm_nefa
          ADD CONSTRAINT "fk_nefa_foa" FOREIGN KEY (featdsetid, featsectid,
                                                  featlayerid, featitemid,
                                                  featcat, featclass)
              REFERENCES org_vm_foa (featdsetid, featsectid,
                                      featlayerid, featitemid,
                                      featcat, featclass) MATCH SIMPLE
              ON UPDATE NO ACTION ON DELETE NO ACTION;
        """
        if self.pg.execute2(sqlcmd) == -1:
            return -1
    
        sqlcmd = """
        ALTER TABLE org_vm_nefa
          ADD CONSTRAINT "fk_nefa_ne" FOREIGN KEY (featdsetid, featsectid,
                                                  featlayerid, nameitemid)
              REFERENCES org_vm_ne (namedsetid, namesectid,
                                  namelayerid, nameitemid) MATCH SIMPLE
              ON UPDATE NO ACTION ON DELETE NO ACTION;
        """
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        
        return 0

    def _Do(self):
        if not self._set_phoneme_path():
            self.log.error('Not indicate phoneme path.')
        self._load_file_to_db()
        return 0

    def _load_file_to_db(self):
        for table, (suffix_name, sep) in self.vm_dict.iteritems():
            self.CreateTable2(table)
            if not suffix_name:  # 不知道后缀
                continue
            files = self._search_files(suffix_name)
            for phoneme_file in files:
                fp = open(phoneme_file, 'r')
                #print phoneme_file
                tmp_file=open('tmp_file.txt','w')
                tmp_file.write(fp.read().replace('\\','\\\\'))
                tmp_file.close()
                fp = open ('tmp_file.txt','r')
                self.pg.copy_from2(fp, table, sep)
                self.pg.commit2()
                fp.close()
                os.remove('tmp_file.txt')
            self.log.info('Import phoneme: %s' % table)
            
            sqlcmd = """ANALYZE %s;"""%table
            if self.pg.execute2(sqlcmd) == -1:
                return -1
            self.pg.commit2()
        return

    def _set_phoneme_path(self):
        import common.common_func
        phoneme_path = common.common_func.GetPath('ta_phoneme_sidefile')
        if phoneme_path:
            self.dir_walder = DirWalker(phoneme_path)
            for phoneme_file in self.dir_walder:
                self.all_files.append(phoneme_file)
            return True
        return False

    def _search_files(self, suffix_name=None):
        if not self.all_files:
            return list()
        if not suffix_name:
            return list(self.dir_walder)
        files = list()
        import os
        for phoneme_file in self.all_files:
            file_ext = os.path.splitext(phoneme_file)
            sufix = file_ext[1]
            file_name = os.path.split(file_ext[0])[1].encode("utf-8") + sufix
            pos_suffix = file_name.rfind(suffix_name)
            pos_underline = file_name.find('_')
            if ((pos_suffix + len(suffix_name) == len(file_name)) and
                pos_suffix == pos_underline):  # 前面不能有下划线
                files.append(phoneme_file)
        return files
