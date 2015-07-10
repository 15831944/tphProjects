# -*- coding: UTF-8 -*-
'''
Created on 2015-3-18

@author: hcz
'''
from component.rdf.hwy.hwy_facility_rdf import HwyFacilityRDF
from component.rdf.hwy.hwy_graph_rdf import is_cycle_path
from component.rdf.hwy.hwy_graph_rdf import HWY_ROAD_CODE
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_SA
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_PA
from component.rdf.hwy.hwy_def_rdf import HWY_UPDOWN_TYPE_UP
from component.rdf.hwy.hwy_def_rdf import HWY_TRUE
from component.rdf.hwy.hwy_def_rdf import HWY_FALSE
ROAD_SEQ_MARGIN = 10
SAPA_TYPE_DICT = {7358: HWY_IC_TYPE_PA,  # Truck Stop
                  7395: HWY_IC_TYPE_SA,  # Rest Area
                  # feattyp: SAPA TYPE
                  }


class HwyFacilityTa(HwyFacilityRDF):
    '''生成设施情报
    '''

    def __init__(self, data_mng):
        '''
        Constructor
        '''
        HwyFacilityRDF.__init__(self, data_mng)
        self.org_facil_dict = {}  # (org_facil_id, facil_cls): road_seq

#     def _make_ic_path(self):
#         self.log.info('Start make IC Path.')
#         self.pg.connect1()
#         self.CreateTable2('mid_temp_hwy_ic_path')
#         for road_code, route_path in self.hwy_data.get_road_code_path():
#             next_seq = ROAD_SEQ_MARGIN
#             end_pos = len(route_path)
#             if is_cycle_path(route_path):
#                 if not self.adjust_path_for_cycle(route_path):
#                     self.log.error('No indicate start node for Cycle Route.'
#                                    'road_code=%s' % road_code)
#                     continue
#                 else:
#                     self._store_moved_path(road_code, route_path)
#                 end_pos = len(route_path) - 1
#             sapa_node_dict = {}  # SaPa出口:SaPa入口 or SaPa入口:SaPa出口
#             facils_list = []
#             # ## 取得设施情报
#             for node_idx in range(0, end_pos):
#                 node = route_path[node_idx]
#                 all_facils = self.G.get_all_facil(node, road_code,
#                                                   HWY_ROAD_CODE)
#                 if not all_facils:
#                     continue
#                 if node in (90360200669534, 90360200681431):
#                     pass
#                 facils_list.append((node, all_facils))
#                 for facilcls, inout_c, path in all_facils:
#                     if facilcls in (HWY_IC_TYPE_SA, HWY_IC_TYPE_PA):
#                         node = path[0]
#                         to_node = path[-1]
#                         if node not in sapa_node_dict:
#                             sapa_node_dict[node] = set([to_node])
#                         else:
#                             sapa_node_dict[node].add(to_node)
#             # ## 设置设施序号
#             sapa_seq_dict = {}  # Sapa node: road_seq
#             for node, all_facils in facils_list:
#                 if node in (90360200669534, 90360200681431):
#                     pass
#                 road_seq_dict, next_seq = self._get_road_seq(node,
#                                                              all_facils,
#                                                              next_seq,
#                                                              sapa_node_dict,
#                                                              sapa_seq_dict,
#                                                              )
#                 for facilcls, inout_c, path in all_facils:
#                     # facil_name = self._get_facil_name(node, path)
#                     road_seq = road_seq_dict.get((facilcls, inout_c))
#                     if not road_seq:
#                         self.log.error('No Road Seq.')
#                         continue
#                     self._store_facil_path(road_code, road_seq, facilcls,
#                                            inout_c, path)
#         self.pg.commit1()
#         self.log.info('End make IC Path.')

    def _make_sapa_info(self):
        '''SAPA对应的Rest Area POI情报。'''
        self.log.info('Start Make SAPA Info.')
        self.CreateTable2('mid_temp_hwy_sapa_info')
        for data in self._get_rest_area_info():
            road_code, road_seq, poi_id, feattyp, name = data
            if feattyp:
                facil_cls = SAPA_TYPE_DICT.get(feattyp)
                if not facil_cls:
                    self.log.error('Unknown Rest Area type. poi_id=%s,'
                                   'rest_area_type' % (poi_id, feattyp))
                    continue
            else:
                facil_cls = HWY_IC_TYPE_PA
            self._store_sapa_info(road_code, road_seq,
                                  facil_cls, poi_id, name)
        self.pg.commit1()
        self._update_sapa_facilcls()
        self.CreateIndex2('mid_temp_hwy_sapa_info_road_code_road_seq_idx')
        self.log.info('End Make SAPA Info.')

    def _make_hwy_service(self):
        '''服务情报'''
        self.CreateTable2('hwy_service')
        self.pg.connect1()
        for data in self._get_store_info():
            road_code, road_seq = data[0:2]
            feattyp_list, subcat_list = data[2:4]
            service_types = self._get_service_types(feattyp_list, subcat_list)
            updown = HWY_UPDOWN_TYPE_UP
            self._store_service_info(road_code, road_seq,
                                     updown, service_types)
        self.pg.commit1()

    def _get_service_types(self, feattyp_list, subcat_list):
        gas_station, information, rest_area = HWY_FALSE, HWY_FALSE, HWY_FALSE
        shopping_corner, postbox, atm = HWY_FALSE, HWY_FALSE, HWY_FALSE
        restaurant, toilet = HWY_FALSE, HWY_FALSE
        for feattyp, subcat in zip(feattyp_list, subcat_list):
            if (feattyp, subcat) in SERVICE_GAS_DICT:
                gas_station = HWY_TRUE
            elif (feattyp, subcat) in SERVICE_INFORMATION_DICT:
                information = HWY_TRUE
            elif (feattyp, subcat) in SERVICE_REST_AREA_DICT:
                rest_area = HWY_TRUE
            elif (feattyp, subcat) in SERVICE_SHOPPING_DICT:
                shopping_corner = HWY_TRUE
            elif (feattyp, subcat) in SERVICE_POSTBOX_DICT:
                postbox = HWY_TRUE
            elif (feattyp, subcat) in SERVICE_ATM_DICT:
                atm = HWY_TRUE
            elif (feattyp, subcat) in SERVICE_RESTAURANT_DICT:
                restaurant = HWY_TRUE
            elif (feattyp, subcat) in SERVICE_TOILET_DICT:
                restaurant = HWY_TRUE
            elif (feattyp, subcat) in SERVICE_UNDEFINED_DICT:
                continue
            else:
                self.log.error('Unknown Category ID. feattyp=%s, cat_id=%s'
                               % (feattyp, subcat))
        return (gas_station, information, rest_area,
                shopping_corner, postbox, atm,
                restaurant, toilet)

    def _get_rest_area_info(self):
        '''取得服'''
        sqlcmd = """
        SELECT DISTINCT road_code, road_seq,
               c.poi_id, feattyp, e.name
          FROM (
            SELECT road_code, road_seq, facilcls_c,
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
          ORDER BY road_code, road_seq
        """
        return self.get_batch_data(sqlcmd)

    def _get_store_info(self):
        sqlcmd = """
        SELECT DISTINCT road_code, road_seq,
                array_agg(feattyp), array_agg(subcat)
          FROM (
            SELECT road_code, road_seq, facilcls_c,
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
          WHERE feattyp not in (7358, 7395) -- 7358:Truck Stop; 7395:Rest Area
          GROUP BY road_code, road_seq
          ORDER BY road_code, road_seq
        """
        return self.get_batch_data(sqlcmd)


