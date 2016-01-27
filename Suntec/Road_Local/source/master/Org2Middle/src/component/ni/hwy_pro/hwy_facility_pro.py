# -*- coding: UTF-8 -*-
'''
Created on 2015-5-27

@author: hcz
'''
from component.rdf.hwy.hwy_def_rdf import HWY_TRUE
from component.rdf.hwy.hwy_def_rdf import HWY_FALSE
from component.rdf.hwy.hwy_facility_rdf import ROAD_SEQ_MARGIN
from component.rdf.hwy.hwy_facility_rdf import HWY_PATH_TYPE_UTURN
from component.rdf.hwy.hwy_facility_rdf import IC_PATH_TYPE_DICT
from component.rdf.hwy.hwy_facility_rdf import HwyFacilityRDF
from component.ni.hwy_pro.hwy_poi_category_pro import HwyPoiCategoryPro
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_SA
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_PA
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_IC
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_TOLL
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_JCT
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_UTURN
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_URBAN_JCT
from component.rdf.hwy.hwy_def_rdf import HWY_INOUT_TYPE_IN
from component.rdf.hwy.hwy_def_rdf import HWY_INOUT_TYPE_OUT
from component.rdf.hwy.hwy_def_rdf import HWY_LINK_TYPE_RAMP
from component.rdf.hwy.hwy_graph_rdf import HWY_LINK_TYPE
from component.rdf.hwy.hwy_graph_rdf import HWY_ROAD_CODE
from component.rdf.hwy.hwy_facility_rdf import HWY_PATH_TYPE_SAPA
from component.rdf.hwy.hwy_facility_rdf import HWY_PATH_TYPE_JCT
from component.rdf.hwy.hwy_facility_rdf import HWY_PATH_TYPE_IC
from component.jdb.hwy.hwy_graph import get_simple_cycle
from component.rdf.hwy.hwy_def_rdf import ANGLE_10
from component.rdf.hwy.hwy_def_rdf import ANGLE_45
from component.rdf.hwy.hwy_def_rdf import ANGLE_100
from component.rdf.hwy.hwy_def_rdf import ANGLE_360
from __builtin__ import True, None
NI_SEQ = 10000000
HWY_UNKOWN_ID_NI = 99999999
SAPA_TYPE_DICT = {'8380': HWY_IC_TYPE_SA,  # 高速停车区
                  '8381': HWY_IC_TYPE_PA,  # 高速停车区
                  '4101': HWY_IC_TYPE_PA,  # 室内停车场
                  '4102': HWY_IC_TYPE_PA,  # 室外停车场
                  }
HWY_ORG_IC_TYPE_PA = 1
HWY_ORG_IC_TYPE_SA = 2
HWY_ORG_IC_TYPE_JCT = 4
HWY_ORG_IC_TYPE_IC = 8
FACIL_TYPE_DICT = {
                   HWY_IC_TYPE_IC: HWY_ORG_IC_TYPE_IC,
                   HWY_IC_TYPE_URBAN_JCT: HWY_ORG_IC_TYPE_JCT,
                   HWY_IC_TYPE_JCT: HWY_ORG_IC_TYPE_JCT,
                   HWY_IC_TYPE_SA: HWY_ORG_IC_TYPE_SA,
                   HWY_IC_TYPE_PA: HWY_ORG_IC_TYPE_PA,
                   HWY_IC_TYPE_UTURN: HWY_ORG_IC_TYPE_JCT,
                   HWY_IC_TYPE_TOLL: None
                   }


