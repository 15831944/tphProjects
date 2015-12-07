# -*- coding: UTF8 -*-
#!/usr/bin/python
'''
Created on 2015-11-4

@author: wushengbing
'''
import component.component_base


class comp_update_link_type_sapa_jct(component.component_base.comp_base):
    '''
    update link_type of sapa link
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Update_link_type')
    
        
    def _Do(self):
        self.__backup_link_tbl_before_linktype_adjust()
        
        self.__get_ic_jct_link()
        self.__modify_jct_paths_both_end_all_the_same()
        self.__get_jct_link_revise()
        self.__get_ic_link_change_to_jct()
        self.__get_sapa_link()
        self.__get_ic_change_to_sapa()
        
        self.__backup_link_tbl_after_linktype_adjust()

        
       
    def __backup_link_tbl_before_linktype_adjust(self):
        self.log.info('backup link_tbl before link_type adjust...')
        sqlcmd = '''
            drop table if exists temp_link_tbl_bak_before_linktype_adjust;
            create table temp_link_tbl_bak_before_linktype_adjust
            as
            select * from link_tbl
        
        '''   
        self.pg.execute2(sqlcmd)
        self.pg.commit2() 
        
    def __backup_link_tbl_after_linktype_adjust(self):
        self.log.info('backup link_tbl after link_type adjust...')
        sqlcmd = '''
            drop table if exists temp_link_tbl_bak_after_linktype_adjust;
            create table temp_link_tbl_bak_after_linktype_adjust
            as
            select * from link_tbl
        
        '''   
        self.pg.execute2(sqlcmd)
        self.pg.commit2() 
        
                
    def __get_ic_jct_link(self):
        self.log.info('Updating link_type of ic/jct links...')
        sqlcmd = '''
            drop table if exists temp_link_type_modify_test;
            create table temp_link_type_modify_test
            as
            (
                select link_id,link_type_new
                from
                (
                    select b.link_id, min(new_link_type) as link_type_new ,c.link_type
                    from temp_link_ramp_single_path as b
                    join link_tbl as c
                    on c.link_id = b.link_id
                    group by b.link_id,c.link_type
                ) as d
                where d.link_type not in (0,7) 
                and d.link_type <> d.link_type_new
            );
            
          
            update link_tbl as a
            set link_type = e.link_type_new
            from temp_link_type_modify_test as e
            where a.link_id = e.link_id;
       
         '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.__get_jct_link_change_to_ic()
        
        
        
    def __get_jct_link_change_to_ic(self):
        
        self.log.info('change jct link to ic link...')
        self.CreateFunction2('mid_highway_connect')
        self.CreateFunction2('mid_cal_zm')
        self.CreateFunction2('mid_calc_jct_steering_angle')
        self.CreateFunction2('mid_filter_the_large_angle_jct')
        self.CreateFunction2('mid_jct_linktype_modify')
        self.CreateFunction2('mid_jct_change_to_ic_connect_to_highway_end')
        self.CreateTable2('temp_jct_change_to_ic')
        
        sqlcmd = '''
                select mid_jct_linktype_modify(a.snode,a.path, a.s_node, a.e_node,a.direction,120)
                from 
                (    
                    select distinct a.* , b.s_node, b.e_node
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
        
    def __modify_jct_paths_both_end_all_the_same(self):
        self.log.info('modify jct links which the paths both end are the same...') 
        self.CreateTable2('temp_jct_path_both_end_same')
        self.CreateFunction2('mid_modify_jct_path_both_end_same')
        sqlcmd = '''
            select mid_modify_jct_path_both_end_same(d.path1::bigint[], d.path2::bigint[])
            from
            (
                select s_link, e_link, string_to_array(path_array[1],'|') as path1, string_to_array(path_array[2],'|') as path2 
                from 
                (
                    select s_link,e_link ,array_agg(array_to_string(links,'|')) as path_array 
                    from 
                    (
                        select distinct a.links[1] as s_link,a.links,a.links[array_length(a.links,1)] as e_link,link_type 
                        from temp_jct_change_to_ic as a
                        where a.link_type = 3 
                    ) as b
                    group by s_link,e_link having count(*) = 2
                ) as c
            ) as d
                    
            ''' 
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        sqlcmd = '''
            drop table if exists temp_jct_path_both_end_same_change;
            create table temp_jct_path_both_end_same_change
            as
            (
                select c.link_id,c.link_type
                from
                (
                    select b.link_id,min(b.link_type) as link_type
                    from
                    (
                        select unnest(a.path) as link_id, link_type 
                        from temp_jct_path_both_end_same as a
                    ) as b
                    group by b.link_id
                ) as c
                left join link_tbl as d
                on c.link_id = d.link_id 
                where d.link_type <> c.link_type
            );
            
            update link_tbl as a
            set link_type = b.link_type
            from temp_jct_path_both_end_same_change as b
            where a.link_id = b.link_id;
        '''  
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
              
    def __get_jct_link_revise(self):
        self.log.info('update jct link which the paths contain sapa link...')
        self.CreateFunction2('mid_highway_connect')
        self.CreateFunction2('mid_find_jct_link_paths') 
        self.CreateFunction2('mid_find_suspect_jct_link')
        self.CreateFunction2('mid_jct_link_paths_filter')
        
        self.CreateTable2('temp_suspect_jct_paths_link')
        self.CreateTable2('temp_jct_paths_link')
         
        sqlcmd = '''
            select mid_find_suspect_jct_link(15)
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
                select b.link_id, 3 as link_type
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
            
        
    def __get_ic_link_change_to_jct(self): 
        self.log.info('update ic link to jct link, which both end connect to jct ...')
        self.CreateFunction2('mid_find_ic_surround_by_jct')
        self.CreateFunction2('mid_find_ic_surround_by_jct_exec')
        self.CreateTable2('temp_ic_surround_by_jct_link')
        
        sqlcmd = '''
            select mid_find_ic_surround_by_jct_exec()
        '''
        
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        sqlcmd = '''
            update link_tbl as k
            set link_type = 3
            from
            (
                select b.link_id
                from
                (
                    select distinct unnest(a.links) as link_id
                    from temp_ic_surround_by_jct_link as a
                ) as b
                left join link_tbl as c
                on c.link_id = b.link_id
                where c.link_type = 5
            ) as d
            where d.link_id = k.link_id
        
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
           
           
    def __get_sapa_link(self):
        self.log.info('get sapa links ...')
        self.CreateFunction2('mid_regulation_exist_on_path')
        self.CreateFunction2('mid_large_steering_angle_on_sapa_path_connect_highway')
        
        sqlcmd = '''
        drop table if exists temp_the_final_sapa_link;
        create table temp_the_final_sapa_link
        as
        (
            select e.link_id,highway_angle
            from
            (
                select link_id,min(highway_angle) as highway_angle
                from
                (
                    select distinct unnest(link_path) as link_id ,highway_angle
                    from
                    (
                        select mid_large_steering_angle_on_sapa_path_connect_highway(link_path,60) as highway_angle,link_path 
                        from 
                        (
                            select mid_regulation_exist_on_path(a.link_path,a.direction) as regulation , a.*  
                            from temp_suspect_jct_paths_link as a
                        ) as b
                        where b.regulation = 0
                    ) as c
                ) as d
                group by d.link_id
            ) as e
            left join link_tbl as f
            on f.link_id = e.link_id
            where e.highway_angle = 0 and f.link_type = 5
        )
        
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2() 
        
        sqlcmd = '''
        
            update link_tbl as a
            set link_type = 7
            from temp_the_final_sapa_link as b
            where b.link_id = a.link_id
        
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
    def __get_ic_change_to_sapa(self):
        self.log.info('update ic link to sapa link, which both end connect to sapa ...')
        self.CreateFunction2('mid_find_ic_surround_by_sapa')
        self.CreateFunction2('mid_find_ic_surround_by_sapa_exec')
        self.CreateTable2('temp_ic_surround_by_sapa_link')
        
        sqlcmd = '''
            select mid_find_ic_surround_by_sapa_exec()
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        sqlcmd = '''
            update link_tbl as k
            set link_type = 7
            from
            (
                select b.link_id
                from
                (
                    select distinct unnest(a.links) as link_id
                    from temp_ic_surround_by_sapa_link as a
                ) as b
                left join link_tbl as c
                on c.link_id = b.link_id
                where c.link_type = 5
            ) as d
            where d.link_id = k.link_id
        
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
