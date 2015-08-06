# -*- coding: UTF-8 -*-
'''
Created on 2015-7-13

@author: PC_ZH
'''
from component.rdf.hwy.hwy_facility_rdf import HwyFacilityRDF
import json
import common
import component.component_base
from common import cache_file
from component.rdf.multi_lang_name_rdf import MultiLangNameRDF
from component.rdf.hwy.hwy_graph_rdf import is_cycle_path
from component.rdf.hwy.hwy_graph_rdf import HWY_ROAD_CODE
from component.rdf.hwy.hwy_def_rdf import HWY_INOUT_TYPE_IN
from component.rdf.hwy.hwy_def_rdf import HWY_INOUT_TYPE_OUT
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_IC
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_JCT
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_SA
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_PA
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_TOLL
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_UTURN
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_SERVICE_ROAD
from component.rdf.hwy.hwy_def_rdf import HWY_TRUE
from component.rdf.hwy.hwy_def_rdf import HWY_FALSE
from component.rdf.hwy.hwy_def_rdf import HWY_UPDOWN_TYPE_UP
from component.jdb.hwy.hwy_data_mng import HwyFacilInfo
from component.rdf.hwy.hwy_facility_rdf import ROAD_SEQ_MARGIN
SAPA_TYPE_DICT = {1: HWY_IC_TYPE_SA,  # COMPLETE REST AREA
                  2: HWY_IC_TYPE_PA,  # PARKING AND REST ROOM ONLY
                  3: HWY_IC_TYPE_PA,  # PARKING ONLY
                  4: HWY_IC_TYPE_SA,  # MOTORWAY SERVICE AREA
                  5: None             # SCENIC OVERLOOK
                  }
ZN_IC_TYPE_IC_JCT = 2
ZN_IC_TYPE_SAPA = 3
HWY_PATH_TYPE_IC = 'IC'
HWY_PATH_TYPE_SAPA = 'SAPA'
HWY_PATH_TYPE_JCT = 'JCT'
HWY_PATH_TYPE_UTURN = 'UTURN'
IC_PATH_TYPE_DICT = {HWY_IC_TYPE_IC: HWY_PATH_TYPE_IC,
                     HWY_IC_TYPE_SA: HWY_PATH_TYPE_SAPA,
                     HWY_IC_TYPE_PA: HWY_PATH_TYPE_SAPA,
                     HWY_IC_TYPE_JCT: HWY_PATH_TYPE_JCT,
                     HWY_IC_TYPE_UTURN: HWY_PATH_TYPE_UTURN,
                     HWY_IC_TYPE_TOLL: '',
                     }


class HwyFacilityZenrin(HwyFacilityRDF):
    '''生成设施情报 '''
    def __init__(self, data_mng, name='HwyFacilityZenrin'):
        '''初始化 变量   对象'''
        HwyFacilityRDF.__init__(self, data_mng, ItemName=name)
        self.G = None
        self.hwy_data = data_mng
        if self.hwy_data:
            self.G = self.hwy_data.get_graph()
        self.org_facil_dict = {}  # (org_facil_id, facil_cls): road_seq
        self.org_node_dict = {}  # (node, facil_cls):facility_id

    def _Do(self):
        self._make_ic_path()
        # 过滤假JCT/UTurn:下了高速又转弯回来的径路
        self._filter_JCT_UTurn()
        # 过滤SAPA: 其实只是JCT
        self._filter_sapa()
        # 过滤辅路、类辅路， 从辅路路径选出SAPA路径(路径两头都有SAPA设施)
        self._filter_service_road()
        # SAPA对应的Rest Area POI情报
#         self._make_sapa_info()
        self._deal_with_uturn()  # 处理U-Turn
        # 148.11217,-35.00432
        # self._del_sapa_node()
        self._make_facil_name()  # 设施名称
        self._make_hwy_node()
        self._make_facil_same_info()  # 并设情报
