# -*- coding: UTF-8 -*-
'''
Created on 2014-12-30

@author: hcz
'''
from component.rdf.hwy import hwy_data_mng_rdf


class HwyMainPath(object):
    '''
    classdocs
    '''

    def __init__(self):
        '''
        Constructor
        '''


class path():
    def __init__(self, G):
        '''
        Constructor
        '''
        self.fwd_paths = []
        self.bwd_paths = []
        self.road_nos = []
        self.road_names = []
        self.curr_path = []  # 当前link所在路径
        self.G = G

    def cal_path(self, u, v):
        if not self.G.has_edge(u, v):
            return
