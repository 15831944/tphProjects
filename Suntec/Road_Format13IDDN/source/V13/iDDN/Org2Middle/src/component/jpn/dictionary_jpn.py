# -*- coding: UTF8 -*-
'''
Created on 2012-3-29

@author: hongchenzai
'''
import component

class comp_dictionary_jpn(component.dictionary.comp_dictionary):
    '''字典(日本)
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.dictionary.comp_dictionary.__init__(self)
        
    def _DoCreateTable(self):
        component.dictionary.comp_dictionary._DoCreateTable(self)
        
        # 道路番号（对相同的道路番号进行GroupBy）
        self.CreateTable2('temp_link_number_group')
        self.CreateTable2('temp_link_number_groupid_nameid')
        self.CreateTable2('temp_link_no')
        
        # 这个表主要在road_illust表的基础上增加了node_id
        self.CreateTable2('temp_road_illust')
        
        # node表中的node名，加上Illust表中的node名
        self.CreateTable2('temp_node_name_and_illust_name')
        self.CreateTable2('temp_node_name_and_illust_name_groupby')
        self.CreateTable2('temp_node_name_and_illust_name_groupid_nameid')
        
    def _DoCreateFunction(self):
        self.CreateFunction2('rdb_add_one_name')
        
        self.CreateFunction2('rdb_insert_name_dictionary')
        
        self.CreateFunction2('rdb_insert_poi_name_dictionary')
        
        return 0        
        
    def _MakeLinkName(self):
        "道路名称字典"
        self.log.info('Make Link Name Dictionary.')
        sqlcmd = """
            SELECT rdb_insert_name_dictionary('road_roadcode', 
                                            'roadcode', 
                                            'roadname', 
                                            'roadyomi', 
                                            'temp_link_name',
                                            'roadname is not null');
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return 0
    
    def _MakeRoadNumber(self):
        " 道路番号字典"
        self.log.info('Make Road Number Dictionary.')
        
        if self.CreateFunction2('rdb_cnv_shield_id') == -1:
            return -1
        
        # 对相同的roadno进行GroupBy,并存入temp_link_number_group
        sqlcmd = """
                insert into temp_link_number_group(linkid_array, shield_id_num)
                (
                select array_agg(objectid), (shield_id::varchar || E'\t' || roadno::varchar) as shield_id_num
                  from (
                    SELECT objectid, roadno, rdb_cnv_shield_id(roadcls_c) as shield_id
                      FROM road_rlk
                      where roadno <> 0
                  ) as a
                  group by (shield_id::varchar || E'\t' || roadno::varchar)
                );
            """
            
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        sqlcmd = """
            SELECT rdb_insert_name_dictionary('temp_link_number_group', 
                                            'gid', 
                                            'shield_id_num', 
                                             null, 
                                            'temp_link_number_groupid_nameid',
                                            NULL);
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        sqlcmd = """
                insert into temp_link_no(link_id, name_id)
                (
                select linkid_array[seq_num] as link_id, name_id
                  from (
                    select linkid_array, generate_series(1,array_upper(linkid_array, 1)) as seq_num, name_id
                      from (
                        select  linkid_array, name_id
                          from temp_link_number_group as a
                          left join temp_link_number_groupid_nameid as b
                          on a.gid = b.gid
                      ) as c
                  ) as d
                );
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return 0
    
    def _MakeCrossName(self):
        "交叉点名称字典"
        self.log.info('Make Cross Name Dictionary.')
        
        # 先把node表的名称插到临时表，temp_node_name_and_illust_name
        sqlcmd = """
                INSERT INTO temp_node_name_and_illust_name (
                  SELECT objectid as node_id, name_kanji, name_yomi
                    FROM road_rnd
                    where name_kanji is not null and name_kanji <> ''
                );
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
   
        # 对同名的node点进行合并, 并插入表temp_node_name_and_illust_name_groupby
        sqlcmd = """
                INSERT INTO temp_node_name_and_illust_name_groupby(node_ids, name_kanji, name_yomi)
                (
                    SELECT array_agg(node_id), name_kanji, name_yomi
                      FROM temp_node_name_and_illust_name
                      group by name_kanji, name_yomi
                );
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
                
        # 插入字典
        sqlcmd = """
            SELECT rdb_insert_name_dictionary('temp_node_name_and_illust_name_groupby', 
                                            'groupid', 
                                            'name_kanji', 
                                            'name_yomi', 
                                            'temp_node_name_and_illust_name_groupid_nameid',
                                            null);
        """
        
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        # 将node id 和 name id 的关联信息插入temp_node_name
        sqlcmd = """
                INSERT INTO temp_node_name (
                    SELECT regexp_split_to_table(array_to_string(node_ids,','), E'\\,+')::integer, b.name_id
                      FROM temp_node_name_and_illust_name_groupby as a
                      LEFT JOIN temp_node_name_and_illust_name_groupid_nameid as b
                      ON a.groupid = b.groupid
                );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        # 收费站名称
        self._MakeTollNodeName()
        return 0
    
    def _MakeTowardName(self):
        "方面名称和方面看板文字列字典"
        
        self.log.info('Make Toward Name and SignPost Name Dictionary.')
        
        # 创建函数 mid_isNotAllDigit：判断字符串，是不是有非数字字符
        sqlcmd = """
                CREATE OR REPLACE FUNCTION mid_isNotAllDigit(_name character varying)
                  RETURNS boolean
                  LANGUAGE plpgsql 
                  AS $$
                begin
                    return (translate(_name, '１２３４５６７８９０', '1234567890') ~ E'\\\D');
                END;
                $$;
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.CreateFunction2('rdb_insert_name_dictionary_towardname')
        self.pg.callproc('rdb_insert_name_dictionary_towardname')
        self.pg.commit2()
        
        return 0
    
#    def _MakeSignPostName(self):
#        "方面看板名称字典"
#        self.log.info('Make SignPost Name Dictionary.')
#        sqlcmd = """
#            SELECT rdb_insert_name_dictionary('road_dir', 
#                                            'gid', 
#                                            'name_kanji', 
#                                            'name_yomi', 
#                                            'temp_signpost_name',
#                                            'name_kanji is not null and guideclass < 12');
#        """
#        self.pg.execute2(sqlcmd)
#        self.pg.commit2()
#        return 0
    
    def _MakePOIName(self):
        "POI名称字典"
        self.log.info('Make POI Name Dictionary.')
        
        sqlcmd = """
                 select rdb_insert_poi_name_dictionary('%s');
                 """
        
        maptypes = ['topmap', 'middlemap', 'basemap', 'citymap']
        for maptype in maptypes:
            self.pg.execute2(sqlcmd % maptype)
            self.pg.commit2()
            
        return 0
    
    def _MakeTollNodeName(self):
        "收费站名称"
        
        self.log.info('Make Toll Node Name Dictionary.')
        # 对收费站相同名称进行合并
        self.CreateTable2('mid_temp_toll_node_name_group')
        self.CreateTable2('mid_temp_toll_node_name')
        
        sqlcmd = """
            insert into mid_temp_toll_node_name_group(node_array, name, name_yomi) 
            (
            select array_agg(node_id) as node_array, name, name_yomi
              from (
                select distinct node_id, name, name_yomi
                   from road_tollnode
                   where node_id is not null
              ) as a
              group by name, name_yomi
            );
        """
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else :
            self.pg.commit2()
            
        # 做成字典
        sqlcmd = """
            SELECT rdb_insert_name_dictionary('mid_temp_toll_node_name_group', 
                                            'gid', 
                                            'name', 
                                            'name_yomi', 
                                            'mid_temp_toll_node_name',
                                            NULL);
        """
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else :
            self.pg.commit2()
        
        # 把收费站名称，记录到交叉点名称中去。
        sqlcmd = """
            INSERT INTO temp_node_name (
                select node_array[seq_num], new_name_id
                   from (
                    select a.gid, node_array, new_name_id, 
                           generate_series(1,array_upper(node_array,1)) as seq_num
                      from mid_temp_toll_node_name_group as a
                      left join mid_temp_toll_node_name as b
                      on a.gid = b.gid
                  ) as c
            );
        """
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else :
            self.pg.commit2()
        return 0