'''
Created on 2012-3-23

@author: sunyifeng
'''

import base

class comp_node_jpn(base.component_base.comp_base):
    '''
    classdocs
    '''


    def __init__(self):
        '''
        Constructor
        '''
        base.component_base.comp_base.__init__(self, 'Node')
        
    def _DoCreateTable(self):
        if self.CreateTable2('node_tbl') == -1:
            return -1
        
        return 0
    
    def _DoCreateFunction(self):

        return 0
    
    def _Do(self):
                
        sqlcmd = """
                insert into node_tbl (
                    node_id, kind, light_flag, toll_flag, mainnodeid, node_lid, name_id, feature_string, feature_key, the_geom )
                select d.tile_node_id, null, a.signal_f, 
                        case when e.node_id is not null or f.node_id is not null then 1 else 0 end, 
                        0, array_to_string(b.node_lid, '|'), c.name_id, 
                        (case when a.new_add then null else a.objectid::varchar end) as feature_string,
                        (case when a.new_add then null else md5(a.objectid::varchar) end) as feature_key,
                        a.the_geom
                    from temp_road_rnd as a
                    left outer join (
                        select n.objectid as objectid, array_agg(l.objectid) as node_lid
                            from temp_road_rnd as n, temp_road_rlk as l
                            where n.objectid = l.fromnodeid or n.objectid = l.tonodeid
                            group by n.objectid ) as b 
                    on a.objectid = b.objectid
                    left outer join temp_node_name as c
                    on a.objectid = c.objectid
                    left outer join middle_tile_node as d
                    on a.objectid = d.old_node_id
                    left outer join (select distinct node_id from road_tollnode where kind > 1 and (kind & 1) <> 1) as e
                    on a.objectid = e.node_id
                    left outer join (
                        select distinct node_id
                        from road_hwynode
                        where kind > 1 and (kind & 1) <> 1

                        union

                        select distinct node_id
                        from road_tollguide
                    ) as f
                    on a.objectid = f.node_id
                """
        
        self.log.info(self.ItemName + ': Now it is inserting node_tbl...')
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
            
        self.log.info(self.ItemName + ': end of inserting node_tbl...')
                  
        return 0
    
    def _DoCreateIndex(self):
        'create index.'
        self.CreateIndex2('node_tbl_node_id_idx')
        self.CreateIndex2('node_tbl_the_geom_idx')
        
        return 0
    
