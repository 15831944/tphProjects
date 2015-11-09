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
        
        # 找到允许ramp算路通过的环岛
        
        self._findproperroundabout()
        # 调整Ramp的road_type和FC
        self._ConvertRampRoadTypeFC()
        
#        self.__Update_ic_jct_linktype()
#        ##修正jct 作成
#        self.__update_jct_link_revise()
        
        # 更新link_tbl中Ramp的RoadType和FC
        self._UpdateRampRoadTypeFC()
        # 更新link_tbl中Ramp的Display Class
        #self._UpdateRampRoadDisplayClass()#20140619式样修改：根据显示美观要求，ramp的display_class不做修改
        self._ConvertRampRoadTypeFC_Toohigh()
        self._UpdateRampRoadTypeFC_Toohigh()
        
        self._UpdateRoundaboutRoadType()
       
        return 0
    
    
    def __Update_ic_jct_linktype(self):
        self.log.info('Updating link_type of ic/jct links...')
        sqlcmd = '''
            drop table if exists temp_link_type_modify_test;
            create table temp_link_type_modify_test
            as
            (
                select link_id,link_type_new,link_type
                from
                (
                    select b.link_id, min(new_link_type) as link_type_new ,c.link_type
                    from temp_link_ramp_single_path as b
                    join link_tbl as c
                    on c.link_id = b.link_id
                    group by b.link_id,c.link_type
                ) as d
                where d.link_type not in (0,7) and d.link_type <> d.link_type_new
            );
            
            
            update link_tbl as a
            set link_type = e.link_type_new
            from temp_link_type_modify_test as e
            where a.link_id = e.link_id;
            
         '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.__change_jct_link_to_ic()
        
        
        
    def __change_jct_link_to_ic(self):
        
        self.log.info('change jct link to ic link...')
        self.CreateFunction2('mid_highway_connect')
        self.CreateFunction2('mid_jct_linktype_modify')
        self.CreateTable2('temp_jct_change_to_ic')
        sqlcmd = '''
                select mid_jct_linktype_modify(a.snode,a.path, a.s_node, a.e_node)
                from 
                (    
                    select a.* , b.s_node, b.e_node
                    from temp_jct_link_paths as a
                    left join link_tbl as b
                    on a.elink = b.link_id
                    
                ) as a
           
                '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        sqlcmd = '''
        
            drop table if exists temp_jct_link_modify_test;
            create table temp_jct_link_modify_test
            as
            (
                select c.link_id, c.new_link_type
                from
                (
                    select b.link_id, min(b.link_type) as new_link_type
                    from 
                    (
                        select unnest(a.links) as link_id, a.link_type
                        from temp_jct_change_to_ic as a
                    ) as b
                    group by b.link_id
                ) as c    
                join link_tbl as d
                on d.link_id = c.link_id
                where c.new_link_type <> d.link_type
            
            );
            
            
            update link_tbl as a
            set link_type = e.new_link_type
            from temp_jct_link_modify_test as e
            where a.link_id = e.link_id;
        '''
        
        self.pg.execute2(sqlcmd)
        self.pg.commit2() 
        
    def __update_jct_link_revise(self):
        self.log.info('update jct link for the special case...')
        self.CreateFunction2('mid_highway_connect')
        self.CreateFunction2('mid_find_jct_link_paths') 
        self.CreateFunction2('mid_find_suspect_jct_link')
        self.CreateFunction2('mid_jct_link_paths_filter')
        self.CreateTable2('temp_suspect_jct_paths_link')
        self.CreateTable2('temp_jct_paths_link')
         
        sqlcmd = '''
            select mid_find_suspect_jct_link(8)
        ''' 
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        sqlcmd = '''
            select mid_jct_link_paths_filter(k.node_s, k.link_path, k.node1,k.node2)
            from
            (
                select distinct  c.link_path,
                    ( case  when d.s_node in (e.s_node,e.e_node) then d.s_node
                        when d.e_node in (e.s_node,e.e_node) then d.e_node end) as node_s,
                    f.s_node as node1, 
                    f.e_node as node2
                from
                (
                    select  b.link_path,
                        b.link_path[1]::bigint as s_link,
                        b.link_path[array_length(b.link_path,1)]::bigint as e_link
                    from temp_suspect_jct_paths_link as b
                    
                ) as c
                left join link_tbl as d
                on d.link_id = c.s_link
                join
                ( 
                  select * from link_tbl 
                  where link_type in (1,2) and road_type in (0,1)
                ) as e
                on d.s_node in (e.s_node,e.e_node) or d.e_node in (e.s_node,e.e_node)
                left join link_tbl as f
                on f.link_id = c.e_link
                --order by c.link_path
            )  as k

        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        sqlcmd = '''
            drop table if exists temp_revise_jct_link;
            create table temp_revise_jct_link
            as
            (
                select b.link_id 
                from
                (
                    select distinct  unnest(a.link_path)::bigint as link_id 
                    from temp_jct_paths_link as a
                ) as b
                left join link_tbl as c
                on b.link_id = c.link_id
                where c.link_type = 5
            );  
            
            update link_tbl as a
            set link_type = 3
            from temp_revise_jct_link as b
            where a.link_id = b.link_id
            
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()  
         
        
    def _findproperroundabout(self):
        '找到这种环岛：即只连接高速本线或者sapalink或者匝道的环岛'
        
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
