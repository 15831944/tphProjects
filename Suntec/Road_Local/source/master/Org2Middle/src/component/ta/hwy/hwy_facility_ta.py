# -*- coding: UTF-8 -*-
'''
Created on 2015-3-18

@author: hcz
'''
from component.rdf.hwy.hwy_graph_rdf import HWY_ROAD_CODE
from component.rdf.hwy.hwy_graph_rdf import HWY_LINK_TYPE
from component.rdf.hwy.hwy_def_rdf import HWY_INOUT_TYPE_OUT
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_IC
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_SA
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_PA
# from component.rdf.hwy.hwy_def_rdf import HWY_UPDOWN_TYPE_UP
from component.rdf.hwy.hwy_def_rdf import HWY_ROAD_TYPE_HWY
from component.rdf.hwy.hwy_def_rdf import HWY_TRUE
from component.rdf.hwy.hwy_def_rdf import HWY_FALSE
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_TOLL
from component.rdf.hwy.hwy_def_rdf import HWY_LINK_TYPE_INNER
from component.rdf.hwy.hwy_def_rdf import HWY_LINK_TYPE_MAIN
from component.rdf.hwy.hwy_def_rdf import HWY_LINK_TYPE_SAPA
from component.default.multi_lang_name import NAME_TYPE_OFFICIAL
from component.default.multi_lang_name import MultiLangName
from component.ta.multi_lang_name_ta import MultiLangNameTa
from component.ta.hwy.hwy_data_mng_ta import SP_TYPE
from component.ta.hwy.hwy_data_mng_ta import SP_TEXT
from component.ta.hwy.hwy_data_mng_ta import SP_LANG_CODE
from component.ta.hwy.hwy_data_mng_ta import SP_EXIT_NUMBER
from component.rdf.hwy.hwy_graph_rdf import HWY_ROAD_TYPE
from component.rdf.hwy.hwy_graph_rdf import HWY_ROAD_NAMES
from component.rdf.hwy.hwy_facility_rdf import HWY_PATH_TYPE_UTURN
from component.rdf.hwy.hwy_facility_rdf import HWY_PATH_TYPE_JCT
from component.rdf.hwy.hwy_facility_rdf import HWY_PATH_TYPE_SAPA
from component.rdf.hwy.hwy_facility_rdf import HwyFacilityRDF
from component.ta.hwy.hwy_poi_category_ta import HwyPoiCategoryTa
MAX_SP_NAME_NUM = 4  # 方面名称的最大数
ROAD_SEQ_MARGIN = 10
SAPA_TYPE_DICT = {7358: HWY_IC_TYPE_PA,  # Truck Stop
                  7395: HWY_IC_TYPE_SA,  # Rest Area
                  # feattyp: SAPA TYPE
                  }
lang_code_dict = {'UND': 'ENG',
                  'ENG': 'ENG',
                  }


