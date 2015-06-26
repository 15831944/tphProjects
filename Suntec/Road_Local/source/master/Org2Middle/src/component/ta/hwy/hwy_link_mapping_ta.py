# -*- coding: UTF8 -*-
'''
Created on 2015-3-18

@author: hcz
'''
from component.rdf.hwy.hwy_link_mapping import HwyLinkMapping


class HwyLinkMappingTa(HwyLinkMapping):
    '''
    ORG Link id ==> Middle Link id for Tomtom Model.
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
        SELECT nw.id::bigint as org_link_id,
               case
                when merge.link_id is not null then merge.link_id
                when split.link_id is not null then split.link_id
                when circle.link_id is not null then circle.link_id
                when dupli.bid is not null then dupli.bid
                else nw.id
               end  as mid_link_id
        FROM org_nw as nw
        left join temp_dupli_link as dupli
        on nw.id::bigint = dupli.aid
        left join temp_circle_link_new_seq_link as circle
        on nw.id::bigint = circle.old_link_id
        left join temp_split_newlink as split
        on nw.id::bigint = split.old_link_id or
           circle.link_id = split.old_link_id or
           dupli.bid = split.old_link_id
        left join temp_merge_link_mapping as merge
        on nw.id::bigint = merge.merge_link_id or
           split.link_id = merge.merge_link_id or
           circle.link_id = merge.merge_link_id
        where nw.frc != -1
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _check_org_link_id_mapping(self):
        sqlcmd = """
        SELECT COUNT(*)
          FROM (
            SELECT m.org_link_id, l.id as link_id
              FROM  org_nw as l, mid_link_mapping as m
              where l.id::bigint = m.org_link_id
          ) as a
          where org_link_id is NUll or link_id is NUll;
        """
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchone2()
        if row:
            if row[0] == 0:
                return True
        return False
