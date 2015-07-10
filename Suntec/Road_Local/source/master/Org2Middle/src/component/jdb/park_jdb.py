# -*- coding: cp936 -*-
'''
Created on 2014-4-22

@author: zhaojie
'''
import component.component_base
import common.common_func

class comp_park_jdb(component.component_base.comp_base):
    '''illust
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'park')
        
    def _DoCreateTable(self):
        self.CreateTable2('park_link_tbl')     
        self.CreateTable2('park_node_tbl')
        self.CreateTable2('park_polygon_tbl')
        self.CreateTable2('temp_park_link')
        self.CreateTable2('temp_park_node')
        return 0
    
    def _Do(self):
        
        strrun_mode = common.common_func.GetPath('park_flag')
        if strrun_mode == 'true':
            self.__make_temp_table()
            
            self.__make_park_link_for_middle()
            self.__make_park_node_for_middle()
            self.__make_park_polygon_for_middle()
            self.__update_link_tbl()
            self.__update_node_tbl()
            pass
        return 0
    
    def __make_temp_table(self):
        self.log.info('start make temp_park_link and temp_park_node parking_id!')
        
        #make node tileID  
        self.CreateFunction2('mid_get_link_node_array')
        self.CreateFunction2('mid_get_link_node_parking_id')
        self.pg.callproc('mid_get_link_node_parking_id')
        self.pg.commit2()
        
        self.log.info('end make temp_park_link and temp_park_node parking_id!')
        return 0
    
    def __make_park_node_for_middle(self):
        self.log.info('start make park_node_tbl for middle!')
        
        self.CreateTable2('temp_park_node_connect')
        self.CreateFunction2('mid_make_park_node_tbl')
        self.pg.callproc('mid_make_park_node_tbl')
        self.pg.commit2()
        
        self.log.info('end make park_node_tbl for middle!')
        return 0
    
    def __make_park_link_for_middle(self):
        self.log.info('start make park_link_tbl for middle!')
        
        self.CreateFunction2('mid_make_park_link_tbl')
        self.pg.callproc('mid_make_park_link_tbl')
        self.pg.commit2()
        
        self.log.info('end make park_link_tbl for middle!')
        return 0
    
    def __make_park_polygon_for_middle(self):
        self.log.info('start make park_polygon_tbl for middle!')
        
        self.CreateFunction2('mid_make_park_polygon_tbl')
        self.pg.callproc('mid_make_park_polygon_tbl')
        self.pg.commit2()
        
        self.log.info('end make park_polygon_tbl for middle!')
        return 0
    
    def __update_link_tbl(self):
        self.log.info('start insert into link from park_link!')
        
        #display_class
        sqlcmd = '''
            update link_tbl as a
                set display_class = 31
            where link_id in (
                select park_link_id
                from park_link_tbl
                where park_link_type = 1
            );
        '''
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
        
        #display_class    
        sqlcmd = '''
            update link_tbl as a
                set display_class = 30
            where link_id in (
                select park_link_id
                from park_link_tbl
                where park_link_type = 0
            );
        '''
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
            
        #feature_string   
        sqlcmd = '''
            update link_tbl as a
                set feature_string = feature_string || ',' || 1::varchar
            where feature_string is not null and link_id in (
                select park_link_id
                from park_link_tbl
            );
        '''
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
            
        #feature_key   
        sqlcmd = '''
            update link_tbl as a
                set feature_key = md5(feature_string)
            where feature_string is not null and link_id in (
                select park_link_id
                from park_link_tbl
            );
        '''
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
            
        self.log.info('end insert into link from park_link!')
        return 0 
    
    def __update_node_tbl(self):
        self.log.info('start insert into node from park!')
        sqlcmd = '''
            update node_tbl
            set kind = (d.park_node_type+100)::character varying
            from
            (
                select park_node_id,park_node_type
                from park_node_tbl
            ) as d
            where node_id = d.park_node_id;
                '''
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
            
        #feature_string     
        sqlcmd = '''
            update node_tbl
            set feature_string = feature_string || ',' || 1::varchar
            where feature_string is not null and node_id in (
                select park_node_id
                from park_node_tbl
                where park_node_type = 0
            );
                '''
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
            
        #feature_key     
        sqlcmd = '''
                update node_tbl
                set feature_key = md5(feature_string)
                where feature_string is not null and node_id in (
                    select park_node_id
                    from park_node_tbl
                    where park_node_type = 0
                );
                '''
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
            
        self.log.info('end insert into node from park!')
        return 0
    
    def _DoCreateIndex(self):
        self.CreateIndex2('park_link_id_idx')
        self.CreateIndex2('park_node_id_idx')
        self.CreateIndex2('park_polygon_id_idx')
        return 0
    
    def __GetRows(self,sqlcmd):
        if sqlcmd:
            self.pg.execute2(sqlcmd)
            return self.pg.fetchall2()
        else:
            self.log.error('sqlcmd is null;')
        return 0  
             
    def _DoCheckValues(self):
        self.log.info('start check values')
        sqlcmd = """
                 SELECT park_link_id
                 FROM park_link_tbl
                 WHERE park_s_node_id IS NULL or park_e_node_id is null
                 union
                 select park_link_id
                 from
                 (
                     select park_link_id,b.park_node_id as s_id,c.park_node_id as e_id
                     from park_link_tbl as a
                     left join
                     park_node_tbl as b
                     on a.park_s_node_id = b.park_node_id
                     left join
                     park_node_tbl as c
                     on a.park_e_node_id = c.park_node_id
                 )as d
                 where d.s_id is null or d.e_id is null;
                 """

        rows = self.__GetRows(sqlcmd)
        if rows:
            self.log.error('in park_link_tbl,park_s_node_id or park_e_node_id is error!')
            return -1
        self.log.info('end check values')
        return 0
    def _DoCheckNumber(self):
        self.log.info('start check park number')
        
        sqlcmd = '''
                select park_link_id,link_id
                from park_link_tbl as a
                left join
                link_tbl as b
                on a.park_link_id = b.link_id  and a.park_link_oneway = b.one_way_code  
                    and a.park_s_node_id = b.s_node and a.park_e_node_id = b.e_node
                    and (b.display_class = 30 or b.display_class = 31)
                where b.link_id is null;
                '''
        rows = self.__GetRows(sqlcmd)
        if rows:
            self.log.error('park_link_tbl not in link_tabl!')
            return -1
        
        sqlcmd = '''
                select park_node_id,node_id
                from park_node_tbl as a
                left join
                node_tbl as b
                on a.park_node_id = b.node_id and b.kind in ('100','101','102','103')
                where b.node_id is null;
                '''
        rows = self.__GetRows(sqlcmd)
        if rows:
            self.log.error('park_node_tbl not in node_tbl!')
            return -1
        
        self.log.info('end check park number')
        return 0