class HwyFacilityNiPro(HwyFacilityRDF):
    '''生成设施情报(中国四维)
    '''

    def __init__(self, data_mng, ItemName='HwyFacilityNiPro'):
        '''
        Constructor
        '''
        HwyFacilityRDF.__init__(self, data_mng, ItemName)
        self.poi_cate = HwyPoiCategoryPro()

    def _Do(self):
        self.poi_cate.Make()
        self.pg.connect2()
        self._make_ic_path()
        self._expand_ics_path_node()  # 把设施路径点展开
        self._make_inout_join_node()
        # 过滤假JCT/UTurn/SAPA:下了高速又转弯回来的径路
        self._filter_JCT_UTurn_SAPA()
        # 过滤不匹配专用数据的SAPA设施路径
        self._filter_sapa()
        # 过滤经过同一个收费点多次的sapa路径
        self._filter_reenter_tollgate_sapa()
        # 过滤双向SAPA: SAPA Link是双向的，SAPA出口同时也是入口，SAPA的入口同时也是出口
        # 把SAPA出口处的SAPA入口删除，同理把SAPA入口处的SAPA出口删除
        self._filter_both_dir_sapa()
        # 过滤辅路、类辅路， 从辅路路径选出SAPA路径(路径两头都有SAPA设施)
        self._filter_service_road()
        # 类辅路==>JCT
        self._convert_service_road_2_jct()
        # 过滤掉高速和一般直接相连的出入口设施
        self._filter_ic()
        # 过滤与SAPA同路径的IC并设
        self._filter_sapa_ic()
        # SAPA对应的Rest Area POI情报
        self._make_sapa_first_node()
        self._make_sapa_info()
        self._make_sapa_link()
        # 1.更新SA/PA的facilcls_c值, 2.更新URBAN_JCT为IC
        self._update_facil()
        # 把设施路径link展开(包含UTurn设放施)
        self._expand_ics_path_link(ignore_uturn=False)
        # 辅路经过多个收站
        self._filter_service_road_by_tollgate()
        self._filter_urban_path()
        # 辅路经过出入口(Ramp和一道的交点)
        self._filter_service_road_by_inout()
        self._deal_with_uturn()  # 处理U-Turn
        # 148.11217,-35.00432
        # self._del_sapa_node()
        self._make_facil_name()  # 设施名称
        self._make_hwy_node()
        self._make_facil_same_info()  # 并设情报
        self._load_poi_category()
        self._make_hwy_store()  # 店铺情报
        self._make_hwy_service()  # 服务情报
        # 把设施路径link展开(注：不包含UTurn设放施)
        self._expand_ics_path_link()
        self._make_facil_position()
        self._make_ic_out_info()  # IC出口服务情报/店铺情报

    def _make_sapa_first_node(self):
        return 0

    def _make_sapa_info(self):
        return 0

    def _make_sapa_link(self):
        return 0

    def _update_undefined_dict(self):
        self.undefined_dict.update(SERVICE_UNDEFINED_DICT)
        return 0

    def _get_max_org_facil_id(self, road_code):
        return NI_SEQ

    def _store_del_sapa_ic(self, del_path):
        '''备份被删除的IC'''
        sqlcmd = '''
        INSERT INTO mid_temp_hwy_sapa_ic_del(gid)
        VALUES(%s)
        '''
        for gid in del_path:
            self.pg.execute2(sqlcmd, (gid,))
        self.pg.commit2()
        # ## 备份路径情报
        sqlcmd = """
        UPDATE mid_temp_hwy_sapa_ic_del AS a
           SET road_code=b.road_code, road_seq = b.road_seq,
               facilcls_c = b.facilcls_c, inout_c = b.inout_c,
               node_id = b.node_id, to_node_id = b.to_node_id,
               node_lid = b.node_lid, link_lid = b.link_lid,
               path_type = b.path_type
          FROM mid_temp_hwy_ic_path as b
          where a.gid = b.gid;
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        # ## 路径从mid_temp_hwy_ic_path表中删去
        sqlcmd = """
        DELETE FROM mid_temp_hwy_ic_path
          WHERE gid in (
          SELECT gid
            FROM mid_temp_hwy_sapa_ic_del
          )
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _filter_sapa_ic(self):
        '''过滤转化为sapa设施的IC设施路径(实际为IC路径，其facilcls因为road_Seq被更新为'1')'''
        self.log.info('Start filter ic shared path with sapa ')
        self.CreateTable2('mid_temp_hwy_sapa_ic_del')
        sqlcmd = '''
        select gids, path_types
        from(
            select node_id, link_lid, array_agg(path_type) as path_types, array_agg(gid) as gids
            from mid_temp_hwy_ic_path
            group by road_code, updown_c, road_seq, node_id, link_lid
        )as m
        where  'IC'=any(path_types) and
               'SAPA' = any(path_types)
        '''
        del_path = list()
        for row in self.pg.get_batch_data2(sqlcmd):
            gids = row[0]
            path_types = row[1]
            gid_path_type_list = zip(gids, path_types)
            for info in gid_path_type_list:
                gid, path_type = info
                if path_type == HWY_PATH_TYPE_IC:
                    del_path.append(gid)
        self._store_del_sapa_ic(del_path)
        self.log.info('End filter ic shared path with sapa')

    def _store_del_reenter_tollgate_sapa(self, del_path):
        '''备份被删除的sapa'''
        sqlcmd = '''
        INSERT INTO mid_temp_hwy_reenter_tollgate_sapa_del(gid)
        VALUES(%s)
        '''
        for gid in del_path:
            self.pg.execute2(sqlcmd, (gid,))
        self.pg.commit2()
        # ## 备份路径情报
        sqlcmd = """
        UPDATE mid_temp_hwy_reenter_tollgate_sapa_del AS a
           SET road_code=b.road_code, road_seq = b.road_seq,
               facilcls_c = b.facilcls_c, inout_c = b.inout_c,
               node_id = b.node_id, to_node_id = b.to_node_id,
               node_lid = b.node_lid, link_lid = b.link_lid,
               path_type = b.path_type
          FROM mid_temp_hwy_ic_path as b
          where a.gid = b.gid;
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        # ## 路径从mid_temp_hwy_ic_path表中删去
        sqlcmd = """
        DELETE FROM mid_temp_hwy_ic_path
          WHERE gid in (
          SELECT gid
            FROM mid_temp_hwy_reenter_tollgate_sapa_del
          )
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _filter_reenter_tollgate_sapa(self):
        '''过滤经过同一个收费点多次的sapa路径'''
        self.log.info('Start filter reenter tollgate sapa')
        self.CreateTable2('mid_temp_hwy_reenter_tollgate_sapa_del')
        sqlcmd = '''
        select gid, string_to_array(node_lid,',')::bigint[] as node_lid
        from mid_temp_hwy_ic_path
        where facilcls_c in (1, 2)
        '''
        del_path = list()
        for row in self.pg.get_batch_data2(sqlcmd):
            gid = row[0]
            path = row[1]
            path_set = set(path[1:-1])
            for node in path_set:
                if self.G.is_tollgate(node):
                    if path.count(node) > 1:
                        del_path.append(gid)
                        break
        self._store_del_reenter_tollgate_sapa(del_path)
        self.log.info('End filter reenter tollgate sapa')

    def _matched_org_ic(self, node_id, inout):
        if self.G.get_org_facil_id(node_id):
            for facil_info in self.G.get_org_facil_id(node_id):
                facil_c, inout_c = facil_info[1:3]
                if facil_c in (HWY_ORG_IC_TYPE_IC,) and inout_c == inout:
                    return True
        return False

    def _filter_ic(self):
        '''过滤掉高速和一般直接相连的出入口设施。'''
        self.log.info('Start filter ic')
        self.CreateTable2('mid_temp_hwy_ic_del')
        sqlcmd = """
        SELECT gid, road_code, updown_c, inout_c, node_id, node_lid
          FROM mid_temp_hwy_ic_path
          where facilcls_c = 5;
        """
        del_list = []
        for row in self.get_batch_data(sqlcmd):
            gid, road_code, updown, inout, node_id, node_lid = row
            if self._matched_org_ic(node_id, inout):
                continue
            reverse = False
            if inout == HWY_INOUT_TYPE_IN:
                reverse = True
            path = map(int, node_lid.split(','))
            # 没有Ramp/JCT，且没有
            if(not self.G.exist_ic_link(path, reverse) and
               not self.G.exist_hwy_main_link(path, reverse) and
               not self.hwy_data.is_road_start_node(road_code,
                                                    updown,
                                                    node_id) and
               not self.hwy_data.is_road_end_node(road_code,
                                                  updown,
                                                  node_id)):
                del_list.append(gid)
                continue
        self._store_del_ic(del_list)
        self.log.info('End filter ic')

    def _update_facil(self):
        '''1.更新SA/PA的facilcls_c值
           2.更新URBAN_JCT为IC'''
        sqlcmd = '''
        update mid_temp_hwy_ic_path
        set facilcls_c = 1   -- 1:SA  2:PA
        where road_seq in (
        select id
        from mid_hwy_org_hw_junction_mid_linkid_merged
        where attr = 2    --1:PA  2:SA
        )
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        # 保存urban jct
        self._backup_urban_jct()
        # 把Urban JCT番号改为IC的番号: Urban JCT没有匹配番号or名称和IC名称相同时
        sqlcmd = """
        UPDATE mid_temp_hwy_ic_path as a
          set facilcls_c = 5, road_seq = ic_road_seq
          from (
            SELECT distinct road_code, road_seq, inout_c, updown_c,
                node_id, ic_road_seq
              FROM mid_temp_hwy_urban_jct_bak
          ) as b
          where a.road_code = b.road_code and
                a.road_seq = b.road_seq and
                a.inout_c = b.inout_c and
                a.updown_c = b.updown_c and
                a.node_id = b.node_id and
                ic_road_seq is not null
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        # IC没有匹配的番号时，用该点Urban JCT的番号
        sqlcmd = """
        UPDATE mid_temp_hwy_ic_path as a set road_seq = b.road_seq
          from (
            SELECT distinct road_code, road_seq, inout_c,
               updown_c, facilcls_c, node_id
              FROM mid_temp_hwy_ic_path
              where facilcls_c = 12 and road_seq <> %s
          ) as b
          where a.facilcls_c = 5 and
                a.road_seq = %s and
                a.road_code = b.road_code and
                a.inout_c = b.inout_c and
                a.updown_c = b.updown_c and
                a.node_id = b.node_id and
                b.road_seq is not null
        """
        self.pg.execute2(sqlcmd, (HWY_UNKOWN_ID_NI, HWY_UNKOWN_ID_NI))
        self.pg.commit2()
        # 修改Urban JCT设施种别
        sqlcmd = '''
        update mid_temp_hwy_ic_path
        set facilcls_c = 5  --IC
        where facilcls_c = 12
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return 0

    def _backup_urban_jct(self):
        '''保存urban jct'''
        self.log.info("Backup urban jct.")
        self.CreateTable2('mid_temp_hwy_urban_jct_bak')
        sqlcmd = """
        INSERT INTO mid_temp_hwy_urban_jct_bak(road_code, road_seq, inout_c,
                                     updown_c, node_id, node_lid, ic_road_seq)
        (
        SELECT c.road_code, c.road_seq, c.inout_c,
               c.updown_c, c.node_id, node_lid, d.road_seq as ic_road_seq
         from (
            SELECT road_code, road_seq, inout_c, updown_c,
                   node_id,node_lid,  name
            FROM (
                SELECT distinct road_code, road_seq, inout_c,
                       updown_c, node_id, node_lid
                  from mid_temp_hwy_ic_path
                  where facilcls_c = 12
              ) AS a
              LEFT JOIN temp_hwy_facil_name_ni as b
              on a.road_seq = b.junction_id
          ) as c
          left join (
            SELECT road_code, road_seq, inout_c, updown_c, node_id, name
            FROM (
                SELECT distinct road_code, road_seq, inout_c,
                       updown_c, node_id
                  from mid_temp_hwy_ic_path
                  where facilcls_c = 5
              ) AS a
              LEFT JOIN temp_hwy_facil_name_ni as b
              on a.road_seq = b.junction_id
         ) as d
         on c.road_code = d.road_code and c.node_id = d.node_id
            and c.inout_c = d.inout_c and c.updown_c = d.updown_c and
            (c.name is null or c.name = d.name)
        );

        """
        self.pg.execute(sqlcmd)
        self.pg.commit2()

    def _make_road_seq(self, road_code, updown, facils_list,
                       sapa_node_dict, next_seq, temp_file_obj):
        sapa_seq_dict = dict()
        next_seq = self._get_max_org_facil_id(road_code)
        for node, all_facils in facils_list:
            sort_facils = self._sort_facils(node, road_code, all_facils)
            road_seq_dict, next_seq = self._get_road_seq(node, road_code,
                                                         next_seq,
                                                         sort_facils,
                                                         sapa_node_dict,
                                                         sapa_seq_dict)
            matched_facil = dict()
            coded_facil_list = list()
            for facil_info in sort_facils:
                road_seq = None
                facilcls, inout_c, inout_link, to_node, path = facil_info
                if facilcls == HWY_IC_TYPE_UTURN:
                    temp_key = (HWY_IC_TYPE_JCT, inout_c, inout_link,
                                to_node, tuple(path))
                    road_seq = road_seq_dict.get(temp_key)
                    if road_seq:
                        facilcls = HWY_IC_TYPE_JCT
                if road_seq:  # UTurn有匹配的JCT
                    # 设施种别依然是JCT，路径种别做出成UTurn
                    path_type = IC_PATH_TYPE_DICT.get(HWY_IC_TYPE_UTURN)
                else:  # UTurn没有匹配的JCT
                    temp_key = (facilcls, inout_c, inout_link,
                                to_node, tuple(path))
                    road_seq = road_seq_dict.get(temp_key)
                    path_type = IC_PATH_TYPE_DICT.get(facilcls)
                facil_key = facilcls, inout_c
                if not road_seq and facilcls == HWY_IC_TYPE_IC:
                    for temp_cls in [HWY_IC_TYPE_SA, HWY_IC_TYPE_PA]:
                        temp_key = (temp_cls, inout_c, inout_link,
                                    to_node, tuple(path))
                        road_seq = road_seq_dict.get(temp_key)
                        if road_seq:
                            # pat_type种别设为IC
                            path_type = IC_PATH_TYPE_DICT.get(facilcls)
                            # 设施种别设为SA/PA
                            facilcls = temp_cls
                            facil_key = temp_cls, inout_c
                            break
                if road_seq:
                    if facil_key not in matched_facil:
                        matched_facil[facil_key] = set([road_seq])
                    else:
                        if road_seq != matched_facil.get(facil_key):
                            matched_facil[facil_key].add(road_seq)
                else:  # 没有匹配专用数据facil_id
                    pass
                coded_facil = (road_code, road_seq, facilcls, inout_c,
                               updown, path, path_type, temp_file_obj)
                coded_facil_list.append(coded_facil)
            #  处理匹配不到road_seq的case
            for temp_facil in coded_facil_list:
                (road_code, road_seq, facilcls, inout_c,
                 updown, path, path_type, temp_file_obj) = temp_facil
                if not road_seq:
                    if facilcls in (HWY_IC_TYPE_SA, HWY_IC_TYPE_PA):
                        road_seq = sapa_seq_dict.get(node)
                        matched_facil[(facilcls, inout_c)] = [road_seq]
                    else:
                        facil_key = facilcls, inout_c
                        if facil_key in matched_facil:
                            road_seq = min(matched_facil.get(facil_key))
                        else:  # 匹配不到原始IC，则匹配SAPA。
                            for temp_cls in [HWY_IC_TYPE_SA, HWY_IC_TYPE_PA]:
                                facil_key = temp_cls, inout_c
                                road_seqs = matched_facil.get(facil_key)
                                if road_seqs:
                                    facilcls = temp_cls
                                    road_seq = min(road_seqs)
                                    break
                    if not road_seq:
                        self.log.warning('no road seq. node=%s,facil=%s'
                                         % (node, facilcls))
                        road_seq = HWY_UNKOWN_ID_NI
                self._store_facil_path_2_file(road_code, road_seq, facilcls,
                                              inout_c, updown, path, path_type,
                                              temp_file_obj)

    def _store_facil_path_2_file(self, road_code, road_seq, facilcls_c,
                                 inout_c, updown_c, path,
                                 path_type, file_obj, facil_name=''):
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
        if not facil_name:
            facil_name = ''
        params = (road_code, road_seq, facilcls_c,
                  inout_c, updown_c, node_id,
                  to_node_id, node_lid, link_lid,
                  path_type, facil_name)
        file_obj.write('%d\t%d\t%d\t'
                       '%d\t%d\t%d\t'
                       '%d\t%s\t%s\t'
                       '%s\t%s\n' % params)

    def _sort_facils(self, node, road_code, all_facils):
        '''出口: 按IC, JCT, SAPA; 入口: SAPA, JCT, IC.'''
        toll_facil_types = list()
        in_facil_types = list()
        out_facil_types = list()
        one_node_jct = list()
        sorted_facils = []
        for facil_path_info in all_facils:
            facilcls, inout_c, path = facil_path_info
            in_out_link = None
            if facilcls in (HWY_IC_TYPE_JCT, HWY_IC_TYPE_URBAN_JCT,
                            HWY_IC_TYPE_UTURN):
                to_node = path[-1]
            else:
                to_node = None
            if inout_c == HWY_INOUT_TYPE_OUT:
                if len(path) < 2:
                    if facilcls in (HWY_IC_TYPE_JCT, HWY_IC_TYPE_URBAN_JCT):
                        out_nodes = self.G.get_main_link(node, road_code,
                                                         HWY_ROAD_CODE,
                                                         same_code=False,
                                                         reverse=False)
                        if len(out_nodes) > 1:
                            self.log.warning('more than one node-jct on node')
                        for out_node in out_nodes:
                            in_out_link = self.G.get_linkid(node, out_node)
                        facil_key = (facilcls, inout_c, in_out_link,
                                     to_node, path)
                        if facil_key not in one_node_jct:
                            out_facil_types.append(facil_key)
                            one_node_jct.append(facil_key)
                    else:
                        self.log.warning('only one node on path.node=%s'
                                         % node)
                else:
                    (u, v) = path[0], path[1]
                    if self.G.has_edge(u, v):
                        in_out_link = self.G.get_linkid(u, v)
                    else:
                        in_out_link = None
                        self.log.error('no edge in graph.u=%s,v=%s' % (u, v))
                    facil_key = (facilcls, inout_c, in_out_link, to_node, path)
                    out_facil_types.append(facil_key)
            elif inout_c == HWY_INOUT_TYPE_IN:
                # 断头路需要过滤
                if len(path) < 2:
                    if facilcls in (HWY_IC_TYPE_JCT, HWY_IC_TYPE_URBAN_JCT):
                        in_nodes = self.G.get_main_link(node, road_code,
                                                        HWY_ROAD_CODE,
                                                        same_code=False,
                                                        reverse=True)
                        if len(in_nodes) > 1:
                            self.log.warning('more than one node-jct on node')
                        for in_node in in_nodes:
                            in_out_link = self.G.get_linkid(in_node, node)
                        facil_key = (facilcls, inout_c, in_out_link,
                                     to_node, path)
                        if facil_key not in one_node_jct:
                            in_facil_types.append(facil_key)
                            one_node_jct.append(facil_key)
                    else:
                        in_out_link = None
                        self.log.warning('only one node on path.node=%s'
                                         % node)
                else:
                    (u, v) = path[1], path[0]
                    if self.G.has_edge(u, v):
                        in_out_link = self.G.get_linkid(u, v)
                    else:
                        self.log.error('no edge in graph.u=%s,v=%s' % (u, v))
                    facil_key = (facilcls, inout_c, in_out_link, to_node, path)
                    in_facil_types.append(facil_key)
            else:
                in_out_link = None
                facil_key = (facilcls, inout_c, in_out_link, to_node, path)
                toll_facil_types.append(facil_key)
        # 入口按种别序排:大到小
        temp_list = list(in_facil_types)
        temp_list.sort(key=lambda l: (l[0], l[2]), reverse=True)
        sorted_facils += temp_list
        # 无出入口种别
        sorted_facils += list(toll_facil_types)
        # 出口设施按种别倒序排: 小到大
        temp_list = list(out_facil_types)
        temp_list.sort(key=lambda l: (l[0], l[2]), reverse=False)
        sorted_facils += temp_list
        return sorted_facils

    def _get_facil_to_node(self, facility_id, inout_c, inoutlink):
        if inout_c == HWY_INOUT_TYPE_IN:
            sqlcmd = '''
            select nodeid
            from(
                select s_junc_pid::bigint as id
                from mid_hwy_org_hw_jct_merged
                where e_junc_pid::bigint = %s
            )as a
            left join mid_hwy_org_hw_junction_mid_linkid_merged as b
            on a.id = b.id
            '''
        elif inout_c == HWY_INOUT_TYPE_OUT:
            sqlcmd = '''
            select nodeid
            from(
                select e_junc_pid::bigint as id
                from mid_hwy_org_hw_jct_merged
                where s_junc_pid::bigint = %s
            )as a
            left join mid_hwy_org_hw_junction_mid_linkid_merged as b
            on a.id = b.id
            '''
        else:
            return None
        to_node = list()
        for row in self.pg.get_batch_data2(sqlcmd, (facility_id,)):
            to_node.append(row[0])
        return to_node

    def _get_org_facil(self, node, road_code, facil_list):
        facil_dict = dict()
        for (org_road_code, facilcls_c, inout_c,
             inout_link, name, facility_id) in facil_list:
            to_node = None
            temp_key = None
            if org_road_code != road_code:
                continue
            if facilcls_c == FACIL_TYPE_DICT[HWY_IC_TYPE_JCT]:
                to_node_list = self._get_facil_to_node(facility_id,
                                                       inout_c,
                                                       inout_link)
                for to_node in to_node_list:
                    temp_key = (facilcls_c, inout_c, inout_link, to_node)
                    if temp_key not in facil_dict:
                        facil_dict[temp_key] = facility_id
            else:
                temp_key = (facilcls_c, inout_c, inout_link, to_node)
                if temp_key in facil_dict.keys():
                    self.log.warning('too many facils.node=%s' % node)
                    temp_facil_id = facil_dict.get(temp_key)
                    if temp_facil_id and temp_facil_id > facility_id:
                        facil_dict[temp_key] = facility_id
                else:
                    facil_dict[temp_key] = facility_id
        return facil_dict

    def get_jct_org_id(self, node, inout, org_facils_dict):
        # 获取点JCT的原始设施番号
        if not org_facils_dict:
            return None
        for key in org_facils_dict.keys():
            facilcls_c, inout_c = key[0:2]
            if(facilcls_c == FACIL_TYPE_DICT[HWY_IC_TYPE_JCT] and
               inout_c == inout):
                return org_facils_dict[key]
        return None

    def _set_sapa_road_seq(self, node, facilcls, seq, sapa_node_dict,
                           sapa_seq_dict, visited):
        if facilcls not in (HWY_IC_TYPE_SA, HWY_IC_TYPE_PA):
            return 0
        if node in sapa_seq_dict:
            return 0
        visited.append(node)
        # 取关联SAPA Node的序号
        if node not in sapa_node_dict:
            self.log.warning('Not in sapa_node_dict. node=%s' % node)
            return 0
        else:
            sapa_seq_dict[node] = seq
        to_nodes = list(sapa_node_dict.get(node))
        for to_node in to_nodes:
            if to_node in visited:
                continue
            self._set_sapa_road_seq(to_node, facilcls, seq, sapa_node_dict,
                                    sapa_seq_dict, visited)
        return 0

    def _get_road_seq(self, node, road_code, road_seq, sorted_facils,
                      sapa_node_dict, sapa_seq_dict):
        road_seq_dict = dict()
        org_facility_id = None
        org_facil_list = self.G.get_org_facil_id(node)
        new_ids = {}
        # sorted_facils is set()
        if org_facil_list:
            org_facils_dict = self._get_org_facil(node,
                                                  road_code,
                                                  org_facil_list)
        else:
            org_facils_dict = {}
        next_road_seq = road_seq
        if not org_facils_dict and not self.G.is_tollgate(node):
            self.log.warning('node has no org_facil,node=%s' % node)
        for facil_idx in range(0, len(sorted_facils)):
            (facilcls, inout, inout_link,
             to_node, path) = sorted_facils[facil_idx]
            road_seq_key = facilcls, inout, inout_link, to_node, tuple(path)
            if facilcls not in (HWY_IC_TYPE_SA, HWY_IC_TYPE_PA):
                ni_facilcls = FACIL_TYPE_DICT[facilcls]
                org_facility_key = ni_facilcls, inout, inout_link, to_node
                org_facility_id = org_facils_dict.get(org_facility_key)
                if not org_facility_id:
                    if facilcls in (HWY_IC_TYPE_JCT, HWY_IC_TYPE_UTURN,
                                    HWY_IC_TYPE_URBAN_JCT):
                        for key, org_id in org_facils_dict.iteritems():
                            if(ni_facilcls == key[0] and
                               inout == key[1] and
                               to_node == key[3]):
                                org_facility_id = org_id
                                break
                # ## Urban JCT如果没有对应JCT, 找同方向的IC设施
                if not org_facility_id and facilcls == HWY_IC_TYPE_URBAN_JCT:
                    for key, org_id in org_facils_dict.iteritems():
                        if(HWY_ORG_IC_TYPE_IC == key[0] and
                           inout == key[1] and
                           inout_link == key[2]):
                            org_facility_id = org_id
                            break
                # ## IC如果没有对应设施,且过经SA link, 找同方向的SAPA设施
                if(not org_facility_id and
                   facilcls == HWY_IC_TYPE_IC and
                   self._just_ic_facil(sorted_facils)):
                    for facilcls in [HWY_IC_TYPE_SA, HWY_IC_TYPE_PA]:
                        ni_facilcls = FACIL_TYPE_DICT[facilcls]
                        org_facility_key = (ni_facilcls, inout,
                                            inout_link, to_node)
                        org_id = org_facils_dict.get(org_facility_key)
                        if org_id:
                            if inout == HWY_INOUT_TYPE_IN:
                                reverse = True
                            else:
                                reverse = False
                            if self.G.is_sapa_path(path,
                                                   reverse=reverse):
                                org_facility_id = org_id
                                road_seq_key = (facilcls, inout, inout_link,
                                                to_node, tuple(path))
                                # print 'IC:', node
                    if org_facility_id:
                        road_seq_dict[road_seq_key] = org_facility_id
                        continue
            else:
                for facilcls in [HWY_IC_TYPE_SA, HWY_IC_TYPE_PA]:
                    ni_facilcls = FACIL_TYPE_DICT[facilcls]
                    org_facility_key = ni_facilcls, inout, inout_link, to_node
                    org_facility_id = org_facils_dict.get(org_facility_key)
                    if org_facility_id:
                        break
            if(facilcls in (HWY_IC_TYPE_SA, HWY_IC_TYPE_PA) and
               node in sapa_node_dict):
                visited = []
                sapa_road_seq = self._get_sapa_road_seq(node,
                                                        facilcls,
                                                        sapa_node_dict,
                                                        sapa_seq_dict,
                                                        visited)
                if sapa_road_seq:
                    road_seq_dict[road_seq_key] = sapa_road_seq
