# -*- coding: UTF8 -*-
'''
Created on 2012-9-4

@author: zym
'''

import os
import component.component_base

DLANE_DIR_P = 2
DLANE_DIR_N = 3
DLANE_DIR_B = 1
ALL_CAR = 0
PASSENGER_CAR = 11


class comp_guideinfo_lane_ta(component.component_base.comp_base):
    '''
    This class deals with guide lane information
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Guideinfo_Lane')

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
#        if self.CreateIndex2('temp_lane_link_info_id_idx') == -1:
#            return -1
#        if self.CreateIndex2('org_ln_id_idx') == -1:
#            return -1
#        if self.CreateIndex2('org_ld_id_validity_idx') == -1:
#            return -1
#        if self.CreateIndex2('org_lf_id_validity_idx') == -1:
#            return -1
        return 0

    def _DoCreateFunction(self):
        return 0

    def _Do(self):
        self.CreateIndex2('org_ld_id_validity_idx')
        self.CreateIndex2('org_lf_id_validity_idx')
        self.CreateIndex2('org_lp_id_idx')
        self.CreateIndex2('org_ln_id_idx')
        self.CreateIndex2('org_ld_id_idx')
        self.CreateIndex2('org_lf_id_idx')
        self.CreateIndex2('nw_id_idx') 
        
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
                            count(*) as length, array_agg(oneway) as oneway_array
                    from(
                            SELECT a.gid, a.id, a.seqnr, a.trpelid, a.trpeltyp, b.oneway
                            FROM org_lp as a
                            left join org_nw as b
                            on a.trpelid = b.id
                            order by id,seqnr
                      ) as a
                    group by id
            ) as b
            where length > 1 and not ('N' = any(oneway_array))
        );
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.CreateIndex2('temp_lane_link_info_id_idx')
        return 0

    # 求得node\laneno
    def _make_node_laneno(self):
        
        sqlcmd = '''
            insert into temp_lane_link_node(
                id, inlink, outlink, passlink, nodeid, laneno
            )
            (
                select id, inlink, outlink, passlink, jnctid, laneno
                from
                (
                    SELECT a.id, inlink, outlink, passlink, jnctid,
                       substr(fromto, 1, position('/' in fromto)-1)::integer laneno
                    FROM temp_lane_link_info as a
                    left join org_ln as b
                    on a.id = b.id
                    where b.id is not null
                )temp
                group by id, inlink, outlink, passlink, jnctid, laneno
            );
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.CreateIndex2('temp_lane_link_node_id_idx')
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
        
    def _get_dflane(self, oneway, dflane_array):
        
        if not dflane_array:
            return dflane_array
             
        num = len(dflane_array)
        dflane_array_temp = list()
        
        if oneway == 'FT':
            for num_idx in range(num):
                if dflane_array[num_idx] == 3:
                    dflane_array_temp.append(dflane_array[num_idx])
                    
        elif oneway == 'TF':
            for num_idx in range(num):
                if dflane_array[num_idx] == 2:
                    dflane_array_temp.append(dflane_array[num_idx])
        else:
            dflane_array_temp = dflane_array

        return dflane_array_temp
    
    def _get_lane_one(self, laneno, validity, oneway):
        
        laneinfo = ''
        len_num = len(validity) - 1
        laneinfo = '0' * len_num
        if laneno == 1:
            laneinfo = '1' + laneinfo[1:len_num]
        else:          
            laneinfo = laneinfo[0:laneno - 1] + '1' + laneinfo[laneno:len_num]
        
        laneinfo = laneinfo[0:]
        
        if oneway == 'FT':
            laneinfo = laneinfo[::-1]
        elif oneway == 'TF':
            laneinfo = laneinfo
        else:
            return ''
        
        return laneinfo
    
    def _get_both_laneinfo(self, laneno, dflane, validity):
        
        laneinfo = ''
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

        return laneinfo
        
    def _Get_lane_info(self, laneno, oneway, dflane_array, vt_array, validity_array, nw_lanes):        
        # 特定车线上的方向
        laneinfo = ''
        
        dflane_array = self._get_dflane(oneway, dflane_array)
        # 车线同向---正向 or 反向
        if oneway == 'FT' or oneway == 'TF':
            if not dflane_array:
                return laneinfo
            
            if vt_array.count(0) == 1:
                laneinfo = self._get_lane_one(laneno, validity_array[vt_array.index(0)], oneway)
            elif vt_array.count(0) == 0 and vt_array.count(11) == 1:
                laneinfo = self._get_lane_one(laneno, validity_array[vt_array.index(11)], oneway)
        # 双向
        else:
            if not dflane_array:
                if nw_lanes == 1:
                    return '1'
                
            if vt_array.count(0) == 1:
                laneinfo = self._get_both_laneinfo(laneno, dflane_array[vt_array.index(0)], validity_array[vt_array.index(0)], oneway)
            elif vt_array.count(0) == 0 and vt_array.count(11) == 1:
                laneinfo = self._get_both_laneinfo(laneno, dflane_array[vt_array.index(11)], validity_array[vt_array.index(11)], oneway)  

        return laneinfo 
    
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
            SELECT inlink, outlink, passlink, nodeid, laneno, oneway, case when c.direction=256 then 2048 else c.direction end,
                    array_agg(d.dflane) dflane_array, 
                array_agg(d.vt) vt_array, array_agg(d.validity) as validity_array, b.lanes
            FROM temp_lane_link_node as a
            left join org_nw as b
            on a.inlink = b.id
            left join org_ld as c
            on a.inlink = c.id and substr(c.validity, a.laneno::integer + 1, 1) = '1'
            left join org_lf as d
            on a.inlink = d.id and d.vt in (0, 11)
            where c.id is not null
            group by inlink, outlink, passlink, nodeid, laneno, oneway, c.direction, b.lanes
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
            direction = row[6]
            dflane_array = row[7]
            vt_array = row[8]
            validity_array = row[9]
            nw_lanes = row[10]
            
            if oneway == 'N' :
                continue

#            laneinfo = self._make_lane_info(inlink, laneno, oneway)
            laneinfo = self._Get_lane_info(laneno, oneway, dflane_array, vt_array, validity_array, nw_lanes)
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

            i = i + 1
        f.close()
        self.pg.commit2()
        return 0