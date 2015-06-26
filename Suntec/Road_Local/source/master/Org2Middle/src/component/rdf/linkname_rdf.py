# -*- coding: cp936 -*-
'''
Created on 2012-9-3

@author: hongchenzai
'''
from base import comp_base

class comp_linkname_rdf(comp_base):
    '''字典(日本)
    '''

    def __init__(self):
        '''
        Constructor
        '''
        comp_base.__init__(self, 'Link_Name')
        
    def _DoCreateTable(self):
        # 道路名称与字典关系表
        self.CreateTable2('link_name_relation_tbl')
        
    def _DoCreateFunction(self):
        self.CreateFunction2('mid_make_link_name')
        
    def _Do(self):
        self.__MakeLinkName()
        return 0
    
    def _DoCreateIndex(self):
        self.CreateIndex2('link_name_relation_tbl_link_id_idx')
        self.CreateIndex2('link_name_relation_tbl_name_id_idx')
        self.CreateIndex2('link_name_relation_tbl_country_code_name_type_idx')
        self.CreateIndex2('link_name_relation_tbl_hierarchy_id_name_type_idx')
        
        return 0
        
    def __MakeLinkName(self):
        "道路名称"
        self.log.info('It is making Like name(Merged)...')
        sqlcmd = """
                insert into  link_name_relation_tbl(link_id, country_code, hierarchy_id, name_id, scale_type, 
                        seq_nm, name_type)
                (
                  SELECT a.link_id, b.iso_country_code, 1 as hierarchy_id, name_id, c.road_type as scale_type, 
                      1 as seqnm, 1 as name_type  
                  FROM temp_link_name as a
                  left join temp_rdf_nav_link as b
                  on a.link_id = b.link_id
                  left join link_tbl as c
                  on a.link_id = c.link_id
                );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return 0

        
        