# -*- coding: UTF8 -*-
'''
Created on 2015-3-12

@author: hcz
'''
from component.component_base import comp_base


class HwyLinkMapping(comp_base):
    '''
    ORG Link id ==> Middle Link id.
    '''

    def __init__(self):
        '''
        Constructor
        '''
        comp_base.__init__(self, 'HwyLinkIdMappingTbl')

    def _DoCreateTable(self):
        # component.dictionary.comp_dictionary._DoCreateTable(self)
        self.CreateTable2('mid_link_mapping')  # 原始link和middle link对应表
        return 0

    def _DoCreateIndex(self):
        return 0

    def _Do(self):
        self._make_link_id_mapping()
        self.CreateIndex2('mid_link_mapping_link_id_idx')
        self.CreateIndex2('mid_link_mapping_org_link_id_idx')
        if not self._check_mid_link_id_mapping():
            self.log.error('MID link_id Error.')
        if not self._check_org_link_id_mapping():
            self.log.error('ORG Link Error.')

    def _make_link_id_mapping(self):
        '''原始link和middle link对应表'''
        sqlcmd = """
        INSERT INTO mid_link_mapping(org_link_id, link_id)
        (
        SELECT a.link_id as org_link_id,
              (case
               when merge.link_id is not null then merge.link_id
               when split.link_id is not null then split.link_id
               else a.link_id
              end) as mid_link_id
        FROM temp_rdf_nav_link as a
        left join temp_split_newlink as split
        on a.link_id = split.old_link_id
        left join temp_merge_link_mapping as merge
        on (split.link_id = merge.merge_link_id) or
           (a.link_id = merge.merge_link_id)
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _check_mid_link_id_mapping(self):
        sqlcmd = """
        SELECT COUNT(*)
          FROM (
            SELECT m.link_id as m_link_id, l.link_id as l_link_id
              FROM  mid_link_mapping as l, mid_link_mapping as m
              where m.link_id = l.link_id
          ) as a
          where m_link_id is NUll or l_link_id is NUll;
        """
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchone2()
        if row:
            if row[0] == 0:
                return True
        return False

    def _check_org_link_id_mapping(self):
        sqlcmd = """
        SELECT COUNT(*)
          FROM (
            SELECT m.org_link_id, l.link_id
              FROM  temp_rdf_nav_link as l, mid_link_mapping as m
              where l.link_id = m.org_link_id
          ) as a
          where org_link_id is NUll or link_id is NUll;
        """
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchone2()
        if row:
            if row[0] == 0:
                return True
        return False
