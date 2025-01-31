# -*- coding: UTF8 -*-
'''
Created on 2015-7-12

@author: PC_ZH
'''
from component.component_base import comp_base


class HwyLinkMappingZenrin(comp_base):
    '''
    ORG Link id ==> Middle Link id.
    '''
    def __init__(self, Item='HwyLinkMappingZenrin'):
        '''
        Constructor
        '''
        comp_base.__init__(self, Item)

    def _Do(self):
        self._make_link_id_mapping()
        self._check_mid_link_id_mapping()
        if not self._check_mid_link_id_mapping():
            self.log.error('MID link_id Error.')

    def _make_link_id_mapping(self):
        ''' '''
        self.log.info('start make link id mapping')
        self.CreateTable2('mid_link_mapping')
        sqlcmd = '''
        INSERT INTO mid_link_mapping(org_link_id, link_id)
        (
          SELECT a.link_id AS org_link_id,
              (CASE
               when merge.link_id is not null then merge.link_id
               when split.link_id is not null then split.link_id
               else a.link_id
              END) AS mid_link_id
          FROM temp_link_mapping AS a
          LEFT JOIN temp_split_newlink AS split
          ON a.link_id = split.old_link_id
          LEFT JOIN temp_merge_link_mapping AS merge
          ON (split.link_id = merge.merge_link_id) or
             (a.link_id = merge.merge_link_id)
        )
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.log.info('end make link id mapping')
        return 0

    def _check_mid_link_id_mapping(self):
        sqlcmd = """
        SELECT COUNT(*)
          FROM (
            SELECT m.org_link_id, l.link_id
              FROM  temp_link_mapping as l, mid_link_mapping as m
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

    def _check_org_link_id_mapping(self):
        return 0
