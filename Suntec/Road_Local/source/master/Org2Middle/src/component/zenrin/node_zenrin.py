import component.component_base

class comp_node_zenrin(component.component_base.comp_base):
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
        self._makeNodeTable()
        return 0
    
    def _makeNodeTable(self):
        self.log.info('make node_tbl...')
        
        self.CreateIndex2('org_node_connect_meshcode_nodeid_idx')
        
        sqlcmd='''
            insert into node_Tbl
            (node_id,light_flag,stopsign_flag,toll_flag,bifurcation_flag,node_lid,z_level,
            the_geom, feature_string, feature_key, org_boundary_flag)
            select a.node_id,
                    case when b.meshcode is null then 0 else 1 end as light_flag,
                    0 as stopsign_flag,
                    case when c.meshcode is null then 0 else 1 end as toll_flag,
                    case when d.meshcode is null then 0 else 1 end as bifurcation_flag,
                    array_to_string(e.linkid_arr,'|') as node_lid,
                    0 as z_level,a.the_geom_4326 ,
                    array_to_string(ARRAY[a.meshcode, a.nodeno::varchar],',') as feature_string,
                    md5(array_to_string(ARRAY[a.meshcode, a.nodeno::varchar],',')) as feature_key,
                    case when crosskind=3 then 1 else 0 end as org_boundary_flag
            from
            (
                select distinct a.meshcode,a.nodeno,a.crosskind,b.node_id,a.the_geom_4326
                from org_node a
                join temp_node_mapping b
                on a.meshcode=b.meshcode and a.nodeno=b.nodeno
                left join org_node_connect c on a.meshcode::integer=c.meshcode_self 
                            and a.nodeno=c.nodeid_self and c.mesh_rel in (2,4)
                where c.nodeid_self is null
            ) a
            left join org_cross_symbol b on a.meshcode=b.meshcode and a.nodeno=b.nodeno and b.symcode='C09999'
            left join org_cross_symbol c on a.meshcode=c.meshcode and a.nodeno=c.nodeno and c.symcode='C00284'
            left join (select distinct meshcode,tnodeno from org_bifurcation) d on a.meshcode=d.meshcode and a.nodeno=d.tnodeno
            join 
            (
                select b.node_id,array_agg(c.link_id) as linkid_arr from org_node a join temp_node_mapping b using(meshcode,nodeno)
                join temp_link_mapping c on a.meshcode=c.meshcode and a.linkno=c.linkno
                group by b.node_id
            ) e
            on a.node_id=e.node_id
               '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        return 0