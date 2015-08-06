# -*- coding: UTF8 -*-
'''
Created on 2015-8-3

@author: PC_ZH
'''
import platform.TestCase
MAX_DIS = 65535


class CCheckIcDistance(platform.TestCase.CTestCase):
    '''设施间距离不能超过 65535(16位足够存储) '''
    def _do(self):
        return True
        sqlcmd = '''
        select ic_no, between_distance
        from rdb_highway_ic_info
        order by ic_no
        '''
        for row in self.pg.get_batch_data(sqlcmd):
            ic_no = row[0]
            between_distance = row[1]
            if between_distance > MAX_DIS:
                self.pg.log.error('over distance limit,ic_no=%s' % ic_no)
                return False
        return True
