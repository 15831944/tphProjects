# -*- coding: cp936 -*-
'''
Created on 2012-3-21

@author: zym
'''

import component.default.guideinfo_lane


class comp_guideinfo_lane_msm(component.default.guideinfo_lane.comp_guideinfo_lane):
    '''诱导车线信息类(malas版)
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.default.guideinfo_lane.comp_guideinfo_lane.__init__(self)

    def _DoCreateTable(self):
        component.default.guideinfo_lane.comp_guideinfo_lane._DoCreateTable(self)
        self.CreateIndex2('lane_tbl_inlinkid_idx')
        self.CreateIndex2('lane_tbl_outlinkid_idx')
        return 0

    def _Do(self):
        self._create_temp_lane_info()
        self._orglink_to_midlink_lane()
        self._get_passlink_dir()
        self._make_lane_tbl()
        return 0

    def _create_temp_lane_info(self):
        self.CreateTable2('temp_lane_info')
        self.CreateFunction2('make_temp_lane_info')
        self.pg.execute2('select make_temp_lane_info();')
        return 0

    def _orglink_to_midlink_lane(self):
        self.CreateTable2('temp_lane_in_topolink')
        self.CreateTable2('temp_topo_link_org_oneway')
        sql_cmd = '''
            insert into temp_topo_link_org_oneway(
                link_id,start_node_id,end_node_id,oneway
            )
            select a.new_link_id,a.start_node_id,a.end_node_id,b.one_way
            from temp_topo_link as a
            left join org_processed_line as b
            on a.link_id = b.link_id and a.folder = b.folder;
        '''
        self.pg.execute2(sql_cmd)
        self.pg.commit2()
        sql_cmd_midlink = '''
			delete from temp_lane_info where folder = 'KL' and outlinkid in (

			  select a.link_id
			  from org_processed_line_backup as a
			  left join org_processed_line as b
			  on a.link_id = b.link_id and a.folder = b.folder
			  where b.link_id is null
			);

            insert into temp_lane_in_topolink(link_id,outlinkid,laneno,lane_cnt_f,lane_cnt_r)
            select c.new_link_id,b.new_link_id,a.laneno,a.lane_cnt_f,a.lane_cnt_r
            from temp_lane_info as a
            left join temp_topo_link as b
            on a.outlinkid = b.link_id and a.folder = b.folder
            left join temp_topo_link as c
            on a.link_id = c.link_id and a.folder = c.folder;
        '''
        self.pg.execute2(sql_cmd_midlink)
        self.pg.commit2()
        return 0

    def _get_passlink_dir(self):
        self.CreateIndex2('temp_topo_link_org_oneway_end_node_id_idx')
        self.CreateIndex2('temp_topo_link_org_oneway_start_node_id_idx')
        self.CreateTable2('temp_lane_info_passlink_dir')
        self.CreateFunction2('mid_findpasslinkbybothlinks')
        self.CreateFunction2('make_nodeid_by_passlink')
        sql_cmd = '''
            select a.link_id,a.outlinkid,a.lanenos,a.lane_cnt_f,a.lane_cnt_r,passlinkstr,dir::smallint,make_nodeid_by_passlink(passlinkstr)
            from(
                select a.*,SUBSTRING(passlink,1,pos_cnt-1) as passlinkstr,SUBSTRING(passlink,pos_cnt+1,1) as dir
                from(
                    select a.*,position('(' in passlink) as pos_cnt
                    from(
                    select a.link_id,a.outlinkid,a.lanenos,a.lane_cnt_f,a.lane_cnt_r,
                        case when b.oneway = 1 then mid_findpasslinkbybothlinks(a.link_id,a.outlinkid,0,b.end_node_id,0)
                         when b.oneway = 0 then mid_findpasslinkbybothlinks(a.link_id,a.outlinkid,b.start_node_id,b.end_node_id,1)
                         else ''
                        end as passlink
                    from (
                        select link_id,outlinkid,lane_cnt_f,lane_cnt_r,array_agg(laneno::integer) as lanenos
                        from temp_lane_in_topolink
                        group by link_id,outlinkid,lane_cnt_f,lane_cnt_r
                    )as a
                    left join temp_topo_link_org_oneway as b
                    on a.link_id = b.link_id
                    ) as a
                ) as a
            ) as a;
        '''
        self.pg.execute2(sql_cmd)
        rows = self.pg.fetchall2()
        insert_sqlcmd = '''
            INSERT INTO temp_lane_info_passlink_dir(
                link_id, outlinkid, lanenos, lane_cnt_f, lane_cnt_r, passlink,
            passlink_dir, nodeid)
            VALUES (%s, %s, %s, %s, %s, %s,%s, %s);
        '''
        for row in rows:
            inlinkid = row[0]
            outlinkid = row[1]
            lanenos = row[2]
            lane_cnt_f = row[3]
            lane_cnt_r = row[4]
            passlinkstr = row[5]
            dir = row[6]
            nodeid = row[7]
            if passlinkstr:
                self.pg.execute(insert_sqlcmd, (
                        inlinkid, outlinkid, lanenos, lane_cnt_f,
                        lane_cnt_r, passlinkstr, dir, nodeid
                                                ))
            else:
                self.log.warning("Can't find the path. inlink=%d, outlink=%d" %
                                 (inlinkid, outlinkid))
        self.pg.commit2()
        return 0

    def _make_lane_tbl(self):
        sql_cmd = '''
            INSERT INTO lane_tbl(
            id, nodeid, inlinkid, outlinkid, passlid, passlink_cnt,
            lanenum, laneinfo, arrowinfo, lanenuml, lanenumr, buslaneinfo)
            VALUES (%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s);
        '''
        get_lanenum_sql = '''
            SELECT lanenos
            FROM temp_lane_info_passlink_dir
            where link_id = %s and passlink_dir = %s;
        '''
        self.pg.execute2('''
            SELECT link_id, outlinkid, lanenos, lane_cnt_f, lane_cnt_r, passlink,
               passlink_dir, nodeid
            FROM temp_lane_info_passlink_dir;
        ''')
        rows = self.pg.fetchall2()
        i = 0
        for row in rows:
            laneno_max = 0
            lanenum = 0
            i = i + 1
            lane_id = i
            nodeid = row[7]
            inlinkid = row[0]
            outlinkid = row[1]
            lanenos = row[2]
            lane_cnt_f = row[3]
            lane_cnt_r = row[4]
            passlinks = row[5].split('|')
            pass_link_cnt = len(passlinks) - 2
            pass_link = '|'.join(passlinks[1:pass_link_cnt+1])
            passlink_dir = row[6]
            self.pg.execute2(get_lanenum_sql, (inlinkid,passlink_dir))
            lanenolist_re = self.pg.fetchall2()
            for lanenolist in lanenolist_re:
                temp_v = max(lanenolist[0])
                if temp_v > laneno_max:
                    laneno_max = temp_v
            if passlink_dir == 2:
                if lane_cnt_f and lane_cnt_f >= laneno_max:
                    lanenum = lane_cnt_f
                else:
                    lanenum = laneno_max
            else:
                if lane_cnt_r and lane_cnt_r >= laneno_max:
                    lanenum = lane_cnt_r
                else:
                    lanenum = laneno_max
            init_laneno = ['0' for j in range(lanenum)]
            for laneno in lanenos:
                init_laneno[lanenum - laneno] = '1'
            self.pg.execute2(sql_cmd, (
                            lane_id, nodeid, inlinkid, outlinkid, pass_link,
                            pass_link_cnt, lanenum, ''.join(init_laneno), None, 0, 0, lanenum * '0'
                            ))
        self.pg.commit2()
        return
