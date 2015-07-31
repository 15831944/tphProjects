# -*- coding: UTF8 -*-
'''
Created on 2013-9-4

@author: hongchenzai
'''

from base import comp_base
######################################################################################
## 方面名称格式转换类
####################################################################################
class comp_trans_dirguide_jdb(comp_base):
    '''方面名称格式转换：把rdb格式数转成老格式的数据(road_dir)。
    '''
    def __init__(self):
        '''
        Constructor
        '''
        comp_base.__init__(self, 'Trans_DirGuide')
    
    def _DoCreateTable(self):
        self.CreateTable2('temp_inf_dirguide')
        self.CreateTable2('temp_lq_dirguide')
        self.CreateTable2('temp_road_dir')
           
    def _DoCreateFunction(self):
        self.CreateFunction2('mid_trans_getnamearray')
        self.CreateFunction2('mid_get_json_string_for_japan_name')
    def _Do(self):
        '''方面名称格式转换：把rdb格式数转成老格式的数据(road_dir)。'''
        self.pg.connect2()
        self._CreatejudenameFunction()
        # 第一步：换转名称
        self._convertRDBName()
        # 第二步：换转link_id序
        self._convertRDBLink()
        # 第三步：合并上面两张表到temp_road_dir
        self._joinNameLink()
        return 0

    def _CreatejudenameFunction(self):
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
  
    def _convertRDBName(self):
        self.log.info('start transform inf_dirguide!')
        #'''名称换成老数据模式(列状==>行状)'''
        '''名称转换(列状==>行状)'''

        self.CreateIndex2('inf_dirguide_id_idx')
        ### 插入名称
        # 注1. 插入第1个到第10个名称,第1-10个名称为空，不插入.当name_kanji[1-10]全部为空时，插入一条空名称的记录
        # 2. guideclass[1-10]_c为空时，插入 -1
        # 3. sequence 为空时，也插入 -1
        sqlcmd = """
            INSERT INTO temp_inf_dirguide(objectid, guideclass_c, name_kanji, 
                            name_yomi, boardptn_c, board_seq, name_seqnm)
            (
              select objectid,name_lid[3]::integer as guideclass_c,
                  (case when name_lid[1] = '0' then null else name_lid[1] end) as name_kanji,
                  (case when name_lid[2] = '0' then null else name_lid[2] end) as name_yomi,
                  boardptn_c,board_seq,name_lid[4]::smallint as name_seqnm
             from
                  (
                      select objectid,
                            string_to_array(unnest(mid_trans_getnamearray(array[[name_kanji1,name_yomi1,guideclass1_c::character varying],
                                [name_kanji2,name_yomi2,guideclass2_c::character varying],
                                [name_kanji3,name_yomi3,guideclass3_c::character varying],
                                [name_kanji4,name_yomi4,guideclass4_c::character varying],
                                [name_kanji5,name_yomi5,guideclass5_c::character varying],
                                [name_kanji6,name_yomi6,guideclass6_c::character varying],
                                [name_kanji7,name_yomi7,guideclass7_c::character varying],
                                [name_kanji8,name_yomi8,guideclass8_c::character varying],
                                [name_kanji9,name_yomi9,guideclass9_c::character varying],
                                [name_kanji10,name_yomi10,guideclass10_c::character varying]])),',') as name_lid,
                            guidepattern_c as boardptn_c,
                            (case when "sequence" is null then -1 else "sequence" end) as board_seq
                      from inf_dirguide
                      order by objectid
                  ) as a
            );
            """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.log.info('end transform inf_dirguide!')
        
        return 0
    
    def _convertRDBLink(self):
        self.log.info('start transform lq_dirguide!')
        #'''link序换成老数据格式(行状==>列状)'''
        '''link序转换(行状==>列状)'''
        
        self.CreateIndex2('lq_dirguide_inf_id_idx')
        sqlcmd = """
            INSERT INTO temp_lq_dirguide(objectid, fromlinkid, tolinkid, midcount, midlinkid,link_dir_lib)
            (
                select  inf_id as objectid, 
                    link_lid[1] as fromlinkid,
                    link_lid[array_upper(link_lid, 1)] as tolinkid,
                    (array_upper(link_lid, 1) - 2 ) as midcount,
                    array_to_string(link_lid[2:(array_upper(link_lid, 1)-1)], ',') as midlinkid,
                    linkdir_lib as link_dir_lib
                from (
                    select inf_id, array_agg(link_id) as link_lid,array_agg(linkdir_c) as linkdir_lib
                    from (
                        SELECT objectid, link_id, linkdir_c, "sequence", inf_id
                        FROM lq_dirguide
                        order by inf_id, sequence
                    ) as a
                    group by inf_id
                ) as b 
            );
            """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.log.info('end transform lq_dirguide!')
        return 0
    
    def _joinNameLink(self):
        self.log.info('start merger lq_dirguide and lq_dirguide,create table temp_road_dir!')
        '''合并名称和Link序表到temp_road_dir.'''
        
        sqlcmd = """
            INSERT INTO temp_road_dir(objectid, fromlinkid, tolinkid, midcount, midlinkid,link_dir_kib,
                guideclass, name_kanji, sp_name, boardptn_c, 
                board_seq, name_seqnm)
            (
            SELECT  a.objectid, fromlinkid, tolinkid, midcount, midlinkid,link_dir_lib,
                guideclass_c, name_kanji, 
                (case when name_kanji is null then null else mid_get_json_string_for_japan_name(name_kanji,name_yomi) end) as sp_name, 
                boardptn_c, board_seq, name_seqnm
              FROM temp_inf_dirguide as a
              left join temp_lq_dirguide as b
              on a.objectid = b.objectid
              order by a.objectid, name_seqnm
            );
            """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.log.info('end merger lq_dirguide and lq_dirguide,create table temp_road_dir!')
        return 0    
    
######################################################################################
## Highway Node格式转换
####################################################################################
class comp_trans_highway_node_jdb(comp_base):
    '''Highway Node格式转换。
    '''
    def __init__(self):
        '''
        Constructor
        '''
        comp_base.__init__(self, 'Trans_HighwayNode')
    
    def _Do(self):
        self.CreateTable2('highway_node_add_nodeid')
        self.CreateIndex2('highway_node_4326_the_geom_idx')
        self.CreateIndex2('road_node_4326_the_geom_idx')
        sqlcmd = """
            INSERT INTO highway_node_add_nodeid
            (
            SELECT a.objectid, b.objectid as node_id, hwymode_f, a.road_code, a.road_seq, 
                   direction_c, inout_c,  approachnode_id, flownode_id, tollclass_c, 
                   tollfunc_c, dummytoll_f, guide_f, tollgate_lane, c.name_kanji, c.name_yomi,
                   dummyname_f, facilclass_c, dummyfacil_f,a.the_geom
              FROM highway_node_4326 as a
              inner join road_node_4326 as b
              on ST_Equals(b.the_geom, a.the_geom)
              left join facil_info_point_4326 as c
              on a.road_code = c.road_code and a.road_seq = c.road_seq
            );
            """
        
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return 0
    
    def _DoCreateIndex(self):
        self.CreateIndex2('highway_node_add_nodeid_node_id_idx')
        self.CreateIndex2('highway_node_add_nodeid_the_geom_idx')
        return 0
        