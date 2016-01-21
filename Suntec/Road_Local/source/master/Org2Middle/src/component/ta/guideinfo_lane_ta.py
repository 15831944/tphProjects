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
        if self.CreateTable2('temp_lane_tbl') == -1:
            return -1
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
        self.CreateFunction2('temp_lane_info_merge')
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
        
        self._merge_same_arrow()
        
        return 0
    
    def _merge_same_arrow(self):
        sqlcmd = '''
            insert into lane_tbl(
                id, nodeid, inlinkid, outlinkid,
                passlid, passlink_cnt,
                lanenum, laneinfo, arrowinfo,
                lanenuml, lanenumr, buslaneinfo
            )
            (
                select min_id, nodeid, inlinkid, outlinkid, passlid, passlink_cnt, 
           lanenum, temp_lane_info_merge(laneinfo_list, lanenum), arrowinfo, lanenuml, lanenumr, buslaneinfo
                from
                (
                    select min(id) as min_id, nodeid, inlinkid, outlinkid, passlid, passlink_cnt, 
                           lanenum, array_agg(laneinfo) as laneinfo_list, arrowinfo, lanenuml, lanenumr, buslaneinfo
                    from temp_lane_tbl
                    group by nodeid, inlinkid, outlinkid, passlid, passlink_cnt, 
                           lanenum, arrowinfo, lanenuml, lanenumr, buslaneinfo
                ) as a
            )
        '''
        
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
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
                            SELECT a.gid, a.id, a.seqnr, a.trpelid, a.trpeltyp, case when b.oneway is null then '' else b.oneway end
                            FROM org_lp as a
                            left join org_nw as b
                            on a.trpelid = b.id
                            order by id,seqnr
                      ) as a
                    group by id
            ) as b
            --where length > 1 and not ('N' = any(oneway_array))
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
    
    def __set_laneinfo_with_specify_pos(self, lane_info_str, pos, setting_char):
        if pos == 0:
            lane_info_str = setting_char + lane_info_str[1:] # setting_char optional value :'1', '0'
        else:
            lane_info_str = lane_info_str[0:pos-1] + setting_char + lane_info_str[pos:]
            
        return lane_info_str
    
    def _set_all_car_condition_laneinfo(self, laneinfo, linkdir, dflane, validity_array, laneno, lane_cnt):
    #    laneinfo = '0' * lane_cnt # initial laneinfo
        rtn_laneinfo = laneinfo
        
        if laneno > len(validity_array):
            print "there is error"
        
        if validity_array[laneno-1] == '0': # this lane is not controlled.
            return rtn_laneinfo
        
        if linkdir == 'FT' and dflane == DLANE_DIR_N:
            rtn_laneinfo = self.__set_laneinfo_with_specify_pos(laneinfo, laneno, '1')
        elif linkdir == 'TF' and dflane == DLANE_DIR_P:
            rtn_laneinfo = self.__set_laneinfo_with_specify_pos(laneinfo, laneno, '1')
        else:
            pass
        
        return rtn_laneinfo
    
    def _cancel_passenger_car_condition_laneinfo(self, laneinfo, linkdir, dflane, validity_array, laneno, lane_cnt):
        rtn_laneinfo = laneinfo
        
        if validity_array[laneno-1] == '0': # this lane is not controlled.
            return rtn_laneinfo
        
        if linkdir == 'FT' and dflane == DLANE_DIR_P:
            rtn_laneinfo = self.__set_laneinfo_with_specify_pos(laneinfo, laneno, '0')
        elif linkdir == 'TF' and dflane == DLANE_DIR_N:
            rtn_laneinfo = self.__set_laneinfo_with_specify_pos(laneinfo, laneno, '0')
        else:
            pass
        
        return rtn_laneinfo
    
    def __get_access_lane_no(self, validity_array, check_value):
        validate_lane_no_list = list()
        
        cnt = len(validity_array)
        for i in range(0, cnt):
            if validity_array[i] == check_value:
                validate_lane_no_list.append(i)
        
        return validate_lane_no_list      
    
    def __get_current_drive_dir_lane(self, bidirection_validate_dir, dflane, validity_array):
        # for this usage scenarios, we suppose that lanes could be run
        # in same direction must be continue one by one, and don't permit
        # to separate
        lane_cnt = 0
        rtn_validate_access_lane_list = None
            
        if bidirection_validate_dir == 'FT' and (dflane == 1 or dflane == 3): # postive could be run
            lane_cnt = validity_array.count('1')
            rtn_validate_access_lane_list = self.__get_access_lane_no(validity_array, '1')
        elif bidirection_validate_dir == 'FT' and dflane == 2:
            lane_cnt = validity_array.count('0')
            rtn_validate_access_lane_list = self.__get_access_lane_no(validity_array, '0')
        elif bidirection_validate_dir == 'TF' and (dflane == 1 or dflane == 2): # negative could be run
            lane_cnt = validity_array.count('1')
            rtn_validate_access_lane_list = self.__get_access_lane_no(validity_array, '1')
        elif bidirection_validate_dir == 'TF' and dflane == 3:
            lane_cnt = validity_array.count('0')
            rtn_validate_access_lane_list = self.__get_access_lane_no(validity_array, '0')
        else:
            lane_cnt = 0
            rtn_validate_access_lane_list = None
            
        return lane_cnt, rtn_validate_access_lane_list
    
    
    def __get_all_car_pos(self, dflane_array, validity_array, vt_array, bidirection_validate_dir):
        index = 0
        
        cnt = len(dflane_array)
        for i in range(0, cnt):
            if ((bidirection_validate_dir == 'FT' and dflane_array[i] == 3) or
                (bidirection_validate_dir == 'TF' and dflane_array[i] == 2)) and (vt_array[i] == 0):
                return i
            
        return -1
        
    def _Get_lane_info(self, laneno, oneway, dflane_array, vt_array, validity_array, nw_lanes,inlink, bidirection_validate_dir):        
        # 特定车线上的方向
        laneinfo = ''
        mid_laneinfo = None
        
        if oneway == 'FT' or oneway == 'TF': # single direction
            if len(dflane_array) == 1 and dflane_array[0] == 'NULL':
                self.log.error('laneno:%s lane of inlink=%s has no dflane_array and validity_array info!!!!',
                                 laneno, inlink)
            else:
                # judgment all car condition
                if vt_array.count(0) <> 0:
                    all_car_pos = vt_array.index(0) 
                    if all_car_pos <> -1:
                        lane_cnt = len(validity_array[all_car_pos])
                        laneinfo = '0' * lane_cnt # initial laneinfo
                        
                        mid_laneinfo = self._set_all_car_condition_laneinfo(laneinfo, oneway, dflane_array[all_car_pos], 
                                                                            validity_array[all_car_pos], laneno, lane_cnt)
                
                # judgment passenger car condition
                if vt_array.count(11) <> 0:
                    passenger_car_pos = vt_array.index(11)  
                    if passenger_car_pos <> -1:
                        lane_cnt = len(validity_array[passenger_car_pos])
                        
                        final_laneinfo = self._cancel_passenger_car_condition_laneinfo(mid_laneinfo, oneway, dflane_array[passenger_car_pos], 
                                                        validity_array[passenger_car_pos], laneno, lane_cnt)
                else :
                    final_laneinfo = mid_laneinfo
                    
                if oneway == 'FT':
                    final_laneinfo = final_laneinfo[::-1]
                    
        else: # two direction 
            if len(dflane_array) == 1 and dflane_array[0] == None:
                self.log.error('laneno:%s lane of inlink=%s has no dflane_array and validity_array info!!!!',
                                 laneno, inlink)
            else:
                # step one: judgment current drive direction
                if vt_array.count(0) <> 0:
