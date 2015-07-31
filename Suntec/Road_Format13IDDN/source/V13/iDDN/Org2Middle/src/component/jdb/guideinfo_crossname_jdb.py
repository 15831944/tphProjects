# -*- coding: UTF8 -*-
'''
Created on 2012-3-27

@author:
'''
import os
from component.guideinfo_crossname import comp_guideinfo_crossname
import component


class comp_guideinfo_crossname_jdb(comp_guideinfo_crossname):
    def __init__(self):
        '''
        Constructor
        '''
        component.guideinfo_crossname.comp_guideinfo_crossname.__init__(self)

    def _DoCreateFunction(self):
        self.CreateFunction2('mid_get_json_string_for_japan_name')

    def _Do(self):
        self._make_node_name_by_intersect()
        self._make_node_name_by_road_node()
        self._make_node_name_by_road_toll_node()
        return 0

    def _make_node_name_by_intersect(self):
        '''get node name by inf_intersection'''
        insert_sql = '''
            INSERT INTO crossname_tbl(
            inlinkid, nodeid, outlinkid, passlid, passlink_cnt, namestr
            )
            VALUES (%s, %s, %s, %s, %s, %s);;
        '''
        sqlcmd = '''
            select links,linkdirs,
                mid_get_json_string_for_japan_name(name_kanji,name_yomi)
            from(
                select array_agg(link_id) as links,
                    array_agg(linkdir_c) as linkdirs,inf_id
                from(
                      SELECT objectid, link_id, linkdir_c, "sequence", inf_id
                      FROM lq_intersection
                      order by inf_id,sequence
                     ) as a
                group by inf_id
             ) as b
            left join inf_intersection as c
            on b.inf_id = c.objectid
            where name_kanji is not null or name_yomi is not null;
        '''
        self.pg.execute2(sqlcmd)
        rows = self.pg.fetchall2()
        for row in rows:
            links = row[0]
            inlink = links[0]
            linklength = len(links)
            outlink = None
            passlib = ''
            passlinkcnt = 0
            fromnodeid = None
            if linklength > 1:
                outlink = links[linklength - 1]
                passlib = '|'.join([str(x) for x in links[1:linklength - 1]])
                passlinkcnt = linklength - 2
                fromnodeid = self._get_fromnode_by_linkdir(
                                            outlink,
                                            (row[1])[linklength - 1]
                                                )
            inlink_dir = (row[1])[0]
            nodeid = self._get_tonode_by_linkdir(inlink, inlink_dir)
            if not nodeid:
                self.log.error('can not get nodeid!!!')
                continue
            mid_tile_link_node_sql = '''
                    select passlink, inlinkid, outlinkid, nodeid,
                    (CASE WHEN
                        passlink is null or passlink = '' THEN 0
                        ELSE
                            array_upper(
                                regexp_split_to_array(passlink, E'\\\|+'),
                                1
                            )
                        END
                        ) as passlink_cnt
                    from(
                        select 1 as id, mid_get_new_passlid(%s, %s,'|') as passlink,
                                tile_link_id as inlinkid
                        from middle_tile_link
                        where %s = old_link_id and %s = Any(old_s_e_node)
                    ) as a
                    left join
                        (select tile_link_id as outlinkid,1 as id
                        from middle_tile_link
                        where %s = old_link_id and %s = Any(old_s_e_node)
                    ) as b
                    on a.id = b.id
                    left join
                    (
                        select tile_node_id as nodeid,1 as id
                        from middle_tile_node
                        where %s = old_node_id
                    ) as c
                    on a.id = c.id
            '''
            self.pg.execute2(mid_tile_link_node_sql, (passlib, outlink, inlink,
                                                      nodeid,
                                                    outlink, fromnodeid, nodeid
                                                      ))
            result = self.pg.fetchone2()
            namestr = row[2]
            self.pg.execute2(insert_sql, (result[1], result[3], result[2],
                                          result[0], result[4], namestr))
        self.pg.commit2()
        return 0

    def _get_tonode_by_linkdir(self, inlink, inlink_dir):
        sqlcmd = '''
            SELECT from_node_id, to_node_id
            FROM road_link
            where objectid = %s;
        '''
        self.pg.execute2(sqlcmd, (inlink,))
        row = self.pg.fetchone2()
        if inlink_dir == 1:
            return row[1]
        elif inlink_dir == 2:
            return row[0]
        else:
            return None

    def _get_fromnode_by_linkdir(self, tolink, tolink_dir):
        sqlcmd = '''
            SELECT from_node_id, to_node_id
            FROM road_link
            where objectid = %s;
        '''
        self.pg.execute2(sqlcmd, (tolink,))
        row = self.pg.fetchone2()
        if tolink_dir == 1:
            return row[0]
        elif tolink_dir == 2:
            return row[1]
        else:
            return None

    def _make_node_name_by_road_node(self):
        '''get node name by road_node'''
        insert_sql = '''
            INSERT INTO crossname_tbl(inlinkid, nodeid, outlinkid,
                            namestr, passlink_cnt,passlid)
            VALUES (%s, %s, %s, %s, %s,%s);
        '''
        sqlcmd = '''
            select tile_node_id,namestr
            from(
                SELECT objectid,
                    mid_get_json_string_for_japan_name(name_kanji, name_yomi) as namestr
                FROM road_node
                where name_kanji is not null or name_yomi is not null
            ) as a
            left join middle_tile_node as b
            on a.objectid = b.old_node_id;
        '''
        rows = self.get_batch_data(sqlcmd)
        for row in rows:
            nodeid = row[0]
