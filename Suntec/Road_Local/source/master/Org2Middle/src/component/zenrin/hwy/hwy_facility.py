# -*- coding: UTF-8 -*-
'''
Created on 2015-7-13

@author: PC_ZH
'''
from component.rdf.hwy.hwy_facility_rdf import HwyFacilityRDF
# import json
import common
# import component.component_base
# from common import cache_file
from component.rdf.multi_lang_name_rdf import MultiLangNameRDF
from component.rdf.hwy.hwy_graph_rdf import is_cycle_path
from component.rdf.hwy.hwy_graph_rdf import HWY_ORG_FACIL_INFO
from component.rdf.hwy.hwy_def_rdf import HWY_INOUT_TYPE_IN
from component.rdf.hwy.hwy_def_rdf import HWY_INOUT_TYPE_OUT
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_IC
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_JCT
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_SA
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_PA
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_TOLL
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_UTURN
from component.jdb.hwy.hwy_data_mng import HwyFacilInfo
SEQ_IC_TYPE_IC = 1
SEQ_IC_TYPE_JCT = 2
SEQ_IC_TYPE_SAPA = 3
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
ZN_FACILITY_NONE = 9999

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

    def _make_hwy_store(self):
        #台湾没有店铺情报
        return 0

    def _make_sapa_info(self):
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
            #  设置设施序号
            path_road_seq_list = list()  # [{},...,{}]
            for node_index in range(0, len(facils_list)):
                node, all_facils = facils_list[node_index]
                temp_road_seq_dict = {}
                # 获取node所有设施种别对应的设施号{'ic_jct':[1,2], 'sapa':[1]}
                temp_road_seq_dict = self._get_org_facility_dict(road_code,
                                                                 node)
                # facils_info_list:
                # ((road_seq, facilcls, inoutc, ic_path),(),())
                print node
                if node == 3620050008856:
                    pass
                facils_info_list = self._get_facils_info(road_code,
                                                         all_facils,
                                                         temp_road_seq_dict,
                                                         path_road_seq_list,
                                                         node_index)
                road_seq_dict = {SEQ_IC_TYPE_IC: [],
                                 SEQ_IC_TYPE_JCT: [],
                                 SEQ_IC_TYPE_SAPA: []}
                ic_road_seqs = list()
                jct_road_seqs = list()
                sapa_road_seqs = list()
                for facil_info in facils_info_list:
                    ic_path, facilcls, inout_c, road_seq = facil_info
                    if facilcls == HWY_IC_TYPE_IC:
                        if road_seq not in ic_road_seqs:
                            ic_road_seqs.append(road_seq)
                    elif facilcls == HWY_IC_TYPE_JCT:
                        if road_seq not in jct_road_seqs:
                            jct_road_seqs.append(road_seq)
                    else:
                        if road_seq not in sapa_road_seqs:
                            sapa_road_seqs.append(road_seq)

                    if not road_seq:
                        self.log.error('No Road Seq.')
                        continue
                    if not path:
                        self.log.error('No Path. node=%s' % node)
                        continue
                    path_type = IC_PATH_TYPE_DICT.get(facilcls)
                    self._store_facil_path_2_file(road_code, road_seq,
                                                  facilcls, inout_c,
                                                  updown, ic_path, path_type,
                                                  temp_file_obj)
                road_seq_dict[SEQ_IC_TYPE_IC] = ic_road_seqs
                road_seq_dict[SEQ_IC_TYPE_JCT] = jct_road_seqs
                road_seq_dict[SEQ_IC_TYPE_SAPA] = sapa_road_seqs
                path_road_seq_list.append(road_seq_dict)
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

    def _get_facils_sorted(self, road_code, all_facils, road_seq_dict,
                           path_road_seq_list, node_index):
        '''将源数据中facility_id按IC/JCT分割'''
        # 获取node设施种别对应的设施数{'ic':1, 'jct':1 , 'sapa':1}
        facils_count_dict = self._get_facil_count(all_facils)
        ic_count, jct_count, sapa_count = facils_count_dict

        org_ic_jct = road_seq_dict[ZN_IC_TYPE_IC_JCT]
        org_sapa = road_seq_dict[ZN_IC_TYPE_SAPA]

        facility_list_ic = list()
        facility_list_jct = list()
        facility_list_sapa = list()
        if ic_count or jct_count:
            if not org_ic_jct:
                if ic_count:
                    facility_list_ic = self._get_prev_org_road_seq(
                                                            road_code,
                                                            path_road_seq_list,
                                                            node_index,
                                                            SEQ_IC_TYPE_IC)
                if jct_count:
                    facility_list_jct = self._get_prev_org_road_seq(
                                                            road_code,
                                                            path_road_seq_list,
                                                            node_index,
                                                            SEQ_IC_TYPE_JCT)
            else:
                if not org_ic_jct:
                    self.log.error('can not find org ic_jct')
                else:
                    org_ic_jct.sort()
                    org_ic_jct_count = len(org_ic_jct)
                    if ic_count and (not jct_count):
                        facility_list_ic.append(org_ic_jct[0])
                    elif (not ic_count) and jct_count:
                        #原始facility_id有多个 且JCT路径有多条 需要保存多个值
                        if jct_count > 1:
