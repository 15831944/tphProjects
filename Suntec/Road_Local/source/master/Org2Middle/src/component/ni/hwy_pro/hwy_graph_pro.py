# -*- coding: UTF-8 -*-
'''
Created on 2015-10-23

@author: hcz
'''
from component.rdf.hwy.hwy_graph_rdf import HWY_PATH_ID
import networkx as nx
from component.ni.hwy.hwy_graphy_ni import HwyGraphNi


class HwyGraphNiPro(HwyGraphNi):
    '''
    classdocs
    '''

    def __init__(self, data=None, **attr):
        '''
        Constructor
        '''
        HwyGraphNi.__init__(self, data, **attr)

    def get_main_path(self, nodes, path_id, code_field=HWY_PATH_ID):
        '''通过间断的点求出整条本线。'''
        path = nodes[0:1]
        for u, v in zip(nodes[:-1], nodes[1:]):
            if u == v:
                continue
            # 取u的out link(node)
            out_nodes = self.get_main_link(u, path_id, code_field,
                                           same_code=True, reverse=False)
            if not out_nodes:
                raise nx.NetworkXError('No Out Main Link. node=%s ' % u)
            elif len(out_nodes) > 1:
                raise nx.NetworkXError('Out Main Link > 1. node=%s ' % u)
            u_out = out_nodes[0]
            if u_out == v:
                path += [u_out]
                continue
            # 取v的in link(node)
            in_nodes = self.get_main_link(v, path_id, code_field,
                                          same_code=True, reverse=True)
            if not in_nodes:
                raise nx.NetworkXError('No In Main Link. node=%s ' % v)
            elif len(in_nodes) > 1:
                raise nx.NetworkXError('In Main Link > 1. node=%s ' % v)
            v_in = in_nodes[0]
            if u_out == v_in:
                path += [u_out, v]
                continue
            # u_out与v_in的路径
            temp_pathes = list(nx.all_simple_paths(self, u_out, v_in))
            if temp_pathes:
                if len(temp_pathes) == 1:
                    path += temp_pathes[0]
                else:
                    self.log.error('Path > 1. u=%s, v=%s' % (u, v))
            else:
                nx.NetworkXError('No Path.  u=%s, v=%s' % (u, v))
        return path
