# -*- coding: UTF8 -*-
'''
Created on 2013-2-4

@author: hongchenzai
'''
import common
import base
class comp_fac_name(base.component_base.comp_base):
    '''
    Facility name(高速道或有料道设施名。)
    '''

    def __init__(self):
        '''
        Constructor
        '''
        base.component_base.comp_base.__init__(self, 'Facility_Name')        
        
    def _DoCreateTable(self):
        
        if self.CreateTable2('mid_fac_name_full_path') == -1:
            return -1
         
        #if self.CreateTable2('mid_fac_name_shot_path') == -1:
        #    return -1 
        return 0
    
    def _DoCreateFunction(self):
        self.CreateFunction2('mid_has_ramp_link')
        self.CreateFunction2('rdb_cnv_link_type')
        self.CreateFunction2('rdb_cnv_link_toll')
        self.CreateFunction2('mid_check_hwy_node_type')
        self.CreateFunction2('mid_check_hwy_exit_node')
        self.CreateFunction2('rdb_cnv_road_type')
        
        return 0
    
    def _Do(self):
        self._MakeRoadPointNode()   # 给road_point找到关联的node点
        self._MakeJCTFacName()
        self._MakeSAFacName()
        self._MakePAFacName()
        self._MakeICExitFacName()
        self._MakeICEnterFacName()
        self._MakeSmartICExitName()
        self._MakeSmartICEnterName()
        self.CreateIndex2('mid_fac_name_full_path_in_link_id_node_id_fac_type_idx')
        # 把过长的Path Link，修成短的
        self._MakeFacNamePassLink()
        self._DealWithInnerLink()    # 处理Inner Link
        self._MakeFacNameDict()      # 字典
        self._MakeFacName2Toward()
        self._MakeHwyTollNodeName()  # 高速收费站名称
        self._CreateView()
        return 0
    
    def _MakeRoadPointNode(self):
        # RoadPoint信息和对应的node
        self.log.info('Making RoadPointNode...')
        self.CreateTable2('mid_road_roadpoint_node')
        self.CreateIndex2('road_rnd_the_geom_idx')
        
        sqlcmd= """
                insert into mid_road_roadpoint_node
                (
                SELECT road_roadpoint.gid, objectid as node_id, 
                       point_mesh_code, point_x, point_y, mesh_id, fac_mesh_code, fac_x, 
                       fac_y, addr_id, road_point, fac_name, fac_yomi, road_code, fac_type, 
                       point_pos, point_name, point_yomi, sa_pa, name_f, fac_f, the_geom
                  FROM road_roadpoint
                  --on a.node_id <> b.node_id and ST_DWithin(b.the_geom, a.the_geom, 0.000007186444528518818301)
                  --where b.node_id is not null
                  inner join road_rnd
                  on ST_DWithin(the_geom, geom, 0.000002694945852358566745 / 10)
                );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.CreateIndex2('mid_road_roadpoint_node_node_id_idx')
        self.CreateIndex2('mid_road_roadpoint_node_the_geom_idx')
        
        self.CreateTable2('mid_road_roadpoint_bunnki_node')
        # road_roadpoint_bunnki
        sqlcmd = """
                insert into mid_road_roadpoint_bunnki_node
                (
                SELECT road_roadpoint_bunnki.gid, objectid as node_id, 
                       point_mesh_code, point_x, point_y, mesh_id, fac_mesh_code, fac_x, 
                       fac_y, addr_id, road_point, fac_name, fac_yomi, road_code, fac_type, 
                       point_pos, point_name, point_yomi, sa_pa, name_f, fac_f, the_geom
                  FROM road_roadpoint_bunnki
                  inner join road_rnd
                  on ST_DWithin(the_geom, geom, 0.000002694945852358566745 / 10)
                );
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.CreateIndex2('mid_road_roadpoint_bunnki_node_the_geom_idx')
        self.CreateIndex2('mid_road_roadpoint_bunnki_node_node_id_idx')
        # 还要check 个数是否正确。
        
        return 0
    
    def _MakeJCTFacName(self):
        '''JCT设施名称及长路径。'''
        self.log.info('Making JCT Facility Name...')
        # 从highway的某一起点,到指定类型(7:JCT出口)的所有点的Path
        self.CreateFunction2('mid_findPath_JCT')
        self.CreateFunction2('mid_get_start_link')
        self.CreateFunction2('mid_get_JCT_name')
        
        # 找到JCT分歧点的对应Forward方向的node点
        sqlcmd = """
INSERT INTO mid_fac_name_full_path(
            node_id, mesh_code, road_code, seq_num, pos_code, updown_flag, 
            fac_type, fac_name, in_link_id, path, to_node_id, path_seq_num)
(
    select fromNode, c.mesh_code, c.road_code, c.seq_num, c.pos_code, c.updown_flag, 
         4 as fac_type, mid_get_JCT_name(fromNode, toNode) as fac_name, in_link_id, path, toNode, path_seq_num
      from ( 
        select fromNode, mesh_code, road_code, seq_num, pos_code, updown_flag, 
            pathes,   
            (regexp_split_to_array(pathes[path_seq_num], E'\\\|+'))[1]::bigint as in_link_id,
            (regexp_split_to_array(pathes[path_seq_num], E'\\\|+'))[2:(array_upper(regexp_split_to_array(pathes[path_seq_num], E'\\\|+'), 1) - 1)] as path, 
            (regexp_split_to_array(pathes[path_seq_num], E'\\\|+'))[array_upper(regexp_split_to_array(pathes[path_seq_num], E'\\\|+'), 1)]::bigint as toNode, 
            path_seq_num
          from (
            select fromNode, mesh_code, road_code, seq_num, pos_code, updown_flag, 
                pathes, generate_series(1, array_upper(Pathes, 1)) as path_seq_num
              from (
                SELECT node_id as fromNode, mesh_code, road_code, seq_num, pos_code, updown_flag, 
                    kind, mid_findPath_JCT(node_id) as pathes     -- find pathes for JCT
                  FROM road_hwynode
                  where pos_code = 8
              ) as a
          ) as b
    ) as c
    order by fromNode, mesh_code, seq_num, pos_code, path 
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
        self.CreateFunction2('mid_findpath_hwy_two_node')
        
        # 注：只取最短的那一条path
        sqlcmd = """
