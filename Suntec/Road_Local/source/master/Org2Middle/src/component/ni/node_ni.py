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
        
        self.CreateIndex2('org_n_id_idx')
        self.CreateTable2('temp_node_mapping')
        self.CreateIndex2('temp_node_mapping_old_node_id_idx')
        self.CreateIndex2('temp_node_mapping_new_node_id_idx')
        self._update_temp_node_z_level_tbl()
        
        sqlcmd='''
                insert into node_Tbl
                (node_id,kind,light_flag,stopsign_flag,toll_flag,bifurcation_flag,mainnodeid,node_lid,node_name,z_level,the_geom)
               
                select a.id::bigint, a.kind, a.light_flag::smallint,
                    case when d.nodeid is not null then 1 else 0 end as stopsign_flag,
                    case when b.nodeid is not null then 1 else 0 end as toll_flag,
                    case when c.nodeid is not null then 1 else 0 end as bifurcation_flag ,
                    a.mainnodeid::bigint as mainnodeid,
                    case when g.id is null then a.node_lid else array_to_string(string_to_array(a.node_lid,'|')||string_to_array(g.node_lid,'|'),'|') end,
                    null as node_name,
                    case when o.node_id is null then 0 else o.z_level end as z_level,
                    a.the_geom
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
                left join temp_node_mapping e
                on a.id=e.old_node_id 
                left join temp_node_mapping f
                on a.id=f.new_node_id
                left join org_n g
                on f.old_node_id=g.id
                left join temp_node_z_level_tbl o
                    on a.id::bigint = o.node_id
                where e.old_node_id is null
               '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        return 0
    
    def _update_temp_node_z_level_tbl(self):
        
        self.log.info('Now it is updating temp_node_z_level_tbl...')
        
        self.CreateTable2('temp_node_z_level_tbl')
        
        sqlcmd = '''
                insert into temp_node_z_level_tbl(node_id, z_level)
                select (
                    case
                        when seq_nm = 0 then snodeid
                        when seq_nm = link_pc - 1 then enodeid
                    end
                    ) as node_id, z as z_level
                from (
                    select seq_nm::smallint, z::smallint, snodeid::bigint, enodeid::bigint, ST_NumPoints(m.the_geom) as link_pc
                    from (
                        select k.*
                        from (
                            select gid_array
                            from (
                                select array_agg(gid) as gid_array, array_agg(id) as id_array
                                from (
                                    select *
                                    from org_z_level
                                    where linktype = '1'
                                ) h
                                group by level_id::bigint
                                order by level_id::bigint
                            ) i
                            where array_upper(gid_array, 1) > 1
                        ) j
                        left join org_z_level k
                            on k.gid = ANY(gid_array)
                    ) l
                    left join org_r m
                        on l.mapid = m.mapid and  l.id = m.id
                    where m.gid is not null
                ) n
                where seq_nm = 0 or seq_nm = link_pc - 1
        '''
        
        
        self.log.info('updating temp_node_z_level_tbl succeeded')
        return 0