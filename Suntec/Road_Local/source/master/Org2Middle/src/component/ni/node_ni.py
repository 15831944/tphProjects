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
        self._makeAdjNodeMapping()
        self._makeNodeZLevel()
        self._makeTrafficLight()
        self._makeNodeTable()
        return 0
    
    def _makeAdjNodeMapping(self):
        self.log.info('make adj node mapping...')
        self.CreateIndex2('org_n_id_idx')
        
        if self.pg.IsExistTable('org_adj'):
            sqlcmd = """
                    update org_n as a set adjoin_nid = b.nodeid2, kind = kind || '|1f00'
                    from org_adj as b
                    where a.id = b.nodeid1;
                    
                    update org_n as a set adjoin_nid = b.nodeid1, kind = kind || '|1f00'
                    from org_adj as b
                    where a.id = b.nodeid2;
                    
                    analyze org_n;
                    """
            self.pg.execute2(sqlcmd)
            self.pg.commit2()
        
        self.CreateTable2('temp_node_mapping')
        self.CreateIndex2('temp_node_mapping_old_node_id_idx')
        self.CreateIndex2('temp_node_mapping_new_node_id_idx')
               
        return 0
    
    def _makeNodeTable(self):
        self.log.info('make node_tbl...')
        sqlcmd='''
                insert into node_Tbl
                (node_id,kind,light_flag,stopsign_flag,toll_flag,bifurcation_flag,mainnodeid,node_lid,
                node_name,z_level,feature_string,feature_key,the_geom)
               
                select a.id::bigint, a.kind, 
                    case when trflight.id is not null then 1 else 0 end as light_flag,
                    case when d.nodeid is not null then 1 else 0 end as stopsign_flag,
                    case when b.nodeid is not null then 1 else 0 end as toll_flag,
                    case when c.nodeid is not null then 1 else 0 end as bifurcation_flag ,
                    a.mainnodeid::bigint as mainnodeid,
                    case when g.id is null then a.node_lid else array_to_string(string_to_array(a.node_lid,'|')||string_to_array(g.node_lid,'|'),'|') end,
                    null as node_name,
                    case when o.node_id is null then 0 else o.z_level end as z_level,
                    a.id::varchar as feature_string, 
                    md5(a.id::varchar) as feature_key, 
                    a.the_geom
                from org_n a 
                left join temp_node_trafficlight trflight
                on a.id = trflight.id
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
                    on a.id = o.node_id
                where e.old_node_id is null
               '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        return 0
    
    def _makeNodeZLevel(self):
        
        self.log.info('Now it is updating temp_node_z_level_tbl...')
        
        self.CreateTable2('temp_node_z_level_tbl')
        
        sqlcmd = '''
                insert into temp_node_z_level_tbl(node_id, z_level)
                select 
                    (case when seq_nm::bigint = 0 then snodeid else enodeid end) as node_id, 
                    z::smallint as z_level
                from (
                    select e.*
                    from (
                        select *
                        from (
                            select array_agg(a.gid) as gid_array
                            from org_z_level a
                            left join org_r b
                            on a.mapid = b.mapid and a.id = b.id
                            where 
                                (b.gid is not null) and 
                                (linktype = '1') and 
                                ((seq_nm::smallint = 0) or (seq_nm::smallint = (ST_NumPoints(b.the_geom) - 1)))
                            group by level_id::bigint
                            order by level_id::bigint
                            ) c
                            where array_upper(gid_array, 1) > 1
                        ) d
                        left join org_z_level e
                        on e.gid = any(gid_array)
                    ) f
                    left join org_r g
                    on f.mapid = g.mapid and f.id = g.id
                where g.gid is not null
                '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('temp_node_z_level_tbl_node_id_idx')
        
        self.log.info('updating temp_node_z_level_tbl succeeded')
        return 0
    
    def _makeTrafficLight(self):
        self.log.info('Now it is making traffic light...')
        
        sqlcmd = """
            --- suspected traffic light nodes.
            drop table if exists temp_node_trafficlight_suspect;
            create table temp_node_trafficlight_suspect as
            SELECT id,the_geom
            FROM org_n where light_flag = '1'  
            union all
            select a.id,b.the_geom 
            from (
                select mainnodeid,unnest(string_to_array(subnodeid,'|')) as id
                from (
                    select mainnodeid
                        ,case when subnodeid2 != '' then subnodeid || '|' || subnodeid2 
                            else subnodeid
                        end as subnodeid
                    from org_n
                    where light_flag = '1' and cross_flag = '3'
                ) nodes
            ) a
            left join org_n b
            on a.id::character varying = b.id;

            CREATE INDEX temp_node_trafficlight_suspect_id_idx
              ON temp_node_trafficlight_suspect
              USING btree
              (id);
            
            --- suspected traffic nodes which have at least one in-link, and this link is not an inner link.
            --- make these nodes as traffic light nodes.
            drop table if exists temp_node_trafficlight;
            create table temp_node_trafficlight as
            select distinct a.* 
            from temp_node_trafficlight_suspect a
            left join org_r b
            on (
                (a.id = b.snodeid and b.direction in ('0','1','3'))
                or 
                (a.id = b.enodeid and b.direction in ('0','1','2'))
            )
            where b.id is not null and
            (b.kind not like '%04' and b.kind not like '%04|%');

            CREATE INDEX temp_node_trafficlight_id_idx
              ON temp_node_trafficlight
              USING btree
              (id);            
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
               
        return 0    