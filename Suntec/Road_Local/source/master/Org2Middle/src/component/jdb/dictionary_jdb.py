# -*- coding: UTF8 -*-
'''
Created on 2012-3-29

@author: hongchenzai
'''
import component.default.dictionary

class comp_dictionary_jdb(component.default.dictionary.comp_dictionary):
    '''字典(日本)
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.default.dictionary.comp_dictionary.__init__(self)
        
    def _DoCreateTable(self):
        component.default.dictionary.comp_dictionary._DoCreateTable(self)
        
        # 道路番号（对相同的道路番号进行GroupBy）
        self.CreateTable2('temp_link_number_group')
        self.CreateTable2('temp_link_number_groupid_nameid')
        self.CreateTable2('temp_link_no')
        
        # 这个表主要在road_illust表的基础上增加了node_id
        #self.CreateTable2('temp_road_illust')
        
        # node表中的node名，加上Illust表中的node名
        self.CreateTable2('temp_node_name_and_illust_name')
        self.CreateTable2('temp_node_name_and_illust_name_groupby')
        self.CreateTable2('temp_node_name_and_illust_name_groupid_nameid')
        
    def _DoCreateFunction(self):
        self.CreateFunction2('rdb_add_one_name')
        
        self.CreateFunction2('rdb_insert_name_dictionary')
        
        #self.CreateFunction2('rdb_insert_poi_name_dictionary')
        
        return 0        
    
    def _Do(self):
        # 创建语言种别的中间表
        self._InsertLanguages()
        # 道路名称字典
        self._MakeLinkName()
        # 道路番号字典
        self._MakeRoadNumber()
        # 交叉点名称字典
        self._MakeCrossName()
        # 方面名称字典
        self._MakeTowardName()
        # 施设名称
        self._MakeFacilName()
        # 收费站名称 -- 依懒于设施名称
        self._MakeTollNodeName()
        
        return 0 
    
    def _MakeLinkName(self):
        "道路名称字典"
        self.log.info('Make Link Name Dictionary.')
        sqlcmd = """
            SELECT rdb_insert_name_dictionary('road_code_list', 
                                            'road_code', 
                                            'name_kanji', 
                                            'name_yomi', 
                                            'temp_link_name',
                                            'name_kanji is not null');
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return 0
    
    def _MakeRoadNumber(self):
        " 道路番号字典"
        self.log.info('Make Road Number Dictionary.')
        
        if self.CreateFunction2('rdb_cnv_shield_id_jdb') == -1:
            return -1
        
        # 对相同的roadno进行GroupBy,并存入temp_link_number_group
        sqlcmd = """
                insert into temp_link_number_group(linkid_array, shield_id_num)
                (
                select array_agg(objectid), 
                        (shield_id::varchar || E'\t' || road_no::varchar) as shield_id_num
                  from (
                    SELECT objectid, road_no, rdb_cnv_shield_id_jdb(roadclass_c) as shield_id
                      FROM road_link_4326
                      where road_no <> 0
                  ) as a
                  group by shield_id_num
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
                    select linkid_array, name_id,
                            generate_series(1,array_upper(linkid_array, 1)) as seq_num
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
                    FROM road_node_4326
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
        
        self.CreateFunction2('rdb_insert_name_dictionary_towardname_jdb')
        self.pg.callproc('rdb_insert_name_dictionary_towardname_jdb')
        self.pg.commit2()
        
        return 0
    
    def _MakeTollNodeName(self):
        "收费站名称 (包含普通和高速) ---- 依懒于设施名称"
        self.log.info('Make Toll Node Name Dictionary.')
        # 把收费站名称，记录到交叉点名称中去。
        # tollclass_c----1: 本線上料金所; 2: ランプ上料金所; 3: バリア
        # 比13春数据增加了27个收费站
        sqlcmd = """
            INSERT INTO temp_node_name (
                SELECT distinct node_id, name_id
                  FROM highway_node_add_nodeid as a
                  left join facil_info_point_4326 as b
                  on a.road_code = b.road_code and a.road_seq = b.road_seq
                  left join mid_temp_facil_point_name as c
                  on b.objectid = c.objectid
                  where a.tollclass_c in (1, 2, 3)
            );
            """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return 0
    
    def _MakeFacilName(self):
        '施设名称'
        self.log.info('Make Facility Name Dictionary.')

        ############################################################
        ### 第一步，相同名称合并
        ############################################################
        self.CreateTable2('mid_temp_facil_info_point_name_group')
        sqlcmd = """
            INSERT INTO mid_temp_facil_info_point_name_group(
                        objectid_lid, name_kanji, name_yomi )
            (
            SELECT array_agg(objectid) as objectid_lid,  name_kanji, name_yomi
               FROM facil_info_point_4326
               group by name_kanji, name_yomi
            );
            """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        #############################################################
        ### 第二步，名称做成字典， 并把group id 和 name id 存到下表
        #############################################################
        self.CreateTable2('mid_temp_facil_info_point_name_groupid_nameid')
        # 做成字典
        sqlcmd = """
            SELECT rdb_insert_name_dictionary('mid_temp_facil_info_point_name_group', 
                                            'group_id', 
                                            'name_kanji', 
                                            'name_yomi', 
                                            'mid_temp_facil_info_point_name_groupid_nameid',
                                            NULL);
            """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        #############################################################
        ### 第三步, 做成设施和名称id关系表(object是关联键)
        #############################################################
        self.CreateTable2('mid_temp_facil_point_name')
        sqlcmd = """
            INSERT INTO mid_temp_facil_point_name(objectid, name_id)
            (
            SELECT objectid_lid[seq_num] as objectid, name_id
              from (
                SELECT group_id, objectid_lid,
                    generate_series(1, array_upper(objectid_lid,1)) as seq_num
                  from mid_temp_facil_info_point_name_group
              ) as a
              left join mid_temp_facil_info_point_name_groupid_nameid as b
              on a.group_id = b.group_id
            );
            """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        
            
        