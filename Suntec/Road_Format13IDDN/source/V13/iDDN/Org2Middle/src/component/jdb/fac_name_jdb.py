# -*- coding: UTF8 -*-
'''
Created on 2013-2-4

@author: hongchenzai
'''
import common
import base
class comp_fac_name_jdb(base.component_base.comp_base):
    '''
    Facility name(高速道或有料道设施名。)
    '''

    def __init__(self):
        '''
        Constructor
        '''
        base.component_base.comp_base.__init__(self, 'Facility_Name')        
        
    def _DoCreateTable(self):
        
        if self.CreateTable2('mid_temp_facilit_matched')  == -1:
            return -1
        return 0
    
    def _DoCreateFunction(self):
        self.CreateFunction2('mid_has_ramp_link')
        self.CreateFunction2('rdb_cnv_link_type')
        self.CreateFunction2('rdb_cnv_link_toll')
        #self.CreateFunction2('mid_check_hwy_node_type')
        self.CreateFunction2('mid_check_hwy_exit_node')
        self.CreateFunction2('rdb_cnv_road_type')
        
        return 0
    
    def _Do(self):
        # 求出口施设点(IC、Ramp、JCT出口)到附近施设点的所有可能路径
        self._MakeOutFacilityFullPath()
        # 求入口施设点(IC、Ramp、SmartIC)到附近施设点的所有可能路径
        self._MakeEnterFicalityFullPath()
        ### 匹配JCT出口和入口
        self._MakeJCTFacName()
        self._MakeSAFacName()
        self._MakePAFacName()
        self._MakeRampExitFacName()
        self._MakeICExitFacName()
        self._MakeSmartICExitName()
        # IC、Ramp、SmartIC入口设施名称及长路径。
        self._MakeEnterFacName()
        #self.CreateIndex2('mid_fac_name_full_path_in_link_id_node_id_fac_type_idx')
        self._AddFacNameId()
        # 把过长的Path Link，修成短的
        self._MakeFacNamePassLink()
        self._DealWithInnerLink()     # 处理Inner Link
        #self._MakeFacNameDict()      # 字典
        self._MakeFacName2Toward()    # 添加TowardName
        #self._MakeHwyTollNodeName()  # 高速收费站名称
        self._CreateView()
        return 0
    
    def _MakeOutFacilityFullPath(self):
        '''求出口施设点(IC、Ramp、JCT出口)到附近施设点的所有可能路径。'''
        self.log.info('Make Facility OUT Full Path.')
        # 注：1. 这里不判断设施号是否一致，只是从出口出发往前一直探索，找到一个点施设点止。
        # 2. 这里取得full_pass_link：是起点施设点的进入link，到下个施设点的进入link
        # 3. 这里两个施设点间可能存在多条路径，主要是因为SA/PA里面会有条种复杂的路
        self.CreateFunction2('mid_check_hwy_node_type_jdb')   # 判断该是不是施设点
        self.CreateFunction2('mid_find_highway_path_out_jdb') # 探索路径
        self.CreateFunction2('mid_get_in_link_jdb')
        self.CreateFunction2('rdb_cnv_oneway_code_new')
        self.CreateTable2('mid_temp_facility_full_path')
        self.CreateTable2('mid_temp_facility_full_path_jct')
        
        ### SAPA/IC/Ramp/SmartIC Out 
        sqlcmd = """
            INSERT INTO mid_temp_facility_full_path(node_id, in_link_id, full_pass_link, to_node)
            (
            select node_id, 
                (regexp_split_to_array(pathes[path_seq_num], E'\\\|+'))[1]::bigint as in_link_id,
                (regexp_split_to_array(pathes[path_seq_num], E'\\\|+'))[2:
                    (array_upper(regexp_split_to_array(pathes[path_seq_num], E'\\\|+'), 
                    1) - 1)] as full_pass_link,
                (regexp_split_to_array(pathes[path_seq_num], E'\\\|+'))[
                    array_upper(regexp_split_to_array(pathes[path_seq_num], E'\\\|+'), 
                    1)]::bigint as to_node
              from (
                select node_id, Pathes, generate_series(1, array_upper(Pathes, 1)) as path_seq_num
                from (
                    -- SAPA/IC/Ramp/SmartIC Out 
                    select node_id, mid_find_highway_path_out_jdb(node_id, array[1, 2, 4, 5, 7]) as Pathes
                      from (
                      SELECT distinct(node_id)
                        FROM highway_node_add_nodeid
                        where tollclass_c = 0 and inout_c = 2 and facilclass_c in (1, 2, 4, 5, 7) 
                      ) as a
                ) as b
              ) as c
            );
            """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        ### JCT
        sqlcmd = """
            INSERT INTO mid_temp_facility_full_path_jct(node_id, in_link_id, full_pass_link, to_node)
            (
            select node_id, 
                (regexp_split_to_array(pathes[path_seq_num], E'\\\|+'))[1]::bigint as in_link_id,
                (regexp_split_to_array(pathes[path_seq_num], E'\\\|+'))[2:
                    (array_upper(regexp_split_to_array(pathes[path_seq_num], E'\\\|+'), 
                    1) - 1)] as full_pass_link,
                (regexp_split_to_array(pathes[path_seq_num], E'\\\|+'))[
                    array_upper(regexp_split_to_array(pathes[path_seq_num], E'\\\|+'), 
                    1)]::bigint as to_node
              from (
                select node_id, Pathes, generate_series(1, array_upper(Pathes, 1)) as path_seq_num
                from (
                    -- JCT Out
                    select node_id, mid_find_highway_path_out_jdb(node_id, array[3]) as Pathes
                      from (
                        SELECT distinct(node_id)
                        FROM highway_node_add_nodeid
                        where tollclass_c = 0 and inout_c = 2 and facilclass_c = 3
                      ) as a
   
                ) as b
              ) as c
            );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.CreateTable2('mid_temp_facility_full_path_node_id_idx')
        return 0
    
    def _MakeEnterFicalityFullPath(self):
        '''求入施设点(IC、Ramp、SmartIC)到附近施设点的所有可能路径。'''
        self.log.info('Make Facility IN Full Path.')
        
        self.CreateFunction2('mid_findpath_hwy_enter_jdb')  # 算速入口的路径
        self.CreateTable2('mid_temp_facility_full_path_enter_no_inlink')
        sqlcmd = '''
            INSERT INTO mid_temp_facility_full_path_enter_no_inlink(node_id, full_pass_link, to_node)
            (
            select (regexp_split_to_array(pathes[path_seq_num], E'\\\|+'))[
                            array_upper(regexp_split_to_array(pathes[path_seq_num], E'\\\|+'), 
                            1)]::bigint as node_id, -- Enter node on Nwy
                    (regexp_split_to_array(pathes[path_seq_num], E'\\\|+'))[1:
                    (array_upper(regexp_split_to_array(pathes[path_seq_num], E'\\\|+'), 
                            1) - 1)] as full_pass_link,
                    node_id as to_node  -- Enter node on Hwy
                    
             from (
                select node_id, Pathes, generate_series(1, array_upper(Pathes, 1)) as path_seq_num
                      from (
                    SELECT  node_id,  mid_findpath_hwy_enter_jdb(node_id) as Pathes
                      FROM (
                        select distinct node_id
                          from highway_node_add_nodeid
                          -- Enter node on Hwy
                          where tollclass_c = 0 and inout_c = 1 and facilclass_c in (4, 5, 7)
                    ) as a
                  ) as b
              ) as c
            );
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        # 入口点(一般道要上高速处)开始的倒着找查in link
        # 当遇到Inner Link时，继续找，找到非Inner Link止
        self.CreateFunction2('mid_findpath_hwy_enter_inlink_jdb')  # 算高速入口点的in link
        self.CreateTable2('mid_temp_facility_full_path_enter')
        sqlcmd = """
            insert into mid_temp_facility_full_path_enter(node_id, in_link_id, full_pass_link,
                    to_node, road_code, road_seq, facilclass_c, inout_c)
            (
            select node_id, enter_link[1]::bigint as in_link_id, 
                enter_link[2:array_upper(enter_link, 1)] || full_pass_link as full_pass_link,
                to_node, road_code, road_seq, facilclass_c, inout_c
              from (
                select node_id, 
                    regexp_split_to_array(enter_link_array[path_seq_num], E'\\\|+') as enter_link,
                    full_pass_link,
                    to_node, road_code, road_seq, 
                    facilclass_c, inout_c
                  from (
                    select node_id, full_pass_link, to_node, road_code, road_seq,
                           enter_link_array,
                           generate_series(1, array_upper(enter_link_array, 1)) as path_seq_num,
                           facilclass_c, inout_c
                      from (
                        select node_id, full_pass_link, to_node, road_code, road_seq,
                            mid_findpath_hwy_enter_inlink_jdb(node_id, full_pass_link[1]::bigint) as enter_link_array,
                            facilclass_c, inout_c
                         from (
                            select distinct a.node_id, full_pass_link, to_node, b.road_code, 
                                b.road_seq, c.facilclass_c, c.inout_c
                              from mid_temp_facility_full_path_enter_no_inlink as a
                              left join highway_node_add_nodeid as b
                              on a.node_id = b.node_id
                              left join highway_node_add_nodeid as c
                              on a.to_node = c.node_id
                              where b.inout_c = 1 and b.tollclass_c = 4 and b.facilclass_c = c.facilclass_c and
                                b.road_code = c.road_code and b.road_seq = c.road_seq
                         ) as a
                      ) as b
                  ) as c
              ) as d
            );
            """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        return 0
    
    def _MakeJCTFacName(self):
        '''JCT设施名称及长路径。'''
        self.log.info('Making JCT Facility Name...')
        # 从highway的某一起点,到指定类型(7:JCT出口)的所有点的Path
        self.CreateFunction2('mid_get_sapa_link_index')  
        
        # [JCT出口] 匹配  [JCT入口] 和 路径 (经过SAPA link的路经都不要)
        # tollclass_c = 0  : 非收费站
        # inout_c = 2      : 出
        # facilclass_c = 3 : JCT
        # 有三个点处在非高速上 node_id: 17750026, 17750028, 19233874
        sqlcmd = """
            INSERT INTO mid_temp_facilit_matched (
                        node_id, in_link_id, to_node, road_code, road_seq, 
                        full_pass_link, facilclass_c, inout_c)
            (
                select distinct a.node_id, in_link_id, to_node, a.road_code, a.road_seq, full_pass_link, 
                        facilclass_c, inout_c
                  from (
                    -- Get All JCT OUT
                    SELECT node_id, road_code, road_seq, inout_c, facilclass_c
                      FROM highway_node_add_nodeid
                      where tollclass_c = 0 and inout_c = 2 and facilclass_c = 3
                  ) as a
                  left join mid_temp_facility_full_path_jct as b
                  on a.node_id = b.node_id 
                  where mid_get_sapa_link_index(full_pass_link) = 0 and in_link_id is not null
                  order by a.node_id, to_node
            );
            """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return 0
        
    def _MakeSAFacName(self):
        '''IC设施名称及长路径。'''
        self.log.info('Making SA Facility Name...')
        # 高速道两点之间的路径
        # 如：SA入口-->SA, PS入口--->PA
        
        # [SA出口] 匹配  [入口] 和 路径 (不经过SAPA link的路经都不要)
        # 由于SA里面可以转圈，存在多条路径 和 算到对面SA的情况
        # 首先会对full_path_link，进行截取，截至第一条SA Link
        # 然后只取最快到达SA Link的那条路径（截取后的路径， 例: node_id = 694200， 3663721）
        # 经过处理后，还存在两条空到对面的情况，
        # 这里先不做处理，直接插入mid_temp_facilit_matched (例：node_id = 3958303, 3958307)
        # tollclass_c = 0  : 非收费站
        # inout_c = 2      : 出
        # facilclass_c = 1 : SA
        # mid_get_sapa_link_index(full_pass_link) > 0 : 经过SAPA link
        sqlcmd = """
            INSERT INTO mid_temp_facilit_matched(
                        node_id, in_link_id, to_node, road_code, road_seq, 
                        full_pass_link, facilclass_c, inout_c)
            (
            select node_id, in_link_id, to_node, road_code, road_seq, 
                regexp_split_to_array((array_agg(pass_link))[1], E'\\,+'),
                facilclass_c, inout_c
              from (
                select distinct a.node_id, in_link_id, to_node, c.road_code, c.road_seq, 
                    array_to_string(full_pass_link[1:mid_get_sapa_link_index(full_pass_link)], 
                            ',') as pass_link,
                    mid_get_sapa_link_index(full_pass_link) as sapa_idx,
                    facilclass_c, inout_c
                  from (
                    -- Get All SA OUT
                    SELECT node_id, road_code, road_seq, inout_c
                      FROM highway_node_add_nodeid
                      where tollclass_c = 0 and inout_c = 2 and facilclass_c = 1
                  ) as a
                  left join mid_temp_facility_full_path as b
                  on a.node_id = b.node_id
                  left join (
                    SELECT node_id, road_code, road_seq, name_kanji, name_yomi, facilclass_c
                      FROM highway_node_add_nodeid
                      where tollclass_c = 0 and inout_c = 1 and facilclass_c = 1
                  ) as c
                  on b.to_node = c.node_id
                  where c.node_id is not null and mid_get_sapa_link_index(full_pass_link) > 0 and
                    a.road_code = c.road_code and a.road_seq = c.road_seq 
                  order by node_id, in_link_id, to_node, sapa_idx
              ) as d
              group by node_id, in_link_id, to_node, road_code, road_seq, facilclass_c, inout_c
            );
            """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        return 0
    
    def _MakePAFacName(self):
        '''IC设施名称及长路径。'''
        self.log.info('Making PA Facility Name...')

        # 参考SA的做成方法
        # 有一个case一条Link要经过两次 (node_id = 16668478)
        sqlcmd = """
            INSERT INTO mid_temp_facilit_matched(
                        node_id, in_link_id, to_node, road_code, road_seq, 
                        full_pass_link, facilclass_c, inout_c)
            (
            select node_id, in_link_id, to_node, road_code, road_seq, 
                regexp_split_to_array((array_agg(pass_link))[1], E'\\,+'),
                facilclass_c, inout_c
              from (
                select distinct a.node_id, in_link_id, to_node, c.road_code, c.road_seq, 
                    array_to_string(full_pass_link[1:mid_get_sapa_link_index(full_pass_link)], 
                            ',') as pass_link,
                    mid_get_sapa_link_index(full_pass_link) as sapa_idx,
                    facilclass_c, inout_c
                  from (
                    -- Get All PA OUT
                    SELECT node_id, road_code, road_seq, inout_c
                      FROM highway_node_add_nodeid
                      where tollclass_c = 0 and inout_c = 2 and facilclass_c = 2
                  ) as a
                  left join mid_temp_facility_full_path as b
                  on a.node_id = b.node_id
                  left join (
                    SELECT node_id, road_code, road_seq, name_kanji, name_yomi, facilclass_c
                      FROM highway_node_add_nodeid
                      where tollclass_c = 0 and inout_c = 1 and facilclass_c = 2
                  ) as c
                  on b.to_node = c.node_id
                  where c.node_id is not null and mid_get_sapa_link_index(full_pass_link) > 0 
                      and a.road_code = c.road_code and a.road_seq = c.road_seq 
                  order by node_id, to_node, sapa_idx
              ) as d
              group by node_id, in_link_id, to_node, road_code, road_seq, facilclass_c, inout_c
            );
            """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        return 0

    def _MakeRampExitFacName(self):
        '''Ramp设施名称及长路径。'''
        self.log.info('Making PA Facility Name...')
        
        sqlcmd = """
            INSERT INTO mid_temp_facilit_matched(
                        node_id, in_link_id, to_node, road_code, road_seq, 
                        full_pass_link, facilclass_c, inout_c)
            (
                select distinct a.node_id, in_link_id, to_node, c.road_code, c.road_seq, 
                     full_pass_link, facilclass_c, inout_c
                  from (
                    -- Get All Ramp OUT
                    SELECT node_id, road_code, road_seq, inout_c
                      FROM highway_node_add_nodeid
                      where tollclass_c = 0 and inout_c = 2 and facilclass_c = 4
                  ) as a
                  left join mid_temp_facility_full_path as b
                  on a.node_id = b.node_id
                  left join (
                    SELECT node_id, road_code, road_seq, name_kanji, name_yomi, facilclass_c
                      FROM highway_node_add_nodeid
                      where tollclass_c = 4 and inout_c = 2 and facilclass_c = 4
                  ) as c
                  on b.to_node = c.node_id
                  where c.node_id is not null and mid_get_sapa_link_index(full_pass_link) = 0 
                      and a.road_code = c.road_code and a.road_seq = c.road_seq 
                  order by node_id, to_node
            );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        # 特例: Ramp要穿过 JCT，才能找到对应的点(node_id = 1188862)    
        return 0
    
    def _MakeICExitFacName(self):
        '''IC设施名称及长路径。'''
        self.log.info('Making IC Exit Facility Name...')
        # 高速道两点之间的路径
        # tollclass_c = 0: 高速上
        # tollclass_c = 4: 一般道上
        sqlcmd = """
            INSERT INTO mid_temp_facilit_matched(
                        node_id, in_link_id, to_node, road_code, road_seq, 
                        full_pass_link, facilclass_c, inout_c)
            (
                select distinct a.node_id, in_link_id, to_node, c.road_code, c.road_seq, 
                    full_pass_link, facilclass_c, inout_c
                  from (
                    -- Get All IC OUT
                    SELECT node_id, road_code, road_seq, inout_c
                      FROM highway_node_add_nodeid
                      where tollclass_c = 0 and inout_c = 2 and facilclass_c = 5
                  ) as a
                  left join mid_temp_facility_full_path as b
                  on a.node_id = b.node_id
                  left join (
                    SELECT node_id, road_code, road_seq, name_kanji, name_yomi, facilclass_c
                      FROM highway_node_add_nodeid
                      where tollclass_c = 4 and inout_c = 2 and facilclass_c = 5
                  ) as c
                  on b.to_node = c.node_id
                  where c.node_id is not null and mid_get_sapa_link_index(full_pass_link) = 0 
                      and a.road_code = c.road_code and a.road_seq = c.road_seq
                  order by node_id, to_node
            );
            """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        return 0
        
    def _MakeSmartICExitName(self):
        """Smart IC 出口设施名称及长路径"""
        
        # 同时有SAPA在的情况SmartIC名称不做: mid_get_sapa_link_index(full_pass_link) = 0
        sqlcmd = """
            INSERT INTO mid_temp_facilit_matched(
                        node_id, in_link_id, to_node, road_code, road_seq, 
                        full_pass_link, facilclass_c, inout_c)
            (
                select distinct a.node_id, in_link_id, to_node, c.road_code, c.road_seq, 
                    full_pass_link, facilclass_c, inout_c
                  from (
                    -- Get All Smart IC OUT
                    SELECT node_id, road_code, road_seq, inout_c
                      FROM highway_node_add_nodeid
                      where tollclass_c = 0 and inout_c = 2 and facilclass_c = 7
                  ) as a
                  left join mid_temp_facility_full_path as b
                  on a.node_id = b.node_id
                  left join (
                    SELECT node_id, road_code, road_seq, name_kanji, name_yomi, facilclass_c
                      FROM highway_node_add_nodeid
                      where tollclass_c = 4 and inout_c = 2 and facilclass_c = 7
                  ) as c
                  on b.to_node = c.node_id
                  where c.node_id is not null and mid_get_sapa_link_index(full_pass_link) = 0 
                      and a.road_code = c.road_code and a.road_seq = c.road_seq
                  order by node_id, to_node
            );
            """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        return 0
    
    def _MakeEnterFacName(self):
        '''IC、Ramp、SmartIC入口设施名称及长路径。'''
        self.log.info('Making IC/Ramp/SmartIC Enter Facility Name...')
        
        self.CreateFunction2('mid_get_intersect_node')          # 那取两条link的交点node
        # mid_temp_facility_full_path_enter表的node_id是highway数据的入口点，
        # 但不定是inlink和下条link的交点
        sqlcmd = """
            INSERT INTO mid_temp_facilit_matched(
                                    node_id, in_link_id, to_node, road_code, road_seq, 
                                    full_pass_link, facilclass_c, inout_c)
            (
            SELECT node_id, in_link_id, to_node, road_code, road_seq, 
                    full_pass_link, facilclass_c, inout_c
              FROM mid_temp_facility_full_path_enter
            );
            """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return 0
        
    def _AddFacNameId(self):
        '''取得对应的名称ID'''
        self.CreateTable2('mid_temp_facilit_matched_add_nameid')
        
        # mid_temp_facility_full_path_enter表的node_id是入口点，但不定是inlink和下条link的交点
        sqlcmd = """
            insert into mid_temp_facilit_matched_add_nameid(
                            node_id, in_link_id, to_node, road_code, road_seq,
                            full_pass_link, facilclass_c, inout_c, sp_name)
            (
            SELECT mid_get_intersect_node(in_link_id, full_pass_link[1]::bigint) as node_id,
                     in_link_id, to_node, a.road_code, a.road_seq, 
                    full_pass_link, a.facilclass_c, inout_c,
                    (case when name_kanji is null then null else mid_get_json_string_for_japan_name(name_kanji,name_yomi) end) as sp_name
              FROM mid_temp_facilit_matched as a
              left join facil_info_point_4326 as b
              on a.road_code = b.road_code and a.road_seq = b.road_seq
            );
            """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return 0
    
    def _MakeFacNamePassLink(self):
        '''[长路径]变成[短路径]。'''
        self.log.info('Short Cut the Facility Name Path...')
        
        self.CreateFunction2('mid_has_ramp_link')               # 判断出去link是否有Ramp
        self.CreateFunction2('mid_get_intersect_node')          # 那取两条link的交点node
        self.CreateFunction2('mid_get_fac_name_shot_passlid')   # 长的路径截成短的
        
        self.CreateTable2('mid_fac_name')
        # 把长的Path Link，修成短的。
        # 规则是：1. 修短到无分支处 。2. 如果各个分支名称和类型相同，就继续继切短
        # 注：如果Smart IC出口名和其他名同时存在时，Smart IC名不做。
        sqlcmd = """
            INSERT INTO mid_fac_name(node_id, in_link_id, out_link_id, pass_link, passlink_cnt, 
                            facilclass_c, inout_c, sp_name )
            (
            SELECT node_id, in_link_id, 
                (regexp_split_to_array(short_pass_link, E'\\\|+'))[link_num]::bigint as out_link_id,
                (case when link_num = 1 then NULL else 
                            array_to_string((regexp_split_to_array(short_pass_link, E'\\\|+'))[1:
                                    (link_num - 1)], '|') end) as pass_link,
                (link_num - 1) as passlink_cnt,
                facilclass_c, inout_c, sp_name 
              from ( 
                    select distinct node_id, in_link_id, short_pass_link, facilclass_c, inout_c, sp_name,
                           array_upper(regexp_split_to_array(short_pass_link, E'\\\|+'), 1) as link_num
                      from (
                            SELECT  node_id, in_link_id, to_node, 
                                    full_pass_link, facilclass_c, inout_c, sp_name,
                                    mid_get_fac_name_shot_passlid(gid, in_link_id, node_id, full_pass_link,
                                            to_node, facilclass_c, inout_c, sp_name) as short_pass_link
                              FROM mid_temp_facilit_matched_add_nameid
                      ) as a
                      where short_pass_link is not null
              ) as b
              order by in_link_id, node_id, out_link_id, facilclass_c, passlink_cnt, inout_c
            );
            """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.CreateIndex2('mid_fac_name_node_id_idx')
        return 0
    
    def _DealWithInnerLink(self):
        '处理Inner Links. 使用高速入口的pass link中的Inner Link，作为Inlink.'
        self.log.info('Deal With Inner Links...')
        self.CreateFunction2('rdb_cnv_link_type')
        self.CreateFunction2('mid_get_intersect_node')
        
        sqlcmd = """
            INSERT INTO mid_fac_name(node_id, in_link_id,
                            out_link_id, pass_link, passlink_cnt, facilclass_c, 
                            inout_c, sp_name)
            (
                select distinct node_id, new_inlink_id, out_link_id, new_passlid, new_passlink_cn,
                       facilclass_c, inout_c, sp_name
                from (
                        select mid_get_intersect_node(new_inlink_id, case when new_passlink_cn > 0 then 
                                                    (new_passlid[1])::bigint else out_link_id end) as node_id,
                              new_inlink_id, out_link_id, 
                              array_to_string(new_passlid, '|') as new_passlid, new_passlink_cn,
                              facilclass_c, inout_c, sp_name
                          from (
                            select node_id, in_link_id, out_link_id, pass_link, 
                                   passlink_cnt, facilclass_c, inout_c, sp_name, 
                                  (regexp_split_to_array(pass_link, E'\\\|+'))[(passlink_seq_num)]::bigint as new_inlink_id,
                                  (regexp_split_to_array(pass_link, E'\\\|+'))[(passlink_seq_num + 1) : passlink_cnt] as new_passlid,
                                  (passlink_cnt - passlink_seq_num ) as new_passlink_cn
                            from (
                                select node_id, in_link_id, out_link_id, pass_link, 
                                       passlink_cnt, facilclass_c, inout_c, sp_name, 
                                       (regexp_split_to_array(pass_link, E'\\\|+'))[passlink_seq_num]::bigint as pass_link_id,
                                       passlink_seq_num
                                from (
                                    SELECT node_id, in_link_id, out_link_id, pass_link, 
                                           passlink_cnt, facilclass_c, inout_c, sp_name, 
                                           generate_series(1, passlink_cnt) as passlink_seq_num
                                      FROM mid_fac_name
                                      where inout_c = 1 and facilclass_c in (4, 5, 7) and passlink_cnt > 0 
                                  ) as a
                            ) as b
                            left join road_link_4326
                            on pass_link_id = objectid
                            where rdb_cnv_link_type(linkclass_c) = 4
                          ) as c
                 ) as e
                 order by new_inlink_id, node_id, out_link_id, facilclass_c, new_passlink_cn, inout_c
            );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return 0
        
    def _MakeFacName2Toward(self):
        '''把Facility name做到表towardname_tbl，但是namekind=2(地点名称)'''
        self.log.info('Insert Facility Name into TowardName table...')

        self.CreateFunction2('mid_get_new_passlid')
        self.CreateFunction2('mid_get_intersect_node')
        self.CreateFunction2('mid_convert_fac_name_guideattr')
        
        sqlcmd = """
                INSERT INTO towardname_tbl(
                            id, nodeid, inlinkid, outlinkid, passlid, passlink_cnt, 
                            direction, guideattr, namekind, namekind2, toward_name, "type")
                (
                SELECT gid, node_id, fromlinkid, tolinkid, passlid,
                       (CASE WHEN passlid is null THEN 0 ELSE array_upper(regexp_split_to_array(passlid, E'\\\|+'), 1) END) as passlink_cnt,
                        0, mid_convert_fac_name_guideattr(facilclass_c) as guideattr, 
                        2 as namekind, 0, toward_name, 1
                  FROM (
                    SELECT foo.gid, A.tile_link_id as fromlinkid, 
                            B.tile_node_id as node_id, 
                            C.tile_link_id as tolinkid, 
                           mid_get_new_passlid(pass_link, out_link_id::integer, '|')  as passlid,
                           facilclass_c, inout_c, toward_name
                        from (
                            SELECT gid, node_id, in_link_id, out_link_id, pass_link,
                                mid_get_intersect_node((case when passlink_cnt = 0 then in_link_id 
                                        else (regexp_split_to_array(pass_link, E'\\\|+'))[passlink_cnt]::bigint end
                                        ), out_link_id) as from_nodeid,
                                facilclass_c, inout_c, sp_name as toward_name
                            FROM mid_fac_name
                      ) as foo
                      LEFT JOIN middle_tile_link AS A
                      ON in_link_id = A.old_link_id and node_id = Any(A.old_s_e_node)
                      LEFT JOIN middle_tile_node AS B
                      ON node_id= B.old_node_id
                      LEFT JOIN middle_tile_link AS C
                      ON out_link_id = C.old_link_id and from_nodeid = Any(C.old_s_e_node)
                      order by foo.gid
                 ) AS d
                 order by gid
                );
            """
        self.pg.execute2(sqlcmd) 
        self.pg.commit2()
        
        return 0
    
    def _CreateView(self):
        #return 0
        self.CreateIndex2('name_dictionary_tbl_name_id_idx')
        
        sqlcmd = """
        create or replace view view_towardname_fac_name
        as 
        (
        SELECT towardname_tbl.gid, id, nodeid, inlinkid, outlinkid, passlid, passlink_cnt, 
               direction, guideattr, namekind, namekind2, towardname_tbl.toward_name, "type", the_geom
          FROM towardname_tbl
          left join node_tbl
          on nodeid = node_id
          where namekind = 2
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return 0
    
    def _DoCheckNumber(self):
        self._DoCheckJCT()
        self._DoCheckSA()
        self._DoCheckPA()
        self._DoCheckICExit()
        self._DoCheckICEnter()
        self._DoCheckRampExit()
        self._DoCheckRampEnter()
        self._DoCheckSmartICExit()
        self._DoCheckSmartICEnter()
        return 0
    
    def _DoCheckJCT(self):
        # 判断JCT出口和入口，是否少了
        # 注：这种方法的缺点是，不能保证JCT出口到入口两两之间的关系都能找出来
        sqlcmd = """
            select count(*)
            from (
                    -- JCT OUT
                    SELECT distinct node_id
                      FROM highway_node_add_nodeid
                      where facilclass_c = 3 and inout_c = 2 and
                            node_id not in (
                            select distinct node_id
                              from mid_temp_facilit_matched
                              where facilclass_c = 3
                            )
                    union  
                    -- JCT IN
                    SELECT distinct node_id
                      FROM highway_node_add_nodeid
                      where facilclass_c = 3 and inout_c = 1 and
                            node_id not in (
                            select distinct to_node
                              from mid_temp_facilit_matched
                              where facilclass_c = 3
                            )
              ) as a;
            """
            
        rst = self._CheckResult(sqlcmd)
        if rst > 0:
            self.log.warning('There are %d JCT, not included.' % rst)
        return 0
    
    def _DoCheckSA(self):
        # SA
        sqlcmd = """
            select count(*)
            from (  -- SA Out
                    SELECT distinct node_id
                      FROM highway_node_add_nodeid
                      where facilclass_c = 1 and inout_c = 2 and
                            node_id not in (
                                    select distinct node_id
                                      from mid_temp_facilit_matched
                                      where facilclass_c = 1 --and inout_c = 2
                            )
                    union  
                    -- SA IN
                    SELECT distinct node_id
                      FROM highway_node_add_nodeid
                      where facilclass_c = 1 and inout_c = 1 and
                            node_id not in (
                                    select distinct to_node
                                      from mid_temp_facilit_matched
                                      where facilclass_c = 1 --and inout_c = 2
                            )
              ) as a;
          """
        rst = self._CheckResult(sqlcmd)
        if rst > 0:
            self.log.warning('There are %d SA, not included.' % rst)
        return 0 
     
    def _DoCheckPA(self):
        # PA
        sqlcmd = """
            select count(*)
            from (
                    -- PA Out
                    SELECT distinct node_id
                      FROM highway_node_add_nodeid
                      where facilclass_c = 2 and inout_c = 2 and tollclass_c = 0 and
                            node_id not in (
                                    select distinct node_id
                                      from mid_temp_facilit_matched
                                      where facilclass_c = 2 --and inout_c = 2
                            )
                    union  
            
                    -- PA IN
                    SELECT distinct node_id
                      FROM highway_node_add_nodeid
                      where facilclass_c = 2 and inout_c = 1 and tollclass_c = 0 and
                            node_id not in (
                                    select distinct to_node
                                      from mid_temp_facilit_matched
                                      where facilclass_c = 2 --and inout_c = 2
                            )
              ) as a;
            """
        rst = self._CheckResult(sqlcmd)
        if rst > 0:
            self.log.warning('There are %d PA, not included.' % rst)
        return 0
    
    def _DoCheckICExit(self):
        # IC Exit
        # 有两个点没有出来Node_id: 4358709 和 2942207
        # 其中node_id: 2942207，好像是原数据tollclass_c做错了,做成了4
        sqlcmd = """
            select count(*)
            from (  -- IC exit on HWY
                    SELECT distinct node_id
                      FROM highway_node_add_nodeid
                      where facilclass_c = 5 and inout_c = 2 and tollclass_c = 0 and
                            node_id not in (
                                    select distinct node_id
                                      from mid_temp_facilit_matched
                                      where facilclass_c = 5 and inout_c = 2
                            )
                    union
                    -- IC exit on NWY
                    SELECT distinct node_id
                      FROM highway_node_add_nodeid
                      where facilclass_c = 5 and inout_c = 2 and tollclass_c = 4 and
                            node_id not in (
                                    select distinct to_node
                                      from mid_temp_facilit_matched
                                      where facilclass_c = 5 and inout_c = 2
                            )
              ) as a;
          """
        rst = self._CheckResult(sqlcmd)
        if rst > 0:
            self.log.warning('There are %d IC Exit, not included.' % rst)
        return 0
    
    def _DoCheckICEnter(self):
        # IC Enter
        # 有两个点没有出来Node_id: 4358708 和 2942205
        # 其中node_id: 2942205，好像是原数据tollclass_c做错了,做成了4
        sqlcmd = """
            select count(*)
            from (  -- IC enter on HWY
                    SELECT distinct node_id
                      FROM highway_node_add_nodeid
                      where facilclass_c = 5 and inout_c = 1 and tollclass_c = 0 and
                            node_id not in (
                                    select distinct to_node
                                      from mid_temp_facilit_matched
                                      where facilclass_c = 5 and inout_c = 1
                            )
                    union
                    -- IC enter on NWY
                    SELECT distinct node_id
                      FROM highway_node_add_nodeid
                      where facilclass_c = 5 and inout_c = 1 and tollclass_c = 4 and
                            node_id not in (
                                    select distinct node_id
                                      from mid_temp_facilit_matched
                                      where facilclass_c = 5 and inout_c = 1
                            )
              ) as a;
          """
        rst = self._CheckResult(sqlcmd)
        if rst > 0:
            self.log.warning('There are %d IC Enter, not included.' % rst)
        return 0
    
    def _DoCheckRampExit(self):
        # Ramp 出口
        # 有两个点没有出来Node_id: 4358709 和 2942207
        # 其中node_id: 2942207，好像是数据做错了,tollclass_c做成了4
        sqlcmd = """
            select count(*)
            from (  -- Ramp exit on HWY
                    SELECT distinct node_id
                      FROM highway_node_add_nodeid
                      where facilclass_c = 4 and inout_c = 2 and tollclass_c = 0 and
                            node_id not in (
                                    select distinct node_id
                                      from mid_temp_facilit_matched
                                      where facilclass_c = 4 and inout_c = 2
                            )
                    union  
                    -- Ramp exit on NWY
                    SELECT distinct node_id
                      FROM highway_node_add_nodeid
                      where facilclass_c = 4 and inout_c = 2 and tollclass_c = 4 and
                            node_id not in (
                                    select distinct to_node
                                      from mid_temp_facilit_matched
                                      where facilclass_c = 4 and inout_c = 2
                            )
              ) as a;
          """
        rst = self._CheckResult(sqlcmd)
        if rst > 0:
            self.log.warning('There are %d Ramp Exit, not included.' % rst)
        return 0
    
    def _DoCheckRampEnter(self):
        # Ramp 入口
        sqlcmd = """
            select count(*)
            from (  -- Ramp enter on HWY
                    SELECT distinct node_id
                      FROM highway_node_add_nodeid
                      where facilclass_c = 4 and inout_c = 1 and tollclass_c = 0 and
                            node_id not in (
                                    select distinct to_node
                                      from mid_temp_facilit_matched
                                      where facilclass_c = 4 and inout_c = 1
                            )
                    union
                    -- Ramp enter on NWY
                    SELECT distinct node_id
                      FROM highway_node_add_nodeid
                      where facilclass_c = 4 and inout_c = 1 and tollclass_c = 4 and
                            node_id not in (
                                    select distinct node_id
                                      from mid_temp_facilit_matched
                                      where facilclass_c = 4 and inout_c = 1
                            )
              ) as a;
          """
        rst = self._CheckResult(sqlcmd)
        if rst > 0:
            self.log.warning('There are %d Ramp Enter, not included.' % rst)
        return 0

    def _DoCheckSmartICExit(self):
        # Smart IC Exit
        # 注：Smart IC有很多没有做成，是因为有SAPA存在。当SAPA同时存在时，SmartIC不做成。
        sqlcmd = """
            select count(*)
            from (  -- SmartIC exit on HWY
                    SELECT distinct node_id
                      FROM highway_node_add_nodeid
                      where facilclass_c = 7 and inout_c = 2 and tollclass_c = 0 and
                            node_id not in (
                                    select distinct node_id
                                      from mid_temp_facilit_matched
                                      where facilclass_c = 7 and inout_c = 2
                            )
                    union  
                    -- SmartIC exit on NWY
                    SELECT distinct node_id
                      FROM highway_node_add_nodeid
                      where facilclass_c = 7 and inout_c = 2 and tollclass_c = 4 and
                            node_id not in (
                                    select distinct to_node
                                      from mid_temp_facilit_matched
                                      where facilclass_c = 7 and inout_c = 2
                            )
              ) as a;
            """
        rst = self._CheckResult(sqlcmd)
        if rst > 0:
            self.log.warning('There are %d Smart IC Exit, not included.' % rst)
        return 0
    
    def _DoCheckSmartICEnter(self):
        # Smart IC Enter
        sqlcmd = """
            select count(*)
            from (  -- Ramp enter on HWY
                    SELECT distinct node_id
                      FROM highway_node_add_nodeid
                      where facilclass_c = 7 and inout_c = 1 and tollclass_c = 0 and
                            node_id not in (
                                    select distinct to_node
                                      from mid_temp_facilit_matched
                                      where facilclass_c = 7 and inout_c = 1
                            )
                    union
                    -- Ramp enter on NWY
                    SELECT distinct node_id
                      FROM highway_node_add_nodeid
                      where facilclass_c = 7 and inout_c = 1 and tollclass_c = 4 and
                            node_id not in (
                                    select distinct node_id
                                      from mid_temp_facilit_matched
                                      where facilclass_c = 7 and inout_c = 1
                            )
              ) as a;
            """
        rst = self._CheckResult(sqlcmd)
        if rst > 0:
            self.log.warning('There are %d Smart IC Enter, not included.' % rst)
        return 0
    
    def _CheckResult(self, sqlcmd):
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchone2()
        if row:
            if row[0] > 0: 
                return row[0]
        return 0
        
    

    