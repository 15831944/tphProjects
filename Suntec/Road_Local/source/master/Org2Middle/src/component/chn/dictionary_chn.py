# -*- coding: cp936 -*-
'''
Created on 2012-6-14

@author: hongchenzai
'''

import component.default.dictionary

class comp_dictionary_chn(component.default.dictionary.comp_dictionary):
    '''�ֵ�(�й�)
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.default.dictionary.comp_dictionary.__init__(self, 'Dictionary')
         
    def _DoCreateTable(self):
        # �����ֵ��
        self.CreateTable2('name_dictionary_tbl')
        
        # node����
        self.CreateFunction1('temp_node_name')
        
        self.CreateFunction1('temp_road_number_old')
        self.CreateFunction1('temp_road_number')
        
        return 0
    
    def _DoCreateFunction(self):
        self.CreateFunction2('mid_translate')
        self.CreateFunction2('mid_add_one_name')
        self.CreateFunction2('mid_make_road_name_dictionary')
        self.CreateFunction2('mid_make_road_alias_name_dictionary')
        self.CreateFunction2('mid_make_node_name_dictionary')
        self.CreateFunction2('mid_make_road_number_dictionary')
        return 0
    
    def _Do(self):
        # ��·�����ֵ�
        self._MakeLinkName()
        # ��·�����ֵ�
        self._MakeRoadNumber()
        # ����������ֵ�
        self._MakeCrossName()
        # ���������ֵ�
        self._MakeTowardName()
        # ���濴�������ֵ�
        self._MakeSignPostName()
        # POI�����ֵ�
        self._MakePOIName()
        return 0
        
    def _MakeLinkName(self):
        "��·�����ֵ�"
        self.log.info('Making Link Name Dictionary...')
        sqlcmd = """
            SELECT mid_make_road_name_dictionary();
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        return 0
    
    def _MakeLinkAliasName(self):
        "��·����(����)�ֵ�"
        # �ж��б����ĵ�·��һ���ٷ���
        if self.__CheckRoadAliasName() == -1:
            self.log.error("A link has alias name, but it hasn't official name .")
            return -1
        
        self.log.info('Making Link Name Dictionary...')
        sqlcmd = """
            SELECT mid_make_road_alias_name_dictionary();
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        return 0
    
    def _MakeRoadNumber(self):
        "��·�����ֵ�"
        # ��һ����·�������(G10|G20)����ɶ�����¼(G10, G20)
        sqlcmd = """
        INSERT INTO temp_road_number_old (road, route_no, seq_nm)
        (
            SELECT road, regexp_split_to_table(route_no, E'\\��+') as route_no, 
                   generate_series(1, array_upper(regexp_split_to_array(route_no, E'\\��+'), 1)) as seq_num
              from org_roadsegment
              where route_no is not null
        );
        """
        
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        # 
        sqlcmd = """
            SELECT mid_make_road_number_dictionary();
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        return 0
    
    def _MakeCrossName(self):
        "����������ֵ�"
        self.log.info('Making Node Name Dictionary...')
        sqlcmd = """
            SELECT mid_make_node_name_dictionary();
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        return 0
    
    def _MakeTowardName(self):
        "���������ֵ�"
        return 0
    
    def _MakeSignPostName(self):
        "���濴�������ֵ�"
        return 0
    
    def _MakePOIName(self):
        "POI�����ֵ�"
        return 0
    
    def __CheckRoadAliasName(self):
        sqlcmd = """
            SELECT count(gid)
              FROM org_roadsegment
              where alias_chn is not null and name_chn is null;
        """
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchone2()
        if row:
            if row[0] == 0:
                return 0
            else:
                return -1
        return 0
    