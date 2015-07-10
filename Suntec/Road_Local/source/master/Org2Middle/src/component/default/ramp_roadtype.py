# -*- coding: cp936 -*-
'''
Created on 2012-8-17

@author: hongchenzai
@alter: zhengchao
'''
import component.component_base

class comp_ramp_roadtype(component.component_base.comp_base):
    '''
    Ramp RoadType
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Ramp_RoadType')        
        
    def _DoCreateTable(self):
        
        if self.CreateTable2('temp_link_ramp_single_path') == -1:
            return -1 
        if self.CreateTable2('temp_link_ramp') == -1:
            return -1 
        if self.CreateTable2('temp_link_ramp_toohigh') == -1:
            return -1
        if self.CreateTable2('temp_link_ramp_toohigh_single_path') == -1:
            return -1
        return 0
    
    def _DoCreateFunction(self):
        
        if self.CreateFunction2('mid_cnv_ramp_atnode') == -1:
            return -1
        if self.CreateFunction2('mid_cnv_ramp') == -1:
            return -1
        if self.CreateFunction2('mid_cnv_ramp_toohigh') == -1:
            return -1
        if self.CreateFunction2('mid_cnv_ramp_toohigh_search') == -1:
            return -1
        return 0
        
    def _DoCreateIndex(self): 
        
        return 0
    
    def _Do(self):
        # ����Ramp��road_type��FC
        self._ConvertRampRoadTypeFC()
        # ����link_tbl��Ramp��RoadType��FC
        self._UpdateRampRoadTypeFC()
        # ����link_tbl��Ramp��Display Class
        #self._UpdateRampRoadDisplayClass()#20140619ʽ���޸ģ�������ʾ����Ҫ��ramp��display_class�����޸�
        self._ConvertRampRoadTypeFC_Toohigh()
        self._UpdateRampRoadTypeFC_Toohigh()
        return 0
    
    def _ConvertRampRoadTypeFC(self):
        "����Ramp��road_type��FC���������ĸߵȼ���·һ�¡�"
        sqlcmd = """
                SELECT mid_cnv_ramp();
            """
        
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
        return 0
    
    def _UpdateRampRoadTypeFC(self):
        "����link_tbl��Ramp��RoadType��FC"
        
        self.log.info('Updating RoadType and FC of Ramp for link_tbl.')
        sqlcmd = """
                UPDATE link_tbl as a
                   SET road_type = (case when b.new_road_type in (0,1) then b.new_road_type else a.road_type end), 
                       function_class = (case when b.new_fc < a.function_class then b.new_fc else a.function_class end)
                   FROM temp_link_ramp as b
                 WHERE a.link_id = b.link_id;
            """
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
        return 0

    def _ConvertRampRoadTypeFC_Toohigh(self):
        "����Ramp��road_type��FC�������ĵ͵ȼ���·һ�¡�"
        sqlcmd = """
                SELECT mid_cnv_ramp_toohigh();
            """
        
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
        return 0
    
    def _UpdateRampRoadTypeFC_Toohigh(self):
        "����link_tbl��Ramp��RoadType��FC"
        
        self.log.info('Updating RoadType and FC of Ramp for link_tbl.')
        sqlcmd = """
                UPDATE link_tbl as a
                   SET road_type = (case when b.new_road_type not in (0,1) then b.new_road_type else a.road_type end), 
                       function_class = (case when b.new_fc > a.function_class then b.new_fc else a.function_class end)
                   FROM temp_link_ramp_toohigh as b
                 WHERE a.link_id = b.link_id;
            """
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
        return 0
    
    def _UpdateRampRoadDisplayClass(self):
        "����link_tbl��Ramp��Display Class. ע��ֻ�������й���TomTom���⣬�������ձ���"
        
        self.log.info('Updating Display Class of Ramp for link_tbl.')
        sqlcmd = """
                UPDATE link_tbl as a
                   SET display_class = (
                        case
                        when b.new_road_type = 0 then 12
                        when b.new_road_type = 1 then 13
                        else a.display_class end)
                   FROM temp_link_ramp as b
                 WHERE a.link_id = b.link_id and a.road_type <> b.new_road_type;
            """
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
        return 0