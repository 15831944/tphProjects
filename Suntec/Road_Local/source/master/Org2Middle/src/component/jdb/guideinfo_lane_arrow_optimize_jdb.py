# -*- coding: cp936 -*-
'''
Created on 2012-3-21

@author: zym
'''

import component.component_base

ROUNDABOUT_FLAG = 0
URBAN_HIGHWAY_FLAG = 1
RAMP_LINK_FLAG = 5
MAIN_LINK1_FLAG = 1
MAIN_LINK2_FLAG = 2
SIDE_TRACK_LINK_FLAG = 6
SAPA_LINK_FLAG = 7
CONNECTION_LINK_FLAG =3
WALKWAY_LINK_FLAG = 8
NARROW_ROAD2_FLAG = 9
PREFECTURAL_LINK_FLAG = 4
LEFT_TURN_FLAG = 8
RIGHT_TURN_FLAG = 9
ONEWAY_F = 2
ONEWAY_R = 3
ONEWAY_B = 1
INNERLINK_MAX_LEN = 1000
ARROWDIC = {"straight" : 1,"left_straight" : 1,"right_back" : 8,"left_back":32,"right_forward":2,"left_forward":128,"right":4,"left":64}

# 此类用于更新lane_tbl表中箭头信息为空的记录。
class comp_guideinfo_lane_arrow_optimize_jdb(component.component_base.comp_base):
    '''诱导车线箭头优化(日本版)
    '''

    def __init__(self):
        '''
        Constructor
        '''

        component.component_base.comp_base.__init__(self, "arrow_optimize")

    def getLaneWithEmptyArrow(self):
        '''从数据库中选取箭头方向为空记录'''
        sql_cmd = '''
            SELECT gid, nodeid, inlinkid, outlinkid, passlid, passlink_cnt
            FROM lane_tbl
            where arrowinfo is null;
        '''
        self.pg.execute2(sql_cmd)
        org_data_list = self.pg.fetchall2()
        return org_data_list

    def _Do(self):
        self.CreateTable2("test_lane_tbl")
        org_data_list = self.getLaneWithEmptyArrow()
        metadata_list = self._get_metadata_list(org_data_list)
        for metadata_map in metadata_list:
            for (gid, metadata) in metadata_map.items():
                basic_arrow = self._get_basic_angle(metadata)
                forceguide_res = self.getTrafficAngleFromForceguide(metadata, basic_arrow)
                #如果有强制诱导但是没有方向
                if forceguide_res[0]:
                    if forceguide_res[1] <> "disangle":
                        basic_arrow = forceguide_res[1]
                    else:
                        basic_arrow = forceguide_res[2]
                        # roundabout 入口
                        result = self._roundabout_enter_filter(metadata, basic_arrow)
                        if result[0]:
                            basic_arrow = result[1]
                        else:
                            # roundabout 出口
                            result = self._roundabout_exit_filter(metadata, basic_arrow)
                            if result[0]:
                                
                                basic_arrow = result[1]
                            else:
                                # 高速入口
                                result = self._highway_enter_filter(metadata, basic_arrow)
                                if result[0]:
                                    basic_arrow = result[1]
                                else:
                                    # 高速出口
                                    result = self._highway_exit_filter(metadata, basic_arrow)
                                    if result[0]:
                                        basic_arrow = result[1]
                                    else:
                                        result[0] = False
#                                         result = self._jct_branch_filter(metadata, basic_arrow)
                                        if result[0]:
                                            basic_arrow = result[1]
                                        else:
                                            result = self._highway_branch_filter(metadata, basic_arrow)
                                            if result[0]:
                                                basic_arrow = result[1]
                else:
                    # roundabout 入口
                    result = self._roundabout_enter_filter(metadata, basic_arrow)
                    if result[0]:
                        
                        basic_arrow = result[1]
                    else:
                        # roundabout 出口
                        result = self._roundabout_exit_filter(metadata, basic_arrow)
                        if result[0]:
                            
                            basic_arrow = result[1]
                        else:
                            # 高速入口
                            result = self._highway_enter_filter(metadata, basic_arrow)
                            if result[0]:
                                basic_arrow = result[1]
                            else:
                                # 高速出口
                                result = self._highway_exit_filter(metadata, basic_arrow)
                                if result[0]:
                                    basic_arrow = result[1]
                                else:
                                    result = self._jct_branch_filter(metadata, basic_arrow)
                                    if result[0]:
                                        basic_arrow = result[1]
                                    else:
                                        result = self._highway_branch_filter(metadata, basic_arrow)
                                        if result[0]:
                                            basic_arrow = result[1]
                    if not result[0]:
                        #小于30度 并且是alonglink相关
                        result2 = self._along_anotherlink_filter(metadata, basic_arrow)
                        if result2[0]:
                            basic_arrow = result2[1]
                        else:
                            result2 = self._straight_another_link_filter(metadata, basic_arrow)
                            if result2[0]:
                                basic_arrow = result2[1]
                links = metadata.get_links()
                self.pg.execute2('''
                INSERT INTO test_lane_tbl(
                gid, nodeid, inlinkid, outlinkid, arrowinfo)
                VALUES (%s, %s, %s, %s, %s);
                ''', (gid,0,links[0],links[1],ARROWDIC.get(basic_arrow)))
