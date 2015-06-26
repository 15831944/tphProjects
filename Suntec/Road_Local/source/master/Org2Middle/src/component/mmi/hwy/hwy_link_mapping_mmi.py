# -*- coding: UTF8 -*-
'''
Created on 2015-4-10

@author: hcz
'''
from component.rdf.hwy.hwy_link_mapping import HwyLinkMapping


class HwyLinkMappingMMi(HwyLinkMapping):
    '''
    ORG Link id ==> Middle Link id for MMi Model.
    '''

    def __init__(self):
        '''
        Constructor
        '''
        HwyLinkMapping.__init__(self)

    def _make_link_id_mapping(self):
        '''原始link和middle link对应表'''
        sqlcmd = """
        INSERT INTO mid_link_mapping(org_link_id, link_id)
        (
        SELECT a.id as org_link_id,
           (case
            when merge.link_id is not null then merge.link_id
            when split.link_id is not null then split.link_id
            else a.id
           end) as mid_link_id
        FROM org_city_nw_gc_polyline as a
        left join temp_split_newlink as split
        on a.id::bigint = split.old_link_id
        left join temp_merge_link_mapping as merge
        on (split.link_id = merge.merge_link_id) or
           (a.id::bigint = merge.merge_link_id)
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _check_org_link_id_mapping(self):
        sqlcmd = """
        SELECT COUNT(*)
          FROM (
            SELECT m.org_link_id, nw.id
              FROM org_city_nw_gc_polyline as nw, mid_link_mapping as m
              where nw.id::bigint = m.org_link_id
          ) as a
          where org_link_id is NUll or id is NUll;
        """
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchone2()
        if row:
            if row[0] == 0:
                return True
        return False