insert into mid_fac_name_full_path(node_id, mesh_code, road_code, seq_num, pos_code, updown_flag, 
             fac_type, fac_name, in_link_id, path, to_node_id, path_seq_num)
(
    select node_id, mesh_code, road_code, seq_num, pos_code, updown_flag, fac_type, fac_name,
           (regexp_split_to_array(Path[1], E'\\\|+'))[1]::bigint as in_link_id,
           (regexp_split_to_array(Path[1], E'\\\|+'))[2:(array_upper(regexp_split_to_array(Path[1], E'\\\|+'), 1))] as path,
           toNode, 1 as path_seq_num
    from (
    SELECT mesh_code, a.node_id, a.road_code, seq_num, pos_code, updown_flag, 
           kind, fac_type, fac_name, b.node_id as toNode, 
           mid_findpath_hwy_two_node(a.node_id, b.node_id, 4) as Path
      FROM road_hwynode as a
      left join mid_road_roadpoint_node as b 
      on a.road_code = b.road_code and seq_num = road_point
      where a.pos_code = 4 and fac_type = 2 
           and ((updown_flag = 1 and point_pos = 1) or (updown_flag = 0 and point_pos = 2))
      
    ) as a
    order by node_id, mesh_code, seq_num, pos_code, path
);
    
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        # check node是否被遗漏了
        return 0
    
    def _MakePAFacName(self):
        '''IC设施名称及长路径。'''
        self.log.info('Making PA Facility Name...')
        # 高速道两点之间的路径
        # 如：SA入口-->SA, PS入口--->PA
        self.CreateFunction2('mid_findpath_hwy_two_node')
        
        # 注：只取最短的那一条path
        sqlcmd = """
insert into mid_fac_name_full_path(node_id, mesh_code, road_code, seq_num, pos_code, updown_flag, 
             fac_type, fac_name, in_link_id, path, to_node_id, path_seq_num)
(
    select node_id, mesh_code, road_code, seq_num, pos_code, updown_flag, fac_type, fac_name,
           (regexp_split_to_array(Path[1], E'\\\|+'))[1]::bigint as in_link_id,
           (regexp_split_to_array(Path[1], E'\\\|+'))[2:(array_upper(regexp_split_to_array(Path[1], E'\\\|+'), 1))] as path,
           toNode, 1 as path_seq_num 
    from (
    SELECT a.node_id, mesh_code, a.road_code, seq_num, pos_code, updown_flag, 
           kind, fac_type, fac_name, b.node_id as toNode,
           mid_findpath_hwy_two_node(a.node_id, b.node_id, 6) as Path
      FROM road_hwynode as a
      left join mid_road_roadpoint_node as b 
      on a.road_code = b.road_code and seq_num = road_point
      where a.pos_code = 6 and fac_type = 3 
          and (
          (updown_flag = 1 and point_pos in (1, 3, 5) )
          or (updown_flag = 0 and point_pos in (2, 4, 6))
          )
     )  as a
     order by node_id, mesh_code, seq_num, pos_code, path
);
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        # check node是否被遗漏了
        
        return 0
    
    def _MakeICExitFacName(self):
        '''IC设施名称及长路径。'''
        self.log.info('Making IC Exit Facility Name...')
        # 高速道两点之间的路径
        # 如：IC--->IC出口, SA入口-->SA, PS入口--->PA
        self.CreateFunction2('mid_get_facility_name')
        self.CreateFunction2('mid_check_hwy_exit_node_2')
        self.CreateFunction2('mid_check_fac_point_type')
        self.CreateFunction2('mid_findPath_hwy_exit')
        self.CreateFunction2('mid_get_facility_name')

        sqlcmd = """
