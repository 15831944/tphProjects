# -*- coding: UTF8 -*-
'''
Created on 2015-10-29
@author: tangpinghui
'''

import math
from component.default.guideinfo_spotguide import comp_guideinfo_spotguide

str_arrow_3_forked_left = 'Arrow_3-forked_Left.png'
str_arrow_3_forked_middle = 'Arrow_3-forked_Middle.png'
str_arrow_3_forked_right = 'Arrow_3-forked_Right.png'
str_arrow_branch_l_left = 'Arrow_Branch_L_Left.png'
str_arrow_branch_l_straight = 'Arrow_Branch_L_Straight.png'
str_arrow_branch_r_right = 'Arrow_Branch_R_Right.png'
str_arrow_branch_r_straight = 'Arrow_Branch_R_Straight.png'
str_arrow_fork_left = 'Arrow_Fork_Left.png'
str_arrow_fork_right = 'Arrow_Fork_Right.png'
str_road_3_forked = 'Road_3-forked.png'
str_road_branch_left = 'Road_Branch_Left.png'
str_road_branch_right = 'Road_Branch_Right.png'
str_road_fork = 'Road_Fork.png'

Rc = 6378137 # 赤道半径
Rj = 6356725 # 极半径 

DIR_LEFT_SIDE = 1
DIR_RIGHT_SIDE = 2

# 此类用于换算两点的经纬度至方位及距离
class LatLonPoint(object):
    def __init__(self, latitude, longitude):   
        self.m_Latitude = latitude;     
        self.m_Longitude = longitude;
        self.m_RadLa = latitude * math.pi/180.0
        self.m_RadLo = longitude * math.pi/180.0
        self.Ec = Rj + (Rc - Rj) * (90.-self.m_Latitude) / 90.0
        self.Ed = self.Ec * math.cos(self.m_RadLa)
        
    # 求相对本点angle方向上距离为distance的点的经纬度。
    # angle：相对本点的方位
    # distance：相对本点的距离
    def getLatLon(self, angle, distance):
        dx = distance*1000 * math.sin(angle * math.pi /180.0)
        dy = distance*1000 * math.cos(angle * math.pi /180.0)      
        lon = (dx/self.Ed + self.m_RadLo) * 180.0/math.pi
        lat = (dy/self.Ec + self.m_RadLa) * 180.0/math.pi
        return LatLonPoint(lon, lat)
     
     
    # 求方位角，一、二象限为0~180°，四、三象限为-0~-180°。
    # todo: 求距离，单位：米
    # latitude：目标点的纬度
    # longitude：目标点的经度
    def getAngle(self, latitude, longitude):
        B = LatLonPoint(latitude, longitude)
        dx = (B.m_RadLo - self.m_RadLo) * self.Ed;
        dy = (B.m_RadLa - self.m_RadLa) * self.Ec;
        angle = math.atan(abs(dx/dy))*180.0/math.pi;
        
        # 判断象限
        deltaLat = B.m_Latitude - self.m_Latitude;
        deltaLon = B.m_Longitude - self.m_Longitude;
        if (deltaLon>=0 and deltaLat>0): # 第一象限
            return angle
        elif(deltaLon<0 and deltaLat>=0): # 第二象限
            return angle
        elif(deltaLon<=0 and deltaLat<0): # 第三象限
            return -angle
        elif(deltaLon>0 and deltaLat<=0): # 第四象限
            return -angle
        