# ==============================================================================
# 服务情报对应表
# ==============================================================================
# キャッシュコーナー/ATM
SERVICE_ATM_DICT = {(7397, 0): None,  # Cash Dispenser
                    }
# ガソリンスタンド/Gas station
SERVICE_GAS_DICT = {(7311, 0): None,  # Gas station
                    }
# 仮眠休憩所/rest_area
SERVICE_REST_AREA_DICT = {(7314, 7314006): None  # Hotel/Motel
                          }
# レストラン/Restaurant
SERVICE_RESTAURANT_DICT = {(7315, 7315145): None,  # Restaurant
                           }
# ショッピングコーナー/Shopping Corner
SERVICE_SHOPPING_DICT = {(7373, 0): None,  # Shopping Center
                         (9361, 9361018): None,  # Food & Drinks: Bakers
                         (9361, 9361025): None,  # Food & Drinks: Wine&Spirits
                         }
# 郵便ポスト/post_box
SERVICE_POSTBOX_DICT = {}
# インフォメーション
SERVICE_INFORMATION_DICT = {(7316, 0): None,  # Tourist Information Office
                            }
# Toilet
SERVICE_TOILET_DICT = {(9932, 9932005): None,  # Toilet
                       }
# 未定义服务种别
SERVICE_UNDEFINED_DICT = {(7304, 7304006): None,  # Apartment
                          (8099, 8099001): None,  # Geographic Feature
                          (8099, 8099016): None,  # Geographic: Bay
                          (8099, 8099020): None,  # Geographic: Cape
                          (9352, 9352011): None,  # Company: Manufacturing
                          (9362, 9362033): None,  # Picnic Area
                          (9932, 9932004): None,  # Public Call Box
                          (9932, 9932006): None,  # Road Rescue
                          (9942, 9942002): None,  # Bus Stop
                          (9942, 9942005): None,  # Coach Stop
                          }