INSERT INTO mid_fac_name_full_path(
            node_id, mesh_code, road_code, seq_num, pos_code, updown_flag, 
            fac_type, fac_name, in_link_id, path, to_node_id, path_seq_num)
(
select node_id, attr[1]::int as mesh_code, attr[2]::int as road_code, attr[3]::int as seq_num, 
      attr[4]::int as pos_code, attr[5]::int as updown_flag, attr[6]::int as fac_type, attr[7] as fac_name,
      in_link_id, path, toNode, path_seq_num
 from (
    select node_id, mid_get_facility_name(node_id, 2, fac_node, array[6, 8]) as attr, in_link_id, path, fac_node as toNode, path_seq_num
      from ( 
        select node_id,
            (regexp_split_to_array(pathes[path_seq_num], E'\\\|+'))[1]::bigint as in_link_id,
            (regexp_split_to_array(pathes[path_seq_num], E'\\\|+'))[2:(array_upper(regexp_split_to_array(pathes[path_seq_num], E'\\\|+'), 1) - 1)] as path, 
            (regexp_split_to_array(pathes[path_seq_num], E'\\\|+'))[array_upper(regexp_split_to_array(pathes[path_seq_num], E'\\\|+'), 1)]::bigint as fac_node, 
            path_seq_num
          from (
        select node_id, Pathes, generate_series(1, array_upper(Pathes, 1)) as path_seq_num
        from (
            select node_id, mid_findPath_hwy_exit(node_id, array[6, 8]) as Pathes
              from (
                select distinct node_id
                  from road_hwynode
                  where pos_code = 2
              ) as a
         ) as b
      ) as c
    ) as d
  ) as e
  order by node_id, mesh_code, seq_num, pos_code, path
);
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        return 0
    
    def _MakeICEnterFacName(self):
        '''IC入口设施名称及长路径。'''
        self.log.info('Making IC Enter Facility Name...')
        # 高速入口前的一条link，到高速主路
        # 如有inner link, 要再往前推
        self.CreateFunction2('mid_get_intersect_node')    # 两条link的相交node
        self.CreateFunction2('mid_is_hwy_enter')          # 高速入口
        self.CreateFunction2('mid_get_enter_name')        # 高速名称
        self.CreateFunction2('mid_check_fac_point_type')    # 判断是不是入口设施所在的点
        self.CreateFunction2('mid_findpath_hwy_enter_2')  
        
        sqlcmd = """
insert into mid_fac_name_full_path(node_id, mesh_code, road_code, seq_num, pos_code, updown_flag, 
             fac_type, fac_name, in_link_id, path, to_node_id, path_seq_num)
(
select node_id, attr[1]::int as mesh_code, attr[2]::int as road_code, attr[3]::int as seq_num, 
      attr[4]::int as pos_code, attr[5]::int as updown_flag, attr[6]::int as fac_type, attr[7] as fac_name,
      in_link_id, path, toNode, path_seq_num
 from (
    select mid_get_intersect_node(in_link_id, path[1]::bigint) as node_id,
        mid_get_enter_name(EnterNode, toNode) as attr,
        in_link_id, path, toNode, path_seq_num
      from (
        select (regexp_split_to_array(pathes[path_seq_num], E'\\\|+'))[array_upper(regexp_split_to_array(pathes[path_seq_num], E'\\\|+'), 1)]::bigint as EnterNode,
               (regexp_split_to_array(Pathes[path_seq_num], E'\\\|+'))[1]::bigint as in_link_id,
               (regexp_split_to_array(Pathes[path_seq_num], E'\\\|+'))[2:(array_upper(regexp_split_to_array(Pathes[path_seq_num], E'\\\|+'), 1) - 1)] as path,
               toNode, path_seq_num
          from (
            select toNode, Pathes, generate_series(1, array_upper(Pathes, 1)) as path_seq_num
              from (
                select node_id as toNode, mid_findpath_hwy_enter_2(node_id, array[5, 7]) as pathes
                  from (
                    SELECT distinct node_id
                      FROM road_hwynode
                      where pos_code = 1
                  ) as a
             ) as b
        ) as c
      ) as d
  ) as e
  order by node_id, mesh_code, seq_num, pos_code, path
);
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return 0
    
    def _MakeSmartICExitName(self):
        """插入[スマートIC出口]信息"""
        self.log.info('Making SMart IC Exit Facility Name...')
        self.CreateFunction2('mid_check_fac_point_type')
        self.CreateFunction2('mid_get_intersect_node')
        self.CreateFunction2('mid_check_hwy_exit_node_2')
        self.CreateFunction2('mid_get_facility_name')
        self.CreateFunction2('mid_findPath_hwy_exit')
        
        sqlcmd = """
