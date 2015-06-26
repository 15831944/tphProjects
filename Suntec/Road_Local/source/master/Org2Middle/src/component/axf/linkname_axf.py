# -*- coding: cp936 -*-
'''
Created on 2012-3-29

@author: hongchenzai
'''
import component.component_base

class comp_linkname_axf(component.component_base.comp_base):
    '''字典(日本)
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Link_Name')
        
    def _DoCreateTable(self):
        # 道路名称与字典关系表
        self.CreateTable2('link_name_relation_tbl')
        
    def _DoCreateFunction(self):
        if self.CreateFunction2('mid_cnv_road_type') == -1:
            return -1
        return 0 
    def _Do(self):
        self.__MakeLinkName()
        self.__MakeLinkNumber()
        pass
    
    def _DoCreateIndex(self):
        self.CreateIndex2('link_name_relation_tbl_link_id_idx')
        self.CreateIndex2('link_name_relation_tbl_name_id_idx')
        self.CreateIndex2('link_name_relation_tbl_hierarchy_id_name_type_idx')
        
        return 0
        
    def __MakeLinkName(self):
        "道路名称"
        self.log.info('It is making link name...')
        sqlcmd = """
                insert into link_name_relation_tbl (link_id, hierarchy_id, name_id, scale_type, shield_id, seq_nm, name_type, country_code)
                (
                SELECT new_road as link_id, 1, new_name_id, c.fc as scale, null as shield_id, a.seq_nm, a.name_type, 'CHN'
                  FROM temp_org_rass_name as a
                  left join temp_rass_name as b
                  on a.gid = b.gid
                  left join org_roadsegment as c
                  on a.id = c.road and a.meshid = c.meshid
                  left join temp_link_mapping as d
                  on a.id = d.road and a.meshid = d.meshid
                  where name_type in (1, 2)
                );
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return 0
    
    def __MakeLinkNumber(self):
        "道路番号"
        # 番号统一做到link_shield_tbl, link_name_relation_tbl不再存储番号信息
        return 0
    
        self.log.info('It is making link number...')
        sqlcmd = """
                insert into link_name_relation_tbl (link_id, hierarchy_id, name_id, scale_type, shield_id, seq_nm, name_type, country_code)
                (
                SELECT new_road as link_id, 1, new_name_id, c.fc as scale, 
                       mid_cnv_road_type(
                                    road_class,
                                    form_way,
                                    link_type,
                                    status,
                                    ownership,
                                    name_chn,
                                    direction
                                    ) as shield_id, a.seq_nm, 3, 'CHN'
                  FROM temp_road_number as a
                  left join temp_link_mapping as b
                  on a.road = b.road and a.meshid = b.meshid
                  left join org_roadsegment as c
                  on a.road = c.road and a.meshid = c.meshid
                );    
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return 0
    
        
    
        