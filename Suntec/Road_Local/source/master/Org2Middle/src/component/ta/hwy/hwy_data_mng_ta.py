# -*- coding: UTF-8 -*-
'''
Created on 2015-3-18

@author: hcz
'''
from component.rdf.hwy.hwy_data_mng_rdf import HwyDataMngRDF
from component.ta.hwy.hwy_graph_ta import HwyGraphTa
from component.rdf.hwy.hwy_path_graph_rdf import HwyPathGraphRDF
from component.rdf.hwy.hwy_graph_rdf import HWY_EXIT_NAME
from component.rdf.hwy.hwy_graph_rdf import HWY_JUNCTION_NAME
SP_TYPE = 'sp_type'
SP_TEXT = 'sp_text'
SP_LANG_CODE = 'sp_lang_code'
SP_EXIT_NUMBER = 'SP_EXIT_NUMBER'
SP_ROUTE_NUMBER = 'SP_ROUTE_NUMBER'
SP_STREET_NAME = 'SP_STREET_NAME'
SP_EXIT_NAME = 'SP_EXIT_NAME'
SP_OTHER_DESTINATION = 'SP_OTHER_DESTINATION'
SP_PLACE_NAME = 'SP_PLACE_NAME'


class HwyDataMngTa(HwyDataMngRDF):
    '''
    Highway Data Manager(TomTom)
    '''

    def __init__(self, ItemName='HwyDataMngTa'):
        '''
        Constructor
        '''
        HwyDataMngRDF.__init__(self, ItemName)
        self._sign_post_dict = {}

    @staticmethod
    def instance():
        if HwyDataMngRDF._instance is None:
            HwyDataMngRDF._instance = HwyDataMngTa()
        return HwyDataMngRDF._instance

    def initialize(self):
        self._graph = HwyGraphTa()  # 高速link图
        self._path_graph = HwyPathGraphRDF(self._graph)  # 高速路径图

    def load_exit_poi_name(self):
        pass

    def load_exit_name(self):
        '''加载出口番号'''
        # 下面的SQL代码时临时
        sqlcmd = """
        SELECT node_id, name
          FROM mid_temp_hwy_exit_name_ta as a
          where is_exit_name = 'Y';
        """
        for exit_info in self.get_batch_data(sqlcmd):
            node_id, exit_name = exit_info
            data = {HWY_EXIT_NAME: exit_name}
            # data = {"exit_name": exit_name}
            if node_id in self._graph:
                self._graph.add_node(node_id, data)
            else:
                pass
                # self.log.warning('Node=%s does not in Graph.' % node_id)

    def load_junction_name(self):
        '''加载出口番号'''
        # 下面的SQL代码时临时
        sqlcmd = """
        SELECT node_id, name
          FROM mid_temp_hwy_exit_name_ta as a
          where is_junction_name = 'Y';
        """
        for exit_info in self.get_batch_data(sqlcmd):
            node_id, junction_name = exit_info
            data = {HWY_JUNCTION_NAME: junction_name}
            if node_id in self._graph:
                self._graph.add_node(node_id, data)
            else:
                pass
                # self.log.warning('Node=%s does not in Graph.' % node_id)

    def load_signpost(self):
        sp_type_dict = {'4E': SP_EXIT_NUMBER,
                        'RN': SP_ROUTE_NUMBER,
                        '6T': SP_STREET_NAME,
                        '4G': SP_EXIT_NAME,
                        '4I': SP_OTHER_DESTINATION,
                        '9D': SP_PLACE_NAME,
                        }
        for row in self._get_sigppost():
            sp_id = row[0]
            node_lid = row[1]
            sp_name_list = []
            for infotyp, txt, lang_code in zip(*row[2:]):
                sp_type = sp_type_dict.get(infotyp)
                if not sp_type:
                    self.log.warning('No sign post type. id=%s', sp_id)
                    continue
                sp_name = {SP_TYPE: sp_type,
                           SP_TEXT: txt,
                           SP_LANG_CODE: lang_code
                           }
                sp_name_list.append(sp_name)
            sp_info = (node_lid, sp_name_list)
            inlink = (node_lid[0], node_lid[1])
            if not self._graph.has_edge(inlink[0], inlink[1]):
                continue
            temp_sp_infos = self._sign_post_dict.get(inlink)
            if temp_sp_infos:
                temp_sp_infos.append(sp_info)
            else:
                self._sign_post_dict[inlink] = [sp_info]

    def get_sp_infos(self, u, v):
        if not self._sign_post_dict:
            self.log.error('_sign_post_dict is none.')
        inlink = (u, v)
        return self._sign_post_dict.get(inlink)

    def _get_sigppost(self):
        self.CreateFunction2('mid_get_s_e_node')
        self.CreateFunction2('mid_linklid_2_nodelid')
        sqlcmd = """
        SELECT id, mid_linklid_2_nodelid(inlinkid, outlinkid, passlid),
               array_agg(infotyp), array_agg(txtcont), array_agg(txtcontlc)
          FROM (
            SELECT a.id, inlinkid, outlinkid, passlid,
                   infotyp, txtcont, txtcontlc
              FROM org_si as a
              LEFT JOIN signpost_uc_tbl as b
              ON a.id = b.id
              INNER JOIN hwy_link_road_code_info
              ON inlinkid = link_id
              WHERE infotyp in (
                    '4E',    -- Exit Number
                    'RN',    -- Route Number
                    '6T',    -- Street Name
                    '4G',    -- Exit Name
                    '4I',    -- Other Destination
                    '9D'     -- Place Name
                 )
               order by id, infotyp <> '4E', -- Exit Number first
                     -- contyp: 1 Branch; 2 Towards; 3 Exit
                     contyp, seqnr, destseq, rnpart
          ) AS a
          GROUP BY id, inlinkid, outlinkid, passlid
          ORDER BY id;
        """
        return self.get_batch_data(sqlcmd)