#                 print links[0],links[1],basic_arrow
#                 self.pg.execute2('''
#                     UPDATE lane_tbl
#                     SET  arrowinfo=%s
#                     WHERE gid=%s;
#                 ''', (ARROWDIC.get(basic_arrow),gid))

        self.pg.commit2()
        #update
        self.pg.execute2('''SELECT gid,arrowinfo
                          FROM test_lane_tbl''')
        records = self.pg.fetchall2()
        for rec in records:
            self.pg.execute2('''
                UPDATE lane_tbl
                SET arrowinfo=%s
                WHERE gid=%s;
            ''',(rec[1],rec[0]))
        self.pg.commit2()
        return
    
    # 从forceguide获取转向角度。
    def getTrafficAngleFromForceguide(self,metadata,basic_arrow):
        links = metadata.get_links()
        inlink = links[0]
        outlink = links[1]
        sqlcmd = '''
            SELECT guide_type
            FROM force_guide_tbl
            where inlinkid = %s and outlinkid = %s; 
        '''
        self.pg.execute2(sqlcmd, (inlink,outlink))
        row = self.pg.fetchone2()
        if not row:
            return [False, basic_arrow]
        if len(row) == 1:
            guide_type = row[0]
            if guide_type == 1:
                if self.isHighWay(inlink):
                    basic_arrow = "straight"
                    return [True, basic_arrow]
                else:
                    return [True, "disangle", basic_arrow]
            if guide_type == 2:
                basic_arrow = "straight"
            elif guide_type == 3:
                basic_arrow = "right_forward"
            elif guide_type in (4,5,6):
                basic_arrow = "right"
            elif guide_type == 7:
                basic_arrow = "right_back"
            elif guide_type == 8:
                basic_arrow = "right_uturn"
            elif guide_type == 9:
                basic_arrow = "left_back"
            elif guide_type in (10,11,12):
                basic_arrow = "left"
            elif guide_type == 13:
                basic_arrow = "left_forward"
            else:
                pass
            return [True, basic_arrow]
        else:
            return [False, basic_arrow]
    #jct 分歧
    def _jct_branch_filter(self,metadata,basic_arrow):
        angles = metadata.get_angles()
        links = metadata.get_links()
        linknum = metadata.get_linknum()
        linkdirs = metadata.get_linkdirs()
        onewaycodes = metadata.get_onewaycodes()
        same_name_link = metadata.get_same_name_link()
        jumpnum = metadata.get_jumpnum()
        rel_p = basic_arrow
        rel_p2 = basic_arrow
        if linknum <= 2:
            return [False, basic_arrow]
        if not self.isHighWay(links[0]) and not self.isHighWay(links[1]):
            return [False, basic_arrow]
        linkcnt = 0
        while linkcnt < linknum:
            if linkcnt in (0,1):
                linkcnt = linkcnt + 1
                continue
            abs_branch_angle = abs(angles[linkcnt])
            if abs_branch_angle <= 75:
                break
            linkcnt = linkcnt + 1
        if linkcnt == linknum:
            return [False, basic_arrow]
        if self.get_linktype(links[0]) in (MAIN_LINK1_FLAG,MAIN_LINK2_FLAG):
            if self.get_linktype(links[1]) in (CONNECTION_LINK_FLAG,RAMP_LINK_FLAG):
                linkcnt = 0
                relcnt = 0
                while linkcnt < linknum:
                    if linkcnt <> 1:
                        #it is not ramp or sapa link
                        if self.get_linktype(links[linkcnt]) in (RAMP_LINK_FLAG,SAPA_LINK_FLAG):
                            return [False, basic_arrow]
                        if linkcnt <> 0:
                            abs_branch_angle = abs(angles[linkcnt])
                            if abs_branch_angle < 75:
                                if self._get_link_rel_position(angles[1], angles[linkcnt]):
                                    rel_p = "right"
                                else:
                                    rel_p = "left"
                                relcnt = relcnt + 1
                                if relcnt > 1:
                                    if rel_p <> rel_p2:
                                        basic_arrow = "straight"
                                        return [True, basic_arrow]
                                rel_p2 = rel_p
                    linkcnt = linkcnt + 1
                if rel_p.find("left") >= 0:
                    basic_arrow = "left"
                else:
                    basic_arrow = "right"
                return [True, basic_arrow]
            elif self.get_roadtype(links[0]) == URBAN_HIGHWAY_FLAG:
                linkcnt = 0
                relcnt = 0
                while linkcnt < linknum:
                    if linkcnt <> 1:
                        if self.get_linktype(links[linkcnt]) == CONNECTION_LINK_FLAG:
                            if linkcnt <> 0:
                                abs_branch_angle = abs(angles[linkcnt])
                                if abs_branch_angle < 75:
                                    if self._get_link_rel_position(angles[1], angles[linkcnt]):
                                        rel_p = "right"
                                    else:
                                        rel_p = "left"
                                    relcnt = relcnt + 1
                                    if relcnt > 1:
                                        if rel_p <> rel_p2:
                                            basic_arrow = "straight"
                                            return [True, basic_arrow]
                                    rel_p2 = rel_p
                        elif linkcnt <> 0:
                            return [False, basic_arrow]
                        else:
                            pass
                    linkcnt = linkcnt + 1
                if rel_p.find("left"):
                    basic_arrow = "left"
                else:
                    basic_arrow = "right"
            else:
                pass
        return [False, basic_arrow]
    def _straight_another_link_filter(self,metadata,basic_arrow):
        angles = metadata.get_angles()
        links = metadata.get_links()
        linknum = metadata.get_linknum()
        linkdirs = metadata.get_linkdirs()
        onewaycodes = metadata.get_onewaycodes()
        same_name_link = metadata.get_same_name_link()
        jumpnum = metadata.get_jumpnum()
        if abs(angles[1]) < 15:
            linkcnt = 0
            while linkcnt < linknum:
                if linkcnt in (0,1):
                    linkcnt = linkcnt + 1
                    continue
                if self.get_linktype(links[0]) == MAIN_LINK2_FLAG \
                    and self.get_linktype(links[1]) == MAIN_LINK2_FLAG:
                    if self.get_linktype(links[linkcnt]) <> MAIN_LINK2_FLAG:
                        linkcnt = linkcnt + 1
                        continue
                if self.get_linktype(links[0]) == MAIN_LINK1_FLAG \
                    and self.get_linktype(links[1]) == MAIN_LINK1_FLAG:
                    if self.get_linktype(links[linkcnt]) <> MAIN_LINK1_FLAG:
                        linkcnt = linkcnt + 1
                        continue
                oneway_flag = self._check_link_triffic(onewaycodes[linkcnt], linkdirs[linkcnt])
                if not oneway_flag:
                    linkcnt = linkcnt + 1
                    continue
                abs_branch_angle = abs(angles[linkcnt])
                if abs_branch_angle < 15 \
                    and self.get_roadtype(links[1]) >= self.get_roadtype(links[linkcnt]):
                    if self.get_roadtype(links[1]) > self.get_roadtype(links[linkcnt]) \
                        or self.get_link_width(links[1],linkdirs[1]) <= self.get_link_width(links[linkcnt],linkdirs[linkcnt]):
                        temp_angle = angles[1] - angles[linkcnt]
                        if (angles[1] >= 0 and angles[linkcnt] >= 0) or (angles[1] < 0 and angles[linkcnt] < 0):
                            if temp_angle < 0:
                                basic_arrow = "left_forward"
                            else:
                                basic_arrow = "right_forward"
                        else:
                            if basic_arrow.find("left") >= 0:
                                basic_arrow = "left_forward"
                            else:
                                basic_arrow = "right_forward"
                        return [True, basic_arrow]
                linkcnt = linkcnt + 1
        return [False, basic_arrow]
    
    def get_link_width(self, linkid, dir_f):
        sql_cmd = '''
            SELECT width_s2e,width_e2s
            FROM link_tbl
            where link_id = %s;
        '''
        self.pg.execute2(sql_cmd, (linkid,))
        row = self.pg.fetchone2()
        if dir_f:
            return row[0]
        else:
            return row[1]
    
    def _along_anotherlink_filter(self,metadata,basic_arrow):
        angles = metadata.get_angles()
        alonglink = metadata.get_alonglink()
        if abs(angles[1]) < 30:
            if alonglink <> -1 and alonglink <> 1:
                if self._get_link_rel_position(angles[1], angles[alonglink]):
                    basic_arrow = "right_forward"
                else:
                    basic_arrow = "left_forward"
                return [True, basic_arrow]
        return [False, basic_arrow]
    
    def _make_alonglink_data(self, metadata):
        angles = metadata.get_angles()
        links = metadata.get_links()
        linknum = metadata.get_linknum()
        linkdirs = metadata.get_linkdirs()
        onewaycodes = metadata.get_onewaycodes()
        same_name_link = metadata.get_same_name_link()
        jumpnum = metadata.get_jumpnum()
        cmp_angle = 15
        if self.get_roadtype(links[0]) >= WALKWAY_LINK_FLAG \
            and self.get_roadtype(links[0]) >= WALKWAY_LINK_FLAG:
            metadata.set_alonglink(-1)
            return
        if linknum == 2:
            metadata.set_alonglink(1)
            return
        if self.check_if_narrow_all_branch_link(metadata):
            return
        alonglink = -1
        find_flag = False
        angle_along = 0
        along_rev_angle = 0
        linkcnt = 0
        while True:
            while linkcnt < linknum:
                if linkcnt == 0:
                    linkcnt = linkcnt + 1
                    continue
                abs_branch_angle = abs(angles[linkcnt])
                if abs_branch_angle > cmp_angle:
                    linkcnt = linkcnt + 1
                    continue
                if abs_branch_angle > 30 and not self._check_link_triffic(onewaycodes[linkcnt], linkdirs[linkcnt]):
                    linkcnt = linkcnt + 1
                    continue
                
                outlink_angle = abs(angles[1])
                if outlink_angle < 30 and not self._check_link_triffic(onewaycodes[linkcnt], linkdirs[linkcnt]):
                    linkcnt = linkcnt + 1
                    continue
                if self.get_roadtype(links[0]) <> self.get_roadtype(links[linkcnt]):
                    linkcnt = linkcnt + 1
                    continue
                if self.get_dispclass(links[0]) <> self.get_dispclass(links[linkcnt]):
                    linkcnt = linkcnt + 1
                    continue
                if alonglink == -1:
                    find_flag = True
                    angle_along = abs_branch_angle
                    alonglink = linkcnt
                else:
                    alonglink = -1
                    break
            if find_flag:
                break
            if cmp_angle > 75:
                break
            cmp_angle = cmp_angle + 15
        if alonglink <> -1:
            linkcnt = 0
            while linkcnt < linknum:
                exist_other_link = False
                if linkcnt == 0 or linkcnt == alonglink:
                    linkcnt = linkcnt + 1
                    continue
                if jumpnum == 0:
                    if self._check_link_triffic(onewaycodes[linkcnt], linkdirs[linkcnt]):
                        linkcnt = linkcnt + 1
                        continue
                abs_branch_angle = abs(angles[linkcnt])
                if self.get_roadtype(links[linkcnt]) <= PREFECTURAL_LINK_FLAG:
                    exist_other_link = True
                if exist_other_link:
                    if angle_along < 60:
                        if abs_branch_angle < angle_along + 15:
                            alonglink = -1
                            break
                        if angle_along >= 35:
                            along_rev_angle = 180 - abs(angles[linkcnt]) + 15
                            if angles[alonglink] > 0:
                                along_rev_angle = along_rev_angle * (-1)
                                if along_rev_angle < angles[linkcnt] and angles[linkcnt] < 0:
                                    alonglink = -1
                                    break
                            else:
                                if angles[linkcnt] > 0 and angles[linkcnt] < along_rev_angle:
                                    alonglink = - 1
                    else:
                        alonglink = -1
                        break
                else:
                    if abs_branch_angle < 11.25 and angle_along > 60 and not same_name_link[alonglink]:
                        alonglink = - 1
                        break
                if same_name_link[linkcnt]:
                    alonglink = - 1
                    break
                linkcnt = linkcnt + 1
        if self.get_linktype(links[0]) == ROUNDABOUT_FLAG:
            linkcnt = 0
            while linkcnt < linknum:
                if linkcnt == 0:
                    linkcnt = linkcnt + 1
                    continue
                if self.get_linktype(links[linkcnt]) == ROUNDABOUT_FLAG:
                    alonglink = linkcnt
                    break
                linkcnt = linkcnt + 1
        
        if self.get_linktype(links[0]) in (LEFT_TURN_FLAG,RAMP_LINK_FLAG):
            if self.get_linktype(links[1]) in (MAIN_LINK1_FLAG,MAIN_LINK2_FLAG):
                if linknum == 3:
                    ibranchlink = -1
                    i = 0
                    while i < linknum:
                        if i <> 0 and i <> 1:
                            ibranchlink = i
                            break
                        i = i + 1
                    if ibranchlink <> -1:
                        if (angles[1] > 0 and angles[ibranchlink] < 0) or (angles[1] < 0 and angles[ibranchlink] > 0):
                            ipathlink_angle = abs(angles[1])
                            ibranchlink_angle = abs(angles[ibranchlink])
                            if ipathlink_angle < 60 and ibranchlink_angle > 90:
                                if alonglink == -1:
                                    alonglink = 1
        metadata.set_alonglink(alonglink)
        return
    def get_dispclass(self, linkid):
        sql_cmd = '''
            SELECT display_class
            FROM link_tbl
            where link_id = %s;
        '''
        self.pg.execute2(sql_cmd, (linkid,))
        row = self.pg.fetchone2()
        return row[0]
    def check_if_narrow_all_branch_link(self, metadata):
        angles = metadata.get_angles()
        links = metadata.get_links()
        linknum = metadata.get_linknum()
        linkcnt = 0
        if self.get_roadtype(links[0]) <> NARROW_ROAD2_FLAG and self.get_roadtype(links[1]) <> NARROW_ROAD2_FLAG:
            while linkcnt < linknum:
                if linkcnt in (0,1):
                    linkcnt = linkcnt + 1
                    continue
                if self.get_roadtype(links[linkcnt]) <> NARROW_ROAD2_FLAG:
                    break
                linkcnt = linkcnt + 1
            if linkcnt == linknum:
                metadata.set_alonglink(1)
                return True
        return False
    def _get_metadata_list(self, org_data_list):
        metadata_list = [] 
        for orgdata in org_data_list:
            gid = orgdata[0]
            nodeid = orgdata[1]
            inlinkid = orgdata[2]
            outlinkid = orgdata[3]
            passlid = orgdata[4]
            passlink_cnt = orgdata[5]
            inlink_attr = []
            links = []
            org_angles = []
            linkdirs = []
            onewaycodes = []
            same_name_link = []
            # 求linknum
            if passlink_cnt == 0:
                # start点
                result_s = self._get_link_attrs_notpasslink(nodeid, inlinkid, outlinkid, "s_node",links,org_angles,linkdirs,onewaycodes,inlink_attr)
                # end点
                result_e = self._get_link_attrs_notpasslink(nodeid, inlinkid, outlinkid, "e_node",links,org_angles,linkdirs,onewaycodes,inlink_attr)
            else:
                # start点
                result_s = self._get_link_attrs_passlink(nodeid, inlinkid, outlinkid, passlid, "s_node",links,org_angles,linkdirs,onewaycodes,inlink_attr)
                # end点
                result_e = self._get_link_attrs_passlink(nodeid, inlinkid, outlinkid, passlid, "e_node",links,org_angles,linkdirs,onewaycodes,inlink_attr)
            links.insert(0, inlink_attr[0])
            linkdirs.insert(0, inlink_attr[1])
            onewaycodes.insert(0, inlink_attr[2])
            org_angles.insert(0, inlink_attr[3])
            
            self.get_same_name_flags(same_name_link, links)
            
            linknum = result_s[4] + result_e[4]
            jump_cnt = result_s[5] + result_e[5]
            angles = self._get_angle_360(org_angles)
            metadata = metadata_item()
            metadata.set_angles(angles)
            metadata.set_links(links)
            metadata.set_linknum(linknum)
            metadata.set_linkdirs(linkdirs)
            metadata.set_same_name_link(same_name_link)
            metadata.set_onewaycodes(onewaycodes)
            metadata.set_inlink(0)
            metadata.set_outlink(1)
            metadata.set_jumpnum(jump_cnt)
            self._make_alonglink_data(metadata)
            data_dic = {}
            data_dic[gid] = metadata
            metadata_list.append(data_dic)
        return metadata_list
    
    def get_same_name_flags(self, same_name_link, links):
        lenth = len(links)
        if lenth < 1:
            return
        same_name_link.insert(0, True)
        sqlcmd = '''
            SELECT  road_name
            FROM link_tbl
            where link_id=%s;
        '''
        self.pg.execute2(sqlcmd, (links[0],))
        row = self.pg.fetchone2()
        inlink_name = row[0]
        i = 1
        while i < lenth:
            self.pg.execute2(sqlcmd, (links[i],))
            row = self.pg.fetchone2()
            temp_name = row[0]
            if temp_name == inlink_name:
                same_name_link.append(True)
            else:
                same_name_link.append(False)
            i = i + 1
        return
    def _get_link_attrs_notpasslink(self, nodeid, inlinkid, outlinkid, node_s_e,links,org_angles,linkdirs,onewaycodes,inlink_attr):
        jump_cnt = 0
        sql_cmd = '''
                SELECT link_id,link_type,road_type,one_way_code,fazm,tazm,e_node
                FROM link_tbl
                where ''' + node_s_e + ''' = %s;
            '''
        self.pg.execute2(sql_cmd, (nodeid,))
        rows = self.pg.fetchall2()
        linknum_s = len(rows)
        for row in rows:
            link_id = row[0]
            link_type = row[1]
            road_type = row[2]
            one_way_code = row[3]
            dir_f = False
            fazm = row[5]
            if node_s_e == "s_node":
                fazm = row[4]
                dir_f = True
            if link_id == inlinkid:
                inlink_attr.append(link_id)
                inlink_attr.append(dir_f)
                inlink_attr.append(one_way_code)
                inlink_attr.append(fazm)
            elif link_id == outlinkid:
                links.insert(0, link_id)
                linkdirs.insert(0, dir_f)
                onewaycodes.insert(0, one_way_code)
                org_angles.insert(0, fazm)
            else:
                links.append(link_id)
                linkdirs.append(dir_f)
                onewaycodes.append(one_way_code)
                org_angles.append(fazm)
        return (links, linkdirs, onewaycodes, org_angles, linknum_s, jump_cnt)

    def _get_link_attrs_passlink(self, nodeid, inlinkid, outlinkid, passlid, node_s_e,links,org_angles,linkdirs,onewaycodes,inlink_attr):
        passlinklist = passlid.split("|")
        jump_cnt = 0
        sql_cmd = '''
            SELECT link_id,link_type,road_type,one_way_code,fazm,tazm,e_node,s_node,length
            FROM link_tbl
            where ''' + node_s_e + ''' = %s;
        '''
        self.pg.execute2(sql_cmd, (nodeid,))
        rows = self.pg.fetchall2()
        linknum_s = len(rows)
        for row in rows:
            link_id = row[0]
            link_type = row[1]
            road_type = row[2]
            one_way_code = row[3]
            fazm = row[5]
            dir_f = False
            enode = row[7]
            if node_s_e == "s_node":
                fazm = row[4]
                dir_f = True
                enode = row[6]
            if link_id == inlinkid:
                inlink_attr.append(link_id)
                inlink_attr.append(dir_f)
                inlink_attr.append(one_way_code)
                inlink_attr.append(fazm)
            elif link_id == long(passlinklist[0]):
                pass_link_length = row[8]
                if pass_link_length > INNERLINK_MAX_LEN or self.get_linktype(link_id) <> 4:
                    links.insert(0, link_id)
                    linkdirs.insert(0, dir_f)
                    onewaycodes.insert(0, one_way_code)
                    org_angles.insert(0, fazm)
                else:
                    passlinklist.append(outlinkid)
                    jump_cnt = 1
                    for midlink in passlinklist[1:]:
                        temp_link = long(midlink)
                        self.pg.execute('''
                            SELECT link_type, road_type, one_way_code, fazm,
                                   tazm, s_node, e_node,length
                            FROM link_tbl
                            where link_id = %s;
                            ''', (temp_link,))
                        row_pass = self.pg.fetchone2()
                        pass_link_length = pass_link_length + row_pass[7]
                        if pass_link_length > INNERLINK_MAX_LEN or row_pass[0] <> 4:
                            links.insert(0, temp_link)
                            if row_pass[5] == enode:
                                linkdirs.insert(0, True)
                                org_angles.insert(0, row_pass[3])
                            else:
                                linkdirs.insert(0, False)
                                org_angles.insert(0, row_pass[4])
                            onewaycodes.insert(0, row_pass[2])
                            break

                        if jump_cnt == len(passlinklist) - 1:
                            links.insert(0, temp_link)
                            if row_pass[5] == enode:
                                linkdirs.insert(0, True)
                                org_angles.insert(0, row_pass[3])
                            else:
                                linkdirs.insert(0, False)
                                org_angles.insert(0, row_pass[4])
                            onewaycodes.insert(0, row_pass[2])
                        if row_pass[5] == enode:
                            enode = row_pass[6]
                        else:
                            enode = row_pass[5]
                        jump_cnt = jump_cnt + 1
            else:
                links.append(link_id)
                linkdirs.append(dir_f)
                onewaycodes.append(one_way_code)
                org_angles.append(fazm)
        return (links, linkdirs, onewaycodes, org_angles, linknum_s, jump_cnt)

    def _get_angle_360(self, org_angles):
        angles = []
        if len(org_angles) > 0:
            angles.append(180)
            ref_angle = org_angles[0]
            in_angle = (ref_angle + 32768) * 360.0 / 65536.0
            if in_angle < 180:
                in_angle = in_angle + 180
            else:
                in_angle = in_angle - 180
            for org_angle in org_angles[1:]:
                out_angle = (org_angle + 32768) * 360.0 / 65536.0
                l_f_flag = self._get_link_rel_position(in_angle, out_angle)
                angle = abs(out_angle - in_angle)
                if angle >= 180:
                    angle = 360 - angle
                if l_f_flag:
                    angles.append(angle)
                else:
                    angles.append(0 - angle)
        return angles

    def _get_link_rel_position(self, ref_angle, cmp_angle):
        small_degree_mim = 0.0
        small_degree_mix = 0.0
        big_degree_mim = 0.0
        big_degree_mix = 0.0
        # 第一,二象限
        if ref_angle < 0:
            ref_angle = 360 - abs(ref_angle)
        if cmp_angle < 0:
            cmp_angle = 360 - abs(cmp_angle)
        if ref_angle > 0 and ref_angle < 180:
            small_degree_mim = 0.0
            small_degree_mix = ref_angle
            big_degree_mim = 180 + ref_angle
            big_degree_mix = 360.0
        # 第三,四象限
        elif ref_angle > 180 and ref_angle < 360:
            small_degree_mim = ref_angle - 180
            small_degree_mix = 180.0
            big_degree_mim = 180.0
            big_degree_mix = ref_angle
        elif ref_angle == 0:
            small_degree_mim = 180.0
            small_degree_mix = 180.0
            big_degree_mim = 180.0
            big_degree_mix = 360.0
        elif ref_angle == 180:
            small_degree_mim = 0
            small_degree_mix = 180.0
            big_degree_mim = 180.0
            big_degree_mix = 180.0
        if ((cmp_angle >= small_degree_mim and cmp_angle <= small_degree_mix) or
          (cmp_angle >= big_degree_mim and cmp_angle <= big_degree_mix)):
            return True
        else:
            return False

    def _get_basic_angle(self, metadata):
        '''metadata元数据，内容（linknum，angles，links，inlink,outlink，jumpnum）'''
        angles = metadata.get_angles()
        outlink = metadata.get_outlink()
        links = metadata.get_links()
        inlink = metadata.get_inlink()
        outlink_angle = angles[outlink]
        abs_outlink_angle = abs(outlink_angle)
        linknum = metadata.get_linknum()
        linkdirs = metadata.get_linkdirs()
        onewaycodes = metadata.get_onewaycodes()
        if abs_outlink_angle > 120:
            if outlink_angle > 0:
                return "right_back"
            else:
                return "left_back"
        elif abs_outlink_angle > 60:
            if outlink_angle > 0:
                return "right"
            else:
                return "left"
        elif abs_outlink_angle > 30:
            condition = False
            link_cnt = 0
            while link_cnt < linknum:
                # 跳过routlink
                if link_cnt in (outlink, inlink):
                    link_cnt = link_cnt + 1
                    continue
                # 右侧
                if outlink_angle > 0:
                    if angles[link_cnt] > 0 and angles[link_cnt] > outlink_angle:
                        condition = True
                # 左侧
                else:
                    if angles[link_cnt] <= 0 and angles[link_cnt] < outlink_angle:
                        condition = True
                link_cnt = link_cnt + 1
            # 右侧
            if outlink_angle > 0:
                if condition:
                    return "right_forward"
                else:
                    return "right"
            # 左侧
            else:
                if condition:
                    return "left_forward"
                else:
                    return "left"
        elif abs_outlink_angle > 15:
            if outlink_angle > 0:
                return "right_forward"
            else:
                return "left_forward"
        else:
            if outlink_angle > 0:
                return "straight"
            else:
                return "left_straight"

    def isHighWay(self, linkid):
        '''如果是收费link，就是hwy，否则不是'''
        sql_cmd = '''
            SELECT toll
            FROM link_tbl
            where link_id = %s;
        '''
        self.pg.execute2(sql_cmd, (linkid,))
        row = self.pg.fetchone2()
        return row[0] == 1

    def _check_link_triffic(self, onewaycode, linkdir):
        ''''''
        if onewaycode == ONEWAY_F:
            if linkdir:
                return True
            else:
                return False
        elif onewaycode == ONEWAY_R:
            if linkdir:
                return False
            else:
                return True
        elif onewaycode == ONEWAY_B:
            return True
        else:
            return False

    def get_linktype(self, linkid):
        sql_cmd = '''
            SELECT link_type
            FROM link_tbl
            where link_id = %s;
        '''
        self.pg.execute2(sql_cmd, (linkid,))
        row = self.pg.fetchone2()
        return row[0]

    def get_roadtype(self, linkid):
        sql_cmd = '''
            SELECT road_type
            FROM link_tbl
            where link_id = %s;
        '''
        self.pg.execute2(sql_cmd, (linkid,))
        row = self.pg.fetchone2()
        return row[0]

    def check_straight_road(self, metadata):
        if not metadata:
            return False
        angles = metadata.get_angles()
        outlink = metadata.get_outlink()
        outlink_angle = angles[outlink]
        abs_outlink_angle = abs(outlink_angle)
        linknum = metadata.get_linknum()
        linkdirs = metadata.get_linkdirs()
        onewaycodes = metadata.get_onewaycodes()
        branchlink_cnt = 0
        angle_branch = 0
        while  branchlink_cnt < linknum:
            if not self._check_link_triffic(onewaycodes[branchlink_cnt], linkdirs[branchlink_cnt]):
                branchlink_cnt = branchlink_cnt + 1
                continue
            angle_branch = abs(angles[branchlink_cnt])
            if abs_outlink_angle > angle_branch:
                return True
            branchlink_cnt = branchlink_cnt + 1
        return False

    def _highway_enter_filter(self, metadata, basic_arrow):
        '''高速 入口'''
        '''首先判断是高速入口'''
        angles = metadata.get_angles()
        outlink = metadata.get_outlink()
        inlink = metadata.get_inlink()
        links = metadata.get_links()
        enter_flag = False
        if self.isHighWay(links[inlink]) \
            or not self.isHighWay(links[outlink]):
            return [False, basic_arrow]

        if self.get_roadtype(links[inlink]) > URBAN_HIGHWAY_FLAG \
            and self.get_linktype(links[outlink]) == RAMP_LINK_FLAG:
            enter_flag = True

        if enter_flag:
            if self.check_straight_road(metadata):
                if angles[outlink] > 0:
                    basic_arrow = "right_forward"
                elif angles[outlink] < 0:
                    basic_arrow = "left_forward"
                else:
                    basic_arrow = "straight"
                return [True, basic_arrow]
            else:
                basic_arrow = "straight"
                return [True, basic_arrow]
        return [False, basic_arrow]

    def _highway_exit_filter(self, metadata, basic_arrow):
        '''高速 出口'''
        '''首先判断是高速出口'''
        angles = metadata.get_angles()
        outlink = metadata.get_outlink()
        inlink = metadata.get_inlink()
        links = metadata.get_links()
        linknum = metadata.get_linknum()
        linkdirs = metadata.get_linkdirs()
        onewaycodes = metadata.get_onewaycodes()
        if self.get_linktype(links[inlink]) in (MAIN_LINK1_FLAG,MAIN_LINK2_FLAG,SAPA_LINK_FLAG):
            if self.get_linktype(links[outlink]) == RAMP_LINK_FLAG:
                if self.get_roadtype(links[inlink]) == self.get_roadtype(links[outlink])\
                    and self.isHighWay(links[inlink]) and self.isHighWay(links[outlink]):
                    basic_arrow = "straight"
                    if linknum >= 3:
                        link_cnt = 0
                        while link_cnt < linknum:
                            if link_cnt in (inlink, outlink):
                                link_cnt = link_cnt + 1
                                continue
                            if not self._check_link_triffic(onewaycodes[link_cnt], linkdirs[link_cnt]):
                                link_cnt = link_cnt + 1
                                continue
                            abs_link_angle = abs(angles[link_cnt])
                            if abs_link_angle > 75 and not self.isHighWay(links[link_cnt]):
                                link_cnt = link_cnt + 1
                                continue
                            if angles[link_cnt] > angles[outlink]:
                                if basic_arrow == "right_forward":
                                    basic_arrow = "straight"
                                    break
                                else:
                                    basic_arrow = "left_forward"
                            else:
                                if basic_arrow == "left_forward":
                                    basic_arrow = "straight"
                                    break
                                else:
                                    basic_arrow = "right_forward"
                            link_cnt = link_cnt + 1
                    return [True, basic_arrow]
        return [False, basic_arrow]

    def _roundabout_enter_filter(self, metadata, basic_arrow):
        '''roundabout 入口'''
        '''首先判断是roundabout入口'''
        angles = metadata.get_angles()
        outlink = metadata.get_outlink()
        inlink = metadata.get_inlink()
        linknum = metadata.get_linknum()
        links = metadata.get_links()
        if self.get_linktype(links[inlink]) != ROUNDABOUT_FLAG \
            and self.get_linktype(links[outlink]) == ROUNDABOUT_FLAG:
            round_dir = "left"
            link_cnt = 0
            while link_cnt < linknum:
                if link_cnt in (outlink,inlink):
                    link_cnt = link_cnt + 1
                    continue
                if self.get_linktype(links[link_cnt]) == ROUNDABOUT_FLAG:
                    if angles[outlink] > angles[link_cnt]:
                        basic_arrow = "right"
                    else:
                        basic_arrow = "left"
                    if round_dir != basic_arrow:
                        return [False, basic_arrow]
                    return [True, basic_arrow]
                link_cnt = link_cnt + 1
        return [False, basic_arrow]

    def _roundabout_exit_filter(self, metadata, basic_arrow):
        '''roundabout 出口'''
        '''首先判断是roundabout出口'''
        angles = metadata.get_angles()
        outlink = metadata.get_outlink()
        inlink = metadata.get_inlink()
        linknum = metadata.get_linknum()
        links = metadata.get_links()
        if self.get_linktype(links[inlink]) == ROUNDABOUT_FLAG \
            and self.get_linktype(links[outlink]) != ROUNDABOUT_FLAG:
            round_dir = "left"
            link_cnt = 0
            while link_cnt < linknum:
                if link_cnt in (outlink, inlink):
                    link_cnt = link_cnt + 1
                    continue
                if self.get_linktype(links[link_cnt]) == ROUNDABOUT_FLAG:
                    if angles[outlink] > angles[link_cnt]:
                        basic_arrow = "right"
                    else:
                        basic_arrow = "left"
                    if round_dir != basic_arrow:
                        return [False, basic_arrow]
                    return [True, basic_arrow]
                link_cnt = link_cnt + 1
        return [False, basic_arrow]
        return
    
    def _sideroad_filter(self, metadata, basic_arrow):
        '''侧道'''
        '''首先判断是侧道'''
        loop_flag = False;
        rel_p = basic_arrow
        rel_p2 = basic_arrow
        angles = metadata.get_angles()
        path_angle = angles[1]
        abs_path_angle = abs(path_angle)
        if abs_path_angle > 45:
            return [False, basic_arrow]
        link_cnt = 0
        link_num = metadata.get_linknum()
        links = metadata.get_links()
