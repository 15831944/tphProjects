# -*- coding: UTF-8 -*-
'''
Created on 2014-6-27

@author: zhangyongmu
'''
import os
import codecs
import component.component_base

SEARCH_RANGE = 0.002 #200
BUILDING_CODE = ['100','36','616','615','619','47','576','620','450','451','697','594','583','1636','635','636']
BUILDING_NUM = 6
MAX_BUILDING_DISTANCE = 150
DELETE_DISTANCE = 10
COUNTRY_RADIS = 50
FRACTION = 0.5 #LINK的中心位置

class comp_guideinfo_building_jdb(component.component_base.comp_base):
    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Guideinfo_building')

    def _DoCreateFunction(self):
        pass

    def _Do(self):
#         self._import_logmark_priority_tbl()
#         self._change_coordinates_4326()
#         self._import_buildingcode_mapping()
#         self._make_need_building_node()
        self._find_building_node()
        return 0
    
    def _import_logmark_priority_tbl(self):
        self.CreateTable2('t_logmark_priority_tbl')
        #插入数据
        sqlcmd = '''
            INSERT INTO t_logmark_priority_tbl(
            attr_code, priority)
            VALUES (%s, %s);
        '''
        self.pg.execute(sqlcmd, ('100',1))
        return
    
    def _find_building_node(self):
        self.CreateTable2('building_structure')
#         self.CreateIndex2('building_structure_the_geom_idx')
        self.CreateFunction2('hex_to_int')
        self.CreateFunction2('delete_nearly_logmark')
        self.CreateFunction2('get_two_point_angle')
        self.CreateFunction2('delete_logmark_by_node_distance')
#         self.CreateFunction2('make_find_node_mapping_building')
        self.CreateFunction2('make_find_node_mapping_building_no_link')
        self.pg.callproc('make_find_node_mapping_building_no_link', (BUILDING_NUM,MAX_BUILDING_DISTANCE,DELETE_DISTANCE,COUNTRY_RADIS,FRACTION))
        self.pg.commit2()
        return
    
    def _make_need_building_node(self):
        self.CreateIndex2('link_tbl_road_type_idx')
        self.CreateTable2('temp_building_mapping_node')
        sqlcmd = '''
            INSERT INTO temp_building_mapping_node(node_id, the_geom, the_geom_circ_buffer, country_road_f)
            select a.node_id,b.the_geom,ST_Buffer(b.the_geom,%s),a.country_road_f
            from(
                select node_id,sum(flag) as country_road_f
                from(
                    select node_id,sum(temp_flag) as flag
                    from(
                        select gid,link_id,unnest(s_e_node) as node_id,case when road_type=2 then 1 else 0 end as temp_flag
                        from(
                            SELECT gid, link_id, array[s_node, e_node] as s_e_node,road_type
                            FROM link_tbl
                            where road_type > 1 and road_type < 7
                            order by gid
                        ) as a
                    ) as b
                    group by node_id having count(*) >= 3
            
                    union
            
                    select node_id,1 as flag
                    from(
                        select a.link_id,a.s_node as node_id, array_agg(distinct b.road_type) as road_types
                        from(
                            SELECT gid, link_id, s_node
                            FROM link_tbl
                            where road_type in (2,8,9)
                        ) as a
                        left join link_tbl as b
                        on a.s_node = b.s_node or a.s_node = b.e_node
                        group by a.link_id,a.s_node
                    ) as a
                    where 2 = any(road_types) and (8 = any(road_types) or 9 = any(road_types))
            
                    union
            
                    select node_id,1 as flag
                    from(
                        select a.link_id,a.e_node as node_id, array_agg(distinct b.road_type) as road_types
                        from(
                            SELECT gid, link_id, e_node
                            FROM link_tbl
                            where road_type in (2,8,9)
                        ) as a
                        left join link_tbl as b
                        on a.e_node = b.s_node or a.e_node = b.e_node
                        group by a.link_id,a.e_node
                    ) as a
                    where 2 = any(road_types) and (8 = any(road_types) or 9 = any(road_types))
                ) as a
                group by node_id
            ) as a
            left join node_tbl as b
            on a.node_id = b.node_id;
        '''
        self.pg.execute2(sqlcmd, (SEARCH_RANGE,))
        self.pg.commit2()
        self.CreateIndex2('temp_building_mapping_node_the_geom_circ_buffer_idx')
        return
    
    def _change_coordinates_4326(self):
        self.CreateTable2('t_logomark_4326')
        self.pg.execute2('''
            INSERT INTO t_logomark_4326(usml_genre_key_s, usml_data_grouping_code, 
                                usml_store_code, uint_sequence, geo_geom, 
                                utny_poi_sequence, attr_code, the_geom, priority)
            select usml_genre_key_s, usml_data_grouping_code, 
                usml_store_code, uint_sequence,geo_geom, utny_poi_sequence,a.attr_code,
                st_transform(st_setsrid(ST_MakePoint(lon::bigint / 256.0 / 3600, lat::bigint / 256.0 / 3600), 4301), 4326), 
                b.priority
            from(
                select *
                from(
                SELECT usml_genre_key_s, usml_data_grouping_code, usml_store_code, uint_sequence, 
                       geo_geom, utny_poi_sequence,(regexp_split_to_array(geo_geom, E'\\,'))[3] as attr_code,
                       (regexp_split_to_array(geo_geom, E'\\,'))[1] as lon,(regexp_split_to_array(geo_geom, E'\\,'))[2] as lat
                FROM t_logomark
                ) as a
                where a.attr_code=any(%s)
            ) as a
            left join t_logmark_priority_tbl as b
            on a.attr_code = b.attr_code
        ''',(BUILDING_CODE,))
        self.pg.commit2()
        self.CreateIndex2('t_logomark_4326_the_geom_idx')
        return
    
    def _import_buildingcode_mapping(self):
        self.CreateTable2('org_logmark_code_mapping')
        code_mapping_file_path = common.common_func.GetPath('code_mapping_file')
        if os.path.exists(code_mapping_file_path):
#             objFile = open(code_mapping_file_path)
            objFile = codecs.open(code_mapping_file_path,'r','shift-jis')
            listline = objFile.readlines()
            objFile.close()
            for line in listline:
                line = line.strip()
                if line:
                    listrow = line.split('''\t''')
                    self.pg.insert('org_logmark_code_mapping',
                                        (
                                        'attr_code',
                                        'kiwicode',
                                        'name_str'
                                          ),
                                          (
                                           listrow[0],
                                           listrow[1],
                                           listrow[2]
                                          )
                                          )
            self.pg.commit2()
        return
    