#         self._make_hwy_store()  # 店铺情报
        self._make_hwy_service()  # 服务情报

    def _make_sapa_info(self):
        return 0
    #zenrin 源数据已经提供了pntname 不需要单独做sapa_name
        self.log.info('Start make sapa info')
        self.CreateTable1('mid_temp_hwy_sapa_info')
        sqlcmd = '''
        INSERT INTO mid_temp_hwy_sapa_info(road_code, updown_c,
                                           road_seq, poi_id,
                                           facilcls_c, sapa_name)
        (
           SELECT distinct road_code, updown_c,
                  road_seq, pntid, facilcls_c, c.pntname
            FROM
            (
               select distinct a.road_code, a.road_seq, a.updown_c,
                      a.facilcls_c, a.node_id , a.inout_c, b.facility_id
               from(
                   select *
                   from  mid_temp_hwy_ic_path
                   where facilcls_c in (1,2) and
                         link_lid is not null
                ) as a
               left join mid_hwy_org_facility_node as b
               on a.road_code = b.path_id and a.node_id = b.node_id
               order by a.road_code, a.road_seq, a.updown_c
            ) as facility
            RIGHT JOIN
            (
                select *
                from org_highwaypoint
                where  pnttype = 3
            )as c
            ON facility.road_code= c.pathid and
               facility.facility_id = c.pntid
            WHERE facility.road_code is not  null
            ORDER BY road_code, updown_c
        )
        '''
        self.pg.execute1(sqlcmd)
        self.pg.commit1()
        self.log.info('End make sapa info')

    def _make_ic_path(self):
        self.log.info('Start make IC Path.')
        self.pg.connect1()
        self.CreateTable2('mid_temp_hwy_ic_path')
        self.CreateTable2('mid_temp_hwy_service_road_path1')
        temp_file_obj = common.cache_file.open('mid_temp_hwy_ic_path')
        for road_code, updown, route_path in self.hwy_data.get_road_code_path():
            self.org_facil_dict = {}  # 清空
            next_seq = ROAD_SEQ_MARGIN
            end_pos = len(route_path)
            if is_cycle_path(route_path):
                if not self.adjust_path_for_cycle(route_path):
                    self.log.error('No indicate start node for Cycle Route.'
                                   'road_code=%s' % road_code)
                    continue
                else:
                    self._store_moved_path(road_code, route_path)
                end_pos = len(route_path) - 1
            path = route_path[:end_pos]
            # 取得线路上所有设施
            all_facils = self._get_facils_for_path(road_code, path)
            facils_list, sapa_node_dict, service_road_list = all_facils
            # ## 设置设施序号
            sapa_seq_dict = {}  # SAPA node: road_seq
            road_seq_dict = {}  # IC/JCT/SAPA (facil_c, inout_c):road_seq
            for node, all_facils in facils_list:
                road_seq_dict, next_seq = self._get_road_seq(node,
                                                             all_facils,
                                                             next_seq,
                                                             sapa_node_dict,
                                                             sapa_seq_dict,
                                                             )
                for facilcls, inout_c, path in all_facils:
                    road_seq = road_seq_dict.get((facilcls, inout_c))
                    if not road_seq:
                        self.log.error('No Road Seq.')
                        continue
                    if not path:
                        self.log.error('No Path. node=%s' % node)
                        continue
                    path_type = IC_PATH_TYPE_DICT.get(facilcls)
                    self._store_facil_path_2_file(road_code, road_seq,
                                                  facilcls, inout_c,
                                                  updown, path, path_type,
                                                  temp_file_obj)
            # 辅路、类辅路设施
            for sr_facil_info in service_road_list:
                inout_c, path = sr_facil_info[1], sr_facil_info[2]
                # 保存到辅路到数库
                self._store_service_road_facil_path(inout_c, path,
                                                    road_code, updown)
        # ## 辅路提交到数据库
        self.pg.commit1()
        # ## 保存设施路径到数据库
        temp_file_obj.seek(0)
        self.pg.copy_from2(temp_file_obj,
                           'mid_temp_hwy_ic_path',
                           columns=('road_code', 'road_seq', 'facilcls_c',
                                    'inout_c', 'updown_c', 'node_id',
                                     'to_node_id', 'node_lid', 'link_lid',
                                     'path_type'
                                    ),
                           )
        self.pg.commit2()
        common.cache_file.close(temp_file_obj, True)
        # 创建索引
        self.CreateIndex2('mid_temp_hwy_ic_path_path_type_idx')
        self.CreateIndex2('mid_temp_hwy_ic_path_road_code_'
                          'road_seq_node_id_inout_c_fac_idx')
        self.log.info('End make IC Path.')

    def _filter_service_road(self):
        '''过滤辅路、类辅路， 从辅路路径选出SAPA路径(路径两头都有SAPA设施)'''
        self.CreateTable2('mid_temp_hwy_service_road_path2')
        self.pg.connect1()
        for sr_info in self._get_service_road():
            road_code, inout = sr_info[0:2]
            f_road_seqs, f_facilcls_cs = sr_info[2:4]
            t_road_seqs, t_facilcls_cs = sr_info[4:6]
            node_lid, link_lid = sr_info[6:8]
            updown = sr_info[8]
            node_lid = eval(node_lid + ',')
            link_lid = eval(link_lid + ',')
            node_id = node_lid[0]
            to_node_id = node_lid[-1]
            comm_seqs = self._get_comm_road_seq(f_road_seqs, f_facilcls_cs,
                                                t_road_seqs, t_facilcls_cs)
            # 两头都有IC设施
            if HWY_IC_TYPE_IC in comm_seqs:
                # ## 辅路、类辅路其实只是出入口(辅路路径link是出入口路径link的合集子集)
                link_set = set(link_lid)
                # 取得起点，IC路径link集合
                f_link_set = self._get_ic_path_link_set(road_code, node_id,
                                                        inout, f_road_seqs,
                                                        f_facilcls_cs, updown)
                if not f_link_set:
                    self.log.error('No From Link Set. node=%,to_node=%s'
                                   % (node_id, to_node_id))
                # 取得终点，IC路径link集合
                t_inout = HWY_INOUT_TYPE_IN
                if inout == HWY_INOUT_TYPE_IN:
                    t_inout = HWY_INOUT_TYPE_OUT
                t_link_set = self._get_ic_path_link_set(road_code, to_node_id,
                                                        t_inout, t_road_seqs,
                                                        t_facilcls_cs, updown)
                if not t_link_set:
                    self.log.error('No To Link Set. node=%,to_node=%s'
                                   % (node_id, to_node_id))
                # 辅路路径link是出入口路径link的合集子集
                if link_set.issubset(f_link_set.union(t_link_set)):
                    # print link_lid
                    continue
            # 如果两头有SAPA设施，那么该路径就是SAPA路径
            if(HWY_IC_TYPE_SA in comm_seqs or HWY_IC_TYPE_PA in comm_seqs):
                if HWY_IC_TYPE_SA in comm_seqs:
                    f_seq_list, t_seq_list = comm_seqs[HWY_IC_TYPE_SA]
                    facilcls_c = HWY_IC_TYPE_SA
                else:
                    f_seq_list, t_seq_list = comm_seqs[HWY_IC_TYPE_PA]
                    facilcls_c = HWY_IC_TYPE_PA
                if len(f_seq_list) != 1 and len(t_seq_list) != 1:
                    self.log.error('SAPA road seq > 1. node=%s,to_node=%s'
                                   % (node_id, to_node_id))
                elif f_seq_list[0] != t_seq_list[0]:  # 非同一个SAPA设施
                    self.log.warning('SAPA f_road_seq != t_road_seq.'
                                     'node_id=%s, to_node_id=%s'
                                     % (node_id, to_node_id))
                else:
                    path_type = IC_PATH_TYPE_DICT.get(facilcls_c)
                    self._store_facil_path(road_code, f_seq_list[0],
                                           facilcls_c, inout,
                                           node_lid, path_type,
                                           updown)
                    continue
            # Service
            self._store_service_road_facil_path2(inout, node_lid,
                                                 road_code, updown)
        pass

    def _get_service_road(self):
        sqlcmd = '''
        SELECT a.road_code, a.inout_c,
               f_road_seqs, f_facilcls_cs,
               t_road_seqs, t_facilcls_cs,
               a.node_lid, a.link_lid, a.updown_c
          FROM mid_temp_hwy_service_road_path1 as a
          LEFT JOIN (
            SELECT node_id, road_code, inout_c,
                   array_agg(road_seq) as f_road_seqs,
                   array_agg(facilcls_c) as f_facilcls_cs
              FROM (
                  SELECT DISTINCT node_id, road_code, inout_c,
                       road_seq, facilcls_c
                  FROM mid_temp_hwy_ic_path
                  ORDER BY  node_id, road_code, road_seq, facilcls_c
                  ) AS f
              where facilcls_c not in (10) -- Not u-turn
              group by node_id, road_code, inout_c
          ) as b
          ON a.node_id = b.node_id and
             a.road_code = b.road_code and
             a.inout_c = b.inout_c
          left join (
            SELECT node_id, road_code, inout_c,
                   array_agg(road_seq) as t_road_seqs,
                   array_agg(facilcls_c) as t_facilcls_cs
              FROM (
                SELECT DISTINCT node_id, road_code, inout_c,
                       road_seq, facilcls_c
                  FROM mid_temp_hwy_ic_path
                  ORDER BY  node_id, road_code, road_seq, facilcls_c
              ) AS t
              where facilcls_c not in (10) -- Not u-turn
              group by node_id, road_code, inout_c
          ) as c
          ON a.to_node_id = c.node_id and
             a.road_code = c.road_code and
             (a.inout_c = 1 and c.inout_c = 2 or
              a.inout_c = 2 and c.inout_c = 1)
        '''
        return self.get_batch_data(sqlcmd)

    def _get_ic_path_link_set(self, roadcode, node, inout,
                              road_seqs, facilcls_cs, updown):
        facil_name = ''
