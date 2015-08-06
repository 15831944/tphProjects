# -*- coding: UTF8 -*-
'''
Created on 2015-6-29

@author: PC_ZH
'''
from component.rdf.hwy.hwy_route_rdf import HwyRouteRDF
from component.jdb.hwy.hwy_graph import HwyGraph


class HwyRouteZenrin(HwyRouteRDF):
    ''' '''

    def __init__(self, data_mng, ItemName='HwyRouteZenrin'):
        '''Constructor '''
        HwyRouteRDF.__init__(self, data_mng, ItemName=ItemName)

    def _Do(self):
        ''' '''
        #鍒朵綔楂橀�熸湰绾縨id_temp_hwy_main_path
        self._make_main_path()
        #娣诲姞鍦扮悊淇℃伅
        self._temp_update_geom()
        #鍒朵綔road_code_info琛�
        self._make_road_code()
        #鐢熸垚link瀵瑰簲鐨剅oad_code(hwy_link_road_code_info)
        self._make_link_road_code_info()
        #妫�鏌rg_highwaypath涓璴ink鏄惁閮藉湪鍋氭垚main path 涓�
        self.check_org_in_main_path()
        return 0

    def _make_main_path(self):
        '''make mid_temp_hwy_main_path '''
        self.log.info('Start Highway Main Link Path.')
        self.CreateTable2('mid_temp_hwy_main_path')
        for G, path_id, updown in self._load_main_link():
            if not G:
                continue
            node = next(G.nodes_iter(False), None)
            if node:
                path = G.dfs_path(node)
                link_ids = G.get_linkids(path)
                path_len = len(path)
                seq = 0
                for seq in range(0, path_len):
                    node_id = path[seq]
                    if seq == 0:
                        link_id = None
                    else:
                        link_id = link_ids[seq - 1]
                    self._insert_into_main_path((path_id, updown,
                                                seq, link_id, node_id))
                    seq += 1
            else:
                self.log.error('error no node')
                continue
        self.pg.commit2()
        self.log.info('End Make Highway Main Link Path.')

    def _load_main_link(self):
        sqlcmd = '''
        SELECT path_id,
               dirflag as updown,
               array_agg(m.link_id) as link_lid,
               array_agg(s_node) as s_node_lid,
               array_agg(e_node) as e_node_lid,
               array_agg(one_way_code) as one_way_code
        FROM(
             SELECT distinct pathid as path_id, seq,pathname,
                             meshcode, linkno, dirflag
             FROM org_highwaypath
             GROUP BY pathid, seq, meshcode, linkno, linktype, dirflag,pathname
             ORDER BY pathid, dirflag, seq
            ) AS a
        LEFT JOIN temp_link_mapping AS map
        ON a.linkno = map.linkno and
           a.meshcode::varchar = map.meshcode::varchar
        LEFT JOIN mid_link_mapping AS m
        ON map.link_id = m.org_link_id
        LEFT JOIN link_tbl
        ON m.link_id = link_tbl.link_id
        GROUP BY dirflag, path_id
        order by path_id, dirflag
        '''
        for rec in self.get_batch_data(sqlcmd):
            G = HwyGraph()
            path_id = rec[0]
            updown = rec[1]
            link_lid = rec[2]
            s_node_l = rec[3]
            e_node_l = rec[4]
            one_way_l = rec[5]
            for (u, v, one_way, link_id) in zip(s_node_l, e_node_l,
                                                one_way_l, link_lid):
                if not one_way:
                    print 'No one_way_code. link_id=%s' % link_id
                G.add_link(u, v, one_way, link_id)
            yield G, path_id, updown

    def _make_road_code(self):
        '''make road_code_info '''
        self.log.info('start make road code info')
        self.CreateTable2('road_code_info')
        sqlcmd = '''
        INSERT INTO road_code_info(path_id, road_name)
        (
         SELECT distinct pathid, pathname
           FROM org_highwaypath
           ORDER BY pathid
        );
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.log.info('end make road code info')
        return 0

    def _make_link_road_code_info(self):
        '''make hwy_link_road_code_info '''
        self.log.info('start make hwy_link_road_code_info')
        self.CreateTable2('hwy_link_road_code_info')
        sqlcmd = '''
        INSERT INTO hwy_link_road_code_info(road_code, updown, node_id,
                                            link_id, seq_nm, the_geom)
        (
            SELECT road_code, updown, node_id,
                   link_id, seq_nm, the_geom
            FROM mid_temp_hwy_main_path as a
            LEFT JOIN road_code_info as b
            ON a.path_id = b.path_id
            ORDER BY road_code, updown, seq_nm
        )
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.log.info('end make hwy_link_road_code_info')
        return 0

    def _insert_into_main_path(self, params):
        ''' '''
        sqlcmd = '''
        INSERT INTO mid_temp_hwy_main_path(path_id, updown,
                                           seq_nm, link_id, node_id)
        VALUES(%s, %s, %s, %s, %s)
        '''
        self.pg.execute2(sqlcmd, params)
        return 0

    def check_org_in_main_path(self):
        self.log.info('start check org link is in main path ')
        sqlcmd = '''
        SELECT c.link_id, meshcode, linkno, d.link_id
        FROM (
          SELECT distinct b.link_id, path.meshcode, path.linkno
            FROM org_highwaypath as path
            LEFT JOIN temp_link_mapping as a
            ON path.meshcode::varchar = a.meshcode::varchar
               and path.linkno = a.linkno
            LEFT JOIN mid_link_mapping as b
            ON a.link_id = b.org_link_id
        ) AS c
        FULL JOIN hwy_link_road_code_info as d
        ON c.link_id = d.link_id
        WHERE (c.link_id is null or d.link_id is null) and seq_nm <> 0
        '''
        for rec in self.pg.get_batch_data2(sqlcmd):
            if rec:
                link_id = rec[0]
                self.log.error('org link not in main path %s' % link_id)
        self.log.info('end check org link is in main path ')
        return 0
