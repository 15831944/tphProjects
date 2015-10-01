# -*- coding: UTF8 -*-
'''
Created on 2015-8-10

@author: PC_ZH
'''
import platform.TestCase


class CCheckPathPoint(platform.TestCase.CTestCase):
    def _do(self):
        '''JCT的设施的探索点情报，出口探索点 或 入口探索点, 至少存在一个'''
        sqlcmd = '''
        select ic_no ,
               array_agg(enter_flag) as enter_flag,
               array_agg(exit_flag) as exit_flags
        from rdb_highway_path_point
        group by ic_no
        order by ic_no
        '''
        for row in self.pg.get_batch_data(sqlcmd):
            ic_no = row[0]
            enter_l_flag = row[1]
            exit_l_flag = row[2]
            if set(enter_l_flag) == [0] and set(exit_l_flag) == [0]:
                self.log.error('no exit and enter flag ,ic_no is %s'
                                % ic_no)
                return False
        return True