#          mian link < 30 is exist?
        while link_cnt < link_num:
            link_angle = abs(angles[link_cnt])
            if link_angle < 30 and (self.get_linktype(links[link_cnt]) == MAIN_LINK1_FLAG\
                                    or self.get_linktype(links[link_cnt]) == MAIN_LINK2_FLAG) :
                break
            link_cnt = link_cnt + 1
        
        if link_cnt == link_num:
            return [False, basic_arrow]
        # inlink is mainlink
        if self.get_linktype(links[0]) in (MAIN_LINK1_FLAG, MAIN_LINK2_FLAG):
            # pathlink is ramp
            if self.get_linktype(links[1]) == RAMP_LINK_FLAG:
                if self.get_roadtype(links[0]) == self.get_roadtype(links[1]) \
                    and not self.isHighWay(links[1]) and not self.isHighWay(links[0]):
                    link_cnt = 0
                    relcnt = 0
                    while link_cnt < link_num:
                        if link_cnt <> 1:
                            if self.get_roadtype(links[0]) == self.get_roadtype(links[link_cnt]):
                                if angles[link_cnt] <> 180:
                                    loop_flag = True
                                    link_angle = abs(angles[link_cnt])
                                    if link_angle < 75:
                                        if self._get_link_rel_position(angles[1], angles[link_cnt]):
                                            rel_p = "right"
                                        else:
                                            rel_p = "left"
                                        relcnt = relcnt + 1
                                        if relcnt > 1:
                                            if rel_p <> rel_p2:
                                                rel_p = "straight"
                                                break
                                        rel_p2 = rel_p
                        link_cnt = link_cnt + 1
                    if loop_flag:
                        basic_arrow = rel_p
                        if basic_arrow == "straight":
                            if angles[1] > 0:
                                basic_arrow = "right"
                            else:
                                basic_arrow = "left"
                        return [True, basic_arrow]
        return [False, basic_arrow]
    
    def _uturn_bylink_filter(self, metadata, basic_arrow):
        '''通过link判断Uturn'''
        '''首先判断是Uturn'''
        
        return [False, basic_arrow]
    
    def _uturn_bylogic_filter(self, metadata, basic_arrow):
        '''通过逻辑算法Uturn'''
        '''首先判断是Uturn'''
        links = metadata.get_links()
        if self.get_linktype(links[0]) in (MAIN_LINK2_FLAG,RAMP_LINK_FLAG,SIDE_TRACK_LINK_FLAG) \
            and self.get_linktype(links[1]) in (MAIN_LINK2_FLAG,RAMP_LINK_FLAG,SIDE_TRACK_LINK_FLAG):
            
            return
        return [False, basic_arrow]
    
    def _highway_branch_filter(self, metadata, basic_arrow):
        '''通过link判断高速'''
        '''首先判断是高速分歧'''
        jumpnum = metadata.get_jumpnum()
        loop_flag = False;
        if jumpnum <> 0:
            return [False, basic_arrow]
        relcnt = 0
        rel_p = basic_arrow
        rel_p2 = basic_arrow
        links = metadata.get_links()
        onewaycodes = metadata.get_onewaycodes()
        linkdirs = metadata.get_linkdirs()
        angles = metadata.get_angles()
        #是否存在另外一条路
        if self._check_if_another_link(metadata):
            return [False, basic_arrow]
        linknum = metadata.get_linknum()
        link_cnt = 0
        if self.get_linktype(links[0]) == RAMP_LINK_FLAG:
            if self.get_linktype(links[1]) == RAMP_LINK_FLAG:
                if self.isHighWay(links[0]) and self.isHighWay(links[1]):
                    while link_cnt < linknum:
                        if link_cnt in (0,1):
                            link_cnt = link_cnt + 1
                            continue
                        #skip not hwy and sapa link
                        if self.get_linktype(links[link_cnt]) == SAPA_LINK_FLAG or not self.isHighWay(links[link_cnt]):
                            link_cnt = link_cnt + 1
                            continue
                        #skip not triffic link
                        if not self._check_link_triffic(onewaycodes[link_cnt], linkdirs[link_cnt]):
                            link_cnt = link_cnt + 1
                            continue
                        loop_flag = True
                        abs_branch_angle = abs(angles[link_cnt])
                        if abs_branch_angle < 75:
                            if self._get_link_rel_position(angles[1], angles[link_cnt]):
                                rel_p = "right"
                            else:
                                rel_p = "left"
                            relcnt = relcnt + 1
                            
                            if relcnt > 1:
                                if rel_p == rel_p2:
                                    rel_p = "straight"
                                    break
                            rel_p2 = rel_p
                        link_cnt = link_cnt + 1
                    if loop_flag:
                        basic_arrow = rel_p
                        return [True, basic_arrow]
                    else:
                        return [False, basic_arrow]
        #目前的link是connection link
        elif self.get_linktype(links[0]) == CONNECTION_LINK_FLAG:
            if self.get_linktype(links[1]) == CONNECTION_LINK_FLAG:
                while link_cnt < linknum:
                    if link_cnt in (0,1):
                        link_cnt = link_cnt + 1
                        continue
                    if self.get_linktype(links[link_cnt]) == SAPA_LINK_FLAG:
                        link_cnt = link_cnt + 1
                        continue
                    loop_flag = True
                    abs_branch_angle = abs(angles[link_cnt])
                    if abs_branch_angle < 75:
                        if self._get_link_rel_position(angles[1], angles[link_cnt]):
                            rel_p = "right"
                        else:
                            rel_p = "left"
                        relcnt = relcnt + 1
                        if relcnt > 1:
                            if rel_p == rel_p2:
                                rel_p = "straight"
                                break
                        rel_p2 = rel_p
                    link_cnt = link_cnt + 1
                if loop_flag:
                    basic_arrow = rel_p
                    return [True, basic_arrow]
                else:
                    return [False, basic_arrow]
        elif self.get_linktype(links[1]) == RAMP_LINK_FLAG:
            if self.get_roadtype(links[0]) == self.get_roadtype(links[1]) and \
                self.isHighWay(links[0]) and self.isHighWay(links[1]):
                while link_cnt < linknum:
                    if link_cnt in (0,1):
                        link_cnt = link_cnt + 1
                        continue
                    abs_branch_angle = abs(angles[link_cnt])
                    if abs_branch_angle < 75:
                        if self._get_link_rel_position(angles[1], angles[link_cnt]):
                            rel_p = "right"
                        else:
                            rel_p = "left"
                        relcnt = relcnt + 1
                        if relcnt > 1:
                            if rel_p == rel_p2:
                                rel_p = "straight"
                                break
                        rel_p2 = rel_p
                    link_cnt = link_cnt + 1
                basic_arrow = rel_p
                return [True, basic_arrow]
            else:
                pass
        else:
            if self.get_roadtype(links[0]) == self.get_roadtype(links[1]) and \
                 self.isHighWay(links[0]) and self.isHighWay(links[1]):
                while link_cnt < linknum:
                    if link_cnt in (0,1):
                        link_cnt = link_cnt + 1
                        continue
                    if self.get_linktype(links[link_cnt]) == SAPA_LINK_FLAG:
                        link_cnt = link_cnt + 1
                        continue
                    if self.get_linktype(links[link_cnt]) == RAMP_LINK_FLAG:
                        link_cnt = link_cnt + 1
                        continue
                    loop_flag = True
                    link_cnt = link_cnt + 1
                if not loop_flag:
                    basic_arrow = "straight"
                    return [True, basic_arrow]
        return [False, basic_arrow]
    
    def _check_if_another_link(self, metadata):
        linknum = metadata.get_linknum()
        link_cnt = 0
        onewaycodes = metadata.get_onewaycodes()
        linkdirs = metadata.get_linkdirs()
        angles = metadata.get_angles()
        while link_cnt < linknum:
            if link_cnt == 1:
                link_cnt = link_cnt + 1
                continue
            oneway_flag = self._check_link_triffic(onewaycodes[link_cnt], linkdirs[link_cnt])
            if not oneway_flag:
                link_cnt = link_cnt + 1
                continue
            abs_angle = abs(angles[link_cnt])
            if abs_angle < 60:
                return False
            link_cnt = link_cnt + 1
        return True
        
