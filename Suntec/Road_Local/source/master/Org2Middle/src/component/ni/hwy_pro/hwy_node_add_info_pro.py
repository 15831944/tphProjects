# -*- coding: UTF-8 -*-
'''
Created on 2015年12月14日

@author: hcz
'''
from component.rdf.hwy.hwy_def_rdf import HWY_TRUE
from component.rdf.hwy.hwy_def_rdf import HWY_FALSE
from component.jdb.hwy.hwy_node_addinfo import HwyTollType
from component.rdf.hwy.hwy_node_addinfo_rdf import AddInfoDataRDF
from component.rdf.hwy.hwy_node_addinfo_rdf import HwyNodeAddInfoRDF
from component.jdb.hwy.hwy_graph import ONE_WAY_POSITIVE
from component.jdb.hwy.hwy_graph import ONE_WAY_RERVERSE


class HwyNodeAddInfoNiPro(HwyNodeAddInfoRDF):
    '''
    classdocs
    '''

    def __init__(self, data_mng, item_name='HwyNodeAddInfoNiPro'):
        '''
        Constructor
        '''
        HwyNodeAddInfoRDF.__init__(self, data_mng, item_name)

    def _make_toll_node_add_info(self):
        self.log.info('Make Tollgate Node Add Info.')
        toll_data = self._get_toll_data()
        for toll in toll_data:
            toll_node, toll_link, tile_id = toll[0:3]
            facil_names, up_downs = toll[3:5]  # 料金所名称
            facility_ids, one_way_code = toll[5:7]
            s_node, e_node = toll[7:9]
            if one_way_code == ONE_WAY_POSITIVE:
                dir_s_node, dir_e_node = s_node, e_node
            elif one_way_code == ONE_WAY_RERVERSE:
                dir_s_node, dir_e_node = e_node, s_node
            else:
                self.log.error('one_way_code=%s, link_id=%s' %
                               (one_way_code, toll_link))
            dummy = HWY_FALSE
            no_toll_money = HWY_FALSE
            # ## 料金所功能種別
            toll_type = HwyTollType()
            # ## 料金施設種別
            # self._set_toll_class(toll_classes, toll_type)
            facility_id = facility_ids[0]
            up_down = self._convert_updwon(up_downs)
            toll_flag = HWY_TRUE
            facility_num = 1
            facil_name = facil_names[0]
            seq_num = 1
            toll_type_num = 0
            toll_addinfo = AddInfoDataRDF(toll_link, toll_node,
                                          toll_flag, no_toll_money,
                                          facility_num, up_down,
                                          facility_id, facil_name,
                                          seq_num, toll_type_num,
                                          toll_type, tile_id=tile_id,
                                          dir_s_node=dir_s_node,
                                          dir_e_node=dir_e_node)
            self._insert_add_info(toll_addinfo)
        self.pg.commit2()

    def _get_toll_data(self):
        sqlcmd = """
        SELECT a.node_id, a.link_id, b.tile_id,
               array_agg(a.facil_name), array_agg(updown),
               array_agg(facility_id), one_way_code,
               s_node, e_node
          FROM hwy_tollgate as a
          LEFT JOIN link_tbl as b
          ON a.link_id = b.link_id
          LEFT JOIN mid_hwy_ic_no as ic
          ON a.road_code = ic.road_code and a.road_seq = ic.road_seq
          where facilcls_c = 6
          group by a.node_id, a.link_id, b.tile_id,
                   s_node, e_node, one_way_code
          ORDER BY b.tile_id, node_id, a.link_id
        """
        return self.get_batch_data(sqlcmd)
