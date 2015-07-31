'''
Created on 2011-12-21

@author: liuxinxing
'''
from base.rdb_ItemBase import ItemBase
from common import rdb_log

class rdb_regulation_client(ItemBase):
    '''
    classdocs
    '''


    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self, 'regulation_client'
                          , 'rdb_link_regulation'
                          , 'record_no'
                          , 'rdb_link_regulation_client'
                          , 'record_no'
                          , False)
        
        
    def Do(self):   
        if self.pg.CreateTable2_ByName('rdb_link_regulation_client') == -1:
            rdb_log.log(self.ItemName, 'Fail to create talbe rdb_regulation_client.', 'error')
            return -1

        # Create Function   
        if self.pg.CreateFunction2_ByName('rdb_split_tileid') == -1:
            return -1
        if self.pg.CreateFunction2_ByName('rdb_split_to_tileid') == -1:
            return -1
        if self.pg.CreateFunction2_ByName('rdb_convert_keystring') == -1:
            return -1
    
        sqlcmd = """
            insert into rdb_link_regulation_client
            (
            record_no, regulation_id, regulation_type, 
            first_link_id, first_link_id_t, 
            last_link_id, last_link_dir, last_link_id_t, 
            link_num, key_string
            ) 
            (
                select  record_no, regulation_id, regulation_type, 
                        rdb_split_tileid(first_link_id), first_link_id_t, 
                        rdb_split_tileid(last_link_id), last_link_dir, last_link_id_t, 
                        link_num, rdb_convert_keystring(key_string)
                from rdb_link_regulation
            )
            """
    
        if self.pg.execute2(sqlcmd) == -1:
            rdb_log.log(self.ItemName, 'Insert record into link regulation client failed.', 'info') 
            self.pg.close2()
            return -1
        
        self.pg.commit2()
    
        self.CreateIndex2('rdb_link_regulation_client_last_link_id_idx')
        self.CreateIndex2('rdb_link_regulation_client_last_link_id_t_idx')  
        self.CreateIndex2('rdb_link_regulation_client_regulation_id_idx')  
        self.CreateIndex2('rdb_link_regulation_client_key_string_idx')  
        self.CreateIndex2('rdb_link_regulation_client_key_string_last_link_id_idx')  
         
        return 0
