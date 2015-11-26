# -*- coding: UTF8 -*-
'''
Created on 2015-10-29
@author: tangpinghui
'''

import math
import common
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

# 功能类，此类用于换算两点的经纬度与方位、距离
A_WGS84 = 6378137.0
E2_WGS84 = 6.69437999013e-3
class LatLonPoint(object):
    def __init__(self, latitude, longitude):
        self.latitude = latitude*256*3600;    
        self.longitude = longitude*256*3600;
        
    def getPointByAngleDistance(self, angle, distance):
        
        return
     
    def getAngleByLatLon(self, latitude, longitude):
        delta_lat = latitude - self.latitude
        delta_lon = longitude - self.longitude
        ref_lat = (self.latitude + delta_lat / 2.0) / 256.0
        ref_lat_radian = ref_lat / 3600.0 * math.pi / 180.0
        sinlat = math.sin(ref_lat_radian)
        c = 1.0 - (E2_WGS84 * sinlat * sinlat)
        M2SecLon = 3600 / ((math.pi/180.0) * A_WGS84 * math.cos(ref_lat_radian)/math.sqrt(c))
        M2SecLat = 1.0 / ((math.pi/648000.0) * A_WGS84 * (1-E2_WGS84)) * math.sqrt(c*c*c)
        real_delta_lat = delta_lat / M2SecLat / 256.0
        real_delta_lon = delta_lon / M2SecLon / 256.0
        real_dir = math.atan2(real_delta_lat, real_delta_lon) * 180.0 / math.pi
        return real_dir
    
    def getAngleByPoint(self, latLonPoint2):
        return self.getAngleByLatLon(latLonPoint2.latitude, latLonPoint2.longitude)
    
    def getDistanceByLatLon(self, latitude, longitude):
        radLat1 = latitude * math.pi / 180
        radLat2 = self.latitude * math.pi / 180
        a = radLat1 - radLat2
        b = longitude * math.pi / 180 - self.longitude * math.pi / 180
        
        s = 2 * math.asin(math.sqrt(math.pow(math.sin(a/2),2) +\
                                    math.cos(radLat1)*math.cos(radLat2)*math.pow(math.sin(b/2),2)))
        s = s * A_WGS84;
        s = round(s * 10000) / 10000;
        return s
    
    def getDistanceByPoint(self, latLonPoint2):
        return self.getDistanceByLatLon(latLonPoint2.latitude, latLonPoint2.longitude)
        
