# -*- coding: UTF8 -*-
'''
Created on 2015-10-29
@author: tangpinghui
'''
import os
import psycopg2
import struct
import common.common_func
from component.default.guideinfo_spotguide import comp_guideinfo_spotguide

str_arrow_3_foked_left = 'Arrow_3-forked_Left.png'
str_arrow_3_foked_middle = 'Arrow_3-forked_Middle.png'
str_arrow_3_foked_right = 'Arrow_3-forked_Right.png'
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
import math

Rc = 6378137 # 赤道半径
Rj = 6356725 # 极半径 

class LatLonAngleCalculator(object):
    def __init__(self, latitude, longitude):   
        self.m_Latitude = latitude;     
        self.m_Longitude = longitude;
        self.m_RadLa = latitude * math.pi/180.0
        self.m_RadLo = longitude * math.pi/180.0
        self.Ec = Rj + (Rc - Rj) * (90.-self.m_Latitude) / 90.0
        self.Ed = self.Ec * math.cos(self.m_RadLa)

    # 求相对本点angle方向上距离为distance的点的经纬度。
    # angle：相对本点的方向
    # distance：相对本点的距离
    def getLatLon(self, angle, distance):
        dx = distance*1000 * math.sin(angle * math.pi /180.0)
        dy = distance*1000 * math.cos(angle * math.pi /180.0)      
        lon = (dx/self.Ed + self.m_RadLo) * 180.0/math.pi
        lat = (dy/self.Ec + self.m_RadLa) * 180.0/math.pi
        return LatLonAngleCalculator(lon, lat)
     
     
    # 求向量 （本点-->目标点）与正东方向（X轴正方向）的夹角，一、二象限为0~180°，四、三象限为-0~-180°。
    # latitude：目标点的纬度
    # longitude：目标点的经度
    def getAngle(self, latitude, longitude):
        B = LatLonAngleCalculator(latitude, longitude)
        dx = (B.m_RadLo - self.m_RadLo) * self.Ed;
        dy = (B.m_RadLa - self.m_RadLa) * self.Ec;
        angle = math.atan(abs(dx/dy))*180.0/math.pi;
        
        # 判断象限
        deltaLat = B.m_Latitude - self.m_Latitude;
        deltaLon = B.m_Longitude - self.m_Longitude;
        if (deltaLon > 0 and deltaLat <= 0):
            angle = (90. - angle) + 90.0
        elif(deltaLon <= 0 and deltaLat < 0):
            angle = angle + 180.0;
        elif(deltaLon < 0 and deltaLat >= 0):
            angle = (90.0 - angle) + 270.0
        return angle

class comp_guideinfo_spotguide_sa8(comp_guideinfo_spotguide):
    def __init__(self):
        comp_guideinfo_spotguide.__init__(self)

    def _DoCreateTable(self):
        self.CreateTable2('spotguide_tbl')

    def _DoCreateFunction(self):
        return 0

    def _Do(self):
        sqlcmd = """select * from """ 
        return 0

    def getPatternnameAndArrowname(self, inlinkRec, nodeRec, outlinkRec):
        
        return



