INSERT INTO mid_fac_name_full_path(
            node_id, mesh_code, road_code, seq_num, pos_code, updown_flag, 
            fac_type, fac_name, in_link_id, path, to_node_id, path_seq_num)
(
select node_id, attr[1]::int as mesh_code, attr[2]::int as road_code, attr[3]::int as seq_num, 
      attr[4]::int as pos_code, attr[5]::int as updown_flag, attr[6]::int as fac_type, attr[7] as fac_name,
      in_link_id, path, toNode, path_seq_num
 from (
    select node_id, mid_get_facility_name(node_id, 14, fac_node, array[11]) as attr, in_link_id, path, fac_node as toNode, path_seq_num
      from ( 
        select node_id,
            (regexp_split_to_array(pathes[path_seq_num], E'\\\|+'))[1]::bigint as in_link_id,
            (regexp_split_to_array(pathes[path_seq_num], E'\\\|+'))[2:(array_upper(regexp_split_to_array(pathes[path_seq_num], E'\\\|+'), 1) - 1)] as path, 
            (regexp_split_to_array(pathes[path_seq_num], E'\\\|+'))[array_upper(regexp_split_to_array(pathes[path_seq_num], E'\\\|+'), 1)]::bigint as fac_node, 
            path_seq_num
          from (
        select node_id, Pathes, generate_series(1, array_upper(Pathes, 1)) as path_seq_num
        from (
            select node_id, mid_findPath_hwy_exit(node_id, array[11]) as Pathes
              from (
                select distinct node_id
                  from road_hwynode
                  where pos_code = 14
                  ---group by node_id 
                --having count(node_id) > 1
              ) as a
         ) as b
      ) as c
    ) as d
  ) as e
  order by node_id, mesh_code, seq_num, pos_code, path
);
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        
        
        return 0
    
    def _MakeSmartICEnterName(self):
        '''Smart IC 入口'''
        self.log.info('Making SMart IC Enter Facility Name...')
        self.CreateFunction2('mid_get_fac_name_passlid')
        self.CreateFunction2('mid_check_fac_point_type')
        self.CreateFunction2('mid_get_intersect_node')
        self.CreateFunction2('mid_findpath_hwy_enter_2')
        
        sqlcmd = """
insert into mid_fac_name_full_path(node_id, mesh_code, road_code, seq_num, pos_code, updown_flag, 
             fac_type, fac_name, in_link_id, path, to_node_id, path_seq_num)
(
select node_id, attr[1]::int as mesh_code, attr[2]::int as road_code, attr[3]::int as seq_num, 
      attr[4]::int as pos_code, attr[5]::int as updown_flag, attr[6]::int as fac_type, attr[7] as fac_name,
      in_link_id, path, toNode, path_seq_num
 from (
    select mid_get_intersect_node(in_link_id, path[1]::bigint) as node_id,
        mid_get_facility_name(toNode, 13, fac_node, array[10]) as attr,
        in_link_id, path, toNode, path_seq_num
      from (
        select (regexp_split_to_array(pathes[path_seq_num], E'\\\|+'))[array_upper(regexp_split_to_array(pathes[path_seq_num], E'\\\|+'), 1)]::bigint as fac_node,
               (regexp_split_to_array(Pathes[path_seq_num], E'\\\|+'))[1]::bigint as in_link_id,
               (regexp_split_to_array(Pathes[path_seq_num], E'\\\|+'))[2:(array_upper(regexp_split_to_array(Pathes[path_seq_num], E'\\\|+'), 1) - 1)] as path,
               toNode, path_seq_num
          from (
            select toNode, Pathes, generate_series(1, array_upper(Pathes, 1)) as path_seq_num
              from (
                select node_id as toNode, mid_findpath_hwy_enter_2(node_id, array[10]) as pathes
                  from (
                    SELECT distinct node_id
                      FROM road_hwynode
                      where pos_code = 13
                  ) as a
             ) as b
        ) as c
      ) as d
  ) as e
  order by node_id, mesh_code, seq_num, pos_code, path
);
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
    def _MakeFacNamePassLink(self):
        '''[长路径]变成[短路径]。'''
        self.log.info('Short Cut the Facility Name Path...')
        
        self.CreateTable2('mid_fac_name')
        # 把长的Path Link，修成短的。
        # 规则是：1. 修短到无分支处 。2. 如果各个分支名称和类型相同，就继续继短
        # 注：如果Smart IC名和其他名同时存在时，Smart IC名不做。
        sqlcmd = """