#                             for count in range(0, jct_count):
#                                 if org_ic_jct_count < (count + 1):
#                                     facility_list_jct.append(org_ic_jct[0])
#                                 else:
#                                     facility_list_jct.append(org_ic_jct[count])
                            if org_ic_jct_count >= jct_count:
                                for count in range(0, jct_count):
                                    facility_list_jct.append(org_ic_jct[count])
                            else:
                                #  self.log.error('cannot match jct')
                                facility_list_jct.append(org_ic_jct[0])
                        elif jct_count == 1:
                            facility_list_jct.append(org_ic_jct[0])
                        else:
                            pass
                    elif ic_count and jct_count:
                        if org_ic_jct_count == 2:
                            facility_list_ic.append(org_ic_jct[0])
                            facility_list_jct.append(org_ic_jct[1])
                        else:
                            self.log.error('cannot macth ic jct')
                    else:
                        pass

        if sapa_count:
            if not org_sapa:
                facility_list_sapa = self._get_prev_org_road_seq(
                                                       road_code,
                                                       path_road_seq_list,
                                                       node_index,
                                                       SEQ_IC_TYPE_SAPA)
            else:
                if not org_sapa:
                    self.log.error('can not find org sapa')
                else:
                    facility_list_sapa.append(org_sapa[0])
        else:
            pass
        return facility_list_ic, facility_list_jct, facility_list_sapa

    def _get_facils_info(self, road_code, all_facils, road_seq_dict,
                         path_road_seq_list, node_index):
        ''' '''
        #facility_tuple=([1010, 1011],[1020,1021],[1030,1031])
        facility_tuple = self._get_facils_sorted(road_code,
                                                 all_facils,
                                                 road_seq_dict,
                                                 path_road_seq_list,
                                                 node_index)
        #facil_tuple=([2010],[2021,2022],[2031])
        facil_tuple = self._get_facil_path_list(road_code, all_facils)
        # road_seq_list: [(ic_path, facil_c, road_seq), (...), (...)]
        facils_info_list = self._get_road_seq_list(facil_tuple,
                                                   facility_tuple)
        return facils_info_list

    def _get_facil_path_list(self, road_code, all_facils):
        ''' '''
        facil_ic = list()
        facil_jct = list()
        facil_sapa = list()
        temp_facil_list_jct = list()
        for facilcls, inout_c, ic_path in all_facils:
            '''JCT path排序 '''
            #添加updown
            if facilcls == HWY_IC_TYPE_JCT:
                nodes = self.G.get_to_main_link_of_path(ic_path, road_code)
                if len(nodes) > 1:
                    self.log.error('more than one node')
                    node = nodes[0]
                elif len(nodes) == 1:
                    node = nodes[0]
                else:
                    node = None
                if node:
                    to_road_code = self.G._get_road_code(ic_path[-1], node)
                    to_updown_code = self.G._get_road_updown(ic_path[-1], node)
                else:
                    to_road_code = None
                    to_updown_code = None
                temp_facil_list_jct.append((to_road_code,
                                            to_updown_code,
                                            facilcls,
                                            ic_path, inout_c))
            elif facilcls == HWY_IC_TYPE_IC:
                facil_ic.append((ic_path, facilcls, inout_c))
            elif facilcls in (HWY_IC_TYPE_SA, HWY_IC_TYPE_PA):
                facil_sapa.append((ic_path, facilcls, inout_c))
        # prepare  JCT list
        if temp_facil_list_jct:
            temp_facil_list_jct.sort(cmp=lambda x, y: cmp(x[0], y[0]),
                                     reverse=False)
        for (road_code, updown, facilcls, ic_path, inout_c) in temp_facil_list_jct:
            facil_jct.append((ic_path, HWY_IC_TYPE_JCT, inout_c))
        # facil = [(ic_path, inout_c),(),...]
        return facil_ic, facil_jct, facil_sapa

    def _get_road_seq_list(self, facils, facility_lid):
        '''取得设施road_seq信息 '''
        road_seq_list = list()
        facil_ic, facil_jct, facil_sapa = facils
        (facility_id_list_ic, facility_id_list_jct,
         facility_id_list_sapa) = facility_lid
        #  ZIP JCT
        if facil_jct:
            facil_jct_count = len(facil_jct)
            if facil_jct_count > 2:
                if len(facility_id_list_jct) > 1:
                    self.log.error('more than 2 jct')
                else:
                    for count in range(0, len(facil_jct)):
                        facil = facil_jct[count]
                        facility_id = (facility_id_list_jct[0],)
                        facil = facil + facility_id
                        road_seq_list.append(facil)
            if facil_jct_count == 2:
                if len(facility_id_list_jct) == 2:
                    for count in range(0, len(facil_jct)):
                        facil = facil_jct[count]
                        facility_id = (facility_id_list_jct[count],)
                        facil = facil + facility_id
                        road_seq_list.append(facil)
                elif len(facility_id_list_jct) == 1:
                    for count in range(0, len(facil_jct)):
                        facil = facil_jct[count]
                        facility_id = (facility_id_list_jct[0],)
                        facil = facil + facility_id
                        road_seq_list.append(facil)
                else:
                    pass
            elif facil_jct_count == 1:
                if facility_id_list_jct:
                    facil = facil_jct[0]
                    facil = facil + (facility_id_list_jct[0],)
                    road_seq_list.append(facil)
                else:
                    self.log.warning('no jct facility id match')
                    facil = facil_jct[0]
                    facil = facil + (ZN_FACILITY_NONE,)
                    road_seq_list.append(facil)
            else:
                pass
        else:
            pass
        #  ZIP IC
        if facil_ic:
            if facility_id_list_ic:
                for facil in facil_ic:
                    facil = facil + (facility_id_list_ic[0],)
                    road_seq_list.append(facil)
            else:
                self.log.warning('no ic facility id match')
                for facil in facil_ic:
                    facil = facil + (ZN_FACILITY_NONE,)
                    road_seq_list.append(facil)
        else:
            pass
        # ZIP sapa
        if facil_sapa:
            if facility_id_list_sapa:
                for facil in facil_sapa:
                    facil = facil + (facility_id_list_sapa[0],)
                    road_seq_list.append(facil)
            else:
                self.log.warning('no sapa facility id match')
                for facil in facil_sapa:
                    facil = facil + (ZN_FACILITY_NONE,)
                    road_seq_list.append(facil)
        else:
            pass

        return road_seq_list

    def _get_facil_count(self, all_facils):
        ic_count = 0
        jct_count = 0
        sapa_count = 0
        for facil_c, inout_c, ic_path in all_facils:
            if facil_c == HWY_IC_TYPE_IC:
                ic_count += 1
            elif facil_c == HWY_IC_TYPE_JCT:
                jct_count += 1
            elif facil_c in (HWY_IC_TYPE_SA, HWY_IC_TYPE_PA):
                sapa_count += 1
            else:
                pass
        return ic_count, jct_count, sapa_count

    def _get_prev_org_road_seq(self, road_code, path_road_seq_list, index, facilcls):
        ''' '''
        if index == 0:
            return []
        index -= 1
        road_seq_dict = path_road_seq_list[index]
        while not road_seq_dict[facilcls]:
            if index == 0:
                return []
            index -= 1
            road_seq_dict = path_road_seq_list[index]
        if not road_seq_dict[facilcls]:
            self.log.error('can not find pre road seq')
        return road_seq_dict[facilcls]

    def _get_org_facility_dict(self, road_code, node):
        ''' '''
        org_facility_dict = {ZN_IC_TYPE_IC_JCT: [],
                             ZN_IC_TYPE_SAPA: []}
        data = self.G.node[node]
        if data:
            hwy_facil_info = data.get(HWY_ORG_FACIL_INFO)
            facility_dict = self._get_facility_lid(road_code, hwy_facil_info)
            org_facility_dict[ZN_IC_TYPE_IC_JCT] = facility_dict[ZN_IC_TYPE_IC_JCT]
            org_facility_dict[ZN_IC_TYPE_SAPA] = facility_dict[ZN_IC_TYPE_SAPA]
        else:
            pass
        return org_facility_dict

    def _get_facility_lid(self, road_code, hwy_facil_info):
        '''{'ic_jct':[1010] , 'sapa':[1011,1012]} '''
        facility_id_dict = {}
        ic_jct_list = list()
        sapa_list = list()
        path_lid, facility_lid, pnttype = hwy_facil_info
        facil_info = zip(path_lid, facility_lid, pnttype)
        for path_id, facility_id, pnttype in facil_info:
            if path_id != road_code:
                continue
            if pnttype == ZN_IC_TYPE_IC_JCT:
                ic_jct_list.append(facility_id)
            elif pnttype == ZN_IC_TYPE_SAPA:
                sapa_list.append(facility_id)
        facility_id_dict[ZN_IC_TYPE_IC_JCT] = ic_jct_list
        facility_id_dict[ZN_IC_TYPE_SAPA] = sapa_list
        return facility_id_dict

    def _get_ic_path_link_set(self, roadcode, node, inout,
                              road_seqs, facilcls_cs, updown):
        facil_name = ''
