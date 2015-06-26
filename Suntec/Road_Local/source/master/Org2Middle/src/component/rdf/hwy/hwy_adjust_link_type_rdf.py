# -*- coding: UTF8 -*-
'''
Created on 2015-3-16

@author: hcz
'''
import component.component_base


class HwyAdjustLinkType(component.component_base.comp_base):
    '''高速Ramp的link type
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'HwyAdjustlinkType')

    def _Do(self):
        # 取得所有SAPA路径经过的link及原有的link_type
        self._get_sapa_path_link()
        self._get_jct_path_link()
        # 把SAPA路径经过的，且link_type原本是Ramp,改成SAPA link
        self._update_sapa_link_type()
        # 把SAPA路径经过的，且link_type原本是Ramp,改成jct link
        self._update_jct_link_type()
        return 0
        # 检查是否存在其他预料之外的link_type(Sapa, JCT, ramp之外)
        self._check_other_link_type()
        # 检查不经过SAPA的UTURN&RAMP
        self._check_isolated_uturn_ramp()

    def _get_sapa_path_link(self):
        '''取得所有SAPA路径经过的link及原有的link_type'''
        self.CreateTable2('mid_temp_sapa_path_link')
        sqlcmd = """
        INSERT INTO mid_temp_sapa_path_link(link_id, link_type)
        (
        SELECT distinct a.link_id, link_type
          FROM (
            SELECT regexp_split_to_table(link_lid, E',')::BIGINT as link_id
              FROM mid_temp_hwy_ic_path
              WHERE facilcls_c in (1, 2)
          ) AS a
          LEFT JOIN link_tbl
          ON a.link_id = link_tbl.link_id
          order by a.link_id
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _get_jct_path_link(self):
        '''取得所有JCT路径经过的link及原有的link_type'''
        # 注：这里非Ramp(SAPA、本线) 不调整成 JCT Link
        self.CreateTable2('mid_temp_jct_path_link')
        sqlcmd = """
        INSERT INTO mid_temp_jct_path_link(link_id, link_type)
        (
        SELECT distinct a.link_id, link_type
          FROM (
            SELECT regexp_split_to_table(link_lid, E',')::BIGINT as link_id
              FROM mid_temp_hwy_ic_path
              WHERE facilcls_c = 3 and link_lid <> ''
          ) AS a
          LEFT JOIN link_tbl
          ON a.link_id = link_tbl.link_id
          order by a.link_id
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _get_uturn_path_link(self):
        ''''''
        # UTURN 还没有实装
        return

    def _update_sapa_link_type(self):
        '''据SAPA路径经过的，且link_type原本是Ramp的link_type,改成SAPA link.
           JCT/本线等不做修改, 无料区间
           (大概修改记录193条)
        '''
        sqlcmd = """
        UPDATE link_tbl SET link_type = 7
          FROM mid_temp_sapa_path_link as a
          where link_tbl.link_id = a.link_id
                and a.link_type = 5;
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _update_jct_link_type(self):
        '''把JCT路径经过的，且link_type原本是Ramp的link_type,改成JCT link.
        '''
        sqlcmd = """
        UPDATE link_tbl SET link_type = 3
          FROM mid_temp_jct_path_link as a
          where link_tbl.link_id = a.link_id
                and a.link_type = 5;
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