#                     sapa_seq_dict[node] = sapa_road_seq
                else:
                    if org_facility_id:
                        road_seq_dict[road_seq_key] = org_facility_id
#                         sapa_seq_dict[node] = org_facility_id
                        visited = []
                        self._set_sapa_road_seq(node, facilcls,
                                                org_facility_id,
                                                sapa_node_dict,
                                                sapa_seq_dict,
                                                visited)
                    else:
                        pass
            else:
                if org_facility_id:
                    # 如果原数据有对应的JCT, 那么UTURN改为JCT
                    if(facilcls == HWY_IC_TYPE_UTURN and
                       org_facility_id not in new_ids):
                        road_seq_key = (HWY_IC_TYPE_JCT, inout, inout_link,
                                        to_node, tuple(path))
                    road_seq_dict[road_seq_key] = org_facility_id
                else:
                    if facilcls in (HWY_IC_TYPE_UTURN, HWY_IC_TYPE_TOLL):
                        road_seq_dict[road_seq_key] = next_road_seq
                        # add not matched road seq to org facil dict
                        org_facils_dict[org_facility_key] = next_road_seq
                        new_ids[next_road_seq] = None
                        next_road_seq = next_road_seq + ROAD_SEQ_MARGIN
                    else:  # JCT/IC
                        pass
        return road_seq_dict, next_road_seq

    def _just_ic_facil(self, facil_infos):
        if not facil_infos:
            return False
        for facil_info in facil_infos:
            facilcls = facil_info[0]
            if facilcls != HWY_IC_TYPE_IC:
                return False
        return True

    def _cat_id_map(self, category_id):
        cat_id = category_id
        return cat_id

    def _get_sapa_type(self, sapa_kind):
        facil_cls = None
        if sapa_kind:
            facil_cls = SAPA_TYPE_DICT.get(sapa_kind)
        else:
            facil_cls = HWY_IC_TYPE_PA
        return facil_cls

    def _get_rest_area_info(self):
        '''取得服务情报'''
        sqlcmd = """
        SELECT DISTINCT road_code, road_seq, c.poi_id,
                        kind, name, updown_c
          FROM (
            SELECT road_code, road_seq, facilcls_c, updown_c,
                   regexp_split_to_table(link_lid, E'\\,+')::bigint as link_id
              FROM mid_temp_hwy_ic_path as a
              where facilcls_c in (1, 2) and   -- 1: sa, 2: pa
                    link_lid <> '' and link_lid is not null
          ) as b
          LEFT JOIN mid_temp_poi_link as c
          ON b.link_id = c.link_id
          LEFT JOIN mid_temp_hwy_sapa_name as d
          ON c.poi_id = d.poi_id
          LEFT JOIN org_poi as e  -- org_p: 11year, org_poi: 14year
          ON c.poi_id = e.poi_id::bigint
          where e.kind IN ('8380', '8381', '4101', '4102')
          ORDER BY road_code, road_seq;
        """
        return self.get_batch_data(sqlcmd)

    def get_paths_2_inout(self, facilcls, path, inout):
        '''第一个和一般道连接的点之后，还有Ramp(中国使用)'''
        return [path]
        if facilcls != HWY_IC_TYPE_IC:
            return []
        if inout == HWY_INOUT_TYPE_IN:  # 入口
            reverse = True
        else:
            reverse = False
        pathes = list(self.G.all_path_2_inout(path, reverse=reverse))
        if pathes:
            return pathes
        return [path]

    def _make_inout_join_node(self):
        '''出入口的汇合点'''
        self.log.info('Make In/out join node.')
        self.CreateTable2('mid_temp_hwy_inout_join_node')
        join_node_set = set()
        inout_c = HWY_INOUT_TYPE_OUT
        for out_node_lids, in_node_lids in self._get_inout_join_path():
            f_node_lids = [map(int, n_lid.split(','))
                           for n_lid in out_node_lids]
            t_node_lids = [map(int, n_lid.split(','))
                           for n_lid in in_node_lids]
            # 求出入口路径的重叠点
            overlay_nodes = self._get_overlay_node(f_node_lids, t_node_lids, 1)
            if not overlay_nodes:  # 没有重叠的点
                if not self._get_overlay_node(f_node_lids, t_node_lids, 0):
                    self.log.error('No overlay_nodes. f_node_lids=%s, '
                                   't_node_lids=%s'
                                   % (f_node_lids, t_node_lids))
                continue
            join_nodes = self._get_join_nodes(overlay_nodes,
                                              f_node_lids,
                                              t_node_lids,
                                              inout_c)
            join_node_set = join_node_set.union(join_nodes)
        self._store_inout_join_node(join_node_set)

    def _get_inout_join_path(self):
        '''出入口交汇的路径'''
        sqlcmd = """
        SELECT distinct out_node_lids, in_node_lids
          FROM (
            SELECT distinct
                   a.road_code as out_road_code,
                   a.road_seq as out_road_seq,
                   a.inout_c as out_inout_c,
                   a.node_id as out_node_id,
                   b.road_code as in_road_code,
                   b.road_seq as in_road_seq,
                   b.inout_c as in_inout_c,
                   b.node_id as in_node_id
              FROM (
                -- IC OUT
                SELECT road_code, road_seq,
                       inout_c, node_id, pass_node_id
                  FROM mid_temp_hwy_ic_path_expand_node
                  WHERE facilcls_c = 5 AND inout_c = 2 AND
                        path_type = 'IC'
              ) AS a
              INNER JOIN (
                -- IC IN
                SELECT road_code, road_seq,
                       inout_c, node_id, pass_node_id
                  FROM mid_temp_hwy_ic_path_expand_node
                  WHERE facilcls_c = 5 AND inout_c = 1 AND
                        path_type = 'IC'
              ) as b
              ON a.pass_node_id = b.pass_node_id
          ) as c
          INNER JOIN (
            -- IC OUT
            SELECT road_code, road_seq, inout_c,
                   node_id, array_agg(node_lid) as out_node_lids
              FROM mid_temp_hwy_ic_path
              WHERE facilcls_c = 5 AND inout_c = 2 AND
                path_type = 'IC'
              group by road_code, road_seq, inout_c, node_id
          ) AS d
          ON out_road_code = d.road_code and
             out_road_seq = d.road_seq and
             out_inout_c = d.inout_c and
             out_node_id =  d.node_id
          INNER JOIN (
            -- IC OUT
            SELECT road_code, road_seq, inout_c,
                   node_id, array_agg(node_lid) as in_node_lids
              FROM mid_temp_hwy_ic_path
              WHERE facilcls_c = 5 AND inout_c = 1 AND
                path_type = 'IC'
              group by road_code, road_seq, inout_c, node_id
          ) AS e
          ON in_road_code = e.road_code and
             in_road_seq = e.road_seq and
             in_inout_c = e.inout_c and
             in_node_id =  e.node_id
        """
        return self.get_batch_data(sqlcmd)

    def _matched_org_path(self, node_lid, inout, org_jct_paths):
        '''判断JCT是否有与之匹配的路径'''
        if inout == HWY_INOUT_TYPE_IN:
            node_lid = node_lid[::-1]
        str_node_lid = '|'.join(map(str, node_lid))
        if str_node_lid in org_jct_paths:
            return True
        return False

    def _get_org_jct_path(self):
        sqlcmd = '''
        select distinct passlid
        from mid_hwy_org_hw_jct_merged
        where passlid <>'' and
              passlid is not null
        '''
        list_path = {}
        for row in self.pg.get_batch_data2(sqlcmd):
            passlid = row[0]
            list_path[passlid] = None
        return list_path

    def _filter_JCT_UTurn_SAPA(self):
        '''过滤假JCT/UTurn/SAPA:下了高速、到了出口又转弯回来的径路'''
        self.log.info('Start Filtering JCT/UTurn Path.')
        self.pg.connect1()
        self.pg.connect2()
        self.CreateTable2('mid_temp_hwy_jct_uturn_del')
        del_pathes = set()
        # 取所有专用数据JCT路径
        org_jct_pathes = self._get_org_jct_path()
        for jct_uturn in self._get_jct_uturn():
            gid_array, join_node, path_type = jct_uturn[0:3]
            node_lids, inout = jct_uturn[3:5]
            node_lids = [map(int, n_lid.split(','))
                         for n_lid in node_lids]
            for gid, node_lid in zip(gid_array, node_lids):
                if join_node in node_lid:
                    if path_type not in (HWY_PATH_TYPE_SAPA,):
                        # 原数据有匹配的路径
                        if self._matched_org_path(node_lid, inout,
                                                  org_jct_pathes):
                            continue
                        # 对某条link来说这是唯一路径, 则不过滤
                        if self._is_only_one_path(node_lid, inout, join_node):
                            continue
                        if self._exist_maintain_link(node_lid):
                            continue
                        # 角度<=170度
                        # if self._is_straight(node_lid, inout,
                        #                     join_node, ANGLE_10):
                        #    print 'straight:', join_node, node_lid
                        #    continue
                    # 所有SAPA路径都经过该汇合点
                    if(path_type == HWY_PATH_TYPE_SAPA and
                       self._all_path_pass_join_node(node_lids, join_node) and
                       not self._pass_inout(node_lid, inout, join_node)):
                        continue
