# -*- coding: UTF8 -*-
'''
Created on 2012-6-14

@author: hongchenzai
'''

import component

class comp_dictionary_axf(component.dictionary.comp_dictionary):
    '''字典(高德):字典包含的道路名称、道路名称别名、道路番号、交叉点名、方面名称、出入口番号。但不包含交叉点别名。
       --对道路名称、道路名称别名、方面名称进行合并
       --对出入口番号进行合并
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.dictionary.comp_dictionary.__init__(self)   
         
    def _DoCreateTable(self):
        # 字典表
        self.CreateTable2('name_dictionary_tbl')
        self.CreateTable2('temp_language_tbl')
        # node点名称
        self.CreateTable2('temp_org_node_name')
        self.CreateTable2('temp_node_name')
        
        self.CreateTable2('temp_road_number_old')
        self.CreateTable2('temp_road_number')
        # SignPost
        self.CreateTable2('temp_node_signpost_dict')
        self.CreateTable2('temp_cross_signpost_dict')
        # 出口
        self.CreateTable2('temp_org_signpost_exit_no')
        self.CreateTable2('temp_org_signpost_exit_no_split')
        self.CreateTable2('temp_node_signpost_exitno_dict')
        self.CreateTable2('temp_cross_signpost_exitno_dict')
        # 入口
        self.CreateTable2('temp_org_signpost_entr_no')
        self.CreateTable2('temp_node_signpost_entrno_dict')
        self.CreateTable2('temp_cross_signpost_entrno_dict')
        
        
        self.CreateTable2('temp_org_rass_name')
        self.CreateTable2('temp_rass_name')
        self.CreateTable2('temp_org_road_alias_name')
        self.CreateTable2('temp_org_road_alias_name')
        
        return 0
    
    def _DoCreateFunction(self):
        self.CreateTranslateFunc()
        self.CreateFunction2('mid_add_one_name')
        # road number
        self.CreateFunction2('mid_make_road_number_dictionary')
        # Node
        self.CreateFunction2('mid_make_node_name_dictionary')
        
        self.CreateFunction2('mid_make_rass_name_dictionary')
        self.CreateFunction2('mid_make_org_road_alias_name')
        self.CreateFunction2('mid_split_org_signpost_exit_no')
        self.CreateFunction2('mid_make_signpost_entrno_dictionary')
        self.CreateFunction2('mid_make_signpost_exitno_dictionary')
        
        return 0
    
    def _Do(self):
        # 给原始表添加索引
        self.CreateIndex2('org_roadnode_node_idx')
        self.CreateIndex2('org_roadcross_node_idx')
        self.CreateIndex2('org_roadsegment_road_idx')
        self.CreateIndex2('org_roadnodesignpost_sp_meshid_idx')
        self.CreateIndex2('org_roadcrosssignpost_sp_meshid_idx') 
        
        # 创建语言种别的中间表
        self._InsertLanguages()
        
        self._MakeOrgName()
        
        # road number
        self._MakeRoadNumber()
        # node name
        self._MakeCrossName()
        # sign post
        self._MakeSignPostName()
        self.__MakeSignPostEnterNO()
        self.__MakeSignPostExitNO()
        
        # POI
        #self._MakePOIName()
        
        # HighWay
        self._MakeHighWayInterChangeName()
        self._MakeHighWaySAPAName()
        return 0
            
    def _MakeRoadNumber(self):
        "Road Number"
        # 一条道路多个番号(G10|G20), 折成多条记录(G10, G20)
        self.log.info('Making Road Number Dictionary...')
        sqlcmd = """
                INSERT INTO temp_road_number_old (road, meshid, route_no, road_class, seq_nm)
                (
                    SELECT a.road, a.meshid,
                           regexp_split_to_table(route_no, E'\\｜+') as new_route_no, 
                           road_class,
                           generate_series(1, array_upper(regexp_split_to_array(route_no, E'\\｜+'), 1)) as seq_num
                      from org_roadsegment as a
                      where route_no is not null
                );
        """
        
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateFunction2('mid_get_shield_id')
        # 
        sqlcmd = """
            SELECT mid_make_road_number_dictionary();
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        return 0
    
    def _MakeCrossName(self):
        "Node Name."
        self.log.info('Making Node Name Dictionary...')
        # 把Node Name和Cross Name插入表temp_org_node_name
        sqlcmd = """
INSERT INTO temp_org_node_name(
     new_node, node, name_chn, name_trd, name_py, name_eng, name_ctn, node_type, meshid)
(
-- cross name
SELECT new_node, c.node, name_chn, name_trd, name_py, name_eng, name_ctn, 2, c.meshid
  from (
    SELECT b.node, a.name_chn, a.name_trd, a.name_py, a.name_eng, a.name_ctn, a.meshid
      from (
        SELECT node as cross_id, name_chn, name_trd, name_py, 
            name_eng, name_ctn, meshid
          FROM org_roadcross
          where name_chn is not null
      ) AS a
      left join org_roadnode as b
      ON a.cross_id = b.comp_node and a.meshid = b.meshid
  ) as c
  LEFT JOIN temp_node_mapping as d
  ON c.node = d.node and c.meshid = d.meshid
                
union 

-- node name
SELECT new_node, a.node, name_chn, name_trd, name_py, name_eng, name_ctn, 1, a.meshid
  FROM (        
    select node, name_chn, name_trd, name_py, name_eng, name_ctn, meshid, comp_node
      from org_roadnode 
      where name_chn is not null 
  ) AS a
  LEFT JOIN (
    SELECT node AS comp_node, meshid
      from org_roadcross
      where name_chn is not null) AS b
  ON a.comp_node = b.comp_node and a.meshid = b.meshid
  LEFT JOIN temp_node_mapping as c
  ON a.node = c.node and a.meshid = c.meshid
  where b.comp_node is null
);
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        # 会把Cross上的名字，关联到node点上
        # 当org_roadcross上和org_roadnode同是存在名称是，只选择Cross上的名字
        sqlcmd = """
            SELECT mid_make_node_name_dictionary();
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        return 0
    
    def _MakeSignPostName(self):
        self.log.info('Making SignPost Dictionary...')
        # Node SignPost
        sqlcmd = """
                insert into temp_node_signpost_dict
                (
                SELECT  b.id, meshid, new_name_id
                  FROM temp_rass_name as a
                  left join temp_org_rass_name as b
                  on a.gid = b.gid
                  where name_type = 3
                );
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('temp_node_signpost_dict_sp_meshid_idx')
        
        # Cross Sign Post
        sqlcmd = """
                insert into temp_cross_signpost_dict
                (
                SELECT  b.id, meshid, new_name_id
                  FROM temp_rass_name as a
                  left join temp_org_rass_name as b
                  on a.gid = b.gid
                  where name_type = 4
                );
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()    
        self.CreateIndex2('temp_cross_signpost_dict_sp_meshid_idx')
        return 0
    
    
    def _MakeOrgName(self):
        """include road name, road alias name, node signpost, cross signpost; 
           but exclude road number and cross name
        """
        
        self.log.info('Making road name/alias name/SignPost Dictionary...')
        
        # road name
        sqlcmd = """
                INSERT INTO temp_org_rass_name(
                            id, meshid, name_chn, name_trd, name_py, name_eng, name_ctn, seq_nm, name_type)
                (
                SELECT road, meshid, name_chn, name_trd, name_py, name_eng, name_ctn, 1, 1
                  FROM org_roadsegment
                  where name_chn is not null
                );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        # road alias name
        self._MakeRoadAliasName()
        
        # Node SignPost
        sqlcmd = """
                INSERT INTO temp_org_rass_name(
                            id, meshid, name_chn, name_trd, name_py, name_eng, name_ctn, name_type)
                (
                SELECT sp, meshid, name_chn, name_trd, name_py, name_eng, name_ctn, 3
                  FROM org_roadnodesignpost
                  where name_chn is not null
                );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        # Cross SignPost
        sqlcmd = """
                INSERT INTO temp_org_rass_name(
                            id, meshid, name_chn, name_trd, name_py, name_eng, name_ctn, name_type)
                (
                SELECT sp, meshid, name_chn, name_trd, name_py, name_eng, name_ctn, 4
                  FROM org_roadcrosssignpost
                  where name_chn is not null
                );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        sqlcmd = """SELECT mid_make_rass_name_dictionary();"""
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.CreateIndex2('temp_org_rass_name_meshid_id_idx')
        self.CreateIndex2('temp_org_rass_name_name_type_idx')
        return 0
    
    def _MakeRoadAliasName(self):
        self.log.info('Making RoadAliasName Dictionary...')
        sqlcmd = """
                INSERT INTO temp_org_road_alias_name(
                            road, meshid, alias_chn, alias_trd, alias_py, alias_eng, alias_ctn)
                (
                SELECT  road, meshid, 
                        replace(alias_chn, '｜','|') as alias_chn, 
                        replace(alias_trd, '｜','|') as alias_trd,
                        alias_py, alias_eng, alias_ctn
                  FROM org_roadsegment
                  where alias_chn is not null
                );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        sqlcmd = """SELECT mid_make_org_road_alias_name();"""
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        return 0
    
    def __MakeSignPostEnterNO(self):
        self.log.info('Making SignPost Enter NO Dictionary...')
        # enter no
        sqlcmd = """
                INSERT INTO temp_org_signpost_entr_no(sp, meshid, entr_no, entr_type)
                (
                SELECT sp, meshid, entr_no, 3
                  FROM org_roadnodesignpost
                  where entr_no is not null
                
                union
                
                SELECT sp, meshid, entr_no, 4
                  FROM org_roadcrosssignpost
                  where entr_no is not null
                ); 
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        sqlcmd = "SELECT mid_make_signpost_entrno_dictionary();"
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        return 0
        
    def __MakeSignPostExitNO(self):
        self.log.info('Making SignPost Exit NO Dictionary...')
        # exit no(node)
        sqlcmd = """
                insert into temp_org_signpost_exit_no(sp, meshid, exitno_chn, exitno_trd, exitno_py, exitno_eng, exitno_ctn, exit_type)
                (
                SELECT  sp, meshid,
                    replace(exitno_chn, '｜','|') as alias_chn,
                    replace(exitno_trd, '｜','|') as exitno_trd,
                    exitno_py,
                    exitno_eng,
                    exitno_ctn,
                    3
                  FROM org_roadnodesignpost
                  where exitno_chn is not null
                );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        # exit no(cross)
        sqlcmd = """
                insert into temp_org_signpost_exit_no(sp, meshid, exitno_chn, exitno_trd, exitno_py, exitno_eng, exitno_ctn, exit_type)
                (
                SELECT  sp, meshid, 
                    replace(exitno_chn, '｜','|') as alias_chn,
                    replace(exitno_trd, '｜','|') as exitno_trd,
                    exitno_py,
                    exitno_eng,
                    exitno_ctn,
                    4
                  FROM org_roadcrosssignpost
                  where exitno_chn is not null
                );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        # 将多个出口号(8|3),拆成多行(8, 3)
        sqlcmd = """SELECT mid_split_org_signpost_exit_no();"""
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        sqlcmd = """SELECT mid_make_signpost_exitno_dictionary();"""
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        return 0
    
    def __CheckRoadAliasName(self):
        sqlcmd = """
            SELECT count(gid)
              FROM org_roadsegment
              where alias_chn is not null and name_chn is null;
        """
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchone2()
        if row:
            if row[0] == 0:
                return 0
            else:
                return -1
        return 0
    
    # POI名称字典
    def _MakePOIName(self):
        "多个POI名时，也只做一个，而且不做POI别名。"
        self.log.info('Making POI Name Dictionary...')
        
        self.CreateTable2('temp_poi_name')
        self.CreateFunction2('mid_make_poi_dictionary')
        self.CreateIndex2('org_poi_meshid_poi_idx')
        self.CreateIndex2('org_poiname_meshid_name_idx')
        
        sqlcmd = """SELECT mid_make_poi_dictionary();"""
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.CreateIndex2('temp_poi_name_meshid_old_name_id_idx')

        
        # 将数据插入表poi_tbl
        self.CreateTable2('poi_tbl')
        sqlcmd = """
                INSERT INTO poi_tbl(poi_id, name_id, poi_type, the_geom)
                (
                SELECT a.gid, b.new_name_id, poi_type,
                       ST_SetSRID(ST_Point(x_coord/3600, y_coord/3600),4326)
                  FROM org_poi as a
                  left join temp_poi_name as b
                  on a."name" = b.old_name_id and a.meshid = b.meshid
                  order by gid
                );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        return 0
    
    # POI名称字典(临时使用)
    def MakePOINameTemp(self):
        "多个POI名时，也只做一个，而且不做POI别名。"
        self.log.info('Making POI Name Dictionary...')
        # 创建表poi_tbl
        self.CreateTable2('poi_tbl')
        
        # 将poi名表插入字典，并把poi数据插入表poi_tbl
        self.CreateFunction2('mid_make_poi_dictionary_temp')
        sqlcmd = """SELECT mid_make_poi_dictionary_temp();"""
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.pg.close2()
        return 0
    
    def MakeHighWayInterChangeName(self):
        """HighWay InterChange 的名称。"""
        self.log.info('Making HighWay InterChange Name...')
        # temp: hcz
        self.pg.connect2()
        # 创建表
        self.CreateTable2('temp_interchange_name')
        self.CreateTranslateFunc()
        self.CreateFunction2('mid_add_one_name')
        
        # 删除名称前的第一个数字（注：只删除仅一个数字开头的，多个数字开头的不删除）
        self.CreateFunction2('mid_delete_first_one_digit')
        
        # 将Hwy InterChange名插入字典
        self.CreateFunction2('mid_make_interchange_name_dictionary')
        sqlcmd = """SELECT mid_make_interchange_name_dictionary();"""
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return 0
    
    def MakeHighWaySAPAName(self):
        """HighWay SA/PA 的名称。"""
        self.log.info('Making HighWay SA/PA Name...')
        # temp: hcz
        self.pg.connect2()
        # 创建表
        self.CreateTable2('temp_sapa_name')
        
        # 将Hwy SAPA名插入字典
        self.CreateFunction2('mid_make_sa_pa_name_dictionary')
        sqlcmd = """SELECT mid_make_sa_pa_name_dictionary();"""
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return 0
    
    def CreateTranslateFunc(self):
        '''创建函数 mid_translate: 把长字符，转成半字符'''
        sqlcmd = """     
-------------------------------------------------------------------------------------------------------------
-- Full-width ==> half-width
-------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION mid_translate(_name character varying)
  RETURNS character varying 
  LANGUAGE plpgsql 
  AS $$
begin
    return translate(_name, 'ＡＢＣＤＥＦＧＨＩＪＫＬＭＮＯＰＱＲＳＴＵＶＷＸＹＺ１２３４５６７８９０', 'ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890');
END;
$$;
"""
        self.pg.execute2(sqlcmd)
        self.pg.commit2()