class HwyFacilityTa(HwyFacilityRDF):
    '''生成设施情报
    '''

    def __init__(self, data_mng, ItemName='HwyFacilityTa'):
        '''
        Constructor
        '''
        HwyFacilityRDF.__init__(self, data_mng, ItemName)
        self.poi_cate = HwyPoiCategoryTa()

    def _make_sapa_info(self):
        '''SAPA对应的Rest Area POI情报。'''
        self.log.info('Start Make SAPA Info.')
        self.CreateTable2('mid_temp_hwy_sapa_info')
        prev_road_code, prev_road_seq = None, None
        for data in self._get_rest_area_info():
            road_code, road_seq, poi_id, feattyp, name, updown = data
            # SAPA有些路径经过SAPA POI, 有些没有经过时, 过滤掉没有经过SAPA POI的情
            if(prev_road_code == road_code and
               prev_road_seq == road_seq and
               not poi_id):
                continue
            prev_road_code, prev_road_seq = road_code, road_seq
            if feattyp:
                facil_cls = SAPA_TYPE_DICT.get(feattyp)
                if not facil_cls:
                    self.log.error('Unknown Rest Area type. poi_id=%s,'
                                   'rest_area_type' % (poi_id, feattyp))
                    continue
            else:
                facil_cls = HWY_IC_TYPE_PA
            self._store_sapa_info(road_code, road_seq,
                                  facil_cls, poi_id, name,
                                  updown)
        self.pg.commit1()
        self._update_sapa_facilcls()
        self.CreateIndex2('mid_temp_hwy_sapa_info_road_code_'
                          'road_seq_updown_c_idx')
        self.log.info('End Make SAPA Info.')

    def _make_hwy_store(self):
        '''店铺情报'''
        self.log.info('Start Make SAPA Facility Stores.')
        self.CreateTable2('hwy_store')
        sqlcmd = """
         SELECT distinct road_code, road_seq, updown_c,
                         c.feattyp as cat_id, subcat, '' as store_chain_id,
                         brandname, array_agg(distance)as distance, f.per_code
           FROM mid_temp_hwy_sapa_link as a
           LEFT JOIN org_mnpoi_pi as c
           ON a.poi_id = c.id
           LEFT JOIN mid_temp_hwy_sapa_name as e
           ON c.id = e.poi_id
           LEFT JOIN temp_poi_category_mapping as f
           ON a.poi_id = f.org_id1
           WHERE feattyp not in (7358, 7395)
                 --  7358: Truck Stop; 7395: Rest Area
                and brandname <> '' and brandname is not null
           group by road_code, road_seq, updown_c, c.feattyp,
                 subcat, store_chain_id, brandname, f.per_code
           ORDER BY road_code, road_seq,
                    distance, store_chain_id
        """
        self.pg.connect1()
        for row in self.pg.get_batch_data2(sqlcmd):
            (road_code, road_seq, updown_c, cat_id,
             subcat, chain_id, chain_name, distance, percode) = row
            if not percode:
                self.log.error('No CategoryID for store. cat_id=%s, subcat=%s,'
                               ' chain_id=%s, chain_name=%s' %
                               (cat_id, subcat, chain_id, chain_name))
                continue
            service_kind = self._get_service_kind(int(percode))
            if not service_kind:
                # self.log.error('No service_kind. cat_id=%s, subcat=%s,'
                #                'chain_id=%s, chain_name=%s' %
                #                (cat_id, subcat, chain_id, chain_name))
                continue
            distance = min(distance)
            self._insert_into_store_info((road_code, road_seq, updown_c,
                                          cat_id, subcat, chain_id, chain_name,
                                          distance, service_kind))
        self.pg.commit1()
        self.log.info('End Make Facility Stores.')

    def _get_sapa_link(self):
        sqlcmd = '''
        SELECT DISTINCT b.road_code, b.road_seq, b.updown_c,
               b.node_id, b.first_node_id, c.poi_id,
               array_agg(b.link_id), b.link_lid, b.node_lid
          FROM (
            SELECT a.road_code, a.road_seq, a.updown_c, link_lid, node_lid,
                   a.node_id as node_id, first_node.node_id as first_node_id,
                   regexp_split_to_table(link_lid, E'\\,+')::bigint as link_id
              FROM mid_temp_hwy_ic_path as a
              LEFT JOIN temp_hwy_sapa_first_node as first_node
                ON a.road_code = first_node.road_code and
                   a.road_seq = first_node.road_seq and
                   a.updown_c = first_node.updown_c
              where facilcls_c in (1, 2) and   -- 1: sa, 2: pa
                    link_lid <> '' and
                    link_lid is not null and
                    inout_c = 2
          ) as b
          LEFT JOIN mid_temp_poi_link as c
          ON b.link_id = c.link_id
          LEFT JOIN org_mnpoi_pi as d
          ON c.poi_id = d.id
          WHERE feattyp not in (7358, 7395) --7358: Truck Stop; 7395: Rest Area
          GROUP BY b.road_code, b.road_seq, b.updown_c,
                   b.node_id, b.first_node_id, c.poi_id,
                   b.link_lid, b.node_lid
          ORDER BY road_code, road_seq
        '''
        return self.get_batch_data(sqlcmd)

    def _insert_into_store_info(self, parm):
        sqlcmd = '''
        INSERT INTO hwy_store(road_code, road_seq, updown_c,
                              store_cat_id, sub_cat, store_chain_id,
                              chain_name, priority, service_kind)
        VALUES(%s, %s, %s, %s,
               %s, %s, %s, %s,
               %s)
        '''
        self.pg.execute1(sqlcmd, parm)

    def _update_undefined_dict(self):
        self.undefined_dict.update(TA_SERVICE_UNSPECIFED_DICT)
        return 0

    def _make_hwy_service(self):
        '''服务情报'''
        self.log.info('Start make SAPA Service Info.')
        self.CreateTable2('hwy_service')
        self.pg.connect1()
        for data in self._get_store_info():
            road_code, road_seq, updwon, category_id_list = data[0:4]
            service_types = self._get_service_types(category_id_list)
            # 服务标志都为HWY_FALSE时，不收录
            if set(service_types) == set([HWY_FALSE]):
                continue
            self._store_service_info(road_code, road_seq,
                                     updwon, service_types)
        self.pg.commit1()
        self.log.info('End make SAPA Service Info.')

    def _get_service_types(self, category_id_list):
        gas_station, information, rest_area = HWY_FALSE, HWY_FALSE, HWY_FALSE
        shopping_corner, postbox, atm = HWY_FALSE, HWY_FALSE, HWY_FALSE
        restaurant, toilet = HWY_FALSE, HWY_FALSE
        for category_id in category_id_list:
            if category_id in self.gas_station_dict:
                gas_station = HWY_TRUE
            elif category_id in self.info_dict:
                information = HWY_TRUE
            elif category_id in self.rest_area_dict:
                rest_area = HWY_TRUE
            elif category_id in self.shopping_corner_dict:
                shopping_corner = HWY_TRUE
            elif category_id in self.post_box_dict:
                postbox = HWY_TRUE
            elif category_id in self.atm_dict:
                atm = HWY_TRUE
            elif category_id in self.restaurant_dict:
                restaurant = HWY_TRUE
            elif category_id in self.toilet_dict:
                toilet = HWY_FALSE  # 一般地不做 toilet
            elif category_id in self.undefined_dict:
                continue
            else:
                self.log.warning('Unknown Category ID. category_id=%s'
                                 % category_id)
        return (gas_station, information, rest_area,
                shopping_corner, postbox, atm,
                restaurant, toilet)

    def _get_rest_area_info(self):
        '''取得服'''
        sqlcmd = """
        SELECT DISTINCT road_code, road_seq,
               c.poi_id, feattyp, e.name, updown_c
          FROM (
            SELECT road_code, road_seq, facilcls_c, updown_c,
                   regexp_split_to_table(link_lid, E'\\,+')::bigint as link_id
              FROM mid_temp_hwy_ic_path as a
              where facilcls_c in (1, 2) and   -- 1: sa, 2: pa
                    link_lid <> '' and link_lid is not null
          ) as b
          LEFT JOIN mid_temp_poi_link as c
          ON b.link_id = c.link_id
          LEFT JOIN org_mnpoi_pi as d
          ON c.poi_id = d.id
          LEFT JOIN mid_temp_hwy_sapa_name as e
          ON c.poi_id = e.poi_id
          WHERE feattyp in (7358, 7395) --  7358: Truck Stop; 7395: Rest Area
          ORDER BY road_code, road_seq, c.poi_id
        """
        return self.get_batch_data(sqlcmd)

    def _get_store_info(self):
        sqlcmd = """
        SELECT road_code, road_seq, updown_c,
               --array_agg(feattyp), array_agg(subcat),
               array_agg(category_id)
         FROM (
                SELECT DISTINCT road_code, road_seq, updown_c,
                        feattyp, subcat, f.per_code as category_id
                  FROM (
                    SELECT road_code, road_seq, updown_c, facilcls_c,
                           regexp_split_to_table(link_lid, E'\\,+'
                                                )::bigint as link_id
                      FROM mid_temp_hwy_ic_path as a
                      where facilcls_c in (1, 2) and   -- 1: sa, 2: pa
                            link_lid <> '' and link_lid is not null
                  ) as b
                  LEFT JOIN mid_temp_poi_link as c
                  ON b.link_id = c.link_id
                  LEFT JOIN org_mnpoi_pi as d
                  ON c.poi_id = d.id
                  LEFT JOIN mid_temp_hwy_sapa_name as e
                  ON c.poi_id = e.poi_id
                  LEFT JOIN temp_poi_category_mapping as f
                  ON c.poi_id = f.org_id1
                  -- 7358:Truck Stop; 7395:Rest Area; 7369: Open Parking Area
                  WHERE feattyp not in (7358, 7395, 7369)
          ) AS f
          GROUP BY road_code, road_seq, updown_c
          ORDER BY road_code, road_seq, updown_c;
        """
        return self.get_batch_data(sqlcmd)

    def _make_facil_name(self):
        '''制作设施名称'''
        self.log.info('Start Make Facil Name.')
        self.pg.connect1()
        self.CreateTable2('mid_temp_hwy_facil_name')
        sqlcmd = """
        SELECT road_code, road_seq, facilcls_c, node_id,
               inout_c, array_agg(node_lid), array_agg(path_type)
          FROM (
            SELECT distinct road_code, road_seq, facilcls_c,
                   inout_c, node_id, node_lid, path_type
              FROM mid_temp_hwy_ic_path
              WHERE facilcls_c <> 10  -- Not U-turn
              ORDER BY road_code, road_seq, node_id,
                       facilcls_c, inout_c
          ) AS a
          group by road_code, road_seq, facilcls_c, node_id, inout_c
          order by road_code, road_seq;
        """
        for data in self.pg.get_batch_data2(sqlcmd):
            road_code, road_seq, facilcls, node_id, inout, pathes = data[:-1]
            temp_pathes = [map(int, path.split(',')) for path in pathes]
            pathe_types = data[-1]
            facil_name = None
            # SAPA
            if facilcls in (HWY_IC_TYPE_SA, HWY_IC_TYPE_PA):
                facil_name = self._get_sapa_facil_name(road_code, road_seq)
                if facil_name:
                    self._store_facil_name(road_code, road_seq, facil_name)
            elif facilcls in (HWY_IC_TYPE_TOLL,):
                facil_name = self.G.get_node_name(node_id)
                if facil_name:
                    self._store_facil_name(road_code, road_seq, facil_name)
            elif facilcls == HWY_IC_TYPE_IC:  # IC
                pathes = []
                # 过滤掉UTUTN路径
                for path, pathe_type in zip(temp_pathes, pathe_types):
                    if pathe_type != HWY_PATH_TYPE_UTURN:
                        pathes.append(path)
                facil_name = self._get_ic_name(road_code, facilcls,
                                               inout, node_id, pathes)
                if facil_name:
                    self._store_facil_name(road_code, road_seq, facil_name)
            else:  # JCT
                continue
        self.pg.commit1()
        self.CreateIndex2('mid_temp_hwy_facil_name_road_code_road_seq_idx')
        self.log.info('End Make Facility Name.')

    def _get_ic_name(self, road_code, facilcls, inout, node_id, pathes):
        if facilcls != HWY_IC_TYPE_IC or inout != HWY_INOUT_TYPE_OUT:
            return None
        # 出口/分歧
        in_nodes = self.G.get_main_link(node_id, road_code,
                                        code_field=HWY_ROAD_CODE,
                                        same_code=True, reverse=True)
        if not in_nodes:
            self.log.error('No In_node. node_id=%s' % node_id)
            return None
        if len(in_nodes) > 1:
            self.log.error('In_node > 1. node_id=%s' % node_id)
            return None
        in_node = in_nodes[0]
        # 路径添加上一般道(原来路径只探到最后一条Ramp)
        pathes = self._get_next_street_path(in_node, pathes)
        json_name = self._get_signpost_info(in_node, node_id, pathes)
        if json_name:
            return json_name
        json_name = self._get_next_street_names(pathes)
        return json_name

    def _get_signpost_info(self, in_node, node_id, pathes):
        sp_infos = self.hwy_data.get_sp_infos(in_node, node_id)
        if not sp_infos:
            return None
        temp_sp_name_list = []
        exit_sp_name_list = []  # 出口、分歧
        main_sp_name_list = []  # 本线
        # 路的末端，去掉本线路径，留下出口、分叉路径
        for node_lid, sp_name_list in sp_infos:
            for path in pathes:
                if self._march_path(node_lid, path):
                    # 退出link非本线
                    out_link = (node_lid[1], node_lid[2])
                    data = self.G[out_link[0]][out_link[1]]
                    link_type = data.get(HWY_LINK_TYPE)
                    # road_type = data.get(HWY_ROAD_TYPE)
                    if(link_type in HWY_LINK_TYPE_MAIN):
                        main_sp_name_list.append(sp_name_list)
                    else:
                        exit_sp_name_list.append(sp_name_list)
                    break
        if exit_sp_name_list:
            temp_sp_name_list = exit_sp_name_list
        else:
            temp_sp_name_list = main_sp_name_list
        if len(temp_sp_name_list) > 1:
            self.log.warning('sp_name_list > 1. node=%s' % node_id)
        # ## 取得出口
        exit_nums = []
        for sp_name_list in temp_sp_name_list:
            for sp_name in sp_name_list:
                if sp_name.get(SP_TYPE) == SP_EXIT_NUMBER:
                    exit_nums.append(sp_name)
        if exit_nums:
            # 出口番号，转成JSON
            json = self._cvt_signpost_name_2_json(exit_nums)
            if not json:
                self.log.error('JSON is none of Exit Number. node=%s'
                               % node_id)
            return json
        # ## 道路番号、道路名称转成JSON
        json = self._get_sp_json_name(node_id, temp_sp_name_list)
        return json

    def _get_sp_json_name(self, node_id, sp_name_list):
        temp_sp_name_list2 = []
        for sp_name_list in sp_name_list:
            temp_sp_name_list2 += sp_name_list
        if temp_sp_name_list2:
            json = self._cvt_signpost_name_2_json(temp_sp_name_list2)
            if not json:
                self.log.error('JSON is none of SP Name. node=%s' % node_id)
            return json
        return None

    def _get_next_street_names(self, pathes):
        # 取种道路种别最高的道路的名称(道路番号，放在名称前面)
        main_normal_links = {}  # 最后一条link，即一般道
        main_first_links = {}
        exit_normal_links = {}
        exit_first_links = {}
        for path in pathes:
            fst_link = (path[1], path[2])  # 第一条脱出link
            fst_data = self.G[fst_link[0]][fst_link[1]]
            link_type = fst_data.get(HWY_LINK_TYPE)
            fst_road_type = fst_data.get(HWY_ROAD_TYPE)
            u, v = path[-2], path[-1]
            data = self.G[u][v]
            if((link_type in HWY_LINK_TYPE_MAIN or
               link_type == HWY_LINK_TYPE_INNER) and
               fst_road_type in HWY_ROAD_TYPE_HWY):
                if (u, v) not in main_normal_links:
                    if fst_road_type in HWY_ROAD_TYPE_HWY:
                        road_type = data.get(HWY_ROAD_TYPE)
                        road_names = data.get(HWY_ROAD_NAMES)
                        if HWY_ROAD_NAMES:
                            main_normal_links[u, v] = (road_type, road_names)
                if fst_link not in main_first_links:
                    road_names = fst_data.get(HWY_ROAD_NAMES)
                    if HWY_ROAD_NAMES:
                        main_first_links[fst_link] = (fst_road_type,
                                                      road_names)
            else:
                if (u, v) not in exit_normal_links:
                    road_type = data.get(HWY_ROAD_TYPE)
                    road_names = data.get(HWY_ROAD_NAMES)
                    if road_names:
                        exit_normal_links[u, v] = (road_type, road_names)
                if fst_link not in exit_first_links:
                    road_names = fst_data.get(HWY_ROAD_NAMES)
                    if HWY_ROAD_NAMES:
                        exit_first_links[fst_link] = (fst_road_type,
                                                      road_names)
        if exit_normal_links:  # 出口一般道名称
            normal_links = exit_normal_links
        elif exit_first_links:  # 出口一般道没有名称时，出口方向的第一条名称
            normal_links = exit_first_links
        elif main_normal_links:  # 本线出口一般道名称
            normal_links = main_normal_links
        else:  # # 本线出口第一条link名称
            normal_links = main_first_links
        json_name = self._get_link_names(normal_links)
        if json_name:
            return json_name
        if not json_name:
            exit_ramp = False
            for path in pathes:
                fst_link = (path[1], path[2])  # 第一条脱出link
                fst_data = self.G[fst_link[0]][fst_link[1]]
                link_type = fst_data.get(HWY_LINK_TYPE)
                if(link_type not in HWY_LINK_TYPE_MAIN and
                   link_type != HWY_LINK_TYPE_INNER):
                    exit_ramp = True
                    break
            if exit_ramp:
                self.log.warning('No Next Street Name. node=%s' % path[1])
        return None

    def _get_link_names(self, normal_link_names):
        '''道路番号 + 道路名称'''
        # 按道路种别排序
        sorted_values = sorted(normal_link_names.values(),
                               cmp=lambda x, y: cmp(x[0], y[0]))
        # 求road_type等级最高的道路名称
        prev_road_type = 16
        rst_route_nums = []
        rst_road_names = []
        for (road_type, road_names) in sorted_values:
            if(road_type > prev_road_type and
               (rst_route_nums or rst_road_names)):
                break
            prev_road_type = road_type
            temp_road_name = None
            if road_names:
                for name_dict in road_names:
                    if name_dict.get("type") == "route_num":  # 道路番号
                        if name_dict not in rst_route_nums:
                            rst_route_nums.append(name_dict)
                    else:
                        if not temp_road_name:  # 只取第一个官名
                            temp_road_name = name_dict
            if temp_road_name and temp_road_name not in rst_road_names:
                rst_road_names.append(temp_road_name)
        # 道路番号 + 道路名称
        num_names = rst_route_nums + rst_road_names
        if num_names:
            json = MultiLangName.json_format_dump2([num_names])
            return json
        return None

    def _get_next_street_path(self, in_node, pathes):
        # 求得和路径相连的一般道(原设施路径不到达一般道)
        next_street_pathes = []
        for path in pathes:
            path = [in_node] + path
            temp_pathes = list(self.G.get_next_street_path(path))
            if not temp_pathes:
                next_street_pathes.append(path)
                data = self.G[path[-2]][path[-1]]
                link_type = data.get(HWY_LINK_TYPE)
                if link_type != HWY_LINK_TYPE_SAPA:
                    self.log.warning('No Normal Road. path=%s' % path)
            else:
                for temp_path in temp_pathes:
                    next_street_pathes.append(temp_path)
        return next_street_pathes

    def _cvt_signpost_name_2_json(self, sp_name_list):
        multi_name = None
        num = 0
        added_sp_names = {}
        for sp_name in sp_name_list:
            str_name = sp_name.get(SP_TEXT)
            lang_code = sp_name.get(SP_LANG_CODE)
            lang_code = lang_code_dict.get(lang_code)
            key = (str_name, lang_code)
            if not multi_name:
                multi_name = MultiLangNameTa(lang_code=lang_code,
                                             str_name=str_name,
                                             name_type=NAME_TYPE_OFFICIAL)
            else:
                if key in added_sp_names:  # 名称已经收录
                    continue
                other_name = MultiLangNameTa(lang_code=lang_code,
                                             str_name=str_name,
                                             name_type=NAME_TYPE_OFFICIAL)
                multi_name.add_alter(other_name)
            added_sp_names[key] = None
            num += 1
            if num > MAX_SP_NAME_NUM:
                break
        if multi_name:
            return multi_name.json_format_dump()
        return None

    def _march_path(self, node_lid, path):
        if len(path) < len(node_lid):
            return False
        # 本线与一般道直接相连的出口路径没有处理
        for node_cnt in range(0, len(node_lid)):
            if node_lid[node_cnt] != path[node_cnt]:
                return False
        return True

    def _filter_JCT_UTurn(self, pathes, curr_path, inout,
                          path_type, join_node):
        if path_type in (HWY_PATH_TYPE_JCT, HWY_PATH_TYPE_UTURN):
            if self._is_straight(curr_path, inout, join_node):
                if self._is_only_one_path(curr_path, inout, join_node):
                    return False
            return True
        else:
            return False

    def _get_filter_path_types(self):
        # TA(南非、澳洲): JCT, UTurn要都进行过滤，SAPA不用过滤
        path_types = [HWY_PATH_TYPE_JCT, HWY_PATH_TYPE_UTURN]
        return path_types

    def _make_ic_out_nearby_poi(self):
        '''IC出口附近POI'''
        self.log.info('Make IC out nearby POI.')
        self.CreateTable2('mid_temp_hwy_ic_out_nearby_poi')
        # 取出口周边2.5Km内的POI
        sqlcmd = """
        INSERT INTO mid_temp_hwy_ic_out_nearby_poi(
                                        road_code, road_seq, updown_c,
                                        node_id, poi_id, cat_id,
                                        subcat, name, brandname,
                                        distance, the_geom)
        (
        SELECT road_code, road_seq, updown_c,
               node_id, id AS poi_id, feattyp AS cat_id,
               subcat, name, brandname,
               ST_Distance(ST_GeographyFromText(ST_asewkt(a.the_geom)),
                           ST_GeographyFromText(ST_asewkt(b.the_geom))) dist,
               b.the_geom
          FROM (
            SELECT road_code, road_seq, updown_c, node_id, the_geom
              FROM hwy_node
              WHERE facilcls_c = 5 and inout_c = 2  -- IC Out
          ) AS a
          INNER JOIN org_mnpoi_pi as b
          ON ST_Dwithin(a.the_geom, b.the_geom, 0.022) -- 0.022: about 2.45km
          WHERE feattyp not in (9920, 7230) and  -- 7230: 交叉点POI
                -- 8099002: Mountain Peak
                not (b.subcat = 8099002 and b.cltrpelid is null)
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('mid_temp_hwy_ic_out_nearby_poi_poi_id_idx')
        # 删除高速路/SAPA上的POI
        self._del_hwy_road_poi()
        # 添加出口路径上的POI
        self._add_ic_out_path_poi()
        self.CreateIndex2('mid_temp_hwy_ic_out_nearby_poi_the_geom_idx')

    def _del_hwy_road_poi(self):
        '''删除高速路/SAPA上的POI'''
        sqlcmd = """
        DELETE FROM mid_temp_hwy_ic_out_nearby_poi as a
          using (
              SELECT b.poi_id, road_type
                FROM mid_temp_poi_link AS b
                LEFT JOIN link_tbl as c
                ON b.link_id = c.link_id
                where road_type = 0 or link_type in (7)
           ) as d
           where a.poi_id = d.poi_id
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _add_ic_out_path_poi(self):
        '''出口路径上的 POI: 距离填0, 使之较周边其他POI的优先级别更高'''
        sqlcmd = """
        INSERT INTO mid_temp_hwy_ic_out_nearby_poi(
                                                road_code, road_seq, updown_c,
                                                node_id, poi_id, cat_id,
                                                subcat, name, brandname,
                                                distance, the_geom)
        (
        SELECT distinct road_code, road_seq, updown_c,
               a.node_id, poi_id, feattyp AS cat_id,
               subcat, name, brandname,
               0 as distance, the_geom
          FROM (
            SELECT distinct road_code, road_seq, updown_c,
                            node_id, pass_link_id
              from mid_temp_hwy_ic_path_expand_link
              where facilcls_c = 5 and inout_c = 2
          ) as a
          INNER JOIN mid_temp_poi_link AS b
          ON pass_link_id = b.link_id
          LEFT JOIN org_mnpoi_pi as c
          ON b.poi_id = c.id
          WHERE feattyp not in (9920, 7230) and  -- 7230: 交叉点POI
                -- 8099002: Mountain Peak
                not (subcat = 8099002 and c.cltrpelid is null)
          ORDER BY road_code, updown_c, road_seq, node_id, poi_id
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _make_ic_out_service(self):
        '''IC出口服务情报'''
        self.log.info('Make IC out service info.')
        self.CreateTable2('hwy_ic_service')
        self.pg.connect1()
        for data in self._get_ic_out_service_info():
            road_code, road_seq, updwon, node_id, category_id_list = data[0:5]
            service_types = self._get_service_types(category_id_list)
            # 服务标志都为HWY_FALSE时，不收录
            if set(service_types) == set([HWY_FALSE]):
                continue
            self._store_ic_service_info(road_code, road_seq,
                                        updwon, node_id, service_types)
        self.pg.commit1()
        pass

    def _make_ic_out_store(self):
        '''IC出口店铺情报'''
        self.log.info('Make IC out Store info.')
        self.CreateTable2('hwy_ic_store')
        for row in self._get_ic_out_store_info():
            (road_code, road_seq, updown_c, node_id, cat_id,
             subcat, chain_id, chain_name, distance, percode) = row
            if not percode:
                self.log.error('No CategoryID for store. cat_id=%s, subcat=%s,'
                               ' chain_id=%s, chain_name=%s' %
                               (cat_id, subcat, chain_id, chain_name))
                continue
            service_kind = self._get_service_kind(int(percode))
            if not service_kind:
                continue
            self._store_ic_store_info((road_code, road_seq, updown_c, node_id,
                                       cat_id, subcat, chain_id, chain_name,
                                       distance, service_kind))
        self.pg.commit1()

    def _get_ic_out_service_info(self):
        sqlcmd = """
        SELECT road_code, road_seq, updown_c, node_id,
               array_agg(per_code) as category_id_list
        FROM (
            SELECT DISTINCT road_code, road_seq, updown_c,
                    node_id, service_name, c.per_code
              FROM mid_temp_hwy_ic_out_nearby_poi as a
              LEFT JOIN temp_poi_category_mapping as b
              ON a.poi_id = b.org_id1
              INNER JOIN (
                select service_name, category_id, per_code
                from(
                    select service_name, category_id, genre, gen1, gen2
                    from hwy_service_category_mapping as m
                    left join temp_poi_category as p
                    on m.category_id = p.per_code
                    where genre = '1st genre'
                    )as a
                inner join temp_poi_category as b
                on a.gen1 = b.gen1
                union
                select service_name, category_id, per_code
                from (
                    select service_name, category_id, genre, gen1, gen2
                    from hwy_service_category_mapping as m
                    left join temp_poi_category as p
                    on m.category_id = p.per_code
                    where genre = '2nd genre'
                     )as c
                inner join temp_poi_category as d
                on c.gen1 = d.gen1 and c.gen2 = d.gen2
                -- ORDER BY category_id
              ) as c
              ON b.per_code = c.per_code
              ORDER BY road_code, road_seq, updown_c, node_id,
                       service_name, per_code
          ) as d
          GROUP BY road_code, road_seq, updown_c, node_id;
        """
        return self.get_batch_data(sqlcmd)

    def _get_ic_out_store_info(self):
        sqlcmd = """
        SELECT  road_code, road_seq, updown_c,
                node_id, a.cat_id, subcat, '' as store_chain_id,
                brandname, min(distance) as distance, per_code
           FROM mid_temp_hwy_ic_out_nearby_poi as a
           LEFT JOIN temp_poi_category_mapping as b
           ON a.poi_id = b.org_id1
           WHERE a.cat_id not in (7358, 7395)
             --  7358: Truck Stop; 7395: Rest Area
             and brandname <> '' and brandname is not null
           group by road_code, road_seq, updown_c, node_id,
                    a.cat_id, subcat, brandname, per_code
           ORDER BY road_code, road_seq, node_id, distance, store_chain_id
        """
        return self.get_batch_data(sqlcmd)

    def _store_ic_service_info(self, road_code, road_seq,
                               updwon, node_id, service_types):
        sqlcmd = """
        INSERT INTO hwy_ic_service(road_code, road_seq, updown_c,
                                   node_id, gas_station, information,
                                   rest_area, shopping_corner, postbox,
                                   atm, restaurant, toilet)
          VALUES (%s, %s, %s,
                  %s, %s, %s,
                  %s, %s, %s,
                  %s, %s, %s);
        """
        params = (road_code, road_seq, updwon, node_id) + service_types
        self.pg.execute1(sqlcmd, params)

    def _store_ic_store_info(self, parm):
        ''''''
        sqlcmd = '''
        INSERT INTO hwy_ic_store(road_code, road_seq, updown_c, node_id,
                                 store_cat_id, sub_cat, store_chain_id,
                                 chain_name, priority, service_kind)
        VALUES(%s, %s, %s, %s,
               %s, %s, %s,
               %s, %s, %s)
        '''
        self.pg.execute1(sqlcmd, parm)


# ==============================================================================
# HwyFacilityTaSaf
# ==============================================================================
class HwyFacilityTaSaf(HwyFacilityTa):
    '''生成设施情报(南非)
    '''

    def __init__(self, data_mng, ItemName='HwyFacilityTaSaf'):
        '''
        Constructor
        '''
        HwyFacilityTa.__init__(self, data_mng, ItemName)

    def _get_sp_json_name(self, node_id, sp_name_list):
        # 南非不用SignPost Name.
        # 即没有SignPost Exit No时，IC设施名称为空。
        return None

    def _get_next_street_names(self, pathes):
        # 南非不用next street name.
        # 即没有SignPost情报时，IC设施名称为空。
        return None

    def _make_ic_out_nearby_poi(self):
        '''# IC出口附近POI'''
        pass

    def _make_ic_out_service(self):
        '''IC出口服务情报'''
        self.CreateTable2('hwy_ic_service')

    def _make_ic_out_store(self):
        '''IC出口店铺情报'''
        self.CreateTable2('hwy_ic_store')

# 未定义服务种别
TA_SERVICE_UNSPECIFED_DICT = {
                              (7301, 7301002): None,  # Traffic Control Department
                              (7315, 7315072): None,  # Brazilian
                              (7315, 7315149): None,  # Yogurt/Juice Bar
                              (7332, 7332005): None,  # Supermarkets & Hypermarkets
                              (7383, 7383005): None,  # Airfield
                              (8099, 8099027): None,
                              (9155, 9155002): None,  # Car Wash
                              (9376, 9376006): None,  # Coffee Shop
                              }