#         updown = HWY_UPDOWN_TYPE_UP
        link_set = set()
        for road_seq, facilcls in zip(road_seqs, facilcls_cs):
            if facilcls != HWY_IC_TYPE_IC:
                continue
            facil = HwyFacilInfo(roadcode, road_seq, facilcls,
                                 updown, node, inout, facil_name)
            path_infos = self.hwy_data.get_ic_path_info(facil)
            for path_info in path_infos:
                path_type = path_info[2]
                if path_type == HWY_PATH_TYPE_IC:
                    link_lid = path_info[1]
                    link_set = link_set.union(set(link_lid))
        return link_set

    def _store_service_road_facil_path(self, inout_c, path,
                                       road_code, updown_c):
        '''辅路、类辅路设施'''
        sqlcmd = """
        INSERT INTO mid_temp_hwy_service_road_path1(
                                              inout_c, node_id,
                                              to_node_id, node_lid,
                                              link_lid, road_code,
                                              updown_c)
           VALUES(%s, %s, %s,
                  %s, %s, %s, %s)
        """
        node_id = path[0]
        to_node_id = path[-1]
        node_lid = ','.join([str(node) for node in path])
        if inout_c == HWY_INOUT_TYPE_OUT:
            link_list = self.G.get_linkids(path)
        elif inout_c == HWY_INOUT_TYPE_IN:
            link_list = self.G.get_linkids(path, reverse=True)
        else:
            link_list = []
        link_lid = ','.join([str(link) for link in link_list])
        params = (inout_c, node_id, to_node_id,
                  node_lid, link_lid, road_code, updown_c)
        self.pg.execute1(sqlcmd, params)

    def _store_facil_path_2_file(self, road_code, road_seq, facilcls_c,
                                 inout_c, updown, path, path_type, file_obj):
        '''保存设施路径到文件'''
        node_id = path[0]
        to_node_id = path[-1]
        node_lid = ','.join([str(node) for node in path])
        if inout_c == HWY_INOUT_TYPE_OUT:
            link_list = self.G.get_linkids(path)
        elif inout_c == HWY_INOUT_TYPE_IN:
            link_list = self.G.get_linkids(path, reverse=True)
        else:
            link_list = []
        link_lid = ','.join([str(link) for link in link_list])
        if not link_lid:
            link_lid = ''
        params = (road_code, road_seq, facilcls_c, inout_c, updown,
                  node_id, to_node_id, node_lid, link_lid, path_type)
        file_obj.write('%d\t%d\t%d\t'
                       '%d\t%d\t%d\t'
                       '%d\t%s\t%s\t%s\n' % params)

    def _make_facil_name(self):
        '''制作设施出口名称'''
        self.log.info('Start Make Facil Name.')
        self.pg.connect1()
        self.CreateTable2('mid_temp_hwy_facil_name')
        sqlcmd = """
         select road_code, updown_c, road_seq, inout_c,
                node_id, pnttype, array_agg(pntname) as facil_name
         from
         (
            SELECT distinct a.road_code, a.updown_c, a.road_seq,
                   a.inout_c, a.node_id , a.pnttype, facility_id, pntname
            FROM
            (
             select distinct road_code, updown_c, road_seq, node_id, inout_c,
                   case
                     when  path_type = 'IC' then 2
                     when  path_type = 'JCT' then 2
                     when  path_type = 'SAPA' then 3
                     else  -1
                   end as pnttype
             from mid_temp_hwy_ic_path
            ) as a
            LEFT JOIN
            (
              select road_code, dirflag, node_id,
                    facility_id, pnttype ,pntname
              from mid_hwy_org_facility_node as node
              left join road_code_info as info
              on node.path_id = info.path_id
              LEFT JOIN org_highwaypoint as c
              ON node.path_id = c.pathid and
                 node.facility_id = c.pntid and
                 ((dirflag = 2 and c.positivefl = 1)or
                  (dirflag = 1 and c.negativefl = 1))
            ) as b
            ON a.road_code = b.road_code and
               a.node_id = b.node_id and
               a.updown_c = b.dirflag and
               a.pnttype = b.pnttype
            ORDER BY road_code, updown_c, road_seq,
                     node_id, facility_id, pnttype
           )as facility
           group by road_code, updown_c, road_seq, inout_c, node_id, pnttype
           order by road_code, updown_c, road_seq, inout_c
        """
        for row in self.pg.get_batch_data2(sqlcmd):
            (road_code, updown_c, road_seq, inout_c,
             node_id, pnttype, facil_names) = row
            if set(facil_names) == set([None]):
                if(pnttype == ZN_IC_TYPE_SAPA and
                   inout_c == HWY_INOUT_TYPE_IN):  # SAPA in
                    if self._exist_sapa_out(road_code,
                                               updown_c,
                                               road_seq):
                        continue
                    else:
                        self.log.error('only enter in SAPA')
                elif(pnttype == ZN_IC_TYPE_IC_JCT and
                     inout_c == HWY_INOUT_TYPE_IN):
                    pass
                else:
                    facil_names = self._get_facil_name(road_code,
                                                       updown_c,
                                                       pnttype,
                                                       node_id)
            multi_name = None
            json_name = None
            for name in facil_names:
                if name:
                    temp_multi_name = MultiLangNameRDF('CHT', name)
                    if multi_name:
                        multi_name.add_alter(temp_multi_name)
                    else:
                        multi_name = temp_multi_name
            if multi_name:
                if facil_names.count(None) > 0:
                    self.log.error('NULL Name in the facil_names. node=%s'
                                   % node_id)
                json_name = multi_name.json_format_dump()
            if not json_name:
                json_name = None
            self._insert_facil_name(road_code, updown_c, road_seq, json_name)
        self.pg.execute1(sqlcmd)
        self.pg.commit1()