#                     if path_type not in (HWY_PATH_TYPE_SAPA,
#                                          HWY_PATH_TYPE_JCT,
#                                          HWY_PATH_TYPE_UTURN
#                                          ):
#                         print node_lid
                    del_pathes.add(gid)
        del org_jct_pathes
        ic_pathes = self._get_filter_ic_by_join_node()
        del_pathes = del_pathes.union(ic_pathes)
        # 删除错误的JCT/UTurn/SAPA
        self._store_del_jct_uturn(del_pathes)
        self.log.info('End Filtering JCT/UTurn Path.')

    def _all_path_pass_join_node(self, node_lids, join_node):
        for node_lid in node_lids:
            if join_node not in node_lid:
                return False
        return True

    def _pass_inout(self, node_lid, inout, join_node):
        # ##角度小于45度
        count = node_lid.count(join_node)
        start_idx = 0
        while count >= 1:
            count -= 1
            join_idx = node_lid.index(join_node, start_idx)
            start_idx = join_idx + 1
            if join_idx <= 0:
                continue
            if join_idx >= len(node_lid) - 1:
                break
            if inout == HWY_INOUT_TYPE_IN:
                reverse = True
            else:
                reverse = False
            if self.G.is_hwy_inout(node_lid[:join_idx + 1], reverse):
                return True
        return False

    def _exist_maintain_link(self, node_lid):
        for u, v in zip(node_lid[:-1], node_lid[1:]):
            if (u, v) in MAINTAIN_LINK_DICT:
                return True
        return False

    def _get_filter_ic_by_join_node(self):
        '''过滤出入口:到达出入口汇合点，又转变回去，再转一圈才出来'''
        sqlcmd = """
        SELECT gids, a.node_id as join_node, node_lids, b.inout_c
          FROM mid_temp_hwy_inout_join_node AS a
          INNER JOIN mid_temp_hwy_ic_path_expand_node as b
          on a.node_id = b.pass_node_id
          INNER JOIN (
            SELECT array_agg(gid) as gids, road_code, road_seq, path_type,
                   node_id, inout_c, array_agg(node_lid) as node_lids
              FROM mid_temp_hwy_ic_path
              WHERE  path_type in ('IC')
              group by road_code, road_seq, node_id, inout_c, path_type
          ) as c
          ON b.road_code = c.road_code and
             b.road_seq = c.road_seq and
             b.node_id = c.node_id and
             b.inout_c = c.inout_c
        """
        del_pathes = set()
        for gids, join_node, node_lids, inout in self.get_batch_data(sqlcmd):
            for gid, node_lid in zip(gids, node_lids):
                node_lid = map(int, node_lid.split(','))
                count = node_lid.count(join_node)
                if count > 1:
                    index = node_lid.index(join_node)
                    cycles = get_simple_cycle(node_lid[index:])
                    for cycle in cycles:
                        if join_node in cycle:
                            del_pathes.add(gid)
                            break
                # ##角度小于45度
                start_idx = 0
                while count >= 1:
                    count -= 1
                    join_idx = node_lid.index(join_node, start_idx)
                    start_idx = join_idx + 1
                    if join_idx <= 0:
                        continue
                    if join_idx >= len(node_lid) - 1:
                        break
                    prev_node = node_lid[join_idx - 1]
                    next_node = node_lid[join_idx + 1]
                    if inout == HWY_INOUT_TYPE_IN:
                        in_edge = (next_node, join_node)
                        out_edge = (join_node, prev_node)
                        reverse = True
                    else:
                        in_edge = (prev_node, join_node)
                        out_edge = (join_node, next_node)
                        reverse = False
                    angle = self.G.get_angle(in_edge, out_edge)
                    if angle <= ANGLE_45 or angle >= (ANGLE_360 - ANGLE_45):
                        del_pathes.add(gid)
                        break
                    if self.G.is_hwy_inout(node_lid[:join_idx+1], reverse):
                        if(angle < ANGLE_100 or
                           angle > (ANGLE_360 - ANGLE_100)):
                            del_pathes.add(gid)
                            break
        return del_pathes

    def _filter_service_road_by_pass_node(self):
        '''过滤经过其他高速本线的辅路路径'''
        self.CreateTable2('mid_temp_hwy_service_road_path_del_by_passnode')
        # 备份删除的辅路路径
        sqlcmd = '''
        INSERT INTO mid_temp_hwy_service_road_path_del_by_passnode(gid,
                                                                   node_lid)
        (
        select distinct a.gid, a.node_lid
        from(
          select distinct gid, road_code, updown_c,
                 node_lid, unnest(pass_nodes[2:array_upper(pass_nodes,1)-1])as pass_node
          from(
            select distinct gid, road_code, updown_c, node_lid,
                   string_to_array(node_lid,',')::bigint[] as pass_nodes
            from mid_temp_hwy_service_road_path1
            ) as path
        )as a
        inner join hwy_link_road_code_info as b
        on a.pass_node = b.node_id and
           (a.road_code <> b.road_code or a.updown_c <> b.updown)
        order by a.gid
        );
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        # 从辅路表中删除路径
        sqlcmd = '''
        delete from mid_temp_hwy_service_road_path1
          where gid in (
              select distinct gid
              from mid_temp_hwy_service_road_path_del_by_passnode
          );
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _filter_service_road(self):
        '''过滤辅路、类辅路， 从辅路路径选出SAPA路径(路径两头都有SAPA设施)'''
        self.CreateTable2('mid_temp_hwy_service_road_path2')
        # 1. 删除经过 出入口汇合点的 类辅路
        self._filter_service_road_by_join_node()
        # 删除经过其他高速本线的辅路路径
        self._filter_service_road_by_pass_node()
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
        self.pg.commit1()

    def _filter_service_road_by_join_node(self):
        '''删除经过 出入口汇合点的 类辅路。'''
        # 1.先备份
        self.CreateTable2('mid_temp_hwy_service_road_path_del')
        sqlcmd = """
        INSERT INTO mid_temp_hwy_service_road_path_del(gid, node_lid)
        (
            SELECT distinct gid, node_lid
              FROM mid_temp_hwy_inout_join_node AS a
              INNER JOIN (
               SELECT gid, regexp_split_to_table(node_lid,
                                           ',')::bigint as pass_node_id,
                      node_lid
             FROM mid_temp_hwy_service_road_path1
              ) as b
              ON a.node_id = b.pass_node_id
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        # 再删除
        sqlcmd = """
        DELETE FROM mid_temp_hwy_service_road_path1 as a
          where a.gid in (
              SELECT gid
               from mid_temp_hwy_service_road_path_del as b
          );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _get_join_nodes(self, overlay_nodes, f_node_lids,
                        t_node_lids, f_inout):
        '''出入口路径的汇合点，像 Y/X字路的汇合点。'''
        join_nodes = set()
        if len(overlay_nodes) <= 1:
            join_nodes = set(overlay_nodes)
            return join_nodes
        for f_node_lid in f_node_lids:
            # 取当前路径的重叠点
            curr_overlay_nodes = overlay_nodes.intersection(f_node_lid)
            # 路径里没有重叠点
            if not curr_overlay_nodes:
                continue
            has_overlay = False
            overlay_sec = []
            node_idx = len(f_node_lid) - 1
            # ## 取最尾部那段重叠路径的第一个点
            while node_idx > 0:
                v = f_node_lid[node_idx]
                u = f_node_lid[node_idx - 1]
                if f_inout == HWY_INOUT_TYPE_OUT:
                    data = self.G[u][v]
                else:
                    data = self.G[v][u]
                # ramp link
                link_type = data.get(HWY_LINK_TYPE)
                if link_type != HWY_LINK_TYPE_RAMP:
                    break
                # ramp link
                link_type = data.get(HWY_LINK_TYPE)
                if link_type != HWY_LINK_TYPE_RAMP:
                    break
                if v in curr_overlay_nodes:
                    overlay_sec.append(v)
                    has_overlay = True
                else:
                    if has_overlay:  # 重叠点必须连续
                        break
                node_idx -= 1
            # 判断X型
            # 求最大重合段
            max_overlay = self._max_overlay_node(overlay_nodes,
                                                 overlay_sec,
                                                 t_node_lids)
            join_nodes = join_nodes.union(max_overlay)
        return join_nodes

    def _max_overlay_node(self, overlay_nodes, overlay_sec, t_node_lids):
        max_overlay = set()
        if not overlay_sec:
            return max_overlay
        for node_lid in t_node_lids:
            node_cnt = len(node_lid) - 1
            while node_cnt >= 0:
                node = node_lid[node_cnt]
                if node in overlay_nodes:
                    if node == overlay_sec[0]:
                        max_overlay.add(node)
                        i = 1
                        while i < len(overlay_sec) and node_cnt - i >= 0:
                            node = node_lid[node_cnt - i]
                            if overlay_sec[i] == node:
                                max_overlay.add(node)
                            else:
                                break
                            i += 1
                    break
                node_cnt -= 1
        return max_overlay

    def _get_overlay_node(self, f_node_lids, t_node_lids, s_idx=1):
        '''重叠点'''
        f_set = set()
        t_set = set()
        for node_lid in f_node_lids:
            f_set = f_set.union(node_lid[s_idx:])
        for node_lid in t_node_lids:
            t_set = t_set.union(node_lid[s_idx:])
        return f_set.intersection(t_set)

    def _only_uturn_path(self, node_id, road_code, updown, inout_c):
        ''' 除了SAPA路径只剩下UTURN路径'''
        sqlcmd = '''
        select array_agg(path_type)
        from mid_temp_hwy_ic_path
        where road_code = %s and
              updown_c = %s and
              node_id = %s and
              inout_c = %s and
              facilcls_c not in (1, 2)
        group by road_code, updown_c, node_id, inout_c
        '''
        for row in self.pg.get_batch_data2(sqlcmd, (road_code,
                                                    updown,
                                                    node_id,
                                                    inout_c)):
            path_type_list = row[0]
            if set(path_type_list) == set([HWY_PATH_TYPE_UTURN]):
                return True
        return False

    def _pre_filter_sapa(self):
        '''过滤SAPA: '''
        self.CreateTable2('mid_temp_hwy_sapa_del')
        self.pg.connect1()
        del_sapa_list = []
        for info in self._get_filter_sapa():
            array_gid, road_code, node_id = info[0:3]
            to_node_id = info[3]
            # node_lids, link_lids = info[4:6]
            inout_c, path_type, updown = info[6:9]
            # 同侧有配对SAPA HWY Node
            if self._match_sapa_node(node_id, road_code, updown, inout_c):
                continue  # 保留SAPA设施
            else:
                # 出口，路的终点
                if(inout_c == HWY_INOUT_TYPE_OUT and
                   self.hwy_data.is_road_end_node(road_code, updown,
                                                  node_id)):
                    if path_type == HWY_PATH_TYPE_UTURN:
                        continue  # 保留SAPA设施
                # 入口，路的起点
                elif(inout_c == HWY_INOUT_TYPE_IN and
                     self.hwy_data.is_road_start_node(road_code, updown,
                                                      node_id)):
                    if path_type == HWY_PATH_TYPE_UTURN:
                        continue  # 保留SAPA设施
                # ## to_node_id同侧是否有配对SAPA HWY Node
                t_sapa_flag = False
                t_inout_c = HWY_INOUT_TYPE_OUT
                if inout_c == HWY_INOUT_TYPE_OUT:
                    t_inout_c = HWY_INOUT_TYPE_IN
                for t_roadinfo in self._get_roadcode_by_node(to_node_id,
                                                             t_inout_c):
                    t_roadcode, t_updwon = t_roadinfo[0:2]
                    if self._match_sapa_node(to_node_id, t_roadcode,
                                             t_updwon, t_inout_c):
                        t_sapa_flag = True
                        break
                # 若当前点除了SAPA路径只剩下UTURN路径，则不过滤
                if self._only_uturn_path(node_id, road_code, updown, inout_c):
                    continue
                # to_node_id同侧有配对SAPA HWY Node
                if t_sapa_flag:
                    del_sapa_list += array_gid
                else:
                    # 两边都没有 配对SAPA HWY Node
                    if path_type == HWY_PATH_TYPE_JCT:
                        self.log.info('No SAPA HWY Node in Both Side.'
                                      'node=%s,to_node=%s' %
                                      (node_id, to_node_id))
        self._store_del_sapa(del_sapa_list)

    def _store_no_org_sapa_del(self, del_sapa_list):
        '''备份删除的sapa设施'''
        sqlcmd = '''
        INSERT INTO mid_temp_hwy_sapa_no_org_del(gid, road_code, road_seq,
                                                 facilcls_c, inout_c,
                                                 node_id, to_node_id,
                                                 node_lid, link_lid,
                                                 path_type)
        (
            select gid, road_code, road_seq, facilcls_c, inout_c, node_id,
                   to_node_id, node_lid, link_lid, path_type
            from mid_temp_hwy_ic_path
            where gid = %s
        )
        '''
        for gid in del_sapa_list:
            self.pg.execute2(sqlcmd, (gid,))
        self.pg.commit2()
        # 从mid_temp_hwy_ic_path中删除设施
        sqlcmd = '''
        DELETE FROM mid_temp_hwy_ic_path
          WHERE gid in (
          SELECT gid
            FROM mid_temp_hwy_sapa_no_org_del
          )
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _store_sapa_to_service_route(self, service_sapa_list):
        sqlcmd = '''
        INSERT INTO mid_temp_hwy_service_road_path1(
                                              inout_c, node_id, to_node_id,
                                              node_lid, link_lid, road_code,
                                              updown_c)
        (
            select inout_c, node_id, to_node_id, node_lid,
                   link_lid, road_code, updown_c
              from mid_temp_hwy_ic_path
              where gid = %s
        );
        DELETE FROM mid_temp_hwy_ic_path
          WHERE gid = %s
        '''
        for gid in service_sapa_list:
            self.pg.execute2(sqlcmd, (gid, gid))
        self.pg.commit2()

    def _get_matched_sapa(self, link_lid_str, inout_c, node):
        matched_gid = None
        sqlcmd = '''
        select gid
        from mid_temp_hwy_ic_path
        where path_type = 'SAPA' and
              link_lid = %s and inout_c = %s
        '''
        link_lid = link_lid_str.split(',')
        link_lid.reverse()
        link_lid_reverse = ','.join(link_lid)
        if inout_c == HWY_INOUT_TYPE_IN:
            matched_inout = HWY_INOUT_TYPE_OUT
        elif inout_c == HWY_INOUT_TYPE_OUT:
            matched_inout = HWY_INOUT_TYPE_IN
        else:
            self.log.error('no matched inout')
            return matched_gid
        gid_list = list()
        for row in self.pg.get_batch_data2(sqlcmd, (link_lid_reverse,
                                                    matched_inout)):
            gid = row[0]
            gid_list.append(gid)
        if len(gid_list) > 1:
            self.log.error('more than one matched sapa')
            return None
        elif len(gid_list) < 1:
            self.log.error('no matched sapa.node:%s' % node)
            return None
        matched_gid = gid_list[0]
        return matched_gid

    def _filter_no_org_sapa(self):
        '''过滤匹配不上专用数据的sapa设施 '''
        self.CreateTable2('mid_temp_hwy_sapa_no_org_del')
        del_sapa_list = list()
        service_sapa_list = list()
        sqlcmd = '''
        select distinct gid, a.road_code, a.inout_c,
               a.node_id, a.to_node_id, a.link_lid
        from
        (
              select distinct gid, road_code, updown_c, road_seq,
                     facilcls_c, inout_c, node_id, to_node_id,
                     node_lid, link_lid, path_type
              from mid_temp_hwy_ic_path
              where path_type = 'SAPA'
        )  AS a
        LEFT JOIN
        (
               select hw_pid, accesstype, nodeid
               from mid_hwy_org_hw_junction_mid_linkid
               where attr in (1, 2)
        )as b
        on a.node_id = b.nodeid and
           a.road_code = b.hw_pid and
           a.inout_c = b.accesstype
        where b.nodeid is null
        '''
        for row in self.pg.get_batch_data2(sqlcmd):
            (gid, road_code, inout_c, node_id, to_node_id, link_lid) = row
            if inout_c == HWY_INOUT_TYPE_IN:
                edges_iter = self.G.in_edges_iter(to_node_id, True)
            elif inout_c == HWY_INOUT_TYPE_OUT:
                edges_iter = self.G.out_edges_iter(to_node_id, True)
            else:
                self.log.error('no inout_c,node=%s' % node_id)
                continue
            same_road_flag = False
            for u, v, data in edges_iter:
                to_road_code = data.get(HWY_ROAD_CODE)
                if to_road_code and road_code == to_road_code:
                    same_road_flag = True
                    break
            matched_gid = self._get_matched_sapa(link_lid, inout_c, node_id)
            if same_road_flag:       
                service_sapa_list.append(gid)
                if matched_gid:
                    service_sapa_list.append(matched_gid)
            else:
                del_sapa_list.append(gid)
                if matched_gid:
                    del_sapa_list.append(matched_gid)
        self._store_no_org_sapa_del(del_sapa_list)
        self._store_sapa_to_service_route(service_sapa_list)
        self.pg.commit2()

    def _filter_sapa(self):
        '''过滤SAPA: '''
        self.log.info('Start Filter SAPA.')
        self.CreateTable2('mid_temp_hwy_sapa_no_org_del')
        # self.CreateTable2('mid_temp_hwy_sapa_del')
        self._pre_filter_sapa()
        self.pg.connect1()
        # 过滤匹配不上专用数据的sapa设施
        # 若是同侧设施则做成辅路，若是异侧则删除。
        self._filter_no_org_sapa()
        # 备份要删除的路径
        # ## 备份路径情报
        sqlcmd = """
        INSERT INTO mid_temp_hwy_sapa_del(gid, road_code, road_seq,
                                          facilcls_c, inout_c, node_id,
                                          to_node_id, node_lid, link_lid,
                                          path_type)
        (
        SELECT gid, road_code, road_seq,
               facilcls_c, inout_c, node_id,
               to_node_id, node_lid, link_lid,
                path_type
          FROM mid_temp_hwy_ic_path
          where facilcls_c in (1, 2) and road_seq = %s
        union

        SELECT gid, road_code, road_seq,
               facilcls_c, inout_c, a.node_id,
               to_node_id, node_lid, link_lid,
               path_type
         FROM mid_temp_hwy_ic_path as a
         INNER JOIN (
            SELECT node_id
              FROM mid_temp_hwy_ic_path
              where facilcls_c in (1, 2) and road_seq = %s
          ) AS b
          on a.to_node_id = b.node_id and path_type = 'SAPA'
        );
        """
        self.pg.execute2(sqlcmd, (HWY_UNKOWN_ID_NI, HWY_UNKOWN_ID_NI))
        self.pg.commit2()
        # ## 路径从mid_temp_hwy_ic_path表中删去
        sqlcmd = """
        DELETE FROM mid_temp_hwy_ic_path
          WHERE gid in (
          SELECT gid
            FROM mid_temp_hwy_sapa_del
          )
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.log.info('End Filter SAPA.')

    def _get_toll_index(self, path):
        node_idx = len(path) - 1
        while node_idx >= 0:
            if self.G.is_tollgate(path[node_idx]):
                return node_idx
            node_idx -= 1
        return node_idx

    def _get_service_types(self, cat_id_list):
        gas_station, information, rest_area = HWY_FALSE, HWY_FALSE, HWY_FALSE
        shopping_corner, postbox, atm = HWY_FALSE, HWY_FALSE, HWY_FALSE
        restaurant, toilet = HWY_FALSE, HWY_FALSE
        if self.poi_cate:
            for cat_id in cat_id_list:
                if cat_id in self.gas_station_dict:
                    gas_station = HWY_TRUE
                elif cat_id in self.info_dict:
                    information = HWY_TRUE
                elif cat_id in self.rest_area_dict:
                    rest_area = HWY_TRUE
                elif cat_id in self.shopping_corner_dict:
                    shopping_corner = HWY_TRUE
                elif cat_id in self.post_box_dict:
                    postbox = HWY_TRUE
                elif cat_id in self.atm_dict:
                    atm = HWY_TRUE
                elif cat_id in self.restaurant_dict:
                    restaurant = HWY_TRUE
                elif cat_id in self.toilet_dict:
                    toilet = HWY_TRUE
                elif cat_id in self.undefined_dict:
                    continue
                else:
                    self.log.error('Unknown Category ID. cat_id=%s' % cat_id)
        return (gas_station, information, rest_area,
                shopping_corner, postbox, atm,
                restaurant, toilet)

    def _convert_service_road_2_jct(self):
        pass

    def _get_sapa_link(self):
        return []

    def _load_poi_category(self):
        self.rest_area_dict = SERVICE_REST_AREA
        self.shopping_corner_dict = SERVICE_SHOPPING_CONNER
        self.restaurant_dict = SERVICE_RESTAURANT
        self.gas_station_dict = SERVICE_GAS_DICT
        self.toilet_dict = SERVICE_TOILET

    def _get_jct_uturn(self):
        '''取得经过in/out汇合点的路径'''
        sqlcmd = """
        SELECT gids, a.node_id as join_node, c.path_type,
               node_lids, b.inout_c
          FROM mid_temp_hwy_inout_join_node AS a
          INNER JOIN mid_temp_hwy_ic_path_expand_node as b
          on a.node_id = b.pass_node_id
          INNER JOIN (
            SELECT array_agg(gid) as gids, road_code, road_seq, path_type,
                   node_id, inout_c, array_agg(node_lid) as node_lids
              FROM mid_temp_hwy_ic_path
              WHERE  path_type in ('JCT', 'UTURN', 'SAPA') or
                     facilcls_c = 12
              group by road_code, road_seq, node_id, inout_c, path_type
          ) as c
          ON b.road_code = c.road_code and
             b.road_seq = c.road_seq and
             b.node_id = c.node_id and
             b.inout_c = c.inout_c;
        """
        return self.get_batch_data(sqlcmd, batch_size=1024)

    def _store_del_jct_uturn(self, del_pathes):
        sqlcmd = """
        INSERT INTO mid_temp_hwy_jct_uturn_del(gid)
           VALUES(%s);
        """
        for gid in del_pathes:
            self.pg.execute2(sqlcmd, (gid,))
        self.pg.commit2()
        # ## 备份路径情报
        sqlcmd = """
        UPDATE mid_temp_hwy_jct_uturn_del AS a
           SET road_code=b.road_code, road_seq = b.road_seq,
               facilcls_c = b.facilcls_c, inout_c = b.inout_c,
               node_id = b.node_id, to_node_id = b.to_node_id,
               node_lid = b.node_lid, link_lid = b.link_lid,
               path_type = b.path_type
          FROM mid_temp_hwy_ic_path as b
          where a.gid = b.gid;
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        # ## 路径从mid_temp_hwy_ic_path表中删去
        sqlcmd = """
        DELETE FROM mid_temp_hwy_ic_path
          WHERE gid in (
          SELECT gid
            FROM mid_temp_hwy_jct_uturn_del
          )
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _get_ic_out_name(self, path):
        '''出口设施名称'''
        pass

    def _get_ic_in_name(self, path):
        '''入口设施名称'''
        node_cnt = len(path) - 1
        while node_cnt >= 0:
            last_node = path[node_cnt]
            enter_name = self.G.get_enter_poi_name(last_node)
            if enter_name:
                return enter_name
            node_cnt -= 1
        return ''

    def _make_hwy_node(self):
        self.log.info('Start Make Highway Node')
        self.CreateTable2('hwy_node')
        sqlcmd = """
        INSERT INTO hwy_node(road_code, road_seq, updown_c,
                             facilcls_c, inout_c, tollcls_c,
                             node_id, facil_name, the_geom)
        (
            SELECT distinct a.road_code, a.road_seq, a.updown_c,
                   facilcls_c, inout_c, 0 as tollcls_c, -- None
                   a.node_id, b.facil_name, the_geom
              FROM  (
                 select distinct road_code, updown_c, road_seq,
                    node_id, facilcls_c, inout_c
                 from mid_temp_hwy_ic_path
                 where facilcls_c <> 10
              ) a
              LEFT JOIN mid_temp_hwy_facil_name as b
              ON a.road_code = b.road_code and
                 a.updown_c = b.updown_c and
                 a.road_seq = b.road_seq
              LEFT JOIN node_tbl as c
              ON a.node_id = c.node_id
              ORDER BY road_code, road_seq
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('hwy_node_the_geom_idx')
        self.CreateIndex2('hwy_node_road_code_road_seq'
                          '_facilcls_c_inout_c_node_id_idx')
        self.log.info('End Make Highway Node')

    def _make_facil_name(self):
        '''制作设施名称'''
        self.log.info('Start Make Facil Name.')
        self.pg.connect1()
        self.CreateTable2('mid_temp_hwy_facil_name')
        sqlcmd = """
        INSERT INTO mid_temp_hwy_facil_name(road_code, updown_c,
                                            road_seq, facil_name)
        (
          select distinct a.road_code, a.updown_c, a.road_seq,
                 b.name as facil_name
          from mid_temp_hwy_ic_path as a
          inner join temp_hwy_facil_name_ni as b
          on a.road_seq = b.junction_id
          inner join mid_hwy_org_hw_junction_mid_linkid_merged as c
          on a.road_code = c.hw_pid and b.junction_id = c.id
          where b.name is not null
          )
        """
        self.pg.execute1(sqlcmd)
        self.pg.commit1()
        # 插入收费站名称
        sqlcmd = '''
        INSERT INTO mid_temp_hwy_facil_name(road_code, updown_c,
                                            road_seq, facil_name)
        (
            SELECT road_code, updown_c, road_seq, t.name as facil_name
            from mid_temp_hwy_ic_path as ic
            left join hwy_tollgate_name as t
            on ic.node_id = t.node_id
            where facilcls_c = 6 and t.name is not null
        )
        '''
        self.pg.execute1(sqlcmd)
        self.pg.commit1()
        self.CreateIndex2('mid_temp_hwy_facil_name_road_code_road_seq_idx')
        self.log.info('End Make Facility Name.')

    def _store_inout_join_node(self, join_nodes):
        sqlcmd = """INSERT INTO mid_temp_hwy_inout_join_node values(%s)"""
        for node in join_nodes:
            self.pg.execute2(sqlcmd, (node,))
        self.pg.commit2()

    def _get_same_facils(self, facil_list, facil_idx, curr_facil):
        same_facils = []
        # 取对应的入口设施
        in_idx = self._get_facil_in_idx(facil_list, facil_idx)
        if in_idx:  # 出口到入口之间的所有设施都是并设
            same_facils = facil_list[(facil_idx + 1):(in_idx + 1)]
        else:
            # 同点的并设
            same_facils = self._get_same_node_facil(facil_list,
                                                    facil_idx,
                                                    curr_facil)
        if in_idx:
            in_facil_info = facil_list[in_idx]
            same_facils += self._get_same_node_facil(facil_list,
                                                     in_idx,
                                                     in_facil_info)
#         org_same_facils = self._get_org_same_facil(facil_list,
#                                                    facil_idx,
#                                                    curr_facil)
#         for facil in org_same_facils:
#             if facil not in same_facils:
#                 same_facils.append(facil)
        # ## 求并设的并设
        for same_idx in range(0, len(same_facils)):
            same_facil = same_facils[same_idx]
            same_facil_idx = facil_idx + same_idx + 1
            c_same_facils = self._get_same_facils(facil_list,
                                                  same_facil_idx,
                                                  same_facil)
            for facil in c_same_facils:
                if facil not in same_facils:
                    same_facils.append(facil)
        return same_facils

    def _make_hwy_store(self):
        '''店铺情报'''
        self.log.info('Start Make Facility Stores.')
        self.CreateTable2('hwy_store')
        sqlcmd = '''
        select distinct road_code, road_seq, updown_c,
               cat as cat_id, 0 as distance
        from hwy_node as a
        left join
        (
        select distinct id,
               regexp_split_to_table(estab_item, '\\\\|+') as cat
         from mid_hwy_org_hw_junction_mid_linkid_merged
         where estab_item <> '' and estab_item is not null
        )as b
        on a.road_seq = b.id
        where a.facilcls_c in (1, 2) and
              b.cat in ('4081', '4082', '4083', '4084', '4085') --'4080'不是品牌
        '''
        self.pg.connect1()
        for row in self.get_batch_data(sqlcmd):
            (road_code, road_seq, updown_c, cat_id, distance) = row
            service_kind = self._get_service_kind(cat_id)
            if cat_id not in GAS_CHAIN_DICT:
                self.log.warning('no chain id. cat=%s' % cat_id)
                continue
            info = GAS_CHAIN_DICT.get(cat_id)  # 中国HWY专用数据中只有加油站店铺
            if not info:
                continue
            kind, chain_id = info
            if not chain_id:
                continue
            self._insert_into_store_info((road_code, road_seq, updown_c, kind,
                                          chain_id, distance, service_kind))
        self.pg.commit1()
        self.log.info('End Make Facility Stores.')

    def _make_hwy_service(self):
        '''服务情报'''
        self.log.info('Start Make Facility Service.')
        self.CreateTable2('hwy_service')
        sqlcmd = """
        select distinct road_code, road_seq, updown_c,
               b.estab_item as cat_ids
        from hwy_node as a
        left join mid_hwy_org_hw_junction_mid_linkid_merged as b
        on a.road_seq = b.id
        where a.facilcls_c in (1, 2) and
              estab_item <> '' and
              estab_item is not null
        """
        self.pg.connect1()
        for service_info in self.get_batch_data(sqlcmd):
            road_code, road_seq, updown, cat_ids = service_info
            if not cat_ids:
                self.log.warning('no service type in service area')
                cat_id_list = []
            else:
                cat_id_list = cat_ids.split('|')
            service_types = self._get_service_types(cat_id_list)
            # 服务标志都为HWY_FALSE时，不收录
            if set(service_types) == set([HWY_FALSE]):
                continue
            self._store_service_info(road_code, road_seq,
                                     updown, service_types)
        self.pg.commit1()
        self.log.info('End Make Facility Service.')

    def _filter_service_road_by_tollgate(self):
        '''辅路经过多个收费站，且link都已经在IC Path里。'''
        self.log.info('Filter Service Road by Tollgate.')
        self.CreateTable2('mid_temp_hwy_service_road_path_del_by_toll')
        sqlcmd = """
        INSERT INTO mid_temp_hwy_service_road_path_del_by_toll(gid, node_lid)
        (
        SELECT d.gid, d.node_lid
          FROM (
            SELECT gid, node_lid,
                   array_agg(pass_node_id) AS toll_nodes, array_agg(toll_flag)
              FROM (
                SELECT distinct a.gid, pass_node_id, toll_flag, a.node_lid
                  FROM (
                    SELECT gid, node_id, to_node_id, node_lid,
                           regexp_split_to_table(node_lid, E'\\\\,+'
                                                 )::bigint as pass_node_id
                      FROM mid_temp_hwy_service_road_path2
                 ) as a
                 INNER JOIN node_tbl as b
                 ON pass_node_id = b.node_id and toll_flag = 1
                 where a.node_id <> pass_node_id AND
                       a.to_node_id <> pass_node_id
                 ORDER BY a.gid, pass_node_id, toll_flag
              ) AS c
              group by gid, node_lid
              having count(*) > 1
         ) AS d
         LEFT JOIN (
            SELECT distinct a.gid
             FROM (
                SELECT gid, regexp_split_to_table(link_lid, E'\\\\,+'
                                                  )::bigint as pass_link_id
                  FROM mid_temp_hwy_service_road_path2
             ) as a
             LEFT JOIN mid_temp_hwy_ic_path_expand_link AS b
             ON a.pass_link_id = b.pass_link_id
             where b.pass_link_id is null
          ) AS e
          ON d.gid = e.gid
          WHERE e.gid is null
          order by d.gid
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        # 从原来的辅路表中删除
        sqlcmd = """
        DELETE FROM mid_temp_hwy_service_road_path2
          where gid in (
            SELECT gid
              from mid_temp_hwy_service_road_path_del_by_toll
          );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _filter_service_road_by_inout(self):
        '''辅路经过出入口(Ramp和一道的交点)'''
        self.CreateTable2('mid_temp_hwy_service_road_path_del_by_inout')
        sqlcmd = """
        INSERT INTO mid_temp_hwy_service_road_path_del_by_inout(gid, node_lid)
        (
        SELECT c.gid, c.node_lid
          FROM (--辅路经过出入口
            SELECT distinct a.gid, a.node_lid
              FROM (
                SELECT gid, node_lid,
                       regexp_split_to_table(node_lid, E'\\\\,+'
                                             )::bigint as pass_node_id
                  FROM mid_temp_hwy_service_road_path2
             ) as a
             INNER JOIN (
                SELECT distinct to_node_id
                  FROM mid_temp_hwy_ic_path
                  WHERE path_type = 'IC'
             ) as b
             ON pass_node_id = b.to_node_id
             ORDER BY a.gid
          ) AS c
          LEFT JOIN (--有link未出现在IC link
            SELECT distinct a.gid
             FROM (
                SELECT gid, regexp_split_to_table(link_lid, E'\\\\,+'
                                                  )::bigint as pass_link_id
                  FROM mid_temp_hwy_service_road_path2
             ) as a
             LEFT JOIN mid_temp_hwy_ic_path_expand_link AS b
             ON a.pass_link_id = b.pass_link_id
             where b.pass_link_id is null
          ) AS d
          ON c.gid = d.gid
          LEFT JOIN ( -- 经过都市link
            SELECT distinct a.gid
             FROM (
                SELECT gid, regexp_split_to_table(link_lid, E'\\\\,+'
                                                  )::bigint as pass_link_id
                  FROM mid_temp_hwy_service_road_path2
             ) as a
             INNER JOIN link_tbl as b
             ON a.pass_link_id = b.link_id and road_type = 1
          ) as e
          ON c.gid = e.gid
          where d.gid is null or e.gid is not null
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit()
        # 从原来的辅路表中删除
        sqlcmd = """
        DELETE FROM mid_temp_hwy_service_road_path2
          where gid in (
            SELECT gid
              from mid_temp_hwy_service_road_path_del_by_inout
          );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _filter_urban_path(self):
        '''过滤经过都市高速的设施路径'''
        del_pathes = set()
        self.pg.connect1()
        for urban_facil in self._get_urban_facil():
            road_code, road_seq, updown = urban_facil[0:3]
            facilcls, inout, node_id = urban_facil[3:6]
            urban_links, path_types, gids = urban_facil[6:9]
            node_lids, link_lids, other_road_seqs = urban_facil[9:12]
            other_facilclss, other_node_lids = urban_facil[12:14]
            urban_links = set(urban_links)
            for(gid, node_lid,
                link_lid, path_type) in zip(gids, node_lids,
                                            link_lids, path_types):
                node_lid = map(int, node_lid.split(','))
                link_lid = map(int, link_lid.split(','))
                urban_pos = self._get_urban_link_pos(link_lid, urban_links)
                if urban_pos < 0:
                    continue
                if urban_pos == 0:
                    del_pathes.add(gid)
                    continue
                new_node_lid = node_lid[0:urban_pos+1]
                if path_type == HWY_PATH_TYPE_IC:
                    del_pathes.add(gid)
                    self._store_facil_path(road_code, road_seq, facilcls,
                                           inout, new_node_lid, path_type,
                                           updown)
                elif path_type == HWY_PATH_TYPE_UTURN:
                    facil = self._match_facil_for_urban(other_road_seqs,
                                                        other_facilclss,
                                                        other_node_lids,
                                                        new_node_lid)
                    if facil:
                        temp_road_seq, temp_facilcls = facil
                        # 强制使用IC Path Type
                        temp_path_type = IC_PATH_TYPE_DICT.get(HWY_IC_TYPE_IC)
                        self._store_facil_path(road_code, temp_road_seq,
                                               temp_facilcls, inout,
                                               new_node_lid, temp_path_type,
                                               updown)
                    else:
                        temp_facilcls = HWY_IC_TYPE_IC
                        temp_road_seq = HWY_UNKOWN_ID_NI
                        temp_path_type = IC_PATH_TYPE_DICT.get(temp_facilcls)
                        self._store_facil_path(road_code, temp_road_seq,
                                               temp_facilcls, inout,
                                               new_node_lid, temp_path_type,
                                               updown)
                    del_pathes.add(gid)
                elif path_type == HWY_PATH_TYPE_SAPA:
                    self.log.warning('Urban link in SAPA. node=%s, link_lid=%s'
                                     % (node_id, link_lid))
                elif path_type == HWY_PATH_TYPE_JCT:
                    self.log.error('Urban link in JCT. node=%s, link_lid=%s'
                                   % (node_id, link_lid))
                else:
                    self.log.warning('Urban link in Unkown Facility.'
                                     'node=%s, facilcls=%s, link_lid=%s'
                                     % (node_id, facilcls, link_lid))
        self.pg.commit1()
        self._store_urban_path(del_pathes)

    def _get_urban_link_pos(self, link_lid, urban_links):
        urban_pos = -1
        link_cnt = 0
        while link_cnt < len(link_lid):
            link_id = link_lid[link_cnt]
            if link_id in urban_links:
                urban_pos = link_cnt
                break
            link_cnt += 1
        return urban_pos

    def _match_facil_for_urban(self, other_road_seqs, other_facilclss,
                               other_node_lids, new_node_lid):
        ic_facils = set()
        for(road_seq, facilcls,
            node_lid) in zip(other_road_seqs, other_facilclss,
                             other_node_lids):
            node_lid = map(int, node_lid.split(','))
            if node_lid[0:2] == new_node_lid[0:2]:
                return road_seq, facilcls
            ic_facils.add(road_seq, facilcls)
        if ic_facils:
            min_facil = ic_facils.pop()
            for facil in ic_facils:
                if facil[0] < min_facil[0]:
                    min_facil = facil
            return min_facil
        return None

    def _get_urban_facil(self):
        sqlcmd = """
        SELECT a.road_code, a.road_seq, a.updown_c,
               a.facilcls_c, a.inout_c, a.node_id,
               a.urban_links, b.path_types, b.gids,
               b.node_lids, link_lids, other_road_seqs,
               other_facilclss, other_node_lids
          FROM (
            SELECT road_code, road_seq, updown_c,
                   facilcls_c, inout_c, node_id,
                   array_agg(pass_link_id) urban_links
              FROM mid_temp_hwy_ic_path_expand_link as a
              INNER JOIN link_tbl as b
              on pass_link_id = b.link_id and road_type = 1
              group by road_code, road_seq, updown_c,
                       facilcls_c, inout_c, node_id
          ) AS a
          LEFT JOIN (
            SELECT road_code, road_seq, updown_c,
                   facilcls_c, inout_c, node_id,
                   array_agg(gid) gids,
                   array_agg(node_lid) as node_lids,
                   array_agg(link_lid) as link_lids,
                   array_agg(path_type) as path_types
              FROM mid_temp_hwy_ic_path
              group by road_code, road_seq, updown_c,
                   facilcls_c, inout_c, node_id
          ) AS b
          ON a.road_code = b.road_code and
             a.road_seq = b.road_seq and
             a.updown_c = b.updown_c and
             a.facilcls_c = b.facilcls_c and
             a.inout_c = b.inout_c and
             a.node_id = b.node_id
          LEFT JOIN (
             SELECT node_id,
                    inout_c other_inout,
                    array_agg(gid) gids,
                    road_code other_road_code,
                    array_agg(road_seq) other_road_seqs,
                    array_agg(updown_c) other_updowns,
                   array_agg(facilcls_c) other_facilclss,
                   array_agg(node_lid) as other_node_lids
              FROM mid_temp_hwy_ic_path
              WHERE path_type = 'IC'
              group by road_code, node_id, inout_c
          ) as c
          ON a.road_code =c.other_road_code and
             a.node_id = c.node_id and
             a.inout_c = c.other_inout
          order by a.road_code, a.road_seq, a.updown_c,
                   a.facilcls_c, a.inout_c, a.node_id
        """
        return self.get_batch_data(sqlcmd)

    def _store_urban_path(self, del_pathes):
        self.CreateTable2('mid_temp_hwy_urban_path_del')
        sqlcmd = """
        INSERT INTO mid_temp_hwy_urban_path_del(gid)
           VALUES(%s);
        """
        for gid in del_pathes:
            self.pg.execute2(sqlcmd, (gid,))
        self.pg.commit2()
        # ## 备份路径情报
        sqlcmd = """
        UPDATE mid_temp_hwy_urban_path_del AS a
           SET road_code=b.road_code, road_seq = b.road_seq,
               facilcls_c = b.facilcls_c, inout_c = b.inout_c,
               node_id = b.node_id, to_node_id = b.to_node_id,
               node_lid = b.node_lid, link_lid = b.link_lid,
               path_type = b.path_type, updown_c = b.updown_c
          FROM mid_temp_hwy_ic_path as b
          where a.gid = b.gid;
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        # ## 路径从mid_temp_hwy_ic_path表中删去
        sqlcmd = """
        DELETE FROM mid_temp_hwy_ic_path
          WHERE gid in (
          SELECT gid
            FROM mid_temp_hwy_urban_path_del
          )
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _uturn_in_other_path(self, path, node, to_node):
        # 判断UTURN路径是否被起末点的其他设施路径包含
        sqlcmd = '''
        select node_id, array_agg(node_lid)
        from mid_temp_hwy_ic_path
        where (node_id = %s or node_id = %s) and
              facilcls_c not in (3, 10)--3:JCT 10:UTURN
        group by node_id
        '''
        path = map(str, path)
        path_str = ','.join(path)
        for row in self.pg.get_batch_data2(sqlcmd, (node, to_node)):
            path_list = row[1]
            for path_info in path_list:
                path_node_list = path_info.split(',')
                path_node_list.reverse()
                path_info_reverse = ','.join(path_node_list)
                if(path_str in path_info or
                   path_str in path_info_reverse):
                    return True
        return False


# ===============================================================================
# ===============================================================================
GAS_CHAIN_DICT = {'4081': ('230215', '2001'),  # 中国石油
                  '4082': ('230215', '2002'),  # 中国石化
                  '4083': ('230215', '2003'),  # 壳牌
                  '4084': ('230215', '2004'),  # 美孚
                  '4085': ('230215', '2005'),  # 英国石油BP
                  }
SERVICE_RESTAURANT = {
                      '1300': 1  # 餐馆
                      }
SERVICE_SHOPPING_CONNER = {
                           '2200': 1  # 商店
                           }
SERVICE_REST_AREA = {
                     '5380': 1  # 旅馆
                     }
SERVICE_GAS_DICT = {'4080': 1,  # 加油站
                    '4081': 1,  # 中国石油
                    '4082': 1,  # 中国石化
                    '4083': 1,  # 壳牌
                    '4084': 1,  # 美孚
                    '4085': 1,  # 英国石油
                    '4094': 1,  # BP(港澳仅有)
                    '4095': 1,  # 埃索(港澳仅有)
                    '4096': 1,  # 加德士(港澳仅有)
                    '4098': 1,  # 华润(港澳仅有)
                    '40A0': 1,  # 加气站
                    '40A1': 1,  # 加油加气站
                    }

SERVICE_TOILET = {
                  '7001': 1  # 公共厕所
                  }
SERVICE_UNDEFINED_DICT = {'10C0': 1,
                          '10C1': 1,
                          '10C3': 1,
                          '1602': 1,
                          '2000': 1,  # 其他零售店
                          '2180': 1,
                          '2280': 1,
                          '2281': 1,
                          '2283': 1,
                          '2284': 1,
                          '2701': 1,  # 文化用品店
                          '2780': 1,  # 古玩字画店
                          '2900': 1,  # 农贸市场
                          '2980': 1,  # 珠宝首饰店
                          '2B00': 1,  # 工艺礼品店
                          '2B80': 1,  # 书店
                          '2C04': 1,
                          '2C05': 1,
                          '2D00': 1,  # 报刊亭
                          '2E80': 1,  # 音像店
                          '4081': 1,
                          '4082': 1,
                          '4083': 1,
                          '4086': 1,
                          '40A0': 1,
                          '40A1': 1,
                          '40B0': 1,  # 电动汽车充电站、充电桩
                          '4102': 1,  # 室外停车场
                          '4106': 1,
                          '4300': 1,  # 汽车销售、汽车服务
                          '4500': 1,  # 汽车用品、汽车养护
                          '5082': 1,
                          '5083': 1,
                          '5084': 1,
                          '6181': 1,  # 高尔夫练习场
                          '6700': 1,
                          '6E00': 1,  # 网吧
                          '6F00': 1,  # 垂钓
                          '7080': 1,  # 普通政府机关
                          '7081': 1,
                          '7100': 1,  # 公安机关
                          '7102': 1,  # 公安机关: 派出所
                          '7103': 1,  # 公安机关: 交警队
                          '7200': 1,  # 医院
                          '7280': 1,  # 社区医疗
                          '7300': 1,  # 公园
                          '7380': 1,  # 广场
                          '7400': 1,  # 会议中心、展览中心
                          '7500': 1,  # 防疫站
                          '7700': 1,  # 福利院、敬老院
                          '8083': 1,  # 车站    长途客运站
                          '8102': 1,  # 机场: 机场城市候机楼
                          '8200': 1,  # 票务中心｜定票处代表
                          '8380': 1,  # 高速服务区
                          '8381': 1,  # 高速停车区
                          '9080': 1,  # 风景名胜
                          '9081': 1,
                          '9300': 1,  # 美术馆
                          '9380': 1,  # 博物馆、纪念馆、展览馆、陈列馆
                          '9700': 1,  # 电台、电视台、电影制片厂
                          'A100': 1,  # 洗浴中心、SPA、足浴
                          'A199': 1,
                          'A280': 1,
                          'A400': 1,
                          'A880': 1,
                          'A300': 1,  # 缴费营业厅代表
                          'A380': 1,  # 邮局
                          'A400': 1,
                          'A480': 1,  # 旅行社
                          'A702': 1,  # 教育: 小学
                          'A709': 1,  # 教育: 技术培训机构
                          'A880': 1,  # 科研机构
                          'A980': 1,  # 公司企业: 公司
                          'A983': 1,
                          'A984': 1,  # 公司企业: 其他单位
                          'AE04': 1,  # 博彩: 彩票销售
                          'AF20': 1
                          }

MAINTAIN_LINK_DICT = {(14867592, 6024327): None,
                      (6024327, 14867592): None,
                      # 原数据的问题
                      (13957699, 13957678): None,
                      (13957678, 13957699): None,
                      (13957698, 13957691): None,
                      (13957691, 13957698): None,
                      }
