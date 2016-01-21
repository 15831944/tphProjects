# -*- coding: UTF8 -*-
'''
Created on 2015-5-11

@author: PC_ZH
'''
import platform.TestCase


class CCheckMainLinkMapping(platform.TestCase.CTestCase):
    '''检查是否所有main link的前后方设施号相差1或0;
                若某link前后设施号差值大于1则返回False
    '''
    def _do(self):
        sqlcmd = '''
        select *
        from highway_mapping as a
        left join highway_road_info as b
        on a.road_no = b.road_no
        where path_type = 'MAIN' and
              forward_ic_no <> 0 and
              backward_ic_no <> 0 and
              b.loop <> 1 and
              ((forward_ic_no - backward_ic_no) >1 or
               (forward_ic_no - backward_ic_no) <0
               )
        '''
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row:
            return False
        return True


class CCheckICLinkMapping(platform.TestCase.CTestCase):
    '''
                检查ic link的前方设施一定在前进道路上，后方设施一定在后退道路上
    '''
    def _load_link_ic_no(self):
        '''加载经过link的ic_no'''
        sqlcmd = '''
        select distinct pass_link_id, b.ic_no, a.inout_c
        from mid_temp_hwy_ic_path_expand_link as a
        left join mid_hwy_ic_no as b
        on a.road_code = b.road_code and
           a.road_Seq = b.road_seq and
           a.updown_c = b.updown and
           a.node_id = b.node_id
        order by pass_link_id
        '''
        ic_dict = {}
        for row in self.pg.get_batch_data(sqlcmd):
            link_id, ic_no, inout_c = row
            ic_dict[(link_id, ic_no, inout_c)] = None
        return ic_dict

    def _do(self):
        return True
        sqlcmd = '''
        select link_id, forward_ic_no, backward_ic_no
        from highway_mapping
        where path_type = 'IC' and
              forward_ic_no <> 0 and --过滤无效ic_no:0
              backward_ic_no <> 0
        '''
        link_id_info = self._load_link_ic_no()
        for row in self.pg.get_batch_data(sqlcmd):
            link_id, f_ic_no, b_ic_no = row
            # ic link的前方设施一定在前进道路上
            # 后方设施一定在后退道路上
            # 1:设施入口； 2：设施出口
            if ((link_id, f_ic_no, 1) not in link_id_info or
                (link_id, b_ic_no, 2) not in link_id_info):
                print link_id
                return False
        return True
