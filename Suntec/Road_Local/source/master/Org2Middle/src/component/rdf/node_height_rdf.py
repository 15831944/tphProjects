# -*- coding: cp936 -*-

import component.component_base

class comp_node_height_rdf(component.component_base.comp_base):
    '''
    classdocs
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Node_Height')
        
    def _DoCreateTable(self):
        
        self.CreateTable2('node_height_tbl')        
        return 0
    
    def _DoCreateIndex(self):
        
        self.CreateIndex2('node_height_tbl_node_id_idx')
        return 0
    
    def _DoCreateFunction(self):

        return 0
    
    def _Do(self):
        
        if self.isCountry('uc') or self.isCountry('usa'):       
            sqlcmd = """
                insert into node_height_tbl(node_id, height)
                (
                    select b.ref_node_id as node_id, 
                        (round(ref_node_zcoord/10.0) - 20000) as height
                    from rdf_link_height a
                    left join temp_rdf_nav_link b
                    on a.link_id = b.link_id
                    union
                    select b.nonref_node_id as node_id, 
                        (round(nref_node_zcoord/10.0) - 20000) as height
                    from rdf_link_height a
                    left join temp_rdf_nav_link b
                    on a.link_id = b.link_id
                ); 
            """
            
            self.pg.execute2(sqlcmd)
            self.pg.commit2()

        return 0
    

    