#         self.CreateIndex2('mid_temp_hwy_facil_name_road_code_road_seq_idx')
        self.log.info('End Make Facility Name.')

    def _exist_sapa_out(self, road_code, updown_c, road_seq):
        sqlcmd = '''
        select *
        from mid_temp_hwy_ic_path
        where road_code = %s and updown_c = %s and
              inout_c = 2 and facilcls_c in(1, 2) and road_seq = %s
        '''
        self.pg.execute1(sqlcmd, (road_code, updown_c, road_seq))
        row = self.pg.fetchone()
        if row:
            return True
        return False

    def _insert_facil_name(self, road_code, updown_c,
                           road_seq, facil_name):
        ''' '''
        sqlcmd = '''
        INSERT INTO mid_temp_hwy_facil_name(road_code, updown_c,
                                            road_seq, facil_name)
           VALUES(%s, %s, %s, %s)
        '''
        self.pg.execute1(sqlcmd, (road_code, updown_c, road_seq, facil_name))
        return 0

    def _make_hwy_node(self):
        self.log.info('Start Make Highway Node')
        self.CreateTable2('hwy_node')
        sqlcmd = """
        CREATE UNIQUE INDEX hwy_node_road_code_road_seq_node_id_idx
          ON hwy_node
          USING btree
          (road_code, updown_c, road_seq, node_id);
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        sqlcmd = """
        SELECT distinct a.road_code , a.updown_c ,a.road_seq ,
               a.facilcls_c, a.inout_c, 0 tollcls_c,
               a.node_id, b.facil_name, c.the_geom
        FROM mid_temp_hwy_ic_path as a
        LEFT JOIN mid_temp_hwy_facil_name as b
        ON  a.road_code = b.road_code and
            a.road_seq = b.road_seq and
            a.updown_c = b.updown_c
        LEFT JOIN node_tbl as c
        ON a.node_id = c.node_id
        order by a.road_code, a.updown_c, a.road_seq
        """
        self.pg.connect1()
        for row in self.pg.get_batch_data2(sqlcmd):
            (road_code, updown_c, road_seq, facilcls_c,
             inout_c, tollcls_c, node_id, facil_name, the_geom) = row
            param = (road_code, updown_c, road_seq,
                     facilcls_c, inout_c, tollcls_c,
                     node_id, facil_name, the_geom)

            self._insert_hwy_node(param)
        self.pg.commit1()
#         self.CreateIndex2('hwy_node_the_geom_idx')
#         self.CreateIndex2('hwy_node_road_code_road_seq'
#                           '_facilcls_c_inout_c_node_id_idx')
        self.log.info('End Make Highway Node')

    def _insert_hwy_node(self, param):
        self.log.info('insert hwy node')
        sqlcmd = '''
        INSERT INTO hwy_node(road_code, updown_c, road_seq,
                             facilcls_c, inout_c, tollcls_c,
                            node_id, facil_name, the_geom)
        VALUES(%s, %s, %s, %s, %s, %s, %s, %s, %s)
        '''
        self.pg.execute1(sqlcmd, param)
        return 0

    def _get_facil_name(self, road_code, updown, pnttype, node_id):
        ''' '''
        self.log.info('get facil name')
        sqlcmd = '''
        SELECT a.road_code, a.updown, a.node_id, a.seq_nm, c.pntname
        FROM hwy_link_road_code_info as a
        left join mid_hwy_org_facility_node as b
        on a.road_code = b.path_id and a.node_id = b.node_id
        left join org_highwaypoint as c
        on b.path_id = c.pathid and b.facility_id = c.pntid and
           ((b.dirflag = 1 and c.negativefl = 1)or
            (b.dirflag = 2 and c.positivefl = 1))
        where road_code = %s and updown = %s and pnttype = %s and
              seq_nm < (
                  SELECT seq_nm
                  FROM hwy_link_road_code_info
                  where road_code = %s and updown = %s and node_id = %s
                  )
        order by a.road_code, a.updown, a.seq_nm desc, a.node_id
        '''
        self.pg.execute1(sqlcmd, (road_code, updown, pnttype,
                                  road_code, updown, node_id))
        for row in self.pg.fetchall():
            (road_code, updown, node_id, seq_nm, pntname) = row
            if pntname:
                return [pntname]
        return [None]

    def _filter_JCT_UTurn(self):
        '''过滤假JCT/UTurn:下了高速又转弯回来的径路'''
        pass

    def _make_hwy_service(self):
        self.log.info('make hwy service')
        self.CreateTable2('hwy_service')
        sqlcmd = '''
           select c.road_code, d.updown_c, d.road_seq,
                  a.gasstation, a.restaurant,
                  a.atm, a.shop, a.motel
            from org_sa as a
            left join mid_hwy_org_facility_node as b
            ON a.pathid = b.path_id and
               a.dirflag = b.dirflag and
               a.pointid = b.facility_id
            LEFT JOIN road_code_info as c
            ON a.pathid = c.path_id
            LEFT JOIN hwy_node as d
            on c.road_code = d.road_code and
               b.node_id = d.node_id and
               a.dirflag = updown_c
               and facilcls_c in (1, 2)
            order by c.road_code, b.dirflag, b.node_id
        '''
        self.pg.connect1()
        for (road_code, updown_c, road_seq,
             gasstation, restaurant, atm,
             shop, motel) in self.get_batch_data(sqlcmd):
            gas_station = gasstation
            information = 0
            rest_area = motel
            shopping_coner = shop
            postbox = 0
            ATM = atm
            Restaurant = restaurant
            toilet = 0
            parm = (gas_station, information, rest_area, shopping_coner,
                    postbox, ATM, Restaurant, toilet)
            self._store_service_info(road_code, road_seq,
                                     updown_c, parm)
        self.pg.commit1()
        self.log.info('End Make Facility Service.')

    def _store_facil_path(self, road_code, road_seq, facilcls_c,
                          inout_c, path, path_type, updown_c=1):
        '''保存设施路径'''
        sqlcmd = """
        INSERT INTO mid_temp_hwy_ic_path(road_code, road_seq, updown_c,
                                         facilcls_c, inout_c, node_id,
                                         to_node_id, node_lid, link_lid,
                                          path_type)
           VALUES(%s, %s, %s,
                  %s, %s, %s,
                  %s, %s, %s, %s)
        """
        node_id = path[0]
        to_node_id = path[-1]
        node_lid = ','.join([str(node) for node in path])
        if inout_c == HWY_INOUT_TYPE_OUT:
            link_list = self.G.get_linkids(path)
        elif inout_c == HWY_INOUT_TYPE_IN:
            link_list = self.G.get_linkids(path, reverse=True)
        else:
            link_list = []
        link_lid = ','.join([str(link) for link in link_list])
        params = (road_code, road_seq, updown_c, facilcls_c,
                  inout_c, node_id, to_node_id, node_lid,
                  link_lid, path_type)
        self.pg.execute1(sqlcmd, params)