class link_object(object):
    def __init__(self, linkid, sNodeid, eNodeid, the_geom_text, name):
        self.linkid=linkid
        self.sNodeid = sNodeid
        self.eNodeid = eNodeid
        self.the_geom_text = the_geom_text
        self.pointlist = self.geomText2Pointlist(the_geom_text)
        self.name = name
        
    def toString(self):
        return """linkid: %.0f, sNodeid: %.0f, eNodeid: %.0f, name: %s""" % \
               (self.linkid, self.sNodeid, self.eNodeid, self.name)
    
    def geomText2Pointlist(self, the_geom_text):
        pointlist = []
        the_geom_text = the_geom_text.replace("""MULTILINESTRING((""", "")
        the_geom_text = the_geom_text.replace("""))""", "")
        geomTextSplit = the_geom_text.split(',')
        for oneGeomText in geomTextSplit:
            pointlist.append(LatLonPoint(oneGeomText.split(' ')[0]), LatLonPoint(oneGeomText.split(' ')[1]))
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
class comp_guideinfo_spotguide_saf(comp_guideinfo_spotguide):
    def __init__(self):
        comp_guideinfo_spotguide.__init__(self)

    def _DoCreateTable(self):
        self.CreateTable2('spotguide_tbl')

    def _DoCreateFunction(self):
        return 0

    def _Do(self):
        sqlcmd = """select id, array_agg(seqnr) as seqnr_list,
                        array_agg(trpelid) as trpelid_list, 
                        array_agg(f_jnctid) as f_jnctid_list,
                        array_agg(t_jnctid) as t_jnctid_list,
                        array_agg(st_astext(the_geom)) as the_geom_text_list,
                        array_agg(name) as name_list
                    from 
                    (
                        select b.*, c.f_jnctid, c.t_jnctid, c.the_geom, c.name
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
                        )as a
                        left join org_sp as b
                        on a.id=b.id
                        left join org_nw as c
                        on b.trpelid=c.id
                        order by b.id, b.seqnr
                    ) as t
                    group by id"""
                    
        self.pg.execute2(sqlcmd)
        rows = self.pg.fetchall2()
        for row in rows:
            trpelid_list = row[2]
            f_jnctid_list = row[3]
            t_jnctid_list = row[4]
            the_geom_text_list = row[5]
            name_list = row[6]
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
            
            self.pg.execute2(spotguide_tbl_insert_str, 
                 (nodeid, trpelid_list[0], trpelid_list[-1],
                 '|'.join(trpelid_list[1:-1]), len(trpelid_list)-2, 0,
                 0, 0, 0, patternPic, arrowPic, 1, False))
            self.pg.commit2()
        return 0

    # 求pattern图 名字与arrow图名字。
    def getPatternAndArrow(self, errMsg, inlinkObj, outlinkObj):
        nodeid = self.getConnectedNodeid(errMsg, inlinkObj, outlinkObj)
        if errMsg[0] <> '':
            return None, None
        
        sqlcmd1 =   """
                    select id, f_jnctid, t_jnctid, the_geom, name
                    from org_nw 
                    where f_jnctid=%.0f and (oneway is null or oneway='TF')
                    union
                    select id, f_jnctid, t_jnctid, the_geom, name
                    from org_nw 
                    where t_jnctid=%.0f and (oneway is null or oneway='FT')
                    """
        sqlcmd = sqlcmd1 % (nodeid, nodeid)
        self.pg.execute2(sqlcmd)
        rows = self.pg.fetchall2()
        
        restOutlinkList = []
        for row in rows:
            linkid = row[0]
            sNodeid = row[1]
            eNodeid = row[2]
            the_geom = row[3]
            name = row[4]
            if linkid == inlinkObj.linkid or linkid == outlinkObj.linkid:
                continue
            else:
                restOutlinkList.append(link_object(linkid, sNodeid, eNodeid, the_geom, name))
                
        inlinkTrafficAngle = self.getTrafficAngleToEast(errMsg, inlinkObj, nodeid, 'to_this_node')
        if errMsg[0] <> '':
            return None, None
        
        outlinkTrafficAngle = self.getTrafficAngleToEast(errMsg, outlinkObj, nodeid, 'from_this_node')
        if errMsg[0] <> '':
            return None, None
        
        patternPic = None
        arrowPic = None
        # 二分歧路口，需进一步判断左分叉、右分叉、Y型分叉。
        if len(restOutlinkList) == 1: 
            restOutlinkTrafficAngle = self.getTrafficAngleToEast(errMsg, restOutlinkList[0], 
                                                                 nodeid, 'from_this_node')
            if errMsg[0] <> '':
                return None, None
            
            # 若inlink与outlink道路名相同，判断为直行
            if inlinkObj.name==outlinkObj.name:
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
            elif inlinkObj.name == restOutlinkList[0].name:
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
                position1, angle1 = self.getPositionOf2Angle(outlinkTrafficAngle, inlinkTrafficAngle)
                position2, angle2 = self.getPositionOf2Angle(restOutlinkTrafficAngle, inlinkTrafficAngle)
                
                # 两流出link都向左
                if position1==DIR_LEFT_SIDE and position2==DIR_LEFT_SIDE:
                    patternPic = str_road_branch_left
                    if angle1 > angle2:
                        arrowPic = str_arrow_branch_l_left
                    else:
                        arrowPic = str_arrow_branch_l_straight

                # 两流出link都向右
                elif position1==DIR_RIGHT_SIDE and position2==DIR_RIGHT_SIDE:
                    patternPic = str_road_branch_right
                    if angle1 > angle2:
                        arrowPic = str_arrow_branch_r_right
                    else:
                        arrowPic = str_arrow_branch_r_straight
                
                # outlink向左，另一流出link向右
                elif position1==DIR_LEFT_SIDE and position2==DIR_RIGHT_SIDE:
                    if angle1>10 and angle2>10:
                        patternPic = str_road_fork
                        arrowPic = str_arrow_fork_left
                    elif angle1>10 and angle2<=10:
                        patternPic = str_road_branch_left
                        arrowPic = str_arrow_branch_l_left
                    elif angle1<=10 and angle2<=10:
                        patternPic = str_road_fork
                        arrowPic = str_arrow_fork_left
                    else: # angle1<=10 and angle2>10:
                        patternPic = str_road_branch_right
                        patternPic = str_arrow_branch_r_straight
                
                # outlink向右，另一流出link向左
                else: # position1==DIR_RIGHT_SIDE and position2==DIR_LEFT_SIDE
                    if angle1>10 and angle2>10:
                        patternPic = str_road_fork
                        arrowPic = str_arrow_fork_right
                    elif angle1>10 and angle2<=10:
                        patternPic = str_road_branch_right
                        arrowPic = str_arrow_branch_r_right
                    elif angle1<=10 and angle2<=10:
                        patternPic = str_road_fork
                        arrowPic = str_arrow_fork_right
                    else: # angle1<=10 and angle2>10:
                        patternPic = str_road_branch_left
                        patternPic = str_arrow_branch_l_straight
        
        # 三分歧路口              
        elif len(restOutlinkList) == 2:
            patternPic = str_road_3_forked
            restOutlinkTrafficAngle1 = self.getTrafficAngleToEast(errMsg, restOutlinkList[0], 
                                                                  nodeid, 'from_this_node')
            restOutlinkTrafficAngle2 = self.getTrafficAngleToEast(errMsg, restOutlinkList[1], 
                                                                  nodeid, 'from_this_node')
            
            position1, angle1 = self.getPositionOf2Angle(outlinkTrafficAngle, restOutlinkTrafficAngle1)
            position2, angle2 = self.getPositionOf2Angle(outlinkTrafficAngle, restOutlinkTrafficAngle2)
            
            # outlink在最左侧
            if position1==DIR_LEFT_SIDE and position2==DIR_LEFT_SIDE:
                arrowPic = str_arrow_3_forked_left
            # outlink在最右侧
            elif position1==DIR_RIGHT_SIDE and position2==DIR_RIGHT_SIDE:
                arrowPic = str_arrow_3_forked_right
            # outlink在中间
            else:
                arrowPic = str_arrow_3_forked_middle
        else:
            errMsg[0] = """the node has more than 3 outlinks.\n""" +\
                        """inlink object: %s\noutlink object: %s""" % \
                        (inlinkObj.toString(), outlinkObj.toString())
            return None, None
        
        return patternPic, arrowPic
    
    # 求当前link上的交通流向与正东方向的夹角。
    # linkObj: 当前link信息
    # nodeid: 当前link的端点
    # trafficDir: 沿当前link流向此点/沿当前link从此点流出，必须等于'to_this_node'或'from_this_node'。
    def getTrafficAngleToEast(self, errMsg, linkObj, nodeid, trafficDir='to_this_node'):
        if trafficDir<>'to_this_node' and trafficDir<>'from_this_node':
            errMsg[0] = """invalid argument trafficDir: %s""" % trafficDir
            return None
        
        if nodeid == linkObj.sNodeid:
            lat1 = 0
            lon1 = 0
            lat2 = 0
            lon2 = 0
            return LatLonPoint(lat1, lon1).getAngle(lat2, lon2)
            
        elif nodeid == linkObj.eNodeid:
            lat1 = 0
            lon1 = 0
            lat2 = 0
            lon2 = 0
            return LatLonPoint(lat1, lon1).getAngle(lat2, lon2)
        
        else:
            errMsg[0] = """nodeid is not the link's endpoint.\n""" +\
                        """linkObj: %s, nodeid: %s""" % (linkObj.toString(), nodeid)
            return None
    
    # 求两条link的相连node。
    def getConnectedNodeid(self, errMsg, inlinkObj, outlinkObj):
        if inlinkObj.sNodeid==outlinkObj.sNodeid or inlinkObj.sNodeid==outlinkObj.eNodeid:
            return inlinkObj.sNodeid
        elif inlinkObj.eNodeid==outlinkObj.sNodeid or inlinkObj.eNodeid==outlinkObj.eNodeid:
            return inlinkObj.eNodeid
        else:
            errMsg[0] = """input in/out links are not connected.\n""" +\
                        """inlink object: %s\noutlink object: %s""" %\
                        (inlinkObj.toString(), outlinkObj.toString())
            return None

    # 求两个角度的相互位置。
    # 返回1：左/右
    # 返回2：左/右的角度
    def getPositionOf2Angle(self, angle1, angle2):
        if 0<=angle1-angle2 and angle1-angle2<180:
            return DIR_LEFT_SIDE, angle1-angle2
        
        if 180<=angle1-angle2 and angle1-angle2<360:
            return DIR_RIGHT_SIDE, 360-(angle1-angle2)
        
        if -360<angle1-angle2 and angle1-angle2<-180:
            return DIR_LEFT_SIDE, 360+(angle1-angle2)
        
        if -180<=angle1-angle2 and angle1-angle2<0:
            return DIR_RIGHT_SIDE, -(angle1-angle2)
        

































