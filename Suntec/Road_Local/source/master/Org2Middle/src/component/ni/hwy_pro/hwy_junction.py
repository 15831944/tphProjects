# -*- coding: UTF8 -*-
'''
Created on 2015年10月26日

@author: hcz
'''
import component.component_base


class HwyJunction(component.component_base.comp_base):
    '''
    ORG Link id ==> Middle Link id for Tomtom Model.
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'HwyJunction')

    def _Do(self):
        # 合并名称相同的并设
        self._merge_hw_junction_id()
        # 合并：两点之间多个设施
        self._merge_hw_junction_mid_linkid()
        # 处理org_hw_jct
        self._merge_hw_jct()

    def _merge_hw_junction_id(self):
        '''将名称相同的并设合并,取较小的ID'''
        self.CreateTable2('mid_hwy_org_hw_junction_merged_id')
        sqlcmd = '''
        INSERT INTO mid_hwy_org_hw_junction_merged_id(id, merged_id)
        (
        SELECT id, unnest(ids) AS merged_id--, road_type
        FROM (
            SELECT hw_pid, nodeid,  accesstype,
                   attr, estab_item, min(id) id,
                   array_agg(id) ids, min(seq_nm) seq_nm,
                   name,
                   (case when id = s_junc_pid then e_road_type
                    else s_road_type end) as road_type
            FROM mid_hwy_org_hw_junction_mid_linkid as a
            LEFT JOIN (
            -- Get JCT to_road_type
               SELECT s_junc_pid, e_junc_pid, s_road_type, e_road_type
                 FROM (
                    SELECT s_junc_pid::bigint, e_junc_pid::bigint,
                       b1.inlinkid as s_link, b2.outlinkid as e_link,
                       link1.road_type as s_road_type,
                       link2.road_type as e_road_type
                      FROM org_hw_jct as a
                      LEFT JOIN mid_hwy_org_hw_junction_mid_linkid as b1
                      ON  s_junc_pid::bigint = b1.id
                      LEFT JOIN mid_hwy_org_hw_junction_mid_linkid as b2
                      ON  e_junc_pid::bigint = b2.id
                      LEFT JOIN link_tbl as link1
                      ON  b1.inlinkid = link1.link_id
                      LEFT JOIN link_tbl as link2
                      ON  b2.outlinkid = link2.link_id
                  ) AS b
            ) AS c
            ON a.id = c.s_junc_pid or a.id = c.e_junc_pid
            LEFT JOIN temp_hwy_facil_name_ni as f
            ON a.id = f.junction_id
           -- where a.nodeid = 6331830
            GROUP BY hw_pid, nodeid, accesstype,
                 attr, estab_item, road_type, name
            ORDER BY hw_pid, seq_nm
        ) AS b
        ORDER BY id, merged_id
        )
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return 0

    def _merge_hw_jct_id(self):
        '''合并：两点之间多对JCT,
           1.保存没有和其他JCT合并的JCT
           2.多对都和其他JCT合并时，都合到最小的那个
           3.多对都没有被合并，无法合关，报错。
        '''
        for jct_info in self._get_repeat_jct():
            jct_info
            pass

    def _get_repeat_jct(self):
        '''取得 两点之间多对JCT '''
        sqlcmd = """
        SELECT b.nodeid::bigint as s_node_id,
               c.nodeid::bigint as e_node_id,
               array_agg(s_junc_pid)::bigint[] as s_junc_pids,
               array_agg(e_junc_pid)::bigint[]::bigint[] as e_junc_pids,
               array_agg(b.hw_pid)::bigint[] as s_hw_pid,
               array_agg(c.hw_pid)::bigint[] as e_hw_pid,
               array_agg(d.id) as new_s_junc_pids,
               array_agg(e.id) as new_e_junc_pids,
               array_agg(f.merged_ids) as s_merged_ids_list,
               array_agg(g.merged_ids) as e_merged_ids_list
          FROM org_hw_jct as a
          LEFT JOIN org_hw_junction as b
          on a.s_junc_pid = b.id
          LEFT JOIN org_hw_junction as c
          on a.e_junc_pid = c.id
          LEFT JOIN mid_hwy_org_hw_junction_merged_id as d
          ON a.s_junc_pid::bigint = merged_id
          LEFT JOIN mid_hwy_org_hw_junction_merged_id as e
          ON a.e_junc_pid::bigint = e.merged_id
          LEFT JOIN (
             SELECT id,
                    array_to_string(array_agg(merged_id), ',') merged_ids
              FROM mid_hwy_org_hw_junction_merged_id
              group by id
          ) as f
          ON d.id = f.id
          LEFT JOIN (
             SELECT id,
                    array_to_string(array_agg(merged_id), ',') merged_ids
              FROM mid_hwy_org_hw_junction_merged_id
              group by id
          ) as g
          ON e.id = g.id
          group by s_node_id, e_node_id
          having count(*) > 1
        """
        return self.get_batch_data(sqlcmd)

    def _merge_hw_junction_mid_linkid(self):
        '''重新对应mid_hwy_org_hw_junction_mid_linkid中id'''
        self.CreateTable2('mid_hwy_org_hw_junction_mid_linkid_merged')
        sqlcmd = '''
        INSERT INTO mid_hwy_org_hw_junction_mid_linkid_merged(
                                        id, nodeid, inlinkid, outlinkid,
                                        accesstype, attr, seq_nm , hw_pid,
                                        estab_item)
        (
            SELECT id, nodeid, inlinkid, outlinkid,
                   accesstype, attr, min(seq_nm) min_seq_nm , hw_pid,
                   estab_item
              FROM (
                SELECT b.id, nodeid, inlinkid, outlinkid,
                       accesstype, attr, seq_nm,
                       hw_pid, estab_item
                  FROM mid_hwy_org_hw_junction_mid_linkid as a
                  LEFT JOIN mid_hwy_org_hw_junction_merged_id as b
                  ON a.id = b.merged_id
              ) as c
              group by hw_pid, id, nodeid, inlinkid, outlinkid,
                       accesstype, attr, estab_item
              ORDER BY hw_pid, min_seq_nm
        );
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('mid_hwy_org_hw_junction_mid_linkid_merged_id_idx')
        self.CreateIndex2('mid_hwy_org_hw_junction_mid_'
                          'linkid_merged_the_geom_idx')
        return 0

    def _merge_hw_jct(self):
        '''重新对应org_hw_jct表中id'''
        self.CreateTable2('mid_hwy_org_hw_jct_merged')
        sqlcmd = '''
        INSERT INTO mid_hwy_org_hw_jct_merged(id, s_junc_pid, e_junc_pid,
                                              dis_betw, direction, passlid)
        (
            SELECT a.id::bigint, b.id as s_junc_pid, c.id as e_junc_pid,
                   dis_betw::float, direction::int,
                   passlid||passlid2 as passlid
              FROM org_hw_jct AS a
              LEFT JOIN mid_hwy_org_hw_junction_merged_id as b
              on a.s_junc_pid::bigint = b.merged_id
              LEFT JOIN mid_hwy_org_hw_junction_merged_id as c
              on a.e_junc_pid::bigint = c.merged_id
              where passlid||passlid2 = '' and
                    b.id is not null and  -- For HKG
                    c.id is not null
        );
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        sqlcmd = '''
        INSERT INTO mid_hwy_org_hw_jct_merged(id, s_junc_pid, e_junc_pid,
                                              dis_betw, direction, passlid)
        (
        SELECT id, s_junc_pid, e_junc_pid,
               dis_betw, direction,
               array_to_string(array_agg(link_id), '|') as passlid
        FROM (
             SELECT distinct gid, id, s_junc_pid, e_junc_pid,
                dis_betw, direction, m.link_id, seq_num
             FROM (
                  SELECT gid, id, s_junc_pid, e_junc_pid,
                     dis_betw, direction, new_passlid[seq_num] AS old_link_id,
                     seq_num
                  FROM (
                       SELECT gid, id, s_junc_pid, e_junc_pid,
                          dis_betw, direction,  new_passlid::bigint[],
                          generate_series(1, array_upper(new_passlid, 1)) as seq_num
                       FROM (
                            SELECT gid, a.id::bigint, b.id as s_junc_pid, c.id as e_junc_pid,
                               dis_betw::float, direction::int, passlid,
                               regexp_split_to_array(passlid||passlid2, E'\\\\|+') as new_passlid
                            FROM org_hw_jct AS a
                            LEFT JOIN mid_hwy_org_hw_junction_merged_id as b
                            on a.s_junc_pid::bigint = b.merged_id
                            LEFT JOIN mid_hwy_org_hw_junction_merged_id as c
                            on a.e_junc_pid::bigint = c.merged_id
                            where passlid||passlid2 <> ''
                       ) AS a
                  ) AS b
             ) AS c
              JOIN mid_link_mapping as m
             ON c.old_link_id::bigint = m.org_link_id
             ORDER BY gid, seq_num
        ) as d
        group by gid, id, s_junc_pid, e_junc_pid, dis_betw, direction
        order by s_junc_pid, e_junc_pid
        )
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return 0

    def _make_link_id_mapping(self):
        # move to Hwy prepare
        return 0
        '''原始link和middle link对应表'''
        sqlcmd = """
        INSERT INTO mid_link_mapping(org_link_id, link_id)
        (
        SELECT a.id::bigint as org_link_id,
              (case
               when merge.link_id is not null then merge.link_id
               when split.link_id is not null then split.link_id
               else a.id::bigint
              end) as mid_link_id
        FROM org_r as a
        left join temp_split_newlink as split
        on a.id::bigint = split.old_link_id
        left join temp_merge_link_mapping as merge
        on (split.link_id = merge.merge_link_id) or
           (a.id::bigint = merge.merge_link_id)
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        pass

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
            SELECT m.org_link_id, l.id::bigint
              FROM  org_r as l, mid_link_mapping as m
              where l.id::bigint = m.org_link_id
          ) as a
          where org_link_id is NUll or id is NUll;
        """
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchone2()
        if row:
            if row[0] == 0:
                return True
        return False

