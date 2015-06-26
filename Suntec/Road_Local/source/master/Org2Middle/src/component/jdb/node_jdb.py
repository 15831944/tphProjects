'''
Created on 2012-3-23

@author: sunyifeng
'''

import component.component_base

class comp_node_jdb(component.component_base.comp_base):
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
    
    def _DoCreateFunction(self):

        return 0
    
    def _Do(self):
                
        sqlcmd = """
            insert into node_tbl (
                node_id, kind, light_flag, toll_flag, mainnodeid, node_lid, node_name, feature_string, feature_key, the_geom 
            )
            select d.tile_node_id, null, a.signal_f, 
                    case when e.objectid_node is not null then 1 else 0 end, 
                    0, array_to_string(b.node_lid, '|'), 
                    case when a.name_kanji is not null or a.name_yomi is not null then
                            mid_get_json_string_for_japan_name(a.name_kanji,a.name_yomi)
                        else null
                    end as node_name,
                    (case when a.new_add then null else a.objectid::varchar end) as feature_string,
                    (case when a.new_add then null else md5(a.objectid::varchar) end) as feature_key,
                    a.the_geom
            from temp_road_node as a
            left outer join (
                select n.objectid as objectid, array_agg(l.objectid) as node_lid
                    from temp_road_node as n, temp_road_link as l
                    where n.objectid = l.from_node_id or n.objectid = l.to_node_id
                    group by n.objectid 
            ) as b 
            on a.objectid = b.objectid
            left outer join middle_tile_node as d
            on a.objectid = d.old_node_id
            left outer join (
                select distinct b.objectid as objectid_node
                from highway_node_4326 as a
                inner join road_node_4326 as b
                on ST_GeoHash(ST_Centroid(a.the_geom), 10) = ST_GeoHash(ST_Centroid(b.the_geom), 10)
                left join facil_info_point_4326 as c
                on a.road_code = c.road_code and a.road_seq = c.road_seq
                where ST_OrderingEquals(a.the_geom, b.the_geom)
                and (tollclass_c in (1,2,3) and tollfunc_c > 0)
                and (
                    (dummyfacil_f = 0 and dummytoll_f = 0)
                    or ((dummyfacil_f = 0 or dummytoll_f = 0)  and facilclass_c = 6)
                )
            ) as e
            on a.objectid = e.objectid_node;
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
    
