# -*- coding: UTF8 -*-
'''
Created on 2012-9-4

@author: zym
'''

import base
import os

DLANE_DIR_P = 2
DLANE_DIR_N = 3
DLANE_DIR_B = 1
ALL_CAR = 0
PASSENGER_CAR = 11


class comp_guideinfo_lane_ta(base.component_base.comp_base):
    '''
    This class deals with guide lane information
    '''

    def __init__(self):
        '''
        Constructor
        '''
        base.component_base.comp_base.__init__(self, 'Guideinfo_Lane')

    def _DoCreateTable(self):
        if self.CreateTable2('lane_tbl') == -1:
            return -1
        if self.CreateTable2('temp_lane_link_info') == -1:
            return -1
        if self.CreateTable2('temp_lane_link_node') == -1:
            return -1
        return 0

    def _DoCreateIndex(self):
        'create index.'
        if self.CreateIndex2('temp_lane_link_info_id_idx') == -1:
            return -1
        if self.CreateIndex2('org_ln_id_idx') == -1:
            return -1
        if self.CreateIndex2('org_ld_id_validity_idx') == -1:
            return -1
        if self.CreateIndex2('org_lf_id_validity_idx') == -1:
            return -1
        return 0

    def _DoCreateFunction(self):
        return 0

    def _Do(self):
        # 制作inlink、outlink、passlink
        self._make_link_info()
        # 制作node\laneno
        self._make_node_laneno()
        # 制作lane_tbl
        self._make_lane_tbl()
        return 0

    # 求得inlink、outlink、passlink
    def _make_link_info(self):

        sqlcmd = '''
        insert into temp_lane_link_info(
            id, inlink, outlink, passlink
        )
        (
            select id,links[1] as inlink,links[length] as outlink,
                array_to_string (links[2:length-1],'|') as passlink
            from(
                    select id, array_agg(seqnr), array_agg(trpelid) as links,
                            count(*) as length
                    from(
                            SELECT gid, id, seqnr, trpelid, trpeltyp
                            FROM org_lp
                            order by id,seqnr
                      ) as a
                    group by id
            ) as b
            where length > 1
        );
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return 0

    # 求得node\laneno
    def _make_node_laneno(self):

        sqlcmd = '''
            insert into temp_lane_link_node(
                id, inlink, outlink, passlink, nodeid, laneno
            )
            (
                SELECT a.id, inlink, outlink, passlink, jnctid,
                   substr(fromto, 1, position('/' in fromto)-1)::integer laneno
                FROM temp_lane_link_info as a
                left join org_ln as b
                on a.id = b.id
                where b.id is not null
              );
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return 0

    def _make_arrow_direction(self, inlink, laneno, f):
        # 求车线的箭头方向
        direction = -1
        # 特定车线上的箭头方向
        sqlcmd = '''
            SELECT distinct direction
            FROM org_ld
            where id = %s and substr(validity,%s,1) = '1';
        '''
        self.pg.execute2(sqlcmd, (inlink, laneno + 1))
        rows = self.pg.fetchall2()
        length = len(rows)
        if length > 1:
            print >> f, "laneno:%s lane of inlink=%s has more arrow!!!!" \
                        % (laneno, inlink)
#      self.log.warning('laneno:%s lane of inlink=%s has more arrow!!!!',
#                  laneno, inlink)
            return None
        elif length == 1:
            direction = rows[0][0]
            return direction
        else:
            print >> f, "laneno:%s lane of inlink=%s has not arrow!!!!" \
                        % (laneno, inlink)
