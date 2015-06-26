# -*- coding: UTF8 -*-
'''
Created on 2014-9-9

@author: hcz
'''
import platform.TestCase
import rdb_common_check
import json
import re


class CCheckSPName(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT sp_name
          FROM rdb_guideinfo_signpost_uc
          where sp_name is not null
          order by gid;
        """
        for row in self.pg.get_batch_data(sqlcmd):
            json_name = row[0]
            ml_name_list = json.loads(json_name)
            for name_list in ml_name_list:
                for name in name_list:
                    # 名称为空
                    if not name.get('val'):
                        return False
        return True


class CCheckSPRouteNo(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT route_no1, route_no2, route_no3, route_no4
          FROM rdb_guideinfo_signpost_uc
          where route_no1 is not null;
        """
        for row in self.pg.get_batch_data(sqlcmd):
            for no_idx in range(0, len(row)):
                json_routeno = row[no_idx]
                if not json_routeno:
                    if no_idx == 0:
                        return False
                    else:
                        continue
                ml_num_list = json.loads(json_routeno.replace('\t', '\\t'))
                for name_list in ml_num_list:
                    for name in name_list:
                        val = name.get('val')
                        if not val:
                            return False
                        # ## 数字 + tab + 番号
                        shield_info = val.split('\t')
                        if len(shield_info) != 2:
                            return False
                        shieldid = shield_info[0]
                        m = re.match(r'[+-]?\d+$', shieldid)
                        if not m:
                            return False
        return True


class CCheckSPExitNo(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT exit_no
          FROM rdb_guideinfo_signpost_uc
          where exit_no is not null
        """
        for row in self.pg.get_batch_data(sqlcmd):
            json_name = row[0]
            ml_no_list = json.loads(json_name)
            for no_list in ml_no_list:
                for no in no_list:
                    # 名称为空
                    if not no.get('val'):
                        return False
        return True


class CCheckPassLinkCount_SP(rdb_common_check.CCheckPassLinkCount):
    def __init__(self, suite, caseinfo):
        rdb_common_check.CCheckPassLinkCount.__init__(self, suite,
                                                      caseinfo,
                                                      'signpost_uc_tbl',
                                                      'passlink_cnt')
