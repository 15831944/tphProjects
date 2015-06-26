# -*- coding: cp936 -*-
'''
Created on 2012-3-21

@author: zym
'''

import component.default.guideinfo_lane
from common import cache_file
ARROW_DIC = {1: 64,  # Turn left
            2: 65,  # Turn left & straight
            3: 1,  # Straight
            4: 5,  # Straight & turn right
            5: 4,  # Turn right
            6: 2048,  # U-turn
            7: 2052,  # U-Turn & turn right
            8: 68,  # Turn left & turn right
            9: 69,  # Turn left, straight & turn right
            99:-1  # Other
            }


class comp_guideinfo_lane_nostra(component.default.guideinfo_lane.comp_guideinfo_lane):
    '''诱导车线信息类(泰国版)
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.default.guideinfo_lane.comp_guideinfo_lane.__init__(self)

    def _DoCreateTable(self):
        self.CreateTable2('mid_temp_guidelane_passlink_shortest_distance')
        component.default.guideinfo_lane.comp_guideinfo_lane._DoCreateTable(self)
        self.CreateIndex2('lane_tbl_inlinkid_idx')
        self.CreateIndex2('lane_tbl_outlinkid_idx')

        return 0

    def _Do(self):
        self._create_tempLanetbl()
#         self._make_lanearrowinfo_and_passlink()
        self._make_lanearrowinfo_and_passlink_shortest_path()
        self._make_lane_tbl()
        return 0

    def _create_tempLanetbl(self):
        '''orgdata:one arcin to many arcout'''
        '''change into many records of one arcin to one arcout'''
        self.CreateTable2('temp_mid_lane_table')
        self.CreateTable2('temp_mid_laneinfo_table')
        sqlcmd = '''
        insert into temp_mid_lane_table(arcin, "no", sign_id,
                   change, lane, direction, arcout, "type")
                (
                SELECT arcin, no, sign_id, change, lane, direction,
                       arcout1 as out_link,  type
                  FROM org_laneinformation

                union

                SELECT arcin, no, sign_id, change, lane, direction,
                       arcout2 as out_link,  type
                  FROM org_laneinformation
                  where arcout2 is not null and arcout2 <> 0
                union

                SELECT arcin, no, sign_id, change, lane, direction,
                       arcout3 as out_link,  type
                  FROM org_laneinformation
                  where arcout3 is not null and arcout3 <> 0
                );'''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return 0

    def _create_tempindex_function(self):
        '''create indexs, tables, function'''
        self.CreateTable2('temp_mid_pathlinks_lane_tbl')
        self.CreateIndex2('temp_topo_link_end_node_idx')
        self.CreateIndex2('temp_topo_link_start_node_idx')
        self.CreateIndex2('temp_topo_link_routeid_idx')
        self.CreateIndex2('org_l_tran_oneway_idx')
        self.CreateIndex2('org_l_tran_routeid_idx')
        self.CreateFunction2('mid_get_laneinfo')
        self.CreateFunction2('mid_findpasslinkbybothlinks')
        self.CreateFunction2('mid_get_expand_box')
        return 0

    def _make_lanearrowinfo_and_passlink_shortest_path(self):
        self._create_tempindex_function()
        temp_file_obj = cache_file.open('signpost_passlink_short_distance')
        sqlcmd = '''
            select a.arcin::bigint, a.arcout::bigint,
            a.direction, a.lane,
            mid_get_laneinfo(lane,no_list),
            b.s_node, b.e_node,
               c.s_node, c.e_node,
               ST_ASEWKT(mid_get_expand_box(a.arcin::bigint,
                                            a.arcout::bigint,
                                            %s)
                        ),
            b.one_way_code, c.one_way_code
            FROM (
                  select arcin, arcout,array_agg(no) as no_list,
                        direction, lane
                    from temp_mid_lane_table
                    group by arcin,arcout,direction, lane
                ) as a
                left join link_tbl as b
                on a.arcin = b.link_id
                left join link_tbl as c
                ON a.arcout = c.link_id
            order by a.arcin,a.arcout;
            '''
        from component.default import link_graph as lg
        datas = self.get_batch_data(sqlcmd, (lg.UNITS_TO_EXPAND,))
        for data in datas:
            in_link_id = data[0]
            out_link_id = data[1]
            direction = data[2]
            lane_num = data[3]
            laneinfo = data[4]
            in_s_node = data[5]
            in_e_node = data[6]
            out_s_node = data[7]
            out_e_node = data[8]
            expand_box = data[9]
            in_oneway = data[10]
            out_oneway = data[11]
            graph_obj = lg.LinkGraph()
            paths = lg.all_shortest_paths_in_expand_box(graph_obj,
                                                expand_box,
                                                (in_s_node, in_e_node),
                                                (out_s_node, out_e_node),
                                                in_oneway_code = in_oneway,
                                                out_oneway_code = out_oneway
                                                       )
            if not paths:
                self.log.warning("Can't find the path. inlink=%d, outlink=%d" %
                                 (in_link_id, out_link_id))
                continue

            if len(paths) > 1:  # 存在多条最短路径
                self.log.warning('Shortest Paths Number>1.Inlink=%d,outlink=%d'
                                % (in_link_id, out_link_id))
            node_id = paths[0][0]
            if not node_id:
                self.log.error('No NodeId.')
            pass_link = graph_obj.get_linkid_of_path(paths[0])
            if pass_link:
                if None in pass_link:
                    self.log.error('Error PassLink.')
                pass_link_cnt = len(pass_link)
                pass_link = '|'.join(pass_link)
            else:
                pass_link = ''
                pass_link_cnt = 0
            second_link = (pass_link.split('|'))[0]
            if not second_link:
                second_link = out_link_id
            str_info = '%d\t%d\t%d\t%d\t%d\t%s\t%s\t%d' % (
                                                    in_link_id,
                                                    out_link_id,
                                                    node_id,
                                                    lane_num,
                                                    ARROW_DIC.get(direction),
                                                    laneinfo,
                                                    pass_link,
                                                    pass_link_cnt
                                                       )
            self._store_name_to_temp_file(temp_file_obj, str_info)
        # ## 把名称导入数据库
        temp_file_obj.seek(0)
        self.pg.copy_from2(temp_file_obj,
                           'mid_temp_guidelane_passlink_shortest_distance')
        self.pg.commit2()
        # close file
        #temp_file_obj.close()
        cache_file.close(temp_file_obj,True)
        return

    def _make_lane_tbl(self):
        insertsql = '''
                    INSERT INTO lane_tbl(id, inlinkid, outlinkid, passlid,
                    passlink_cnt,lanenum, laneinfo, arrowinfo,
                    nodeid,lanenuml, lanenumr, buslaneinfo)
                    values(%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s);
                '''
        sqlcmd = '''
                SELECT inlinkid, outlinkid, nodeid, lane_num, direction,
                    laneinfo, passlid,passlink_cnt
                  FROM mid_temp_guidelane_passlink_shortest_distance
                  where direction <> -1;
        '''
        datas = self.get_batch_data(sqlcmd)
        kid = 1
        for data in datas:
            inlinkid = data[0]
            outlinkid = data[1]
            nodeid = data[2]
            lane_num = data[3]
            direction = data[4]
            laneinfo = data[5]
            passlid = data[6]
            if passlid is None:
                passlid = ''
            passlink_cnt = data[7]
            businfo = '0' * lane_num
            self.pg.execute2(insertsql,
                             (kid, inlinkid, outlinkid, passlid,
                              passlink_cnt, lane_num, laneinfo, direction,
                              nodeid, 0, 0, businfo))
            kid = kid + 1
        self.pg.commit2()
        return

    def _make_lanearrowinfo_and_passlink(self):
        ''' make mid table -->lane_tbl'''
        self._create_tempindex_function()
        sqlcmd = '''select a.arcin::bigint, a.arcout::bigint,
                    a.direction, a.lane,
                    {s1},
                    mid_get_laneinfo(lane,no_list)
                    FROM (
                        select arcin, arcout,array_agg(no) as no_list,
                        direction,lane
                        from temp_mid_lane_table
                        group by arcin,arcout,direction, lane
                    ) as a
                    left join temp_topo_link as b
                    on a.arcin = b.routeid
                    left join org_l_tran as c
                    on a.arcin = c.routeid
                    {s2}
                    order by a.arcin,a.arcout;
        '''
        list = []
        list.append(('mid_findpasslinkbybothlinks(arcin::bigint, arcout::bigint,b.start_node_id,b.end_node_id, 1) as pathlink', '''where c.oneway is null '''))
        list.append(('mid_findpasslinkbybothlinks(arcin::bigint, arcout::bigint,b.start_node_id,b.end_node_id, 0) as pathlink', '''where c.oneway = 'FT' '''))
        list.append(('mid_findpasslinkbybothlinks(arcin::bigint, arcout::bigint, b.end_node_id,b.start_node_id, 0) as pathlink', '''where c.oneway = 'TF' '''))
        i = 0
        for (S1, S2) in list:
            tempsqlcmd = sqlcmd.format(s1=S1, s2=S2)
            self.pg.execute2(tempsqlcmd)
            result = self.pg.fetchall2()
            for row in result:
                i = i + 1
                pathlinks = row[4]
                if pathlinks is None:
                    self.log.error("can not find route between: \
                                in_link=%s, out_link=%s"
                               % (row[0], row[1]))
                    continue
                # handl with pathlink and nodeid##
                linklist = pathlinks.split('|')
                linkcount = len(linklist)
                inlink = linklist[0]
                secondlink = linklist[1]
                resultpathlink = ''
                if linkcount > 2:
                    resultpathlink = pathlinks[pathlinks.find('|') + 1:]
                    resultpathlink = \
                            resultpathlink[0:resultpathlink.rfind('|')]

                businfo = ''
                businfo = businfo + '0' * row[3]
                arrow_info = ARROW_DIC.get(row[2])
                if arrow_info == -1:
                    businfo = row[5]
                    continue
                insertsql = '''
                    INSERT INTO lane_tbl(id, inlinkid, outlinkid, passlid, passlink_cnt,
                        lanenum, laneinfo, arrowinfo, nodeid,lanenuml, lanenumr, buslaneinfo)
                    values(%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s);
                '''
                nodeid = self._get_node_between_two_links(inlink, secondlink)
                self.pg.execute2(insertsql, (i, row[0], row[1], resultpathlink,
                                             linkcount - 2, row[3],
                                             row[5], arrow_info,
                                             nodeid, 0, 0, businfo)
                                 )
            self.pg.commit2()

    def _get_node_between_two_links(self, link1, link2):
            ''' get intersect node between link1 and link2'''
            node_sqlcmd = '''
            SELECT routeid, start_node_id, end_node_id
              FROM temp_topo_link
              where routeid = %s;
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
                self.log.error('')
                return None
            return nodeid

    def _store_name_to_temp_file(self, file_obj, str_info):
        if file_obj:
            file_obj.write('%s\n' % str_info)
        return 0
