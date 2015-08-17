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
        if self.CreateTable2('temp_roundabout') == -1:
            return -1
        if self.CreateTable2('temp_roundabout_for_searchramp') == -1:
            return -1
        if self.CreateTable2('temp_roundabout_road_type') == -1:
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
        return 0
        
    def _DoCreateIndex(self): 
        
        return 0
    
    def _Do(self):
        
        # �ҵ�����ramp��·ͨ���Ļ���
        
        self._findproperroundabout()
        # ����Ramp��road_type��FC
        self._ConvertRampRoadTypeFC()
        # ����link_tbl��Ramp��RoadType��FC
        self._UpdateRampRoadTypeFC()
        # ����link_tbl��Ramp��Display Class
        self._UpdateRampRoadDisplayClass()#20140619ʽ���޸ģ�������ʾ����Ҫ��ramp��display_class�����޸�
        #self._ConvertRampRoadTypeFC_Toohigh()
        self._UpdateRampRoadTypeFC_Toohigh()
        
        self._UpdateRoundaboutRoadType()
        
        self._UpdateRampDisplayClass()
        return 0
    
    def _findproperroundabout(self):
        '�ҵ����ֻ�������ֻ���Ӹ��ٱ��߻���sapalink�����ѵ��Ļ���'
        
        if not (self.proj_name.lower()=='rdf' and self.proj_country.lower()=='hkg'):
            return 0
        
        self.log.info('Finding proper roundabout.')
        sqlcmd = """
                SELECT mid_find_roundabout();
            """
        
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
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
    
    def _UpdateRoundaboutRoadType(self):
        '����Roundabout��road_type'
        
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
    
    def _UpdateRampDisplayClass(self):
        
        self.log.info('Updating Disp Class of Ramp')
        
        self.search_road=common.search_road.search_road(link_id=None,dir=0,onewayflg=True,thrucondition=' link_type=5',errorcondition=' road_type in (0,1)',pg=self.pg)
        sqlcmd= '''
                select link_id
                from link_tbl
                where road_type in (0,1) and link_type in (1,2) and one_way_code <> 0
                '''
        sqlcmd_update='''
                update link_tbl
                set display_class=%d
                where link_id=%d
                        '''
        self.pg.execute(sqlcmd)
        link_id_list=self.pg.fetchall2()
        #print link_id_list
        for (link_id,) in link_id_list:
            self.search_road.start_link_id=link_id
            self.search_road.init_path()
            self.search_road.search()
            results=self.search_road.return_paths()
            if not results or (len(results)==1 and len(results[0]['path'])<3):
                continue
            #print not results
            sqlcmd=''' select display_class from link_tbl where link_id=%d'''
            display_class_list=[]
            all_link_list=set([])
            for result in results:
                
                if result['endflag']<>3:
                    #print result
                    self.pg.execute(sqlcmd%(result['path'][-1]))
                    display_class_list.append(self.pg.fetchall2()[0][0])
                    all_link_list=all_link_list|set(result['path'][1:-1])
            #print display_class_list
            new_display_class=max(display_class_list)
            for link_id in all_link_list:
                #print link_id
                self.pg.execute(sqlcmd_update% (new_display_class,link_id))
        self.pg.commit()