# -*- coding: cp936 -*-
'''
Created on 2012-9-29

@author: hongchenzai
'''
import component.component_base

class comp_shield_jpn(component.component_base.comp_base):
    '''
    shield
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Shield')
        
    def _DoCreateTable(self):
        # 道路名称与字典关系表
        self.CreateTable2('link_shield_tbl')
        
    def _DoCreateIndex(self):
        self.CreateIndex2('link_shield_tbl_link_id_idx')  
       
    def _DoCreateFunction(self):
        
        # 国道为2, 县道为4
        if self.CreateFunction2('rdb_cnv_shield_id') == -1:
            return -1
         
        if self.CreateFunction2('zl_test_function_type') == -1:
            return -1
        return 0
    
    def _Do(self):
        self.__MakeLinkShield()
        return 0
    
    def __MakeLinkShield(self):
        "制作shield信息"
        self.log.info('It is making Link Shield...')
        sqlcmd = """
                insert into link_shield_tbl (link_id, name_id, shield_prior, seqnm)
                (
                SELECT c.tile_link_id, 
                       name_id, 
                       zl_test_function_type(navicls_c, widearea_f) as shield_prior, 
                       1 as seqnm
                  FROM (
                        SELECT objectid, roadcls_c, navicls_c, widearea_f, roadno, split_seq_num
                         FROM temp_road_rlk
                         where roadno > 0
                  ) as a
                  left join temp_link_no as b
                  on a.objectid = b.link_id
                  left join middle_tile_link as c
                  on a.objectid = c.old_link_id and a.split_seq_num = c.split_seq_num
                );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return 0
    
    def _DoCheckNumber(self):
        "判断link"
        sqlcmd = """
                SELECT 
                    (SELECT count(gid)        -- number of link, which have road number. 
                      FROM temp_road_rlk
                      where roadno > 0
                    ) as src_num,
                
                    (SELECT count(gid)
                      FROM link_shield_tbl
                    ) as dest_num;
                """
        
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchone2()
        if row:
            if row[0] != row[1]:
                self.log.warning("Number of ORG Road_NO's links: " + str(row[0]) + ','\
                                 'But Number of link_name_relation_tbl: ' + str(row[1]) + '.')
            else:
                self.log.info("Number of ORG Road_NO's links and "\
                              'Number of link_name_relation_tbl: ' + str(row[1]) + '.')
        return 0
        