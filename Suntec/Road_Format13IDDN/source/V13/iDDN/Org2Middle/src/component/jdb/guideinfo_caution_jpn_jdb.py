'''
Created on 2012-2-23

@author: sunyifeng
'''

import common
import base
import io
import os


class comp_guideinfo_caution_jpn(base.component_base.comp_base):

    def __init__(self):
        '''
        Constructor
        '''
        base.component_base.comp_base.__init__(self, 'Guideinfo Caution')

    def _DoCreateTable(self):

        self.CreateTable2('caution_tbl')
        self.CreateTable2('temp_boundary')
        sqlcmd = '''
            Alter table temp_boundary drop CONSTRAINT enforce_geotype_the_geom;
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateTable2('temp_hlink')
        self.CreateTable2('temp_inode')
        sqlcmd = '''
            Alter table temp_inode drop CONSTRAINT enforce_geotype_the_geom;
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateTable2('temp_bnode')
        self.CreateTable2('temp_guideinfo_boundary')
        self.CreateTable2('temp_admin_wavid')
        return 0

    def _DoCreateFunction(self):

        self.CreateFunction2('mid_get_inout_link')
        return 0

    def _Do(self):
        self.log.info('making guideinfo caution info ...')
        self.log.info('making guideinfo caution boundary info ...')
        self._DoBoundary()
        self.log.info('making guideinfo caution info OK.')
        self.log.info('making guideinfo caution boundary info OK.')
        self._check_passlink()

    def _DoBoundary(self):
        pg = self.pg
        pgcur = self.pg.pgcur2
        admin_wav_path = common.GetPath('admin_wav')
        f = io.open(admin_wav_path, 'r', 8192, 'utf8')
        pgcur.copy_from(f, 'temp_admin_wavid', ',', "", 8192, None)
        pg.commit2()
        f.close()

        sqlcmd = """
            INSERT into temp_boundary(ad_cd, the_geom)
                SELECT ad_cd, ST_Boundary(the_geom) as the_geom
                  FROM
                  (
                      select ad_code as ad_cd,the_geom
                      from mid_admin_zone
                      where ad_order = 1
                  ) as a;
        """

        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        sqlcmd = """
            INSERT INTO
                temp_hlink(link_id, s_node, e_node, one_way_code, the_geom)
                SELECT link_id, s_node, e_node, one_way_code, the_geom
                  FROM link_tbl
                  where road_type in (0,1) and link_type = 2;
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('temp_boundary_the_geom_idx')
        self.CreateIndex2('temp_hlink_the_geom_idx')
        sqlcmd = """
            INSERT INTO
                temp_inode(link_id, s_node, e_node, one_way_code, the_geom)
                SELECT distinct a.link_id, a.s_node, a.e_node, a.one_way_code,
                        ST_Intersection(a.the_geom, b.the_geom) as the_geom
                    FROM temp_hlink as a
                    inner join temp_boundary as b
                    on ST_Intersects(a.the_geom, b.the_geom) = TRUE
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        sqlcmd = """
            INSERT INTO temp_bnode(
                            link_id, s_node, e_node, one_way_code, bnode,
                            link_id2, s_node2, e_node2, one_way_code2, the_geom
                            )
                SELECT t1.link_id, t1.s_node, t1.e_node,
                        t1.one_way_code, t1.bnode,
                        t2.link_id, t2.s_node, t2.e_node,
                        t2.one_way_code, t1.the_geom
                    FROM
                    (
                        SELECT link_id, s_node, e_node, one_way_code,
                            case when slocate < 0.5 and slocate < 1 - elocate then s_node else e_node end as bnode,
                            case when slocate < 0.5 and slocate < 1 - elocate then sp else ep end as the_geom,
                            slocate,
                            elocate
                        FROM
                        (
                        SELECT a.link_id, a.s_node, a.e_node, a.one_way_code,
                            ST_Line_Locate_Point(the_geom, sbp) as slocate,
                            ST_Line_Locate_Point(the_geom, ebp) as elocate,
                            sp,
                            ep
                            FROM
                              (
                              select m.link_id, m.s_node, m.e_node, m.one_way_code, n.the_geom, 
                                    ST_ClosestPoint(m.the_geom, ST_StartPoint(n.the_geom)) as sbp,
                                    ST_ClosestPoint(m.the_geom, ST_EndPoint(n.the_geom)) as ebp,
                                    ST_StartPoint(n.the_geom) as sp,
                                    ST_EndPoint(n.the_geom) as ep
                                from temp_inode as m
                                inner join link_tbl as n
                                on m.link_id = n.link_id
                              ) as a
                        ) as t
                    ) as t1
                    inner join link_tbl as t2
                    on t1.bnode = t2.s_node or t1.bnode = t2.e_node
                    where t1.link_id != t2.link_id;
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        sqlcmd = """
            INSERT INTO temp_guideinfo_boundary(
                inlinkid, nodeid, outlinkid, innode, outnode, out_adcd
                )
                SELECT distinct inlinkid, bnode, outlinkid, innode,
                    outnode, b.ad_cd
                from
                (
                    SELECT links[1] as inlinkid, bnode, links[2] as outlinkid, 
                        links[3] as innode, links[4] as outnode
                    from
                    (
                    SELECT
                        bnode,
                        mid_get_inout_link(
                                link_id, s_node, e_node, one_way_code,
                                link_id2, s_node2, e_node2, one_way_code2
                                ) as links
                        from temp_bnode
                    ) as t
                    where links is not null
                ) as a
                inner join node_tbl as c
                on a.outnode = c.node_id
                inner join (
                    select ad_code as ad_cd,the_geom
                    from mid_admin_zone
                    where ad_order = 1
                    )as b
                on ST_Intersects(c.the_geom, b.the_geom) = TRUE;
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        sqlcmd = """
            INSERT INTO caution_tbl(
                inlinkid, nodeid, outlinkid, passlid,
                passlink_cnt, data_kind, voice_id
            )
                select inlinkid, nodeid, outlinkid, null, 0, 4, b.wav_id
                    from temp_guideinfo_boundary as a
                    left join temp_admin_wavid as b
                    on a.out_adcd = b.ad_cd
                    order by inlinkid, nodeid, outlinkid;
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return 0

    def _check_passlink(self):
        self.CreateFunction2('mid_check_guide_item_new')
        check_sql = '''
            select mid_check_guide_item_new({0});
        '''
#         try:
        self.pg.execute2(check_sql.format("\'caution_tbl\'"))
        row = self.pg.fetchone2()
        if row[0]:
            self.log.warning('passlink is not continuous,num=%s!!!', row[0])
            pass
#         except Exception:
#             self.log.error('passlink is not continuous!!!')
#             self.pg.commit2()
        return
