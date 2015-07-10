# -*- coding: UTF-8 -*-
'''
Created on 2015��6��12��

@author: PC_ZH
'''


def deal_with_sapa_link(self):
    '''deal with bug: Exist SAPA Link, but no SAPA Facility.'''

    sqlcmd = '''
        SELECT mid_hwy_premote_sapa_link();
    '''
    self.pg.execute2(sqlcmd)
    self.pg.commit2()

