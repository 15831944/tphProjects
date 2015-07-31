# -*- coding: UTF8 -*-
'''
Created on 2011-12-22

@author: liuxinxing
'''

import platform.TestCase


class CCheckInlinkOutLink(platform.TestCase.CTestCase):
    def _do(self):
        self.pg.CreateFunction_ByName('check_org_junctionview_in_outlink')
        reg_count = self.pg.callproc('check_org_junctionview_in_outlink')
        return (reg_count == 1)


class CCheckNum(platform.TestCase.CTestCase):
    def _do(self):
        reg_count = self.pg.getOnlyQueryResult("select count(*) from org_junctionview")
        return (reg_count > 0)


class CCheckInlinkOutlinkNull(platform.TestCase.CTestCase):
    def _do(self):
        in_count = self.pg.getOnlyQueryResult("select count(*) from org_junctionview where arc1 is null or arc1 = 0")
        out_count = self.pg.getOnlyQueryResult("select count(*) from org_junctionview where arc2 is null or arc1 = 0")
        return ((in_count + out_count) == 0)


class CCheckInlinkOutlinkInLinkTab(platform.TestCase.CTestCase):
    def _do(self):
        in_count = self.pg.getOnlyQueryResult("SELECT count(*) \
                                      FROM org_junctionview as a\
                                    left join org_l_tran as b\
                                        on  a.arc1 = b.routeid\
                                        where b.routeid is null;")
        out_count = self.pg.getOnlyQueryResult("SELECT count(*) \
                                      FROM org_junctionview as a\
                                    left join org_l_tran as b\
                                        on  a.arc2 = b.routeid\
                                        where b.routeid is null;")
        return ((in_count + out_count) == 0)


class CChecklinksSameArrow(platform.TestCase.CTestCase):
    def _do(self):
        in_count = self.pg.getOnlyQueryResult("select count(*)\
                                            from (\
                                            SELECT count(*)\
                                            FROM org_junctionview\
                                            group  by arc1,arc2\
                                            ) as a;")
        out_count = self.pg.getOnlyQueryResult("select count(*)\
                                            from (\
                                            SELECT count(*)\
                                            FROM org_junctionview\
                                            group  by arrowimg\
                                            ) as a;")
        return (in_count == out_count)


class CCheckArrowNameAndPicName(platform.TestCase.CTestCase):
    def _do(self):
        count = self.pg.getOnlyQueryResult("SELECT count(*)\
                                          FROM org_junctionview\
                                      where day_pic = night_pic \
                                      and strpos(arrowimg,day_pic) <> 1;")
        return (count == 0)


class CCheckInlinkOutLinkSame(platform.TestCase.CTestCase):
    def _do(self):
        count = self.pg.getOnlyQueryResult("SELECT count(*) \
                                        FROM org_junctionview \
                                        where arc1 = arc2;")
        return (count == 0)


class CCheckPicNull(platform.TestCase.CTestCase):
    def _do(self):
        in_count = self.pg.getOnlyQueryResult('''select count(*)
                                        from org_junctionview
                                    where day_pic is null or day_pic = '0' ''')
        out_count = self.pg.getOnlyQueryResult(''' select count(*)
                                    from org_junctionview
                                where night_pic is null or night_pic = '0' ''')
        return ((in_count + out_count) == 0)
