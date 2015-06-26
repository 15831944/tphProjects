# -*- coding: cp936 -*-
'''
Created on 2012-7-5

@author: hongchenzai
'''
import component.component_base

class comp_linkname_jpn(component.component_base.comp_base):
    '''��·���ƺͷ���
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Link_Name')
        
    def _DoCreateTable(self):
        # ��·�������ֵ��ϵ��
        self.CreateTable2('link_name_relation_tbl')
        
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
        "��·����"
        self.log.info('It is making link name...')
        sqlcmd = """
                insert into link_name_relation_tbl (link_id, hierarchy_id, name_id, scale_type, shield_id, seq_nm, name_type, country_code)
                (
                SELECT d.link_id, 1 as hierarchy_id, name_id, function_class as scale_type, null as shield_id, 
                    1 as seq_nm, 1 as offical_name, 'JP' as country_code
                  FROM temp_road_rlk as a
                  left join temp_link_name as b
                  on a.roadcode = b.road_code 
                  left join middle_tile_link as c
                  on a.objectid = c.old_link_id and a.split_seq_num = c.split_seq_num
                  left join link_tbl as d
                  on c.tile_link_id = d.link_id
                  where a.roadcode > 0 and b.road_code is not null
                  --order by b.objectid, b.split_seq_num
                );
                """
        # ע�⣺ road_rlk���roadcode����Щ��road_roadcode�����Ҳ��������Լ�������(b.road_code is not null).
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return 0
    
    def __MakeLinkNumber(self):
        "��·����"
        # �ձ����յ��ϲ���ʾ��·�ŷ��ţ� ���԰���������ص�
        return 0
    
        self.log.info('It is making link number...')
        sqlcmd = """
                insert into link_name_relation_tbl (link_id, hierarchy_id, name_id, scale_type, shield_id, seq_nm, name_type, country_code)
                (
                SELECT d.link_id, 1 as hierarchy_id, name_id, function_class as scale_type, function_class as shield_id, 
                    1 as seq_nm, 3 as road_number, 'JP' as country_code
                  FROM temp_road_rlk as a
                  left join temp_link_no as b
                  on a.roadno = b.roadno
                  left join middle_tile_link as c
                  on a.objectid = c.old_link_id and a.split_seq_num = c.split_seq_num
                  left join link_tbl as d
                  on c.tile_link_id = d.link_id
                  where a.roadno > 0
                  --order by b.objectid, b.split_seq_num
                );
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return 0
    
        
    
        