#             self.log.warning('laneno:%s lane of inlink=%s has not arrow!!!!',
#                             laneno, inlink)
            return None

    def _get_one_dir_laneinfo(self, inlink, direc, laneno):
        # inlink单方向，求laneinfo
        laneinfo_str = ''
        sqlcmd = '''
            SELECT distinct dflane, vt, validity
            FROM org_lf
            where id = %s and dflane = %s and vt = %s
                and substr(validity,%s,1) = '1';
        '''
        self.pg.execute2(sqlcmd, (inlink, direc, ALL_CAR, laneno + 1))
        rows = self.pg.fetchall2()
        if not rows:
            self.pg.execute2(sqlcmd, (inlink, direc,
                                      PASSENGER_CAR, laneno + 1))
            rows = self.pg.fetchall2()
        length = len(rows)
        if length == 1:
            lannum = len(rows[0][2]) - 1
            for i in range(lannum):
                    if i == (laneno - 1):
                        laneinfo_str = laneinfo_str + '1'
                    else:
                        laneinfo_str = laneinfo_str + '0'
            if direc == DLANE_DIR_N:
                laneinfo_str = laneinfo_str[::-1]
            if direc == DLANE_DIR_B:
                self.log.error('dflane = 1 data is not normal!!!')
                return laneinfo_str
        else:
            self.log.error('not find lanes follow with lane given!!!')
        return laneinfo_str

    def _get_both_dirs_laneinfo(self, inlink, laneno):
        # inlink双方向，求laneinfo,只考虑dflane为2,3的情况，其他情况会记录错误日志
        laneinfo = ''
        sqlcmd = '''
                SELECT distinct dflane, vt, validity
                FROM org_lf
                where id = %s and vt = %s
                    and substr(validity, %s, 1) = '0';
            '''
        self.pg.execute2(sqlcmd, (inlink, ALL_CAR, laneno + 1))
        rows = self.pg.fetchall2()
        if not rows:
            self.pg.execute2(sqlcmd, (inlink, PASSENGER_CAR, laneno + 1))
        length = len(rows)
        if length == 1:
            dflane = rows[0][0]
            validity = rows[0][2]
            str_len = len(validity)
            lannum = 0
            lannum = validity.count('0')
            # 如果车道在右
            if validity[1] == '0':
                for i in range(lannum):
                    if i == (laneno - 1):
                        laneinfo = laneinfo + '1'
                    else:
                        laneinfo = laneinfo + '0'
            else:
                i = lannum
                start = str_len - 1 - i
                while start < str_len - 1:
                    if start == (laneno - 1):
                        laneinfo = laneinfo + '1'
                    else:
                        laneinfo = laneinfo + '0'
                    start = start + 1
            if dflane == DLANE_DIR_P:
                laneinfo = laneinfo[::-1]
            if dflane == DLANE_DIR_B:
                self.log.error('dflane = 1 data is not normal!!!')
                return laneinfo

        # 有一些只有一条车道的双向link，这样的link不收录到org_lf
        # 如果只有一条车道不能保证businfo的制作
        elif length == 0:
            sqlcmd_only_lane = '''
                    SELECT distinct lanes
                    FROM org_nw
                    where id = %s;
                '''
            self.pg.execute2(sqlcmd_only_lane, (inlink,))
            row = self.pg.fetchone2()
            if row and row[0] == 1:
                laneinfo = '1'
            else:
                self.log.error('not find lanes follow with lane given!!!')
        else:
            self.log.error('find more lanes follow with lane given!!!')
        return laneinfo

    def _make_lane_info(self, inlink, laneno, oneway):
        # 特定车线上的方向
        laneinfo = ''
        # 车线同向---正向
        if oneway == 'FT':
            laneinfo = self._get_one_dir_laneinfo(inlink, DLANE_DIR_N, laneno)
        # 反向
        elif oneway == 'TF':
            laneinfo = self._get_one_dir_laneinfo(inlink, DLANE_DIR_P, laneno)
        # 双向
        elif oneway is None:
            laneinfo = self._get_both_dirs_laneinfo(inlink, laneno)
        else:
            self.log.error('oneway wrong value!!!')
        return laneinfo

    def _make_buslane_info(self, inlink, laneno, oneway):
        businfo = ''
        sqlcmd = '''
            SELECT distinct vt
            FROM org_lf
            where id = %s;
        '''
        self.pg.execute2(sqlcmd, (inlink,))
        rows = self.pg.fetchall2()
        length = len(rows)
        if length == 1 and rows[0][0] == 17:
            bus_sqlcmd = '''
                SELECT distinct dflane, vt, validity
                FROM org_lf
                where id = %s and vt = 17 and substr(validity, %s, 1) = '1';
            '''
            self.pg.execute2(bus_sqlcmd, (inlink, laneno + 1))
            lists = self.pg.fetchall2()
            length = len(lists)
            if length != 1:
                return businfo
            else:
                validity = lists[0][2]
                va_length = len(validity)
                if oneway:
                    lannum = va_length - 1
                    for i in range(lannum):
                        if i == (laneno - 1):
                            businfo = businfo + '1'
                        else:
                            businfo = businfo + '0'
                else:
                    lannum = validity.count('1')
                    if validity[1] == '1':
                        for i in range(lannum):
                            if i == (laneno - 1):
                                businfo = businfo + '1'
                            else:
                                businfo = businfo + '0'
                    else:
                        i = lannum
                        start = va_length - 1 - i
                        while start < va_length - 1:
                            if start == (laneno - 1):
                                businfo = businfo + '1'
                            else:
                                businfo = businfo + '0'
                if lists[0][0] == DLANE_DIR_N:
                        businfo = businfo[::-1]
        else:
            return businfo

    def _make_lane_tbl(self):
        '''打开日志文件'''
        file_path = os.path.join('..', 'docs', 'lane_log.txt')
        f = open(file_path, "w")
        insert_sql = '''
            INSERT INTO lane_tbl(
                id, nodeid, inlinkid, outlinkid,
                passlid, passlink_cnt,
                lanenum, laneinfo, arrowinfo,
                lanenuml, lanenumr, buslaneinfo
                )
            VALUES (%s, %s, %s, %s,
                    %s, %s,
                    %s, %s, %s,
                    %s, %s, %s);
        '''
        sqlcmd = '''
            SELECT distinct inlink, outlink, passlink, nodeid, laneno, oneway
            FROM temp_lane_link_node as a
            left join org_nw as b
            on a.inlink = b.id;
        '''
        rows = self.get_batch_data(sqlcmd)
        i = 1
        for row in rows:
            inlink = row[0]
            outlink = row[1]
            passlink = row[2]
            passlinkcnt = 0
            if passlink:
                passlinkcnt = len(passlink.split('|'))
            nodeid = row[3]
            laneno = row[4]
            oneway = row[5]
            direction = self._make_arrow_direction(inlink, laneno, f)
            if direction:
                laneinfo = self._make_lane_info(inlink, laneno, oneway)
                businfo = ''
                lane_len = len(laneinfo)
                if laneinfo != '':
                    businfo = '0' * lane_len
                    self.pg.execute2(insert_sql,
                                     (i, nodeid, inlink, outlink,
                                      passlink, passlinkcnt,
                                      lane_len, laneinfo, direction,
                                      0, 0, businfo
                                                  ))
                else:
                    self.log.warning(
                                'direction is not null but laneinfo is null'
                                     )
                    continue
            i = i + 1
        f.close()
        self.pg.commit2()
        return 0
