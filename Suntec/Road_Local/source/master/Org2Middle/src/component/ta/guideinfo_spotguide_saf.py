# -*- coding: UTF8 -*-
'''
Created on 2015-10-29
@author: tangpinghui
'''

import common
from common.guideinfo_pushlink_common import link_object
from component.default.guideinfo_spotguide import comp_guideinfo_spotguide

# 
str_arrow_3_forked_left = 'Arrow_3-forked_Left'
str_arrow_3_forked_middle = 'Arrow_3-forked_Middle'
str_arrow_3_forked_right = 'Arrow_3-forked_Right'
str_arrow_branch_l_left = 'Arrow_Branch_L_Left'
str_arrow_branch_l_straight = 'Arrow_Branch_L_Straight'
str_arrow_branch_r_right = 'Arrow_Branch_R_Right'
str_arrow_branch_r_straight = 'Arrow_Branch_R_Straight'
str_arrow_fork_left = 'Arrow_Fork_Left'
str_arrow_fork_right = 'Arrow_Fork_Right'
str_road_3_forked = 'Road_3-forked'
str_road_branch_left = 'Road_Branch_Left'
str_road_branch_right = 'Road_Branch_Right'
str_road_fork = 'Road_Fork'

class link_object_spotguide(link_object):
    def __init__(self, errMsg, linkid, sNodeid, eNodeid, link_type, the_geom_text, name):
        link_object.__init__(self, errMsg, linkid, sNodeid, eNodeid, link_type, the_geom_text)
        self.name = name
        
    def toString(self):
        strTemp = link_object.toString(self)
        strTemp += """, name: %s""" % self.name
        return strTemp
        
spotguide_tbl_insert_str = '''
        insert into spotguide_tbl(nodeid, inlinkid, outlinkid,
                                  passlid, passlink_cnt, direction,
                                  guideattr, namekind, guideclass,
                                  patternno, arrowno, type,
                                  is_exist_sar)
          values(%s, %s, %s,
                 %s, %s, %s,
                 %s, %s, %s,
                 %s, %s, %s,
                 %s)
'''