INSERT INTO mid_fac_name(
            id, nodeid, inlinkid, outlinkid, passlid, passlink_cnt, 
            pos_code, fac_type, fac_name, tonode_array)
(
-- JCT, SA, PA, IC Enter, IC Exit
SELECT IDs, node_id, in_link_id, 
    (regexp_split_to_array(passlid, E'\\\|+'))[link_num]::bigint as outlinkid,
    (case when link_num = 1 then NULL else 
    array_to_string((regexp_split_to_array(passlid, E'\\\|+'))[1:(link_num - 1)], '|') end) as new_passlid,
    (link_num - 1) as passlink_cnt,
    pos_code, fac_type, fac_name, tonode_array
  from (
    select array_agg(gid) as IDs, node_id, in_link_id, passlid, pos_code, fac_type, fac_name, 
          array_upper(regexp_split_to_array(passlid, E'\\\|+'), 1) as link_num, array_agg(to_node_id) as tonode_array
    from (
        SELECT gid, node_id, mesh_code, road_code, seq_num, pos_code, updown_flag, 
               fac_type, fac_name, in_link_id, path, to_node_id, path_seq_num,
               mid_get_fac_name_passlid(gid, in_link_id, node_id::bigint, path, to_node_id, pos_code::smallint, fac_name) as passlid
          FROM mid_fac_name_full_path
          order by gid
      ) as a
    group by node_id, in_link_id, passlid, pos_code, fac_type, fac_name
  ) as b
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
INSERT INTO mid_fac_name(
            nodeid, inlinkid, outlinkid, passlid, passlink_cnt, 
            pos_code, fac_type, fac_name, tonode_array)
(
    select node_id, new_inlink_id, outlinkid, new_passlid, new_passlink_cn,
           pos_code, fac_type, fac_name, tonode_array
    from (
        select min(gid) as gid, node_id, new_inlink_id, outlinkid, new_passlid, new_passlink_cn,
              pos_code, fac_type, fac_name, tonode_array
        from (
            select gid, id, 
                 mid_get_intersect_node(new_inlink_id, case when new_passlink_cn > 0 then 
                          (new_passlid[1])::bigint else outlinkid end) as node_id,
                  new_inlink_id, outlinkid, 
                  array_to_string(new_passlid, '|') as new_passlid, new_passlink_cn,
                  pos_code, fac_type, fac_name, tonode_array
              from (
                select b.gid, id, outlinkid, passlink_cnt, 
                       pos_code, fac_type, fac_name, tonode_array,
                      (regexp_split_to_array(passlid, E'\\\|+'))[(passlink_seq_num)]::bigint as new_inlink_id,
                      (regexp_split_to_array(passlid, E'\\\|+'))[(passlink_seq_num + 1) : passlink_cnt] as new_passlid,
                      (passlink_cnt - passlink_seq_num ) as new_passlink_cn
                from (
                    select a.gid, id, nodeid, inlinkid, outlinkid, passlid, passlink_cnt, 
                           pos_code, fac_type, fac_name, tonode_array,
                           (regexp_split_to_array(passlid, E'\\\|+'))[passlink_seq_num]::bigint as pass_link_id,
                           passlink_seq_num
                    from (
                        SELECT gid, id, nodeid, inlinkid, outlinkid, passlid, passlink_cnt, 
                               pos_code, fac_type, fac_name, tonode_array, 
                               generate_series(1, passlink_cnt) as passlink_seq_num
                          FROM mid_fac_name
                          where pos_code = 1 and passlid is not null and passlid <> ''
                      ) as a
                ) as b
                left join road_rlk
                on pass_link_id = objectid
                where rdb_cnv_link_type(linkcls_c) = 4
              ) as c
        ) as e
        group by node_id, new_inlink_id, outlinkid, new_passlid, new_passlink_cn, pos_code, fac_type, fac_name, tonode_array
    ) as f
    order by gid, new_passlink_cn desc
);
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return 0
    
    def _MakeHwyTollNodeName(self):
        '高速收费站名称。'
        self.log.info('Make Highway Toll Node Name Dictionary.')
        sqlcmd = """
            SELECT rdb_insert_name_dictionary('mid_road_roadpoint_node', 
                                            'node_id', 
                                            'fac_name', 
                                             null, 
                                            'temp_node_name',
                                            'fac_type = 9');
        """
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else :
            self.pg.commit2()
        
        return 0
    
    def _MakeFacNameDict(self):
        """做Facility Name的字典。"""
        self.log.info('Making Facility Name Dictionary...')
        self.CreateTable2('mid_temp_fac_name_id')
        self.CreateFunction2('mid_insert_name_dictionary_fac_name')
        
        self.pg.callproc('mid_insert_name_dictionary_fac_name')
        
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
                            direction, guideattr, namekind, name_id, "type")
                (
                SELECT gid, node_id, fromlinkid, tolinkid, passlid,
                       (CASE WHEN passlid is null THEN 0 ELSE array_upper(regexp_split_to_array(passlid, E'\\\|+'), 1) END) as passlink_cnt,
                        0, mid_convert_fac_name_guideattr(fac_type) as guideattr, 
                        2 as namekind, new_name_id, 1
                FROM (
                    SELECT foo.gid, A.tile_link_id as fromlinkid, 
                            B.tile_node_id as node_id, 
                            C.tile_link_id as tolinkid, 
                           mid_get_new_passlid(passlid, outlinkid::integer, '|')  as passlid,
                           from_nodeid, pos_code, fac_type, new_name_id
                        from (
                            SELECT gid, id, nodeid, inlinkid, outlinkid, passlid,
                                mid_get_intersect_node((case when passlink_cnt = 0 then inlinkid 
                                        else (regexp_split_to_array(passlid, E'\\\|+'))[passlink_cnt]::bigint end
                                        ), outlinkid) as from_nodeid,
                                pos_code, fac_type, fac_name
                            FROM mid_fac_name
                      ) as foo
                      LEFT JOIN mid_temp_fac_name_id
                      ON foo.gid = mid_temp_fac_name_id.gid
                      LEFT JOIN middle_tile_link AS A
                      ON inlinkid = A.old_link_id and nodeid = Any(A.old_s_e_node)
                      LEFT JOIN middle_tile_node AS B
                      ON nodeid = B.old_node_id
                      LEFT JOIN middle_tile_link AS C
                      ON outlinkid = C.old_link_id and from_nodeid = Any(C.old_s_e_node)
                      order by foo.gid
                     ) AS d
                );
            """
        self.pg.execute2(sqlcmd) 
        self.pg.commit2()
        
        return 0
    
    def _CreateView(self):
        return 0
        self.CreateIndex2('name_dictionary_tbl_name_id_idx')
        
        sqlcmd = """
        create or replace view view_towardname_fac_name
        as 
        (
        SELECT towardname_tbl.gid, id, nodeid, inlinkid, outlinkid, passlid, passlink_cnt, 
               direction, guideattr, namekind, towardname_tbl.name_id, "type", name, the_geom
          FROM towardname_tbl
          left join node_tbl
          on nodeid = node_id
          left join name_dictionary_tbl
          on towardname_tbl.name_id = name_dictionary_tbl.name_id
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
        self._DoCheckSmartICExit()
        self._DoCheckSmartICEnter()
        return 0
    
    def _DoCheckJCT(self):
        # jct 
        sqlcmd = """
            select count(node_id)
              from road_hwynode
              where pos_code = 8 and node_id not in (
                select distinct nodeid
                  from mid_fac_name
                  where pos_code = 8
              )
          """
        rst = self._CheckResult(sqlcmd)
        if rst > 0:
            self.log.warning('There are %d JCT, not included.' % rst)
        return 0
    
    def _DoCheckSA(self):
        # sa
        sqlcmd = """
            select count(node_id)
              from road_hwynode
              where pos_code = 4 and node_id not in (
                select distinct nodeid
                  from mid_fac_name
                  where pos_code = 4
              )
          """
        rst = self._CheckResult(sqlcmd)
        if rst > 0:
            self.log.warning('There are %d SA, not included.' % rst)
        return 0 
     
    def _DoCheckPA(self):
        # PA
        sqlcmd = """
            select count(node_id)
              from road_hwynode
              where pos_code = 6 and node_id not in (
                select distinct nodeid
                  from mid_fac_name
                  where pos_code = 6
              )
            """
        rst = self._CheckResult(sqlcmd)
        if rst > 0:
            self.log.warning('There are %d PA, not included.' % rst)
        return 0
    
    def _DoCheckICExit(self):
        # IC Exit
        sqlcmd = """
        select count(node_id)
          from road_hwynode
          where pos_code = 2 and node_id not in (
            select distinct nodeid
              from mid_fac_name
              where pos_code = 2
          )
          """
        rst = self._CheckResult(sqlcmd)
        if rst > 0:
            self.log.warning('There are %d IC Exit, not included.' % rst)
        return 0
    
    def _DoCheckICEnter(self):
        # -- IC enter (node_id: 4358708/12318246)
        sqlcmd = """
            select count(node_id)
              from road_hwynode
              where pos_code = 1 and node_id not in (
                select distinct tonode_array[seq_num]
                  from (
                    select tonode_array, generate_series(1, array_upper(tonode_array, 1)) as seq_num
                      from mid_fac_name
                      where pos_code = 1
                ) as a
              )
            """
        rst = self._CheckResult(sqlcmd)
        if rst > 0:
            self.log.warning('There are %d IC Enter, not included.' % rst)
        return 0

    def _DoCheckSmartICExit(self):
        # Smart IC Exit
        sqlcmd = """
            select count(node_id)
              from road_hwynode
              where pos_code = 14 and node_id not in (
                select distinct nodeid
                  from mid_fac_name
                 -- where pos_code = 14
              )
            """
        rst = self._CheckResult(sqlcmd)
        if rst > 0:
            self.log.warning('There are %d Smart IC Exit, not included.' % rst)
        return 0
    
    def _DoCheckSmartICEnter(self):
        # Smart IC Enter
        sqlcmd = """
                select count(node_id)
                  from road_hwynode
                  where pos_code = 13 and node_id not in (
                    select distinct tonode_array[seq_num]
                      from (
                        select tonode_array, generate_series(1, array_upper(tonode_array, 1)) as seq_num
                          from mid_fac_name
                          where pos_code = 13
                    ) as a
                  )
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
        
    

    