# -*- coding: UTF8 -*-
#!/usr/bin/python
'''
Created on 2015-9-17

@author: wushengbing
'''
import component.component_base
import common.common_func

class comp_update_sapa_link(component.component_base.comp_base):
    '''
    update link_type of sapa link
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Update_link_type')
#        self.proj_name = common.common_func.GetProjName()  
#        self.proj_country = common.common_func.getProjCountry()
        
    
    
    
    def _Do(self):
        
        self.__find_highway_sapa_link()
        
#        #method 1
#        self.__find_sapa_link_suspect()
#        self.__updata_sapa_link()
        
        #method 2
        self.__find_sapa_link()
        self.__update_sapa_link_rev2()
        
    
            
    def __find_highway_sapa_link(self):
        
        self.log.info('find highway sapa link...')
        
        sqlcmd = '''
            drop table if exists temp_highway_link_buffer;
            create table temp_highway_link_buffer
            as 
            (
                select link_id,ST_Buffer(the_geom, 0.0063, 8) as the_geom_buff,the_geom
                from link_tbl as a
                where a.road_type in (0,1)
            )
            
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        sqlcmd = '''
            drop table if exists temp_highway_sapa_link;
            create table temp_highway_sapa_link
            as
            (
                select distinct a.* 
                from 
                (
                    select * from link_tbl as a 
                    where a.link_type = 7
                ) as a
                join temp_highway_link_buffer as b
                on ST_Intersects(a.the_geom, b.the_geom_buff)
            );
            
          '''
               
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('temp_highway_sapa_link_s_node_idx')
        self.CreateIndex2('temp_highway_sapa_link_e_node_idx')
   
        
    def __find_sapa_link_suspect(self):
        
        self.log.info('find the suspect sapa link...')
        
        sqlcmd = '''
            drop table if exists temp_find_sapa_passlinks;
            create table temp_find_sapa_passlinks
            (
                gid serial,
                passlink varchar
            );
        
         '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        
        self.CreateFunction2('mid_find_sapa_links') 
        sqlcmd = '''
            select mid_find_sapa_links(a.link_id, 10)  
            from temp_highway_sapa_link as a
        
         '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        sqlcmd = '''
            drop table if exists temp_find_sapa_link_suspect;
            create table temp_find_sapa_link_suspect
            as 
            (
                select b.* 
                from link_tbl as b
                join
                (
                    select distinct unnest(string_to_array(a.passlink,'|'))::bigint as link_id  
                    from temp_find_sapa_passlinks as a
                ) as c
                on b.link_id = c.link_id and b.link_type <> 7
            )
        
           '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        
        self.CreateFunction2('mid_delete_not_sapa_link')
        sqlcmd = '''
             select mid_delete_not_sapa_link()
    
           '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
       
    
    def __find_sapa_link(self):   
        self.log.info('find sapa link...')
        self.CreateFunction2('mid_find_sapa_links_rev2')
        self.CreateFunction2('mid_change_sapa')
        self.CreateTable2('temp_sapa_link')
        
        sqlcmd = '''
            select mid_change_sapa(40)
            '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
    
        sqlcmd = '''
            drop table if exists temp_change_to_sapa_link;
            create table temp_change_to_sapa_link
            as
            (
                select b.* from link_tbl as b
                join 
                (
                    select distinct unnest(a.link_array) as link_id 
                    from temp_sapa_link as a
                ) as c
                on b.link_id = c.link_id
                where b.link_type <> 7
            )
            
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
    
        
    def __updata_sapa_link(self):
        
        self.log.info('update link_type...')
        sqlcmd = '''
            update link_tbl as a
            set link_type = 7
            from temp_find_sapa_link_suspect as b
            where b.link_id = a.link_id
            
           '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
    
    
    def __update_sapa_link_rev2(self):
        self.log.info('update link_type by method 2...')
        sqlcmd = '''
            update link_tbl as a
            set link_type = 7
            from temp_change_to_sapa_link as b
            where a.link_id = b.link_id

        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        
        
        
        
        
        
        
        
        
      
            
      
     
        
