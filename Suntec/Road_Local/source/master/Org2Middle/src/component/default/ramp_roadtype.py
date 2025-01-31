# -*- coding: cp936 -*-
'''
Created on 2012-8-17

@author: hongchenzai
@alter: zhengchao
'''
import component.component_base
import common.common_func
import common.search_road

class comp_ramp_roadtype(component.component_base.comp_base):
    '''
    Ramp RoadType
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Ramp_RoadType')
        self.proj_name = common.common_func.GetProjName()  
        self.proj_country = common.common_func.getProjCountry()
                
    def _DoCreateTable(self):
        
        if self.CreateTable2('temp_link_ramp_single_path') == -1:
            return -1 
        if self.CreateTable2('temp_link_ramp') == -1:
            return -1 
        if self.CreateTable2('temp_link_ramp_toohigh') == -1:
            return -1
        if self.CreateTable2('temp_link_ramp_toohigh_single_path') == -1:
            return -1
        if self.CreateTable2('temp_single_roundabout') == -1:
            return -1
        if self.CreateTable2('temp_roundabout_for_searchramp') == -1:
            return -1
        if self.CreateTable2('temp_roundabout_road_type') == -1:
            return -1
        if self.CreateTable2('link_tbl_bak_ramp_roadtype')== -1:
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
        if self.CreateFunction2('mid_find_roundabout') == -1:
            return -1
        if self.CreateFunction2('mid_get_angle_sequence_temp') == -1:
            return -1
        if self.CreateFunction2('mid_cal_zm')== -1:
            return -1
        return 0
        
    def _DoCreateIndex(self): 
        
        return 0
    
    def _Do(self):
        
        # 找到允许ramp算路通过的环岛
        
        self._findproperroundabout()
        # 调整Ramp的road_type和FC
        self._ConvertRampRoadTypeFC()
              
        # 更新link_tbl中Ramp的RoadType和FC
        self._UpdateRampRoadTypeFC()
        # 更新link_tbl中Ramp的Display Class
        #self._UpdateRampRoadDisplayClass()#20140619式样修改：根据显示美观要求，ramp的display_class不做修改
        self._ConvertRampRoadTypeFC_Toohigh()
        self._UpdateRampRoadTypeFC_Toohigh()
        
        self._UpdateRoundaboutRoadType()
       
        return 0
   
      
    def _findproperroundabout(self):
        '找到这种环岛：即只连接高速本线或者sapalink或者匝道的环岛'
        
        if not (self.proj_name.lower()=='rdf' and self.proj_country.lower()=='hkg'):
            return 0
        
        self.log.info('Finding proper roundabout.')

        if self.CreateTable2('temp_roundabout') == -1:
            return -1

        sqlcmd = """
                SELECT mid_find_roundabout();
            """
        
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
        return 0
        
    def _ConvertRampRoadTypeFC(self):
        self.CreateTable2('temp_jct_link_paths')
        "调整Ramp的road_type和FC，和相连的高等级道路一致。"
        sqlcmd = """
                SELECT mid_cnv_ramp();
            """
        
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
        return 0
    
    def _UpdateRampRoadTypeFC(self):
        "更新link_tbl中Ramp的RoadType和FC"
        
        self.log.info('Updating RoadType and FC of Ramp for link_tbl.')
        sqlcmd = """
                UPDATE link_tbl as a
                   SET road_type = (case when b.new_road_type in (0,1) and b.new_road_type<a.road_type then b.new_road_type else a.road_type end), 
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
        "调整Ramp的road_type和FC和相连的低等级道路一致。"
        sqlcmd = """
                SELECT mid_cnv_ramp_toohigh();
            """
        
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
        return 0
    
    def _UpdateRampRoadTypeFC_Toohigh(self):
        "更新link_tbl中Ramp的RoadType和FC"
        
        self.log.info('Updating RoadType and FC of Ramp for link_tbl.')
        sqlcmd = """
                UPDATE link_tbl as a
                   SET road_type = (case when b.new_road_type not in (0,1) and b.new_road_type>a.road_type then b.new_road_type else a.road_type end)--, 
                      -- function_class = (case when b.new_fc > a.function_class then b.new_fc else a.function_class end)
                   FROM temp_link_ramp_toohigh as b
                 WHERE a.link_id = b.link_id;
            """
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
        return 0
    
    def _UpdateRampRoadDisplayClass(self):
        "更新link_tbl中Ramp的Display Class. 注：只适用于中国和TomTom海外，不适用日本。"
        
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
    
    def _UpdateRoundaboutRoadType(self):
        '提升Roundabout的road_type'
        
        self.log.info('Updating Road type of Roundabout for link_tbl.')
        sqlcmd = """
                update link_tbl a
                set road_type=b.new_road_type
                from
                (
                    select b.link_id,a.new_road_type
                    from 
                    (
                        select roundabout_id,min(new_road_type) as new_road_type 
                        from temp_roundabout_road_type
                        group by roundabout_id
                    ) a
                    join temp_roundabout_for_searchramp b
                    on a.roundabout_id=b.roundabout_id
                ) b
                where a.link_id=b.link_id
            """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
