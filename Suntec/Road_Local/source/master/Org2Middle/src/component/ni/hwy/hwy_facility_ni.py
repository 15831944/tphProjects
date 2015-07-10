# -*- coding: UTF-8 -*-
'''
Created on 2015-5-27

@author: hcz
'''
from component.rdf.hwy.hwy_def_rdf import HWY_TRUE
from component.rdf.hwy.hwy_def_rdf import HWY_FALSE
from component.rdf.hwy.hwy_facility_rdf import HwyFacilityRDF
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_SA
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_PA
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

    def _get_sapa_type(self, sapa_kind):
        facil_cls = None
        if sapa_kind:
            facil_cls = SAPA_TYPE_DICT.get(sapa_kind)
        else:
            facil_cls = HWY_IC_TYPE_PA
        return facil_cls

    def _get_rest_area_info(self):
        '''取得服'''
        sqlcmd = """
        SELECT DISTINCT road_code, road_seq, c.poi_id,
                        kind, name
          FROM (
            SELECT road_code, road_seq, facilcls_c,
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
          ORDER BY road_code, road_seq
        """
        return self.get_batch_data(sqlcmd)

    def _get_service_types(self, cat_id_list):
        gas_station, information, rest_area = HWY_FALSE, HWY_FALSE, HWY_FALSE
        shopping_corner, postbox, atm = HWY_FALSE, HWY_FALSE, HWY_FALSE
        restaurant, toilet = HWY_FALSE, HWY_FALSE
        for cat_id in cat_id_list:
            if cat_id in SERVICE_GAS_DICT:
                gas_station = HWY_TRUE
            elif cat_id in SERVICE_INFORMATION_DICT:
                information = HWY_TRUE
            elif cat_id in SERVICE_REST_AREA_DICT:
                rest_area = HWY_TRUE
            elif cat_id in SERVICE_SHOPPING_DICT:
                shopping_corner = HWY_TRUE
            elif cat_id in SERVICE_POSTBOX_DICT:
                postbox = HWY_TRUE
            elif cat_id in SERVICE_ATM_DICT:
                atm = HWY_TRUE
            elif cat_id in SERVICE_RESTAURANT_DICT:
                restaurant = HWY_TRUE
            elif cat_id in SERVICE_UNDEFINED_DICT:
                continue
            else:
                self.log.error('Unknown Category ID. cat_id=%s' % cat_id)
        return (gas_station, information, rest_area,
                shopping_corner, postbox, atm,
                restaurant, toilet)

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
                          '7880': 1,  # 公共厕所
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
