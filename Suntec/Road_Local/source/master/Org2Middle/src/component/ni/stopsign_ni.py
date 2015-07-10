import component.component_base

class comp_stopsign_ni(component.component_base.comp_base):
    '''
    classdocs
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'stopsign')
        
    def _DoCreateTable(self):
        if self.CreateTable2('stopsign_tbl') == -1:
            return -1
       
        return 0
    
    def _DoCreateIndex(self):
        'create index.'
        self.CreateIndex2('stopsign_tbl_link_id_idx')
    
    def _DoCreateFunction(self):

        return 0
    
    def _Do(self):
        
        self.CreateIndex2('org_r_id_idx')
        self.CreateIndex2('org_n_id_idx')
        sqlcmd='''
                insert into stopsign_tbl(link_id, node_id, pos_flag,the_geom)
                select inlinkid::bigint,
                    (case when d.old_node_id is null then a.nodeid else d.new_node_id end)::bigint,
                    case when c.snodeid = a.nodeid then 2::smallint 
                        when c.enodeid = a.nodeid then 3::smallint 
                    else 0::smallint end as pos_flag
                    ,b.the_geom 
                from org_trfcsign a 
                join org_n b on a.nodeid = b.id 
                join org_r c on a.inlinkid = c.id 
                left join temp_node_mapping d on a.nodeid=d.old_node_id
                where type='40'
               '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return 0
    

        
    