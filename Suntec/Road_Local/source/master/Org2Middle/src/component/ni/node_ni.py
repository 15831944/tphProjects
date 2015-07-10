import component.component_base

class comp_node_ni(component.component_base.comp_base):
    '''
    classdocs
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Node')
        
    def _DoCreateTable(self):
        if self.CreateTable2('node_tbl') == -1:
            return -1
       
        return 0
    
    def _DoCreateIndex(self):
        'create index.'
        self.CreateIndex2('node_tbl_node_id_idx')
        self.CreateIndex2('node_tbl_the_geom_idx')
    
    def _DoCreateFunction(self):

        return 0
    
    def _Do(self):
        
        sqlcmd='''
                insert into node_Tbl
                (node_id,kind,light_flag,stopsign_flag,toll_flag,bifurcation_flag,mainnodeid,node_lid,node_name,the_geom)
               
                select id::bigint, kind, light_flag::smallint,
                    case when d.nodeid is not null then 1 else 0 end as stopsign_flag,
                    case when b.nodeid is not null then 1 else 0 end as toll_flag,
                    case when c.nodeid is not null then 1 else 0 end as bifurcation_flag ,
                    mainnodeid::bigint as mainnodeid,
                    node_lid,
                    null as node_name,
                    the_geom
                from org_n a 
                left join
                (
                    select distinct id as nodeid from org_c
                    where condtype='3' 
                    union
                    select distinct b.nodeid from org_cond a
                    left join org_cnl b
                    on a.condid = b.condid
                    where condtype='3'
                ) b on a.id = b.nodeid 
                left join
                (
                    select distinct id as nodeid from org_c
                    where condtype='5' 
                    union
                    select distinct b.nodeid from org_cond a
                    left join org_cnl b
                    on a.condid = b.condid
                    where condtype='5'
                ) c on a.id = c.nodeid 
                left join
                (
                    select distinct(nodeid) from org_trfcsign 
                    where type = '40' and nodeid<>''
                ) d on a.id = d.nodeid 
               '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return 0
    

        
    