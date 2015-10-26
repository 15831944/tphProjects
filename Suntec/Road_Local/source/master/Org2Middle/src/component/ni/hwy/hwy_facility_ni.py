# -*- coding: UTF-8 -*-
'''
Created on 2015-5-27

@author: hcz
'''
from component.rdf.hwy.hwy_def_rdf import HWY_TRUE
from component.rdf.hwy.hwy_def_rdf import HWY_FALSE
from component.rdf.hwy.hwy_facility_rdf import HwyFacilityRDF
from component.ni.hwy.hwy_poi_category_ni import HwyPoiCategoryNi
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_SA
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_PA
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_IC
from component.rdf.hwy.hwy_def_rdf import HWY_INOUT_TYPE_IN
from component.rdf.hwy.hwy_def_rdf import HWY_INOUT_TYPE_OUT
from component.rdf.hwy.hwy_def_rdf import HWY_LINK_TYPE_SAPA
from component.rdf.hwy.hwy_graph_rdf import HWY_LINK_TYPE

SAPA_TYPE_DICT = {'8380': HWY_IC_TYPE_SA,  # 高速停车区
                  '8381': HWY_IC_TYPE_PA   # 高速停车区
                  }


class HwyFacilityNi(HwyFacilityRDF):
    '''生成设施情报(中国四维)
    '''

    def __init__(self, data_mng, ItemName='HwyFacilityNi'):
        '''
        Constructor
        '''
        HwyFacilityRDF.__init__(self, data_mng, ItemName)
        self.poi_cate = HwyPoiCategoryNi()

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

    def _filter_JCT_UTurn(self):
        '''过滤假JCT/UTurn:下了高速又转弯回来的径路'''
        self.log.info('Start Filtering JCT/UTurn Path.')
        self.pg.connect1()
        self.pg.connect2()
        self.CreateTable2('mid_temp_hwy_jct_uturn_del')
        del_pathes = []
        for jct_uturn in self._get_jct_uturn():
            gid, path_type = jct_uturn[0:2]
            road_code, inout_c = jct_uturn[2:4]
            node_lid = map(int, jct_uturn[4].split(','))
            if jct_uturn[5]:
                link_lid = map(int, jct_uturn[5].split(','))
            else:
                self.log.warning('No link_lid. node_id=%s.' % node_lid[0])
                continue
            # print gid
            f_node_lids = [map(int, n_lid.split(','))
                           for n_lid in jct_uturn[6]]
            f_link_lids = []
            for l_lid in jct_uturn[7]:
                if l_lid:
                    f_link_lids.append(map(int, l_lid.split(',')))
                else:
                    f_link_lids.append([])
                    self.log.warning('No link_lid. node_id=%s.' % node_lid[0])
            t_node_lids = [map(int, n_lid.split(','))
                           for n_lid in jct_uturn[8]]
            t_link_lids = []
            for l_lid in jct_uturn[9]:
                if l_lid:
                    t_link_lids.append(map(int, l_lid.split(',')))
                else:
                    t_link_lids.append([])
                    self.log.warning('No link_lid. node_id=%s.' % node_lid[0])
            # 求出入口路径的重叠点
            overlay_nodes = self._get_overlay_node(f_node_lids, t_node_lids)
            if not overlay_nodes:  # 没有重叠的点
                # 存留路径
                continue
            node_set = set(node_lid)
            if not node_set.intersection(overlay_nodes):  # 重叠点不在JCT/UTurn路径里
                # 存留路径
                continue
            join_nodes = self._get_join_nodes(overlay_nodes,
                                              f_node_lids,
                                              inout_c)
            if node_set.intersection(join_nodes):
                del_pathes.append(gid)
            else:
                if overlay_nodes:
                    print path_type, link_lid
        # 删除错误的JCT/UTurn
        self._store_del_jct_uturn(del_pathes)
        self.log.info('End Filtering JCT/UTurn Path.')

    def _get_join_nodes(self, overlay_nodes, f_node_lids, f_inout):
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
            node_idx = len(f_node_lid) - 1
            # ## 取最尾部那段重叠路径的第一个点
            while node_idx > 0:
                v = f_node_lid[node_idx]
                u = f_node_lid[node_idx - 1]
                if v in curr_overlay_nodes:
                    join_nodes.add(v)
                    has_overlay = True
                else:
                    if has_overlay:  # 重叠点必须连续
                        break
                if f_inout == HWY_INOUT_TYPE_OUT:
                    data = self.G[u][v]
                else:
                    data = self.G[v][u]
                # SAPA link
                link_type = data.get(HWY_LINK_TYPE)
                if link_type == HWY_LINK_TYPE_SAPA:
                    break
                # Roundabout
                node_idx -= 1
        return join_nodes

    def _get_overlay_node(self, f_node_lids, t_node_lids):
        '''重叠点'''
        f_set = set()
        t_set = set()
        for node_lid in f_node_lids:
            # 取收费站之前的点
            s_idx = self._get_toll_index(node_lid)
            if s_idx < 1:
                s_idx = 1
            f_set = f_set.union(node_lid[s_idx:])
        for node_lid in t_node_lids:
            # 取收费站之前的点
            s_idx = self._get_toll_index(node_lid)
            if s_idx < 1:
                s_idx = 1
            t_set = t_set.union(node_lid[s_idx:])
        return f_set.intersection(t_set)

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

    def _get_jct_uturn(self):
        sqlcmd = """
        SELECT gid, path_type,
               a.road_code, a.inout_c,
               a.node_lid, a.link_lid,
               f_node_lids, f_link_lids,
               t_node_lids, t_link_lids
          FROM (
            -- get JCT/UTurn
            SELECT gid, road_code, road_seq, facilcls_c,
                   inout_c, node_id, to_node_id,
                   node_lid, link_lid, path_type
              from mid_temp_hwy_ic_path
              where path_type in ('JCT', 'UTURN') -- 'JCT', 'UTURN'
          ) as a
          INNER JOIN (
            SELECT node_id, road_code, inout_c,
                   array_agg(node_lid) as f_node_lids,
                   array_agg(link_lid) as f_link_lids
              FROM (
                SELECT DISTINCT node_id, road_code, inout_c,
                       node_lid, link_lid
                  FROM mid_temp_hwy_ic_path
                  where facilcls_c in (5 ) and path_type <> 'UTURN'
                  ORDER BY  node_id, road_code
              ) AS f
              group by node_id, road_code, inout_c
          ) as b
          ON a.node_id = b.node_id and
             a.road_code = b.road_code and
             a.inout_c = b.inout_c
          INNER JOIN (
            SELECT node_id, road_code, inout_c,
                   array_agg(node_lid) as t_node_lids,
                   array_agg(link_lid) as t_link_lids
              FROM (
                SELECT DISTINCT node_id, road_code, inout_c,
                       node_lid, link_lid
                  FROM mid_temp_hwy_ic_path
                  where facilcls_c in (5) and path_type <> 'UTURN'
                  ORDER BY  node_id, road_code
              ) AS t
              group by node_id, road_code, inout_c
          ) as c
          ON a.to_node_id = c.node_id and
             --a.road_code = c.road_code and
             (a.inout_c = 1 and c.inout_c = 2 or
              a.inout_c = 2 and c.inout_c = 1)
          ORDER BY gid;
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

SERVICE_GAS_DICT = {'4080': 1,  # 加油站
                    '4081': 1,  # 中国石油
                    '4082': 1,  # 中国石化
                    '4083': 1,  # 中海油
                    '4086': 1,  # 壳牌
                    '4087': 1,  # 美孚
                    '4094': 1,  # BP(港澳仅有)
                    '4095': 1,  # 埃索(港澳仅有)
                    '4096': 1,  # 加德士(港澳仅有)
                    '4098': 1,  # 华润(港澳仅有)
                    '40A0': 1,  # 加气站
                    '40A1': 1,  # 加油加气站
                    }
SERVICE_INFORMATION_DICT = {}
SERVICE_REST_AREA_DICT = {'5080': 1,  # 星级宾馆
                          '5082': 1,  # 1，2星级宾馆
                          '5083': 1,  # 3星级宾馆
                          '5084': 1,  # 4星级宾馆
                          '5085': 1,  # 5星级宾馆
                          '5380': 1,  # 旅馆、招待所
                          '5400': 1,  # 非星级渡假村、疗养院
                          }
SERVICE_SHOPPING_DICT = {'2080': 1,  # 便利店
                         '2100': 1,  # 超市
                         '2200': 1,  # 百货商城、百货商店
                         '2280': 1,  # 烟酒副食、地方特产
                         '2283': 1,  # 烟酒副食、地方特产: 副食专卖店
                         '2284': 1,  # 烟酒副食、地方特产: 地方土特产专卖店
                         '2500': 1,  # 茶叶、茶具专卖店
                         '2580': 1,  # 面包房
                         '2180': 1,  # 小商品城
                         '2380': 1,  # 服装、鞋帽店
                         '2C04': 1,  # 家居用品
                         }
SERVICE_POSTBOX_DICT = {'A400': 1,  # 物流、快运
                        }
SERVICE_ATM_DICT = {'A199': 1,  # ATM/自助银行
                    }
SERVICE_RESTAURANT_DICT = {'1080': 1,  # 快餐
                           '10C0': 1,  # 快餐: 麦当劳
                           '10C1': 1,  # 快餐: 肯德基
                           '10C3': 1,  # 快餐: 德克士
                           '1300': 1,  # 异国风味
                           '1380': 1,  # 中餐馆
                           '1600': 1,  # 茶楼，咖啡店
                           '1601': 1,  # 茶楼代表
                           '1602': 1,  # 咖啡店代表
                           '1680': 1,  # 冷饮店
                           }
# Toilet
SERVICE_TOILET_DICT = {'7880': 1,  # 公共厕所
                       }

SERVICE_UNDEFINED_DICT = {'2000': 1,  # 其他零售店
                          '2701': 1,  # 文化用品店
                          '2780': 1,  # 古玩字画店
                          '2900': 1,  # 农贸市场
                          '2980': 1,  # 珠宝首饰店
                          '2B00': 1,  # 工艺礼品店
                          '2B80': 1,  # 书店
                          '2D00': 1,  # 报刊亭
                          '2E80': 1,  # 音像店
                          '40B0': 1,  # 电动汽车充电站、充电桩
                          '4102': 1,  # 室外停车场
                          '4300': 1,  # 汽车销售、汽车服务
                          '4500': 1,  # 汽车用品、汽车养护
                          '6181': 1,  # 高尔夫练习场
                          '6E00': 1,  # 网吧
                          '6F00': 1,  # 垂钓
                          '7080': 1,  # 普通政府机关
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
                          '9080': 1,  # 风景名胜
                          '9300': 1,  # 美术馆
                          '9380': 1,  # 博物馆、纪念馆、展览馆、陈列馆
                          '9700': 1,  # 电台、电视台、电影制片厂
                          'A100': 1,  # 洗浴中心、SPA、足浴
                          'A300': 1,  # 缴费营业厅代表
                          'A380': 1,  # 邮局
                          'A480': 1,  # 旅行社
                          'A702': 1,  # 教育: 小学
                          'A709': 1,  # 教育: 技术培训机构
                          'A880': 1,  # 科研机构
                          'A980': 1,  # 公司企业: 公司
                          'A984': 1,  # 公司企业: 其他单位
                          'AE04': 1,  # 博彩: 彩票销售
                          '8380': 1,  # 高速服务区
                          '8381': 1,  # 高速停车区
                          }