class link_object(object):
    def __init__(self, linkid, sNodeid, eNodeid, the_geom_text, name):
        self.id = linkid
        self.sNodeid = sNodeid
        self.eNodeid = eNodeid
        self.the_geom_text = the_geom_text
        self.pointlist = self.geomText2Pointlist(the_geom_text)
        self.name = name
        
    def toString(self):
        return """id: %.0f, sNodeid: %.0f, eNodeid: %.0f, name: %s""" % \
               (self.id, self.sNodeid, self.eNodeid, self.name)
    
    def geomText2Pointlist(self, the_geom_text):
        pointlist = []
        the_geom_text = the_geom_text.replace("""MULTILINESTRING((""", "")
        the_geom_text = the_geom_text.replace("""))""", "")
        geomTextSplit = the_geom_text.split(',')
        for oneGeomText in geomTextSplit:
            lat = float(oneGeomText.split(' ')[1])
            lon = float(oneGeomText.split(' ')[0])
            pointlist.append(LatLonPoint(lat, lon))
        return pointlist
        

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
DIR_LEFT_SIDE = 1
DIR_RIGHT_SIDE = 2

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
        b.stubble) = 0)
        order by a.id
    )as c
    left join org_sp as d
    on c.id=d.id
    left join org_nw as e
    on d.trpelid=e.id
    order by d.id, d.seqnr
) as t
group by id;"""
                    
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
        b.stubble) = 0
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
        c.stubble) = 0)
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
        b.stubble) = 0
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
        c.stubble) = 0)
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
        b.stubble) = 0
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
        c.stubble) = 0)
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
        b.stubble) = 0
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
        c.stubble) = 0)
) as t;
        """
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
            inlinkObj = link_object(trpelid_list[0], f_jnctid_list[0], t_jnctid_list[0],
                                    the_geom_text_list[0], name_list[0])
            outlinkObj = link_object(trpelid_list[1], f_jnctid_list[1], t_jnctid_list[1],
                                     the_geom_text_list[1], name_list[1])
            patternPic, arrowPic = self.getPatternAndArrow(errMsg, inlinkObj, outlinkObj)
            if errMsg[0] <> '':
                self.log.warning(errMsg[0])
                continue
            
            nodeid = self.getConnectedNodeid(errMsg, inlinkObj, outlinkObj)
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
        nodeid = self.getConnectedNodeid(errMsg, inlinkObj, outlinkObj)
        if errMsg[0] <> '':
            return None, None
                
        inlinkTrafficAngle = self.getTrafficDirAngleToEast(errMsg, inlinkObj, nodeid, 'to_this_node')
        if errMsg[0] <> '':
            return None, None
        
        outlinkTrafficAngle = self.getTrafficDirAngleToEast(errMsg, outlinkObj, nodeid, 'from_this_node')
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
            if linkid == inlinkObj.id or linkid == outlinkObj.id:
                continue
            else:
                restOutlinkList.append(link_object(linkid, sNodeid, eNodeid, the_geom_text, name))
        
        patternPic = None
        arrowPic = None
        # 二分歧路口，需进一步判断左分叉、右分叉、Y型分叉。
        if len(restOutlinkList) == 1: 
            restOutlinkTrafficAngle = self.getTrafficDirAngleToEast(errMsg, restOutlinkList[0],
                                                                    nodeid, 'from_this_node')
            if errMsg[0] <> '':
                return None, None
            
            # 若inlink与outlink道路名相同，判断为直行
            if inlinkObj.name == outlinkObj.name and inlinkObj.name <> restOutlinkList[0].name:
                position, angle = self.getPositionOf2Angle(restOutlinkTrafficAngle, outlinkTrafficAngle)
                # 另一流出link在outlink左边
                if position == DIR_LEFT_SIDE:
                    patternPic = str_road_branch_left
                    arrowPic = str_arrow_branch_l_straight
                # 另一流出link在outlink右边
                else: 
                    patternPic = str_road_branch_right
                    arrowPic = str_arrow_branch_r_straight

            # 若inlink与另一流出link的道路名相同，判断为转弯
            elif inlinkObj.name <> outlinkObj.name and inlinkObj.name == restOutlinkList[0].name:
                position, angle = self.getPositionOf2Angle(outlinkTrafficAngle, restOutlinkTrafficAngle)
                # outlink在另一流出link左边
                if position == DIR_LEFT_SIDE:
                    patternPic = str_road_branch_left
                    arrowPic = str_arrow_branch_l_left
                # outlink在另一流出link右边
                else:
                    patternPic = str_road_branch_right
                    arrowPic = str_arrow_branch_r_right
                    
            # 无法从道路名获取更多信息，仅根据角度计算。
            else:
                outlinkDir, outlinkAngle = self.getPositionOf2Angle(outlinkTrafficAngle, inlinkTrafficAngle)
                restOutlinkDir, angle2 = self.getPositionOf2Angle(restOutlinkTrafficAngle, inlinkTrafficAngle)
                
                # 两流出link都向左
                if outlinkDir == DIR_LEFT_SIDE and restOutlinkDir == DIR_LEFT_SIDE:
                    patternPic = str_road_branch_left
                    if outlinkAngle > angle2:
                        arrowPic = str_arrow_branch_l_left
                    else:
                        arrowPic = str_arrow_branch_l_straight

                # 两流出link都向右
                elif outlinkDir == DIR_RIGHT_SIDE and restOutlinkDir == DIR_RIGHT_SIDE:
                    patternPic = str_road_branch_right
                    if outlinkAngle > angle2:
                        arrowPic = str_arrow_branch_r_right
                    else:
                        arrowPic = str_arrow_branch_r_straight
                
                # outlink向左，另一流出link向右
                elif outlinkDir == DIR_LEFT_SIDE and restOutlinkDir == DIR_RIGHT_SIDE:
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
            restOutlinkTrafficAngle1 = self.getTrafficDirAngleToEast(errMsg, restOutlinkList[0],
                                                                  nodeid, 'from_this_node')
            restOutlinkTrafficAngle2 = self.getTrafficDirAngleToEast(errMsg, restOutlinkList[1],
                                                                  nodeid, 'from_this_node')
            
            outlinkDir, outlinkAngle = self.getPositionOf2Angle(outlinkTrafficAngle, restOutlinkTrafficAngle1)
            position2, angle2 = self.getPositionOf2Angle(outlinkTrafficAngle, restOutlinkTrafficAngle2)
            
            # outlink在最左侧
            if outlinkDir == DIR_LEFT_SIDE and position2 == DIR_LEFT_SIDE:
                arrowPic = str_arrow_3_forked_left
            # outlink在最右侧
            elif outlinkDir == DIR_RIGHT_SIDE and position2 == DIR_RIGHT_SIDE:
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
    
    # 求当前link上的交通流向与正东方向的夹角。
    # linkObj: 当前link信息
    # nodeid: 当前link的端点
    # trafficDir: 沿当前link流向此点/沿当前link从此点流出，必须等于'to_this_node'或'from_this_node'。
    def getTrafficDirAngleToEast(self, errMsg, linkObj, nodeid, trafficDir='to_this_node'):
        if trafficDir <> 'to_this_node' and trafficDir <> 'from_this_node':
            errMsg[0] = """invalid argument trafficDir: %s""" % trafficDir
            return None
        
        if nodeid == linkObj.sNodeid:
            point1 = linkObj.pointlist[0]
            point2 = None
            for i in range(1, len(linkObj.pointlist)):
                if point1.getDistanceByPoint(linkObj.pointlist[i]) > 5:
                    point2 = linkObj.pointlist[i]
                    break
            # 没有任何一个形状点与sNodeid的距离大于5米，则使用eNodeid为point2
            if point2 == None:
                point2 = linkObj.pointlist[-1]
            
            if trafficDir == 'to_this_node':
                return point2.getAngleByPoint(point1)
            else:  # trafficDir == 'from_this_node'
                return point1.getAngleByPoint(point2)
            
        elif nodeid == linkObj.eNodeid:
            point1 = linkObj.pointlist[-1]
            point2 = None
            for i in range(len(linkObj.pointlist) - 2, -1, -1):
                if point1.getDistanceByPoint(linkObj.pointlist[i]) > 5:
                    point2 = linkObj.pointlist[i]
                    break
            # 没有任何一个形状点与eNodeid的距离大于5米，则使用sNodeid为point2
            if point2 == None:
                point2 = linkObj.pointlist[0]
            
            if trafficDir == 'to_this_node':
                return point2.getAngleByPoint(point1)
            else:  # trafficDir == 'from_this_node'
                return point1.getAngleByPoint(point2)
        
        else:
            errMsg[0] = """nodeid is not the link's endpoint.\n""" + \
                        """link: %s, nodeid: %s""" % (linkObj.toString(), nodeid)
            return None
    
    # 求两条link的相连node。
    def getConnectedNodeid(self, errMsg, inlinkObj, outlinkObj):
        if inlinkObj.sNodeid == outlinkObj.sNodeid or inlinkObj.sNodeid == outlinkObj.eNodeid:
            return inlinkObj.sNodeid
        elif inlinkObj.eNodeid == outlinkObj.sNodeid or inlinkObj.eNodeid == outlinkObj.eNodeid:
            return inlinkObj.eNodeid
        else:
            errMsg[0] = """input in/out links are not connected.\n""" + \
                        """inlink: %s\noutlink: %s""" % \
                        (inlinkObj.toString(), outlinkObj.toString())
            return None

    # 求两个角度的相互位置。
    # 返回1：左/右
    # 返回2：左/右的角度
    def getPositionOf2Angle(self, angle1, angle2):
        if 0 <= angle1 - angle2 and angle1 - angle2 < 180:
            return DIR_LEFT_SIDE, angle1 - angle2
        
        if 180 <= angle1 - angle2 and angle1 - angle2 < 360:
            return DIR_RIGHT_SIDE, 360 - (angle1 - angle2)
        
        if -360 < angle1 - angle2 and angle1 - angle2 < -180:
            return DIR_LEFT_SIDE, 360 + (angle1 - angle2)
        
        if -180 <= angle1 - angle2 and angle1 - angle2 < 0:
            return DIR_RIGHT_SIDE, -(angle1 - angle2)
        

































