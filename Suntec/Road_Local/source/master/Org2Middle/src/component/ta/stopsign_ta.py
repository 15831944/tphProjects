# -*- coding: cp936 -*-

import component.component_base

class comp_stopsign_ta(component.component_base.comp_base):
    '''
    classdocs
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'stopsign')
        
    def _DoCreateTable(self):
        self.CreateTable2('stopsign_tbl')
        return 0
    
    def _DoCreateIndex(self):
        self.CreateIndex2('stopsign_tbl_link_id_idx') 
        return 0    
    
    def _Do(self):
        
        self.CreateTable2('temp_stopsign')  
        sqlcmd = """
            insert into stopsign_tbl(link_id, node_id, pos_flag,the_geom)
            select id
                ,case 
                    when first_length > last_length then t_jnctid 
                    else f_jnctid
                end as node_id
                ,case 
                    when first_length > last_length then 3 
                    else 2
                end as pos_flag
                ,the_geom
            from (
                select ST_Length_Spheroid(first_part,
                                       'SPHEROID("WGS_84", 6378137, 298.257223563)'
                                   ) as first_length 
                    ,ST_Length_Spheroid(last_part,
                                       'SPHEROID("WGS_84", 6378137, 298.257223563)'
                                   ) as last_length
                    ,*
                from (
                    select *,st_line_substring(the_geom_link,0,fraction) as first_part
                        ,st_line_substring(the_geom_link,fraction,1) as last_part
                    from (
                        select *,ST_Line_Locate_Point(the_geom_link,the_geom) as fraction
                        from temp_stopsign 
                    ) a
                ) b
            ) c;
        """
        
        self.pg.execute2(sqlcmd)
        self.pg.commit2()  
        return 0
    
            