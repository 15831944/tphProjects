# -*- coding: cp936 -*-
'''
Created on 2013-12-12

@author: hongchenzai
'''
from base.rdb_ItemBase import ItemBase
from common import rdb_log


class rdb_language(ItemBase):
    '''语言种别
    '''
    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self, 'Language')

    def Do_CreateTable(self):
        self.CreateTable2('rdb_language')

    def Do(self):
        sqlcmd = """
        INSERT INTO rdb_language(language_id, l_full_name, l_talbe,
                 pronunciation, p_table, language_code,
                 language_code_client, language_id_client)
        (
        SELECT language_id, l_full_name, l_talbe,
               pronunciation, p_table, language_code,
               language_code_client, language_id_client
          FROM language_tbl
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def Do_CreatIndex(self):
        if self.CreateIndex2('rdb_language_language_code_idx') == -1:
            return -1