class metadata_item(object):
    def __init__(self):
        self.linknum = 0
        self.angles = []
        self.links = []
        self.inlink = 0
        self.outlink = 0
        self.jumpnum = 0
        self.linkdirs = []
        self.onewaycodes = []
        self.alonglink = -1
        self.same_name_link = []
        
    def set_same_name_link(self,same_name_link):
        self.same_name_link = same_name_link
        
    def get_same_name_link(self):
        return self.same_name_link
    
    def set_linknum(self, linknum):
        self.linknum = linknum

    def set_inlink(self, inlink):
        self.inlink = inlink

    def set_outlink(self, outlink):
        self.outlink = outlink

    def set_jumpnum(self, jumpnum):
        self.jumpnum = jumpnum

    def set_angles(self, angles):
        self.angles = angles

    def set_links(self, links):
        self.links = links

    def set_linkdirs(self, linkdirs):
        self.linkdirs = linkdirs

    def get_linknum(self):
        return self.linknum

    def get_inlink(self):
        return self.inlink

    def get_outlink(self):
        return self.outlink

    def get_jumpnum(self):
        return self.jumpnum

    def get_angles(self):
        return self.angles

    def get_links(self):
        return self.links

    def get_linkdirs(self):
        return self.linkdirs

    def set_onewaycodes(self, onewaycodes):
        self.onewaycodes = onewaycodes

    def get_onewaycodes(self):
        return self.onewaycodes
    
    def set_alonglink(self, alonglink):
        self.alonglink = alonglink
        
    def get_alonglink(self):
        return self.alonglink