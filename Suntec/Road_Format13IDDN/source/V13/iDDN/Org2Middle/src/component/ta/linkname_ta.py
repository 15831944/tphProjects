# -*- coding: cp936 -*-
'''
Created on 2012-9-3

@author: hongchenzai
'''
from base import comp_base

class comp_linkname_ta(comp_base):
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
        #self.CreateFunction2('mid_make_link_number')
        
    def _Do(self):
        self.__MakeLinkCountry()
        self.__MakeLinkName()
        pass
    
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
                  SELECT a.link_id, b.country_code, 1 as hierarchy_id, new_name_id, c.road_type as scale_type, 
                      1 as seqnm,  a.name_type  -- name_type: [1: officer], [2: alter name], [3: Route Number]
                  FROM temp_mid_road_name as a
                  left join temp_link_country as b
                  on a.link_id = b.link_id
                  left join link_tbl as c
                  on a.link_id = c.link_id
                  where c.link_id is not null
                );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return 0
    
    def __MakeLinkCountry(self):
        "名称/番号道路的country_code"
        # 从gc表, 查找名称道路的国家code
        # 如果r_order00有值就用r_order00，否则用l_order00
        
        self.CreateTable2('temp_link_country')
        sqlcmd = """
                insert into temp_link_country
                (
                SELECT id, country_code
                  from (
                    SELECT id, 
                           (case when r_order00 is not null then r_order00 else l_order00 end) as country_code
                      from gc
                  ) as a
                  group by id, country_code 
                );
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return 0
    
    def _DoCheckLogic(self):
        self.__CheckLinkName()
        #self.__CheckLinkNumber()
        return 0
    
    def __CheckLinkName(self):
        "检查道路名称和原始数据是否一致"
        sqlcmd = """
                SELECT count(id)
                  from gc
                  left join link_name_relation_tbl as a
                  on id = link_id
                  LEFT JOIN name_dictionary_tbl as b
                  on a.name_id = b.name_id
                  where nametyp & 4 <> 4 and b.name = fullname 
                    and link_id is null;
                """
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchone2()
        if row:
            if row[0] > 0:
                self.log.error('Check Link Name Failed.')
            else:
                self.log.info('Checked Link Name Logic.')
        return 0
    
    def _DoCheckNumber(self):
        "判断link"
        sqlcmd = """
                SELECT 
                    (SELECT count(gid)        -- number of link_name 
                      FROM gc
                      where nametyp & 4 <> 4
                    )
                    +
                    (SELECT count(gid)        -- number of link_number
                      FROM rn
                    ) as src_num,
                
                    (SELECT count(gid)
                      FROM link_name_relation_tbl
                    ) as dest_num;
                """
        
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchone2()
        if row:
            if row[0] != row[1]:
                self.log.warning('Number of gc and rn: ' + str(row[0]) + ','\
                                 'But Number of link_name_relation_tbl: ' + str(row[1]) + '.')
            else:
                self.log.info('Number of gc and rn: ' + str(row[0]) + ','\
                              'Number of link_name_relation_tbl: ' + str(row[1]) + '.')
        return 0
        
        
        