#             namestr = self.get_name_json_format(row[1], row[2])
            self.pg.execute2(insert_sql, (None, nodeid, None, row[1], 0, ''))
        self.pg.commit2()
        return 0

    def _make_node_name_by_road_toll_node(self):
        '''get node name by road_tollnode'''
        file_path = os.path.join('.', 'cache', 'crossname_jdb_log.txt')
        if os.path.isfile(file_path):
            os.remove(file_path)
        f = open(file_path, "w")
        update_sql = '''
                UPDATE crossname_tbl
                SET namestr=%s
                WHERE gid=%s;
        '''
        cmp_sql = '''
                SELECT gid
                FROM crossname_tbl
                where inlinkid is null and nodeid = %s;
        '''
        insert_sql = '''
            INSERT INTO crossname_tbl(inlinkid, nodeid, outlinkid,
                                namestr, passlink_cnt, passlid)
            VALUES (%s, %s, %s, %s,%s,%s);
        '''
        sqlcmd = """
        select tile_node_id,name_ks,name_ys,mid_get_json_string_for_japan_name(
                                            name_ks[1],
                                            name_ys[1]
                                                    ) as namestr
        from(
              select node_id, array_agg(name_kanji) as name_ks,
                    array_agg(name_yomi) as name_ys
              from
              (
                  SELECT  b.objectid as node_id, c.name_kanji, c.name_yomi
                    FROM (
                        select *
                        from highway_node_4326
                        where hwymode_f = 1 and dummytoll_f <> 1 and tollclass_c in (1,2,3) and tollfunc_c > 0
                    )as a
                    inner join road_node_4326 as b
                    on ST_Equals(b.the_geom, a.the_geom)
                    left join (
                          select *
                          from facil_info_point_4326
                          where facilclass_c = 6
                    )as c
                    on a.road_code = c.road_code and a.road_seq = c.road_seq
                    where (c.name_kanji is not null or c.name_yomi is not null)
                    and c.name_kanji like '%料金所'

                    union

                    SELECT  b.objectid as node_id, c.name_kanji, c.name_yomi
                    FROM (
                    select *
                    from highway_node_4326
                    where hwymode_f = 1 and dummytoll_f <> 1 and tollclass_c in (1,2,3) and tollfunc_c > 0 and tollclass_c = 3
                    )as a
                    inner join road_node_4326 as b
                    on ST_Equals(b.the_geom, a.the_geom)
                    left join facil_info_point_4326 as c
                    on a.road_code = c.road_code and a.road_seq = c.road_seq
                    where c.name_kanji is not null or c.name_yomi is not null
              ) as d
              group by node_id
            ) as a
            left join middle_tile_node as b
            on a.node_id = b.old_node_id;
        """
#         rows = self.get_batch_data(sqlcmd)
        self.pg.execute2(sqlcmd)
        rows = self.pg.fetchall2()
        for row in rows:
            nodeid = row[0]
            name_ks = row[1]
            if len(name_ks) > 1:
                print >> f, "one node has many names,nodeid=%s!!!!,name1=%s,name2=%s" \
                        % (nodeid, name_ks[0], name_ks[1])
#               self.log.warning('one node has many names,nodeid=%s!!!!,name1=%s,name2=%s', nodeid, name_ks[0], name_ks[1])
#             namestr = self.get_name_json_format(name_ks[0], (row[2])[0])
            self.pg.execute2(cmp_sql, (nodeid,))
            cmp_row = self.pg.fetchone2()
            if cmp_row:
                self.pg.execute2(update_sql, (row[3], cmp_row[0]))
            else:
                self.pg.execute2(insert_sql, (None, nodeid, None,
                                                row[3], 0, ''))
        self.pg.commit2()
        f.close()
        return 0