#       updown = HWY_UPDOWN_TYPE_UP
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

    def _make_facil_name(self):
        '''制作设施出口名称'''
        self.log.info('Start Make Facil Name.')
        self.pg.connect1()
        self.CreateTable2('mid_temp_hwy_facil_name')
        sqlcmd = '''
            SELECT distinct a.road_code, a.updown_c, a.road_seq, pntname
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
            LEFT JOIN org_highwaypoint as b
            ON a.road_code = b.pathid and a.road_seq = b.pntid
            ORDER BY road_code, updown_c, road_seq
        '''
        for row in self.get_batch_data(sqlcmd):
            (r_code, updown, r_seq, facil_name) = row
            multi_name = None
            json_name = None
            if facil_name:
                    temp_multi_name = MultiLangNameRDF('CHT', facil_name)
                    if multi_name:
                        multi_name.add_alter(temp_multi_name)
                    else:
                        multi_name = temp_multi_name
            if multi_name:
                json_name = multi_name.json_format_dump()
            if not json_name:
                json_name = None
            self._insert_facil_name(r_code, updown, r_seq, json_name)
        self.pg.execute1(sqlcmd)
        self.pg.commit1()

        self.log.info('End Make Facility Name.')
        return 0

    def _make_facil_name1(self):
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
          (road_code, updown_c, road_seq, inout_c, node_id);
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        sqlcmd = """
        SELECT distinct a.road_code , a.updown_c ,a.road_seq ,
               a.facilcls_c, a.inout_c, 0 tollcls_c,
               a.node_id, b.facil_name, c.the_geom
        FROM mid_temp_hwy_ic_path as a
        LEFT JOIN
        (
            select distinct road_code, updown_c,
                   road_seq,facil_name
            from mid_temp_hwy_facil_name
            where facil_name is not null
        ) as b
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
                  a.info, a.gasstation, a.restaurant,
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
             info, gasstation, restaurant, atm,
             shop, motel) in self.get_batch_data(sqlcmd):
            gas_station = gasstation
            information = info
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

    def _is_one_node(self, path):
        if len(path) == 1:
            return True
        else:
            return False
