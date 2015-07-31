# -*- coding: cp936 -*-
'''
Created on 2012-9-29

@author: hongchenzai
'''
from base import comp_base

class comp_shield_ta(comp_base):
    '''
    shield
    '''

    def __init__(self):
        '''
        Constructor
        '''
        comp_base.__init__(self, 'Shield')
        
    def _DoCreateTable(self):
        # 道路名称与字典关系表
        self.CreateTable2('link_shield_tbl')
        
    def _DoCreateIndex(self):
        self.CreateIndex2('link_shield_tbl_link_id_idx')  
        
    def _Do(self):
        self.__MakeLinkShield()
        return 0
    
    def __MakeLinkShield(self):
        "制作shield信息"
        self.log.info('It is making Link Shield...')
        sqlcmd = """
                insert into link_shield_tbl (link_id, name_id, shield_prior, seqnm)
                (
                SELECT link_id, new_name_id, rteprior, seqnm
                  FROM temp_mid_shield_number
                  order by link_id, seqnm
                );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return 0
        