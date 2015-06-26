# -*- coding: cp936 -*-
'''
Created on 2012-12-25

@author: hongchenzai
'''
import component.component_base

class comp_shield_axf(component.component_base.comp_base):
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
        
    def _Do(self):
        #self._DoTemp()
        #return 0;
    
        self.CreateFunction2('mid_cnv_road_type')
        sqlcmd = """
                INSERT INTO link_shield_tbl(
                            link_id, name_id, shield_prior, seqnm)
                (
                select new_road as link_id, name_id,
                    mid_cnv_road_type(
                                                    road_class,
                                                    form_way,
                                                    link_type,
                                                    status,
                                                    ownership,
                                                    name_chn,
                                                    direction
                                                    ) as shield_prior,
                        seq_nm
                  from temp_road_number as a
                  left join org_roadsegment as b
                  on a.meshid = b.meshid and a.road = b.road 
                  left join temp_link_mapping as c
                  on a.road = c.road and a.meshid = c.meshid
                  left join name_dictionary_tbl
                  on new_name_id = name_id and language = 1  -- Simplified Chinese
                  order by new_road, seq_nm
                )
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return 0
    
    def _DoTemp(self):
        # 临时使用，主要是因为无原始数据表，只有
        self.CreateFunction2('mid_cnv_road_type')
        sqlcmd = """
                INSERT INTO link_shield_tbl(
                            link_id, name_id, shield_prior, seqnm)
                (
                select new_road as link_id, name_id,
                    mid_cnv_road_type(
                                                    road_class,
                                                    roadtype_c,
                                                    linktype_c,
                                                    status,
                                                    ownership,
                                                    name_chn,
                                                    oneway_c
                                                    ) as shield_id,
                        seq_nm
                  from temp_road_number as a
                  left join temp_road_newid2 as b
                  on a.meshid = b.meshid and a.road = b.road 
                  left join temp_link_mapping as c
                  on a.road = c.road and a.meshid = c.meshid
                  left join name_dictionary_tbl
                  on new_name_id = name_id and language = 1  -- Simplified Chinese
                )
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return 0