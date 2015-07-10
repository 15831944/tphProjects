# -*- coding: UTF8 -*-
'''
Created on 2015-4-29

@author: hcz
'''
from component.rdf.hwy.hwy_prepare_rdf import HwyPrepareRDF


class HwyPrepareNi(HwyPrepareRDF):
    '''
    Hwy Prepare (TomTom)
    '''

    def __init__(self):
        '''
        Constructor
        '''
        HwyPrepareRDF.__init__(self, itemname='HwyPrepareNi')

    def _DoCreateTable(self):
        return 0

    def _DoCreateIndex(self):
        return 0

    def _Do(self):
        # self._make_hwy_exit_poi()
        # self._make_all_hwy_Node()
        return 0
