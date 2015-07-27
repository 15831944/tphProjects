# -*- coding: cp936 -*-
'''
Created on 2013-7

@author: zym
'''

import component.component_base


class comp_guideinfo_lane_mmi(component.component_base.comp_base):
    '''
    classdocs
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Guideinfo_Lane')

    def _DoCreateTable(self):
        if self.CreateTable2('lane_tbl') == -1:
            return -1

        return 0

    def _DoCreateIndex(self):
        'create index.'
        return 0

    def _DoCreateFunction(self):
        self.CreateFunction2('mid_get_expand_box')
        return 0

    def _Do(self):
        self._make_lane_tbl()
        return 0

    def _make_lane_tbl(self):
        '''得到inlink，outlink，nodeid，direction，oneway，laneno'''
        insertsql = '''
                INSERT INTO lane_tbl(id, inlinkid, outlinkid, passlid,
                    passlink_cnt, lanenum, laneinfo, arrowinfo,
                    nodeid, lanenuml, lanenumr, buslaneinfo)
                values(%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s);
                '''
        sqlcmd = '''           
        select f.nodeid, f.inlink, f.outlink, f.laneno_arr,
                f.direction, f.oneway_lane, f.seqnrs,
                ST_ASEWKT(mid_get_expand_box(f.inlink::bigint,
                            f.outlink::bigint, %s)
                        ),
                g.s_node, g.e_node, g.one_way_code
            from
            (
              select nodeid,inlink,outlink,direction,oneway_lane,seqnrs,array_agg(laneno) as laneno_arr from
              (    
                  select e.nodeid, e.inlink, e.outlink, e.laneno,
                        e.direction, e.oneway_lane, array_agg(seqnr) as seqnrs
                  from
                    (
                        select distinct c.*,d.seqnr
                        from
                        (
                            SELECT distinct junction as nodeid,
                                from_edge as inlink,to_edge as outlink,
                                fr_seqnr as laneno,b.dr_cat as direction,
                                side as oneway_lane
                            FROM org_lane_connectivity as a
                            join org_lane as b
                            on a.from_edge = b.edge_id and a.fr_seqnr = b.seqnr
                        ) as c
                        left join org_lane as d
                        on c.inlink = d.edge_id and c.oneway_lane = d.side
                        order by c.inlink,c.outlink,d.seqnr desc
                    ) as e
                    group by e.nodeid, e.inlink, e.outlink, e.laneno,
                            e.direction, e.oneway_lane
                           order by e.nodeid, e.inlink, e.outlink, 
                            e.direction, e.oneway_lane,e.laneno desc
                ) a
                group by nodeid,inlink,outlink,direction,oneway_lane,seqnrs
            )as f
            left join link_tbl as g
            on f.outlink = g.link_id;
        '''
        from component.default import link_graph as lg
        rows = self.get_batch_data(sqlcmd, (lg.UNITS_TO_EXPAND,))
        gid = 1
        for row in rows:
            nodeid = row[0]
            inlink = row[1]
            outlink = row[2]
            laneno_arr = row[3]
            direction = row[4]
            oneway = row[5]
            lanes = row[6]
            '''check lane if series'''
            check_flag = self._check_lanes(laneno_arr, lanes)

            lanenum = len(lanes)
            laneinfo = ''

            '''get laneinfo'''
            if check_flag:
                init_lanes = ['0'] * lanenum
                for laneno in laneno_arr:
                    init_lanes[lanes.index(laneno)] = '1'
                laneinfo = ''.join(init_lanes)
            else:
                continue

            if oneway == 'TF':
                laneinfo = laneinfo[::-1]
            elif not oneway and lanenum != 1:
                self.log.error('lane trffic direction is error!!!')
                continue

            '''get lane arrow direction'''
            '''in org data only u-turn right relation to value different'''
            if direction >= 256:
                direction = direction - 256 + 2048

            '''get passlink and passlinkcnt'''
            temp_node = self._getnode_between_links(inlink, outlink)
            pass_link = ''
            pass_link_cnt = 0
            # 如果inlink和outlink不相连
            if not temp_node or temp_node != nodeid:
                expand_box = row[7]
                out_s_node = row[8]
                out_e_node = row[9]
                out_oneway = row[10]
                graph_obj = lg.LinkGraph()
                paths = list()
                if out_oneway == lg.ONE_WAY_BOTH:
                    paths = lg.all_shortest_paths_in_expand_box(graph_obj,
                                                                expand_box,
                                                                (nodeid,),
                                                         (out_s_node, out_e_node),
                                                         direction = 1
                                                               )
                else:
                    if out_oneway == lg.ONE_WAY_POSITIVE:
                        end_node = out_s_node
                    elif out_oneway == lg.ONE_WAY_RERVERSE:
                        end_node = out_e_node
                    else:
                        continue
                    paths = lg.all_shortest_paths_in_expand_box(graph_obj,
                                                                expand_box,
                                                                (nodeid,),
                                                                (end_node,),
                                                                direction = 2
                                                                )
                if not paths:
                    self.log.warning("Can't find the path. inlink=%d, outlink=%d" %
                                     (inlink, outlink))
                    continue

                if len(paths) > 1:  # 存在多条最短路径
                    self.log.warning('Shortest Paths Number>1.Inlink=%d,outlink=%d'
                                    % (inlink, outlink))
                node_id = paths[0][0]
                if node_id != nodeid:
                    self.log.error('node is not match with node of org_data!!!')
                pass_link = graph_obj.get_linkid_of_path(paths[0])
                if pass_link:
                    if None in pass_link:
                        self.log.error('Error PassLink.')
                    pass_link_cnt = len(pass_link)
                    pass_link = '|'.join(pass_link)
                else:
                    self.log.error('not normal stuation！！！.')

            businfo = lanenum * '0'
            self.pg.execute2(insertsql, (gid, inlink, outlink, pass_link,
                                         pass_link_cnt, lanenum, laneinfo,
                                         direction, nodeid, 0, 0, businfo))
            gid = gid + 1
        self.pg.commit2()
        return 0

    def _check_lanes(self, laneno_arr, lanes):
        for laneno in laneno_arr:
            if laneno not in lanes:
                return False
        length = len(lanes)
        if (lanes[0] - lanes[length - 1] + 1) != length:
            return False
        return True

    def _getnode_between_links(self, link1, link2):
        ''' get intersect node between link1 and link2'''
        node_sqlcmd = '''
            SELECT id, f_jnctid, t_jnctid
            FROM org_city_nw_gc_polyline
            where id = %s;
        '''
        self.pg.execute2(node_sqlcmd, (link1,))
        inres_row = self.pg.fetchone2()
        self.pg.execute2(node_sqlcmd, (link2,))
        outres_row = self.pg.fetchone2()
        nodeid = 0
        if inres_row[1] in (outres_row[1], outres_row[2]):
            nodeid = inres_row[1]
        elif inres_row[2] in (outres_row[1], outres_row[2]):
            nodeid = inres_row[2]
        else:
            return None
        return nodeid
