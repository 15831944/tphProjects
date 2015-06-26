# -*- coding: UTF8 -*-

import component.component_base

class comp_stopsign_rdf(component.component_base.comp_base):
    
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

        sqlcmd = """
            insert into stopsign_tbl(link_id, node_id, pos_flag,the_geom)
            select c.link_id,c.node_id
                ,case when c.node_id = d.ref_node_id then 2
                    when c.node_id = d.nonref_node_id then 3
                    else 0
                end as pos_flag
                ,e.the_geom
            from rdf_condition as a
            inner join rdf_condition_driver_alert as b
            on a.condition_id = b.condition_id and a.condition_type = 17 and b.traffic_sign_type=20
            inner join rdf_nav_strand as c
            on a.nav_strand_id = c.nav_strand_id
            inner join temp_rdf_nav_link d
            on c.link_id = d.link_id
            inner join temp_rdf_nav_node e
            on c.node_id = e.node_id
            order by a.condition_id,c.nav_strand_id,c.seq_num;
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        return 0

    