#                    all_car_pos = vt_array.index(0) 
                    all_car_pos = self.__get_all_car_pos(dflane_array, validity_array, vt_array, bidirection_validate_dir)
                    if all_car_pos ==-1:
                        pass
                    
                    if all_car_pos <> -1:
                        lane_cnt = len(validity_array[all_car_pos])
                        laneinfo = '0' * lane_cnt # initial laneinfo
                        
                        mid_laneinfo = self._set_all_car_condition_laneinfo(laneinfo, bidirection_validate_dir, dflane_array[all_car_pos], 
                                                                            validity_array[all_car_pos], laneno, lane_cnt)
                    
                        rtn_validate_lane_cnt, rtn_validate_access_lane_list = self.__get_current_drive_dir_lane(bidirection_validate_dir, 
                                                                                                    dflane_array[all_car_pos], validity_array[all_car_pos])  
                        
                        if rtn_validate_lane_cnt <> len(rtn_validate_access_lane_list):
                            self.log.error('inlink=%s in %s direction has separate access lane info!!!!',inlink, bidirection_validate_dir)
                    
                # judgment passenger car condition
                if vt_array.count(11) <> 0:
                    passenger_car_pos = vt_array.index(11)  
                    if passenger_car_pos <> -1:
                        lane_cnt = len(validity_array[passenger_car_pos])
                        
                        final_laneinfo = self._cancel_passenger_car_condition_laneinfo(mid_laneinfo, bidirection_validate_dir, dflane_array[passenger_car_pos], 
                                                        validity_array[passenger_car_pos], laneno, lane_cnt)
                else:
                    final_laneinfo = mid_laneinfo
                
                # cut laneinfo in car driving direction
                final_laneinfo = final_laneinfo[rtn_validate_access_lane_list[0]:
                                                rtn_validate_access_lane_list[rtn_validate_lane_cnt-1]+1]
                 
                if bidirection_validate_dir == 'FT':
                    final_laneinfo = final_laneinfo[::-1]
                    
        return final_laneinfo
    
    def _make_lane_tbl(self):
        '''打开日志文件'''
        file_path = os.path.join('..', 'docs', 'lane_log.txt')
        f = open(file_path, "w")
        insert_sql = '''
            INSERT INTO temp_lane_tbl(
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
            array_agg(d.vt) vt_array, array_agg(substring(d.validity, 2)) as validity_array, b.lanes,b.f_jnctid, b.t_jnctid
            FROM temp_lane_link_node as a
            left join org_nw as b
            on a.inlink = b.id
            left join org_ld as c
            on a.inlink = c.id and substr(c.validity, a.laneno::integer + 1, 1) = '1'
            left join org_lf as d
            on a.inlink = d.id and d.vt in (0, 11)
            where c.id is not null
            group by inlink, outlink, passlink, nodeid, laneno, oneway, c.direction, b.lanes,b.f_jnctid, b.t_jnctid
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
            start_nodeid = row[11]
            end_nodeid = row[12]
            
            if oneway == 'N' :
                continue
            
            if oneway == None: #bidirection, so we should get validate lane cnt by drive way
                if nodeid == start_nodeid:
                    bidirection_validate_dir = 'TF'
                else:
                    bidirection_validate_dir = 'FT'
            else:
                bidirection_validate_dir = oneway
                
            if len(dflane_array) == 1 and dflane_array[0] == None:
                continue
                
#            laneinfo = self._make_lane_info(inlink, laneno, oneway)
            laneinfo = self._Get_lane_info(laneno, oneway, dflane_array, vt_array, 
                                           validity_array, nw_lanes, inlink, bidirection_validate_dir)
            
            # function without bus lane 
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