# 作成南非spotguide数据，依赖signpost_tbl_uc，link_tbl，node_tbl。
class comp_guideinfo_spotguide_saf(comp_guideinfo_spotguide):
    def __init__(self):
        comp_guideinfo_spotguide.__init__(self)

    def _DoCreateTable(self):
        self.CreateTable2('spotguide_tbl')
        
    def _DoCreatIndex(self):
        return 0

    def _DoCreateFunction(self):
        return 0

    def _Do(self):
        self.proj_country = common.common_func.getProjCountry().lower()
        self.CreateIndex2('nw_f_jnctid_idx')
        self.CreateIndex2('nw_t_jnctid_idx')
        self._GenerateNaviLinkFromSignpost()
        self._GenerateNaviLinkFromBifurcation()
        self._GenerateSpotguide()
        tollIllustName = common.common_func.GetPath('toll_station_illust')
        comp_guideinfo_spotguide._GenerateSpotguideTblForTollStation(self, tollIllustName)
        return 0
    
    # 从org_sp表获取高速相关signpost的link序列。
    def _GenerateNaviLinkFromSignpost(self):
        sqlcmd = """
drop table if exists temp_spotguide_nav_link_from_signpost;
select id as signpost_id,
    array_agg(seqnr) as seqnr_list,
    array_agg(trpelid) as trpelid_list, 
    array_agg(f_jnctid) as f_jnctid_list,
    array_agg(t_jnctid) as t_jnctid_list,
    array_agg(st_astext(the_geom)) as the_geom_text_list,
    array_agg(name) as name_list
into temp_spotguide_nav_link_from_signpost
from 
(
    select d.id, d.seqnr, d.trpelid, e.f_jnctid, e.t_jnctid, e.the_geom, e.name
    from 
    (
        select distinct a.id
        from 
        org_sp as a
        left join org_nw as b
        on a.trpelid=b.id
        where (mid_cnv_road_type(b.freeway,
        b.frc,
        b.ft,
        b.fow,
        b.privaterd,
        b.backrd,
        b.procstat,
        b.carriage,
        b.nthrutraf,
        b.sliprd,
        b.stubble,
        '%X') = 0)
        order by a.id
    )as c
    left join org_sp as d
    on c.id=d.id
    left join org_nw as e
    on d.trpelid=e.id
    order by d.id, d.seqnr
) as t
group by id;"""
        sqlcmd = sqlcmd.replace('%X', self.proj_country)
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
    
    # 从org_jc表生成高速相关bifurcation点的link序列。  
    def _GenerateNaviLinkFromBifurcation(self):
        sqlcmd = """
drop table if exists temp_spotguide_nav_link_from_bifurcation;
select * 
into temp_spotguide_nav_link_from_bifurcation
from
(
select a.id as nodeid, 
    array[b.id, c.id] as trpelid_list, 
    array[b.f_jnctid, c.f_jnctid] as f_jnctid_list,
    array[b.t_jnctid, c.t_jnctid] as t_jnctid_list,
    array[st_astext(b.the_geom), st_astext(c.the_geom)] as the_geom_text_list,
    array[b.name, c.name] as name_list
from
org_jc as a
left join org_nw as b
on a.id=b.f_jnctid
left join org_nw as c
on a.id=c.f_jnctid
where a.jncttyp=2 
    and b.id<>c.id
    and (b.oneway='TF' or b.oneway is null)
    and (c.oneway='FT' or c.oneway is null)
    and (mid_cnv_road_type(b.freeway,
        b.frc,
        b.ft,
        b.fow,
        b.privaterd,
        b.backrd,
        b.procstat,
        b.carriage,
        b.nthrutraf,
        b.sliprd,
        b.stubble,
        '%X') = 0
    or
    mid_cnv_road_type(c.freeway,
        c.frc,
        c.ft,
        c.fow,
        c.privaterd,
        c.backrd,
        c.procstat,
        c.carriage,
        c.nthrutraf,
        c.sliprd,
        c.stubble,
        '%X') = 0)
union
select a.id as nodeid, 
    array[b.id, c.id] as trpelid_list, 
    array[b.f_jnctid, c.f_jnctid] as f_jnctid_list,
    array[b.t_jnctid, c.t_jnctid] as t_jnctid_list,
    array[st_astext(b.the_geom), st_astext(c.the_geom)] as the_geom_text_list,
    array[b.name, c.name] as name_list
from
org_jc as a
left join org_nw as b
on a.id=b.f_jnctid
left join org_nw as c
on a.id=c.t_jnctid
where a.jncttyp=2 
    and b.id<>c.id
    and (b.oneway='TF' or b.oneway is null)
    and (c.oneway='TF' or c.oneway is null)
    and (mid_cnv_road_type(b.freeway,
        b.frc,
        b.ft,
        b.fow,
        b.privaterd,
        b.backrd,
        b.procstat,
        b.carriage,
        b.nthrutraf,
        b.sliprd,
        b.stubble,
        '%X') = 0
    or
    mid_cnv_road_type(c.freeway,
        c.frc,
        c.ft,
        c.fow,
        c.privaterd,
        c.backrd,
        c.procstat,
        c.carriage,
        c.nthrutraf,
        c.sliprd,
        c.stubble,
        '%X') = 0)
union
select a.id as nodeid, 
    array[b.id, c.id] as trpelid_list, 
    array[b.f_jnctid, c.f_jnctid] as f_jnctid_list,
    array[b.t_jnctid, c.t_jnctid] as t_jnctid_list,
    array[st_astext(b.the_geom), st_astext(c.the_geom)] as the_geom_text_list,
    array[b.name, c.name] as name_list
from
org_jc as a
left join org_nw as b
on a.id=b.t_jnctid
left join org_nw as c
on a.id=c.f_jnctid
where a.jncttyp=2 
    and b.id<>c.id
    and (b.oneway='FT' or b.oneway is null)
    and (c.oneway='FT' or c.oneway is null)
    and (mid_cnv_road_type(b.freeway,
        b.frc,
        b.ft,
        b.fow,
        b.privaterd,
        b.backrd,
        b.procstat,
        b.carriage,
        b.nthrutraf,
        b.sliprd,
        b.stubble,
        '%X') = 0
    or
    mid_cnv_road_type(c.freeway,
        c.frc,
        c.ft,
        c.fow,
        c.privaterd,
        c.backrd,
        c.procstat,
        c.carriage,
        c.nthrutraf,
        c.sliprd,
        c.stubble,
        '%X') = 0)
union
select a.id as nodeid, 
    array[b.id, c.id] as trpelid_list, 
    array[b.f_jnctid, c.f_jnctid] as f_jnctid_list,
    array[b.t_jnctid, c.t_jnctid] as t_jnctid_list,
    array[st_astext(b.the_geom), st_astext(c.the_geom)] as the_geom_text_list,
    array[b.name, c.name] as name_list
from
org_jc as a
left join org_nw as b
on a.id=b.t_jnctid
left join org_nw as c
on a.id=c.t_jnctid
where a.jncttyp=2 
    and b.id<>c.id
    and (b.oneway='FT' or b.oneway is null)
    and (c.oneway='TF' or c.oneway is null)
    and (mid_cnv_road_type(b.freeway,
        b.frc,
        b.ft,
        b.fow,
        b.privaterd,
        b.backrd,
        b.procstat,
        b.carriage,
        b.nthrutraf,
        b.sliprd,
        b.stubble,
        '%X') = 0
    or
    mid_cnv_road_type(c.freeway,
        c.frc,
        c.ft,
        c.fow,
        c.privaterd,
        c.backrd,
        c.procstat,
        c.carriage,
        c.nthrutraf,
        c.sliprd,
        c.stubble,
        '%X') = 0)
) as t;
        """
        sqlcmd = sqlcmd.replace('%X', self.proj_country)
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return 0
    
    # 前面已生成两个link序列表：
    # temp_spotguide_nav_link_from_signpost
    # temp_spotguide_nav_link_from_bifurcation
    # 从以上两个link序列表取合集，当产生重复时，优先使用signpost获取的数据。
    # 根据此合集生成spotguide的相关数据。
    def _GenerateSpotguide(self):
        sqlcmd = """
select trpelid_list, f_jnctid_list, t_jnctid_list,
    the_geom_text_list, name_list
from temp_spotguide_nav_link_from_signpost
union 
select a.trpelid_list, a.f_jnctid_list, a.t_jnctid_list,
    a.the_geom_text_list, a.name_list
from 
temp_spotguide_nav_link_from_bifurcation as a
left join temp_spotguide_nav_link_from_signpost as b
on (a.trpelid_list[1]=b.trpelid_list[1] 
and a.trpelid_list[2]=b.trpelid_list[2])
where b.trpelid_list is null;
                """
        self.pg.execute2(sqlcmd)
        rows = self.pg.fetchall2()
        for row in rows:
            trpelid_list = row[0]
            f_jnctid_list = row[1]
            t_jnctid_list = row[2]
            the_geom_text_list = row[3]
            name_list = row[4]
            errMsg = ['']            
            inlinkObj = link_object_spotguide(errMsg, trpelid_list[0], f_jnctid_list[0], t_jnctid_list[0],
                                    -1, the_geom_text_list[0], name_list[0])
            outlinkObj = link_object_spotguide(errMsg, trpelid_list[1], f_jnctid_list[1], t_jnctid_list[1],
                                     -1, the_geom_text_list[1], name_list[1])
            patternPic, arrowPic = self.getPatternAndArrow(errMsg, inlinkObj, outlinkObj)
            if errMsg[0] <> '':
                self.log.warning(errMsg[0])
                continue
            
            nodeid = inlinkObj.getConnectionNodeid(errMsg, outlinkObj)
            if errMsg[0] <> '':
                self.log.warning(errMsg[0])
                continue
            
            if len(trpelid_list) < 2:
                self.log.warning("""length of trpelid less than 2. trpelid: %s""" % '|'.join(trpelid_list))
                continue
            
            passlid = ''
            if len(trpelid_list) > 2:
                passlid = '|'.join(map(lambda x: """%.0f"""%x, trpelid_list[1:-1]))
            
            self.pg.execute2(spotguide_tbl_insert_str,
                 (nodeid, trpelid_list[0], trpelid_list[-1],
                 passlid, len(trpelid_list) - 2, 0,
                 0, 0, 0, patternPic, arrowPic, 1, False))
        self.pg.commit2()
        return 0
    
    # 求pattern图 名字与arrow图名字。
    def getPatternAndArrow(self, errMsg, inlinkObj, outlinkObj):
        nodeid = inlinkObj.getConnectionNodeid(errMsg, outlinkObj)
        if errMsg[0] <> '':
            return None, None
                
        inlinkTrafficAngle = inlinkObj.getTrafficDirAngleToEast(errMsg, nodeid, 'to_this_node')
        if errMsg[0] <> '':
            return None, None
        
        outlinkTrafficAngle = outlinkObj.getTrafficDirAngleToEast(errMsg, nodeid, 'from_this_node')
        if errMsg[0] <> '':
            return None, None
        
        sqlcmd1 = """
                    select distinct id, f_jnctid, t_jnctid, st_astext(the_geom), name
                    from org_nw 
                    where f_jnctid=%.0f and (oneway='FT' or oneway is null)
                    union 
                    select distinct id, f_jnctid, t_jnctid, st_astext(the_geom), name
                    from org_nw 
                    where t_jnctid=%.0f and (oneway='TF' or oneway is null)
                    """
        sqlcmd = sqlcmd1 % (nodeid, nodeid)
        self.pg.execute2(sqlcmd)
        rows = self.pg.fetchall2()
        
        restOutlinkList = []
        for row in rows:
            linkid = row[0]
            sNodeid = row[1]
            eNodeid = row[2]
            the_geom_text = row[3]
            name = row[4]
            if linkid == inlinkObj.link_id or linkid == outlinkObj.link_id:
                continue
            else:
                restOutlinkList.append(link_object_spotguide(errMsg, linkid, sNodeid, eNodeid, -1, the_geom_text, name))
        
        patternPic = None
        arrowPic = None
        # 二分歧路口，需进一步判断左分叉、右分叉、Y型分叉。
        if len(restOutlinkList) == 1: 
            restOutlinkTrafficAngle = restOutlinkList[0].getTrafficDirAngleToEast(errMsg, nodeid, 'from_this_node')
            if errMsg[0] <> '':
                return None, None
            
            # 若inlink与outlink道路名相同，判断为直行
            if inlinkObj.name == outlinkObj.name and inlinkObj.name <> restOutlinkList[0].name:
                position, angle = link_object.getPositionAndAngleOfA2B(restOutlinkTrafficAngle, outlinkTrafficAngle)
                # 另一流出link在outlink左边
                if position == link_object.DIR_LEFT_SIDE:
                    patternPic = str_road_branch_left
                    arrowPic = str_arrow_branch_l_straight
                # 另一流出link在outlink右边
                else: 
                    patternPic = str_road_branch_right
                    arrowPic = str_arrow_branch_r_straight

            # 若inlink与另一流出link的道路名相同，判断为转弯
            elif inlinkObj.name <> outlinkObj.name and inlinkObj.name == restOutlinkList[0].name:
                position, angle = link_object.getPositionAndAngleOfA2B(outlinkTrafficAngle, restOutlinkTrafficAngle)
                # outlink在另一流出link左边
                if position == link_object.DIR_LEFT_SIDE:
                    patternPic = str_road_branch_left
                    arrowPic = str_arrow_branch_l_left
                # outlink在另一流出link右边
                else:
                    patternPic = str_road_branch_right
                    arrowPic = str_arrow_branch_r_right
                    
            # 无法从道路名获取更多信息，仅根据角度计算。
            else:
                outlinkDir, outlinkAngle = link_object.getPositionAndAngleOfA2B(outlinkTrafficAngle, inlinkTrafficAngle)
                restOutlinkDir, angle2 = link_object.getPositionAndAngleOfA2B(restOutlinkTrafficAngle, inlinkTrafficAngle)
                
                # 两流出link都向左
                if outlinkDir == link_object.DIR_LEFT_SIDE and restOutlinkDir == link_object.DIR_LEFT_SIDE:
                    patternPic = str_road_branch_left
                    if outlinkAngle > angle2:
                        arrowPic = str_arrow_branch_l_left
                    else:
                        arrowPic = str_arrow_branch_l_straight

                # 两流出link都向右
                elif outlinkDir == link_object.DIR_RIGHT_SIDE and restOutlinkDir == link_object.DIR_RIGHT_SIDE:
                    patternPic = str_road_branch_right
                    if outlinkAngle > angle2:
                        arrowPic = str_arrow_branch_r_right
                    else:
                        arrowPic = str_arrow_branch_r_straight
                
                # outlink向左，另一流出link向右
                elif outlinkDir == link_object.DIR_LEFT_SIDE and restOutlinkDir == link_object.DIR_RIGHT_SIDE:
                    if outlinkAngle > 10 and angle2 > 10:
                        patternPic = str_road_fork
                        arrowPic = str_arrow_fork_left
                    elif outlinkAngle > 10 and angle2 <= 10:
                        patternPic = str_road_branch_left
                        arrowPic = str_arrow_branch_l_left
                    elif outlinkAngle <= 10 and angle2 <= 10:
                        patternPic = str_road_fork
                        arrowPic = str_arrow_fork_left
                    else:  # outlinkAngle<=10 and angle2>10:
                        patternPic = str_road_branch_right
                        arrowPic = str_arrow_branch_r_straight
                
                # outlink向右，另一流出link向左
                else:  # outlinkDir==DIR_RIGHT_SIDE and position2==DIR_LEFT_SIDE
                    if outlinkAngle > 10 and angle2 > 10:
                        patternPic = str_road_fork
                        arrowPic = str_arrow_fork_right
                    elif outlinkAngle > 10 and angle2 <= 10:
                        patternPic = str_road_branch_right
                        arrowPic = str_arrow_branch_r_right
                    elif outlinkAngle <= 10 and angle2 <= 10:
                        patternPic = str_road_fork
                        arrowPic = str_arrow_fork_right
                    else:  # outlinkAngle<=10 and angle2>10:
                        patternPic = str_road_branch_left
                        arrowPic = str_arrow_branch_l_straight
        
        # 三分歧路口              
        elif len(restOutlinkList) == 2:
            patternPic = str_road_3_forked
            restOutlinkTrafficAngle1 = restOutlinkList[0].getTrafficDirAngleToEast(errMsg, nodeid, 'from_this_node')
            restOutlinkTrafficAngle2 = restOutlinkList[1].getTrafficDirAngleToEast(errMsg, nodeid, 'from_this_node')
            
            position1, angle1 = link_object.getPositionAndAngleOfA2B(outlinkTrafficAngle, restOutlinkTrafficAngle1)
            position2, angle2 = link_object.getPositionAndAngleOfA2B(outlinkTrafficAngle, restOutlinkTrafficAngle2)
            
            # outlink在最左侧
            if position1 == link_object.DIR_LEFT_SIDE and position2 == link_object.DIR_LEFT_SIDE:
                arrowPic = str_arrow_3_forked_left
            # outlink在最右侧
            elif position1 == link_object.DIR_RIGHT_SIDE and position2 == link_object.DIR_RIGHT_SIDE:
                arrowPic = str_arrow_3_forked_right
            # outlink在中间
            else:
                arrowPic = str_arrow_3_forked_middle
        elif len(restOutlinkList) == 0:
            errMsg[0] = """the node has only 1 outlink.\n""" + \
                        """inlink: %s\noutlink: %s\nnodeid: %.0f""" % \
                        (inlinkObj.toString(), outlinkObj.toString(), nodeid)
            return None, None
        else: # len(restOutlinkList) > 2:
            errMsg[0] = """the node has more than 3 outlinks.\n""" + \
                        """inlink: %s\noutlink: %s\nnodeid: %.0f""" % \
                        (inlinkObj.toString(), outlinkObj.toString(), nodeid)
            return None, None
        
        return patternPic, arrowPic
    

        

































