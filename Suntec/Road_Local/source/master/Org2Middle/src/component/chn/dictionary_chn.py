# -*- coding: cp936 -*-
'''
Created on 2012-6-14

@author: hongchenzai
'''

import component.default.dictionary

class comp_dictionary_chn(component.default.dictionary.comp_dictionary):
    '''字典(中国)
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.default.dictionary.comp_dictionary.__init__(self, 'Dictionary')
         
    def _DoCreateTable(self):
        # 名称字典表
        self.CreateTable2('name_dictionary_tbl')
        
        # node名称
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
        # 道路名称字典
        self._MakeLinkName()
        # 道路番号字典
        self._MakeRoadNumber()
        # 交叉点名称字典
        self._MakeCrossName()
        # 方面名称字典
        self._MakeTowardName()
        # 方面看板名称字典
        self._MakeSignPostName()
        # POI名称字典
        self._MakePOIName()
        return 0
        
    def _MakeLinkName(self):
        "道路名称字典"
        self.log.info('Making Link Name Dictionary...')
        sqlcmd = """
            SELECT mid_make_road_name_dictionary();
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        return 0
    
    def _MakeLinkAliasName(self):
        "道路名称(别名)字典"
        # 判断有别名的道路，一定官方名
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
        "道路名称字典"
        # 将一条道路多个番号(G10|G20)，拆成多条记录(G10, G20)
        sqlcmd = """
        INSERT INTO temp_road_number_old (road, route_no, seq_nm)
        (
            SELECT road, regexp_split_to_table(route_no, E'\\｜+') as route_no, 
                   generate_series(1, array_upper(regexp_split_to_array(route_no, E'\\｜+'), 1)) as seq_num
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
        "交叉点名称字典"
        self.log.info('Making Node Name Dictionary...')
        sqlcmd = """
            SELECT mid_make_node_name_dictionary();
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        return 0
    
    def _MakeTowardName(self):
        "方面名称字典"
        return 0
    
    def _MakeSignPostName(self):
        "方面看板名称字典"
        return 0
    
    def _MakePOIName(self):
        "POI名称字典"
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
    